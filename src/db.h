#pragma once
#include <GyverDBFile.h>
#include <LittleFS.h>
#include <Looper.h>

enum kk : size_t {
    wifi_ssid,
    wifi_pass,
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
        _db.init(kk::wifi_ssid, "");
        _db.init(kk::wifi_pass, "");

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
