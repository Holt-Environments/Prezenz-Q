
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
  LedDriver::setState(LedDriver::State::ON);
}

void onNotDetected() {
  Serial.println("not detected!");
  LedDriver::setState(LedDriver::State::OFF);
}

void testLedTransition() {
  static unsigned long current;
  static unsigned long time;
  static bool state;

  current = millis();
  time = current % 10000;
  
  if((state == true) && (time < 5000)){
    state = false;
    LedDriver::setState(LedDriver::State::OFF);
  }

  if((state == false) && (time >= 5000)){
    state = true;
    LedDriver::setState(LedDriver::State::ON);
  }
}

void setup()
{
  Serial.begin(ARDUINO_SERIAL_BAUD);
  
  if(!TofSensor::init(&onDetected, &onNotDetected)){
    while(true){
      Serial.println("sensor failed to initialize");
    }
  };
  
  LedDriver::init();
  LedDriver::setState(LedDriver::State::WAITING);
}

void loop()
{
  TofSensor::update();

  // testLedTransition();

  LedDriver::update();
}

