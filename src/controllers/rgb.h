#pragma once
#include <Adafruit_NeoPixel.h>
#include "controller_base.h"
#include "configs/config.h"

class RGBLine : public ControllerBase {
public:
    using UpdaterCallback = std::function<uint16_t()>;

    RGBLine(uint32_t ms) 
        : ControllerBase(ms), _pin(RGB_PIN), _num_leds(RGB_NUMPIXELS), _leds(nullptr) {
        _is_initialized = false;
    }
    
    void setup() override {
        if (_leds != nullptr) {
            delete _leds;
        }
        
        _leds = new Adafruit_NeoPixel(_num_leds, _pin, NEO_GRB + NEO_KHZ800);
        _leds->begin();
        _leds->clear();

        _is_initialized = true;
    }

    void setUpdaterCb(UpdaterCallback cb) {
        _u_cb = cb;
    }

    void exec() override {
        if (!_is_initialized) {
            setup();
        }

        if (_u_cb) {
            uint16_t co2_value = _u_cb();
            renderLevel(co2_value, 400.0f, 8000.0f);
        }
    }

    void renderLevel(float value, float min, float max) {
        if (_num_leds <= 0 || !_is_initialized || _leds == nullptr) return;
        
        float normalized = (value - min) / (max - min);
        normalized = constrain(normalized, 0.0f, 1.0f);
        
        uint8_t r, g, b;
        
        if (normalized < 0.5f) {
            r = 255 * (normalized * 2);
            g = 255;
            b = 0;
        } else {
            r = 255;
            g = 255 * (2 - normalized * 2);
            b = 0;
        }
        
        for (int i = 0; i < _num_leds; i++) {
            _leds->setPixelColor(i, r, g, b);
        }
        
        _leds->show();
    }

    void clear() {
        if (_leds != nullptr) {
            _leds->clear();
            _leds->show();
        }
    }

    void copyState(const ControllerBase& other) override {
        const RGBLine& rgb_other = static_cast<const RGBLine&>(other);
        _pin = rgb_other._pin;
        _num_leds = rgb_other._num_leds;
        _is_initialized = rgb_other._is_initialized;
    }

    const char* getType() const override {
        return "rgb";
    }

    ~RGBLine() {
        if (_leds != nullptr) {
            delete _leds;
            _leds = nullptr;
        }
    }

private:
    uint8_t _pin;
    uint8_t _num_leds;
    Adafruit_NeoPixel* _leds;
    UpdaterCallback _u_cb;
};