#include <Arduino.h>



/* defines */

#define CYCLIC_DELAY        5       /* cyclic period */

/* input pins: digital */

#define PIN_IN_ACC          0       /* key position in ACC */
#define PIN_IN_POSL         1       /* position light */
#define PIN_IN_LOW_BEAM     2       /* low beam light */
#define PIN_IN_HIGH_BEAM    3       /* high beam light */
#define PIN_IN_TURN_L       4       /* turn left light */
#define PIN_IN_TURN_R       5       /* turn right light */
#define PIN_IN_REV          6       /* reverse light */

/* input pins: analog */

#define PIN_IN_BAT_VOL      A0      /* battery voltage */

/* output pins: digital */

#define PIN_OUT_TICK_LED    13      /* tick led */

/* output pins: PWM */

#define PIN_OUT_DRL         7       /* day running light */
#define PIN_OUT_CORN_L      8       /* cornering left light */
#define PIN_OUT_CORN_R      9       /* cornering right light */

#define DEC_VAL(v)  if (v > 0) v--;

#define TICK_TIME           500     /* ON/OFF time (ms) */
#define TICK_TIME_ON        50      /* ON time (ms) */
#define TICK_TIME_OFF       950     /* OFF time (ms) */
#define TICK_CNT            (TICK_TIME / CYCLIC_DELAY)
#define TICK_CNT_ON         (TICK_TIME_ON / CYCLIC_DELAY)
#define TICK_CNT_OFF        (TICK_TIME_OFF / CYCLIC_DELAY)

/* battery */

#define BAT_VOL_CHARGE_THR  134     /* charing low threshold */

/* daytime running light */

#define DRL_VAL_HB          100     /* (%) level for high beam */
#define DRL_VAL_DRL         20      /* (%) level for DRL */
#define DRL_VAL_POS         5       /* (%) level for POS */
#define DRL_PWM_MAX         255     /* PWM max digital value */
#define DRL_PWM_MIN         0       /* PWM min digital value */
#define DRL_VAL(p)          ((p * DRL_PWM_MAX) / DRL_VAL_HB) /* calculate PWM value based on percentage */

/* turn signal */

#define TURN_ON_LOW_TIME    900     /* active low time (ms) */
#define TURN_ON_LOW_CNT     (TURN_ON_LOW_TIME / CYCLIC_DELAY)

/* cornering */

#define CORN_KEEP_TIME      2000    /* delayed on time (ms) */
#define CORN_FADE_TIME      1500    /* fade out time (ms) */
#define CORN_FADE_CNT       (CORN_FADE_TIME / CYCLIC_DELAY) 
#define CORN_KEEP_CNT       (CORN_KEEP_TIME / CYCLIC_DELAY) 
#define CORN_PWM_MAX        255     /* PWM max digital value */
#define CORN_PWM_MIN        0       /* PWM min digital value */
#define CORN_VAL_MIN        0       /* (%) level min for cornering */
#define CORN_VAL_MAX        100     /* (%) level max for cornering */
#define CORN_VAL(p)         ((p * CORN_PWM_MAX) / CORN_VAL_MAX)
#define CORN_FADE_VAL(c)    ((c * CORN_PWM_MAX) / CORN_FADE_CNT)    /* calculate out value based on counter */

/* typedefs */

typedef uint8_t   batVol;
typedef uint8_t   drlVal;
typedef uint8_t   turnLowDelay;
typedef uint8_t   cornFadeDelay;
typedef uint8_t   cornVal;

typedef enum state
{
  STATE_OFF   = 0,
  STATE_ON    = 1,
  STATE_NA    = 2
} state_t;

typedef enum digVal
{
  DIG_VAL_LOW   = 0,
  DIG_VAL_HIGH  = 1,
  DIG_VAL_NA    = 2
} digVal_e;

typedef enum digState
{
  DIG_STATE_LOW       = 0,
  DIG_STATE_HIGH      = 1,
  DIG_STATE_RISING    = 2,
  DIG_STATE_FALLING   = 3,
  DIG_STATE_NA        = 4
} digState_e;

typedef enum engState
{
  ENG_ACC_OFF         = 0,
  ENG_ACC_ON          = 1,
  ENG_RUNNING         = 2,
  ENG_NA              = 3
} engState_e;

typedef enum turnState
{
  TURN_OFF            = 0,
  TURN_LEFT           = 1,
  TURN_RIGHT          = 2,
  TURN_WARN           = 3,
  TURN_NA             = 4
} turnState_e;

typedef enum turnSignal
{
  TURN_SIG_OFF        = 0,
  TURN_SIG_ON_HIGH    = 1,
  TURN_SIG_ON_LOW     = 2,
  TURN_SIG_NA         = 3
} turnSignal_e;

typedef enum cornState
{
  CORN_STATE_OFF      = 0,
  CORN_STATE_ON       = 1,
  CORN_STATE_FADE_OUT = 2,
  CORN_STATE_NA       = 3
} cornState_e;

typedef struct tickLed
{
  digVal_e  out;
  uint8_t   cnt;
} tickLed_t;

typedef struct digIn
{
  digVal_e   now;
  digVal_e   old;
  digState_e state;
} digIn_t;

typedef struct engine
{
  engState_e  state;
} engine_t;

typedef struct drl
{
  state_t drl;
  state_t pos;
  drlVal  out;
} drl_t;

typedef struct turn
{
  turnState_e   state;  
  turnSignal_e  sig;    /* signal state */
  turnLowDelay  cnt;    /* ON low state counter left */
} turn_t;

typedef struct corn
{
  cornState_e   state;
  cornFadeDelay cntFadeL;   /* fade out counter for left corner */
  cornFadeDelay cntFadeR;   /* fade out counter for right corner */
  cornFadeDelay cntOnL;     /* delayed on counter for left corner */
  cornFadeDelay cntOnR;     /* delayed on counter for right corner */
  cornVal       outL;       /* output left */
  cornVal       outR;       /* output right */
} corn_t;

/* function declaration */

void bcm_ext_init (void);
void bcm_ext_main (void);
