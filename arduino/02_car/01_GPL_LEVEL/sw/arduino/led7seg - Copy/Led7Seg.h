#ifndef LED7SEG_H
#define LED7SEG_H

#include "Arduino.h"
#include <avr/interrupt.h>

namespace Led7Seg 
{
 	void setup();
  void initPort(void);
  void initTimer(void);
  void loadValue(unsigned int value);
  void updateLed7Seg(void);
}
#endif // LED7SEG_H