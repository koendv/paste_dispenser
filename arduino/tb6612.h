#ifndef TB6612_H
#define TB6612_H

#include <arduino.h>

namespace tb6612 {
  static const uint8_t kMaxMicrosteps = 64;
  
  void setup();

  void sleep();
  void wakeup();

  void setMicrosteps(uint8_t microsteps_per_step);
  void step(bool clockwise);
}
#endif
// not truncated
