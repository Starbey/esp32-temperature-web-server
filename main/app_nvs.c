#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "nvs_flash.h"
#include "app_nvs.h"
#include "wifi_app.h"


static const char TAG[] = "nvs";

const char appNVSStaCredsNamespace[] = "stacreds"; // NVS namespace

esp_err_t appNVSSaveStaCreds(void){
    nvs_handle handle;
    esp_err_t espErr;
    
    ESP_LOGI(TAG, "appNVSSaveStaCreds(): Saving station mode credentials to flash");

    wifi_config_t *wifiStaConfig = wifiAppGetWifiConfig(); // save wifi configuration
    
    if(wifiStaConfig){ // ensure pointer is valid
        espErr = nvs_open(appNVSStaCredsNamespace, NVS_READWRITE, &handle);     
        if(espErr != ESP_OK){
            printf("appNVSSaveStaCreds(): Error (%s) opening NVS handle\n", esp_err_to_name(espErr));
            return espErr;
        }

        /* set SSID */
        espErr = nvs_set_blob(handle, "ssid", wifiStaConfig->sta.ssid, MAX_SSID_LENGTH); // "ssid" is the key; as memory in NVS is stored as key-value
        if(espErr != ESP_OK){
            printf("appNVSSaveStaCreds(): Error (%s) setting SSID to NVS value\n", esp_err_to_name(espErr));
            return espErr;
        }

        /* set password */
        espErr = nvs_set_blob(handle, "password", wifiStaConfig->sta.password, MAX_PASSWORD_LENGTH); // "ssid" is the key; as memory in NVS is stored as key-value
        if(espErr != ESP_OK){
            printf("appNVSSaveStaCreds(): Error (%s) setting password to NVS value\n", esp_err_to_name(espErr));
            return espErr;
        }

        /* commit credentials to NVS */
        espErr = nvs_commit(handle);
        if(espErr != ESP_OK){
            printf("appNVSSaveStaCreds(): Error (%s) committing credentials to NVS\n", esp_err_to_name(espErr));
        }
        nvs_close(handle);
        ESP_LOGI(TAG,"appNVSSaveStaCreds(): Wrote wifiStaConfig: Station SSID: %s, Password: %s", wifiStaConfig->sta.ssid, wifiStaConfig->sta.password);
    }
    printf("appNVSSaveStaCreds(): Returned ESP_OK");
    return ESP_OK;
}

bool appNVSLoadStaCreds(void){
    nvs_handle handle;
    esp_err_t espErr;

    ESP_LOGI(TAG, "appNVSLoadStaCreds(): Loading WiFi credentials from NVS");

    if (nvs_open(appNVSStaCredsNamespace, NVS_READONLY, &handle) == ESP_OK){
        wifi_config_t *wifiStaConfig = wifiAppGetWifiConfig();

        /* if wifiStaConfig pointer is NULL, allocate memory for a new instance of wifi_config_t */
        if(wifiStaConfig == NULL){
            wifiStaConfig = (wifi_config_t*) malloc(sizeof(wifi_config_t));
        }
        memset(wifiStaConfig, 0, sizeof(wifi_config_t));

        /* allocate buffer */
        size_t wifiConfigSize = sizeof(wifi_config_t);
        uint8_t *wifiConfigBuf = (uint8_t*) malloc(sizeof(uint8_t) * wifiConfigSize); 
        memset(wifiConfigBuf, 0, sizeof(wifiConfigSize));

        /* load SSID */
        wifiConfigSize = sizeof(wifiStaConfig->sta.ssid);
        espErr = nvs_get_blob(handle, "ssid", wifiConfigBuf, &wifiConfigSize);

        if(espErr != ESP_OK){
            free(wifiConfigBuf);
            printf("appNVSLoadStaCreds(): (%s) No station SSID found in NVS\n", esp_err_to_name(espErr));
            return false;
        }
        memcpy(wifiStaConfig->sta.ssid, wifiConfigBuf, wifiConfigSize); // copy ssid from buffer to current config

        /* load password */
        wifiConfigSize = sizeof(wifiStaConfig->sta.password);
        espErr = nvs_get_blob(handle, "password", wifiConfigBuf, &wifiConfigSize);

        if(espErr != ESP_OK){
            free(wifiConfigBuf);
            printf("appNVSLoadStaCreds(): (%s) No station password found in NVS\n", esp_err_to_name(espErr));
            return false;
        }
        memcpy(wifiStaConfig->sta.password, wifiConfigBuf, wifiConfigSize); // copy password from buffer to current config

        /* deallocate all memory and close NVS */
        free(wifiConfigBuf);
        nvs_close(handle);

        printf("appNVSLoadStaCreds(): SSID: %s, Password: %s\n", wifiStaConfig->sta.ssid, wifiStaConfig->sta.password);
        return wifiStaConfig->sta.ssid[0] != '\0'; // returns true if SSID is valid (not a blank string)
    }
    else{
        return false;
    }
}

esp_err_t appNVSClearStaCreds(void){
    nvs_handle handle;
    esp_err_t espErr;

    ESP_LOGI(TAG, "appNVSClearStaCreds(): Clearing WiFi station mode credentials from NVS");

    espErr = nvs_open(appNVSStaCredsNamespace, NVS_READWRITE, &handle);
    if(espErr != ESP_OK){
        printf("appNVSClearStaCreds(): Error (%s) opening NVS handle\n", esp_err_to_name(espErr));
        return espErr;
    }

    /* clear credentials from NVS */
    espErr = nvs_erase_all(handle);
    if(espErr != ESP_OK){
        printf("appNVSClearStaCreds(): Error (%s) erasing station mode credentials\n", esp_err_to_name(espErr));
        return espErr;
    }

    /* commit changes to NVS */
    espErr = nvs_commit(handle);
    if(espErr != ESP_OK){
        printf("appNVSClearStaCreds(): Error (%s) NVS commit\n", esp_err_to_name(espErr));
    }

    nvs_close(handle);
    printf("appNVSClearStaCreds(): Returned ESP_OK");

    return ESP_OK;
}