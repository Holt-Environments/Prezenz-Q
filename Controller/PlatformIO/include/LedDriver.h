/**
 * @file LedDriver.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-28
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#define LED_R 5
#define LED_G 6
#define LED_B 3
#define LED_W 9
#define SIN_PERIOD 3000 // ms
#define TRANSITION_MAX_INDEX 500 // NOT MS!!!!

namespace HoltEnvironments {

namespace PrezenzQ {

class LedDriver {

public:

  /**
   * The state of the driver. Each state corresponds to a looping function in the driver.
   */
  enum State { OFF, WAITING, ON };

  static bool init();
  static void setState(State _state);
  static void update();

private:

  /**
   * Contains the Arduino PWM values that will be sent to the corresponding PWM pins
   * to control the brightness of the led channels.
   */
  typedef struct led_color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char w;
  } LedColor;

  typedef void (*ledValueGeneratingFunction)(long _current_millis, LedColor *_color);

  typedef struct generator_state {
    ledValueGeneratingFunction current;
    ledValueGeneratingFunction previous;
  } GeneratorState;

  static GeneratorState led_value_generator;

  static void updateGenerator(ledValueGeneratingFunction _function);

  static void ledLoopOff(long _current_millis, LedColor *_color);
  static void ledLoopWaiting(long _current_millis, LedColor *_color);
  static void ledLoopOn(long _current_millis, LedColor *_color);

  static int transition_index;
  static void resetTransitionIndex();

  static void setLedColor(LedColor *_current_color);

  static float sin_wave(unsigned long _millis, int _period, int _offset);
  static unsigned char lerp(unsigned char _a, unsigned char _b, float _t);

  static void report(LedColor *_current_color);
};

} //  PrezenzQ

} //  HoltEnvironments

