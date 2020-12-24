#ifndef LED7SEG_H
#define LED7SEG_H

#include "Arduino.h"
#include <avr/interrupt.h>

namespace Led7Seg 
{
 	void setup(byte _noOfDigits, byte *digitPins, byte *segmentPins);
  void initPort(void);
  void initTimer(void);
  void loadValue(byte *digitsValue);
  void update(void);
  void _overflow();
}
#endif // LED7SEG_H