#pragma once
#include "configs/config.h"

struct DisplayState
{
    unsigned long last_render_time = 0;
    float last_co2_value = -1;
    float last_temp_value = -1;
    float last_pressure_value = -1;
    float last_humidity_value = -1;
    String last_fw_ver = String(APP_VERSION);
    bool has_updates = false;
    bool dark_theme = false;
    bool last_wifi_state = false;
    bool last_mqtt_state = false;
    bool last_co2_sensor_state = false;
};