
#include <Wire.h>
#include <VL53L1X.h>

//  XSHUT isnt necessary and isn't used in this code but
//  here it is defined for possible future use. XSHUT becomes
//  important if ever we need to change the I2C address of the
//  sensor before it boots up.
#define SENSOR_XSHUT 6
#define SENSOR_I2C_CLOCK 400000

//  Maximum response time for sensor before sensor returns an error
#define SENSOR_TIMEOUT 0 // ms

//  Sensor_timing_budget_ms controlls how long the sensor is actively reading.
//  Sensor_wait_ms is the amount of ms that the sensor will NOT read between successive
//  intervals.
#define SENSOR_TIMING_BUDGET_MS 200 // ms
#define SENSOR_WAIT_MS 200 // ms
#define SENSOR_TIMING_BUDGET ((long)SENSOR_TIMING_BUDGET_MS * 1000) // 50,000 us = 50 ms
#define SENSOR_WAIT (SENSOR_TIMING_BUDGET_MS + SENSOR_WAIT_MS)

//  The trigger distance does not have a "real world equivalent" yet. This is a unit-less number.
#define SENSOR_TRIGGER_DISTANCE 350
#define DEFAULT_MAX_DISTANCE 1000

//  Sensor should be unique for each device. values are 0-255 char values, where alphabet chars
//  are highly reccomended.
#define SENSOR_ID 'D'
#define SENSOR_ERROR_INIT_FAILED 0x30 // '0' Error value for when sensor initialization has timed out.
#define SENSOR_ERROR_TIMEOUT 0x31 // '1' Error value for when sensor reading has timed out.
#define SENSOR_DETECTION_BUFFER_MAX 75

int tof_sensor_init();
int tof_sensor_update();
