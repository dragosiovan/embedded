




int getDoorState(void)
{
   int state;
   
   if (input(PIN_IN_DOOR) == LOW)
   {
      state = ON;
   }
   else
   {
      state = OFF;
   }
   return state; 
} /* getDoorState(void) */


int getKeyInState(void)
{
   int state;
   
   if (input(PIN_IN_ACC) == HIGH)
   {
      state = ON;
   }
   else
   {
      state = OFF;
   }
   return state; 
} /* getKeyInState(void) */


int getKl15State(void)
{
   int state;
   
   if (input(PIN_IN_KL15) == HIGH)
   {
      state = ON;
   }
   else
   {
      state = OFF;
   }
   return state; 
} /* getKl15State(void) */


int getPosLightState(void)
{
   int state;
   
   if (input(PIN_IN_POS_LIGHT) == HIGH)
   {
      state = ON;
   }
   else
   {
      state = OFF;
   }
   return state;   
} /* getPosLightState() */


int getSeatBeltState(void)
{
   int state;
   
   if (input(PIN_IN_SEAT_BELT) == HIGH)
   {
      state = ON;
   }
   else
   {
      state = OFF;
   }
   return state;   
} /* getSeatBeltState() */
