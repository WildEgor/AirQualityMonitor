#include "co2_scale.h"

CO2Scale &CO2Scale::getInstance()
{
    static CO2Scale instance;
    return instance;
}

CO2Scale::CO2Scale() = default;

void CO2Scale::init(GyverDBFile *db)
{
    _db = db;
    _initScales();
}

void CO2Scale::getScale(uint16_t &rs, uint16_t &re, uint16_t &os, uint16_t &oe, uint16_t &ys, uint16_t &ye, uint16_t &gs, uint16_t &ge)
{
    if ((*_db)[kk::co2_scale_type].toString() == "1")
    {
        rs = 75;
        re = 100;
        os = 50;
        oe = 75;
        ys = 25;
        ye = 50;
        gs = 0;
        ge = 25;
    }
    else
    {
        rs = 66;
        re = 100;
        os = -1;
        oe = -1;
        ys = 33;
        ye = 66;
        gs = 0;
        ge = 33;
    }

    return;
}

void CO2Scale::getColor(uint16_t value, uint8_t &r, uint8_t &g, uint8_t &b)
{
    const ColorThreshold *scale;
    size_t size;

    if ((*_db)[kk::co2_scale_type].toString() == "1")
    {
        scale = _default_scale;
        size = 4;
    }
    else
    {
        scale = _easy_scale;
        size = 3;
    }

    // try find by color in scale
    for (size_t i = 0; i < size; ++i)
    {
        if (value <= scale[i].threshold)
        {
            r = scale[i].r;
            g = scale[i].g;
            b = scale[i].b;
            return;
        }
    }

    // if not in scale choose second default color
    r = scale[size - 1].r;
    g = scale[size - 1].g;
    b = scale[size - 1].b;
}

float CO2Scale::getMin() { return _min; }
float CO2Scale::getMax() { return _max; }
float CO2Scale::getHumanMax() { return _human_max; }

bool CO2Scale::needAlarm(uint16_t value)
{
    float co2_lvl = (*_db)[kk::co2_alarm_lvl].toFloat();
    if (co2_lvl <= 0)
    {
        return false;
    }

    return value >= co2_lvl + 10;
}

void CO2Scale::_initScales()
{
    _default_scale[0] = {690, 0, 255, 0};    // green
    _default_scale[1] = {990, 255, 255, 0};  // yellow
    _default_scale[2] = {1290, 255, 128, 0}; // orange
    _default_scale[3] = {1500, 255, 0, 0};   // red

    _easy_scale[0] = {790, 0, 255, 0};    // green
    _easy_scale[1] = {1090, 255, 255, 0}; // yellow
    _easy_scale[2] = {1500, 255, 0, 0};   // red
}