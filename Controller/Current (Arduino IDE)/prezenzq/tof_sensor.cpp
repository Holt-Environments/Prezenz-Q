
/**
   =======================================================================
   PrezenzQ Controller - tof_sensor.cpp

   Holt Environments
   Author: Anthony Mesa
   Date: 05/24/2022

   =======================================================================
*/

#include "tof_sensor.h"

//  Object for working with TOF sensor
static VL53L1X sensor;
static int sensor_initialized;

//If there is an issue with the sensor initialization,
//then the Arduino will loop continuously in an error
//state
int tof_sensor_init()
{
  Wire.begin();
  Wire.setClock(SENSOR_I2C_CLOCK);

  sensor.setTimeout(SENSOR_TIMEOUT);

  sensor_initialized = sensor.init();

  sensor.setROISize(4, 4);

  if (sensor_initialized)
  {
    sensor.setDistanceMode(VL53L1X::Short);
    sensor.setMeasurementTimingBudget(SENSOR_TIMING_BUDGET);
    sensor.startContinuous(SENSOR_WAIT);
    return 1;
  } else {
    return 0;
  }
}

bool object_detected_in_range(int _sensor_value)
{
  return _sensor_value < SENSOR_TRIGGER_DISTANCE;
}

int tof_sensor_update()
{
  static char receipt;
  static bool detection_state;
  static int detection_buffer;
  static bool trigger_state;
  static int sensor_value;
  static int previous_signal_status_id;

  if (sensor.dataReady()) {
    sensor_value = sensor.read(false);
  }

  if (sensor.timeoutOccurred())
  {
    return -1;
  }

  int signal_status_id = sensor.ranging_data.range_status;
  String signal_status = VL53L1X::rangeStatusToString(sensor.ranging_data.range_status);

  bool is_error;

  if (signal_status_id != previous_signal_status_id) {
    Serial.print(signal_status);
    Serial.print("- sensor value: ");
    Serial.println(sensor_value);
    previous_signal_status_id = signal_status_id;
  }

  if (signal_status.equals("signal fail"))
  {
    detection_buffer = 0;

    if ((trigger_state == true)) {
      return 1;
    }
  } else if (signal_status_id == 0) {
    detection_state = object_detected_in_range(sensor_value);

    if (detection_state)
      detection_buffer++;
    else if (!detection_state)
      detection_buffer = 0;

    if ((trigger_state == false) && (detection_buffer > SENSOR_DETECTION_BUFFER_MAX))
    {
      trigger_state = true;
      return 0;
    }
    else if ((trigger_state == true) && (detection_buffer == 0))
    {
      trigger_state = false;
      return 1;
    }
  }
}
