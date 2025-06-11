#pragma once

// system 
#define APP_NAME "AirQualityMonitor"
#define APP_LOG_LEVEL "DEBUG"
#define F_VERSION "1.0"
#define RESET_DB false // Factory reset db
#define USE_DARK_THEME true // Choose color theme
#define MS_100 100
#define SEC_1 1000
#define SEC_5 5000
#define SEC_10 10000
#define SEC_30 30000

// mqtt service to interact with yandex (see wqtt.ru)
#define MQTT_SERVER "m8.wqtt.ru"
#define MQTT_PORT 20336
#define MQTT_DEFAULT_CO2_TOPIC "common/aqm/co2"
#define MQTT_DEFAULT_TVOC_TOPIC "common/aqm/tvoc"

// wifi settings (see secrets.example.h too)
#define WIFI_AP_NAME "AQM_AP" // Prefix for wi-fi point with settings
#define WIFI_AP_PASS "adminadmin"
#define WIFI_CONN_RETRY_COUNT 5
#define WIFI_CONN_RETRY_TIMEOUT 500 // ms

// rgb settings
#define RGB_PIN         19
#define RGB_NUMPIXELS   4 // Number of LEDs in the strip. Min: 1, Max: 255
#define RGB_DEFAUL_ALERT_TRHLD 1200