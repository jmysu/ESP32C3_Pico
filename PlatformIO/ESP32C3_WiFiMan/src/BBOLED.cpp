//
// Small Simple OLED library demo
//
// Demonstrates how to initialize and use a few functions of the ss_oled library
// If your MCU has enough RAM, enable the backbuffer to see a demonstration
// of the speed difference between drawing directly on the display versus
// deferred rendering, followed by a "dump" of the memory to the display
//
#include <Arduino.h>
#ifdef ARDUINO_ARCH_ESP32
 #include "esp32-hal-log.h"
 #include <esp_log.h>
#endif
#include <ss_oled.h>

#include <TimeLib.h>
#include <Ticker.h>
Ticker tickerTimerClock;

// if your system doesn't have enough RAM for a back buffer, comment out
// this line (e.g. ATtiny85)
#define USE_BACKBUFFER

#ifdef USE_BACKBUFFER
static uint8_t ucBackBuffer[1024];
#else
static uint8_t *ucBackBuffer = NULL;
#endif

// Use -1 for the Wire library default pins
// or specify the pin numbers to use with the Wire library or bit banging on any GPIO pins
// These are the pin numbers for the M5Stack Atom default I2C
#define SDA_PIN 9
#define SCL_PIN 8
// Set this to -1 to disable or the GPIO pin number connected to the reset
// line of your display if it requires an external reset
#define RESET_PIN -1
// let ss_oled figure out the display address
#define OLED_ADDR -1
// don't rotate the display
#define FLIP180 0
// don't invert the display
#define INVERT 0
// Bit-Bang the I2C bus
#define USE_HW_I2C 0

// Change these if you're using a different OLED display
#define MY_OLED OLED_128x32
//#define MY_OLED OLED_64x32
//#define OLED_WIDTH 64
//#define OLED_HEIGHT 32

SSOLED ssoled;

void updateClock()
{
  char buf[16];
  sprintf(buf, "%02d:%02d:%02d", hour(), minute(), second());

  //oledFill(&ssoled, 0x0, 1);
  oledWriteString(&ssoled, 0, 0,0,(char *)buf, FONT_LARGE, 0, 1);
  //oledDumpBuffer(&ssoled, NULL);
}
void setupSSOLED() {
  int rc;
// The I2C SDA/SCL pins set to -1 means to use the default Wire library
// If pins were specified, they would be bit-banged in software
// This isn't inferior to hw I2C and in fact allows you to go faster on certain CPUs
// The reset pin is optional and I've only seen it needed on larger OLEDs (2.4")
//    that can be configured as either SPI or I2C
//
// oledInit(SSOLED *, type, oled_addr, rotate180, invert, bWire, SDA_PIN, SCL_PIN, RESET_PIN, speed)

  rc = oledInit(&ssoled, MY_OLED, OLED_ADDR, FLIP180, INVERT, USE_HW_I2C, SDA_PIN, SCL_PIN, RESET_PIN, 400000L); // use standard I2C bus at 400Khz
  log_d("SSOLED:%0X", rc);
  if (rc != OLED_NOT_FOUND)  {
    char *msgs[] = {(char *)"SSD1306 @ 0x3C", (char *)"SSD1306 @ 0x3D",(char *)"SH1106 @ 0x3C",(char *)"SH1106 @ 0x3D"};
    log_d("Found OLED %s", msgs[rc]);
    oledFill(&ssoled, 0, 1);
    oledWriteString(&ssoled, 0,0,0,msgs[rc], FONT_NORMAL, 0, 1);
    oledSetBackBuffer(&ssoled, ucBackBuffer);
    delay(200);
    }

  // put your main code here, to run repeatedly:
int i, x, y;
char szTemp[32];
unsigned long ms;

  oledFill(&ssoled, 0x0, 1);
  oledWriteString(&ssoled, 0, 16,0,(char *)"0123456789", FONT_SMALL, 0, 1);
  oledWriteString(&ssoled, 0,  0,1,(char *)"-SSOLED-", FONT_NORMAL, 1, 1);
  oledWriteString(&ssoled, 0, 64,1,(char *)"**Demo**", FONT_NORMAL, 0, 1);
  oledWriteString(&ssoled, 0,  0,2,(char *)"12345678", FONT_NORMAL, 0, 1);
  oledWriteString(&ssoled, 0, 64,3,(char *)"12345678", FONT_NORMAL, 0, 1);
  oledRectangle(&ssoled, 0, 0, 126, 31, 1, 0);
  oledDumpBuffer(&ssoled, NULL);

  log_d("SS_OLED done!");
  //delay(500);

  tickerTimerClock.attach_ms(1000, updateClock);

} /* loop() */
