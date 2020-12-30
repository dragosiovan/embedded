

/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */
#include "C:\work\settings.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WIFI
const char * ssid     = WIFI_SSID;
const char * password = WIFI_PASS;
String hostName = "10.0.0.192";

// MQTT
//const char * mqttServer = "10.0.0.192";
IPAddress mqttServer(10,0,0,193);
const int    mqttPort = 1833;
//const char * mqttUser = "";
//const char * mqttPassword = "";
char server[] = "10.0.0.192";    // name address for Google (using DNS)






void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  Serial.println();
  Serial.println("-----------------------");
 
}


WiFiClient espClient;

PubSubClient client(espClient);
//PubSubClient client(mqttServer, mqttPort, callback, espClient);





void setup() 
{

  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.println();

/* WIFI START */
  
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


///////////


  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (espClient.connect(server, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    espClient.println("GET /search?q=arduino HTTP/1.1");
    espClient.println("Host: www.google.com");
    espClient.println("Connection: close");
    espClient.println();
  }




/////////
  
  
   
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    //if (client.connect("ESP8266Client", mqttUser, mqttPassword ))
    if (client.connect("ESP8266Client")) 
    {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
 
  client.publish("button", "Hello from ESP8266");
  client.subscribe("led");

}



int value = 0;

void loop() 
{
  client.loop();
  client.publish("button", "Hello from ESP8266");


  
//  delay(5000);
//  ++value;
//
//  Serial.print("connecting to ");
//  Serial.println(host);
//  
//  // Use WiFiClient class to create TCP connections
//  WiFiClient client;
//  const int httpPort = 80;
//  if (!client.connect(host, httpPort)) 
//  {
//    Serial.println("connection failed");
//    return;
//  }
//  
//  Serial.print("Requesting URL: ");
//  Serial.println(url);
  
  
}
