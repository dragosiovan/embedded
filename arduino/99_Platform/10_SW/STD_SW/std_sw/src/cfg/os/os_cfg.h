/*
 * FILE DESCRIPTION
 */

#if (!defined OS_CFG_H)
#define OS_CFG_H


/*******************************************************************************
 * INCLUDE
 ******************************************************************************/

#include "..\..\std\std_type.h"
#include "..\..\std\os\os_type.h"

/*******************************************************************************
 * DEFINE
 ******************************************************************************/

/* Start of OS configuration parameters */

#define OS_EVENT_NO             (2)
#define OS_TIME_RESOLUTION      (1)     /* time resolution (ms) */

#define OS_EVENT_ID(name)       OS_EVENT_ID_##name
#define OS_EVENT_ID_DEMO        (0)
#define OS_EVENT_ID_CYCLIC      (1)
#define OS_EVENT_MASK(name)     OS_EVENT_MASK_##name
#define OS_EVENT_MASK_DEMO      (1 << OS_EVENT_ID_DEMO)
#define OS_EVENT_MASK_CYCLIC    (1 << OS_EVENT_ID_CYCLIC)

#define OS_EVENT_PER_DEMO       (500)   /* event period (ms) */
#define OS_EVENT_PER_CYCLIC     (10)    /* event period (ms) */

#define OS_EVENT_CNT(name)      OS_EVENT_CNT_##name
#define OS_EVENT_CNT_DEMO       (OS_EVENT_PER_DEMO / OS_TIME_RESOLUTION)    /* event counter */
#define OS_EVENT_CNT_CYCLIC     (OS_EVENT_PER_CYCLIC / OS_TIME_RESOLUTION)    /* event counter */

/* End of OS configuration parameters */

/*******************************************************************************
 * TYPE
 ******************************************************************************/


/*******************************************************************************
 * VAR
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION
 ******************************************************************************/

#endif /* OS_CFG_H */
