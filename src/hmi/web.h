#pragma once
#include <SettingsGyver.h>

#include "configs/config.h"
#include "db/settings_db.h"
#include "connections/mqtt_conn.h"
#include "sensors/co2.h"
#include "sensors/sensor_base.h"
#include "controllers/rgb.h"
#include "connections/wifi_conn.h"
#include "services/publisher.h"
#include "services/ota.h"
#include "display.h"

#define LOG_COMPONENT "WebPannel"
#include "services/logger.h"

class WebPanel : public LoopTickerBase {
public:
    WebPanel(SettingsDB& settingsDb, 
        WiFiConn& wifiConn, 
        OTA& ota,
        MQTTConn& mqttConn, 
        RGBController& rgbCtrl,
        Display& hmi,
        CO2Sensor& co2sensor
    );
    WebPanel(SettingsDB& settingsDb, 
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
    Display* _display;
    CO2Sensor* _co2_sensor;

    bool _is_initialized;
};
