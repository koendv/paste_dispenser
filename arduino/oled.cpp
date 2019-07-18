#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>
#include "oled.h"

static SSD1306AsciiWire o_led;

namespace oled {

  static bool detected = false;

  
  void setup() {

    // autodetect whether display on i2c address 0x3c or 0x3d
    uint8_t i2c_address;
    
    Wire.begin();
    i2c_address = 0x3c;
    Wire.beginTransmission(i2c_address);
    detected = Wire.endTransmission() == 0;
    if (!detected) {
      i2c_address = 0x3d;
      Wire.beginTransmission(i2c_address);
      detected = Wire.endTransmission() == 0;
      if (!detected) return; // not found
    }

    // display found. initialize.
    Wire.setClock(400000L); 
    o_led.begin(&Adafruit128x32, i2c_address); // for 128x32 oled. Use Adafruit128x64 for 128x64 oled.
    o_led.displayRemap(true); // set display orientation
    o_led.setFont(Verdana_digits_24); // This font consists of digits only
    o_led.clear();
    return;
  }

  void clear() {
    if (!detected) return;
    o_led.clear();
  }

  void print(uint16_t speed) {
    if (!detected) return;
    o_led.clear();
    o_led.print(speed);
  }
}
// not truncated
