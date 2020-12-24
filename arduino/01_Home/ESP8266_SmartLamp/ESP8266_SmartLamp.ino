/*
 * Pinout:
 * D0
 * D1 - I2C SCL
 * D2 - I2C SDA
 * D3
 * D4
 * D5 - ENCODER SELECT
 * D6 - ENCODER DT
 * D7 - ENCODER_CLK
 * D8
 * 
 * I2C:
 * - Display (OLED)
 * - Senzor BME280
 * 
 */



//#define DEBUG_ESP_WIFI
//#define DEBUG_ESP_PORT  Serial
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <Encoder.h>
#define ENC_USE_MAX_MIN_POS
#include <RotaryEncoderSwitch.h>
#include <GenericLamp.h>
//#include <StatusLED.h>


#define DEBUG_MQTT_PUB      FALSE


// DISPLAY
/* 
 * TODO: in Adafruit_SSD1306.cpp  
 * needed for I2C speed
 * 
 *  #define TRANSACTION_START
 *  #define TRANSACTION_END
 *  
 *  #if 0
 *  ...
 *  #else
 *  #define TRANSACTION_START
 *  #define TRANSACTION_END
 *  #endif
 *  
 *  
 * .arduino15/packages/esp8266/hardware/esp8266/2.5.2/cores/esp8266/core_esp8266_si2c.cpp 
 * twi_dcount = 1; always (400k)
 */

//#define SSD1306_128_64
//#define SSD1306_128_32
#include <Adafruit_SSD1306.h>

#define OLED_RESET -1
/* Object named display, of the class Adafruit_SSD1306 */
Adafruit_SSD1306 display(OLED_RESET);
//Adafruit_SSD1306 display();
uint8_t dispCnt;


#define DEV_ID          "lamp_01"

#define HELLO_STR       "SmartLamp start"

#define WIFI_SSID       "embedded"
#define WIFI_PASS       "82668266"
#define MQTT_SERVER     "10.0.1.217"
#define MQTT_PORT       1883
#define MQTT_CLNT_ID    DEV_ID

/* PINs */

#define ENC_PIN_SEL     D5
#define ENC_PIN_CLK     D6
#define ENC_PIN_DT      D7
#define PIN_LED_1       D8 // GPIO2
#define PIN_LED_2       D0 // GPIO2
#define PIN_SW_FLASH    0
#define PIN_LED_STAT    D1
#define BUILTIN_LED     D4

#define PIN_I2C_SCL     D1
#define PIN_I2C_SDA     D2
#define PIN_BME_SCL     PIN_I2C_SCL
#define PIN_BME_SDA     PIN_I2C_SDA


#define ENC_MIN_VAL     0
//#define ENC_MAX_VAL     10
#define ENC_MAX_VAL     20
#define LAMP_MAX_VAL    1024

#define RECONNECT_DELAY     1000
#define ENC_CHECK_PERIOD    1           // cyclic check of the enc signals (ms)

#define PER_CYCLIC_PUB      2000        // cyclic publish status period (ms)
#define PER_SENS_READ       2000        // cyclic check of the sensors value (ms)

#define MQTT_MSG_SIZE   50
#define MQTT_TOP_PING_TAG       "/pong/"
#define MQTT_MSG_PING_FMT       "%ld"
#define MQTT_MSG_VAL_DEC_FMT    "%ld"
#define MQTT_MSG_VAL_FLO_FMT    "%.2f"
#define MQTT_TOP_SET_LAMP_ST    "/set/led"
#define MQTT_TOP_SET_LAMP_VAL   "/set/val"
#define MQTT_TOP_GET_LAMP_ST    "/get/led"
#define MQTT_TOP_GET_LAMP_VAL   "/get/val"

#define MQTT_TOP_PING           MQTT_TOP_PING_TAG DEV_ID        /* /pong/<device_id>  */
#define MQTT_TOP_SENS_TEMP_TAG  "/sens/temp/"
#define MQTT_TOP_SENS_TEMP      MQTT_TOP_SENS_TEMP_TAG DEV_ID   /* /sens/temp/<dev_id> */
#define MQTT_TOP_SENS_HUMI_TAG  "/sens/humi/"
#define MQTT_TOP_SENS_HUMI      MQTT_TOP_SENS_HUMI_TAG DEV_ID   /* /sens/humi/<dev_id> */
#define MQTT_TOP_SENS_PRES_TAG  "/sens/pres/"
#define MQTT_TOP_SENS_PRES      MQTT_TOP_SENS_PRES_TAG DEV_ID   /* /sens/pres/<dev_id> */


#define MQTT_PING_VAL_RESET_BIT 0x80

typedef enum 
{
    WIFI_ST_NA = 0,
    WIFI_ST_INIT = 1,
    WIFI_ST_CONNECTING = 2,
    WIFI_ST_CONNECTED = 3,
    WIFI_ST_DISCONNECTED = 4,
} WifiState;

typedef enum
{
    STATE_INIT               = 0,
    STATE_USR_ACT            = 1,        /* user action: short feedback */
    STATE_WIFI_CONNECTING    = 2,
    STATE_WIFI_CONNECTED     = 3,        /* mqtt state is higher prio */
    STATE_WIFI_DISCONNECTED  = 4,
    STATE_MQTT_CONNECTED     = 5,
    STATE_MQTT_DISCONNECTED  = 6,
    STATE_MAX
} LampState;

//StateColor stateColor[STATE_MAX] =
//{
//  /* ON  OFF  COLOR ACT  COLOR IDLE */
//    {  0,  0, BLUE_L   , BLUE_LL   },         // STATE_INIT
//    {  0,  0, BLUE_H   , BLACK     },         // STATE_USR_ACT
//    {  0,  0, YELLOW_M , YELLOW_LL },         // STATE_WIFI_CONNECTING
//    {  0,  0, GREEN_M  , GREEN_LL  },         // STATE_WIFI_CONNECTED
//    {  0,  0, RED_M    , RED_LL    },         // STATE_WIFI_DISCONNECTED
//    {  0,  0, WHITE_M  , WHITE_LL  },         // STATE_MQTT_CONNECTED
//    {  0,  0, PURPLE_M , PURPLE_LL }          // STATE_MQTT_DISCONNECTED
//};

/************************* WiFi Access Point *********************************/

const char * wifiSsid = WIFI_SSID;
const char * wifiPass = WIFI_PASS;
const char * mqttSrv = MQTT_SERVER;
const uint16_t mqttPort = MQTT_PORT;

uint32_t timeNow = 0;
uint32_t timeOld = 0;
uint32_t timeOldEnc = 0;        /* encoder time base */
uint32_t timeOldAct = 0;        /* activity time base */
uint32_t timeStatuLed = 0;
uint32_t timeOldSens = 0;
uint8_t statusLed = 0;

float sensTemp;
float sensHumi;
float sensPres;


uint8_t actCnt = 0;

long lastReconnectAttempt = 0;

WiFiClient wifiClnt;
PubSubClient mqttClnt (wifiClnt);

int32_t mqttState = MQTT_DISCONNECTED;
bool mqttIsCon = false;

long lastMsg = 0;
char mqttMsg[MQTT_MSG_SIZE];
int value = 0;

uint8_t pingVal = MQTT_PING_VAL_RESET_BIT;

//STATE state = STATE_NA;
//STATE oldState = STATE_NA;
WifiState wifiState = WIFI_ST_NA;
IPAddress ip;                       /* local IP address */


RotaryEncoderSwitch enc (ENC_PIN_CLK, ENC_PIN_DT, ENC_PIN_SEL);
GenericLamp lamp (PIN_LED_1, PIN_LED_2, LAMP_TYPE_1_2_WB);
//StatusLED statLed (PIN_LED_STAT);

int pos = ENC_POS_NA;
int sel = SEL_EV_NONE;

#define ACTIVE_COUNTER      5000    /* (~ms) */
#define USER_ACT_FB_DUR     20      /* user action feedback duration(~ms) */
#define STATUS_LED_PER      500

uint8_t cnt = 0;
uint8_t cntOld = 0;
bool isActive = true;                 /* activity flag */
bool isUserAct = false;               /* activity flag */

void setupEnc (void);
void setupLamp (void);
void setupWifi (void);
void setupMqtt (void);
//void setupStateLed (void);

void loopEnc (void);
void loopWifi (void);
void loopMqtt (void);
void loopActivity (void);
//void loopShowState (void);

void mqttPub (char * topic, char * value);
void mqttPubCyclic (void);
void mqttCbk (char * topic, byte * payload, unsigned int length);

void activeRestart (void);

#define BME_ADR 0x76
Adafruit_BME280 bme; // I2C

#define ADR_DISPLAY   0x3C


typedef enum
    {
    SELECT_MIN = 0,
    SELECT_BRIGHT = 0,
    SELECT_CTEMP  = 1,
    SELECT_MAX
    } MENU_SELECT;

uint8_t menuSel = SELECT_BRIGHT;
uint8_t menuCnt = 0;
uint8_t menuCntOld = 0;

/******************************************************************************
*
* setup - main setup
*
* TODO
*
* RETURNS: N/A
*/

void setup (void)
{
    Serial.begin (115200);
    Serial.println ();
    Serial.println (HELLO_STR);

    pinMode (BUILTIN_LED, OUTPUT);
    digitalWrite (BUILTIN_LED, LOW);

    
    setupSensor ();
    
    setupLamp ();
    setupEnc ();
  
    setupWifi ();
    setupMqtt ();

    setupMenu ();

   // publishLamp ();
}

/******************************************************************************
*
* loop - main loop
*
* TODO
*
* RETURNS: N/A
*/

void loop (void)
{
    timeNow = millis ();

    loopEnc ();             // encoder cyclic
    loopWifi ();            // wifi cyclic
    loopMqtt ();            // mqtt cyclic

    readSensors ();

//    loopActivity ();
//    loopShowState ();

    loopMenu ();
    delay (1);
}

/******************************************************************************
*
* loopMenu
*
* TODO
*
* RETURNS: N/A
*/

void setupMenu (void)
{
    char time[30];

    display.begin(SSD1306_SWITCHCAPVCC, ADR_DISPLAY); /* Initialize display with address 0x3C */
  //  display.invertDisplay(1);
    display.clearDisplay();  /* Clear display */
    display.setTextSize(1);  /* Select font size of text. Increases with size of argument. */
    display.setTextColor(WHITE);  /* Color of text*/

  //  sprintf(time, "%02d:%02d:%02d", h, m, s);
    drawStr(0, 0, "IP:");
    menuUpdate ();
    display.display();
}

/******************************************************************************
*
* loopMenu
*
* TODO
*
* RETURNS: N/A
*/

void loopMenu (void)
{
    State_e state;
    VALUE_IN val;

    if (menuCnt != menuCntOld)
    {
//    Serial.print ("men-sel=");
//    Serial.println (menuSel);

//    Serial.print ("men-bright=");
    val = lamp.getVal1 ();
//    Serial.println (val);
    val = map (val, ENC_MIN_VAL, ENC_MAX_VAL, 0, 100);
    drawLevelBar ("*", 0, 10, val, (menuSel == SELECT_BRIGHT), 0);

//    Serial.print ("men-ctemp=");
    val = lamp.getVal2 ();
//    Serial.println (val);
    val = map (val, ENC_MIN_VAL, ENC_MAX_VAL, 0, 100);
    drawScrollBar (0, 20, val, (menuSel == SELECT_CTEMP), 0);

    menuCnt = menuCntOld;
    display.display();
    }
}

void menuUpdate ()
{
    menuCnt++;
}

/******************************************************************************
*
* drawStr
*
* TODO
*
* RETURNS: N/A
*/

void drawStr(uint8_t x, uint8_t y, String str)
{
  display.setCursor(x, y);  /* Set x,y coordinates */
  display.println(str);
}

/******************************************************************************
*
* setupSensor - setup all sensors
*
* TODO
*
* RETURNS: N/A
*/

void setupSensor ()
{  
  Serial.println ("Sensor Test...");
  
  Serial.println ("Initialize i2c...");
  pinMode (PIN_BME_SDA, INPUT_PULLUP);        //Set input (SDA) pull-up resistor on
  Wire.setClock (2000000);                // Set I2C bus speed 
  Wire.begin (PIN_BME_SDA, PIN_BME_SCL);
  Serial.println ("...done");

  Serial.println ("Initialize BME280...");
  if (!bme.begin (BME_ADR))
  {
    Serial.print ("Ooops, no BME280 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }
  else 
  {
    Serial.println ("BME280 ready.");
  }
}

#define MENU_BAR_W 64
#define MENU_BAR_H 5

/******************************************************************************
*
* setupSensor - setup all sensors
*
* TODO
*
* RETURNS: N/A
*/

void drawLevelBar(String str, int x, int y, int level, int sel, int act)
{
    int w = MENU_BAR_W;
    int h = MENU_BAR_H;
    int f = 0;
    int wl = 9;           // width label
    int xc,yc,wc,hc;      // cursors

    display.fillRect (x, y, w, h, BLACK);   /* clear area */

    drawStr (x, y, str);

    x = x + wl;   // offset for header sign *
    w = w - 2*wl; // centered - 2x header size
    f = map (level, 0, 100, 0, w);

    display.drawLine (x, y+h/2, x+w-1, y+h/2, WHITE);
    //display.drawLine (x, y+h, x+w-1, y+h, WHITE);
    display.fillRect (x, y, f, h, BLACK);
    if (sel)
    {
        display.fillRect (x, y, f, h, WHITE);
    }
    else
    {
        display.drawRect (x, y, f, h, WHITE);
    }
    dispCnt++;
}

/******************************************************************************
*
* setupSensor - setup all sensors
*
* TODO
*
* RETURNS: N/A
*/

void drawScrollBar(int x, int y, int level, int sel, int act)
{
    int w = MENU_BAR_W;
    int w1;
    int h = MENU_BAR_H;
    int f = 0;            // cursor location
    int c;                // cursor width
    int wl = 9;           // width label
    int xc,yc,wc,hc;      // cursors

    if (act)
    {
        c = 5;
    }
    else
    {
        c = 5;//1;
    }

    display.fillRect (x, y, w, h, BLACK); /* clear area */
    drawStr (x, y, "W");
    drawStr (x+w-wl+2, y, "C");

    x = x + wl; /* leave space for label */
    w1 = w - 2*wl;
    f = map (level, 0, 100, 0, w1-c) ;

    /* draw line (not active) or frame (active) */

    xc = x+f;
    yc = y;
    wc = c;
    hc = h;

    display.drawLine (x, y+h/2, x+w1-1, y+h/2, WHITE);
    if (sel)      // selected item
    {
//        if (act)    // active controller
//        {
            display.fillRect (xc, yc, wc, hc, WHITE);
//        }
    }
    else            // not selected item
    {
        display.fillRect (xc, yc, wc, hc, BLACK);
        display.drawRect (xc, yc, wc, hc, WHITE);
    }

    dispCnt++;
}

/******************************************************************************
*
* loopEnc - loop encoder
*
* TODO
*
* RETURNS: N/A
*/

void loopEnc (void)
{
    uint32_t   encCheckPer;
    EncEvent_e encEv;

    int led;
    int newPos;
    int newSel;
    int c;

    c = cnt;
    encCheckPer = timeNow - timeOldEnc;
    if (encCheckPer >= ENC_CHECK_PERIOD)
    {
        timeOldEnc = timeNow;
        enc.cyclic();
    }


#if 0 // use position
    newPos = enc.getPosition();
    if (pos != newPos)
    {
        Serial.print(newPos);
        Serial.println();
        pos = newPos;
        lamp.setVal (pos);
//        led = map (pos, 0, ENC_MAX_VAL, 1024, 0);
//        analogWrite(LED_PIN, led);
    }
#endif


    encEv = enc.getEncEvent ();
    enc.clearEncEvent ();
    if (encEv == ENC_EV_CW)
    {
//        Serial.print("ENC_CW: ");
//        Serial.println(menuSel);
        switch (menuSel)
        {
        case (SELECT_BRIGHT):
            lamp.incVal1 ();
            break;
        case (SELECT_CTEMP):
            lamp.incVal2 ();
            break;
        default:
            break;
        }
        publishLamp ();
    }
    else if (encEv == ENC_EV_CCW)
    {
//        Serial.print("ENC_CCW: ");
//        Serial.println(menuSel);
        switch (menuSel)
        {
        case (SELECT_BRIGHT):
            lamp.decVal1 ();
            break;
        case (SELECT_CTEMP):
            lamp.decVal2 ();
            break;
        default:
            break;
        }
        publishLamp ();
    }

    newSel = enc.getSelectEvent();
    enc.clearSelectEvent();
    if (sel != newSel)
    {
        Serial.print("ST: ");
        Serial.println(lamp.getState());


        sel = newSel;
        if (newSel == SEL_EV_CLICK)
        {
//            lamp.switchState ();
            Serial.println ("Click");
        }
        else if (newSel == SEL_EV_SG_CLICK)
        {
            lamp.switchState ();
            publishLamp ();
            Serial.println ("Single Click");
        }
        else if (newSel == SEL_EV_DB_CLICK)
        {
            menuSel++;
            if (menuSel >= SELECT_MAX)
                {
                menuSel = SELECT_MIN;
                }
            menuCnt++;

            Serial.println ("Double Click");
        }
        else if (newSel == SEL_EV_LONG_PRESS)
        {
            Serial.println ("Long Press");
        }
        else if (newSel == SEL_EV_LLONG_PRESS)
        {
            Serial.println ("Long Long Press");
        }
    }
}

/******************************************************************************
*
* loop - main loop
*
* TODO
*
* RETURNS: N/A
*/

void setupMqtt (void) 
{
    mqttClnt.setServer(mqttSrv, mqttPort);
    mqttClnt.setCallback(mqttCbk);
}

/******************************************************************************
*
* loopMqtt - main loop
*
* TODO
*
* RETURNS: N/A
*/

void loopMqtt (void)
{
    bool isCon;

    if (wifiState != WIFI_ST_CONNECTED)     /* wifi not connected */
        {
        mqttIsCon = false;
        return;
        }

    isCon = mqttClnt.connected ();

    if (!isCon)     // not connected
    {
        if (mqttIsCon)      // connection lost
            {
            Serial.println ("MQTT connexion lost");
            mqttIsCon = false;
            }

        long now = millis ();
        if (now - lastReconnectAttempt > RECONNECT_DELAY)
        {
            Serial.println ("Attempting MQTT connection...");
            lastReconnectAttempt = now;
            if (mqttClnt.connect (MQTT_CLNT_ID))
            {
                Serial.println ("MQTT connected");
                mqttIsCon = true;

                // Once connected, publish an announcement...
                mqttClnt.publish ("outTopic", "hello world");
                // ... and resubscribe
                mqttClnt.subscribe (MQTT_TOP_SET_LAMP_ST);
                mqttClnt.subscribe (MQTT_TOP_SET_LAMP_VAL);
            }
        }
    }
    else
    {
        mqttClnt.loop();

// TODO: sample publish periodically
        if (timeNow - lastMsg > PER_CYCLIC_PUB)
        {
            lastMsg = timeNow;
            mqttPubCyclic ();
        }
//
        mqttClnt.loop();
    }
}

/******************************************************************************
*
* loop - main loop
*
* TODO
*
* RETURNS: N/A
*/

void mqttPub (char * topic, char * msg)
{
    if (!mqttClnt.connected())
    {
        return;
    }
#if DEBUG_MQTT_PUB
    Serial.print("Pub: [");
    Serial.print(topic);
    Serial.print("]=[");
    Serial.print(msg);
    Serial.println("]");
#endif
    mqttClnt.publish(topic, msg);
}

/******************************************************************************
*
* loop - main loop
*
* TODO
*
* RETURNS: N/A
*/

void mqttPubCyclic (void)
{
    if (!mqttClnt.connected())
    {
        return;
    }

    pingVal++;
    if ((pingVal == MQTT_PING_VAL_RESET_BIT) ||
        (pingVal == 0))
      {
        pingVal = 1;
      }
    snprintf (mqttMsg, MQTT_MSG_SIZE, MQTT_MSG_PING_FMT, pingVal);
    mqttPub (MQTT_TOP_PING, mqttMsg);
}

/******************************************************************************
*
* mqttCbk - main loop
*
* TODO
*
* RETURNS: N/A
*/

void mqttCbk (char * topic, byte * payload, unsigned int length)
{
    int val;
    uint16_t setVal;
    String s = String((char *)payload);
    
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println();

    if (strcmp (topic, MQTT_TOP_SET_LAMP_ST) == 0)    // set state
    {
        // Switch on the LED if an 1 was received as first character
        if ((char)payload[1] == 'N')        // ON/OFF
        {
            lamp.setOn ();
            publishLamp ();
        }
        else
        {
            lamp.setOff ();
            publishLamp ();
        }
    }
    else if (strcmp (topic, MQTT_TOP_SET_LAMP_VAL) == 0)  // set value 
    {
        scanf ((char *)payload, "%d", &val);
//        Serial.print ("Val ");
//        Serial.print (val);
        val = s.toInt();
        setVal = map (val, 0, 100, ENC_MIN_VAL, ENC_MAX_VAL);
//        Serial.print ("SetVal ");
//        Serial.print (setVal);
        lamp.setVal (setVal);
        publishLamp ();
    }
}


void cyclic_time (void) 
{

}




///******************************************************************************
//*
//* setupLight - configure Light
//*
//* TODO
//*
//* RETURNS: N/A
//*/
//
//void setupStateLed (void)
//{
//    statLed.init ();
//    statLed.confStCol (STATE_MAX, &stateColor[0]);
//    statLed.setState (STATE_INIT, isActive);
//}


/* intHndClk
 *
 */

ICACHE_RAM_ATTR void intHndClk (void)
{
    enc.clkEdgeInt();
}

/* intHndDt
 *
 */

ICACHE_RAM_ATTR void intHndDt (void)
{
    enc.dtEdgeInt();
}

/* intHndSel
 *
 */

ICACHE_RAM_ATTR void intHndSel (void)
{
    enc.selEdgeInt();
}

#if 0 // avoid using timer interrupts
/* tmrHnd
 * cyclick check for rotary encoder changes
 */

ICACHE_RAM_ATTR void inline tmrHnd (void)
{
    timer0_write(ESP.getCycleCount() + 80000);
    cnt++;
    //enc.cyclic();
}
#endif

void setupLamp (void)
{
    Serial.println ("Setup Lamp");
    //lamp.setPol (POLA_NEG);
    lamp.setPol (POLA_POS, POLA_POS);
    lamp.setInRange1 (ENC_MAX_VAL);
    lamp.setInRange2 (ENC_MAX_VAL);
    lamp.setVal2 (ENC_MAX_VAL/2);
    lamp.setOutRange1 (LAMP_MAX_VAL);
    lamp.setOutRange2 (ENC_MAX_VAL);
}

/* setupEnc
 * setup the encoder 
 */

void setupEnc (void)
{
    Serial.println ("Setup Encoder");
    enc.setMinMaxPos (ENC_MIN_VAL, ENC_MAX_VAL);

    noInterrupts ();

#if 0  // avoid using timer interrupts
    timer0_isr_init ();
    timer0_attachInterrupt (tmrHnd);
    timer0_write (ESP.getCycleCount () + 80000);  /* 1 ms */
#endif

    pinMode (ENC_PIN_CLK, INPUT_PULLUP);
    pinMode (ENC_PIN_DT, INPUT_PULLUP);
    pinMode (ENC_PIN_SEL, INPUT_PULLUP);

    attachInterrupt (ENC_PIN_CLK, intHndClk, CHANGE);
    attachInterrupt (ENC_PIN_DT, intHndDt, CHANGE);
    attachInterrupt (ENC_PIN_SEL, intHndSel, CHANGE);

    interrupts();
}


/******************************************************************************
*
* setupWifi - configure WIFI client connectivity
*
* TODO
*
* RETURNS: N/A
*/

void setupWifi (void)
{
//    updateShowState (STATE_CONNECTING);

    Serial.println ("Setup WiFi");
    Serial.print ("Connecting to [");
    Serial.print (wifiSsid);
    Serial.println ("] ...");

    /*
     * Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     * would try to act as both a client and an access-point and could cause
     * network-issues with your other WiFi-devices on your WiFi-network.
     */

    WiFi.mode (WIFI_STA);
    WiFi.begin (wifiSsid, wifiPass);
    wifiState = WIFI_ST_CONNECTING;
}

/******************************************************************************
*
* loopWifi -
*
* TODO
*
* RETURNS: N/A
*/

void loopWifi (void)
{
    /* wifi module reported status:
     * WL_IDLE_STATUS      = 0,
     * WL_NO_SSID_AVAIL    = 1,
     * WL_SCAN_COMPLETED   = 2,
     * WL_CONNECTED        = 3,
     * WL_CONNECT_FAILED   = 4,
     * WL_CONNECTION_LOST  = 5,
     * WL_DISCONNECTED     = 6
     *
     */

    int wifiModState;

    wifiModState = WiFi.status ();

    if ((wifiState == WIFI_ST_CONNECTING) &&        /* first connection */
        (wifiModState == WL_CONNECTED))
    {
        wifiSetState (WIFI_ST_CONNECTED);
    }
    else if ((wifiState == WIFI_ST_CONNECTED) &&    /* disconnected */
             (wifiModState != WL_CONNECTED))
    {
        wifiSetState (WIFI_ST_DISCONNECTED);
    }
    else if ((wifiState == WIFI_ST_DISCONNECTED) &&  /* reconnected */
             (wifiModState == WL_CONNECTED))
    {
        wifiSetState (WIFI_ST_CONNECTED);
    }
}

String IpAddress2String(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ; 
}

/******************************************************************************
*
* setupWifi - configure WIFI client connectivity
*
* TODO
*
* RETURNS: N/A
*/

void wifiSetState (WifiState st)
{
    if (wifiState == st)
    {
        return;
    }

    Serial.print ("WiFi status changed to: ");
    if (st == WIFI_ST_CONNECTED)
    {
        Serial.println ("[connected]");
        Serial.print ("  IP address: [");
        Serial.print (WiFi.localIP ());
        Serial.println ("]");
        drawStr(20, 0, (WiFi.localIP()).toString());
        display.display();
    }
    else if (st == WIFI_ST_DISCONNECTED)
    {
        Serial.println ("[disconnected]");
    }
    wifiState = st;
}

void activeRestart (void)
{
    timeOldAct = timeNow;
    actCnt++;
    isActive = true;
    isUserAct = true;
}

void loopActivity (void)
{
    if (timeNow - timeStatuLed > STATUS_LED_PER)
    {
        timeStatuLed = timeNow;
        statusLed = !statusLed;
        digitalWrite (BUILTIN_LED, statusLed);
    }

    if (isActive)
    {
        if (isUserAct && (timeNow - timeOldAct > USER_ACT_FB_DUR))
        {
            isUserAct = false;
        }

        if (timeNow - timeOldAct < ACTIVE_COUNTER)
        {
            return;
        }

        Serial.println ("Go idle ...");
        actCnt++;
        isActive = false;
    }
}

/******************************************************************************
*
* loopShowState - configure WIFI client connectivity
*
* TODO
*
* RETURNS: N/A
*/

//void loopShowState (void)
//{
//     STATE st;
//     static uint8_t cnt = 0;
//
//    /* LED status priorities:
//     * - click activities
//     * -
//     *
//     *    STATE_INIT          = 0,
//     *    STATE_IDLE          = 1
//     *    STATE_CLICK         = 2
//     *    STATE_CONNECTING    = 3
//     *    STATE_CONNECTED     = 4
//     *    STATE_DISCONNECTED  = 5
//     */
//
//    if (isUserAct)
//    {
//        st = STATE_USR_ACT;
//    }
//    else if (wifiState == WIFI_ST_CONNECTING)
//    {
//        st = STATE_WIFI_CONNECTING;
//    }
//    else if (wifiState == WIFI_ST_DISCONNECTED)
//    {
//        st = STATE_WIFI_DISCONNECTED;
//    }
//    else if (wifiState == WIFI_ST_CONNECTED)
//    {
//        if (mqttIsCon)
//        {
//            st = STATE_MQTT_CONNECTED;
//        }
//        else
//        {
//            st = STATE_MQTT_DISCONNECTED;
//        }
//    }
//
//    if ((st != state) || (cnt != actCnt))
//    {
//        state = st;
//        cnt = actCnt;
//#if 0
//        Serial.print ("Show state [");
//        Serial.print (state);
//        Serial.print ("] is active [");
//        Serial.print (isActive);
//        Serial.println ("]");
//        //statLed.setState (state, isActive);
//        Serial.println (" -> State DONE");
//#endif
//    }
//
//}

void publishLamp (void)
{
    State_e state;
    VALUE_IN val; 

//    Serial.print ("PUB LAMP");
    val = lamp.getVal ();
    val = map (val, ENC_MIN_VAL, ENC_MAX_VAL, 0, 100);
    state = lamp.getState ();

    snprintf (mqttMsg, MQTT_MSG_SIZE, MQTT_MSG_VAL_DEC_FMT, val);
    mqttPub (MQTT_TOP_GET_LAMP_VAL, mqttMsg);

    if (STATE_OFF == state)
        {
        mqttPub (MQTT_TOP_GET_LAMP_ST, "OFF");
        menuUpdate ();
        }
    else if (STATE_ON == state)
        {
        mqttPub (MQTT_TOP_GET_LAMP_ST, "ON");
        menuUpdate ();
        }
    else 
        {
        }
}


/******************************************************************************
*
* readSensors - read all sensors
*
* Read sensors values and publish on MQTT.
*
* RETURNS: N/A
*/

void readSensors ()
{
  uint32_t perSens;
  
  perSens = timeNow - timeOldSens;
  if (perSens < PER_SENS_READ)
  {
    return;
  }

  timeOldSens = timeNow;      
  
  sensTemp = bme.readTemperature ();
//  Serial.print ("Temperature: ");
//  Serial.print (sensTemp);
//  Serial.println (" C");
  snprintf (mqttMsg, MQTT_MSG_SIZE, MQTT_MSG_VAL_FLO_FMT, sensTemp);
  mqttPub (MQTT_TOP_SENS_TEMP, mqttMsg);

  sensPres = bme.readPressure () * 0.7500637554192 / 100.0F;
//  Serial.print ("Pressure:    ");
//  Serial.print (sensPres);
//  Serial.println (" mmHg");
  snprintf (mqttMsg, MQTT_MSG_SIZE, MQTT_MSG_VAL_FLO_FMT, sensPres);
  mqttPub (MQTT_TOP_SENS_PRES, mqttMsg);
  
  sensHumi = bme.readHumidity ();
//  Serial.print ("Humidity:    ");
//  Serial.print (sensHumi);
//  Serial.println (" %");
  snprintf (mqttMsg, MQTT_MSG_SIZE, MQTT_MSG_VAL_FLO_FMT, sensHumi);
  mqttPub (MQTT_TOP_SENS_HUMI, mqttMsg);
}
