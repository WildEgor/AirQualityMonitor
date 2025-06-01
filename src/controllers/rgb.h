#pragma once
#include <Adafruit_NeoPixel.h>
#include "controller_base.h"
#include "configs/config.h"
#include "sensors/co2.h"

class RGBController : public ControllerBase {
public:
    using UpdaterCallback = std::function<uint16_t()>;

    RGBController(uint32_t ms, SettingsDB& settingsDb);
    ~RGBController();

    void setup() override;
    void exec() override;

    void setUpdaterCb(UpdaterCallback cb);
    void renderLevel(float value, float min, float max);
    void clear();

    void copyState(const ControllerBase& other) override;
    const char* getType() const override;

private:
    uint8_t _pin;
    uint8_t _num_leds;
    Adafruit_NeoPixel* _leds;
    GyverDBFile* _db;
    UpdaterCallback _u_cb;
    CO2Scale* _co2_scale;
    bool _is_initialized;
    bool _blink;
    uint16_t _default_period;
    uint16_t _curr_period;
};