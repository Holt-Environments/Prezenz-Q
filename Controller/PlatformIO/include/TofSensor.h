
/*
TofSensor.h
AUTHOR: Anthony Mesa
*/

#pragma once

#include <VL53L1X.h>

namespace HoltEnvironments {

namespace PrezenzQ {

class TofSensor {
public:

  typedef void (*callback)();

  static int init(callback _on_detected, callback _on_not_detected);
  static void update();

private:

  static VL53L1X sensor;
  static callback on_detected;
  static callback on_not_detected;

  static bool object_detected_in_range(int _sensor_value);
};

} //  PrezenzQ

} //  HoltEnvironments