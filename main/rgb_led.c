/*
 * rgb_led.c
 *
 *  Created on: Mar. 19, 2024
 *      Author: benja
 */

#include <stdbool.h>

#include "driver/ledc.h"
#include "rgb_led.h"

ledcInfo_t  ledcCh[LED_CHANNEL_NUM];

bool gPwmInitHandle = false; // handle for rgb_led_pwm_init

static void ledPwmInit(void){
	int rgbCh;

	/* red */
	ledcCh[0].channel = LEDC_CHANNEL_0;
	ledcCh[0].gpio = LED_RED_GPIO;
	ledcCh[0].mode = LEDC_HIGH_SPEED_MODE;
	ledcCh[0].timerIndex = LEDC_TIMER_0;

	/* green */
	ledcCh[1].channel = LEDC_CHANNEL_1;
	ledcCh[1].gpio = LED_GREEN_GPIO;
	ledcCh[1].mode = LEDC_HIGH_SPEED_MODE;
	ledcCh[1].timerIndex = LEDC_TIMER_0;

	/* blue */
	ledcCh[2].channel = LEDC_CHANNEL_2;
	ledcCh[2].gpio = LED_BLUE_GPIO;
	ledcCh[2].mode = LEDC_HIGH_SPEED_MODE;
	ledcCh[2].timerIndex = LEDC_TIMER_0;

	/* config timer 0 */
	ledc_timer_config_t ledcTimer = {
			.duty_resolution = LEDC_TIMER_8_BIT,
			.freq_hz = 100,
			.speed_mode = LEDC_HIGH_SPEED_MODE,
			.timer_num = LEDC_TIMER_0
	};

	ledc_timer_config(&ledcTimer);

	/* config channels */
	for (rgbCh = 0; rgbCh< LED_CHANNEL_NUM; rgbCh++){
		ledc_channel_config_t ledcChannel = {
			.channel = ledcCh[rgbCh].channel,
			.duty = 0,
			.hpoint = 0,
			.gpio_num = ledcCh[rgbCh].gpio,
			.intr_type = LEDC_INTR_DISABLE,
			.speed_mode = ledcCh[rgbCh].mode,
			.timer_sel = ledcCh[rgbCh].timerIndex,
		};

		ledc_channel_config(&ledcChannel);
	}
	gPwmInitHandle = true; // ensures ledPwmInit() cannot be called more than once
}

/* sets RGB colour */
static void ledSetColour(uint8_t red, uint8_t green, uint8_t blue){
	/***** values must be from 0-255 for an 8-bit number *****/

	/* red channel */
	ledc_set_duty(ledcCh[0].mode, ledcCh[0].channel, red);
	ledc_update_duty(ledcCh[0].mode, ledcCh[0].channel);

	/* green channel */
	ledc_set_duty(ledcCh[1].mode, ledcCh[1].channel, green);
	ledc_update_duty(ledcCh[1].mode, ledcCh[1].channel);

	/* blue channel */
	ledc_set_duty(ledcCh[2].mode, ledcCh[2].channel, blue);
	ledc_update_duty(ledcCh[2].mode, ledcCh[2].channel);
}

void ledWifiAppStarted(void){
	if(!gPwmInitHandle) ledPwmInit();
	ledSetColour(APP_STARTED_R, APP_STARTED_G, APP_STARTED_B);
}

void ledHttpServerStarted(void){
	if(!gPwmInitHandle) ledPwmInit();
	ledSetColour(SERVER_STARTED_R, SERVER_STARTED_G, SERVER_STARTED_B);
}

void ledWifiConnected(void){
	if(!gPwmInitHandle) ledPwmInit();
	ledSetColour(WIFI_CONNECTED_R, WIFI_CONNECTED_G, WIFI_CONNECTED_B);
}