#ifndef SETTINGS_H
#define SETTINGS_H

namespace settings {

  static const uint8_t kMaxProfile = 4; // 5 profiles, numbered 0..4
  static const uint16_t kMaxSpeed = 500;
  static const uint16_t kDefaultSpeed = 125;
  static const uint16_t kMaxPullbackDelay = 10000;
  static const uint16_t kMaxSteps = 10000;
  static const uint16_t kMaxMicrosteps = 64;
  static const uint16_t kDefaultMicrosteps = 2;

  extern uint16_t profileNumber; // current profile
  extern uint16_t forwardSpeed; // speed when forward button is pressed.
  extern uint16_t forwardSteps; // number of steps to move when forward button is pressed. if 0 keep moving until forward button is released.
  extern uint16_t backwardSpeed; // speed when backward button is pressed
  extern uint16_t pullbackDelay; // time to wait after forward motion before pulling back. in milliseconds.
  extern uint16_t pullbackSteps; // number of steps to pull back. if 0 don't pull back.
  extern uint16_t microSteps; // microsteps per full step. valid values are 1, 2, 4, 5, 16, 32, 64.

  void writeConfig(); // writes current profile and profile number to eeprom.
  void readConfig(bool restoreProfileNumber = false); // reads profile from eeprom. Optionally restores profile number (used at startup)
  void readchar(); // read one character from the serial port
  
  extern void setup();
  extern void loop();
}

#endif

// not truncated
