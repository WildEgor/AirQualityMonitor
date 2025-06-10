#pragma once

#define F_VERSION "1.0"
#define AP_NAME "AQM_AP" // Prefix for wi-fi point with settings
#define AP_PASS "adminadmin"
#define RESET_DB true

#define MQTT_SERVER "m8.wqtt.ru"
#define MQTT_PORT 20336
#define MQTT_DEFAULT_CO2_TOPIC "common/aqm/co2"
#define MQTT_DEFAULT_TVOC_TOPIC "common/aqm/tvoc"

#define WIFI_CONN_RETRY_COUNT 5
#define WIFI_CONN_RETRY_TIMEOUT 500 // ms

#define RGB_PIN         19
#define RGB_NUMPIXELS   4 // Number of LEDs in the strip. Min: 1, Max: 255

#define MS_100 100
#define SEC_1 1000
#define SEC_5 5000
#define SEC_30 30000