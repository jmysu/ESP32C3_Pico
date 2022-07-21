/**
 * ----------------------------------------------------------------------------
 * ESP32 Remote Control with WebSocket
 * ----------------------------------------------------------------------------
 * © 2020 Stéphane Calderoni
 *   2022 Jimmy.Su
 * ----------------------------------------------------------------------------
          ESP-32-C3-12F Kit  

                        ┌────────────────┐
                        │     ┌──────┐   │
                AD0 | 01│●  ┌─┘      │  ●│30 | IO19 | LED-Cold | Neopixel
     LynxTx <   IO1 | 02│●  │        │  ●│29 | IO18 | LED-Warm | Buzzer
                NC  | 03│   └────────┘  ●│28 | IO10 
                IO2 | 04│● ┌─────┐19 18 ●│27 | IO9  | I2C SDA  | BOOTKEY
                IO3 | 05│● │3/4/5│⏄ ⏄  ●│26 | IO8  | I2C SCL
                IO4 | 06│● └─────┘      ■│25 | 3V3
                IO5 | 07│●              ■│24 | GND
                NC  | 08│                │23 | NC
                NC  | 09│               ●│22 | IO6  < TouchRx
                GND | 10│■              ●│21 | IO7  > TouchTx
                3V3 | 11│■               │20 | NC
                EN  | 12│               ●│19 | IO20 < USB-RX
     LynxRx >   IO0 | 13│●              ●│18 | IO21 > USB-TX
                GND | 14│■              ■│17 | GND
                5V  | 15│■    ┌────┐    ■│16 | 3V3
                        └─────┃    ┃─────┘          
                              ┗━━━━┛
=============================================================================
 */

#include <Arduino.h>
//#include <SPIFFS.h>
#include <LittleFS.h>
#define SPIFFS LittleFS
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

// ----------------------------------------------------------------------------
// Definition of macros
// ----------------------------------------------------------------------------
#define LED_BUILTIN 18
#define LED_PIN   3
#define BTN_PIN   9
#define HTTP_PORT 80

// ----------------------------------------------------------------------------
// Definition of global constants
// ----------------------------------------------------------------------------

// Button debouncing
const uint8_t DEBOUNCE_DELAY = 10; // in milliseconds

// WiFi credentials
const char *WIFI_SSID = "BreezeHill_50";
const char *WIFI_PASS = "0932902190";

// ----------------------------------------------------------------------------
// Definition of the LED component
// ----------------------------------------------------------------------------

struct Led {
    // state variables
    uint8_t pin;
    bool    on;

    // methods
    void update() {
        digitalWrite(pin, on ? HIGH : LOW);
    }
};

// ----------------------------------------------------------------------------
// Definition of the Button component
// ----------------------------------------------------------------------------

struct Button {
    // state variables
    uint8_t  pin;
    bool     lastReading;
    uint32_t lastDebounceTime;
    uint16_t state;

    // methods determining the logical state of the button
    bool pressed()                { return state == 1; }
    bool released()               { return state == 0xffff; }
    bool held(uint16_t count = 0) { return state > 1 + count && state < 0xffff; }

    // method for reading the physical state of the button
    void read() {
        // reads the voltage on the pin connected to the button
        bool reading = digitalRead(pin);

        // if the logic level has changed since the last reading,
        // we reset the timer which counts down the necessary time
        // beyond which we can consider that the bouncing effect
        // has passed.
        if (reading != lastReading) {
            lastDebounceTime = millis();
        }

        // from the moment we're out of the bouncing phase
        // the actual status of the button can be determined
        if (millis() - lastDebounceTime > DEBOUNCE_DELAY) {
            // don't forget that the read pin is pulled-up
            bool pressed = reading == LOW;
            if (pressed) {
                     if (state  < 0xfffe) state++;
                else if (state == 0xfffe) state = 2;
            } else if (state) {
                state = state == 0xffff ? 0 : 0xffff;
            }
        }

        // finally, each new reading is saved
        lastReading = reading;
    }
};

// ----------------------------------------------------------------------------
// Definition of global variables
// ----------------------------------------------------------------------------

Led    onboard_led = { LED_BUILTIN, false };
Led    led         = { LED_PIN, false };
Button button      = { BTN_PIN, HIGH, 0, 0 };

AsyncWebServer server(HTTP_PORT);
AsyncWebSocket ws("/ws");

// ----------------------------------------------------------------------------
// SPIFFS initialization
// ----------------------------------------------------------------------------

void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("Cannot mount SPIFFS volume...");
    while (1) {
        onboard_led.on = millis() % 200 < 50;
        onboard_led.update();
    }
  }
  Serial.println("LittleFS OK!");
}

// ----------------------------------------------------------------------------
// Connecting to the WiFi network
// ----------------------------------------------------------------------------

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("Trying to connect [%s] ", WiFi.macAddress().c_str());
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
  }
  Serial.printf(" %s\n", WiFi.localIP().toString().c_str());
}

// ----------------------------------------------------------------------------
// Web server initialization
// ----------------------------------------------------------------------------

String processor(const String &var) {
    return String(var == "STATE" && led.on ? "on" : "off");
}

//void onRootRequest(AsyncWebServerRequest *request) {
//  request->send(SPIFFS, "/index.html", "text/html", false, processor);
//}

void initWebServer() {
        // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println("Serving index.html");
        request->send(LittleFS, "/index.html", "text/html");
        });
    // Route to load style.css file
    server.on("/index.css", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println("Serving index.css");
        request->send(LittleFS, "/index.css", "text/css");
        });
    // Route to load style.css file
    server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println("Serving index.js");
        request->send(LittleFS, "/index.js", "text/javascript");
        });

    server.begin();
}

// ----------------------------------------------------------------------------
// WebSocket initialization
// ----------------------------------------------------------------------------

void notifyClients() {
    const uint8_t size = JSON_OBJECT_SIZE(1);
    StaticJsonDocument<size> json;
    json["status"] = led.on ? "on" : "off";

    char buffer[32];
    size_t len = serializeJson(json, buffer);
    ws.textAll(buffer, len);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
 
        const uint8_t size = JSON_OBJECT_SIZE(1);
        StaticJsonDocument<size> json;
        DeserializationError err = deserializeJson(json, data);
        if (err) {
            Serial.print(F("deserializeJson() failed with code "));
            Serial.println(err.c_str());
            return;
        }

        serializeJsonPretty(json, Serial);
        const char *action = json["action"];
        if (strcmp(action, "toggle") == 0) {
            led.on = !led.on;
            notifyClients();
        }

    }
}

void onEvent(AsyncWebSocket       *server,
             AsyncWebSocketClient *client,
             AwsEventType          type,
             void                 *arg,
             uint8_t              *data,
             size_t                len) {

    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            break;
        case WS_EVT_DATA:
            handleWebSocketMessage(arg, data, len);
            break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
    }
}

void initWebSocket() {
    ws.onEvent(onEvent);
    server.addHandler(&ws);
}

// ----------------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------------

void setup() {
    pinMode(onboard_led.pin, OUTPUT);
    pinMode(led.pin,         OUTPUT);
    pinMode(button.pin,      INPUT);

    Serial.begin(115200); delay(500);
    while(!Serial) {}

    initSPIFFS();
    initWiFi();
    initWebSocket();
    initWebServer();
}

// ----------------------------------------------------------------------------
// Main control loop
// ----------------------------------------------------------------------------

void loop() {
    ws.cleanupClients();

    button.read();

    if (button.pressed()) {
        led.on = !led.on;
        notifyClients();
    }
    
    onboard_led.on = millis() % 1000 < 50;

    led.update();
    onboard_led.update();
    yield();
}