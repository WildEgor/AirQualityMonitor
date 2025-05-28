#include <Looper.h>
#include <TFT_eSPI.h>
#include <TFT_eWidget.h>
#include "gc9a01_config.h"
#include "sensors/co2.h"

class Display: public LoopTimerBase {
public:
    Display(uint32_t ms, SettingsDB& settingsDb, CO2Sensor& co2_sensor) 
        : LoopTimerBase(ms), _db(&settingsDb.getDB()), _co2_sensor(co2_sensor), _co2_meter(nullptr), _co2_scale(&CO2Scale::getInstance()) {}

    void setup() {
        _co2_scale->init(_db);
        _tft.init();
        _tft.setRotation(1);
        _tft.fillScreen(TFT_BLACK);
        _tft.setTextColor(TFT_WHITE, TFT_BLACK);
        _tft.setTextSize(2);
        _tft.setCursor(0, 0);

        uint16_t rs,re,os,oe,ys,ye,gs,ge;
        _co2_scale->getScale(rs,re,os,oe,ys,ye,gs,ge);
        
        _co2_meter = MeterWidget(&_tft);
        _co2_meter.setZones(rs,re,os,oe,ys,ye,gs,ge);
        _co2_meter.analogMeter(0, 0, 2.0, "pm", "0", "0.5", "1.0", "1.5", "2.0");    // Draw analogue meter at 0, 0
    }

    void exec() {
        if (!_co2_sensor.getIsInitialized()) {}
        float value = 0.00;
        value = static_cast<float>(_co2_sensor.getCO2());
        _co2_meter.updateNeedle(value, 0);
        Serial.println(value);
    }

private:
    TFT_eSPI _tft = TFT_eSPI();
    GyverDBFile* _db;
    MeterWidget _co2_meter;
    CO2Sensor& _co2_sensor;
    CO2Scale* _co2_scale;
};
