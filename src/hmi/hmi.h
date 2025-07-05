#pragma once
#include <Looper.h>
#include <TFT_eSPI.h>

#include "model/hmi_data.h"
#include "widgets/meter.h"
#include "configs/config.h"
#include "sensors/co2.h"
#include "sensors/tp.h"
#include "services/ota.h"

#define LOG_COMPONENT "HMI"
#include "services/logger.h"

class HMI: public LoopTimerBase {
public:
    HMI(uint32_t ms, SettingsDB& settingsDb, CO2Sensor& co2_sensor, TPSensor& tp_sensor, WiFiConn& wifiConn, OTA& ota)
        : LoopTimerBase(ms), 
        _db(&settingsDb.db()), 
        _co2_sensor(co2_sensor),
        _tp_sensor(tp_sensor), 
        _co2_meter(nullptr), 
        _co2_scale(&CO2Scale::getInstance()), 
        _wifi(&wifiConn), 
        _ota(&ota) {
        _force_redraw = true;
        _intro_shown = true;
        _state.dark_theme = false;
        _state.last_co2_value = -1;
        _state.last_wifi_state = false;
        _state.last_co2_sensor_state = false;
        _state.last_render_time = 0;
        _state.last_fw_ver = _ota->version();

        LOG_INFO("init tft...");

        _tft.init();
        _tft.setRotation(0);
        _init_theme(true);
        LOG_INFO("init tft ok!");

        LOG_INFO("init widgets...");
        _co2_meter = MeterWidget(&_tft);
        uint16_t rs,re,os,oe,ys,ye,gs,ge;
        _co2_scale->init(_db);
        _co2_scale->getScale(rs,re,os,oe,ys,ye,gs,ge);
        _co2_meter.setTheme(_state.dark_theme);
        _co2_meter.setZones(rs,re,os,oe,ys,ye,gs,ge);
        _co2_meter.analogMeter(0, 0, _co2_scale->getHumanMax(), "co2", "", "", "", "", ""); 
        LOG_INFO("init widgets ok!");

        _render();
        this->addLoop();
    }

    void exec() {
        _render();
    }

    void setTheme(bool dark) {
        if (_state.dark_theme != dark) {
            _state.dark_theme = dark;
            _init_theme(true);
            _co2_meter.setTheme(dark);
            _co2_meter.analogMeter(0, 0, _co2_scale->getHumanMax(), "co2", "", "", "", "", "");
            _force_redraw = true; 
            _render();
        }
    }

private:
    TFT_eSPI _tft = TFT_eSPI();
    GyverDBFile* _db;
    MeterWidget _co2_meter;
    CO2Sensor& _co2_sensor;
    CO2Scale* _co2_scale;
    TPSensor& _tp_sensor;
    WiFiConn* _wifi;
    OTA* _ota;

    HMIState _state;
    bool _intro_shown = false;
    bool _force_redraw = false;

    void _render() {
        if (!_should_render()) {
            return;
        }

        LOG_DEBUG("render started...");
        _print_wifi_info();
        _print_gauge();
        _print_sensor_state();
        LOG_DEBUG("rendered ok!");
        
        _force_redraw = false;
    }

    bool _should_render() {
        if (_force_redraw) {
            return true;
        }

        bool current_wifi_state = _wifi->connected();
        if (current_wifi_state != _state.last_wifi_state) {
            _state.last_wifi_state = current_wifi_state;
            return true;
        }

        bool current_co2_sensor_state = _co2_sensor.isCalibrating();
        if (current_co2_sensor_state != _state.last_co2_sensor_state) {
            _state.last_co2_sensor_state = current_co2_sensor_state;
            _force_redraw = true;
            return true;
        }

        float current_co2 = _co2_sensor.getCO2();
        if (abs(current_co2 - _state.last_co2_value) > 5.0) {
            _state.last_co2_value = current_co2;
            return true;
        }

        String current_fw_ver = _ota->version();
        if (current_fw_ver != _state.last_fw_ver) {
            _state.last_fw_ver = current_fw_ver;
            return true;
        }

        if ((millis() - _state.last_render_time) > SEC_5) {
            _state.last_render_time = millis();
            _force_redraw = true;
            return true;
        }

        return false;
    }

    void _print_wifi_info() {
        if (_intro_shown || _force_redraw) {
            _intro_shown = false;

            if (_state.dark_theme) {
                _tft.setTextColor(TFT_LIGHTGREY);
                _tft.fillRect(20, 130, 200, 10, TFT_BLACK);
            } else {
                _tft.setTextColor(TFT_LIGHTGREY);
                _tft.fillRect(20, 130, 200, 10, TFT_WHITE);
            }

            LOG_DEBUG("admin panel: http://" + _wifi->ip());

            _tft.setCursor(20, 130);
            _tft.print(F("admin panel: http://"));
            _tft.println(_wifi->ip());

            if (_state.dark_theme) {
                _tft.fillRect(110, 145, 60, 10, TFT_BLACK);
            } else {
                _tft.fillRect(110, 145, 60, 10, TFT_WHITE);
            }
            
            _tft.setCursor(110, 145);
            if (!_state.last_wifi_state) {
                _tft.setTextColor(TFT_RED);
                _tft.println(F("WIFI"));
                LOG_ERROR("wifi not connected");
            } else {
                _tft.setTextColor(TFT_DARKGREEN);
                _tft.println(F("WIFI"));
            }

            _tft.setCursor(100, 185);
            if (_state.dark_theme) {
                _tft.fillRect(100, 185, 60, 10, TFT_BLACK);
            } else {
                _tft.fillRect(100, 185, 60, 10, TFT_WHITE);
            }

            _tft.setTextColor(TFT_LIGHTGREY);
            _tft.setCursor(100, 185);
            _tft.print(F("v "));
            _tft.println(_state.last_fw_ver);
        }
    }

    void _print_gauge() {
        if (!_co2_sensor.isInitialized()) {
            return;
        }
    
        float value = static_cast<float>(_co2_sensor.getCO2());
        if (value > _co2_scale->getHumanMax()){
            value = _co2_scale->getHumanMax();
        }

        _init_theme(false);
        _tft.setCursor(0, 0);

        LOG_DEBUG("update gauge value: " + String(value));
        _co2_meter.updateNeedle(value, 20);
    }

    void _print_sensor_state() {
        if (_force_redraw) {
            _tft.setCursor(80, 165);

            _tft.setTextColor(TFT_CYAN);
            if (_state.dark_theme) {
                _tft.fillRect(80, 165, 80, 10, TFT_BLACK);
            } else {
                _tft.fillRect(80, 165, 80, 10, TFT_WHITE);
            }

            if (_state.last_co2_sensor_state) {
                _tft.println(F("CALIBRATION"));
            } else {
                if (_state.dark_theme) {
                    _tft.fillRect(80, 165, 80, 10, TFT_BLACK);
                } else {
                    _tft.fillRect(80, 165, 80, 10, TFT_WHITE);
                }
            }
        }
    }

    void _init_theme(bool fill) {
        _state.dark_theme = (*_db)[kk::use_dark_theme].toBool();

        _tft.setTextSize(1);

        if (_state.dark_theme) {
            if (fill) {
                _tft.fillScreen(TFT_BLACK);
            }
            _tft.setTextColor(TFT_WHITE);
            return;
        }

        if (fill) {
            _tft.fillScreen(TFT_WHITE);
        }
        _tft.setTextColor(TFT_BLACK);
    }
};
