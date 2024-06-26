#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "sys/param.h"
#include "esp_timer.h"
#include "esp_wifi.h"

#include "DHT22.h"
#include "http_server.h"
#include "tasks_common.h"
#include "wifi_app.h"
#include "sntp_time_sync.h"

static const char TAG[] = "http_server"; // tag for console msgs

static int g_wifiConnectStatus = NONE;
static int g_fwUpdateStatus = OTA_UPDATE_PENDING;
static bool g_isLocalTimeSet = false;

static httpd_handle_t httpServerHandle = NULL;

static TaskHandle_t httpServerMonitorTaskHandle = NULL;

static QueueHandle_t httpServerMonitorQueueHandle;

const esp_timer_create_args_t fwUpdateResetArgs = {
    .callback = &httpServerFwUpdateResetCallback,
    .arg = NULL,
    .dispatch_method = ESP_TIMER_TASK,
    .name = "fwUpdateReset",
};

esp_timer_handle_t fwUpdateReset;

/* embedded files: jquery, index.html, app.css, app.hs, favicon.ico */
extern const uint8_t jquery_3_3_1_min_js_start[]    asm("_binary_jquery_3_3_1_min_js_start");
extern const uint8_t jquery_3_3_1_min_js_end[]    asm("_binary_jquery_3_3_1_min_js_end");

extern const uint8_t index_html_start[]    asm("_binary_index_html_start");
extern const uint8_t index_html_end[]    asm("_binary_index_html_end");

extern const uint8_t app_css_start[]    asm("_binary_app_css_start");
extern const uint8_t app_css_end[]    asm("_binary_app_css_end");

extern const uint8_t app_js_start[]    asm("_binary_app_js_start");
extern const uint8_t app_js_end[]    asm("_binary_app_js_end");

extern const uint8_t favicon_ico_start[]    asm("_binary_favicon_ico_start");
extern const uint8_t favicon_ico_end[]    asm("_binary_favicon_ico_end");

/**
 * Checks global FW update status and creates fwUpdateReset timer if g_fwUpdateStatus is true
*/
static void httpServerFwUpdateResetTimer(void){
    if(g_fwUpdateStatus == OTA_UPDATE_SUCCESS){
        ESP_LOGI(TAG, "httpServerFwUpdateResetTimer: FW updated successful; starting FW update reset timer");

        /* wait for webpage to acknowledge back and init the timer */
        ESP_ERROR_CHECK(esp_timer_create(&fwUpdateResetArgs, &fwUpdateReset));
        ESP_ERROR_CHECK(esp_timer_start_once(fwUpdateReset, 8000000));
    }
    else {
        ESP_LOGI(TAG, "httpServerFwUpdateResetTimer: FW update unsuccessful");
    }
}

/**
 * HTTP server monitor task handler
 * @param parameters not in use
*/
static void httpServerMonitor(void *parameters){
    httpServerQueueMsg_t msg;

    while(1){
        if (xQueueReceive(httpServerMonitorQueueHandle, &msg, portMAX_DELAY)){
            switch(msg.msgID){
                case HTTP_MSG_WIFI_CONNECT_INIT:
                    ESP_LOGI(TAG,"HTTP_MSG_WIFI_CONNECT_INIT");
                    g_wifiConnectStatus = HTTP_WIFI_STATUS_WIFI_CONNECTING;
                    break;

                case HTTP_MSG_WIFI_CONNECT_SUCCESS:
                    ESP_LOGI(TAG, "HTTP_MSG_WIFI_CONNECT_SUCCESS");
                    g_wifiConnectStatus = HTTP_WIFI_STATUS_CONNECT_SUCCESS;
                    break;

                case HTTP_MSG_WIFI_CONNECT_FAILED:
                    ESP_LOGI(TAG, "HTTP_MSG_WIFI_CONNECT_FAILED");
                    g_wifiConnectStatus = HTTP_WIFI_STATUS_CONNECT_FAILED;
                    break;

                case HTTP_MSG_WIFI_USER_DISCONNECT:
                    ESP_LOGI(TAG, "HTTP_MSG_WIFI_USER_DISCONNECT");
                    g_wifiConnectStatus = HTTP_WIFI_STATUS_DISCONNECTED;
                    break;

                case HTTP_MSG_OTA_UPDATE_SUCCESS:
                    ESP_LOGI(TAG, "HTTP_MSG_WIFI_OTA_UPDATE_SUCCESS");
                    g_fwUpdateStatus = OTA_UPDATE_SUCCESS;
                    httpServerFwUpdateResetTimer();
                    break;

                case HTTP_MSG_OTA_UPDATE_FAILED:
                    ESP_LOGI(TAG, "HTTP_MSG_WIFI_OTA_UPDATE_FAILED");
                    g_fwUpdateStatus = OTA_UPDATE_FAILED;
                    break;

                case HTTP_MSG_TIME_SERVICE_INITIALIZED:
                    ESP_LOGI(TAG, "HTTP_MSG_TIME_SERVICE_INITIALIZED");
                    g_isLocalTimeSet = true;
                    break;
            }
        }
    }
}

/**
 * JQuery get handler is requested when accessing the webpage
 * @param req HTTP request for which the URI needs to be handled
 * @return ESP_OK
*/
static esp_err_t httpServerJqueryHandler(httpd_req_t *req){
    ESP_LOGI(TAG, "JQuery requested");
    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_send(req, (const char *)jquery_3_3_1_min_js_start, jquery_3_3_1_min_js_end - jquery_3_3_1_min_js_start); // pass request, buf, buf length

    return ESP_OK;
}

/**
 * Sends index.html page
 * @param req HTTP request for which the URI needs to be handled
 * @return ESP_OK
*/
static esp_err_t httpServerIndexHtmlHandler(httpd_req_t *req){
    ESP_LOGI(TAG, "index.html requested");
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)index_html_start, index_html_end - index_html_start); // pass request, buf, buf length

    return ESP_OK;
}

/**
 * app.css handler is requested when accessing the webpage
 * @param req HTTP request for which  the URI needs to be handled
 * @return ESP_OK
*/
static esp_err_t httpServerAppCssHandler(httpd_req_t *req){
    ESP_LOGI(TAG, "app.css requested");
    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, (const char *)app_css_start, app_css_end - app_css_start); // pass request, buf, buf length

    return ESP_OK;
}

/**
 * app.js handler is requested when accessing the webpage
 * @param req HTTP request for which  the URI needs to be handled
 * @return ESP_OK
*/
static esp_err_t httpServerAppJsHandler(httpd_req_t *req){
    ESP_LOGI(TAG, "app.js requested");
    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_send(req, (const char *)app_js_start, app_js_end - app_js_start); // pass request, buf, buf length

    return ESP_OK;
}

/**
 * sends icon file when accessing the webpage
 * @param req HTTP request for which the URI needs to be handled
 * @return ESP_OK
*/
static esp_err_t httpServerFaviconIcoHandler(httpd_req_t *req){
    ESP_LOGI(TAG, "favicon.ico requested");
    httpd_resp_set_type(req, "image/x-icon");
    httpd_resp_send(req, (const char *)favicon_ico_start, favicon_ico_end - favicon_ico_start); // pass request, buf, buf length

    return ESP_OK;
}

/**
 * Receives bin file from the webpage and handlers FW update
 * @param req HTTP request for which the URI needs to be handled
 * @return ESP_OK or ESP_FAIL if timeout due to update being unable to start
*/
esp_err_t httpServerOTAUpdateHandler(httpd_req_t *req){
    esp_ota_handle_t otaHandle;

    char otaBuf[1024]; // buffer to hold data received from webpage
    int contentLen = req->content_len;
    int contentRecv = 0;
    int recvLen;
    bool isReqBodyStarted = false;
    bool flashSuccessful = false;

    const esp_partition_t *updatePartition = esp_ota_get_next_update_partition(NULL); // pointer to other partition to write new fw into

    do{
        if ( (recvLen = httpd_req_recv(req, otaBuf, MIN(contentLen, sizeof(otaBuf)))) < 0 ) {
            /* check if timeout occured */
            if(recvLen == HTTPD_SOCK_ERR_TIMEOUT) {
                ESP_LOGI(TAG, "httpServerOTAUpdateHandler: HTTPD_SOCK_ERR_TIMEOUT");
                continue; // try to receive again if timeout occured
            }
            ESP_LOGI(TAG, "httpServerOTAUpdateHandler: OTA other error %d", recvLen);
            return ESP_FAIL;
        }

        printf("httpServerOTAUpdateHandler: OTA Rx: %d of %d\r", contentRecv, contentLen); // log content and its length

        if (!isReqBodyStarted) {
            isReqBodyStarted = true;

            /* get location of .bin file content from request */
            char *p_bodyStart = strstr(otaBuf, "\r\n\r\n") + strlen("\r\n\r\n"); // search for first instance of start chars in web form data
            int bodyPartLen = recvLen - (p_bodyStart - otaBuf); // length of non-junk data

            printf("httpServerOTAUpdateHandler: OTA file size: %d\r\n", contentLen);
            esp_err_t status = esp_ota_begin(updatePartition, OTA_SIZE_UNKNOWN, &otaHandle);
            if(status != ESP_OK) {
                printf("httpServerOTAUpdateHandler: Error with OTA begin, cancelling OTA\r\n");
                return ESP_FAIL;
            }
            else {
                printf("httpServerOTAUpdateHandler: Writing to partition subtype %d at offset 0x%lx\r\n", updatePartition->subtype, updatePartition->address);
            }               

            /* write this first part of the data */
            esp_ota_write(otaHandle, p_bodyStart, bodyPartLen);
            contentRecv += bodyPartLen;
        }
        else {
            /* just write OTA data */
            esp_ota_write(otaHandle, otaBuf, recvLen);
            contentRecv += recvLen;
        }

    } while (recvLen > 0 && contentRecv < contentLen);

    if (esp_ota_end(otaHandle) == ESP_OK){
        /* update partition */
        if(esp_ota_set_boot_partition(updatePartition) == ESP_OK) {
            const esp_partition_t *bootPartition = esp_ota_get_boot_partition(); // pointer to next boot partition (the one we just wrote to)
            ESP_LOGI(TAG, "httpServerOTAUpdateHandler: Next boot partition subtype %d at offset 0x%lx", bootPartition->subtype, bootPartition->address);
            flashSuccessful = true;
        }
        else {
            ESP_LOGI(TAG, "httpServerOTAUpdateHandler: FLASHED ERROR");
        }
    }
    else {
        ESP_LOGI(TAG, "httpServerOTAUpdateHandler: esp_ota_end FLASHED ERROR");       
    }

    if (flashSuccessful){
        httpServerMonitorSendMsg(HTTP_MSG_OTA_UPDATE_SUCCESS);
    }
    else {
        httpServerMonitorSendMsg(HTTP_MSG_OTA_UPDATE_FAILED);
    }

    return ESP_OK;
}

/**
 * OTA status handler responds with the FW update status after the OTA update is started and responds with the compile time/date when the page is first requested
 * @param req HTTP request for which the URI needs to be handled
 * @return ESP_OK
*/
esp_err_t httpServerOTAStatusHandler(httpd_req_t *req){
    char otaJSON[100];

    ESP_LOGI(TAG, "OTAstatus requested");

    sprintf(otaJSON, "{\"ota_update_status\":%d,\"compile_time\":\"%s\",\"compile_date\":\"%s\"}", g_fwUpdateStatus, __TIME__, __DATE__);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, otaJSON, strlen(otaJSON));

    return ESP_OK;
}

/**
 * DHT22 sensor readings json handler; responds with temp and humidity data
 * @param req HTTP request for which the URI needs to be handled
 * @return ESP_OK
*/
static esp_err_t httpServerGetDHTSensorReadingsJsonHandler(httpd_req_t *req){
    ESP_LOGI(TAG, "/dhtSensor.json requested");

    char dhtSensorJson[100];
    sprintf(dhtSensorJson, "{\"temp\":\"%.1f\",\"humidity\":\"%.1f\"}", getTemperature(), getHumidity());
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, dhtSensorJson, strlen(dhtSensorJson));

    return ESP_OK;
}

/**
 * WiFi connect json handler; invoked when connect button is pressed. Receives inputted SSID and pw from webpage.
 * @param req HTTP request for which the URI needs to be handled
 * @return ESP_OK
*/
static esp_err_t httpServerWifiConnectJsonHandler(httpd_req_t *req){
    ESP_LOGI(TAG, "/wifiConnect.json requested");

    size_t ssidLen = 0;
    size_t pwLen = 0;
    char *ssidStr = NULL;
    char *pwStr = NULL;

    /* get SSID header */
    ssidLen = httpd_req_get_hdr_value_len(req, "my-connect-ssid") +1;
    if (ssidLen > 1){
        ssidStr = malloc(ssidLen);
        if(httpd_req_get_hdr_value_str(req, "my-connect-ssid", ssidStr, ssidLen) == ESP_OK){
            ESP_LOGI(TAG, "httpServerWifiConnectJsonHandler(): Found header for my-connect-ssid: %s", ssidStr);
        }
    }

    /* get password header */
    pwLen = httpd_req_get_hdr_value_len(req, "my-connect-pwd") +1;
    if (pwLen > 1){
        pwStr = malloc(pwLen);
        if(httpd_req_get_hdr_value_str(req, "my-connect-pwd", pwStr, pwLen) == ESP_OK){
            ESP_LOGI(TAG, "httpServerWifiConnectJsonHandler(): Found header for my-connect-pwd: %s", pwStr);
        }
    }

    /* update wifi network config */
    wifi_config_t *wifiConfig = wifiAppGetWifiConfig();
    memset(wifiConfig, 0, sizeof(wifi_config_t));
    memcpy(wifiConfig->sta.ssid, ssidStr, ssidLen);
    memcpy(wifiConfig->sta.password, pwStr, pwLen);
    wifiAppSendMsg(WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER);

    free(ssidStr);
    free(pwStr);

    return ESP_OK;
}

/**
 * Updates connection status on the webpage
 * @param req HTTP request for which the URI needs to be handled
 * @return ESP_OK
 */
static esp_err_t httpServerWifiConnectStatusJsonHandler(httpd_req_t *req){
    ESP_LOGI(TAG, "/wifiConnectStatus requested");
    
    char statusJson[100];
    sprintf(statusJson, "{\"wifi_connect_status\":%d}", g_wifiConnectStatus);
    
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, statusJson, strlen(statusJson));

    return ESP_OK;
}

/**
 * Updates connection info on the webpage
 * @param req HTTP request for which the URI needs to be handled
 * @return ESP_OK
*/
static esp_err_t httpServerGetWifiConnectInfoJsonHandler(httpd_req_t *req){
    ESP_LOGI(TAG, "/wifiConnectInfo.json requested");

    char infoJson[200];
    memset(infoJson, 0, sizeof(infoJson));

    char ip[IP4ADDR_STRLEN_MAX]; // max length for an IP4 address
    char netmask[IP4ADDR_STRLEN_MAX];
    char gw[IP4ADDR_STRLEN_MAX];

    if(g_wifiConnectStatus == HTTP_WIFI_STATUS_CONNECT_SUCCESS){
        wifi_ap_record_t wifiData;
        ESP_ERROR_CHECK(esp_wifi_sta_get_ap_info(&wifiData));
        char *ssid = (char*) wifiData.ssid;

        esp_netif_ip_info_t ipInfo;
        ESP_ERROR_CHECK(esp_netif_get_ip_info(espNetifSta, &ipInfo));
        esp_ip4addr_ntoa(&ipInfo.ip, ip, IP4ADDR_STRLEN_MAX); // converts ip to ascii
        esp_ip4addr_ntoa(&ipInfo.netmask, netmask, IP4ADDR_STRLEN_MAX);
        esp_ip4addr_ntoa(&ipInfo.gw, gw, IP4ADDR_STRLEN_MAX);

        sprintf(infoJson, "{\"ip\":\"%s\",\"netmask\":\"%s\",\"gw\":\"%s\",\"ap\":\"%s\"}", ip, netmask, gw, ssid);
    }

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, infoJson, strlen(infoJson));
    
    return ESP_OK;
}

/**
 * Sends a message to the WiFi application to disconnect
 * @param req HTTP request for which the URL needs to be handled
 * @return ESP_OK
*/
static esp_err_t httpServerGetWifiDisconnectJsonHandler(httpd_req_t *req){
    ESP_LOGI(TAG, "/wifiDisconnect.json requested");

    wifiAppSendMsg(WIFI_APP_MSG_USER_REQUESTED_STA_DISCONNECT);

    return ESP_OK;
}

/**
 * Responds to HTTP request by sending local time
 * @param req HTTP request for which the URL needs to be handled
 * @return ESP_OK
*/
static esp_err_t httpServerGetLocalTimeJsonHandler(httpd_req_t *req){
    ESP_LOGI(TAG, "/localTime.json requested");

    char localTimeJson[100] = {0};

    if(g_isLocalTimeSet){
        sprintf(localTimeJson, "{\"time\":\"%s\"}", sntpTimeSyncGetTime());
    }
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, localTimeJson, strlen(localTimeJson));

    return ESP_OK;
}

/**
 * Responds to HTTP request by sending AP SSID
 * @param req HTTP request for which the URL needs to be handled
 * @return ESP_OK
*/
static esp_err_t httpServerGetAPSSIDJsonHandler(httpd_req_t *req){
    ESP_LOGI(TAG, "/apSSID.json requested");
    
    char ssidJson[50];

    wifi_config_t *wifiConfig = wifiAppGetWifiConfig();
    esp_wifi_get_config(ESP_IF_WIFI_AP, wifiConfig);
    char *ssid = (char*) wifiConfig->ap.ssid;
    sprintf(ssidJson, "{\"ssid\":\"%s\"}", ssid);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, ssidJson, strlen(ssidJson));

    return ESP_OK;
}

/**
 * Configures default HTTPD server
 * #return http server instance handle if successful, NULL otherwise
*/
static httpd_handle_t httpServerConfig(void){
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    /* create HTTP monitor task */
    xTaskCreatePinnedToCore(&httpServerMonitor, "HTTP_Server_Monitor", HTTP_SERVER_MONITOR_STACK_SIZE, NULL, HTTP_SERVER_MONITOR_PRIORITY, &httpServerMonitorTaskHandle, HTTP_SERVER_MONITOR_CORE_ID);

    /* create msg queue */
    httpServerMonitorQueueHandle = xQueueCreate(3, sizeof(httpServerQueueMsg_t));

    config.core_id = HTTP_SERVER_TASK_CORE_ID;
    config.task_priority = HTTP_SERVER_TASK_PRIORITY;
    config.stack_size = HTTP_SERVER_TASK_STACK_SIZE;
    config.max_uri_handlers = 20;
    config.recv_wait_timeout = 10;
    config.send_wait_timeout = 10;

    ESP_LOGI(TAG, "httpServerConfig(): Starting server on port: '%d' with task priority: '%d'", config.server_port, config.task_priority);

    /* start httpd server */
    if (httpd_start(&httpServerHandle, &config) == ESP_OK){
        ESP_LOGI(TAG, "httpServerConfig: Registering URI handlers");

        /* register jquery handler */
        httpd_uri_t jquery_js = {
            .uri = "/jquery-3.3.1.min.js",
            .method = HTTP_GET,
            .handler = httpServerJqueryHandler,
            .user_ctx = NULL,
        };
        httpd_register_uri_handler(httpServerHandle, &jquery_js);

        /* register index.html handler */
        httpd_uri_t index_html = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = httpServerIndexHtmlHandler,
            .user_ctx = NULL,
        };
        httpd_register_uri_handler(httpServerHandle, &index_html);        /* register index.html handler */

        /* register app.css handler */
        httpd_uri_t app_css = {
            .uri = "/app.css",
            .method = HTTP_GET,
            .handler = httpServerAppCssHandler,
            .user_ctx = NULL,
        };
        httpd_register_uri_handler(httpServerHandle, &app_css);

        /* register app.js handler */
        httpd_uri_t app_js = {
            .uri = "/app.js",
            .method = HTTP_GET,
            .handler = httpServerAppJsHandler,
            .user_ctx = NULL,
        };
        httpd_register_uri_handler(httpServerHandle, &app_js);

        /* register favicon.ico handler */
        httpd_uri_t favicon_ico = {
            .uri = "/favicon.ico",
            .method = HTTP_GET,
            .handler = httpServerFaviconIcoHandler,
            .user_ctx = NULL,
        };
        httpd_register_uri_handler(httpServerHandle, &favicon_ico);

        /* register OTAupdate handler */
        httpd_uri_t OTA_update = {
            .uri = "/OTAupdate",
            .method = HTTP_POST,
            .handler = httpServerOTAUpdateHandler,
            .user_ctx = NULL,
        };
        httpd_register_uri_handler(httpServerHandle, &OTA_update);

        /* register OTAstatus handler */
        httpd_uri_t OTA_status = {
            .uri = "/OTAstatus",
            .method = HTTP_POST,
            .handler = httpServerOTAStatusHandler,
            .user_ctx = NULL,
        };
        httpd_register_uri_handler(httpServerHandle, &OTA_status);

        /* register dhtSensor.json handler */
        httpd_uri_t dht_sensor_json = {
            .uri = "/dhtSensor.json",
            .method = HTTP_GET,
            .handler = httpServerGetDHTSensorReadingsJsonHandler,
            .user_ctx = NULL,
        };
        httpd_register_uri_handler(httpServerHandle, &dht_sensor_json);

        /* register wifiConnect.json handler */
        httpd_uri_t wifi_connect_json = {
            .uri = "/wifiConnect.json",
            .method = HTTP_POST,
            .handler = httpServerWifiConnectJsonHandler,
            .user_ctx = NULL,
        };
        httpd_register_uri_handler(httpServerHandle, &wifi_connect_json);


        /* register wifiConnectStatus.json handler */
        httpd_uri_t wifi_connect_status_json = {
            .uri = "/wifiConnectStatus",
            .method = HTTP_POST,
            .handler = httpServerWifiConnectStatusJsonHandler,
            .user_ctx = NULL,
        };
        httpd_register_uri_handler(httpServerHandle, &wifi_connect_status_json);

        /* register wifiConnectInfo.json handler */
        httpd_uri_t wifi_connect_info_json = {
            .uri = "/wifiConnectInfo.json",
            .method = HTTP_GET,
            .handler = httpServerGetWifiConnectInfoJsonHandler,
            .user_ctx = NULL,
        };
        httpd_register_uri_handler(httpServerHandle, &wifi_connect_info_json);

        
        /* register wifiDisconnect.json handler */
        httpd_uri_t wifi_disconnect_json = {
            .uri = "/wifiDisconnect.json",
            .method = HTTP_DELETE,
            .handler = httpServerGetWifiDisconnectJsonHandler,
            .user_ctx = NULL,
        };
        httpd_register_uri_handler(httpServerHandle, &wifi_disconnect_json);

        /* register localTime.json handler */
        httpd_uri_t local_time_json = {
            .uri = "/localTime.json",
            .method = HTTP_GET,
            .handler = httpServerGetLocalTimeJsonHandler,
            .user_ctx = NULL,
        };
        httpd_register_uri_handler(httpServerHandle, &local_time_json);

        /* register apSSID.json handler */
        httpd_uri_t ap_ssid_json = {
            .uri = "/apSSID.json",
            .method = HTTP_GET,
            .handler = httpServerGetAPSSIDJsonHandler,
            .user_ctx = NULL,
        };
        httpd_register_uri_handler(httpServerHandle, &ap_ssid_json);

        return httpServerHandle;
    }
    
    return NULL;
}

void httpServerStart(void){
    if(httpServerHandle == NULL){
        httpServerHandle = httpServerConfig();
    }
}

void httpServerStop(void){
    if(httpServerHandle){ // handle != null
        httpd_stop(httpServerHandle);
        ESP_LOGI(TAG, "httpServerStop: Stopping HTTP server");
        httpServerHandle = NULL;
    }
    if(httpServerMonitorTaskHandle){
        vTaskDelete(httpServerMonitorTaskHandle);
        ESP_LOGI(TAG, "httpServerStop: Stopping HTTP server monitor");
        httpServerMonitorTaskHandle = NULL;
    }
}

BaseType_t httpServerMonitorSendMsg(httpServerMsg_e msgID){
    httpServerQueueMsg_t msg;
    msg.msgID = msgID;

    return xQueueSend(httpServerMonitorQueueHandle, &msg, portMAX_DELAY);
}

void httpServerFwUpdateResetCallback(void *arg){
    ESP_LOGI(TAG, "httpServerFwUpdateResetCallback: Timer timed out. Restarting the device");
    esp_restart();
}