#define LOG_COMPONENT "WiFiConn"
#include "services/logger.h"
#include "wifi_conn.h"

WiFiConn::WiFiConn(SettingsDB& settingsDb, WiFiAdapter& wifiAdapter) : LoopTickerBase(), _db(&settingsDb.getDB()), _wifi_adapter(&wifiAdapter), _is_initialized(false) {}

void WiFiConn::setup() {
    LOG_INFO("init...");
    
    if (!isConnected()) {
        _connectToWiFi((*_db)[kk::wifi_ssid], (*_db)[kk::wifi_pass]);
    }

    _is_initialized = true;
    LOG_INFO("init ok!");
}

void WiFiConn::exec() {
    if (!_is_initialized) {
        setup();
        return;
    }

    _wifi_adapter->tick();
}

void WiFiConn::connect() {
    _connectToWiFi((*_db)[kk::wifi_ssid], (*_db)[kk::wifi_pass]);
}

bool WiFiConn::isConnected() {
    return _wifi_adapter->connected();
}

String WiFiConn::ip() {
    return _wifi_adapter->ip();
}

void WiFiConn::_connectToWiFi(const String& ssid, const String& pass) {
    if (ssid.length() == 0) return;
    LOG_INFO("connecting to " + ssid);
    _wifi_adapter->connect(ssid, pass);
}
