
/*
LedDriver.h
AUTHOR: Anthony Mesa
*/

#pragma once

namespace HoltEnvironments {

namespace PrezenzQ {

class LedDriver {

public:

  enum State { OFF, WAITING, ON };

  static int init();
  static void setState(State _state);
  static void update();

private:

  typedef struct led_color_s {
    int r;
    int g;
    int b;
    int w;
  } LedColor;

  static void (*generateLedValue)(LedColor *_color);
  static void ledLoopOff(LedColor *_color);
  static void ledLoopWaiting(LedColor *_color);
  static void ledLoopOn(LedColor *_color);

};

} //  PrezenzQ

} //  HoltEnvironments

