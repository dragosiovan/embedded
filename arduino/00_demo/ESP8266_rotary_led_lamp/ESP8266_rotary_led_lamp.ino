

#include "C:\work\settings.h"


#include <ESP8266WiFi.h>
#include <Encoder.h>



// Rotary Encoder Pins
#define RE_SW D5
#define RE_DT D6
#define RE_CLK D7

#define MQTT_SERVER      "10.0.0.192"
#define MQTT_SERVERPORT  1883
#define MQTT_USERNAME    ""
#define MQTT_KEY         ""


typedef enum 
{
  WIFI_ST_NA = 0,
  WIFI_ST_INIT = 1,
  WIFI_ST_CONNECTING = 1,
  WIFI_ST_CONNECTED = 3,
  WIFI_ST_DISCONNECTED = 4,
} WifiState;

/************************* WiFi Access Point *********************************/
const char * ssid     = WIFI_SSID;
const char * password = WIFI_PASS;

WifiState wifiState = WIFI_ST_NA;

Encoder myEnc(RE_DT, RE_CLK);






void setup() 
{
  Serial.begin(115200);
  Serial.println("Basic Encoder Test:");

  setupWifi ();
}

long oldPosition  = -999;


void loop() 
{
  checkCon();
  
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) 
  {
    oldPosition = newPosition;
    Serial.println(newPosition);
  }
  loopShowState ();
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

    /*
     * Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     * would try to act as both a client and an access-point and could cause
     * network-issues with your other WiFi-devices on your WiFi-network.
     */

    WiFi.mode (WIFI_STA);
    WiFi.begin (ssid, password);
    wifiState = WIFI_ST_CONNECTING;

    if (WiFi.status () == WL_CONNECTED)
    {
      wifiSetState(WIFI_ST_CONNECTED);
    }

    Serial.println ("...done");
    Serial.println ();
}

/******************************************************************************
*
* setupWifi - configure WIFI client connectivity
*
* TODO
*
* RETURNS: N/A
*/

void checkCon (void)
{
  int wifiSt;

  wifiSt = WiFi.status ();
  if ((wifiSt == WL_CONNECTED) && (wifiState != WIFI_ST_CONNECTED))    /* got connected */
  {
    wifiSetState(WIFI_ST_CONNECTED);
  }
  else if ((wifiSt == WL_CONNECTED) && (wifiState != WIFI_ST_CONNECTED)) /* connection lost */
  {
    wifiSetState(WIFI_ST_DISCONNECTED);
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

void wifiSetState (WifiState state)
{
  wifiState = WIFI_ST_CONNECTED;

  if (wifiState == WIFI_ST_CONNECTED)
  {
    Serial.println ("WiFi connected");
    Serial.println ("IP address: ");
    Serial.println (WiFi.localIP ());
  }
  else if (wifiState == WIFI_ST_DISCONNECTED)
  {
    Serial.println ("WiFi disconnected");
  }
}

void loopShowState (void)
{
  
}
