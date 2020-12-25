
///////////////////////////////////////
// DRVMSTAR_U VERSION: V01
////////////////////////////////////////
#ifndef _DRVMSTAR_U_H_
#define _DRVMSTAR_U_H_

#ifdef _DRVMSTAR_U_C_
#define _DRVMSTARDEC_
#else
#define _DRVMSTARDEC_   extern
#endif

_DRVMSTARDEC_ void msInitClockGating( void );
_DRVMSTARDEC_ void msTMDSInit(void);
_DRVMSTARDEC_ void drvmStar_Init( void );
_DRVMSTARDEC_ void mStar_InterlaceModeSetting( void );
_DRVMSTARDEC_ void mStar_SetPanelSSC( BYTE freqMod, BYTE range ); //2011.9.13 18:33 CC
_DRVMSTARDEC_ void mStar_IPPowerControl(void);
_DRVMSTARDEC_ void msTMDSSetMux( InputPortType inport );
_DRVMSTARDEC_ void I_Gen_Tuning( void );
_DRVMSTARDEC_ void Power_ModCtrl(BYTE ucSwitch);
_DRVMSTARDEC_ BYTE mStar_ScalerDoubleBuffer(Bool u8Enable);
_DRVMSTARDEC_ void mStar_SetTimingGen( WORD u16HDE_OP21, WORD u16VDE_OP21, BOOL OP21_FreeRun);
_DRVMSTARDEC_ void drvmStar_SetupFreeRunMode( void );
_DRVMSTARDEC_ void mStar_SetFrameDivider( BYTE vfreq_N );
_DRVMSTARDEC_ void mStar_SetIVS( void );
_DRVMSTARDEC_ void mStar_LPLL1_Setting( const DWORD kHzODCLK );
_DRVMSTARDEC_ void mStar_LPLL2_Setting( const DWORD kHzODCLK );
_DRVMSTARDEC_ void mStar_CheckFastClock(WORD u16hPeriod);
_DRVMSTARDEC_ void drvmStar_SetupInputPort_VGA(void);
_DRVMSTARDEC_ void drvmStar_SetupInputPort_DVI(void);
_DRVMSTARDEC_ BYTE GetVfreq( void );
#if ENABLE_FREESYNC
_DRVMSTARDEC_ void msDrv_forceIP1VsyncPolarityEnable (void);
#endif

typedef enum
{
    DVI_POWER_ON,
    DVI_POWER_STANDBY,
    DVI_POWER_DOWN

} DVI_PowerCtrlType;

_DRVMSTARDEC_ void drvDVI_PowerCtrl(DVI_PowerCtrlType ctrl);
#if ENABLE_HDMI
_DRVMSTARDEC_ void drvmStar_SetupInputPort_HDMI(void);
#endif

#if ENABLE_DP_INPUT
_DRVMSTARDEC_ void drvmStar_SetupInputPort_DisplayPort(void);
#endif

#if DVI_PORT||HDMI_PORT
_DRVMSTARDEC_ WORD mSTar_GetInputHTotal(void);
#endif
#if ENABLE_FREESYNC
_DRVMSTARDEC_ void msSetupDelayLine(void);
#endif

#if Enable_Expansion
_DRVMSTARDEC_ WORD GetImageOutH();
_DRVMSTARDEC_ WORD GetImageOutV();
#else
#define GetImageOutH()  PANEL_WIDTH
#define GetImageOutV()  PANEL_HEIGHT
#endif

//#define V_FREQ_IN       (((VFreq( HFreq( SrcHPeriod ), SrcVTotal )+5)/10) * ((CURRENT_SOURCE_IS_INTERLACE_MODE())?2:1))
#define IVS_DEFAULT     8
#define VFREQ_4MFRC     30 // If VFreqIn <= VFREQ_4MFRC, do FRC
#endif
