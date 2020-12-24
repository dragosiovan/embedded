#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <ESP8266WiFi.h>
#include "user_interface.h"
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include <WiFiUdp.h>


/* 
 *  00 - standing
 *  01 - driving fw
 *  10 - driving reverse
 */
#define STATUS_DRIVE_MASK     0x0003
#define STATUS_DRIVE_STANDING 0x0000
#define STATUS_DRIVE_FORWARD  0x0001
#define STATUS_DRIVE_REVERSE  0x0002
#define STATUS_DRIVE_STOPPED  0x0003    /* self stopped */

#define DRIVE_DIR_STAND       0x0000
#define DRIVE_DIR_FORWARD     0x0001
#define DRIVE_DIR_REVERSE     0x0002

#define DRIVE_TURN_OFF        0x0000
#define DRIVE_TURN_LEFT       0x0001
#define DRIVE_TURN_RIGHT      0x0002

#define LIGHTS_POS            0x0001
#define LIGHTS_TURN_L         0x0002
#define LIGHTS_TURN_R         0x0004
#define LIGHTS_WARN           0x0008


typedef uint16 carStatus_t;
typedef uint16 carVoltage_t;
typedef uint16 carThrotleLevel_t;
typedef uint8  carDirection_t;
typedef uint8  carDriveState_t;
typedef uint16 carTurnLevel_t;
typedef uint8  carTurn_t;
typedef uint8  carLights_t;

typedef struct statusMsg
{
  carStatus_t   status;
  carVoltage_t  voltage;
  
} statusMsg_t;

typedef struct cmdMsg
{
  carDriveState_t     driveState;
  carThrotleLevel_t   throtLevel;
  carTurnLevel_t      driveDir;
  carTurn_t           turnLvl;
} cmdMsg_t;



carDriveState_t     driveState = DRIVE_DIR_STAND;
carThrotleLevel_t   thrLvl = 0;
carTurnLevel_t      driveDir = DRIVE_TURN_OFF;
carTurn_t           turnLvl = 0;


//SSID and Password to your ESP Access Point
const char * ssid = "smart_car";
const char * password = "12345678";
 

/* UDP server */

#define BUFFER_LENGTH 256

WiFiUDP Udp;

IPAddress     mcastAddr(224,224,224,224);
unsigned int  mcastPort = 1234;
char          inPkt[BUFFER_LENGTH]; 

char replyPacket[] = "Hi there! Got the message :-)";
bool multicast = true; //false;

#define MENU_LINES        7
#define MENU_LINE_LEN     30
#define MENU_LINE_HIGHT   9
char bufMenu[MENU_LINES][MENU_LINE_LEN];
int menuPos;


#define OLED_RESET D5
/* Object named display, of the class Adafruit_SSD1306 */
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
//#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


// 5x7 font

void setup()   {
  memset(bufMenu, 0, MENU_LINES * MENU_LINE_LEN);
//  sprintf(bufMenu[0], "0000000000000000000");
//  sprintf(bufMenu[1], "1111111111111111111");
//  sprintf(bufMenu[2], "2222222222222222222");
//  sprintf(bufMenu[3], "3333333333333333333");
//  sprintf(bufMenu[4], "4444444444444444444");
//  sprintf(bufMenu[5], "5555555555555555555");
//  sprintf(bufMenu[6], "6666666666666666666");
  
  Serial.begin(115200);
  Serial.println ("Initializing...");
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); /* Initialize display with address 0x3C */
//  display.invertDisplay(1);
  display.setTextColor(WHITE);  /* Color of text*/
  display.clearDisplay();       /* Clear display */
  display.setTextSize(1);       /* Select font size of text. Increases with size of argument. */

  Serial.println("Initializing...");
  
  display_str(0, 0, "Initializing...");
  display.display();
  init_com();
  display.display();
  menuPos = 2;

  
}

void menuDisplay (void)
{
  display.clearDisplay();
  for (int i = 0; i < MENU_LINES; i++)
  {
    display_str(0, MENU_LINE_HIGHT * i, bufMenu[i]);
  }
  display.display();
}


void loop() 
{
  int nrCl;
  char buf[20] = {0};
  char ip[30] = {0};

  IPAddress myIP = WiFi.softAPIP(); //Get IP address
  Serial.println(myIP);
  sprintf(bufMenu[0], "IP: %d.%d.%d.%d", myIP[0], myIP[1], myIP[2], myIP[3]);
  nrCl = WiFi.softAPgetStationNum();
  Serial.printf("Stations connected to soft-AP = %d\n", WiFi.softAPgetStationNum());
  sprintf (bufMenu[1], "Clients: %d", nrCl);
  
  readUdp();
  sendUdp();
  
  menuDisplay();
  delay(500);
}


void init_com()
{
  char ip[30] = {0};
  WiFi.mode(WIFI_AP);           //Only Access point
  WiFi.softAP(ssid, password);  //Start HOTspot removing password will disable security
 
  IPAddress myIP = WiFi.softAPIP(); //Get IP address
  Serial.print("HotSpt IP:");
  Serial.println(myIP);
  sprintf(ip,"IP: %d.%d.%d.%d", myIP[0], myIP[1], myIP[2], myIP[3]);
  display_str(0, 10, "Server ON");
  display_str(0, 20, ip);
  if (multicast)
  {
    Udp.beginMulticast(WiFi.localIP(), mcastAddr, mcastPort);
  }
  else
  {
    Udp.begin(mcastPort);
  }
}


void display_str(uint8_t x, uint8_t y, char* str)
{
  display.setCursor(x, y);  /* Set x,y coordinates */
  display.println(str);
}

void readUdp()
{
  int packetLength = Udp.parsePacket(); 
  if (packetLength != 0)
  {
    int len = Udp.read(inPkt, BUFFER_LENGTH);
    if (len > 0) 
    {
      inPkt[len] = 0;
      Serial.printf("%s\n", inPkt);
      bufMenu[menuPos][0] = ' ';
      menuPos++;
      if (menuPos == MENU_LINES)
      {
        menuPos = 2;
      }
      bufMenu[menuPos][0] = '*';
      memcpy (&bufMenu[menuPos][1], inPkt, len+1);
    }
  }
}

void sendUdp()
{
  int sz;
  char msg[] = "message";
  IPAddress     addr(192,168,4,2);

  if (multicast)
  {
    if (Udp.beginPacketMulticast(mcastAddr, mcastPort, WiFi.localIP(),  3) != 1)
      Serial.println("error createing multicast packet");
    sz = Udp.write(msg);
    Serial.print("written multicast ");
    Serial.println(sz);
    Udp.endPacket();    
  }
  else
  {
    if (Udp.beginPacket(addr, mcastPort) != 1)
      Serial.println("error createing multicast packet");
    sz = Udp.write(msg);
    Serial.print("written unicast ");
    Serial.println(sz);
    Udp.endPacket();
  }   
}


void processMessage (char *pData)
{
  cmdMsg_t * pMsg;

  pMsg = (cmdMsg_t *) pData;

  driveState = pMsg->driveState;
  throtLevel = pMsg->throtLevel;
  driveDir = pMsg->driveDir;
  turnLvl = pMsg->turnLvl;
  
}
