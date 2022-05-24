
#include "bt_conn.h"

//  Software serial for bluetooth device so that
//  we can still use the normal serial port for
//  USB communication when plugged in.
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

void bt_conn_send(int _data){
  HC05.write(_data);
}

void bt_conn_send(byte* _data, int _len) {
  HC05.write(_data, _len);
}

int bt_conn_available(){
  return HC05.available();  
}

int bt_conn_read(){
  return HC05.read();
}

int bt_conn_get_command(){
  if (bt_conn_available())
  {
    return bt_conn_read();
  }
}
