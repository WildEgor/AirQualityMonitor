#include "settings.h"

static const char TAG[] = "Settings";

Settings::Settings(
    SettingsDB& settingsDb, 
    WiFiConn& wifiConn, 
    MQTTConn& mqttConn, 
    SensorContainer& sensors,
    CO2Publisher& co2Pub,
    RGBController& rgbController,
    Display& display
) 
    : LoopTickerBase(), 
    _db(&settingsDb.getDB()), 
    _wifi_conn(&wifiConn), 
    _mqtt_conn(&mqttConn),
    _sensors(&sensors), 
    _co2_pub(&co2Pub),
    _rgb_controller(&rgbController), 
    _display(&display),
    _is_initialized(false) {}

void Settings::setup() {
    Serial.println("init settings...");

    _sett = SettingsGyver("AirQualityMonitor v" F_VERSION, _db);
    _sett.begin();

    _sett.onUpdate([this](sets::Updater& u) {
        this->update(u);
    });

    _sett.onBuild([this](sets::Builder& b) {
        this->build(b);
    });

    Serial.println("settings ok!");

    _is_initialized = true;
}

void Settings::exec() {
    if (!_is_initialized) {
        setup();
        return;
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
            b.Input(kk::mqtt_co2_topic, "Топик co2");
            b.Input(kk::mqtt_tvoc_topic, "Топик tvoc");
            b.Button(SH("mqtt_save"), "Подключить");
        }
        {
            sets::Menu m(b, "Датчик CO2");
            b.Number(kk::co2_danger_lvl, "Значение тревоги", nullptr, 0, 8000);
            b.Select(kk::co2_scale_type, "Тип шкалы CO2", co2_scale_types);
            b.Button(SH("co2_save"), "Сохранить");
        }
        {
            sets::Menu m(b, "Common");
            b.Switch(kk::rgb_enabled, "rgb");
            b.Switch(kk::use_dark_theme, "dark theme");
            b.Button(SH("common_save"), "Сохранить");
        }
    }

    if (b.build.isAction()) {
        switch (b.build.id) {
            case SH("wifi_save"):
                Serial.println("wifi_save pressed");

                if (_db && _db->update()) {
                    _wifi_conn->connect();
                    return;
                }

                Serial.println("db update failed for wifi settings!");
                break;
                
            case SH("mqtt_save"):
                Serial.println("mqtt_save pressed");
                
                if (_db && _db->update()) {
                    _mqtt_conn->connect();

                    String new_co2_topic = (*_db)[kk::mqtt_co2_topic].toString();
                    String new_tvoc_topic = (*_db)[kk::mqtt_tvoc_topic].toString();
                    if (!new_co2_topic.isEmpty() && !new_tvoc_topic.isEmpty()) {
                        _co2_pub->setTopics(new_co2_topic, new_tvoc_topic);
                    }

                    return;
                }

                Serial.println("db update failed for MQTT settings!");
                break;
            
            case SH("co2_save"):
                Serial.println("co2_save pressed");

                if (_db && _db->update()) {
                    return;
                }

                Serial.println("db update failed for CO2 settings!");
                break;
                
            case SH("common_save"):
                Serial.println("common_save pressed");
                if (_db && _db->update()) {
                    _rgb_controller->toggle((*_db)[kk::rgb_enabled].toBool());
                    _display->setTheme((*_db)[kk::use_dark_theme].toBool());
                    return;
                }

                Serial.println("db update failed for common settings!");
                break;
        }
    }
}
