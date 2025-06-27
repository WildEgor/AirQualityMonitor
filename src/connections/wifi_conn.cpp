#define LOG_COMPONENT "WiFiConn"
#include "services/logger.h"
#include "wifi_conn.h"

WiFiConn::WiFiConn(SettingsDB& settingsDb, WiFiAdapter& wifiAdapter) : LoopTickerBase(), _db(&settingsDb.getDB()), _wifi_adapter(&wifiAdapter), _is_initialized(false) {
    LOG_INFO("init...");
    
    if (!connected()) {
        _connect((*_db)[kk::wifi_ssid], (*_db)[kk::wifi_pass]);
    }

    LOG_INFO("init ok!");
    
    this->addLoop();
    _is_initialized = true;
}

void WiFiConn::exec() {
    _wifi_adapter->exec();
}

void WiFiConn::connect() {
    _connect((*_db)[kk::wifi_ssid], (*_db)[kk::wifi_pass]);
}

bool WiFiConn::connected() {
    return _wifi_adapter->connected();
}

String WiFiConn::ip() {
    return _wifi_adapter->ip();
}

void WiFiConn::_connect(const String& ssid, const String& pass) {
    if (ssid.length() == 0) return;
    LOG_INFO("connecting to " + ssid);
    _wifi_adapter->connect(ssid, pass);
}
