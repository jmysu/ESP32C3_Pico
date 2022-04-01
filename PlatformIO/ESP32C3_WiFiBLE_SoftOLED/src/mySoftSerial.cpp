// On ESP8266:
// At 80MHz runs up 57600ps, and at 160MHz CPU frequency up to 115200bps with only negligible errors.
// Connect pin 13 to 15.
// For verification and as a example for how to use SW serial on the USB to PC connection,
// which allows the use of HW Serial on GPIO13 and GPIO15 instead, #define SWAPSERIAL below.
// Notice how the bitrates are also swapped then between RX/TX and GPIO13/GPIO15.
// Builtin debug output etc. must be stopped on HW Serial in this case, as it would interfere with the
// external communication on GPIO13/GPIO15.

/*
Microsoft serial mouse
   Microsoft serial mouse is the most popular 2 button serial mouse type. Typically that cheap tho button mouse which comes with the computer is Microsoft mouse system. 
   Microsoft mouse is supported in all major operating systems.

Mouse resolution and tracking rate
   Maximum tracking rate for Microsoft mouse is 40 reports/second * 127 counts per report = 5080 counts per second. The most common range for typical mouses is 100 to 400 CPI (count per inch) 
   but can be up to 1000 CPI (cheap ones typically are 100 CPI or 200 CPI models). 
   This means that you can move 100 CPI mouse up to speed of 50.8 inches per second and 400 CPI mouse maximally at 12.7 inches per second. 
   The actual accuracy of movement the software sees is detemined by the settings of the mouse driver (many mouse drivers have option to adjust mouse sensitivity).

Pinout
9 pin    25 pin     Wire Name            Comments
shell     1         Protective Ground
3         2         TD                  Serial data from host to mouse (only for power)
2         3         RD                  Serial data from mouse to host
7         4         RTS                 Positive voltage to mouse
8         5         CTS
6         6         DSR
5         7         Signal Ground
4         20        DTR                 Positive voltage to mouse and reset/detection

Graphical description how the data is contained in the packet
              1st byte        2nd byte         3rd byte
          ================  ===============  ================
           - 1 ? ? Y Y X X  - 0 X X X X X X  - 0 Y Y Y Y Y Y
          ================  ===============  ================
               | | \ / \ /      \---------/      \---------/
               | |  |   |            |                |
               | |  |   \----\       |                |
               | |  \--------|-------|--------\       |
               | |          / \ /---------\  / \ /---------\
               | |         ================ =================
               | |          0 0 0 0 0 0 0 0  0 0 0 0 0 0 0 0
 Left Button --/ |         ================ =================
Right Button ----/            X increment      Y increment

Mouse identification
   When DTR/RTS line are toggled, mouse should send one data byte containing letter 'M' (ascii 77).
*/
#include <Arduino.h>
#include <hal/gpio_types.h>

#include <SoftwareSerial.h>
auto& usbSerial = Serial;
SoftwareSerial touchSerial, mouseSerial;

#define pinTouchRx 18
#define pinTouchTx 19
#define pinMouseTx  1
#define pinMouseRTS 3

/* Serial mouse packet encoding routine */
/* Encodes a 3 byte mouse packet that complies with Microsoft Mouse/Logitech protocol */
void encodeMousePacket(int x, int y, bool lb, bool rb, unsigned char* output)
{  
  /* Cap values just in case to avoid overflow errors */
  char cx = constrain(x, -127, 127);
  char cy = constrain(y, -127, 127);
    
  /* Packet 0 */
  output[0] = ((cx>>6)&0x3) | /* last 2 bits of X */
    (((cy>>6)&0x3)<<2) |      /* Last 2 bits of Y */
    (rb<<4)|(lb<<5)|0x40;     /* Mouse buttons and start packet bit */

  output[1] = cx&0x3F;        /* Packet 1 ( first 6 bits of X ) */
  output[2] = cy&0x3F;        /* Packet 2 ( first 6 bits of Y ) */
}
/* Three button mouse event */
void sendMSMouseEvent(int8_t deltaX, int8_t deltaY, bool lb, bool rb) {
  uint8_t packet[3];
  encodeMousePacket(deltaX, deltaY, lb, rb, packet);
  mouseSerial.write(packet, 3);
}

//
/* Send init bytes when RTS has been toggled to high (inverted on USB-serial chip) */
bool isRTS=false;
void checkMouseRTS()
{
    if (!digitalRead(pinMouseRTS)) {
        if (!isRTS) {
            // Send 'M' after 20ms
            delay(20);
            mouseSerial.write('M');
            /* For Logitec mouse a '3' is sent after 63ms */
            //Serial.write( '3' );
            isRTS = true;
            }
        }
    else {
        isRTS = false;
        }
}

//Mouse move simulation
unsigned long lastMillis = 0;
uint16_t patternCounter = 0;
int16_t lastPosX = 0;
int16_t lastPosY = 0;
void moveMouse10S() 
{
  // move the mouse in a pattern every 10s
  if ((millis() - lastMillis) > 10000) {
    if (patternCounter > 360u) {
        lastMillis = millis();  // reset lastMillis
        patternCounter = 0u;    // reset pattern counter
    } else {
        const int16_t radius = 50;
        int16_t posX = radius * sin(1 * 2 * PI * patternCounter / 360); // lying 8 pattern -> oo
        int16_t posY = radius * sin(2 * 2 * PI * patternCounter / 360); // lying 8 pattern -> oo
        //int16_t posX = radius * sin(2 * PI * patternCounter / 360);   // circle
        //int16_t posY = - radius * cos(2 * PI * patternCounter / 360); // circle
        int16_t deltaX = posX - lastPosX;
        int16_t deltaY = posY - lastPosY;
        if ((deltaX != 0) || (deltaY != 0)) {
            sendMSMouseEvent(deltaX, deltaY, false, false);
            }
        //sendMSMouseEvent(posX - lastPosX, posY - lastPosY, false, false, false);
        delay(2);
        lastPosX = posX;
        lastPosY = posY;
        patternCounter++;
        }
    }
}

void sendMouseTestString() 
{
    for (char ch = ' '; ch <= 'z'; ch++) {
        mouseSerial.write(ch);
        }
    mouseSerial.println();
}

void setupSoftSerial() {

    //usbSerial.begin(115200);
    // Important: the buffer size optimizations here, in particular the isrBufSize (11) that is only sufficiently
    // large to hold a single word (up to start - 8 data - parity - stop), are on the basis that any char written
    // to the loopback SoftwareSerial adapter gets read before another write is performed.
    // Block writes with a size greater than 1 would usually fail. Do not copy this into your own project without
    // reading the documentation.
    //testSerial.begin(BAUD_RATE, SWSERIAL_8N1, D7, D8, false, 95, 11);
    //touchSerial.begin(9600, SWSERIAL_8N1, pinTouchRx, pinTouchTx);
    //mouseSerial.begin(1200, SWSERIAL_7N1, pinMouseRTS, pinMouseTx);

    //Only following pin TX OK for ESP32-C3-12F-Kit: 0,1,3,4,5,6,7,18,19
    //
    mouseSerial.begin(1200, SWSERIAL_7N1, -1, 19);
    
    //pinMode(pinMouseRTS, INPUT_PULLUP);

    while(!usbSerial){}
    usbSerial.println(PSTR("\nSoftware serial test started"));
    mouseSerial.println("Hello Mouse!");
}

void loopSoftSerial() {
    //Read touchRx when available
    //while (touchSerial.available() > 0) {
    //    Serial.write(touchSerial.read()); //USBSerial echo
    //    yield();
    //    }
    while (usbSerial.available() >0) {
        char  c = usbSerial.read();
        mouseSerial.write(c); // USB Serial => mouse
        if (c == '\r') {
            sendMouseTestString();
            break;
            }
        yield();    
        }
    //checkMouseRTS();
   //moveMouse10S(); 
    //mouseSerial.println(millis());
    //delay(1000);
}
