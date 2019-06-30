/* Koen De Vleeschauwer 2019. */

/*
 * oled. 
 */

#ifndef OLED_H
#define OLED_H

#include <arduino.h>

namespace oled {

extern void setup();
extern void clear();
extern void print(uint16_t speed);

}  // namespace oled

#endif
// not truncated
