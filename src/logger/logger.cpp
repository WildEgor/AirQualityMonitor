#include "logger.h"

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() : _current_level(LogLevel::DEBUG), _current_component("") {}

void Logger::setLevel(const String& level) {
    String levelUpper = level;
    levelUpper.toUpperCase();
    
    if (levelUpper == "DEBUG") {
        _current_level = LogLevel::DEBUG;
    } else if (levelUpper == "INFO") {
        _current_level = LogLevel::INFO;
    } else if (levelUpper == "WARNING" || levelUpper == "WARN") {
        _current_level = LogLevel::WARNING;
    } else if (levelUpper == "ERROR") {
        _current_level = LogLevel::ERROR;
    } else {
        LOG_ERROR("Invalid log level: " + level);
    }
}

void Logger::setComponent(const char* component) {
    _current_component = component;
}

void Logger::log(LogLevel level, const char* component, const String& message) {
    if (level < _current_level) return;

    String level_str;
    switch (level) {
        case LogLevel::DEBUG:
            level_str = "DEBUG";
            break;
        case LogLevel::INFO:
            level_str = "INFO";
            break;
        case LogLevel::WARNING:
            level_str = "WARN";
            break;
        case LogLevel::ERROR:
            level_str = "ERROR";
            break;
    }

    Serial.println(String("[") + level_str + "][" + component + "] " + message);
}

void Logger::debug(const String& message) {
    log(LogLevel::DEBUG, _current_component, message);
}

void Logger::info(const String& message) {
    log(LogLevel::INFO, _current_component, message);
}

void Logger::warning(const String& message) {
    log(LogLevel::WARNING, _current_component, message);
}

void Logger::error(const String& message) {
    log(LogLevel::ERROR, _current_component, message);
} 