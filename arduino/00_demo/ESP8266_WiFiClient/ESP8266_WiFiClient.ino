#include <ESP8266WiFi.h>

/************************* WiFi Access Point *********************************/
const char * ssid     = "embedded";
const char * password = "04523307";

/* Create an ESP8266 WiFiClient class */

WiFiClient client;

void setup () {
  Serial.begin(115200);
  delay(10);
  Serial.println ();
  Serial.println ("Initializing ...");
  Serial.println ();

  /* WiFi connect */

  Serial.print ("Connecting to ");
  Serial.println (ssid);
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

  long rssi = WiFi.RSSI();
  Serial.print("RSSI:");
  Serial.println(rssi);
    
  Serial.println ("...done");
  Serial.println ();  
}

void loop() {
  // put your main code here, to run repeatedly:

}
