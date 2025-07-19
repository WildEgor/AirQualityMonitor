#include "web.h"

sets::Logger webLogger(255);
bool cfm_fw = false;

WebPanel::WebPanel(
    SettingsDB &settingsDb,
    WiFiConn &wifiConn)
    : LoopTickerBase(),
      _sett(String(APP_NAME) + " v" + String(APP_VERSION), &settingsDb.db()),
      _db(&settingsDb.db()),
      _wifi_conn(&wifiConn),
      _is_initialized(false)
{
    _init();
}

WebPanel::WebPanel(
    SettingsDB &settingsDb,
    WiFiConn &wifiConn,
    OTA &ota,
    MQTTConn &mqttConn,
    RGBController &rgbController,
    Display &display,
    CO2Sensor &co2sensor,
    TPHSensor &tphSeonsor)
    : LoopTickerBase(),
      _sett(String(APP_NAME) + " v" + ota.version(), &settingsDb.db()),
      _db(&settingsDb.db()),
      _wifi_conn(&wifiConn),
      _ota(&ota),
      _mqtt_conn(&mqttConn),
      _rgb_controller(&rgbController),
      _display(&display),
      _co2_sensor(&co2sensor),
      _tph_sensor(&tphSeonsor),
      _is_initialized(false)
{
    _init();
}

void WebPanel::exec()
{
    if (!_is_initialized)
    {
        LOG_ERROR("call setup first!");
        return;
    }

    _sett.tick();
}

void WebPanel::_init()
{
    LOG_INFO("init...");

    Logger::getInstance().initWebLogger(webLogger);

    _sett.config.requestTout = SEC_10;
    _sett.config.pingTout = SEC_30;
    _sett.config.updateTout = 0;
    _sett.config.theme = sets::Colors::Green;
    _sett.begin(false);

    _sett.onUpdate([this](sets::Updater &u)
                   { this->_update(u); });

    _sett.onBuild([this](sets::Builder &b)
                  { this->_build(b); });

    _sett.onFocusChange([this]()
                        { LOG_DEBUG("browser connected!"); });

    LOG_INFO("init ok!");

    _is_initialized = true;
    this->addLoop();
}

void WebPanel::_update(sets::Updater &u)
{
#ifdef WEB_PANEL_DASHBOARD
    u.update(SH("eco2_gauge"), _co2_sensor->getCO2());
    u.update("tvoc_gauge"_h, _co2_sensor->getTVOC());
    u.update("temp_gauge"_h, _tph_sensor->getTemperature());
    u.update("pressure_gauge"_h, _tph_sensor->getPressure());
    u.update("humidity_gauge"_h, _tph_sensor->getHumidity());
#endif

    u.update(H(log), webLogger);

    if (_ota && _ota->hasUpdate())
        u.update("update"_h, "New updates available. Try update firmware?");
}

void WebPanel::_build(sets::Builder &b)
{
#ifdef WEB_PANEL_DASHBOARD
    SUB_BUILD_BEGIN
    sets::Group g(b, "Dashboard");
    b.LinearGauge(SH("eco2_gauge"), "eCO2", _co2_sensor->getCO2Min(), _co2_sensor->getCO2Max(), "ppm", 0.0f);
    b.LinearGauge("tvoc_gauge"_h, "TVOC", _co2_sensor->getTVOCMin(), _co2_sensor->getTVOCMax(), "ppb", 0.0f);
    b.LinearGauge("temp_gauge"_h, "Temp", _tph_sensor->getTemperatureMin(), _tph_sensor->getTemperatureMax(), "°C", 0.0f);
    b.LinearGauge("pressure_gauge"_h, "Pressure", _tph_sensor->getPressureMin(), _tph_sensor->getPressureMax(), "hPa", 0.0f);
    b.LinearGauge("humidity_gauge"_h, "Humidity", _tph_sensor->getHumidityMin(), _tph_sensor->getHumidityMax(), "%", 0.0f);
    SUB_BUILD_END
#endif

    SUB_BUILD_BEGIN
    sets::Menu m(b, "WiFi");
    b.Input(kk::wifi_ssid, "SSID");
    b.Pass(kk::wifi_pass, "Password");
    b.Button(SH("wifi_save"), "Save");
    SUB_BUILD_END

    SUB_BUILD_BEGIN
    sets::Menu m(b, "MQTT");
    b.Switch(kk::mqtt_enabled, "Enabled");
    b.Input(kk::mqtt_server, "Server");
    b.Number(kk::mqtt_port, "Port");
    b.Input(kk::mqtt_username, "Username");
    b.Pass(kk::mqtt_pass, "Password");
    b.Input(kk::mqtt_device_id, "Device ID");
    b.Button(SH("mqtt_save"), "Save");
    SUB_BUILD_END

    SUB_BUILD_BEGIN
    sets::Menu m(b, "CO2");
    b.Number(kk::co2_alarm_lvl, "Alarm value", nullptr, 0, 8000);
    b.Select(kk::co2_scale_type, "Scale type", co2_scale_types);
    b.Button(SH("co2_save"), "Save");

    sets::Group g(b, "Calibration");
    if (b.beginButtons())
    {
        b.Button(SH("co2_calibrate_run"), "Run", sets::Colors::Green);
        b.Button(SH("co2_calibrate_stop"), "Stop", sets::Colors::Red);
        b.endButtons();
    }
    SUB_BUILD_END

    SUB_BUILD_BEGIN
    sets::Menu m(b, "System");
    b.Switch(kk::rgb_enabled, "RGB Enabled");
    b.Switch(kk::use_dark_theme, "Use dark theme");
    b.Select(kk::rotate_display, "Rotate display", display_rotate_values);
    b.Select(kk::log_lvl, "Log", log_levels);
    b.Button(SH("common_save"), "Save");
    b.Log(H(log), webLogger);
    if (b.Button(SH("update_fw"), "Update firmware") || b.Confirm("update"_h))
    {
        if (_ota)
        {
            LOG_INFO("ota update start");
            _ota->update(true);
        }
    }
    SUB_BUILD_END

    SUB_BUILD_BEGIN
    b.HTML("", R"(<a href="https://github.com/WildEgor/AirQualityMonitor/blob/master/docs/en/UserManual.md">User Manual</a>)");
    SUB_BUILD_END

    SUB_BUILD_BEGIN
    if (b.build.isAction())
    {
        switch (b.build.id)
        {
        case SH("wifi_save"):
            LOG_DEBUG("wifi_save pressed");

            if (_db && _db->update() && _wifi_conn)
            {
                _wifi_conn->connect();
                return;
            }

            break;

        case SH("mqtt_save"):
            LOG_DEBUG("mqtt_save pressed");

            if (_db && _db->update() && _mqtt_conn)
            {
                _mqtt_conn->setDeviceID((*_db)[kk::mqtt_device_id].toString());
                _mqtt_conn->connect();
                return;
            }

            break;

        case SH("co2_save"):
            LOG_DEBUG("co2_save pressed");

            if (_db && _db->update())
            {
                if (_display)
                {
                    _display->forceRender();
                }
                return;
            }

            break;

        case SH("co2_calibrate_run"):
            LOG_DEBUG("co2_calibrate_run pressed");

            _co2_sensor->startCalibration();
            break;

        case SH("co2_calibrate_stop"):
            LOG_DEBUG("co2_calibrate_stop pressed");

            _co2_sensor->forceStopCalibration();
            break;

        case SH("common_save"):
            LOG_DEBUG("common_save pressed");

            if (_db && _db->update())
            {
                SET_LOG_LEVEL((*_db)[kk::log_lvl].toString());

                if (_rgb_controller)
                {
                    _rgb_controller->toggle((*_db)[kk::rgb_enabled].toBool());
                }

                if (_display)
                {
                    _display->setTheme((*_db)[kk::use_dark_theme].toBool());
                    _display->setRotation((*_db)[kk::rotate_display].toString());
                }

                return;
            }

            break;
        }
    }
    SUB_BUILD_END
}
