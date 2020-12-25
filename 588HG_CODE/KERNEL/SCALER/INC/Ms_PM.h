
#if (MS_PM)

#ifndef MS_PM_H
#define MS_PM_H

#ifdef _MS_PM_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif

#if (MainBoardType==MainBoard_MST125B || MainBoardType == MainBoard_MST125A_A01B_S || MainBoardType == MainBoard_MST250B_D01A_S)
#include "TSUMU_DEMO_PMSET.h"
#elif MainBoardType==MainBoard_MST139A && CHIP_ID==CHIP_TSUMY
#include "TSUMY_DEMO_PMSET.h"
#elif MainBoardType==MainBoard_MST139A && CHIP_ID==CHIP_TSUMB
#include "TSUMB_DEMO_PMSET.h"
#elif MainBoardType==MainBoard_MST135B
#include "TSUMV_DEMO_PMSET.h"
#elif  MainBoardType==MainBoard_5281_M0A
#include "OTS_VS230PGLW_PMSET.h"
#elif MainBoardType==MainBoard_5270_M0A
#include "OTS_VS215DHLW_PMSET.h"
#elif MainBoardType==MainBoard_5269_M0A
#include "TSUMY_DEMO_PMSET.h"
#elif MainBoardType == MainBoard_MST144A_A02A
#include "TSUM2_DEMO_PMSET.h"
#elif MainBoardType == MainBoard_TSUML887DC5 || MainBoardType == MainBoard_TSUMP387MT9_1_DP || \
	MainBoardType == MainBoard_TSUMP387MT9_1 || MainBoardType == MainBoard_tsuml88bdc2_1 || \
	MainBoardType == MainBoard_TSUML88CMC2 || MainBoardType == MainBoard_TSUML18CN2 || MainBoardType == MainBoard_TSUML58CN3 ||\
	MainBoardType == MainBoard_TSUML887DS_1 || MainBoardType == MainBoard_TSUMOP38CMT9_HHD || \
	MainBoardType == MainBoard_TSUMOP38CMT9_HDH || MainBoardType == MainBoard_TSUMO58CMG_H || \
	MainBoardType == MainBoard_TSUMO58CMG_DP || MainBoardType == MainBoard_TSUMO58FDT9_VP58C41_B ||\
	MainBoardType == MainBoard_TSUMP589MG_DP || MainBoardType == MainBoard_TSUMU589MT9_H || MainBoardType == MainBoard_TSUMP589HT9_DP || \
	MainBoardType == MainBoard_TSUMP589MG_H ||MainBoardType == MainBoard_TSUMO58FMG_H_ || MainBoardType == MainBoard_TSUMOP38CMT9_1A3C ||\
	MainBoardType == MainBoard_TSUMOLP88CDC5 || MainBoardType == MainBoard_TSUMO58FDT9_1A3C || MainBoardType ==MainBoard_TSUMO58FDT9_VP58C11 ||\
	MainBoardType == MainBoard_TSUMU88CDT9_V8811||MainBoardType == MainBoard_TSUMO58FDT9_VSP58C31_3_IN_1||MainBoardType == ZD_588HGV00R0XCOOB05
	#if CHIP_ID == CHIP_TSUMC
	#include "TSUMC_DEMO_PMSET.h"
   	#elif CHIP_ID == CHIP_TSUMD
	#include "TSUMD_DEMO_PMSET.h"
   	#elif CHIP_ID == CHIP_TSUMJ
	#include "TSUMJ_DEMO_PMSET.h"
	#elif CHIP_ID == CHIP_TSUM9
	#include "TSUM9_DEMO_PMSET.h"
	#elif CHIP_ID == CHIP_TSUMF
	#include "TSUMF_DEMO_PMSET.h"	
	#elif CHIP_ID == CHIP_TSUMK
	#include "TSUMK_DEMO_PMSET.h"
   	#endif
#else
#message "please implement PMSET for new board"
#endif

typedef enum
{
    ePM_POWERON,
    ePM_STANDBY,
    ePM_POWEROFF,

    ePM_INVAILD
}ePM_Mode;

typedef enum
{
    ePMSTS_NON,
    ePMSTS_VGA_ACT,
#if( PM_SUPPORT_WAKEUP_DVI )
    ePMSTS_DVI_0_ACT,
    ePMSTS_DVI_1_ACT,
#endif
    ePMSTS_SAR_ACT,
    ePMSTS_GPIO_ACT,
    ePMSTS_MCCS04_ACT,
    ePMSTS_MCCS05_ACT,
    ePMSTS_MCCS01_ACT,
    ePMSTS_CEC_ACT,
    ePMSTS_FORCE_ON,
#if( PM_SUPPORT_WAKEUP_DP )
    ePMSTS_DP_ACT,
#endif
 #if (PM_POWERkEY_GETVALUE)
    ePMSTS_POWERGPIO_ACT,
 #endif
 #if(PM_CABLE_DETECT_USE_SAR)
   ePMSTS_CABLESAR_ACT,
  #endif
  ePMSTS_ITE_POWER_KEY_ACT,    //  11
  ePMSTS_ITE_SOURCE_KEY_ACT,
  ePMSTS_ITE_FACTORY_KEY_ACT,	//120523 Modify
  ePMSTS_ITE_OSDLOCK_KEY_ACT,	//120524 Modify
      ePMSTS_MHL_SOURCE_ACT,
      ePMSTS_GPIO_POWER_KEY_ACT,
#if ENABLE_MHL
    ePMSTS_MHL_ACT,
#endif
#if (Customer_Type_Sel==Customer_XINTAO)
	ePMSTS_ITE_BURNIN_KEY_ACT,
#endif
    ePMSTS_INVAID
}ePMStatus;

typedef enum
{
    ePMDVI_DVI0,
    ePMDVI_DVI1,
    ePMDVI_DVI0_DVI1,

    ePMDVI_INVALID
}ePM_DVI;

typedef enum
{
    ePMGPIO04_DectDVI5V,
    ePMGPIO02_DectDVI5V,
    ePMGPIO01_DectDVI5V,
    ePMGPIO00_DectDVI5V,
    ePMDVI5V_INVALID
}ePM_DVI5V;

typedef enum
{
    ePMSAR_SAR0 = BIT0,
    ePMSAR_SAR1 = BIT1,
    ePMSAR_SAR2 = BIT2,
    ePMSAR_SAR3 = BIT3,
    ePMSAR_SAR12 = BIT1|BIT2,
    ePMSAR_SAR123 = BIT1|BIT2|BIT3,
    ePMSAR_SAR01 = BIT0|BIT1,
    ePMSAR_SAR012 = BIT0|BIT1|BIT2,
    ePMSAR_INVALID
}ePM_SAR;

typedef enum
{
    ePM_ENTER_PM=0,
    ePM_EXIT_PM=1,
    ePM_WAIT_EVENT=2,
    ePM_IDLE=3
}ePM_State;

typedef struct
{
    BYTE bHVSync_enable:1;
    BYTE bSOG_enable:1;
    BYTE bGPIO_enable:1;
    BYTE bSAR_enable:1;
    BYTE bMCCS_enable:1;
    BYTE bEDID_enable:1;
#if( PM_SUPPORT_WAKEUP_DVI )
    BYTE bDVI_enable:1;
#endif  // end of #if( PM_SUPPORT_WAKEUP_DVI )
#if 1//( PM_SUPPORT_WAKEUP_DP )&&(ENABLE_DP_INPUT)
    BYTE bDP_enable:1;
#endif  // end of #if( PM_SUPPORT_WAKEUP_DP )
#if( PM_SUPPORT_AC2DC )
    BYTE bACtoDC_enable:1;
#endif  // end of #if( PM_SUPPORT_AC2DC )
    BYTE bMCUSleep:1;
    ePM_DVI ePMDVImode;
    ePM_SAR ePMSARmode;
}sPM_Config;

typedef struct
{
    BYTE ucPMMode;
    ePM_State ePMState;
    sPM_Config  sPMConfig;
    #if CABLE_DETECT_VGA_USE_SAR||CABLE_DETECT_VGA_USE_SAR
     BYTE bCABLE_SAR_VALUE;
    #endif

}sPM_Info;

typedef enum
{
    ePM_CLK_RCOSC,
    ePM_CLK_XTAL,

    ePMCLK_INVALID
}ePM_CLK;

#define ConvPercent(x)  ( (WORD)(x)*255/100)

// ACDET low voltage threshold // 0x66
#define Low_Volt_1_20 0x00<<2
#define Low_Volt_1_25 0x01<<2
#define Low_Volt_1_15 0x10<<2
#define Low_Volt_1_10 0x11<<2
// ACDET high voltage threshold // 0x66
#define High_Volt_1_40 0x00
#define High_Volt_1_45 0x01
#define High_Volt_1_50 0x10
#define High_Volt_1_55 0x11

//=============================
#define SAR_Ch          SAR2
//------------------------------------------------
#define SAR0     0
#define SAR1     1
#define SAR2     2
#define SAR3     3
//------------------------------------------------
#define SAR_KEY_TH      0x30

//=============================
#define LEDPWM              LEDPWM1
#define LED_PWM_EN      LED_PWM_Amber
#define PWMDUTY_Val     ConvPercent(2) //%
#define PWMDIV_Val          0xFF // 97.16Hz

//=============================
//#define SAR1   BIT0
//#define SAR2   BIT1
//#define SARALL SAR2|SAR1
//=============================
#define LEDPWM0 0
#define LEDPWM1 1
#define LEDPWM2 2
#define LEDPWM3 3
#define LED_PWM_SEL_P07     LEDPWM<<5
#define LED_PWM_SEL_P06     LEDPWM<<3  // P06 & P15 as same
#define LED_PWM_SEL     LED_PWM_SEL_P07|LED_PWM_SEL_P06

// PWM0 0xC2,0xC3 ; PWM1 0xC4,0xC5
#if (LEDPWM==LEDPWM1)
#define PWMDIV      0xC4
#define PWMDUTY   0xC5
#elif (LEDPWM==LEDPWM0)
#define PWMDIV      0xC2
#define PWMDUTY   0xC3
#elif (LEDPWM==LEDPWM2)
#define PWMDIV      0xC6
#define PWMDUTY   0xC7
#elif (LEDPWM==LEDPWM3)
#define PWMDIV      0xC6
#define PWMDUTY   0xC7
#endif

//=============================
#define LED_P07_EN       	BIT1
#define LED_P06_EN        	BIT0
#define LED_P15_EN         	BIT0
#define LED_PWM_Green       LED_P06_EN
#define LED_PWM_Amber       LED_P07_EN

//**************************************************************************
//  RCOSC = XTAL * Counter / 512 => Counter = RCOSC *512/XTAL,
//  IF RCOSC=4M, Counter=143 =>8Fh
//  CHIP_TSUMC uses FRO_12M /3 =4M to calibration ==> Counter=143
//**************************************************************************
#define RCOSC_CLK       4
#define RCOSC_TARGET    143
#define RCOSC       	0
#define XTAL        	1
#define XTALDIV2    	2
#define XTALDIV4    	3

//--------PM Option----------------
//#define PM_CLOCK                XTAL
#if PM_CLOCK==RCOSC
#define PM_TIMER_CLOCK_PERIOD (0x10000-265) //more precise
//#define PM_TIMER_CLOCK_PERIOD   (0x10000-((((unsigned long)14318*RCOSC_TARGET/512)*INT_PERIOD)/12))
#define PM_FORCE_DELAY_DIV      8
#define PM_DELAY4US_LOOP        1
#elif PM_CLOCK==XTAL
#define PM_TIMER_CLOCK_PERIOD   (0x10000-(((unsigned long)14318*INT_PERIOD)/12))
#define PM_FORCE_DELAY_DIV      2
#define PM_DELAY4US_LOOP        4
#elif PM_CLOCK==XTALDIV2
#define PM_TIMER_CLOCK_PERIOD   (0x10000-(((unsigned long)14318/2*INT_PERIOD)/12))
#define PM_FORCE_DELAY_DIV      4
#define PM_DELAY4US_LOOP        2
#elif PM_CLOCK==XTALDIV4
#define PM_TIMER_CLOCK_PERIOD   (0x10000-(((unsigned long)14318/4*INT_PERIOD)/12))
#define PM_FORCE_DELAY_DIV      8
#define PM_DELAY4US_LOOP        1
#endif

INTERFACE BYTE xdata Check,Check1;
#if ENABLE_DP_INPUT
extern bit g_bDoDPInit;
#endif
extern bit g_bMcuPMClock;

INTERFACE Bool msPM_IsState_IDLE(void);

// setting configuration meta data
INTERFACE void msPM_Init(void);
INTERFACE void msPM_SetFlag_Standby(void);
INTERFACE void msPM_SetFlag_PMDCoff(void);
#if 0
INTERFACE void msPM_InputForceMode();
INTERFACE void msPM_SetFlag_Standby_ForceVGA(void);
INTERFACE void msPM_SetFlag_Standby_ForceDVI0(void);
INTERFACE void msPM_SetFlag_Standby_ForceDVI1(void);
INTERFACE void msPM_SetFlag_Standby_ForceAllPort(void);
#endif

#if 0
INTERFACE void msPM_PowerDownMacro(void);
INTERFACE Bool msPM_StartRCOSCCal(void);
INTERFACE Bool msPM_GetPMStatus(void);
INTERFACE void msPM_SetPMClock(void);
INTERFACE void msPM_PassWord(Bool bEnable);
#endif
INTERFACE void msPM_ClearStatus(Bool bResetPM);
INTERFACE void msPM_Handler(void);
INTERFACE void msPM_WaitingEventActivity(void);
//INTERFACE void msPM_InitWaitingEventActivity(void);
INTERFACE void msPM_PMWakeupInit(void);

#if 0
INTERFACE BYTE msPM_Reset(void);
#endif

INTERFACE void msPM_Exit(void);
INTERFACE void msPM_SetPMMode(void);
INTERFACE void msPM_WaitingEvent(void);
INTERFACE Bool msPM_Checkagain(void);
INTERFACE void msPM_InterruptEnable(Bool benable);

#if  0
INTERFACE void msPM_SetConfigurationMeta(
            BOOL bHW_enable, BOOL bMCUSleep, BOOL bXTALOn_enable, BOOL bCorePWOn_enable,
            BOOL bHVSync_enable, BOOL bSOG_enable, BOOL bGPIO_enable,
            BOOL bSAR_enable, BOOL bMCCS_enable, BOOL bEDID_enable,
#if( PM_SUPPORT_WAKEUP_DVI )
            BOOL bDVI_enable,
#endif
#if( PM_SUPPORT_WAKEUP_DP )
            BOOL bDP_enable,
#endif
#if( PM_SUPPORT_AC2DC )
            BOOL bACtoDC_enable,
#endif
            BYTE ePMDVImode, BYTE ePMSARmode, BYTE ePMPMCLK, BYTE ePMLiveCLK);
#endif

INTERFACE void msPM_Functions(void);
//INTERFACE void msPM_ForceExit(void);
INTERFACE void msPM_ForceEnter(void);

INTERFACE void msPM_EnableSOGDetect(Bool bEnable);
INTERFACE void msPM_EnableSARDetect(Bool bEnable);
INTERFACE void msPM_EnableGPIODetect(Bool bEnable);
INTERFACE void msPM_EnableMCCSDetect(Bool bEnable);
INTERFACE void msPM_EnableHVSyncDetect(Bool bEnable);
#if( PM_SUPPORT_AC2DC )
INTERFACE void msPM_EnableAC2DC(Bool bEnable);
#endif//end of #if( PM_SUPPORT_AC2DC )
#if (ENABLE_DVI || ENABLE_HDMI)
#if( PM_SUPPORT_WAKEUP_DVI )
INTERFACE void msPM_EnableDVIDetect(Bool bEnable);
#endif//end of #if( PM_SUPPORT_WAKEUP_DVI )
#endif
INTERFACE void msPM_EnableDPDetect(BOOL bEnable);

//function for time sharing feature
INTERFACE void msPM_EnableDVIClockAmp (Bool bEnable);

INTERFACE void msPM_OutputTriState(void);
//void msPM_SetupWakeUpFunc(void);
//void msPM_printConfiguration(void);

INTERFACE void msPM_MCCSReset(void);
INTERFACE Bool DDCCI_CheckDMPSON(void);
INTERFACE Bool msPM_StartRCOSCCal(void);
INTERFACE void msPM_SetPMClockLive(BYTE);
INTERFACE void msPM_PassWord(BOOL);

#undef INTERFACE
#endif

#elif (CHIP_ID==CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK) || (CHIP_ID==CHIP_TSUMD) || (CHIP_ID == CHIP_TSUMJ) // #elif of #if (MS_PM)
#ifndef MS_PM_H
#define MS_PM_H
#ifdef _MS_PM_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif

//**************************************************************************
//  RCOSC = XTAL * Counter / 512 => Counter = RCOSC *512/XTAL,
//  IF RCOSC=4M, Counter=143 =>8Fh
//  CHIP_TSUMC uses FRO_12M /3 =4M to calibration ==> Counter=143
#define RCOSC_TARGET    143 //4MHz
INTERFACE Bool msPM_StartRCOSCCal(void);
#undef INTERFACE
#endif	// #endif of #ifndef MS_PM_H

#elif (CHIP_ID==CHIP_TSUM9 ||CHIP_ID==CHIP_TSUMF)
#ifndef MS_PM_H
#define MS_PM_H
#ifdef _MS_PM_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE void msPM_Exit(void);
#undef INTERFACE
#endif	// #endif of #ifndef MS_PM_H

#endif

