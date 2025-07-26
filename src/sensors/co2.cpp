#include "co2.h"

CO2Sensor::CO2Sensor(uint32_t ms) : SensorBase(ms), _state(Sensor_INIT)
{
    LOG_INFO("init...");
    _data.co2 = 400.0f;
    _data.tvoc = 0.0;
    _data.current_baseline = 0x00;

    if (!_enable_test && !_init())
    {
        LOG_ERROR("init failed! please check your wiring.");
        this->addLoop();
        return;
    }

    _is_initialized = true;
    _state = Sensor_RUNNING;

    LOG_INFO("init ok!");
    exec();
    this->addLoop();
}

void CO2Sensor::setup() {}

void CO2Sensor::exec()
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

float CO2Sensor::getCO2() { return _data.co2; }
float CO2Sensor::getTVOC() { return _data.tvoc; }

const char *CO2Sensor::getType() const
{
    return "co2";
}

float CO2Sensor::getCO2Min()
{
    return 400.0f;
}

float CO2Sensor::getCO2Max()
{
    return 8192.0f;
}

float CO2Sensor::getTVOCMin()
{
    return 0.0f;
}

float CO2Sensor::getTVOCMax()
{
    return 1187.0f;
}

bool CO2Sensor::_init()
{
    if (_enable_test)
    {
        return true;
    }

    _sensor = CCS811(CCS811_ADDR);

    Wire.begin();
    if (_sensor.beginWithStatus() != CCS811Core::CCS811_Stat_SUCCESS)
    {
        LOG_ERROR("init failed!");
        return false;
    }

    _sensor.setDriveMode(2); // Set measurement interval: 1 - every 1s, 2 - every 10s, 3 - every 60s

    return true;
}

void CO2Sensor::_check_data()
{
    if (_enable_test)
    {
        _mock_data();
        _print_data();
        return;
    }

    if (_sensor.dataAvailable())
    {
        _sensor.readAlgorithmResults();

        _data.co2 = static_cast<float>(_sensor.getCO2());
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

        _print_data();
    }
    else if (_sensor.checkForStatusError())
    {
        uint8_t error = _sensor.getErrorRegister();

        if (error == 0xFF) // comm error
        {
            LOG_ERROR("failed to get ERROR_ID register.");
        }
        else
        {
            String errMsg = "Error: ";
            if (error & 1 << 5)
                errMsg += "HeaterSupply";
            if (error & 1 << 4)
                errMsg += " HeaterFault ";
            if (error & 1 << 3)
                errMsg += " MaxResistance ";
            if (error & 1 << 2)
                errMsg += " MeasModeInvalid ";
            if (error & 1 << 1)
                errMsg += " ReadRegInvalid ";
            if (error & 1 << 0)
                errMsg += "MsgInvalid";
            if (!errMsg.isEmpty())
                LOG_ERROR(errMsg);
        }
    }
}

void CO2Sensor::_mock_data()
{
    _data.tvoc = 3000.1;
    _data.co2 += 50.0;
    if (_data.co2 >= 1500.0) _data.co2 = 0.0;
}

void CO2Sensor::_print_data()
{
    LOG_DEBUG("CO2: " + String(_data.co2) + " ppm, TVOC: " + String(_data.tvoc) + " ppb");
}

// --- CO2Scale ---
CO2Scale &CO2Scale::getInstance()
{
    static CO2Scale instance;
    return instance;
}

CO2Scale::CO2Scale() = default;

void CO2Scale::init(GyverDBFile *db)
{
    _db = db;
    _initScales();
}

void CO2Scale::getScale(uint16_t &rs, uint16_t &re, uint16_t &os, uint16_t &oe, uint16_t &ys, uint16_t &ye, uint16_t &gs, uint16_t &ge)
{
    if ((*_db)[kk::co2_scale_type].toString() == "1")
    {
        rs = 75;
        re = 100;
        os = 50;
        oe = 75;
        ys = 25;
        ye = 50;
        gs = 0;
        ge = 25;
    }
    else
    {
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

void CO2Scale::getColor(uint16_t value, uint8_t &r, uint8_t &g, uint8_t &b)
{
    const ColorThreshold *scale;
    size_t size;

    if ((*_db)[kk::co2_scale_type].toString() == "1")
    {
        scale = _default_scale;
        size = 4;
    }
    else
    {
        scale = _easy_scale;
        size = 3;
    }

    // try find by color in scale
    for (size_t i = 0; i < size; ++i)
    {
        if (value <= scale[i].threshold)
        {
            r = scale[i].r;
            g = scale[i].g;
            b = scale[i].b;
            return;
        }
    }

    // if not in scale choose second default color
    r = scale[size - 1].r;
    g = scale[size - 1].g;
    b = scale[size - 1].b;
}

float CO2Scale::getMin() { return _min; }
float CO2Scale::getMax() { return _max; }
float CO2Scale::getHumanMax() { return _human_max; }

bool CO2Scale::needAlarm(uint16_t value)
{
    float co2_lvl = (*_db)[kk::co2_alarm_lvl].toFloat();
    if (co2_lvl <= 0)
    {
        return false;
    }

    return value >= co2_lvl + 10;
}

void CO2Scale::_initScales()
{
    _default_scale[0] = {690, 0, 255, 0}; // green
    _default_scale[1] = {990, 255, 255, 0}; // yellow
    _default_scale[2] = {1290, 255, 128, 0}; // orange
    _default_scale[3] = {1500, 255, 0, 0}; // red

    _easy_scale[0] = {790, 0, 255, 0}; // green
    _easy_scale[1] = {1090, 255, 255, 0}; // yellow
    _easy_scale[2] = {1500, 255, 0, 0}; // red
}
