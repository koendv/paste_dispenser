#ifndef MOTOR_H
#define MOTOR_H

#include <arduino.h>

namespace motor {

extern uint32_t steps;

extern void setup();

extern void loop();

extern void sleep();

// steps_per_sec 0 indicates stop. Turns on motor if it's off.
extern void setSpeed(uint16_t steps_per_sec, boolean is_forward);

extern bool isNonZeroSpeed();
  
}  // namespace motor

#endif  // ifdef
// not truncated
