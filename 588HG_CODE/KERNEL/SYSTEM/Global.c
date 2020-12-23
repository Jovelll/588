#include "types.h"
#include "board.h"
#include "global.h"
#include "Debug.h"
#include "Common.h"
#if (LANGUAGE_TYPE==ASIA)||(LANGUAGE_TYPE==SIXTEEN)
#include "menudef.h"
#endif
#include "MsDLC.h"
#include "MsACE.h"
#include "Userpref.h"
#include "mseread.h"
#if ENABLE_DPS
#include "msDPS_Setting.h"
#endif
#if ENABLE_DSC
#include "msDSC_Setting.h"
#endif
#include "COLOR_VERIFY.h"
#include "msEread.h"
#include "Detect.h"
#if MS_DAC
#include "drvDAC.h"
#endif
#include "menufunc.h"
#if 1///ENABLE_EDP_OUTPUT
#include "DPTxApp.h"
#endif
#if CHIP_ID >= CHIP_TSUMC
volatile BOOL g_bOutputVsyncIsrFlag;
#endif




#if ENABLE_WATCH_DOG_INT
BYTE xdata u8WDT_Status=0;
#endif

#if EnableTime1Interrupt
WORD XDATA g_u16MsCounter1 = 1000; // for System timer
BOOL XDATA bSecondTimer1 = FALSE;
BYTE XDATA u8SecondTIMER1 = 0;
#endif

#if (CHIP_ID == CHIP_TSUM9||CHIP_ID == CHIP_TSUMF)
BOOL g_bFROTrimResult;
#endif

#if ENABLE_DPS||ENABLE_DSC
BOOL idata bRunToolFlag = 0;
#endif
BYTE g_ucTimer0_TH0;
BYTE g_ucTimer0_TL0;
BYTE xdata g_u8SystemSpeedMode=0;
#if IR_FUNC_EN
BYTE xdata g_ucTimer1_TH0;
BYTE xdata g_ucTimer1_TL0;
BYTE idata IRClearCounter=0;
//BYTE idata IRFLAG=0;
#endif

#if DEPOP_DCONOFF_FLOW
BYTE xdata u8PowerDownCount=0;
#endif

#if ENABLE_DP_INPUT
bit g_bDoDPInit;
#endif

#if ENABLE_MHL
BYTE xdata MHLExtenCountFlag = 0; // 0 means current MHL port has 1 chance to extend time for staying at current port
#endif

BYTE  xdata g_SwitchSec=DEF_FORCE_DPMS;
BYTE  xdata g_CountSwitchPortTimeFlag=FALSE;


bit g_bMcuPMClock = 0;
bit SecondFlag = 0;
bit ms10Flag = 0;
bit ms50Flag = 0;
bit ms500Flag = 0; // 120815 coding, set to bit data type for isr usage
bit ms1000Flag = 0;

bit InputTimingChangeFlag = 0; // 120815 coding, set to bit data type for isr usage

#if ENABLE_USB_INPUT
DWORD xdata u32UsbCount = 0;
#endif

bit g_bInputSOGFlag = 0;
BYTE bInputVGAisYUV = 0;   // (0) VGA is RGB    (1) VGA is YUV //20150121

WORD ms_Counter = SystemTick; // for System timer
BYTE xdata ModeDetectCounter = 20;
WORD xdata u16PanelOffOnCounter = 0;
bit bPanelOffOnFlag = 0;

bit bPanelOnFlag = 0; // 100811 coding addition to indicate panel power status
BYTE xdata BackLightActiveFlag;
xdata BYTE KeypadButton = 0;

BYTE xdata DDCWPActiveFlag; //cc 2011.09.09 15:50

BYTE xdata Second;

#if MS_DAC
WORD xdata w16DAC_DePopCounter = 0;
bit bDAC_DePopCountFlag = 0;
BYTE xdata w16DAC_DePopStep = 0;
bit bDAC_DePopFlag = 0;
#endif

BYTE xdata TPDebunceCounter = 0;
bit StartScanKeyFlag = 0;
bit bKeyReadyFlag = 0;
#if ENABLE_TOUCH_KEY
#if TOUCH_KEY_POWER_KEY_DEBOUNCE
bit bPowerKeyPressed = 0;
BYTE xdata PowerKeyDebounceCntr = 0;
#endif
#endif

bit TI0_FLAG; // 120815 coding, set to bit data type for isr usage
BYTE xdata SIORxBuffer[_UART_BUFFER_LENGTH_];
BYTE xdata SIORxIndex;
BYTE xdata u8ReSyncCounter;

BYTE xdata OsdCounter = 0;
BYTE xdata HotKeyCounter = 0;
BYTE xdata PowerDownCounter = 0;
BYTE xdata InputTimingStableCounter = 0;

DWORD xdata BlacklitTime = 0;

#if ENABLE_BURNIN_MENU
DWORD xdata BurninTime = 0;
bit SaveBurninTimeFlag = 0;
#endif

bit SaveBlacklitTimeFlag = 0;
//BYTE LanguageState=0;
//BYTE CurrentLanguage=0;   //wumaozhong add 20050615
//BYTE ShortcutLuminance=0;
//BYTE SaveIndex=0; // saveIndex
WORD xdata SystemFlags = 0; // for system status flags
WORD xdata System2Flags = 0;
BYTE xdata System3Flags = 0;  // Rex 100701 for 3D
//BYTE xdata ChipVer = 0;
BYTE xdata SaveIndex = 0; // saveIndex
BYTE xdata PatternNo = 0;
BYTE xdata BrightnessTemp = 0;
BYTE xdata PrevHistoValue = 0;
#if ENABLE_FACTORY_AUTOTEST
bit BFactoryAutoTest=0;
#endif

#if ENABLE_TIME_MEASUREMENT
WORD volatile xdata g_u16TimeMeasurementCounter = 0;
bit bTimeMeasurementFlag=0;
WORD xdata g_u16Time1;
WORD xdata g_u16Time2;
#endif

bit gBoolVisualKey = 0;
BYTE xdata gByteVisualKey = 0xff;
BYTE xdata LanguageIndex;
BYTE xdata SecondTblAddr;
#if ENABLE_HDCP
BYTE xdata HDCPFlag = 0;
#endif
#if ENABLE_HDCP&&HDCPKEY_IN_Flash
BYTE xdata HDCPDataBuffer[6]; //[6];
#endif

BYTE xdata InputColorFormat = 0; // indicate input color space is RGB or YUV

bit DebugOnlyFlag = 0;
#if 1//Enable_Expansion
bit bExpansionBit = 0;
#endif
bit SaveFactorySettingFlag = 0;

WORD u16DelayCounter = 0;
bit bDelayFlag = 0;

WORD xdata u16TimeOutCounter = 0; // 120510 coding, addition for checking register status
bit bTimeOutCounterFlag = 0;
bit SaveSettingFlag=0;
#if ShowSourceType
bit DisplayInputMsgFlag=0;
#endif
BYTE xdata UserPrefSelectInputType=0;
#if Display_Checksum						//added  for factory menu checksum on jan 24th ,2019
WORD xdata  Code_checksun;
#endif

#ifdef Internal_EDID_Write_To_24C02
 bit bNoUsePin;
BYTE xdata EnableReadDDCType=I2C_EEPROM;
BYTE xdata DDC1[256];

#endif

#if EN_HotKeyTimes
BYTE xdata HotKeyTimes_Minutes=0;
BYTE xdata HotKeyTimes_Second=0;

#endif


bit g_bServiceEDIDUnLock = 0;

OverScanType  xdata OverScanSetting;

PanelSettingType xdata PanelSetting;
InputTimingType xdata InputTiming;
MonitorSettingType xdata MonitorSetting; //Benz 2007.4.15   19:44:07idata - >xdata
#if USEFLASH
MonitorSettingType2 xdata  MonitorSetting2;
#endif
ModeSettingType xdata ModeSetting;
FactorySettingType xdata  FactorySetting;
//==========================for AutoBacklight=====
BYTE xdata HistogramNow = 0;
BYTE xdata HistogramPrev = 0;
BYTE xdata BacklightNow = 0;
BYTE xdata BacklightPrev = 0;
BYTE xdata HistogramFlag = 1;

//xdata BYTE xfr_regs[256] _at_ 0xC000;
//xdata BYTE g_FlashID = 0;

BYTE xdata LastMenuPage = 0;

//bit PowerGoodExistFlag = 0;

extern void msdrvDPRxAppDummy(void);
extern void msDebugDummy(void);
extern void msDPSSettingDummy(void);
extern void msDPSHandlerDummy(void);
extern void msDSCSettingDummy(void);
extern void msDSCHandlerDummy(void);
extern void msdrvHDMIDummy(void);
extern void HDMIDummy(void);
extern void msMhalMHLDummy(void);
extern void msMapiMHLDummy(void);
extern void msMdrvMHLDummy(void);
extern void msGammaDummy(void);
extern void msHDCPDummy(void);
extern void msPMDummy(void);
extern void msDACDummy(void);
extern void mStarDummy(void);
extern void msapmStarDummy(void);
extern void msMainDummy(void);
extern void msAdjustDummy(void);

BYTE xdata SwitchPortCntr = 0;
//bit g_bBacklightOn = 0;

BYTE code UserDataCode[];// =//Set UserDataCode To Bank3 Addr:A000

#if DECREASE_V_SCALING
WORD xdata DecVScaleValue = 0;
#endif

#if ENABLE_DAC_DEPOP
BYTE EARDePopFlag = 0;
BYTE EARDePopStep = 1;
WORD xdata ucDePOP4SecCounter = 0;
#endif

#if 1//CHIP_ID == CHIP_TSUM2 || CHIP_ID == CHIP_TSUMU || CHIP_ID == CHIP_TSUMC
void msFBDummy(void);
#endif

#if !ENABLE_USB_INPUT
void mshidcoreDummy(void);
void mshostlibDummy(void);
void msusbmainDummy(void);
void mshostlib_2Dummy(void);
void msusbmain_2Dummy(void);
void mshidcore_2Dummy(void);
#endif
#if (!ENABLE_DEBUG) || (!ENABLE_USB_INPUT)
void msdrvintpipeDummy(void);
void msdrvintpipe_2Dummy(void);
#endif

#if !ENABLE_RTE
void msOverDriveDummy();
#endif
#if 1//!ENABLE_RTE
void drvmsOVDDummy(void);
#endif

#if !ENABLE_LED_CONTROLLER
    msLEDControllerDummy();
#endif
#if  !ENABLE_3DLUT
 void ms3DLUT_Dummy();
#endif

#if  !ENABLE_R2_2DTO3D
void ms2DTo3DDummy(void);
#endif

#if ENABLE_TOUCH_KEY
BYTE xdata TouchKeyRestortCounter; // unit: 50mS
    #if ENABLE_LOW_CONTACT
    BYTE xdata TouchKeyLowContactDelay; // unit: 50mS
    BYTE xdata TouchKeyLowContactDetectCntr; // unit: 1S, use to detect low contact peroid
    #endif
    #if 0//TOUCH_KEY_CTRL_LED
    BYTE xdata TouchKeySetLEDStatus;
    #endif
#else
void TouchKeyDummy(void);
#endif

extern void UserDataSpaceDummy(void);

void DummyCall(void)
{
    BYTE i = 0;

    if(i)
    {
        (WORD)i = GetStandardModeVTotal();
        UserDataSpaceDummy();
    #if 1//ENABLE_DEBUG
        mStarDummy();
        msapmStarDummy();
        msMainDummy();
        msAdjustDummy();
    #endif

    #if !ENABLE_USB_INPUT && (CHIP_ID == CHIP_TSUMD)
        mshidcoreDummy();
        mshostlibDummy();
        msusbmainDummy();
        mshostlib_2Dummy();
        msusbmain_2Dummy();
        mshidcore_2Dummy();
    #endif

    #if ((!ENABLE_DEBUG) || (!ENABLE_USB_INPUT)) && (CHIP_ID == CHIP_TSUMD)
        msdrvintpipeDummy();
        msdrvintpipe_2Dummy();
    #endif

    #if  !ENABLE_R2_2DTO3D
        ms2DTo3DDummy();
    #endif

    #if (!ENABLE_DP_INPUT) && (CHIP_ID == CHIP_TSUMU || CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID == CHIP_TSUMD || CHIP_ID == CHIP_TSUM9|| CHIP_ID == CHIP_TSUMF)
        msdrvDPRxAppDummy();
    #endif

    #if !ENABLE_DEBUG
        msDebugDummy();
    #endif

    #if !ENABLE_DPS
        msDPSSettingDummy();
        msDPSHandlerDummy();
    #endif

    #if !ENABLE_DSC
        msDSCSettingDummy();
        msDSCHandlerDummy();
    #endif

    #if (!ENABLE_DEBUG) || (!ENABLE_HDMI)
        HDMIDummy();
    #endif

    #if  1//(!ENABLE_HDMI)
        msdrvHDMIDummy();
    #endif

    #if 0//!ENABLE_MHL
        msMhalMHLDummy();
        msMapiMHLDummy();
        msMdrvMHLDummy();
    #endif

        msMHLDummy();

    #if !Enable_Gamma
        msGammaDummy();
    #endif

    #if (!ENABLE_DEBUG) || (!ENABLE_HDCP) || defined(FPGA)
        msHDCPDummy();
    #endif

    #if (!ENABLE_DEBUG) || (!MS_PM)
        msPMDummy();
    #endif

    #if 1//CHIP_ID == CHIP_TSUM2 || CHIP_ID == CHIP_TSUMU || CHIP_ID == CHIP_TSUMC
        msFBDummy();
    #endif

    #if !ENABLE_RTE
        msOverDriveDummy();
    #endif

    #if 1//!ENABLE_RTE
        drvmsOVDDummy();
    #endif

    #if !(ENABLE_DEBUG&&COLORVERIFY_DEBUG)
        msColorVerifyDummy();
    #endif

    #if !ENABLE_LED_CONTROLLER
        msLEDControllerDummy();
    #endif

    #if  !ENABLE_3DLUT
        ms3DLUT_Dummy();
    #endif

    #if !ENABLE_TOUCH_KEY
        TouchKeyDummy();
    #endif

    #if !ENABLE_MHL
        msEread_SetMHLInitialValue();
    #endif

    #if 1//!MS_DAC &&
        msDACDummy();
    #endif

        GetPow2(0);
        GetLog(0);
        GetImageHeight();
        GetImageWidth();

    #if defined(FPGA) || !(CHIP_ID==CHIP_TSUMC || CHIP_ID==CHIP_TSUMK ||CHIP_ID==CHIP_TSUMD)
    {
        BYTE bHDCPKeyInEfuse,ucValue,u8ADCBandgapTrimVal;
        bHDCPKeyInEfuse = msEread_IsHDCPKeyInEfuse();
        ucValue = msEread_GetHDCPKeyFromEfuse(0);
        msEread_CompareHDCPChecksumInEfuse(0);
        u8ADCBandgapTrimVal = msEread_GetADCBandgapTrimValFromEfuse();
        msEread_SetComboInitialValue();
        msEread_SetHDMIInitialValue();
        msEread_SetMHLInitialValue();
    }
    #endif

    #if (CHIP_ID == CHIP_TSUM9 ||CHIP_ID == CHIP_TSUMF)
    #if(MS_PM)
        msPM_StartRCOSCCal();
    #endif
        drvGPIO_PWMAlignVSyncDelay(0, 0);
        drvGPIO_PWMInverse(0, 0);
        msDrvWriteInternalEDID(0, 0, 0);
        msDrvReadInternalEDID(0, 0);
    #endif
    #if ENABLE_DEBUG && MS_DAC && (CHIP_ID==CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID==CHIP_TSUMD||CHIP_ID==CHIP_TSUM9||CHIP_ID==CHIP_TSUMF)
        PCM_GEN(0,0,0);
    #endif
    #if ENABLE_SW_DOUBLE_BUFFER
        msSWDBWrite2ByteMask(0, 0, 0);
        msSWDBWriteBit(0, 0, 0);
        msWrite3Byte(0, 0);
    #endif
        GetHSyncWidth();
		GetNonFuncValue();
	#if ENABLE_EDP_OUTPUT
	    DPTxCheckCRC();
	    DPAUX_TEST();
	    DPTxEDIDRead();
	#else
		msDpTxDummy();
	#endif
		msAdjustXVYccVideoHue( MAIN_WINDOW, 50 );
		msAdjustXVYCCVideoContrastRGB(MAIN_WINDOW, 0x80, 0x80, 0x80, 0x80);
		msAdjustXVYccVideoSaturation( MAIN_WINDOW, 128 );

    }
}

void Init_GlobalVariables( void )
{
/////////////////////////////////////////////////
// add for prevent complier warning
    DummyCall();
//////////////////////////////////////////////////
	#if Enable_Expansion
	OverScanSetting.Enable= 1;
	OverScanSetting.AspRatio= UserprefExpansionMode;
	OverScanSetting.ScanRatio= 98;
	OverScanSetting.ImageRatio= 100;
#endif

#if CHIP_ID >= CHIP_TSUMC
	g_bOutputVsyncIsrFlag = 0;
#endif

    InputTimingStableCounter = 0;
    Second = 0;
    OsdCounter = 0;
    PowerDownCounter = 0;
    bPanelOffOnFlag = 0;
    bPanelOnFlag = 0;
    Clr_StartScanKeyFlag();
    TPDebunceCounter = SKPollingInterval;

#if USEFLASH
    BlacklitTime = UserprefBacklighttime;
#endif

#if ENABLE_BURNIN_MENU
     BurninTime = UserprefBurnintime;
#endif

#if ENABLE_TOUCH_KEY
    TouchKeyRestortCounter = 0;
    #if ENABLE_LOW_CONTACT
    TouchKeyLowContactDetectCntr = TOUCH_KEY_LOW_CONTACT_DETECT_PEROID;
    TouchKeyLowContactDelay = 0;
    #endif
    #if 0//TOUCH_KEY_CTRL_LED
    TouchKeySetLEDStatus = TouchKeyLEDStatus_OFF;
    #endif
#endif

    SrcFlags = SyncLoss;
    SrcModeIndex = 0xFF;
#if (Customer_Type_Sel==Customer_XINTAO)
    if( BurninModeFlag) //&&FactoryModeFlag 
#else
	if( BurninModeFlag&&FactoryModeFlag) 
#endif
    {
        Set_DoBurninModeFlag();
    }
    else
    {
        Clr_DoBurninModeFlag();
		 Clr_BurninModeFlag();
    }

    if( !( ProductModeFlag || FactoryModeFlag ) )
        Set_ShowInputInfoFlag();

    if( PowerOnFlag )
    {
        Set_InputTimingChangeFlag();
#if DisplayPowerOnLogo
        if(!FactoryModeFlag) //eshin
            Set_DisplayLogoFlag();
	 else
	     Clr_DisplayLogoFlag();
#else
        Clr_DisplayLogoFlag();  //2009-10-13
#endif
    }
    else
    {
        SystemFlags |= bForcePowerSavingBit;
        Clr_DisplayLogoFlag();
    }

#if LANGUAGE_TYPE == ASIA
#if 0  //disable by smc.lmw 20160628

    if( UserPrefLanguage == LANG_TChina )
    {
        LanguageIndex = 1;
        SecondTblAddr = 0x80;
    }
    else if( UserPrefLanguage == LANG_Korea )
    {
        LanguageIndex = 2;
        SecondTblAddr = 0x80; //SecondTblAddr = 0xFF;
    }
    else
#endif
    {
        LanguageIndex = 0;
        SecondTblAddr = 0xFF;
    }
#elif LANGUAGE_TYPE == SIXTEEN                    //090330 xiandi.yu
    if( UserPrefLanguage == LANG_SChina )
    {
        LanguageIndex = 1;
        SecondTblAddr = 0x80;
    }
    else if( UserPrefLanguage == LANG_Japanese )
    {
        LanguageIndex = 2;
        SecondTblAddr = 0x80; //SecondTblAddr = 0xFF;
    }
    else
    {
        LanguageIndex = 0;
        SecondTblAddr = 0xFF;
    }
#elif LANGUAGE_TYPE == EUROPE
    LanguageIndex = 0;
    SecondTblAddr = 0xFF;
#endif

#if DECREASE_V_SCALING
    DecVScaleValue = 0;
#endif

#if ENABLE_DPS
#if DPS_UserBrightControl_EN
    REG_DPS_UserBrightContlGain = UserPrefBrightness;//(float)USER_PREF_BRIGHTNESS * 256 / 100;
#else
    REG_DPS_UserBrightContlGain = 100;
#endif
#endif
#if EarphoneDet_Enable
   if(PowerOnFlag)	  
   {
      if(HarphoneDet_Pin)//Ear Push In
      {
            Clr_HarphoneDetHighFlag();	
      }
      else
      {
            Set_HarphoneDetHighFlag();	
      }
   }
#endif

}

BYTE GetPow2(BYTE exp)
{
    if( exp == 0 )
        return 1;
    else
        return (0x02<<(exp-1));
}

BYTE GetLog(DWORD value)
{
    BYTE i = 0;

    if( value == 0 )
        return 0;

    while(value)
    {
        value >>= 1;
        i++;
    }

    return (i-1);
}

