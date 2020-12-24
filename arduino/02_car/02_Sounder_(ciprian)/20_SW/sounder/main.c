

#include <main.h>
#include <types.h>

#include <inm.h>
#include <outc.h>
#include <sounder.h>

#include <inm.c>
#include <outc.c>
#include <sounder.c>


/*******************************************************************************
 * GLOBAL VARS - START  */

char tmrAlarm = 0;
char tmrEv = 0;


/* GLOBAL VARS - START
 ******************************************************************************/



/*******************************************************************************
 * INTERRUPTS - START */

#if 1
#int_RTCC
void  RTCC_isr(void) 
{
   tmrAlarm++;
   if (tmrAlarm == 5)
   {
      tmrAlarm = 0;
      tmrEv = 1;
   }
}
#endif

#if 0
#int_EXT
void  EXT_isr(void) 
{

}
#endif

#if 0
#int_RA
void  RA_isr(void) 
{

}

#endif
/* INTERRUPTS - END
 ******************************************************************************/





void init()
{
   set_tris_a(0b11011111); // set GP1 output, all other inputs
   
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_4|RTCC_8_bit);    /* 1.0 ms overflow */
   //setup_comparator(NC_NC);

   enable_interrupts(INT_RTCC);
   enable_interrupts(GLOBAL);
} /* init() */


/*******************************************************************************
 * MAIN - START */


void main()
{
   init();
   
   while(true)
   {
      while (tmrEv == 0); /* wait 5ms to pass */
      
      tmrEv = 0;
      mainSounder();

   }
} /* main() */

/* MAIN - END
 ******************************************************************************/


