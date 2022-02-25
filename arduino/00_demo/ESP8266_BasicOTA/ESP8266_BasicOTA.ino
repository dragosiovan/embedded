#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#if 1
#define OUT_HIGH  LOW
#define OUT_LOW   HIGH
#else
#define OUT_HIGH  HIGH
#define OUT_LOW   LOW
#endif

#define OTA_HOST   "OTA_LEDS_7"

#include "c:/work/settings.h"
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

int led = 2;


void setup() {

  pinMode(led, OUTPUT);
  
  Serial.begin(115200);
  Serial.println("Booting..2");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  ArduinoOTA.setPort(8080);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(OTA_HOST);

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }  
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);

    int i;
    int j;
    for (i=0; i<3; i++)
    {    
      for (j=0; j<1024/8; j++)
      {
        analogWrite(led, j*8);
        delay(2);
      }

      for (j=1023/8; j>0; j--)
      {
        analogWrite(led, j*8);
        delay(2);  
      }

//      digitalWrite(led, OUT_HIGH);
//      delay(100);
//      digitalWrite(led, OUT_LOW);
//      delay(100);    
    } 
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
    
    int i;
    int j;
    for (i=0; i<5; i++)
    {    
      for (j=0; j<1024/8; j++)
      {
        analogWrite(led, j*8);
        delay(1);  
      }

      for (j=1023/8; j>0; j--)
      {
        analogWrite(led, j*8);
        delay(1);  
      }

//      digitalWrite(led, OUT_HIGH);
//      delay (100);
//      digitalWrite(led, OUT_LOW);
//      delay(100);      
    } 
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    int val;
    val = map((progress / (total / 100)), 0, 100, 0, 1023);
    analogWrite(led, val);
    Serial.printf("Progress: %u%%\n", (progress / (total / 100)));
//    Serial.println();
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}




int ledState = LOW;

unsigned long previousMillis = 0;
const long interval = 1000;


void loop() {
  ArduinoOTA.handle();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;  // Note that this switches the LED *off*
    } else {
      ledState = LOW;  // Note that this switches the LED *on*
    }
    digitalWrite(led, ledState);
  }
  
}
