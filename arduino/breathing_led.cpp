/*
 * Breathing Led.
 * Provides simple led on/off, blinking and breathing. 
 * "breathe" effect requires led connected to a PWM output.
 */

#include "breathing_led.h"
#include "pins.h"

// define LED_PWM if kLedPin supports pwm.
#define LED_PWM 1

namespace breathingLed {

const unsigned long kBreathingPeriod = 10; /* 10 millis. 2 * 256 * 10 milli = 5 seconds. */ 
const unsigned int kAlwaysOn = 255; /* Duty cycle for "always on" */

static bool ledOn = false;

/* when blinking the led is switched on onTimeMillis milliseconds, off offTimeMillis milliseconds */ 
static uint16_t onTimeMillis = 0;
static uint16_t offTimeMillis = 0;

/* timer */
static unsigned long lastTimeMillis = 0;
static unsigned long timerMillis = 0;

/* breathing */
static uint16_t breathe_x = 0;
static uint16_t brightness = 0;
bool breathe_deeper = true;
static bool breathing = false;

void setup() {
  off();
  return;
}

extern void breathe() {
#ifdef LED_PWM
  // led pin supports analogWrite
  if (breathing) return;
  /* initialize values used to calculate brightness */
  breathe_x = 0;
  brightness = 0;
  breathe_deeper = true;
  /* set timers */
  breathing = true;
  ledOn = true;
  timerMillis = kBreathingPeriod;
  lastTimeMillis = millis();
  return;
#else
  // led pin does not support analogWrite - just blink once every five seconds
  blink(1, 5000);
#endif
}

extern void on(){
  blink(256, 250);
  return;
}

extern void off(){
  blink(0, 250);
  return;
}

/*
 * "duty" is a duty cycle from 0 to 256, where 0 makes the pin always LOW and 256 makes the pin always HIGH.
 * "period_milli" is a period in microseconds.
 */
extern void blink(uint16_t duty, uint16_t periodMillis) {
  breathing = false;
  onTimeMillis = periodMillis * duty / 256;
  offTimeMillis = periodMillis - onTimeMillis;
  lastTimeMillis = millis();
  if (onTimeMillis != 0) {
    digitalWrite(kLedPin, HIGH);
    ledOn = true;
    timerMillis = onTimeMillis;
  }
  else {
    digitalWrite(kLedPin, LOW);
    ledOn = false;
    timerMillis = offTimeMillis;
  }
  return;
}

extern void loop(){
  unsigned long now = millis();
  if (now - lastTimeMillis < timerMillis) return;
  lastTimeMillis = now;
  if (breathing) {
    uint16_t delta = breathe_x + breathe_x + 1; // (x + 1)^2 = x^2 + 2 * x + 1
    if (breathe_deeper) {
      brightness += delta;
      if (breathe_x == 254) breathe_deeper = false; // stop at x = 254. x = 255 gives 65536, which overflows uint16_t.
      else breathe_x++;  
    } else {
      brightness -= delta;
      if (breathe_x == 0) breathe_deeper = true;
      else --breathe_x;
    }
    uint16_t duty_cycle = brightness >> 8;
    analogWrite(kLedPin, duty_cycle);
  } else if (ledOn && (offTimeMillis != 0)) {
    digitalWrite(kLedPin, 0);
    ledOn = false;
    timerMillis = offTimeMillis;
  } else if (!ledOn && (onTimeMillis != 0)) {
    digitalWrite(kLedPin, 1);
    ledOn = true;
    timerMillis = onTimeMillis;
  }
  return;
}
  
}  // namespace breathingLed
