#ifndef PINS_H
#define PINS_H 1
#include <Arduino.h>

/* Pin definitions. These come from the schematic.
 * Pin assignment follows the following logic:
 * - buttons are in one and the same atmega port. This allows reading all buttons in one read. (see buttons.cpp)
 * - stepper motor wires are in one and the same atmega port. This allows going to the next step in one write. (see motor_io.cpp)
 */

/*
 * On the solder paste dispenser, the led is connected to pwm-capable pin 3. 
 * On an arduino, the led is connected to not pwm-capable pin LED_BUILTIN. 
 * If you want to run this sketch on an arduino, change the following:
 * In param.h:
 * static const uint8_t kLedPin = LED_BUILTIN;
 * In breathing_led.cpp replace
 * #define LED_PWM 1
 * with
 * #define LED_PWM 0
 */

/*
 * Stepper pins are defined in tb6612.cpp
 */

/* buttons
 * All buttons are in one and the same port. (port D). 
 * This allows getting the values of the buttons reading one byte.
 * The port is hardcoded in buttons.h.
 * For port D the pin numbers are also the index of the buttons in buttons::isPressed[].
 */

// Arduino input pin for the foot pedal switch. Active low.
static const uint8_t kFootswitchPin = 2;
// Arduino input pin for the "forward" (push) button. Active low.
static const uint8_t kForwardButtonPin = 7;
// Arduino input pin for the "backward" (pull) button. Active low.
static const uint8_t kBackwardButtonPin = 4;
// Arduino input pin for the "Fast"  button. Active low.
static const uint8_t kFastButtonPin = 6;
// Arduino input pin for the "Slow" button. Active low.
static const uint8_t kSlowButtonPin = 5;

/* Led */

// Arduino pwm output pin for status led.
static const uint8_t kLedPin = 3;

#endif
// not truncated
