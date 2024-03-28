#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/apps/sntp.h"

#include "tasks_common.h"
#include "http_server.h"
#include "wifi_app.h"
#include "sntp_time_sync.h"

static const char TAG[] = "sntp_time_sync";

static bool sntpOpModeSet = false;

/**
 * Init SNTP service using SNTP_OPMODE_POLL mode
*/
static void sntpTimeSyncInitSNTP(void){
    ESP_LOGI(TAG, "Initializing SNTP service");

    if(!sntpOpModeSet){
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
        sntpOpModeSet = true;
    }

    sntp_setservername(0, "pool.ntp.org"); // URL of SNTP web API

    sntp_init();

    httpServerMonitorSendMsg(HTTP_MSG_TIME_SERVICE_INITIALIZED);
}

/**
 * Checks if current time is the same as SNTP time. If not, sync current time
*/
static void sntpTimeSyncObtainTime(void){
    time_t currTime = 0;
    struct tm timeInfo = {0}; // struct that splits time up into day, month, year etc.

    time(&currTime);
    localtime_r(&currTime, &timeInfo);

    /* check if we need to initialize or reinitialize time */
    if(timeInfo.tm_year < (2016 - 1900)){
        sntpTimeSyncInitSNTP(); 
        setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1); // timezone: EST
        tzset();

    }
}

/**
 * SNTP time sync task
 * @param parameters not in use
*/
static void sntpTimeSync(void *parameters){
    while(1){
        sntpTimeSyncObtainTime();
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

char* sntpTimeSyncGetTime(void){
    static char timeBuf[100] = {0};

    time_t currTime = 0;
    struct tm timeInfo = {0};

    time(&currTime);
    localtime_r(&currTime, &timeInfo);

    if(timeInfo.tm_year < (2016 - 1900)){
        ESP_LOGI(TAG, "Time not yet set");
    }
    else {
        strftime(timeBuf, sizeof(timeBuf), "%d.%m.%Y %H:%M:%S", &timeInfo);
        ESP_LOGI(TAG, "Current time info: %s", timeBuf);
    }

    return timeBuf;
}


void sntpTimeSyncTaskStart(void){
    xTaskCreatePinnedToCore(&sntpTimeSync, "SNTP_Time_Sync_Task", SNTP_TIME_SYNC_TASK_STACK_SIZE, NULL, SNTP_TIME_SYNC_TASK_PRIORITY, NULL, SNTP_TIME_SYNC_TASK_CORE_ID);
}