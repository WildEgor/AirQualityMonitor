#pragma once
#include <Arduino.h>

#include "db/settings_db.h"
#include "configs/config.h"

class WiFiAdapter {
public:
    WiFiAdapter(const String& APname = "ESP_AP", const String& APpass = "", uint16_t timeout = 60, bool closeAP = false) {}
    virtual ~WiFiAdapter() {}

    virtual void connect(const String& ssid, const String& pass = "") = 0;
    virtual bool connecting() = 0;
    virtual bool connected() = 0;
    virtual bool tick() = 0;
    virtual String ip() = 0;
}; 

class WiFiConn : public LoopTickerBase {
public:
    WiFiConn(SettingsDB& settingsDb, WiFiAdapter& wifiAdapter);

    void setup();
    void exec() override;
    void connect();
    bool isConnected();
    String ip();

private:
    void _connectToWiFi(const String& ssid, const String& pass);

    GyverDBFile* _db;
    WiFiAdapter* _wifi_adapter;
    bool _is_initialized;
};
