#include "settings.h"

static const char TAG[] = "Settings";

Settings::Settings() 
    : LoopTickerBase(), _wifi_conn(nullptr), _mqtt_conn(nullptr), _sensors(nullptr), _rgb_controller(nullptr), _needs_init(true) {}

void Settings::setup(
    SettingsDB& settingsDb, 
    WiFiConn& wifiConn, 
    MQTTConn& mqttConn, 
    SensorContainer& sensors,
    RGBController& rgbController
) {
    Serial.println("init settings...");
    _db = &settingsDb.getDB();
    _wifi_conn = &wifiConn;
    _mqtt_conn = &mqttConn;
    _sensors = &sensors;
    _rgb_controller = &rgbController;
}

void Settings::exec() {
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

void Settings::update(sets::Updater& u) {
    // ...
}

void Settings::build(sets::Builder& b) {
    {
        sets::Group g(b, "Настройки");
        {
            sets::Menu m(b, "WiFi");
            b.Input(kk::wifi_ssid, "SSID");
            b.Pass(kk::wifi_pass, "*****");
            b.Button(SH("wifi_save"), "Подключить");
        }
        {
            sets::Menu m(b, "MQTT");
            b.Switch(kk::mqtt_enabled, "Включить");
            b.Input(kk::mqtt_server, "xx.wqtt.ru");
            b.Number(kk::mqtt_port);
            b.Input(kk::mqtt_username, "user");
            b.Pass(kk::mqtt_pass, "pass");
            b.Button(SH("mqtt_save"), "Подключить");
        }
        {
            sets::Menu m(b, "Датчик CO2");
            b.Time(kk::co2_measure_prd, "Период измерений");
            b.Time(kk::co2_pub_prd, "Период публикаций");
            b.Number(kk::co2_danger_lvl, "Значение тревоги", nullptr, 0, 8000);
            b.Select(kk::co2_scale_type, "Тип шкалы CO2", co2_scale_types);
            b.Button(SH("co2_save"), "Сохранить");
        }
        {
            sets::Menu m(b, "RGB");
            b.Switch(kk::rgb_enabled, "Включить");
            b.Button(SH("rgb_save"), "Сохранить");
        }
    }

    if (b.build.isAction()) {
        switch (b.build.id) {
            case SH("wifi_save"):
                Serial.println("wifi_save pressed");
                if (_db && _db->update()) {
                    _wifi_conn->connect();
                } else {
                    Serial.println("db update failed for wifi settings!");
                }
                break;
                
            case SH("mqtt_save"):
                Serial.println("mqtt_save pressed");
                if (_db && _db->update()) {
                    _mqtt_conn->connect();
                } else {
                    Serial.println("db update failed for MQTT settings!");
                }
                break;
            
            case SH("co2_save"):
                Serial.println("co2_save pressed");
                if (_db && _db->update()) {
                    uint32_t new_interval = (*_db)[kk::co2_measure_prd].toInt();
                    if (new_interval > 0 && new_interval < 10000) {
                        _sensors->updateInterval("co2", new_interval);
                    } else {
                        Serial.println("invalid CO2 measurement interval!");
                    }
                } else {
                    Serial.println("db update failed for CO2 settings!");
                }
                break;
                
            case SH("rgb_save"):
                Serial.println("rgb_save pressed");
                if (_db && _db->update()) {
                    bool enabled = (*_db)[kk::rgb_enabled].toBool();
                    _rgb_controller->toggle(enabled);
                } else {
                    Serial.println("db update failed for RGB settings!");
                }
                break;
        }
    }
}
