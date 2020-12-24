#include "bcm_ext.h"


void setup() 
{
  Serial.begin (115200);
  Serial.println ("Initializing...");

  bcm_ext_init ();
}


void loop() 
{
  bcm_ext_main ();
  delay (CYCLIC_DELAY);
}
