#include <Arduino.h>
#include <Ticker.h>
Ticker tickerPWM;

double PWM_FREQUENCY = 1000; // this variable is used to define the time period 
uint8_t PWM_CHANNEL0 = 0; // this variable is used to select the channel number
uint8_t PWM_CHANNEL1 = 1; // this variable is used to select the channel number
uint8_t PWM_RESOUTION = 8; // this will define the resolution of the signal which is 8 in this case
uint8_t PWM_Duty = 0;

extern bool isWiFiConnected;
uint8_t iDelta=32;
void updatePWM() {
    
    if (isWiFiConnected) 
        iDelta=8;
    else 
        iDelta =32;

    PWM_Duty += iDelta;
    ledcWrite(PWM_CHANNEL0, PWM_Duty>>1);       //50%     
    ledcWrite(PWM_CHANNEL1, (255-PWM_Duty)>>1); //50%
}

void setupEspPWM()
{
    ledcSetup(PWM_CHANNEL0, PWM_FREQUENCY, PWM_RESOUTION);
    ledcAttachPin(5, PWM_CHANNEL0);  //Blue
    ledcSetup(PWM_CHANNEL1, PWM_FREQUENCY, PWM_RESOUTION);
    ledcAttachPin(18, PWM_CHANNEL1); //Amber 

    tickerPWM.attach_ms(100, updatePWM); //10Hz
}

