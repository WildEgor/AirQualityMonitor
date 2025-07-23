#pragma once
#include <GyverDBFile.h>
#include <LittleFS.h>
#include <Looper.h>

#include "configs/config.h"

#define LOG_COMPONENT "SettingsDB"
#include "services/logger.h"

/**
 * @enum kk
 * @brief Database keys, used as map keys for settings storage
 */
enum kk : size_t
{
    wifi_ssid,        ///< WiFi SSID
    wifi_pass,        ///< WiFi password
    mqtt_enabled,     ///< MQTT enabled flag
    mqtt_server,      ///< MQTT server address
    mqtt_port,        ///< MQTT server port
    mqtt_username,    ///< MQTT username
    mqtt_pass,        ///< MQTT password
    mqtt_device_id,   ///< MQTT device ID
    co2_scale_type,   ///< CO2 scale type
    co2_alarm_lvl,    ///< CO2 alarm level
    temp_offset,      ///< Temp offset
    rgb_enabled,      ///< RGB enabled flag
    use_dark_theme,   ///< Use dark theme flag
    rotation_display, ///< Rotation display
    log_lvl,           ///< Log level
    cfm_fr             ///< Configrm factory reset
};

/**
 * @var co2_scale_types
 * @brief List of available CO2 scale types for UI
 */
extern String co2_scale_types;
/**
 * @var log_levels
 * @brief List of available log levels for UI
 */
extern String log_levels;

/**
 * @class SettingsDB
 * @brief Implements database logic for application settings
 */
class SettingsDB : public LoopTickerBase
{
public:
    /**
     * @brief Constructor
     */
    SettingsDB();

    /**
     * @brief Initialize database dependencies
     */
    void setup();

    /**
     * @brief Reset db values to factory values and reboot device
     */
    void factory_reset();

    /**
     * @brief Handle database updates
     */
    void exec() override;

    /**
     * @brief Return database instance
     * @return GyverDBFile reference
     */
    GyverDBFile &db();

private:
    GyverDBFile _db; ///< Internal database instance
};
