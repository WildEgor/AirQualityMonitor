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
#include "services/ota.h"

#define LOG_COMPONENT "Settings"
#include "services/logger.h"

class Settings : public LoopTickerBase {
public:
    Settings(SettingsDB& settingsDb, 
        WiFiConn& wifiConn, 
        OTA& ota,
        MQTTConn& mqttConn, 
        RGBController& rgbCtrl,
        HMI& hmi,
        CO2Sensor& co2sensor
    );
    Settings(SettingsDB& settingsDb, 
        WiFiConn& wifiConn
    );

    void exec() override;

private:
    void _init();
    void _update(sets::Updater& u);
    void _build(sets::Builder& b);

    SettingsGyver _sett;
    GyverDBFile* _db;
    WiFiConn* _wifi_conn;
    OTA* _ota;
    MQTTConn* _mqtt_conn;
    RGBController* _rgb_controller;
    HMI* _hmi;
    CO2Sensor* _co2_sensor;

    bool _is_initialized;
};
