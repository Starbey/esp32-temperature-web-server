#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "tasks_common.h"
#include "wifi_app.h"
#include "wifi_reset_button.h"

static const char TAG[] = "wifi_reset_button";

SemaphoreHandle_t wifiResetSemaphore = NULL;

/**
 * ISR handler for WiFi reset button
 * @param arg not in use
*/
void IRAM_ATTR wifiResetButtonISRHandler(void *arg){
    xSemaphoreGiveFromISR(wifiResetSemaphore, NULL);
}


/**
 * When reset button is pressed, sends msg to WiFi app to disconnect from WiFi and clear the saved credentials in NVS
 * @param parameters not in use
*/
void wifiResetButtonTask(void *parameters){
    while(1){
        if(xSemaphoreTake(wifiResetSemaphore, portMAX_DELAY) == pdTRUE){
            ESP_LOGI(TAG, "WiFi reset button interrupt occured");

            /* send a msg to disconnect WiFi and clear credentials */
            wifiAppSendMsg(WIFI_APP_MSG_USER_REQUESTED_STA_DISCONNECT);
            vTaskDelay(pdMS_TO_TICKS(2000)); // debouncing

        }
    }
}

void wifiResetButtonConfig(void){
    wifiResetSemaphore = xSemaphoreCreateBinary();

    /* configure GPIO for button */
    esp_rom_gpio_pad_select_gpio(WIFI_RESET_BUTTON);
    gpio_set_direction(WIFI_RESET_BUTTON, GPIO_MODE_INPUT);
    gpio_set_intr_type(WIFI_RESET_BUTTON, GPIO_INTR_NEGEDGE); // trigger interrupt when falling edge occurs

    /* create reste button handler task */
    xTaskCreatePinnedToCore(&wifiResetButtonTask, "WiFi_Reset_Button", WIFI_RESET_BUTTON_TASK_STACK_SIZE, NULL, WIFI_RESET_BUTTON_TASK_PRIOTIY, NULL, WIFI_RESET_BUTTON_TASK_CORE_ID);

    /* install GPIO ISR service */
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

    /* attach ISR */
    gpio_isr_handler_add(WIFI_RESET_BUTTON, wifiResetButtonISRHandler, NULL);
}