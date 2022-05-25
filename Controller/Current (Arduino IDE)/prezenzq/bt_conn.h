
/**
   =======================================================================
   PrezenzQ Controller - bt_conn.h

   Holt Environments
   Author: Anthony Mesa
   Date: 05/24/2022

   bt_conn provides functionality for communication over serial Bluetooth.
   Software serial is used so as not to interfere with serial USB
   communication.

   Currently, an HC05 Bluetooth module is being used for this controller.

   This module is expected to be handled in one of two modes, NORMAL mode,
   and DEBUGGING mode. In NORMAL mode, the module works as expected sending
   and receiving data back and forth between any connected device(s). In
   DEBUGGING mode however, the device doesn't communicate with any other
   devices. All serial USB input is diverted to communicate with the module
   directly, and vis-versa. This is so that AT commands can be sent to the
   device for configuration of the bluetooth module.

   BLE isn't supported, but that is a goal for the future.

   =======================================================================
*/

#ifndef ARDUINO_LIB
#define ARDUINO_LIB
#include <Arduino.h>
#endif

#include <SoftwareSerial.h>

/**
   Any digital pins should be able to be used for this. The number is the
   digital pin on the Arduino corresponding to the Arduino's digital RX and TX.
*/
#define BT_RX 8 // WORKS LIKE THIS DONT CHANGE
#define BT_TX 7

#define BT_BAUD 9600
#define BT_DEBUG_BAUD 38400

/**
   This analog pin is used on the controller due to the proximity of the
   debug switch to the Arduino Nano.
*/
#define BT_DEBUG_ENABLED A0

/**
   Currently this function only initializes whether the bluetooth module
   is to start in debug mode or not. This function should always be called
   before any other bt_conn functions are called. Not sure what would happen
   if bt_conn_init() isn't called first other than the debugging mode may not
   be available to access, but I am also not going to test it to find out.
*/
int bt_conn_init();

/**
   Starts the bluetooth device in NORMAL or DEBUGGING mode.
*/
void bt_conn_start();
void bt_conn_send(int _data);
void bt_conn_send(byte* _data, int _len);
int bt_conn_available();
int bt_conn_read();
int bt_conn_get_command();
