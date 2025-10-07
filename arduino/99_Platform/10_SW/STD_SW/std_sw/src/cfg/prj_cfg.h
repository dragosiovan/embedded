/*
 * FILE DESCRIPTION
 */

#if (!defined APP_CFG_H)
#define APP_CFG_H


/*******************************************************************************
 * INCLUDE
 ******************************************************************************/

#include "..\std\std_type.h"

/*******************************************************************************
 * DEFINE
 ******************************************************************************/

/* DEV_FAMILY - device family might be:
 * - LT - any light source
 * - SW - switch for controlling various events
 * - SN - general sensor
 * - TS - thermostat
 */
#define CFG_PRJ_DEV_FAMILY          "LT"

/* DEV_LOC - device location might be:
 * - BHR - bathroom
 * - BDR - bedroom
 * - LVR - living room
 * - STR - stairs
 * - ATC - attic
 */
#define CFG_PRJ_DEV_LOC             "BDR"

/* DEV_ID - identifies a specific device of similar family */
#define CFG_PRJ_DEV_ID              "01"



#define CFG_PRJ_USE_APP_DEMO        STD_ON
#define CFG_PRJ_PIN_LED_DEMO        LED_BUILTIN

#define CFG_PRJ_USE_GLOBAL_CONF     STD_ON
/* shared by multiple projects. it might contain sensitive data (e.g. WIFI password */
#define CFG_PRJ_GLOBAL_CONF_FILE    "C:\work\settings.h"
/* project specific. be careful with sensitive data, don't commit to public repos */
#define CFG_PRJ_LOCAL_CONF_FILE     "settings.h"

#if CFG_PRJ_USE_GLOBAL_CONF
#include CFG_PRJ_GLOBAL_CONF_FILE
#else
#include CFG_PRJ_LOCAL_CONF_FILE
#endif


#define CFG_PRJ_USE_OTA             STD_ON

#define OTA_PORT                    8080


/* IO configuration */
#define CFG_PRJ_UART_BAUD           115200

#define PWM_FREQ            200       // (hz)


/*******************************************************************************
 * TYPE
 ******************************************************************************/


/*******************************************************************************
 * VAR
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION
 ******************************************************************************/

extern void funct_aa(void);

#endif /* APP_CFG_H */
