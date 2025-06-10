#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include "db/settings_db.h"
#include "configs/config.h"

class WiFiConn : public LoopTickerBase {
public:
    WiFiConn(SettingsDB& settingsDb);

    void setup();
    void exec() override;
    void connect();
    bool isConnected();
    String ip();

private:
    void _connectToWiFi(const String& ssid, const String& pass);

    GyverDBFile* _db;
    bool _wifi_ok;
    bool _is_initialized;
};
