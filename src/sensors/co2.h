#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <SparkFunCCS811.h>
#include "sensor_base.h"
#include "connections/mqtt_conn.h"
#include "model/co2_data.h"

#define CCS811_ADDR 0x5B //Default I2C Address
//#define CCS811_ADDR 0x5A //Alternate I2C Address

class CO2Sensor : public SensorBase {
public:
    CO2Sensor(uint32_t ms) 
        : SensorBase(ms), _mock(false) {}

    void setup() override {
        Serial.println("initializing CO2 sensor...");
        _is_initialized = false;
        
        if (!_mock) {
            _sensor = CCS811(CCS811_ADDR);
        
            Wire.begin();
            
            if (!_init()) {
                return;
            }

            _sensor.setDriveMode(1);
        }

        _is_initialized = true;
        Serial.println("CO2 sensor initialized successfully!");
    }

    void exec() override {
        if (!_mock) {
            if (!_is_initialized) {
                init();
                return;
            }
            _check_data();
            return;
        }

        _check_test_data();
    }

    uint16_t getCO2() { return _co2_data.co2; }
    uint16_t getTVOC() { return _co2_data.tvoc; }
    const char* getType() const override {
        return "co2"; // TODO: make random unique
    }
    bool getIsInitialized() const override {
        return _is_initialized;
    }

    void enableTest() {
        _mock = true;
    }

    void copyState(const SensorBase& other) override {
        const CO2Sensor& co2_other = static_cast<const CO2Sensor&>(other);
        _mock = co2_other._mock;
        _co2_data = co2_other._co2_data;
        _is_initialized = co2_other._is_initialized;
    }

private:
    CCS811 _sensor;
    bool _mock;
    bool _is_initialized;
    CO2Data _co2_data;

    bool _init() {
        if (!_sensor.begin()) {
            Serial.println("failed to start CO2 sensor! please check your wiring.");
            return false;
        }

        return true;
    }

    void _check_data() {
        if (_sensor.dataAvailable()) {
            _sensor.readAlgorithmResults();
            
            _co2_data.co2 = _sensor.getCO2();
            _co2_data.tvoc = _sensor.getTVOC();
            
            _print_data();
        }
    }

    void _check_test_data() {
        _co2_data.co2 = 800;
        _co2_data.tvoc = 3000;

        _print_data();
    }

    void _pub_event() {
        Looper.sendEvent("co2_data", &_co2_data);
    }

    void _print_data() {
        Serial.print("CO2: ");
        Serial.print(_co2_data.co2);
        Serial.print(" ppm, TVOC: ");
        Serial.print(_co2_data.tvoc);
        Serial.println(" ppb");
    }
};

// CO2Publisher publish after N ms payload with CO2 Sensor data to topics
class CO2Publisher : public LoopTimerBase {
public:
    CO2Publisher(uint32_t ms, CO2Sensor& sensor, MQTTConn& mqtt) 
        : LoopTimerBase(ms), _sensor(sensor), _mqtt(mqtt), _enabled(true) {}

    // exec - check if sensor initialized and publish data
    void exec() override {
        if (!_sensor.getIsInitialized() || !_mqtt.isConnected()) {
            return;
        }
    
        _mqtt.publish(_co2_topic, String(_sensor.getCO2()));
        _mqtt.publish(_tvoc_topic, String(_sensor.getTVOC()));
    }

    // setTopics - can change topic(s) names
    void setTopics(const String& co2, const String& tvoc) {
        if (co2.length() == 0 || tvoc.length() == 0) return;
        
        _co2_topic = co2;
        _tvoc_topic = tvoc;
    }

    void enable() {
        _enabled = true;
    }

    void disable() {
        _enabled = false;
    }

private:
    CO2Sensor& _sensor;
    MQTTConn& _mqtt;

    bool _enabled;
    String _co2_topic = "common/aqm/co2";
    String _tvoc_topic = "common/aqm/tvoc";
};