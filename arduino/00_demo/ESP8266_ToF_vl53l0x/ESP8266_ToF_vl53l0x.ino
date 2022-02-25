#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

#define PIN_OUT_LED         D4 //11


void setup() 
{
  int i;
  
  Serial.begin(115200);

  pinMode(PIN_OUT_LED, OUTPUT);
  digitalWrite(PIN_OUT_LED, LOW);

  for (i=0; i<10; i++)
  {
    digitalWrite(PIN_OUT_LED, (i&1));
    delay(100);
  }

  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }
  
  Serial.println("Adafruit VL53L0X test");
  if (!lox.begin(VL53L0X_I2C_ADDR, true, &Wire ,Adafruit_VL53L0X::VL53L0X_SENSE_LONG_RANGE)) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  // power 
  Serial.println(F("VL53L0X API Simple Ranging example\n\n")); 
}


void loop() {
  uint16_t dist = 0; 
  
  VL53L0X_RangingMeasurementData_t measure;
    
//  Serial.print("Reading a measurement... ");
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    dist = measure.RangeMilliMeter;
    Serial.print("Distance(mm): "); Serial.println(dist);
  } else {
    Serial.println(" out of range ");
  }


    if (dist != 0 && dist < 500)
    {
       digitalWrite(PIN_OUT_LED, 0);
    }
    else
    {
      digitalWrite(PIN_OUT_LED, 1);
    }

      delay(100);
//  delay(30);
}
