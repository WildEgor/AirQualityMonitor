#pragma once
#include <Arduino.h>
#include <Looper.h>
#include <map>
#include <string>

class SensorBase : public LoopTimerBase {
public:
    SensorBase(uint32_t ms) : LoopTimerBase(ms), _is_initialized(false) {}

    virtual void setup() = 0;
    virtual void exec() override = 0;
    virtual bool isInitialized() const { return _is_initialized; }
    virtual void copyState(const SensorBase& other) = 0;

    virtual void updateInterval(uint32_t new_ms) {
        restart(new_ms);
    }

    void setInterval(uint32_t new_ms) {
        restart(new_ms);
    }
    virtual const char* getType() const = 0;

protected:
    bool _is_initialized;
};

class SensorContainer {
public:
    SensorContainer() {}

    void addSensor(const std::string& name, SensorBase* sensor) {
        if (sensor) {
            sensor->setup();
            sensor->addLoop();
            _sensors[name] = sensor;
        }
    }

    bool updateInterval(const std::string& name, uint32_t new_interval) {
        auto it = _sensors.find(name);
        if (it == _sensors.end()) return false;

        it->second->removeLoop();
        it->second->setInterval(new_interval);
        it->second->addLoop();

        Serial.printf("sensor %s update interval -> %d\n", name, new_interval);
        
        return true;
    }

    void removeSensor(const std::string& name) {
        auto it = _sensors.find(name);
        if (it != _sensors.end()) {
            it->second->removeLoop();
            delete it->second;
            _sensors.erase(it);
        }
    }

    void clear() {
        for (auto& pair : _sensors) {
            pair.second->removeLoop();
            delete pair.second;
        }
        _sensors.clear();
    }

    ~SensorContainer() {
        clear();
    }

private:
    std::map<std::string, SensorBase*> _sensors;
};