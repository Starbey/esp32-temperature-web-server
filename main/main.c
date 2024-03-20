#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "rgb_led.h"

void app_main(void)
{
    while (true) {
    	ledWifiAppStarted();
    	vTaskDelay(1000 / portTICK_PERIOD_MS);
    	ledHttpServerStarted();
    	vTaskDelay(1000 / portTICK_PERIOD_MS);
    	ledWifiConnected();
    	vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
