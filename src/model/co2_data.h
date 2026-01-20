#pragma once

/**
 * @struct CO2Data
 * @brief Structure for storing CO2 sensor data
 */
struct CO2Data
{
    uint16_t current_baseline; ///< Current sensor baseline
    float co2;                 ///< CO2 value in ppm
    float tvoc;                ///< TVOC value in ppb
    uint8_t aqi;              ///< AQI value
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