
/**
   =======================================================================
   PrezenzQ Controller

   Holt Environments
   Author: Anthony Mesa
   Date: 05/24/2022

   The PrezenzQ controller is meant to be paired with the PrezenzQ video
   queuing desktop application available at:

    https://github.com/Holt-Environments/Prezenz-Q/tree/master/Queue/Windows%20x64

   This controller uses an Arduino Nano and utilizes Bluetooth connection via
   an HC05 bluetooth module, time of flight sensing via a VL53L1X TOF sensor,
   and an external LED strip (at the time of writing this, the controller is only
   designed to manipulate a single channel of RGB color, in this specific case,
   the color blue is what we want to control).

   The controller draws 24V power at a maximum of around 8-10 amps. The schematics
   for the controller and information on the hardware is available here:

   https://github.com/Holt-Environments/Prezenz-Q/tree/master/Controller/Schematics

   If you are reading this, live in NC, and build things with computers, you
   should contact us about career opprotunities:

   https://www.holtenvironments.com/contact/

   =======================================================================
*/

#include "prezenzq.h"
#include "led_strip.h"
#include "bt_conn.h"
#include "tof_sensor.h"
#include "button.h"

/**
   This function pointer is used by being called in the Arduino's loop function.
   There are three main states for this controller to be in, DEBUG, MANUAL, and SENSOR.
   The functionality for those states are defined in their own functions below, and
   when the controller has to switch from one state to the other, one can simply
   change to which of those state functions that this function pointer is pointing.
*/
void (*volatile execute_control_state)() = NULL;

/**
   The DEBUG control state is largely for messing with the HC05's
   AT commands. This just makes it so that the led strip still updates
   (in debug mode it should be slowly flashing continuously), and any
   data sent to the controller via USB serial is sent directly to the
   HC05 module and vis-versa.
*/
void debug_control_state()
{
  led_strip_update();

  if (bt_conn_available())
    Serial.write(bt_conn_read());
  if (Serial.available())
    bt_conn_send(Serial.read());
}

/**
   In the MANUAL control state, the controller can only be updated via
   button press. A single press of the button switches the state from
   on to off and vis-versa.

   To switch back to the SENSOR control state, the button must be held
   down for the seconds duration defined in the button.h header.
*/
void manual_control_state()
{
  static int switch_state;
  static enum button_state current_button_state;

  // Get any updates from the videoq on PC
  handle_video_q_update();

  led_strip_update();

  // Read the current button state
  current_button_state = button_get_state();

  /**
     If the button is held, then try to initialize the sensor
     before switching to SENSOR control mode. If the sensor can not
     be initialized, then the controller stays in the MANUAL control
     state.

     If the button is only clicked, then control the videoq accordingly.
  */
  if (current_button_state == BUTTON_HELD) {
    if (tof_sensor_init()) {
      led_strip_set_command(LED_STRIP_OFF);
      execute_control_state = &sensor_control_state;
      button_led_set(0);
      digitalWrite(ARDUINO_LED, LOW);
    } else {
      Serial.println("init sensor failed");
    }
  } else if (current_button_state == BUTTON_CLICKED) {

    /**
       There are three switched states (these states are independent of the
       button states defined in button.h). The switch is initialized at state 0,
       indicating that the button hasn't been pressed yet. Upon a button click,
       the state will increment to 1, indicating that a video should be queued.
       Another button click will advance the switch state to 2, meaning that
       the video currently queued should be dequeued, and the switch state is then
       reset to 0, so that upon next button click it is advanced to 1 again.
    */

    switch_state++;

    /**
       This is initialized as the 'dequeue command' given the 0x00 payload. A
       payload of 0x01 will indicate the 'queue video' command.
    */
    byte cmd[4] = { '[', SENSOR_ID, 0x00, ']' };

    if (switch_state == 1) {
      cmd[2] = 0x01; // update the payload to the 'queue' command.
    } else if (switch_state == 2) {
      switch_state = 0; // keep the initial 0x00 payload and reset the switch state.
    }

    bt_conn_send(cmd, 4);
  }
}

/**
   Get the most recent commands sent to the controller via the
   prezenzq video player running on the connected PC.

   Currently this only serves to manipulate the LED strip based
   on three different ascii values recieved, F, W, and N.
*/
void handle_video_q_update() {
  static char command;

  command = bt_conn_get_command();

  switch (command)
  {
    case 'F':
      led_strip_set_command(LED_STRIP_OFF);
      break;
    case 'W':
      led_strip_set_command(LED_STRIP_WAITING);
      break;
    case 'N':
      led_strip_set_command(LED_STRIP_ON);
      break;
  }
}

/**
   In the SENSOR control state, the controller is updated via the Time-Of-Flight
   sensor. When an object is placed within or removed from a specified zone
   within the sensor's view, then the videoq is controlled accordingly.

   To switch back to MANUAL control, the button must be held down for the
   seconds duration defined in the button.h header.
*/
void sensor_control_state()
{
  static enum button_state current_button_state;

  // Get any updates from the videoq on PC
  handle_video_q_update();

  led_strip_update();

  // Read the current button state
  current_button_state = button_get_state();

  //If the button is held, then switch to MANUAL control mode.
  if (current_button_state == BUTTON_HELD) {
    led_strip_set_command(LED_STRIP_OFF);
    execute_control_state = &manual_control_state;
    button_led_set(1);
    digitalWrite(ARDUINO_LED, HIGH);
    return;
  }

  int sensor_status = tof_sensor_update();

  /**
     This is initialized as the 'dequeue command' given the 0x00 payload. A
     payload of 0x01 will indicate the 'queue video' command.
  */
  byte cmd[4] = { '[', SENSOR_ID, 0x00, ']' };

  switch (sensor_status) {
    case -1:
      bt_conn_send(SENSOR_ERROR_TIMEOUT);
      break;
    case 0:
      cmd[2] = 0x01; // update the payload to the 'queue' command.
      bt_conn_send(cmd, 4);
      break;
    case 1:
      bt_conn_send(cmd, 4); // keep the initial 0x00 payload
      break;
  }
}


/**
   This is the first code to run on the controller. First the bluetooth connection
   is initialized. If this initialization returns a number greater than 0, then
   the device should enter the DEBUG state.

   If the device is to enter normal mode, then the TOF sensor is initialized. If
   an issue occurs with the sensor's initialization, then the device enters the
   MANUAL control state.

   If the TOF sensor starts up properly, then the device enters the BLUETOOTH
   control state.
*/
void setup()
{
  pinMode(ARDUINO_LED, OUTPUT);
  digitalWrite(ARDUINO_LED, LOW);
  Serial.begin(ARDUINO_SERIAL_BAUD);

  if (bt_conn_init() > 0) {

    // Enter DEBUG state

    led_strip_set_command(LED_STRIP_WAITING);
    execute_control_state = &debug_control_state;
  }
  else if (!tof_sensor_init())
  {
    // Enter MANUAL state

    digitalWrite(ARDUINO_LED, HIGH);
    led_strip_set_command(LED_STRIP_OFF);
    button_led_set(1);
    execute_control_state = &manual_control_state;
  } else {

    // Enter SENSOR state

    led_strip_set_command(LED_STRIP_OFF);
    execute_control_state = &sensor_control_state;
  }

  bt_conn_start();
  led_strip_init();
  button_init();
}

/**
 * After setup, this loop runs indefinitely. All this does is execute whichever
 * function that the execute_control_state function pointer is pointing at.
 */
void loop()
{
  if (execute_control_state != NULL)
    (*execute_control_state)();
}
