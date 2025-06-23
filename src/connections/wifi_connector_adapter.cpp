#include <WiFiConnector.h>
#include "wifi_conn.h"
#include "configs/config.h"

class WiFiConnectorAdapter : public WiFiAdapter {
public:
    WiFiConnectorAdapter(const String& APname = "ESP_AP", const String& APpass = "") {
        _wifiConnector = new WiFiConnectorClass(APname, APpass, 30, false); // TODO: move to config
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
        if (_wifiConnector->connecting()) {
            return "0.0.0.0";
        }
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