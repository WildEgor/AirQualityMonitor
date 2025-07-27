#include "Arduino.h"
#include <Looper.h>

#include "db/settings_db.h"
#include "model/co2_data.h"
#include "configs/config.h"
#include "connections/mqtt_conn.h"
#include "connections/wifi_conn.h"
#include "connections/wifi_connector_adapter.cpp"
#include "sensors/sensor_base.h"
#include "sensors/co2_base.h"
#include "sensors/ccs811.h"
#include "sensors/ens160.h"
#include "sensors/tph.h"
#include "hmi/display.h"
#include "hmi/web.h"
#include "controllers/rgb.h"
#include "services/logger.h"
#include "services/publisher.h"
#include "services/ota.h"

/**
 * @brief Initialization of all main components: logging, database, WiFi and MQTT connections, data publishing, display, RGB, and web interface
 */
void setup()
{
  /**
   * @note Logging initialization
   */
  Serial.begin(SERIAL_SPEED);
  SET_LOG_LEVEL(APP_LOG_LEVEL);
  LOG_INFO("init...");

  /**
   * @note Database initialization
   */
  SettingsDB *sdb = new SettingsDB();

  /**
   * @note WiFi connection initialization
   */
  WiFiAdapter *wifia = new WiFiConnectorAdapter(
      WIFI_AP_NAME,
      WIFI_AP_PASS,
      WIFI_CONN_RETRY_TIMEOUT,
      false);
  WiFiConn *wifi = new WiFiConn(*sdb, *wifia);

  /**
   * @note OTA firmware update initialization
   */
  OTA *ota = new OTA(*wifi);

  /**
   * @note MQTT connection initialization
   */
  MQTTConn *mqtt = new MQTTConn(*sdb, *wifi);

  /**
   * @note Sensors initialization
   */
  CO2SensorBase *co2 = nullptr;

  CCS811_CO2Sensor *ccs811 = new CCS811_CO2Sensor(SEC_30);
  ENS160_CO2Sensor *ens160 = new ENS160_CO2Sensor(SEC_30);

  if (ccs811->begin())
  {
    co2 = ccs811;
    LOG_INFO("CCS811 setup success");
  }
  else if (ens160->begin())
  {
    co2 = ens160;
    LOG_INFO("ENS160 setup success");
  }
  else
  {
    LOG_ERROR("co2 sensor not working");
  }

  TPHSensor *tph = new TPHSensor(*sdb, SEC_30);

  /**
   * @note Enable test mode for sensors (data emulation)
   */
#ifdef ENABLE_TEST
  co2->enableTest();
  tph->enableTest();
#endif

/**
 * @note Disable sending to MQTT to prevent broke data
 */
#ifndef ENABLE_TEST
  /**
   * @note Configure CO2 value publishing to topic [device_id]/co2
   */
  MQTTPublisher *co2p = new MQTTPublisher(SEC_30, *mqtt, MQTT_DEFAULT_CO2_TOPIC);
  co2p->setValueCb([co2]() -> float
                   { return co2->getCO2(); });

  /**
   * @note Configure TVOC value publishing to topic [device_id]/tvoc
   */
  MQTTPublisher *tvocp = new MQTTPublisher(SEC_30, *mqtt, MQTT_DEFAULT_TVOC_TOPIC);
  tvocp->setValueCb([co2]() -> float
                    { return co2->getTVOC(); });

  /**
   * @note Configure temperature publishing to topic [device_id]/temp
   */
  MQTTPublisher *tempp = new MQTTPublisher(SEC_30, *mqtt, MQTT_DEFAULT_TEMP_TOPIC);
  tempp->setValueCb([tph]() -> float
                    { return tph->getTemperature(); });

  /**
   * @note Configure pressure publishing to topic [device_id]/pressure
   */
  MQTTPublisher *pp = new MQTTPublisher(SEC_30, *mqtt, MQTT_DEFAULT_PRESSURE_TOPIC);
  pp->setValueCb([tph]() -> float
                 { return tph->getPressure(); });

  /**
   * @note Configure humidity publishing to topic [device_id]/humidity
   */
  MQTTPublisher *hp = new MQTTPublisher(SEC_30, *mqtt, MQTT_DEFAULT_HUMIDITY_TOPIC);
  hp->setValueCb([tph]() -> float
                 { return tph->getHumidity(); });
#endif

  /**
   * @note Display initialization
   */
  Display *display = new Display(
      SEC_1,
      *sdb,
      *co2,
      *tph,
      *wifi,
      *mqtt,
      *ota);

  /**
   * @note RGB controller initialization for CO2 level visualization
   */
  RGBController *rgb = new RGBController(MS_500, *sdb);
  rgb->setUpdaterCb([co2]() -> float
                    { return co2->getCO2(); });

  /**
   * @note Web interface initialization
   */
  WebPanel *wp = new WebPanel(
      *sdb,
      *wifi,
      *ota,
      *mqtt,
      *rgb,
      *display,
      *co2,
      *tph);

  LOG_INFO("init ok!");
}

/**
 * @brief Main loop. Handles all tickers and timers.
 */
void loop()
{
  Looper.loop();
}