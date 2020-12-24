
/* defines */

#define MCAST_ADDR            (224,224,224,224)
#define MCAST_PORT            (1234)

/* ports */

#define PIN_VOLTAGE           A0

#define ANA_RANGE             1024

#define VOL_SUPPLY            3300
#define VOL_BAT_DIV_R1        20000
#define VOL_BAT_DIV_R2        10000         

#define MOT_SPEED_STOP        0

#define PIN_MOT_PWM           D0
#define PIN_MOT_PLUS          D1
#define PIN_MOT_MINUS         D2
#define MOT_CTL_OFF           0
#define MOT_CTL_ON            1

#define MOT_REQ_MIN           0           /* request between 0..100 % */
#define MOT_REQ_MAX           100
#define MOT_CTL_MIN           0           /* motor PWM control 0..1023 % */
#define MOT_CTL_MAX           1023

#define PIN_MOT_DIR           D3




#define STATUS_CLEAR          0x0000
/* 
 *  00 - standing
 *  01 - driving fw
 *  10 - driving reverse
 */
#define STATUS_DRIVE_MASK     0x0003
#define STATUS_DRIVE_STANDING 0x0000
#define STATUS_DRIVE_FORWARD  0x0001
#define STATUS_DRIVE_REVERSE  0x0002
#define STATUS_DRIVE_STOPPED  0x0003    /* self stopped */

#define DRIVE_DIR_STAND       0x0000
#define DRIVE_DIR_FORWARD     0x0001
#define DRIVE_DIR_REVERSE     0x0002

#define DRIVE_TURN_OFF        0x0000
#define DRIVE_TURN_LEFT       0x0001
#define DRIVE_TURN_RIGHT      0x0002

#define LIGHTS_POS            0x0001
#define LIGHTS_TURN_L         0x0002
#define LIGHTS_TURN_R         0x0004
#define LIGHTS_WARN           0x0008

/* typedefs */

typedef uint8  msgType_t;
typedef uint16 status_t;
typedef uint16 voltage_t;
typedef uint8  driveState_t;
typedef uint16 speed_t;
typedef uint8  direction_t;
typedef uint16 turnLevel_t;
typedef uint8  turn_t;
typedef uint8  lights_t;

typedef struct msgStatus
{
  status_t          status;
  voltage_t         voltage;
} msgStatus_t;

typedef struct msgReques
{
  driveState_t      driveState;     /* standing, forward, reverse */
  speed_t           speed;          /* motor speed */
  direction_t       direction;      /* left, straight, right */
  turnLevel_t       turnLvl;        /* how much to turn */
} msgRequest_t;

typedef struct message
{
  msgType_t      msgType;
    
} message_t;


typedef enum 
{
  MSG_TYPE_REQ      = 1,
  MSG_TYPE_RESP     = 2,
  MSG_TYPE_EVENT    = 3,
  MSG_TYPE_STATUS   = 4
} msgType_e;
