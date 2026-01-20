#pragma once
#include <Arduino.h>

#include "model/co2_data.h"
#include "db/settings_db.h"

/**
 * @name CO2Scale
 * @details Class for working with CO2 color scale and thresholds
 */
class CO2Scale
{
public:
    /**
     * @name getInstance
     * @return CO2Scale& - singleton instance of the class
     */
    static CO2Scale &getInstance();

    /**
     * @name init
     * @param db - pointer to database object
     * @details Initialize scale from database
     */
    void init(GyverDBFile *db);
    /**
     * @name getColor
     * @param value - CO2 value
     * @param r,g,b - color components
     * @details Get color by CO2 value
     */
    void getColor(uint16_t value, uint8_t &r, uint8_t &g, uint8_t &b);
    /**
     * @name getScale
     * @details Get color zone boundaries of the scale
     */
    void getScale(uint16_t &rs, uint16_t &re, uint16_t &os, uint16_t &oe, uint16_t &ys, uint16_t &ye, uint16_t &gs, uint16_t &ge);

    /**
     * @name getMin
     * @return float - minimum scale value
     */
    float getMin();
    /**
     * @name getMax
     * @return float - maximum scale value
     */
    float getMax();
    /**
     * @name getHumanMax
     * @return float - maximum value for human comfort
     */
    float getHumanMax();
    /**
     * @name needAlarm
     * @param value - CO2 value
     * @return bool - whether alarm is needed
     */
    bool needAlarm(uint16_t value);

private:
    /**
     * @name CO2Scale
     * @details Private constructor for singleton
     */
    CO2Scale();
    /**
     * @name _initScales
     * @details Internal initialization of color scales
     */
    void _initScales();

    GyverDBFile *_db;
    ColorThreshold _default_scale[5];
    ColorThreshold _easy_scale[3];
    float _min = 400.0f;
    float _max = 8000.0f;
    float _human_max = 1500.0f;
};