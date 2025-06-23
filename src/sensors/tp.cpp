#include "tp.h"
#define LOG_COMPONENT "TPSensor"
#include "services/logger.h"

TPSensor::TPSensor(uint32_t ms) 
    : SensorBase(ms), _mock(false), _is_initialized(false) {}

void TPSensor::setup() {
    LOG_INFO("init...");
    _is_initialized = false;
    
    if (!_mock) {
        if (!_init()) {
            LOG_ERROR("failed to init sensor! please check your wiring.");
            return;
        }
    }

    _is_initialized = true;
    LOG_INFO("init ok!");
}

void TPSensor::exec() {
    if (_mock) {
        _check_test_data();
        return;
    }

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

bool TPSensor::isInitialized() const { 
    return _is_initialized; 
}

void TPSensor::enableTest() { 
    _mock = true; 
}

void TPSensor::copyState(const SensorBase& other) {
    const TPSensor& _other = static_cast<const TPSensor&>(other);
    _mock = _other._mock;
    _data = _other._data;
    _is_initialized = _other._is_initialized;
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
    float temp = _sensor.readTemperature();
    float pressure = _sensor.readPressure();

    _data.temp = constrain(temp, getTemperatureMin(), getTemperatureMax());
    _data.pressure = constrain(pressure, getPressureMin(), getPressureMax());

    _print_data();
    _pub_event();
}

void TPSensor::_check_test_data() {
    _data.temp = 25.0f;
    _data.pressure = 101325.0f;  // Standard pressure
    _print_data();
}

void TPSensor::_pub_event() {
    LOG_DEBUG("pub sensor data");
    Looper.sendEvent("tp_data", &_data);
}

void TPSensor::_print_data() {
    LOG_DEBUG(String("temp: ") + _data.temp + " °C, " +
             "pressure: " + (_data.pressure / 100.0f) + " hPa");
}