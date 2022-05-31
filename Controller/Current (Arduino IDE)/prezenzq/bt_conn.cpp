
/**
   =======================================================================
   PrezenzQ Controller - bt_conn.cpp

   Holt Environments
   Author: Anthony Mesa
   Date: 05/24/2022

   =======================================================================
*/

#include "bt_conn.h"

/**
   Using software serial to communicate with the
   HCO5 so that we can still use the normal serial
   port for USB communication when plugged in
*/
static SoftwareSerial HC05(BT_RX, BT_TX);
static int debug_enabled;

int bt_conn_init() {
  pinMode(BT_DEBUG_ENABLED, INPUT);
  debug_enabled = digitalRead(BT_DEBUG_ENABLED);
  return debug_enabled;
}

void bt_conn_start() {
  if (debug_enabled) {
    HC05.begin(BT_DEBUG_BAUD);
  } else {
    HC05.begin(BT_BAUD);
  }
}

void bt_conn_send(int _data) {
  HC05.write(_data);
}

void bt_conn_send(byte* _data, int _len) {
  HC05.write(_data, _len);
}

int bt_conn_available() {
  return HC05.available();
}

int bt_conn_read() {
  return HC05.read();
}

int bt_conn_get_command() {
  if (bt_conn_available())
  {
    return bt_conn_read();
  }
}

/**
 * This implementation outlined below was part of a previous refactor that didn't
 * stick. The purpose of this function was to develop a state machine that is able
 * to recognize any commands sent of veriable length. This code follows as the 
 * implementation of a finite state machine whose graph has long been erased
 * from my whiteboard by now unfortunately.
 * 
 * The commands that are received should always follow the regex: 
 *
 * (\[[A-Z]((?![\[\]])[\x00-\xff])*\]) 
 * 
 * [{ID CHARACTER}{MSG CHARACTERS}]
 * 
 * It begins with a '[' and ends with a ']'. The first character after the open
 * bracket is an ID character that matches the ID of the controller sending the
 * message. all characters after the ID character that preceed the ending bracket
 * make up the message being sent.
 * 
 * This was not meant to be a final implementation, as more message sending
 * caveats grew as a result of exploring how to properly detect non-corrupted 
 * messages.
 * 
 * There needs to be some kind of checksum implemented. The more I wrap my brain
 * around this the more I feel I am reinventing the wheel and need to look at
 * communication protocols that already exist...
 * 
 *//**
void Bluetooth::process_waiting_commands()
{
  static char receipt;
  static char serial_buffer [BLUETOOTH_BUFFER_LIMIT];
  static int serial_buffer_index; // inits to 0
  static int buffer_state;

  //  If nothing available via HC05, then return.
  if (HC05.available() <= 0)
  {
    serial_buffer_index = 0;
    return;
  }

  //  If data is available via the HC-05, then try to parse
  //  a command from it.
  while (HC05.available() > 0)
  {
    //  When the serial buffer index exeeds the buffer limit,
    //  then the data needs to be ignored, so the index can
    //  be reset to 0.
    if (serial_buffer_index > BLUETOOTH_BUFFER_LIMIT)
    {
      serial_buffer_index = 0;
      return;
    }

    char byte_read = HC05.read();

    //  State is off and command beginning is found
    if ((buffer_state == 0) && (byte_read == '['))
    {
      buffer_state = 1;
    }

    //  State is on, and non-ending-bracket value is found
    else if ((buffer_state == 1) && (byte_read != ']'))
    {
      serial_buffer[serial_buffer_index] = byte_read;

      //  Since the current index spot is now filled with a proper value,
      //  increment the index for the next value to be placed.
      serial_buffer_index++;
    }

    //  State is on, and ending-bracket value is found
    else if ((buffer_state == 1) && (byte_read == ']'))
    {
      buffer_state = 0;

      //  specifically in this instance, we are taking the single-char payload from 
      //  the buffer and sending it to handleCommand. This should be handled differently in
      //  the future so that the protocol can be more extensible.
      receipt = serial_buffer[1];
      handleCommand(&receipt);
      
      serial_buffer_index = 0;
    }
  }
}
*/