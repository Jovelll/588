
///////////////////////////////////////
// DRVMSTAR_V VERSION: V01
////////////////////////////////////////
#ifndef _DRVMSTAR_2_H_
#define _DRVMSTAR_2_H_

#ifdef _DRVMSTAR_2_C_
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
#if ENABLE_HDMI
_DRVMSTARDEC_ void drvmStar_SetupInputPort_HDMI(void);
#endif

typedef enum
{
    DVI_POWER_ON,
    DVI_POWER_STANDBY,
    DVI_POWER_DOWN

} DVI_PowerCtrlType;

_DRVMSTARDEC_ void drvDVI_PowerCtrl(DVI_PowerCtrlType ctrl);



#define SSC_STEP  0x00B2//0x0124
#define SSC_SPAN  0x013A//0x0050 
#define LPLL_SSC  _DISABLE
//#define V_FREQ_IN       (((VFreq( HFreq( SrcHPeriod), SrcVTotal)+5)/10) * ((SrcFlags&bInterlaceMode)?2:1))
#define IVS_DEFAULT     22//19 //12
#define VFREQ_4MFRC     30 // If VFreqIn <= VFREQ_4MFRC, do FRC

#define FRAMEPLL_TIMEOUT    3000 // unit: mS
#define WaitOutputFpllStable()  {SetTimOutConter(FRAMEPLL_TIMEOUT);while(((msRead2Byte(REG_3854)&0x1F4) !=0x1F4 )&& (bTimeOutCounterFlag));}//if(!DDCCI_MCCS_CMD_FLAG) {g_u16u16DelayCounter=5000;while(!(msRead2Byte(REG_3854)==0x1F4) && (g_u16u16DelayCounter));}

typedef struct _RegTblType
{
    WORD wReg;
    BYTE ucValue;
    BYTE ucMask;
}RegTblType;
typedef enum _FSyncMode
{
    FLM_FBL,
    FLM_FB_FPLL_NOLOCK,
    FLM_FB_FPLL,
}FSyncMode;

_DRVMSTARDEC_ void msWritePowerOnTble(void);
_DRVMSTARDEC_ void msSetFrameSyncMode(FSyncMode ucFrameLockMode);
_DRVMSTARDEC_ void mStar_SetFrameDivider( BYTE vfreq_N );
_DRVMSTARDEC_ BYTE GetVfreq( void );
_DRVMSTARDEC_ WORD mStar_SetIVS( void );
_DRVMSTARDEC_ Bool msDVIEQCalibration(void);
_DRVMSTARDEC_ void mStar_SetTimingGen( WORD u16HDE_OP21, WORD u16VDE_OP21, BOOL OP21_FreeRun);
#if ENABLE_FREESYNC
_DRVMSTARDEC_ void msDrv_forceIP1VsyncPolarityEnable (void);
#endif




#undef _DRVMSTARDEC_
#endif
