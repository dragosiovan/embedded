/******************************************************
 * Catalin Batrinu bcatalin@gmail.com 
 * Read temperature, humidity and pressure from BME280
 * and send it to thingspeaks.com
*******************************************************/

#include <Wire.h>
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>




Adafruit_BME280 bme; // I2C
// replace with your channel’s thingspeak API key,
String apiKey = "YOUR-API-KEY";
const char* ssid = "YOUR-SSID";
const char* password = "YOUR-ROUTER-PASSWORD";
const char* server = "api.thingspeak.com";
WiFiClient client;


/**************************  
 *   S E T U P
 **************************/
void setup() {
  Serial.begin(115200);
  Serial.println(F("BMP280 test"));
  
  if (!bme.begin(0x76)) {  
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  WiFi.begin(ssid, password);
  
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
}

  /**************************  
 *  L O O P
 **************************/
void loop() {

    if (client.connect(server,80))  // "184.106.153.149" or api.thingspeak.com
    {
        String postStr = apiKey;
        postStr +="&field1=";
        postStr += String(bme.readTemperature());
        postStr +="&field2=";
        postStr += String(bme.readHumidity());
        postStr +="&field3=";
        postStr += String(bme.readPressure() / 100.0F);
        postStr += "\r\n\r\n";
        
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(postStr.length());
        client.print("\n\n");
        client.print(postStr);    
    }
    client.stop(); 
    //every 20 sec   
    delay(20000);
}
