#pragma once
#include "configs/config.h"

struct HMIState {
    unsigned long last_render_time = 0;
    float last_co2_value = -1;
    String last_fw_ver = String(APP_VERSION);
    bool dark_theme = false;
    bool last_wifi_state = false;
    bool last_co2_sensor_state = false;
};