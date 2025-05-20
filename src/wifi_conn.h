#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include "db.h"
#include "config.h"

#define CONN_RETRY_COUNT 20

class WiFiConn : public LoopTickerBase {
public:
    WiFiConn() : LoopTickerBase(), _wifi_ok(false), _db(nullptr), _needs_init(true) {}

    void setup(SettingsDB& dbWrapper) {
        _db = &dbWrapper.getDB();
        initWiFi();
    }

    void exec() override {}

    // connect only if password set
    void connect() {
        if (_db && (*_db)[kk::wifi_ssid].length() && !_wifi_ok) {
            Serial.println("connect to wifi...");
            connectToWiFi((*_db)[kk::wifi_ssid], (*_db)[kk::wifi_pass]);
        }
    }

    WiFiClass getClient() {
        return WiFi;
    }

private:
    void initWiFi() {
        Serial.println("init wifi...");

        // First disconnect and reset WiFi
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        delay(100);
        
        // Set WiFi to AP+STA mode
        WiFi.mode(WIFI_AP_STA);
        
        // Configure AP
        WiFi.softAP(AP_NAME, AP_PASS);
        
        Serial.print("AP IP address: ");
        Serial.println(WiFi.softAPIP());
        
        // Try to connect to saved network if available
        if (_db && (*_db)[kk::wifi_ssid].length()) {
            Serial.println("connect wifi...");
            connectToWiFi((*_db)[kk::wifi_ssid], (*_db)[kk::wifi_pass]);
        }
        
        Serial.println("init wifi ok!");
    }

    void connectToWiFi(const String& ssid, const String& pass) {
        if (ssid.length() == 0) return;
        
        Serial.print("connecting to ");
        Serial.println(ssid);
        
        WiFi.begin(ssid.c_str(), pass.c_str());
        
        int retries = 0;
        while (WiFi.status() != WL_CONNECTED && retries < CONN_RETRY_COUNT) {
            delay(500);
            Serial.print(".");
            retries++;
        }
        Serial.println();
        
        if (WiFi.status() == WL_CONNECTED) {
            Serial.print("wifi ok! IP address: ");
            Serial.println(WiFi.localIP());
            _wifi_ok = true;
        } else {
            Serial.println("wifi error! connection failed!");
            _wifi_ok = false;
        }
    }

    bool _wifi_ok;
    bool _needs_init;
    GyverDBFile* _db;
};
