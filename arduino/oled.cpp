#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>
#include "oled.h"

static SSD1306AsciiWire o_led;

// i2c address usually 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

namespace oled {

  static bool detected = false;
  
  void setup() {
    Wire.begin();
    Wire.beginTransmission(I2C_ADDRESS);
    detected = Wire.endTransmission() == 0;
    if (!detected) return;

    Wire.setClock(400000L); 
    o_led.begin(&Adafruit128x32, I2C_ADDRESS);
    o_led.displayRemap(true); // display orientation
    o_led.setFont(Verdana_digits_24); // This font consists of digits only
    o_led.clear();
    return;
  }

  void clear() {
    if (detected)
      o_led.clear();
  }

  void print(uint16_t speed) {
    if (detected) {
      o_led.clear();
      o_led.print(speed);
    }
  }
}
// not truncated
