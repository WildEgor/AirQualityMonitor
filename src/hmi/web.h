#pragma once
#include <SettingsGyver.h>

#include "configs/config.h"
#include "db/settings_db.h"
#include "connections/mqtt_conn.h"
#include "sensors/co2.h"
#include "sensors/tph.h"
#include "controllers/rgb.h"
#include "connections/wifi_conn.h"
#include "services/publisher.h"
#include "services/ota.h"
#include "display.h"

#define LOG_COMPONENT "WebPannel"
#include "services/logger.h"

/**
 * @name WebPanel
 * @details Class for managing the web interface panel, settings, and integration with other modules
 */
class WebPanel : public LoopTickerBase {
public:
    /**
     * @name WebPanel
     * @param settingsDb - reference to settings database
     * @param wifiConn - reference to WiFi connection
     * @param ota - reference to OTA update service
     * @param mqttConn - reference to MQTT connection
     * @param rgbCtrl - reference to RGB controller
     * @param hmi - reference to display/HMI
     * @param co2sensor - reference to CO2 sensor
     * @param tphSensor - reference to TPH sensor
     * @details Full-featured constructor for WebPanel
     */
    WebPanel(SettingsDB& settingsDb, 
        WiFiConn& wifiConn, 
        OTA& ota,
        MQTTConn& mqttConn, 
        RGBController& rgbCtrl,
        Display& hmi,
        CO2Sensor& co2sensor,
        TPHSensor& tphSensor
    );
    /**
     * @name WebPanel
     * @param settingsDb - reference to settings database
     * @param wifiConn - reference to WiFi connection
     * @details Minimal constructor for WebPanel (WiFi only)
     */
    WebPanel(SettingsDB& settingsDb, 
        WiFiConn& wifiConn
    );

    /**
     * @name exec
     * @details Main loop for web panel logic
     */
    void exec() override;

private:
    /**
     * @name _init
     * @details Internal initialization of web panel and dependencies
     */
    void _init();
    /**
     * @name _update
     * @param u - settings updater
     * @details Update settings from web interface
     */
    void _update(sets::Updater& u);
    /**
     * @name _build
     * @param b - settings builder
     * @details Build web interface structure and settings
     */
    void _build(sets::Builder& b);

    SettingsGyver _sett;              ///< Settings manager instance
    GyverDBFile* _db;                 ///< Pointer to database file
    WiFiConn* _wifi_conn;             ///< Pointer to WiFi connection
    OTA* _ota;                        ///< Pointer to OTA update service
    MQTTConn* _mqtt_conn;             ///< Pointer to MQTT connection
    RGBController* _rgb_controller;   ///< Pointer to RGB controller
    Display* _display;                ///< Pointer to display/HMI
    CO2Sensor* _co2_sensor;           ///< Pointer to CO2 sensor
    TPHSensor* _tph_sensor;           ///< Pointer to TPH sensor

    bool _is_initialized;             ///< Initialization flag
};
