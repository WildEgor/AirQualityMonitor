#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "db/settings_db.h"
#include "configs/config.h"
#include "connections/ntp_conn.h"
#include "connections/wifi_conn.h"

extern WiFiClient _espClient;
extern PubSubClient _pub_client;

class MQTTConn : public LoopTickerBase {
public:
    MQTTConn(SettingsDB& settingsDb, WiFiConn& wifiConn);

    void setup();
    void exec() override;
    void connect();
    void publish(const String& topic, const String& payload);
    bool isEnabled() const;
    bool isConnected() const;
    bool isInitialized() const;

private:
    void _connectToMQTT(const String& mqtt_server, uint16_t mqtt_port, const String& mqtt_user, const String& mqtt_password);

    bool _mqtt_ok;
    bool _is_initialized;
    GyverDBFile* _db;
    WiFiConn* _wifi;
};
