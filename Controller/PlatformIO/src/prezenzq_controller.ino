
/*
================================================================================
    Libraries
================================================================================
*/

#include <SoftwareSerial.h>
#include <Wire.h>
#include <VL53L1X.h>
#include "PrezenzQController.h"

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
#define SENSOR_XSHUT 6
#define SENSOR_I2C_CLOCK 400000
#define SENSOR_TIMEOUT 0 // ms
#define SENSOR_TIMING_BUDGET_MS 200 // ms
#define SENSOR_WAIT_MS 200 // ms
#define SENSOR_TIMING_BUDGET ((long)SENSOR_TIMING_BUDGET_MS * 1000) // us
#define SENSOR_WAIT (SENSOR_TIMING_BUDGET_MS + SENSOR_WAIT_MS)
#define SENSOR_TRIGGER_DISTANCE 350
#define DEFAULT_MAX_DISTANCE 1000
#define SENSOR_ID 'E'
#define SENSOR_ERROR_INIT_FAILED 0x30 // '0'
#define SENSOR_ERROR_TIMEOUT 0x31 // '1'
#define SENSOR_DETECTION_BUFFER_MAX 75
#define LED_R 0
#define LED_G 0
#define LED_B 3
#define LED_W 0
#define LED_PERIOD 3000 // ms
#define ARDUINO_LED 13
#define ARDUINO_SERIAL_BAUD 9600

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

void setup()
{
  Serial.println();
}

void loop()
{

}

// void sensorInitFailed()
// {
//   digitalWrite(ARDUINO_LED, HIGH);
//   delay(50);
//   digitalWrite(ARDUINO_LED, LOW);
//   delay(100);
//   digitalWrite(ARDUINO_LED, HIGH);
//   delay(50);
//   digitalWrite(ARDUINO_LED, LOW);
//   delay(100);
//   digitalWrite(ARDUINO_LED, HIGH);
//   delay(50);
//   digitalWrite(ARDUINO_LED, LOW);
//   delay(1000);
// }

// //  If there is an issue with the sensor initialization,
// //  then the Arduino will loop continuously in an error
// //  state
// bool initSensor()
// {
//   Wire.begin();
//   Wire.setClock(SENSOR_I2C_CLOCK);
  
//   sensor.setTimeout(SENSOR_TIMEOUT);

//   sensor_initialized = sensor.init();

//   sensor.setROISize(4, 4);
  
//   if (sensor_initialized)
//   {
//     sensor.setDistanceMode(VL53L1X::Short);
//     sensor.setMeasurementTimingBudget(SENSOR_TIMING_BUDGET);
//     sensor.startContinuous(SENSOR_WAIT);
//     return true;
//   } else {
//     return false;
//   }
// }

// bool object_detected_in_range(int _sensor_value)
// {
//   return _sensor_value < SENSOR_TRIGGER_DISTANCE;
// }

// void ledOff()
// {
//   digitalWrite(ARDUINO_LED, LOW);
//   analogWrite(LED_B, 0);
// }

// void ledOn()
// {
//   digitalWrite(ARDUINO_LED, HIGH);
//   analogWrite(LED_B, 255);
// }

// void ledWaiting()
// {
//   double value = millis() % LED_PERIOD;
//   double test = (.5 * sin(2 * PI * (value / LED_PERIOD))) + .5;

//   if(test > .5)
//     digitalWrite(ARDUINO_LED, HIGH);
//   if(test <= .5)
//     digitalWrite(ARDUINO_LED, LOW);
    
//   analogWrite(LED_B, (int)(test * 255));
// }

// void control_led()
// {
//   (*ledCommand)();
// }

// void handleCommand(char* command)
// {
//   switch(*command)
//   {
//     case 'F':
//       ledCommand = &ledOff;
//       break;
//     case 'W':
//       ledCommand = &ledWaiting;
//       break;
//     case 'N':
//       ledCommand = &ledOn;
//       break;
//   }
// }

// void process_waiting_commands()
// {
//   static char receipt;
//   if(HC05.available())
//   {
//     receipt = HC05.read();
//     handleCommand(&receipt);
//   }
// }

// void defaultMode()
// {
//   control_led();
// }

// void debugMode()
// {
//   control_led();

//   if (HC05.available())
//     Serial.write(HC05.read());
//   if (Serial.available())
//     HC05.write(Serial.read());
// }

// void handleSensor()
// {
//   static char receipt;
//   static bool detection_state;
//   static int detection_buffer;
//   static bool trigger_state;
//   static int sensor_value;

//   process_waiting_commands();
  
//   if(sensor.dataReady()){
//     sensor_value = sensor.read(false);
//   }

//   if(sensor.timeoutOccurred())
//   { 
//     Serial.println("timeout");
//     HC05.write(SENSOR_ERROR_TIMEOUT);
//     return;
//   }

//   int signal_status_id = sensor.ranging_data.range_status;
//   String signal_status = VL53L1X::rangeStatusToString(sensor.ranging_data.range_status);
  
//   bool is_error;

//   Serial.println(signal_status);

//   if(signal_status.equals("signal fail"))
//   {
//     detection_buffer = 0;

//     if((trigger_state == true)){
//       Serial.println("not detected");
//       trigger_state = false;
//       byte cmd[4] = { '[', SENSOR_ID, 0x00, ']' };
//       HC05.write(cmd, 4);
//     }
//   } else if (signal_status_id == 0){
//     Serial.println(sensor_value);

//     detection_state = object_detected_in_range(sensor_value);
  
//     if(detection_state)
//       detection_buffer++;
//     else if(!detection_state)
//       detection_buffer = 0;
  
//     if((trigger_state == false) && (detection_buffer > SENSOR_DETECTION_BUFFER_MAX))
//     {
//       Serial.println("detected");
//       trigger_state = true;
//       byte cmd[4] = { '[', SENSOR_ID, 0x01, ']' };
//       HC05.write(cmd, 4);
//     } 
//     else if ((trigger_state == true) && (detection_buffer == 0))
//     {
//       Serial.println("not detected");
//       trigger_state = false;
//       byte cmd[4] = { '[', SENSOR_ID, 0x00, ']' };
//       HC05.write(cmd, 4);
//     }
//   }
  
//   return;
// }

// String error;

// //  Arduino
// //================================================
// void setup() 
// {
//   pinMode(ARDUINO_LED, OUTPUT);
//   pinMode(LED_B, OUTPUT);
//   pinMode(BT_DEBUG_ENABLED, INPUT);

//   digitalWrite(ARDUINO_LED, LOW);
//   Serial.begin(ARDUINO_SERIAL_BAUD);

//   //  The sensor is working, so next check that the 
//   //  device has been switch to development mode.

//   if(digitalRead(BT_DEBUG_ENABLED) == HIGH)
//   {
//     error = "bt debug enabled\n";
//     ledCommand = &ledWaiting;
//     HC05.begin(38400);
//     executeCode = &debugMode;
//   } else if(!initSensor())  //  Initialize the sensor and see if it is working.
//   {
//     error = "sensor not initialized\n";
//     executeCode = &sensorInitFailed;
//   } else {   //  The device is working properly in normal mode.
//     error = "everything fine\n";
//     ledCommand = &ledOff;
//     HC05.begin(BT_BAUD);
//     executeCode = &defaultMode;
//   }
// }

// void loop()
// {
//   if(executeCode != NULL)
//     (*executeCode)();

//   handleSensor();
// }
