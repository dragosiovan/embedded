




// CONSTANTs
// PINs
#define PIN_IN_VOLTAGE_LEVEL  PIN_A0
#define PIN_IN_KL15           PIN_A1
#define PIN_IN_ACC            PIN_A2
#define PIN_IN_POS_LIGHT      PIN_A3
#define PIN_IN_DOOR           PIN_A4
#define PIN_OUT_BUZ           PIN_A5

#define LOW   0
#define HIGH  1

#define ON TRUE
#define OFF FALSE

#define DOOR_OPEN 0

int getDoorState(void);
int getKeyInState(void);
int getKl15State(void);
int getPosLightState(void);

#define GET_KL15_STATE        input(PIN_A1)
#define GET_KL_ACC_STATE      input(PIN_A2)
#define GET_POS_LIGHT_STATE   input(PIN_A3)
#define GET_DOOR_STATE        input(PIN_A4)

