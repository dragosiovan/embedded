#include "bcm_ext.h"


/* locals */

/* inputs digital */

static digIn_t inAcc    = {DIG_VAL_LOW, DIG_VAL_LOW, DIG_STATE_LOW};
static digIn_t inPosL   = {DIG_VAL_LOW, DIG_VAL_LOW, DIG_STATE_LOW};
static digIn_t inLowB   = {DIG_VAL_LOW, DIG_VAL_LOW, DIG_STATE_LOW};
static digIn_t inHighB  = {DIG_VAL_LOW, DIG_VAL_LOW, DIG_STATE_LOW};
static digIn_t inTurnL  = {DIG_VAL_LOW, DIG_VAL_LOW, DIG_STATE_LOW};
static digIn_t inTurnR  = {DIG_VAL_LOW, DIG_VAL_LOW, DIG_STATE_LOW};
static digIn_t inRev    = {DIG_VAL_LOW, DIG_VAL_LOW, DIG_STATE_LOW};

/* inputs analog */

static batVol bat = 0;

/* internals */

static tickLed_t tickLed = {DIG_VAL_LOW, 0};
static engine_t engine = {ENG_ACC_OFF};
static drl_t drl = {STATE_OFF, STATE_OFF, 0};
static turn_t turn = {TURN_OFF, TURN_SIG_OFF, 0};
static corn_t corn = {CORN_STATE_OFF, 0, 0, 0, 0, CORN_PWM_MIN, CORN_PWM_MIN};

/* forward declarations */

static void bcm_ext_init_pins (void);
static void bcm_ext_init_inputs (void);
static void bcm_ext_read_inputs (void);
static void bcm_ext_read_input_digital (digIn_t * digIn, uint8_t pin);
static void bcm_ext_write_outputs (void);
static void bcm_ext_write_outputs_tickLed (void);
static void bcm_ext_write_outputs_drl (void);
static void bcm_ext_write_outputs_corn (void);
static void bcm_ext_stateMachine (void);
static void bcm_ext_stateMachine_tickLed (void);
static void bcm_ext_stateMachine_engine (void);
static void bcm_ext_stateMachine_turnLight (void);
static void bcm_ext_stateMachine_drl (void);
static void bcm_ext_stateMachine_corn (void);


/*************************************************************
* bcm_ext_init
*************************************************************/

void bcm_ext_init (void)
{
  bcm_ext_init_pins ();
}


/*************************************************************
* bcm_ext_main
*************************************************************/

void bcm_ext_main (void)
{
  bcm_ext_read_inputs ();
  bcm_ext_stateMachine ();
  bcm_ext_write_outputs ();
}


/*************************************************************
* bcm_ext_init_pins
*************************************************************/

static void bcm_ext_init_pins (void)
{
  /* input digital */

  pinMode (PIN_IN_ACC, INPUT);
  pinMode (PIN_IN_POSL, INPUT);
  pinMode (PIN_IN_LOW_BEAM, INPUT);
  pinMode (PIN_IN_HIGH_BEAM, INPUT);
  pinMode (PIN_IN_TURN_L, INPUT);
  pinMode (PIN_IN_TURN_R, INPUT);
  pinMode (PIN_IN_REV, INPUT);

  /* input analog */

  pinMode (PIN_IN_BAT_VOL, INPUT);

  /* output digital */

  pinMode (PIN_OUT_TICK_LED, OUTPUT);

  /* output PWM */

  pinMode (PIN_OUT_DRL, OUTPUT);
  pinMode (PIN_OUT_CORN_L, OUTPUT);
  pinMode (PIN_OUT_CORN_R, OUTPUT);
}

/*************************************************************
* bcm_ext_init_pins
*************************************************************/

static void bcm_ext_init_inputs (void)
{
  /* input digital */

  /* input analog */

  /* output digital */

  /* output PWM */

}

/*************************************************************
* bcm_ext_read_input
*************************************************************/

static void bcm_ext_read_inputs (void)
{
  bcm_ext_read_input_digital (&inAcc,   PIN_IN_ACC);
  bcm_ext_read_input_digital (&inPosL,  PIN_IN_POSL);
  bcm_ext_read_input_digital (&inLowB,  PIN_IN_LOW_BEAM);
  bcm_ext_read_input_digital (&inHighB, PIN_IN_HIGH_BEAM);
  bcm_ext_read_input_digital (&inTurnL, PIN_IN_TURN_L);
  bcm_ext_read_input_digital (&inTurnR, PIN_IN_TURN_R);
  bcm_ext_read_input_digital (&inRev,   PIN_IN_REV);
}


/*************************************************************
* bcm_ext_read_input_digital
*************************************************************/

static void bcm_ext_read_input_digital (digIn_t * digIn, uint8_t pin)
{
  digIn->old = digIn->now;
  digIn->now = digitalRead (pin);

  if ((digIn->old == DIG_VAL_LOW) && 
      (digIn->now == DIG_VAL_HIGH))
  {
    digIn->state = DIG_STATE_RISING;
  }
  else if ((digIn->old == DIG_VAL_HIGH) && 
           (digIn->now == DIG_VAL_LOW))
  {
    digIn->state = DIG_STATE_FALLING;
  }
  else
  {
    digIn->state = (digState_e) digIn->now;
  }
}


/*************************************************************
* bcm_ext_write_outputs
*************************************************************/

static void bcm_ext_write_outputs (void)
{
  bcm_ext_write_outputs_tickLed ();
  bcm_ext_write_outputs_drl ();
  bcm_ext_write_outputs_corn (); 
}


/*************************************************************
* bcm_ext_write_outputs_tickLed
*************************************************************/

static void bcm_ext_write_outputs_tickLed (void)
{
  digitalWrite (PIN_OUT_TICK_LED, tickLed.out);
}


/*************************************************************
* bcm_ext_write_outputs_drl
*************************************************************/

static void bcm_ext_write_outputs_drl (void)
{
  analogWrite (PIN_OUT_DRL, drl.out);
}


/*************************************************************
* bcm_ext_write_outputs_corn
*************************************************************/

static void bcm_ext_write_outputs_corn (void)
{
  analogWrite (PIN_OUT_CORN_L, corn.outL);
  analogWrite (PIN_OUT_CORN_R, corn.outR);
}


/*************************************************************
* bcm_ext_stateMachine
*************************************************************/

static void bcm_ext_stateMachine (void)
{
  bcm_ext_stateMachine_tickLed ();
  bcm_ext_stateMachine_engine ();
  bcm_ext_stateMachine_turnLight ();
  bcm_ext_stateMachine_drl ();
  bcm_ext_stateMachine_corn ();
} 


/*************************************************************
* bcm_ext_stateMachine_tickLed
*************************************************************/

static void bcm_ext_stateMachine_tickLed (void)
{
  if (tickLed.cnt == 0)
  {
    if (tickLed.out == DIG_VAL_LOW)
    {
      tickLed.cnt = TICK_CNT_ON;
      tickLed.out = DIG_VAL_HIGH;
    }
    else
    {
      tickLed.cnt = TICK_CNT_OFF;
      tickLed.out = DIG_VAL_LOW;
    }
  }
  else
  {
    tickLed.cnt--;
  }
}

/*************************************************************
* bcm_ext_stateMachine_engine
*************************************************************/

static void bcm_ext_stateMachine_engine (void)
{
  switch (engine.state)
  {
    case (ENG_NA):
    case (ENG_ACC_OFF):
    {
      if (inAcc.state == DIG_STATE_RISING)
      {
        engine.state = ENG_ACC_ON;
      }
      break;
    }
    case (ENG_ACC_ON):
    {
      if (bat > BAT_VOL_CHARGE_THR)
      {
        engine.state = ENG_RUNNING;
      }
    }
    case (ENG_RUNNING):
    {
      if (inAcc.state == DIG_STATE_FALLING)
      {
        engine.state = ENG_ACC_OFF;
      }
      break;
    }
    default:
      break;
  }
}


/*************************************************************
* bcm_ext_stateMachine_turnLight
*************************************************************/

static void bcm_ext_stateMachine_turnLight (void)
{
  turnState_e now;

  /* current signal state */
  
  if ((inTurnL.now == DIG_VAL_HIGH) &&    /* warning light */
      (inTurnR.now == DIG_VAL_HIGH))
  {
    now = TURN_WARN;
  }
  else if ((inTurnL.now == DIG_VAL_HIGH) &&    /* only turn left light: ON HIGH */
           (inTurnR.now == DIG_VAL_LOW))
  {
    now = TURN_LEFT;
  }
  else if ((inTurnL.now == DIG_VAL_LOW) &&    /* only turn right light: ON HIGH */
           (inTurnR.now == DIG_VAL_HIGH))
  {
    now = TURN_RIGHT;
  }
  else
  {
    now = TURN_OFF;
  }

  /* check if active low */
  
  if (now != TURN_OFF)          /* signal is ON HIGH */
  {
    turn.state = now;
    turn.cnt = TURN_ON_LOW_CNT;   /* be prepared to count down active low period */
    turn.sig = TURN_SIG_ON_HIGH;
  }
  else  /* now is TURN_OFF */
  {
    /* detect transition from ON HIGH -> ON LOW */

    if (turn.cnt > 0)           /* still on active low */
    {
      turn.sig = TURN_SIG_ON_LOW;
      turn.cnt--;
    }

    if (turn.cnt == 0)          /* switch to OFF */
    {
      turn.sig = TURN_SIG_OFF;
      turn.state = TURN_OFF;
    }
  }
}

/*************************************************************
* bcm_ext_stateMachine_drl
*************************************************************/

static void bcm_ext_stateMachine_drl (void)
{
  /* use drl (high beam) at very low level for pos light */
  
  if (inPosL.now == DIG_VAL_HIGH)         /* position light ON */
  {
    drl.pos = STATE_ON;
  }
  else 
  {
    drl.pos = STATE_OFF;
  }

  /* use drl (high beam) at low level for day running light */
  
  if ((engine.state == ENG_RUNNING) &&    /* motor running */
      (inPosL.now == DIG_VAL_LOW))        /* position light OFF */
  {
    drl.drl = STATE_ON;
  }
  else
  {
    drl.drl = STATE_OFF;
  }

  /* calculate PWM output value */

  if (drl.drl == STATE_ON)
  {
    drl.out = DRL_VAL (DRL_VAL_DRL);
  }
  else if (drl.pos == STATE_ON)
  {
    drl.out = DRL_VAL (DRL_VAL_POS);
  }
  else
  {
    drl.out = DRL_PWM_MIN;
  }
}


/*************************************************************
* bcm_ext_stateMachine_corn
*************************************************************/

static void bcm_ext_stateMachine_corn (void)
{
  /* count delayed ON period */
  
  DEC_VAL (corn.cntOnL);
  DEC_VAL (corn.cntOnR);

  /* count fade out period */
  
  DEC_VAL (corn.cntFadeL);
  DEC_VAL (corn.cntFadeR);

  /* active state */
  
  if (inRev.now == DIG_VAL_HIGH)      /* reversing -> activate cornering on both sides */
  {
    corn.cntOnL = CORN_KEEP_CNT;
    corn.cntOnR = CORN_KEEP_CNT;
  }
  
  if (turn.state == TURN_LEFT)        /* turning left -> corner left */
  {
    corn.cntOnL = CORN_KEEP_CNT;      /* prepare delayed ON counter */
  }

  if (turn.state == TURN_RIGHT)       /* turning right -> corner right */
  {
    corn.cntOnR = CORN_KEEP_CNT;      /* prepare delayed ON counter */
  }

  /* switch to fade out */
  
  if (corn.cntOnL == 0)               /* delayed ON time finished */
  {
    corn.cntFadeL = CORN_FADE_CNT;    /* prepare fade out counter */
  }

  if (corn.cntOnR == 0)               /* delayed ON time finished */
  {
    corn.cntFadeR = CORN_FADE_CNT;    /* prepare fade out counter */
  }

  /* calculate output values */

  if (corn.cntOnL != 0)
  {
    corn.outL = CORN_PWM_MAX;
  }  
  else 
  {
    corn.outL = CORN_FADE_VAL (corn.cntFadeL);
  }

  if (corn.cntOnR != 0)
  {
    corn.outR = CORN_PWM_MAX;
  }  
  else 
  {
    corn.outR = CORN_FADE_VAL (corn.cntFadeR);
  }
}
