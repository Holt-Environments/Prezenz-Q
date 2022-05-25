
/**
   =======================================================================
   PrezenzQ Controller - led_strip.h

   Holt Environments
   Author: Anthony Mesa
   Date: 05/24/2022

   led_strip.h defines those functions related to controlling the connected
   LED strip. Currently this iteration of the controller is only designed
   to control a single channel of color. The device as of this date is
   physically incapable of controlling more channels. To control all RGBW
   channels, 3 more MOSFETS need to be fitted to the device and wired
   accordingly. To get the model number for the mosfets used in this device,
   you can check the hardware schematics here:

   https://github.com/Holt-Environments/Prezenz-Q/tree/master/Controller/Schematics

   =======================================================================
*/

#ifndef ARDUINO_LIB
#define ARDUINO_LIB
#include <Arduino.h>
#endif

/**
   The definitions for R, G, and W aren't used in this code. They are left
   over by work done in a seperate refactor. The code for this refactor
   is here:

   LedDriver.h: https://github.com/Holt-Environments/Prezenz-Q/blob/refactor2/Controller/PlatformIO/include/LedDriver.h
   LedDriver.cpp: https://github.com/Holt-Environments/Prezenz-Q/blob/refactor2/Controller/PlatformIO/src/LedDriver.cpp

   In this refactor, 4 color channel LED strip control is enabled, as well
   as graduated transitions between LED strip states using linear interpolation.
*/
#define LED_R -1;
#define LED_G -1
#define LED_B 3
#define LED_W -1

#define LED_PERIOD 3000 // ms

enum led_state {
  LED_STRIP_OFF,
  LED_STRIP_WAITING,
  LED_STRIP_ON
};

/**
   Needs to be called before any other LED related functions are called.
*/
void led_strip_init();

/**
   Sets the command to be evaluated when led_strip_update is next called.
*/
void led_strip_set_command(enum led_state _new_state);
void led_strip_update();
