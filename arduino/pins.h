/* Koen De Vleeschauwer 2019. */

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
 * Stepper standby. Set to 0 for stepper motor sleeping, 1 for stepper motor active.
 */

static const uint8_t kStepperStandbyPin = 8;

/* 
 *  The hardware for microstepping is there, but microstepping is not implemented yet.
 *  The stepper pwm pins are connected to Timer1.
 */

static const uint8_t kStepperPwmAPin = 9;
static const uint8_t kStepperPwmBPin = 10;

/* the four wires of the stepper motor. 
 * All wires of the stepper are in one and the same port. (port C). 
 * This allows setting the values of the four wires writing one byte.
 * The definitions here are just for show, stepper pins are hardcoded in motor_io.cpp
 */

// Stepper motor A1 (28byj-48: blue) wire.
static const uint8_t kStepperA1Pin = A3;
// Stepper motor A2 (28byj-48: pink) wire.
static const uint8_t kStepperA2Pin = A2;
// Stepper motor B1 (28byj-48: yellow) wire.
static const uint8_t kStepperB1Pin = A1;
// Stepper motor B2 (28byj-48: orange) wire.
static const uint8_t kStepperB2Pin = A0;
// When running a unipolar stepper from a bipolar driver ic, connect the common lead to ground.

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
