/*
 * Stairs LEDs strip.
 * It switches automatically when you what to use the stairs, from each of the
 * directions.
 * Current implementation uses:
 * 1 12V power supply for LED strips
 * 1 5V power supply for controller
 * 1 ESP8266 controller
 * 1 LED strip controlled by MOSFET
 * 2 detection sensors:
 * - 1 PIR for downstairs detection
 * - 1 ultrasonic upstairs for distance measurement
 *
 *
 * INPUTS:
 * - supply - 12V
 * - sensor 1 (PIR)
 * - sensor 2 (ultrasonic)
 *
 * OUTPUTS:
 * - LED
 * - LED sensor 1
 * - LED sensor 2
 *
 * +12V   ------------->
 * GND    ------------->
 *
 * +5v    <------------
 * OUT    ------------>
 * GND    <------------
 *
 * LED1   <------------
 * GND    <------------
 *
 *
 * +5V    <------------
 * TRIG   <------------
 * ECHO   ------------>
 * GND    <------------
 *
 * LED2   <------------
 * GND    <------------
 *
 *
 * LED    <------------
 *        <------------
 *        <------------
 * GND    <------------
 *        <------------
 *        <------------
 *
 */





#include "dev_config.h"
#include "c:\work\settings.h"

#define USE_OTA  1

#if USE_OTA
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#endif


#define DBG_F_PLOT_OUT_LED    B00000001
#define DBG_F_PLOT_OUT_EYE    B00000010
#define DBG_F_PLOT_IN_DIST    B00000100

#define DBG_PLOT_OUT_LED      (dbgPlot & DBG_F_PLOT_OUT_LED)
#define DBG_PLOT_OUT_EYE      (dbgPlot & DBG_F_PLOT_OUT_EYE)
#define DBG_PLOT_IN_DIST      (dbgPlot & DBG_F_PLOT_IN_DIST)

uint8_t dbgPlot = 0
//| DBG_F_PLOT_OUT_LED
//| DBG_F_PLOT_OUT_EYE
//| DBG_F_PLOT_IN_DIST
| 0;


#define DBG_F_INIT            B00000001
#define DBG_F_STATE           B00000010
#define DBG_F_STATE_EXT       B00000100
#define DBG_F_OUT_LED         B00001000
#define DBG_F_IN_MOT          B00010000
#define DBG_F_IN_US           B00100000 // ultra sonic
#define DBG_F_STATE_EYE       B01000000
#define DBG_F_STATE_EXT_EYE   B10000000


#define DBG_NOT_PLOT      (dbgPlot == 0) // plot disabled

#define DBG_INIT          ((DBG_NOT_PLOT) && (dbgVal & DBG_F_INIT))
#define DBG_STATE         ((DBG_NOT_PLOT) && (dbgVal & DBG_F_STATE))
#define DBG_STATE_EXT     ((DBG_NOT_PLOT) && (dbgVal & DBG_F_STATE_EXT))
#define DBG_STATE_EYE     ((DBG_NOT_PLOT) && (dbgVal & DBG_F_STATE_EYE))
#define DBG_STATE_EXT_EYE ((DBG_NOT_PLOT) && (dbgVal & DBG_F_STATE_EXT_EYE))
#define DBG_OUT_LED       ((DBG_NOT_PLOT) && (dbgVal & DBG_F_OUT_LED))
#define DBG_IN_MOT        (dbgVal & DBG_F_IN_MOT)
#define DBG_IN_US         (dbgVal & DBG_F_IN_US)

uint8_t dbgVal = 0 
| DBG_F_INIT
| DBG_F_STATE
| DBG_F_STATE_EXT
//| DBG_F_OUT_LED
//| DBG_F_STATE_EYE
//| DBG_F_STATE_EXT_EYE
| DBG_F_IN_MOT
| DBG_F_IN_US
| 0;


#define DBG_LOG 0

#define OUTPUT_NEG          0
#define OUTPUT_EYE_NEG      0

#define USE_MOT_DET_1       1   // PIR
#define USE_MOT_DET_2       1   // ultrasonic


#define PIN_IN_LI_SEN       A0
#define PIN_US_ECHO         D2 //2     // UltraSonic ECHO receive
#define PIN_US_TRIG         D3 //3     // UltraSonic Trigger send pulse
#define PIN_IN_MOT_DET_1    D5 //10
#define PIN_OUT_EYE         D6
#define PIN_OUT_LED_SENS1   D7
#define PIN_OUT_LED         D8 //11


#define LED_PWM_MAX         1023      // PWM 100%
//#define LED_PWM_MAX         255         // PWM 100%
#define PER_MAX             1000

//#define LED_PER_MAX_100     100 // %
//#define LED_PER_IDLE_100    10  // %

#define LED_PER_MAX         (LED_PER_MAX_100*(PER_MAX/100))          // /PER %
#define LED_PER_IDLE        (LED_PER_IDLE_100*(PER_MAX/100))         // /PER %


#if OUTPUT_NEG
#define OUT_HIGH  LOW
#define OUT_LOW   HIGH
#else
#define OUT_HIGH  HIGH
#define OUT_LOW   LOW
#endif



#define MAIN_CYCLE          10        // (ms) check cycle

#define LED_FADE_ON_STEPS   (LED_FADE_ON_TIME/MAIN_CYCLE)

#define LED_FADE_IDLE_ON_TIME  ((LED_FADE_ON_TIME/LED_PER_MAX_100*(LED_PER_MAX_100-LED_PER_IDLE_100)))
#define LED_FADE_IDLE_ON_STEPS (LED_FADE_IDLE_ON_TIME/MAIN_CYCLE)

#define LED_FADE_IDLE_STEPS (LED_FADE_IDLE_TIME/MAIN_CYCLE)
#define LED_FADE_OFF_STEPS  (LED_FADE_OFF_TIME/MAIN_CYCLE)
#define LED_ON_STEPS        (LED_ON_TIME/MAIN_CYCLE)
#define LED_IDLE_STEPS      (LED_IDLE_TIME/MAIN_CYCLE)



#define EYE_FADE_ON_STEPS   (EYE_FADE_ON_TIME/MAIN_CYCLE)
#define EYE_FADE_IDLE_STEPS (LED_FADE_IDLE_TIME/MAIN_CYCLE)
#define EYE_FADE_OFF_STEPS  (LED_FADE_OFF_TIME/MAIN_CYCLE)
#define EYE_ON_STEPS        (EYE_ON_TIME/MAIN_CYCLE)
#define EYE_IDLE_STEPS      (EYE_IDLE_TIME/MAIN_CYCLE)





#define US_DEB_CNT      3       // debounce counter for ultra sonic mot det

#define US_SPEED        0.034   // sound speed (cm/us)
#define US_PULSE_TOUT   (2*US_MAX_DIST/US_SPEED)


/******************************************************************************/
/********************************** TYPES DEFINITION **************************/

typedef enum
{
  LED_STATE_OFF       = 0,
  LED_STATE_FAD_ON    = 1,
  LED_STATE_ON        = 2,
  LED_STATE_FAD_IDLE  = 3,
  LED_STATE_IDLE      = 4,
  LED_STATE_FAD_OFF   = 5,
  LED_STATE_MAX 
} LedState_e;



/******************************************************************************/
/********************************** LOCAL VARIABLES ***************************/

LedState_e eyeState = LED_STATE_OFF;
uint16_t eyeIdleDeb = 0;
uint16_t eyeStateSteps = 0;
uint16_t outEyePer = 0;
uint16_t outEye =
#if OUTPUT_NEG
  LED_PWM_MAX;
#else
  0;
#endif


LedState_e ledState = LED_STATE_OFF;


uint16_t timeOld = 0;
uint16_t timeNow = 0;
  
uint16_t stateSteps = 0;
uint16_t outLEDper = 0;
uint16_t outLED =
#if OUTPUT_NEG
  LED_PWM_MAX;
#else
  0;
#endif

bool motDet = false;         // move detection
bool motDet1 = false;        // move detection sens 1
bool motDet2 = false;        // move detection sens 2

uint8_t usMotCnt = 0;        // ultrasonic mot detection debounce counter

uint8_t motSens1Old = LOW;       // previous state of the motion sensor 1
uint8_t motSens2Old = LOW;       // previous state of the motion sensor 2

long usPulseDur; // variable for the duration of sound wave travel
int usDist;      // variable for the distance measurement


#if USE_OTA
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

#define OTA_PORT    8080
#define OTA_HOST    "LED_SZ_01"
#endif


/******************************************************************************/
/********************************** SETUP *************************************/

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200); // serial

  if (DBG_INIT)
  {
    Serial.println("Initializing...");
  }
  
  setupLED();
  setupLightSensor();
  setupMotDet();

#if USE_OTA
  setupOTA();
#endif

  if (DBG_INIT)
  {
    Serial.println("Initialization done.");
  }
}



/******************************************************************************/
/********************************** MAIN LOOP *********************************/

void loop() 
{
#if USE_OTA
  ArduinoOTA.handle();
#endif

  if (!isCycleTime())
    return;

  // inputs 
	readLightSensor();
	readMotDet();

	// processing
	processSMLED();
  processSMEye();

  // output
  writeLED();
}



/******************************************************************************/
/********************************** PROCESSING ********************************/

void processSMLED()
{
  switch (ledState)
  {
    
    /********************************** LED OFF *******************************/
    case (LED_STATE_OFF):
    {
      if (motDet)
      {
        // switch to FADE IN 0% -> 100%
        ledState = LED_STATE_FAD_ON;    // MOVE TO FADE IN
        stateSteps = LED_FADE_ON_STEPS;

        if (DBG_STATE)
        {
          Serial.print("MOVE TO FADE ON: "); Serial.print(timeNow); Serial.print("-"); Serial.println(stateSteps);
        }
      }
      break;
    }
    
    
    /********************************** LED FADE to ON ***************************/
    case (LED_STATE_FAD_ON):
    {
      // fade from 0% -> 100%
      if (stateSteps > 0) // still need to fade in
      {
        stateSteps--;
        outLEDper = getLedPer ((LED_FADE_ON_STEPS - stateSteps), 0, LED_FADE_ON_STEPS, 0, LED_PER_MAX);
        
        if (DBG_STATE_EXT)
        {
          Serial.print("LED_STATE_FAD_ON: "); Serial.print(stateSteps); Serial.print(" LED: "); Serial.println(outLEDper);
        }
      }
      else // fade in done
      {
        ledState = LED_STATE_ON;  // MOVE TO ON
        stateSteps = LED_ON_STEPS;
        
        if (DBG_STATE)
        {
          Serial.print("MOVE TO ON TIMEOUT: "); Serial.print(timeNow); Serial.print("-"); Serial.println(stateSteps);
        }     
      }

      break;
    }
    

    /********************************** LED ON TIMEOUT ************************/
    case (LED_STATE_ON):
    {
      if (motDet)
      {
        stateSteps = LED_ON_STEPS; // reset ON timeout
      }
      else if (stateSteps > 0) // still need to timeout ON
      {
        stateSteps--;

        if (DBG_STATE_EXT)
        {
          Serial.print("LED_STATE_ON: "); Serial.print(stateSteps); Serial.print(" LED: "); Serial.println(outLEDper);
        }
      }
      else
      {
        ledState = LED_STATE_FAD_IDLE;   // MOVE TO FADE OUT
        stateSteps = LED_FADE_IDLE_STEPS;
        
        if (DBG_STATE)
        {
          Serial.print("MOVE TO FADE IDLE: "); Serial.print(timeNow); Serial.print("-"); Serial.println(stateSteps);
        }     
      }
      
      break;
    }
    

    /********************************** LED FADE to IDLE **************************/
    case (LED_STATE_FAD_IDLE):
    {
      uint16_t cmpLed;
      
      if (motDet)
      {
        ledState = LED_STATE_FAD_ON; // FADE IN back to ON

        if (DBG_STATE_EXT)
        {
          Serial.print("switch to FADEIN from FADE IDLE:"); Serial.print(stateSteps);
        }
        
        stateSteps = map(outLEDper, 0, LED_PER_MAX, LED_FADE_ON_STEPS, 0); // recalculate corresponding for FADEIN duration, FADE IN back to ON

        if (DBG_STATE)
        {
          Serial.print("MOVE TO FADE ON:"); Serial.println(stateSteps);
        }
        break;
      }
      // fade from 100% -> 0%
      else if (stateSteps > 0) // still need to fade out
      {
        stateSteps--;

        outLEDper = getLedPer (stateSteps, 0, LED_FADE_IDLE_STEPS, LED_PER_IDLE, LED_PER_MAX);
      }

      if (DBG_STATE_EXT)
      {
        Serial.print("LED_STATE_FAD_IDLE: "); Serial.print(stateSteps); Serial.print(" LED: "); Serial.println(outLEDper);
      }

      if (stateSteps == 0) // reached the target IDLE value
      {
        ledState = LED_STATE_IDLE;        // move to IDLE state = low brightness
        outLEDper = LED_PER_IDLE;
        stateSteps = LED_IDLE_STEPS;

        if (DBG_STATE)
        {
          Serial.print("MOVE TO IDLE: "); Serial.print(timeNow); Serial.print("-"); Serial.print(stateSteps); Serial.print("- outLEDper:"); Serial.println(outLEDper);
        }      
      }

      break;
    }


    /********************************** LED IDLE **************************/
    // low brightness for a period
    
    case (LED_STATE_IDLE):
    {
      if (motDet)
      {
        // TODO

        ledState = LED_STATE_FAD_ON; // FADE IN back to ON
        Serial.print("switch to FADEIN from IDLE:"); Serial.println(stateSteps);
        stateSteps = map(outLEDper, 0, LED_PER_MAX, LED_FADE_ON_STEPS, 0); // recalculate corresponding for FADEIN duration, FADE IN back to ON
        
        Serial.print("MOVE to FADE IN in steps: "); Serial.println(stateSteps);
        break;
      }
      // fade from 100% -> 0%
      else if (stateSteps > 0) // still need to fade out
      {
        stateSteps--;
      }
      else
      {
        // switch to OFF
        ledState = LED_STATE_FAD_OFF;
        stateSteps = LED_FADE_OFF_STEPS;

        if (DBG_STATE)
        {
          Serial.print("MOVE TO FADE OFF: "); Serial.print(timeNow); Serial.print("-"); Serial.println(stateSteps);
        }
      }
      
      outLEDper = LED_PER_IDLE;

        if (DBG_STATE_EXT)
        {
          Serial.print("LED_STATE_IDLE: "); Serial.print(stateSteps); Serial.print(" LED: "); Serial.println(outLEDper);
        }
      
      break;
    }
    

    /********************************** LED FADE OFF **************************/
    case (LED_STATE_FAD_OFF):
    {

      if (motDet)
      {
        ledState = LED_STATE_FAD_ON; // FADE IN back to ON
        Serial.print("switch to FADEIN from FADE OFF:"); Serial.print(stateSteps);
        stateSteps = map(outLEDper, 0, LED_PER_MAX, LED_FADE_ON_STEPS, 0); // recalculate corresponding for FADEIN duration, FADE IN back to ON
        Serial.print(", to FADEOUT:"); Serial.println(stateSteps);
        break;
      }
      // fade from 100% -> 0%
      else if (stateSteps > 0) // still need to fade out
      {
        stateSteps--;
      }
      else
      {
        // switch to OFF
        ledState = LED_STATE_OFF;

        if (DBG_STATE)
        {
          Serial.print("MOVE TO OFF: "); Serial.print(timeNow); Serial.print("-"); Serial.println(stateSteps);
        }    
      }
      
      outLEDper = getLedPer (stateSteps, 0, LED_FADE_OFF_STEPS, 0, LED_PER_IDLE);

      if (DBG_STATE_EXT)
      {
        Serial.print("LED_STATE_FAD_OFF: "); Serial.print(stateSteps); Serial.print(" LED: "); Serial.println(outLEDper);
      }

      break;
    }

       
    default: 
      break;
  }
}







void processSMEye()
{
  if (eyeIdleDeb > 0)
  {
    eyeIdleDeb--;
  }

  switch (eyeState)
  {

    /********************************** Eye OFF *******************************/
    case (LED_STATE_OFF):
    {
      if (motDet && (eyeIdleDeb == 0))
      {
        // switch to FADE IN 0% -> 100%
        eyeState = LED_STATE_FAD_ON;    // MOVE TO FADE IN
        eyeStateSteps = EYE_FADE_ON_STEPS;

        eyeIdleDeb = EYE_IDLE_STEPS;

        if (DBG_STATE_EYE)
        {
          Serial.print("EYE: MOVE TO FADE ON: "); Serial.print(timeNow); Serial.print("-"); Serial.println(eyeStateSteps);
        }
      }
      break;
    }


    /********************************** Eye FADE to ON ***************************/
    case (LED_STATE_FAD_ON):
    {
      // fade from 0% -> 100%
      if (eyeStateSteps > 0) // still need to fade in
      {
        eyeStateSteps--;
        outEyePer = getLedPer ((EYE_FADE_ON_STEPS - eyeStateSteps), 0, EYE_FADE_ON_STEPS, 0, LED_PER_MAX);

        if (DBG_STATE_EXT_EYE)
        {
          Serial.print("EYE: LED_STATE_FAD_ON: "); Serial.print(stateSteps); Serial.print(" LED: "); Serial.println(outEyePer);
        }
      }
      else // fade in done
      {
        eyeState = LED_STATE_ON;  // MOVE TO ON
        eyeStateSteps = EYE_ON_STEPS;

        if (DBG_STATE_EYE)
        {
          Serial.print("EYE: MOVE TO ON TIMEOUT: "); Serial.print(timeNow); Serial.print("-"); Serial.println(eyeStateSteps);
        }
      }

      break;
    }


    /********************************** Eye ON TIMEOUT ************************/
    case (LED_STATE_ON):
    {
      if (eyeStateSteps > 0) // still need to timeout ON
      {
        eyeStateSteps--;

        if (DBG_STATE_EXT_EYE)
        {
          Serial.print("EYE: LED_STATE_ON: "); Serial.print(stateSteps); Serial.print(" LED: "); Serial.println(outEyePer);
        }
      }
      else
      {
        eyeState = LED_STATE_FAD_OFF;   // MOVE TO FADE OUT
        eyeStateSteps = EYE_FADE_OFF_STEPS;

        if (DBG_STATE_EYE)
        {
          Serial.print("EYE: MOVE TO FADE OFF: "); Serial.print(timeNow); Serial.print("-"); Serial.println(eyeStateSteps);
        }
      }

      break;
    }


    /********************************** LED FADE OFF **************************/
    case (LED_STATE_FAD_OFF):
    {

      // fade from 100% -> 0%
      if (eyeStateSteps > 0) // still need to fade out
      {
        eyeStateSteps--;
      }
      else
      {
        // switch to OFF
        eyeState = LED_STATE_OFF;

        if (DBG_STATE_EYE)
        {
          Serial.print("EYE: MOVE TO OFF: "); Serial.print(timeNow); Serial.print("-"); Serial.println(eyeStateSteps);
        }
      }

      outEyePer = getLedPer (eyeStateSteps, 0, EYE_FADE_OFF_STEPS, 0, LED_PER_IDLE);

      if (DBG_STATE_EXT_EYE)
      {
        Serial.print("EYE: LED_STATE_FAD_OFF: "); Serial.print(eyeStateSteps); Serial.print(" LED: "); Serial.println(outEyePer);
      }

      break;
    }


    default:
      break;
  }
}





/******************************************************************************/
/********************************** SETUP *************************************/

/******************************************************************************/
void setupLED(void)
{
  analogWriteFreq(200);

  pinMode(PIN_OUT_LED, OUTPUT);
  digitalWrite(PIN_OUT_LED, LOW);

  // LED sensor 2 ultrasonic
  pinMode(PIN_OUT_EYE, OUTPUT);
  digitalWrite(PIN_OUT_EYE, LOW);

  // LED sensor 1 PIR
  pinMode(PIN_OUT_LED_SENS1, OUTPUT);
  digitalWrite(PIN_OUT_LED_SENS1, LOW);
}
  
/******************************************************************************/
void setupLightSensor(void)
{
  pinMode(PIN_IN_LI_SEN, INPUT);
}


/******************************************************************************/
void setupMotDet(void)
{
  // PIR sensor
  pinMode(PIN_IN_MOT_DET_1, INPUT);

  // UltraSonic distance sensor
  pinMode(PIN_US_TRIG, OUTPUT);   // Sets the trigPin as an OUTPUT
  digitalWrite(PIN_US_TRIG, LOW);
  pinMode(PIN_US_ECHO, INPUT);    // Sets the echoPin as an INPUT
}


#if USE_OTA
/******************************************************************************/
void setupOTA(void)
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  ArduinoOTA.setPort(OTA_PORT);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(OTA_HOST);

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  /******************************************************************************/
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
        analogWrite(PIN_OUT_LED, j*8);
        delay(2);
      }

      for (j=1023/8; j>0; j--)
      {
        analogWrite(PIN_OUT_LED, j*8);
        delay(2);
      }

//      digitalWrite(PIN_OUT_LED, OUT_HIGH);
//      delay(100);
//      digitalWrite(PIN_OUT_LED, OUT_LOW);
//      delay(100);
    }
  });

  /******************************************************************************/
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");

    int i;
    int j;
    for (i=0; i<5; i++)
    {
      for (j=0; j<1024/8; j++)
      {
        analogWrite(PIN_OUT_LED, j*8);
        delay(1);
      }

      for (j=1023/8; j>0; j--)
      {
        analogWrite(PIN_OUT_LED, j*8);
        delay(1);
      }

//      digitalWrite(PIN_OUT_LED, OUT_HIGH);
//      delay (100);
//      digitalWrite(PIN_OUT_LED, OUT_LOW);
//      delay(100);
    }
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    int val;
    val = map((progress / (total / 100)), 0, 100, 0, 1023);
    analogWrite(PIN_OUT_LED, val);

    Serial.printf("Progress: %u%%\n", (progress / (total / 100)));
  });

  /******************************************************************************/
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
#endif


/******************************************************************************/
/********************************** UTIL **************************************/


/******************************************************************************/
bool isCycleTime(void)
{ 
  uint16_t dif;

  timeNow = millis();
  dif = timeNow - timeOld;

  if (dif < MAIN_CYCLE)
    return false;
  else
  {
    timeOld = timeNow;
    return true;
  }   
}


/******************************************************************************/
uint32_t getLedPer(uint16_t val, uint16_t inL, uint16_t inH, uint16_t outL, uint16_t outH)
{
  uint32_t per;
  
  per = map(val, inL, inH, outL, outH);
  
#if 0 //DBG_LOG
  Serial.print("        getLedPer: ("); Serial.print(val); Serial.print(")#(");
  Serial.print(inL);Serial.print(":"); Serial.print(inH); Serial.print(")#(");
  Serial.print(outL);Serial.print(":"); Serial.print(outH); Serial.print(")#");
  Serial.println();
#endif

  return per;
}

/******************************************************************************/
/********************************** OUTPUTS ***********************************/


/******************************************************************************/
void writeLED()
{
  uint32_t val;

  //val = (LED_PWM_MAX * outLEDper) / PER_MAX;
  val = map(outLEDper, 0, PER_MAX, 0, LED_PWM_MAX);

  outLED = val;
#if !OUTPUT_NEG
#else
  outLED = LED_PWM_MAX - outLED;
#endif

  analogWrite(PIN_OUT_LED, outLED);

  if (DBG_PLOT_OUT_LED)
  {
    Serial.print(" OUTper:"); Serial.print(outLEDper); Serial.print(" LEDper:"); Serial.print(outLEDper*100/PER_MAX); Serial.print(", PWMout:"); Serial.println(outLED);
  }


  //val = (LED_PWM_MAX * outLEDper) / PER_MAX;
  val = map(outEyePer, 0, PER_MAX, 0, LED_PWM_MAX);

  outEye = val;
#if !OUTPUT_EYE_NEG
#else
  outEye = LED_PWM_MAX - outEye;
#endif

  analogWrite(PIN_OUT_EYE, outEye);

  if (DBG_PLOT_OUT_LED)
  {
    Serial.print(" Eye OUTper:"); Serial.print(outEyePer); Serial.print(" EyePer:"); Serial.print(outEyePer*100/PER_MAX); Serial.print(", PWMout:"); Serial.println(outEye);
  }
}


/******************************************************************************/
/********************************** INPUTS ************************************/


/******************************************************************************/
void readLightSensor()
{

}


/******************************************************************************/
void readMotDet()
{
  motDet1 = false; // PIR
//  motDet2 = false; // change on debounce TODO

#if USE_MOT_DET_1
  if (HIGH == digitalRead(PIN_IN_MOT_DET_1))
  {
    motDet1 = true;
  }
#endif
  digitalWrite(PIN_OUT_LED_SENS1, motDet1);


#if USE_MOT_DET_2
#if 0//DBG_LOG
  Serial.print("Pulse timeout:"); Serial.println(US_PULSE_TOUT);
#endif

  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(PIN_US_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_US_TRIG, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  usPulseDur = pulseIn(PIN_US_ECHO, HIGH, US_PULSE_TOUT);

  if (usPulseDur > 0)
  {
    // Calculating the distance
    usDist = usPulseDur * US_SPEED / 2; // Speed of sound wave divided by 2 (go and back)

    if (usDist < US_DET_DIST)
    {
      if (usMotCnt < US_DEB_CNT) // do debouncing
      {
        usMotCnt++;
      }

      if (usMotCnt == US_DEB_CNT)
      {
        motDet2 = true;
#if 0 //DBG_LOG
      Serial.println("Mot det dist = ON");
#endif
      }
    }
    else
    {
      if (usMotCnt > 0) // do debouncing
      {
        usMotCnt--;
      }

      if (usMotCnt == 0)
      {
        motDet2 = false;
#if 0 //DBG_LOG
      Serial.println("Mot det dist = OFF");
#endif
      }
    }
  }

  if (DBG_PLOT_IN_DIST)
  {
    Serial.print("Dist(cm):"); Serial.print(usDist); Serial.println(" ");
  }
#endif // USE_MOT_DET_2


  if (DBG_IN_MOT)
  {
    char ch;
    if (Serial.available() > 0)
      ch = Serial.read();

    if (ch == '1')
    {
      Serial.println("Mot det 1");
      motDet1 = true;
    }

    if (ch == '2')
    {
      Serial.println("Mot det 2");
      motDet2 = true;
    }
  }

  if (motDet1 || motDet2)
  {
#if 0 //DBG_LOG
    Serial.println("Mot det");
#endif

    motDet = true;
  }
  else
  {
    motDet = false;
  }
}
