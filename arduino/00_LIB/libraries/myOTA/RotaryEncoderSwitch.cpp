

#include "Arduino.h"
#include "RotaryEncoderSwitch.h"

/* RotaryEncoderSwitch
 *
 */

RotaryEncoderSwitch::RotaryEncoderSwitch(int clk, int dt, int sel)
{
    // Remember Hardware Setup
    _pinClk = clk;
    _pinDt = dt;
    _selPin = sel;
  
    _encEv = ENC_EV_NA;

    // Setup the input pins
    pinMode(clk, INPUT);
    pinMode(dt, INPUT);
    pinMode(sel, INPUT);

    // start with position 0;
    _pos = ENC_POS_MIN;
    _min = ENC_POS_MIN;
    _max = ENC_POS_MAX;

    _debClk = HIGH;
    _debDt = HIGH;
    _lastClk = HIGH;
    _lastDt = HIGH;

    _tmrClk = 0;
    _tmrDt = 0;

    _selTmr = 0;
    _selDebCnt = SEL_DEB_CNT;
    _selClkDwn1Cnt = SEL_CLICK_DWN_1_CNT;
    _selClkDwn1Tmr = 0;
    _selClkUp1Cnt = SEL_CLICK_UP_1_CNT;
    _selClkUp1Tmr = 0;
    _selLngPressCnt = SEL_LONG_PRESS_CNT;
    _selLngPressTmr = 0;
    _selLLngPressCnt = SEL_LONG_LONG_PRESS_CNT;
    _selLLngPressTmr = 0;

    _selDeb = HIGH;
    _selLast = HIGH;
    _selEv = SEL_EV_NONE;
    _selStInt = SEL_ST_OFF;
} // RotaryEncoderSwitch()

/* setMinMaxPos
 *
 */

void RotaryEncoderSwitch::setMinMaxPos(int min, int max)
{
    _min = min;
    _max = max;
} // getPosition()

/* getPosition
 *
 */

long  RotaryEncoderSwitch::getPosition(void)
{
    return _pos;
} // getPosition()

EncEvent_e  RotaryEncoderSwitch::getEncEvent (void)
{
    return _encEv;
} // getEncEvent()


void  RotaryEncoderSwitch::clearEncEvent (void)
{
    _encEv = ENC_EV_NA;
} // clearEncEvent()


/* setPosition
 *
 */

void RotaryEncoderSwitch::setPosition(long pos)
{
    // only adjust the external part of the position.
    _pos = pos;
} // setPosition()



/* getSelectState
 *
 */

int RotaryEncoderSwitch::getSelectState(void)
{
    return _selDeb;
}

/* getSelectEvent
 *
 */

int RotaryEncoderSwitch::getSelectEvent(void)
{
    return _selEv;
}

/* clearSelectEvent
 *
 */

void RotaryEncoderSwitch::clearSelectEvent(void)
{
    _selEv = SEL_EV_NONE;
}

/* cyclic
 *
 */

void RotaryEncoderSwitch::cyclic(void)
{
#ifdef ENC_USE_DEB
    if (_tmrClk > 0)
        _tmrClk--;
    else
        _debClk = digitalRead (_pinClk);

    if (_tmrDt > 0)
        _tmrDt--;
    else
        _debDt = digitalRead (_pinDt);
#else
#endif

    if (_selTmr > 0)
        _selTmr--;
    else
        _selDeb = digitalRead (_selPin);    // current pin state debounced

    selUpdate ();

#ifdef ENC_USE_DEB
    if (_lastClk == LOW &&
        _debClk == HIGH &&
        _lastDt == LOW)
    {
        _encEv = ENC_EV_CCW;
        ENC_CCW(_pos, _min, _max);
    }
    else if (_lastDt == LOW &&
             _debDt == HIGH &&
             _lastClk == LOW)
    {
        _encEv = ENC_EV_CW;
        ENC_CW(_pos, _min, _max);
    }

    _lastClk = _debClk;
    _lastDt = _debDt;
#endif
}

/* clkEdgeInt
 *
 */

void RotaryEncoderSwitch::clkEdgeInt(void)
{
#ifdef ENC_USE_DEB
    _tmrClk = _debCntEnc;
#else
    int inClk;
    int inDt;

    inClk = digitalRead (_pinClk);
    inDt = digitalRead (_pinDt);
    if (inClk == LOW)
        {
        if (inDt == LOW) // clockwise
            {
            _encEv = ENC_EV_CW;
            ENC_CW(_pos, _min, _max);
            }
        else             // counterclockwise
            {
            _encEv = ENC_EV_CCW;
            ENC_CCW(_pos, _min, _max);
            }
        }
#endif
}

/* dtEdgeInt
 *
 */

void RotaryEncoderSwitch::dtEdgeInt(void)
{
#ifdef ENC_USE_DEB
    _tmrDt = _debCntEnc;
#else

#endif
}

/* selEdgeInt
 *
 */

void RotaryEncoderSwitch::selEdgeInt(void)
{
#ifdef ENC_USE_DEB
    _selTmr = _selDebCnt;
#else

#endif
}

/* setEncDebCnt
 *
 */

void RotaryEncoderSwitch::setEncDebCnt(int val)
{
#ifdef ENC_USE_DEB
    _debCntEnc = val;
#else

#endif
}

/* selUpdate
 *
 */

void RotaryEncoderSwitch::selUpdate(void)
{
    int swEdgeSt = SW_EDGE_OFF;

    // edge detection
    if ((_selLast == HIGH) && (_selDeb == LOW))
        swEdgeSt = SW_EDGE_FALLING;
    else if ((_selLast == LOW) && (_selDeb == HIGH))
        swEdgeSt = SW_EDGE_RISING;

    // state machine
    switch (_selStInt)
    {
    case SEL_ST_OFF:        // no activity
        if (swEdgeSt == SW_EDGE_FALLING)        // first click down detected
        {
            _selStInt = SEL_ST_DOWN_1;
            _selEv = SEL_EV_CLICK;              // publish click start
            _selLngPressTmr = _selLngPressCnt;
        }

        break;

    case SEL_ST_DOWN_1:     // 1st click down detected

        // single click detection
        if (swEdgeSt == SW_EDGE_RISING)         // first click up detected
        {
            _selStInt = SEL_ST_UP_1;
            _selClkUp1Tmr = _selClkUp1Cnt;      // start counting click up
        }

        // long press detection
        if (_selLngPressTmr > 0)
        {
            _selLngPressTmr--;
        }
        else                                    // timer expired -> long press event detected
        {
            _selStInt = SEL_ST_LONG_PRESS_DEB;  // debounce for long long press
            _selEv = SEL_EV_LONG_PRESS;         // publish long press state
            _selLLngPressTmr = _selLLngPressCnt;// load long long press
        }

        break;

    case SEL_ST_UP_1:       // 1st click up detected

        // double click detection
        if (swEdgeSt == SW_EDGE_FALLING)        // 2st click down detected
        {
            _selStInt = SEL_ST_DB_CLICK;
            _selEv = SEL_EV_DB_CLICK;           // publish double click event
        }

        // single click detection
        if (_selClkUp1Tmr > 0)
        {
            _selClkUp1Tmr--;
        }
        else
        {
            _selStInt = SEL_ST_CLICK;
            _selEv = SEL_EV_SG_CLICK;           // publish single click event
        }

        break;

    case SEL_ST_LONG_PRESS_DEB:

        /* long long press detection */
        if (_selLLngPressTmr > 0)
        {
            _selLLngPressTmr--;
        }
        else
        {
            _selStInt = SEL_ST_LLONG_PRESS;     // long long press
            _selEv = SEL_EV_LLONG_PRESS;        // publish long press event
        }

        /* long long press debounce stopped */
        if (swEdgeSt == SW_EDGE_RISING)
        {
            _selStInt = SEL_ST_LONG_PRESS;      // remain in long press stable
        }

        break;

    case SEL_ST_CLICK:          // sigle click detected
    case SEL_ST_DB_CLICK:       // double click detected
    case SEL_ST_LONG_PRESS:     // long press detected
    case SEL_ST_LLONG_PRESS:    // long long press detected
        /* do nothing */
    //		if (_selSt == SEL_OFF)      // event consumed
            _selStInt = SEL_ST_OFF;
        break;

    default:
        break;
    }

    _selLast = _selDeb;
}

