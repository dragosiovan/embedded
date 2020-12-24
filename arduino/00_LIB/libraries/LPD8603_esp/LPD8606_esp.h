#ifndef LPD8806_H
#define LPD8806_H

#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
 #include <pins_arduino.h>
#endif

/**************************************************************************/
/*! 
    @brief  Class that stores state and functions for interacting with LPD8806 LED strips
*/
/**************************************************************************/
class LPD8806 {

 public:

  LPD8806(uint16_t n, uint8_t dpin, uint8_t cpin); // Configurable pins
  void begin(void);
  void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
  void setPixelColor(uint16_t n, uint16_t c);
  void show(void);
  uint16_t numPixels(void);
  uint16_t Color(byte, byte, byte);
  uint16_t getPixelColor(uint16_t);

 private:

  uint16_t   numLeds;    // Number of RGB LEDs in strip
  uint16_t * pixels;     // Holds LED color values (3 bytes each) + latch bytes
  int8_t     clockPin;
  int8_t     dataPin;     // Clock & data pin numbers
  int8_t     del;         // delay

};
#endif
