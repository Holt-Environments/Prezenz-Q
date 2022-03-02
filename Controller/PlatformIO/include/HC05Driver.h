/**
 * @file HC05Driver.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-28
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <SoftwareSerial.h>

#define BT_RX 8
#define BT_TX 7
#define BT_NORMAL_BAUD 9600
#define BT_DEBUG_BAUD 38400
#define BT_DEBUG_ENABLED A0
#define CONTROLLER_BUFFER_LIMIT 64

#define OUT_OF_COMMAND 0
#define START_COMMAND 1
#define IN_COMMAND 2
#define END_COMMAND 3

#define R_BRACKET '['
#define L_BRACKET ']'

namespace HoltEnvironments {

namespace PrezenzQ {

class HC05Driver {

public:

  static bool init();
  static void update();
  static void debugUpdate();
  static void sendByteData(byte cmd[4], int len);

private:

  // static SoftwareSerial HC05;

  static void evaluateCharacter(unsigned char _byte_read);
  static void handleCommand(unsigned char _response[]);
  static void emptyBuffer();
};

} //  HoltEnvironments

} //  PrezenzQ