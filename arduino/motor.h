#ifndef MOTOR_H
#define MOTOR_H

#include <arduino.h>

namespace motor {

  extern uint16_t steps; // step counter
  extern void setSpeed(uint16_t steps_per_sec, boolean is_forward); // Start rotation with speed steps_per_second and direction is_forward. 

  extern void setup();
  extern void loop();
  
}

#endif
// not truncated
