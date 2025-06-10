#include "settings_db.h"

String co2_scale_types = "DEFAULT;EASY";
static const char TAG[] = "SettingsDB";

SettingsDB::SettingsDB() : LoopTickerBase(), _db(&LittleFS, "/settings.db") {}

void SettingsDB::setup() {
    Serial.println("init db...");
    bool fsInitialized = true;

#ifdef ESP32
    fsInitialized = LittleFS.begin(true);
#else
    fsInitialized = LittleFS.begin();
#endif

    if (!fsInitialized) {
        Serial.println("init littlefs failed");
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

    // ============================== CO2 ==============================
    _db.init(kk::co2_measure_prd, 60);
    _db.init(kk::co2_pub_prd, 60);
    _db.init(kk::co2_scale_type, "DEFAULT");
    _db.init(kk::co2_danger_lvl, 1200);

    // ============================== COMMON ==============================
    _db.init(kk::rgb_enabled, false);
    _db.init(kk::use_dark_theme, true);

    _db.dump(Serial);

    Serial.println("db ok!");
}

void SettingsDB::exec() {
    _db.tick();
}

GyverDBFile& SettingsDB::getDB() {
    return _db;
}
