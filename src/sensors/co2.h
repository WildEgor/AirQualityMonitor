#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <SparkFunCCS811.h>

#include "sensor_base.h"
#include "connections/mqtt_conn.h"
#include "model/co2_data.h"
#include "db/settings_db.h"

#define LOG_COMPONENT "CO2Sensor"
#include "services/logger.h"

#define CCS811_ADDR 0x5A

struct ColorThreshold {
    uint16_t threshold;
    uint8_t r, g, b;
};

typedef enum {
    CO2Sensor_INIT,
    CO2Sensor_RUNNING,
    CO2Sensor_CALIBRATING,
} CO2Sensor_State;

class CO2Sensor : public SensorBase {
public:
    CO2Sensor(uint32_t ms);

    void setup() override;
    void exec() override;
    float getCO2Min();
    float getCO2Max();
    float getCO2();
    float getTVOCMin();
    float getTVOCMax();
    float getTVOC();
    const char* getType() const override;

    bool isCalibrating() {
        return _state == CO2Sensor_CALIBRATING;
    }

    void startCalibration() {
        LOG_DEBUG("force start calibration");

        if (_data.current_baseline == 0x01) {
            _data.current_baseline = 0x00;
        }

        _state = CO2Sensor_CALIBRATING;

        if (_data.current_baseline == 0x00) {
            _data.current_baseline = _sensor.getBaseline();
        }

        delay(5000);
    };

    void forceStopCalibration() {
        if (_state != CO2Sensor_CALIBRATING || _data.current_baseline == 0x01) {
            return;
        }

        LOG_DEBUG("force stop calibration");

        CCS811Core::CCS811_Status_e errorStatus = _sensor.setBaseline(_data.current_baseline);
        if (errorStatus == CCS811Core::CCS811_Stat_SUCCESS){
          LOG_INFO("baseline written to sensor");
        } else {
            LOG_ERROR("set baseline failed!");
            LOG_ERROR(_sensor.statusString(errorStatus));
        }

        _data.current_baseline = 0x01;

        delay(5000);

        _state = CO2Sensor_RUNNING;

        LOG_INFO("calibration success");
    };

private:
    CCS811 _sensor;
    CO2Data _data;
    CO2Sensor_State _state;

    bool _init();
    void _check_data();
    void _print_data();
};

class CO2Scale {
public:
    static CO2Scale& getInstance();

    void init(GyverDBFile* db);
    void getColor(uint16_t value, uint8_t& r, uint8_t& g, uint8_t& b);
    void getScale(uint16_t& rs, uint16_t& re, uint16_t& os, uint16_t& oe, uint16_t& ys, uint16_t& ye, uint16_t& gs, uint16_t& ge);

    float getMin();
    float getMax();
    float getHumanMax();
    bool needAlarm(uint16_t value);

private:
    CO2Scale();
    void _initScales();

    GyverDBFile* _db;
    ColorThreshold _default_scale[5];
    ColorThreshold _easy_scale[3];
    float _min = 400.0f;
    float _max = 8000.0f;
    float _human_max = 1500.0f;
};
