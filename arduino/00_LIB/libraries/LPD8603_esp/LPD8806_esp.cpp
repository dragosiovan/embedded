/*

*/

#include "LPD8606_esp.h"

#define NOP ; //del++;del++;del++;

/**************************************************************************/
/*! 
    @brief  Instantiates a new LPD8806 class using Software SPI
    @param  n Number of pixels we will be addressing
    @param  dpin Our bit-bang data pin
    @param  cpin Our bit-bag clock pin
*/
/**************************************************************************/
LPD8806::LPD8806(uint16_t n, uint8_t dpin, uint8_t cpin) {
  dataPin = dpin;
  clockPin = cpin;
  numLeds = n;

  pixels = (uint16_t *) malloc (numLeds*2);
  for (uint16_t i=0; i<numLeds; i++) {
    setPixelColor (i, 0, 0, 0);
  }
}

/**************************************************************************/
/*! 
    @brief  Activate hard/soft SPI as appropriate
*/
/**************************************************************************/
void LPD8806::begin(void) {
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  digitalWrite(dataPin, LOW);
  digitalWrite(clockPin, LOW);
}

/**************************************************************************/
/*! 
    @brief  Retrieve number of LEDs in strip 
    @returns  Number of LEDs in strip 
*/
/**************************************************************************/
uint16_t LPD8806::numPixels(void) {
  return numLeds;
}

/**************************************************************************/
/*! 
    @brief  Writes all the LED data to the strip at once! 
*/
/**************************************************************************/
void LPD8806::show(void) {
  uint16_t bit = 0;
  uint16_t i = numLeds;
  uint16_t j;

  /* send out header */

  digitalWrite(dataPin, LOW);
  digitalWrite(clockPin, LOW);
  for (bit = 0; bit < 32; bit++) {
    digitalWrite(clockPin, HIGH);
    NOP;
    digitalWrite(clockPin, LOW);
    NOP;
  }

  /* send each pixel */

  for (i = 0; i < numLeds; i++) {
    for(bit=0x8000; bit; bit >>= 1) {
      if (pixels[i] & bit) digitalWrite(dataPin, HIGH);
      else                 digitalWrite(dataPin, LOW);
    digitalWrite(clockPin, HIGH);
    NOP;
    digitalWrite(clockPin, LOW);
    NOP;
    }
  }

  /* n leds pulse at the end */

  digitalWrite(dataPin, LOW);
  for (i = 0; i < numLeds; i++) {
    digitalWrite(clockPin, HIGH);
    NOP;
    digitalWrite(clockPin, LOW);
    NOP;
  }
}

/**************************************************************************/
/*! 
    @brief  Convert separate R,G,B into combined 32-bit GRB color
    @param  r Red value, 0-127
    @param  g Green value, 0-127
    @param  b Blue value, 0-127
    @returns 21-bit color value in uint32_t with red, green and blue packed
*/
/**************************************************************************/
uint16_t LPD8806::Color(byte r, byte g, byte b) {
  uint16_t data;

  data = r & 0x1F;
  data <<= 5;
  data |= g & 0x1F;
  data <<= 5;
  data |= b & 0x1F;
  data |= 0x80000;

  return data;
}

/**************************************************************************/
/*! 
    @brief  Set pixel color from separate 7-bit R, G, B components
    @param  n Pixel # to change (0 is first pixel)
    @param  r Red value, 0-127
    @param  g Green value, 0-127
    @param  b Blue value, 0-127
*/
/**************************************************************************/
void LPD8806::setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
  uint16_t data;

  if(n >= numLeds) return;

  data = r & 0x1F;
  data <<= 5;
  data |= g & 0x1F;
  data <<= 5;
  data |= b & 0x1F;
  data |= 0x8000;

  pixels[n] = data;
}

/**************************************************************************/
/*! 
    @brief  Set pixel color from 'packed' 32-bit GRB (not RGB) value
    @param  n Pixel # to change (0 is first pixel)
    @param  c Packed color word
*/
/**************************************************************************/
void LPD8806::setPixelColor(uint16_t n, uint16_t c) {
  if (n >= numLeds) return;

  pixels[n] = c | 0x8000;
}

/**************************************************************************/
/*! 
    @brief  Query color from previously-set pixel (returns packed 32-bit GRB value)
    @param  n Pixel # to change (0 is first pixel)
    @returns  Packed color word
*/
/**************************************************************************/
uint16_t LPD8806::getPixelColor(uint16_t n) {
  if(n >= numLeds) return 0;

  return (pixels[n] & 0x7FFF);
}

