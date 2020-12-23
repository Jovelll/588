#ifndef _MSTAR_H_
#define _MSTAR_H_

#ifdef _MSTAR_C_
#define _MSTARDEC_
#else
#define _MSTARDEC_  extern
#endif

#define BW_SCREEN_OFF   0
#define BW_SCREEN_WHITE 1
#define BW_SCREEN_BLACK 2

_MSTARDEC_ BYTE xdata iGenTuningFinished;
//_MSTARDEC_ void I_Gen_Tuning( void );
_MSTARDEC_ void mStar_ACOnInit(void);
_MSTARDEC_ void mStar_Init( void );
_MSTARDEC_ Bool mStar_SetupMode( void );
_MSTARDEC_ void mStar_SetupFreeRunMode( void );
_MSTARDEC_ void mStar_PrepareForTimingChange( void );
_MSTARDEC_ void mStar_SetupInputPort( void );
_MSTARDEC_ void mStar_SetAnalogInputPort( Bool ToSOGPort );
//_MSTARDEC_ void mStar_SetScalingFilter( void );
_MSTARDEC_ Bool mStar_SetCaptureWindow( void );
//_MSTARDEC_ void mStar_ResetDClkPLL( void );
_MSTARDEC_ void mStar_PowerDown( void );
_MSTARDEC_ void mStar_PowerUp( void );
_MSTARDEC_ void ScalerReset( void );
//_MSTARDEC_ void mStar_SetScalingFactor( void );
_MSTARDEC_ void GPIOPWMSelect( void );
//_MSTARDEC_ void CheckADC( void ); // check DVI phase overflow
#if !ENABLE_LED_CONTROLLER
_MSTARDEC_ void SetPWMFreq( WORD freq );
#endif
#if BrightFreqByVfreq
_MSTARDEC_ void SetFourtimesPWMFreq(void);
#endif
//_MSTARDEC_ void mStar_SetPanelSSC( void ); // Rex 100706
//_MSTARDEC_ void ShortLineTune( bit disable ); // Rex 100706

_MSTARDEC_ void mStar_MCU_Clock_Select(Bool IsNormal);
_MSTARDEC_ void mStar_EnableModeChangeINT(Bool u8Enable);

//_MSTARDEC_ Bool mStar_SetPanelTiming(void);
//_MSTARDEC_ BYTE mStar_ScalerDoubleBuffer(Bool u8Enable);
_MSTARDEC_ void mStar_BlackWhiteScreenCtrl(BYTE u8Ctrl);
//_MSTARDEC_ void mStar_SetDClkPLL(DWORD u32ODCLK_KHZ);
_MSTARDEC_  BYTE  IsColorspaceRGB(void);
_MSTARDEC_  BYTE  IsColorspaceRGBInput(void);
_MSTARDEC_ WORD GetHSyncWidth(void);
_MSTARDEC_ void SetInputColorFormat( void );

//#if ENABLE_HDMI
_MSTARDEC_  void  SettingInputColorimetry(void);
//#endif

#if Enable_Expansion
_MSTARDEC_ void IsSupportedAspRatioMode(WORD u16width, WORD u16height);
#endif

	
#if (CHIP_ID==CHIP_TSUM9 ||CHIP_ID==CHIP_TSUMF)
_MSTARDEC_ void msTrimFROFromEfuse(void);
#if (ENABLE_XTAL_LESS)
_MSTARDEC_ Bool msStartRCOSC432MCal(void);
#endif
#endif

#endif
