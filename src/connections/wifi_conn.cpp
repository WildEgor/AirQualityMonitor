#define LOG_COMPONENT "WiFiConn"
#include "services/logger.h"
#include "wifi_conn.h"
#include "wifi_std_adapter.cpp"

WiFiConn::WiFiConn(SettingsDB& settingsDb, WiFiAdapter& wifiAdapter) : LoopTickerBase(), _db(&settingsDb.getDB()), _wifi_adapter(&wifiAdapter), _is_initialized(false) {}

void WiFiConn::setup() {
    LOG_INFO("init...");
    
    if (!isConnected()) {
        _connectToWiFi((*_db)[kk::wifi_ssid], (*_db)[kk::wifi_pass]);
            
        LOG_INFO("init ok!");

        _is_initialized = true;
    }
}

void WiFiConn::exec() {
    if (!_is_initialized) {
        setup();
        return;
    }

    connect();

    if (_wifi_adapter->connected()) {
        _wifi_adapter->tick();
    }
}

void WiFiConn::connect() {
    if (!isConnected() && (*_db)[kk::wifi_ssid].length()) {
        _connectToWiFi((*_db)[kk::wifi_ssid], (*_db)[kk::wifi_pass]);
    }
}

bool WiFiConn::isConnected() {
    return _wifi_adapter->connected() && !_wifi_adapter->connecting();
}

String WiFiConn::ip() {
    return _wifi_adapter->ip();
}

void WiFiConn::_connectToWiFi(const String& ssid, const String& pass) {
    if (ssid.length() == 0) return;

    LOG_INFO("connecting to " + ssid);

    _wifi_adapter->connect(ssid, pass);

    int retries = 0;
    while (!_wifi_adapter->connected() && retries < WIFI_CONN_RETRY_COUNT) {
        if (WIFI_CONN_RETRY_TIMEOUT >= 500) {
            delay(WIFI_CONN_RETRY_TIMEOUT);
        } else {
            delay(500);
        }
        LOG_INFO("retry...");
        retries++;
    }
}
