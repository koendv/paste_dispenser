#include <Arduino.h>
#include "motor.h"
#include "tb6612.h"

namespace motor {
  
  uint16_t steps = 0; // step counter
  static boolean is_forward = true; // rotation direction
  static uint32_t next_step_micros = 0; // time of next step, in microseconds
  static uint32_t step_time_micros = 0; // microseconds between steps. If 0 indicates zero speed.

  void setSpeed(uint16_t steps_per_sec, boolean forward) {
  
    if (!steps_per_sec) {
      step_time_micros = 0;
      return;  
    }
    
  //#define AUTO_GEAR_SHIFT
  #ifdef AUTO_GEAR_SHIFT
    /* 
     *  Auto gear shift. Doubles microstepping when speed halves.
     *  This increases resolution at slow speeds, and limits the load on the processor at high speeds.
     *  Needs to be tuned for every stepper used.
     */
    const uint16_t auto_gear_shift = 64;
    uint16_t microsteps = 1;
    if (steps_per_sec > auto_gear_shift) microsteps = 2;
    else if (steps_per_sec > auto_gear_shift / 2) microsteps = 4;
    else if (steps_per_sec > auto_gear_shift / 4) microsteps = 8;
    else if (steps_per_sec > auto_gear_shift / 16) microsteps = 16;
    else microsteps = 32;
    tb6612::setMicrosteps(microsteps); // disregard microsteps setting
    steps_per_sec *= microsteps;
  #endif
  
    step_time_micros = 1000000L / steps_per_sec;
    is_forward = forward;
    next_step_micros = micros(); // now
  } 
  
  void setup() {
    tb6612::setup();
  }
  
  void loop() {
    if (!step_time_micros)
      return;
    uint32_t now_micros = micros();
    if (now_micros >= next_step_micros) {
      tb6612::step(is_forward);
      steps++;
      next_step_micros = now_micros + step_time_micros; // set time of next step
    }
  }
}
// not truncated
