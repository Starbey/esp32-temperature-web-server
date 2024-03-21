/**
 * App entry point
*/

#include "nvs_flash.h"

#include "wifi_app.h"

void app_main(void)
{
    /* initialize (non-volatile storage) NVS */
	esp_err_t ret = nvs_flash_init();
	if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){ // these errors can be handled by erasing and reflashing
		ESP_ERROR_CHECK(nvs_flash_erase()); // clears flash
		ret = nvs_flash_init();
	}

	wifiAppStart();
}
