| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# _ESP32 Web Server_

A web server project using the ESP32's WiFi capabilities. Also incorporates AWS IoT Core. Made using ESP-IDF and FreeRTOS.



## Features
- WiFi task allows ESP32 to act as both a station and SoftAP
- Stores user AP credentials in non-volatile storage
- Web server handles HTTP requests received from a JavaScript webpage
- Task to periodically poll a DHT22 sensor
- User can view system logs and DHT22 data through the AWS IoT core message broker, which publishes messages via MQTT