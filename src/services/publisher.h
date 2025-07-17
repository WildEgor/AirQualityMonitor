#pragma once
#include <Arduino.h>
#include "Looper.h"

#include "connections/mqtt_conn.h"

/**
 * @name MQTTPublisher
 * @details Class for periodic publishing of values to an MQTT topic
 */
class MQTTPublisher : public LoopTimerBase
{
public:
    /**
     * @brief Callback to get the value for publishing
     */
    using ValueCallback = std::function<float()>;

    /**
     * @brief Constructor
     * @param ms Publish interval in milliseconds
     * @param mqtt Reference to MQTTConn object
     * @param topic MQTT topic for publishing (default is empty)
     */
    MQTTPublisher(uint32_t ms, MQTTConn &mqtt, const String &topic = "")
        : LoopTimerBase(ms), _mqtt(mqtt), _enabled(true), _topic(topic)
    {
        this->addLoop();
    }

    /**
     * @brief Main publish loop
     */
    void exec() override
    {
        if (!_enabled || !_mqtt.connected() || !_cb)
            return;
        publish();
    }

    /**
     * @brief Set MQTT topic
     * @param topic New topic
     */
    void setTopic(const String &topic)
    {
        if (!topic.isEmpty())
            _topic = topic;
    }

    /**
     * @brief Set callback to get the value
     * @param cb Callback function
     */
    void setValueCb(ValueCallback cb)
    {
        _cb = cb;
    }

    /**
     * @brief Enable publishing
     */
    void enable() { _enabled = true; }
    /**
     * @brief Disable publishing
     */
    void disable() { _enabled = false; }

private:
    MQTTConn &_mqtt;   ///< Reference to MQTT connection
    bool _enabled;     ///< Publishing enabled flag
    String _topic;     ///< MQTT topic
    ValueCallback _cb; ///< Callback to get the value

    /**
     * @brief Publish value to MQTT
     */
    void publish()
    {
        if (_topic.isEmpty())
        {
            return;
        }

        float value = _cb();
        String payload = String(value, 2);
        _mqtt.publish(_topic, payload);
    }
};
