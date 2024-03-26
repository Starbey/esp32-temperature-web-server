/**
 * Saves station mode WiFi credentials to non-volatile storage (NVS)
 * @return ESP_OK
*/
esp_err_t appNVSSaveStaCreds(void);

/**
 * Loads saved credentials from NVS
 * @return true if previously saved credentials were found
*/
bool appNVSLoadStaCreds(void);

/**
 * Clears station mode credentials from NVS
 * @return ESP_OK
*/
esp_err_t appNVSClearStaCreds(void);