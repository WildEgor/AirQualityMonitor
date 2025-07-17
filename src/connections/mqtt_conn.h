#pragma once
#include <Arduino.h>
#include <PubSubClient.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

#include "db/settings_db.h"
#include "configs/config.h"
#include "connections/wifi_conn.h"

#define LOG_COMPONENT "MQTTConn"
#include "services/logger.h"

extern WiFiClient _espClient;
extern PubSubClient _pub_client;

/**
 * @class MQTTConn
 * @brief Implements MQTT connection logic
 */
class MQTTConn : public LoopTickerBase
{
public:
    /**
     * @brief Constructor
     * @param settingsDb Reference to settings database
     * @param wifiConn Reference to WiFi connection
     */
    MQTTConn(SettingsDB &settingsDb, WiFiConn &wifiConn);

    /**
     * @brief Handle (re)connect logic
     */
    void exec() override;
    /**
     * @brief Connect to broker
     */
    void connect();
    /**
     * @brief Publish data to topic
     * @param topic MQTT topic
     * @param payload Data to publish
     */
    void publish(const String &topic, const String &payload);
    /**
     * @brief Set device ID for topic prefixes
     * @param id Device ID
     */
    void setDeviceID(const String &id);
    /**
     * @brief Check if publishing is enabled
     * @return true if enabled
     */
    bool isEnabled() const;
    /**
     * @brief Check if still connected to MQTT
     * @return true if connected
     */
    bool connected() const;
    /**
     * @brief Check if class is initialized
     * @return true if initialized
     */
    bool isInitialized() const;

private:
    /**
     * @brief Internal connection logic
     * @param mqtt_server MQTT host
     * @param mqtt_port MQTT port
     * @param mqtt_user MQTT user
     * @param mqtt_password MQTT password
     */
    void _connectToMQTT(const String &mqtt_server, uint16_t mqtt_port, const String &mqtt_user, const String &mqtt_password);

    GyverDBFile *_db = nullptr; ///< Pointer to database
    WiFiConn *_wifi = nullptr;  ///< Pointer to WiFi connection
    String _device_id = "";    ///< Device ID
    bool _is_initialized = false; ///< Initialization flag
};
