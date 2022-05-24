
#ifndef ARDUINO_LIB
#define ARDUINO_LIB
#include <Arduino.h>
#endif

#include <SoftwareSerial.h>

//  Any digital pins should be able to be used for this. The number is the
//  digital pin on the Arduino corresponding to the Arduino's digital RX and TX.
#define BT_RX 8 // WORKS LIKE THIS DONT CHANGE
#define BT_TX 7
#define BT_BAUD 9600
#define BT_DEBUG_BAUD 38400
#define BT_DEBUG_ENABLED A0

int bt_conn_init();
void bt_conn_start();
void bt_conn_send(int _data);
void bt_conn_send(byte* _data, int _len);
int bt_conn_available();
int bt_conn_read();
int bt_conn_get_command();
