#define OTA_UPDATE_PENDING              0
#define OTA_UPDATE_SUCCESS              1
#define OTA_UPDATE_FAILED               -1

/**
 * Connection statuses for WiFi
*/
typedef enum httpServerWifiConnectStatus{
    NONE = 0,
    HTTP_WIFI_STATUS_WIFI_CONNECTING,
    HTTP_WIFI_STATUS_CONNECT_FAILED,
    HTTP_WIFI_STATUS_CONNECT_SUCCESS,
    HTTP_WIFI_STATUS_DISCONNECTED,
}httpServerWifiConnectStatus_e;

/**
 * Messages for HTTP monitor
*/

typedef enum httpServerMsg{
    HTTP_MSG_WIFI_CONNECT_INIT = 0,
    HTTP_MSG_WIFI_CONNECT_SUCCESS,
    HTTP_MSG_WIFI_CONNECT_FAILED,
    HTTP_MSG_WIFI_USER_DISCONNECT,
    HTTP_MSG_OTA_UPDATE_SUCCESS,
    HTTP_MSG_OTA_UPDATE_FAILED,
} httpServerMsg_e;

/**
 * Message queue struct
*/

typedef struct httpServerQueueMsg{
    httpServerMsg_e msgID;
} httpServerQueueMsg_t;

/**
 * Sends a message to the queue
 * @param msgID msg ID from the httpServerMessage_e enum
 * @return pdTRUE if an item was successfully send, otherwise pdFALSE
*/
BaseType_t httpServerMonitorSendMsg(httpServerMsg_e msgID);

/**
 * Starts HTTP server
*/
void httpServerStart(void);

/**
 * Stops HTTP server
*/
void httpServerStop(void);

/**
 * Timer callback which calls esp_restart upon successful firmware update
*/
void httpServerFwUpdateResetCallback(void *arg);