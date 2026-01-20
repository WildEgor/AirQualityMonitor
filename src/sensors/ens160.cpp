#include "ens160.h"

ENS160_CO2Sensor::ENS160_CO2Sensor(uint32_t ms) : CO2SensorBase(), SensorBase(ms)
{
    LOG_INFO("init...");
    _state = Sensor_INIT;
    _data.co2 = 400.0f;
    _data.tvoc = 0.0;
    _is_initialized = _enable_test;

    if (!_enable_test && !_init())
    {
        LOG_ERROR("init failed! please check your wiring.");
        this->addLoop();
        return;
    }

    _state = Sensor_RUNNING;

    LOG_INFO("init ok!");
    exec();
    this->addLoop();
}

bool ENS160_CO2Sensor::begin()
{
    const int max_retries = 3;
    for (int attempt = 0; attempt < max_retries; ++attempt)
    {
        if (_init())
        {
            return true;
        }
        LOG_DEBUG("try retry init...");
        delay(500);
    }
    return _is_initialized;
}

void ENS160_CO2Sensor::exec()
{
    if (!_is_initialized)
    {
        _init();
        return;
    }

    if (_state != Sensor_CALIBRATING)
    {
        _state = Sensor_RUNNING;
        _check_data();
    }
}

float ENS160_CO2Sensor::getCO2() { return _data.co2; }
float ENS160_CO2Sensor::getTVOC() { return _data.tvoc; }

const char *ENS160_CO2Sensor::getType() const
{
    return "ens160";
}

float ENS160_CO2Sensor::getCO2Min()
{
    return 400.0f;
}

float ENS160_CO2Sensor::getCO2Max()
{
    return 8192.0f;
}

float ENS160_CO2Sensor::getTVOCMin()
{
    return 0.0f;
}

float ENS160_CO2Sensor::getTVOCMax()
{
    return 1187.0f;
}

bool ENS160_CO2Sensor::_init()
{
    if (_enable_test || _is_initialized)
    {
        return true;
    }

    LOG_DEBUG("init...");

    _sensor.begin(&Wire, ENS160_ADDR);

    if (!_sensor.init())
    {
        LOG_ERROR("init failed!");
        _is_initialized = false;
        return false;
    }

    _sensor.startStandardMeasure();

    _is_initialized = true;

    LOG_INFO("initialized!");

    return true;
}

void ENS160_CO2Sensor::_check_data()
{
    if (_enable_test)
    {
        _mock_data();
        _print_data();
        return;
    }

    _sensor.wait();

    if (_sensor.update() == RESULT_OK)
    {
        if (_sensor.hasNewData())
        {
            _data.co2 = static_cast<float>(_sensor.getEco2());
            if (_data.co2 >= getCO2Max())
            {
                _data.co2 = getCO2Max();
            }
            if (_data.co2 <= getCO2Min())
            {
                _data.co2 = getCO2Min();
            }

            _data.tvoc = static_cast<float>(_sensor.getTvoc());
            if (_data.tvoc >= getTVOCMax())
            {
                _data.tvoc = getTVOCMax();
            }
            if (_data.tvoc <= getTVOCMin())
            {
                _data.tvoc = getTVOCMin();
            }

            _data.aqi = static_cast<uint8_t>(_sensor.getAirQualityIndex_UBA());

            _print_data();
        }
    }
}

void ENS160_CO2Sensor::_mock_data()
{
    _data.tvoc = 3000.1;
    _data.co2 += 50.0;
    if (_data.co2 >= 1500.0)
        _data.co2 = 0.0;
}

void ENS160_CO2Sensor::_print_data()
{
    LOG_DEBUG("ENS160 - CO2: " + String(_data.co2) + " ppm, TVOC: " + String(_data.tvoc) + " ppb");
}
