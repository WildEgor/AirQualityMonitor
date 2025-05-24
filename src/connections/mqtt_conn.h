#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "db/settings_db.h"
#include "configs/config.h"
#include "connections/ntp_conn.h"

WiFiClient _espClient;
PubSubClient _pub_client(_espClient);

class MQTTConn : public LoopTickerBase {
public:
    MQTTConn() : LoopTickerBase(), _mqtt_ok(false), _db(nullptr), _needs_init(true) {}

    void setup(SettingsDB& settingsDb, WiFiConn& wifiConn) {
        _db = &settingsDb.getDB();
        _wifi = &wifiConn;

        if (!isEnabled()) {
            return;
        }

        Serial.println("init MQTT...");

        _connectToMQTT(
            (*_db)[kk::mqtt_server], 
            (*_db)[kk::mqtt_port].toInt16(), 
            (*_db)[kk::mqtt_username], 
            (*_db)[kk::mqtt_pass]);
        
        Serial.println("init MQTT ok!");
    }

    void exec() override {
        if (!isEnabled()) {
            return;
        }

        if (!_wifi->isConnected()) {
            _mqtt_ok = false;
            return;
        }

        if (!_pub_client.connected()) {
            connect();
            return;
        }

        _pub_client.loop();
    }

    void connect() {
        if (!isEnabled()) {
            return;
        }

        if (!_wifi->isConnected()) {
            return;
        }

        if (!isConnected()) {
            Serial.println("connect to MQTT...");
            _connectToMQTT(
                (*_db)[kk::mqtt_server], 
                (*_db)[kk::mqtt_port].toInt16(), 
                (*_db)[kk::mqtt_username], 
                (*_db)[kk::mqtt_pass]);
        }
    }

    void publish(const String& topic, const String& payload) {
        if (!isEnabled()) {
            return;
        }

        if (!_wifi->isConnected()) {
            return;
        }

        Serial.print("pub to topic: ");
        Serial.println(topic);
        Serial.print(" value: ");
        Serial.println(payload.toFloat());

        _pub_client.publish(topic.c_str(), payload.c_str(), false);
    }

    bool isEnabled() const {
        return (*_db)[kk::mqtt_enabled].toBool();
    }

    bool isConnected() const {
        return _mqtt_ok;
    }

private:
    void _connectToMQTT(const String& mqtt_server, const uint16_t mqtt_port, const String& mqtt_user, const String& mqtt_password) {
        if (!_wifi->isConnected()) {
            return;
        }

        if (mqtt_server.isEmpty()) {
            Serial.println("MQTT server address is empty");
            return;
        }

        if (mqtt_user.isEmpty() || mqtt_password.isEmpty()) {
            Serial.println("MQTT server auth is empty");
            return;
        }

        if (mqtt_port == 0 || mqtt_port > 65535) {
            Serial.println("invalid MQTT port");
            return;
        }

        Serial.print("connecting to MQTT ");
        Serial.println(mqtt_server);

        _pub_client.setKeepAlive(60);
        _pub_client.setSocketTimeout(30);
        _pub_client.setServer(mqtt_server.c_str(), mqtt_port);

        const int max_retries = 3;
        String client_id = "AQM-" + WiFi.macAddress() + String(random(0xffff), HEX);

        if (client_id.isEmpty()) {
            Serial.println("failed to generate client ID");
            return;
        }

        Serial.println("clientId: " + client_id);
        Serial.println("mqtt_user: " + mqtt_user);
        Serial.println("mqtt_password: " + mqtt_password);
        Serial.println("mqtt_server: " + mqtt_server);
        Serial.println("mqtt_port: " + String(mqtt_port));

        for (int retry_count = 0; retry_count < max_retries; retry_count++) {
            Serial.println("attempting MQTT connection client...");

            if (_pub_client.connect(client_id.c_str(), mqtt_user.c_str(), mqtt_password.c_str())) {
                Serial.println("connected");
                _mqtt_ok = true;
                return;
            }
            
            Serial.print("failed, rc=");
            Serial.print(_pub_client.state());
            Serial.println(" try again...");
            delay(1000);
        }

        _mqtt_ok = false;
        Serial.println("failed to connect to MQTT server after maximum retries");
    }

    bool _mqtt_ok;
    bool _needs_init;
    GyverDBFile* _db;
    WiFiConn* _wifi;
};
