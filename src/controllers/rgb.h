#pragma once
#include <Adafruit_NeoPixel.h>

#include "controller_base.h"
#include "configs/config.h"
#include "sensors/co2.h"

#define LOG_COMPONENT "RGBController"
#include "services/logger.h"

/**
 * @name RGBController
 * @details Class for controlling RGB LED strip based on CO2 levels
 */
class RGBController : public ControllerBase
{
public:
    /**
     * @name UpdaterCallback
     * @details Callback type for updating CO2 value
     * @return uint16_t - current CO2 value
     */
    using UpdaterCallback = std::function<uint16_t()>;

    /**
     * @name RGBController
     * @param ms - update interval in milliseconds
     * @param settingsDb - reference to settings database
     * @details Constructor for RGBController
     */
    RGBController(uint32_t ms, SettingsDB &settingsDb);
    /**
     * @name ~RGBController
     * @details Destructor for RGBController
     */
    ~RGBController();

    /**
     * @name exec
     * @details Main update loop for RGB controller
     */
    void exec() override;
    /**
     * @name toggle
     * @param value - enable or disable RGB
     * @details Enable or disable RGB output
     */
    void toggle(bool value) override;

    /**
     * @name setUpdaterCb
     * @param cb - callback to provide CO2 value
     * @details Set callback for updating CO2 value
     */
    void setUpdaterCb(UpdaterCallback cb);
    /**
     * @name renderLevel
     * @param value - CO2 value
     * @details Render color based on CO2 value
     */
    void renderLevel(float value);
    /**
     * @name clear
     * @details Clear all LEDs (turn off)
     */
    void clear();
    /**
     * @name getType
     * @return const char* - controller type
     * @details Return controller type string
     */
    const char *getType() const override;

private:
    /**
     * @name _leds
     * @details Pointer to Adafruit_NeoPixel instance
     */
    Adafruit_NeoPixel *_leds;
    /**
     * @name _db
     * @details Pointer to settings database file
     */
    GyverDBFile *_db;
    /**
     * @name _u_cb
     * @details Callback for updating CO2 value
     */
    UpdaterCallback _u_cb;
    /**
     * @name _co2_scale
     * @details Pointer to CO2 scale instance for color mapping
     */
    CO2Scale *_co2_scale;
    /**
     * @name _blink
     * @details State for alarm blinking
     */
    bool _blink;
    /**
     * @name _pin
     * @details Pin number for RGB strip
     */
    uint8_t _pin;
    /**
     * @name _num_leds
     * @details Number of LEDs in the strip
     */
    uint8_t _num_leds;
    /**
     * @name _default_period
     * @details Default update period
     */
    uint16_t _default_period;
    /**
     * @name _curr_period
     * @details Current update period
     */
    uint16_t _curr_period;

    /**
     * @name _renderAlarm
     * @param value - CO2 value
     * @details Render alarm state (blinking red) if CO2 is above threshold
     */
    void _renderAlarm(float value);
};