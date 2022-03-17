/*
 * espC3-12f kit led toggler
 *
 * 
 * 
 */ 
#include <Arduino.h>
#include <Ticker.h>
#include <driver/gpio.h>
 
#define LED_AMBER GPIO_NUM_18
#define LED_CYAN  GPIO_NUM_19
#define LED_RED   GPIO_NUM_3
#define LED_GREEN GPIO_NUM_4
#define LED_BLUE  GPIO_NUM_5

Ticker tickerLED;
Ticker tickerWiFi;

bool isBlinkLED_A, isBlinkLED_C, isBlinkLED_R, isBlinkLED_G, isBlinkLED_B;
void blinkLED() {
    if (isBlinkLED_A)
        digitalWrite(LED_AMBER, !digitalRead(LED_AMBER));
    if (isBlinkLED_C)    
        digitalWrite(LED_CYAN, !digitalRead(LED_CYAN));
    if (isBlinkLED_R)    
            digitalWrite(LED_RED, !digitalRead(LED_RED));
    if (isBlinkLED_G)    
            digitalWrite(LED_GREEN, !digitalRead(LED_GREEN));
    if (isBlinkLED_B)    
            digitalWrite(LED_BLUE, !digitalRead(LED_BLUE));  
}


extern void checkWiFi();
void setupTicker() {

    pinMode(LED_RED,   OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE,  OUTPUT);
    pinMode(LED_AMBER, OUTPUT);
    pinMode(LED_CYAN,  OUTPUT);

    gpio_set_direction(LED_AMBER, GPIO_MODE_INPUT_OUTPUT);  //compatibility for ESP-C3 digitalRead
    gpio_set_direction(LED_CYAN,  GPIO_MODE_INPUT_OUTPUT);    
    gpio_set_direction(LED_RED,   GPIO_MODE_INPUT_OUTPUT);    
    gpio_set_direction(LED_GREEN, GPIO_MODE_INPUT_OUTPUT);   
    gpio_set_direction(LED_BLUE,  GPIO_MODE_INPUT_OUTPUT);    

    digitalWrite(LED_AMBER, HIGH);
    digitalWrite(LED_CYAN,  LOW);

    tickerLED.attach(0.5, blinkLED); //0.5second
    tickerWiFi.attach(3, checkWiFi); //3 seconds
    
}