
/**
   =======================================================================
   PrezenzQ Controller - led_strip.cpp

   Holt Environments
   Author: Anthony Mesa
   Date: 05/24/2022

   =======================================================================
*/

#include "led_strip.h"

static void (*ledCommand)() = NULL;

void led_strip_init() {
  pinMode(LED_B, OUTPUT);
}

void ledOff()
{
  analogWrite(LED_B, 0);
}

void ledOn()
{
  analogWrite(LED_B, 255);
}

/**
   This state creates a sin value that is written to the LED. The sin
   function used oscillates between 0 and 1, where the period is defined
   in led_strip.h. The sin value is mapped to values 0 - 254 required for
   controlling an analog signal.
*/
void ledWaiting()
{
  double value = millis() % LED_PERIOD;
  double test = (.5 * sin(2 * PI * (value / LED_PERIOD))) + .5;
  analogWrite(LED_B, (int)(test * 255));
}

/**
   Simply executes whichever function that the ledCommand function pointer
   is currently pointing to.
*/
void led_strip_update()
{
  (*ledCommand)();
}

void led_strip_set_command(enum led_state _new_state) {
  switch (_new_state) {
    case LED_STRIP_OFF:
      ledCommand = &ledOff;
      break;
    case LED_STRIP_WAITING:
      ledCommand = &ledWaiting;
      break;
    case LED_STRIP_ON:
      ledCommand = &ledOn;
      break;
  }
}

/**
 * Below is an old implementation of a previous refactor. I have included it here
 * for posterity's sake as it contains logic to deal with all 4 color channels and
 * an algorithm for smoothly fading between all led state changes, as currently when
 * the LED state changes, it makes a hard cut from one LED animation to the other.
 * 
 */
/**
class LedStrip {
  
public:

  enum LedState { OFF, WAITING, ON };

  static void setState(LedState _state);
  static void init(LedState _state);
  static void update();
  
private:

  typedef struct led_color{
    int r = 0;
    int g = 0;
    int b = 0;
    int w = 0;
  } LedColor;

  static int transition_index;

  static void (*currentLedFunction)(long _current_millis, LedColor &_color);
  static void (*previousLedFunction)(long _current_millis, LedColor &_color);
  
  static void loopOff(long _current_millis, LedColor &_color);
  static void loopOn(long _current_millis, LedColor &_color);
  static void loopWaiting(long _current_millis, LedColor &_color);

  static void setLedFunction(void (*_led_function)(long _current_millis, LedColor &_color));
  static void updateCurrentColor(LedColor &_current_color);
  static void resetTransitionIndex();

  static float sin_wave(int _i);
};

void (*LedStrip::currentLedFunction)(long _current_millis, LedColor &_color) = NULL;
void (*LedStrip::previousLedFunction)(long _current_millis, LedColor &_color) = NULL;
int LedStrip::transition_index = 0;

void LedStrip::init(LedState _state)
{  
  pinMode(ARDUINO_LED, OUTPUT);
  digitalWrite(ARDUINO_LED, LOW);
  
  pinMode(LED_R, OUTPUT);
  digitalWrite(LED_R, LOW);

  pinMode(LED_G, OUTPUT);
  digitalWrite(LED_G, LOW);

  pinMode(LED_B, OUTPUT);
  digitalWrite(LED_B, LOW);

  pinMode(LED_W, OUTPUT);
  digitalWrite(LED_W, LOW);

  setState(_state);
}

void LedStrip::loopOff(long _current_millis, LedColor &_color)
{  
  _color.r = 0;
  _color.g = 0;
  _color.b = 0;
  _color.w = 0;
}

void LedStrip::loopOn(long _current_millis, LedColor &_color)
{
  _color.r = 255;
  _color.g = 255;
  _color.b = 255;
  _color.w = 255;
}

void LedStrip::loopWaiting(long _current_millis, LedColor &_color)
{
  _color.r = 0;
  _color.g = 0;
  _color.w = 0;
  
  int mil_index = _current_millis % LED_PERIOD;
  float sin_normalized = sin_wave(mil_index);
  
  _color.b = (int)(sin_normalized * 255);
}

void LedStrip::resetTransitionIndex()
{
  transition_index = 0;
}

void LedStrip::setLedFunction(void (*_led_function)(long _current_millis, LedColor &_color))
{
  resetTransitionIndex();
  previousLedFunction = currentLedFunction;
  currentLedFunction = _led_function;
}

void LedStrip::setState(LedState _state)
{
  switch(_state)
  {
    case LedState::OFF:
      setLedFunction(&loopOff);
      break;
    case LedState::ON:
      setLedFunction(&loopOn);
      break;
    case LedState::WAITING:
      setLedFunction(&loopWaiting);
      break;
  }
}

void LedStrip::updateCurrentColor(LedColor &_current_color)
{
  static long current_millis;
  static float transition_scalar;
  static bool in_transition;
  static LedColor previous_function_color;

  current_millis = millis();
  
  if(transition_index >= TRANSITION_PERIOD - 1)
  {
    in_transition = false;
  } else if (transition_index > -1)
  {
    in_transition = true;
  }

  transition_index++;

  if(in_transition){
    transition_scalar = (float)(transition_index % TRANSITION_PERIOD) / TRANSITION_PERIOD;

    (*previousLedFunction)(current_millis, previous_function_color);
    (*currentLedFunction)(current_millis, _current_color);

    int lerp_r = previous_function_color.r + (_current_color.r - previous_function_color.r) * (transition_scalar);
    int lerp_g = previous_function_color.g + (_current_color.g - previous_function_color.g) * (transition_scalar);
    int lerp_b = previous_function_color.b + (_current_color.b - previous_function_color.b) * (transition_scalar);
    int lerp_w = previous_function_color.w + (_current_color.w - previous_function_color.w) * (transition_scalar);

    //  Make sure that none of the values come up negative.
    int lerp_r_corrected = lerp_r >= 0 ? lerp_r : 0;
    int lerp_g_corrected = lerp_g >= 0 ? lerp_g : 0;
    int lerp_b_corrected = lerp_b >= 0 ? lerp_b : 0;
    int lerp_w_corrected = lerp_w >= 0 ? lerp_w : 0;

    _current_color.r = lerp_r_corrected;
    _current_color.g = lerp_g_corrected;
    _current_color.b = lerp_b_corrected;
    _current_color.w = lerp_w_corrected;    
  } else {
    (*currentLedFunction)(current_millis, _current_color);
  }
}

void LedStrip::update()
{
  static LedColor current_color;

  updateCurrentColor(current_color);

  analogWrite(LED_R, current_color.r);
  analogWrite(LED_G, current_color.g);
  analogWrite(LED_B, current_color.b);
  analogWrite(LED_W, current_color.w);
}

float LedStrip::sin_wave(int _i)
{
  return (.5 * sin(2 * PI * ((float)_i / LED_PERIOD))) + .5;
}
*/