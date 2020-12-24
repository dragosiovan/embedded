/*
 * Hello world web server
 * circuits4you.com
 */
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

//SSID and Password to your ESP Access Point
const char* ssid = "ESPWebServer";
const char* password = "12345678";

ESP8266WebServer server(80); //Server on port 80


#define PIN_BME_SCL   D1
#define PIN_BME_SDA   D2

#define BME_ADR 0x76
Adafruit_BME280 bme;

float sensTemp;
float sensHumi;
float sensPres;


//==============================================================
//     This rutine is exicuted when you open its IP in browser
//==============================================================
void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266!");
}

//===============================================================
//                  SETUP
//===============================================================
void setup(void){
  Serial.begin(115200);
  Serial.println("");

  setupSensor ();

  
  WiFi.mode(WIFI_AP);           //Only Access point
  WiFi.softAP(ssid, password);  //Start HOTspot removing password will disable security

  IPAddress myIP = WiFi.softAPIP(); //Get IP address
  Serial.print("HotSpt IP:");
  Serial.println(myIP);
 
  server.on("/", handleRoot);      //Which routine to handle at root location

  server.begin();                  //Start server
  Serial.println("HTTP server started");

  
}
//===============================================================
//                     LOOP
//===============================================================
void loop(void){
  server.handleClient();          //Handle client requests
  readSensors ();

  delay(500);
}






/******************************************************************************
*
* setupSensor - setup all sensors
*
* TODO
*
* RETURNS: N/A
*/

void setupSensor ()
{  
  Serial.println ("Sensor Test...");
  
  Serial.println ("Initialize i2c...");
  pinMode (PIN_BME_SDA, INPUT_PULLUP);        //Set input (SDA) pull-up resistor on
  Wire.setClock (2000000);                // Set I2C bus speed 
  Wire.begin (PIN_BME_SDA, PIN_BME_SCL);
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


/******************************************************************************
*
* readSensors - read all sensors
*
* Read sensors values and publish on MQTT.
*
* RETURNS: N/A
*/

void readSensors ()
{
  sensTemp = bme.readTemperature ();
  Serial.print ("Temperature: ");
  Serial.print (sensTemp);
  Serial.println (" C");

  sensPres = bme.readPressure () * 0.7500637554192 / 100.0F;
  Serial.print ("Pressure:    ");
  Serial.print (sensPres);
  Serial.println (" mmHg");
  
  sensHumi = bme.readHumidity ();
  Serial.print ("Humidity:    ");
  Serial.print (sensHumi);
  Serial.println (" %");
}


