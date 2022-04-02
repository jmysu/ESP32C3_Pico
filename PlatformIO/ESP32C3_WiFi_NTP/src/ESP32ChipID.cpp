#include <Arduino.h>
#include "esp_chip_info.h"
esp_chip_info_t chip_info;

#ifdef ARDUINO_ARCH_ESP32
 #include "esp32-hal-log.h"
 #include <esp_log.h>
 #ifdef ESP_IDF_VERSION_MAJOR // IDF 4+
    #if CONFIG_IDF_TARGET_ESP32 // ESP32/PICO-D4
    #include "esp32/rom/rtc.h"
    #elif CONFIG_IDF_TARGET_ESP32S2
    #include "esp32s2/rom/rtc.h"
    #elif CONFIG_IDF_TARGET_ESP32C3
    #include "esp32c3/rom/rtc.h"
    #elif CONFIG_IDF_TARGET_ESP32S3
    #include "esp32s3/rom/rtc.h"
    #else 
    #error Target CONFIG_IDF_TARGET is not supported
    #endif
    #else // ESP32 Before IDF 4.0
    #include "rom/rtc.h"
 #endif
#endif



void verbose_print_reset_reason(RESET_REASON reason)
{
  switch ( reason)
  {
    case 1  : Serial.println ("Vbat power on reset");break;
    case 3  : Serial.println ("Software reset digital core");break;
    case 4  : Serial.println ("Legacy watch dog reset digital core");break;
    case 5  : Serial.println ("Deep Sleep reset digital core");break;
    case 6  : Serial.println ("Reset by SLC module, reset digital core");break;
    case 7  : Serial.println ("Timer Group0 Watch dog reset digital core");break;
    case 8  : Serial.println ("Timer Group1 Watch dog reset digital core");break;
    case 9  : Serial.println ("RTC Watch dog Reset digital core");break;
    case 10 : Serial.println ("Instrusion tested to reset CPU");break;
    case 11 : Serial.println ("Time Group reset CPU");break;
    case 12 : Serial.println ("Software reset CPU");break;
    case 13 : Serial.println ("RTC Watch dog Reset CPU");break;
    case 14 : Serial.println ("for APP CPU, reseted by PRO CPU");break;
    case 15 : Serial.println ("Reset when the vdd voltage is not stable");break;
    case 16 : Serial.println ("RTC Watch dog reset digital core and rtc module");break;
    default : Serial.println ("NO_MEAN");
  }
}
 
void printESP32info() {

    log_i("CPU reset reason:");
    verbose_print_reset_reason(rtc_get_reset_reason(0));

    esp_chip_info(&chip_info);     
    log_w("\nChip Info:\n-----------------");
    //Serial.println("Hardware info");
    //Serial.println("-----------------");
    switch (chip_info.model) {
        case CHIP_ESP32://  = 1, //!< ESP32
            Serial.println("ESP32");
            break;
        case CHIP_ESP32S2:// = 2, //!< ESP32-S2
            Serial.println("ESP32S2");
            break;
        case CHIP_ESP32S3:// = 9, //!< ESP32-S3
            Serial.println("ESP32S3");
            break;
        case CHIP_ESP32C3:// = 5, //!< ESP32-C3
            Serial.println("ESP32C3");
            break;
        case CHIP_ESP32H2:// = 6, //!< ESP32-H2
            Serial.println("ESP32H2");
            break;
    }
    
    Serial.printf("%d cores Wifi %s%s\n", chip_info.cores, (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
     (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");
    Serial.printf("Silicon revision: %d\n", chip_info.revision);
    Serial.printf("%dMB %s flash\n", spi_flash_get_chip_size()/(1024*1024),
    (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embeded" : "external");
    
    //get chip id
    String chipId = String((uint32_t)ESP.getEfuseMac(), HEX);
    chipId.toUpperCase();
    
    Serial.printf("Chip id: %s\n", chipId.c_str());
    Serial.println("-----------------");
}
