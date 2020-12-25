#ifndef __GLOBAL_H__
#define __GLOBAL_H__
#include "Board.h"
#include "userprefdef.h"
#include "mode.h"

///////////////////////////////////////////////////////
#include <math.h>
#include <intrins.h>
#include "types.h"
#include "debug.h"
#include "Common.h"
#include "ms_rwreg.h"
#include "ms_reg.h"
#include "Reg52.h"
#include "i2cdef.h"

#include "misc.h"
#include "mstar.h"
#include "menudef.h"
#include "drvadc.h"
#include "drvgpio.h"

#include "halRwreg.h"
#include "drvMcu.h"
#include "drvAdjust.h"
#include "drvDDC2Bi.h"
//#include "drvPower.h" // the function moves to drvmStar
#include "drvmStar.h"
#include "msScaler.h"
#include "msfb.h"
#if ENABLE_RTE
#include "msovd.h"
#include "drvmsOVD.h"
#endif
#include "appmStar.h"
#if  ENABLE_LED_CONTROLLER
#include "LedControl.h"
#endif
#if ENABLE_HDMI
#include "msHDMI.h"
#include "drv_msHDMI.h"
#endif
#if ENABLE_R2_2DTO3D
#include "ms2DTo3D.h"
#endif
//#include "mapi_mhl.h"
//#include "mdrv_mhl.h"
//#include "mhal_mhl.h"
#include "drvMHL.h"
#if ENABLE_TOUCH_PANEL
#include "touchpanel.h"
#endif
#if ENABLE_TOUCH_KEY
#include "touchkey.h"
#endif
#if ENABLE_DP_INPUT
#include "drvDPRxApp.h"
#endif
#include "Ms_PM.h"
#include "CodeBuildTime.h"

#if CHIP_ID >= CHIP_TSUMC
extern volatile BOOL g_bOutputVsyncIsrFlag;
#endif

#if ENABLE_WATCH_DOG_INT
extern BYTE xdata u8WDT_Status;
#endif
#if ENABLE_FREESYNC
#include "ComboApp.h"
#endif

#if EnableTime1Interrupt
extern  WORD XDATA g_u16MsCounter1; // for System timer
extern  BOOL XDATA bSecondTimer1;
extern  BYTE XDATA u8SecondTIMER1;
#endif
#if (CHIP_ID==CHIP_TSUMU|| CHIP_ID==CHIP_TSUM2|| CHIP_ID==CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID==CHIP_TSUMC|| \
     CHIP_ID==CHIP_TSUMD|| CHIP_ID==CHIP_TSUMJ|| CHIP_ID==CHIP_TSUM9|| CHIP_ID==CHIP_TSUMF)
    #define CURRENT_IS_FBL() (g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_0)
#else
    #define CURRENT_IS_FBL() (1)
#endif

#if (CHIP_ID == CHIP_TSUM9 ||CHIP_ID == CHIP_TSUMF)
extern BOOL g_bFROTrimResult;
#endif

#if ENABLE_DPS||ENABLE_DSC
extern idata BOOL bRunToolFlag;
#endif

#if ENABLE_MHL
extern BYTE xdata MHLExtenCountFlag; // 120529 coding addition for switching input port
#endif

///////////////////////////////////////////////////////
#define CURRENT_INPUT_IS_VGA()  (SrcInputType == Input_VGA)
#define CURRENT_INPUT_IS_DVI()  ((SrcInputType == Input_DVI)||(SrcInputType == Input_DVI2)||(SrcInputType == Input_DVI3))
#define CURRENT_INPUT_IS_HDMI() ((SrcInputType == Input_HDMI)||(SrcInputType == Input_HDMI2)||(SrcInputType == Input_HDMI3))
#define CURRENT_INPUT_IS_DISPLAYPORT() ((SrcInputType == Input_Displayport)||(SrcInputType == Input_Displayport3))
#define CURRENT_INPUT_IS_TMDS() (CURRENT_INPUT_IS_DVI() || CURRENT_INPUT_IS_HDMI())

#if ENABLE_MHL
#if CHIP_ID == CHIP_TSUM2
#define CURRENT_INPUT_MHL_CABLE_PLUGGED()   (CURRENT_INPUT_IS_DVI() && GET_MHL_CABLE_PLUGGED())
#else
#define CURRENT_INPUT_MHL_CABLE_PLUGGED()   (CurrentInputMHLCablePlugged())
#endif
#define CURRENT_INPUT_IS_HYBRID_MHL_HDMI()    (SrcInputType == Input_DVI) // MHL/HDMI
#define CURRENT_INPUT_IS_PURE_HDMI()    (SrcInputType == Input_HDMI) // MHL/HDMI
#endif

#if ENABLE_DP_INPUT
#define CURRENT_INPUT_IS_DISPLAYPORT_PortC() ((SrcInputType == Input_Displayport3))
#define CURRENT_INPUT_IS_DISPLAYPORT_PortB() ((SrcInputType == Input_Displayport))
#endif

#if INPUT_TYPE == INPUT_1A2C || INPUT_TYPE == INPUT_1A3C || INPUT_TYPE == INPUT_1A1C2
#define CURRENT_INPUT_IS_DIGITAL()      (SrcInputType == Input_Digital)
#define CURRENT_INPUT_IS_DIGITAL2()     (SrcInputType == Input_Digital2)
#if INPUT_TYPE == INPUT_1A3C
#define CURRENT_INPUT_IS_DIGITAL3()     (SrcInputType == Input_Digital3)
#endif
#endif

#define INPUT_IS_VGA(InputType)  (InputType == Input_VGA)
#define INPUT_IS_DVI(InputType)  ((InputType == Input_DVI)||(InputType == Input_DVI2)||(InputType == Input_DVI3))
#define INPUT_IS_HDMI(InputType) ((InputType == Input_HDMI)||(InputType == Input_HDMI2)||(InputType == Input_HDMI3))
#define INPUT_IS_DISPLAYPORT(InputType) ((InputType == Input_Displayport)||(InputType == Input_Displayport3))
#define INPUT_IS_TMDS() (CURRENT_INPUT_IS_DVI() || CURRENT_INPUT_IS_HDMI())

#define CURRENT_INPUT_IS_HDMI0()    (SrcInputType == Input_HDMI)
#define CURRENT_INPUT_IS_HDMI1()    (SrcInputType == Input_HDMI2)
#define CURRENT_INPUT_IS_HDMI2()    (SrcInputType == Input_HDMI3)

#define CURRENT_INPUT_IS_DVI0()    (SrcInputType == Input_DVI)
#define CURRENT_INPUT_IS_DVI1()    (SrcInputType == Input_DVI2)
#define CURRENT_INPUT_IS_DVI2()    (SrcInputType == Input_DVI3)

#if ENABLE_MHL
#if CHIP_ID == CHIP_TSUM2
#define CURRENT_INPUT_MHL_CABLE_PLUGGED()   (CURRENT_INPUT_IS_DVI() && GET_MHL_CABLE_PLUGGED())
#define IS_MHL_PORT0_CABLE_CONNECT()     (FALSE)
#define IS_MHL_PORT1_CABLE_CONNECT()     (FALSE)
#define IS_MHL_PORT2_CABLE_CONNECT()     (FALSE)
#elif (CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID == CHIP_TSUMD || CHIP_ID == CHIP_TSUM9 || CHIP_ID == CHIP_TSUMF)
#define CURRENT_INPUT_MHL_CABLE_PLUGGED()   (CurrentInputMHLCablePlugged())
    #if ENABLE_MHL_C1
    #define IS_MHL_PORT0_CABLE_CONNECT()     (MHL_CABLE_SAR_DETECT_PORT0 >= CBUS_CABLE_DETECT_LEVEL)
    #else
    #define IS_MHL_PORT0_CABLE_CONNECT()     (FALSE)
    #endif

    #if ENABLE_MHL_C2
    #define IS_MHL_PORT1_CABLE_CONNECT()     (MHL_CABLE_SAR_DETECT_PORT1 >= CBUS_CABLE_DETECT_LEVEL)
    #else
    #define IS_MHL_PORT1_CABLE_CONNECT()     (FALSE)
    #endif

    #if ENABLE_MHL_C3
    #define IS_MHL_PORT2_CABLE_CONNECT()     (MHL_CABLE_SAR_DETECT_PORT2 >= CBUS_CABLE_DETECT_LEVEL)
    #else
    #define IS_MHL_PORT2_CABLE_CONNECT()     (FALSE)
    #endif
#endif
#define CURRENT_INPUT_IS_HYBRID_MHL_HDMI()    (SrcInputType == Input_DVI) // MHL/HDMI
#define CURRENT_INPUT_IS_PURE_HDMI()    (SrcInputType == Input_HDMI) // MHL/HDMI

#else // !ENABLE_MHL
#define CURRENT_INPUT_MHL_CABLE_PLUGGED()   (FALSE)
#define IS_MHL_PORT0_CABLE_CONNECT()     (FALSE)
#define IS_MHL_PORT1_CABLE_CONNECT()     (FALSE)
#define IS_MHL_PORT2_CABLE_CONNECT()     (FALSE)
#endif

#define EXPANSION_MODE_IS_FULL()    (UserprefExpansionMode == Expansion_Full)

#define COLOR_SPACE_IS_YUV()   (!IsColorspaceRGB())
#define CURRENT_SOURCE_IS_INTERLACE_MODE()  (SrcFlags & bInterlaceMode)

#if (CHIP_ID == CHIP_TSUML||CHIP_ID == CHIP_TSUMB||CHIP_ID == CHIP_TSUMY||CHIP_ID == CHIP_TSUMV)
#define SOURCE_FORMAT_IN_NORMAL()   (TRUE)
#else
#define SOURCE_FORMAT_IN_NORMAL()   (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL)
#endif

#define V_FREQ_IN   ((SrcVFreq+5)/10)
#define IS_INPUT_VFREQ_IN_PANEL_RANGE()        (SrcVFreq >= PanelMinVfreq && SrcVFreq <= PanelMaxVfreq)
#define IS_INPUT_VFREQ_24Hz()                           (SrcVFreq >= 235 && SrcVFreq <= 245) // need this case for 60Hz panel
#define IS_INPUT_VFREQ_DOUBLE_IN_PANEL_RANGE()        ((V_FREQ_IN*2) >= (PanelMinVfreq/10) && (V_FREQ_IN*2) <= (PanelMaxVfreq/10))
#define IS_INPUT_VFREQ_TRIPLE_IN_PANEL_RANGE()        ((V_FREQ_IN*3) >= (PanelMinVfreq/10) && (V_FREQ_IN*3) <= (PanelMaxVfreq/10))
#define INPUT_TIMING_EXECUTE_FRAME_PLL_LOCK()       (!IS_INPUT_VFREQ_IN_PANEL_RANGE() && \
                                                                                                (IS_INPUT_VFREQ_24Hz() || \
                                                                                                  IS_INPUT_VFREQ_DOUBLE_IN_PANEL_RANGE() || \
                                                                                                  IS_INPUT_VFREQ_TRIPLE_IN_PANEL_RANGE()))
///////////////////////////////////////////////////////
extern BYTE  xdata g_SwitchSec;
extern BYTE  xdata g_CountSwitchPortTimeFlag;
#if DEPOP_DCONOFF_FLOW
extern BYTE xdata u8PowerDownCount;
#endif
//------Eson Start--------------------------------
extern xdata BYTE OsdFontColor;
#if CHIP_ID>=CHIP_TSUMV
#define CPC_Black       0x0F
#else
#define CPC_Black       0x00
#endif
#define CPC_White       0x01
#define CPC_Red         0x02
#define CPC_Gray        0x03
#define CPC_SmallLogo_4C        0x10

#define LayerXSize      18
#define Layer1XPos      1
#define Layer2XPos      (1+Layer1XPos+LayerXSize)
#define Layer3XPos      (1+Layer2XPos+LayerXSize)
#define LayerYPos       3
#define SaperationYPos       (LayerYPos+1)

#define HotLayerXPos      2
#define HotLayerYPos      4


#define GaugeXPos           (Layer3XPos+0)
#define NumberXPos          (GaugeXPos+12)

#define MonoFontStart       0x00
#define Saperation_2C       (MonoFontStart+0x02)
#define TopLine_2C          (MonoFontStart+0x03)
#define TopBottom_2C        (MonoFontStart+0x04)
#define BottomLine_2C       (MonoFontStart+0x05)
#define GaugeSelect_2C      (MonoFontStart+0x06)
#define GaugeNormal_2C      (MonoFontStart+0x07)
#define Mark_2C             (MonoFontStart+0x08)

#define CursorFontStart     (Mark_2C+0x01)
#define CursorLT_2C     (CursorFontStart+0x00)
#define CursorTop_2C    (CursorFontStart+0x01)
#define CursorRT_2C     (CursorFontStart+0x02)
#define CursorLeft_2C   (CursorFontStart+0x03)
#define CursorRight_2C  (CursorFontStart+0x04)
#define CursorLD_2C     (CursorFontStart+0x05)
#define CursorDown_2C   (CursorFontStart+0x06)
#define CursorRD_2C     (CursorFontStart+0x07)

#define NumberFontStart (CursorRD_2C+0x01)

#define PropFontAddr1       0x20
#define PropFontAddr2       0x80

#define _4ColorFontStart    0x180
#define SmallLogo_4C        _4ColorFontStart
#define SmallLogo_Size      39

#define Logo_4C         0x02

#if DisplayLogo!=NoBrand
#if DisplayLogo ==MstarLOGO
#define Logo_Size       231//MstarLOGO
#elif DisplayLogo ==NANCHANG
#define Logo_Size      204//NANCHANG  
#elif DisplayLogo ==EPAI
#define Logo_Size       243//EPAI
#elif DisplayLogo ==MAYA
#define Logo_Size      122//MAYA
#elif DisplayLogo ==IMAGIC
#define Logo_Size       136//IMAGIC
#elif DisplayLogo ==FANGZHENG
#define Logo_Size       205//FANGZHENG
#elif DisplayLogo ==GRC
#define Logo_Size       103//GRC
#elif DisplayLogo ==BUBALUS
#define Logo_Size      227//BUBALUS
#elif DisplayLogo ==BUBALUSWHITE
#define Logo_Size      227//BUBALUSWHITE
#elif DisplayLogo ==SAMPIN
#define Logo_Size      252//SAMPIN
#elif DisplayLogo ==PCTOP
#define Logo_Size      140//PCTOP
#elif DisplayLogo ==FOXIN
#define Logo_Size      141
#elif DisplayLogo ==ZEBION
#define Logo_Size      236
#elif DisplayLogo ==SUGON
#define Logo_Size      123
#elif DisplayLogo ==SONGUAN
#define Logo_Size      247
#elif DisplayLogo ==INFORCE
#define Logo_Size      175
#elif DisplayLogo ==MAGIX
#define Logo_Size      243
#elif DisplayLogo ==DRAZILPC
#define Logo_Size      191
#elif DisplayLogo ==MEGAONIC
#define Logo_Size      231


#endif
#endif

//------Eson End--------------------------------

#define xfr_regs ((BYTE xdata*)0xC000)
#define HIBYTE(value)             ((BYTE)((value) >> 8))
#define LOBYTE(value)             ((BYTE)(value))

#define CommandFontStart     1
#define FactoryNumberFontStart  3   //modifie By SMC.LMW20151119
#define FfontStart    NumberFontStart + 10//0x06   //100907 aoivd overlap with arrow  //modifie By SMC.LMW20151118
#define DrawtTEXTIconStart  0xD2
#define DrawtTEXTIconSize   0x0C


#define Arrow    0xCE   // 100901
#define ArrowSize    0x02   // 100901
// 4Color--------------------------------
#define colorFontStart          0xCE
#define ColorGuageEdgeStar      0xEA
#define ColorGuageEdgeSize      0x02

#define ColorGuageFontStar    0x00//0x100
#define ColorGuageFontSize    0x05

#define SelectFrameStar 0x0A//0x10A
#define SelectFrameSize 0x08

//8Color------------------------------
#define Color_8ColorFontStart    0x1E
#define ColorIcon_8ColorFontStart       0x1E
#if Enable_CascadeMode
#define EXIT_8ColorFontStart                0x19
#define GradulFrame_8ColorFontStart     0x22
#define GrayIcon_8ColorFontStart            0xA3
#define GrayIcon_8ColorFontStart_2          0x00
#define SelectFrame_8ColorFontStart         0x5D// 100907 0x36
#else
#define EXIT_8ColorFontStart                    0x77
#define GradulFrame_8ColorFontStart     0x80
#define GrayIcon_8ColorFontStart            0x01
#define SelectFrame_8ColorFontStart         0xBB//100722 0x94
#endif




//extern bit g_bBacklightOn;
#if ENABLE_DP_INPUT
extern bit g_bDoDPInit;
#endif

extern bit   g_bInputSOGFlag;
extern BYTE bInputVGAisYUV;   // (0) VGA is RGB    (1) VGA is YUV //20150121


extern BYTE g_ucTimer0_TH0;
extern BYTE g_ucTimer0_TL0;
extern BYTE xdata g_u8SystemSpeedMode;
extern bit g_bMcuPMClock;
#if IR_FUNC_EN
extern BYTE xdata g_ucTimer1_TH0;
extern BYTE xdata g_ucTimer1_TL0;
extern BYTE idata IRClearCounter;
//extern BYTE idata IRFLAG;
#endif


extern bit SecondFlag;
extern bit ms10Flag;
extern bit ms50Flag;
extern bit ms500Flag; //
extern bit ms1000Flag; //

extern bit InputTimingChangeFlag;

#define Set_SecondFlag()            (SecondFlag = 1)
#define Clr_SecondFlag()            (SecondFlag = 0)

#define Set_ms10Flag()          (ms10Flag = 1)
#define Clr_ms10Flag()          (ms10Flag = 0)

#define Set_ms50Flag()          (ms50Flag = 1)
#define Clr_ms50Flag()          (ms50Flag = 0)

#define Set_ms500Flag()         (ms500Flag = 1)
#define Clr_ms500Flag()         (ms500Flag = 0)
#define Set_ms1000Flag()         (ms1000Flag = 1)
#define Clr_ms1000Flag()         (ms1000Flag = 0)
#define Set_InputTimingChangeFlag()         (InputTimingChangeFlag = 1)
#define Clr_InputTimingChangeFlag()         (InputTimingChangeFlag = 0)
#if ENABLE_USB_INPUT
extern DWORD xdata u32UsbCount;
#endif
extern WORD ms_Counter; // for System timer
extern BYTE xdata ModeDetectCounter;
extern WORD xdata u16PanelOffOnCounter;
extern bit bPanelOffOnFlag;
#define SetPanelOffOnCounter(x) ((x)?(bPanelOffOnFlag=0,u16PanelOffOnCounter=x,bPanelOffOnFlag=1):(bPanelOffOnFlag=0))

extern bit bPanelOnFlag;
extern BYTE xdata BackLightActiveFlag;

extern BYTE xdata DDCWPActiveFlag;
extern bit g_bServiceEDIDUnLock;

#ifdef Internal_EDID_Write_To_24C02
typedef enum I2CType_enum
{ 
   I2C_EEPROM,
   VGA_EDID,
   DVI_EDID,
   HDMI_EDID,
   I2CMAX
} I2CType;

extern bit bNoUsePin;
extern BYTE xdata EnableReadDDCType;
extern BYTE xdata DDC1[256];
#endif


extern xdata BYTE KeypadButton;
#if DECREASE_V_SCALING
extern WORD xdata DecVScaleValue;
#define VSTART_OFFSET  (DecVScaleValue/2)
#endif

#if Enable_Expansion
extern WORD xdata DispalyWidth ;
extern WORD xdata HStartOffset ;
#endif

extern BYTE xdata Second;
#if MS_DAC
extern WORD xdata w16DAC_DePopCounter;
extern bit bDAC_DePopCountFlag;
#define SetDAC_DePopCounter(x) ((x)?(bDAC_DePopCountFlag=0,w16DAC_DePopCounter=x,bDAC_DePopCountFlag=1):(bDAC_DePopCountFlag=0))

extern BYTE xdata w16DAC_DePopStep;
extern bit bDAC_DePopFlag;

#endif

extern BYTE xdata InputTimingStableCounter;
extern WORD xdata SystemFlags; // for system status flags
extern WORD xdata System2Flags;
extern BYTE xdata System3Flags;  // Rex 100701
extern BYTE xdata OsdCounter;
extern BYTE xdata HotKeyCounter;
extern BYTE xdata PowerDownCounter;
//extern BYTE PatternNo;
extern DWORD xdata BlacklitTime;
#if ENABLE_BURNIN_MENU
extern DWORD xdata BurninTime ;
extern bit SaveBurninTimeFlag;

#endif

extern bit SaveBlacklitTimeFlag;
extern bit gBoolVisualKey;
extern BYTE xdata gByteVisualKey;
//extern BYTE SaveIndex;
//extern BYTE xdata ChipVer;
extern BYTE xdata SaveIndex;
extern BYTE xdata PatternNo;
/*
extern BYTE  UserPrefSubRedGain;
extern BYTE  UserPrefSubGreenGain;
extern BYTE  UserPrefSubBlueGain;
 */
extern BYTE xdata BrightnessTemp;
extern BYTE xdata PrevHistoValue;

#if (ENABLE_DEBUG && ENABLE_TIME_MEASUREMENT)
extern WORD volatile xdata g_u16TimeMeasurementCounter;
extern bit bTimeMeasurementFlag;
 #define SetTimeMeasurementCounter(x) ((x)?(bTimeMeasurementFlag=0,g_u16TimeMeasurementCounter=x,bTimeMeasurementFlag=1):(bTimeMeasurementFlag=0))

extern WORD xdata g_u16Time1;
extern WORD xdata g_u16Time2;
#define INIT_TIMER()           SetTimeMeasurementCounter(0xFFFF);
#define GET_T1()                g_u16Time1=g_u16TimeMeasurementCounter
#define GET_T2()                g_u16Time2=g_u16TimeMeasurementCounter
#define PRINT_DT()              printData(" Delta T=%d",(g_u16Time1-g_u16Time2))
#define TIME_MEASURE_BEGIN()    {INIT_TIMER();GET_T1();}
#define TIME_MEASURE_END()      {GET_T2();PRINT_DT();}
#else
#define TIME_MEASURE_BEGIN()
#define TIME_MEASURE_END()
#endif

#if Display_Checksum			
extern WORD xdata  Code_checksun;
#endif


#if ENABLE_TOUCH_KEY
extern BYTE xdata TouchKeyRestortCounter;

    #if ENABLE_LOW_CONTACT
    extern BYTE xdata TouchKeyLowContactDelay; // unit: 50mS
    extern BYTE xdata TouchKeyLowContactDetectCntr; // unit: 1S, use to detect low contact peroid
    #define TOUCH_KEY_LOW_CONTACT_DETECT_PEROID     20 // unit: 1S
    #endif


    #if TOUCH_KEY_CTRL_LED
    //extern BYTE xdata TouchKeySetLEDStatus;
    typedef enum
    {
        TouchKeyLEDStatus_OFF,
        TouchKeyLEDStatus_AMBER_ON,
        TouchKeyLEDStatus_GREEN_ON,
    }TouchKeyLEDStatusType;
    #endif
#endif

#if ENABLE_HDCP
extern xdata BYTE HDCPFlag;
#endif

typedef enum
{
    VGA1,// 0
    DVI1,// 1
    HDMI1,// 2
    DP1,// 3
    VGA2,// 4
    DVI2,// 5
    HDMI2,// 6
    DP2,// 7
    EEPROM = 0x10,
    HDCP1,
    HDCP2//for DP
} DeviceID;

typedef enum
{
    VGA_Port1 = BIT7,
    VGA_Port2 = BIT6,
    DVI_Port1 = BIT5,
    DVI_Port2 = BIT4,
    HDMI_Port1 = BIT3,
    HDMI_Port2 = BIT2,
    DP_Port1 = BIT1,
    DP_Port2 = BIT0,
} InputPortID;

#ifndef DDC_BUFFER_LENGTH
#define DDC_BUFFER_LENGTH   50//MaxBufLen 50 // ddc buffer length
#endif

#define _UART_BUFFER_LENGTH_    DDC_BUFFER_LENGTH

extern BYTE xdata SIORxBuffer[_UART_BUFFER_LENGTH_];
extern BYTE xdata SIORxIndex;
extern BYTE xdata u8ReSyncCounter;
extern bit TI0_FLAG;


#if 1//Enable_Expansion
extern bit bExpansionBit;
#endif

extern WORD u16DelayCounter;
extern bit bDelayFlag;
extern WORD xdata u16TimeOutCounter;
extern bit bTimeOutCounterFlag;
#define SetTimOutConter(x) ((x)?(bTimeOutCounterFlag=0,u16TimeOutCounter=x,bTimeOutCounterFlag=1):(bTimeOutCounterFlag=0))
extern bit SaveSettingFlag;

extern BYTE xdata TPDebunceCounter;
#define SKPollingInterval   50 // unit: ms
extern bit StartScanKeyFlag;
#define Set_StartScanKeyFlag()       (StartScanKeyFlag=1)
#define Clr_StartScanKeyFlag()       (StartScanKeyFlag=0)
extern bit bKeyReadyFlag;
#define Set_bKeyReadyFlag()         (bKeyReadyFlag=1)
#define Clr_bKeyReadyFlag()         (bKeyReadyFlag=0)
#if ENABLE_TOUCH_KEY
#if TOUCH_KEY_POWER_KEY_DEBOUNCE
extern bit bPowerKeyPressed;
extern BYTE xdata PowerKeyDebounceCntr;
#endif
#endif


//extern xdata BYTE xfr_regs[256];
//extern xdata BYTE g_FlashID;
extern BYTE xdata LanguageIndex;
extern BYTE xdata SecondTblAddr;
extern BYTE xdata HistogramNow;
extern BYTE xdata HistogramPrev;
extern BYTE xdata HistogramFlag;
extern BYTE xdata BacklightNow;
extern BYTE xdata BacklightPrev;
extern BYTE xdata LastMenuPage;


extern OverScanType  xdata OverScanSetting;

#define Set_SaveSettingFlag()			(SaveSettingFlag=1)
#define Clr_SaveSettingFlag()			(SaveSettingFlag=0)
#if ShowSourceType
extern bit DisplayInputMsgFlag;
#define Set_DisplayInputMsgFlag()				 (DisplayInputMsgFlag=1)
#define Clr_DisplayInputMsgFlag()				  (DisplayInputMsgFlag=0)
#endif
extern BYTE xdata UserPrefSelectInputType;


#if CompressPropFont
typedef WORD PropFontType;
#else
typedef struct
{
    BYTE SpaceWidth;
    WORD LineData[18];
} PropFontType;
#endif
#if ENABLE_HDCP&&HDCPKEY_IN_Flash
extern BYTE xdata HDCPDataBuffer[6];
#endif
#define BKSVDataAddr  0
#define HDCPKetAddr     5

extern BYTE xdata InputColorFormat; // indicate input color space is RGB or YUV
typedef enum
{
    INPUTCOLOR_RGB,
    INPUTCOLOR_YUV,
    INPUTCOLOR_MAX
} InputColorFormatType;


extern BYTE xdata SwitchPortCntr;

extern InputModeType code StandardMode[];
extern InputResolutionType code StandardModeResolution[];

extern PanelSettingType xdata PanelSetting;
extern InputTimingType xdata InputTiming;
extern MonitorSettingType xdata MonitorSetting;
#if USEFLASH
extern MonitorSettingType2 xdata  MonitorSetting2;
#endif
extern ModeSettingType xdata ModeSetting;
#if 1//USEFLASH
extern FactorySettingType xdata FactorySetting;
#endif
#ifdef UseInternalDDCRam
extern BYTE xdata DDC1[128];
extern bit EnCheckVccFlag;
extern BYTE SystemVccDropDownCnt;
#endif
//extern bit PowerGoodExistFlag;
#if ENABLE_DAC_DEPOP
extern BYTE EARDePopFlag;
extern BYTE EARDePopStep ;
extern WORD xdata ucDePOP4SecCounter;
#endif

#if Enable_Expansion
#define DefExpansion    Expansion_Full
#endif


#if (CHIP_ID == CHIP_TSUMF)
#define DEF_ADC_PGA_GAIN    0x3F
#else
#define DEF_ADC_PGA_GAIN    0x00
#endif

#define DEF_ADC_RED_GAIN    0x500
#define DEF_ADC_GREEN_GAIN    0x500
#define DEF_ADC_BLUE_GAIN    0x500
#define DEF_ADC_OFFSET  0

//#define ADC_GAIN_MAX    0x400*

// for srgb
//=======================================================================================//
//#define DefsRGB_RedColor        DefWarm_RedColor
//#define DefsRGB_GreenColor  DefWarm_GreenColor
//#define DefsRGB_BlueColor       DefWarm_BlueColor

#if PanelRSDS
#define DefVcomValue       0x80
#endif



#define MaxBrightnessValue 100 // 160//0x80 wmz 20051017
#define MinBrightnessValue  0 //0x80    wmz 20051017
#ifndef DefBrightness
#define DefBrightness    50
#endif
#ifndef DefRedColor
#define DefRedColor 0x80
#endif
#ifndef DefGreenColor
#define DefGreenColor   0x80
#endif
#ifndef DefBlueColor
#define DefBlueColor    0x80
#endif

#define DefColorUser            0x80//136//148//155 // 155x106/128 = 128

#define DefVolume			50
#define MaxVolumeValue		100
#define MinVolumeValue		0

#define DefHue                  50
#define DefSaturation           0x80

#define MinSubContrast       55
#define MaxSubContrast       255

#define MinSubBrightness     55
#define MaxSubBrightness     255

#define DefSubContrast          ((MaxSubContrast-MinSubContrast)/2+MinSubContrast)
#define DefSubBrightness        ((MaxSubBrightness-MinSubBrightness)/2+MinSubBrightness)


#define MaxContrastValue        100//(0x80+50) // 156 // 160//0x80 wmz 20051017
#define MinContrastValue        0///(0x80-50)//56//36 // 32 //0x80    wmz 20051017

#define DefContrast         50//((MaxContrastValue-MinContrastValue)/2+MinContrastValue)//106 //96 // 92 // 0x5c // 0x60 //32~96~160 171(0xAB)

#if ENABLE_SHARPNESS
#define MaxSharpness    0x1F   //user sharpness adjust gain: 0x10~0x1F==>1.0~ 1.9
#define MinSharpness    0x00     //user sharpness adjust gain  0x00~0x0F==>0.0~ 0.9
#define DefSharpness         ((MaxSharpness-MinSharpness)/2+MinSharpness)
#endif
#if ENABLE_R2_2DTO3D
#define Def2Dto3D             0   //on off
#define Def2Dto3DGameMode     0
#define Def2Dto3DGain             2  //weakness to strongness 0~4
#define Def2Dto3DOffset           2//weakness to strongness 0~4
#define MAX_T3DOFFSET        4//weakness to strongness 0~4
#define MAX_T3DGAIN          4//weakness to strongness 0~4
#endif
// 090721
#define MaxColorValue           (0xff-28)//((UserPrefColorTemp == CTEMP_USER)?(0xFF):(DefColorUser+100))
#define MinColorValue           (0+28)//((UserPrefColorTemp == CTEMP_USER)?(0x10):(DefColorUser-100))

#define ClockAdjRange   100
#define DefRGBgainBase  20
#define DefContrastBase   0//48
#define MAX_VIDEO_SATURATION    0xFF
#define MIN_VIDEO_SATURATION    0
#define MAX_VIDEO_HUE           100
#define MIN_VIDEO_HUE           0
#define MaxVolume         100//74 // 79
#define MinVolume          00//24 // 26 // 27
#define MaxHStart   UserPrefAutoHStart+50
#define MinHStart   UserPrefAutoHStart-50
#define MaxVStart 2*UserPrefAutoVStart-1
#define MinVStart 1  // 2006/10/26 4:18PM by Emily  0
#define MaxClock    (StandardModeHTotal+50)
#define MinClock    (StandardModeHTotal-50)

//============================================================================================
// current input timing information
#define SrcHPeriod      InputTiming.HPeiod
#define SrcVTotal       InputTiming.VTotal
#define SrcModeIndex        InputTiming.ModeIndex
#define SrcFlags        InputTiming.fFlags
#define SrcInputType        InputTiming.InputType
#define SrcVFreq        InputTiming.VFreq
#if ENABLE_CHECK_TMDS_DE
#define SrcTMDSWidth    InputTiming.TMDSWidth
#define SrcTMDSHeight   InputTiming.TMDSHeight
#define SrcTMDSHStart   InputTiming.TMDSHStart
#define SrcTMDSVStart   InputTiming.TMDSVStart
#endif
//============================================================================================
// VESA Standard mode default settings
#if   0
#define StandardModeWidth          GetStandardModeWidth(StandardModeGroup)//StandardModeResolution[StandardModeGroup].DispWidth
#define StandardModeHeight  GetStandardModeHeight(StandardModeGroup)//StandardModeResolution[StandardModeGroup].DispHeight
#define StandardModeGroup   GetStandardModeGroup(SrcModeIndex)//StandardMode[SrcModeIndex].ResIndex
#define StandardModeHStart  GetStandardModeHStart(SrcModeIndex)//StandardMode[SrcModeIndex].HStart
#define StandardModeVStart  GetStandardModeVStart(SrcModeIndex)//StandardMode[SrcModeIndex].VStart
#define StandardModeHTotal  GetStandardModeHTotal(SrcModeIndex)//StandardMode[SrcModeIndex].HTotal
#define StandardModeVTotal  GetStandardModeVTotal(SrcModeIndex)//StandardMode[SrcModeIndex].VTotal
#define StandardModeHFreq   GetStandardModeHFreq(SrcModeIndex)//StandardMode[SrcModeIndex].HFreq
#define StandardModeVFreq       GetStandardModeVFreq(SrcModeIndex)//StandardMode[SrcModeIndex].VFreq
#endif
#define StandardModeWidth   GetStandardModeWidth()//StandardModeResolution[StandardModeGroup].DispWidth
#define StandardModeHeight  GetStandardModeHeight()
#define StandardModeGroup   GetStandardModeGroup()
#define StandardModeHStart  GetStandardModeHStart()
#define StandardModeVStart  GetStandardModeVStart()
#define StandardModeHTotal  GetStandardModeHTotal()
#define StandardModeVTotal  GetStandardModeVTotal()
#define StandardModeHFreq    GetStandardModeHFreq()
#define StandardModeVFreq       GetStandardModeVFreq()
//============================================================================================

#if 1
// Monitor Setting
#define MonitorFlags                      MonitorSetting.MonitorFlag
#define UserPrefColorTemp           MonitorSetting.ColorTemp
#define UserPrefBrightness         MonitorSetting.Brightness
#define UserPrefContrast                 MonitorSetting.Contrast
#define UserPrefBrightnessTemp		   MonitorSetting.BrightnessTemp
#define UserPrefContrastTemp			 MonitorSetting.ContrastTemp
#define UserPrefBrightnessWarm1   MonitorSetting.BrightnessWarm1
#define UserPrefContrastWarm1     MonitorSetting.ContrastWarm1
#define UserPrefBrightnessNormal      MonitorSetting.BrightnessNormal
#define UserPrefContrastNormal    MonitorSetting.ContrastNormal
#define UserPrefBrightnessCool1   MonitorSetting.BrightnessCool1
#define UserPrefContrastCool1           MonitorSetting.ContrastCool1
#define UserPrefBrightnessSRGB  MonitorSetting.BrightnesssRGB
#define UserPrefContrastSRGB        MonitorSetting.ContrastsRGB
#define UserPrefRedColor                 MonitorSetting.RedColor
#define UserPrefGreenColor        MonitorSetting.GreenColor
#define UserPrefBlueColor               MonitorSetting.BlueColor
#define UserPrefBrightnessUser         MonitorSetting.BrightnessUser
#define UserPrefContrastUser                 MonitorSetting.ContrastUser
#define UserPrefRedColorUser                 MonitorSetting.RedColorUser
#define UserPrefGreenColorUser        MonitorSetting.GreenColorUser
#define UserPrefBlueColorUser               MonitorSetting.BlueColorUser


#define UserPrefHue                    MonitorSetting.Hue
#define UserPrefSaturation             MonitorSetting.Saturation

#if MWEFunction
#define UserPrefColorFlags          MonitorSetting.ColorFlag
#define UserPrefSubContrast            MonitorSetting.SubContrast
#define UserPrefSubBrightness          MonitorSetting.SubBrightness
#define UserPrefBFSize                 MonitorSetting.BF_Size
#define UserPrefBFHSize                MonitorSetting.BF_HSize
#define UserPrefBFVSize                MonitorSetting.BF_VSize
#define UserPrefBFHStart               MonitorSetting.BF_Hstart
#define UserPrefBFVStart               MonitorSetting.BF_Vstart
#endif

#if Enable_Gamma
#define UserPrefGamaMode        MonitorSetting.GamaMode
#define UserPrefGamaOnOff        MonitorSetting.GamaOnOff
#endif
#if (AudioFunc && (ENABLE_HDMI || ENABLE_DP_INPUT))
#define UserPrefAudioSource     MonitorSetting.AudioSource
#endif
#if AudioIC_CS8003_Enable
	#if EarphoneDet_Enable	
	#define UserPrefEarVolume		   MonitorSetting.EarVolume
	#endif
#endif
#if Audio_Switch
#define UserPrefHDMIAudioType		MonitorSetting.HDMIAudioType
#endif
#if LowBlueLight
		#define UserPrefLowBlueMode		  MonitorSetting.LowBlueMode
#endif
#define UserPrefVolume         MonitorSetting.Volume
#define UserOSDMuteOnFlag		MonitorSetting.MuteOnFlag
#define UserPrefECOMode       MonitorSetting.ECOMode
#define UserPrefLanguage          MonitorSetting.Language
#define UserPrefOsdHStart     MonitorSetting.OsdHStart
#define UserPrefOsdVStart         MonitorSetting.OsdVStart
#define UserPrefOsdTime       MonitorSetting.OsdTime
#define UserPrefOsdTransparency		MonitorSetting.OsdTransparency
#if ENABLE_OSD_ROTATION
#define UserPrefOsdRotateMode       MonitorSetting.OSDRotateMode
#endif
#if ENABLE_FREESYNC
#define UserprefFreeSyncMode        MonitorSetting.FreeSyncMode
#endif
#define UserPrefInputType          MonitorSetting.InputType
#define UserPrefInputPriorityType        MonitorSetting.InputSelectType
//#define UserPrefLastMenuIndex        MonitorSetting.LastMenuPage
#define UserPrefSavedModeIndex  MonitorSetting.SavedModeIndex // current mode index can be used
#define UserprefRedBlackLevel           MonitorSetting.RedBlackLevel
#define UserprefGreenBlackLevel             MonitorSetting.GreenBlackLevel
#define UserprefBlueBlackLevel              MonitorSetting.BlueBlackLevel


#define UserPrefDcrMode         MonitorSetting.DcrMode

#if Enable_Expansion
#define UserprefExpansionMode        MonitorSetting.ExpansionMode
#endif

#if ENABLE_DLC
#define UserprefDLCMode        MonitorSetting.DLCMode
#endif
#if ENABLE_DSC
#define UserprefDSCMode        MonitorSetting.DSCMode
#endif
#if ENABLE_DPS
#define UserprefDPSMode        MonitorSetting.DPSMode
#endif
#if ENABLE_RTE
#define UserprefOverDriveSwitch        MonitorSetting.OverDriveMode
#define UserprefOverDriveWeighting        MonitorSetting.OverDriveWeighting

#endif
#if ENABLE_SUPER_RESOLUTION
#define UserPrefSuperResolutionMode        MonitorSetting.SuperResolutionMode
#endif
#if ENABLE_SHARPNESS
#define UserPrefSharpness           MonitorSetting.Sharpness
#endif
#if ENABLE_DeBlocking
#define UserPrefDeBlocking           MonitorSetting.DeBlocking
#endif
#if ENABLE_TNR
#define UserPrefNRmode           MonitorSetting.NRmode
#endif
#if ENABLE_3DLUT
#define UserPref3DLUTmode          MonitorSetting.RGB3DLUTMode
#endif

#define UserPrefInputColorFormat        MonitorSetting.InputColorFormat

#define UserprefHistogram1              MonitorSetting.Histogram1
#define UserprefHistogram2              MonitorSetting.Histogram2
#define UserprefALha              MonitorSetting.ALhaA
#define UserprefBata              MonitorSetting.BataA
#define MSBankNo            MonitorSetting.BankNo
#define MSAddr              MonitorSetting.Addr
#define MSValue             MonitorSetting.Value
#if USEFLASH
#define UserprefBacklighttime  MonitorSetting2.Backlighttime
#else
#define UserprefBacklighttime  MonitorSetting.Backlighttime
#endif

#if ENABLE_BURNIN_MENU
#define UserprefBurnintime  MonitorSetting2.Burnintime
#endif

#endif

#define UserPref2Dto3D          MonitorSetting.T3DMode
#define UserPref2Dto3DGameMode  MonitorSetting.T3DGameMode
#define UserPref2Dto3DGain      MonitorSetting.T3DGain
#define UserPref2Dto3DOffset    MonitorSetting.T3DOffset

#define UserPrefAdcPgaGain      FactorySetting.AdcPgaGain
#define UserPrefAdcRedGain      FactorySetting.AdcRedGain
#define UserPrefAdcGreenGain        FactorySetting.AdcGreenGain
#define UserPrefAdcBlueGain     FactorySetting.AdcBlueGain
#define UserPrefAdcRedOffset        FactorySetting.AdcRedOffset
#define UserPrefAdcGreenOffset  FactorySetting.AdcGreenOffset
#define UserPrefAdcBlueOffset       FactorySetting.AdcBlueOffset

#define UserPrefRedColorWarm1     FactorySetting.RedColorWarm1
#define UserPrefGreenColorWarm1   FactorySetting.GreenColorWarm1
#define UserPrefBlueColorWarm1   FactorySetting.BlueColorWarm1
#define UserPrefRedColorNormal    FactorySetting.RedColorNormal
#define UserPrefGreenColorNormal   FactorySetting.GreenColorNormal
#define UserPrefBlueColorNormal   FactorySetting.BlueColorNormal
#define UserPrefRedColorCool1        FactorySetting.RedColorCool1
#define UserPrefGreenColorCool1  FactorySetting.GreenColorCool1
#define UserPrefBlueColorCool1   FactorySetting.BlueColorCool1
#define UserPrefRedColorSRGB        FactorySetting.RedColorsRGB
#define UserPrefGreenColorSRGB    FactorySetting.GreenColorsRGB
#define UserPrefBlueColorSRGB      FactorySetting.BlueColorsRGB
#define FUserPrefBrightnessWarm1      FactorySetting.FBrightnessWarm1
#define FUserPrefContrastWarm1    FactorySetting.FContrastWarm1
#define FUserPrefBrightnessNormal     FactorySetting.FBrightnessNormal
#define FUserPrefContrastNormal   FactorySetting.FContrastNormal
#define FUserPrefBrightnessCool1      FactorySetting.FBrightnessCool1
#define FUserPrefContrastCool1          FactorySetting.FContrastCool1
#define FUserPrefBrightnessSRGB         FactorySetting.FBrightnesssRGB
#define FUserPrefContrastSRGB           FactorySetting.FContrastsRGB
#define UserPrefLogoON	FactorySetting.LogoON  //120420 Modify
#if PanelRSDS
#define UserPrefVcomValue       FactorySetting.VcomValue
#endif

#if USEFLASH
#define FactoryProductModeValue         FactorySetting.ProductModeValue
#else
#define FactoryCheckSum         FactorySetting.CheckSum
#endif

#define UserPrefLastMenuIndex          LastMenuPage



#if MWEFunction
#define bColorAutoDetectBit           BIT0
#define bSkinBit                      BIT1
#define bBlueBit                      BIT2
#define bGreenBit                     BIT3
#define bMWEModeBit                   BIT4
#define bDemoBit                      BIT5
#define bFullEnhanceBit               BIT6
#define bPictureBoostBit              BIT7
#define MWEModeFlag                 (UserPrefColorFlags&bMWEModeBit)
#define Set_MWEModeFlag()           (UserPrefColorFlags|=bMWEModeBit)
#define Clr_MWEModeFlag()           (UserPrefColorFlags&=~bMWEModeBit)
#define ColorAutoDetectFlag         (UserPrefColorFlags&bColorAutoDetectBit)
#define Set_ColorAutoDetectFlag()   (UserPrefColorFlags|=bColorAutoDetectBit )
#define Clr_ColorAutoDetectFlag()   (UserPrefColorFlags&=~bColorAutoDetectBit )
#define SkinFlag                    (UserPrefColorFlags&bSkinBit)
#define Set_SkinFlag()              (UserPrefColorFlags|=bSkinBit)
#define Clr_SkinFlag()              (UserPrefColorFlags&=~bSkinBit)
#define BlueFlag                    (UserPrefColorFlags&bBlueBit)
#define Set_BlueFlag()              (UserPrefColorFlags|=bBlueBit)
#define Clr_BlueFlag()              (UserPrefColorFlags&=~bBlueBit)
#define GreenFlag                   (UserPrefColorFlags&bGreenBit)
#define Set_GreenFlag()             (UserPrefColorFlags|=bGreenBit)
#define Clr_GreenFlag()             (UserPrefColorFlags&=~bGreenBit)
#define DemoFlag                    (UserPrefColorFlags&bDemoBit    )
#define Set_DemoFlag()              (UserPrefColorFlags|=bDemoBit   )
#define Clr_DemoFlag()              (UserPrefColorFlags&=~bDemoBit  )
#define FullEnhanceFlag             (UserPrefColorFlags&bFullEnhanceBit )
#define Set_FullEnhanceFlag()       (UserPrefColorFlags|=bFullEnhanceBit    )
#define Clr_FullEnhanceFlag()       (UserPrefColorFlags&=~bFullEnhanceBit )
#define PictureBoostFlag            (UserPrefColorFlags&bPictureBoostBit    )
#define Set_PictureBoostFlag()      (UserPrefColorFlags|=bPictureBoostBit )
#define Clr_PictureBoostFlag()      (UserPrefColorFlags&=~bPictureBoostBit  )
#endif
//============================================================================================
// current input timing setting
#define UserPrefHTotal          ModeSetting.HTotal
#define UserPrefHStart          ModeSetting.HStart
#define UserPrefVStart          ModeSetting.VStart
#define UserPrefPhase           ModeSetting.Phase
#define UserPrefAutoHStart      ModeSetting.AutoHStart
#define UserPrefAutoVStart      ModeSetting.AutoVStart  //2004/01/15 num11
#define UserPrefAutoTimes       ModeSetting.AutoTimes
//======================================================================================
#define SyncLoss            (bHSyncLoss|bVSyncLoss)
#define SyncPolarity(status)        (status&0x03)
#define GetSyncPolarity(status)     (1<<(status&0x03))
//=================================================================================
// Input Timing information flags
#define SyncLossState()         (SrcFlags&SyncLoss)
#define UserModeFlag            (SrcFlags&bUserMode)
#define UnsupportedModeFlag     (SrcFlags&bUnsupportMode)
#define NativeModeFlag          (SrcFlags&bNativeMode)
#define ShrinkModeFlag          (SrcFlags&bShrinkMode)
//==================================================================================
// Monitor status flags for storing into NVRAM
#define bPowerOnBit         BIT0    // system power on/off status
#define bFactoryModeBit     BIT1    // factory mode
#define bBurninModeBit          BIT2    // burnin mode
#define bDoBurninModeBit        BIT3
#define bOsdLockModeBit         BIT4
#define bShowOsdLockBit         BIT5
//    #define bMuteOnBit                BIT6
//#define bOverDriveOnBit         BIT6
#define bDCCCIONOBit                BIT7
#define bTNROnBit               BIT8
#if ENABLE_OSD_ROTATION
#define bMenuRotationBit           BIT9
#endif

#if EarphoneDet_Enable
  #define bHarphoneDetHighBit                     	  BIT10
  #define HarphoneDetHighFlag			  (MonitorFlags&bHarphoneDetHighBit)
  #define Set_HarphoneDetHighFlag()		   (MonitorFlags|=bHarphoneDetHighBit)
  #define Clr_HarphoneDetHighFlag()		   (MonitorFlags&=~bHarphoneDetHighBit)
#endif

#if Enable_Menu_LockOSD
    #define bUserPrefOsdLockBit                       BIT11
    #define UserPrefOsdLockFlag            	   	(MonitorFlags&bUserPrefOsdLockBit)
    #define Set_UserPrefOsdLockFlag()		(MonitorFlags|=bUserPrefOsdLockBit)
    #define Clr_UserPrefOsdLockFlag()		(MonitorFlags&=~bUserPrefOsdLockBit)
#endif
#if Enable_LockPowerKey
    #define PowerLockBit      				BIT12
    #define PowerLockFlag                    		(MonitorFlags&PowerLockBit)
    #define Set_PowerLockFlag()          		(MonitorFlags|=PowerLockBit)
    #define Clr_PowerLockFlag()          	 	(MonitorFlags&=~PowerLockBit)   
#endif

#if HDMI_EDID_Write_To_24C02
    #define bLoadHDMIEDIDBit                       BIT13
    #define LoadHDMIEDIDFlag            		   (MonitorFlags&bLoadHDMIEDIDBit)
    #define Set_LoadHDMIEDIDFlag()		   (MonitorFlags|=bLoadHDMIEDIDBit)
    #define Clr_LoadHDMIEDIDFlag()		   (MonitorFlags&=~bLoadHDMIEDIDBit)
#endif

#if VGA_EDID_Write_To_24C02
   #define bLoadVGAEDIDBit                     BIT14
   #define LoadVGAEDIDFlag                     (MonitorFlags&bLoadVGAEDIDBit )
   #define Set_LoadVGAEDIDFlag()                 (MonitorFlags|=bLoadVGAEDIDBit)
   #define Clr_LoadVGAEDIDFlag()                  (MonitorFlags&=~bLoadVGAEDIDBit)
#endif

#if DVI_EDID_Write_To_24C02
   #define bLoadDVIEDIDBit                     BIT15
   #define LoadDVIEDIDFlag                     (MonitorFlags&bLoadDVIEDIDBit )
   #define Set_LoadDVIEDIDFlag()                 (MonitorFlags|=bLoadDVIEDIDBit)
   #define Clr_LoadDVIEDIDFlag()                  (MonitorFlags&=~bLoadDVIEDIDBit)
#endif
//========= flags status
#define PowerOnFlag         (MonitorFlags&bPowerOnBit)
#define FactoryModeFlag     (MonitorFlags&bFactoryModeBit)
#define BurninModeFlag          (MonitorFlags&bBurninModeBit)
#define DoBurninModeFlag        (MonitorFlags&bDoBurninModeBit)
#define OsdLockModeFlag         (MonitorFlags&bOsdLockModeBit)
#define ShowOsdLockFlag         (MonitorFlags&bShowOsdLockBit)
//#define MuteOnFlag                (MonitorFlags&bMuteOnBit)
//#define OverDriveOnFlag             (MonitorFlags&bOverDriveOnBit)
#define DDCciFlag               (MonitorFlags&bDCCCIONOBit)
#define TNROnFlag               (MonitorFlags&bTNROnBit)

#if ENABLE_OSD_ROTATION
#define MENU_ROTATION_FLAG          (MonitorFlags&bMenuRotationBit)
#endif
//--------- flags setting
#define Set_PowerOnFlag()         (MonitorFlags|=bPowerOnBit)
#define Set_FactoryModeFlag()     (MonitorFlags|=bFactoryModeBit)
#define Set_BurninModeFlag()      (MonitorFlags|=bBurninModeBit)
#define Set_DoBurninModeFlag()  (MonitorFlags|=bDoBurninModeBit)
#define Set_OsdLockModeFlag()     (MonitorFlags|=bOsdLockModeBit)
#define Set_ShowOsdLockFlag()     (MonitorFlags|=bShowOsdLockBit)
//#define Set_MuteOnFlag()        (MonitorFlags|=bMuteOnBit)
//#define Set_OverDriveOnFlag()   (MonitorFlags|=bOverDriveOnBit)
#define Set_DDCciFlag()     (MonitorFlags|=bDCCCIONOBit)
#define Set_TNROnFlag()     (MonitorFlags|=bTNROnBit)

#define Clr_PowerOnFlag()         (MonitorFlags&=~bPowerOnBit)
#define Clr_FactoryModeFlag()     (MonitorFlags&=~bFactoryModeBit)
#define Clr_BurninModeFlag()      (MonitorFlags&=~bBurninModeBit)
#define Clr_DoBurninModeFlag() (MonitorFlags&=~bDoBurninModeBit)
#define Clr_OsdLockModeFlag()     (MonitorFlags&=~bOsdLockModeBit)
#define Clr_ShowOsdLockFlag()     (MonitorFlags&=~bShowOsdLockBit)
//#define Clr_MuteOnFlag()        (MonitorFlags&=~bMuteOnBit)
//#define Clr_OverDriveOnFlag()   (MonitorFlags&=~bOverDriveOnBit)
#define Clr_DDCciFlag()     (MonitorFlags&=~bDCCCIONOBit)
#define Clr_TNROnFlag()     (MonitorFlags&=~bTNROnBit)
#if ENABLE_OSD_ROTATION
#define SET_MENU_ROTATION_FLAG()    (MonitorFlags|=bMenuRotationBit)
#define CLR_MENU_ROTATION_FLAG()    (MonitorFlags&=~bMenuRotationBit)
#endif


//==================================================================================
// System status flags
//#define bInputTimingChangeBit       BIT0
#define bPowerSavingBit         BIT1    // system power saving status
#define bForcePowerSavingBit        BIT2    // indicated system must enter power down
#define bInputTimingStableBit       BIT3    // input timing stable
#define bDoModeSettingBit           BIT4    //
#define bCableNotConnectedBit       BIT5
#define bFreeRunModeBit         BIT6
#define bOsdTimeoutBit              BIT7    // osd timeout
#define bSaveMonitorSettingBit      BIT8    // flag for storing settings into NVRam
#define bSaveModeSettingBit     BIT9
#define bChangePatternBit           BIT10   // switch input port
#define bRxBusyBit                  BIT11   // debug is busy
#define bms500Bit                   BIT12
#define bReverseXBit                BIT13
#define bReverseYBit                BIT14
#define bSARwakeupBit       BIT15
//=========System2 Flag Bit================================================================
#define bDisplayLogoBit             BIT0
#define bFlashBit                       BIT1
#define bReadyForChangeBit      BIT2
#define bLoadOSDDataBit             BIT3
//#define bLoadFontBit              BIT4
#define bPushECOHotKeyBit              BIT4
#define bAutoColorBit                   BIT5
#define bDLKOverBit             BIT6
#define bPressExitKeyBit            BIT7
#define bEnableShowAutoBit      BIT8

#define bShowInputInfoBit           BIT9
#define bJustShowValueBit           BIT10
#define bDecVScaleBit               BIT11
//#define bPressMenuKeyBit          BIT12
//#define bMuteBit                  BIT13
#define bBackToStandbyBit                   BIT12
#define bBackToUnsupportBit                 BIT13
#define bProductModeBit         BIT14
#define bForcePowerDownBit      BIT15
//=============================System3 Flag Bit==============================
#define bOSDTimeOutSet3DBit                 BIT0  // Rex 100701

//========== system flags status
//#define InputTimingChangeFlag       (SystemFlags&bInputTimingChangeBit)
#define PowerSavingFlag         (SystemFlags&bPowerSavingBit)
#define ForcePowerSavingFlag        (SystemFlags&bForcePowerSavingBit)
#define InputTimingStableFlag       (SystemFlags&bInputTimingStableBit)
#define DoModeSettingFlag           (SystemFlags&bDoModeSettingBit)
#define CableNotConnectedFlag       (SystemFlags&bCableNotConnectedBit)
#define FreeRunModeFlag         (SystemFlags&bFreeRunModeBit)
#define RxBusyFlag                  (SystemFlags&bRxBusyBit)
#define OsdTimeoutFlag          (SystemFlags&bOsdTimeoutBit)
#define ChangePatternFlag           (SystemFlags&bChangePatternBit)
#define SaveMonitorSettingFlag      (SystemFlags&bSaveMonitorSettingBit)
#define SaveModeSettingFlag     (SystemFlags&bSaveModeSettingBit)
//#define ms500Flag                   (SystemFlags&bms500Bit)
#define ReverseXFlag                (SystemFlags&bReverseXBit)
#define ReverseYFlag                (SystemFlags&bReverseYBit)
#define SARwakeupFlag       (SystemFlags&bSARwakeupBit)
//#define AutoColorFlag             (SystemFlags&bAutoColorBit)
//--------- System2 Flag status------
#define DisplayLogoFlag             (System2Flags&bDisplayLogoBit)
#define FlashFlag               (System2Flags&bFlashBit)
#define ReadyForChangeFlag          (System2Flags&bReadyForChangeBit)
#define LoadOSDDataFlag             (System2Flags&bLoadOSDDataBit)
//#define LoadFontFlag              (System2Flags&bLoadFontBit)
#define PushECOHotKeyFlag              (System2Flags&bPushECOHotKeyBit)     //111110 Rick modified adjust ECO condition - B40139
//#define AutoColorFlag               (System2Flags&bAutoColorBit)
//#define DisableLuminanceFlag      (System2Flags&bDisableLuminanceBit)
//#define DLKOverFlag               (System2Flags&bDLKOverBit)
//#define SpeedupIncBrighnessFlag           (System2Flags&bSpeedupIncBrighnessBit)
#define ShowInputInfoFlag           (System2Flags&bShowInputInfoBit)
#define JustShowValueFlag           (System2Flags&bJustShowValueBit)

//    #define PanelOnFlag               (System2Flags&bPanelOnBit)
#define BackToStandbyFlag               (System2Flags&bBackToStandbyBit)
#define BackToUnsupportFlag             (System2Flags&bBackToUnsupportBit)

//=======================================================================================//
// for hot key control
//=======================================================================================//
#define PressExitFlag               (System2Flags&bPressExitKeyBit)
#define EnableShowAutoFlag          (System2Flags&bEnableShowAutoBit)
//#define PressMenuFlag             (System2Flags&bPressMenuKeyBit)
//#define MuteFlag          (System2Flags&bMuteBit)
#define ProductModeFlag         (System2Flags&bProductModeBit)
//#define ForcePowerDownFlag          (System2Flags&bForcePowerDownBit)
//--------- system flags setting
//#define Set_InputTimingChangeFlag() (SystemFlags|=bInputTimingChangeBit)
//#define Clr_InputTimingChangeFlag() (SystemFlags&=~bInputTimingChangeBit)
#define Set_PowerSavingFlag()           (SystemFlags|=bPowerSavingBit)
#define Clr_PowerSavingFlag()           (SystemFlags&=~bPowerSavingBit)
#define Set_ForcePowerSavingFlag()  (SystemFlags|=bForcePowerSavingBit)
#define Clr_ForcePowerSavingFlag()  (SystemFlags&=~bForcePowerSavingBit)
#define Set_InputTimingStableFlag() (SystemFlags|=bInputTimingStableBit)
#define Clr_InputTimingStableFlag() (SystemFlags&=~bInputTimingStableBit)
#define Set_RxBusyFlag()                (SystemFlags|=bRxBusyBit)
#define Clr_RxBusyFlag()                (SystemFlags&=~bRxBusyBit)
#define Set_DoModeSettingFlag()     (SystemFlags|=bDoModeSettingBit)
#define Clr_DoModeSettingFlag()     (SystemFlags&=~bDoModeSettingBit)
#define Set_OsdTimeoutFlag()            (SystemFlags|=bOsdTimeoutBit)
#define Clr_OsdTimeoutFlag()            (SystemFlags&=~bOsdTimeoutBit)
#define Set_CableNotConnectedFlag() (SystemFlags|=bCableNotConnectedBit)
#define Clr_CableNotConnectedFlag() (SystemFlags&=~bCableNotConnectedBit)
#define Set_FreeRunModeFlag()           (SystemFlags|=bFreeRunModeBit)
#define Clr_FreeRunModeFlag()           (SystemFlags&=~bFreeRunModeBit)
#define Set_ChangePatternFlag()     (SystemFlags|=bChangePatternBit)
#define Clr_ChangePatternFlag()     (SystemFlags&=~bChangePatternBit)
#define Set_SaveMonitorSettingFlag()            (SystemFlags|=bSaveMonitorSettingBit)
#define Clr_SaveMonitorSettingFlag()            (SystemFlags&=~bSaveMonitorSettingBit)
#define Set_SaveModeSettingFlag()       (SystemFlags|=bSaveModeSettingBit)
#define Clr_SaveModeSettingFlag()       (SystemFlags&=~bSaveModeSettingBit)
//#define Set_ms500Flag()             (SystemFlags|=bms500Bit)
//#define Clr_ms500Flag()             (SystemFlags&=~bms500Bit)
#define Set_ReverseXFlag()          (SystemFlags|=bReverseXBit)
#define Clr_ReverseXFlag()          (SystemFlags&=~bReverseXBit)
#define Set_ReverseYFlag()          (SystemFlags|=bReverseYBit)
#define Clr_ReverseYFlag()          (SystemFlags&=~bReverseYBit)
#define Set_SARwakeupFlag()         (SystemFlags|=bSARwakeupBit)
#define Clr_SARwakeupFlag()         (SystemFlags&=~bSARwakeupBit)

//#define Set_ISPFlag()                 (SystemFlags|=bISPBit)
//#define Clr_ISPFlag()                 (SystemFlags&=~bISPBit)
//=============================================================
//Set AOC LOGO STATUS
#define Set_DisplayLogoFlag()          (System2Flags|=bDisplayLogoBit)
#define Clr_DisplayLogoFlag()          (System2Flags&=~bDisplayLogoBit)
#define Set_FlashFlag()                (System2Flags|=bFlashBit)
#define Clr_FlashFlag()                (System2Flags&=~bFlashBit)
#define Set_ReadyForChangeFlag()       (System2Flags|=bReadyForChangeBit)
#define Clr_ReadyForChangeFlag()       (System2Flags&=~bReadyForChangeBit)
#define Set_LoadOSDDataFlag()          (System2Flags|=bLoadOSDDataBit)
#define Clr_LoadOSDDataFlag()          (System2Flags&=~bLoadOSDDataBit)
#define Set_PushECOHotKeyFlag()           (System2Flags|=bPushECOHotKeyBit)
#define Clr_PushECOHotKeyFlag()           (System2Flags&=~bPushECOHotKeyBit)
//#define Set_AutoColorFlag()            (System2Flags|=bAutoColorBit)
//#define Clr_AutoColorFlag()            (System2Flags&=~bAutoColorBit)
//#define Set_DisableLuminanceFlag()  (System2Flags|=bDisableLuminanceBit)
//#define Clr_DisableLuminanceFlag()  (System2Flags&=~bDisableLuminanceBit)
//#define Set_DLKOverFlag()  (System2Flags|=bDLKOverBit)
//#define Clr_DLKOverFlag()  (System2Flags&=~bDLKOverBit)
//#define Set_SpeedupIncBrighnessFlag()  (System2Flags|=bSpeedupIncBrighnessBit)
//#define Clr_SpeedupIncBrighnessFlag()  (System2Flags&=~bSpeedupIncBrighnessBit)
#define Set_ShowInputInfoFlag()  (System2Flags|=bShowInputInfoBit)
#define Clr_ShowInputInfoFlag()  (System2Flags&=~bShowInputInfoBit)
#define Set_JustShowValueFlag()  (System2Flags|=bJustShowValueBit)
#define Clr_JustShowValueFlag()  (System2Flags&=~bJustShowValueBit)

//    #define Set_PanelOnFlag()         (System2Flags|=bPanelOnBit)
//    #define Clr_PanelOnFlag()         (System2Flags&=~bPanelOnBit)
#define Set_PressExitFlag()         (System2Flags|=bPressExitKeyBit)
#define Clr_PressExitFlag()         (System2Flags&=~bPressExitKeyBit)
#define Set_EnableShowAutoFlag()   (System2Flags|=bEnableShowAutoBit)
#define Clr_EnableShowAutoFlag()   (System2Flags&=~bEnableShowAutoBit)
//#define Set_PressMenuFlag()       (System2Flags|=bPressMenuKeyBit)
//#define Clr_PressMenuFlag()           (System2Flags&=~bPressMenuKeyBit)
//#define Set_MuteFlag()            (System2Flags|=bMuteBit)
//#define Clr_MuteFlag()            (System2Flags&=~bMuteBit)
#define Set_ProductModeFlag()       (System2Flags|=bProductModeBit)
#define Clr_ProductModeFlag()           (System2Flags&=~bProductModeBit)
//#define Set_ForcePowerDownFlag()            (System2Flags|=bForcePowerDownBit)
//#define Clr_ForcePowerDownFlag()            (System2Flags&=~bForcePowerDownBit)

#define Set_BackToStandbyFlag()       (System2Flags|=bBackToStandbyBit)
#define Clr_BackToStandbyFlag()        (System2Flags&=~bBackToStandbyBit)

#define Set_BackToUnsupportFlag()       (System2Flags|=bBackToUnsupportBit)
#define Clr_BackToUnsupportFlag()        (System2Flags&=~bBackToUnsupportBit)

//#define Set_PowerGoodExistFlag()             (PowerGoodExistFlag=1)
//#define Clr_PowerGoodExistFlag()                (PowerGoodExistFlag=0)
//====================System3flags===================================
#define OSDTimeOutSet3D             (System3Flags&bOSDTimeOutSet3DBit)  // Rex 100701
#define Set_OSDTimeOutSet3DFlag()       (System3Flags|=bOSDTimeOutSet3DBit)
#define Clr_OSDTimeOutSet3DFlag()        (System3Flags&=~bOSDTimeOutSet3DBit)

//===============================================================

#if 1 // ENABLE_HDCP
//=======================================================================================//
// for
//=======================================================================================//
#define bHdcpNVRamBit           BIT1
#define bWriteHDCPcodeBit           BIT2
#define bLoadHDCPKeyBit             BIT3
#define LoadHDCPKeyFlag   (HDCPFlag&bLoadHDCPKeyBit)
#define Set_LoadHDCPKeyFlag()      (HDCPFlag|=bLoadHDCPKeyBit)
#define Clr_LoadHDCPKeyFlag()      (HDCPFlag&=~bLoadHDCPKeyBit)
#define HdcpNVRamFlag           (HDCPFlag&bHdcpNVRamBit)
#define Set_HdcpNVRamFlag()         (HDCPFlag|=bHdcpNVRamBit)
#define Clr_HdcpNVRamFlag()         (HDCPFlag&=~bHdcpNVRamBit)
#define WriteHDCPcodeFlag                  (HDCPFlag&bWriteHDCPcodeBit)
#define Set_WriteHDCPcodeFlag()       (HDCPFlag|=bWriteHDCPcodeBit)
#define Clr_WriteHDCPcodeFlag()     (HDCPFlag&=~bWriteHDCPcodeBit)
#endif
//=================================================================


extern bit DebugOnlyFlag;
#define Set_DebugOnlyFlag()          (DebugOnlyFlag=1)
#define Clr_DebugOnlyFlag()             (DebugOnlyFlag=0)
extern bit SaveFactorySettingFlag;
#define Set_SaveFactorySettingFlag()             (SaveFactorySettingFlag=1)
#define Clr_SaveFactorySettingFlag()                (SaveFactorySettingFlag=0)

extern BYTE GetPow2(BYTE exp);
extern BYTE GetLog(DWORD value);

extern Bool IsOverScan(void);

typedef enum
{
    Items_Off,
    Items_On,
};

typedef enum
{
    MainMenuItems_BriteCont,
    MainMenuItems_ColorSettings,
    MainMenuItems_InputSource,
    MainMenuItems_3D,
    MainMenuItems_DisplaySettings,
    MainMenuItems_OtherSettings,
    MainMenuItems_Information,
    MainMenuItems_AutoAdjust,

    MainMenuItems_Max,
};

typedef enum
{
    BriteContMenuItems_Brightness,
    BriteContMenuItems_Contrast,
#if ENABLE_DLC
    BriteContMenuItems_DLC,
#endif
#if ENABLE_DPS
    BriteContMenuItems_DPS,
#endif
#if ENABLE_DSC
    BriteContMenuItems_DSC,
#endif
    BriteContMenuItems_DCR,
    BriteContMenuItems_Max,
};

typedef enum
{
    ColorSettingsMenuItems_ColorTemp,
    ColorSettingsMenuItems_Red,
    ColorSettingsMenuItems_Green,
    ColorSettingsMenuItems_Blue,
    ColorSettingsMenuItems_Hue,
    ColorSettingsMenuItems_Saturation,
#if ENABLE_SUPER_RESOLUTION
    ColorSettingsMenuItems_SuperResolution,
#endif
    ColorSettingsMenuItems_ColorMode,
    ColorSettingsMenuItems_ColorFormat,
    ColorSettingsMenuItems_Max,
};

typedef enum
{
    ColorModeMenuItems_Off,
    ColorModeMenuItems_Enhance,
    ColorModeMenuItems_Demo,

    ColorModeMenuItems_Max,
};

typedef enum
{
    SuperResolutionMenuItems_Off,
    SuperResolutionMenuItems_Weak,
    SuperResolutionMenuItems_Middle,
    SuperResolutionMenuItems_Strong,

    SuperResolutionMenuItems_Max,
};

typedef enum
{
    InputSourceMenuItems_Auto,
    InputSourceMenuItems_VGA,
    InputSourceMenuItems_Digital0,
    InputSourceMenuItems_Digital1,
    InputSourceMenuItems_Digital2,

    InputSourceMenuItems_Max,
};

typedef enum
{
    _3DMenuItems_3DFormat,
    _3DMenuItems_2Dto3D,
    _3DMenuItems_Separation,
    _3DMenuItems_Depth,

    _3DMenuItems_Max,
};

typedef enum
{
    _3DFormatMenuItems_Auto,
    _3DFormatMenuItems_SidebySide,
    _3DFormatMenuItems_TopandBottom,

    _3DFormatMenuItems_Max,
};

typedef enum
{
    _2Dto3DMenuItems_Off,
    _2Dto3DMenuItems_On,

    _2Dto3DMenuItems_Max,
};

typedef enum
{
    DisplaySettingsMenuItems_Gamma,
    DisplaySettingsMenuItems_HPos,
    DisplaySettingsMenuItems_VPos,
#if ENABLE_SHARPNESS
    DisplaySettingsMenuItems_Sharpness,
#endif
    DisplaySettingsMenuItems_Clock,
    DisplaySettingsMenuItems_Phase,
#if Enable_Expansion
    DisplaySettingsMenuItems_Aspect,
#endif
    #if ENABLE_RTE
    DisplaySettingsMenuItems_OD,
    #endif

    DisplaySettingsMenuItems_Max,
};

typedef enum
{
    GammaMenuItems_Off,
    GammaMenuItems_On,

    GammaMenuItems_Max,
};

typedef enum
{
    AspectRatioMenuItems_Full,
    AspectRatioMenuItems_4_3,
    AspectRatioMenuItems_16_9,
    AspectRatioMenuItems_1_1,
    AspectRatioMenuItems_KeepInputRatio,
    AspectRatioMenuItems_OverScan,
    AspectRatioMenuItems_Max,
};

typedef enum
{
    ODMenuItems_Off,
    ODMenuItems_On,

    ODMenuItems_Max,
};

typedef enum
{
    OtherSettingsMenuItems_Language,
#if AudioFunc
    OtherSettingsMenuItems_Volume,
#endif
    OtherSettingsMenuItems_OSDTrans,
    OtherSettingsMenuItems_OSDHPos,
    OtherSettingsMenuItems_OSDVPos,
    OtherSettingsMenuItems_OSDTimeout,
#if ENABLE_OSD_ROTATION
    OtherSettingsMenuItems_OSDRotate,
#endif
#if ENABLE_FREESYNC
    OtherSettingsMenuItems_FreeSync,
#endif
    OtherSettingsMenuItems_Reset,

    OtherSettingsMenuItems_Max,
};


typedef enum  //add by smc.lmw 20160628
{
    OSDLanguageMenuItems_English,
    OSDLanguageMenuItems_Chinese,
};
#if ENABLE_OSD_ROTATION
typedef enum
{
    OSDRotateMenuItems_Off,
    OSDRotateMenuItems_90,
#if defined(_OSD_ROTATION_180_)
    OSDRotateMenuItems_180,
#endif
#if defined(_OSD_ROTATION_270_)
    OSDRotateMenuItems_270,
#endif
    OSDRotateMenuItems_Max,
};
#endif

#if ENABLE_FREESYNC
typedef enum
{
    FreeSyncMenuItems_Off,
    FreeSyncMenuItems_On,
    FreeSyncMenuItems_Max,
};
#define IS_HDMI_FREESYNC_ISR()  (msAPI_combo_IPGetDDRFlag_ISR()&&CURRENT_INPUT_IS_HDMI())
#define IS_HDMI_FREESYNC()    	(msAPI_combo_IPGetDDRFlag()&&CURRENT_INPUT_IS_HDMI())

#define IS_DP_FREESYNC()      	(msAPI_combo_IPGetDDRFlag()&&CURRENT_INPUT_IS_DISPLAYPORT())
#endif



#endif
