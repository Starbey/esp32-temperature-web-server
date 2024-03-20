/*
 * rgb_led.h
 *
 *  Created on: Mar. 19, 2024
 *      Author: benja
 */

#ifndef MAIN_RGB_LED_H_
#define MAIN_RGB_LED_H_

/* LED GPIOs */
#define LED_RED_GPIO				21
#define LED_GREEN_GPIO				22
#define LED_BLUE_GPIO				23

/* LED colour mix channels */
#define LED_CHANNEL_NUM				3

/* LED colours */
#define APP_STARTED_R				255
#define APP_STARTED_G				42
#define APP_STARTED_B				251

#define SERVER_STARTED_R			255
#define SERVER_STARTED_G			251
#define SERVER_STARTED_B			5

#define WIFI_CONNECTED_R			5
#define WIFI_CONNECTED_G			230
#define WIFI_CONNECTED_B			255

/* RGB LED config. */
typedef struct{
	int channel;
	int gpio;
	int mode;
	int timerIndex;
}ledcInfo_t;

/* wifi application started */
void ledWifiAppStarted(void);

/* http server started */
void ledHttpServerStarted(void);

/* esp32 connected to access point */
void ledWifiConnected(void);



#endif /* MAIN_RGB_LED_H_ */