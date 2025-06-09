#include "wifi_conn.h"

WiFiConn::WiFiConn() : LoopTickerBase(), _wifi_ok(false), _db(nullptr) {}

void WiFiConn::setup(SettingsDB& settingsDb) {
    _db = &settingsDb.getDB();
    
    _initWiFi();
}

void WiFiConn::exec() {
    if (WiFi.status() == WL_CONNECTED) {
        return;
    }
    connect();
}

void WiFiConn::connect() {
    if (!isConnected() && (*_db)[kk::wifi_ssid].length()) {
        Serial.println("connect to wifi...");
        _connectToWiFi((*_db)[kk::wifi_ssid], (*_db)[kk::wifi_pass]);
    }
}

bool WiFiConn::isConnected() {
    return _wifi_ok;
}

void WiFiConn::_initWiFi() {
    Serial.println("init wifi...");

    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    delay(100);

    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(AP_NAME, AP_PASS);

    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());

    if ((*_db)[kk::wifi_ssid].length()) {
        Serial.println("connect wifi...");
        _connectToWiFi((*_db)[kk::wifi_ssid], (*_db)[kk::wifi_pass]);
    }

    Serial.println("init wifi ok!");
}

void WiFiConn::_connectToWiFi(const String& ssid, const String& pass) {
    if (ssid.length() == 0) return;

    Serial.print("connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid.c_str(), pass.c_str());

    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < WIFI_CONN_RETRY_COUNT) {
        if (WIFI_CONN_RETRY_TIMEOUT >= 500) {
            delay(WIFI_CONN_RETRY_TIMEOUT);
        } else {
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
