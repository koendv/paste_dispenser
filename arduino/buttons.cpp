/* Koen De Vleeschauwer 2019. */

/* 
 * Reading and debouncing tactile switches.
 * Using the same port for all buttons. This allows reading all buttons in a single byte read. 
 */

#include "buttons.h"
#include <Arduino.h>


static const unsigned long kMillisBetweenReads = 10;

static uint8_t buttonShiftreg[MAX_BUTTON];
static unsigned long lastReadMillis = 0;

namespace buttons {

  bool isPressed[MAX_BUTTON];
  
  void setup() {
    // initialize registers
    for (uint8_t i = 0; i < MAX_BUTTON; i++) {
      buttonShiftreg[i] = 0x0;
      isPressed[i] = false;
    }
    return; 
  }

  /* 
   * Read buttons.
   * Debounce buttons by reading values in a 8-bit shift register.
   */
  void loop() {
    unsigned long now = millis();
    if (now - lastReadMillis < kMillisBetweenReads) return;

    uint8_t port_bits;
    port_bits = PIND; // All buttons are connected to port D.
    lastReadMillis = now;
    
    /* Debouncing. Set if button pressed for eight consecutive cycles; clear if released for eight consecutive cycles;
     * keep old value if bouncing.
     */
    for (uint8_t i = 0; i < MAX_BUTTON; i++) {
      // extract button bit from port 
      bool pressed = !((port_bits >> i) & 0x1); // buttons are active low
      // add to shift register
      buttonShiftreg[i] <<= 1;
      if (pressed)  buttonShiftreg[i] |= 0x1;
      // button pressed if eight consecutive ones, released if eight consecutive zeroes.
      if (buttonShiftreg[i] == 0x00) isPressed[i] = false;
      else if (buttonShiftreg[i] == 0xff) isPressed[i] = true; 
    }
    return;
  } 
}
// not truncated
