#pragma once
#include <Arduino.h>
#include <Looper.h>

class ControllerBase : public LoopTimerBase {
public:
    ControllerBase(uint32_t ms) : LoopTimerBase(ms), _is_initialized(false), _enabled(true) {}

    virtual void exec() override = 0;
    bool IsInitialized() { return _is_initialized; }

    virtual void updateInterval(uint32_t new_ms) {
        restart(new_ms);
    }

    void setInterval(uint32_t new_ms) {
        restart(new_ms);
    }

    virtual void toggle(bool value) {
        _enabled = value;
    }

    virtual const char* getType() const = 0;

protected:
    bool _is_initialized;
    bool _enabled;
};