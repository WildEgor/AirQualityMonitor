#pragma once
#include <Arduino.h>

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    static Logger& getInstance();
    
    void setLevel(const String& level);
    void setComponent(const char* component);

    void log(LogLevel level, const char* component, const String& message);

    void debug(const String& message);
    void info(const String& message);
    void warning(const String& message);
    void error(const String& message);

private:
    Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    LogLevel _current_level;
    const char* _current_component;
};

#define LOGGER Logger::getInstance()

#define LOG_DEBUG(msg) LOGGER.debug(msg)
#define LOG_INFO(msg) LOGGER.info(msg)
#define LOG_WARNING(msg) LOGGER.warning(msg)
#define LOG_ERROR(msg) LOGGER.error(msg)

#define SET_LOG_LEVEL(level) LOGGER.setLevel(level)
#define SET_LOG_COMPONENT(component) LOGGER.setComponent(component) 