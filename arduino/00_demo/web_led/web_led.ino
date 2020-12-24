/*******************************************************************
    A project for controlling a 12V LED strip using an ESP8266
    Features:
    - LED strip can be toggled using the rotary encoder button
    - The LED strip brightness can be adjusted using a rotary encoder
    Main Hardware:
    - Wemos D1 Mini (or any ESP8266)
    - 12V LED strip
    - IRLB8721 Mosfet
    - Rotary Encoder
    Written by Brian Lough
 *******************************************************************/


// ----------------------------
// Additional Libraries - each one of these will need to be installed.
// ----------------------------
#include <ESP8266WiFi.h>
#include <time.h>
#include <Encoder.h>
// For interfacing with the rotary encoder
// Available on the library manager (Search for "Encoder")
// https://github.com/PaulStoffregen/Encoder


#define LED_B D2
#define LED_R D1
#define LED_G D8

// Rotary Encoder Pins
#define RE_SW D5
#define RE_DT D6
#define RE_CLK D7


Encoder myEnc(RE_DT, RE_CLK);

int pushButtonCoolDownDue = 0;
int brightness = 200;
boolean isLedsOn = false;

long oldPosition  = -999;

void setup() {
  Serial.begin(115200);
  Serial.println();

  WiFi.begin("Nokia7plus", "asdf5678");

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  configTime(2 * 3600, 0 * 3600, "pool.ntp.org", "time.nist.gov");
 
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
Serial.println("");
  
  pinMode(LED_B, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(RE_SW, INPUT_PULLUP);
}
void sunrize(int brightness_i){
    if ((brightness_i>0) && (brightness_i<1024)){
       analogWrite(LED_B, brightness_i);
       analogWrite(LED_R, brightness_i);
       analogWrite(LED_G, brightness_i);
    }
}
// the loop function runs over and over again forever
void loop() {

  int time1 = millis();
  if (time1 > pushButtonCoolDownDue) {
    if (digitalRead(RE_SW) == LOW) {
      if (isLedsOn) {
        isLedsOn = false;
        analogWrite(LED_B, 0);
        digitalWrite(LED_B, LOW);
        analogWrite(LED_B, 0);
        digitalWrite(LED_R, LOW);
        analogWrite(LED_B, 0);
        digitalWrite(LED_G, LOW);
      } else {
        isLedsOn = true;
        analogWrite(LED_B, brightness);
        analogWrite(LED_R, brightness);
        analogWrite(LED_G, brightness);
      }

      pushButtonCoolDownDue = time1 + 500;
    }
  }

  if (isLedsOn) {
    long newPosition = myEnc.read();
    if (newPosition > oldPosition) {
      brightness += 10;
      if (brightness > 1023)
      {
        brightness = 1023;
      }
    } else if (newPosition < oldPosition) {
      brightness -= 10;
      if (brightness < 10)
      {
        brightness = 10;
      }
    }

    oldPosition = newPosition;
    analogWrite(LED_B, brightness);
    analogWrite(LED_R, brightness);
    analogWrite(LED_G, brightness);
  }

//////////////////////////////////
  struct tm y2k = {0};
  double seconds;

  y2k.tm_hour = 17;   y2k.tm_min = 29; y2k.tm_sec = 0;
  y2k.tm_year = 2019 - 1900; y2k.tm_mon = 2 - 1; y2k.tm_mday = 5;

//////////////////////////////////

  time_t now = time(nullptr);
  //if (-1024 < difftime(now,mktime(&y2k))<=0)
  {
    //Serial.println(difftime(now,mktime(&y2k)));
    //Serial.println(ctime(&now));
    sunrize(-difftime(now,mktime(&y2k)));
  }
    


}
