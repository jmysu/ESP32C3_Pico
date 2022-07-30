// Copyright (c) 2021 Cesanta Software Limited
// All rights reserved
//
// This Arduino sketch demonstrates "elk" JavaScript LED toggling on ESP32C3-12F:
//

#include <Arduino.h>
//#include <WiFi.h>
#include "JS.h"
#include <LittleFS.h>

// const char *ssid = "WIFI_NETWORK";
// const char *pass = "WIFI_PASSWORD";

extern void lfsFileRead(fs::FS &fs, const char * path);
extern void lfsJasonRead(fs::FS &fs, const char * path);
bool elkLoadJS(String path);

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("Starting ELK3LFS...");

  //WiFi.begin(ssid, pass);
  //while (WiFi.status() != WL_CONNECTED) Serial.print("."), delay(300);
  //Serial.print("\nWiFi Connected, IP address: ");
  //Serial.println(WiFi.localIP());

  //lfsFileRead(LittleFS, "/init.js");
  //lfsJasonRead(LittleFS, "/config.json");
  elkLoadJS("/init.js");
}

void loop() {
}
