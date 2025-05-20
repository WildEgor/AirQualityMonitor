#include "Arduino.h"
#include <Looper.h>
#include "db.h"
#include "settings.h"
#include "wifi_conn.h"
#include "mqtt_conn.h"
#include "sensors/co2.h"
#include "sensors/sensor_base.h"

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("setup...");

  SettingsDB* sdb = new SettingsDB();
  sdb->setup();
  sdb->addLoop();

  WiFiConn* wifi = new WiFiConn();
  wifi->setup(*sdb);
  wifi->addLoop();

  delay(1000); // hmmm

  MQTTConn* mqtt = new MQTTConn();
  mqtt->setup(*sdb);
  mqtt->addLoop();

  CO2Sensor* co2 = new CO2Sensor(1000);

  SensorContainer* sensors = new SensorContainer();
  sensors->addSensor(co2->getType(), co2);
  
  CO2Publisher* co2p = new CO2Publisher(5000, *co2, *mqtt);
  co2p->addLoop();

  Settings* sett = new Settings();
  sett->setup(*sdb, *wifi, *mqtt, *sensors);
  sett->addLoop();
}

void loop() {
  Looper.loop();
}