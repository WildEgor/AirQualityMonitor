#include "tph.h"

TPHSensor::TPHSensor(uint32_t ms) 
    : SensorBase(ms) {
    LOG_INFO("init...");

    _data.pressure = 0.0;
    _data.temp = 0.0;
    _data.humidity = 0.0;
    
    if (!_enable_test && !_init()) {
        LOG_ERROR("init failed! please check your wiring.");
        this->addLoop();
        return;
    }

    _is_initialized = true;
    LOG_INFO("init ok!");
    exec();
    this->addLoop();
}

void TPHSensor::setup() {}

void TPHSensor::exec() {
    if (!_is_initialized) {
        _init();
        return;
    }
    
    _check_data();
}

float TPHSensor::getTemperature() { 
    return _data.temp; 
}

float TPHSensor::getPressure() { 
    return _data.pressure; 
}

float TPHSensor::getHumidity() { 
    return _data.humidity; 
}

const char* TPHSensor::getType() const { 
    return "tph_sensor"; 
}

float TPHSensor::getTemperatureMin() { 
    return -40.0f; 
}

float TPHSensor::getTemperatureMax() { 
    return 85.0f; 
}

float TPHSensor::getPressureMin() { 
    return 30000.0f;  // Minimum pressure: 300 hPa in Pa
}

float TPHSensor::getPressureMax() { 
    return 110000.0f; // Maximum pressure: 1100 hPa in Pa
}

float TPHSensor::getHumidityMin() { 
    return 0.0f; // Maximum pressure: 0 % in RH
}

float TPHSensor::getHumidityMax() { 
    return 100.0f; // Maximum pressure: 100 % in RH
}

bool TPHSensor::_init() {
    if (!_sensor.begin(BME280_ADDR)) {
        LOG_ERROR("init failed! please check your wiring.");
        return false;
    }

    _sensor.setFilter(FILTER_COEF_4);
    
    return true;
}

void TPHSensor::_check_data() {
    if (_enable_test) {
        _data.temp = 25.0f;
        _data.pressure = 1.0f;
        _data.humidity = 100.0f;
        _print_data();
        return;
    }

    _data.temp = constrain(_sensor.readTemperature(), getTemperatureMin(), getTemperatureMax());
    _data.pressure = constrain(_sensor.readPressure(), getPressureMin(), getPressureMax());
    _data.humidity = constrain(_sensor.readHumidity(), getHumidityMin(), getHumidityMax());

    _print_data();
}

void TPHSensor::_print_data() {
    LOG_DEBUG(String("temp: ") + _data.temp + " °C, " +
             "pressure: " + (_data.pressure / 100.0f) + " hPa, " +
            "humidity: " + _data.humidity + " %");
}