#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "db.h"
#include "config.h"

WiFiClient espClient;
PubSubClient pub_client(espClient);

class MQTTConn : public LoopTickerBase {
public:
    MQTTConn() : LoopTickerBase(), _mqtt_ok(false), _db(nullptr), _needs_init(true) {}

    void setup(SettingsDB& dbWrapper) {
        _db = &dbWrapper.getDB();
        initMQTT();
    }

    void exec() override {
        if (!pub_client.connected()) {
            connect();
            return;
        }

        pub_client.loop();
    }

    void connect() {
        if (_db && !_mqtt_ok) {
            Serial.println("connect to MQTT...");
            connectToMQTT((*_db)[kk::mqtt_server], (*_db)[kk::mqtt_port].toInt16(), (*_db)[kk::mqtt_username], (*_db)[kk::mqtt_pass]);
        }
    }

    void publish(const String& topic, const String& payload) {
        Serial.print("pub to topic: ");
        Serial.println(topic);
        Serial.print(" value: ");
        Serial.println(payload.toFloat());

        pub_client.publish(topic.c_str(), payload.c_str(), false);
    }

private:
    void initMQTT() {
        Serial.println("init MQTT...");
    
        pub_client.setKeepAlive(60);
        pub_client.setSocketTimeout(30);

        connectToMQTT((*_db)[kk::mqtt_server], (*_db)[kk::mqtt_port].toInt16(), (*_db)[kk::mqtt_username], (*_db)[kk::mqtt_pass]);
        
        Serial.println("init MQTT ok!");
    }

    void connectToMQTT(const String& mqtt_server, const uint16_t mqtt_port, const String& mqtt_user, const String& mqtt_password) {
        if (mqtt_server.length() == 0) {
            Serial.println("MQTT server address is empty");
            return;
        }

        if (mqtt_user.length() == 0 || mqtt_password.length() == 0) {
            Serial.println("MQTT server auth is empty");
            return;
        }

        if (mqtt_port == 0 || mqtt_port > 65535) {
            Serial.println("invalid MQTT port");
            return;
        }

        Serial.print("connecting to MQTT ");
        Serial.println(mqtt_server);

        pub_client.setServer(mqtt_server.c_str(), mqtt_port);

        int retry_count = 0;
        const int max_retries = 3;
        
        while (!pub_client.connected() && retry_count < max_retries) {
            Serial.println("attempting MQTT connection client...");
            String clientId = "AQM-" + WiFi.macAddress();

            Serial.println(clientId);
            
            if (clientId.length() == 0) {
                Serial.println("failed to generate client ID");
                return;
            }

            bool isConnected = pub_client.connect(clientId.c_str(), mqtt_user.c_str(), mqtt_password.c_str());

            if (isConnected) {
                Serial.println("connected");
                _mqtt_ok = true;
                break;
            } else {
                Serial.print("failed, rc=");
                Serial.print(pub_client.state());
                Serial.println(" try again...");
                delay(1000);
                _mqtt_ok = false;
                retry_count++;
            }
        }

        if (!_mqtt_ok) {
            Serial.println("failed to connect to MQTT server after maximum retries");
        }

        delay(1000);
    }

    bool _mqtt_ok;
    bool _needs_init;
    GyverDBFile* _db;
};
