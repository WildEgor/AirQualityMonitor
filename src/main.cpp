#include "Arduino.h"
#include <Looper.h>

#include "db/settings_db.h"
#include "model/co2_data.h"
#include "configs/config.h"
#include "configs/settings.h"
#include "connections/mqtt_conn.h"
#include "connections/wifi_conn.h"
#include "connections/wifi_connector_adapter.cpp"
#include "sensors/sensor_base.h"
#include "sensors/co2.h"
#include "sensors/tp.h"
#include "hmi/hmi.h"
#include "controllers/rgb.h"
#include "services/logger.h"
#include "services/publisher.h"

void setup() {
  Serial.begin(115200);
  
  SET_LOG_LEVEL(APP_LOG_LEVEL);
  LOG_INFO("init...");

  SettingsDB* sdb = new SettingsDB();

  WiFiAdapter* wifia = new WiFiConnectorAdapter(
    WIFI_AP_NAME, 
    WIFI_AP_PASS, 
    WIFI_CONN_RETRY_TIMEOUT, 
    false
  );
  WiFiConn* wifi = new WiFiConn(*sdb, *wifia);

  MQTTConn* mqtt = new MQTTConn(*sdb, *wifi);

  CO2Sensor* co2 = new CO2Sensor(SEC_30);
  TPSensor* tp = new TPSensor(SEC_30);

#ifdef ENABLE_TEST
  co2->enableTest();
  tp->enableTest();
#endif

  MQTTPublisher* co2p = new MQTTPublisher(SEC_30, *mqtt, MQTT_DEFAULT_CO2_TOPIC);
  co2p->setValueCb([co2]() -> float {
    return co2->getCO2();
  });

  MQTTPublisher* tvocp = new MQTTPublisher(SEC_30, *mqtt, MQTT_DEFAULT_TVOC_TOPIC);
  tvocp->setValueCb([co2]() -> float {
    return co2->getTVOC();
  });

  MQTTPublisher* tempp = new MQTTPublisher(SEC_30, *mqtt, MQTT_DEFAULT_TEMP_TOPIC);
  tempp->setValueCb([tp]() -> float {
    return tp->getTemperature();
  });

  MQTTPublisher* pp = new MQTTPublisher(SEC_30, *mqtt, MQTT_DEFAULT_PRESSURE_TOPIC);
  pp->setValueCb([tp]() -> float {
    return tp->getPressure();
  });

  HMI* hmi = new HMI(SEC_1, *sdb, *co2, *tp, *wifi);

  RGBController* rgb = new RGBController(SEC_1, *sdb);
  rgb->setUpdaterCb([co2]() -> float {
    return co2->getCO2();
  });

  Settings* sett = new Settings(*sdb, *wifi, *mqtt, *rgb, *hmi);

  LOG_INFO("init ok!");
}

void loop() {
  Looper.loop();
}