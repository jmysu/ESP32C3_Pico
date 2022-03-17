#include <Arduino.h>
#include "esp_chip_info.h"

esp_chip_info_t chip_info;

 
void printESP32info() {

    esp_chip_info(&chip_info);     
    Serial.println("Hardware info");
    Serial.println("-----------------");
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
