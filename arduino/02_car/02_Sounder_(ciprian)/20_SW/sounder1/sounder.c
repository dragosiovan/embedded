
#include "main.h"
#include "sounder.h"
#include "inm.h"


#define BUZ_CNT_KEY  
#define BUZ_CNT_LIGHT


int buzMode = 0;
int buzCnt = 0;
int buzState = 0;


typedef enum buz_mode_e
{
   BUZ_MODE_OFF = 0,
   BUZ_MODE_ON_KEY = 1,
   BUZ_MODE_ON_LIGHT = 2
};

void buzzer()
{
   switch (buzMode)
   {
      case BUZ_MODE_OFF:
         {
         }
         break;
      case BUZ_MODE_ON_KEY:
         {
         }
         break;
      case BUZ_MODE_ON_LIGHT:
         {
         }
         break;
      default:
         break;
   }
}


/*
@Algorithm:
buzMode shall be set KEY if all cond (AND)
      DOOR == OPEN
   && KEY_IN == ON
   && KL15 == OFF
   
buzMode shall be set to LIGHT if all conds (AND)
      DOOR == OPEN
   && KEY_IN == OFF
   && POS_LIGHT == ON
 */
void mainSounder()
{
   if (   (getDoorState() == DOOR_OPEN)
       && (getKeyInState() == ON)
       && (getKl15State() == OFF)
      )
   {
      buzMode = BUZ_MODE_ON_KEY;
   }
   else if (   (getDoorState() == DOOR_OPEN)
            && (getKeyInState() == OFF)
            && (getPosLightState() == ON)
           )
   {
      buzMode = BUZ_MODE_ON_LIGHT;
   }
   else
   {
      buzMode = BUZ_MODE_OFF;
   }
      output_bit(PIN_A0, 1);

}
