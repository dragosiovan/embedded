/*
 * Wall mounted LED with rotary and push button for switching ON/OFF and dimming
 * It switches automatically when you what to use the stairs, from each of the
 * directions.
 *
 * Current implementation uses:
 * 1 5V power supply
 * 1 LED
 *  2 detection sensors:
 * - 1 PIR for downstairs detection
 * - 1 ultrasonic upstairs for distance measurement
 *
 * INPUTS:
 * - supply - 12V
 * - sensor 1 (PIR)
 * - sensor 2 (ultrasonic)
 *
 * OUTPUTS:
 * - LED
 * - LED sensor 1
 * - LED sensor 2
 *
 * +12V   ------------->
 * GND    ------------->
 *
 * +5v    <------------
 * OUT    ------------>
 * GND    <------------
 *
 * LED1   <------------
 * GND    <------------
 *
 *
 * +5V    <------------
 * TRIG   <------------
 * ECHO   ------------>
 * GND    <------------
 *
 * LED2   <------------
 * GND    <------------
 *
 *
 * LED    <------------
 *        <------------
 *        <------------
 * GND    <------------
 *        <------------
 *        <------------
 *
 */





#include "dev_config.h"
#include "c:\work\settings.h"
#include <Encoder.h>

#define USE_OTA  1

#if USE_OTA
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#endif


//#define DBG_F_PLOT_OUT_LED    B00000001
//#define DBG_F_PLOT_OUT_EYE    B00000010
//#define DBG_F_PLOT_IN_DIST    B00000100
//
//#define DBG_PLOT_OUT_LED      (dbgPlot & DBG_F_PLOT_OUT_LED)
//#define DBG_PLOT_OUT_EYE      (dbgPlot & DBG_F_PLOT_OUT_EYE)
//#define DBG_PLOT_IN_DIST      (dbgPlot & DBG_F_PLOT_IN_DIST)

uint8_t dbgPlot = 0
//| DBG_F_PLOT_OUT_LED
//| DBG_F_PLOT_OUT_EYE
//| DBG_F_PLOT_IN_DIST
| 0;


#define DBG_F_INIT            B00000001
//#define DBG_F_STATE           B00000010
//#define DBG_F_STATE_EXT       B00000100
//#define DBG_F_OUT_LED         B00001000
//#define DBG_F_IN_MOT          B00010000
//#define DBG_F_IN_US           B00100000 // ultra sonic
//#define DBG_F_STATE_EYE       B01000000
//#define DBG_F_STATE_EXT_EYE   B10000000


#define DBG_NOT_PLOT      (dbgPlot == 0) // plot disabled

#define DBG_INIT          ((DBG_NOT_PLOT) && (dbgVal & DBG_F_INIT))
//#define DBG_STATE         ((DBG_NOT_PLOT) && (dbgVal & DBG_F_STATE))
//#define DBG_STATE_EXT     ((DBG_NOT_PLOT) && (dbgVal & DBG_F_STATE_EXT))
//#define DBG_STATE_EYE     ((DBG_NOT_PLOT) && (dbgVal & DBG_F_STATE_EYE))
//#define DBG_STATE_EXT_EYE ((DBG_NOT_PLOT) && (dbgVal & DBG_F_STATE_EXT_EYE))
//#define DBG_OUT_LED       ((DBG_NOT_PLOT) && (dbgVal & DBG_F_OUT_LED))
//#define DBG_IN_MOT        (dbgVal & DBG_F_IN_MOT)
//#define DBG_IN_US         (dbgVal & DBG_F_IN_US)

uint8_t dbgVal = 0
| DBG_F_INIT
//| DBG_F_STATE
//| DBG_F_STATE_EXT
//| DBG_F_OUT_LED
//| DBG_F_STATE_EYE
//| DBG_F_STATE_EXT_EYE
//| DBG_F_IN_MOT
//| DBG_F_IN_US
| 0;


// PINs definitions
#define PIN_RE_SW           D5    // rotary encoder switch
#define PIN_RE_DT           D6    // rotary encoder DT
#define PIN_RE_CLK          D7    // rotary encoder CLK
#define PIN_OUT_LED         D8    // LED output


#define LED_PWM_MAX         1023      // PWM 100%
//#define LED_PWM_MAX         255         // PWM 100%
#define PER_MAX             1000

//#define LED_PER_MAX_100     100 // %
//#define LED_PER_IDLE_100    10  // %

#define LED_PER_MAX         (LED_PER_MAX_100*(PER_MAX/100))          // /PER %
#define LED_PER_IDLE        (LED_PER_IDLE_100*(PER_MAX/100))         // /PER %


#if OUTPUT_NEG
#define OUT_HIGH  LOW
#define OUT_LOW   HIGH
#else
#define OUT_HIGH  HIGH
#define OUT_LOW   LOW
#endif


#define MAIN_CYCLE          10        // (ms) check cycle


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



Encoder myEnc(PIN_RE_DT, PIN_RE_CLK);
long oldPosition  = -999;


/******************************************************************************/
/********************************** SETUP *************************************/

void setup() {
  Serial.begin(115200);

//  if (DBG_INIT)
  {
    Serial.print("Initializing Wall LED ");
    Serial.println(OTA_HOST);
  }

  setupLED();

  setupWifi ();


}



void loop() {
  checkCon();

  long newPosition = myEnc.read();
  if (newPosition != oldPosition)
  {
    oldPosition = newPosition;
    Serial.println(newPosition);
  }
  loopShowState ();

}


/******************************************************************************/
/********************************** SETUP *************************************/

/******************************************************************************/
void setupLED(void)
{
  analogWriteFreq(200);

  pinMode(PIN_OUT_LED, OUTPUT);
  digitalWrite(PIN_OUT_LED, OUT_LOW);
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
