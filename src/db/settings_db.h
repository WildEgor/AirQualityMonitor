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
    co2_measure_prd,
    co2_pub_prd,
    co2_scale_type,
};

extern String co2_scale_types;

class SettingsDB : public LoopTickerBase {
public:
    SettingsDB();

    void setup();
    void exec() override;
    GyverDBFile& getDB();

private:
    GyverDBFile _db;
};
