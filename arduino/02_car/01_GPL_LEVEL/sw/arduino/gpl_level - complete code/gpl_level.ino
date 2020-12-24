#include <avr/interrupt.h>
#include <Led7Segment.h>

#define LED7SEG_COMMON_CATHODE
//#define LED7SEG_COMMON_ANODE

#if (defined LED7SEG_COMMON_CATHODE) && (defined LED7SEG_COMMON_ANODE) /* both common cathode and common anode defined: not allowed */
#error %THE LED7SEG TYPE IS NOT CLEAR DEFINED%
#elif defined LED7SEG_COMMON_CATHODE           /* common cathode display selected */
#define DIGIT_MUX_ACTIVE   HIGH  /* active digit */
#define DIGIT_MUX_INACTIVE LOW   /* inactive digit */
#define SEGMENT_ACTIVE     HIGH  /* active segment */
#define SEGMENT_INACTIVE   LOW   /* inactive segment */
#elif defined LED7SEG_COMMON_ANODE             /* common anode display selected */
#define DIGIT_MUX_ACTIVE   LOW   /* active digit */
#define DIGIT_MUX_INACTIVE HIGH  /* inactive digit */
#define SEGMENT_ACTIVE     LOW   /* active segment */
#define SEGMENT_INACTIVE   HIGH  /* inactive segmenet
#else                                          /* display type not defined */
#error %PLEASE SELECT THE LED7SEG TYPE: CATHODE OR ANODE%  
#endif



#define DIGITS_NO  2 /* number of digits of LED7SEG display */

const byte digitSegments[][8] = 
{
  /* A     B     C     D     E     F     G     DP */
  /*     '0' B11111100 */
  { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH,  LOW,  LOW},
  /*     '1' B01100000 */
  {  LOW, HIGH, HIGH,  LOW,  LOW,  LOW,  LOW,  LOW},
  /*     '2' B11011010 */
  { HIGH, HIGH,  LOW, HIGH, HIGH,  LOW, HIGH,  LOW},
  /*     '3' B11110010 */
  { HIGH, HIGH, HIGH, HIGH,  LOW,  LOW, HIGH,  LOW},
  /*     '4' B01100110 */
  {  LOW, HIGH, HIGH,  LOW,  LOW, HIGH, HIGH,  LOW},
  /*     '5' B10110110 */
  { HIGH,  LOW, HIGH, HIGH,  LOW, HIGH, HIGH,  LOW},
  /*     '6' B10111110 */
  { HIGH,  LOW, HIGH, HIGH, HIGH, HIGH, HIGH,  LOW},
  /*     '7' B11100000 */
  { HIGH, HIGH, HIGH,  LOW,  LOW,  LOW,  LOW,  LOW},
  /*     '8' B11111110 */
  { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH,  LOW},
  /*     '9' B11110110 */
  { HIGH, HIGH, HIGH, HIGH,  LOW, HIGH, HIGH,  LOW},
  /* 10: ' ' B00000000 */
  {  LOW,  LOW,  LOW,  LOW,  LOW,  LOW,  LOW,  LOW}
};

/* Timer */
volatile unsigned int tmr_tcnt2;

/* 7 SEG LED */
byte activeDigit = 0;                        /* active digit for MUX */
byte previousDigit = 0;                      /* previous active digit for MUX */
byte digitPins[] = {9, 10, 11, 12};          /* LSB to MSB */
byte segmentPins[] = {2, 3, 4, 5, 6, 7, 8};  /* Segment a to g */
byte digitsValue[] = {1, 0};                /* digits values */

/* measurement */
unsigned int level = 23;                          /* measurement value */


/***********************************************************************************************
 * Timer initialization
 **********************************************************************************************/
void setup()
{
  byte digitIdx = 0;

  Serial.begin(57600);
  
  initPort();
  initTimer();
} /* setup() */


void initPort(void)
{
  byte i = 0;
  
  for (i=0; i<8; i++)
  {
    pinMode(segmentPins[i], OUTPUT);
    digitalWrite(segmentPins[i], LOW);
  }
  
  for (i=0; i<DIGITS_NO; i++)
  {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], LOW);
  }
}


/***********************************************************************************************
 * Timer initialization
 **********************************************************************************************/
void loop()
{
  int adc = 0;
//  for (byte i = 0; i < 10; i++) // read the ch0 adc 10 times to average
//  {
//    level += analogRead(A0); 
//    delay(10); // wait for adc to stabilize
//  }
//  level /= 10; // average the adc
  
  
  
  calcDigit();
  //updateLed7Seg();
  
  Serial.print("\n\r digit value "); 
  Serial.print(digitsValue[1], DEC); 
  Serial.print(" "); 
  Serial.print(digitsValue[0], DEC);
//  
  delay(1000);
} /* loop() */

void calcDigit(void)
{
  digitsValue[0] = level%10;
  digitsValue[1] = level/10;
}


/***********************************************************************************************
 * Timer overflow interrupt
 **********************************************************************************************/
ISR(TIMER2_OVF_vect) 
{
#if defined (__AVR_ATmega8__)
  TCNT2 = tmr_tcnt2;    // reload timer
#endif
  updateLed7Seg();
} /* TIMER2_OVF_vect */


/***********************************************************************************************
 * Timer initialization
 **********************************************************************************************/
void initTimer(void)
{
  float prescaler = 0.0;

#if defined (__AVR_ATmega8__)
#warning CONFIGURE TIMER FOR ATMEGA8
  TIMSK &= ~(1<<TOIE2);
  TCCR2 &= ~((1<<WGM21) | (1<<WGM20));
  TIMSK &= ~(1<<OCIE2);
  ASSR &= ~(1<<AS2);

  if ((F_CPU >= 1000000UL) && (F_CPU <= 16000000UL)) {	// prescaler set to 64
    TCCR2 |= (1<<CS22);
    TCCR2 &= ~((1<<CS21) | (1<<CS20));
    prescaler = 64.0;
  } 
  else if (F_CPU < 1000000UL) {	// prescaler set to 8
    TCCR2 |= (1<<CS21);
    TCCR2 &= ~((1<<CS22) | (1<<CS20));
    prescaler = 8.0;
  } 
  else { // F_CPU > 16Mhz, prescaler set to 128
    TCCR2 |= ((1<<CS22) && (1<<CS20));
    TCCR2 &= ~(1<<CS21);
    prescaler = 128.0;
  }
#else
#error %TIMER NOT CONFIGURED FOR THIS PLATFORM%
#endif
  tmr_tcnt2 = 256 - (int)((float)F_CPU * 0.001 / prescaler);

#if defined (__AVR_ATmega8__)
  TCNT2 = tmr_tcnt2;    // reload timer 
  TIMSK |= (1<<TOIE2);  // reactivate interrupt
#endif
} /* initTimer() */


/***********************************************************************************************
 * LED 7SEG MUX 
 **********************************************************************************************/
void updateLed7Seg(void)
{  
  byte segment = 0;
  
  /* first disable previous active digit, before updating the segments */
  digitalWrite(digitPins[activeDigit], HIGH/*DIGIT_MUX_INACTIVE*/); 
  
  /* cycle through the digits */
  activeDigit++; /* switch to next digit mux */

  if (DIGITS_NO == activeDigit)
  {
    activeDigit = 0; /* switch to the first digit */
  }

/* output the digit on 7SEG */
  for (segment = 0; segment < 8; segment++)
  {
    digitalWrite(segmentPins[segment], digitSegments[digitsValue[activeDigit]][segment]);
  }

/* enable the active digit, after segments are updated */
  digitalWrite(digitPins[activeDigit], LOW /*DIGIT_MUX_ACTIVE*/);       
} /* updateLed7Seg() */


