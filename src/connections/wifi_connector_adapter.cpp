#define LOG_COMPONENT "WiFiConn"
#include "services/logger.h"
#include <WiFiConnector.h>
#include "wifi_conn.h"
#include "configs/config.h"

class WiFiConnectorAdapter : public WiFiAdapter {
public:
    WiFiConnectorAdapter(
        const String& APname = "ESP_AP", 
        const String& APpass = "",
        uint16_t timeout = 60,
        bool closeAP = false) {
        _wifiConnector = new WiFiConnectorClass(APname, APpass, timeout, closeAP);

        _wifiConnector->onConnect([this]() {
            LOG_INFO("connected!");
        });

        _wifiConnector->onError([this]() {
            LOG_ERROR("connection error");
        });
    }

    void connect(const String& ssid, const String& pass = "") override {
        _wifiConnector->connect(ssid, pass);
    }

    bool connecting() override {
        return _wifiConnector->connecting();
    }

    bool connected() override {
        return _wifiConnector->connected();
    }

    String ip() override {
        if (!_wifiConnector->connected()) {
            return WiFi.softAPIP().toString();
        }
        return WiFi.localIP().toString();
    }

    bool tick() override {
        return _wifiConnector->tick();
    }
private:
    WiFiConnectorClass* _wifiConnector;
};