#pragma once
#include <SettingsGyver.h>
#include "config.h"
#include "wifi_checker.h"
#include "db.h"

class Settings : public LoopTickerBase {
public:
    Settings() : LoopTickerBase(), _wifi(nullptr), _needs_init(true) {}

    void setup(SettingsDB& dbWrapper, WiFiChecker& wifiWrapper) {
        Serial.println("init settings...");
        _db = &dbWrapper.getDB();
        _wifi = &wifiWrapper;
    }

    void exec() override {
        if (_needs_init) {
            _sett = SettingsGyver("AirQuality v" F_VERSION, _db);
            _sett.begin();

            _sett.onUpdate([this](sets::Updater& u) {
                this->update(u);
            });

            _sett.onBuild([this](sets::Builder& b) {
                this->build(b);
            });
            
            _needs_init = false;
            Serial.println("settings ok!");
        }
        _sett.tick();
    }

private:
    void update(sets::Updater& u) {}

    void build(sets::Builder& b) {
        {
            sets::Group g(b, "Настройки");
            {
                sets::Menu m(b, "WiFi");
                sets::Group g(b);
                b.Input(kk::wifi_ssid, "SSID");
                b.Pass(kk::wifi_pass, "Pass");
                b.Button(SH("wifi_save"), "Подключить");
            }
        }

        if (b.build.isAction()) {
            switch (b.build.id) {
                case SH("wifi_save"):
                    if (_db) {
                        _db->update();
                        if (_wifi) {
                            _wifi->connect();
                        }
                    }
                    break;
            }
        }
    }

    SettingsGyver _sett;
    GyverDBFile* _db;
    WiFiChecker* _wifi;
    bool _needs_init;
};
