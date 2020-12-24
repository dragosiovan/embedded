#include <12F675.h>

//#device adc=8


#FUSES NOWDT                   //No Watch Dog Timer
#FUSES INTRC_IO                //Internal RC Osc, no CLKOUT
#FUSES NOCPD                   //No EE protection
#FUSES NOPROTECT               //Code not protected from reading
#FUSES NOMCLR                  //Master Clear pin used for I/O
#FUSES NOPUT                   //No Power Up Timer
#FUSES NOBROWNOUT              //No brownout reset
//#FUSES BANDGAP_HIGH          
#use delay(clock=4000000)
