#define LOG_COMPONENT "RGBController"
#include "rgb.h"

RGBController::RGBController(uint32_t ms, SettingsDB& settingsDb)
    : ControllerBase(ms),
      _pin(RGB_PIN),
      _num_leds(RGB_NUMPIXELS),
      _leds(nullptr),
      _db(&settingsDb.getDB()),
      _co2_scale(&CO2Scale::getInstance()),
      _is_initialized(false) {}

RGBController::~RGBController() {
    if (_leds != nullptr) {
        delete _leds;
        _leds = nullptr;
    }
}

void RGBController::setup() {
    LOG_INFO("init...");

    if (_leds != nullptr) {
        delete _leds;
    }

    _leds = new Adafruit_NeoPixel(_num_leds, _pin, NEO_GRB + NEO_KHZ800);
    _leds->begin();
    _leds->setBrightness(100);
    clear();

    _co2_scale->init(_db);
    _default_period = this->getPeriod();
    _enabled = (*_db)[kk::rgb_enabled].toBool();

    _is_initialized = true;

    LOG_INFO("init ok!");
}

void RGBController::setUpdaterCb(UpdaterCallback cb) {
    _u_cb = cb;
}

void RGBController::exec() {
    _curr_period = this->getPeriod();

    if (!_is_initialized) {
        LOG_ERROR("call setup() first!");
        return;
    }

    if (_u_cb) {
        uint16_t co2_value = _u_cb();

        if (_co2_scale->needAlarm(co2_value)) {
            if (_curr_period != SEC_1) {
                this->updateInterval(SEC_1);
            }

            if (_blink) {
                _blink = false;
                clear();
                return;
            }
        } else {
            if (_curr_period != _default_period) {
                this->updateInterval(_default_period);
            }
        }

        _blink = true;
        renderLevel(co2_value, _co2_scale->getMin(), _co2_scale->getMax());
    }
}

void RGBController::toggle(bool value) {
    _enabled = value;
    clear();

    if (value) {
        LOG_DEBUG("enabled");
        return;
    }

    LOG_DEBUG("disabled");
}

void RGBController::renderLevel(float value, float min, float max) {
    if (!_enabled || _num_leds <= 0 || !_is_initialized || _leds == nullptr) return;

    uint8_t r, g, b;
    _co2_scale->getColor(value, r, g, b);

    for (int i = 0; i < _num_leds; i++) {
        _leds->setPixelColor(i, r, g, b);
    }

    _leds->show();
}

void RGBController::clear() {
    if (_leds != nullptr) {
        LOG_DEBUG("cleared");
        
        _leds->clear();
        _leds->show();
    }
}

void RGBController::copyState(const ControllerBase& other) {
    const RGBController& rgb_other = static_cast<const RGBController&>(other);
    _pin = rgb_other._pin;
    _num_leds = rgb_other._num_leds;
    _is_initialized = rgb_other._is_initialized;
    _enabled = rgb_other._enabled;
}

const char* RGBController::getType() const {
    return "rgb";
}
