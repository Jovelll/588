///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   MsADC.h
/// @author MStar Semiconductor Inc.
/// @brief  ADC Function
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MSADC_TSUMXXTX_H_
#define _MSADC_TSUMXXTX_H_

#ifdef _MSADC_TSUMXXTX_C_
#define MSADC_TSUMXXTXDEC
#else
#define MSADC_TSUMXXTXDEC	extern
#endif

#define MAX_PHASE_VALUE         	0x7F
#define MIN_PHASE_VALUE           	0

#include "Mdrv_adctbl.h"
//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

//#define DGAINCALWITHINPUT
//#define DGAINCALWITHOUTINPUT
#define DADCWORSTPHASE    0 // ADC find good phase criteria by worst phase
#define DADCPLLPROTECTION    1 // ADC PLL protection mechnism

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
    E_LPFBW_80MHz = 0x8,
    E_LPFBW_70MHz = 0x9,
    E_LPFBW_60MHz = 0xA,
    E_LPFBW_45MHz = 0xB,
    E_LPFBW_30MHz = 0xC,
    E_LPFBW_20MHz = 0xD,
    E_LPFBW_15MHz = 0xE,
    E_LPFBW_5MHz = 0xF,  
}ADCLPFBW_LIST;


//-------------------------------------------------------------------------------------------------
//  Function Prototype
//-------------------------------------------------------------------------------------------------

MSADC_TSUMXXTXDEC void msADC_init(Bool IsShareGrd);
MSADC_TSUMXXTXDEC void msADC_SetInputMux(InputPortType port_type );
MSADC_TSUMXXTXDEC void msADC_SetADCSource(ADC_SOURCE_TYPE inputsrc_type);
MSADC_TSUMXXTXDEC void msADC_SetADCModeSetting(ADC_INPUTSOURCE_TYPE enADCInput, WORD u16PixelClk);
MSADC_TSUMXXTXDEC void msADC_AdjustHTotal(WORD htotal);
MSADC_TSUMXXTXDEC void msADC_AdjustPhase(BYTE phase);
MSADC_TSUMXXTXDEC void msADC_AdjustAdcGain(WORD rcode, WORD gcode, WORD bcode);
MSADC_TSUMXXTXDEC void msADC_AdjustAdcOffset(WORD rcode, WORD gcode, WORD bcode);
MSADC_TSUMXXTXDEC void msADC_ADCOffsetGainMismatchCal(void);
MSADC_TSUMXXTXDEC Bool msADC_AutoPhaseAdjustment(BYTE vsyncTime);
MSADC_TSUMXXTXDEC Bool msADC_AutoAdcColor(BYTE vsyncTime);
MSADC_TSUMXXTXDEC void msADC_SetupHsyncPolarity(Bool bActiveHigh);
MSADC_TSUMXXTXDEC void msADC_AdjustCalDuring(BYTE u8CalDur);
MSADC_TSUMXXTXDEC void msADC_SetupADCLPF(ADCLPFBW_LIST val);

#if DADCPLLPROTECTION
MSADC_TSUMXXTXDEC Bool msADC_SetModewithPLLProtection(ADC_INPUTSOURCE_TYPE enADCInput, WORD u16PixelClk, WORD htotal);
#endif //#ifdef DADCPLLPROTECTION
#ifdef DGAINCALWITHOUTINPUT
MSADC_TSUMXXTXDEC void msADC_ADCAutoGainCalwithoutInput(void);
#endif
#ifdef DGAINCALWITHINPUT
MSADC_TSUMXXTXDEC void msADC_ADCAutoGainCalwithInput(void);
#endif

#endif //_MSADC_H_

