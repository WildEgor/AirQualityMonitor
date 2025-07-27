#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <GyverBME280.h>

#include "sensor_base.h"
#include "connections/mqtt_conn.h"
#include "model/tph_data.h"
#include "db/settings_db.h"
#include "configs/config.h"

#define LOG_COMPONENT "TPHSensor"
#include "services/logger.h"

class TPHSensor : public SensorBase
{
public:
    TPHSensor(SettingsDB &settingsDb, uint32_t ms);

    bool begin() override;
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
    const char *getType() const override;

private:
    GyverDBFile *_db;
    GyverBME280 _sensor;
    TPHData _data;

    bool _init();
    void _check_data();
    void _print_data();
};