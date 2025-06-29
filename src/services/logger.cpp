#include "logger.h"

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() : _current_level(LogLevel::DEBUG), _wl(nullptr) {}

void Logger::initWebLogger(sets::Logger& wl) {
    _wl = &wl;
}

void Logger::setLevel(const String& level) {
    String levelUpper = level;
    levelUpper.toUpperCase();
    
    if (levelUpper == "DEBUG" || levelUpper == "0") {
        _current_level = LogLevel::DEBUG;
    } else if (levelUpper == "INFO" || levelUpper == "1") {
        _current_level = LogLevel::INFO;
    } else if (levelUpper == "WARN" || levelUpper == "2") {
        _current_level = LogLevel::WARN;
    } else if (levelUpper == "ERROR" || levelUpper == "3") {
        _current_level = LogLevel::ERROR;
    } else {
        log(LogLevel::ERROR, "Logger", "invalid log level: " + level);
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
        case LogLevel::WARN:
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
    if (_wl) {
        _wl->println("[" + level_str + "][" + component + "] " + message);
    }
}