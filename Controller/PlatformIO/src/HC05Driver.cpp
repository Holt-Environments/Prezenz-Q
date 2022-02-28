/**
 * @file HC05Driver.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-28
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "Arduino.h"
#include "HC05Driver.h"

using HoltEnvironments::PrezenzQ::HC05Driver;

SoftwareSerial HC05(BT_RX, BT_TX);

/**
 * @brief Initializes the bluetooth module, and returns whether the state of
 * the module is in normal mode, or debugging mode.
 * 
 * @return true The HC05 is in normal mode
 * @return false The HC05 is in debug mode
 */
bool HC05Driver::init()
{
  if(digitalRead(BT_DEBUG_ENABLED) == HIGH)
  {
    HC05.begin(BT_DEBUG_BAUD);
    Serial.println("HC05 initialized in debug mode...");
    return false;
  } else {
    HC05.begin(BT_NORMAL_BAUD);
    Serial.println("HC05 initialized in normal mode...");
    return true;
  }
}

void HC05Driver::debugUpdate()
{
  if (HC05.available()){
    Serial.write(HC05.read());
  }
  if (Serial.available()){
    HC05.write(Serial.read());
  }
}

void HC05Driver::handleCommand(unsigned char _response[])
{
  static unsigned char command;
  static unsigned char device_id;

  device_id = _response[0];
  command = _response[1];

  for(int i = 0; i < 16; i++)
  {
    Serial.print(_response[i]);
  }
  Serial.println();

  switch(command){
    case 0:
      Serial.println("command 0 handled!");
      break;
    case 10:
      Serial.println("command 10 handled");
      break;
    default:
      Serial.println("recieved command that cant be handled");
  }
}

void HC05Driver::evaluateCharacter(unsigned char _byte_read) 
{
  static int current_state;
  static unsigned char response[16];
  static unsigned char response_index;

  Serial.print("char: ");
  Serial.print(_byte_read);

  switch(current_state)
  {
    case OUT_OF_COMMAND:
      if(_byte_read == R_BRACKET){
        current_state = START_COMMAND;
      }
      break;

    case START_COMMAND:
      if(_byte_read == L_BRACKET){
        current_state = END_COMMAND;
      }
      else if(_byte_read != R_BRACKET){
        response[response_index] = _byte_read;
        response_index++;
        current_state = IN_COMMAND;
      }
      break;

    case IN_COMMAND:
      if(_byte_read == R_BRACKET){
        current_state = START_COMMAND;
      } else if (_byte_read == L_BRACKET) {
        current_state = END_COMMAND;
      } else if (response_index > 15)
      {
        response_index = 0;
        current_state = OUT_OF_COMMAND;
      } else {
        response[response_index] = _byte_read;
        response_index++;
      }
      break;
      
    case END_COMMAND:
      if(_byte_read == R_BRACKET){
        current_state = START_COMMAND;
      } else if (_byte_read != L_BRACKET){

        /*  Fills the rest of the command space with 0
         */
        for(int i = response_index; i < 16; i++)
        {
          response[i] = 0;
        }

        Serial.println("found something!");

        handleCommand(response);        

        response_index = 0;
        current_state = OUT_OF_COMMAND;
      }
      break;
  }
}

/**
 * @brief Updates the bluetooth module's FSA
 * 
 */
void HC05Driver::update()
{
  while (HC05.available() > 0){
    Serial.println("data available!");
    evaluateCharacter(HC05.read());
  }
}