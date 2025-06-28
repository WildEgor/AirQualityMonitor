#include "tp.h"
#define LOG_COMPONENT "TPSensor"
#include "services/logger.h"

TPSensor::TPSensor(uint32_t ms) 
    : SensorBase(ms) {
    LOG_INFO("init...");
    // this->addLoop(); // HINT: uncomment if bme180 support
    
    if (!_enable_test && !_init()) {
        LOG_ERROR("init failed! please check your wiring.");
        return;
    }

    _is_initialized = true;
    LOG_INFO("init ok!");
}

void TPSensor::setup() {}

void TPSensor::exec() {
    if (!_is_initialized) {
        init();
        return;
    }
    
    _check_data();
}

float TPSensor::getTemperature() { 
    return _data.temp; 
}

float TPSensor::getPressure() { 
    return _data.pressure; 
}

const char* TPSensor::getType() const { 
    return "tp_sensor"; 
}

float TPSensor::getTemperatureMin() { 
    return -40.0f; 
}

float TPSensor::getTemperatureMax() { 
    return 85.0f; 
}

float TPSensor::getPressureMin() { 
    return 30000.0f;  // 300 hPa in Pa
}

float TPSensor::getPressureMax() { 
    return 110000.0f; // 1100 hPa in Pa
}

bool TPSensor::_init() {
    if (!_sensor.begin(BMP280_ADDR)) {
        LOG_ERROR("failed to begin() sensor! please check your wiring.");
        return false;
    }

    _sensor.setMode(FORCED_MODE);
    return true;
}

void TPSensor::_check_data() {
    if (_enable_test) {
        _data.temp = 25.0f;
        _data.pressure = 101325.0f;
        _print_data();
        return;
    }

    _data.temp = constrain(_sensor.readTemperature(), getTemperatureMin(), getTemperatureMax());
    _data.pressure = constrain(_sensor.readPressure(), getPressureMin(), getPressureMax());

    _print_data();
}

void TPSensor::_print_data() {
    LOG_DEBUG(String("temp: ") + _data.temp + " °C, " +
             "pressure: " + (_data.pressure / 100.0f) + " hPa");
}