#define LOG_COMPONENT "SettingsDB"
#include "services/logger.h"
#include "configs/config.h"
#include "settings_db.h"

String co2_scale_types = "DEFAULT;EASY";
String log_levels = "DEBUG;INFO;WARN;ERROR";

SettingsDB::SettingsDB() : LoopTickerBase(), _db(&LittleFS, DB_NAME) {
    LOG_INFO("init...");
    bool fsInitialized = true;

#ifdef ESP32
    fsInitialized = LittleFS.begin(true);
#else
    fsInitialized = LittleFS.begin();
#endif

    if (!fsInitialized) {
        LOG_ERROR("init littlefs failed!");
        return;
    }

    _db.begin();
#ifdef RESET_DB
    _db.reset();
#endif

    // ============================== APP ==============================
    _db.init(kk::rgb_enabled, !APP_DARK_THEME);
    _db.init(kk::use_dark_theme, APP_DARK_THEME);
    _db.init(kk::log_level, APP_LOG_LEVEL);

    // ============================== WIFI ==============================
    _db.init(kk::wifi_ssid, WIFI_SSID);
    _db.init(kk::wifi_pass, WIFI_PASS);

    // ============================== MQTT ==============================
    _db.init(kk::mqtt_enabled, true);
    _db.init(kk::mqtt_server, MQTT_SERVER);
    _db.init(kk::mqtt_port, MQTT_PORT);
    _db.init(kk::mqtt_username, MQTT_USERNAME);
    _db.init(kk::mqtt_pass, MQTT_PASS);
    _db.init(kk::mqtt_device_id, MQTT_DEFAULT_DEVICE_ID);

    // ============================== CO2 ==============================
    _db.init(kk::co2_scale_type, "DEFAULT");
    _db.init(kk::co2_danger_lvl, RGB_DEFAULT_ALERT_TRHLD);

#ifdef ENABLE_TEST
    _db.dump(Serial);
#endif

    LOG_INFO("init ok!");

    this->addLoop();
}

void SettingsDB::exec() {
    _db.tick();
}

GyverDBFile& SettingsDB::getDB() {
    return _db;
}
