
#ifndef ARDUINO_LIB
#define ARDUINO_LIB
#include <Arduino.h>
#endif

//  When not using a specific led color channel (When that led color channel is not wired/connected to 
//  a corresponding Arduino digital pin) its definition should be set to (-1).
#define LED_R -1;
#define LED_G -1
#define LED_B 3
#define LED_W -1
#define LED_PERIOD 3000 // ms

enum state{
  LED_STRIP_OFF, 
  LED_STRIP_WAITING, 
  LED_STRIP_ON
};

void led_strip_init();
void led_strip_set_command(enum state _new_state);
void led_strip_update();
