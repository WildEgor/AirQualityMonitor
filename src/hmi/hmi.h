#pragma once
#include <Looper.h>
#include <TFT_eSPI.h>

#include "widgets/meter.h"
#include "configs/config.h"
#include "sensors/co2.h"
#include "sensors/tp.h"

#define LOG_COMPONENT "HMI"
#include "services/logger.h"

class HMI: public LoopTimerBase {
public:
    HMI(uint32_t ms, SettingsDB& settingsDb, CO2Sensor& co2_sensor, TPSensor& tp_sensor, WiFiConn& wifiConn)
        : LoopTimerBase(ms), 
        _db(&settingsDb.getDB()), 
        _co2_sensor(co2_sensor),
        _tp_sensor(tp_sensor), 
        _co2_meter(nullptr), 
        _co2_scale(&CO2Scale::getInstance()), 
        _wifi(&wifiConn), 
        _show_intro(true),
        _dark_theme(false) {
        LOG_INFO("init tft...");
        _dark_theme = (*_db)[kk::use_dark_theme].toBool();

        _tft.init();
        _tft.setRotation(0);
        _init_theme(true);
        
        LOG_INFO("init tft ok!");

        LOG_INFO("init widgets...");
        _co2_meter = MeterWidget(&_tft);
        _co2_meter.setTheme(_dark_theme);

        uint16_t rs,re,os,oe,ys,ye,gs,ge;
        _co2_scale->init(_db);
        _co2_scale->getScale(rs,re,os,oe,ys,ye,gs,ge);
        _co2_meter.setZones(rs,re,os,oe,ys,ye,gs,ge);
        // TODO: refactor
        _co2_meter.analogMeter(0, 0, _co2_scale->getHumanMax(), "pm", "", "", "", "", ""); 
        LOG_INFO("init widgets ok!");
        this->addLoop();
    }

    void exec() {
        _render();
    }

    void setTheme(bool dark) {
        _dark_theme = dark;
        _co2_meter.setTheme(dark);
        _render();
    }

private:
    TFT_eSPI _tft = TFT_eSPI();
    GyverDBFile* _db;
    MeterWidget _co2_meter;
    CO2Sensor& _co2_sensor;
    CO2Scale* _co2_scale;
    TPSensor& _tp_sensor;
    WiFiConn* _wifi;

    bool _show_intro = true;
    bool _dark_theme = false;

    void _render() {
        _show_intro = true;
        _print_gauge();
        _print_ip();
    }

    void _print_ip() {
        if (!_wifi->connected()) {
            _tft.setCursor(35, 135);
            _init_theme(false);
            _tft.setTextColor(TFT_RED);
            _tft.println("WIFI NOT CONNECTED!");
            LOG_ERROR("wifi not connected;");
            delay(500);
            _show_intro = true;
            return;
        }

        if (!_show_intro) {
            return;
        }
        
        _tft.setCursor(20, 130);
        _init_theme(false);
        String adminURL = "Admin panel: http://" + _wifi->ip();
        _tft.println(adminURL);
        LOG_ERROR(adminURL);
        delay(3000);

        _show_intro = false;
    }

    void _print_gauge() {
        if (!_co2_sensor.isInitialized()) {
            LOG_WARN("co2 sensor not initialized");
            delay(500);
            return;
        }

        if (!_co2_sensor.hasChanges()) {
            return;
        }
    
        float value = static_cast<float>(_co2_sensor.getCO2());
        if (value > _co2_scale->getHumanMax()){
            value = _co2_scale->getHumanMax();
        }

        _tft.setCursor(0, 0);
        _init_theme(false);

        _co2_meter.updateNeedle(value, 50);
    }

    void _init_theme(bool fill) {
        _tft.setTextSize(1);

        if (_dark_theme) {
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
