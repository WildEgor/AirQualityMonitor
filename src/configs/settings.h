#pragma once
#include <SettingsGyver.h>
#include "config.h"
#include "db/settings_db.h"
#include "connections/mqtt_conn.h"
#include "sensors/co2.h"
#include "sensors/sensor_base.h"
#include "controllers/rgb.h"
#include "hmi/hmi.h"
#include "connections/wifi_conn.h"
#include "services/publisher.h"

class Settings : public LoopTickerBase {
public:
    Settings(SettingsDB& settingsDb, 
        WiFiConn& wifiConn, 
        MQTTConn& mqttConn, 
        RGBController& rgbCtrl,
        HMI& hmi
    );
    Settings(SettingsDB& settingsDb, WiFiConn& wifiConn);

    void setup();
    void exec() override;

private:
    void update(sets::Updater& u);
    void build(sets::Builder& b);

    SettingsGyver _sett;
    GyverDBFile* _db;
    WiFiConn* _wifi_conn;
    MQTTConn* _mqtt_conn;
    RGBController* _rgb_controller;
    HMI* _hmi;
    bool _is_initialized;
};
