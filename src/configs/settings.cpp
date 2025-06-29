#define LOG_COMPONENT "Settings"
#include "services/logger.h"
#include "settings.h"
#include "services/publisher.h"
#include "configs/config.h"

Settings::Settings(
    SettingsDB& settingsDb, 
    WiFiConn& wifiConn
) 
    : LoopTickerBase(), 
    _db(&settingsDb.db()), 
    _wifi_conn(&wifiConn), 
    _is_initialized(false) {
        _init();
    }

Settings::Settings(
    SettingsDB& settingsDb, 
    WiFiConn& wifiConn, 
    MQTTConn& mqttConn, 
    RGBController& rgbController,
    HMI& hmi
) 
    : LoopTickerBase(), 
    _db(&settingsDb.db()), 
    _wifi_conn(&wifiConn), 
    _mqtt_conn(&mqttConn), 
    _rgb_controller(&rgbController), 
    _hmi(&hmi),
    _is_initialized(false) {
        _init();
    }

void Settings::exec() {
    if (!_is_initialized) {
        LOG_ERROR("call setup first!");
        return;
    }

    _sett.tick();
}

void Settings::_init() {
    LOG_INFO("init...");

    _sett = SettingsGyver(String(APP_NAME) + " v" + String(APP_VERSION), _db);
    _sett.config.requestTout = SEC_10;
    _sett.config.pingTout = SEC_30;
    _sett.config.updateTout = 0;
    _sett.config.theme = sets::Colors::Green;
    _sett.begin(false);

    _sett.onUpdate([this](sets::Updater& u) {
        this->_update(u);
    });

    _sett.onBuild([this](sets::Builder& b) {
        this->_build(b);
    });

    _sett.onFocusChange([this]() {
        LOG_INFO("browser connected!");
    });

    LOG_INFO("init ok!");

    this->addLoop();
    _is_initialized = true;
}

void Settings::_update(sets::Updater& u) {}

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
            b.Input(kk::mqtt_server, "server");
            b.Number(kk::mqtt_port, "port");
            b.Input(kk::mqtt_username, "user");
            b.Pass(kk::mqtt_pass, "pass");
            b.Input(kk::mqtt_device_id, "device id");
            b.Button(SH("mqtt_save"), "Подключить");
        }
        {
            sets::Menu m(b, "Датчик CO2");
            b.Number(kk::co2_alarm_lvl, "Значение тревоги", nullptr, 0, 8000);
            b.Select(kk::co2_scale_type, "Тип шкалы CO2", co2_scale_types);
            b.Button(SH("co2_save"), "Сохранить");
        }
        {
            sets::Menu m(b, "Системные");
            b.Switch(kk::rgb_enabled, "rgb");
            b.Switch(kk::use_dark_theme, "dark theme");
            b.Select(kk::log_lvl, "Уровень логирования", log_levels);
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

                break;
                
            case SH("mqtt_save"):
                LOG_DEBUG("mqtt_save pressed");
                
                if (_db && _db->update() && _mqtt_conn) {
                    _mqtt_conn->setDeviceID((*_db)[kk::mqtt_device_id].toString());
                    _mqtt_conn->connect();
                    return;
                }

                break;
            
            case SH("co2_save"):
                LOG_DEBUG("co2_save pressed");

                if (_db && _db->update()) {
                    return;
                }

                break;
                
            case SH("common_save"):
                LOG_DEBUG("common_save pressed");
                
                if (_db && _db->update()) {
                    SET_LOG_LEVEL((*_db)[kk::log_lvl].toString());

                    if (_rgb_controller) {
                        _rgb_controller->toggle((*_db)[kk::rgb_enabled].toBool());
                    }

                    if (_hmi) {
                        _hmi->setTheme((*_db)[kk::use_dark_theme].toBool());
                    }
                
                    return;
                }

                break;
        }
    }
}
