#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "lwip/netdb.h"

#include "rgb_led.h"
#include "tasks_common.h"
#include "wifi_app.h"
#include "http_server.h"
#include "app_nvs.h"

static const char TAG[] = "wifi_app";

/* wifi app callback */
static wifiConnectedEventCallback_t wifiConnectedEventCb;

wifi_config_t *wifiConfig = NULL;

static int g_retryNumber; // number of retries when a connection attempt fails

/* wifi application event group handle and status bits; used to tell app whether we're connecting with saved credentials or from the http server */
static EventGroupHandle_t wifiAppEventGroup;
const int WIFI_APP_CONNECTING_USING_SAVED_CREDS_BIT = BIT0;
const int WIFI_APP_CONNECTING_FROM_HTTP_SERVER_BIT = BIT1;
const int WIFI_APP_USER_REQUESTED_STA_DISCONNECT_BIT = BIT2;
const int WIFI_APP_STA_CONNECTED_GOT_IP_BIT = BIT3; // set when ESP32 has a connection and is assigned an IP

static QueueHandle_t wifiAppQueueHandle; // event queue handle
esp_netif_t* espNetifSta = NULL;
esp_netif_t* espNetifAp = NULL;

/**
 * WiFi application event handler
 * @param arg data, aside from event data that is passed to the handler when it is called
 * @param eventBase base ID of the event to register the handler for (base = group of events)
 * @param eventId ID of the event to register the handler for (specific event within base)
 * @param eventData event data
*/
static void wifiAppEventHandler(void *arg, esp_event_base_t eventBase, int32_t eventId, void *eventData){
    if (eventBase == WIFI_EVENT){
        switch(eventId){
            case WIFI_EVENT_AP_START: 
                ESP_LOGI(TAG, "WIFI_EVENT_AP_START");
                break;
            case WIFI_EVENT_AP_STOP: 
                ESP_LOGI(TAG, "WIFI_EVENT_AP_STOP");
                break;
            case WIFI_EVENT_AP_STACONNECTED: 
                ESP_LOGI(TAG, "WIFI_EVENT_AP_STACONNECTED");
                break;
            case WIFI_EVENT_AP_STADISCONNECTED: 
                ESP_LOGI(TAG, "WIFI_EVENT_AP_STADISCONNECTED");
                break;
            case WIFI_EVENT_STA_START: 
                ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
                break;
            case WIFI_EVENT_STA_CONNECTED: 
                ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED");
                break;
            case WIFI_EVENT_STA_DISCONNECTED: 
                ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");

                wifi_event_sta_disconnected_t *wifiEventStaDisconnected;
                wifiEventStaDisconnected = (wifi_event_sta_disconnected_t*) malloc(sizeof(wifi_event_sta_disconnected_t)); // set memory location
                *wifiEventStaDisconnected = *( (wifi_event_sta_disconnected_t*) eventData ); //set value
                printf("WIFI_EVENT_STA_DISCONNECTED, reason code %d\n", wifiEventStaDisconnected->reason);

                if (g_retryNumber < MAX_CONNECTION_RETRIES){ // keep trying to reconnect until max retries
                    esp_wifi_connect();
                    g_retryNumber++;
                }
                else {
                    wifiAppSendMsg(WIFI_APP_MSG_STA_DISCONNECTED);
                }
                break;
        }
    }
    else if (eventBase == IP_EVENT){
        switch(eventId){
            case IP_EVENT_STA_GOT_IP:
                ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");
                wifiAppSendMsg(WIFI_APP_MSG_STA_CONNECTED_GOT_IP);
                break;
        }
    }
}

/**
 * Inits the WiFi app for WiFi and IP events
*/
static void wifiAppEventHandlerInit(void){
    ESP_ERROR_CHECK(esp_event_loop_create_default()); // create default event loop

    esp_event_handler_instance_t instanceWifiEvent;
    esp_event_handler_instance_t instanceIPEvent;
    
    /* register wifi event handler to main event loop */
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifiAppEventHandler, NULL, &instanceWifiEvent));

    /* register IP event handler to main event loop */
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, ESP_EVENT_ANY_ID, &wifiAppEventHandler, NULL, &instanceIPEvent));
}

/**
 * Initializes the TCP and default WiFi config
*/

static void wifiAppDefaultWifiInit(void){
    ESP_ERROR_CHECK(esp_netif_init()); // initializes the TCP stack

    /* default WiFi config */
    wifi_init_config_t wifiInitConfig = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifiInitConfig));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    espNetifSta = esp_netif_create_default_wifi_sta();
    espNetifAp = esp_netif_create_default_wifi_ap();
}

/**
 * Configures the WiFi access point settings and assigns the static IP to the SoftAP
*/

static void wifiAppSoftApConfig(void){
    /* config wifi access point (AP) */
    wifi_config_t apConfig = {
        .ap = {
            .ssid = WIFI_AP_SSID,
            .ssid_len = strlen(WIFI_AP_SSID),
            .password = WIFI_AP_PASSWORD,
            .channel = WIFI_AP_CHANNEL,
            .ssid_hidden = WIFI_AP_SSID_HIDDEN,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .max_connection = WIFI_AP_MAX_CONNECTIONS,
            .beacon_interval = WIFI_AP_BEACON_INTERVAL,
        },
    };

    /* config DHCP for the AP */
    esp_netif_ip_info_t apIpInfo;
    memset(&apIpInfo, 0, sizeof(apIpInfo)); // clear apIpInfo before setting

    esp_netif_dhcps_stop(espNetifAp); // stops the DHCP server; must call before making any changes to the DHCP server

    /* assign AP's static IP, gateway, and netmask; converts internet address from standard text into binary form */
    inet_pton(AF_INET, WIFI_AP_IP, &apIpInfo.ip);
    inet_pton(AF_INET, WIFI_AP_GATEWAY, &apIpInfo.gw);
    inet_pton(AF_INET, WIFI_AP_NETMASK, &apIpInfo.netmask);

    /* statically config network interface */
    ESP_ERROR_CHECK(esp_netif_set_ip_info(espNetifAp, &apIpInfo));

    /* start AP DHCP server for connecting stations */
    ESP_ERROR_CHECK(esp_netif_dhcps_start(espNetifAp));

    /* set mode as AP/station */ 
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &apConfig)); // set config
    ESP_ERROR_CHECK(esp_wifi_set_bandwidth(WIFI_IF_AP, WIFI_AP_BANDWIDTH)); // set bandwidth
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_STA_POWER_SAVE)); // set bandwidth
}

/**
 * Attempts to connect the ESP32 to an AP using the updated station config
*/
static void wifiAppConnectSta(void){
    ESP_ERROR_CHECK(esp_wifi_set_config( ESP_IF_WIFI_STA, wifiAppGetWifiConfig() )); // note that wifiAppGetWifiConfig() returns a pointer
    ESP_ERROR_CHECK(esp_wifi_connect());
}

/**
 * WiFi app main task
 * @param parameters not in use
*/
static void wifiAppTask(void *parameters){
    wifiAppQueueMsg_t msg;
    EventBits_t eventBits;

    /* initialize event handler */
    wifiAppEventHandlerInit();

    /* initialize TCP/IP stack and wifi config */
    wifiAppDefaultWifiInit();

    /* config softAP */
    wifiAppSoftApConfig();

    /* start wifi */
    ESP_ERROR_CHECK(esp_wifi_start());

    /* send first event msg */
    wifiAppSendMsg(WIFI_APP_MSG_LOAD_SAVED_CREDENTIALS);

    while(1){
        if (xQueueReceive(wifiAppQueueHandle, &msg, portMAX_DELAY)){
            switch(msg.msgID){
                case WIFI_APP_MSG_LOAD_SAVED_CREDENTIALS:
                    ESP_LOGI(TAG, "WIFI_APP_MSG_LOAD_SAVED_CREDENTIALS");

                    if(appNVSLoadStaCreds()){
                        ESP_LOGI(TAG, "Loaded station config");
                        wifiAppConnectSta();
                        xEventGroupSetBits(wifiAppEventGroup, WIFI_APP_CONNECTING_USING_SAVED_CREDS_BIT);
                    }
                    else{
                        ESP_LOGI(TAG, "Unable to load station config");
                    }

                    /* start http server */
                    wifiAppSendMsg(WIFI_APP_MSG_START_HTTP_SERVER);
                    break;

                case WIFI_APP_MSG_START_HTTP_SERVER:
                    ESP_LOGI(TAG, "WIFI_APP_MSG_START_HTTP_SERVER");
                    httpServerStart();
                    ledHttpServerStarted();
                    break;

                case WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER:
                    ESP_LOGI(TAG, "WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER");

                    xEventGroupSetBits(wifiAppEventGroup, WIFI_APP_CONNECTING_FROM_HTTP_SERVER_BIT);

                    /* attempt to connect */
                    wifiAppConnectSta();
                    g_retryNumber = 0;

                    /* let HTTP server know about connection attempt */
                    httpServerMonitorSendMsg(HTTP_MSG_WIFI_CONNECT_INIT);

                    break;

                case WIFI_APP_MSG_STA_CONNECTED_GOT_IP:
                    ESP_LOGI(TAG, "WIFI_APP_MSG_STA_CONNECTED_GOT_IP");
                    xEventGroupSetBits(wifiAppEventGroup, WIFI_APP_STA_CONNECTED_GOT_IP_BIT);
                    ledWifiConnected();
                    httpServerMonitorSendMsg(HTTP_MSG_WIFI_CONNECT_SUCCESS);

                    eventBits = xEventGroupGetBits(wifiAppEventGroup);
                    if(eventBits & WIFI_APP_CONNECTING_USING_SAVED_CREDS_BIT){
                        xEventGroupClearBits(wifiAppEventGroup, WIFI_APP_CONNECTING_USING_SAVED_CREDS_BIT); // clear bits in case we want to disconnect and reconnect
                    }
                    else { // save sta creds only if connecting from http server
                        appNVSSaveStaCreds();
                    }

                    if(eventBits & WIFI_APP_CONNECTING_FROM_HTTP_SERVER_BIT){
                        xEventGroupClearBits(wifiAppEventGroup, WIFI_APP_CONNECTING_FROM_HTTP_SERVER_BIT);
                    }

                    /* check for connection callback. if it's set, call it */
                    if(wifiConnectedEventCb){
                        wifiAppCallCallback();
                    }

                    break;

                case WIFI_APP_MSG_STA_DISCONNECTED:
                    ESP_LOGI(TAG, "WIFI_APP_MSG_STA_DISCONNECTED");
                    httpServerMonitorSendMsg(HTTP_MSG_WIFI_CONNECT_FAILED);

                    eventBits = xEventGroupGetBits(wifiAppEventGroup);
                    if(eventBits & WIFI_APP_CONNECTING_USING_SAVED_CREDS_BIT){
                        ESP_LOGI(TAG, "WIFI_APP_MSG_STA_DISCONNECTED: Disconnect attempt using saved credentials");
                        xEventGroupClearBits(wifiAppEventGroup, WIFI_APP_CONNECTING_USING_SAVED_CREDS_BIT);
                        appNVSClearStaCreds(); // after maximum (non-user requested) disconnects, delete station creds from NVS
                    }
                    else if(eventBits & WIFI_APP_CONNECTING_FROM_HTTP_SERVER_BIT){
                        ESP_LOGI(TAG, "WIFI_APP_MSG_STA_DISCONNECTED: Disconnect attempt from HTTP server");
                        xEventGroupClearBits(wifiAppEventGroup, WIFI_APP_CONNECTING_FROM_HTTP_SERVER_BIT);
                        appNVSClearStaCreds();
                        httpServerMonitorSendMsg(HTTP_MSG_WIFI_CONNECT_FAILED);
                    }
                    else if(eventBits & WIFI_APP_USER_REQUESTED_STA_DISCONNECT_BIT){
                        ESP_LOGI(TAG, "WIFI_APP_MSG_STA_DISCONNECTED: User requested disconnect");
                        xEventGroupClearBits(wifiAppEventGroup, WIFI_APP_USER_REQUESTED_STA_DISCONNECT_BIT);
                        httpServerMonitorSendMsg(HTTP_MSG_WIFI_USER_DISCONNECT);
                    }
                    else{ // case for when AP is unavailable
                        ESP_LOGI(TAG, "WIFI_APP_MSG_STA_DISCONNECTED: Attempt failed; check WiFi AP availability");
                    }

                    if(eventBits & WIFI_APP_STA_CONNECTED_GOT_IP_BIT){
                        xEventGroupClearBits(wifiAppEventGroup, WIFI_APP_STA_CONNECTED_GOT_IP_BIT);
                    }

                    break;

                case WIFI_APP_MSG_USER_REQUESTED_STA_DISCONNECT:
                    ESP_LOGI(TAG, "WIFI_APP_MSG_USER_REQUESTED_STA_DISCONNECT");

                    eventBits = xEventGroupGetBits(wifiAppEventGroup); // check if there's a valid connection before disconnecting
                    if(eventBits & WIFI_APP_STA_CONNECTED_GOT_IP_BIT){
                        xEventGroupSetBits(wifiAppEventGroup, WIFI_APP_USER_REQUESTED_STA_DISCONNECT_BIT);

                        g_retryNumber = MAX_CONNECTION_RETRIES; // ensures app does not try to reconnect when disconnect button is pressed

                        ESP_ERROR_CHECK(esp_wifi_disconnect());
                        appNVSClearStaCreds();
                        ledHttpServerStarted();
                    }
                    break;

                default:
            }
        }
    }
}

/***** wifi functions *****/

BaseType_t wifiAppSendMsg(wifiAppMsg_e msgID){
    wifiAppQueueMsg_t msg;
    msg.msgID = msgID;
    return xQueueSend(wifiAppQueueHandle, &msg, portMAX_DELAY); // returns pdTRUE if queue is not full
}

wifi_config_t* wifiAppGetWifiConfig(void){
    return wifiConfig;
}

void wifiAppSetCallback(wifiConnectedEventCallback_t callback){
    wifiConnectedEventCb = callback;
}

void wifiAppCallCallback(void){
    wifiConnectedEventCb();
}

void wifiAppStart(void){
    ESP_LOGI(TAG, "STARTING WIFI APPLICATION");
    ledWifiAppStarted();
    esp_log_level_set("wifi", ESP_LOG_NONE); // disable default wifi logging msgs

    /* allocate memory for wifi config */
    wifiConfig = (wifi_config_t*) malloc(sizeof(wifi_config_t));
    memset(wifiConfig, 0, sizeof(wifi_config_t)); // clear out possible pre-existing garbage data before using wifiConfig

    wifiAppQueueHandle = xQueueCreate(3, sizeof(wifiAppQueueMsg_t));

    wifiAppEventGroup = xEventGroupCreate();

    /* start wifi application task */
    xTaskCreatePinnedToCore(&wifiAppTask, "WiFi_App_Task", WIFI_APP_TASK_STACK_SIZE, NULL, WIFI_APP_TASK_PRIORITY, NULL, WIFI_APP_TASK_CORE_ID);

}
