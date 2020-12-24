#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// assign the SPI bus to pins
#define BME_SCK  D1
#define BME_MISO D5
#define BME_MOSI D2
#define BME_CS   D3
#define BME2_CS  D4

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI
//Adafruit_BME280 bme2(BME2_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

unsigned long delayTime;

void setup() {
    Serial.begin(115200);
    Serial.println(F("BME280 test"));

    bool status;
    
    // default settings
    status = bme.begin();
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
    }

//    // default settings
//    status = bme2.begin();
//    if (!status) {
//        Serial.println("Could not find a valid BME280 sensor 2 , check wiring!");
//    }    
    
    delayTime = 5000;

    Serial.println();
}


void loop() { 
    printValues();
    delay(delayTime);
}


void printValues() {
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");
//    Serial.print("Temperature = ");
//    Serial.print(bme2.readTemperature());
//    Serial.println(" *C");    

    Serial.print("Pressure = ");
    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");
//    Serial.print("Pressure = ");
//    Serial.print(bme2.readPressure() / 100.0F);
//    Serial.println(" hPa");    

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");
//    Serial.print("Approx. Altitude = ");
//    Serial.print(bme2.readAltitude(SEALEVELPRESSURE_HPA));
//    Serial.println(" m");    

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");
//    Serial.print("Humidity = ");
//    Serial.print(bme2.readHumidity());
//    Serial.println(" %");

    Serial.println();
}
