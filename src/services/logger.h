#pragma once
#include <Arduino.h>
#include "configs/config.h"

enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR
};

class Logger {
public:
    static Logger& getInstance();
    
    void setLevel(const String& level);
    void log(LogLevel level, const char* component, const String& message);
    
private:
    Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    LogLevel _current_level;
};

#ifndef LOG_COMPONENT
#define LOG_COMPONENT APP_NAME
#endif

#define LOGGER Logger::getInstance()
#define LOG_DEBUG(msg) LOGGER.log(LogLevel::DEBUG, LOG_COMPONENT, msg)
#define LOG_INFO(msg) LOGGER.log(LogLevel::INFO, LOG_COMPONENT, msg)
#define LOG_WARN(msg) LOGGER.log(LogLevel::WARN, LOG_COMPONENT, msg)
#define LOG_ERROR(msg) LOGGER.log(LogLevel::ERROR, LOG_COMPONENT, msg)
#define SET_LOG_LEVEL(level) LOGGER.setLevel(level)