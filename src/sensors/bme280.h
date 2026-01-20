#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <GyverBME280.h>

#include "tph_base.h"
#include "sensor_base.h"
#include "model/tph_data.h"
#include "db/settings_db.h"
#include "configs/config.h"

#define LOG_COMPONENT "BME280"
#include "services/logger.h"

class BME280_TPHSensor : public TPHSensorBase, public SensorBase
{
public:
    BME280_TPHSensor(uint32_t ms, SettingsDB &settingsDb);

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
    float getTemperatureMin();
    float getTemperatureMax();
    float getTemperature();
    float getPressureMin();
    float getPressureMax();
    float getPressure();
    float getHumidityMin();
    float getHumidityMax();
    float getHumidity();
    /**
     * @name getType
     * @return const char* - sensor type
     */
    const char *getType() const override;

private:
    GyverDBFile *_db;
    GyverBME280 _sensor;
    TPHData _data;

    bool _init();
    void _check_data();
    void _print_data();
};