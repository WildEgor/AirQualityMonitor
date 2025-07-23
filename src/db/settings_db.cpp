#include "configs/config.h"
#include "settings_db.h"

/**
 * @var co2_scale_types
 * @brief Colors CO2: 3 or 4 color ranges
 */
String co2_scale_types = "3 color;4 color";
/**
 * @var log_levels
 * @brief Log levels
 */
String log_levels = "DEBUG;INFO;WARN;ERROR";

SettingsDB::SettingsDB() : LoopTickerBase(), _db(&LittleFS, DB_NAME)
{
    LOG_INFO("init...");
    bool fsInitialized = true;

#ifdef ESP32
    fsInitialized = LittleFS.begin(true);
#else
    fsInitialized = LittleFS.begin();
#endif

    if (!fsInitialized)
    {
        LOG_ERROR("init littlefs failed!");
        return;
    }

    _db.begin();

    /**
     * @note Reset database
     */
#ifdef DB_RESET
    _db.reset();
#endif

    /**
     * @note Init settings: APP, WIFI, MQTT, CO2
     */
    // ============================== APP ==============================
    _db.init(kk::rgb_enabled, RGB_ENABLED);
    _db.init(kk::use_dark_theme, APP_DARK_THEME);
    _db.init(kk::log_lvl, APP_LOG_LEVEL);
    _db.init(kk::rotation_display, TFT_ROTATION_0);
    _db.init(kk::temp_offset, TEMP_DEFAULT_OFFSET);
    _db.init(kk::cfm_fr, false);

    // ============================== WIFI ==============================
    _db.init(kk::wifi_ssid, WIFI_SSID);
    _db.init(kk::wifi_pass, WIFI_PASS);

    // ============================== MQTT ==============================
    _db.init(kk::mqtt_enabled, MQTT_ENABLED);
    _db.init(kk::mqtt_server, MQTT_SERVER);
    _db.init(kk::mqtt_port, MQTT_PORT);
    _db.init(kk::mqtt_username, MQTT_USERNAME);
    _db.init(kk::mqtt_pass, MQTT_PASS);
    _db.init(kk::mqtt_device_id, MQTT_DEFAULT_DEVICE_ID);

    // ============================== CO2 ==============================
    _db.init(kk::co2_scale_type, APP_CO2_DEFAULT_SCALE_TYPE);
    _db.init(kk::co2_alarm_lvl, APP_CO2_DEFAULT_ALERT_TRHLD);

    /**
     * @note Show db dump
     */
    _db.dump(Serial);

    _db.update();

    LOG_INFO("init ok!");

    this->addLoop();
}

void SettingsDB::factory_reset()
{
    _db.reset();

    _db[kk::cfm_fr] = false;
    _db[kk::rgb_enabled] = false;
    _db[kk::use_dark_theme] = false;
    _db[kk::temp_offset] = TEMP_DEFAULT_OFFSET;
    _db[kk::log_lvl] = APP_DEFAULT_LOG_LEVEL;
    _db[kk::rotation_display] = TFT_ROTATION_0;
    _db[kk::wifi_ssid] = EMPTY_SECRET;
    _db[kk::wifi_pass] = EMPTY_SECRET;
    _db[kk::mqtt_enabled] = false;
    _db[kk::mqtt_server] = EMPTY_SECRET;
    _db[kk::mqtt_username] = EMPTY_SECRET;
    _db[kk::mqtt_pass] = EMPTY_SECRET;
    _db[kk::mqtt_device_id] = MQTT_DEFAULT_DEVICE_ID;
    _db[kk::co2_scale_type] = APP_CO2_DEFAULT_SCALE_TYPE;
    _db[kk::co2_alarm_lvl] = APP_CO2_DEFAULT_ALERT_TRHLD;

    _db.update();

    ESP.restart();
}

void SettingsDB::exec()
{
    _db.tick();
}

GyverDBFile &SettingsDB::db()
{
    return _db;
}
