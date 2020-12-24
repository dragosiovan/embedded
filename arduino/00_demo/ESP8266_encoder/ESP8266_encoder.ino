#include <sys/time.h>                   // struct timeval

#define ENC_CLK_PIN D6
#define ENC_DT_PIN  D7
#define ENC_SW_PIN  D2


volatile long encoderValue = 0;
long lastencoderValue = -1;


int sw = 0;
int lastSw = 0;

int clk_valid = HIGH;
int dt_valid = HIGH;
int clk_last = HIGH;
int dt_last = HIGH;
int sw_valid = HIGH;
int tmrClk = 0;
int tmrDt = 0;
int tmrSw = 0;

/* 
 *  Cn Dn Co Do     (Clock and Data new/old)
 *  1  0  0  0      increment
 *  0  1  0  0      decrement
 *  
 *  
 *  
 *  
 */


bool toggle = false;


void inline tmrHnd (void);

void setup() 
{
  Serial.begin (115200);
  delay (10);  

  pinMode (ENC_CLK_PIN, INPUT);
  pinMode (ENC_DT_PIN, INPUT);
  pinMode (ENC_SW_PIN, INPUT);

  pinMode (D0, OUTPUT);
  pinMode (D1, OUTPUT);
  pinMode (D2, OUTPUT);
  digitalWrite (D0, HIGH);
  digitalWrite (D1, HIGH);
  digitalWrite (D2, HIGH);

  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(tmrHnd);
  timer0_write(ESP.getCycleCount() + 80000);  /* 1 ms */
  interrupts();
  

  attachInterrupt (ENC_CLK_PIN, intHndClk, CHANGE);
  attachInterrupt (ENC_DT_PIN, intHndDt, CHANGE);
  attachInterrupt (ENC_SW_PIN, intHndSw, CHANGE);
}



void inline tmrHnd (void)
{
  timer0_write(ESP.getCycleCount() + 80000);
  
  if (tmrClk > 0)  
  {
    tmrClk--;
  }
  else
  {
    clk_valid = digitalRead (ENC_CLK_PIN);
  }

  if (tmrDt > 0)
  {
    tmrDt--;
  }
  else
  {
    dt_valid = digitalRead (ENC_DT_PIN);
  }

  if (tmrSw > 0)  
  {
    tmrSw--;
  }
  else
  {
    sw_valid = digitalRead (ENC_SW_PIN);
  }


  if (clk_last == LOW && 
      clk_valid == HIGH && 
      dt_valid == LOW)
  {
    digitalWrite (D0, 0);
    digitalWrite (D0, 1);
    encoderValue--;
  }
  else if (dt_last == LOW &&
           dt_valid == HIGH &&
           clk_valid == LOW)
  {
    digitalWrite (D1, 0);
    digitalWrite (D1, 1);
    encoderValue++;
  }
  clk_last = clk_valid;
  dt_last = dt_valid;
}

void loop() 
{
  if (lastencoderValue != encoderValue)
  {
    Serial.print ("enc: ");
    Serial.println (encoderValue);
    lastencoderValue = encoderValue;
  }

  delay (10);
}




void intHndClk ()
{
  tmrClk = 1;
}

void intHndDt ()
{
  tmrDt = 1;
}

void intHndSw ()
{
  tmrSw = 1;
}
