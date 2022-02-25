/*
 * LED sensor controller
 */

#define CURRENT_DEVICE    1

#if (CURRENT_DEVICE == 1)
#define OTA_HOST            "LED_SZ_01"
#else
#error "Current device ID not defined!!!"
#endif




#define OTA_PORT            8080

#define PWM_FREQ            200       // (hz)

#define LED_PER_MAX_100     100       // %
#define LED_PER_IDLE_100    10        // %


#define LED_FADE_ON_TIME    1000      // (ms) fade to ON time
#define LED_ON_TIME         10000     // (ms) ON time
#define LED_FADE_IDLE_TIME  3000      // (ms) fade to IDLE time
#define LED_IDLE_TIME       5000      // (ms) idle time
#define LED_FADE_OFF_TIME   1000      // (ms) fade to OFF time


#define EYE_FADE_ON_TIME    300       // (ms)
#define EYE_FADE_IDLE_TIME  0         // (ms)
#define EYE_FADE_OFF_TIME   300       // (ms)
#define EYE_ON_TIME         100       // (ms) ON timeout
#define EYE_IDLE_TIME       5000      // (ms)


#define US_MAX_DIST         200       // max distance (cm)
#define US_DET_DIST         60        // detection distance (cm)
