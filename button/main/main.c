#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"

#define BUTTON_GPIO GPIO_NUM_0
#define DEBOUNCE_TIME_MS 50
#define LONG_PRESS_TIME_MS 1000
#define DOUBLE_PRESS_MAX_GAP_MS 500

static QueueHandle_t button_event_queue = NULL;
static esp_timer_handle_t debounce_timer;

typedef enum {
    BUTTON_PRESS,
    BUTTON_RELEASE,
    BUTTON_LONG_PRESS,
    BUTTON_DOUBLE_PRESS
} button_event_t;

static void IRAM_ATTR button_isr_handler(void* arg) {
    // Start/restart a debounce timer
    esp_timer_stop(debounce_timer);
    esp_timer_start_once(debounce_timer, DEBOUNCE_TIME_MS * 1000);
}

static void debounce_timer_callback(void* arg) {
    button_event_t event = gpio_get_level(BUTTON_GPIO) == 0 ? BUTTON_PRESS : BUTTON_RELEASE;
    xQueueSendFromISR(button_event_queue, &event, NULL);
}

static void button_task(void* arg) {
    button_event_t event;
    int64_t last_press_time = 0;
    bool waiting_for_double_press = false;

    while (true) {
        if (xQueueReceive(button_event_queue, &event, portMAX_DELAY)) {
            int64_t now = esp_timer_get_time();

            if (event == BUTTON_PRESS) {
                if (waiting_for_double_press && (now - last_press_time) <= DOUBLE_PRESS_MAX_GAP_MS * 1000) {
                    ESP_LOGI("Button", "Double press detected");
                    waiting_for_double_press = false; // Reset waiting flag
                } else {
                    waiting_for_double_press = true;
                }
                last_press_time = now;
            } else if (event == BUTTON_RELEASE) {
                if (waiting_for_double_press) {
                    int64_t press_duration = now - last_press_time;
                    if (press_duration >= LONG_PRESS_TIME_MS * 1000) {
                        ESP_LOGI("Button", "Long press detected");
                    } else {
                        ESP_LOGI("Button", "Short press detected");
                    }
                    waiting_for_double_press = false; // Avoid double detection
                }
            }
        }
    }
}

void app_main() {
    button_event_queue = xQueueCreate(10, sizeof(button_event_t));
    esp_timer_create_args_t debounce_timer_args = {
        .callback = &debounce_timer_callback,
        .name = "debounce_timer"
    };
    esp_timer_create(&debounce_timer_args, &debounce_timer);

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_NEGEDGE; // Interrupt on falling edge
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << BUTTON_GPIO);
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_GPIO, button_isr_handler, NULL);

    xTaskCreate(button_task, "button_task", 2048, NULL, 10, NULL);
}
