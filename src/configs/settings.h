#pragma once
#include <SettingsGyver.h>
#include "config.h"
#include "db/settings_db.h"
#include "connections/wifi_conn.h"
#include "connections/mqtt_conn.h"
#include "sensors/co2.h"
#include "sensors/sensor_base.h"
#include "controllers/rgb.h"

class Settings : public LoopTickerBase {
public:
    Settings();

    // Setup the settings object with necessary wrappers
    void setup(SettingsDB& settingsDb, WiFiConn& wifiConn, MQTTConn& mqttConn, SensorContainer& sensors, RGBController& rgbCtrl);

    // Override the exec function from LoopTickerBase
    void exec() override;

private:
    void update(sets::Updater& u);
    void build(sets::Builder& b);

    SettingsGyver _sett;
    GyverDBFile* _db;
    WiFiConn* _wifi_conn;
    MQTTConn* _mqtt_conn;
    SensorContainer* _sensors;
    RGBController* _rgb_controller;
    bool _needs_init;
};
