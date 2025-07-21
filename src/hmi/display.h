#pragma once
#include <Looper.h>
#include "SPI.h"
#include <TFT_eSPI.h>

#include "model/display_data.h"
#include "widgets/meter.h"
#include "configs/config.h"
#include "sensors/co2.h"
#include "sensors/tph.h"
#include "connections/wifi_conn.h"
#include "connections/mqtt_conn.h"
#include "services/ota.h"

#define LOG_COMPONENT "Display"
#include "services/logger.h"

/**
 * @name Display
 * @details Class for managing the TFT display, rendering widgets and sensor data
 */
class Display : public LoopTimerBase
{
public:
    /**
     * @name Display
     * @param ms - update interval in milliseconds
     * @param settingsDb - reference to settings database
     * @param co2_sensor - reference to CO2 sensor
     * @param tph_sensor - reference to temperature/pressure sensor
     * @param wifiConn - reference to WiFi connection
     * @param mqttConn - reference to MQTT connection
     * @param ota - reference to OTA update service
     * @details Display class constructor, initializes display and widgets
     */
    Display(
        uint32_t ms,
        SettingsDB &settingsDb,
        CO2Sensor &co2_sensor,
        TPHSensor &tph_sensor,
        WiFiConn &wifiConn,
        MQTTConn &mqttConn,
        OTA &ota)
        : LoopTimerBase(ms),
          _db(&settingsDb.db()),
          _co2_sensor(co2_sensor),
          _tph_sensor(tph_sensor),
          _co2_meter(nullptr),
          _co2_scale(&CO2Scale::getInstance()),
          _wifi(&wifiConn),
          _mqtt(&mqttConn),
          _ota(&ota)
    {
        _tft_rotate = (*_db)[kk::rotation_display].toInt();
        _force_redraw = true;
        _state.dark_theme = (*_db)[kk::use_dark_theme].toBool();
        _state.last_co2_value = -1;
        _state.last_wifi_state = false;
        _state.last_mqtt_state = false;
        _state.last_co2_sensor_state = false;
        _state.last_render_time = 0;
        _state.last_fw_ver = _ota->version();

        LOG_INFO("init tft...");

        _tft.init();
        _tft.setRotation(_tft_rotate);
        _init_theme(true);
        LOG_INFO("init tft ok!");

        LOG_INFO("init widgets...");
        _co2_meter = MeterWidget(&_tft);

        _co2_scale->init(_db);
        uint16_t rs, re, os, oe, ys, ye, gs, ge;
        _co2_scale->getScale(rs, re, os, oe, ys, ye, gs, ge);
        _co2_meter.setZones(rs, re, os, oe, ys, ye, gs, ge);
        _co2_meter.setTheme(_state.dark_theme);
        _co2_meter.analogMeter(0, 0, _co2_scale->getHumanMax(), "CO2", "", "", "", "", "");
        LOG_INFO("init widgets ok!");

        _render();
        this->addLoop();
    }

    /**
     * @name exec
     * @details Main display update loop, triggers rendering
     */
    void exec()
    {
        _render();
    }

    /**
     * @name setTheme
     * @param dark - true for dark theme, false for light theme
     * @details Change display theme and force redraw
     */
    void setTheme(bool dark)
    {
        if (_state.dark_theme == dark)
            return;

        _state.dark_theme = dark;
        _init_theme(true);

        _co2_meter.setTheme(dark);
        uint16_t rs, re, os, oe, ys, ye, gs, ge;
        _co2_scale->getScale(rs, re, os, oe, ys, ye, gs, ge);
        _co2_meter.setZones(rs, re, os, oe, ys, ye, gs, ge);
        _co2_meter.analogMeter(0, 0, _co2_scale->getHumanMax(), "CO2", "", "", "", "", "");

        _force_redraw = true;
        _render();
    }

    /**
     * @name moveRotation
     * @details Change display rotation
     */
    void moveRotation()
    {
        _tft_rotate += 1;
        if (_tft_rotate > 3)
            _tft_rotate = TFT_ROTATION_360;

        (*_db)[kk::rotation_display] = _tft_rotate;

        _tft.setRotation(_tft_rotate);

        if (_state.dark_theme)
        {
            _tft.fillScreen(TFT_BLACK);
        }
        else
        {
            _tft.fillScreen(TFT_WHITE);
        }

        _co2_meter.setTheme(_state.dark_theme);
        uint16_t rs, re, os, oe, ys, ye, gs, ge;
        _co2_scale->getScale(rs, re, os, oe, ys, ye, gs, ge);
        _co2_meter.setZones(rs, re, os, oe, ys, ye, gs, ge);
        _co2_meter.analogMeter(0, 0, _co2_scale->getHumanMax(), "CO2", "", "", "", "", "");

        _force_redraw = true;
        _render();
    }

    /**
     * @name forceRender
     * @details Re-render display data
     */
    void forceRender()
    {
        _co2_meter.setTheme(_state.dark_theme);
        uint16_t rs, re, os, oe, ys, ye, gs, ge;
        _co2_scale->getScale(rs, re, os, oe, ys, ye, gs, ge);
        _co2_meter.setZones(rs, re, os, oe, ys, ye, gs, ge);
        _co2_meter.analogMeter(0, 0, _co2_scale->getHumanMax(), "CO2", "", "", "", "", "");

        _force_redraw = true;
        _render();
    }

private:
    /**
     * @name _tft
     * @details TFT display driver instance
     */
    TFT_eSPI _tft = TFT_eSPI();
    /**
     * @name _db
     * @details Pointer to settings database
     */
    GyverDBFile *_db;
    /**
     * @name _co2_meter
     * @details CO2 meter widget instance
     */
    MeterWidget _co2_meter;
    /**
     * @name _co2_sensor
     * @details Reference to CO2 sensor
     */
    CO2Sensor &_co2_sensor;
    /**
     * @name _co2_scale
     * @details Pointer to CO2 scale instance
     */
    CO2Scale *_co2_scale;
    /**
     * @name _tph_sensor
     * @details Reference to temperature/pressure sensor
     */
    TPHSensor &_tph_sensor;
    /**
     * @name _wifi
     * @details Pointer to WiFi connection
     */
    WiFiConn *_wifi;
    /**
     * @name _mqtt
     * @details Pointer to MQTT connection
     */
    MQTTConn *_mqtt;
    /**
     * @name _ota
     * @details Pointer to OTA update service
     */
    OTA *_ota;
    /**
     * @name _state
     * @details Display state structure
     */
    DisplayState _state;
    /**
     * @name _force_redraw
     * @details Flag to force display redraw
     */
    bool _force_redraw = false;
    /**
     * @name _tft_rotate
     * @details Set display orientation
     */
    int _tft_rotate = TFT_ROTATION_0;

    /**
     * @name _render
     * @details Render all display widgets and info
     */
    void _render()
    {
        if (!_should_render())
        {
            return;
        }

        LOG_DEBUG("render started...");
        _print_wifi_info();
        _print_mqtt_info();
        _print_gauge();
        _print_sensor_state();
        _print_fw_version();
        LOG_DEBUG("rendered ok!");

        _force_redraw = false;
    }

    /**
     * @name _should_render
     * @details Check if display needs to be updated
     * @return bool - true if update is needed
     */
    bool _should_render()
    {
        if (_force_redraw)
        {
            return true;
        }

        bool current_wifi_state = _wifi->connected();
        if (current_wifi_state != _state.last_wifi_state)
        {
            _state.last_wifi_state = current_wifi_state;
            return true;
        }

        bool current_mqtt_state = _mqtt->connected();
        if (current_mqtt_state != _state.last_mqtt_state)
        {
            _state.last_mqtt_state = current_mqtt_state;
            return true;
        }

        bool current_co2_sensor_state = _co2_sensor.isCalibrating();
        if (current_co2_sensor_state != _state.last_co2_sensor_state)
        {
            _state.last_co2_sensor_state = current_co2_sensor_state;
            _force_redraw = true;
            return true;
        }

        float current_co2 = _co2_sensor.getCO2();
        if (abs(current_co2 - _state.last_co2_value) > 5.0)
        {
            _state.last_co2_value = current_co2;
            return true;
        }

        float current_temp = _tph_sensor.getTemperature();
        if (abs(current_temp - _state.last_temp_value) > 1.0)
        {
            _state.last_temp_value = current_temp;
            return true;
        }

        float current_pressure = _tph_sensor.getPressure();
        if (abs(current_pressure - _state.last_pressure_value) > 1.0)
        {
            _state.last_pressure_value = current_pressure;
            return true;
        }

        float current_humidity = _tph_sensor.getHumidity();
        if (abs(current_humidity - _state.last_humidity_value) > 5.0)
        {
            _state.last_humidity_value = current_humidity;
            return true;
        }

        String current_fw_ver = _ota->version();
        if (current_fw_ver != _state.last_fw_ver)
        {
            _state.last_fw_ver = current_fw_ver;
            return true;
        }

        bool current_has_updates = _ota->hasUpdate();
        if (current_has_updates != _state.has_updates)
        {
            _state.has_updates = current_has_updates;
            return true;
        }

        if ((millis() - _state.last_render_time) > SEC_5)
        {
            _state.last_render_time = millis();
            _force_redraw = true;
            return true;
        }

        return false;
    }

    /**
     * @name _print_fw_version
     * @details Print firmware version
     */
    void _print_fw_version()
    {
        // show current fw version
        _tft.setCursor(100, 185);
        if (_state.dark_theme)
        {
            _tft.fillRect(100, 185, 60, 10, TFT_BLACK);
        }
        else
        {
            _tft.fillRect(100, 185, 60, 10, TFT_WHITE);
        }
        _tft.setTextColor(TFT_LIGHTGREY);
        _tft.print(F("v "));
        _tft.println(_state.last_fw_ver);

        // show little green round dot as updates notification
        _tft.setCursor(145, 185);
        if (_state.dark_theme)
        {
            if (_state.has_updates)
            {
                _tft.drawSmoothCircle(145, 185, 2, TFT_GREENYELLOW, TFT_BLACK);
            }
            else
            {
                _tft.drawSmoothCircle(145, 185, 2, TFT_BLACK, TFT_BLACK);
            }
        }
        else
        {
            if (_state.has_updates)
            {
                _tft.drawSmoothCircle(145, 185, 2, TFT_GREEN, TFT_WHITE);
            }
            else
            {
                _tft.drawSmoothCircle(145, 185, 2, TFT_WHITE, TFT_WHITE);
            }
        }
    }

    /**
     * @name _print_mqtt_info
     * @details Print MQTT info
     */
    void _print_mqtt_info()
    {
        _tft.setCursor(130, 145);
        if (_state.dark_theme)
        {
            _tft.fillRect(130, 145, 60, 10, TFT_BLACK);
        }
        else
        {
            _tft.fillRect(130, 145, 60, 10, TFT_WHITE);
        }

        if (!_state.last_mqtt_state)
        {
            _tft.setTextColor(TFT_RED);
            _tft.println(F("MQTT"));
            LOG_ERROR("mqtt not connected");
        }
        else
        {
            _tft.setTextColor(TFT_GREEN);
            _tft.println(F("MQTT"));
        }
    }

    /**
     * @name _print_wifi_info
     * @details Print WiFi info and firmware version to display
     */
    void _print_wifi_info()
    {
        _init_theme(false);

        _tft.setCursor(20, 130);
        if (_state.dark_theme)
        {
            _tft.fillRect(20, 130, 200, 10, TFT_BLACK);
        }
        else
        {
            _tft.fillRect(20, 130, 200, 10, TFT_WHITE);
        }

        LOG_DEBUG("admin panel: http://" + _wifi->ip());

        _tft.setTextColor(TFT_LIGHTGREY);
        _tft.print(F("admin panel: http://"));
        _tft.println(_wifi->ip());

        _tft.setCursor(90, 145);
        if (_state.dark_theme)
        {
            _tft.fillRect(90, 145, 60, 10, TFT_BLACK);
        }
        else
        {
            _tft.fillRect(90, 145, 60, 10, TFT_WHITE);
        }

        if (!_state.last_wifi_state)
        {
            _tft.setTextColor(TFT_RED);
            _tft.println(F("Wi-Fi"));
            LOG_ERROR("wifi not connected");
        }
        else
        {
            _tft.setTextColor(TFT_GREEN);
            _tft.println(F("Wi-Fi"));
        }
    }

    /**
     * @name _print_gauge
     * @details Print and update CO2 gauge widget
     */
    void _print_gauge()
    {
        if (!_co2_sensor.isInitialized())
            return;

        float value = static_cast<float>(_co2_sensor.getCO2());
        if (value > _co2_scale->getHumanMax())
        {
            value = _co2_scale->getHumanMax();
        }

        _init_theme(false);

        LOG_DEBUG("update gauge value: " + String(value));
        _tft.setCursor(0, 0);
        _co2_meter.updateNeedle(value, 10);
    }

    /**
     * @name _print_sensor_state
     * @details Print sensor state (e.g. calibration) to display
     */
    void _print_sensor_state()
    {
        _init_theme(false);

        _tft.setCursor(90, 165);
        if (_state.dark_theme)
        {
            _tft.fillRect(90, 165, 80, 10, TFT_BLACK);
        }
        else
        {
            _tft.fillRect(90, 165, 80, 10, TFT_WHITE);
        }

        _tft.setTextColor(TFT_CYAN);
        if (_state.last_co2_sensor_state)
        {
            _tft.println(F("CALIBRATION"));
        }
        else
        {
            if (_state.dark_theme)
            {
                _tft.fillRect(90, 165, 80, 10, TFT_BLACK);
            }
            else
            {
                _tft.fillRect(90, 165, 80, 10, TFT_WHITE);
            }
        }
    }

    /**
     * @name _init_theme
     * @param fill - whether to fill the background
     * @details Initialize display theme (dark/light)
     */
    void _init_theme(bool fill)
    {
        _tft.setTextSize(1);

        if (_state.dark_theme)
        {
            if (fill)
                _tft.fillScreen(TFT_BLACK);
            _tft.setTextColor(TFT_WHITE);
            return;
        }

        if (fill)
            _tft.fillScreen(TFT_WHITE);
        _tft.setTextColor(TFT_BLACK);
    }
};
