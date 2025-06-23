#define LOG_COMPONENT "SettingsDB"
#include "services/logger.h"
#include "settings_db.h"

String co2_scale_types = "DEFAULT;EASY";

SettingsDB::SettingsDB() : LoopTickerBase(), _db(&LittleFS, "/settings.db") {}

void SettingsDB::setup() {
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

    // ============================== WIFI ==============================
    _db.init(kk::wifi_ssid, WIFI_SSID);
    _db.init(kk::wifi_pass, WIFI_PASS);

    // ============================== MQTT ==============================
    _db.init(kk::mqtt_enabled, true);
    _db.init(kk::mqtt_server, MQTT_SERVER);
    _db.init(kk::mqtt_port, MQTT_PORT);
    _db.init(kk::mqtt_username, MQTT_USERNAME);
    _db.init(kk::mqtt_pass, MQTT_PASS);
    _db.init(kk::mqtt_co2_topic, MQTT_DEFAULT_CO2_TOPIC);
    _db.init(kk::mqtt_tvoc_topic, MQTT_DEFAULT_TVOC_TOPIC);

    // ============================== CO2 ==============================
    _db.init(kk::co2_scale_type, "DEFAULT");
    _db.init(kk::co2_danger_lvl, RGB_DEFAULT_ALERT_TRHLD);

    // ============================== COMMON ==============================
    _db.init(kk::rgb_enabled, !APP_DARK_THEME);
    _db.init(kk::use_dark_theme, APP_DARK_THEME);

    _db.dump(Serial);

    LOG_INFO("init ok!");
}

void SettingsDB::exec() {
    _db.tick();
}

GyverDBFile& SettingsDB::getDB() {
    return _db;
}
