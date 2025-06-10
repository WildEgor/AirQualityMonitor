#include "wifi_conn.h"

WiFiConn::WiFiConn(SettingsDB& settingsDb) : LoopTickerBase(), _db(&settingsDb.getDB()), _wifi_ok(false), _is_initialized(false) {}

void WiFiConn::setup() {
    Serial.println("init wifi...");

    _wifiConnector = new WiFiConnectorClass(AP_NAME, AP_PASS, 60, true);

    _wifiConnector->onConnect([this]() {
        Serial.print("AP IP address: ");
        Serial.println(WiFi.localIP());
        _wifi_ok = true;
    });

    _wifiConnector->onError([this]() {
        Serial.println("WiFi error");
        _wifi_ok = false;
    });
    
    if (!_wifiConnector->connecting()) {
        if ((*_db)[kk::wifi_ssid].length()) {
            Serial.println("connect wifi...");
            _connectToWiFi((*_db)[kk::wifi_ssid], (*_db)[kk::wifi_pass]);
            
            Serial.println("init wifi ok!");
        } else {
            Serial.println("wifi ssid too empty!");
        }

        _is_initialized = true;
    }
}

void WiFiConn::exec() {
    if (!_is_initialized) {
        setup();
        return;
    }

    _wifiConnector->tick();

    if (_wifiConnector->connected()) {
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
    return _wifiConnector->connected();
}

String WiFiConn::ip() {
    return WiFi.localIP().toString();
}

void WiFiConn::_connectToWiFi(const String& ssid, const String& pass) {
    if (ssid.length() == 0) return;

    Serial.print("connecting to ");
    Serial.println(ssid);

    _wifiConnector->connect(ssid, pass);

    int retries = 0;
    while (!_wifiConnector->connected() && retries < WIFI_CONN_RETRY_COUNT) {
        if (WIFI_CONN_RETRY_TIMEOUT >= 500) {
            delay(WIFI_CONN_RETRY_TIMEOUT);
        } else {
            delay(500);
        }
        Serial.print(".");
        retries++;
    }
}
