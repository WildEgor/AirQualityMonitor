#include "ota.h"

OTA::OTA(WiFiConn &wifiConn) : LoopTickerBase(), _wifi_conn(&wifiConn)
{
    LOG_INFO("init...");

    _ver = APP_VERSION;
    _notes = "";

    LOG_INFO("init ok!");
    _is_initialized = true;
    this->addLoop();
};

bool OTA::hasUpdate()
{ 
    if (!_wifi_conn->connected()) return false;

    String _remote_ver = "";
    _ota.checkUpdate(&_remote_ver, &_notes);
    return !String(APP_VERSION).equals(_remote_ver);
}

bool OTA::update(bool now)
{
    if (!_wifi_conn->connected()) return false;
    
    if (_ota.checkUpdate(&_ver, &_notes))
    {
        LOG_INFO("update to " + _ver);
        if (!_notes.isEmpty())
            LOG_INFO("notes: " + _notes);

        if (!now)
        {
            _ota.update();
            return true;
        }

        return _ota.updateNow();
    }
    else
    {
        LOG_INFO("no updates");
        return true;
    }

    LOG_ERROR("update failed");

    return false;
};

String OTA::version()
{
    return _ver;
}

void OTA::exec()
{
    _ota.tick();
}