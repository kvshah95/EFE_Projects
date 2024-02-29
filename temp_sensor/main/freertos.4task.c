#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "driver/uart.h"

#define LED_GPIO GPIO_NUM_2
#define BUTTON_GPIO GPIO_NUM_0
#define UART_NUM UART_NUM_1

SemaphoreHandle_t uartSemaphore;
SemaphoreHandle_t buttonSemaphore;

static void task1(void *pvParameters) {
    const TickType_t xDelay = pdMS_TO_TICKS(10);
    while (true) {
        ESP_LOGI("Task1", "Tsk1-P1 <-");
        vTaskDelay(pdMS_TO_TICKS(100));
        ESP_LOGI("Task1", "Tsk1-P1 ->");
        vTaskDelay(xDelay);
    }
}

static void task2(void *pvParameters) {
    const TickType_t xDelay = pdMS_TO_TICKS(250);
    while (true) {
        ESP_LOGI("Task2", "Tsk2-P2 <-");
        vTaskDelay(pdMS_TO_TICKS(10));
        ESP_LOGI("Task2", "Tsk2-P2 ->");
        vTaskDelay(xDelay);
    }
}

static void task3(void *pvParameters) {
    uint8_t data;
    while (true) {
        if (xSemaphoreTake(uartSemaphore, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI("Task3", "Tsk3-P3 <-");
            if (uart_read_bytes(UART_NUM, &data, 1, pdMS_TO_TICKS(100)) > 0) {
                if (data == 'L' || data == 'l') {
                    gpio_set_level(LED_GPIO, !gpio_get_level(LED_GPIO));
                }
            }
            vTaskDelay(pdMS_TO_TICKS(50));
            ESP_LOGI("Task3", "Tsk3-P3 ->");
        }
    }
}

static void task4(void *pvParameters) {
    while (true) {
        if (xSemaphoreTake(buttonSemaphore, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI("Task4", "Tsk4-P4 <-");
            vTaskDelay(pdMS_TO_TICKS(10));
            ESP_LOGI("Task4", "Tsk4-P4 ->");
        }
    }
}

static void IRAM_ATTR button_isr_handler(void* arg) {
    xSemaphoreGiveFromISR(buttonSemaphore, NULL);
}

void app_main(void) {
    uartSemaphore = xSemaphoreCreateBinary();
    buttonSemaphore = xSemaphoreCreateBinary();

    // Initialize UART for Task 3
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_param_config(UART_NUM, &uart_config);
    uart_driver_install(UART_NUM, 1024 * 2, 0, 0, NULL, 0);

    // Initialize GPIO for LED and Button
    gpio_pad_select_gpio(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(BUTTON_GPIO);
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_GPIO, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(BUTTON_GPIO, GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_GPIO, button_isr_handler, NULL);

    // Create tasks
    xTaskCreate(task1, "Task1", 2048, NULL, 1, NULL);
    xTaskCreate(task2, "Task2", 2048, NULL, 2, NULL);
    xTaskCreate(task3, "Task3", 2048, NULL, 3, NULL);
    xTaskCreate(task4, "Task4", 2048, NULL, 4, NULL);

    // Give the semaphore initially for UART input task
    xSemaphoreGive(uartSemaphore);
}
