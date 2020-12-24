/******************************************************
 * Catalin Batrinu bcatalin@gmail.com 
 * Read temperature, humidity and pressure from BME280
 * and send it to thingspeaks.com
*******************************************************/

#include <Wire.h>
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>


#define PIN_BME_SCL   D1
#define PIN_BME_SDA   D2
#define BME_ADR       0x76

Adafruit_BME280 bme; // I2C

/**************************  
 *   S E T U P
 **************************/
void setup() {
  Serial.begin(115200);

  Serial.println ("Sensor Test...");
  Serial.print ("SDA: ");
  Serial.println (SDA);
  Serial.print ("SCL: ");
  Serial.println (SCL);

  
  Serial.println ("Initialize i2c...");
  pinMode (PIN_BME_SDA, INPUT_PULLUP);        //Set input (SDA) pull-up resistor on
  Wire.setClock (2000000);                // Set I2C bus speed 
  Wire.begin (PIN_BME_SDA, PIN_BME_SCL);
//  Wire.begin ();
  Serial.println ("...done");

  Serial.println ("Initialize BME280...");
  if (!bme.begin (BME_ADR))
  {
    Serial.print ("Ooops, no BME280 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }
  else 
  {
    Serial.println ("BME280 ready.");
  }
}

float sensTemp;
float sensHumi;
float sensPres;


  /**************************  
 *  L O O P
 **************************/
void loop() 
{
  sensTemp = bme.readTemperature ();
  Serial.print ("Temperature: ");
  Serial.print (sensTemp);
  Serial.println (" C");

  sensPres = bme.readPressure () / 100.0F;
  Serial.print ("Pressure:    ");
  Serial.print (sensPres);
  Serial.println (" hPa");
  
  sensHumi = bme.readHumidity ();
  Serial.print ("Humidity:    ");
  Serial.print (sensHumi);
  Serial.println (" %");
    
  delay(1000);
}

