///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   drvADC.h
/// @author MStar Semiconductor Inc.
/// @brief  ADC Function
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DRVADC_H_
#define _DRVADC_H_

//#include "drv_adctbl.h"
#include "Mdrv_adctbl.h"
//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

//#define _DGAIN_CAL_WITH_INPUT_
//#define _DGAIN_CAL_WITHOUT_INPUT_
#define DADCWORSTPHASE      0 // ADC find good phase criteria by worst phase
#define DADCPLLPROTECTION   1 // ADC PLL protection mechnism
#define ENABLE_ADC_RESET    1 // ADC software reset for °ª·Å¥Õ«Ìissue

#ifdef _DGAIN_CAL_WITHOUT_INPUT_
// New ADC internal calibration without input
//	0: original method 
//	  - Internal ADC calibration by LDO or bandgap
//	1: new method I 
//	  - Precondition: ADC bandgap needs be trimmed and store in Efuse. 
//	  - Mismatch gain is calibrated to maxi swing
//	  - Digital gain is fixed at 0x421( (1024/(1024-32)) << 10 )
// 	  - Defeat: Due to process deviation, some chips may not be guaranteed to maxi swing
//	2: new method II
//	  - Precondition: ADC bandgap needs be trimmed and store in Efuse. 
//	  - Mismatch gain is fixed at 1
//	  - Digital target gain: 1024/(1024-32)*PGA gain/0.7
#define DADCCALBYBANDGAP    1 // 0: ADC internal calibration by LDO / 1: ADC internal calibration by Bandgap
#define DADCNEWGAINCALMETHOD    2 // 0:original / 1: new method I / 2: new method II
#else
#define DADCCALBYBANDGAP    0 // 0: ADC internal calibration by LDO / 1: ADC internal calibration by Bandgap
#define DADCNEWGAINCALMETHOD    0 // 0:original / 1: new method I / 2: new method II
#endif // #ifdef _DGAIN_CAL_WITHOUT_INPUT_

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

typedef enum
{
    // Unknow
    ADC_INPUTSOURCE_UNKNOW = 0x00,
    // Single source
    ADC_INPUTSOURCE_RGB    = 0x01,
    ADC_INPUTSOURCE_YPBPR  = 0x02,
}ADC_INPUTSOURCE_TYPE;

typedef enum
{
    E_LPFBW_300MHz = 0x0,
    E_LPFBW_250MHz = 0x1,
    E_LPFBW_200MHz = 0x2,
    E_LPFBW_180MHz = 0x3,
    E_LPFBW_160MHz = 0x4,
    E_LPFBW_140MHz = 0x5,
    E_LPFBW_120MHz = 0x6,
    E_LPFBW_100MHz = 0x7,
    E_LPFBW_80MHz  = 0x8,
    E_LPFBW_70MHz  = 0x9,
    E_LPFBW_60MHz  = 0xA,
    E_LPFBW_45MHz  = 0xB,
    E_LPFBW_30MHz  = 0xC,
    E_LPFBW_20MHz  = 0xD,
    E_LPFBW_15MHz  = 0xE,
    E_LPFBW_5MHz   = 0xF,
}ADCLPFBW_LIST;
typedef enum
{
    ADC_POWER_ON,
    ADC_POWER_STANDBY,
    ADC_POWER_DOWN

} ADC_PowerCtrlType;

typedef enum
{
    ADC_HSYNC_LVL_0 = 0x0, // REFH=1.54V, REFL=1.10V
    ADC_HSYNC_LVL_1 = 0x1, // REFH=1.54V, REFL=0.88V
    ADC_HSYNC_LVL_2 = 0x2, // REFH=1.76V, REFL=1.10V
    ADC_HSYNC_LVL_3 = 0x3, // REFH=1.76V, REFL=0.88V
    ADC_HSYNC_LVL_4 = 0x4, // REFH=1.87V, REFL=1.10V
    ADC_HSYNC_LVL_5 = 0x5, // REFH=1.87V, REFL=0.88V
    ADC_HSYNC_LVL_6 = 0x6, // REFH=1.76V, REFL=1.32V
    ADC_HSYNC_LVL_7 = 0x7, // REFH=1.87V, REFL=1.32V
} ADC_HSYNCLVL_T;

//-------------------------------------------------------------------------------------------------
//  Function Prototype
//-------------------------------------------------------------------------------------------------
#ifdef _DRVADC_C_
#define INTERFACE
#else
#define INTERFACE   extern
#endif

INTERFACE void drvADC_init(Bool IsShareGrd);
INTERFACE void drvADC_SetInputMux(InputPortType port_type );
INTERFACE void drvADC_SetADCSource(ADC_SOURCE_TYPE inputsrc_type);
INTERFACE void drvADC_SetADCModeSetting(ADC_INPUTSOURCE_TYPE enADCInput, WORD u16PixelClk);
INTERFACE void drvADC_AdjustHTotal(WORD htotal);
INTERFACE void drvADC_SetPhaseCode(BYTE phasecode);
INTERFACE void drvADC_SetRGBGainCode(WORD rcode, WORD gcode, WORD bcode);
INTERFACE void drvADC_SetRGBOffsetCode(WORD rcode, WORD gcode, WORD bcode);
INTERFACE void drvADC_ADCOffsetGainMismatchCal(void);
//INTERFACE Bool drvADC_AutoPhaseAdjustment(BYTE vsyncTime);
INTERFACE Bool drvADC_AutoAdcColor(BYTE vsyncTime);
INTERFACE void drvADC_SetupADCLPF(ADCLPFBW_LIST val);
INTERFACE void drvADC_SetRedGainCode(WORD u16RCode);
INTERFACE void drvADC_SetGreenGainCode(WORD u16GCode);
INTERFACE void drvADC_SetBlueGainCode(WORD u16BCode);
INTERFACE void drvADC_SetRedOffsetCode(WORD u16RCode);
INTERFACE void drvADC_SetGreenOffsetCode(WORD u16GCode);
INTERFACE void drvADC_SetBlueOffsetCode(WORD u16BCode);
INTERFACE void drvADC_PowerCtrl(BYTE u8State);
INTERFACE BYTE drvADC_WaitAutoStatusReady(WORD regAddr, BYTE regFlag);

#if DADCPLLPROTECTION
INTERFACE Bool drvADC_SetModewithPLLProtection(ADC_INPUTSOURCE_TYPE enADCInput, WORD u16PixelClk, WORD htotal);
#endif //#ifdef DADCPLLPROTECTION
#ifdef _DGAIN_CAL_WITHOUT_INPUT_
INTERFACE void drvADC_ADCAutoGainCalwithoutInput(void);
#endif
#ifdef _DGAIN_CAL_WITH_INPUT_
INTERFACE void drvADC_ADCAutoGainCalwithInput(void);
#endif
INTERFACE void drvADC_SetupHsyncPolarity(Bool IsActiveHigh);
INTERFACE void drvADC_AdjustCalDuring(BYTE u8CalDur);

INTERFACE void drvADC_SetHsyncRefLevel(ADC_HSYNCLVL_T level);

#undef INTERFACE

#endif //_DRVADC_H_

