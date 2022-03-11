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

Ticker togglerLED;

uint8_t idx;
void toggle() {

    digitalWrite(LED_AMBER, !digitalRead(LED_AMBER));
    digitalWrite(LED_CYAN, !digitalRead(LED_CYAN));
    
    switch (idx++) {
        case 0:
            digitalWrite(LED_RED, !digitalRead(LED_RED));
            break;
        case 1:
            digitalWrite(LED_GREEN, !digitalRead(LED_GREEN));
            break;
        case 2:
            digitalWrite(LED_BLUE, !digitalRead(LED_BLUE));
            break;
            break;
        default:
            idx = 0;
            break;    
        }
}

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

  togglerLED.attach(0.5, toggle); //0.5second

}