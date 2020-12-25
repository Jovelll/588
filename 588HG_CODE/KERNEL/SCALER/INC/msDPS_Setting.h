
//===================================================================================================================================================================:
#ifndef _MSDPS_H_
#define _MSDPS_H_
//===================================================================================================================================================================:
#define SHARE_DLC_XDATA     0
#define DPS_UserBrightControl_EN 1
#define DPS_AggrPowerSaveMode_EN 1
#define DPS_YminForWindowSave_EN 0
#define DPS_HistPseuMax_MOD1_EN  0
#define DPS_HistPseuMax_MOD2_EN  0 
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
#define ISR_LumaHistogram32H    u16Array32
#define ISR_HistogramTotalPix   g_dwFactory
#define ISR_HistogramTotalLuma  g_dwFactory_7
#define ISR_HistogramMax        g_HighStrength
#define ISR_HistogramMin        g_LowStrength  
//#define ucWin DLCStatus
#else

extern xdata WORD ISR_LumaHistogram32H[32];
extern xdata WORD  ISR_HistogramTotalPix; // >>6
extern xdata WORD  ISR_HistogramTotalLuma; // >>14
extern xdata BYTE ISR_HistogramMax;
extern xdata BYTE ISR_HistogramMin;

#endif
extern xdata WORD  REG_DPS_UserBrightContlGain;// 0~256: 0~100%

//===============DPS_AggrPowerSaveMode===============================================================================================================================:
extern xdata WORD  REG_DPS_UserBrightLimitGain;
extern xdata WORD  REG_DPS_AggressiveYgainGain;//  256   // 0~256: 0~100%
extern xdata WORD  REG_DPS_AggressivePdutyGain;//  256   // 0~256: 0~100%
extern xdata WORD  REG_DPS_AggressiveGainRange;//  240   // apply AggressiveGain when ImgYmax < Range
//===================================================================================================================================================================:
extern xdata BYTE  ISR_preLoopCount;
extern xdata BYTE  ISR_curLoopCount;
extern xdata BYTE  ucWin;
extern xdata BYTE  Flag_DPS_Latch_Busy;
extern xdata WORD  CurYgain ;
extern xdata BYTE  CurBacklight ;
extern xdata BYTE  REG_DPS_YOUT_ENABLE;
extern xdata BYTE  REG_DPS_BW_MODE_EN;
extern xdata BYTE  REG_DPS_BW_MODE_Gain;
extern xdata BOOL  REG_DPS_RUN_Flag;
extern BYTE code DPS_Lumi_Table[17][11];
#if DPS_YminForWindowSave_EN
extern xdata BYTE  REG_DPS_PowerLevel;
#endif
#if DPS_HistPseuMax_MOD1_EN
extern xdata BYTE  REG_MouseCount;
#endif
extern void msDPS_Interrupt ( void );
extern void msDPS_On_Setting ( void );
extern void msDPS_Off_Setting ( void );
extern void msDPS_AdjustYgain ( WORD YgainValue ) ;
extern void msDPS_AdjustBacklight ( BYTE BLValue );
//===================================================================================================================================================================:
// DPS Debug Mode  
//===================================================================================================================================================================:
#define DEBUG_Ygain 0
#define DEBUG_PWM 0
#define DEBUG_HISTOGRAM 0 
#define DEBUG_HISTOGRAM_PAM 0 
//===================================================================================================================================================================:
#else
extern void msDPSSettingDummy(void);
#endif
//===================================================================================================================================================================:

