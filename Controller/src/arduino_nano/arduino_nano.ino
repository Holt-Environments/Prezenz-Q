
//================================================
//  Libraries
//================================================
#include <SoftwareSerial.h>
#include <Wire.h>
#include <VL53L1X.h>
#include <VariableTimedAction.h>

//================================================
//  Definitions
//================================================

//  Any digital pins should be able to be used for this. The number is the
//  digital pin on the Arduino corresponding to the Arduino's digital RX and TX.
#define BT_RX 8 // WORKS LIKE THIS DONT CHANGE
#define BT_TX 7
#define BT_BAUD 9600
#define BT_DEBUG_ENABLED A0

//  XSHUT isnt necessary and isn't used in this code but 
//  here it is defined for possible future use. XSHUT becomes
//  important if ever we need to change the I2C address of the
//  sensor before it boots up.
#define SENSOR_XSHUT 6
#define SENSOR_I2C_CLOCK 400000

//  Maximum response time for sensor before sensor returns an error
#define SENSOR_TIMEOUT 500 // ms

//  Sensor_timing_budget_ms controlls how long the sensor is actively reading.
//  Sensor_wait_ms is the amount of ms that the sensor will NOT read between successive
//  intervals.
#define SENSOR_TIMING_BUDGET_MS 50 // ms
#define SENSOR_WAIT_MS 0 // ms
#define SENSOR_TIMING_BUDGET ((long)SENSOR_TIMING_BUDGET_MS * 1000) // 50,000 us = 50 ms
#define SENSOR_WAIT (SENSOR_TIMING_BUDGET_MS + SENSOR_WAIT_MS)

//  The trigger distance does not have a "real world equivalent" yet. This is a unit-less number.
#define SENSOR_TRIGGER_DISTANCE 600

//  Sensor should be unique for each device. values are 0-255 char values, where alphabet chars
//  are highly reccomended.
#define SENSOR_ID 'B'
#define SENSOR_ERROR_INIT_FAILED 0x30 // '0' Error value for when sensor initialization has timed out.
#define SENSOR_ERROR_TIMEOUT 0x31 // '1' Error value for when sensor reading has timed out.
#define SENSOR_DETECTION_BUFFER_MAX 20

//  When not using a specific led color channel (When that led color channel is not wired/connected to 
//  a corresponding Arduino digital pin) its definition should be set to (-1).
#define LED_R -1;
#define LED_G -1
#define LED_B 3
#define LED_W -1
#define LED_PERIOD 3000 // ms

//  Arduino specific
#define ARDUINO_LED 13
#define ARDUINO_SERIAL_BAUD 9600

//================================================
//  Global Variables
//================================================

//  Software serial for bluetooth device so that
//  we can still use the normal serial port for
//  USB communication when plugged in.
SoftwareSerial HC05(BT_RX, BT_TX);

//  Object for working with TOF sensor
VL53L1X sensor;

//  Function pointer for handling incoming LED commands
void (*volatile ledCommand)();
void (*volatile executeCode)() = NULL;
bool sensor_initialized;

//================================================
//  Functions
//================================================

void sensorInitFailed()
{
  digitalWrite(ARDUINO_LED, HIGH);
  delay(50);
  digitalWrite(ARDUINO_LED, LOW);
  delay(100);
  digitalWrite(ARDUINO_LED, HIGH);
  delay(50);
  digitalWrite(ARDUINO_LED, LOW);
  delay(100);
  digitalWrite(ARDUINO_LED, HIGH);
  delay(50);
  digitalWrite(ARDUINO_LED, LOW);
  delay(1000);
}

//  If there is an issue with the sensor initialization,
//  then the Arduino will loop continuously in an error
//  state
bool initSensor()
{
  Wire.begin();
  Wire.setClock(SENSOR_I2C_CLOCK);
  
  sensor.setTimeout(SENSOR_TIMEOUT);

  sensor_initialized = sensor.init();
  
  if (sensor_initialized)
  {
    sensor.setDistanceMode(VL53L1X::Long);
    sensor.setMeasurementTimingBudget(SENSOR_TIMING_BUDGET);
    sensor.startContinuous(SENSOR_WAIT);
    return true;
  } else {
    return false;
  }
}

bool object_detected_in_range(int _sensor_value)
{
  return _sensor_value < SENSOR_TRIGGER_DISTANCE;
}

void ledOff()
{
  digitalWrite(ARDUINO_LED, LOW);
  analogWrite(LED_B, 0);
}

void ledOn()
{
  digitalWrite(ARDUINO_LED, HIGH);
  analogWrite(LED_B, 255);
}

void ledWaiting()
{
  double value = millis() % LED_PERIOD;
  double test = (.5 * sin(2 * PI * (value / LED_PERIOD))) + .5;

  if(test > .5)
    digitalWrite(ARDUINO_LED, HIGH);
  if(test <= .5)
    digitalWrite(ARDUINO_LED, LOW);
    
  analogWrite(LED_B, (int)(test * 255));
}

void control_led()
{
  (*ledCommand)();
}

void handleCommand(char* command)
{
  switch(*command)
  {
    case 'F':
      ledCommand = &ledOff;
      break;
    case 'W':
      ledCommand = &ledWaiting;
      break;
    case 'N':
      ledCommand = &ledOn;
      break;
  }
}

void process_waiting_commands()
{
  static char receipt;
  if(HC05.available())
  {
    receipt = HC05.read();
    handleCommand(&receipt);
  }
}

void defaultMode()
{
  control_led();
}

void debugMode()
{
  control_led();

  if (HC05.available())
    Serial.write(HC05.read());
  if (Serial.available())
    HC05.write(Serial.read());
}

class DetectionController : public VariableTimedAction{
private:
  unsigned long run()
  {
    static char receipt;
    static bool detection_state;
    static int detection_buffer;
    static bool trigger_state;

    process_waiting_commands();
      
    int sensor_value = sensor.read();
  
    if(sensor.timeoutOccurred())
    { 
      HC05.write(SENSOR_ERROR_TIMEOUT);
      return;
    }
  
    detection_state = object_detected_in_range(sensor_value);
  
    if(detection_state)
      detection_buffer++;
    else if(!detection_state)
      detection_buffer = 0;
  
    if((trigger_state == false) && (detection_buffer > SENSOR_DETECTION_BUFFER_MAX))
    {
      trigger_state = true;
      byte cmd[4] = { '[', SENSOR_ID, 0x01, ']' };
      HC05.write(cmd, 4);
    } 
    else if ((trigger_state == true) && (detection_buffer == 0))
    {
      trigger_state = false;
      byte cmd[4] = { '[', SENSOR_ID, 0x00, ']' };
      HC05.write(cmd, 4);
    }

    return 0;
  }
};

DetectionController detection_controller;

String error;

//  Arduino
//================================================
void setup() 
{
  pinMode(ARDUINO_LED, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(BT_DEBUG_ENABLED, INPUT);

  digitalWrite(ARDUINO_LED, LOW);
  Serial.begin(ARDUINO_SERIAL_BAUD);

  //  The sensor is working, so next check that the 
  //  device has been switch to development mode.

  if(digitalRead(BT_DEBUG_ENABLED) == HIGH)
  {
    error = "bt debug enabled\n";
    ledCommand = &ledWaiting;
    HC05.begin(38400);
    executeCode = &debugMode;
  } else if(!initSensor())  //  Initialize the sensor and see if it is working.
  {
    error = "sensor not initialized\n";
    executeCode = &sensorInitFailed;
  } else {   //  The device is working properly in normal mode.
    error = "everything fine\n";
    ledCommand = &ledOff;
    HC05.begin(BT_BAUD);
    executeCode = &defaultMode;
    detection_controller.start(50);
  }
}

void loop()
{
  if(executeCode != NULL)
    (*executeCode)();

  VariableTimedAction::updateActions(); // C++
}
