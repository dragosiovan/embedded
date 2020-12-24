// -----
// StatusLED.cpp - Library for using rotary encoders.
// This class is implemented for use with the Arduino environment.
// Copyright (c) by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
// More information on: http://www.mathertel.de/Arduino
// -----
// 18.01.2014 created by Matthias Hertel
// 17.06.2015 minor updates.
// -----

#include "Arduino.h"
#include "StatusLED.h"

/* RotaryEncoderSwitch
 *
 */

StatusLED::StatusLED (PIN pin)     /* single channel */
{
    uint8_t i;

    _pin = pin;
//    pinMode (_pin, OUTPUT);
//    digitalWrite (_pin, HIGH);
    _neo = Adafruit_NeoPixel(LED_COUNT, _pin, NEO_GRB + NEO_KHZ800);
//    _leds = OctoWS2811 (1, displayMemory, drawingMemory, config);
    _col = LED_COL_DEF;
    _state = STATE_NA;

    _stNo = 0;
    _stCol = NULL;

    _phase = PHASE_NA;
    _tick = 0;
} // StatusLED ()

void StatusLED::init (void)
{
    uint8_t pos;

    _neo.begin ();
    _neo.setBrightness (LED_DEF_BRIG);
    showColor (_col);
} // init

void StatusLED::cyclic (void)
{
    if (_stCol[_state].perOn != 0)  // blinking is needed
    {
        if (_tick > 0)
            _tick--;

        if (_tick == 0)     /* switch phase */
        {
            if (_phase == PHASE_ON)
            {
                _phase = PHASE_OFF;
                setTick (_stCol[_state].perOff);
                showColor (BLACK);
            }
            else
            {
                _phase = PHASE_ON;
                setTick (_stCol[_state].perOn);
                showColor (_col);
            }
        }
    }
} // cyclic ()


void StatusLED::confStCol (uint8_t stNo, StateColor * stCol)
{
    _stNo = stNo;
    _stCol = stCol;
} // init

void StatusLED::showColor (COLOR col)
{
////    uint8_t pos;
////
////    for (pos = 0; pos < LED_COUNT; pos++)
////    {
//        _neo.setPixelColor (pos, _col);
//    }
//    Serial.println (">StatusLED::showColor");
    _neo.setPixelColor (0, col);
    _neo.show();
//    Serial.println ("<StatusLED::showColor");
} // show ()

void StatusLED::setColor (COLOR col)
{
//    Serial.println (">StatusLED::setColor");
    _col = col;
    showColor (_col);
//    Serial.println ("<StatusLED::setColor");
} // show ()

void StatusLED::setState (STATE state, bool isActive)
{
    COLOR col;
//    Serial.print (">StatusLED::setState [");
//    Serial.print (state);
//    Serial.println ("]");
    if (state < _stNo)
    {
        _state = state;
    }

    if (isActive)
    {
        col = _stCol[_state].colAct;
    }
    else
    {
        col = _stCol[_state].colIdle;
    }

    _isAct = isActive;
    _phase = PHASE_ON;
//    setTick (_stCol[state].perOn);
    setColor (col);
//    Serial.println ("<StatusLED::setState");
} // show ()

void StatusLED::setTick (TICK tick)
{
//    Serial.println ("StatusLED::setTick");
    _tick = TICK_FACTOR * tick;
}

