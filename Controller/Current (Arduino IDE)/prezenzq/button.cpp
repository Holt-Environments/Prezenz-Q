
/**
   =======================================================================
   PrezenzQ Controller - button.cpp

   Holt Environments
   Author: Anthony Mesa
   Date: 05/24/2022

   =======================================================================
*/

#include "button.h"

static InputDebounce buttonTest;
static int button_state;

/**
   The button uses an external pull-down resistor on the pcb board.
*/
void button_init() {
  button_state = 0;
  pinMode(SINGLE_STATE_BUTTON_LED, OUTPUT);
  buttonTest.setup(SINGLE_STATE_BUTTON, BUTTON_DEBOUNCE_DELAY, InputDebounce::PIM_EXT_PULL_DOWN_RES);
}

/**
   The LED being set is expected to be only a single channel. The current
   hardware implementation is such that the LED is on or off, high or low.

   Even though this is the case currently, it could be the case that an
   analog-type LED be used in the future, with one or more channels. In that
   case it is an exercise for the reader to implement the logic necessary.
*/
void button_led_set(int _value) {
  if (_value == 0) {
    digitalWrite(SINGLE_STATE_BUTTON_LED, LOW);
  } else {
    digitalWrite(SINGLE_STATE_BUTTON_LED, HIGH);
  }
}

/**
   This function seeks to provide a way of getting the state of the button
   at any given time, where the states available are defined in the
   button_state enum. There is some funky logic going on here and it is
   integral into providing manual button capability, so only edit this
   function if you have a very good grasp on what is going on and why.
*/
enum button_state button_get_state() {

  static unsigned int buttonTest_StateOnCount = 0;

  /**
     curr_button_held will either be a non-zero value corresponding
     to the millis of when the button was first pressed, or it will
     be set to -1 indicating that the button should no longer be held.
     As you can see later in the function, this is used to make sure
     that this function only reports the 'HELD' button state once
     after its discovery, rather than every time that the function
     is called (which would repeatedly return the HELD state erroneously).
  */
  static long curr_button_held = 0;
  static enum button_state current_state = BUTTON_OFF;
  static uint8_t button_first_detected = 1;

  unsigned long now = millis();
  unsigned int buttonTest_OnTime = buttonTest.process(now);

  /**
     buttonTest_OnTime will either be 0 or a positive non-zero value
     representing the last time the the button was initially pressed.
     If the value is non-zero, then we need to handle the case that the
     button is pressed or held. In this specific implementation, handling of
     the pressed state isn't done here, but rather in the else section of
     this if-statement, that is, when buttonTest_OnTime becomes 0.
  */
  if (buttonTest_OnTime) {

    /**
       If button_first_detected is 1 then we are in the first function
       loop in which a new button press has been detected, in which case
       the millis need to be recorded to test a button hold against. Afterward
       button_first_detected is set to 0. It is reset to 1 when the button
       is released.
    */
    if (button_first_detected == 1) {
      curr_button_held = millis();
    }

    button_first_detected = 0;

    /**
       curr_button_held is only set to -1 in the event a HELD state was
       detected. Given that buttonTest_OnTime is non-zero, then the button
       must be in a pressed or held state. Below we test for the HELD state
       and if a held state is detected then curr_button_held is set to -1
       so that current_state is only set to BUTTON_HELD once, rather than
       continuously returning BUTTON_HELD while held. After a hold has been
       detected once, curr_button_held is set to -1 to reset the state to
       BUTTON_OFF since the 'hold' is technically complete.
    */
    if (curr_button_held == -1) {
      current_state = BUTTON_OFF;
    }

    /**
       Test if the difference between the current millis recorded and the
       millis recorded on the first detected button press-down are greater than
       the duration we have defined for the button hold.
    */
    else if ((millis() - curr_button_held) > BUTTON_HOLD_DURATION)
    {
      curr_button_held = -1; // Resetting after the first HELD detected.
      Serial.println("Held");
      current_state = BUTTON_HELD;
    }
  }

  /**
     When buttonTest_onTime is 0 then either the button is inactive or a
     button release has just been detected.
  */
  else {
    button_first_detected = 1; // Resetting for the next time a press-down is detected

    /**
       The current state is set to OFF here so that if directly below that a
       button release is detected, we can set the current state to PRESSED.
       If a button release isn't detected, then keep and return this OFF state.
    */
    current_state = BUTTON_OFF;

    /**
       The state count is maintained withing the InputDebounce button. This number
       increments every time the library detects a successful press. So to detect
       a press for our purposes, we get the count and see if it changed from the last
       count recorded. If the count has incremented, we need to handle a new button press.
    */
    unsigned int count = buttonTest.getStatePressedCount();
    if (buttonTest_StateOnCount != count) {
      buttonTest_StateOnCount = count; // Update the count saved to test against the next count.
      Serial.println("pressed");
      current_state = BUTTON_PRESSED;
    }
  }

  return current_state;
}
