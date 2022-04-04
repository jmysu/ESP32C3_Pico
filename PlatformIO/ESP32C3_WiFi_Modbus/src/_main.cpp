/* _I2CMasterSlave.cpp
 *
 *   I2C Master Slaver test
 * 
 *
 */
#include <Arduino.h>
#include <TimeLib.h>
#ifdef ARDUINO_ARCH_ESP32
 #include "esp32-hal-log.h"
 #include <esp_log.h>
#endif

#include <Ticker.h>
Ticker tickerCheckNTP;

static const char *TAG = "_main";
extern void printESP32info();
extern void setupTickerLED();
extern void setupWiFi();
extern void loopWiFi();
extern void setupSSOLED();
extern void setupEspPWM();
extern void loopEspPWM();
extern bool syncNTP();
extern void setupModbusServer();

extern bool isWiFiConnected;
void checkNTP(){
    if (isWiFiConnected) {
        if (syncNTP()) {
            tickerCheckNTP.detach();
            log_d("NTP synced, tickerCheckNTP detached!");
            }
        }
}

void setup() {
    pinMode(3, OUTPUT);
    digitalWrite(3, HIGH);

     // Set up serial port and wait until connected
    Serial.begin(115200);
    while(!Serial){}

    log_w("Starting...");
    digitalWrite(3, LOW);
    #if defined(ESP32)
        printESP32info();
    #endif

    log_e("%s", "Give me FIVE!");
    log_v("%s", "Starting NTP-Clock...");
    setupEspPWM();
    //setupTickerLED(); //The LED On/Off ticker, will overide the PWM LED!!!
    setupWiFi();
    setupSSOLED();    

    tickerCheckNTP.attach(5.0, checkNTP);   //Assign a 5 seconds ticker for checking NTP
                                            //Once synced, detatch the ticker!     
    setupModbusServer();
}

//extern BLEServer *pServer;
extern bool isWiFiConnected;
void loop() {
    loopWiFi(); 
}