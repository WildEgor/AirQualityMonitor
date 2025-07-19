#pragma once
#include <Arduino.h>
#include <Looper.h>

/**
 * @class ControllerBase
 * @brief Abstract base class for all controllers with timer-based execution
 */
class ControllerBase : public LoopTimerBase {
public:
    /**
     * @brief Constructor
     * @param ms Timer interval in milliseconds
     */
    ControllerBase(uint32_t ms) : LoopTimerBase(ms), _is_initialized(false), _enabled(true) {}

    /**
     * @brief Main execution loop (must be implemented by derived classes)
     */
    virtual void exec() override = 0;
    /**
     * @brief Check if controller is initialized
     * @return true if initialized
     */
    bool IsInitialized() { return _is_initialized; }

    /**
     * @brief Update timer interval (virtual)
     * @param new_ms New interval in milliseconds
     */
    virtual void updateInterval(uint32_t new_ms) {
        restart(new_ms);
    }

    /**
     * @brief Set timer interval
     * @param new_ms New interval in milliseconds
     */
    void setInterval(uint32_t new_ms) {
        restart(new_ms);
    }

    /**
     * @brief Enable or disable the controller
     * @param value true to enable, false to disable
     */
    virtual void toggle(bool value) {
        _enabled = value;
    }

    /**
     * @brief Get controller type as string
     * @return const char* type string
     */
    virtual const char* getType() const = 0;

protected:
    bool _is_initialized; ///< Initialization flag
    bool _enabled;        ///< Enable/disable flag
};