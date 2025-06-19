#include "wifi_conn.h"
#define LOG_COMPONENT "WiFiConn"
#include "logger/logger.h"

WiFiConn::WiFiConn(SettingsDB& settingsDb) : LoopTickerBase(), _db(&settingsDb.getDB()), _wifi_ok(false), _is_initialized(false) {}

void WiFiConn::setup() {
    LOG_INFO("init...");

    _wifiConnector = new WiFiConnectorClass(WIFI_AP_NAME, WIFI_AP_PASS, 60, true);

    _wifiConnector->onConnect([this]() {
        LOG_INFO("AP IP address: " + WiFi.localIP().toString());
        _wifi_ok = true;
    });

    _wifiConnector->onError([this]() {
        LOG_ERROR("connection error");
        _wifi_ok = false;
    });
    
    if (!_wifiConnector->connecting()) {
        if ((*_db)[kk::wifi_ssid].length()) {
            _connectToWiFi((*_db)[kk::wifi_ssid], (*_db)[kk::wifi_pass]);
            
            LOG_INFO("init ok!");
        } else {
            LOG_WARNING("wifi ssid is empty!");
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

    LOG_INFO("connecting to " + ssid);

    _wifiConnector->connect(ssid, pass);

    int retries = 0;
    while (!_wifiConnector->connected() && retries < WIFI_CONN_RETRY_COUNT) {
        if (WIFI_CONN_RETRY_TIMEOUT >= 500) {
            delay(WIFI_CONN_RETRY_TIMEOUT);
        } else {
            delay(500);
        }
        LOG_INFO("retry...");
        retries++;
    }
}
