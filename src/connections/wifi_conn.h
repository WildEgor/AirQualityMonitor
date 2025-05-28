#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include "db/settings_db.h"
#include "configs/config.h"

class WiFiConn : public LoopTickerBase {
public:
    WiFiConn();

    void setup(SettingsDB& settingsDb);
    void exec() override;

    void connect();
    bool isConnected();

private:
    void _initWiFi();
    void _connectToWiFi(const String& ssid, const String& pass);

    bool _wifi_ok;
    GyverDBFile* _db;
};
