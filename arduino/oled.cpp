/* Koen De Vleeschauwer 2019. */

#include <SSD1306Ascii.h>
#include <SSD1306AsciiAvrI2c.h>
#include "oled.h"

static SSD1306AsciiAvrI2c o_led;

// i2c address usually 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

namespace oled {
  
  void setup() {
    o_led.begin(&Adafruit128x32, I2C_ADDRESS);
    // o_led.displayRemap(false); // display orientation
    o_led.setI2cClock(400000); // 400 kHz clock
    o_led.setFont(Verdana_digits_24); // This font consists of digits only
    o_led.clear();
    return;
  }

  void clear() {
    o_led.clear();
  }

  void print(uint16_t speed) {
    o_led.clear();
    o_led.print(speed);
  }
}
// not truncated
