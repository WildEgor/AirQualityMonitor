#pragma once
#include <Arduino.h>
#include <Looper.h>
#include <GyverNTP.h>

class NTPConn : public LoopTickerBase {
public:
    NTPConn() : LoopTickerBase() {}

    void setup() {
        NTP.begin(3); // TODO: use settings_db to get timezone

        NTP.onError([]() {
            Serial.println(NTP.readError());
            Serial.print("online: ");
            Serial.println(NTP.online());
        });

        NTP.onSecond([]() {});

        _is_initialized = true;
    }

    void exec() override {
        if (NTP.tick()) {
            _ms = NTP.ms();
        }
    }

    bool getIsInitialized() {
        return _is_initialized;
    }
    uint16_t getMs() {
        return _ms;
    }

private:
    uint16_t _ms;
    bool _is_initialized;
};

