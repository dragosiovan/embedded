#include "LPD8606_esp.h"
//#include "SPI.h"

// Simple test for 260 (5 meters) of LPD8806-based RGB LED strip, this is a
// modified version of the original demo - it creates a rainbow, animates it
// and fades from black->colours->white->colours->black in a cycle, holding
// somewhat longer on the full colour, all while cycling the rainbow colours.

// This has been tested with a high density (260 LEDs / 5m) strip and a nano
// board, running from a 5V 10A PSU.

// From https://gist.github.com/tomoinn/5969009

/*****************************************************************************/

// Number of RGB LEDs in strand:
const int nLEDs = 10;
const int cMAX = 0x1F;

// Chose 2 pins for output; can be any valid output pins. These aren't used
// in the code as it stands as I'm using the hardware SPI support on the nano
// int dataPin  = 2;
// int clockPin = 3;
int dataPin = D0;
int clockPin = D1;

// The offset is used to cycle colours along the strip
int offset = 0;

// Multiple is used to space out the spectrum, a value of 4 will be four 
// times as many cycles as a value of 1 on any given length
const int multiple = 4;

// Number of steps the colour pattern advances each tick
const int offsetDelta = 2;

// First parameter is the number of LEDs in the strand.  The LED strips
// are 32 LEDs per meter but you can extend or cut the strip.  Next two
// parameters are SPI data and clock pins:
//LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);

// You can optionally use hardware SPI for faster writes, just leave out
// the data and clock pin parameters.  But this does limit use to very
// specific pins on the Arduino.  For "classic" Arduinos (Uno, Duemilanove,
// etc.), data = pin 11, clock = pin 13.  For Arduino Mega, data = pin 51,
// clock = pin 52.  For 32u4 Breakout Board+ and Teensy, data = pin B2,
// clock = pin B1.  For Leonardo, this can ONLY be done on the ICSP pins.

// Hardware SPI on the nano uses clock = 13, data = 11
LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);

void rainbow (uint8_t wait, uint8_t st);
void static_rainbow (uint8_t wait, uint8_t st);

void setup() {
  Serial.begin(115200);
  
  // Start up the LED strip
  strip.begin();
  // Update the strip, to start they are all 'off'
  strip.show();
}

void loop() {

  //rainbow (50, 3);
  static_rainbow (70, 6);
}


void rainbow (uint8_t wait, uint8_t st)
{
  static uint8_t pos = 0;
  static uint8_t mode = 0;
  static uint8_t cnt = 0;
  static uint8_t r = 0x1F;
  static uint8_t g = 0;
  static uint8_t b = 0;
  

  switch (mode)
  {
    case 0: 
      r -= st;
      g += st;
      if (g > cMAX) { mode = 1; g = cMAX; r = 0; }
      break;
    case 1: 
      g -= st;
      b += st;
      if (b > cMAX) { mode = 2; b = cMAX; g = 0; }
      break;
    case 2: 
      b -= st;
      r += st;
      if (b > cMAX) { mode = 0; r = cMAX; b = 0; }
      break;
  }


//  Serial.print ("Print: ");
//  Serial.print (r);
//  Serial.print (" ");
//  Serial.print (g);
//  Serial.print (" ");
//  Serial.println (b);
  strip.setPixelColor (pos, r, g, b);
  strip.show ();
  delay (wait);

  pos++;
  if (pos >= nLEDs)
    pos = 0;
}


void getColor (uint8_t color, uint8_t *r, uint8_t *g, uint8_t *b)
{
  switch (color/32)
  {
    case 0:
      *r = 31 - color%32;
      *g = ~(*r) & 0x1F;
      *b = 0;
      break;
    case 1:
      *g = 31 - color%32;
      *b = ~(*g) & 0x1F;
      *r = 0;
      break;    
    case 2:
      *b = 31 - color%32;
      *r = ~(*b) & 0x1F;
      *g = 0;

      break;    
  }
}


void static_rainbow (uint8_t wait, uint8_t st)
{
  static uint8_t cur = 0;
  uint8_t i = 0;
  uint8_t r;
  uint8_t g;
  uint8_t b;


//  Serial.print (cur);
  
  for (i = 0; i < nLEDs; i++)
  {
    getColor ((cur+i*st) % 96, &r, &g, &b);
    strip.setPixelColor (i, r, g, b);
//    Serial.print (" ");
//    Serial.print (r, HEX);
//    Serial.print (" ");
//    Serial.print (g, HEX);
//    Serial.print (" ");
//    Serial.print (b, HEX);
//    Serial.print ("  ");
  }  

//  Serial.println ("---");


  strip.show ();
  delay (wait);

  cur ++;
  if (cur >= 96) cur = 0;  
}

