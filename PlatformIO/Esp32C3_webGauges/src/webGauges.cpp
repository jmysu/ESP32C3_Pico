
/*********
  Rui Santos
  Complete instructions at https://RandomNerdTutorials.com/esp32-web-server-gauges/

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJSON.h>

//#include "SPIFFS.h"
#include <LittleFS.h>
#define SPIFFS LittleFS

//#include <Adafruit_Sensor.h>

// Replace with your network credentials
const char* ssid = "BreezeHillxxx";
const char* password = "093290xxxx";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create an Event Source on /events
AsyncEventSource events("/events");

 

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 3000;

String getSensorReadings() {
  //readings["donut"] =  random(100);
  //String jsonString = JSON.stringify(readings);
  //return jsonString;
  String jsonString;
  StaticJsonDocument<64> doc;
  doc["donut"] =  random(100);
  //serializeJson(doc, Serial);
  serializeJson(doc, jsonString);
  return jsonString;
}

// Initialize SPIFFS
void initFS() {
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting FS");
  }
  Serial.println("FS mounted successfully");
}

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);

  initWiFi();
  initFS();
  
  // !!!The web handlers order matter!!!
  // AsyncWeb Request Life Cycle
  //  -TCP connection is received by the server
  //  -The connection is wrapped inside Request object
  //  -When the request head is received (type, url, get params, http version and host), 
  //   the server goes through all Rewrites (in the order they were added) 
  //   to rewrite the url and inject query parameters, next, it goes through all attached Handlers(in the order they were added)
  //   trying to find one that canHandle the given request. If none are found, the default(catch-all) handler is attached.

  // Request for the latest sensor readings
  server.on("/readings", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json"); 
    String json = getSensorReadings();
    response->print(json);
    request->send(response);
    json = String();
  });

  // Web Server Root URL
  //server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
  //  request->send(LittleFS, "/index.html", "text/html");
  //});
  //server.serveStatic("/", LittleFS, "/");
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
/*
  server.onNotFound([](AsyncWebServerRequest *request){
    Serial.printf("NOT_FOUND: ");
    if(request->method() == HTTP_GET)
      Serial.printf("GET");
    else if(request->method() == HTTP_POST)
      Serial.printf("POST");
    else if(request->method() == HTTP_DELETE)
      Serial.printf("DELETE");
    else if(request->method() == HTTP_PUT)
      Serial.printf("PUT");
    else if(request->method() == HTTP_PATCH)
      Serial.printf("PATCH");
    else if(request->method() == HTTP_HEAD)
      Serial.printf("HEAD");
    else if(request->method() == HTTP_OPTIONS)
      Serial.printf("OPTIONS");
    else
      Serial.printf("UNKNOWN");
    Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

    if(request->contentLength()){
      Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
      Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
    }

    int headers = request->headers();
    int i;
    for(i=0;i<headers;i++){
      AsyncWebHeader* h = request->getHeader(i);
      Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
    }

    int params = request->params();
    for(i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isFile()){
        Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      } else if(p->isPost()){
        Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      } else {
        Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }

    request->send(404);
  });
*/
  events.onConnect([](AsyncEventSourceClient * client) {
    if (client->lastId()) {
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);

  // Start server
  server.begin();
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    // Send Events to the client with the Sensor Readings Every 10 seconds
    events.send("ping", NULL, millis());
    events.send(getSensorReadings().c_str(), "new_readings" , millis());
    lastTime = millis();
  }
}
