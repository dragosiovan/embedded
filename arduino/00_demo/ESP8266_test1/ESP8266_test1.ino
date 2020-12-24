
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"


typedef enum {
  CLIMA_STATUS_OFF = 0,
  CLIMA_STATUS_OK = 1,
  CLIMA_STATUS_COOL = 2,
  CLIMA_STATUS_HEAT = 3
};

typedef enum {
  CLIMA_CTRL_OFF = 0,
  CLIMA_CTRL_AUTO = 1,
  CLIMA_CTRL_COOL = 2,
  CLIMA_CTRL_HEAT = 3
};

/************************* WiFi Access Point *********************************/
const char * ssid = "embedded";
const char * password = "04523307";

/************************* MQTT Broker Setup *********************************/

#define MQTT_SERVER      "10.0.0.192"
#define MQTT_SERVERPORT  1883
#define MQTT_USERNAME    ""
#define MQTT_KEY         ""


// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_SERVERPORT);


/****************************** Feeds ***************************************/

Adafruit_MQTT_Subscribe mqSetRgb       = Adafruit_MQTT_Subscribe (&mqtt, "/set/rgb");
Adafruit_MQTT_Subscribe mqSetLed       = Adafruit_MQTT_Subscribe (&mqtt, "/set/light");
Adafruit_MQTT_Subscribe mqSetClima     = Adafruit_MQTT_Subscribe (&mqtt, "/set/clima");
Adafruit_MQTT_Subscribe mqSetTemp      = Adafruit_MQTT_Subscribe (&mqtt, "/set/temp");
Adafruit_MQTT_Publish   mqtStatClima   = Adafruit_MQTT_Publish   (&mqtt, "/status/clima");
Adafruit_MQTT_Publish   mqTemp         = Adafruit_MQTT_Publish   (&mqtt, "/sens/temp");
Adafruit_MQTT_Publish   mqHumi         = Adafruit_MQTT_Publish   (&mqtt, "/sens/humi");
Adafruit_MQTT_Publish   mqPres         = Adafruit_MQTT_Publish   (&mqtt, "/sens/pres");



int ledPin = D4; 
int ledLevel = 0;

float sensTemp;
float sensHumi;
float sensPres;
int setClimaMode = CLIMA_CTRL_OFF;
int stateClima = CLIMA_STATUS_OFF;
int setClimaTemp = 0;



#define BME_SDA D3
#define BME_SDL D2
#define BME_ADR 0x76
Adafruit_BME280 bme; // Note Adafruit assumes I2C adress = 0x77 my module (eBay) uses 0x76 so the library address has been changed.


/* forward declarations */

void mqtt_reconnect ();
void setup_wifi ();
void setup_sensor ();
void lRgbCblk (char *data, uint16_t len);
void lLedCblk (double val);
void sClimaCblk (double state);
void sTempCblk (double val);
void checkClima (void);

void setup() 
{
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println("Initializing");
  Serial.println();

  setup_wifi ();
  setup_sensor ();
  setup_mqtt ();

  pinMode (ledPin, OUTPUT);

  Serial.println("...done");  
  Serial.println();
}

void loop () 
{
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  mqtt_reconnect();

  // this is our 'wait for incoming subscription packets and callback em' busy subloop
  // try to spend your time here:
  mqtt.processPackets(1000);

//  // ping the server to keep the mqtt connection alive
//  // NOT required if you are publishing once every KEEPALIVE seconds
//  
//  if(! mqtt.ping()) {
//    mqtt.disconnect();
//  }



// for (int i=0; i < 1024; i++){
//      analogWrite(2, i);
//      delay(1);
//   } 
//   for (int i=1023; i > 0; i--){
//      analogWrite(2, i);
//      delay(1);
//   } 
  
//  //ledPin = 2;
//  ledPin = D4;
//  Serial.print ("Pin ");
//  Serial.println(ledPin);
//  pinMode (ledPin, OUTPUT);
//  Serial.println (" -set low");
//  digitalWrite (ledPin, LOW); 
//  delay (1000);
//  Serial.println (" -set high");
//  digitalWrite (ledPin, HIGH); 
//  delay (1000);


  sensTemp = bme.readTemperature();
  Serial.print("Temperature: ");
  Serial.print(sensTemp);
  Serial.println(" C");
  if (!mqTemp.publish(sensTemp)) {
    Serial.println("Publish Temperature: Failed");
  } 

  sensPres = bme.readPressure() / 100.0F;
  Serial.print("Pressure:    ");
  Serial.print(sensPres);
  Serial.println(" hPa");
  if (! mqPres.publish(sensPres)) {
    Serial.println("Publish Pressure: Failed");
  } 
  
  sensHumi = bme.readHumidity();
  Serial.print("Humidity:    ");
  Serial.print(sensHumi);
  Serial.println(" %");
  if (! mqHumi.publish(sensHumi)) {
    Serial.println("Publish Humidity: Failed");
  } 

//  delay(1000);
}


/************************* Sensor setup **************************************/

void setup_sensor ()
{
  Serial.println("Sensor Test...");
  
  Serial.println("Initialize i2c...");
  pinMode(BME_SDA, INPUT_PULLUP); //Set input (SDA) pull-up resistor on
  Wire.setClock(2000000);    // Set I2C bus speed 
  Wire.begin(BME_SDA, BME_SDL);
  Serial.println("...done");

  Serial.println("Initialize BME280...");
  if (!bme.begin(BME_ADR))
  {
    Serial.print("Ooops, no BME280 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }
  else 
  {
    Serial.println("BME280 ready.");
  }
}


/************************* WiFi setup ****************************************/

void setup_wifi ()
{
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("...done");
  Serial.println();
}


void setup_mqtt ()
{
  mqSetRgb.setCallback (lRgbCblk);
  mqSetLed.setCallback (lLedCblk);  
  mqSetClima.setCallback (sClimaCblk);
  mqSetTemp.setCallback (sTempCblk);
  
  mqtt.subscribe (&mqSetRgb);
  mqtt.subscribe (&mqSetLed);
  mqtt.subscribe (&mqSetClima);
  mqtt.subscribe (&mqSetTemp);
}


void mqtt_reconnect() 
{
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 5;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 0.5 seconds...");
       mqtt.disconnect();
       delay(500);  // wait 1 second
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }

  Serial.println("MQTT Connected!");
}

/************************* MQTT callbacks ************************************/

void lRgbCblk (char *data, uint16_t len) 
{
  Serial.print ("Hey we're in a slider callback, the slider value is: ");
  Serial.println (data);
}

void lLedCblk (double level) 
{
  int val;
  Serial.print ("Hey we're in a onoff callback, the button value is: ");
  Serial.println (level);
  
  val = map (level, 0, 100, 1023, 0);
  analogWrite (ledPin, val);
//  if (state == 0)
//    digitalWrite (ledPin, LOW); 
//  else
//    digitalWrite (ledPin, HIGH); 
}

void sClimaCblk (double state)
{
  Serial.print ("Hey we're in a clima state callback, the button value is: ");
  Serial.println (state);
  setClimaMode = state;

  checkClima ();
}

void sTempCblk (double val)
{
  Serial.print ("Hey we're in a temperature set callback, the value is: ");
  Serial.println (val);  
  setClimaTemp = val;
  checkClima ();
}

void checkClima (void)
{
  if (setClimaMode == CLIMA_CTRL_OFF)
  {
    stateClima = CLIMA_STATUS_OFF;
  }
  else if (setClimaMode == CLIMA_CTRL_COOL)
  {
    stateClima = CLIMA_STATUS_COOL;
  }
  else if (setClimaMode == CLIMA_CTRL_HEAT)
  {
    stateClima = CLIMA_STATUS_HEAT;
  }
  else if (setClimaMode = CLIMA_CTRL_AUTO)
  {
    if (setClimaTemp < sensTemp)
    {
      stateClima = CLIMA_STATUS_COOL;
    }
    else if (setClimaTemp > sensTemp)
    {
      stateClima = CLIMA_STATUS_HEAT;
    }
    else
    {
      stateClima = CLIMA_STATUS_OK;
    }    
  }
}
  

