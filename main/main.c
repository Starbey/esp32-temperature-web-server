/**
 * App entry point
*/

#include "esp_log.h"
#include "nvs_flash.h"

#include "wifi_app.h"
#include "DHT22.h"
#include "wifi_reset_button.h"
#include "sntp_time_sync.h"

static const char TAG[] = "main";

/* wifi connected callback function */
void wifiApplicationConnectedEvents(void){
	ESP_LOGI(TAG, "WiFi app connected");
	sntpTimeSyncTaskStart(); // must have a valid wifi connection before starting this task
}

void app_main(void)
{
    /* initialize (non-volatile storage) NVS */
	esp_err_t ret = nvs_flash_init();
	if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){ // these errors can be handled by erasing and reflashing
		ESP_ERROR_CHECK(nvs_flash_erase()); // clears flash
		ret = nvs_flash_init();
	}

	wifiAppStart();

	/* configure WiFi reset GPIO and interrupt */
	wifiResetButtonConfig();

	dht22TaskStart();

	/* set connected event callback */
	wifiAppSetCallback(wifiApplicationConnectedEvents);
}
