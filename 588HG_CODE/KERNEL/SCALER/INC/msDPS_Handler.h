//===================================================================================================================================================================:
#ifndef _MSDPS_Hand_H_
#define _MSDPS_Hand_H_
//===================================================================================================================================================================:

//#define DPS_AggrPowerSaveMode_EN 0
//#define DPS_YminForWindowSave_EN 1
#define ABS2(x,y) (x>y)?(x-y):(y-x)
#define MAX2(x,y) (x>y)? x : y
#define MIN2(x,y) (x>y)? y : x

//===================================================================================================================================================================:
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
///// DPS Common Registers ///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//#if DPS_YminForWindowSave_EN
#define  REG_DPS_WindowSaveDensityTH  200
#define  REG_DPS_WindowSaveYavgLimit  16
#define  REG_DPS_WindowSaveYgain      1500
#define  REG_DPS_WindowSaveHisWeight_0_Up       30000
#define  REG_DPS_WindowSaveHisWeight_0_Down     35000  
#define  REG_DPS_WindowSaveHisWeight_31_Up      65
#define  REG_DPS_WindowSaveHisWeight_31_Down    80
//#endif
#define  REG_DPS_PowerLimitHW  3    // 0~ 10: 0~100% (ToDo: assigned by HW setting)
#define  REG_DPS_PowerLimitFW  3    // 0~ 10: 0~100% (ToDo: assigned by DPS table)
#define  REG_DPS_RowStart  0
//#define  REG_DPS_ColStart  0
#define  REG_DPS_LevelLimit  255

#define  REG_DPS_ImgYmaxForPureColor  255
#define  REG_DPS_ImgYminForPureColor  32
#define  REG_DPS_ImgYmaxForNatureImg  255
#define  REG_DPS_ImgYminForNatureImg  80

#define  REG_DPS_HistDiffCountThd  3

#define  REG_DPS_PureColorDstyThd  230
#define  REG_DPS_PureColorDiffThd  8

#define  REG_DPS_StepTargetDelta  8
#define  REG_DPS_StepIncCountThd  6
#define  REG_DPS_StepDecCountThd  8

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
///// DPS SceneChange Registers //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//--SceneChange Level 0 (Disable)
#if 0
#define  REG_DPS_SceChgMaxDiffThd  255
#define  REG_DPS_SceChgAvgDiffThd  255
#define  REG_DPS_SceChgHistWgtThd  255
#define  REG_DPS_SceChgHistCntThd  255
#endif

//--SceneChange Level 1 (Default)
#if 1
#define  REG_DPS_SceChgMaxDiffThd  96
#define  REG_DPS_SceChgAvgDiffThd  32
#define  REG_DPS_SceChgHistWgtThd  128
#define  REG_DPS_SceChgHistCntThd  25
#endif

//--SceneChange Level 2 (Aggressive)
#if 0
#define  REG_DPS_SceChgMaxDiffThd  64
#define  REG_DPS_SceChgAvgDiffThd  16
#define  REG_DPS_SceChgHistWgtThd  64
#define  REG_DPS_SceChgHistCntThd  20
#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
///// DPS AdaptiveUpdateScheme Registers /////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//--AdaptiveUpdateScheme (Disable)
#if 0
#define  REG_DPS_UpdateFrameCyc  120
#define  REG_DPS_YgainUpdateCyc  0
#define  REG_DPS_PdutyUpdateCyc  0
#define  REG_DPS_YgainDiffMinTH  2
#define  REG_DPS_PdutyDiffMinTH  1
#endif

//--AdaptiveUpdateScheme (Default)
#if 1
#define  REG_DPS_UpdateFrameCyc  60
#define  REG_DPS_YgainUpdateCyc  0
#define  REG_DPS_PdutyUpdateCyc  0
#define  REG_DPS_YgainDiffMinTH  3
#define  REG_DPS_PdutyDiffMinTH  1
#endif

//--AdaptiveUpdateScheme (Aggressive)
#if 0
#define  REG_DPS_UpdateFrameCyc  30
#define  REG_DPS_YgainUpdateCyc  0
#define  REG_DPS_PdutyUpdateCyc  0
#define  REG_DPS_YgainDiffMinTH  4
#define  REG_DPS_PdutyDiffMinTH  1
#endif

//===================================================================================================================================================================:
extern void msDPS_Handler ( void );
#if 0
extern WORD msDPS_Ver(void);
#endif

#else
extern void msDPSHandlerDummy(void);
#endif
//===================================================================================================================================================================:

