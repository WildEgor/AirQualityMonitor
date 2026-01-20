#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <SparkFun_ENS160.h>

#include "co2_base.h"
#include "sensor_base.h"
#include "configs/config.h"

#define LOG_COMPONENT "ENS160"
#include "services/logger.h"

class ENS160_CO2Sensor : public CO2SensorBase, public SensorBase
{
public:
    ENS160_CO2Sensor(uint32_t ms);

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
    SparkFun_ENS160 _sensor = SparkFun_ENS160();

    /**
     * @name _init
     * @details Internal sensor initialization
     * @return bool - initialization success
     */
    bool _init();
    /**
     * @name _check_data
     * @details Check and update sensor data
     */
    void _check_data();
    /**
     * @name _print_data
     * @details Print current data to log
     */
    void _print_data();
    /**
     * @name _mock_data
     * @details Mock data for scale and alarm testing
     */
    void _mock_data();
};