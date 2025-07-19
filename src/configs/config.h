#pragma once
#include <Arduino.h>
// #include "configs/secrets.h" // HINT: for development only
#include "configs/secrets.example.h"

#define STRINGIZER(arg) #arg
#define STR_VALUE(arg) STRINGIZER(arg)

// app
#define APP_NAME "AirQualityMonitor"
#define APP_VERSION STR_VALUE(BUILD_VERSION) // Change version via project.json!
#define APP_LOG_LEVEL "DEBUG"                 // DEBUG, ERROR, WARN, INFO
// #define ENABLE_TEST // Enable mock sensor reading
#define APP_DARK_THEME false // Select color theme
// app

// maint
// #define DB_RESET // Factory reset database
#define DB_NAME "/settings.db"
#define PROJECT_PATH "WildEgor/AirQualityMonitor/master/project.json"
#define USER_MANUAL_URL "https://github.com/WildEgor/AirQualityMonitor/blob/master/docs/en/UserManual.md"

// feature flags
// #define WEB_PANEL_DASHBOARD
// maint

// System constants (do not change)
#define CCS811_ADDR 0x5A
#define BME280_ADDR 0x76 // 0x77 or 0x76
#define SERIAL_SPEED 115200
#define MS_100 100
#define MS_500 500
#define SEC_1 1000
#define SEC_3 3000
#define SEC_5 5000
#define SEC_10 10000
#define SEC_30 30000
// system

// MQTT service for interaction with Yandex (see wqtt.ru)
#define MQTT_ENABLED false
#define MQTT_SERVER "m8.wqtt.ru"
#define MQTT_PORT 20336
#define MQTT_DEFAULT_DEVICE_ID "common/aqm" // Used as topic prefix for uniqueness
#define MQTT_DEFAULT_CO2_TOPIC "co2"
#define MQTT_DEFAULT_TVOC_TOPIC "tvoc"
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
#define RGB_PIN 19
#define RGB_NUMPIXELS 4              // Number of LEDs in the strip. Min: 1, Max: 255
#define RGB_DEFAULT_ALERT_TRHLD 1200 // CO2 threshold for red blinking
// rgb settings

// hmi
#define TFT_WIDTH 240
#define TFT_HEIGHT 240
#define TFT_ROTATION_0 2 // start position
#define TFT_ROTATION_360 0

/**
 * NOTE: Make changes here
 * To change pins: .pio/libdeps/mhetesp32devkit/TFT_eSPI/User_Setups/Setup200_GC9A01.h
 * Uncomment the correct driver: .pio/libdeps/mhetesp32devkit/TFT_eSPI/User_Setup_Select.h
 */
#define GC9A01_DRIVER

// esp_32_live_mini
#define TFT_MOSI 23 // On some display driver boards, it might be labeled as "SDA" etc.
#define TFT_SCLK 18
#define TFT_CS 5   // Chip select control pin
#define TFT_DC 16  // Data/Command control pin
#define TFT_RST 17 // Reset pin (can be connected to Arduino RESET pin)
// esp_32_s2_mini
// #define TFT_MOSI 9
// #define TFT_SCLK 11
// #define TFT_CS   5
// #define TFT_DC   7
// #define TFT_RST  3

#define SPI_FREQUENCY 27000000
#define SPI_READ_FREQUENCY 5000000

#define LOAD_GLCD  // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2 // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4 // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6 // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7 // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
#define LOAD_FONT8 // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define LOAD_GFXFF // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts
#define SMOOTH_FONT
// hmi