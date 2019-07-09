/*
 * Breathing Led.
 * Provides simple led on/off, blinking and breathing. 
 * Best if led is connected to a PWM output.
 */

#ifndef BREATHING_LED_H
#define BREATHING_LED_H

#include <arduino.h>

namespace breathingLed {

extern void setup();

extern void loop();

extern void on();

extern void off();

/*
 * Smoothly change LED brightness. Requires pwm.
 */

extern void breathe();

/*
 * blink led.
 * "duty" is a duty cycle from 0 to 256, where 0 makes the pin always LOW and 256 makes the pin always HIGH.
 * "periodMillis" is a period in milliseconds. 
 */
 
extern void blink(uint16_t duty, uint16_t periodMillis);

}  // namespace breathingLed

#endif
// not truncated
