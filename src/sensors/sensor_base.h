#pragma once
#include <Arduino.h>
#include <Looper.h>

#include "configs/config.h"

class SensorBase : public LoopTimerBase
{
public:
    SensorBase(uint32_t ms) : LoopTimerBase(ms), _enable_test(false), _is_initialized(false) {}

    virtual bool begin() = 0;
    virtual void exec() override = 0;

    void enableTest()
    {
        updateInterval(SEC_3);
        _enable_test = true;
    }

    virtual void updateInterval(uint32_t new_ms)
    {
        restart(new_ms);
    }

    void setInterval(uint32_t new_ms)
    {
        restart(new_ms);
    }
    virtual const char *getType() const = 0;

protected:
    bool _is_initialized;
    bool _enable_test;
};