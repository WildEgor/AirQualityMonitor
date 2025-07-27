#include "ccs811.h"

CCS811_CO2Sensor::CCS811_CO2Sensor(uint32_t ms) : CO2SensorBase(), SensorBase(ms)
{
    LOG_INFO("init...");
    _state = Sensor_INIT;
    _data.co2 = 400.0f;
    _data.tvoc = 0.0;
    _data.current_baseline = 0x00;
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

bool CCS811_CO2Sensor::begin()
{
    _init();
    return _is_initialized;
}

void CCS811_CO2Sensor::exec()
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

float CCS811_CO2Sensor::getCO2() { return _data.co2; }
float CCS811_CO2Sensor::getTVOC() { return _data.tvoc; }

const char *CCS811_CO2Sensor::getType() const
{
    return "co2";
}

float CCS811_CO2Sensor::getCO2Min()
{
    return 400.0f;
}

float CCS811_CO2Sensor::getCO2Max()
{
    return 8192.0f;
}

float CCS811_CO2Sensor::getTVOCMin()
{
    return 0.0f;
}

float CCS811_CO2Sensor::getTVOCMax()
{
    return 1187.0f;
}

bool CCS811_CO2Sensor::_init()
{
    if (_enable_test || _is_initialized)
    {
        return true;
    }

    _sensor = CCS811(CCS811_ADDR);

    Wire.begin();
    if (_sensor.beginWithStatus() != CCS811Core::CCS811_Stat_SUCCESS)
    {
        LOG_ERROR("init failed!");
        _is_initialized = false;
        return false;
    }

    _sensor.setDriveMode(2); // Set measurement interval: 1 - every 1s, 2 - every 10s, 3 - every 60s

    _is_initialized = true;

    return true;
}

void CCS811_CO2Sensor::_check_data()
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

void CCS811_CO2Sensor::_mock_data()
{
    _data.tvoc = 3000.1;
    _data.co2 += 50.0;
    if (_data.co2 >= 1500.0)
        _data.co2 = 0.0;
}

void CCS811_CO2Sensor::_print_data()
{
    LOG_DEBUG("CO2: " + String(_data.co2) + " ppm, TVOC: " + String(_data.tvoc) + " ppb");
}
