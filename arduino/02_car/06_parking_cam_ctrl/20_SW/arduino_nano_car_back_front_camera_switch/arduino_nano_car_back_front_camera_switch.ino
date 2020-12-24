

/*
 *
 * REV CAM VIDEO   >----
 *                       \__NC
 *                        __  \----> VIDEO OUT
 *                       /  NO|
 * FRONT CAM VIDEO >----      |
 *                            |
 * VIDEO SW RELAY  >----------
 *
 *
 *                         --------
 * REVERSE LIGHT     >----|        |----> STATUS LED FRONT (WHITE)
 *                        |        |
 * SWITCH SELECT     >----|        |----> STATUS LED BACK (RED)
 *                        |        |
 * FRONT BUZ         >----|        |----> VIDEO SW RELAY
 *                        |        |
 *                        |        |----> VIDEO ACT
 *                         --------
 *
 *                 ------                              ------
 *  ---- init --->|      |---- (switch triggered) --->|      |
 *                | AUTO |                            | REAR |
 *                |      |<-- (timeout_det == TRUE) --|      |
 *                 ------                              ------
 *                    ^                                   |
 *                    |                                   |
 *        (switch_trigered) ||                     (switch_trigered)
 *        (timeout_det == TRUE)                           |
 *                    |          -------                  |
 *                     ---------|       |<----------------
 *                              | FRONT |
 *                              |       |
 *                               -------
 *
 *
 *       ----------------------------------------------------------------
 *      |                            AUTO                                |
 *      |                                                                |
 *      |    -------          (f_dist < fdmax) &&             -------    |
 *      |   |       |-------- (auto_front == TRUE) && ------>|       |   |
 * --init-->| AUTO_ |         (rev_act == FALSE)             | AUTO_ |   |
 *      |   | IDLE  |                                        | FRONT |   |
 *      |   |       |<------- (f_dist > fdmax) || ---------->|       |   |
 *      |    -------          (sw_trig == TRUE)               -------    |
 *      |                                                                |
 *      |                                                                |
 *      |                                                                |
 *      |                                                                |
 *      |                                                                |
 *      |                                                                |
 *      |                                                                |
 *      |                                                                |
 *      |                                                                |
 *      |                                                                |
 *      |                                                                |
 *
 *
 *   AUTO_IDLE --sw--> FRONT --sw--> REAR --sw--> AUTO_IDLE
 *                                        --to--> AUTO_IDLE
 *                           --to--> AUTO_IDLE
 *   AUTO_IDLE --rv--> AUTO_REAR --/rv--> AUTO_FRON --to--> AUTO_IDLE
 *                               --sw--> FRONT --sw--> AUTO_REAR
 *                                             --to--> AUTO_REAR
 *
 *
 *  1. std reverse
 *  2. rev + front (ON on request)
 *  3. rev + front (ON always)
 *
 */

#define DEBUG      1
#if (DEBUG)
#define DBG_HDR     Serial.print (millis()); Serial.print (":");
#define DBG(x)      DBG_HDR; Serial.print (x)
#define DBGLN(x)    DBG_HDR; Serial.println (x)
#else
#define DBG
#define DBGLN
#endif

#define PIN_OUT_LED_FRONT       2      // white
#define PIN_OUT_LED_BACK        3      // red
#define PIN_OUT_CAM_POW         4      // camera power
#define PIN_OUT_V_REL           5      // video switch
#define PIN_OUT_V_ACT           6      // video activate (to Headunit)
#define PIN_IN_SW_SEL           7      // switch
#define PIN_IN_REV              8      // reverse active
#define PIN_IN_SW_CFG1          9      // config switch 1
#define PIN_IN_SW_CFG2          10     // config switch 2

#define PIN_IN_SW_ON            LOW
#define PIN_IN_SW_OFF           HIGH

#define PIN_IN_REV_ON           HIGH
#define PIN_IN_REV_OFF          LOW

#define PIN_IN_SW_CFG_ON        LOW
#define PIN_IN_SW_CFG_OFF       HIGH

#define PIN_OUT_RELAY_ON        HIGH
#define PIN_OUT_RELAY_OFF       LOW

#define PIN_OUT_LED_FRONT_ON    HIGH
#define PIN_OUT_LED_FRONT_OFF   LOW

#define PIN_OUT_LED_BACK_ON     HIGH
#define PIN_OUT_LED_BACK_OFF    LOW

#define CYCLIC_PERIOD           50      // period in (ms)

#define TO_SW_FRONT          5000   // front manual activation timeout
#define TO_SW_REAR           5000   // rear manual activation timeout
#define TO_REV_TO_FRONT      5000    // front auto activation from reverse timeout

#define LED_BLINK_PER        500
#define LED_BLINK_TO         (LED_BLINK_PER/2)

typedef enum
{
    SW_STATE_AUTO_IDLE  = 0,   // no video active, relay is OFF
    SW_STATE_AUTO_REAR  = 1,   // no video active, relay is OFF
    SW_STATE_AUTO_FRONT = 2,   // no video active, relay is OFF
    SW_STATE_REAR       = 3,   // reverse light active
    SW_STATE_FRONT      = 4,
    SW_STATE_MAX
} SW_STATE;

typedef enum
{
    SW_CFG_STD_REV   = 0,     // only reverse in standard mode
    SW_CFG_ON_REQ    = 1,     // reverse + front, powered on request
    SW_CFG_ON_ALWAYS = 2      // reverse + front, always powered
} SW_CFG;

uint8_t pinInRev = 0;
uint8_t pinInFrontBuz = 0;

bool ledBlinkOn = false;
uint16_t ledBlinkPer = 0;

SW_CFG swCfg;
bool swCfg1Act = false;     // config switch active
bool swCfg2Act = false;     // config switch active

bool swSelAct = false;      // select switch active
bool swSelActed = false;    // select switch triggered
uint16_t swSelTo = 0;       // selection switch timeout
uint16_t rev2frontTo = 0;       // selection switch timeout

bool swFrontAuto = false;   // auto switch to front camera based on distance
bool revAct = false;        // reverse active (activate by reverse light)
bool revActed = false;      // reverse activated (activated by reverse light)

SW_STATE swState = SW_STATE_AUTO_IDLE;
SW_STATE swStatePrev = SW_STATE_AUTO_IDLE;

uint32_t timeNow = 0;
uint32_t timeOld = 0;

/******************************************************************************
*
* setup - main setup
*
* TODO
*
* RETURNS: N/A
*/

void setup()
    {
    Serial.begin (115200);
    Serial.println ("Parking camera switch");
    setupPins ();
    }

/******************************************************************************
*
* setup - main setup
*
* TODO
*
* RETURNS: N/A
*/

void loop()
    {
    timeNow = millis();
    if ((timeNow - timeOld) < CYCLIC_PERIOD)
        {
        return;
        }
    timeOld = timeNow;

    if (swSelTo > CYCLIC_PERIOD)
        {
        swSelTo -= CYCLIC_PERIOD;
        }
    else
        {
        swSelTo = 0;        // select switch timeout
        }

    if (rev2frontTo > CYCLIC_PERIOD)
        {
        rev2frontTo -= CYCLIC_PERIOD;
        }
    else
        {
        rev2frontTo = 0;
        }

    loopReadInputs ();
    loopState ();
    loopWriteOutputs ();
    }

/******************************************************************************
*
* setup - main setup
*
* TODO
*
* RETURNS: N/A
*/

void setupPins (void)
    {
    pinMode (PIN_OUT_LED_FRONT, OUTPUT);
    pinMode (PIN_OUT_LED_BACK, OUTPUT);
    pinMode (PIN_OUT_V_REL, OUTPUT);
    pinMode (PIN_OUT_V_ACT, OUTPUT);

    pinMode (PIN_IN_SW_SEL, INPUT_PULLUP);
    pinMode (PIN_IN_REV, INPUT);
    pinMode (PIN_IN_SW_CFG1, INPUT_PULLUP);
    pinMode (PIN_IN_SW_CFG2, INPUT_PULLUP);
    }

/******************************************************************************
*
* setup - main setup
*
* TODO
*
* RETURNS: N/A
*/

void loopReadInputs (void)
    {
    uint8_t pin;

    // select switch

    pin = digitalRead (PIN_IN_SW_SEL);
    if ((pin == PIN_IN_SW_ON) && (!swSelAct))    // switch triggered
        {
        swSelActed = true;
        }
    else
        {
        swSelActed = false;
        }

    if (pin == PIN_IN_SW_ON)                  // switch active
        {
        swSelAct = true;
        }
    else
        {
        swSelAct = false;
        }

    // config switch

    pin = digitalRead (PIN_IN_SW_CFG1);
    if (pin == PIN_IN_SW_ON)                  // switch active
        {
        swCfg1Act = true;
        }
    else
        {
        swCfg1Act = false;
        }
    pin = digitalRead (PIN_IN_SW_CFG2);
    if (pin == PIN_IN_SW_ON)                  // switch active
        {
        swCfg2Act = true;
        }
    else
        {
        swCfg2Act = false;
        }

    if (swCfg1Act && swCfg2Act)
        {
        swCfg = SW_CFG_ON_ALWAYS;
        }
    else if (swCfg1Act && !swCfg2Act)
        {
        swCfg = SW_CFG_ON_REQ;
        }
    else
        {
        swCfg = SW_CFG_STD_REV;
        }

    // reverse activation

    pin = digitalRead (PIN_IN_REV);

    if ((pin == PIN_IN_REV_ON) && (!revAct))    /* reverse activated */
        {
        revActed = true;
        }
    else
        {
        revActed = false;
        }

    if (pin == PIN_IN_REV_ON)                   /* reverse active */
        {
        revAct = true;
        }
    else
        {
        revAct = false;
        }
    }

/******************************************************************************
*
* setup - main setup
*
* TODO
*
* RETURNS: N/A
*/

void loopState (void)
    {
    static int cnt = 1;

    if (swCfg == SW_CFG_STD_REV)
        {
        if (cnt != 0)
            {
            DBGLN ("CFG->SW_STATE_AUTO_IDLE");
            cnt = 0;
            }
         swState = SW_STATE_AUTO_IDLE;
        return;
        }
    cnt++;

    switch (swState)
        {
/* AUTO IDLE */
        case (SW_STATE_AUTO_IDLE):
            if (revActed)
                {
                DBGLN ("SW_STATE_AUTO_IDLE->SW_STATE_AUTO_REAR");
                swStatePrev = swState;
                swState = SW_STATE_AUTO_REAR;
                }
            else if (swSelActed) // selection switch trigger
                {
                DBGLN ("SW_STATE_AUTO_IDLE->SW_STATE_REAR");
                swStatePrev = swState;
                swState = SW_STATE_REAR;
                swSelTo = TO_SW_REAR;
                }
            break;

/* AUTO REAR */
        case (SW_STATE_AUTO_REAR):
            if (!revAct)    // reverse inactive
                {
                DBGLN ("SW_STATE_AUTO_REAR->SW_STATE_AUTO_FRONT");
                swStatePrev = swState;
                swState = SW_STATE_AUTO_FRONT;
                rev2frontTo = TO_REV_TO_FRONT;
                }
            else if (swSelActed)     // request to switch -> FRONT
                {
                DBGLN ("SW_STATE_AUTO_REAR->SW_STATE_FRONT");
                swStatePrev = swState;
                swState = SW_STATE_FRONT;
                swSelTo = TO_SW_FRONT;
                }

            break;

/* AUTO FRONT */
        case (SW_STATE_AUTO_FRONT):
            if (revActed ||
                ((rev2frontTo == 0) && revAct))
                {
                DBGLN ("SW_STATE_AUTO_FRONT->SW_STATE_AUTO_REAR");
                swStatePrev = swState;
                swState = SW_STATE_AUTO_REAR;
                }
            else if (swSelActed || (rev2frontTo == 0))
                {
                DBGLN ("SW_STATE_AUTO_FRONT->SW_STATE_AUTO_IDLE");
                swStatePrev = swState;
                swState = SW_STATE_AUTO_IDLE;
                }
            break;

/* REAR */
        case (SW_STATE_REAR):
            if (swSelActed)
                {
                DBGLN ("SW_STATE_REAR->SW_STATE_FRONT");
                swStatePrev = swState;
                swState = SW_STATE_FRONT;
                swSelTo = TO_SW_FRONT;
                }
            else if (swSelTo == 0) // switch select timeout
                {
                if (revAct)     // switch back to auto REAR
                    {
                    DBGLN ("SW_STATE_REAR->SW_STATE_AUTO_REAR");
                    swStatePrev = swState;
                    swState = SW_STATE_AUTO_REAR;
                    }
                else
                    {
                    DBGLN ("SW_STATE_REAR->SW_STATE_AUTO_IDLE");
                    swStatePrev = swState;
                    swState = SW_STATE_AUTO_IDLE;
                    }
                }
            break;

/* FRONT */
        case (SW_STATE_FRONT):
            if (swSelActed)
                {
                DBGLN ("SW_STATE_FRONT->SW_STATE_AUTO_IDLE");
                swStatePrev = swState;
                swState = SW_STATE_AUTO_IDLE;
                }
            else if (swSelAct)       // keep state while button pressed
                {

                }
            else if (swSelTo == 0) // selection switch trigger
                {
                if (revAct)     // switch back to auto REAR
                    {
                    DBGLN ("SW_STATE_FRONT->SW_STATE_AUTO_REAR");
                    swStatePrev = swState;
                    swState = SW_STATE_AUTO_REAR;
                    }
                else
                    {
                    DBGLN ("SW_STATE_FRONT->SW_STATE_AUTO_IDLE");
                    swStatePrev = swState;
                    swState = SW_STATE_AUTO_IDLE;
                    }
                }
            break;

        default:
            break;
        }
    }

/******************************************************************************
*
* setup - main setup
*
* TODO
*
* RETURNS: N/A
*/

void loopWriteOutputs (void)
    {
    // video switch relay

    if ((swState == SW_STATE_FRONT) ||          /* switch relay to N0 */
        (swState == SW_STATE_AUTO_FRONT))
        {
        digitalWrite (PIN_OUT_V_REL, PIN_OUT_RELAY_ON);
        }
    else
        {
        digitalWrite (PIN_OUT_V_REL, PIN_OUT_RELAY_OFF);
        }

    // CAM power

    if (swCfg == SW_CFG_ON_ALWAYS)
        {
        digitalWrite (PIN_OUT_CAM_POW, PIN_OUT_RELAY_ON);
        }
    else if ((swCfg == SW_CFG_ON_REQ) && (swState != SW_STATE_AUTO_IDLE))
        {
        digitalWrite (PIN_OUT_CAM_POW, PIN_OUT_RELAY_ON);
        }
    else
        {
        digitalWrite (PIN_OUT_CAM_POW, PIN_OUT_RELAY_OFF);
        }

    // video activation

    if (swState != SW_STATE_AUTO_IDLE)
        {
        digitalWrite (PIN_OUT_V_REL, PIN_OUT_RELAY_ON);
        }
    else
        {
        digitalWrite (PIN_OUT_V_REL, PIN_OUT_RELAY_OFF);
        }

    // LED activation

    switch (swState)
        {
        case (SW_STATE_AUTO_IDLE):      // status LEDs OFF
            if (swCfg == SW_CFG_ON_ALWAYS)
            {
              
            }
            else if (swCfg == SW_CFG_ON_REQ)
            {
        
            }
            else if (swCfg = SW_CFG_STD_REV)
            {
              
            }
            digitalWrite (PIN_OUT_LED_BACK, PIN_OUT_LED_BACK_OFF);
            digitalWrite (PIN_OUT_LED_FRONT, PIN_OUT_LED_FRONT_OFF);
            break;

        case (SW_STATE_AUTO_REAR):           // static RED status LED
            digitalWrite (PIN_OUT_LED_BACK, PIN_OUT_LED_BACK_ON);
            digitalWrite (PIN_OUT_LED_FRONT, PIN_OUT_LED_FRONT_OFF);
            break;

        case (SW_STATE_AUTO_FRONT):          // static WHITE status LED
            digitalWrite (PIN_OUT_LED_BACK, PIN_OUT_LED_BACK_OFF);
            digitalWrite (PIN_OUT_LED_FRONT, PIN_OUT_LED_FRONT_ON);
            break;

        case (SW_STATE_REAR):           // blinking RED status LED
            digitalWrite (PIN_OUT_LED_FRONT, PIN_OUT_LED_FRONT_OFF);
            if (ledBlinkPer == 0)
                {
                ledBlinkPer = LED_BLINK_TO;
                ledBlinkOn = !ledBlinkOn;
                }
            digitalWrite (PIN_OUT_LED_BACK, ledBlinkOn);
            break;

        case (SW_STATE_FRONT):          // blinking WHITE status LED
            digitalWrite (PIN_OUT_LED_BACK, PIN_OUT_LED_BACK_OFF);
            if (ledBlinkPer == 0)
                {
                ledBlinkPer = LED_BLINK_TO;
                ledBlinkOn = !ledBlinkOn;
                }
            digitalWrite (PIN_OUT_LED_FRONT, ledBlinkOn);
            break;

        default:
            ledBlinkPer = 0;
            ledBlinkOn = false;
            break;
        }

    if (ledBlinkPer > CYCLIC_PERIOD)
        {
        ledBlinkPer -= CYCLIC_PERIOD;
        }
    else
        {
        ledBlinkPer = 0;
        }
    }
