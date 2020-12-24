


#include <main.h>
#include <inm.h>





int getDoorState(void)
{
   return OFF;
} /* getDoorState(void) */


int getKeyInState(void)
{
   return OFF;
} /* getKeyInState(void) */


int getKl15State(void)
{
   return OFF;
} /* getKl15State(void) */


int getPosLightState(void)
{
   int state;
   
   if (input(PIN_A0) == HIGH)
   {
      state = ON;
   }
   else
   {
      state = OFF;
   }
   return state;   
} /* getPosLightState() */
