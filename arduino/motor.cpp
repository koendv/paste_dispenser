#include <Arduino.h>
#include "motor.h"
#include "tb6612.h"

namespace motor {

  uint16_t steps = 0; // step counter
  static boolean is_forward = true; // rotation direction
  static uint32_t next_step_micros = 0; // time of next step, in microseconds
  static uint32_t step_time_micros = 0; // microseconds between steps. If 0 indicates zero speed.


 // Standby mode. When in standby, the motor does not resist turning, but does not warm up either.

  void sleep() {
    tb6612::sleep();
  }

  void wakeup() {
    tb6612::wakeup();
  }

  /*
   * microstep setting: 1, 1/2, 1/4, 1/8, 1/16, 1/32, 1/64 microstepping.
   * Microstepping can be changed at any moment.
   */
  void setMicrosteps(uint8_t microsteps) {
    tb6612::setMicrosteps(microsteps);
  }

  void setSpeed(uint16_t steps_per_sec, boolean forward) {

    if (!steps_per_sec) {
      step_time_micros = 0;
      return;
    }

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
