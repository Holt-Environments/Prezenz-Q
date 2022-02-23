
/*
================================================================================
    Libraries
================================================================================
*/

#include <SoftwareSerial.h>
#include "PrezenzQController.h"
#include "LedDriver.h"
#include "TofSensor.h"

/*
================================================================================
  Definitions
================================================================================

BT_RX, BT_TX: 
Any digital pins should be able to be used for this. The number is the digital 
pin on the Arduino corresponding to the Arduino's digital RX and TX.

SENSOR_XSHUT: 
XSHUT isnt necessary and isn't used in this code but here it is defined for 
possible future use. XSHUT becomes important if ever we need to change the I2C 
address of the sensor before it boots up.

SENSOR_TIMEOUT:
Maximum response time for sensor before sensor returns an error

SENSOR_TIMING_BUDGET_MS, SENSOR_WAIT_MS:
Sensor_timing_budget_ms controlls how long the sensor is actively reading. 
Sensor_wait_ms is the amount of ms that the sensor will NOT read between 
successive intervals.

SENSOR_TRIGGER_DISTANCE:
The trigger distance does not have a "real world equivalent" yet. This is a 
unit-less number.

SENSOR_ID:
Sensor should be unique for each device. values are 0-255 char values, where 
alphabet chars are highly reccomended.

*/

#define BT_RX 8
#define BT_TX 7
#define BT_BAUD 9600
#define BT_DEBUG_ENABLED A0
#define DEFAULT_MAX_DISTANCE 1000
#define SENSOR_ID 'E'
#define SENSOR_ERROR_INIT_FAILED 0x30 // '0'
#define SENSOR_ERROR_TIMEOUT 0x31 // '1'
#define ARDUINO_LED 13
#define ARDUINO_SERIAL_BAUD 9600

/*
================================================================================
    Namespaces
================================================================================
*/

using HoltEnvironments::PrezenzQ::TofSensor;

/*
================================================================================
    Global Variables
================================================================================
*/

SoftwareSerial HC05(BT_RX, BT_TX);
VL53L1X sensor;
void (*volatile ledCommand)();
void (*volatile executeCode)() = NULL;
bool sensor_initialized;

/*
================================================================================
    Arduino
================================================================================
*/

void onDetected() {
  Serial.println("detected!");
}

void onNotDetected() {
  Serial.println("not detected!");
}

void setup()
{
  Serial.begin(ARDUINO_SERIAL_BAUD);
  Serial.println(TofSensor::init(&onDetected, &onNotDetected);
}

void loop()
{
  TofSensor::update();
}

