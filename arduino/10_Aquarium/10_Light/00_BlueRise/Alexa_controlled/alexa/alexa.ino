#include <Arduino.h>
#ifdef ESP32
    #include <WiFi.h>
#else
    #include <ESP8266WiFi.h>
#endif
#include "fauxmoESP.h"


#include <esp32-hal-ledc.h>





#define USE_LEDC  0

#define WIFI_SSID "DaD"
#define WIFI_PASS "rosirosi"


fauxmoESP fauxmo;

// -----------------------------------------------------------------------------

#define SERIAL_BAUDRATE     115200
#define LED_STATUS          2 //LED_BUILTIN                  //status LED
#define STATUS_PERIOD       1000 // (ms)



#define PIN_CHANNEL_UV    16  // Reef UV
#define PIN_CHANNEL_VI    17  // Reef Violet
#define PIN_CHANNEL_BL    18  // Reef Blue Low
#define PIN_CHANNEL_BH    19  // Reef Blue High
#define PIN_CHANNEL_IB    23  // Reef Ice Blue
#define PIN_CHANNEL_CW    25  // Reef Cold White
#define PIN_CHANNEL_RW    26  // Refugium White
#define PIN_CHANNEL_RB    27  // Refugium Red Blue


#define LIGHT_CHANNEL_UV  "Aqua UV"           // Reef UV
#define LIGHT_CHANNEL_VI  "Aqua VI"       // Reef Violet
#define LIGHT_CHANNEL_BL  "Aqua BL"     // Reef Blue Low
#define LIGHT_CHANNEL_BH  "Aqua BH"    // Reef Blue High
#define LIGHT_CHANNEL_IB  "Aqua IB"     // Reef Ice Blue
#define LIGHT_CHANNEL_CW  "Aqua CW"   // Reef Cold White
#define LIGHT_CHANNEL_RW  "Aqua WW"    // Refugium White
#define LIGHT_CHANNEL_RB  "Aqua RB" // Refugium Red Blue


const uint32_t freq = 5000;
const uint8_t resolution = 10;
const uint16_t dutyThr = 30; 


const int statusLed = LED_STATUS;

#define ACTIVITY_TIME   5
#define ACTIVITY_PERIOD 30
unsigned int activity = 0;

#define MAX_NO_OF_DEVICES 8

typedef enum
{
  LED_CH_UV = 0,    // Reef UV
  LED_CH_VI = 1,    // Reef Violet
  LED_CH_BL = 2,    // Reef Blue Low
  LED_CH_BH = 3,    // Reef Blue High
  LED_CH_IB = 4,    // Reef Ice Blue
  LED_CH_CW = 5,    // Reef Cold White
  LED_CH_RW = 6,    // Refugium White
  LED_CH_RB = 7,    // Refugium Red Blue
  LED_CH_MAX = 8
};

typedef struct
{
  uint8_t pin;
  char *  chanName;
} ChannelMap_t;


ChannelMap_t chanMap[LED_CH_MAX] =
{
  { // LED_CH_UV - ultra violet
    .pin = PIN_CHANNEL_UV, 
    .chanName = LIGHT_CHANNEL_UV
  },
  { // LED_CH_VI - violet
    .pin = PIN_CHANNEL_VI, 
    .chanName = LIGHT_CHANNEL_VI
  },
  { // LED_CH_BL - blue low
    .pin = PIN_CHANNEL_BL, 
    .chanName = LIGHT_CHANNEL_BL
  },
  { // LED_CH_BH - blue high
    .pin = PIN_CHANNEL_BH, 
    .chanName = LIGHT_CHANNEL_BH
  },
  { // LED_CH_IB - ice blue
    .pin = PIN_CHANNEL_IB, 
    .chanName = LIGHT_CHANNEL_IB
  },
  { // LED_CH_CW - cold white
    .pin = PIN_CHANNEL_CW, 
    .chanName = LIGHT_CHANNEL_CW
  },
  { // LED_CH_RW - recife white
    .pin = PIN_CHANNEL_RW, 
    .chanName = LIGHT_CHANNEL_RW
  },
  { // LED_CH_RB - recife red blue
    .pin = PIN_CHANNEL_RB, 
    .chanName = LIGHT_CHANNEL_RB
  }
};


/******************************************************************************
* setup - main setup
* - initialize UART
* - initialize WIFI
* - initialize light channels
* - setup Alexa
* RETURNS: N/A
******************************************************************************/
void setup() 
{

    // Init serial port and clean garbage
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();
    Serial.println();

    // LEDs
    setupLight();

    // Wifi
    setupWifi();

    setupAlexa();
}


/******************************************************************************
* loop - main loop
* - initialize UART
* - initialize WIFI
* - initialize light channels
* RETURNS: N/A
******************************************************************************/
void loop() 
{
    // This is a sample code to output free heap every 5 seconds
    // This is a cheap way to detect memory leaks
    // static unsigned long last_heap = millis();
    static unsigned long last_status = millis();
    static unsigned char statusLed = 0;
    unsigned int period;

    // fauxmoESP uses an async TCP server but a sync UDP server
    // Therefore, we have to manually poll for UDP packets
    fauxmo.handle();

    // if (millis() - last_heap > 5000) 
    // {
    //     last_heap = millis();
    //     // Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
    // }

    if (activity > 0)
    {
      period = ACTIVITY_PERIOD;
    }
    else
    {
      period = STATUS_PERIOD;
    }

    if (millis() - last_status > period) 
    {
      last_status = millis();

      if (activity > 0)
      {
        activity--;
      }
      digitalWrite(LED_STATUS, statusLed&1);
      statusLed++;
      // Serial.printf("[MAIN] STAUS: %d:%d\n", statusLed, statusLed&1);
    }
}


/******************************************************************************
* setupWifi - setup WIFI connection
* RETURNS: N/A
******************************************************************************/
void setupWifi() 
{
  // Set WIFI module to STA mode
  WiFi.mode(WIFI_STA);
  // Connect
  Serial.printf("[WIFI] Connecting to [%s] ", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  // Wait
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(100);    
  }
  Serial.println();
  // Connected!
  Serial.printf("[WIFI] STATION Mode, SSID: [%s], IP address: [%s]\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}


/******************************************************************************
* setupLight - setup Light channels
* RETURNS: N/A
******************************************************************************/
void setupLight()
{
  Serial.printf("Initializing light channels...\n");

  pinMode(LED_STATUS, OUTPUT);
  digitalWrite(LED_STATUS, HIGH);

    // ledcAttach(LED_STATUS, freq, resolution);
    // ledcWrite(LED_STATUS, 0);

  for (uint8_t devid = 0; devid < LED_CH_MAX; devid++)
  {
    // Serial.printf("... light channel [%s] on pin [%d]=OUTPUT,LOW\n", chanMap[devid].chanName, chanMap[devid].pin);
  

#if USE_LEDC
    ledcAttach(chanMap[devid].pin, freq, resolution);
    ledcWrite(chanMap[devid].pin, 0);
    // if you want to attach a specific channel, use the following instead
    // ledcAttachChannel(chanMap[devid].pin, freq, resolution, 0);
#else
    pinMode(chanMap[devid].pin, OUTPUT);
    digitalWrite(chanMap[devid].pin, LOW);
    analogWriteResolution(chanMap[devid].pin, 10);
#endif
  }
}


/******************************************************************************
* setupAlexa - setup Alexa devices
* - add each light channel as a separat device
* - define handling routine
* RETURNS: N/A
******************************************************************************/
void setupAlexa()
{
  Serial.printf("Setting up Alexa devices ...\n");

  // By default, fauxmoESP creates it's own webserver on the defined port
  // The TCP port must be 80 for gen3 devices (default is 1901)
  // This has to be done before the call to enable()
  fauxmo.createServer(true); // not needed, this is the default value
  fauxmo.setPort(80); // This is required for gen3 devices

  // You have to call enable(true) once you have a WiFi connection
  // You can enable or disable the library at any moment
  // Disabling it will prevent the devices from being discovered and switched
  fauxmo.enable(true);

  // You can use different ways to invoke alexa to modify the devices state:
  // "Alexa, turn yellow lamp on"
  // "Alexa, turn on yellow lamp
  // "Alexa, set yellow lamp to fifty" (50 means 50% of brightness, note, this example does not use this functionality)

  // Add virtual devices    
  for (uint8_t devid = 0; devid < LED_CH_MAX; devid++)
  {
    Serial.printf("... add device id [%d] name [%s]\n", devid, chanMap[devid].chanName);
    fauxmo.addDevice(chanMap[devid].chanName);
  }

  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) 
  {
    uint32_t duty; 

    // Callback when a command from Alexa is received. 
    // You can use device_id or device_name to choose the element to perform an action onto (relay, LED,...)
    // State is a boolean (ON/OFF) and value a number from 0 to 255 (if you say "set kitchen light to 50%" you will receive a 128 here).
    // Just remember not to delay too much here, this is a callback, exit as soon as possible.
    // If you have to do something more involved here set a flag and process it in your main loop.
    
    // Serial.printf("[ALEXA] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);

    activity = ACTIVITY_TIME;

    for (uint8_t devid = 0; devid < MAX_NO_OF_DEVICES; devid++)
    {
      // Checking for device_id is simpler if you are certain about the order they are loaded and it does not change.
      // Otherwise comparing the device_name is safer.

      if (strcmp(chanMap[devid].chanName, device_name) == 0)
      {
        if (state != 0)
        {
          // 0..dutyThr (out of range 0..255) the value is set as it is for high resolution at high intesity
          // dutyThr..255 is mapped to the higher resolution of 0..1023
          // if (value < dutyThr)
          //   // duty = value;
          //   duty = map(value, 4, dutyThr, 1, dutyThr);
          // else
          //   duty = map(value, dutyThr, 255, dutyThr, 255);

          duty = value;
        }
        else
        {
          duty = 0;
        }

        // fauxmo.setState(chanMap[devid].chanName, state, retval);

#if USE_LEDC
          ledcWrite(chanMap[devid].pin, duty);
          // ledcWrite(LED_STATUS, duty);
#else
          analogWrite(chanMap[devid].pin, duty);
          // analogWrite(LED_STATUS, duty);
#endif

      } 
    }

    // If your device state is changed by any other means (MQTT, physical button,...)
    // you can instruct the library to report the new state to Alexa on next request:
    // fauxmo.setState(ID_YELLOW, true, 255);
  });
}