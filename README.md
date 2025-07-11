# AirQualityMonitor

<p align="center">
  <img src="./docs/images/showcase.jpg" width="250" title="showcase">
</p>

Air Quality Monitor based on ESP32 and CCS811 sensor

## Features

1. Measures eCO₂ and TVOC levels;  
2. Displays CO₂ levels on a screen and via an RGB LED.
3. Measures temperature and pressure;
4. Publishes the data to MQTT topics;
5. Alert via rgb;
6. Support web ui.

## How to
1. Please edit `.pio/libdeps/mhetesp32devkit/TFT_eSPI/User_Setup_Select.h` uncomment `#include <User_Setups/Setup200_GC9A01.h>` (see `src/configs/config.h`);
2. Rename `src/configs/secrets.example.h` to `src/configs/secrets.h` and edit - use correct wi-fi and mqtt creds; 
3. Optional. Edit `src/configs/config.h`;
4. Upload and enjoy!

## Links

- [Manual](/docs/en/UserManual.md)
- [Wiring Diagram](/resources/) 
- [Source Code](/src/) 
- [3D Printed Case](/resources/3dprint/)  

> WARNING! Reduce the model by 10 times before printing!