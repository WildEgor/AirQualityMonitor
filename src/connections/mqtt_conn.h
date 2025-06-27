#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "db/settings_db.h"
#include "configs/config.h"
#include "connections/wifi_conn.h"

extern WiFiClient _espClient;
extern PubSubClient _pub_client;

class MQTTConn : public LoopTickerBase {
public:
    MQTTConn(SettingsDB& settingsDb, WiFiConn& wifiConn);

    void exec() override;
    void connect();
    void publish(const String& topic, const String& payload);
    void setDeviceID(const String& id);
    bool isEnabled() const;
    bool connected() const;
    bool isInitialized() const;

private:
    void _connectToMQTT(const String& mqtt_server, uint16_t mqtt_port, const String& mqtt_user, const String& mqtt_password);

    GyverDBFile* _db = nullptr;
    WiFiConn* _wifi = nullptr;
    String _device_id = "";
    bool _is_initialized = false;
};
