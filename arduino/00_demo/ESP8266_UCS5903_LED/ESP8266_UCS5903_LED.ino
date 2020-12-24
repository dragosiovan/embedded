#define CLK   D0
#define DATA  D1

#define NOP del++;del++;del++;

uint8_t dataPin = D0;
uint8_t clockPin = D1;


uint8_t numLeds = 10;
uint16_t pixels[10];
int i;
uint16_t bit;
int del;

void display ()
{
/* first send 32 clocks */
  digitalWrite(dataPin, LOW);
  digitalWrite(clockPin, LOW);
  for (bit = 0; bit < 32; bit++) {
    digitalWrite(clockPin, HIGH);
//    NOP;
    digitalWrite(clockPin, LOW);
//    NOP;
  }

  for (i = 0; i < numLeds; i++) {
    for(bit=0x8000; bit; bit >>= 1) {
      if (pixels[i] & bit) digitalWrite(dataPin, HIGH);
      else                 digitalWrite(dataPin, LOW);
    digitalWrite(clockPin, HIGH);
//    NOP;
    digitalWrite(clockPin, LOW);
//    NOP;
    }
  }

  /* n leds pulse at the end */

  digitalWrite(dataPin, LOW);
  for (i = 0; i < numLeds; i++) {
    digitalWrite(clockPin, HIGH);
//    NOP;
    digitalWrite(clockPin, LOW);
//    NOP;
  }
  
}


void set_pixel (uint8_t pixel, uint8_t r, uint8_t g, uint8_t b)
{
  pixels[pixel] = r & 0xF;
  pixels[pixel] <<= 5;
  pixels[pixel] |= g & 0x1;
  pixels[pixel] <<= 5;
  pixels[pixel] |= b & 0xF;
  pixels[pixel] |= 0x8000;
  Serial.print ("Pixel ");
  Serial.print (pixel);
  Serial.print (" value ");
  Serial.println (pixels[pixel]);
  
}


void setup() {
  Serial.begin (115200);
  
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  digitalWrite(dataPin, LOW);
  digitalWrite(clockPin, LOW);
  for (i = 0; i < numLeds; i++)
  {
    set_pixel (i, 0x1F, 0x1F, 0x1F);
  }
  display ();
}


void loop() {
  delay (1000);
}
