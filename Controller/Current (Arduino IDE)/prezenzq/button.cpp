
#include "button.h"

Button button1(SINGLE_STATE_BUTTON, BUTTON_HOLD_DURATION);

void button_init(){
  pinMode(SINGLE_STATE_BUTTON, INPUT);
  pinMode(SINGLE_STATE_BUTTON_LED, OUTPUT);
}

enum button_state button_get_state(){
  static int button_state;

  button_state = button1.checkPress();

  switch(button_state){
    case -1:
      return BUTTON_HELD;
    case 0:
      return BUTTON_OFF;
    case 1:
      return BUTTON_CLICKED;
  }
}

void button_led_set(int _value){
  if(_value = 0){
    digitalWrite(SINGLE_STATE_BUTTON_LED, LOW);
  } else {
    digitalWrite(SINGLE_STATE_BUTTON_LED, HIGH);
  }
}
