#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

namespace motor {

  extern uint16_t steps; // step counter

  extern void sleep();
  extern void wakeup();

  extern void setMicrosteps(uint8_t microsteps_per_step); // Set microstepping. 1 = full step, 2 = half stepping, etc. 
  extern void setSpeed(uint16_t steps_per_sec, boolean is_forward); // Start rotation with speed steps_per_second and direction is_forward. 

  extern void setup();
  extern void loop();
  
}

#endif
// not truncated
