#pragma once

struct CO2Data {
    float co2;
    float tvoc;

    bool changed;
    float co2_old;
    float tvoc_old;
};