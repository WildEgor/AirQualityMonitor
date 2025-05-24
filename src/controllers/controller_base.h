#pragma once
#include <Arduino.h>
#include <Looper.h>

class ControllerBase : public LoopTimerBase {
public:
    ControllerBase(uint32_t ms) : LoopTimerBase(ms), _is_initialized(false) {}

    virtual void setup() = 0;
    virtual void exec() override = 0;
    virtual bool getIsInitialized() const { return _is_initialized; }
    virtual void copyState(const ControllerBase& other) = 0;

    virtual void updateInterval(uint32_t new_ms) {
        restart(new_ms);
    }

    void setInterval(uint32_t new_ms) {
        restart(new_ms);
    }

    virtual const char* getType() const = 0;

protected:
    bool _is_initialized;
};