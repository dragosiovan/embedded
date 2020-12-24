

#include <main.h>
#include <types.h>

//#include <inm.h>
#include <sounder.h>



/*******************************************************************************
 * GLOBAL VARS - START  */

char tmrAlarm = 0;
char tmrEv = 0;


/* GLOBAL VARS - START
 ******************************************************************************/



/*******************************************************************************
 * INTERRUPTS - START */

#if 0
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

#int_EXT
void  EXT_isr(void) 
{

}


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
   setup_comparator(NC_NC);

   enable_interrupts(INT_RTCC);
   enable_interrupts(INT_RA);
   enable_interrupts(GLOBAL);
} /* init() */


/*******************************************************************************
 * MAIN - START */


void main()
{
buzzer();
    while(true)
    {
      while (tmrEv == 0) /* wait 5ms to pass */
      {
         tmrEv = 0;
      }
    }
} /* main() */

/* MAIN - END
 ******************************************************************************/



#if 0
void main()
{

//   setup_adc_ports(AN0);

    //Example blinking LED program
//    while(true){
//      output_low(LED);
//      delay_ms(DELAY);
//      output_high(LED);
//      delay_ms(DELAY);
//    }

}

#endif
