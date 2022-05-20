
#include "LedDriver.h"
#include "TofSensor.h"
#include "HC05Driver.h"

#define ARDUINO_SERIAL_BAUD 9600
#define ARDUINO_LED 13

using HoltEnvironments::PrezenzQ::TofSensor;
using HoltEnvironments::PrezenzQ::LedDriver;
using HoltEnvironments::PrezenzQ::HC05Driver;

void onDetected() {
  Serial.println("detected!");
  byte cmd[4] = { '[', 0x41, 0x01, ']' };
  HC05Driver::sendByteData(cmd, 4);
}

void onNotDetected() {
  Serial.println("not detected!");
  byte cmd[4] = { '[', 0x41, 0x00, ']' };
  HC05Driver::sendByteData(cmd, 4);
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
  pinMode(ARDUINO_LED, OUTPUT);

  if(!TofSensor::init(&onDetected, &onNotDetected)){
    while(true){
      digitalWrite(ARDUINO_LED, HIGH);
      delay(100);
      digitalWrite(ARDUINO_LED, LOW);
      delay(100);
      digitalWrite(ARDUINO_LED, HIGH);
      delay(100);
      digitalWrite(ARDUINO_LED, LOW);
      delay(100);
      digitalWrite(ARDUINO_LED, HIGH);
      delay(100);
      digitalWrite(ARDUINO_LED, LOW);
      delay(1000);
    }
  };
  
  LedDriver::init();
  LedDriver::setState(LedDriver::State::WAITING);

  if(!HC05Driver::init())
  {
    while(true)
    {
      HC05Driver::debugUpdate();
    }
  }
}

void loop()
{
  // testLedTransition();

  TofSensor::update();
  LedDriver::update();
  HC05Driver::update();
}

