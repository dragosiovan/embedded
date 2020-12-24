
/*
 * works well for direction, but it counts for multiple times for a sigle click rotation
 */



#define ENC_PIN_A     D6
#define ENC_PIN_B     D7



int p=0, pulses=0, A_SIG=1, B_SIG=1;

void setup(){
  Serial.begin(115200);
  attachInterrupt(ENC_PIN_A, A_FALL, FALLING);
  attachInterrupt(ENC_PIN_B, B_FALL, FALLING);
  Serial.print("pulses:");
  Serial.println(pulses);
}//setup


void loop(){
    if (p != pulses)
    {
      Serial.print("pulses:");
      Serial.println(pulses);
      p = pulses;
    }
}

ICACHE_RAM_ATTR void A_RISE(){
 detachInterrupt(ENC_PIN_A);
 A_SIG=1;
 
 if(B_SIG==0)
 pulses--;//moving forward
 if(B_SIG==1)
 pulses++;//moving reverse
 attachInterrupt(ENC_PIN_A, A_FALL, FALLING);
}

ICACHE_RAM_ATTR void A_FALL(){
  detachInterrupt(ENC_PIN_A);
 A_SIG=0;
 
 if(B_SIG==1)
 pulses--;//moving forward
 if(B_SIG==0)
 pulses++;//moving reverse
 attachInterrupt(ENC_PIN_A, A_RISE, RISING);  
}

ICACHE_RAM_ATTR void B_RISE(){
 detachInterrupt(ENC_PIN_B);
 B_SIG=1;
 
 if(A_SIG==1)
 pulses--;//moving forward
 if(A_SIG==0)
 pulses++;//moving reverse
 attachInterrupt(ENC_PIN_B, B_FALL, FALLING);
}

ICACHE_RAM_ATTR void B_FALL(){
 detachInterrupt(ENC_PIN_B);
 B_SIG=0;
 
 if(A_SIG==0)
 pulses--;//moving forward
 if(A_SIG==1)
 pulses++;//moving reverse
 attachInterrupt(ENC_PIN_B, B_RISE, RISING);
}
