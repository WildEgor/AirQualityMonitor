#pragma once
#include <Adafruit_NeoPixel.h>
#include "controller_base.h"
#include "configs/config.h"
#include "sensors/co2.h"

class RGBController : public ControllerBase {
public:
    using UpdaterCallback = std::function<uint16_t()>;

    RGBController(uint32_t ms, SettingsDB& settingsDb) 
        : ControllerBase(ms), _pin(RGB_PIN), _num_leds(RGB_NUMPIXELS), _leds(nullptr) {
        _is_initialized = false;
        _db = &settingsDb.getDB();
        _co2_scale = &CO2Scale::getInstance();
    }
    
    void setup() override {
        if (_leds != nullptr) {
            delete _leds;
        }
        
        _leds = new Adafruit_NeoPixel(_num_leds, _pin, NEO_GRB + NEO_KHZ800);
        _leds->begin();
        _leds->clear();

        _co2_scale->init(_db);

        _is_initialized = true;
    }

    void setUpdaterCb(UpdaterCallback cb) {
        _u_cb = cb;
    }

    void exec() override {
        if (!_is_initialized) {
            setup();
            return;
        }

        if (_u_cb) {
            uint16_t co2_value = _u_cb();
            renderLevel(co2_value, _co2_scale->getMin(), _co2_scale->getMax());
        }
    }

    void renderLevel(float value, float min, float max) {
        if (_num_leds <= 0 || !_is_initialized || _leds == nullptr) return;
        
        uint8_t r, g, b;
        _co2_scale->getColor(value, r, g, b);
        
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
        const RGBController& rgb_other = static_cast<const RGBController&>(other);
        _pin = rgb_other._pin;
        _num_leds = rgb_other._num_leds;
        _is_initialized = rgb_other._is_initialized;
    }

    const char* getType() const override {
        return "rgb";
    }

    ~RGBController() {
        if (_leds != nullptr) {
            delete _leds;
            _leds = nullptr;
        }
    }

private:
    uint8_t _pin;
    uint8_t _num_leds;
    Adafruit_NeoPixel* _leds;
    GyverDBFile* _db;
    UpdaterCallback _u_cb;
    CO2Scale* _co2_scale;
};