
///////////////////////////////////////
// DRVMSTAR_V VERSION: V01
////////////////////////////////////////
#ifndef _DRVMSTAR_C_H_
#define _DRVMSTAR_C_H_

#ifdef _DRVMSTAR_C_C_
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

_DRVMSTARDEC_ void msInitClockGating( void );
_DRVMSTARDEC_ void msTMDSInit(void);

#if ENABLE_AUTOEQ
_DRVMSTARDEC_ void drv_TMDS_AUTOEQ_TRIG(void);
#endif
_DRVMSTARDEC_ void TMDS_Config_For_PM(void);
_DRVMSTARDEC_ void TMDSISR(void);
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
_DRVMSTARDEC_ void mStar_SetTimingGen( WORD u16HDE_OP21, WORD u16VDE_OP21, BOOL OP21_FreeRun);
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

_DRVMSTARDEC_ void drvDVI_PowerCtrl(DVI_PowerCtrlType ctrl);
_DRVMSTARDEC_ void ComboInputControl(ComboInputType ctrl);


#define SSC_STEP  0x00B2//0x0124
#define SSC_SPAN  0x013A//0x0050 
#define LPLL_SSC  _DISABLE
//#define V_FREQ_IN       (((VFreq( HFreq( SrcHPeriod), SrcVTotal)+5)/10) * ((SrcFlags&bInterlaceMode)?2:1))
#define IVS_DEFAULT     22//19 //12
#define VFREQ_4MFRC     30 // If VFreqIn <= VFREQ_4MFRC, do FRC
//#define WaitOutputFpllStable()  {u16TimeOutCounter=2000;bTimeOutCounterFlag=1;while(((msRead2Byte(REG_3854)&0xF4) !=0xF4 )&& (u16TimeOutCounter));}//if(!DDCCI_MCCS_CMD_FLAG) {g_u16u16DelayCounter=5000;while(!(msRead2Byte(REG_3854)==0x1F4) && (g_u16u16DelayCounter));}



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
    FLM_FB_FPLL_NOLOCK,
    FLM_FB_FPLL,
}FSyncMode;

enum DVI_INPUT_PORT_TYPE
{
    DVI_INPUT_NONE = 0,
    DVI_INPUT_PORT0,
    DVI_INPUT_PORT1,
    DVI_INPUT_PORT2,
    DVI_INPUT_ALL,
    DVI_INPUT_ALL_OFF,
    DVI_INPUT_PORT_MASK,
};

_DRVMSTARDEC_ void msWritePowerOnTble(void);
_DRVMSTARDEC_ void msSetFrameSyncMode(FSyncMode ucFrameLockMode);
_DRVMSTARDEC_ BYTE mStar_SetFrameDivider( BYTE vfreq_N );
_DRVMSTARDEC_ BYTE GetVfreq( void );
_DRVMSTARDEC_ void mStar_SetIVS( void );
#if ENABLE_FREESYNC
_DRVMSTARDEC_ void msDrv_forceIP1VsyncPolarityEnable (void);
#endif
#if CHIP_ID==CHIP_TSUMK
_DRVMSTARDEC_ BYTE GetVfreq( void );
#endif

#undef _DRVMSTARDEC_
#endif
