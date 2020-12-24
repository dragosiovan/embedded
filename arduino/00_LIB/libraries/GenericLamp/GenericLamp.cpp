

#include "Arduino.h"
#include "GenericLamp.h"

/* RotaryEncoderSwitch
 *
 */

GenericLamp::GenericLamp (PIN pin1, LampType_e type)     /* single channel */
{
    _type = type;

    _pin1 = pin1;

    _pol1 = POLA_DEF;

    _state1 = STATE_DEF;
} // GenericLamp ()

GenericLamp::GenericLamp (PIN pin1, PIN pin2, LampType_e type)   /* dual channel */
{
    _type = type;

    _pin1 = pin1;
    _pin2 = pin2;

    _pol1 = POLA_DEF;
    _pol2 = POLA_DEF;

    _state1 = STATE_DEF;
} // GenericLamp ()

GenericLamp::GenericLamp (PIN pin1, PIN pin2, PIN pin3, LampType_e type) /* RGB */
{
    _type = type;

    _pin1 = pin1;
    _pin2 = pin2;
    _pin3 = pin3;

    _pol1 = POLA_DEF;
    _pol2 = POLA_DEF;
    _pol3 = POLA_DEF;
} // GenericLamp ()

GenericLamp::GenericLamp (PIN pin1, PIN pin2, PIN pin3, PIN pin4, LampType_e type) /* RGB */
{
    _type = type;

    _pin1 = pin1;
    _pin2 = pin2;
    _pin3 = pin3;
    _pin4 = pin4;

    _pol1 = POLA_DEF;
    _pol2 = POLA_DEF;
    _pol3 = POLA_DEF;
    _pol4 = POLA_DEF;
} // GenericLamp ()

GenericLamp::GenericLamp (PIN pin1, PIN pin2, PIN pin3, PIN pin4, PIN pin5, LampType_e type) /* RGB */
{
    _type = type;

    _pin1 = pin1;
    _pin2 = pin2;
    _pin3 = pin3;
    _pin4 = pin4;
    _pin5 = pin5;

    _pol1 = POLA_DEF;
    _pol2 = POLA_DEF;
    _pol3 = POLA_DEF;
    _pol4 = POLA_DEF;
    _pol5 = POLA_DEF;
} // GenericLamp ()


void GenericLamp::setPol1 (Pola_e pol1)
{
    _pol1 = pol1;
} // sePol1 ()

void GenericLamp::setPol2 (Pola_e pol2)
{
    _pol2 = pol2;
} // sePol2 ()

void GenericLamp::setPol3 (Pola_e pol3)
{
    _pol3 = pol3;
} // sePol3 ()

void GenericLamp::setPol4 (Pola_e pol4)
{
    _pol4 = pol4;
} // sePol4 ()

void GenericLamp::setPol5 (Pola_e pol5)
{
    _pol5 = pol5;
} // sePol5 ()

void GenericLamp::setPol (Pola_e pol1)
{
    _pol1 = pol1;
} // sePol ()

void GenericLamp::setPol (Pola_e pol1, Pola_e pol2)
{
    _pol1 = pol1;
    _pol2 = pol2;
} // sePol ()

void GenericLamp::setPol (Pola_e pol1, Pola_e pol2, Pola_e pol3)
{
    _pol1 = pol1;
    _pol2 = pol2;
    _pol3 = pol3;
} // sePol ()

void GenericLamp::setPol (Pola_e pol1, Pola_e pol2, Pola_e pol3, Pola_e pol4, Pola_e pol5)
{
    _pol1 = pol1;
    _pol2 = pol2;
    _pol3 = pol3;
    _pol4 = pol4;
    _pol5 = pol5;
} // sePol ()

void GenericLamp::setInRange (VALUE_IN inRan1)
{
    _inRan1 = inRan1;
} // setInRange ()

void GenericLamp::setInRange1 (VALUE_IN inRan1)
{
    _inRan1 = inRan1;
} // setInRange1 ()

void GenericLamp::setInRange2 (VALUE_IN inRan2)
{
    _inRan2 = inRan2;
} // setInRange2 ()

void GenericLamp::setOutRange (VALUE_IN outRan1)
{
    _outRan1 = outRan1;
} // setOutRange ()

void GenericLamp::setOutRange1 (VALUE_IN outRan1)
{
    _outRan1 = outRan1;
} // setOutRange ()

void GenericLamp::setOutRange2 (VALUE_IN outRan2)
{
    _outRan2 = outRan2;
} // setOutRange ()

VALUE_IN GenericLamp::getVal (void)
{
    return _val1;
} // getVal ()

VALUE_IN GenericLamp::getVal1 (void)
{
    return _val1;
} // getVal ()

VALUE_IN GenericLamp::getVal2 (void)
{
    return _val2;
} // getVal2 ()

VALUE_IN GenericLamp::getVal3 (void)
{
    return _val3;
} // getVal3 ()

VALUE_IN GenericLamp::getVal4 (void)
{
    return _val4;
} // getVal4 ()

VALUE_IN GenericLamp::getVal5 (void)
{
    return _val5;
} // getVal5 ()

void GenericLamp::setVal (VALUE_IN val1)
{
    setVal1 (val1);
} // setVal ()

void GenericLamp::setVal1 (VALUE_IN val1)
{
    VALUE_OUT val;

    if (val1 == 0)
        {
        _state1 = STATE_OFF;
        }
    else
        {
        _state1 = STATE_ON;
        }
    _val1 = val1;
    updateOut ();
} // setVal1 ()

void GenericLamp::setVal2 (VALUE_IN val2)
{
    VALUE_OUT val;

    if (val2 == 0)
        {
        _state1 = STATE_OFF;
        }
    else
        {
        _state1 = STATE_ON;
        }
    _val2 = val2;
    //updateOut1 ();
} // setVal ()

void GenericLamp::setVal3 (VALUE_IN val1)
{
    VALUE_OUT val;

    if (val1 == 0)
        {
        _state1 = STATE_OFF;
        }
    else
        {
        _state1 = STATE_ON;
        }
    _val1 = val1;
    updateOut ();
} // setVal ()

void GenericLamp::setVal4 (VALUE_IN val1)
{
    VALUE_OUT val;

    if (val1 == 0)
        {
        _state1 = STATE_OFF;
        }
    else
        {
        _state1 = STATE_ON;
        }
    _val1 = val1;
    updateOut ();
} // setVal ()

void GenericLamp::setVal5 (VALUE_IN val1)
{
    VALUE_OUT val;

    if (val1 == 0)
        {
        _state1 = STATE_OFF;
        }
    else
        {
        _state1 = STATE_ON;
        }
    _val1 = val1;
    updateOut ();
} // setVal ()



void GenericLamp::incVal (void)
{
    incVal1 ();
} // incVal ()

void GenericLamp::incVal1 (void)
{
    if (_state1 == STATE_ON)
        {
//        Serial.println("INC:ON");
        if (_val1 < _inRan1)
            {
            _val1++;
            }
        }
    else if (_state1 == STATE_OFF)
        {
//        Serial.println("INC:OFF");
        _val1 = 0;
        _val1++;
        _state1 = STATE_ON;
        }
    updateOut ();
} // incVal ()

void GenericLamp::incVal2 (void)
{
    if (_val2 < _inRan1)
        {
        _val2++;
        }
    updateOut ();
} // incVal ()


void GenericLamp::decVal (void)
{
    decVal1 ();
} // decVal ()

void GenericLamp::decVal1 (void)
{
    if (_state1 == STATE_ON)
        {
        if (_val1 > 0)
            {
            _val1--;
            }
        if (_val1 == 0)
            {
            _state1 = STATE_OFF;
            }
        }
    else if (_state1 == STATE_OFF)
        {
        }

    updateOut ();
} // decVal ()

void GenericLamp::decVal2 (void)
{
    if (_val2 > 0)
        {
        _val2--;
        }
    updateOut ();
} // decVal ()


void GenericLamp::setOn (void)
{
    _state1 = STATE_ON;

    if (_val1 == 0)
        _val1++;

    updateOut ();
}

void GenericLamp::setOff (void)
{
    _state1 = STATE_OFF;

    updateOut ();
}

void GenericLamp::switchState (void)
{

    if (_state1 == STATE_OFF)
        setOn ();
    else
        setOff ();
}

State_e GenericLamp::getState (void)
{
    return _state1;
}

void GenericLamp::updateOut (void)
{
    VALUE_OUT outVal;
    VALUE_OUT outVal1;
    VALUE_OUT outVal2;
    VALUE_OUT outVal3;
    VALUE_OUT outVal4;
    VALUE_OUT outVal5;
    VALUE_IN val;

    switch (_type)
        {
        case (LAMP_TYPE_1_1_MONO):
            if (_state1 == STATE_OFF)
                val = 0;
            else
                val = _val1;

            outVal1 = getOutVal (_pol1, val, _inRan1, _outRan1);
            analogWrite (_pin1, outVal1);
            pinMode (_pin1, OUTPUT);
            break;

        case (LAMP_TYPE_1_2_WB):
//            Serial.print ("_val1: ");
//            Serial.println (_val1);

            outVal = getOutVal (_pol1, _val1, _inRan1, _outRan1);
            outVal1 = outVal * _val2 / _inRan2;
            outVal2 = outVal * (_inRan2 - _val2) / _inRan2;
            analogWrite (_pin1, outVal1);
            pinMode (_pin1, OUTPUT);
            analogWrite (_pin2, outVal2);
            pinMode (_pin2, OUTPUT);
            break;

        case (LAMP_TYPE_2_2_MONO):
            break;

        default:
            break;
        }
}

VALUE_OUT GenericLamp::getOutVal (Pola_e pol, VALUE_IN inVal, VALUE_IN inRang, VALUE_OUT outRang)
{
    VALUE_OUT outVal;
    VALUE_OUT outMin = 0;
    VALUE_OUT outMax = 0;

    if (pol == POLA_POS)
        outMax = outRang;
    else if (pol == POLA_NEG)
        outMin = outRang;
    outVal = map (inVal, 0, inRang, outMin, outMax);

    return outVal;
}



