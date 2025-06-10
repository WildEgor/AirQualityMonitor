#include <Looper.h>
#include <TFT_eSPI.h>
#include "widgets/meter.h"
#include "gc9a01_config.h"
#include "sensors/co2.h"

class Display: public LoopTimerBase {
public:
    Display(uint32_t ms, SettingsDB& settingsDb, CO2Sensor& co2_sensor, WiFiConn& wifiConn)
        : LoopTimerBase(ms), 
        _db(&settingsDb.getDB()), 
        _co2_sensor(co2_sensor), 
        _co2_meter(nullptr), 
        _co2_scale(&CO2Scale::getInstance()), 
        _wifi(&wifiConn), 
        _show_intro(true),
        _black_theme(true) {}

    void setup() {
        Serial.println("init tft");
        _tft.init();
        _tft.setRotation(0);
        _init_theme(true);
        
        Serial.println("init tft ok");

        Serial.println("init widgets");
        _co2_meter = MeterWidget(&_tft);
        uint16_t rs,re,os,oe,ys,ye,gs,ge;
        _co2_scale->init(_db);
        _co2_scale->getScale(rs,re,os,oe,ys,ye,gs,ge);
        _co2_meter.setZones(rs,re,os,oe,ys,ye,gs,ge);
        // TODO: refactor
        _co2_meter.analogMeter(0, 0, _co2_scale->getHumanMax(), "pm", "", "", "", "", ""); 
        Serial.println("init widgets ok");
    }

    void exec() {
        _print_gauge();
        _print_ip();
    }

private:
    TFT_eSPI _tft = TFT_eSPI();
    GyverDBFile* _db;
    MeterWidget _co2_meter;
    CO2Sensor& _co2_sensor;
    CO2Scale* _co2_scale;
    WiFiConn* _wifi;

    bool _show_intro;
    bool _black_theme; // TODO: adapt MeterWidget

    void _print_ip() {
        if (!_wifi->isConnected()) {
            _tft.setCursor(35, 135);
            _init_theme(false);
            _tft.setTextColor(TFT_RED);
            _tft.println("WIFI NOT CONNECTED!");
            delay(500);
            _show_intro = true;
            return;
        }

        if (!_show_intro) {
            return;
        }
        
        _tft.setCursor(20, 130);
        _init_theme(false);
        _tft.println("Admin pannel: http://" + _wifi->ip());
        delay(3000);

        _show_intro = false;
    }

    void _print_gauge() {
        if (!_co2_sensor.isInitialized()) {
            Serial.println("display: co2 sensor not initialized");
            delay(500);
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

        if (!_black_theme) {
            if (fill) {
                _tft.fillScreen(TFT_WHITE);
            }
            _tft.setTextColor(TFT_BLACK);
        } else {
            if (fill) {
                _tft.fillScreen(TFT_BLACK);
            }
            _tft.setTextColor(TFT_WHITE);
        }
    }
};
