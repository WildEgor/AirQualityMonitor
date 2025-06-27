#pragma once
#include <Arduino.h>

#include <WiFiConnector.h>
#include "db/settings_db.h"
#include "configs/config.h"

// WiFiAdapter abstract class for wifi connectors
class WiFiAdapter {
public:
    WiFiAdapter(const String& APname = "ESP_AP", const String& APpass = "", uint16_t timeout = 60, bool closeAP = false) {}
    virtual ~WiFiAdapter() {}

    // try connect to network
    virtual void connect(const String& ssid, const String& pass = "") = 0;
    // check and return state
    virtual bool connecting() = 0;
    // check and return state
    virtual bool connected() = 0;
    // call in loop
    virtual bool exec() = 0;
    // return ip (softAP or local)
    virtual String ip() = 0;
private:
    bool _is_initialized = false;
}; 

// WiFiConn wifi connection
class WiFiConn : public LoopTickerBase {
public:
    WiFiConn(SettingsDB& settingsDb, WiFiAdapter& wifiAdapter);

    // connect to network
    void connect();
    // return state
    bool connected();
    // call in loop
    void exec() override;
    // return ip (softAP or local)
    String ip();

private:
    // call WiFiAdapter
    void _connect(const String& ssid, const String& pass);

    GyverDBFile* _db;
    WiFiAdapter* _wifi_adapter;
    bool _is_initialized = false;
};
