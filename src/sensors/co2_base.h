#pragma once
#include <Arduino.h>

#include "model/sensor_state.h"
#include "model/co2_data.h"

class CO2SensorBase
{
public:
    CO2SensorBase() {};

    virtual bool begin() = 0;

    virtual float getCO2Min() = 0;
    virtual float getCO2Max() = 0;
    virtual float getCO2() = 0;

    virtual float getTVOCMin() = 0;
    virtual float getTVOCMax() = 0;
    virtual float getTVOC() = 0;

    virtual void startCalibration() = 0;
    virtual void forceStopCalibration() = 0;

    /**
     * @name isCalibrating
     * @return bool - whether the sensor is in calibration mode
     */
    bool isCalibrating()
    {
        return _state == Sensor_CALIBRATING;
    }

protected:
    /**
     * @name _state
     * @details Structure for storing sensor state
     */
    Sensor_State _state;
    /**
     * @name _data
     * @details Structure for storing current sensor data
     */
    CO2Data _data;
};