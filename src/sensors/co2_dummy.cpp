#include "co2_dummy.h"

Dummy_CO2Sensor::Dummy_CO2Sensor() : CO2SensorBase(), SensorBase(0)
{
    LOG_INFO("init...");
    _state = Sensor_INIT;
    _data.co2 = 400.0f;
    _data.tvoc = 0.0;
    _is_initialized = _enable_test;

    if (!_enable_test && !_init())
    {
        LOG_ERROR("init failed! please check your wiring.");
        return;
    }

    _state = Sensor_RUNNING;

    LOG_INFO("init ok!");
    exec();
}

bool Dummy_CO2Sensor::begin()
{
    _init();
    return _is_initialized;
}

void Dummy_CO2Sensor::exec()
{
    if (!_is_initialized)
    {
        _init();
        return;
    }

    if (!isCalibrating())
    {
        _state = Sensor_RUNNING;
        _check_data();
    }
}

float Dummy_CO2Sensor::getCO2() { return _data.co2; }
float Dummy_CO2Sensor::getTVOC() { return _data.tvoc; }
float Dummy_CO2Sensor::getAQI() { return _data.aqi; }

const char *Dummy_CO2Sensor::getType() const
{
    return "co2_dummy";
}

float Dummy_CO2Sensor::getCO2Min()
{
    return 400.0f;
}

float Dummy_CO2Sensor::getCO2Max()
{
    return 8192.0f;
}

float Dummy_CO2Sensor::getTVOCMin()
{
    return 0.0f;
}

float Dummy_CO2Sensor::getTVOCMax()
{
    return 1187.0f;
}

bool Dummy_CO2Sensor::_init()
{
    if (_enable_test)
    {
        return true;
    }

    LOG_DEBUG("init...");

    _is_initialized = true;

    LOG_INFO("initialized!");

    return true;
}

void Dummy_CO2Sensor::_check_data()
{
    if (_enable_test)
    {
        _mock_data();
        _print_data();
        return;
    }
}

void Dummy_CO2Sensor::_mock_data()
{
    _data.tvoc = 3000.1;
    _data.co2 += 50.0;
    if (_data.co2 >= 1500.0)
        _data.co2 = 0.0;
}

void Dummy_CO2Sensor::_print_data()
{
    LOG_DEBUG("CO2 Dummy - CO2: " + String(_data.co2) + " ppm, TVOC: " + String(_data.tvoc) + " ppb");
}
