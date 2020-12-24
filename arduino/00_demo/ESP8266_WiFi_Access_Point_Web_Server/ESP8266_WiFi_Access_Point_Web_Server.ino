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

#define BUTTON_PIN    D3
#define LED_PIN       D4

WiFiServer server (80);


// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output4State = "off";

// Assign output variables to GPIO pins
const int output4 = D4;


void setup()
{
  Serial.begin (115200);
  Serial.println ();

  // Initialize the output variables as outputs
  pinMode(output4, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output4, LOW);
  

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


  server.begin();
  Serial.printf("Web server started, open %s in a web browser\n", WiFi.localIP().toString().c_str());
}


void loop ()
{
  client_status ();
  web_server ();
  delay (1000);
}




void web_server ()
{
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /4/on") >= 0) {
              Serial.println("GPIO 4 on");
              output4State = "on";
              digitalWrite(output4, HIGH);
            } else if (header.indexOf("GET /4/off") >= 0) {
              Serial.println("GPIO 4 off");
              output4State = "off";
              digitalWrite(output4, LOW);
            } 
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP8266 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 4  
            client.println("<p>GPIO 4 - State " + output4State + "</p>");
            // If the output4State is off, it displays the ON button       
            if (output4State=="off") {
              client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
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


// prepare a web page to be send to a client (web browser)
String prepareHtmlPage()
{
  String htmlPage =
     String("HTTP/1.1 200 OK\r\n") +
            "Content-Type: text/html\r\n" +
            "Connection: close\r\n" +  // the connection will be closed after completion of the response
            "Refresh: 5\r\n" +  // refresh the page automatically every 5 sec
            "\r\n" +
            "<!DOCTYPE HTML>" +
            "<html>" +
            "Analog input:  " + String(digitalRead(BUTTON_PIN)) +
            "</html>" +
            "\r\n";
  return htmlPage;
}
