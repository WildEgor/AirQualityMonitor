#include "wifi_conn.h"

/**
 * @brief Constructor for WiFiConn
 * @param settingsDb Reference to settings database
 * @param wifiAdapter Reference to WiFi adapter
 */
WiFiConn::WiFiConn(SettingsDB &settingsDb, WiFiAdapter &wifiAdapter) : LoopTickerBase(), _db(&settingsDb.db()), _wifi_adapter(&wifiAdapter), _is_initialized(false)
{
    LOG_INFO("init...");

    if (!connected())
    {
        connect();
    }

    LOG_INFO("init ok!");

    this->addLoop();
    _is_initialized = true;
}

void WiFiConn::exec()
{
    _wifi_adapter->exec();
}

void WiFiConn::connect()
{
    _connect((*_db)[kk::wifi_ssid], (*_db)[kk::wifi_pass]);
}

bool WiFiConn::connected()
{
    return _wifi_adapter->connected();
}

String WiFiConn::ip()
{
    return _wifi_adapter->ip();
}

/**
 * @brief Internal connect logic calling WiFiAdapter
 * @param ssid WiFi SSID
 * @param pass WiFi password
 */
void WiFiConn::_connect(const String &ssid, const String &pass)
{
    if (ssid.length() == 0)
        return;
    LOG_INFO("connecting to " + ssid);
    _wifi_adapter->connect(ssid, pass);
}
