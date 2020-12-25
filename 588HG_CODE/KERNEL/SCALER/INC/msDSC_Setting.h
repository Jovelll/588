
//===================================================================================================================================================================:
#ifndef _MSDSC_H_
#define _MSDSC_H_
//===================================================================================================================================================================:
#define SHARE_DLC_XDATA     0
//#define DPS_UserBrightControl_EN 1
//#define DPS_AggrPowerSaveMode_EN 1
//===================================================================================================================================================================:
//Fading Area setting
#define FadingArea_0 0
#define FadingArea_1  1

#define Fading_slop  							1
#define Fading_alpha  						5
#define Fading_area0_Cr_coordination  		0x1B8
#define Fading_area0_Cb_coordination  		0x249
#define Fading_area1_Cr_coordination  		0x262
#define Fading_area1_Cb_coordination  		0x1B8
#define Fading_Range  						0x20
//===================================================================================================================================================================:
// Extern XDATA variable from msDLC to share the parameter 
//===================================================================================================================================================================:
#if SHARE_DLC_XDATA
extern XDATA WORD  u16Array32[32];
extern XDATA BYTE  g_LowStrength;
extern XDATA BYTE  g_HighStrength;
extern XDATA DWORD g_dwFactory;
extern XDATA DWORD g_dwFactory_7;
extern XDATA DWORD g_dwFactory_9;
extern XDATA DWORD g_dwLstep;

// Define DPS project parameters
#define ISR_SatHistogram32H    u16Array32
#else
extern xdata WORD ISR_SatHistogram32H[32];
#endif
extern xdata BYTE  ISR_preDSCLoopCount;
extern xdata BYTE  ISR_curDSCLoopCount;
extern xdata BYTE  u8Win;
extern xdata BYTE  Flag_DSC_Latch_Busy;
extern xdata WORD  CurxvYgain ;
//extern xdata BYTE  REG_DPS_BW_MODE_EN;
//extern xdata BYTE  REG_DPS_BW_MODE_Gain;
extern xdata BOOL  REG_DSC_RUN_Flag;
extern void msDSC_Interrupt ( void );
extern void msDSC_On_Setting ( void );
extern void msDSC_Off_Setting ( void );
extern void msDSC_AdjustYgain ( WORD YgainValue ) ;
extern void msDSC_FadingArea_Setting (BYTE u8AresSelect, WORD u16Cr,WORD u16Cb, BYTE u8Range);

//===================================================================================================================================================================:
// DPS Debug Mode  
//===================================================================================================================================================================:
#define DEBUG_DSCSatgain 			0
#define DEBUG_SAT_HISTOGRAM 		0 
#define DSC_EnablexvYCC_ForDebug 	0 
#define DEBUG_DSC_withoutInterrupt 	0 
#define DEBUG_ColorMaxrix_Value   	0 

#define UseHistrogramToTest 			0 // 1:original y histrogram 0:Saturation Histrogram
#define DSC_UsexvYccColorMatrix_EN   1 // 0:3x3 1:xvycc 3x3

//===================================================================================================================================================================:
#else
extern void msDSCSettingDummy(void);
#endif
//===================================================================================================================================================================:

