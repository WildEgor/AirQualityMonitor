# User Manual

## Table of Contents  
1. [Introduction](#introduction)  
2. [Device Overview](#device-overview)  
   - 2.1 [Components](#components)  
   - 2.2 [Interface](#interface)  
3. [Getting Started](#getting-started)  
   - 3.1 [Installation](#installation)  
   - 3.2 [First Power-On](#first-power-on)  
   - 3.3 [Connecting to Wi-Fi](#connecting-to-wi-fi)  
4. [Interface Overview](#interface-overview)  
5. [Advanced Settings](#advanced-settings)  

---

## Introduction  
Air Quality Monitor (AQM) is an IoT device designed to measure indoor CO₂/TVOC levels, temperature, pressure, humidity, and transmit real-time data via an RGB LED and MQTT to platforms like Yandex.  

---

## Device Overview  

### Components  
1. **Front Panel**:  
   - 1.28" IPS Round Display  
2. **Rear Panel**:  
   - USB Type-C Port (for power and firmware updates)  
3. **Internal Components**:  
   - CO₂ Sensor (CCS811)  
   - Temperature/Pressure/Humidity Sensor (BME280)  
   - Microcontroller (ESP32 Live Mini)  
   - RGB Indicator (WS2812B)  

### Interface  
- **Display**: Shows CO₂ levels, network status, and calibration info.  
- **Web Panel**: Accessible via device IP for configuration.  

---

## Getting Started  

### Installation  
1. Place the device on a flat surface 1-2 meters above breathing level.  
2. Ensure ventilation holes are unobstructed.  

### First Power-On  
1. Connect the USB cable to a 5V/1A power source.  
2. The screen will display:  
    - CO₂ meter  
    - Admin panel address  
    - Wi-Fi Status (see [Connecting to Wi-Fi](#connecting-to-wi-fi))  
    - MQTT Status (see [MQTT Configuration](#mqtt-configuration))  

<div align="center">
  <img src="../images/first_start.jpg" width="250" alt="First startup screen">
</div>

### Connecting to Wi-Fi  
1. Connect to the device’s access point `AQM_AP`.

<div align="center">
  <img src="../images/aqm_ap.jpg" width="250" alt="AQM access point">
  <p style="text-align: center; font-style: italic;">Device creates an access point</p>
</div>

2. Open a browser to the displayed address (e.g., http://192.168.4.1).  
3. Go to **WIFI** menu, enter credentials, and click **Save**.  

> **Note**: Connection status updates within seconds.

<div align="center">
  <img src="../images/web_ui_wifi_menu.png" width="250" alt="Wi-Fi settings">
</div>

---

## Interface Overview  

### Device Interface  
| Element                   | Description                                                                 |
|---------------------------|-----------------------------------------------------------------------------|
| **CO₂ Meter**             | Color gradient (green, yellow, orange, red)                                |
| **Current CO₂ Value**     | eCO₂ value (400.0–1500.0)                                                  |
| **Web Panel Link**        | URL (e.g., `http://192.168.1.100`)                                         |
| **Network Status**        | Wi-Fi/MQTT connection (green/red)                                       |
| **Calibration Status**    | "CALIBRATION" text in teal                                                 |
| **Firmware Version**      | Version with notification (e.g., `v1.0.0`)                                 |

### Web Panel  
Accessible at `http://[device-address]`:  

1. **Wi-Fi**: Configure SSID and password.  
<div align="center">
  <img src="../images/web_ui_wifi_menu.png" width="250" alt="Wi-Fi menu">
</div>  
2. **MQTT**: Enable/disable, server settings, credentials.  
<div align="center">
  <img src="../images/web_ui_mqtt_menu.png" width="250" alt="MQTT settings">
</div>  
3. **CO₂**: Set alarms, scale type, calibration.  
<div align="center">
  <img src="../images/web_ui_co2_menu.png" width="250" alt="CO₂ settings">
</div>  
4. **System**: Toggle RGB, theme, logging, firmware update.  
<div align="center">
  <img src="../images/web_ui_system_menu.png" width="250" alt="System settings">
</div>  

---

## Advanced Settings  

### MQTT Configuration  
- Set broker details (host, port, credentials).  
- Use unique device ID for topic prefix.  

<div align="center">
  <img src="../images/web_ui_mqtt_menu.png" width="250" alt="MQTT broker settings">
  <p style="text-align: center; font-style: italic;">MQTT broker settings</p>
</div>

### Example Using [wqtt.ru](https://wqtt.ru)  
1. Sign up at [wqtt.ru](https://wqtt.ru).  
2. Add device and sensors with topics `common/aqm/co2`, `tvoc`, `temp`, `pressure` and `humidity`.  

<div align="center">
  <img src="../images/wqtt_settings.png" width="250" alt="wqtt.ru settings">
</div>  

3. Sync with Yandex Smart Home via [integration steps](#yandex-smart-home-integration).  

### Yandex Smart Home Integration  
1. Link [wqtt.ru](https://wqtt.ru) in Yandex app.  

<div align="center">
  <img src="../images/ysh_wqtt.jpg" width="250" alt="wqtt.ru in Yandex">
</div>  

2. Refresh devices and rename AQM.  

<div align="center">
  <img src="../images/ysh_add_aqm.jpg" width="250" alt="Add AQM in Yandex">
</div>  

3. View data and configure alerts.  

<div align="center">
  <img src="../images/ysh_aqm_data.jpg" width="250" alt="AQM data in Yandex">
</div>  

### Sensor Calibration  
1. Place in clean air and run calibration via web panel.  
2. Wait 20 minutes, then stop calibration and reboot.  

<div align="center">
  <img src="../images/calibration.jpg" width="250" alt="Calibration mode">
  <p style="text-align: center; font-style: italic;">Device in calibration mode</p>
</div>

### Firmware Update  
1. Navigate to `System > Update firmware`.  
2. Upload `firmware.bin` from `/bin` folder.  

<div align="center">
  <img src="../images/web_ui_maintenance.png" width="250" alt="Maintenance menu">
  <p style="text-align: center; font-style: italic;">Maintenance menu</p>
</div>