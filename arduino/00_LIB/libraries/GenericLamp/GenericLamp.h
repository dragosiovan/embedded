// -----
// GenericLamp.h - Library for using generic LED lamp.
// This class is implemented for use with the Arduino environment.
// Copyright (c) by Dragos Iovan
// This work is licensed under a BSD style license.
// -----
// 17.07.2019 created by Dragos Iovan
// -----

#ifndef GenericLamp_h
#define GenericLamp_h

#include "Arduino.h"

#define PIN_NA              0xFF
#define POLA_DEF            POLA_POS
#define STATE_DEF           STATE_OFF

typedef uint8_t PIN;
typedef uint16_t VALUE_OUT;     // value of the output pin
typedef uint16_t VALUE_IN;      // input/controll value

/* polarity */
typedef enum
{
    // n light channels + m output pins
    LAMP_TYPE_1_1_MONO    = 0x00,    // 1 channel monochrome (default)                  eg: white warm/cold, any other single color
    LAMP_TYPE_1_2_WB      = 0x01,    // 1 channel white balance                         eg: white warm-cold
    LAMP_TYPE_2_2_MONO    = 0x02,    // 2 channels, both monochrome                     eg: warm white + mono ambilight
    LAMP_TYPE_2_3_WB_MONO = 0x03,    // 2 channels, 1 white balance, 1 monoch           eg: white warm-cold + mono ambilight
    LAMP_TYPE_3_3_RGB     = 0x04,    // 3 channels, all monochrome                      eg: rgb
    LAMP_TYPE_4_4_W_RGB   = 0x05,    // 4 channels, 1 white + 3 all monochrome          eg: warm/cold white + rgb
    LAMP_TYPE_4_5_WB_RGB  = 0x06,    // 4 channels, 1 white balance + 3 all monochrome  eg: warm/cold white + rgb
} LampType_e;


/* polarity */
typedef enum
{
    STATE_NA  = 0x00,    // N/A
    STATE_OFF = 0x01,    // OFF state
    STATE_ON  = 0x02     // ON state
} State_e;

/* channels type */
typedef enum
{
    CHAN_NA   = 0x00,   // N/A
    CHAN_MONO = 0x01,   // monocrome (white)
    CHAN_DUAL = 0x02,   // dual color (WW, CW)
    CHAN_RGB  = 0x03    // Red, Gree, Blue
} Chan_e;

/* polarity */
typedef enum
{
    POLA_NA  = 0x00,    // N/A
    POLA_POS = 0x01,    // positive
    POLA_NEG = 0x02     // negative
} Pola_e;

class GenericLamp
{
public:
    // ----- Constructor -----
    GenericLamp (PIN pin1, LampType_e type);                                         // LAMP_TYPE_1_1_MONO
    GenericLamp (PIN pin1, PIN pin2, LampType_e type);                               // LAMP_TYPE_1_2_WB, LAMP_TYPE_2_2_MONO
    GenericLamp (PIN pin1, PIN pin2, PIN pin3, LampType_e type);                     // LAMP_TYPE_2_3_WB_MONO, LAMP_TYPE_3_3_RGB
    GenericLamp (PIN pin1, PIN pin2, PIN pin3, PIN pin4, LampType_e type);           // LAMP_TYPE_4_4_W_RGB
    GenericLamp (PIN pin1, PIN pin2, PIN pin3, PIN pin4, PIN pin5, LampType_e type); // LAMP_TYPE_4_5_WB_RGB

    void setPol1 (Pola_e pol1);
    void setPol2 (Pola_e pol2);
    void setPol3 (Pola_e pol3);
    void setPol4 (Pola_e pol4);
    void setPol5 (Pola_e pol5);
    void setPol (Pola_e pol1);
    void setPol (Pola_e pol1, Pola_e pol2);
    void setPol (Pola_e pol1, Pola_e pol2, Pola_e pol3);
    void setPol (Pola_e pol1, Pola_e pol2, Pola_e pol3, Pola_e pol4);
    void setPol (Pola_e pol1, Pola_e pol2, Pola_e pol3, Pola_e pol4, Pola_e pol5);

    void setVal (VALUE_IN val1);
    void setVal1 (VALUE_IN val1);
    void setVal2 (VALUE_IN val1);
    void setVal3 (VALUE_IN val1);
    void setVal4 (VALUE_IN val1);
    void setVal5 (VALUE_IN val1);

    VALUE_IN getVal (void);
    VALUE_IN getVal1 (void);
    VALUE_IN getVal2 (void);
    VALUE_IN getVal3 (void);
    VALUE_IN getVal4 (void);
    VALUE_IN getVal5 (void);
    void incVal (void);
    void incVal1 (void);
    void incVal2 (void);
    void decVal (void);
    void decVal1 (void);
    void decVal2 (void);

    void setOn (void);
    void setOff (void);
    State_e getState (void);
    void switchState (void);


    void setInRange (VALUE_IN inRan1);
    void setInRange1 (VALUE_IN inRan1);
    void setInRange2 (VALUE_IN inRan1);

    void setOutRange (VALUE_OUT outRan1);
    void setOutRange1 (VALUE_OUT outRan1);
    void setOutRange2 (VALUE_OUT outRan2);

private:
    LampType_e _type;

    PIN        _pin1;
    PIN        _pin2;
    PIN        _pin3;
    PIN        _pin4;
    PIN        _pin5;

    Pola_e     _pol1;
    Pola_e     _pol2;
    Pola_e     _pol3;
    Pola_e     _pol4;
    Pola_e     _pol5;

    State_e    _state1;

    VALUE_IN   _val1;
    VALUE_IN   _val2;
    VALUE_IN   _val3;
    VALUE_IN   _val4;
    VALUE_IN   _val5;

    VALUE_IN   _inRan1;
    VALUE_IN   _inRan2;
    VALUE_IN   _inRan3;
    VALUE_IN   _inRan4;
    VALUE_IN   _inRan5;

    VALUE_OUT  _outRan1;
    VALUE_OUT  _outRan2;
    VALUE_OUT  _outRan3;
    VALUE_OUT  _outRan4;
    VALUE_OUT  _outRan5;

    VALUE_OUT  getOutVal (Pola_e pol, VALUE_IN inVal, VALUE_IN inRang, VALUE_OUT outRang);
    void       updateOut (void);
};

#endif /* GenericLamp_h */

// End
