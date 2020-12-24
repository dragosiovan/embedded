#include <Wire.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_NeoPixel.h>

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

typedef enum 
{
  CLIMA_STATUS_OFF  = 0,
  CLIMA_STATUS_OK   = 1,
  CLIMA_STATUS_COOL = 2,
  CLIMA_STATUS_HEAT = 3
};

char * climaStatusStr [4] =
{
  "OFF",
  "OK",
  "COOL",
  "HEAT"
};

uint8_t climaOut[4][2] =
{
  {LOW,   LOW},
  {LOW,   LOW},
  {HIGH,  LOW},
  {LOW,   HIGH}
};

typedef enum 
{
  CLIMA_CTRL_OFF    = 0,
  CLIMA_CTRL_AUTO   = 1,
  CLIMA_CTRL_COOL   = 2,
  CLIMA_CTRL_HEAT   = 3
};

char * climaStateStr [4] =
{
  "OFF",
  "AUTO",
  "COOL",
  "HEAT"
};

/* PINs */

#define PIN_LED       D0
#define PIN_BME_SCL   D1
#define PIN_BME_SDA   D2
#define PIN_CLIMA_0   D3
#define PIN_CLIMA_1   D4
#define PIN_DOOR_MOT  D5
#define PIN_RGB       D6
#define PIN_DOOR_SW   D7


/************************* WiFi Access Point *********************************/
const char * ssid     = "embedded";
const char * password = "04523307";

/************************* MQTT Broker Setup *********************************/

#define MQTT_SERVER      "10.0.0.192"
#define MQTT_SERVERPORT  1883
#define MQTT_USERNAME    ""
#define MQTT_KEY         ""

/* RGB strip */

#define LED_COUNT     20  // number of LEDs on the strip
#define LED_DEF_COL   0x010101

Adafruit_NeoPixel neo = Adafruit_NeoPixel(LED_COUNT, PIN_RGB, NEO_GRB + NEO_KHZ800);

#define DOOR_CLOSE_POS  10
#define DOOR_OPEN_POS   80
#define DOOR_SPEED      10

int doorReq = 0;
int doorSw = LOW;
Servo doorServo;
int statusDoor = -1;



/* Create an ESP8266 WiFiClient class to connect to the MQTT server. */

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_SERVERPORT);


/****************************** Feeds ***************************************/

Adafruit_MQTT_Subscribe mqttSetRgb       = Adafruit_MQTT_Subscribe (&mqtt, "/set/rgb", MQTT_QOS_1);
Adafruit_MQTT_Subscribe mqttSetLed       = Adafruit_MQTT_Subscribe (&mqtt, "/set/light");
Adafruit_MQTT_Subscribe mqttSetClima     = Adafruit_MQTT_Subscribe (&mqtt, "/set/clima");
Adafruit_MQTT_Subscribe mqttSetTemp      = Adafruit_MQTT_Subscribe (&mqtt, "/set/temp");
Adafruit_MQTT_Subscribe mqttSetDoor      = Adafruit_MQTT_Subscribe (&mqtt, "/set/door");
Adafruit_MQTT_Publish   mqttStatDoor     = Adafruit_MQTT_Publish   (&mqtt, "/status/door");
Adafruit_MQTT_Publish   mqttStatClima    = Adafruit_MQTT_Publish   (&mqtt, "/status/clima");
Adafruit_MQTT_Publish   mqttTemp         = Adafruit_MQTT_Publish   (&mqtt, "/sens/temp");
Adafruit_MQTT_Publish   mqttHumi         = Adafruit_MQTT_Publish   (&mqtt, "/sens/humi");
Adafruit_MQTT_Publish   mqttPres         = Adafruit_MQTT_Publish   (&mqtt, "/sens/pres");




int ledLevel = 0;

float sensTemp;
float sensHumi;
float sensPres;
float climaDelta = 1;
int setClimaMode = CLIMA_CTRL_OFF;
int stateClima = CLIMA_STATUS_OFF;
int setClimaTemp = 0;


#define BME_ADR 0x76
Adafruit_BME280 bme;


/* forward declarations */

void mqttReconnect ();
void setupWifi ();
void setupSensor ();
void setupLight ();

void readSensor ();
void mqttClbkSetRgb (char *data, uint16_t len);
void mqttClbkSetLed (double val);
void mqttClbkSetClima (double state);
void mqttClbkSetTemp (double val);
void mqttClbkSetDoor (double state);

void checkClima (void);



/******************************************************************************
*
* setup - setup all that is needed (called by env)
*
* TODO
*
* RETURNS: N/A
*/

void setup() 
{
  Serial.begin(115200);
  delay(10);
  Serial.println ();
  Serial.println ("Initializing ...");
  Serial.println ();

  setupWifi ();
  setupSensor ();
  setupMqtt ();
  setupLight ();

  pinMode (PIN_LED, OUTPUT);
  pinMode (PIN_CLIMA_0, OUTPUT);
  pinMode (PIN_CLIMA_1, OUTPUT);


  pinMode(PIN_DOOR_SW, INPUT);
  doorServo.attach(PIN_DOOR_MOT);
  doorReq = 0;
  setDoor ();

  Serial.println ("...done");  
  Serial.println ();
}

/******************************************************************************
*
* loop - cyclic main function (called by env)
*
* TODO
*
* RETURNS: N/A
*/

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

  readSensors ();

  sw = digitalRead (PIN_DOOR_SW);
  if ((sw == HIGH) && (doorSw == LOW))
  {
    Serial.println ("Switch button!!!");
    doorReq = !statusDoor; // switch door status
    setDoor ();
  }
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
* setupWifi - configure WIFI client connectivity
*
* TODO
*
* RETURNS: N/A
*/

void setupWifi ()
{
  Serial.print ("Connecting to ");
  Serial.println (ssid);
//  WiFi.mode (WIFI_STA);
//  WiFi.begin (ssid, password);

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


/******************************************************************************
*
* setupMqtt - configure MQTT client
*
* TODO
*
* RETURNS: N/A
*/

void setupMqtt ()
{
  mqttSetRgb.setCallback   (mqttClbkSetRgb);
  mqttSetLed.setCallback   (mqttClbkSetLed);  
  mqttSetClima.setCallback (mqttClbkSetClima);
  mqttSetTemp.setCallback  (mqttClbkSetTemp);
  mqttSetDoor.setCallback  (mqttClbkSetDoor);
  
  
  mqtt.subscribe (&mqttSetRgb);
  mqtt.subscribe (&mqttSetLed);
  mqtt.subscribe (&mqttSetClima);
  mqtt.subscribe (&mqttSetTemp);
  mqtt.subscribe (&mqttSetDoor);
}


/******************************************************************************
*
* setupLight - configure Light
*
* TODO
*
* RETURNS: N/A
*/

void setupLight ()
{
  int pos;
  
  neo.begin();
  neo.setBrightness(255);

  for (pos = 0; pos < LED_COUNT; pos++)
  {
    neo.setPixelColor (pos, LED_DEF_COL);
  }
  neo.show();
}

/******************************************************************************
*
* mqttReconnect - handle MQTT reconnection
*
* TODO
*
* RETURNS: N/A
*/

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
  setDoor ();
  Serial.println ("MQTT Connected!");
}


/******************************************************************************
*
* mqttClbkSetRgb - MQTT callback for subscribed /set/rgb
*
* TODO
*
* RETURNS: N/A
*/

void mqttClbkSetRgb (char *data, uint16_t len) 
{
  int pos;
  uint8_t r;
  uint8_t g;
  uint8_t b;
  String msg = String ((char *) data);
  
//  Serial.print ("Hey we're in a slider callback, the slider value is: ");
  Serial.println (data);
  Serial.println (msg);
  r = (msg.substring(0,msg.indexOf(','))).toInt();
  g = msg.substring(msg.indexOf(',')+1,msg.lastIndexOf(',')).toInt();
  b = msg.substring(msg.lastIndexOf(',')+1).toInt();
  
  for (pos = 0; pos < LED_COUNT; pos++)
  {
    neo.setPixelColor (pos, r, g, b);
    //break;
  }
  neo.show();
  
  Serial.print("R: ");
  Serial.println(r);
  Serial.print("G: ");
  Serial.println(g);
  Serial.print("B: ");
  Serial.println(b);
}


/******************************************************************************
*
* mqttClbkSetLed - MQTT callback for subscribed /set/led
*
* TODO
*
* RETURNS: N/A
*/

void mqttClbkSetLed (double level) 
{
  int val;
  Serial.print ("Hey we're in a onoff callback, the button value is: ");
  Serial.println (level);
  
  val = map (level, 0, 100, 1023, 0);
  analogWrite (PIN_LED, val);
//  if (state == 0)
//    digitalWrite (ledPin, LOW); 
//  else
//    digitalWrite (ledPin, HIGH); 
}

/******************************************************************************
*
* sClbkSetClima - MQTT callback for subscribed /set/clima
*
* TODO
*
* RETURNS: N/A
*/

void mqttClbkSetClima (double state)
{
  Serial.print ("Hey we're in a clima state callback, the button value is: ");
  Serial.println (state);
  setClimaMode = state;

  checkClima ();
}

/******************************************************************************
*
* mqClbkSetTemp - MQTT callback for subscribed /set/temp
*
* TODO
*
* RETURNS: N/A
*/

void mqttClbkSetTemp (double val)
{
  Serial.print ("Hey we're in a temperature set callback, the value is: ");
  Serial.println (val);  
  setClimaTemp = val;
  checkClima ();
}

/******************************************************************************
*
* mqttClbkSetDoor - MQTT callback for subscribed /set/door
*
* TODO
*
* RETURNS: N/A
*/

void mqttClbkSetDoor (double state)
{
  int pos;
  
  Serial.print ("Hey we're in a door set callback, the value is: ");
  Serial.println (state);  
  Serial.print ("Current door");
  Serial.println (statusDoor);

//  statusDoor = !statusDoor; // switch door status
  if (statusDoor != state)
  {
    doorReq = state;
    setDoor ();
  }
}

/******************************************************************************
*
* setDoor - set door status
*
* TODO
*
* RETURNS: N/A
*/

void setDoor (void)
{
  int pos;
  
  if (doorReq != statusDoor)
  {
    Serial.print ("Door status changed ");
    Serial.println (doorReq);
    
    if (doorReq == 1)   // close -> open
    {
      Serial.println ("Open the door");
      for (pos = DOOR_CLOSE_POS; pos < DOOR_OPEN_POS; pos++)
      {
        doorServo.write(pos);
        delay(DOOR_SPEED);
      }
    }
    else                // open -> close
    {
      Serial.println ("Close the door");
      for (pos = DOOR_OPEN_POS; pos > DOOR_CLOSE_POS; pos--) 
      {
        doorServo.write(pos);              // tell servo to go to position in variable 'pos'
        delay(DOOR_SPEED);                 // waits 15ms for the servo to reach the position
      }    
    }
    statusDoor = doorReq;
  }
  mqttStatDoor.publish (statusDoor);
}


/******************************************************************************
*
* checkClima - update clima state
*
* Depending on requested clima mode and requested temperature, update the 
* actuators status (cooling/heating).
*
* RETURNS: N/A
*/

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
  digitalWrite (PIN_CLIMA_0, climaOut[stateClima][0]);
  digitalWrite (PIN_CLIMA_1, climaOut[stateClima][1]);

  Serial.print ("Clima ctrl pins: ");
  Serial.print (climaOut[stateClima][0]);
  Serial.print ("-");
  Serial.println (climaOut[stateClima][1]);
  
  Serial.print ("Publish state clima: ");
  Serial.println (climaStatusStr[stateClima]);  
  mqttStatClima.publish (stateClima);
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
  if (!mqttTemp.publish (sensTemp)) 
  {
    Serial.println ("Publish Temperature: Failed");
  } 
  checkClima ();

  sensPres = bme.readPressure () * 0.7500637554192 / 100.0F;
  Serial.print ("Pressure:    ");
  Serial.print (sensPres);
  Serial.println (" mmHg");
  if (!mqttPres.publish (sensPres))
  {
    Serial.println ("Publish Pressure: Failed");
  } 
  
  sensHumi = bme.readHumidity ();
  Serial.print ("Humidity:    ");
  Serial.print (sensHumi);
  Serial.println (" %");
  if (!mqttHumi.publish (sensHumi)) 
  {
    Serial.println("Publish Humidity: Failed");
  } 
}

