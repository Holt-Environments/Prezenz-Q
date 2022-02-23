
/*
TofSensor.h
AUTHOR: Anthony Mesa
*/

#pragma once

#include <VL53L1X.h>

#define SENSOR_XSHUT 6
#define SENSOR_I2C_CLOCK 400000
#define SENSOR_TIMEOUT 0 // ms
#define SENSOR_TIMING_BUDGET_MS 200 // ms
#define SENSOR_WAIT_MS 200 // ms
#define SENSOR_TIMING_BUDGET ((long)SENSOR_TIMING_BUDGET_MS * 1000) // 50,000 us = 50 ms
#define SENSOR_WAIT (SENSOR_TIMING_BUDGET_MS + SENSOR_WAIT_MS)
#define SENSOR_TRIGGER_DISTANCE 350
#define SENSOR_DETECTION_BUFFER_MAX 75

namespace HoltEnvironments {

namespace PrezenzQ {

class TofSensor {
public:

  typedef void (*volatile callback)();

  static int init(callback _on_detected, callback _on_undetected);
  static void update();

private:

  static VL53L1X sensor;
  static callback on_detected;
  static callback on_undetected;

  static bool object_detected_in_range(int _sensor_value);
};

} //  PrezenzQ

} //  HoltEnvironments