/**
 * Messages for HTTP monitor
*/

typedef enum httpServerMsg{
    HTTP_MSG_WIFI_CONNECT_INIT = 0,
    HTTP_MSG_WIFI_CONNECT_SUCCESS,
    HTTP_MSG_WIFI_CONNECT_FAILED,
    HTTP_MSG_OTA_UPDATE_SUCCESSFUL,
    HTTP_MSG_OTA_UPDATE_FAILED,
    HTTP_MSG_OTA_UPDATE_INITIALIZED,
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