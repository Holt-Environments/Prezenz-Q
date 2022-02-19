
/*
LedDriver.h
AUTHOR: Anthony Mesa

Provides basic functionality for controlling a non-addressable LED strip via arduino.
*/

/*
================================================================================
    Libraries
================================================================================
*/

#include "Arduino.h"
#include "LedDriver.h"

/*
================================================================================
  Definitions
================================================================================
*/

#define LED_R 0
#define LED_G 0
#define LED_B 3
#define LED_W 0
#define LED_PERIOD 3000 // ms
#define TRANSITION_PERIOD 100// ms

/**
 * Structure containing two mutable function pointers. The current function pointer references the led
 * generating function to use for setting the LEDs. The previous function pointer references the most 
 * recent generating function, which is used in transitioning between led states.
 */
HoltEnvironments::PrezenzQ::LedDriver::GeneratorState HoltEnvironments::PrezenzQ::LedDriver::led_value_generator;

/**
 * Indicates the current transition frame to be calculated within the transition space whose frame length is defined by TRANSITION_PERIOD.
 */
int HoltEnvironments::PrezenzQ::LedDriver::transition_index = 0;

/**
 * Initializes the driver.
 * 
 * Initialization includes setting the related pins for the LED and setting the 
 * state of the driver to off.
 *
 * @see setState()
 * @return int 
 */
int HoltEnvironments::PrezenzQ::LedDriver::init() {

  pinMode(LED_R, OUTPUT);
  digitalWrite(LED_R, LOW);

  pinMode(LED_G, OUTPUT);
  digitalWrite(LED_G, LOW);

  pinMode(LED_B, OUTPUT);
  digitalWrite(LED_B, LOW);

  pinMode(LED_W, OUTPUT);
  digitalWrite(LED_W, OUTPUT);

  setState(State::OFF);

  return 0;
}

/**
 * Sets the transition index back to 0.
 */
void HoltEnvironments::PrezenzQ::LedDriver::resetTransitionIndex()
{
  transition_index = 0;
}

/**
 * Updates the current and previous functions contained in the LED value generator structure. Since this indicates
 * that a transition between LED states is taking place, resetTransitionState is called.
 * 
 * @see resetTransitionState
 * 
 * @param _function The function address to be assigned to the generator's current function pointer.
 */
void HoltEnvironments::PrezenzQ::LedDriver::updateGenerator(HoltEnvironments::PrezenzQ::LedDriver::ledValueGeneratingFunction _function)
{
  led_value_generator.previous = led_value_generator.current;
  led_value_generator.current = _function;
}

/**
 * Sets the state of the driver.
 * 
 * Setting the state of the driver updates the function being used to generate the 
 * corresponding LED color/pattern;
 * 
 * @see State
 * 
 * @param _state Initial state that the controller should start with.
 */
void HoltEnvironments::PrezenzQ::LedDriver::setState(HoltEnvironments::PrezenzQ::LedDriver::State _state) {
  switch(_state){
    case State::OFF:
      updateGenerator(&ledLoopOff);
      break;
    case State::WAITING:
      updateGenerator(&ledLoopWaiting);
      break;
    case State::ON:
      updateGenerator(&ledLoopOn);
      break;
  }
}

/**
 * Sets the LED color. The led color set is linearly interpolated between the previous LED state and
 * the current LED state.
 * 
 * @param _current_color Pointer to the struct holding the current color to be sent to the led strip.
 */
void HoltEnvironments::PrezenzQ::LedDriver::setLedColor(LedColor *_current_color)
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

    (*led_value_generator.previous)(current_millis, &previous_function_color);
    (*led_value_generator.current)(current_millis, _current_color);

    int lerp_r = previous_function_color.r + (_current_color->r - previous_function_color.r) * (transition_scalar);
    int lerp_g = previous_function_color.g + (_current_color->g - previous_function_color.g) * (transition_scalar);
    int lerp_b = previous_function_color.b + (_current_color->b - previous_function_color.b) * (transition_scalar);
    int lerp_w = previous_function_color.w + (_current_color->w - previous_function_color.w) * (transition_scalar);

    //  Make sure that none of the values come up negative.
    int lerp_r_corrected = lerp_r >= 0 ? lerp_r : 0;
    int lerp_g_corrected = lerp_g >= 0 ? lerp_g : 0;
    int lerp_b_corrected = lerp_b >= 0 ? lerp_b : 0;
    int lerp_w_corrected = lerp_w >= 0 ? lerp_w : 0;

    _current_color->r = lerp_r_corrected;
    _current_color->g = lerp_g_corrected;
    _current_color->b = lerp_b_corrected;
    _current_color->w = lerp_w_corrected;    
  } else {
    (*led_value_generator.current)(current_millis, _current_color);
  }
}

/**
 * Updates the pins of the Arduino with the values generated by the function
 * pointed to by the generateLedValue function pointer.
 * 
 * A static LedColor struct contains the rgb values to be sent to the led strip.
 * The address of this struct is passed to the generating function so that
 * it can update the values stored in the static color struct.
 * 
 * @see led_value_generator
 * @see ledLoopOff()
 * @see ledLoopWaiting()
 * @see ledLoopOn()
 */
void HoltEnvironments::PrezenzQ::LedDriver::update() {
  static LedColor color;

  setLedColor(&color);

  analogWrite(LED_R, color.r);
  analogWrite(LED_G, color.g);
  analogWrite(LED_B, color.b);
  analogWrite(LED_W, color.w);
}

/**
 * Updates the rgbw values of the LedColor struct parameter provided to display the led
 * strip's off state.
 * 
 * @param _color 
 */
void HoltEnvironments::PrezenzQ::LedDriver::ledLoopOff(long _current_millis, HoltEnvironments::PrezenzQ::LedDriver::LedColor *_color){
  if(_color != NULL){
    _color->r = 0;
    _color->g = 0;
    _color->b = 0;
    _color->w = 0;
  }
}

/**
 * Updates the rgbw values of the LedColor struct parameter provided to display the led 
 * strip's waiting state.
 * 
 * @param _color 
 */
void HoltEnvironments::PrezenzQ::LedDriver::ledLoopWaiting(long _current_millis, HoltEnvironments::PrezenzQ::LedDriver::LedColor *_color){
  if(_color != NULL){
    _color->r = 0;
    _color->g = 2;
    _color->b = 0;
    _color->w = 0;
  }
}

/**
 * Updates the rgbw values of the LedColor struct parameter provided to display the led 
 * strip's on state.
 * 
 * @param _color 
 */
void HoltEnvironments::PrezenzQ::LedDriver::ledLoopOn(long _current_millis, HoltEnvironments::PrezenzQ::LedDriver::LedColor *_color){
  if(_color != NULL){
    _color->r = 0;
    _color->g = 0;
    _color->b = 255;
    _color->w = 0;
  }
}

/**
 * Math function that returns the appropriate Y value, where the X input value 
 * is the ratio of the current index vs. the full period length.
 *
 * @param _i  The current index of the sin function. 
 * @param _period The domain value representing a full period in the sin wave.
 * @return float 
 */
float HoltEnvironments::PrezenzQ::LedDriver::sin_wave(int _i, int _period)
{
  float x = ((float)_i / LED_PERIOD);
  float y = (.5 * sin(2 * PI * x)) + .5;
  return y;
}