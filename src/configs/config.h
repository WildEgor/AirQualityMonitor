#pragma once

// app 
#define APP_NAME "AirQualityMonitor"
#define APP_VERSION "1.2.3" // change version via project.json also!
#define APP_PLATFORM esp_32_d1_mini // "esp_32_d1_mini" "esp_32_s2_mini" "esp8266_d1_mini"
#define APP_LOG_LEVEL "DEBUG" // DEBUG, ERROR, WARN, INFO
// #define ENABLE_TEST // mock sensor reading
#define APP_DARK_THEME true // Choose color theme
// app

// maint
#define DB_RESET // Factory reset db
#define DB_NAME "/settings.db"
#define PROJECT_PATH "WildEgor/AirQualityMonitor/develop/project.json"
// maint

// system (dont change)
#define MS_100 100
#define MS_500 500
#define SEC_1 1000
#define SEC_5 5000
#define SEC_10 10000
#define SEC_30 30000
// system

// mqtt service to interact with yandex (see wqtt.ru)
#define MQTT_ENABLED true
#define MQTT_SERVER "m8.wqtt.ru"
#define MQTT_PORT 20336
#define MQTT_DEFAULT_DEVICE_ID "common/aqm" // used as topic prefix for unique
#define MQTT_DEFAULT_CO2_TOPIC "co2"
#define MQTT_DEFAULT_TVOC_TOPIC "tvoc"
#define MQTT_DEFAULT_TEMP_TOPIC "temp"
#define MQTT_DEFAULT_PRESSURE_TOPIC "pressure"
// mqtt

// wifi settings (see secrets.example.h too)
#define WIFI_AP_NAME "AQM_AP" // Prefix for wi-fi point with settings
#define WIFI_AP_PASS "adminadmin"
#define WIFI_CONN_RETRY_TIMEOUT 15 // sec
// wifi

// rgb settings
#define RGB_ENABLED false
#define RGB_PIN         19
#define RGB_NUMPIXELS   4 // Number of LEDs in the strip. Min: 1, Max: 255
#define RGB_DEFAULT_ALERT_TRHLD 1200 // When blink with red led
// rgb settings

// hmi
/**
 * HINT: make changes here
 * change pins: .pio/libdeps/mhetesp32devkit/TFT_eSPI/User_Setups/Setup200_GC9A01.h
 * uncomment using correct driver: .pio/libdeps/mhetesp32devkit/TFT_eSPI/User_Setup_Select.h
 */
#define GC9A01_DRIVER

#define TFT_WIDTH  240
#define TFT_HEIGHT 240

#if APP_PLATFORM == esp_32_d1_mini
    #define TFT_MOSI 23 // In some display driver board, it might be written as "SDA" and so on.
    #define TFT_SCLK 18
    #define TFT_CS   5  // Chip select control pin
    #define TFT_DC   16  // Data Command control pin
    #define TFT_RST  17  // Reset pin (could connect to Arduino RESET pin)
    #define SPI_FREQUENCY  27000000
    #define SPI_READ_FREQUENCY  5000000
#endif

#if APP_PLATFORM == esp_32_s2_mini
    #define TFT_MOSI 35 // 11
    #define TFT_SCLK 37 // 7, 36
    #define TFT_CS   33
    #define TFT_DC   34
    #define TFT_RST  36
    #define SPI_FREQUENCY  40000000 
    #define SPI_READ_FREQUENCY  5000000
#endif

#if APP_PLATFORM == esp8266_d1_mini 
#endif

#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts
#define SMOOTH_FONT
// hmi