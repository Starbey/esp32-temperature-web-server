#include "esp_http_server.h"
#include "esp_log.h"

#include "http_server.h"
#include "tasks_common.h"
#include "wifi_app.h"

static const char TAG[] = "http_server"; // tag for console msgs
static httpd_handle_t httpServerHandle = NULL;

static TaskHandle_t httpServerMonitorTaskHandle = NULL;

static QueueHandle_t httpServerMonitorQueueHandle;

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
                    break;
                case HTTP_MSG_WIFI_CONNECT_SUCCESS:
                    ESP_LOGI(TAG, "HTTP_MSG_WIFI_CONNECT_SUCCESS");
                    break;
                case HTTP_MSG_WIFI_CONNECT_FAILED:
                    ESP_LOGI(TAG, "HTTP_MSG_WIFI_CONNECT_FAILED");
                    break;
                case HTTP_MSG_OTA_UPDATE_INIT:
                    ESP_LOGI(TAG, "HTTP_MSG_WIFI_OTA_UPDATE_INIT");
                    break;
                case HTTP_MSG_OTA_UPDATE_SUCCESS:
                    ESP_LOGI(TAG, "HTTP_MSG_WIFI_OTA_UPDATE_SUCCESS");
                    break;
                case HTTP_MSG_OTA_UPDATE_FAILED:
                    ESP_LOGI(TAG, "HTTP_MSG_WIFI_OTA_UPDATE_FAILED");
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
 * @param req HTTP request for which  the URI needs to be handled
 * @return ESP_OK
*/
static esp_err_t httpServerFaviconIcoHandler(httpd_req_t *req){
    ESP_LOGI(TAG, "favicon.ico requested");
    httpd_resp_set_type(req, "image/x-icon");
    httpd_resp_send(req, (const char *)favicon_ico_start, favicon_ico_end - favicon_ico_start); // pass request, buf, buf length

    return ESP_OK;
}

/**
 * Configures default HTTPD server
 * #return http server instance handle if successful, NULL otherwise
*/
static httpd_handle_t httpServerConfig(void){
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    /* create HTTP monitor task */
    xTaskCreatePinnedToCore(&httpServerMonitor, "HTTP_Server_Monitor", NULL, HTTP_SERVER_MONITOR_STACK_SIZE, HTTP_SERVER_MONITOR_PRIORITY, &httpServerMonitorTaskHandle, HTTP_SERVER_MONITOR_CORE_ID);

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