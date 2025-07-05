#include "ota.h"

OTA::OTA(): LoopTickerBase() {
    LOG_INFO("init...");
    
    LOG_INFO("init ok!");
    _is_initialized = true;
    this->addLoop();
};

bool OTA::hasUpdate() {
    return _ota.hasUpdate();
}

bool OTA::update(bool now) {
    String ver, notes;

    if (_ota.checkUpdate(&ver, &notes)) {
        LOG_INFO("update to " + ver);
        LOG_INFO("notes: " + notes);

        if (!now) {
            _ota.update();
            return true;
        }

        return _ota.updateNow();
    }

    LOG_ERROR("update failed");

    return false;
};

String OTA::version() {
    return _ota.version();
}

void OTA::exec() {
    _ota.tick();
}