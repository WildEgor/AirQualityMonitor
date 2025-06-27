#define LOG_COMPONENT "Settings"
#include "services/logger.h"
#include "settings.h"
#include "services/publisher.h"

Settings::Settings(
    SettingsDB& settingsDb, 
    WiFiConn& wifiConn
) 
    : LoopTickerBase(), 
    _db(&settingsDb.getDB()), 
    _wifi_conn(&wifiConn), 
    _is_initialized(false) {
        LOG_INFO("init...");

        _sett = SettingsGyver("AirQualityMonitor v" APP_VERSION, _db);
        _sett.begin();

        _sett.onUpdate([this](sets::Updater& u) {
            this->_update(u);
        });

        _sett.onBuild([this](sets::Builder& b) {
            this->_build(b);
        });

        LOG_INFO("init ok!");

        this->addLoop();
        _is_initialized = true;
    }

Settings::Settings(
    SettingsDB& settingsDb, 
    WiFiConn& wifiConn, 
    MQTTConn& mqttConn, 
    RGBController& rgbController,
    HMI& hmi
) 
    : LoopTickerBase(), 
    _db(&settingsDb.getDB()), 
    _wifi_conn(&wifiConn), 
    _mqtt_conn(&mqttConn), 
    _rgb_controller(&rgbController), 
    _hmi(&hmi),
    _is_initialized(false) {
        LOG_INFO("init...");

        _sett = SettingsGyver("AirQualityMonitor v" APP_VERSION, _db);
        _sett.begin();

        _sett.onUpdate([this](sets::Updater& u) {
            this->_update(u);
        });

        _sett.onBuild([this](sets::Builder& b) {
            this->_build(b);
        });

        LOG_INFO("init ok!");

        this->addLoop();
        _is_initialized = true;
    }

void Settings::exec() {
    if (!_is_initialized) {
        LOG_ERROR("call setup first!");
        return;
    }

    _sett.tick();
}

void Settings::_update(sets::Updater& u) {
    // ...
}

void Settings::_build(sets::Builder& b) {
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
            b.Input(kk::mqtt_device_id, MQTT_DEFAULT_DEVICE_ID);
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
            b.Select(kk::log_level, "Уровень логирования", log_levels);
            b.Button(SH("common_save"), "Сохранить");
        }
    }

    if (b.build.isAction()) {
        switch (b.build.id) {
            case SH("wifi_save"):
                LOG_DEBUG("wifi_save pressed");

                if (_db && _db->update() && _wifi_conn) {
                    _wifi_conn->connect();
                    return;
                }

                LOG_ERROR("db update failed for wifi settings!");
                break;
                
            case SH("mqtt_save"):
                LOG_DEBUG("mqtt_save pressed");
                
                if (_db && _db->update() && _mqtt_conn) {
                    _mqtt_conn->setDeviceID((*_db)[kk::mqtt_device_id].toString());
                    _mqtt_conn->connect();
                    return;
                }

                LOG_ERROR("db update failed for mqtt settings!");
                break;
            
            case SH("co2_save"):
                LOG_DEBUG("co2_save pressed");

                if (_db && _db->update()) {
                    return;
                }

                LOG_ERROR("db update failed for co2 settings!");
                break;
                
            case SH("common_save"):
                LOG_DEBUG("common_save pressed");
                
                if (_db && _db->update()) {
                    SET_LOG_LEVEL((*_db)[kk::log_level].toString());

                    if (_rgb_controller && _hmi) {
                        _rgb_controller->toggle((*_db)[kk::rgb_enabled].toBool());
                        _hmi->setTheme((*_db)[kk::use_dark_theme].toBool());
                    }
                
                    return;
                }

                LOG_ERROR("db update failed for common settings!");
                break;
        }
    }
}
