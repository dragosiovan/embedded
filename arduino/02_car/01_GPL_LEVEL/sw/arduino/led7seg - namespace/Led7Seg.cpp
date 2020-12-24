#include "Led7Seg.h"


#define DIGIT_ACTIVE_PERIOD   3  /* (ms) */


const byte _digitSegments[][8] = 
{
  /* A     B     C     D     E     F     G     DP */
  /* '0' B11111100 */
  { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH,  LOW,  LOW},
  /* '1' B01100000 */
  {  LOW, HIGH, HIGH,  LOW,  LOW,  LOW,  LOW,  LOW},
  /* '2' B11011010 */
  { HIGH, HIGH,  LOW, HIGH, HIGH,  LOW, HIGH,  LOW},
  /* '3' B11110010 */
  { HIGH, HIGH, HIGH, HIGH,  LOW,  LOW, HIGH,  LOW},
  /* '4' B01100110 */
  {  LOW, HIGH, HIGH,  LOW,  LOW, HIGH, HIGH,  LOW},
  /* '5' B10110110 */
  { HIGH,  LOW, HIGH, HIGH,  LOW, HIGH, HIGH,  LOW},
  /* '6' B10111110 */
  { HIGH,  LOW, HIGH, HIGH, HIGH, HIGH, HIGH,  LOW},
  /* '7' B11100000 */
  { HIGH, HIGH, HIGH,  LOW,  LOW,  LOW,  LOW,  LOW},
  /* '8' B11111110 */
  { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH,  LOW},
  /*     '9' B11110110 */
  { HIGH, HIGH, HIGH, HIGH,  LOW, HIGH, HIGH,  LOW},
  /* 10: ' ' B00000000 */
  {  LOW,  LOW,  LOW,  LOW,  LOW,  LOW,  LOW,  LOW}
};

/* Timer */
volatile unsigned int tmr_tcnt2;

/* 7 SEG LED */
byte _activeDigit = 0;                        /* active digit for MUX */

byte _digitsValue[4] = {10, 10, 10, 10};               /* digits values */
byte _noOfDigits;
byte *_digitPins;
byte *_segmentPins;
unsigned int _value;
unsigned long _msecs;
unsigned long _msCnt;

/*******************************************************************************
 * 
 ******************************************************************************/
void Led7Seg::setup(byte noOfDigits, byte *digitPins, byte *segmentPins)
{
  _noOfDigits = noOfDigits;
  _digitPins = digitPins;
  _segmentPins = segmentPins;
  _msecs =  DIGIT_ACTIVE_PERIOD;
     
  initPort();
  initTimer();
} /* setup() */


/*******************************************************************************
 * 
 ******************************************************************************/
void Led7Seg::main(unsigned int level, unsigned int level)
{

} /* setup() */


/*******************************************************************************
 * 
 ******************************************************************************/
void Led7Seg::loadValue(byte *digitsValue) // extract the integer value into decimal values
{
  for (byte i=0; i<_noOfDigits; i++)
  {
    _digitsValue[i] = digitsValue[i];
  }
}


/*******************************************************************************
 * 
 ******************************************************************************/
void Led7Seg::update(void)
{  
  byte segment = 0;
  
  /* first disable previous active digit, before updating the segments */
  digitalWrite(_digitPins[_activeDigit], HIGH);
  
  /* cycle through the digits */
  _activeDigit++; /* switch to next digit mux */
  if (_noOfDigits == _activeDigit)
  {
    _activeDigit = 0; /* switch to the first digit */
  } 
  
/* output the digit on 7SEG */
  for (segment = 0; segment < 7; segment++)
  {
    digitalWrite(_segmentPins[segment], _digitSegments[_digitsValue[_activeDigit]][segment]);
  }

/* enable the active digit, after segments are updated */
  digitalWrite(_digitPins[_activeDigit], LOW);   
    
} /* update() */


/*******************************************************************************
 * 
 ******************************************************************************/
void Led7Seg::initPort(void)
{
  byte i = 0;
  
  for (i=0; i<8; i++)
  {
    pinMode(_segmentPins[i], OUTPUT);
    digitalWrite(_segmentPins[i], LOW);
  }
  
  for (i=0; i<_noOfDigits; i++)
  {
    pinMode(_digitPins[i], OUTPUT);
    digitalWrite(_digitPins[i], HIGH);
  }
}


/*******************************************************************************
 * 
 ******************************************************************************/
void Led7Seg::initTimer(void)
{
  float prescaler = 0.0;

#if defined (__AVR_ATmega8__)
//#warning CONFIGURE TIMER FOR ATMEGA8
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


/*******************************************************************************
 * 
 ******************************************************************************/
void Led7Seg::_overflow() 
{
	_msCnt += 1;
	if (_msCnt >= _msecs) 
	{
		_msCnt = 0;   /* reset period counter */
		update();
	}
}


/*******************************************************************************
 * 1ms timer
 ******************************************************************************/
ISR(TIMER2_OVF_vect) 
{
#if defined (__AVR_ATmega8__)
  TCNT2 = tmr_tcnt2;    // reload timer
#endif
  Led7Seg::_overflow();
} /* TIMER2_OVF_vect */