// -----
// RotaryEncoderSwitch.h - Library for using rotary encoders.
// This class is implemented for use with the Arduino environment.
// Copyright (c) by Dragos Iovan
// This work is licensed under a BSD style license.
// -----
// 18.01.2019 created by Dragos Iovan
// -----

#ifndef RotaryEncoderSwitch_h
#define RotaryEncoderSwitch_h

#include "Arduino.h"


#define ENC_USE_DEB

#define ENC_DEB_CNT             1       // default encoder switch debounce counter
#define SEL_DEB_CNT             2       // default select switch debounce counter
#define ENC_USE_MAX_MIN_POS
#define ENC_POS_MAX             100
#define ENC_POS_MIN             0
#define ENC_POS_NA              -1

#define SEL_CLICK_DWN_1_CNT     150 //500     // 1st click down debounce counter (ms)
#define SEL_CLICK_UP_1_CNT      150     // 1st click up debounce counter (ms)
#define SEL_LONG_PRESS_CNT      1000    // long press counter (ms)
#define SEL_LONG_LONG_PRESS_CNT 1000    // long press counter (ms)

#ifdef ENC_USE_MAX_MIN_POS
#define ENC_POS_MAX_COND(val,max)   if (val < max)
#define ENC_POS_MIN_COND(val,min)   if (val > min)
#else
#define ENC_POS_MAX_COND(val,max)
#define ENC_POS_MIN_COND(val,min)
#endif

#define ENC_INC_POS(val,max)    {ENC_POS_MAX_COND(val,max) val++;}
#define ENC_DEC_POS(val,min)    {ENC_POS_MIN_COND(val,min) val--;}

#define ENC_DIR_CW                      // enable clock wise

#ifdef ENC_DIR_CW
#define ENC_CW(val,min,max)     ENC_INC_POS(val,max)
#define ENC_CCW(val,min,max)    ENC_DEC_POS(val,min)
#else
#define ENC_CW(val,min,max)     ENC_DEC_POS(val,min)
#define ENC_CCW(val,min,max)    ENC_INC_POS(val,max)
#endif

/* select switch event */
typedef enum
{
    SEL_EV_NONE             = 0x00,     // no event
    SEL_EV_CLICK            = 0x01,     // press click event
    SEL_EV_SG_CLICK         = 0x02,     // single click event
    SEL_EV_DB_CLICK         = 0x03,     // double click event
    SEL_EV_LONG_PRESS       = 0x04,     // long press event
    SEL_EV_LLONG_PRESS      = 0x05      // long long press event
} SelEv_e;

typedef enum
{
    SEL_ST_OFF              = 0x00,     // no activity
    SEL_ST_DOWN_1           = 0x01,     // 1st click down
    SEL_ST_UP_1             = 0x02,     // 1st click up
    SEL_ST_CLICK            = 0x03,     // single click detected
    SEL_ST_DB_CLICK         = 0x04,     // double click event
    SEL_ST_LONG_PRESS_DEB   = 0x05,     // long press event debounce
    SEL_ST_LONG_PRESS       = 0x06,     // long press event stable
    SEL_ST_LLONG_PRESS      = 0x07,     // long long press event
} SelState_e;

typedef enum
{
    SW_EDGE_OFF             = 0x00,
    SW_EDGE_RISING          = 0x01,
    SW_EDGE_FALLING         = 0x02
} SwState_e;

typedef enum
{
    ENC_EV_NA               = 0x00,
    ENC_EV_CW               = 0x01,     // ClockWise
    ENC_EV_CCW              = 0x02      // CounterClockWise
} EncEvent_e;

class RotaryEncoderSwitch
{
public:
    // ----- Constructor -----
    RotaryEncoderSwitch (int clk, int dt, int sw);

    // retrieve select switch state
    int getSelectState (void);
    int getSelectEvent (void);
    void clearSelectEvent (void);        /* clear event */

    // retrieve the current position
    long  getPosition ();
    EncEvent_e  getEncEvent (void);
    void  clearEncEvent (void);

    // adjust the current position
    void setPosition (long newPosition);

    void setMinMaxPos (int min, int max);
    void setEncDebCnt (int val);

    void cyclic (void);                  // time base cyclic check
    void clkEdgeInt (void);              // edge interrupt for clock
    void dtEdgeInt (void);               // edge interrupt for data
    void selEdgeInt (void);              // edge interrupt for switch

private:

    void selUpdate(void);

    // rotary
    EncEvent_e _encEv;

    int _debClk;            // debounce clk
    int _debDt;             // debounce dt
    int _lastClk;
    int _lastDt;
    long _pos;              // Internal position (4 times _positionExt)
    int _tmrClk;            // timer clk
    int _tmrDt;             // timer dt
    int _pinClk;            // cfg:
    int _pinDt;             // cfg:
    int _min;               // cfg: min position
    int _max;               // cfg: max position
    int _debCntEnc;         // cfg: debounce count

    // select
    int _selPin;            // cfg: switch pin
    int _selDebCnt;         // cfg: select debounce counter
    int _selClkDwn1Cnt;     // cfg: 1st click-down counter
    int _selClkUp1Cnt;      // cfg: 1st click-up counter
    int _selLngPressCnt;    // cfg: long press counter
    int _selLLngPressCnt;   // cfg: long press counter

    int _selDeb;            // debounced pin state
    int _selLast;
    int _selEv;             // event
    int _selStInt;          // internal state
    int _selTmr;            // input pin debounce timer
    int _selClkDwn1Tmr;     // 1st click down timer
    int _selClkUp1Tmr;      // 1st click up timer
    int _selLngPressTmr;    // long press timer
    int _selLLngPressTmr;   // long press timer
};

#endif /* RotaryEncoderSwitch_h */

// End
