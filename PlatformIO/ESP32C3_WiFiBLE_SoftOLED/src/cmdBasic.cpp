/*Commander example - basic
 * Demonstrating commands to get and set an int and a float
 */
#if !defined(ANSI_LIB_VERSION)
 #include <ansi.h>
 ANSI myansi(&Serial);
#endif
#if defined(ESP32)
 #define LOG_LOCAL_LEVEL ESP_LOG_ERROR
 #include <esp_log.h>
 extern void printESP32info();
#endif

#include <Commander.h>
Commander cmd;
 

String cmdDeviceInfo = "#\tSerial Commander\n#\thttps://github.com/jmysu";
extern void setupANSI();
extern void initialiseCommander();
//SETUP ---------------------------------------------------------------------------
void setupCmd() {

  //Serial.begin(115200);
  //while (!Serial){}

  Serial.println();
  #if defined(ESP32)
    esp_log_level_set("*", LOG_LOCAL_LEVEL);
    printESP32info();
  #endif

  initialiseCommander();
  while(!Serial){;}

  cmd.printCommandList();
  cmd.printUserString();
  cmd.println();
  Serial.println("Type 'help' to get help");
  cmd.printCommandPrompt();

  //testANSI();
  myansi.bold();
  myansi.foreground(myansi.rgb2color(180, 200, 158));
  myansi.print("Hello ");
  myansi.background(myansi.rgb2color(180, 0, 158));
  myansi.print("Commander!>");
  myansi.normal();
  
}

//MAIN LOOP ---------------------------------------------------------------------------
void loopCmd() {
  cmd.update();
}
