/* _I2CMasterSlave.cpp
 *
 *   I2C Master Slaver test
 * 
 *
 */
#include <Arduino.h>
#ifdef ARDUINO_ARCH_ESP32
 #include "esp32-hal-log.h"
 #include <esp_log.h>
#endif

#include <Ticker.h>
//Ticker tickerScanI2C;

//#include <Wire.h>
//#include <SoftWire.h>
//#include <AsyncDelay.h>
//SoftWire swire(9,8);

static const char *TAG = "I2Ctest";
extern void printESP32info();
/*
//Send I2C call back
bool isFoundI2C=false;
void scanI2C()
{
    const uint8_t firstAddr = 1;
	const uint8_t lastAddr = 0x7F;
	Serial.println();
	Serial.print("Scanning all addresses in range 0x");
	Serial.print(firstAddr, HEX);
	Serial.print(" - 0x");
	Serial.print(lastAddr, HEX);
	Serial.println("...");

	for (uint8_t addr = firstAddr; addr <= lastAddr; addr++) {
		uint8_t startResult = swire.llStart((addr << 1) + 1); // Signal a read
		swire.stop();

		if (startResult == 0) {
			//Serial.print("\rDevice found at 0x");
            //Serial.print(addr, HEX);
			//Serial.flush();
            log_d("\rDevice found at 0x%02X", addr);
            isFoundI2C = true;
		    }
        delay(10);
	    }
}
*/
extern void setupTickerLED();
extern void setupWiFi();
extern void loopWiFi();
extern void setupBLE();
extern void loopBLE();
extern void setupOLED();
extern void loopOLED();

//extern void setupSoftI2C();
//extern void scanSoftI2C();
//extern void setupBBI2C(); 
extern void setupSSOLED();

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
    log_v("%s", "Starting...");
    setupTickerLED();

    //SDA:9 SCL:8
    //Wire.begin(9,8); //Setup master
    //Wire.begin((uint8_t)0x78, 9,8); //Setup slave
    //swire.setTimeout_ms(50);
    //swire.begin(); //master
 
    //tickerScanI2C.attach_ms(10000, scanI2C);

    //setupBBI2C();
    //setupSSOLED();
    setupWiFi();
    //setupBLE();

    //setupOLED();
    setupSSOLED();
    //setupSoftI2C();
    //scanSoftI2C();
 
}

//extern BLEServer *pServer;
void loop() {
    //if (isFoundI2C) tickerScanI2C.detach();

    loopWiFi(); 
    //loopBLE();

    //loopOLED();
}