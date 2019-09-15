#include "pins.h"
#include "settings.h"
#include "motor.h"
#include "buttons.h"
#include "breathing_led.h"
#include "oled.h"
#include "fsm.h"

//#define DEBUG 1

namespace fsm {
  
  static const long kIdleTimeoutMillis = 20000; // put motor and driver in standby after 20 seconds idle time.
  unsigned long idleTimeMillis = 0;

  static bool pulled_back = false; // True if previous motion ended in pullback
  static uint16_t dot_steps = 0; // Number of steps to take in single shot mode.

  static const uint8_t STATE_IDLE = 0;           // Waiting for work. If "forward" button pushed and forwardSteps is zero, move to STATE_FORWARD and make beads. If "forward" button pushed and forwardSteps is not zero, move to STATE_DOT_WAIT and make dots. 
  static const uint8_t STATE_FORWARD = 1;        // Making beads. Moving piston forward as long as "push" button is pressed.  
  static const uint8_t STATE_BACKWARD = 2;       // Withdrawing. Moving piston backward as long as "pull" button is pressed.
  static const uint8_t STATE_DOT_WAIT = 3;       // Making dots. "forward" button pressed. Wait for user to release the "forward" button. Then move to STATE_DOT and make dot.
  static const uint8_t STATE_DOT = 4;            // Making dots. Moving piston forward a configured number of steps.
  static const uint8_t STATE_PULLBACK_WAIT = 5;  // After forward movement, if pullbackSteps non-zero then wait for pullback timeout to expire. If user does not press "forward" or "backward" buttons before timeout, move to STATE_PULLBACK.
  static const uint8_t STATE_PULLBACK = 6;       // Pullback. Withdraw a configured number of steps, to stop oozing. 
  static const uint8_t STATE_SLEEP = 7;          // Waiting for work, but with the motor switched off.
  
  static uint8_t state = STATE_IDLE;

  void setup () {
    state = STATE_IDLE;
    return; 
  }

  bool sleeping() {
    return state == STATE_SLEEP;
  }

  inline bool forwardButtonPressed() {
    return buttons::isPressed[kForwardButtonPin] || buttons::isPressed[kFootswitchPin];
  }

  inline bool backwardButtonPressed() {
    return buttons::isPressed[kBackwardButtonPin];
  }

  static uint16_t steps_per_second = 0; 
  
  void motorOn(bool is_forward) {

    uint16_t new_steps_per_second = 0;
 
    if (is_forward) new_steps_per_second = settings::forwardSpeed;
    else new_steps_per_second = settings::backwardSpeed;

    if (new_steps_per_second != steps_per_second) {
      steps_per_second = new_steps_per_second;
      oled::print(steps_per_second);
    }

    motor::setSpeed(steps_per_second, is_forward); // move motor

    // have led blink faster when motor runs faster
    const uint16_t steps_to_led_ratio = settings::kMaxSpeed / 250;
    uint16_t led_period_millis = 500 - steps_per_second / steps_to_led_ratio; // faster than map(steps_per_second, 0, settings::kMaxSpeed, 500, 250);
    led_period_millis = constrain(led_period_millis, 100, 1000); // constrain to reasonable values
    breathingLed::blink(64, led_period_millis); // blink led

    return;
  }

  void motorOff() {
    motor::setSpeed(0, true);
    breathingLed::off();

    if (steps_per_second != settings::forwardSpeed) {
      steps_per_second = settings::forwardSpeed;
      oled::print(steps_per_second);
    }
    
    return;
  }

  void nextState(uint8_t new_state) {
    if (state == new_state) return;
#ifdef DEBUG
    switch(new_state) {
      case STATE_IDLE:
        Serial.println(F("STATE_IDLE"));
        break;
      case STATE_FORWARD:
        Serial.println(F("STATE_FORWARD"));
        break;
      case STATE_BACKWARD:
        Serial.println(F("STATE_BACKWARD"));
        break;
      case STATE_DOT_WAIT:
        Serial.println(F("STATE_DOT_WAIT"));
        break;  
      case STATE_DOT:
        Serial.println(F("STATE_DOT"));
        break;
      case STATE_PULLBACK_WAIT:
        Serial.println(F("STATE_PULLBACK_WAIT"));
        break; 
      case STATE_PULLBACK:
        Serial.println(F("STATE_PULLBACK"));
        break;     
      case STATE_SLEEP:
        Serial.println(F("STATE_SLEEP"));
        break;
      default:
        Serial.println(F("STATE ERROR"));
        break;  
    }
#endif
    state = new_state;
    if ((new_state == STATE_IDLE) || (new_state == STATE_PULLBACK_WAIT))  {
      idleTimeMillis = millis();
    }
  }
  
  void loop () {
    switch (state) {
      case STATE_PULLBACK_WAIT:
        if (millis() - idleTimeMillis > settings::pullbackDelay) {
          pulled_back = settings::forwardSteps != 0; // if working in "dot" mode increase number of steps to compensate for pullback
          motor::steps = 0;
          motorOn(false);
          nextState(STATE_PULLBACK);
          break;
        }
        pulled_back = false;
        // fallthrough to STATE_IDLE
      case STATE_IDLE:
        if (forwardButtonPressed()) {
          if (settings::forwardSteps == 0) {
            motorOn(true);
            nextState(STATE_FORWARD);
          } else {
            breathingLed::on();
            nextState(STATE_DOT_WAIT);
          }
        } else if (backwardButtonPressed()) {
          motorOn(false);
          nextState(STATE_BACKWARD);
        } else if (millis() - idleTimeMillis > kIdleTimeoutMillis) {
          motor::sleep(); // put driver in standby
          oled::clear(); // clear display to avoid burn-in
          breathingLed::breathe();
          nextState(STATE_SLEEP);
        } else if (steps_per_second != settings::forwardSpeed) { // updating display is low priority
          steps_per_second = settings::forwardSpeed;
          oled::print(steps_per_second);
        }
        break;
      case STATE_FORWARD:
        if (!forwardButtonPressed()) {
          motorOff();
          if (settings::pullbackSteps != 0) nextState(STATE_PULLBACK_WAIT);
          else nextState(STATE_IDLE);
        }
        // Handle speed changes
        if (steps_per_second != settings::forwardSpeed) motorOn(true);
        break;
      case STATE_BACKWARD:
        if (!backwardButtonPressed()) {
          motorOff();
          nextState(STATE_IDLE);
        }
        break;
      case STATE_DOT_WAIT:
        if (!forwardButtonPressed()) { // wait for button release
          // if previous forward motion ended in pullback, add pullbackSteps to the number of steps to do.
          // This keeps the amount of paste dispensed the same, whether pullback happened or not.
          dot_steps = settings::forwardSteps;
          if (pulled_back) dot_steps += settings::pullbackSteps;
          pulled_back = false; 
          motor::steps = 0;
          motorOn(true);
          nextState(STATE_DOT);  
        }
        break;
      case STATE_DOT:
        if (motor::steps >= dot_steps) {
          motorOff();
          if (settings::pullbackSteps != 0) nextState(STATE_PULLBACK_WAIT);
          else nextState(STATE_IDLE);
        }
        break;   
      case STATE_PULLBACK:
        if (motor::steps >= settings::pullbackSteps) {
          motorOff();
          nextState(STATE_IDLE); 
        }
        break;  
      case STATE_SLEEP:
        // wake up if user is pushing buttons or configuring using the serial console
        if (forwardButtonPressed() || backwardButtonPressed() || buttons::isPressed[kSlowButtonPin] || buttons::isPressed[kFastButtonPin] || (steps_per_second != settings::forwardSpeed)) {
          motor::wakeup(); // wake up driver
          oled::print(steps_per_second); // wake up display
          breathingLed::off();
          nextState(STATE_IDLE);
        }
        break;
      default:
        nextState(STATE_IDLE);
        break;
    }
  }
}
// not truncated
