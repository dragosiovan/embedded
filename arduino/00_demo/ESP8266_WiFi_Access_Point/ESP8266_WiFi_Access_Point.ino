#include <ESP8266WiFi.h>

extern "C" {
  #include<user_interface.h>
}

/* configuration  wifi */
const char *ssid = "ESP8266_ap";
const char *pass = "12345678";

IPAddress local_IP(192,168,7,1);
IPAddress gateway(192,168,7,1);
IPAddress subnet(255,255,255,0);


void setup()
{
  Serial.begin (115200);
  Serial.println ();

  Serial.println ("Setting soft-AP configuration ... ");
  WiFi.softAPConfig (local_IP, gateway, subnet);
  Serial.println ("Setting soft-AP ... ");
  boolean result = WiFi.softAP (ssid, pass);
  if (result == true)
  {
    Serial.println ("Ready");
  }
  else
  {
    Serial.println ("Failed!");
  }
}

void loop ()
{
  client_status ();
  delay (1000);
}

void client_status () {

  unsigned char number_client;
  struct station_info * stat_info;
  
  struct ip_addr * IPaddress;
  IPAddress address;
  int i=1;
  
  number_client= wifi_softap_get_station_num();
  stat_info = wifi_softap_get_station_info();
  
  Serial.print (" Total Connected Clients are = ");
  Serial.println (number_client);
  
    while (stat_info != NULL) {
    
      IPaddress = &stat_info->ip;
      address = IPaddress->addr;
      
      Serial.print("client= ");
      
      Serial.print (i);
      Serial.print (" IP adress is = ");
      Serial.print ((address));
      Serial.print (" with MAC adress is = ");
      
      Serial.print (stat_info->bssid[0],HEX); Serial.print(" ");
      Serial.print (stat_info->bssid[1],HEX); Serial.print(" ");
      Serial.print (stat_info->bssid[2],HEX); Serial.print(" ");
      Serial.print (stat_info->bssid[3],HEX); Serial.print(" ");
      Serial.print (stat_info->bssid[4],HEX); Serial.print(" ");
      Serial.print (stat_info->bssid[5],HEX); Serial.print(" ");
      
      stat_info = STAILQ_NEXT(stat_info, next);
      i++;
      Serial.println();
    }
    
  delay(500);
}
