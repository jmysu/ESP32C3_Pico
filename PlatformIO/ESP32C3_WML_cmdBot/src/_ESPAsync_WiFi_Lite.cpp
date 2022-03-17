/****************************************************************************************************************************
  _ESPAsync_WiFi_Lite.cpp
    For ESP32 boards

  ESPAsync_WiFiManager_Lite (https://github.com/khoih-prog/ESPAsync_WiFiManager_Lite) is a library 
  for the ESP32/ESP8266 boards to enable store Credentials in EEPROM/SPIFFS/LittleFS for easy 
  configuration/reconfiguration and autoconnect/autoreconnect of WiFi and other services without Hardcoding.

  Built by Khoi Hoang https://github.com/khoih-prog/ESPAsync_WiFiManager_Lite
  Licensed under MIT license

  include files:
    for WiFi Manager: 
       _Credentials.h
       _defines.h
       _dynamicParam.h
    for serial commander:
       myANSI.cpp
       cmdBasic.cpp
       cmdMasterCommands.cpp
    for ticker:
       myTicker.cpp   


  *****************************************************************************************************************************/
#include <Arduino.h>
  
#include "_defines.h"
#include "_Credentials.h"
#include "_dynamicParams.h"

ESPAsync_WiFiManager_Lite* ESPAsync_WiFiManager;


extern bool isBlinkLED_R, isBlinkLED_G, isBlinkLED_B;
void checkWiFi()
{
  isBlinkLED_R = false;
  isBlinkLED_G = false;
  isBlinkLED_B = false;  
  if (WiFi.status() == WL_CONNECTED)
    //Serial.print("H");        // H means connected to WiFi
    isBlinkLED_B=true;
  else  {
    if (ESPAsync_WiFiManager->isConfigMode())
      //Serial.print("C");        // C means in Config Mode
      isBlinkLED_G=true;
    else
      //Serial.print("F");        // F means not connected to WiFi  
      isBlinkLED_R=true;
    }
}

void heartBeatPrintWiFi()
{
  static int num = 1;

  if (WiFi.status() == WL_CONNECTED)
    Serial.print("H");        // H means connected to WiFi
  else
  {
    if (ESPAsync_WiFiManager->isConfigMode())
      Serial.print("C");        // C means in Config Mode
    else
      Serial.print("F");        // F means not connected to WiFi  
  }

  if (num == 80)
  {
    Serial.println();
    num = 1;
  }
  else if (num++ % 10 == 0)
  {
    Serial.print(F(" "));
  }
}
/*
extern bool checkNTP();
void check_status()
{
  static unsigned long checkstatus_timeoutWiFi = 0;
  static unsigned long checkstatus_timeoutNTP = 0;
  

  //KH
#define HEARTBEAT_INTERVAL_WIFI    10000L
#define HEARTBEAT_INTERVAL_NTP     60000L

  // Print hearbeat every HEARTBEAT_INTERVAL (10) seconds.
  if ((millis() > checkstatus_timeoutWiFi) || (checkstatus_timeoutWiFi == 0)){
    heartBeatPrint();
    checkstatus_timeoutWiFi = millis() + HEARTBEAT_INTERVAL_WIFI;
    }
  // Check NTP every 60 seconds  
  if ((millis() > checkstatus_timeoutNTP) || (checkstatus_timeoutNTP == 0)){
    checkNTP();
    checkstatus_timeoutNTP = millis() + HEARTBEAT_INTERVAL_NTP;   
    }

}
*/

#if USING_CUSTOMS_STYLE
const char NewCustomsStyle[] /*PROGMEM*/ = "<style>div,input{padding:5px;font-size:1em;}input{width:95%;}body{text-align: center;}\
button{background-color:blue;color:white;line-height:2.4rem;font-size:1.2rem;width:100%;}fieldset{border-radius:0.3rem;margin:0px;}</style>";
#endif

extern void setupTicker();
extern void setupCmd();
extern void loopCmd();
extern void setupTelegram();
extern void loopTelegram();
void setup()
{
  // Debug console
  Serial.begin(115200);
  while (!Serial);

  delay(200);
  //pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.print(F("\nStarting ESPAsync_WiFi using ")); Serial.print(FS_Name);
  Serial.print(F(" on ")); Serial.println(ARDUINO_BOARD);
  Serial.println(ESP_ASYNC_WIFI_MANAGER_LITE_VERSION);

#if USING_MRD  
  Serial.println(ESP_MULTI_RESET_DETECTOR_VERSION);
#else
  Serial.println(ESP_DOUBLE_RESET_DETECTOR_VERSION);
#endif

  ESPAsync_WiFiManager = new ESPAsync_WiFiManager_Lite();

  // Optional to change default AP IP(192.168.4.1) and channel(10)
  //ESPAsync_WiFiManager->setConfigPortalIP(IPAddress(192, 168, 120, 1));
  ESPAsync_WiFiManager->setConfigPortalChannel(0);

#if USING_CUSTOMS_STYLE
  ESPAsync_WiFiManager->setCustomsStyle(NewCustomsStyle);
#endif

#if USING_CUSTOMS_HEAD_ELEMENT
  ESPAsync_WiFiManager->setCustomsHeadElement("<style>html{filter: invert(10%);}</style>");
#endif

#if USING_CORS_FEATURE  
  ESPAsync_WiFiManager->setCORSHeader("Your Access-Control-Allow-Origin");
#endif

  // Set customized DHCP HostName
  ESPAsync_WiFiManager->begin(HOST_NAME);
  //Or use default Hostname "NRF52-WIFI-XXXXXX"
  //ESPAsync_WiFiManager->begin();


  setupTicker();
  setupCmd();
  setupTelegram();
}

#if USE_DYNAMIC_PARAMETERS
void displayCredentials()
{
  Serial.println(F("\nYour stored Credentials :"));

  for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
  {
    Serial.print(myMenuItems[i].displayName);
    Serial.print(F(" = "));
    Serial.println(myMenuItems[i].pdata);
  }
}

void displayCredentialsInLoop()
{
  static bool displayedCredentials = false;

  if (!displayedCredentials)
  {
    for (int i = 0; i < NUM_MENU_ITEMS; i++)
    {
      if (!strlen(myMenuItems[i].pdata))
      {
        break;
      }

      if ( i == (NUM_MENU_ITEMS - 1) )
      {
        displayedCredentials = true;
        displayCredentials();
      }
    }
  }
}

#endif

void loop()
{
  ESPAsync_WiFiManager->run();
  
  //check_status();

  loopCmd();
  loopTelegram();
#if USE_DYNAMIC_PARAMETERS
  displayCredentialsInLoop();
#endif
}
