/*
 * FILE DESCRIPTION
 */

#if (!defined OS_TYPE_H)
#define OS_TYPE_H

/*******************************************************************************
 * INCLUDE
 ******************************************************************************/

#include "..\std_type.h"
#include "..\..\cfg\os\os_cfg.h"

/*******************************************************************************
 * DEFINE
 ******************************************************************************/


#define OS_EVENT_NONE               (0x0)

#define OS_EVENT_SET(val, pos)      ((val) |= (1 << pos))
#define OS_EVENT_IS_SET(val, ev)    ((val & ev) != OS_EVENT_NONE)


/*******************************************************************************
 * TYPE
 ******************************************************************************/

typedef uint32 osEvent_Type;

/* event configuration parameters */
typedef struct
{
    uint32  id;
    uint32  val;
    boolean as;     /* auto start */
} OsConfEvent_Type;

/* OS Scheduler configuration parameters */
typedef struct
{
    uint32           evNo;
    uint32           timeRes;
    OsConfEvent_Type ev[OS_EVENT_NO];
} OsConf_Type;

/* event running parameters */
typedef struct
{
    boolean en;  /* event enabled */
    uint32  cnt; /* time base counter */
} OsRunEvent_Type;

/* OS Scheduler run parameters */
typedef struct
{
    uint32          time;    /* OS time (os ticks/ms) */
    osEvent_Type    actEv;
    OsRunEvent_Type ev[OS_EVENT_NO];
} OsRun_Type;

/*******************************************************************************
 * VAR
 ******************************************************************************/

extern OsConf_Type osConf;

/*******************************************************************************
 * FUNCTION
 ******************************************************************************/

#endif /* OS_TYPE_H */
