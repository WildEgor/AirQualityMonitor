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
        _dark_theme(false),
        _last_co2_value(-1),
        _last_wifi_state(false),
        _intro_shown(true),
        _force_redraw(true),
        _last_render_time(0) {

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
        _co2_meter.setTheme(_dark_theme);
        _co2_meter.setZones(rs,re,os,oe,ys,ye,gs,ge);
        _co2_meter.analogMeter(0, 0, _co2_scale->getHumanMax(), "pm", "", "", "", "", ""); 
        LOG_INFO("init widgets ok!");

        _render();
        this->addLoop();
    }

    void exec() {
        _render();
    }

    void setTheme(bool dark) {
        if (_dark_theme != dark) {
            _dark_theme = dark;
            _init_theme(true);
            _co2_meter.setTheme(_dark_theme);
            _co2_meter.analogMeter(0, 0, _co2_scale->getHumanMax(), "pm", "", "", "", "", "");
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

    bool _show_intro = true;
    bool _dark_theme = false;
    float _last_co2_value = -1;
    bool _last_wifi_state = false;
    bool _intro_shown = false;
    bool _force_redraw = false;
    unsigned long _last_render_time = 0;

    void _render() {
        // Only render if there are actual changes or forced redraw
        if (!_should_render()) {
            return;
        }

        LOG_DEBUG("render started...");

        _print_gauge();
        _print_wifi_info();

        LOG_DEBUG("rendered ok!");
        
        _last_render_time = millis();
        _force_redraw = false;
    }

    bool _should_render() {
        // Force redraw if theme changed
        if (_force_redraw) {
            return true;
        }

        // Check if CO2 value changed
        float current_co2 = _co2_sensor.getCO2();

        LOG_DEBUG("new co2 value: " + String(current_co2));
        LOG_DEBUG("last co2 value: " + String(_last_co2_value));

        if (abs(current_co2 - _last_co2_value) > 5.0) {
            _last_co2_value = current_co2;
            return true;
        }

        // Check if WiFi state changed
        bool current_wifi_state = _wifi->connected();
        if (current_wifi_state != _last_wifi_state) {
            _last_wifi_state = current_wifi_state;
            return true;
        }

        // Show intro message periodically (every 30 seconds) if not shown
        if (!_intro_shown && (millis() - _last_render_time) > SEC_10) {
            return true;
        }

        return false;
    }

    void _print_wifi_info() {
        if (_intro_shown || _force_redraw) {
            _intro_shown = false;

            _tft.setCursor(50, 135);
    
            if (!_wifi->connected()) {
                _tft.setTextColor(TFT_RED);
                _tft.println("WIFI X");
                LOG_ERROR("wifi not connected");
                return;
            } else {
                _tft.setTextColor(TFT_GREEN);
                _tft.println("WIFI OK");
            }
        }

        // Show admin URL only once or when forced
        if (!_intro_shown || _force_redraw) {
            _init_theme(false);
            _tft.setTextColor(TFT_GREEN);
            _tft.setCursor(20, 130);
            String adminURL = "Admin panel: http://" + _wifi->ip();
            _tft.println(adminURL);
            LOG_ERROR(adminURL);
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

        // Only update if value actually changed significantly
        _init_theme(false);
        _tft.setCursor(0, 0);

        // Reduce animation delay for better performance
        LOG_DEBUG("update gauge value: " + String(value));
        _co2_meter.updateNeedle(value, 20);
    }

    void _init_theme(bool fill) {
        _dark_theme = (*_db)[kk::use_dark_theme].toBool();

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
