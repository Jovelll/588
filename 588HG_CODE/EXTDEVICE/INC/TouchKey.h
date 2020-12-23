

#if TOUCH_KEY_SOURCE == TOUCH_KEY_SOURCE_ITE
#include "IT7230_TouchKey.h"
#include "IT7230_I2C.h"
#include "IT7230_SettingTable.h"

#define TouchKey_Init   IT7230_Init
#define TouchKey_Check_ESD   IT7230_Check_ESD
#define TouchKey_GetKeyStatus    IT7230_GetKeyStatus
#define TouchKey_Restort_Reg   IT7230_Restort_Reg
#define TouchKey_InitStatus     IT7230_Init_Status
    #if TOUCH_KEY_CTRL_LED
    #define TouchKey_CtrlLED    IT7230_LEDStatus
    //#define TouchKey_GetLEDStatus   IT7230_GetLEDStatus
    #endif
#elif TOUCH_KEY_SOURCE == TOUCH_KEY_SOURCE_SMSC
#elif TOUCH_KEY_SOURCE == TOUCH_KEY_SOURCE_CYPRESS
#else
#message "please implement function for new touch key source"
#endif


