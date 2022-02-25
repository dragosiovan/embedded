#include "C:\users\ext_drio\desktop\tmp\settings.h"


#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>


#ifndef STASSID
#define STASSID WIFI_SSID
#define STAPSK  WIFI_PASS
#endif

const char* ssid = "DaD";
const char* password = "rosirosi";
const char* host = "OTA-LEDS_4";

int led_pin = LED_BUILTIN;
#define N_DIMMERS 1
int dimmer_pin[] = {14, 5, 15};

void setup() {
  Serial.begin(115200);
  Serial.println("[Initializing]");

  /* switch on led */
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);

  Serial.println("Booting");
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    Serial.println("Retrying connection...");
  }
  /* switch off led */
  digitalWrite(led_pin, HIGH);

  /* configure dimmers, and OTA server events */
  analogWriteRange(1000);
  analogWrite(led_pin, 990);

  for (int i = 0; i < N_DIMMERS; i++) {
    pinMode(dimmer_pin[i], OUTPUT);
    analogWrite(dimmer_pin[i], 50);
  }

  ArduinoOTA.setPort(8080);
  ArduinoOTA.setHostname(host);
  ArduinoOTA.onStart([]() { // switch off all the PWMs during upgrade
    Serial.println("OTA start");
    for (int i = 0; i < N_DIMMERS; i++) {
      //analogWrite(dimmer_pin[i], 0);
    }
    analogWrite(led_pin, 0);
  });

  ArduinoOTA.onEnd([]() { // do a fancy thing with our board led at end
    Serial.println("OTA end");
    for (int i = 0; i < 30; i++) {
      //analogWrite(led_pin, (i * 100) % 1001);
      //delay(50);
    }
    analogWrite(led_pin, 0);
  });

  ArduinoOTA.onError([](ota_error_t error) {
    (void)error;
    Serial.print("OTA error:"); Serial.println(error);
    ESP.restart();
  });

  /* setup the OTA server */
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address -- : ");
  Serial.println(WiFi.localIP());


}

unsigned long ms = 0;
uint8_t cnt = 0;

void loop() {
  ArduinoOTA.handle();

  if (millis() - ms > 500)
  {
    ms = millis();
    Serial.print(".");//Serial.print(ms);
    cnt++;
    if (cnt == 80)
    {
      cnt = 0;
      Serial.println();
    }   
  }
}
