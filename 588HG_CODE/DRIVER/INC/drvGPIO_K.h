///////////////////////////////////////////////////////////////////////////////
/// @file drvScalerGPIO.h
/// @brief Scaler GPIO/PWM access
/// @author MStarSemi Inc.
///
/// Driver for Scaler GPIO/PWM access.
///
/// Features
///  -
///  -
///////////////////////////////////////////////////////////////////////////////

#ifndef _DRVGPIO_H
#define _DRVGPIO_H

#include "GPIO_DEF.h"

/**************************************************
* Modified for CHIP_TSUMK PMW driver
**************************************************/
typedef enum
{
    _PWM0_,
    _PWM1_,
    _PWM2_,
    _PWM3_,
    _PWM4_,
    _PWM5_,
    _PWM6_,
    _PWM7_,
    _PWM8_,
    _PWM9_,
    _DISP_PWM0_,
    _DISP_PWM1_,
    _DISP_PWM2_,
    _DISP_PWM3_,
    _DISP_PWM4_,
    _DISP_PWM5_,
    _NOUSE_
} PWMNoType;

typedef enum
{
    _PWM0_GP0_,  //GPIO01
    _PWM1_GP0_,  //GPIO02
    _PWM2_GP0_,  //GPIO03
    _PWM3_GP0_,  //GPIO11
    _PWM4_GP0_,  //GPIO22
    _PWM5_GP0_,  //GPIO23
    _PWM6_GP0_,  //GPIO24
    _PWM7_GP0_,  //GPIO25
    _PWM8_GP0_,  //GPIO50
    _PWM9_GP0_,  //GPIO51
    
    _PWM2_GP1_=10,  //SAR2
    _PWM3_GP1_,  //SAR3
    _PWM8_GP1_,  //GPIO56
    _PWM9_GP1_,  //GPIO20
    
    _PWM2_GP2_=20,  //GPIO_L9
    _PWM3_GP2_,  //GPIO_L10
    _PWM8_GP2_,  //GPIO_L11
    _PWM9_GP2_,  //GPIO_L12
	
    _DISP_PWM0_GP0_=30, //PAD_GPIO51
    _DISP_PWM1_GP0_, //PAD_GPIO52
    _DISP_PWM2_GP0_, //PAD_SAR0
    _DISP_PWM3_GP0_, //PAD_GPIO37
    _DISP_PWM4_GP0_, //PAD_GPIO55
    _DISP_PWM5_GP0_, //PAD_GPIO56

    _NO_USE_,
} PWM_GP_No_Type;


//PWM Config
#define _PWM_DB_EN_ BIT0
#define _PWM_DB_VSYNC_MODE_ BIT1//0:vsync double buffer,1:double buffer
#define _PWM_VSYNC_ALIGN_EN_ BIT2
#define _PWM_HSYNC_ALIGN_EN_ BIT3
#define _PWM_POLARITY_EN_ BIT4
#define _PWM_OD_EN_ BIT5

#if (MS_PM)
typedef enum //Mike 110318 //Sky110406
{
    _LED_PUSH_PULL_,
    _LED_OPEN_DRAIN_,
} PWMPadControlType;
#endif

#define LINEAR_MAPPING_VALUE(orgNow,orgMin,orgMax,newMin,newMax) \
    ((((DWORD)((orgNow)-(orgMin)))*((newMax)-(newMin))+(((orgMax)-(orgMin))>>1))/((orgMax)-(orgMin))+(newMin))


#ifdef _MSOSD_C
#define INTERFACE
#else
#define INTERFACE   extern
#endif

//INTERFACE void drvGPIO_PWMAlignHSync(DWORD u32PWMCh, Bool fEnable, BYTE u8Counter);
//INTERFACE void drvGPIO_PWMAlignVSync(DWORD u32PWMCh, Bool fEnable, BYTE u8Counter);
INTERFACE void drvGPIO_SetPWMFreq(BYTE u8PWMCh,DWORD u32PWMFreq);
INTERFACE void drvGPIO_SetPWMDuty(BYTE u8PWMCh, WORD u16ByteDuty);
INTERFACE void drvGPIO_SetBacklightDuty(BYTE u8PWMCh, WORD u16ByteDuty);
INTERFACE void msAPIPWMConfig(BYTE u8PWMCh, DWORD u32Freq, WORD u16Duty, BYTE u8Config, BYTE u8Counter ,WORD u16Shift);
INTERFACE void msDrvPWMEnableSwitch(BYTE u8PWMCh, Bool bEnable);
INTERFACE void msDrvPWMAlignSync(BYTE u8PWMCh,BYTE u8Config, BYTE u8Counter);
INTERFACE void msDrvPWMDoubleBuffer(BYTE u8PWMCh, BYTE u8Config);
INTERFACE void msAPIPWMShift(BYTE u8PWMCh, DWORD u32PWMShift);
INTERFACE void drvGPIO_PWMAlignVSyncDelay(BYTE u8PWMCh,BYTE u8DlyNum);
INTERFACE void drvGPIO_uncall(void);

#if 0//GLASSES_TYPE==GLASSES_NVIDIA && defined(_NV_LED_BL_3D_EN_)
//===========================
#define LED_BL_ON_120           1416
#define LED_BL_OFF_120          258
#define LED_BL_ON_110           1420
#define LED_BL_OFF_110          266
#define LED_BL_ON_100           1528
#define LED_BL_OFF_100          248
//===========================

typedef struct{
    WORD    u16LedOn;
    WORD    u16LedOff;
}LED_BL_PARAM;

INTERFACE LED_BL_PARAM xdata LedParameter;
INTERFACE void ms_LEDParameter_Init(void);
INTERFACE void  ms_LED_Backlight(Bool bEnable);
#endif

#undef INTERFACE
#endif //_DRVGPIO_H

