#include "logger.h"

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() : _current_level(LogLevel::DEBUG) {}

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
        Serial.println("[ERROR][Logger] Invalid log level: " + level);
    }
}

void Logger::log(LogLevel level, const char* component, const String& message) {
    if (level < _current_level) return;
    
    String level_str;
    String color_code;
    switch (level) {
        case LogLevel::DEBUG:
            level_str = "DEBUG";
            color_code = "\033[36m"; // Cyan
            break;
        case LogLevel::INFO:
            level_str = "INFO";
            color_code = "\033[32m"; // Green
            break;
        case LogLevel::WARNING:
            level_str = "WARN";
            color_code = "\033[33m"; // Yellow
            break;
        case LogLevel::ERROR:
            level_str = "ERROR";
            color_code = "\033[31m"; // Red
            break;
    }
    String reset_code = "\033[0m";
    Serial.println(color_code + "[" + level_str + "][" + component + "] " + message + reset_code);
}