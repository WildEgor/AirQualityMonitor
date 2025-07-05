#pragma once
#include <AutoOTA.h>
#include <Looper.h>
#include "configs/config.h"

#define LOG_COMPONENT "OTA"
#include "services/logger.h"

class OTA: public LoopTickerBase {
public:
    OTA();

    void exec() override;
    bool hasUpdate();
    bool update(bool now);
    String version();

private:
   AutoOTA _ota = AutoOTA(APP_VERSION, PROJECT_PATH);
   bool _is_initialized = false;
   String _ver = "";
   String _notes = "";
};