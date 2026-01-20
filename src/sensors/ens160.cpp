#include "ens160.h"

ENS160_CO2Sensor::ENS160_CO2Sensor(uint32_t ms) : CO2SensorBase(), SensorBase(ms)
{
    LOG_INFO("init...");
    _state = Sensor_INIT;
    _data.co2 = 400.0f;
    _data.tvoc = 0.0;

    if (!_enable_test && _is_initialized)
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
            return _is_initialized;
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
float ENS160_CO2Sensor::getAQI() { return _data.aqi; }

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
        LOG_DEBUG("initialized!");
        _is_initialized = true;
        return _is_initialized;
    }

    LOG_DEBUG("init...");

    if (!_sensor.begin())
    {
        LOG_ERROR("init failed!");
        _is_initialized = false;
        return _is_initialized;
    } else {
        LOG_DEBUG("begin success");
    }

    if (_sensor.setOperatingMode(SFE_ENS160_RESET))
    {
        LOG_INFO("ready");
        delay(100);
        _sensor.setOperatingMode(SFE_ENS160_STANDARD);
    }
    else
    {
        LOG_ERROR("set operation mode fail");
        _is_initialized = false;
        return _is_initialized;
    }

    int ensStatus = _sensor.getFlags();
    LOG_INFO("gas sensor status flags (0 - Standard, 1 - Warm up, 2 - Initial Start Up): " + String(ensStatus));

    _is_initialized = true;
    LOG_INFO("initialized!");

    return _is_initialized;
}

void ENS160_CO2Sensor::_check_data()
{
    if (_enable_test)
    {
        _mock_data();
        _print_data();
        return;
    }

    if (_sensor.checkDataStatus())
    {
        LOG_DEBUG("data is ready");
        _data.co2 = static_cast<float>(_sensor.getECO2());
        if (_data.co2 >= getCO2Max())
        {
            _data.co2 = getCO2Max();
        }
        if (_data.co2 <= getCO2Min())
        {
            _data.co2 = getCO2Min();
        }

        _data.tvoc = static_cast<float>(_sensor.getTVOC());
        if (_data.tvoc >= getTVOCMax())
        {
            _data.tvoc = getTVOCMax();
        }
        if (_data.tvoc <= getTVOCMin())
        {
            _data.tvoc = getTVOCMin();
        }

        _data.aqi = static_cast<uint8_t>(_sensor.getAQI());

        _print_data();
    } else {
        LOG_DEBUG("data not ready");
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
