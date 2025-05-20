#pragma once
#include <GyverDBFile.h>
#include <LittleFS.h>
#include <Looper.h>
#include "config.h"

enum kk : size_t {
    wifi_ssid,
    wifi_pass,
    mqtt_enabled,
    mqtt_server,
    mqtt_port,
    mqtt_username,
    mqtt_pass,
    co2_measure_prd,
    co2_pub_prd,
};

class SettingsDB : public LoopTickerBase {
public:
    SettingsDB() : LoopTickerBase(), _db(&LittleFS, "/settings.db") {}

    void setup() {
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

        // ============================== WIFI ==============================
        _db.init(kk::wifi_ssid, "");
        _db.init(kk::wifi_pass, "");

        // ============================== MQTT ==============================
        _db.init(kk::mqtt_enabled, true);
        _db.init(kk::mqtt_server, MQTT_SERVER);
        _db.init(kk::mqtt_port, MQTT_PORT);
        _db.init(kk::mqtt_username, MQTT_USERNAME);
        _db.init(kk::mqtt_pass, MQTT_PASS);

        // ============================== CO2 ==============================
        _db.init(kk::co2_measure_prd, 60);
        _db.init(kk::co2_pub_prd, 60);

        Serial.println("db ok!");
    }

    void exec() override {
        _db.tick();
    }

    GyverDBFile& getDB() {
        return _db;
    }

private:
    GyverDBFile _db;
};
