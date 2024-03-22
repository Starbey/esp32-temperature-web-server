#include "esp_http_server.h"
#include "esp_log.h"

#include "http_server.h"
#include "tasks_common.h"
#include "wifi_app.h"

static const char TAG[] = "http_server"; // tag for console msgs
static httpd_handle_t httpServerHandle = NULL;

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
 * Configures default HTTPD server
 * #return http server instance handle if successful, NULL otherwise
*/
static httpd_handle_t httpServerConfig(void){
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    /* todo: create HTTP monitor task */

    /* todo: create msg queue */

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
        ESP_LOGI(TAG, "http_server_stop: Stopping HTTP server");
        httpServerHandle = NULL;
    }
}