#pragma once
#include <Arduino.h>
#include <WiFiConnector.h>

#include "db/settings_db.h"
#include "configs/config.h"

#define LOG_COMPONENT "WiFiConn"
#include "services/logger.h"

/**
 * @class WiFiAdapter
 * @brief Abstract class for WiFi connectors
 */
class WiFiAdapter
{
public:
    /**
     * @brief Constructor
     * @param APname Access point name
     * @param APpass Access point password
     * @param timeout Connection timeout in seconds
     * @param closeAP Close AP after connection
     */
    WiFiAdapter(const String &APname = "AQM_AP",
                const String &APpass = "",
                uint16_t timeout = 60,
                bool closeAP = false) {}
    virtual ~WiFiAdapter() {}

    /**
     * @brief Try to (re)connect to WiFi network
     * @param ssid WiFi name
     * @param pass WiFi password
     */
    virtual void connect(const String &ssid, const String &pass = "") = 0;
    /**
     * @brief Check if in connecting mode
     * @return true if connecting
     */
    virtual bool connecting() = 0;
    /**
     * @brief Check if connected to network
     * @return true if connected
     */
    virtual bool connected() = 0;
    /**
     * @brief Handle (re)connect logic
     * @return true if successful
     */
    virtual bool exec() = 0;
    /**
     * @brief Return softAP or local IP
     * @return Device IP as string
     */
    virtual String ip() = 0;

private:
    bool _is_initialized = false; ///< Initialization flag
};

/**
 * @class WiFiConn
 * @brief Implements WiFi connection logic
 */
class WiFiConn : public LoopTickerBase
{
public:
    /**
     * @brief Constructor
     * @param settingsDb Reference to settings database
     * @param wifiAdapter Reference to WiFi adapter
     */
    WiFiConn(SettingsDB &settingsDb, WiFiAdapter &wifiAdapter);

    /**
     * @brief Connect to network
     */
    void connect();

    /**
     * @brief Return connection status
     * @return true if connected
     */
    bool connected();
    /**
     * @brief Check if class is initialized
     * @return true if initialized
     */
    bool isInitialized()
    {
        return _is_initialized;
    };
    /**
     * @brief Handle connection loop
     */
    void exec() override;
    /**
     * @brief Return softAP or local IP
     * @return Device IP as string
     */
    String ip();

private:
    /**
     * @brief Internal connect logic calling WiFiAdapter
     * @param ssid WiFi SSID
     * @param pass WiFi password
     */
    void _connect(const String &ssid, const String &pass);

    GyverDBFile *_db;           ///< Pointer to database
    WiFiAdapter *_wifi_adapter; ///< Pointer to WiFi adapter
    bool _is_initialized = false; ///< Initialization flag
};
