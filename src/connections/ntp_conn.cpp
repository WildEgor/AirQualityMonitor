#include "ntp_conn.h"

NTPConn::NTPConn() : LoopTickerBase(), _ms(0), _is_initialized(false) {}

void NTPConn::setup() {
    NTP.begin(3);  // TODO: use settings_db to get timezone

    NTP.onError([]() {
        Serial.println(NTP.readError());
        Serial.print("online: ");
        Serial.println(NTP.online());
    });

    NTP.onSecond([]() {
        // you can add code here if needed
    });

    _is_initialized = true;
}

void NTPConn::exec() {
    if (NTP.tick()) {
        _ms = NTP.ms();
    }
}

bool NTPConn::getIsInitialized() {
    return _is_initialized;
}

uint16_t NTPConn::getMs() {
    return _ms;
}
