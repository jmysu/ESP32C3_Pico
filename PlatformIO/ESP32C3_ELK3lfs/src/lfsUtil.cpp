/* Checks and writes out the files on a LittleFS filesystem on ESP8266 and ESP32 platforms
   This sketch only performs READ operations on the LittleFS filesystem and should not modify the filesystem's contents.

   NOTE: The LittleFS image must have already been uploaded prior to using this sketch.
*/
#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

StaticJsonDocument<512> jsonDoc; ///< JSON document

// Via https://github.com/espressif/arduino-esp32/blob/master/libraries/LittleFS/examples/LITTLEFS_test/LITTLEFS_test.ino
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    #ifdef ARDUINO_ARCH_ESP8266
    File root = fs.open(dirname, "r");
    #else
    File root = fs.open(dirname);
    #endif

    if(!root){
        Serial.println("- failed to open directory");
        return;
        }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
        }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                #ifdef ARDUINO_ARCH_ESP8266
                listDir(fs, file.fullName(), levels -1);
                #else
                listDir(fs, file.path(), levels -1);
                #endif
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
            }
        file = root.openNextFile();
        }
}

// via https://github.com/espressif/arduino-esp32/blob/master/libraries/LittleFS/examples/LITTLEFS_test/LITTLEFS_test.ino
void lfsFileRead(fs::FS &fs, const char * path){
  Serial.print("\n[LFS]Mounting LittleFS filesystem...");
  if(!LittleFS.begin()){
    Serial.println("ERROR: LittleFS Mount Failed!");
    return;
    }
  Serial.println("Mounted!");
  Serial.printf("[LFS]Used/Total:%lu/%luKB\n", LittleFS.usedBytes()/1024L, LittleFS.totalBytes()/1024L);


    Serial.printf("Reading file: %s\r\n", path);
    #ifdef ARDUINO_ARCH_ESP8266
    File file = fs.open(path, "r");
    #else
    File file = fs.open(path);
    #endif

    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
        }

    Serial.println("- read from file:");
    while(file.available()){
        Serial.write(file.read());
        }
 
    file.close();
}

void lfsJasonRead(fs::FS &fs, const char *path) {
  // attempt to mount LittleFS filesystem
  Serial.print("\n[LFS]Mounting LittleFS filesystem...");
  if(!LittleFS.begin()){
    Serial.println("ERROR: LittleFS Mount Failed!");
    return;
    }
  Serial.println("Mounted!");
  Serial.printf("[LFS]Used/Total:%lu/%luKB\n", LittleFS.usedBytes()/1024L, LittleFS.totalBytes()/1024L);

  // list everything on the filesystem '/' directory where secrets is expected
  //listDir(LittleFS, "/", 3);
  if (!LittleFS.exists(path)) {
    Serial.println("ERROR: Unable to find "+String(path)+" on LittleFS filesystem!");
    return;
    }
  // Test we can open the secrets file using ArduinoJSON
  File jfile = LittleFS.open(path, "r");
  if (!jfile) {
    Serial.println("ERROR: Unable to open json file!");
    return;
    }
  Serial.printf("Deserialize %s...\n", path);  
  // Test we can deserialize the secrets file using ArduinoJSON
  DeserializationError err = deserializeJson(jsonDoc, jfile);
  if (err) {
    Serial.print("ERROR: Failed to deserialize json file : ");
    Serial.println(err.c_str());
    return;
    }
  //Serialize to Serial output!
  serializeJsonPretty(jsonDoc, Serial);
  
  // close the file
  jfile.close();
  // clear the JSON document and release all memory from the memory pool
  jsonDoc.clear();

  // close fs
  //LittleFS.end();

  //Serial.println("DONE!");
}
