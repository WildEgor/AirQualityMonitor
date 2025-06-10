#pragma once
#include <SettingsGyver.h>
#include "config.h"
#include "db/settings_db.h"
#include "connections/wifi_conn.h"
#include "connections/mqtt_conn.h"
#include "sensors/co2.h"
#include "sensors/sensor_base.h"
#include "controllers/rgb.h"
#include "hmi/display.h"

class Settings : public LoopTickerBase {
public:
    Settings(SettingsDB& settingsDb, 
        WiFiConn& wifiConn, 
        MQTTConn& mqttConn, 
        SensorContainer& sensors, 
        RGBController& rgbCtrl,
        Display& display
    );

    void setup();
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
    Display* _display;
    bool _is_initialized;
};
