#include <arduino.h>
#include <EEPROMWearLevel.h>
#include "pins.h"
#include "buttons.h"
#include "settings.h"
#include "fsm.h"

// Change EEPROM_LAYOUT_VERSION when number of indexes changed etc.

#define EEPROM_LAYOUT_VERSION 1
#define EEPROM_INDEXES 1
#define EEPROM_INDEX_STORED_PROFILE 0

#define EEPROM_MAGIC_NUMBER 0xdeadbeef

namespace settings {

  /*
   * Button state
   */
  static long kButtonAutoRepeat = 500; /* milliseconds */

  static const uint8_t BUTTON_IDLE = 0;  // Waiting for work.
  static const uint8_t BUTTON_FAST = 1;  // "fast" button pressed. Increase speed.
  static const uint8_t BUTTON_SLOW = 2;  // "slow" button pressed. Decrease speed.

  static uint8_t button_state = BUTTON_IDLE;

  /*
   * active profile
   */

  uint32_t magicNumber; // check if saved settings are valid
  uint16_t profileNumber; // current profile
  uint16_t forwardSpeed; // speed when forward button is pressed.
  uint16_t forwardSteps; // number of steps to move when forward button is pressed. if 0 keep moving until forward button is released (default).
  uint16_t backwardSpeed; // speed when backward button is pressed. default 500.
  uint16_t pullbackDelay; // time to wait after forward motion before pulling back. in milliseconds.
  uint16_t pullbackSteps; // number of steps to pull back. if 0 don't pull back (default).
  uint16_t microSteps; // microsteps per full step. valid values are 1, 2, 4, 5, 16, 32, 64.

  /*
   * profiles as saved in eeprom
   */

  typedef struct  {
    uint32_t magicNumber;
    uint8_t profileNumber; // default profile to use at next boot
    uint16_t forwardSpeed[kMaxProfile+1];
    uint16_t forwardSteps[kMaxProfile+1];
    uint16_t backwardSpeed[kMaxProfile+1];
    uint16_t pullbackDelay[kMaxProfile+1];
    uint16_t pullbackSteps[kMaxProfile+1];
    uint16_t microSteps[kMaxProfile+1];
  } profile;

  /*
   * enforce valid values in profile
   */

  void constrainConfig() {
    profileNumber = constrain(profileNumber, 0, kMaxProfile);
    forwardSpeed = constrain(forwardSpeed, 1, kMaxSpeed);
    forwardSteps = constrain(forwardSteps, 0, kMaxSteps);
    backwardSpeed = constrain(backwardSpeed, 1, kMaxSpeed);
    pullbackDelay = constrain(pullbackDelay, 0, kMaxPullbackDelay);
    pullbackSteps = constrain(pullbackSteps, 0, kMaxSteps);
    microSteps = constrain(microSteps, 1, kMaxMicrosteps);
    if ((microSteps != 1) && (microSteps != 2) && (microSteps != 4) && (microSteps != 8) && (microSteps != 16) && (microSteps != 32)  && (microSteps != 64)) microSteps = 2; // valid values for microstepping
    return;
  }

  /*
   * save current profile to eeprom
   */
  
  void writeConfig() {
    profile storedProfile;
    constrainConfig(); // make certain we write valid values
    EEPROMwl.get(EEPROM_INDEX_STORED_PROFILE, storedProfile);
    storedProfile.magicNumber = EEPROM_MAGIC_NUMBER;
    storedProfile.profileNumber = profileNumber;
    storedProfile.forwardSpeed[profileNumber] = forwardSpeed;
    storedProfile.forwardSteps[profileNumber] = forwardSteps;
    storedProfile.backwardSpeed[profileNumber] = backwardSpeed;
    storedProfile.pullbackDelay[profileNumber] = pullbackDelay;
    storedProfile.pullbackSteps[profileNumber] = pullbackSteps;
    storedProfile.microSteps[profileNumber] = microSteps;
    EEPROMwl.put(EEPROM_INDEX_STORED_PROFILE, storedProfile);
    return;
  }

  // restoreProfileNumber = false:  read profile only, keep profileNumber unchanged
  // restoreProfileNumber = true: read both profile and profileNumber 
  void readConfig(bool restoreProfileNumber = false) {
    profile storedProfile;
    uint16_t storedProfileNumber;
    EEPROMwl.get(EEPROM_INDEX_STORED_PROFILE, storedProfile);
    if (storedProfile.magicNumber != EEPROM_MAGIC_NUMBER) { // eeprom corrupted or uninitialized
      storedProfile.magicNumber = EEPROM_MAGIC_NUMBER;
      storedProfile.profileNumber = 0;
      for (uint8_t i=0; i <= kMaxProfile; i++) {
        storedProfile.forwardSpeed[i] = kDefaultSpeed;
        storedProfile.forwardSteps[i] = 0;
        storedProfile.backwardSpeed[i] = kMaxSpeed;
        storedProfile.pullbackDelay[i] = 0;
        storedProfile.pullbackSteps[i] = 0;
        storedProfile.microSteps[i] = kDefaultMicrosteps;
      }
      Serial.println(F("eeprom init"));
      EEPROMwl.put(EEPROM_INDEX_STORED_PROFILE, storedProfile);
    }
    if (restoreProfileNumber) profileNumber = storedProfile.profileNumber;
    profileNumber = constrain(profileNumber, 0, kMaxProfile); // make certain profile number sane
    forwardSpeed = storedProfile.forwardSpeed[profileNumber];
    forwardSteps = storedProfile.forwardSteps[profileNumber];
    backwardSpeed = storedProfile.backwardSpeed[profileNumber];
    pullbackDelay = storedProfile.pullbackDelay[profileNumber];
    pullbackSteps = storedProfile.pullbackSteps[profileNumber];
    microSteps = storedProfile.microSteps[profileNumber];
    constrainConfig(); // make certain we have valid values
    return;
  }

  /*
   * Buttons!
   */

  inline bool fastButtonPressed() {
    return buttons::isPressed[kFastButtonPin];
  }

  inline bool slowButtonPressed() {
    return buttons::isPressed[kSlowButtonPin];
  }

  void changeSpeed(int16_t delta) {
    int16_t newSpeed = forwardSpeed + delta;
    forwardSpeed = constrain(newSpeed, 1, kMaxSpeed);
  }

  void setup (){
    profile storedProfile;
    EEPROMwl.begin(EEPROM_LAYOUT_VERSION, EEPROM_INDEXES);
    readConfig(true); // read profile and profileNumber
    
  }

  static long lastPressedMillis = 0;
  // tactile button auto-repeat values
  static const uint8_t speed_delta[] = {1, 1, 2, 5, 10, 20, 50, 100, 0};
  static uint8_t delta_idx = 0;
  
  void loop() {
    
    readchar(); // handle serial port

    // Handle fast/slow tactile buttons
    long nowMillis = millis();
    switch (button_state) {
      case BUTTON_IDLE:
        if (fastButtonPressed()) {
          lastPressedMillis = nowMillis;
          delta_idx = 0;
          changeSpeed(speed_delta[delta_idx+1]);
          button_state = BUTTON_FAST;
        } else if (slowButtonPressed()) {
          lastPressedMillis = nowMillis;
          delta_idx = 0;
          changeSpeed(-speed_delta[delta_idx+1]);
          button_state = BUTTON_SLOW;
        }
        break;
      case BUTTON_FAST:
        if (!fastButtonPressed()) button_state = BUTTON_IDLE;
        if (nowMillis - lastPressedMillis > kButtonAutoRepeat) {
          changeSpeed(speed_delta[delta_idx]);
          if (speed_delta[delta_idx+1] != 0) delta_idx++;
          lastPressedMillis = nowMillis;
        }
        break;
      case BUTTON_SLOW:
        if (!slowButtonPressed()) button_state = BUTTON_IDLE;
        if (nowMillis - lastPressedMillis > kButtonAutoRepeat) {
          changeSpeed(-speed_delta[delta_idx]);
          if (speed_delta[delta_idx+1] != 0) delta_idx++;
          lastPressedMillis = nowMillis;
        }
        break; 
      default:
        button_state = BUTTON_IDLE;
        break;
    }
    
    return;
  }
}

// not truncated
