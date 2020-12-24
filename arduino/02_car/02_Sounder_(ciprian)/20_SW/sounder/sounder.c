


#define BUZ_CNT_KEY  
#define BUZ_CNT_LIGHT


int buzMode = 0;
int buzModeOld = 0;
int buzState = 0;
int buzCnt = 0;
int buzStep = 0;


typedef enum buz_mode_e
{
   BUZ_MODE_OFF = 0,
   BUZ_MODE_ON_KEY = 1,
   BUZ_MODE_ON_LIGHT = 2,
   BUZ_MODE_ON_BELT = 3
};

#define BUZ_STEPS_KEY   12
#define BUZ_STEPS_POS   18
#define BUZ_STEPS_BELT   12
const int rom buzKeyIn[BUZ_STEPS_KEY] = {10, 20, 10, 150, 10, 20, 10, 150, 10, 20, 10, 150};
const int rom buzPos[BUZ_STEPS_POS] = {10, 21, 10, 21, 10, 100, 10, 21, 10, 21, 10, 100, 10, 21, 10, 21, 10, 100};
const int rom buzBelt[BUZ_STEPS_BELT] = {10, 250, 10, 250, 10, 250, 10, 250, 10, 250, 10, 250};


void buzzer()
{
   if (buzModeOld != buzMode)
   {
      buzStep = 0;
      buzCnt = 0;
      buzState = 0;
   }
   
   switch (buzMode)
   {
      case BUZ_MODE_OFF:
      {
         SET_BUZ_STATE(0);
      }
      break;
      case BUZ_MODE_ON_KEY:
      {
         if (buzCnt == 0)
         {
            if (buzStep < BUZ_STEPS_KEY)
            {
               buzStep++;
               
               buzCnt = buzKeyIn[buzStep-1];
            
               if (buzState == 1)
                  buzState = 0;
               else
                  buzState = 1;
               SET_BUZ_STATE(buzState);
            }
         }
         else
         {
            buzCnt--;
         }
      }
      break;
      case BUZ_MODE_ON_LIGHT:
      {
        if (buzCnt == 0)
         {
            if (buzStep < BUZ_STEPS_POS)
            {
               buzStep++;
               
               buzCnt = buzPos[buzStep-1];
            
               if (buzState == 1)
                  buzState = 0;
               else
                  buzState = 1;
               SET_BUZ_STATE(buzState);
            }
         }
         else
         {
            buzCnt--;
         }
      }
      break;
      case BUZ_MODE_ON_BELT:
      {
        if (buzCnt == 0)
         {
            if (buzStep < BUZ_STEPS_BELT)
            {
               buzStep++;
               
               buzCnt = buzBelt[buzStep-1];
            
               if (buzState == 1)
                  buzState = 0;
               else
                  buzState = 1;
               SET_BUZ_STATE(buzState);
            }
         }
         else
         {
            buzCnt--;
         }
      }
      break;
      default:
         break;
   }
   
   buzModeOld = buzMode;
}


/*
@Algorithm:
buzMode shall be set KEY if all cond (AND)
      DOOR == OPEN
   && KEY_IN == ON
   && KL15 == OFF

buzMode shall be set SEAT_BELY if all cond (AND)
      SEAT_BELT == NOT_SET
   && KL15 == ON
   
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
   else if (   (getSeatBeltState() == OFF)
            && (getKl15State() == ON)
           )
   {
      buzMode = BUZ_MODE_ON_BELT;
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
   
   buzzer();

}
