#pragma once
#include <GyverDBFile.h>
#include <LittleFS.h>
#include <Looper.h>
#include "configs/config.h"
#include "configs/secrets.h"

enum kk : size_t {
    wifi_ssid,
    wifi_pass,
    mqtt_enabled,
    mqtt_server,
    mqtt_port,
    mqtt_username,
    mqtt_pass,
    mqtt_device_id,
    co2_measure_prd,
    co2_pub_prd,
    co2_scale_type,
    co2_danger_lvl,
    rgb_enabled,
    use_dark_theme,
    log_level,
};

extern String co2_scale_types;
extern String log_levels;

class SettingsDB : public LoopTickerBase {
public:
    SettingsDB();

    void setup();
    void exec() override;
    GyverDBFile& getDB();

private:
    GyverDBFile _db;
};
