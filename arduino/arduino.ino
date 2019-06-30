/* Koen De Vleeschauwer 2019. */

#include "pins.h"
#include "settings.h"
#include "motor.h"
#include "buttons.h"
#include "breathing_led.h"
#include "oled.h"
#include "fsm.h"
#include "loopspersecond.h"

//#define DEBUG

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print(F("setup"));

  pinMode(kStepperStandbyPin, OUTPUT);
  pinMode(kStepperPwmAPin, OUTPUT);
  pinMode(kStepperPwmBPin, OUTPUT);
  digitalWrite(kStepperStandbyPin, HIGH);
  digitalWrite(kStepperPwmAPin, HIGH);
  digitalWrite(kStepperPwmBPin, HIGH);

  /* buttons */
  pinMode(kFootswitchPin, INPUT_PULLUP);
  pinMode(kForwardButtonPin, INPUT_PULLUP);
  pinMode(kBackwardButtonPin, INPUT_PULLUP);
  pinMode(kFastButtonPin, INPUT_PULLUP);
  pinMode(kSlowButtonPin, INPUT_PULLUP);

  /* led */
  pinMode(13, INPUT); // XXX
  pinMode(kLedPin, OUTPUT);

  Serial.print(F(" motor"));
  motor::setup();
  Serial.print(F(" buttons"));
  buttons::setup();
  Serial.print(F(" oled"));
  oled::setup();
  Serial.print(F(" led"));
  breathingLed::setup();
  Serial.print(F(" settings"));
  settings::setup();
  
  fsm::setup();
  loopsPerSecond::setup();

  motor::setSpeed(0, true);
  
  Serial.println();
  Serial.println(F("ready"));

  return;
};

static uint8_t prio_cnt = 0; // separate tasks in low and high priority

void loop() {
#ifdef DEBUG
  loopsPerSecond::loop(); // approx. 50000 loops per second when running at 500 steps/second
#endif

  motor::loop();
  fsm::loop();
  buttons::loop();

  // Run settings and led once out of every 16 loops.
  if ((prio_cnt++ & 0xf) != 0) return;

  settings::loop();
  breathingLed::loop();
  
  return;
};
// not truncated
