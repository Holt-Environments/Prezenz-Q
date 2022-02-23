
#include "TofSensor.h"
#include <Wire.h>

#define SENSOR_XSHUT 6
#define SENSOR_I2C_CLOCK 400000
#define SENSOR_TIMEOUT 0 // ms
#define SENSOR_TIMING_BUDGET_MS 200 // ms
#define SENSOR_WAIT_MS 200 // ms
#define SENSOR_TIMING_BUDGET ((long)SENSOR_TIMING_BUDGET_MS * 1000) // 50,000 us = 50 ms
#define SENSOR_WAIT (SENSOR_TIMING_BUDGET_MS + SENSOR_WAIT_MS)
#define SENSOR_TRIGGER_DISTANCE 350
#define SENSOR_DETECTION_BUFFER_MAX 75

using HoltEnvironments::PrezenzQ::TofSensor;

VL53L1X TofSensor::sensor;
TofSensor::callback TofSensor::on_detected = NULL;
TofSensor::callback TofSensor::on_not_detected = NULL;

/**
 * @brief Initializes the sensor and I2C library that allows us to interface
 * with the sensor.
 * 
 * @return int Return value is -1 if the sensor fails to initialize,
 * and is 0 otherwise.
 */
int TofSensor::init(TofSensor::callback _on_detected, TofSensor::callback _on_not_detected) {

  on_detected = _on_detected;
  on_not_detected = _on_not_detected;

  Wire.begin();
  Wire.setClock(SENSOR_I2C_CLOCK);
  
  sensor.setTimeout(SENSOR_TIMEOUT);

  bool sensor_is_initialized = sensor.init();

  if (sensor_is_initialized)
  {
    sensor.setROISize(4, 4);
    sensor.setDistanceMode(VL53L1X::Short);
    sensor.setMeasurementTimingBudget(SENSOR_TIMING_BUDGET);
    sensor.startContinuous(SENSOR_WAIT);
    return 0;
  } else {
    return -1;
  }
}

bool TofSensor::object_detected_in_range(int _sensor_value)
{
  return _sensor_value < SENSOR_TRIGGER_DISTANCE;
}

void TofSensor::update() {
  static bool detection_state;
  static int detection_buffer;
  static bool trigger_state;
  static int sensor_value;
  
  if(sensor.dataReady()){
    sensor_value = sensor.read(false);
    Serial.println(sensor_value);
  } else {
    return;
  }

  if(sensor.timeoutOccurred())
  { 
    Serial.println("timeout");
    return;
  }

  int signal_status_id = sensor.ranging_data.range_status;
  
  if(signal_status_id != 0)
  {
    Serial.println("failed");
    detection_buffer = 0;

    if((trigger_state == true)){
      trigger_state = false;
      return;
    }
  } else if (signal_status_id == 0){
    Serial.println("success");

    detection_state = object_detected_in_range(sensor_value);
  
    Serial.print("detection_state: ");
    Serial.println(detection_state);

    if(detection_state)
      detection_buffer++;
    else if(!detection_state)
      detection_buffer = 0;
  
    if((trigger_state == false) && (detection_buffer > SENSOR_DETECTION_BUFFER_MAX))
    {
      trigger_state = true;
      Serial.println("triggered on");
      (*on_detected)();
    } 
    else if ((trigger_state == true) && (detection_buffer == 0))
    {
      trigger_state = false;
      Serial.println("triggered off");
      (*on_not_detected)();
    }
  }
}
