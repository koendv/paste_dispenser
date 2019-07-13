/*
 * Driver for tb6612fng stepper.
 * 
 * TB6612 connections:
 * - AIN1, AIN2, BIN1, BIN2 have to be connected to the same atmega port. Here: port C. 
 * - AIN1, AIN2, BIN1, BIN2 are defined, not as pin numbers, but as bits within that port. 
 * - PWMA, PWMB are connected to Timer1 OCR1A, OCR1B
 * - STANDBY is connected to a digital output pin.
 */
 
#include <Arduino.h>
#include <TimerOne.h>
#include "tb6612.h"
#include "settings.h"

// for maximum torque (square path microstepping) define MAXTORQUE 1. for maximum smoothness (sine-cosine microstepping), undefine MAXTORQUE.

#define MAXTORQUE 1

// Connect all 4 motor pins to a single atmega port so all pins can be updated in a single write. 
// Using port C.

#define MOTOR_PORT      (PORTC)
#define MOTOR_PORT_DDR  (DDRC)

namespace tb6612 {

  // bitmasks for the stepper wires.
  
  static const uint8_t AIN1 = (1 << 2); // Coil A+ corresponds to port C bit 2. (PC2)
  static const uint8_t AIN2 = (1 << 3); // Coil A- corresponds to port C bit 3. (PC3)
  static const uint8_t BIN1 = (1 << 1); // Coil B+ corresponds to port C bit 1. (PC1)
  static const uint8_t BIN2 = (1 << 0); // Coil B- corresponds to port C bit 0. (PC0)
  
  static const uint8_t kAllPins = AIN1 | AIN2 | BIN1 | BIN2; // mask for all stepper wires.
  
  // The stepper pwm pins are connected to Timer1 for microstepping.
  
  static const uint8_t kStepperPwmAPin = 9;
  static const uint8_t kStepperPwmBPin = 10;
  
  // Stepper standby pin. Set to 0 for stepper motor sleeping, 1 for stepper motor active.
  
  static const uint8_t kStepperStandbyPin = 8;
  
  // The following tables have been calculated by the program "microstepping" in the data/ directory. Assumes 25 kHz PWM on Timer1 on a 16 MHz atmega328p.
  
  // coil energizing sequence
  static const uint8_t PROGMEM step_table[kMaxMicrosteps*4] = {BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1|BIN1, AIN1, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, AIN1|BIN2, BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2|BIN2, AIN2, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1, AIN2|BIN1};
  
  #ifdef MAXTORQUE
  // square phasor for max torque
  static const uint16_t PROGMEM ocr1_table[kMaxMicrosteps*4] = {0, 7, 15, 23, 31, 39, 47, 55, 63, 71, 80, 88, 96, 105, 114, 123, 132, 141, 151, 160, 170, 181, 191, 202, 213, 225, 237, 249, 262, 275, 289, 304, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 304, 289, 275, 262, 249, 237, 225, 213, 202, 191, 181, 170, 160, 151, 141, 132, 123, 114, 105, 96, 88, 80, 71, 63, 55, 47, 39, 31, 23, 15, 7, 0, 7, 15, 23, 31, 39, 47, 55, 63, 71, 80, 88, 96, 105, 114, 123, 132, 141, 151, 160, 170, 181, 191, 202, 213, 225, 237, 249, 262, 275, 289, 304, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 319, 304, 289, 275, 262, 249, 237, 225, 213, 202, 191, 181, 170, 160, 151, 141, 132, 123, 114, 105, 96, 88, 80, 71, 63, 55, 47, 39, 31, 23, 15, 7};
  #else
  // sine-cosine stepping for max smoothness
  static const uint16_t PROGMEM ocr1_table[kMaxMicrosteps*4] = {0, 7, 15, 23, 31, 39, 46, 54, 62, 70, 77, 85, 92, 100, 107, 115, 122, 129, 136, 143, 150, 157, 164, 170, 177, 184, 190, 196, 202, 208, 214, 220, 225, 231, 236, 242, 247, 251, 256, 261, 265, 270, 274, 278, 281, 285, 289, 292, 295, 298, 300, 303, 305, 308, 310, 311, 313, 315, 316, 317, 318, 318, 319, 319, 319, 319, 319, 318, 318, 317, 316, 315, 313, 311, 310, 308, 305, 303, 300, 298, 295, 292, 289, 285, 281, 278, 274, 270, 265, 261, 256, 251, 247, 242, 236, 231, 225, 220, 214, 208, 202, 196, 190, 184, 177, 170, 164, 157, 150, 143, 136, 129, 122, 115, 107, 100, 92, 85, 77, 70, 62, 54, 46, 39, 31, 23, 15, 7, 0, 7, 15, 23, 31, 39, 46, 54, 62, 70, 77, 85, 92, 100, 107, 115, 122, 129, 136, 143, 150, 157, 164, 170, 177, 184, 190, 196, 202, 208, 214, 220, 225, 231, 236, 242, 247, 251, 256, 261, 265, 270, 274, 278, 281, 285, 289, 292, 295, 298, 300, 303, 305, 308, 310, 311, 313, 315, 316, 317, 318, 318, 319, 319, 319, 319, 319, 318, 318, 317, 316, 315, 313, 311, 310, 308, 305, 303, 300, 298, 295, 292, 289, 285, 281, 278, 274, 270, 265, 261, 256, 251, 247, 242, 236, 231, 225, 220, 214, 208, 202, 196, 190, 184, 177, 170, 164, 157, 150, 143, 136, 129, 122, 115, 107, 100, 92, 85, 77, 70, 62, 54, 46, 39, 31, 23, 15, 7};
  #endif
  
  // index in stepper table.
  static uint8_t idx = 0;
  static uint8_t idx_step = kMaxMicrosteps/2; // default is half-stepping
  static uint8_t new_idx_step = 0;
  
  /*
   * Update stepper pins.
   * Direct write to atmega registers for speed.
   */
  static inline void setOutputPins() {
    uint8_t idx_b = idx + kMaxMicrosteps;
    uint8_t pin_updt = pgm_read_byte(step_table + idx);
    uint16_t ocr1a_updt = pgm_read_word(ocr1_table + idx);
    uint16_t ocr1b_updt = pgm_read_word(ocr1_table + idx_b);
    
    cli();
    MOTOR_PORT = ((MOTOR_PORT & ~kAllPins) | pin_updt);
    OCR1A = ocr1a_updt;
    OCR1B = ocr1b_updt;
    sei();
  }
  
  void step(bool clockwise) {
    // Only change microstepping at an index where old and new steppings coincide.
    if ((new_idx_step != 0) && ((idx & (new_idx_step - 1)) == 0)) {
      idx_step = new_idx_step;
      new_idx_step = 0;
    }
    
    if (clockwise) idx+=idx_step; else idx-=idx_step; // No worries, we're using an 8-bit index on a 256-entry table.
    setOutputPins();
  }

  /*
   * microstep setting: 1, 1/2, 1/4, 1/8, 1/16, 1/32, 1/64 microstepping.
   * Microstepping can be changed at any moment.
   */
  void setMicrosteps(uint8_t microsteps) {
    switch(microsteps) {
      case 64:
        new_idx_step = 1; // 1/64 microstep
        break;
      case 32:
        new_idx_step = 2; // 1/32 microstep
        break;
      case 16:
        new_idx_step = 4; // 1/16 microstep
        break;
      case 8:
        new_idx_step = 8; // 1/8 microstep
        break;
      case 4:
        new_idx_step = 16; // 1/4 microstep
        break;
      default:
      case 2: 
        new_idx_step = 32; // half step
        break;
      case 1: 
        new_idx_step = 64; // full step
        break;   
    }
  }
  
  // Standby mode. When in standby, the motor does not resist turning, but does not warm up either.
  
  void sleep() {
    digitalWrite(kStepperStandbyPin, false);
  }
  
  void wakeup() {
    digitalWrite(kStepperStandbyPin, true);
  }
  
  // Initialisation routines
  
  void setup() {
    // initialize AIN1 .. BIN2 as outputs.
    MOTOR_PORT_DDR |= kAllPins;
  
    // set PWM frequency to 25 kHz = 40 usec period
    Timer1.initialize(40);  // 40 us = 25 kHz
  
    // initialize PWM pins
    static const uint16_t kAlwaysOn = 1023;
    Timer1.pwm(kStepperPwmAPin, kAlwaysOn);
    Timer1.pwm(kStepperPwmBPin, kAlwaysOn);
  
    // initialize position
    idx = 0;
    setMicrosteps(settings::microSteps); // default is half-stepping
    setOutputPins();

    // initialize standby pin
    pinMode(kStepperStandbyPin, OUTPUT);
    wakeup();
  }

}
// not truncated
