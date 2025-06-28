#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <SparkFunCCS811.h>

#include "sensor_base.h"
#include "connections/mqtt_conn.h"
#include "model/co2_data.h"
#include "db/settings_db.h"

#define CCS811_ADDR 0x5A

struct ColorThreshold {
    uint16_t threshold;
    uint8_t r, g, b;
};

class CO2Sensor : public SensorBase {
public:
    CO2Sensor(uint32_t ms);

    void setup() override;
    void exec() override;
    float getCO2Min();
    float getCO2Max();
    float getCO2();
    float getTVOCMin();
    float getTVOCMax();
    float getTVOC();
    const char* getType() const override;

private:
    CCS811 _sensor;
    CO2Data _data;

    bool _init();
    void _check_data();
    void _print_data();
};

class CO2Scale {
public:
    static CO2Scale& getInstance();

    void init(GyverDBFile* db);
    void getColor(uint16_t value, uint8_t& r, uint8_t& g, uint8_t& b);
    void getScale(uint16_t& rs, uint16_t& re, uint16_t& os, uint16_t& oe, uint16_t& ys, uint16_t& ye, uint16_t& gs, uint16_t& ge);

    float getMin();
    float getMax();
    float getHumanMax();
    bool needAlarm(uint16_t value);

private:
    CO2Scale();
    void _initScales();

    GyverDBFile* _db;
    ColorThreshold _default_scale[5];
    ColorThreshold _easy_scale[3];
    float _min = 400.0f;
    float _max = 8000.0f;
    float _human_max = 1500.0f;
};
