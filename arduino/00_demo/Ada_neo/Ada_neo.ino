
#include <Adafruit_NeoPixel.h>

#define LED_PIN    D6  // digital pin used to drive the LED strip, (for ESP8266 DMA, must use GPIO3/RX/D9)
#define LED_COUNT  20  // number of LEDs on the strip

Adafruit_NeoPixel neo = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


void setup() 
{
  Serial.begin(115200);

  neo.begin();
  neo.setBrightness(255);
  neo.show();
}

#define BLINK  0xFFC000
//#define OFF_L  0xFF0000
#define OFF_L  0x000000
int state = 0; 
int cnt = 0;
int pos = 0;
// 0 = OFF
// 1 = fill in
// 2 = on
// 3 = fill out

void loop() 
{
//  Serial.print ("cnt ");
//  Serial.println (cnt);
//  Serial.print ("pos ");
//  Serial.println (pos);
  
  if (state == 0)
  {
    delay (10);
    cnt++;
    if (cnt == 35)
    {
      Serial.println ("Switch to state 1");
      state = 1;
      pos = 0;
    }
  }
  else if (state == 1)
  {
    neo.setPixelColor (pos, BLINK);
    neo.show();
    pos ++;
    delay (5);
    if (pos == 20)
    {
      Serial.println ("Switch to state 2");
      state = 2;
      cnt = 0;
      pos = 0;
    }
  }
  else if (state == 2)
  {
    delay (10);
    cnt++;
    if (cnt == 25)
    {
      Serial.println ("Switch to state 0");
      state = 3;
      pos = 0;
    }
  }
  else if (state == 3)
  {
    neo.setPixelColor (pos, OFF_L);
    neo.show();
    pos ++;
    delay (5);
    if (pos == 20)
    {
      Serial.println ("Switch to state 3");
      state = 0;
      cnt = 0;
      pos = 0;
    }

  }
  
  
}



