#define LOG_COMPONENT "MQTTConn"
#include "services/logger.h"
#include "mqtt_conn.h"

WiFiClient _espClient;
PubSubClient _pub_client(_espClient);

MQTTConn::MQTTConn(SettingsDB& settingsDb, WiFiConn& wifiConn) : LoopTickerBase(), _db(&settingsDb.db()), _wifi(&wifiConn) {
    if (!isEnabled()) return;

    LOG_INFO("init...");

    _device_id = (*_db)[kk::mqtt_device_id].toString();

    _connectToMQTT(
        (*_db)[kk::mqtt_server],
        (*_db)[kk::mqtt_port].toInt16(),
        (*_db)[kk::mqtt_username],
        (*_db)[kk::mqtt_pass]);

    LOG_INFO("init ok!");

    this->addLoop();
    _is_initialized = true;
}

void MQTTConn::exec() {
    if (!isEnabled()) return;

    if (!_wifi->connected()) {
        return;
    }

    bool pub_connected = _pub_client.loop();
    if (!pub_connected) {
        connect();
    }
}

void MQTTConn::connect() {
    if (!isEnabled()) return;

    if (!_wifi->connected()) return;

    if (!connected()) {
        LOG_INFO("connect to server...");

        _connectToMQTT(
            (*_db)[kk::mqtt_server],
            (*_db)[kk::mqtt_port].toInt16(),
            (*_db)[kk::mqtt_username],
            (*_db)[kk::mqtt_pass]);
    }
}

void MQTTConn::publish(const String& topic, const String& payload) {
    if (!isEnabled()) return;
    if (!_wifi->connected()) return;

    LOG_DEBUG("pub to topic: " + _device_id + "/" + topic + " value: " + payload);

    String t = _device_id + "/" + topic;

    bool ok = _pub_client.publish(t.c_str(), payload.c_str(), false);
    if (!ok) {
        LOG_ERROR("publish failed");
    }
}

void MQTTConn::setDeviceID(const String& id) {
    if (id.isEmpty()) return;
    
    _device_id = id;
}

bool MQTTConn::isEnabled() const {
    return (*_db)[kk::mqtt_enabled].toBool();
}

bool MQTTConn::connected() const {
    return _pub_client.connected();
}

bool MQTTConn::isInitialized() const {
    return _is_initialized;
}

void MQTTConn::_connectToMQTT(const String& mqtt_server, uint16_t mqtt_port, const String& mqtt_user, const String& mqtt_password) {
    if (!_wifi->connected()) return;

    if (mqtt_server.isEmpty()) {
        LOG_ERROR("server address is empty");
        return;
    }

    if (mqtt_user.isEmpty() || mqtt_password.isEmpty()) {
        LOG_ERROR("server creds is empty");
        return;
    }

    if (mqtt_port == 0 || mqtt_port > 65535) {
        LOG_ERROR("invalid port");
        return;
    }

    LOG_INFO("connecting to " + mqtt_server);

    _pub_client.setKeepAlive(60);
    _pub_client.setSocketTimeout(30);
    _pub_client.setServer(mqtt_server.c_str(), mqtt_port);

    String client_id = "AQM-" + WiFi.macAddress() + String(random(0xffff), HEX);

    if (client_id.isEmpty()) {
        LOG_ERROR("failed to generate client ID");
        return;
    }

    LOG_DEBUG("client_id: " + client_id);
    LOG_DEBUG("mqtt_user: " + mqtt_user);
    LOG_DEBUG("mqtt_password: " + mqtt_password);
    LOG_DEBUG("mqtt_server: " + mqtt_server);
    LOG_DEBUG("mqtt_port: " + String(mqtt_port));

    const int max_retries = 3;
    for (int retry_count = 0; retry_count < max_retries; retry_count++) {
        LOG_DEBUG("attempting connection client...");

        if (_pub_client.connect(client_id.c_str(), mqtt_user.c_str(), mqtt_password.c_str())) {
            LOG_INFO("connected");
            return;
        }

        LOG_ERROR("failed, rc=" + String(_pub_client.state()) + " try again...");
        delay(1000);
    }

    LOG_ERROR("failed to connect to server after maximum retries");
}
