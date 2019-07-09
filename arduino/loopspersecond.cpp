#include "loopspersecond.h"
#include <Arduino.h>

namespace loopsPerSecond {
  static long lastTimeMillis = 0;
  static long loops = 0;

  void setup() {
    lastTimeMillis = millis();
    loops = 0;
  }

  void loop() {
    long nowMillis = millis();
    loops++;
    if (nowMillis - lastTimeMillis > 1000) {
      Serial.print(loops);
      Serial.println(F(" loops/s"));
      lastTimeMillis = nowMillis;
      loops = 0;
    }
  }
}
// not truncated
