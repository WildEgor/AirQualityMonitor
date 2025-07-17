#pragma once

/**
 * @struct CO2Data
 * @brief Structure for storing CO2 sensor data
 */
struct CO2Data
{
    bool mock;                 ///< Flag for mock/test mode
    uint16_t current_baseline; ///< Current sensor baseline
    float co2;                 ///< CO2 value in ppm
    float tvoc;                ///< TVOC value in ppb
};

/**
 * @struct ColorThreshold
 * @brief Structure for color threshold mapping
 */
struct ColorThreshold
{
    uint16_t threshold; ///< Threshold value
    uint8_t r, g, b;    ///< RGB color components
};

/**
 * @enum CO2Sensor_State
 * @brief Enumeration for CO2 sensor states
 */
typedef enum
{
    CO2Sensor_INIT,       ///< Initialization state
    CO2Sensor_RUNNING,    ///< Normal operation state
    CO2Sensor_CALIBRATING ///< Calibration state
} CO2Sensor_State;