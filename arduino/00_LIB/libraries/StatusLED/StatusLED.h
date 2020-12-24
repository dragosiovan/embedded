// -----
// StatusLED.h - Library for using Status LED.
// This class is implemented for use with the Arduino environment.
// Copyright (c) by Dragos Iovan
// This work is licensed under a BSD style license.
// -----
// 23.07.2019 created by Dragos Iovan
// -----

#ifndef StatusLED_h
#define StatusLED_h

#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
//#include <PololuLedStrip.h>
//#include <OctoWS2811.h>

//#define PIN_NA              0xFF
//#define POLA_DEF            POLA_POS
//#define STATE_DEF           STATE_OFF

#define LED_COUNT             1
#define LED_DEF_BRIG          0x255
#define LED_COL_DEF           0x004400
#define LED_COL_OFF           ((COLOR) 0x000000)
#define STATE_NA              0xFF
#define STATE_DEF             0
#define BLINK_PER_OFF         0         // no blink
#define TICK_FACTOR           10

#define BLACK                 0x000000
#define WHITE_H               0xFFFFFF
#define WHITE_M               0xA0A0A0
#define WHITE_L               0x505050
#define WHITE_LL              0x101010
#define RED_H                 0xFF0000
#define RED_M                 0xA00000
#define RED_L                 0x500000
#define RED_LL                0x100000
#define GREEN_H               0x00FF00
#define GREEN_M               0x00A000
#define GREEN_L               0x005000
#define GREEN_LL              0x001000
#define BLUE_H                0x0000FF
#define BLUE_M                0x0000A0
#define BLUE_L                0x000050
#define BLUE_LL               0x000010
#define YELLOW_H              0xFFFF00
#define YELLOW_M              0xA0A000
#define YELLOW_L              0x505000
#define YELLOW_LL             0x101000
#define PURPLE_H              0xFF00FF
#define PURPLE_M              0xA000A0
#define PURPLE_L              0x500050
#define PURPLE_LL             0x100010



typedef uint8_t  PIN;
typedef uint32_t COLOR;
typedef uint8_t  STATE;
typedef uint8_t  BLINK_PERIOD;
typedef uint16_t TICK;

typedef enum
{
    PHASE_OFF = 0,
    PHASE_ON = 1,
    PHASE_NA
} PHASE;

typedef struct
{
    uint8_t perOn;
    uint8_t perOff;
    COLOR   colAct;
    COLOR   colIdle;
} StateColor;

class StatusLED
{
public:
    // ----- Constructor -----
    StatusLED (PIN pin);

    void init  (void);
    void cyclic (void);
    void confStCol (uint8_t stNo, StateColor * stCol);
    void setState (STATE state, bool isActive);
    void setColor (COLOR col);

private:
    void showColor (COLOR col);
    void setStateColor (uint8_t state, COLOR col, BLINK_PERIOD per);
    void setTick (TICK tick);

    PIN               _pin;
    Adafruit_NeoPixel _neo;
//    OctoWS2811        _leds;

//    DMAMEM int displayMemory[10*6];
//    int drawingMemory[10*6];
//    const int config = WS2811_GRB | WS2811_800kHz;


    COLOR             _col;
    STATE             _state;
    bool              _isAct;
    uint8_t           _stNo;
    StateColor *      _stCol;
    TICK              _tick;
    PHASE             _phase;
};

#endif /* GenericLamp_h */

// End
