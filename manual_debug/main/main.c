#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include "esp_log.h"

#define TASK_TAG 
esp_err_t return_zero(int known, int num);
TASK_1
// Function prototype for demonstration
/*esp_err_t example_function(void);

void app_main(void) {
    esp_err_t ret = example_function();
    if (ret != ESP_OK) {
        // Log the error with the function name and the error code converted to a readable string
        ESP_LOGE(TAG, "%s failed with error: %s", __func__, esp_err_to_name(ret));
    }
}

esp_err_t example_function(void) {
    // Simulate an operation that fails, returning an ESP-IDF standard error code
    // For example purposes, we use ESP_FAIL. Replace this with your actual function logic.
    return ESP_FAIL; // Simulated failure
}
*/
void callingFunk()
{
  char buffer[30];
  memset(buffer, 'x', sizeof(buffer));
  buffer[29] = 0;
  printf("buffer is %s\n", buffer);
  for (int i = 0; i < 30; i++)
  {
    int num1 = rand() % 5;
    ESP_LOGI(TASK_TAG,"num1 is %d\n",num1);
    int known = 100;
    if (num1 != 0)
    {
    printf("result is %d\n", known / num1);
    }
    else {
    ESP_LOGE(TASK_TAG,"num1 is %d\n",num1);
    }
    
  }
}

void task1(void *params)
{
  callingFunk();
  vTaskDelete(NULL);
}

void app_main(void)
{
  xTaskCreate(task1, "task1,", 1024 * 2, NULL, 5, NULL);
}

void app_main(void) {
    esp_err_t ret = example_function();
    if (ret != ESP_OK) {
        // Log the error with the function name and the error code converted to a readable string
        ESP_LOGE(TAG, "%s failed with error: %s", __func__, esp_err_to_name(ret));
    }
}