


#include <Wire.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_NeoPixel.h>

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"


/* PINs */

#define PIN_LED       D4
#define PIN_BUTTON    D3


/************************* WiFi Access Point *********************************/
const char * ssid     = "embedded";
const char * password = "04523307";

/************************* MQTT Broker Setup *********************************/

#define MQTT_SERVER      "10.0.0.192"
#define MQTT_SERVERPORT  1883
#define MQTT_USERNAME    ""
#define MQTT_KEY         ""



/* Create an ESP8266 WiFiClient class to connect to the MQTT server. */

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_SERVERPORT);


/****************************** Feeds ***************************************/

Adafruit_MQTT_Subscribe mqttSetLed       = Adafruit_MQTT_Subscribe (&mqtt, "/set/led");
Adafruit_MQTT_Publish   mqttStatButton   = Adafruit_MQTT_Publish   (&mqtt, "/status/button");


int setLed = 0;
int statusButton = 0;



/* forward declarations */

void mqttReconnect ();
void setupWifi ();
void setupSensor ();
void setupLight ();


void mqttClbkSetLed (double val);



void setup() 
{
  Serial.begin (115200);
  delay (10);
  Serial.println ();
  Serial.println ("Initializing ...");
  Serial.println ();

  pinMode (PIN_LED, OUTPUT);
  pinMode (PIN_BUTTON, OUTPUT);

  setupWifi ();
  setupMqtt ();



  Serial.println ("...done");  
  Serial.println ();
}

void loop () 
{
  int sw;
  
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  mqttReconnect();

  // this is our 'wait for incoming subscription packets and callback em' busy subloop
  // try to spend your time here:
  mqtt.processPackets(1000);


//  // ping the server to keep the mqtt connection alive
//  // NOT required if you are publishing once every KEEPALIVE seconds
//  
//  if(! mqtt.ping()) {
//    mqtt.disconnect();
//  }

  sw = digitalRead (PIN_BUTTON);
  mqttStatButton.publish (sw);
}



void setupWifi ()
{
  Serial.print ("Connecting to ");
  Serial.println (ssid);

  /* 
   * Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
   * would try to act as both a client and an access-point and could cause
   * network-issues with your other WiFi-devices on your WiFi-network.
   */
   
  WiFi.mode (WIFI_STA);
  WiFi.begin (ssid, password);
  
  while (WiFi.status () != WL_CONNECTED) 
  {
    delay (500);
    Serial.print (".");
  }

  Serial.println ("");
  Serial.println ("WiFi connected");  
  Serial.println ("IP address: ");
  Serial.println (WiFi.localIP ());
  Serial.println ("...done");
  Serial.println ();
}


void setupMqtt ()
{
  mqttSetLed.setCallback   (mqttClbkSetLed);      
  mqtt.subscribe (&mqttSetLed);
}



void mqttReconnect() 
{
  int8_t ret;

  if (mqtt.connected ())  // Stop if already connected.
  {
    return;
  }

  Serial.print ("Connecting to MQTT... ");

  uint8_t retries = 5;
  while ((ret = mqtt.connect ()) != 0) // connect will return 0 for connected
  {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 0.5 seconds...");
    mqtt.disconnect();
    delay(500);  // wait 1 second
    retries--;
    if (retries == 0)
    {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }

  Serial.println ("MQTT Connected!");
}




void mqttClbkSetLed (double level) 
{
  int val;
  Serial.print ("Hey we're in a onoff callback, the button value is: ");
  Serial.println (level);
  
  if (level == 0)
    digitalWrite (PIN_LED, LOW); 
  else
    digitalWrite (PIN_LED, HIGH); 
}




