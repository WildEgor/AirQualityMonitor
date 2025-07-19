#pragma once
#include <Arduino.h>
#include <SettingsGyver.h>

#include "configs/config.h"

/**
 * @enum LogLevel
 * @brief Log level: DEBUG - all logs, INFO - informational, WARN - warnings, ERROR - only critical messages
 */
enum class LogLevel
{
    DEBUG, ///< Debug messages
    INFO,  ///< Informational messages
    WARN,  ///< Warning messages
    ERROR  ///< Error messages
};

/**
 * @class Logger
 * @brief Implements formatting and serial printing for logging
 */
class Logger
{
public:
    /**
     * @brief Get singleton instance of Logger
     * @return Logger&
     */
    static Logger &getInstance();

    /**
     * @brief Set global log level
     * @param level Log level as string
     */
    void setLevel(const String &level);
    /**
     * @brief Print log message with level and component prefix
     * @param level Log level
     * @param component Component name
     * @param message Log message
     */
    void log(LogLevel level, const char *component, const String &message);
    /**
     * @brief Set external web UI logger
     * @param wl Reference to web logger
     */
    void initWebLogger(sets::Logger &wl);

private:
    Logger();
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    sets::Logger *_wl;        ///< Pointer to web logger
    LogLevel _current_level;  ///< Current log level
};

/**
 * @def LOG_COMPONENT
 * @brief Default log component name (application name)
 */
#ifndef LOG_COMPONENT
#define LOG_COMPONENT APP_NAME
#endif

/**
 * @def LOGGER
 * @brief Singleton logger instance
 */
#define LOGGER Logger::getInstance()
/**
 * @def LOG_DEBUG
 * @brief Log debug message
 */
#define LOG_DEBUG(msg) LOGGER.log(LogLevel::DEBUG, LOG_COMPONENT, msg)
/**
 * @def LOG_INFO
 * @brief Log info message
 */
#define LOG_INFO(msg) LOGGER.log(LogLevel::INFO, LOG_COMPONENT, msg)
/**
 * @def LOG_WARN
 * @brief Log warning message
 */
#define LOG_WARN(msg) LOGGER.log(LogLevel::WARN, LOG_COMPONENT, msg)
/**
 * @def LOG_ERROR
 * @brief Log error message
 */
#define LOG_ERROR(msg) LOGGER.log(LogLevel::ERROR, LOG_COMPONENT, msg)
/**
 * @def SET_LOG_LEVEL
 * @brief Set global log level
 */
#define SET_LOG_LEVEL(level) LOGGER.setLevel(level)