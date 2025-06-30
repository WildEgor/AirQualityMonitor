#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <GyverBME280.h>

#include "sensor_base.h"
#include "connections/mqtt_conn.h"
#include "model/tp_data.h"
#include "db/settings_db.h"

#define LOG_COMPONENT "TPSensor"
#include "services/logger.h"

#define BMP280_ADDR 0x76  // Default I2C address (0x77 alternative)

class TPSensor : public SensorBase {
public:
    TPSensor(uint32_t ms);
    
    void setup() override;
    void exec() override;
    float getTemperatureMin();
    float getTemperatureMax();
    float getTemperature();
    float getPressureMin();
    float getPressureMax();
    float getPressure();
    const char* getType() const override;

private:
    GyverBME280 _sensor;
    TPData _data;
    
    bool _init();
    void _check_data();
    void _print_data();
};