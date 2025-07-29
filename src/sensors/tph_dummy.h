#pragma once
#include <Arduino.h>

#include "tph_base.h"
#include "sensor_base.h"
#include "model/tph_data.h"
#include "configs/config.h"

#define LOG_COMPONENT "TPH_Dummy"
#include "services/logger.h"

class Dummy_TPHSensor : public TPHSensorBase, public SensorBase
{
public:
    Dummy_TPHSensor();
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
    TPHData _data;

    bool _init();
    void _check_data();
    void _print_data();
};