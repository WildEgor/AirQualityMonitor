#pragma once

/**
 * @enum Sensor_State
 * @brief Enumeration for sensor states
 */
typedef enum
{
    Sensor_INIT,       ///< Initialization state
    Sensor_RUNNING,    ///< Normal operation state
    Sensor_CALIBRATING ///< Calibration state
} Sensor_State;