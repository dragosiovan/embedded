


#define DBG_F_STATE       B00000001
#define DBG_F_STATE_EXT   B00000010
#define DBG_F_OUT_LED     B00000100
uint8_t dbgVal = 0 
| DBG_F_STATE
| DBG_F_STATE_EXT
| DBG_F_OUT_LED
| 0;

#define DBG_STATE         (dbgVal & DBG_F_STATE)
#define DBG_STATE_EXT     (dbgVal & DBG_F_STATE_EXT)
#define DBG_OUT_LED       (dbgVal & DBG_F_OUT_LED)


#define DEBUG1  1
#define DBG_LOG 0

#define OUTPUT_NEG          1

#define USE_MOT_DET_1       0
#define USE_MOT_DET_2       1

#define PIN_IN_MOT_DET_1    10  
#define PIN_IN_MOT_DET_2    10  

#define PIN_US_ECHO         2     // UltraSonic ECHO receive
#define PIN_US_TRIG         3     // UltraSonic Trigger send pulse
#define PIN_IN_LI_SEN       A0
#define PIN_OUT_LED         11


//#define LED_PWM_MAX         1023      // PWM 100%
#define LED_PWM_MAX         255         // PWM 100%
#define PER_MAX             1000

#define LED_PER_MAX_100     50 // %
#define LED_PER_IDLE_100    10 // %

#define LED_PER_MAX         (50*PER_MAX/100)          // /PER %
#define LED_PER_IDLE        (10*PER_MAX/100)          // /PER %




#define MAIN_CYCLE          10        // (ms) check cycle

#define LED_FADE_ON_TIME    1000      // (ms)
#define LED_FADE_ON_STEPS   (LED_FADE_ON_TIME/MAIN_CYCLE)

#define LED_FADE_IDLE_ON_TIME  ((LED_FADE_ON_TIME/LED_PER_MAX_100*(LED_PER_MAX_100-LED_PER_IDLE_100)))
#define LED_FADE_IDLE_ON_STEPS (LED_FADE_IDLE_ON_TIME/MAIN_CYCLE)

#define LED_FADE_IDLE_TIME  2000      // (ms)
#define LED_FADE_IDLE_STEPS (LED_FADE_IDLE_TIME/MAIN_CYCLE)
#define LED_FADE_OFF_TIME   1000      // (ms)
#define LED_FADE_OFF_STEPS  (LED_FADE_OFF_TIME/MAIN_CYCLE)

#define LED_ON_TIME         3*1000 //(30*1000) // (ms) ON timeout
#define LED_ON_STEPS        (LED_ON_TIME/MAIN_CYCLE)
#define LED_IDLE_TIME       3*1000      // (ms)
#define LED_IDLE_STEPS      (LED_IDLE_TIME/MAIN_CYCLE)





#define US_SPEED        0.034   // sound speed (cm/us)
#define US_MAX_DIST     200     // max distance (cm)
#define US_DET_DIST     50 //70      // detection distance (cm)
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
bool motDet1 = false;        // move detection
bool motDet2 = false;        // move detection

uint8_t motSens1Old = LOW;       // previous state of the motion sensor 1
uint8_t motSens2Old = LOW;       // previous state of the motion sensor 2

long usPulseDur; // variable for the duration of sound wave travel
int usDist;      // variable for the distance measurement

/******************************************************************************/
/********************************** SETUP *************************************/

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200); // serial 
  Serial.println("Initializing..."); 
  
  setupLED();
  setupLightSensor();
  setupMotDet();
  
  Serial.println("Initialization done.");
}



/******************************************************************************/
/********************************** MAIN LOOP *********************************/

void loop() 
{
  if (!isCycleTime())
    return;

  // inputs 
	readLightSensor();
	readMotDet();

	// processing
  processSMLED();
  
  // output
  writeLED();
}



/******************************************************************************/
/********************************** INPUTS ************************************/


void readLightSensor()
{
  
}

void readMotDet()
{
  motDet1 = false;
  motDet2 = false;
  
#if USE_MOT_DET_1  
  if (HIGH == digitalRead(PIN_IN_MOT_DET_1))
  {
    motDet1 = true;
  }
#endif
  

#if USE_MOT_DET_2
#if DBG_LOG  
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
      motDet2 = true;
#if DBG_LOG
      Serial.println("Mot det 2");
#endif      
    }
  }
#if DBG_LOG  
  // Displays the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(usDist);
  Serial.println(" cm");
#endif  

#endif // USE_MOT_DET_2


#if DEBUG1
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
#endif
  
  if (motDet1 || motDet2)
  {
#if DBG_LOG    
    Serial.println("Mot det");
#endif

    motDet = true;
  }
  else
  {
    motDet = false;
  }
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
        
//        stateSteps = map(stateSteps, 0, LED_FADE_IDLE_STEPS, LED_FADE_ON_STEPS, 0); // recalculate corresponding for FADEIN duration, FADE IN back to ON
        stateSteps = map(outLEDper, 0, PER_MAX, LED_FADE_ON_STEPS, 0); // recalculate corresponding for FADEIN duration, FADE IN back to ON

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
//        stateSteps = LED_FADE_IDLE_ON_STEPS; // recalculate corresponding for FADEIN duration, FADE IN back to ON
        stateSteps = map(outLEDper, 0, PER_MAX, LED_FADE_ON_STEPS, 0); // recalculate corresponding for FADEIN duration, FADE IN back to ON
        
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
//        stateSteps = map(stateSteps, 0, LED_FADE_IDLE_STEPS, LED_FADE_ON_STEPS, 0); // recalculate corresponding for FADEIN duration, FADE IN back to ON

        stateSteps = map(outLEDper, 0, PER_MAX, LED_FADE_ON_STEPS, 0); // recalculate corresponding for FADEIN duration, FADE IN back to ON

//        stateSteps = LED_FADE_OUT_STEPS - stateSteps;
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


/******************************************************************************/
/********************************** OUTPUTS ***********************************/


/******************************************************************************/
void writeLED()
{
  outLED = ((uint32_t)(LED_PWM_MAX * outLEDper)) / PER_MAX;

#if !OUTPUT_NEG
#else
  outLED = LED_PWM_MAX - outLED;
#endif

  analogWrite(PIN_OUT_LED, outLED);

  if (DBG_OUT_LED)
  {
    Serial.print("++ writeLED: ["); Serial.print(outLEDper*100/PER_MAX); Serial.print("%]["); Serial.print(outLED);Serial.println("]");
  }
}




/******************************************************************************/
/********************************** SETUP *************************************/

/******************************************************************************/
void setupLED(void)
{
  pinMode(PIN_OUT_LED, OUTPUT);
  digitalWrite(PIN_OUT_LED, LOW);
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
//  pinMode(PIN_IN_MOT_DET_2, INPUT);

  // UltraSonic distance sensor
  pinMode(PIN_US_TRIG, OUTPUT);   // Sets the trigPin as an OUTPUT
  digitalWrite(PIN_US_TRIG, LOW);
  pinMode(PIN_US_ECHO, INPUT);    // Sets the echoPin as an INPUT
}



/******************************************************************************/
/********************************** UTIL **************************************/


/******************************************************************************/
bool isCycleTime(void)
{ 
  timeNow = millis();
  
  if (timeNow - timeOld < MAIN_CYCLE)
    return false;
  else
  {
    //  Serial.print("time "); Serial.print(timeNow); Serial.print(" "); Serial.println(timeOld);
    timeOld = timeNow;
    return true;
  }   
}


/******************************************************************************/
uint32_t getLedPer(uint32_t val, uint32_t inL, uint32_t inH, uint32_t outL, uint32_t outH)
{
  uint32_t per;
  
  per = map(val, inL, inH, outL, outH);
  
#if 1 //DBG_LOG
  Serial.print("getLedPer: ("); Serial.print(inL);Serial.print(":"); Serial.print(inH); Serial.print(")-(");
  Serial.print(outL);Serial.print(":"); Serial.print(outH); Serial.print(")-");
  Serial.println(val);
#endif

  return per;
}
