#include "WiFi.h"
#include "wifi_conn.h"
#include "configs/config.h"

class WiFiStdAdapter : public WiFiAdapter {
public:
     WiFiStdAdapter(const String& APname = "ESP_AP", const String& APpass = "") {
        WiFi.mode(WIFI_AP_STA);
        WiFi.softAP(APname, APpass);
    }

    void connect(const String& ssid, const String& pass = "") override {
       WiFi.begin(ssid, pass);
    }

    bool connecting() override {
       return WiFi.status() != WL_CONNECTED;
    }

    bool connected() override {
        return WiFi.status() == WL_CONNECTED;
    }

    String ip() override {
        if (WiFi.status() != WL_CONNECTED) {
            return WiFi.softAPIP().toString();
        }
        return WiFi.localIP().toString();
    }

    bool tick() override {
        return WiFi.status() == WL_CONNECTED;
    }
};