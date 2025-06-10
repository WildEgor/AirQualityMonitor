#include "co2.h"

CO2Sensor::CO2Sensor(uint32_t ms) : SensorBase(ms), _mock(false) {}

void CO2Sensor::setup() {
    Serial.println("initializing CO2 sensor...");
    _is_initialized = false;

    if (!_mock) {
        _sensor = CCS811(CCS811_ADDR);
        Wire.begin();

        if (!_init()) return;

        _sensor.setDriveMode(2); // 1 - every 1s, 2 - 10s, 3 - 60s measure
    }

    _is_initialized = true;
    Serial.println("CO2 sensor initialized successfully!");
}

void CO2Sensor::exec() {
    if (!_mock) {
        if (!_is_initialized) {
            init();
            return;
        }
        _check_data();
    } else {
        _check_test_data();
    }
}

uint16_t CO2Sensor::getCO2() { return _co2_data.co2; }
uint16_t CO2Sensor::getTVOC() { return _co2_data.tvoc; }

const char* CO2Sensor::getType() const {
    return "co2";
}

float CO2Sensor::getCO2Min() {
    return 400.0f;
}

float CO2Sensor::getCO2Max() {
    return 8192.0f;
}

float CO2Sensor::getTVOCMin() {
    return 0.0f;
}

float CO2Sensor::getTVOCMax() {
    return 1187.0f;
}

bool CO2Sensor::isInitialized() const {
    return _is_initialized;
}

void CO2Sensor::enableTest() {
    _mock = true;
}

void CO2Sensor::copyState(const SensorBase& other) {
    const CO2Sensor& co2_other = static_cast<const CO2Sensor&>(other);
    _mock = co2_other._mock;
    _co2_data = co2_other._co2_data;
    _is_initialized = co2_other._is_initialized;
}

bool CO2Sensor::_init() {
    if (!_sensor.begin()) {
        Serial.println("failed to start CO2 sensor! please check your wiring.");
        return false;
    }
    return true;
}

void CO2Sensor::_check_data() {
    if (_sensor.dataAvailable()) {
        _sensor.readAlgorithmResults();

        _co2_data.co2 = _sensor.getCO2();
        if (_co2_data.co2 > getCO2Max()) {
            _co2_data.co2 = getCO2Max();
        }
        if (_co2_data.co2 < getCO2Min()) {
            _co2_data.co2 = getCO2Min();
        }

        _co2_data.tvoc = _sensor.getTVOC();
        if (_co2_data.tvoc > getTVOCMax()) {
            _co2_data.tvoc = getTVOCMax();
        }
        if (_co2_data.tvoc < getTVOCMin()) {
            _co2_data.tvoc = getTVOCMin();
        }
        _print_data();
    }
}

void CO2Sensor::_check_test_data() {
    _co2_data.co2 = 800;
    _co2_data.tvoc = 3000;
    _print_data();
}

void CO2Sensor::_pub_event() {
    Serial.println("publish co2_data");
    Looper.sendEvent("co2_data", &_co2_data);
}

void CO2Sensor::_print_data() {
    Serial.print("CO2: ");
    Serial.print(_co2_data.co2);
    Serial.print(" ppm, TVOC: ");
    Serial.print(_co2_data.tvoc);
    Serial.println(" ppb");
}

// --- CO2Publisher ---

CO2Publisher::CO2Publisher(uint32_t ms, CO2Sensor& sensor, MQTTConn& mqtt)
    : LoopTimerBase(ms), _sensor(sensor), _mqtt(mqtt), _enabled(true),
      _co2_topic(MQTT_DEFAULT_CO2_TOPIC), _tvoc_topic(MQTT_DEFAULT_TVOC_TOPIC) {}

void CO2Publisher::exec() {
    if (!_sensor.isInitialized() || !_mqtt.isConnected()) return;

    _mqtt.publish(_co2_topic, String(_sensor.getCO2()));
    _mqtt.publish(_tvoc_topic, String(_sensor.getTVOC()));
}

void CO2Publisher::setTopics(const String& co2, const String& tvoc) {
    if (!co2.isEmpty()) _co2_topic = co2;
    if (!tvoc.isEmpty()) _tvoc_topic = tvoc;
}

void CO2Publisher::enable() { _enabled = true; }
void CO2Publisher::disable() { _enabled = false; }

// --- CO2Scale ---

CO2Scale& CO2Scale::getInstance() {
    static CO2Scale instance;
    return instance;
}

CO2Scale::CO2Scale() = default;

void CO2Scale::init(GyverDBFile* db) {
    _db = db;
    _initScales();
}

void CO2Scale::getScale(uint16_t& rs, uint16_t& re, uint16_t& os, uint16_t& oe, uint16_t& ys, uint16_t& ye, uint16_t& gs, uint16_t& ge) {
    if ((*_db)[kk::co2_scale_type].toString() == "DEFAULT") {
        rs = 75;
        re = 100;
        os = 50;
        oe = 75;
        ys = 25;
        ye = 50;
        gs = 0;
        ge = 25;
    } else {
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

void CO2Scale::getColor(uint16_t value, uint8_t& r, uint8_t& g, uint8_t& b) {
    const ColorThreshold* scale;
    size_t size;

    if ((*_db)[kk::co2_scale_type].toString() == "DEFAULT") {
        scale = _default_scale;
        size = 5;
    } else {
        scale = _easy_scale;
        size = 3;
    }

    for (size_t i = 0; i < size; ++i) {
        if (value <= scale[i].threshold) {
            r = scale[i].r;
            g = scale[i].g;
            b = scale[i].b;
            return;
        }
    }

    r = scale[size - 1].r;
    g = scale[size - 1].g;
    b = scale[size - 1].b;
}

float CO2Scale::getMin() { return _min; }
float CO2Scale::getMax() { return _max; }
float CO2Scale::getHumanMax() { return _human_max; }

bool CO2Scale::needAlarm(uint16_t value) { 
    float co2_lvl = (*_db)[kk::co2_danger_lvl].toFloat();
    if (co2_lvl <= 0) {
        return false;
    }

    return value >= co2_lvl + 100; 
}

void CO2Scale::_initScales() {
    _default_scale[0] = {600,  0,   255, 0};
    _default_scale[1] = {800,  0,   255, 128};
    _default_scale[2] = {1000, 255, 255, 0};
    _default_scale[3] = {1500, 255, 128, 0};
    _default_scale[4] = {8000, 255, 0,   0};

    _easy_scale[0] = {600,  0,   255, 0};
    _easy_scale[1] = {1500, 255, 255, 0};
    _easy_scale[2] = {8000, 255, 0,   0};
}
