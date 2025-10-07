/*
 * STANDARD TYPES DEFINITION FILE
 */

#if (!defined STD_TYPE_H)
#define STD_TYPE_H

/*******************************************************************************
 * INCLUDE
 ******************************************************************************/

#include "Arduino.h"

/*******************************************************************************
 * DEFINE
 ******************************************************************************/

#define STD_OFF             (0)
#define STD_ON              (1)

#define GLOBAL_CONF_FILE    "C:\work\settings.h"

#define FALSE               (0)
#define TRUE                (1)

#define DBG_INIT          ((DBG_NOT_PLOT) && (dbgVal & DBG_F_INIT))
#define DBG_STATE         ((DBG_NOT_PLOT) && (dbgVal & DBG_F_STATE))
#define DBG_STATE_EXT     ((DBG_NOT_PLOT) && (dbgVal & DBG_F_STATE_EXT))
#define DBG_STATE_EYE     ((DBG_NOT_PLOT) && (dbgVal & DBG_F_STATE_EYE))
#define DBG_STATE_EXT_EYE ((DBG_NOT_PLOT) && (dbgVal & DBG_F_STATE_EXT_EYE))
#define DBG_OUT_LED       ((DBG_NOT_PLOT) && (dbgVal & DBG_F_OUT_LED))
#define DBG_IN_MOT        (dbgVal & DBG_F_IN_MOT)
#define DBG_IN_US         (dbgVal & DBG_F_IN_US)

#define DISABLE_INTERRUPTS()
#define ENSABLE_INTERRUPTS()


//#define LED_PER_MAX_100     100       // %
//#define LED_PER_IDLE_100    10        // %
//
//
//#define LED_FADE_ON_TIME    1000      // (ms) fade to ON time
//#define LED_ON_TIME         10000     // (ms) ON time
//#define LED_FADE_IDLE_TIME  3000      // (ms) fade to IDLE time
//#define LED_IDLE_TIME       5000      // (ms) idle time
//#define LED_FADE_OFF_TIME   1000      // (ms) fade to OFF time
//
//
//#define EYE_FADE_ON_TIME    300       // (ms)
//#define EYE_FADE_IDLE_TIME  0         // (ms)
//#define EYE_FADE_OFF_TIME   300       // (ms)
//#define EYE_ON_TIME         100       // (ms) ON timeout
//#define EYE_IDLE_TIME       5000      // (ms)
//
//
//#define US_MAX_DIST         200       // max distance (cm)
//#define US_DET_DIST         60        // detection distance (cm)


/*******************************************************************************
 * TYPE
 ******************************************************************************/

typedef unsigned int uint32;

//typedef char boolean;

typedef enum
{
    WIFI_ST_NA = 0,
    WIFI_ST_INIT = 1,
    WIFI_ST_CONNECTING = 1,
    WIFI_ST_CONNECTED = 3,
    WIFI_ST_DISCONNECTED = 4,
} WifiState;

/*******************************************************************************
 * VAR
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION
 ******************************************************************************/

#endif /* STD_TYPE_H */
