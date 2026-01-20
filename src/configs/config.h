#pragma once
#include <Arduino.h>
#include "configs/secrets.example.h"

#define STRINGIZER(arg) #arg
#define STR_VALUE(arg) STRINGIZER(arg)

// app
#define APP_NAME "AirQualityMonitor"
#define APP_VERSION STR_VALUE(BUILD_VERSION) // Change version via project.json!
#define APP_PLATFORM STR_VALUE(BUILD_PLATFORM)
#define APP_DEFAULT_LOG_LEVEL "DEBUG"
#define APP_LOG_LEVEL "DEBUG" // DEBUG, ERROR, WARN, INFO
// #define ENABLE_TEST // Enable mock sensor reading
#define APP_DARK_THEME false             // Select color theme
#define APP_CO2_DEFAULT_ALERT_TRHLD 1200 // CO2 threshold for red blinking
#define APP_CO2_DEFAULT_SCALE_TYPE "4 color"
// app

// maint
// #define DB_RESET // Factory reset database
#ifdef DB_RESET
#include "configs/secrets.h" // HINT: for development only
#endif
#define DB_NAME "/settings.db"
#define PROJECT_PATH "WildEgor/AirQualityMonitor/master/project.json"
#define USER_MANUAL_URL "https://github.com/WildEgor/AirQualityMonitor/blob/master/docs/en/UserManual.md"

// feature flags
// #define WEB_PANEL_DASHBOARD
// maint

// System constants (do not change)
#define CCS811_ADDR 0x5A
#define ENS160_ADDR 0x53 // 0x52 or 0x53
#define BME280_ADDR 0x76 // 0x77
#define TEMP_DEFAULT_OFFSET 15
#define SERIAL_SPEED 115200
#define MS_100 100
#define MS_500 500
#define SEC_1 1000
#define SEC_3 3000
#define SEC_5 5000
#define SEC_10 10000
#define SEC_15 15000
#define SEC_30 30000
#define EMPTY_SECRET "*****"
// system

// MQTT service for interaction with Yandex (see wqtt.ru)
#define MQTT_ENABLED false
#define MQTT_SERVER "m8.wqtt.ru"
#define MQTT_PORT 20336
#define MQTT_DEFAULT_DEVICE_ID "common/aqm" // Used as topic prefix for uniqueness
#define MQTT_DEFAULT_CO2_TOPIC "co2"
#define MQTT_DEFAULT_TVOC_TOPIC "tvoc"
#define MQTT_DEFAULT_AQI_TOPIC "aqi"
#define MQTT_DEFAULT_TEMP_TOPIC "temp"
#define MQTT_DEFAULT_PRESSURE_TOPIC "pressure"
#define MQTT_DEFAULT_HUMIDITY_TOPIC "humidity"
// mqtt

// WiFi settings (see also secrets.example.h)
#define WIFI_AP_NAME "AQM_AP" // Prefix for Wi-Fi access point with settings
#define WIFI_AP_PASS "adminadmin"
#define WIFI_CONN_RETRY_TIMEOUT 15 // seconds
// wifi

// RGB settings
#define RGB_ENABLED false
#define RGB_NUMPIXELS 4 // Number of LEDs in the strip. Min: 1, Max: 255
// rgb settings

/**
 * NOTE: Make changes here
 * To change pins: .pio/libdeps/mhetesp32devkit/TFT_eSPI/User_Setups/Setup200_GC9A01.h
 * Uncomment the correct driver: .pio/libdeps/mhetesp32devkit/TFT_eSPI/User_Setup_Select.h
 */
// hmi
#define TFT_WIDTH 240
#define TFT_HEIGHT 240
#define TFT_ROTATION_0 2 // start position
#define TFT_ROTATION_360 0
// hmi