/*
 * FILE DESCRIPTION
 */

/*******************************************************************************
 * INCLUDE
 ******************************************************************************/

#include "os.h"

/*******************************************************************************
 * DEFINE
 ******************************************************************************/


/*******************************************************************************
 * TYPE
 ******************************************************************************/


/*******************************************************************************
 * VAR
 ******************************************************************************/

/* OS Scheduler run time parameter */
OsRun_Type osRun;


/*******************************************************************************
 * FUNCTION
 ******************************************************************************/

/*******************************************************************************
 * Function used to ...
 * Inputs:
 * Outputs:
 * Return:
 * Note:
 ******************************************************************************/
extern void os_Init(void)
{
    uint32 idx;

    /* initialize run-time data */
    osRun.actEv = OS_EVENT_NONE;
    osRun.time = 0;
    for (idx = 0; idx < osConf.evNo; idx++)
    {
        osRun.ev[idx].cnt = 0;
        osRun.ev[idx].en = FALSE;
    }
}


/*******************************************************************************
 * Function used to ...
 * Inputs:
 * Outputs:
 * Return:
 * Note:
 ******************************************************************************/
extern void os_Start(void)
{
    uint32 idx;

    for (idx = 0; idx < osConf.evNo; idx++)
    {
        if (osConf.ev[idx].as)
        {
            os_SetEventState(osConf.ev[idx].id, true);
        }
    }
}


/*******************************************************************************
 * Function used to ...
 * Inputs:
 * Outputs:
 * Return:
 * Note:
 ******************************************************************************/
void os_SetEventState(osEvent_Type ev, boolean state)
{
    uint32 idx;

//    DISABLE_INTERRUPTS();

    /* for all configured events */
    for (idx = 0; idx < osConf.evNo; idx++)
    {
        if (osConf.ev[idx].id == ev)
        {
            osRun.ev[idx].en = state;
        }
    }

//    ENABLE_INTERRUPTS();
}


/*******************************************************************************
 * Function used to ...
 * Inputs:
 * Outputs:
 * Return: TRUE if event occurred, FALSE otherwise
 * Note:
 ******************************************************************************/
boolean os_WaitEvent(osEvent_Type events)
{
    uint32       now;
    uint32       tDif;
    boolean      sched = FALSE;
    osEvent_Type actEvents;

    now = millis();
    tDif = now - osRun.time;

    if (tDif != 0)
    {

    }

//    /* just wait for one of the specified events to be set by OS */
//    do {
//        DISABLE_INTERRUPTS();
//        actEvents = osSchRun.actEv;
//        ENABLE_INTERRUPTS();
//    }
//    while (OS_SCH_EVENT_NONE == (actEvents & events));
//
//    uint32 idx;
//
//    osRun.time++; /* always increment time with a tick */
//
//    /* for all configured events */
//    for (idx = 0; idx < osSchConf.evNo; idx++)
//    {
//        /* event is enabled */
//        if (osSchRun.ev[idx].en)
//        {
//            osSchRun.ev[idx].cnt++;
//            if (osSchRun.ev[idx].cnt == osSchConf.ev[idx].val)
//            {
//                osSchRun.ev[idx].cnt = 0; /* reset counter */
//                OS_SCH_EVENT_SET(osSchRun.actEv, osSchConf.ev[idx].id);
//            }
//        }
//    }
    return TRUE;
}


/*******************************************************************************
 * Function used to ...
 * Inputs:
 * Outputs:
 * Return:
 * Note:
 ******************************************************************************/
void os_GetEvent(osEvent_Type * pEvents)
{
//    DISABLE_INTERRUPTS();
    *pEvents = osRun.actEv;
//    ENABLE_INTERRUPTS();
}


/*******************************************************************************
 * Function used to ...
 * Inputs:
 * Outputs:
 * Return:
 * Note:
 ******************************************************************************/
void os_ClearEvent(osEvent_Type events)
{
//    DISABLE_INTERRUPTS();
    osRun.actEv &= ~events;
//    ENABLE_INTERRUPTS();
}
