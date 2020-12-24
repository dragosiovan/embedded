#include <Wire.h>
 
#define BME280_REGISTER_CHIPID      0xD0
#define BME280_REGISTER_VERSION     0xD1
 
void setup()
{
  Wire.begin();
 
  Serial.begin(115200);
  while (!Serial);             // Leonardo: wait for serial monitor
  Serial.println("\nI2C Scanner");
}
 
 
void loop()
{
  byte error, address;
  int nDevices;
 
  Serial.println("Scanning...");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    Serial.print ("Check address: ");
    Serial.print (address, HEX);
    Serial.print ("...");
    
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.println("");
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");

      readBmp (address);
 
      nDevices++;
    }
    else if (error==4)
    {
      Serial.println("");
      Serial.print("Unknown error at address 0x");
      Serial.println(address,HEX);
    }
    else
    {
      Serial.println("NA");
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
 
  delay(5000);           // wait 5 seconds for next scan
}




void readBmp (uint8_t addr)
{
  uint8_t value;
  
  Wire.beginTransmission((uint8_t)addr);
  Wire.write((uint8_t)BME280_REGISTER_CHIPID);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)addr, (byte)1);
  value = Wire.read();
  Serial.print ("Value: ");
  Serial.println (value, HEX);
}

