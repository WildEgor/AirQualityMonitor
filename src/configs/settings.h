#pragma once
#include <SettingsGyver.h>
#include "config.h"
#include "db/settings_db.h"
#include "connections/wifi_conn.h"
#include "connections/mqtt_conn.h"
#include "sensors/co2.h"
#include "sensors/sensor_base.h"

class Settings : public LoopTickerBase {
public:
    Settings() : LoopTickerBase(), _wifi_conn(nullptr), _mqtt_conn(nullptr), _sensors(nullptr), _needs_init(true) {}

    void setup(SettingsDB& dbWrapper, WiFiConn& wifiWrapper, MQTTConn& mqttWrapper, SensorContainer& sensorsWrapper) {
        Serial.println("init settings...");
        _db = &dbWrapper.getDB();
        _wifi_conn = &wifiWrapper;
        _mqtt_conn = &mqttWrapper;
        _sensors = &sensorsWrapper;
    }

    void exec() override {
        if (_needs_init) {
            _sett = SettingsGyver("AirQualityMonitor v" F_VERSION, _db);
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
                b.Pass(kk::wifi_pass, "*****");
                b.Button(SH("wifi_save"), "Подключить");
            }
            {
                sets::Menu m(b, "MQTT");
                sets::Group g(b);
                b.Switch(kk::mqtt_enabled, "Включить");
                b.Input(kk::mqtt_server, "xx.wqtt.ru");
                b.Number(kk::mqtt_port);
                b.Input(kk::mqtt_username, "user");
                b.Pass(kk::mqtt_pass, "pass");
                b.Button(SH("mqtt_save"), "Подключить");
            }
            {
                sets::Group g(b, "Датчик");
                b.Time(kk::co2_measure_prd, "Период измерений");
                b.Time(kk::co2_pub_prd, "Период публикаций");
                b.Select(kk::co2_scale_type, "Тип шкалы CO2", co2_scale_types);
                b.Button(SH("co2_save"), "Сохранить");
            }
        }

        if (b.build.isAction()) {
            switch (b.build.id) {
                case SH("wifi_save"):
                    if (_db) {
                        _db->update();
                        if (_wifi_conn) {
                            _wifi_conn->connect();
                        }
                    }
                    break;
                case SH("mqtt_save"):
                    if (_db) {
                        _db->update();
                        _mqtt_conn->connect();
                    }
                    break;
                case SH("co2_save"):
                    if (_db && _sensors) {
                        _db->update();
                        
                        uint32_t new_interval = (*_db)[kk::co2_measure_prd].toInt();
                        if (new_interval > 0 && new_interval < 10000) {
                            _sensors->updateInterval("co2", new_interval);
                        }
                    }
                    break;
            }
        }
    }

    SettingsGyver _sett;
    GyverDBFile* _db;
    WiFiConn* _wifi_conn;
    MQTTConn* _mqtt_conn;
    SensorContainer* _sensors;
    bool _needs_init;
};
