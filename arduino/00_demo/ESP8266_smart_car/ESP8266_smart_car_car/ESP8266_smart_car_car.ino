#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <ESP8266WiFi.h>
#include "user_interface.h"
#include <WiFiClient.h>
#include <WiFiUdp.h>

#include "common.h"

status_t         status = STATUS_CLEAR;
driveState_t     driveState = DRIVE_DIR_STAND;
speed_t          speed = 0;
direction_t      direction = DRIVE_TURN_OFF;
turnLevel_t      turnLvl = 0;


//SSID and Password to your ESP Access Point
const char * ssid = "smart_car";
const char * password = "12345678";
 

/* UDP server */

#define BUFFER_LENGTH 256

WiFiUDP Udp;

IPAddress     mcastAddr(MCAST_ADDR);
unsigned int  mcastPort = MCAST_PORT;
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

/*************************************************************
* setup
*************************************************************/

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

/*************************************************************
* getVoltage
*************************************************************/

void menuDisplay (void)
{
  display.clearDisplay();
  for (int i = 0; i < MENU_LINES; i++)
  {
    display_str(0, MENU_LINE_HIGHT * i, bufMenu[i]);
  }
  display.display();
}

/*************************************************************
* getVoltage
*************************************************************/

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
  sendStatus();
  sendTest();
  
  menuDisplay();
  delay(500);
}

/*************************************************************
* getVoltage
*************************************************************/

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

/*************************************************************
* getVoltage
*************************************************************/

void display_str(uint8_t x, uint8_t y, char* str)
{
  display.setCursor(x, y);  /* Set x,y coordinates */
  display.println(str);
}

/*************************************************************
* getVoltage
*************************************************************/

void readUdp()
{
  int len;
  int packetLength;
  
  packetLength = Udp.parsePacket(); 
  if (packetLength != 0)
  {
    len = Udp.read(inPkt, BUFFER_LENGTH);
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

      processMessage (inPkt, len);
    }
  }
}

/*************************************************************
* getVoltage
*************************************************************/

void sendStatus()
{
  msgStatus_t msgStatus;
  
  msgStatus.status = 0;
  msgStatus.voltage = getBatVoltage();
  sendUdp ((char *) &msgStatus, sizeof (msgStatus_t));
}

/*************************************************************
* getVoltage
*************************************************************/

void sendTest ()
{
  char msg[] = "message";
  int len = 1;
  sendUdp (msg, len);
}

/*************************************************************
* getVoltage
*************************************************************/

void sendUdp(char * pData, int len)
{
  int sz;
  
  if (Udp.beginPacketMulticast (mcastAddr, mcastPort, WiFi.localIP(),  3) != 1)
  {
    Serial.println ("error createing multicast packet");
  }
    
  sz = Udp.write (pData, len);
  Serial.print ("written multicast ");
  //Serial.println(sz);
  Udp.endPacket();    
}


/*************************************************************
* getVoltage
*************************************************************/

void initDrive ()
{
  /* initialize motor speed 0 */

  pinMode (PIN_MOT_PWM, OUTPUT);              /* output mot PWM */
  analogWrite (PIN_MOT_PWM, MOT_SPEED_STOP);  /* 0 % PWM */

  /* set standing */

  pinMode (PIN_MOT_PLUS, OUTPUT);             /* output mot PLUS */
  pinMode (PIN_MOT_MINUS, OUTPUT);            /* output mot MINUS */
  digitalWrite (PIN_MOT_PLUS, MOT_CTL_OFF);   /* OFF mot MINUS */
  digitalWrite (PIN_MOT_MINUS, MOT_CTL_OFF);  /* OFF mot PLUS */
}

/*************************************************************
* getVoltage
*************************************************************/

void initDirection ()
{
  pinMode (PIN_MOT_DIR, OUTPUT);             /* output mot PLUS */
  
   /* set forward/standing/reverse */

   /* set turn level straight */
}



/*************************************************************
* getVoltage
*************************************************************/

void setDrive ()
{
  int motPwm;
  int motPlus = MOT_CTL_OFF;
  int motMinus = MOT_CTL_OFF;
   
  /* set forward/standing/reverse: check requested drive state and apply motor direction */

  switch (driveState)
  {
    case STATUS_DRIVE_FORWARD:
      motPlus = MOT_CTL_ON;
      break;
    case STATUS_DRIVE_REVERSE:
      motMinus = MOT_CTL_ON;
      break;
    case STATUS_DRIVE_STANDING:
    case STATUS_DRIVE_STOPPED:
    default:
      break;
  }
  digitalWrite (PIN_MOT_PLUS, motPlus);
  digitalWrite (PIN_MOT_MINUS, motMinus);

  /* set motor speed: map request range to motor control range, then apply motor pwm */

  motPwm = map (speed, MOT_REQ_MIN, MOT_REQ_MAX, MOT_CTL_MIN, MOT_CTL_MAX);  
  analogWrite (PIN_MOT_PWM, motPwm);
}

/*************************************************************
* getVoltage
*************************************************************/

void setDirection ()
{
  (void) direction;
}

/*************************************************************
* getVoltage
*************************************************************/

int processRequest (msgRequest_t * pReq)
{
  driveState = pReq->driveState;
  
  if (pReq->speed > MOT_REQ_MAX)
  {
    speed = MOT_REQ_MAX;  
  }
  else
  {
    speed = pReq->speed;
  }
  
  direction = pReq->direction;
  turnLvl = pReq->turnLvl;
}

/*************************************************************
* getVoltage
* analog input:    0 - vol - 3.3V (3300mV)
* converted value: 0 - ana - 1023
* 
* bat   R1   vol   R2   GND
*  *-========-*-========-*
*  
*  vol - R2
*  bat - R1 + R2
*************************************************************/

int getBatVoltage()
{
  int ana;
  int vol;
  int bat;
  
  ana = analogRead(PIN_VOLTAGE);
  vol = (ana * VOL_SUPPLY) / ANA_RANGE;
  bat = (vol * (VOL_BAT_DIV_R1 + VOL_BAT_DIV_R2)) / VOL_BAT_DIV_R2;
  
  return vol;
}

/*************************************************************
* processMessage
*************************************************************/

int processMessage(char * pData, int len)
{
  msgType_t      msgType;
  msgRequest_t * pReq;

  msgType = pData[0];

  switch (msgType)
  {
    case MSG_TYPE_REQ:
      if (len <= (sizeof (msgRequest_t)))
      {
        return -1;
      }
      pReq = (msgRequest_t *) (pData + 1);
      processRequest (pReq);
      break;
    case MSG_TYPE_RESP:
    case MSG_TYPE_EVENT:
    case MSG_TYPE_STATUS:
    default:
      break;
  }

  return 0;
}
