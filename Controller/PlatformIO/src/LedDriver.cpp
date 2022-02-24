
/*
LedDriver.h
AUTHOR: Anthony Mesa

Provides basic functionality for controlling a non-addressable LED strip via arduino.
*/

#include "Arduino.h"
#include "LedDriver.h"

using HoltEnvironments::PrezenzQ::LedDriver;

/**
 * Structure containing two mutable function pointers. The current function pointer references the led
 * generating function to use for setting the LEDs. The previous function pointer references the most 
 * recent generating function, which is used in transitioning between led states.
 */
LedDriver::GeneratorState LedDriver::led_value_generator;

/**
 * Indicates the current transition frame to be calculated within the transition space whose frame length is defined by TRANSITION_PERIOD.
 */
int LedDriver::transition_index;

/**
 * Initializes the driver.
 * 
 * Initialization includes setting the related pins for the LED and setting the 
 * state of the driver to off.
 *
 * @see setState()
 * @return int 
 */
int LedDriver::init() {

  resetTransitionIndex();

  if(LED_R >= 0){
    pinMode(LED_R, OUTPUT);
    digitalWrite(LED_R, LOW);
  }
  
  if(LED_G >= 0){
    pinMode(LED_G, OUTPUT);
    digitalWrite(LED_G, LOW);  
  }
  
  if(LED_B >= 0){
    pinMode(LED_B, OUTPUT);
    digitalWrite(LED_B, LOW);
  }

  if(LED_W >= 0){
    pinMode(LED_W, OUTPUT);
    digitalWrite(LED_W, OUTPUT);
  }

  setState(State::OFF);

  return 0;
}

/**
 * Sets the transition index back to 0.
 */
void LedDriver::resetTransitionIndex()
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
void LedDriver::updateGenerator(LedDriver::ledValueGeneratingFunction _function)
{
  resetTransitionIndex();
  led_value_generator.previous = led_value_generator.current;
  led_value_generator.current = _function;
}

/**
 * Sets the state of the driver.
 * 
 * Setting the state of the driver updates the function being used to generate the 
 * corresponding LED color/pattern;
 * 
 * Beware that calling this function will subsequently call updateGenerator which
 * will reset the transition index to 0. If setState is called on every frame of the
 * arduino's loop(), then the transition index will be constantly reset.
 * 
 * @see State
 * 
 * @param _state Initial state that the controller should start with.
 */
void LedDriver::setState(LedDriver::State _state) {
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
 * @brief Provides interpolation between two char values
 * 
 * @param _a Value being lerped from.
 * @param _b Value being lerped to.
 * @param _t Interpolation factor, between 0.0 and 1.0.
 * @return unsigned char Value as a result of interpolation.
 */
unsigned char LedDriver::lerp(unsigned char _a, unsigned char _b, float _t)
{
  return _a + (_b - _a) * _t;
}

/**
 * Sets the LED color. The led color set is linearly interpolated between the previous LED state and
 * the current LED state.
 * 
 * @param _current_color Pointer to the struct holding the current color to be sent to the led strip.
 */
void LedDriver::setLedColor(LedColor *_current_color)
{
  static unsigned long current_millis;
  static float transition_scalar;
  static bool in_transition;
  static LedColor previous_function_color;
  
  if(transition_index >= TRANSITION_MAX_INDEX - 1)
  {
    // This ensures that the transition index waits at a constant
    // value and does not overflow in time.
    transition_index = TRANSITION_MAX_INDEX + 1;
    in_transition = false;
  } else if (transition_index > -1)
  {
    in_transition = true;
  }

  transition_index++;

  if(in_transition){
    transition_scalar = (float)(transition_index % TRANSITION_MAX_INDEX) / TRANSITION_MAX_INDEX;

    current_millis = millis();

    (*led_value_generator.previous)(current_millis, &previous_function_color);
    (*led_value_generator.current)(current_millis, _current_color);

    int lerp_r = lerp(previous_function_color.r, _current_color->r, transition_scalar);
    int lerp_g = lerp(previous_function_color.g, _current_color->g, transition_scalar);
    int lerp_b = lerp(previous_function_color.b, _current_color->b, transition_scalar);
    int lerp_w = lerp(previous_function_color.w, _current_color->w, transition_scalar);

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
void LedDriver::update() {
  static LedColor color;

  setLedColor(&color);

  if(LED_R >= 0)
    analogWrite(LED_R, color.r);
  
  if(LED_G >= 0)
    analogWrite(LED_G, color.g);
  
  if(LED_B >= 0)
    analogWrite(LED_B, color.b);
  
  if(LED_W >= 0)
    analogWrite(LED_W, color.w);
}

/**
 * Updates the rgbw values of the LedColor struct parameter provided to display the led
 * strip's off state.
 * 
 * @param _color 
 */
void LedDriver::ledLoopOff(long _current_millis, LedDriver::LedColor *_color){
  
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
void LedDriver::ledLoopWaiting(long _current_millis, LedDriver::LedColor *_color){

  unsigned long curr_millis = millis();

  if(_color != NULL){
    _color->r = (unsigned char)(sin_wave(curr_millis, SIN_PERIOD, 300) * 255);
    _color->g = (unsigned char)(sin_wave(curr_millis, SIN_PERIOD, 100) * 255);
    _color->b = (unsigned char)(sin_wave(curr_millis, SIN_PERIOD, 1200) * 255);
    _color->w = (unsigned char)(sin_wave(curr_millis, SIN_PERIOD, 2000) * 255);
  }
}

/**
 * @brief Updates the rgbw values of the LedColor struct parameter provided to display the led 
 * strip's on state.
 * 
 * @param _color 
 */
void LedDriver::ledLoopOn(long _current_millis, LedDriver::LedColor *_color){
  if(_color != NULL){
    _color->r = 255;
    _color->g = 255;
    _color->b = 255;
    _color->w = 255;
  }
}

/**
 * @brief Math function that provides a continuous sine wave.
 * 
 * millis() should be used to pass the elapsed milliseconds to this function. The
 * reason that millis() isn't called inside this function is so that the caller
 * can synchronize multiple sin wave functions by calling millis() once and passing
 * that value to multiple sin wave functions.
 * 
 * The sin function .5 * sin(2 * pi * x) + .5 will produce a sine wave that completes
 * a single cycle over the domain from 0.0 - 1.0. Because sin is a repeating function, we only care to 
 * provide those x values from 0.0 to 1.0 in a looping fashion to yield our Y. X is provided in
 * this range by getting the modulus of the current millis elapsed over the period length
 * and then dividing by the same period length. 
 * 
 * The offset, ideally 0.0 - 1.0, offsets the sinewave from its initial counterpart.
 * 
 * @param _millis The current milliseconds elapsed.
 * @param _period The sin wave period in milliseconds.
 * @param _offset The offset of the sin wave.
 * @return float Value between 0.0 - 1.0.
 */
float LedDriver::sin_wave(unsigned long _millis, int _period, int _offset)
{
  float i = _millis % _period;
  float x = ((float) i / _period);
  float r = ((float) _offset / _period);
  float y = (.5 * sin((2 * PI * x) + (2 * PI * r))) + .5;
  return y;
}