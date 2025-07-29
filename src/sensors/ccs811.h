#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <SparkFunCCS811.h>

#include "sensor_base.h"
#include "co2_base.h"
#include "configs/config.h"
#include "model/sensor_state.h"
#include "model/sensor_state.h"
#include "model/co2_data.h"

#define LOG_COMPONENT "CCS811"
#include "services/logger.h"

/**
 * @name CCS811_CO2Sensor
 * @details Class for working with CCS811 sensor: reading data, calibration, state management
 */
class CCS811_CO2Sensor : public CO2SensorBase, public SensorBase
{
public:
    /**
     * @name CCS811_CO2Sensor
     * @param ms - polling period in milliseconds
     * @details CCS811_CO2Sensor class constructor
     */
    CCS811_CO2Sensor(uint32_t ms);

    /**
     * @name setup
     * @details Initialize CCS811 dependencies
     */
    bool begin() override;
    /**
     * @name exec
     * @details Main polling and data processing loop for the sensor
     */
    void exec() override;
    /**
     * @name getCO2Min
     * @return float - minimum CO2 value
     */
    float getCO2Min();
    /**
     * @name getCO2Max
     * @return float - maximum CO2 value
     */
    float getCO2Max();
    /**
     * @name getCO2
     * @return float - current CO2 value
     */
    float getCO2();
    /**
     * @name getTVOCMin
     * @return float - minimum TVOC value
     */
    float getTVOCMin();
    /**
     * @name getTVOCMax
     * @return float - maximum TVOC value
     */
    float getTVOCMax();
    /**
     * @name getTVOC
     * @return float - current TVOC value
     */
    float getTVOC();
    /**
     * @name getType
     * @return const char* - sensor type
     */
    const char *getType() const override;

    /**
     * @name isCalibrating
     * @return bool - whether the sensor is in calibration mode
     */
    bool isCalibrating()
    {
        return _state == Sensor_CALIBRATING;
    }

    /**
     * @name startCalibration
     * @details Force start sensor calibration
     */
    void startCalibration()
    {
        LOG_DEBUG("force start calibration");

        if (_data.current_baseline == 0x01)
        {
            _data.current_baseline = 0x00;
        }

        _state = Sensor_CALIBRATING;

        if (_data.current_baseline == 0x00)
        {
            _data.current_baseline = _sensor.getBaseline();
        }

        delay(5000);
    };

    /**
     * @name forceStopCalibration
     * @details Force stop sensor calibration and write baseline
     */
    void forceStopCalibration()
    {
        if (_state != Sensor_CALIBRATING || _data.current_baseline == 0x01)
        {
            return;
        }

        LOG_DEBUG("force stop calibration");

        CCS811Core::CCS811_Status_e errorStatus = _sensor.setBaseline(_data.current_baseline);
        if (errorStatus == CCS811Core::CCS811_Stat_SUCCESS)
        {
            LOG_DEBUG("baseline written to sensor");
            LOG_INFO("calibration success");
        }
        else
        {
            LOG_DEBUG("set baseline failed!");
            LOG_DEBUG(_sensor.statusString(errorStatus));
            LOG_INFO("calibration failed");
        }

        _data.current_baseline = 0x01;

        delay(5000);

        _state = Sensor_RUNNING;
    };

private:
    /**
     * @name _sensor
     * @details CCS811 driver instance
     */
    CCS811 _sensor = CCS811(CCS811_ADDR);
    /**
     * @name _data
     * @details Structure for storing current sensor data
     */
    CO2Data _data;
    /**
     * @name _state
     * @details Current sensor state (initialization, running, calibration)
     */
    Sensor_State _state;

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
