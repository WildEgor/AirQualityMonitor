#include "bme280.h"

BME280_TPHSensor::BME280_TPHSensor(uint32_t ms, SettingsDB &settingsDb)
    : TPHSensorBase(), SensorBase(ms), _db(&settingsDb.db())
{
    LOG_INFO("init...");

    _data.pressure = 0.0;
    _data.temp = 0.0;
    _data.humidity = 0.0;

    if (!_enable_test && !_is_initialized)
    {
        LOG_ERROR("init failed! please check your wiring.");
        this->addLoop();
        return;
    }

    LOG_INFO("init ok!");
    exec();
    this->addLoop();
}

bool BME280_TPHSensor::begin()
{
    const int max_retries = 3;
    for (int attempt = 0; attempt < max_retries; ++attempt) {
        if (_init()) {
            return true;
        }
        delay(100);
    }
    return _is_initialized;
}

void BME280_TPHSensor::exec()
{
    if (!_is_initialized)
    {
        _init();
        return;
    }

    _check_data();
}

float BME280_TPHSensor::getTemperature()
{
    return _data.temp;
}

float BME280_TPHSensor::getPressure()
{
    return _data.pressure;
}

float BME280_TPHSensor::getHumidity()
{
    return _data.humidity;
}

const char *BME280_TPHSensor::getType() const
{
    return "tph_sensor";
}

float BME280_TPHSensor::getTemperatureMin()
{
    return -40.0f;
}

float BME280_TPHSensor::getTemperatureMax()
{
    return 85.0f;
}

float BME280_TPHSensor::getPressureMin()
{
    return 30000.0f; // Minimum pressure: 300 hPa in Pa
}

float BME280_TPHSensor::getPressureMax()
{
    return 110000.0f; // Maximum pressure: 1100 hPa in Pa
}

float BME280_TPHSensor::getHumidityMin()
{
    return 0.0f; // Maximum pressure: 0 % in RH
}

float BME280_TPHSensor::getHumidityMax()
{
    return 100.0f; // Maximum pressure: 100 % in RH
}

bool BME280_TPHSensor::_init()
{
    if (_enable_test || _is_initialized)
    {
        return true;
    }

    if (!_sensor.begin(BME280_ADDR))
    {
        LOG_ERROR("init failed! please check your wiring.");
        _is_initialized = false;
        return false;
    }

    _sensor.setFilter(FILTER_COEF_4);

    _is_initialized = true;

    return true;
}

void BME280_TPHSensor::_check_data()
{
    if (_enable_test)
    {
        _data.temp = 25.0f;
        _data.pressure = 1.0f;
        _data.humidity = 100.0f;
        _print_data();
        return;
    }

    _data.temp = constrain(_sensor.readTemperature() - (*_db)[kk::temp_offset].toFloat(), getTemperatureMin(), getTemperatureMax());
    _data.pressure = constrain(_sensor.readPressure(), getPressureMin(), getPressureMax());
    _data.humidity = constrain(_sensor.readHumidity(), getHumidityMin(), getHumidityMax());

    _print_data();
}

void BME280_TPHSensor::_print_data()
{
    LOG_DEBUG(String("temp: ") + _data.temp + " °C, " +
              "pressure: " + (_data.pressure / 100.0f) + " hPa, " +
              "humidity: " + _data.humidity + " %");
}