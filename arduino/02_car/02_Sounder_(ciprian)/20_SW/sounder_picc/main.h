#include <12F675.h>

#FUSES NOWDT                    //No Watch Dog Timer
#FUSES HS                       //High speed Osc (> 4mhz for PCM/PCH) (>10mhz for PCD)
#FUSES NOPUT                    //No Power Up Timer
#FUSES MCLR                     //Master Clear pin enabled
#FUSES NOBROWNOUT               //No brownout reset
#FUSES NOPROTECT                //Code not protected from reading
#FUSES NOCPD                    //No EE protection

#use delay(clock=4000000)
#define LED PIN_A0
#define DELAY 1000



