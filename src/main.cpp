#include "Arduino.h"
#include <Looper.h>

#include "db/settings_db.h"
#include "configs/config.h"
#include "configs/settings.h"
#include "connections/mqtt_conn.h"
#include "sensors/sensor_base.h"
#include "sensors/co2.h"
#include "sensors/tp.h"
#include "hmi/hmi.h"
#include "controllers/rgb.h"
#include "model/co2_data.h"
#include "connections/wifi_conn.h"
#include "services/logger.h"
#include "services/publisher.h"

// #include "connections/wifi_std_adapter.cpp"
#include "connections/wifi_connector_adapter.cpp"

void setup() {
  Serial.begin(115200);
  
  SET_LOG_LEVEL(APP_LOG_LEVEL);
  LOG_INFO("init...");

  SettingsDB* sdb = new SettingsDB();
  sdb->setup();
  sdb->addLoop();

  // WiFiAdapter* wifia = new WiFiStdAdapter(WIFI_AP_NAME, WIFI_AP_PASS);
  WiFiAdapter* wifia = new WiFiConnectorAdapter(WIFI_AP_NAME, WIFI_AP_PASS);

  WiFiConn* wifi = new WiFiConn(*sdb, *wifia);
  wifi->setup();
  wifi->addLoop();

  MQTTConn* mqtt = new MQTTConn(*sdb, *wifi);
  mqtt->setup();
  mqtt->addLoop();

  CO2Sensor* co2 = new CO2Sensor(SEC_10);
#ifdef ENABLE_TEST
  co2->enableTest();
#endif
  co2->setup();
  co2->addLoop();

  MQTTPublisher* co2p = new MQTTPublisher(SEC_30, *mqtt, MQTT_DEFAULT_CO2_TOPIC);
  co2p->setValueCb([co2]() -> float {
    return co2->getCO2();
  });
  co2p->addLoop();

  MQTTPublisher* tvocp = new MQTTPublisher(SEC_30, *mqtt, MQTT_DEFAULT_TVOC_TOPIC);
  tvocp->setValueCb([co2]() -> float {
    return co2->getTVOC();
  });
  tvocp->addLoop();

  TPSensor* tp = new TPSensor(SEC_10);
#ifdef ENABLE_TEST
  tp->enableTest();
#endif
  tp->setup();
  tp->addLoop();

  MQTTPublisher* tempp = new MQTTPublisher(SEC_30, *mqtt, MQTT_DEFAULT_TEMP_TOPIC);
  tempp->setValueCb([tp]() -> float {
    return tp->getTemperature();
  });
  tempp->addLoop();

  MQTTPublisher* pp = new MQTTPublisher(SEC_30, *mqtt, MQTT_DEFAULT_PRESSURE_TOPIC);
  pp->setValueCb([tp]() -> float {
    return tp->getPressure();
  });
  pp->addLoop();

  HMI* hmi = new HMI(MS_100, *sdb, *co2, *tp, *wifi);
  hmi->setup();
  hmi->addLoop();

  RGBController* rgb = new RGBController(SEC_1, *sdb);
  rgb->setup();
  rgb->setUpdaterCb([co2]() -> float {
    return co2->getCO2();
  });
  rgb->addLoop();

  Settings* sett = new Settings(*sdb, *wifi, *mqtt, *rgb, *hmi);
  sett->setup();
  sett->addLoop();

  LOG_INFO("init ok!");
}

void loop() {
  Looper.loop();
}