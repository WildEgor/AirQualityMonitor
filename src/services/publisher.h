#pragma once
#include <Arduino.h>
#include "Looper.h"

#include "connections/mqtt_conn.h"

class MQTTPublisher : public LoopTimerBase {
public:
    using ValueCallback = std::function<float()>;

    MQTTPublisher(uint32_t ms, MQTTConn& mqtt, const String& topic = "")
        : LoopTimerBase(ms), _mqtt(mqtt), _enabled(true), _topic(topic) {
            this->addLoop();
        }

    void exec() override {
        if (!_enabled || !_mqtt.connected() || !_cb) return;
        publish();
    }

    void setTopic(const String& topic) {
        if (!topic.isEmpty()) _topic = topic;
    }

    void setValueCb(ValueCallback cb) {
        _cb = cb;
    }

    void enable() { _enabled = true; }
    void disable() { _enabled = false; }

private:
    MQTTConn& _mqtt;
    bool _enabled;
    String _topic;
    ValueCallback _cb;

    void publish() {
        if (_topic.isEmpty()) {
            return;
        }

        float value = _cb();
        String payload = String(value, 2);
        _mqtt.publish(_topic, payload);
    }
};
