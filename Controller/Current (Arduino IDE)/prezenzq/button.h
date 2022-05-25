
/**
   =======================================================================
   PrezenzQ Controller - button.h

   Holt Environments
   Author: Anthony Mesa
   Date: 05/24/2022

   button.h provides the necessary logic for interfacing with
   an external button connected to the controller. This functionality
   allows the controller to be triggered manually in the event that the
   sensor isn't working properly.

   Debounce is a typical issue with buttons. No hardware debouncing is
   implemented, rather software debouncing is provided by the InputDebounce
   library which can be found here:

   https://github.com/Mokolea/InputDebounce

   =======================================================================
*/

#ifndef ARDUINO_LIB
#define ARDUINO_LIB
#include <Arduino.h>
#endif

#include <InputDebounce.h>

// Pins for button and button LED
#define SINGLE_STATE_BUTTON 5
#define SINGLE_STATE_BUTTON_LED 6

#define BUTTON_HOLD_DURATION 3000 // 3 Seconds
#define BUTTON_DEBOUNCE_DELAY 100 // .2 Seconds

enum button_state {BUTTON_OFF, BUTTON_PRESSED, BUTTON_HELD};

/**
   Initializes the manual button. This function calls
   pinMode() on a couple pins so this must be called before
   any other button calls can be made, else undefined behavior
   may occur.

   The Arduino pin for the button is defined via the
   SINGLE_STATE_BUTTON definition.
*/
void button_init();

/**
   Gets the currently stored state of the button. The available
   states returned are defined in the button_state enum.
*/
enum button_state button_get_state();

/**
   Sets the led to the value provided. Currently this function
   is designed to only set one channel of an/the LED.

   Any int values can be provided, but only values 0 - 254
   are evaluated. A

   The digital pin for the LED to be set is defined via
   SINGLE_STATE_BUTTON_LED. This function must be called
   after button_init() or undefined behavior may occur.
*/
void button_led_set(int _value);
