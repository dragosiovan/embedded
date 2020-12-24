#include <SPI.h>

// HW SPI
#define BME_SCK  D5
#define BME_MISO D6
#define BME_MOSI D7
#define BME_CS   D3

#define BME280_REGISTER_CHIPID             0xD0
#define BME280_REGISTER_VERSION            0xD1

int value;

void setup() {
  Serial.begin (115200);

  digitalWrite (BME_CS, HIGH);
  pinMode (BME_CS, OUTPUT);
  SPI.begin ();
}

void loop() {

  SPI.beginTransaction (SPISettings (500000, MSBFIRST, SPI_MODE0));
  digitalWrite (BME_CS, LOW);
  SPI.transfer (BME280_REGISTER_CHIPID | 0x80); // read operation MSB = 1
  value = SPI.transfer (0);
  digitalWrite (BME_CS, HIGH);
  SPI.endTransaction (); // release the SPI bus

  Serial.print ("CHIP ID: 0x");
  Serial.println (value, HEX);

  delay (2000);
}
