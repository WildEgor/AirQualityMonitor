#pragma once
#include <Arduino.h>
#include <Looper.h>
#include <GyverNTP.h>

class NTPConn : public LoopTickerBase {
public:
    NTPConn();

    void setup();
    void exec() override;

    bool getIsInitialized();
    uint16_t getMs();

private:
    uint16_t _ms;
    bool _is_initialized;
};
