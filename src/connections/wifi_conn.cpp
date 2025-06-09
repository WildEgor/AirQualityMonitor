#include "wifi_conn.h"

WiFiConn::WiFiConn(SettingsDB& settingsDb) : LoopTickerBase(), _db(&settingsDb.getDB()), _wifi_ok(false), _is_initialized(false) {}

void WiFiConn::setup() {
    Serial.println("init wifi...");

    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    delay(100);

    // TODO: need find fix wifi cause no AP available and if connect it crash
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(AP_NAME, AP_PASS);

    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());

    if ((*_db)[kk::wifi_ssid].length()) {
        Serial.println("connect wifi...");
        _connectToWiFi((*_db)[kk::wifi_ssid], (*_db)[kk::wifi_pass]);

        Serial.println("init wifi ok!");
        _is_initialized = true;
    } else {
        Serial.println("wifi ssid too empty!");
    }
}

void WiFiConn::exec() {
    if (!_is_initialized) {
        setup();
        return;
    }

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
