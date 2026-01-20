#include "tph_dummy.h"

Dummy_TPHSensor::Dummy_TPHSensor()
    : TPHSensorBase(), SensorBase(0)
{
    LOG_INFO("init...");

    _data.pressure = 0.0;
    _data.temp = 0.0;
    _data.humidity = 0.0;
    _is_initialized = _enable_test;

    if (!_enable_test && !_init())
    {
        LOG_ERROR("init failed! please check your wiring.");
        return;
    }

    LOG_INFO("init ok!");
    exec();
}

bool Dummy_TPHSensor::begin()
{
    _init();
    return _is_initialized;
}

void Dummy_TPHSensor::exec()
{
    if (!_is_initialized)
    {
        _init();
        return;
    }

    _check_data();
}

float Dummy_TPHSensor::getTemperature()
{
    return _data.temp;
}

float Dummy_TPHSensor::getPressure()
{
    return _data.pressure;
}

float Dummy_TPHSensor::getHumidity()
{
    return _data.humidity;
}

const char *Dummy_TPHSensor::getType() const
{
    return "dummy_tph";
}

float Dummy_TPHSensor::getTemperatureMin()
{
    return -40.0f;
}

float Dummy_TPHSensor::getTemperatureMax()
{
    return 85.0f;
}

float Dummy_TPHSensor::getPressureMin()
{
    return 30000.0f; // Minimum pressure: 300 hPa in Pa
}

float Dummy_TPHSensor::getPressureMax()
{
    return 110000.0f; // Maximum pressure: 1100 hPa in Pa
}

float Dummy_TPHSensor::getHumidityMin()
{
    return 0.0f; // Maximum pressure: 0 % in RH
}

float Dummy_TPHSensor::getHumidityMax()
{
    return 100.0f; // Maximum pressure: 100 % in RH
}

bool Dummy_TPHSensor::_init()
{
    _is_initialized = true;
    return true;
}

void Dummy_TPHSensor::_check_data()
{
    if (_enable_test)
    {
        _data.temp = 25.0f;
        _data.pressure = 1.0f;
        _data.humidity = 100.0f;
        _print_data();
        return;
    }
    _print_data();
}

void Dummy_TPHSensor::_print_data()
{
    LOG_DEBUG(String("temp: ") + _data.temp + " °C, " +
              "pressure: " + (_data.pressure / 100.0f) + " hPa, " +
              "humidity: " + _data.humidity + " %");
}