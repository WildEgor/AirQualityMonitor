# [ENG] Air Quality Monitor (AQM) User Manual  

---

## Table of Contents  
1. [Introduction](#introduction)  
2. [Device Overview](#device-overview)  
   - 2.1 [Physical Components](#physical-components)  
   - 2.2 [Display UI](#display-ui)  
3. [Getting Started](#getting-started)  
   - 3.1 [Unboxing and Setup](#unboxing-and-setup)  
   - 3.2 [Initial Power-On](#initial-power-on)  
   - 3.3 [Connecting to Wi-Fi](#connecting-to-wifi)  
4. [Understanding the UI](#understanding-the-ui)  
   - 4.1 [Device Interface](#device-interface)  
   - 4.2 [Web Admin Panel](#web-admin-panel)  
5. [Advanced Configuration](#advanced-configuration)  
6. [Maintenance & Safety](#maintenance--safety)   

---

## Introduction  
The Air Quality Monitor (AQM) is an IoT-enabled device designed to measure indoor CO₂ levels and provide real-time feedback via visual, haptic (RGB LED), and remote (MQTT) feedback.

---

## Device Overview  
### Physical Components  
1. **Front Panel**:  
   - 1.28" IPS round display  
2. **Rear Panel**:  
   - Micro-USB port (power and firmware update)   
3. **Internal Hardware**:  
   - CO₂ sensor (CCS811)  
   - Microcontroller (ESP32 based) 
   - RGB indicator    

### Display UI  
On startup, the round display shows:  
1. **CO₂ Meter**: Circular gauge with color zones (Green/Yellow/Orange/Red).  
2. **Current CO₂ Value**: Numeric readout (e.g., "850 ppm") in left corner of gauge.  
3. **Network Status**:  
   - "WIFI" (green - connected, red - disconnected)  
4. **Admin panel**: Local IP address (e.g., `http://192.168.1.100`).  
5. **Calibration Status**: "CALIBRATION" label.  
6. **Firmware Version**: Shown at the bottom (e.g., "v1.0.0").  

---

## Getting Started  
### Unboxing and Setup  
1. **Package Contents**:  
   - AQM Device  
   - Micro-USB cable  
   - Quick Start Guide  
   - Mounting parts  

2. **Assembly**:  
   - Place the device on a flat surface (table or wall), 1-2 meters from breathing height.  
   - Ensure there are no obstructions to air vents.  

### Initial Power-On  
1. Plug in the Micro-USB cable to a 5V/1A power adapter.  
2. **First Boot Sequence**:  
   - The display will show:  
      - Step 1: CO₂ sensor meter  
      - Step 2: Wi-Fi status (see [Section 3.3](#how-to-connect-the-device-to-wi-fi))  

### How to Connect the Device to Wi-Fi  
1. Connect to the device’s hotspot (SSID: `AQM-AP`).  
2. Open a web browser and enter `http://192.168.4.1`.  
3. Select the **WIFI** menu item and input your Wi-Fi credentials. Click **Save** button.  
**Note**: After saving, the device will reboot and attempt to join the network. Status updates will appear in several seconds.  

---

## Understanding the UI  
### Device Interface  
| Element          | Description                                                                 |  
|-------------------|-----------------------------------------------------------------------------|  
| **CO₂ meter**     | Visual gauge with colors: Green, Yellow, Orange, Red                       |  
| **CO₂ value**     | Current eCO₂ value in format (e.g., "850 ppm")                              |  
| **Admin panel link** | Link to web admin panel (e.g., `http://192.168.1.100`)                   |  
| **WIFI status**   | Internet connection status (green - connected, red - disconnected)         |  
| **Calibration status** | Sensor calibration label (e.g., "CALIBRATION")                         |  
| **Firmware version** | Firmware version number (e.g., "v1.0.0")                                 |  

### Web Admin Panel  
Accessible via `http://[device-ip]`, the web UI includes:   
1. **Wi-Fi Settings**:  
   - SSID (network name) and password.  
2. **MQTT Configuration**:  
   - **Broker details**:  
     - **Enable**: Enable publishing to topics.  
     - **Host**: Server host.  
     - **Port**: Server port.  
     - **Username**: Account username.  
     - **Password**: Account password.  
     - **Device ID**: Used for unique ID for server and topic prefix.  
3. **CO₂ Settings**:  
   - **Alarm value**: Set alarm thresholds.  
   - **Scale types**: Choose between DEFAULT (4 colors) or EASY (3 colors) gauge.  
   - **Calibration**: Buttons to start/stop calibration.  
4. **System Settings**:  
   - **Use dark theme**: Toggle between light and dark themes.  
   - **Update firmware**: Download and install the latest firmware version.   

---

## Advanced Configuration  
### Custom MQTT  
- Choose your own host, port, and credentials for connection.  
- Specify a device ID to prevent topic duplicates.  

### Theme Customization  
- Choose **Dark Mode** in **System > Use dark theme**.  

---

## Maintenance & Safety  
- **Sensor Calibration**:  
  - The sensor can perform auto-calibration every 24 hours after start. However, during the first week of operation, it is recommended to save a new baseline every 24 hours. After one week of operation, it can be saved every 1-28 days. Periodically re-calibrate the sensor, e.g., every 7 days.  
  - To calibrate:  
    - Put the sensor in clean air conditions.  
    - Press the `run` button, and ensure the "CALIBRATION" label appears.  
    - Wait approximately 20 minutes and press the `stop` button.  
    - The "CALIBRATION" label should disappear.  
    - Power off and then power on the device. If any issues arise (e.g., power corruption), the sensor will still read and save the calibrated value. You can try recalibrating at any time.  

- **Firmware Updates**:  
  - In the menu, select **System** and press `Update firmware`.  
  - Wait a few minutes. The device will restart, and the new version will be updated in the header section.  

- **Warnings**:  
  - Do not block air vents.  
  - Avoid exposing the device to direct sunlight or water.