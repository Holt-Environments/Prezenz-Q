
/**
   =======================================================================
   PrezenzQ Controller - led_strip.cpp

   Holt Environments
   Author: Anthony Mesa
   Date: 05/24/2022

   =======================================================================
*/

#include "led_strip.h"

static void (*ledCommand)() = NULL;

void led_strip_init() {
  pinMode(LED_B, OUTPUT);
}

void ledOff()
{
  analogWrite(LED_B, 0);
}

void ledOn()
{
  analogWrite(LED_B, 255);
}

/**
   This state creates a sin value that is written to the LED. The sin
   function used oscillates between 0 and 1, where the period is defined
   in led_strip.h. The sin value is mapped to values 0 - 254 required for
   controlling an analog signal.
*/
void ledWaiting()
{
  double value = millis() % LED_PERIOD;
  double test = (.5 * sin(2 * PI * (value / LED_PERIOD))) + .5;
  analogWrite(LED_B, (int)(test * 255));
}

/**
   Simply executes whichever function that the ledCommand function pointer
   is currently pointing to.
*/
void led_strip_update()
{
  (*ledCommand)();
}

void led_strip_set_command(enum led_state _new_state) {
  switch (_new_state) {
    case LED_STRIP_OFF:
      ledCommand = &ledOff;
      break;
    case LED_STRIP_WAITING:
      ledCommand = &ledWaiting;
      break;
    case LED_STRIP_ON:
      ledCommand = &ledOn;
      break;
  }
}
