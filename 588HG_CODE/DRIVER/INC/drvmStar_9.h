///////////////////////////////////////
// DRVMSTAR_V VERSION: V01
////////////////////////////////////////

#ifndef _DRVMSTAR_9_H_
#define _DRVMSTAR_9_H_

#ifdef _DRVMSTAR_9_C_
#define _DRVMSTARDEC_
#else
#define _DRVMSTARDEC_   extern
#endif

#ifndef USE_MOD_HW_CAL
#define USE_MOD_HW_CAL        (1)   // coding test// choose hardware(1) or software(0) MOD calibration
#endif

#if ENABLE_AUTOEQ
extern Bool xdata AUTOEQ_FLAG;
#endif


#ifndef I_GEN_CH //use a defined port
  #if PanelDualPort!=0
    #define I_GEN_CH  CH2
  #else
    #if PanelSwapPort!=0
      #define I_GEN_CH  CH6 // need check
    #else
      #define I_GEN_CH  CH2 // need check
    #endif
  #endif
#endif

#ifndef LVDS_XSWING_CH //LVDS double swing ch
#define LVDS_XSWING_CH  0 // Bit0~Bit9 maps to ch0~ch9
#endif

#define HDMI_VCO_LOOP_DIV2              145
#define HDMI_VCO_LOOP_DIV4              72
#define HDMI_VCO_LOOP_DIV8              36
#define HDMI_VCO_CODE_CALCULATE(a)      (a *64 /14.318)
#define HDMI_EQ_VALUE                   0x0A
#define HDMI_R_CHANNEL_EQ               HDMI_EQ_VALUE
#define HDMI_G_CHANNEL_EQ               HDMI_EQ_VALUE
#define HDMI_B_CHANNEL_EQ               HDMI_EQ_VALUE
#define HDMI_CLK_CHANNEL_EQ             HDMI_EQ_VALUE
#define HDMI_PLL_LDO_VALUE              BIT12
#define ENABLE_FRO12M_CAL               (0 && !ENABLE_XTAL_LESS) //1 or 0 1:K FRO freq, 0:FRO get from Efuse

_DRVMSTARDEC_ void msInitClockGating( void );
_DRVMSTARDEC_ void msTMDSInit(void);
_DRVMSTARDEC_ void TMDSISR(void);
_DRVMSTARDEC_ void drv_TMDS_DATA_ALIGN(void);
_DRVMSTARDEC_ void drvmStar_Init( void );
_DRVMSTARDEC_ void mStar_InterlaceModeSetting( void );
_DRVMSTARDEC_ void mStar_SetPanelSSC( BYTE freqMod, BYTE range ); //2011.9.13 18:33 CC
_DRVMSTARDEC_ void mStar_IPPowerControl( void );
_DRVMSTARDEC_ void msTMDSSetMux( InputPortType inport );
_DRVMSTARDEC_ void I_Gen_Tuning( void );
_DRVMSTARDEC_ void Power_ModCtrl( BYTE ucSwitch );
_DRVMSTARDEC_ BYTE mStar_ScalerDoubleBuffer( Bool u8Enable );
_DRVMSTARDEC_ void mStar_SetScalingFactor( void );
_DRVMSTARDEC_ void mStar_SetDClkPLL( DWORD u32ODCLK_KHZ );
_DRVMSTARDEC_ void mStar_CheckFastClock( WORD u16SCLK, WORD u16DCLK );
_DRVMSTARDEC_ void mStar_SetScalingFilter(void);
_DRVMSTARDEC_ void drvmStar_SetupFreeRunMode( void );
_DRVMSTARDEC_ void mStar_SetTimingGen( WORD u16HDE_OP21, WORD u16VDE_OP21, BOOL OP21_FreeRun);
_DRVMSTARDEC_ void drvmStar_SetupInputPort_VGA( void );
_DRVMSTARDEC_ void drvmStar_SetupInputPort_DVI( void );
_DRVMSTARDEC_ void drvmStar_LoadInternalEDID_VGA( void );
_DRVMSTARDEC_ void drvmStar_PowerOnInit( void );
_DRVMSTARDEC_ void drvmStar_MpllSrcCfg( void );
#if ENABLE_FRO12M_CAL
_DRVMSTARDEC_ BOOL drvmStar_FRO12MCalibration(void);
#endif
#if ENABLE_HDMI
_DRVMSTARDEC_ void drvmStar_SetupInputPort_HDMI(void);
#endif

#if ENABLE_CABLE_5V_EDID
_DRVMSTARDEC_ void InsertEDIDData(BYTE ucAddress, BYTE ucData);
_DRVMSTARDEC_ void drvmStar_LoadInternalEDID_VGA( void );
_DRVMSTARDEC_ void drvmStar_LoadInternalEDID_HDMI(BYTE *pEdid);
_DRVMSTARDEC_ void drvmStar_EnableHDMIFreeSyncEDID(Bool Enable);

#endif

typedef enum
{
    DVI_POWER_ON,
    DVI_POWER_STANDBY,
    DVI_POWER_DOWN
} DVI_PowerCtrlType;

typedef enum
{
    COMBO_INPUT_OFF,
    COMBO_INPUT_POWERSAVING,
    COMBO_INPUT_ANALOG,
    COMBO_INPUT_DIGITAL,
    COMBO_INPUT_DIGITAL_A,
    COMBO_INPUT_DIGITAL_B,
    COMBO_INPUT_DIGITAL_C,
} ComboInputType;

_DRVMSTARDEC_ void ComboInputControl(ComboInputType ctrl);
_DRVMSTARDEC_ void drvDVI_PowerCtrl(DVI_PowerCtrlType ctrl);
//_DRVMSTARDEC_ void drvDVI_PortMuxControl(BYTE ucPort);

extern  XDATA WORD wHF0_L0, wHF0_L1, wHF1_L0, wHF1_L1, wHF2_L0, wHF2_L1;


#define SSC_STEP        0x00B2//0x0124
#define SSC_SPAN        0x013A//0x0050
#define LPLL_SSC        _DISABLE
#define IVS_DEFAULT     22//19 //12

typedef struct _RegTblType
{
    WORD wReg;
    BYTE ucValue;
    BYTE ucMask;
}RegTblType;

typedef struct _RegTbl2Type
{
    WORD wReg;
    WORD wValue;
    WORD wMask;
}RegTbl2Type;

typedef enum _FSyncMode
{
    FLM_FBL,
	FLM_FBL_FRAMEPLL
}FSyncMode;

enum DVI_INPUT_PORT_TYPE
{
    DVI_INPUT_NONE = 0,
    DVI_INPUT_PORT0,
    DVI_INPUT_PORT1,
    DVI_INPUT_PORT2,
    DVI_INPUT_ALL,
    DVI_INPUT_PORT_MASK,
};

_DRVMSTARDEC_ void msWritePowerOnTble(void);
_DRVMSTARDEC_ void msSetFrameSyncMode(FSyncMode ucFrameLockMode);
_DRVMSTARDEC_ BYTE GetVfreq( void );
#if ENABLE_FREESYNC
_DRVMSTARDEC_ void msDrv_forceIP1VsyncPolarityEnable (void);
#endif

#undef _DRVMSTARDEC_
#endif

