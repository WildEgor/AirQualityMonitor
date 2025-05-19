#include "Arduino.h"
#include <Looper.h>
#include "db.h"
#include "settings.h"
#include "wifi_checker.h"

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("setup...");

  // Initialize database first
  SettingsDB* sdb = new SettingsDB();
  sdb->setup();
  sdb->addLoop();

  // Initialize WiFi second
  WiFiChecker* wifi = new WiFiChecker();
  wifi->setup(*sdb);
  wifi->addLoop();

  // Wait for WiFi to initialize
  delay(1000);

  // Initialize settings last, after WiFi is ready
  Settings* sett = new Settings();
  sett->setup(*sdb, *wifi);
  sett->addLoop();
}

void loop() {
  Looper.loop();
}