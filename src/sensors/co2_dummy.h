#pragma once
#include <Arduino.h>

#include "co2_base.h"
#include "sensor_base.h"
#include "configs/config.h"

#define LOG_COMPONENT "CO2Dummy"
#include "services/logger.h"

class Dummy_CO2Sensor : public CO2SensorBase, public SensorBase
{
public:
    Dummy_CO2Sensor();

    float getCO2Min();
    float getCO2Max();
    float getCO2();

    float getTVOCMin();
    float getTVOCMax();
    float getTVOC();

    float getAQI();

    bool begin() override;
    void exec() override;
    const char *getType() const override;

    /**
     * @name startCalibration
     * @details Force start sensor calibration
     */
    void startCalibration()
    {
        LOG_DEBUG("force start calibration");
    };

    /**
     * @name forceStopCalibration
     * @details Force stop sensor calibration and write baseline
     */
    void forceStopCalibration()
    {
        if (!isCalibrating())
        {
            return;
        }

        LOG_DEBUG("force stop calibration");

        _state = Sensor_RUNNING;
    };

private:
    CO2Data _data;
    
    bool _init();
    void _check_data();
    void _mock_data();
    void _print_data();
};