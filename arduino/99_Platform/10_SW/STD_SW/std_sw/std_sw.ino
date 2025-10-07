/* PROJECT DESCRIPTION
 * 
 * GENERAL PRODUCT DESCRIPTION
 * - What product is used for?
 * - What are the features?
 * - What are the use cases?
 * 
 * PRODUCT INTERFACES:
 * 1. Power supply
 * 2. Inputs
 * 3. Outputs
 * 4. Communication (Ethernet, Wifi, CAN, etc.)
 * 5. Internal buses
 *
 * SYSTEM COMPONENTS
 * - What is the MCU? (discrete or development platform)
 * - What is the power supply unit?
 * - Sensors (switches, voltage, temperature, humidity, etc.)
 * - Actuators (LEDs, Display, motor, etc.)
 *
 * HSI (Hardware/Software Interfaces)
 * - Digital inputs/outputs
 * - Analog inputs
 * - PWM outputs
 * - Buses: SPI, I2C, ...
 *
 * DESIGN DETAILS
 * - SW approach (input processing, algorithm, output providing, etc.)
 */
 

/*******************************************************************************
 * INCLUDE
 ******************************************************************************/

#include "src\std\std_sw.h"
#include "src\std\os\os.h"
#include "src\app\app.h"
#include "src\cfg\app_cfg.h"
#include "src\cfg\app_cfg.h"

/*******************************************************************************
 * DEFINE
 ******************************************************************************/


/*******************************************************************************
 * TYPE
 ******************************************************************************/


/*******************************************************************************
 * VAR
 ******************************************************************************/


uint16_t dbgVal = 0;
uint32_t vaar;

/************************* WiFi Access Point *********************************/
// const char * ssid     = WIFI_SSID;
// const char * password = WIFI_PASS;
// WifiState wifiState = WIFI_ST_NA;
// bool a;

/*******************************************************************************
 * FUNCTION
 ******************************************************************************/

void setup()
{
//    funct_aa();
//    Serial.begin(DEV_UART_BAUD);
//
    os_Init();
//    app_setup();

//    hwal_InitDriver();

//    hwal_Init();
//
//#if (USE_DEMO_APP)
//    DemoApp_Init();
//#else
//    PosMan_Init();
//    IlmMan_Init();
//#endif /* USE_DEMO_APP */
//    LedDrv_Init();


    // os_SetEventState(OS_SCH_EVENT_ID(CYCLIC), true);
}

void loop()
{
     osEvent_Type osEv;

     os_WaitEvent(  OS_EVENT_MASK(CYCLIC)
#if (USE_STATUS_APP)
                  | OS_EVENT_MASK(ACTIVITY)
#endif /* USE_STATUS_APP */
                  );
     os_GetEvent(&osEv);
     os_ClearEvent(osEv);

// #if (USE_STATUS_APP)
//     if (OS_SCH_EVENT_IS_SET(osEv, OS_SCH_EVENT_MASK(ACTIVITY)))
//     {
//         StatusApp_Main();
//     }
// #endif /* USE_STATUS_APP */

//     /* CYCLIC event detected */
//     if (OS_SCH_EVENT_IS_SET(osEv, OS_SCH_EVENT_MASK(CYCLIC)))
//     {
//         /* 01. Receive all inputs */
//         hwal_MainInput(); /* get inputs */

//         /* 02. Application processing */
// #if (USE_DEMO_APP)
//         DemoApp_Main();
// #else
//         PosMan_Main();  /* application processing: position manager */
//         IlmMan_Main();  /* application processing: illumination manager */
// #endif /* USE_DEMO_APP */
//         LedDrv_Main();
//         /* 03. Provide all outputs */
//         hwal_MainOutput(); /* provide outputs */
//     }

    // app_inputs();
    // app_processing();
    // app_outputs();
}
