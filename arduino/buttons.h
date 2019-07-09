#ifndef BUTTONS_H
#define BUTTONS_H 1

// number of bits in byte
#define MAX_BUTTON 8

namespace buttons {

  extern bool isPressed[MAX_BUTTON];

  void setup();
  void loop();

}
#endif
// not truncated
