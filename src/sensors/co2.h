#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <SparkFunCCS811.h>

#include "sensor_base.h"
#include "configs/config.h"
#include "connections/mqtt_conn.h"
#include "model/co2_data.h"
#include "db/settings_db.h"

#define LOG_COMPONENT "CO2Sensor"
#include "services/logger.h"

/**
 * @name CO2Sensor
 * @details Class for working with CCS811 sensor: reading data, calibration, state management
 */
class CO2Sensor : public SensorBase
{
public:
    /**
     * @name CO2Sensor
     * @param ms - polling period in milliseconds
     * @details CO2Sensor class constructor
     */
    CO2Sensor(uint32_t ms);

    /**
     * @name setup
     * @details Initialize CCS811 dependencies
     */
    void setup() override;
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
        return _state == CO2Sensor_CALIBRATING;
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

        _state = CO2Sensor_CALIBRATING;

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
        if (_state != CO2Sensor_CALIBRATING || _data.current_baseline == 0x01)
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

        _state = CO2Sensor_RUNNING;
    };

private:
    /**
     * @name _sensor
     * @details CCS811 driver instance
     */
    CCS811 _sensor;
    /**
     * @name _data
     * @details Structure for storing current sensor data
     */
    CO2Data _data;
    /**
     * @name _state
     * @details Current sensor state (initialization, running, calibration)
     */
    CO2Sensor_State _state;

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

/**
 * @name CO2Scale
 * @details Class for working with CO2 color scale and thresholds
 */
class CO2Scale
{
public:
    /**
     * @name getInstance
     * @return CO2Scale& - singleton instance of the class
     */
    static CO2Scale &getInstance();

    /**
     * @name init
     * @param db - pointer to database object
     * @details Initialize scale from database
     */
    void init(GyverDBFile *db);
    /**
     * @name getColor
     * @param value - CO2 value
     * @param r,g,b - color components
     * @details Get color by CO2 value
     */
    void getColor(uint16_t value, uint8_t &r, uint8_t &g, uint8_t &b);
    /**
     * @name getScale
     * @details Get color zone boundaries of the scale
     */
    void getScale(uint16_t &rs, uint16_t &re, uint16_t &os, uint16_t &oe, uint16_t &ys, uint16_t &ye, uint16_t &gs, uint16_t &ge);

    /**
     * @name getMin
     * @return float - minimum scale value
     */
    float getMin();
    /**
     * @name getMax
     * @return float - maximum scale value
     */
    float getMax();
    /**
     * @name getHumanMax
     * @return float - maximum value for human comfort
     */
    float getHumanMax();
    /**
     * @name needAlarm
     * @param value - CO2 value
     * @return bool - whether alarm is needed
     */
    bool needAlarm(uint16_t value);

private:
    /**
     * @name CO2Scale
     * @details Private constructor for singleton
     */
    CO2Scale();
    /**
     * @name _initScales
     * @details Internal initialization of color scales
     */
    void _initScales();

    GyverDBFile *_db;
    ColorThreshold _default_scale[5];
    ColorThreshold _easy_scale[3];
    float _min = 400.0f;
    float _max = 8000.0f;
    float _human_max = 1500.0f;
};
