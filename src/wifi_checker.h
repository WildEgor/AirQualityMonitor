#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include "db.h"

class WiFiChecker : public LoopTickerBase {
public:
    WiFiChecker() : _wifi_ok(false), _db(nullptr), _needs_init(true) {}

    void setup(SettingsDB& dbWrapper) {
        _db = &dbWrapper.getDB();
        wifi_init();
    }

    void exec() override {
    }

    void connect() {
        if (_db && (*_db)[kk::wifi_ssid].length() && !_wifi_ok) {
            Serial.println("connect to wifi...");
            connectToWiFi((*_db)[kk::wifi_ssid], (*_db)[kk::wifi_pass]);
        }
    }

private:
    void wifi_init() {
        Serial.println("init wifi...");

        // First disconnect and reset WiFi
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        delay(100);
        
        // Set WiFi to AP+STA mode
        WiFi.mode(WIFI_AP_STA);
        
        // Configure AP
        const char* ap_ssid = "AirQualityMonitor AP";
        const char* ap_pass = "admin";
        WiFi.softAP(ap_ssid, ap_pass);
        
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
        
        Serial.print("Connecting to ");
        Serial.println(ssid);
        
        WiFi.begin(ssid.c_str(), pass.c_str());
        
        // Wait for connection with timeout
        int timeout = 0;
        while (WiFi.status() != WL_CONNECTED && timeout < 20) {
            delay(500);
            Serial.print(".");
            timeout++;
        }
        Serial.println();
        
        if (WiFi.status() == WL_CONNECTED) {
            Serial.print("Connected! IP address: ");
            Serial.println(WiFi.localIP());
            _wifi_ok = true;
        } else {
            Serial.println("Connection failed!");
            _wifi_ok = false;
        }
    }

    bool _wifi_ok;
    bool _needs_init;
    GyverDBFile* _db;
};
