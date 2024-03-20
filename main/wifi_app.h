#ifndef MAIN_WIFI_APP_H_
#define MAIN_WIFI_APP_H_

#include "esp_netif.h"
#include "esp_wifi_types.h"
#include "freertos/FreeRTOS.h"

// Callback typedef
typedef void (*wifi_connected_event_callback_t)(void);

// WiFi application settings
#define WIFI_AP_SSID				"ESP32_AP"			// AP name
#define WIFI_AP_PASSWORD			"starbey"			// AP password
#define WIFI_AP_CHANNEL				1					// AP channel
#define WIFI_AP_SSID_HIDDEN			0					// AP visibility
#define WIFI_AP_MAX_CONNECTIONS		5					// AP max clients
#define WIFI_AP_BEACON_INTERVAL		100					// AP beacon: 100 milliseconds recommended
#define WIFI_AP_IP					"192.168.0.1"		// AP default IP
#define WIFI_AP_GATEWAY				"192.168.0.1"		// AP default Gateway (should be the same as the IP)
#define WIFI_AP_NETMASK				"255.255.255.0"		// AP netmask
#define WIFI_AP_BANDWIDTH			WIFI_BW_HT20		// AP bandwidth 20 MHz (40 MHz is the other option)
#define WIFI_STA_POWER_SAVE			WIFI_PS_NONE		// Power save not used
#define MAX_SSID_LENGTH				32					// IEEE standard maximum
#define MAX_PASSWORD_LENGTH			64					// IEEE standard maximum
#define MAX_CONNECTION_RETRIES		5					// Retry number on disconnect

// netif object for the Station and Access Point
extern esp_netif_t* espNetifSta;
extern esp_netif_t* espNetifAp;

/**
 * Message IDs for the WiFi application task
 * @note Expand this based on your application requirements.
 */
typedef enum wifiAppMsg
{
	WIFI_APP_MSG_START_HTTP_SERVER = 0,
	WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER,
	WIFI_APP_MSG_STA_CONNECTED_GOT_IP,
} wifiAppMsg_e;

/**
 * structure for the message queue
 * @note Expand this based on application requirements e.g. add another type and parameter as required
 */
typedef struct wifiAppQueueMsg
{
	wifiAppMsg_e msgID;
} wifiAppQueueMsg_t;

/**
 * sends a message to the queue
 * @param msgID message ID from the wifi_app_message_e enum.
 * @return pdTRUE if an item was successfully sent to the queue, otherwise pdFALSE.
 */
BaseType_t wifiAppSendMsg(wifiAppMsg_e msgID);

/**
 * Starts the WiFi RTOS task
 */
void wifiAppStart(void);




#endif /* MAIN_WIFI_APP_H_ */


























