#include <Looper.h>
#include <TFT_eSPI.h>
#include <TFT_eWidget.h>
#include "gc9a01_config.h"
#include "sensors/co2.h"

class Display: public LoopTimerBase {
public:
    Display(uint32_t ms, CO2Sensor& sensor) 
        : LoopTimerBase(ms), _sensor(sensor), _co2_meter(nullptr) {}

    void setup() {
        _tft.init();
        _tft.setRotation(1);
        _tft.fillScreen(TFT_BLACK);
        _tft.setTextColor(TFT_WHITE, TFT_BLACK);
        _tft.setTextSize(2);
        _tft.setCursor(0, 0);
        
        _co2_meter = MeterWidget(&_tft);
        // Colour zones are set as a start and end percentage of full scale (0-100)
        // If start and end of a colour zone are the same then that colour is not used
        //            --Red--  -Org-   -Yell-  -Grn-
        _co2_meter.setZones(75, 100, 50, 75, 25, 50, 0, 25); // Example here red starts at 75% and ends at 100% of full scale
        // Meter is 239 pixels wide and 126 pixels high
        _co2_meter.analogMeter(0, 0, 2.0, "pm", "0", "0.5", "1.0", "1.5", "2.0");    // Draw analogue meter at 0, 0
    }

    void exec() {
        float value = 0.00;
        if (_sensor.getIsInitialized()) {
            value = static_cast<float>(_sensor.getCO2());
        }
        _co2_meter.updateNeedle(value, 0);
        Serial.println(value);
    }

private:
    TFT_eSPI _tft = TFT_eSPI();
    MeterWidget _co2_meter;

    CO2Sensor& _sensor;
};
