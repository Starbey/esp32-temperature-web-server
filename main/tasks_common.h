/**** wifi application task ****/
#define WIFI_APP_TASK_STACK_SIZE            4096
#define WIFI_APP_TASK_PRIORITY              5
#define WIFI_APP_TASK_CORE_ID               0

/**** http server task ****/
#define HTTP_SERVER_TASK_STACK_SIZE         8192
#define HTTP_SERVER_TASK_PRIORITY           4
#define HTTP_SERVER_TASK_CORE_ID            0

/**** http server monitor task ****/
#define HTTP_SERVER_MONITOR_STACK_SIZE      4096
#define HTTP_SERVER_MONITOR_PRIORITY        3
#define HTTP_SERVER_MONITOR_CORE_ID         0

/**** DHT22 sensor task ****/
#define DHT22_TASK_STACK_SIZE               4096
#define DHT22_TASK_PRIORITY                 5
#define DHT22_TASK_CORE_ID                  1 // will never block, so better to execute on another core so it doesn't preempt anything

/**** wifi reset button task ****/
#define WIFI_RESET_BUTTON_TASK_STACK_SIZE   2048
#define WIFI_RESET_BUTTON_TASK_PRIOTIY      6
#define WIFI_RESET_BUTTON_TASK_CORE_ID      0

/**** sntp time sync task ****/
#define SNTP_TIME_SYNC_TASK_STACK_SIZE      4096
#define SNTP_TIME_SYNC_TASK_PRIORITY        4
#define SNTP_TIME_SYNC_TASK_CORE_ID         1

/**** aws iot task ****/
#define AWS_IOT_TASK_STACK_SIZE             9216
#define AWS_IOT_TASK_PRIORITY               6
#define AWS_IOT_TASK_CORE_ID                1