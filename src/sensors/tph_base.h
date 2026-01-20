#pragma once
#include <Arduino.h>

#include "model/sensor_state.h"
#include "model/tph_data.h"

#define LOG_COMPONENT "TPHSensor"
#include "services/logger.h"

class TPHSensorBase
{
public:
    TPHSensorBase() {};

    virtual bool begin() = 0;

    virtual float getTemperatureMin() = 0;
    virtual float getTemperatureMax() = 0;
    virtual float getTemperature() = 0;
    virtual float getPressureMin() = 0;
    virtual float getPressureMax() = 0;
    virtual float getPressure() = 0;
    virtual float getHumidityMin() = 0;
    virtual float getHumidityMax() = 0;
    virtual float getHumidity() = 0;

private:
    /**
     * @name _state
     * @details Structure for storing sensor state
     */
    Sensor_State _state;
    TPHData _data;
};