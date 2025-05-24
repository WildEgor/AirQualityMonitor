#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include "db/settings_db.h"
#include "configs/config.h"

class WiFiConn : public LoopTickerBase {
public:
    WiFiConn() : LoopTickerBase(), _wifi_ok(false), _db(nullptr) {}

    void setup(SettingsDB& settingsDb) {
        _db = &settingsDb.getDB();
        _initWiFi();
    }

    void exec() override {
       if (WiFi.status() == WL_CONNECTED) {
          return;
       }

       connect();
    }

    // connect only if password set
    void connect() {
        if (!isConnected() && (*_db)[kk::wifi_ssid].length()) {
            Serial.println("connect to wifi...");
            _connectToWiFi((*_db)[kk::wifi_ssid], (*_db)[kk::wifi_pass]);
        }
    }

    bool isConnected() {
        return _wifi_ok;
    }

private:
    void _initWiFi() {
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
        if ((*_db)[kk::wifi_ssid].length()) {
            Serial.println("connect wifi...");
            _connectToWiFi((*_db)[kk::wifi_ssid], (*_db)[kk::wifi_pass]);
        }
        
        Serial.println("init wifi ok!");
    }

    void _connectToWiFi(const String& ssid, const String& pass) {
        if (ssid.length() == 0) return;
        
        Serial.print("connecting to ");
        Serial.println(ssid);
        
        WiFi.begin(ssid.c_str(), pass.c_str());
        
        int retries = 0;
        while (WiFi.status() != WL_CONNECTED && retries < WIFI_CONN_RETRY_COUNT) {
            if (WIFI_CONN_RETRY_TIMEOUT >= 500) {
                delay(WIFI_CONN_RETRY_TIMEOUT);
            }else {
                delay(500);         
            }
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
    GyverDBFile* _db;
};
