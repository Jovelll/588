
///////////////////////////////////////
// DRVMSTAR_V VERSION: V01
////////////////////////////////////////
#ifndef _DRVMSTAR_V_H_
#define _DRVMSTAR_V_H_

#ifdef _DRVMSTAR_V_C_
#define _DRVMSTARDEC_
#else
#define _DRVMSTARDEC_   extern
#endif


#ifndef USE_MOD_HW_CAL
#define USE_MOD_HW_CAL        (1)   // coding test// choose hardware(1) or software(0) MOD calibration
#endif

#ifndef I_GEN_CH //use a defined port
#if (LVDS_CHANNEL&MOD_PORTA)
#define I_GEN_CH  CH3_CLK
#elif (LVDS_CHANNEL&MOD_PORTB)
#define I_GEN_CH  CH9_CLK
#elif (LVDS_CHANNEL&MOD_PORTC)
#define I_GEN_CH  CH15_CLK
#elif (LVDS_CHANNEL&MOD_PORTD)
#define I_GEN_CH  CH21_CLK
#endif
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
_DRVMSTARDEC_ void mStar_SetScalingFactor( void );
_DRVMSTARDEC_ void mStar_SetDClkPLL(DWORD u32ODCLK_KHZ);
_DRVMSTARDEC_ void mStar_CheckFastClock(WORD u16SCLK, WORD u16DCLK);
_DRVMSTARDEC_ void mStar_SetScalingFilter(void);
_DRVMSTARDEC_ void drvmStar_SetupFreeRunMode( void );
_DRVMSTARDEC_ void drvmStar_SetupInputPort_VGA(void);
_DRVMSTARDEC_ void drvmStar_SetupInputPort_DVI(void);
_DRVMSTARDEC_ void mStar_SetTimingGen( WORD u16HDE_OP21, WORD u16VDE_OP21, BOOL OP21_FreeRun);
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
#endif
