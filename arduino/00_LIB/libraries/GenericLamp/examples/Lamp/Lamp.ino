#define ENC_USE_MAX_MIN_POS
#include <RotaryEncoderSwitch.h>

#define ENC_PIN_CLK     D6
#define ENC_PIN_DT      D7
#define ENC_PIN_SEL     D0

#define LED_PIN         D4 // GPIO2

#define ENC_MAX_VAL     10

RotaryEncoderSwitch enc(ENC_PIN_CLK, ENC_PIN_DT, ENC_PIN_SEL);

void inline tmrHnd (void);

/* setup
 *
 */

void setup()
{
    Serial.begin(115200);
    Serial.println("RotaryEncoderSwitch library example.");

    pinMode (LED_PIN, OUTPUT);

    enc.setMinMaxPos(0, ENC_MAX_VAL);

    noInterrupts();

    timer0_isr_init();
    timer0_attachInterrupt(tmrHnd);
    timer0_write(ESP.getCycleCount() + 80000);  /* 1 ms */

    pinMode(ENC_PIN_CLK,INPUT_PULLUP);
    pinMode(ENC_PIN_DT,INPUT_PULLUP);
    pinMode(ENC_PIN_SEL,INPUT_PULLUP);

    attachInterrupt (ENC_PIN_CLK, intHndClk, CHANGE);
    attachInterrupt (ENC_PIN_DT, intHndDt, CHANGE);
    attachInterrupt (ENC_PIN_SEL, intHndSel, CHANGE);

    interrupts();
}

/* intHndClk
 *
 */

void intHndClk (void)
{
    enc.clkEdgeInt();
}

/* intHndDt
 *
 */

void intHndDt (void)
{
    enc.dtEdgeInt();
}

/* intHndSel
 *
 */

void intHndSel (void)
{
    enc.selEdgeInt();
}

/* tmrHnd
 * cyclick check for rotary encoder changes
 */

void inline tmrHnd (void)
{
    timer0_write(ESP.getCycleCount() + 80000);
    enc.cyclic();
}

/* loop
 *
 */

void loop()
{
    static int pos = -999;
    static int sel = SEL_EV_NONE;
    int led;#define ENC_USE_MAX_MIN_POS
#include <RotaryEncoderSwitch.h>

#define ENC_PIN_CLK     D7
#define ENC_PIN_DT      D6
#define ENC_PIN_SEL     D5

#define LED_PIN         D4 // GPIO2

#define ENC_MAX_VAL     10

RotaryEncoderSwitch enc(ENC_PIN_CLK, ENC_PIN_DT, ENC_PIN_SEL);

uint8_t cnt = 0;
uint8_t cntOld = 0;

void inline tmrHnd (void);

/* setup
 *
 */

void setup()
{
    Serial.begin(115200);
    Serial.println("RotaryEncoderSwitch library example.");

    pinMode (LED_PIN, OUTPUT);

    enc.setMinMaxPos(0, ENC_MAX_VAL);

    noInterrupts();

    timer0_isr_init();
    timer0_attachInterrupt(tmrHnd);
    timer0_write(ESP.getCycleCount() + 80000);  /* 1 ms */

    pinMode(ENC_PIN_CLK,INPUT_PULLUP);
    pinMode(ENC_PIN_DT,INPUT_PULLUP);
    pinMode(ENC_PIN_SEL,INPUT_PULLUP);

    attachInterrupt (ENC_PIN_CLK, intHndClk, CHANGE);
    attachInterrupt (ENC_PIN_DT, intHndDt, CHANGE);
    attachInterrupt (ENC_PIN_SEL, intHndSel, CHANGE);

    interrupts();
}

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

/* tmrHnd
 * cyclick check for rotary encoder changes
 */

ICACHE_RAM_ATTR void inline tmrHnd (void)
{
    timer0_write(ESP.getCycleCount() + 80000);
    cnt++;
    //enc.cyclic();
}

/* loop
 *
 */

void loop()
{
    uint8_t c;
    static int pos = -999;
    static int sel = SEL_EV_NONE;
    int led;
    int newPos;
    int newSel;

    c = cnt;
    if (c != cntOld)
    {
        cntOld = c;
        enc.cyclic();
    }

    newPos = enc.getPosition();
    if (pos != newPos)
    {
        Serial.print(newPos);
        Serial.println();
        pos = newPos;
        led = map (pos, 0, ENC_MAX_VAL, 1024, 0);
        analogWrite(LED_PIN, led);
    }

    newSel = enc.getSelectEvent();
    enc.clearSelectEvent();
    if (sel != newSel)
    {
        sel = newSel;
        if (newSel == SEL_EV_CLICK)
            Serial.println ("Click");
        else if (newSel == SEL_EV_SG_CLICK)
            Serial.println ("Single Click");
        else if (newSel == SEL_EV_DB_CLICK)
            Serial.println ("Double Click");
        else if (newSel == SEL_EV_LONG_PRESS)
            Serial.println ("Long Press");
        else if (newSel == SEL_EV_LLONG_PRESS)
            Serial.println ("Long Long Press");
    }


    //enc.cyclic();

    delay (1);
}


    int newPos;
    int newSel;

    newPos = enc.getPosition();
    if (pos != newPos)
    {
        Serial.print(newPos);
        Serial.println();
        pos = newPos;
        led = map (pos, 0, ENC_MAX_VAL, 1024, 0);
        analogWrite(LED_PIN, led);
    }

    newSel = enc.getSelectEvent();
    enc.clearSelectEvent();
    if (sel != newSel)
    {
        sel = newSel;
        if (newSel == SEL_EV_CLICK)
            Serial.println ("Click");
        else if (newSel == SEL_EV_SG_CLICK)
            Serial.println ("Single Click");
        else if (newSel == SEL_EV_DB_CLICK)
            Serial.println ("Double Click");
        else if (newSel == SEL_EV_LONG_PRESS)
            Serial.println ("Long Press");
        else if (newSel == SEL_EV_LLONG_PRESS)
            Serial.println ("Long Long Press");
    }

    delay (10);
}


