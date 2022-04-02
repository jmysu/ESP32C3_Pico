#include <Arduino.h>

int PWM_FREQUENCY = 1000; // this variable is used to define the time period 
int PWM_CHANNEL0 = 0; // this variable is used to select the channel number
int PWM_CHANNEL1 = 1; // this variable is used to select the channel number
int PWM_RESOUTION = 8; // this will define the resolution of the signal which is 8 in this case
int PWM_Duty = 127;


void setupEspPWM()
{
    ledcSetup(PWM_CHANNEL0, PWM_FREQUENCY, PWM_RESOUTION);
    ledcAttachPin(5, PWM_CHANNEL0);
    ledcSetup(PWM_CHANNEL1, PWM_FREQUENCY, PWM_RESOUTION);
    ledcAttachPin(18, PWM_CHANNEL1);
}

unsigned long lastPWMMillis=0;
uint8_t duty=0;
void loopEspPWM()
{
    if (millis()-lastPWMMillis>100) {
        lastPWMMillis = millis();
        duty +=8;
        if (duty>127) duty=0;

        ledcWrite(PWM_CHANNEL0, duty);
        ledcWrite(PWM_CHANNEL1, 127-duty); 
        }
}