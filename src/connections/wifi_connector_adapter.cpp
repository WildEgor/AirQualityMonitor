#include "wifi_conn.h"
#include "configs/config.h"

#define LOG_COMPONENT "WiFiConn"
#include "services/logger.h"

/**
 * @class WiFiConnectorAdapter
 * @brief Implements connection to WiFi using gyverlibs/WiFiConnector
 */
class WiFiConnectorAdapter : public WiFiAdapter
{
public:
    /**
     * @brief Constructor
     * @param APname Access point name
     * @param APpass Access point password
     * @param timeout Connection timeout in seconds
     * @param closeAP Close AP after connection
     */
    WiFiConnectorAdapter(
        const String &APname = "AQM_AP",
        const String &APpass = "",
        uint16_t timeout = 60,
        bool closeAP = false)
    {
        LOG_INFO("init...");
        _wifiConnector = new WiFiConnectorClass(APname, APpass, timeout, closeAP);

        _wifiConnector->onConnect([this]()
                                  { LOG_INFO("connected!"); });

        _wifiConnector->onError([this]()
                                { LOG_WARN("connection error"); });

        _is_initialized = true;
        LOG_INFO("init ok!");
    }

    void connect(const String &ssid, const String &pass = "") override
    {
        _wifiConnector->connect(ssid, pass);
    }

    bool connecting() override
    {
        return _wifiConnector->connecting();
    }

    bool connected() override
    {
        return _wifiConnector->connected();
    }

    String ip() override
    {
        if (!_wifiConnector->connected())
        {
            return WiFi.softAPIP().toString();
        }
        return WiFi.localIP().toString();
    }

    bool exec() override
    {
        return _wifiConnector->tick();
    }

private:
    WiFiConnectorClass *_wifiConnector = nullptr; ///< Pointer to WiFiConnectorClass instance
    bool _is_initialized = false; ///< Initialization flag
};