#include "Arduino.h"
#include <Looper.h>

#include "db/settings_db.h"
#include "configs/settings.h"
#include "connections/wifi_conn.h"
#include "connections/mqtt_conn.h"
#include "sensors/co2.h"
#include "sensors/sensor_base.h"
#include "hmi/display.h"
#include "controllers/rgb.h"
#include "model/co2_data.h"

void setup() {
  Serial.begin(115200);
  delay(SEC_1);

  Serial.println("setup...");

  SettingsDB* sdb = new SettingsDB();
  sdb->setup();
  sdb->addLoop();

  WiFiConn* wifi = new WiFiConn(*sdb);
  wifi->setup();
  wifi->addLoop();

  MQTTConn* mqtt = new MQTTConn(*sdb, *wifi);
  mqtt->setup();
  mqtt->addLoop();

  CO2Sensor* co2 = new CO2Sensor(SEC_5);
  co2->setup();
  co2->addLoop();

  CO2Publisher* co2p = new CO2Publisher(SEC_5, *co2, *mqtt);
  co2p->addLoop();

  SensorContainer* sensors = new SensorContainer();
  sensors->addSensor(co2->getType(), co2);

  // TODO: add show ip to connect
  Display* display = new Display(MS_100, *sdb, *co2, *wifi);
  display->setup();
  display->addLoop();

  RGBController* rgb = new RGBController(SEC_1, *sdb);
  rgb->setup();
  rgb->setUpdaterCb([co2]() -> uint16_t {
    return co2->getCO2();
  });
  rgb->addLoop();

  Settings* sett = new Settings(*sdb, *wifi, *mqtt, *sensors, *rgb);
  sett->setup();
  sett->addLoop();
}

void loop() {
  Looper.loop();
}