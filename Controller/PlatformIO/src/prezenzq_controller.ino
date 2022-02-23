
// #include <SoftwareSerial.h>
// #include "PrezenzQController.h"
#include "LedDriver.h"
#include "TofSensor.h"

#define BT_RX 8
#define BT_TX 7
#define BT_BAUD 9600
#define BT_DEBUG_ENABLED A0
#define DEFAULT_MAX_DISTANCE 1000
#define SENSOR_ID 'E'
#define SENSOR_ERROR_INIT_FAILED 0x30 // '0'
#define SENSOR_ERROR_TIMEOUT 0x31 // '1'

#define ARDUINO_SERIAL_BAUD 9600

using HoltEnvironments::PrezenzQ::TofSensor;
using HoltEnvironments::PrezenzQ::LedDriver;

// SoftwareSerial HC05(BT_RX, BT_TX);

void onDetected() {
  Serial.println("detected!");
  LedDriver::setState(LedDriver::State::WAITING);
}

void onNotDetected() {
  Serial.println("not detected!");
  LedDriver::setState(LedDriver::State::OFF);
}

void setup()
{
  Serial.begin(ARDUINO_SERIAL_BAUD);
  TofSensor::init(&onDetected, &onNotDetected);
  LedDriver::init();
  LedDriver::setState(LedDriver::State::WAITING);
}

void loop()
{
  TofSensor::update();
  LedDriver::update();
}

