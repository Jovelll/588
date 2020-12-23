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
* Modified for CHIP_TSUMC PMW driver
**************************************************/
#if CHIP_ID == CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID == CHIP_TSUMD||CHIP_ID == CHIP_TSUMJ||CHIP_ID == CHIP_TSUM9 ||CHIP_ID == CHIP_TSUMF
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
    _PWM_NUMS_,
} PWMNoType;
#else
typedef enum //SKY091127
{
    _PWM0_,
    _PWM1_,
    _PWM2_,
    _PWM3_,
    _PWM4_,
    _PWM5_,
    _PWM6_,
    _PWM7_,
#if (CHIP_ID == CHIP_TSUM9 || CHIP_ID == CHIP_TSUMF)
    _PWM8_,
    _PWM9_,
#endif
    _PWM_NUMS_
} PWMNoType;
#endif

/*
#define _PWM0_  0
#define _PWM1_  1
#define _PWM2_  2
#define _PWM3_  3
#define _PWM4_  4
#define _PWM5_  5
#define _PWM6_  6
#define _PWM7_  7
#define _PWM_NUMS_  8
*/

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

#if 0//(USE_SCALER_GPIO)
INTERFACE void drvGPIO_SetScalerGPIO_OEnable(BYTE u8GPIO,Bool Enable);
INTERFACE void drvGPIO_SetScalerGPIO(BYTE u8GPIO);
INTERFACE void drvGPIO_ClrScalerGPIO(BYTE u8GPIO);
INTERFACE BYTE drvGPIO_GetScalerGPIO(BYTE u8GPIO);
#endif
#if (CHIP_ID == CHIP_TSUMJ) || (CHIP_ID == CHIP_TSUM9) ||(CHIP_ID == CHIP_TSUMF)
INTERFACE void drvGPIO_PWMAlignHSync(BYTE u8PWMCh, Bool fEnable);
#endif

INTERFACE void drvGPIO_PWMAlignVSync(BYTE u8PWMCh,Bool fEnable);
#if (CHIP_ID == CHIP_TSUM9)||(CHIP_ID == CHIP_TSUMF)
INTERFACE void drvGPIO_PWMAlignVSyncDelay(BYTE u8PWMCh,BYTE u8DlyNum);
INTERFACE void drvGPIO_PWMInverse(BYTE u8PWMCh,BOOL bEnable);
#endif
INTERFACE void drvGPIO_SetPWMFreq(BYTE u8PWMCh, DWORD u32PWMFreq);
//INTERFACE void drvGPIO_SetPWMDuty(BYTE u8PWMCh,DWORD u32Duty);
INTERFACE void drvGPIO_SetBacklightDuty(BYTE u8PWMCh,BYTE u8Brightness);
#if 0//(SICODE_IS_B)
INTERFACE DWORD drvGPIO_GetPWMDuty8Bits(BYTE u8PWMCh);
#endif
#if 1//ENABLE_3D_FUNCTION && GLASSES_TYPE!=GLASSES_NVIDIA
INTERFACE void drvGPIO_3DGlass_LEDBL(void);
#endif

#if 1//GLASSES_TYPE==GLASSES_NVIDIA && defined(_NV_LED_BL_3D_EN_)
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

