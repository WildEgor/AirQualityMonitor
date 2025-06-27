#pragma once
#include <Arduino.h>
#include <Looper.h>

class SensorBase : public LoopTimerBase {
public:
    SensorBase(uint32_t ms) : LoopTimerBase(ms), _enable_test(false), _is_initialized(false) {}

    virtual void setup() = 0;
    virtual void exec() override = 0;
    bool isInitialized() { return _is_initialized; }
    void enableTest() { _enable_test = true; }

    virtual void updateInterval(uint32_t new_ms) {
        restart(new_ms);
    }

    void setInterval(uint32_t new_ms) {
        restart(new_ms);
    }
    virtual const char* getType() const = 0;

protected:
    bool _is_initialized;
    bool _enable_test;
};