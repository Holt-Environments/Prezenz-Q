
#include "led_strip.h"

static void (*volatile ledCommand)() = NULL;

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

void ledWaiting()
{
  double value = millis() % LED_PERIOD;
  double test = (.5 * sin(2 * PI * (value / LED_PERIOD))) + .5;
  analogWrite(LED_B, (int)(test * 255));
}

void led_strip_update()
{
  (*ledCommand)();
}

void led_strip_set_command(enum state _new_state) {
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
