#define _MSTAR_C_
#include <math.h>
#include "types.h"
#include "board.h"
#include "ms_reg.h"
#include "global.h"
#include "Mcu.h"
//#include "panel.h"
#include "adjust.h"
//#include "msADC.h"
#include "ms_rwreg.h"
#include "debug.h"
#include "Common.h"
#include "misc.h"
#include "ddc.h"
#include "power.h"
#include "detect.h"
#include "msOsd.h"
#include "gamma.h"
//#include "extlib.h"
#include "menu.h"
#include "menudef.h"
#include "menufunc.h"
#include "msflash.h"
//#include "msid_v1.h"
#include "drvgpio.h"
#include "drv_adctbl.h"
#include "drvadc.h"
#include "msEread.h"
#if ENABLE_RTE
#include "msovd.h"
#endif
#if ENABLE_FREESYNC
#include "drvDPRxApp.h"
#endif
#if (MS_PM)
#include "Ms_PM.h"
#endif
#include "MsHDCP.h"
#include "MsDLC.h"
#include "MsACE.h"
#include "Userpref.h"
#include "panel.h"
#if ENABLE_HDMI
#include "msHDMI.h"
#include "msHDMIACE.h"

#if ENABLE_CEC
#include "apiCEC.h"
#endif

#endif
#if ENABLE_EDP_OUTPUT
#include "DPTxApp.h"
#endif

#if MS_DAC
#include "drvdac.h"
#endif
#if ENABLE_3DLUT
#include "ms3DLUT.h"
#endif
#include "drvGPIO.h"



///////////////////////////////////////////
//#include "drvMcu.h"
#include "drvmStar.h"
//#include "appmStar.h"
//#include "halRwreg.h"
//////////////////////////////////////////
#if ENABLE_DP_INPUT
#include "drvDPRxApp.h"
#endif

// INTERNAL
#include "mstar.h"
#define MSTAR_DEBUG    0
#if ENABLE_DEBUG&&MSTAR_DEBUG
#define MST_printData(str, value)   printData(str, value)
#define MST_printMsg(str)           printMsg(str)
#else
#define MST_printData(str, value)
#define MST_printMsg(str)
#endif
//*******************************************************************
//
// Constant Definition
//
//*******************************************************************
#define ADC2Sets    0//0// 1 for chip with 2 ADC(9x5x & 9x4x)
#define OutDClk1    108
#define OutDClk2    135
#if PanelLVDS
//14.318 * 15 * 8 / 3.5 = 490.9, 490.9<<19=257374477.16,   490.9<<3=8=3927
#define DClkFactor  257374477ul
//#define DClkFactor      257650102// use 215 as base
#else
//14.318 * 15 * 8 / 4 = 429.54, 429.54<<19=225202667.52,   429.54<<3=8=3436.32
#define DClkFactor  225202667ul
//#define DClkFactor      257650102// use 215 as base
#endif
#define FreeRunDClk (DWORD)DClkFactor/PanelDCLK
// 2006/8/25 11:43AM by Emily BYTE InputSclk;  //2006-07-07 Andy

#if ENABLE_HDMI
#define DIFF(a, b)                  (a>b?(a-b):(b-a))
#endif
//*******************************************************************
//
// extern declaration
//
//*******************************************************************
//*******************************************************************
//
// local declaration
//
//*******************************************************************

void mStar_SetupInputPort( void );
//void mStar_SetupFreeRunMode( void );
void mStar_SetupADC( void );
Bool mStar_SetCaptureWindow( void );
//void mStar_SetScalingFactor( void );
//void mStar_SetupPattern( void );
//Bool mStar_SetPanelTiming( void );
void mStar_InitADC( void );
//void mStar_InitTCON( void );
//void mStar_SetScalingFilter( void );
void mStar_SetUserPref( void );
//void mStar_ResetDClkPLL( void );
#if !ENABLE_LED_CONTROLLER
void SetPWMFreq( WORD freq );
#endif


void ScalerReset( void )
{
    SC0_SUB_BANK_SEL(0);//msWriteByte( SC0_00, 0x00 );
    SC0_SCALER_RESET(0);//msWriteByte( SC0_F1, 0x00 );
    mStar_ScalerDoubleBuffer(FALSE);
    // power down contorl 0xF0~0xF1
    SC0_SCALER_RESET(0);//msWriteByte( SC0_F1, 0x00 ); //20081024 RD suggest
    SC0_SCALER_POWER_DOWN(0, 0xFF);//msWriteByte( SC0_F0, 0x00 ); //|BIT6); // power up chip
    SC0_SCALER_RESET(0x7F);//msWriteByte( SC0_F1, 0x7F ); // software reset // DDCCI enable bit6
    ForceDelay1ms( 1 );
    SC0_SCALER_RESET(0);//msWriteByte( SC0_F1, 0x00 ); // software reset
    SC0_NORMAL_MODE();//msWriteByte( SC0_F8, 0x00 ); // ???? important
}
#if CHIP_ID== CHIP_TSUMK
void GPIOPWMSelect( void )
{
    BYTE u8Retry=10;
    if(!u8Retry)
    {
        drvGPIO_uncall();
    }

#if AudioFunc
    msAPIPWMConfig(VOLUME_PWM, VOLUME_PWM_FREQ, VOLUME_PWM_DUTY, VOLUME_PWM_CONFIG, VOLUME_PWM_VS_ALG_NUM ,0);
    msDrvPWMEnableSwitch(VOLUME_PWM, TRUE);
#endif
    msAPIPWMConfig(BACKLIGHT_PWM, BACKLIGHT_PWM_FREQ, BACKLIGHT_PWM_DUTY, BACKLIGHT_PWM_CONFIG, BACKLIGHT_PWM_VS_ALG_NUM ,0);
    msDrvPWMEnableSwitch(BACKLIGHT_PWM, TRUE);
}
#else
void SetPWMFreqAndHVsyncAlign(void)
{
    drvGPIO_SetPWMFreq(BrightnessPWM, BRIGHTNESS_FREQ);
    drvGPIO_PWMAlignVSync(BrightnessPWM, BRIGHTNESS_VSYNC_ALIGN);

#if (CHIP_ID == CHIP_TSUMJ) || (CHIP_ID == CHIP_TSUM9) || (CHIP_ID == CHIP_TSUMF)
    drvGPIO_PWMAlignHSync(BrightnessPWM, BRIGHTNESS_HSYNC_ALIGN);
#endif

#if AudioFunc
    //drvGPIO_SetPWMFreq(VolumePWM, VOLUME_FREQ);
    //drvGPIO_PWMAlignVSync(VolumePWM, VOLUME_VSYNC_ALIGN);
#endif
}
void GPIOPWMSelect( void )
{
    Init_BrightnessPWM();
#if AudioFunc
    Init_VolumePWM();
#endif
    SetPWMFreqAndHVsyncAlign();
}
#endif
#if 1//ENABLE_3DLUT
extern void drvGammaOnOff(BYTE u8Switch, BYTE u8Window);
#endif

void mStar_ACOnInit(void)
{
    // 130415 coding test
#if ENABLE_HDCP
    hw_Set_HdcpHpd();
    hw_Set_HdcpHpd2();
    hw_Set_HdcpHpd3();
#endif
#if ENABLE_MHL
    mapi_mhl_init();
#endif

#if ENABLE_DP_INPUT
    DPRxInit();
#endif
#if 0//(ENABLE_HDMI && ENABLE_CABLE_5V_EDID && ENABLE_FREESYNC)
    drvmStar_EnableHDMIFreeSyncEDID(UserprefFreeSyncMode);
#endif
#if ENABLE_EDP_OUTPUT
	 DPTxInit();
#endif

    mStar_SetupInputPort();
}

//*******************************************************************
// Function Name: mStar_Init(void)
//
// Description: Initialize mStar chip while 1st power on system
//
// Caller: mStar_InitADC(), mStar_InitTCON, mStar_SetupFreeRunMode()
//         mStar_SetupInputPort() in mStar.c
//         msWriteByte(), mStar_WriteWord() in ms_rwreg.c
//         mStar_WriteDDC1(), mStar_WriteDDC2() in ddc.c
//         mStar_InitGamma() in gamma.c
//         Osd_InitOSD() in osd.c
// Callee: Init_Device() in main.c
//*******************************************************************
void mStar_Init( void )
{
    BYTE NOUSE=0;

    if (NOUSE==0x01)
        SetSPI_Quad_En(TRUE);// reverse Qual enable function

#if ( ((CHIP_ID == CHIP_TSUM9)||(CHIP_ID == CHIP_TSUMF)) && !defined(FPGA) )
    //msEread_SelectEfuse(1);
#endif


#if (CHIP_ID == CHIP_TSUMC)||(CHIP_ID==CHIP_TSUMK)||(CHIP_ID == CHIP_TSUMD)||(CHIP_ID == CHIP_TSUMJ)||(CHIP_ID == CHIP_TSUM9)||(CHIP_ID == CHIP_TSUMF)
#if USE_VCTRL
    msWriteByteMask(REG_01A5,BIT1,BIT1);
    ForceDelay1ms(2);
    msWriteByteMask(REG_0183,0x10,0x70);
    ForceDelay1ms(2);
    msWriteByteMask(REG_0183,0x20,0x70);
    ForceDelay1ms(2);
    msWriteByteMask(REG_0183,0x30,0x70);
    ForceDelay1ms(2);
    msWriteByteMask(REG_0183,0x40,0x70);
    ForceDelay1ms(2);
    msWriteByteMask(REG_0183,0x50,0x70);
#elif USE_EXTERNAL_LDO
    msWriteByteMask(REG_01A5,0,BIT1);
    msWriteByteMask(REG_0183,0x50,0x70);
#else
#if CHIP_ID==CHIP_TSUMF
    if (msEread_GetDataFromEfuse(0x07)<228)
    {
        msWriteByteMask(REG_0183,0x70,0x70);
    }
    else
#endif
    {
        msWriteByteMask(REG_0183,0x00,0x70);
    }

#endif

#endif

#if ENABLE_FLASH_CURRENT_ADJUSTMENT
    msWriteByteMask(REG_0213, FLASH_DEFAULT_CURRENT, BIT1|BIT0); // SPI_CK driving current
    msWriteByteMask(REG_0213, FLASH_DEFAULT_CURRENT<<2, BIT3|BIT2); // SPI_DI driving current
    msWriteByteMask(REG_0213, FLASH_DEFAULT_CURRENT<<4, BIT5|BIT4); // SPI_DO driving current
    msWriteByteMask(REG_0213, FLASH_DEFAULT_CURRENT<<6, BIT7|BIT6); // SPI_CZ driving current
#endif

    drvmStar_Init();

    GPIOPWMSelect();

    Power_ModCtrl(_DISABLE); //Jison 110421 disable panel data out

    mStar_InitADC();
#if  ENABLE_LED_CONTROLLER
    msLED_Init();
#endif
    mStar_SetupFreeRunMode();
#if defined(  FPGA)

#else
    #if ENABLE_HDCP
    //msDisableHDCP();
    msInitHDCPProductionKey();
    #endif
#endif
#if ENABLE_HDMI
    mstar_HDMIInitialVariable();
    mstar_HDMIInitialAudio();
#if ENABLE_CEC
    api_CEC_init();
#endif
#endif

#if MS_DAC
    #if ENABLE_DP_AUDIO || ENABLE_HDMI
    msAudio_I2S_SPDIF_Init();
    #endif

    msAudioDAC_Init(); // Audio DAC init
#endif

#if ENABLE_RTE
    msOverDriverInit();
    #ifdef _MULTI_OD_
    msLoad_MultiODTable();
    #else
    msRTE_LoadLUT();
    #endif
    msOverDriveOnOff( FALSE );
#endif

 #if (CHIP_ID == CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK) || (CHIP_ID == CHIP_TSUMD) || (CHIP_ID==CHIP_TSUM9) || (CHIP_ID==CHIP_TSUMF)
 #if ENABLE_DP_INPUT
    if(!CURRENT_INPUT_IS_DISPLAYPORT())
     {
             DPRxIRQEnable(FALSE);
             //DPSetOffLine();
     }
    else
    {
        drvmStar_SetupInputPort_DisplayPort();
    }
#endif
#endif

    Osd_InitOSD();

    iGenTuningFinished = 0;

#if DisplayPowerOnLogo
    if( DisplayLogoFlag )
    {
        Menu_InitAction();
    }
#endif

#if Enable_Gamma
    mStar_SetupGamma( UserPrefGamaMode );
  // mStar_SetupGamma( GAMA3 );
  	UserPrefGamaOnOff=FALSE;
    drvGammaOnOff(UserPrefGamaOnOff, MAIN_WINDOW );
    drvGammaOnOff(UserPrefGamaOnOff, SUB_WINDOW );
#endif
        InitialPeaking();
#if CHIP_ID == CHIP_TSUMV       //111006 Rick modified  - A045
    msAdjustSharpness( MAIN_WINDOW, 0x22, 0 );
#elif ( (CHIP_ID == CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK) || (CHIP_ID == CHIP_TSUMD) ||(CHIP_ID==CHIP_TSUMJ) || (CHIP_ID==CHIP_TSUM9)|| (CHIP_ID==CHIP_TSUMF))
  #if ENABLE_SHARPNESS
    msAdjustSharpness( MAIN_WINDOW, UserPrefSharpness, 0 );
  #endif
#else
    msAdjustSharpness( MAIN_WINDOW, 0x09, 0 );
#endif

    InitACEVar(); // Initialize the variable at ACE.c

    if(NOUSE)
    {
        InitialPeaking();
        msAdjustSharpness( MAIN_WINDOW, 0x10, 0 );
        InitDLCVar();
        msAdjustSubBrightness( MAIN_WINDOW, 0x80, 0x80, 0x80 );
        msAdjustVideoContrast( MAIN_WINDOW, 0x80 );
        msAdjustVideoHue( MAIN_WINDOW, DefHue );
        msAdjustVideoSaturation( MAIN_WINDOW, 0x80 );
    }
#if ENABLE_TNR
    AdjustTNR(1);
#endif

    msWriteByteMask(SC7_D0,BIT6,BIT6);//Main window Y noise-masking dither enable
    msWriteByteMask(SC7_D2,BIT6,BIT6);//Sub window Y noise-masking dither enable

#if ENABLE_3DLUT
    ms3DLutSetHardwareGain(0);
    ms3DLutLoadTbl(0);
    // disable the other color engine
    drvGammaOnOff(_DISABLE, MAIN_WINDOW );
#endif

#if ENABLE_SUPER_RESOLUTION
    mStar_SetupSuperResolution( UserPrefSuperResolutionMode );
#endif
#if ENABLE_TNR
    mStar_SetupNRmode( UserPrefNRmode);
#endif

#if ENABLE_DeBlocking
    msInitDeBlocking( );
#endif
    mStar_SetUserPref();
}

#if UseINT //Enable mode change INT
void mStar_EnableModeChangeINT(Bool u8Enable)
{
    //BYTE u8OrgValue=msReadByte(SC0_CE);
    if (u8Enable)
    {
        //msWriteByte( SC0_CC, 0x00 );
        //msWriteByte( SC0_CD, 0x00 );
        INT_STATUS_CLEAR();

        if (CURRENT_INPUT_IS_VGA())//(SrcInputType == Input_Analog1)
        {
            INT_SCALER_A(0xB0);//msWriteByte(SC0_CE, 0xB0);
            INT_SCALER_B(0x00);//msWriteByte(SC0_CF, 0); // clock change
        }
        else
        {
#if ENABLE_FREESYNC
                INT_SCALER_A(0x28);
#else
                INT_SCALER_A(0x30);//msWriteByte(SC0_CE, 0x30); // for YCbCr
#endif
            	INT_SCALER_B(BIT2);//msWriteByte(SC0_CF, BIT2); // clock change
        }
    }
    else
    {
        INT_SCALER_A(0x00);//msWriteByte(SC0_CE, 0x00);
        INT_SCALER_B(0x00);//msWriteByte(SC0_CF, 0x00);
        //msWriteByte(SC0_CC, 0x00);
        //msWriteByte(SC0_CD, 0x00);
        INT_STATUS_CLEAR();
    }
    //return u8OrgValue;
}
#endif

Bool mStar_SetPanelTiming(void)
{
    if(appmStar_SetPanelTiming() == FALSE)
        return FALSE;

    return TRUE;
}

//*******************************************************************
// Function Name: mStar_SetupMode
//
// Decscription: setup registers for input timing,
// return      : TRUE,
// caller: mStar_SetupADC(), mStar_SetupCaptureWindow(),
//         mStar_SetScalingFactor(), mStar_SetPanelTiming(),
//         mStar_SetUserPref() in mstar.c
//         mStar_FineTuneDVIPhase(), mStar_ValidTimingDetect() in detect.c
//         mSar_WriteByte(), msReadByte() in ms_rwreg.c
// callee: mStar_ModeHandler() in detect.c
//*******************************************************************
Bool mStar_SetupMode( void )
{
#if ENABLE_DISPLAY_UNDERSCAN
    WORD hFreq, vFreq;
#endif

    Clr_FreeRunModeFlag();
    Clr_BackToUnsupportFlag();
    Clr_BackToStandbyFlag();

    mStar_SetupADC(); // setup ADC block, including polarity & htotal, phase, vco
#if ENABLE_DVI_DUAL_LINK //&& DL_PXL_RAT_DET
	if (IsSrcUseDualLinkDviPort())
		msDVIDualLinkMode(msGetDVIDualLinkStatus());
#endif

    if( mStar_SetCaptureWindow() == FALSE )        // setup capture window
    {
#if ENABLE_DEBUG_TIMING_CHANGE_POINT
        SetTimingChangeFirstPoint(TimingChange_CaptureWindow);
#endif
        return FALSE;
    }

#if ENABLE_DISPLAY_UNDERSCAN // 120529 coding modified for just MHL timing Vfreq under 48Hz could into frame buffer
    hFreq = HFreq( SrcHPeriod );
    vFreq = VFreq( hFreq, SrcVTotal );
    if(CURRENT_SOURCE_IS_INTERLACE_MODE())
    {
        vFreq *= 2;
        #if ENABLE_DEBUG
    printMsg("CURRENT_SOURCE_IS_INTERLACE_MODE");
        #endif
    }

    if(vFreq < DISPLAY_UNDERSCAN_FREQ)
    {
        UnderScanSetting.Enable = 1;
        UnderScanSetting.HSize = DISPLAY_UNDERSCAN_SPACE;
        UnderScanSetting.VSize = DISPLAY_UNDERSCAN_SPACE;
    #if 0//ENABLE_RTE
        msOverDriveOnOff( FALSE );
    #endif
    }
    else
    {
        UnderScanSetting.Enable = 0;
        UnderScanSetting.HSize = 0;
        UnderScanSetting.VSize = 0;
    }
#endif

#if (CHIP_ID == CHIP_TSUMU || CHIP_ID >= CHIP_TSUM2 )
    mStar_SetupPath();
#else
    mStar_SetScalingFactor(); // setup scaling factor
    mStar_SetScalingFilter();
#endif


#if 0//!ENABLE_HDMI        //110926 Rick add for enter in unsupport mode while interlace input
    if(CURRENT_SOURCE_IS_INTERLACE_MODE())
    {
        #if DEBUG_PRINT_ENABLE
        printMsg( "Interlace --> not support" );
        #endif
        SrcFlags |= bUnsupportMode;
        return TRUE;
    }
    else
#endif

    if( mStar_SetPanelTiming() == FALSE )        // set output dclk
    {
        #if DEBUG_PRINT_ENABLE
        printMsg( "Dot clock --> not support" );
        #endif
        SrcFlags |= bUnsupportMode;
        return TRUE;
    }

    if( InputTimingChangeFlag || mStar_ValidTimingDetect() )        // check if input timing has changed
    {
#if ENABLE_DEBUG_TIMING_CHANGE_POINT
        SetTimingChangeFirstPoint(TimingChange_SetupMode);
#endif
        return FALSE;
    }

    // enable double buffer
    mStar_ScalerDoubleBuffer(TRUE);

    // use interrupt to speedup mode changing while input timing is changing
#if UseINT
    mStar_EnableModeChangeINT(TRUE);
#endif

    mStar_InterlaceModeSetting();

    #if ENABLE_RTE
        msOverDriveOnOff( UserprefOverDriveSwitch );
    #endif

#if ENABLE_TNR
    if( TNROnFlag )
        msTNROnOff( TRUE );
    else
        msTNROnOff( FALSE );
#endif

#if BrightFreqByVfreq
    SetFourtimesPWMFreq(); // due to pwm period is not fixed, needs to set freq before period
#endif

    mStar_SetUserPref(); // restore user setting // 2006/10/16 12:38AM by Emily
    SetInputColorFormat();
    return TRUE;
}

//*******************************************************************
// Function Name: mStar_SetupFreeRunMode
//
// Decscription: setup registers for free run mode without any input timing,
//
// caller: mStar_ResetDClkPLL() in mstar.c
//         mSar_WriteByte(), msReadByte(), mStar_WriteWord() in ms_rwreg.c
// callee: mStar_ModeHandler() in detect.c
//*******************************************************************
void mStar_SetupFreeRunMode( void )
{
    drvmStar_SetupFreeRunMode();
    Set_FreeRunModeFlag();

#if BrightFreqByVfreq
#if ENABLE_LED_CONTROLLER     //111216 modified LED blacklit dimming PWMfreq setting
    #if LED_CTRL_BRIGHTNESS_BY_CURRENT
        SetDimmingPWMFreq( BRIGHTNESS_VSYNC_ALIGN, PWM_DIMMING_FREQ );
    #else
        SetDimmingPWMFreq( BRIGHTNESS_VSYNC_ALIGN, 240 );
    #endif
#else
    //SetPWM4Vfreq();
    SetPWMFreq( 240 );
#endif
#endif

#if ENABLE_FACTORY_SSCADJ // 2011-09-16 19:46 CC
    mStar_SetPanelSSC(FactorySetting.SSCModulation, FactorySetting.SSCPercentage);
#else
    mStar_SetPanelSSC(PANEL_SSC_MODULATION_DEF, PANEL_SSC_PERCENTAGE_DEF);
#endif

}
//*******************************************************************
// Function Name: mStar_SetAnalogInputPort
//
// Decscription: setup registers for Separate Sync/Composite Sync/SOG,
//
// caller: mSar_WriteByte(), msReadByte() in ms_rwreg.c
//
// callee: mStar_MonitorInputTiming() in detect.c
//*******************************************************************
void mStar_SetAnalogInputPort( Bool ToSOGPort )
{
    BYTE regValue = SC0_READ_INPUT_SETTING();//msReadByte(SC0_02);

#if MS_VGA_SOG_EN
   if(!ToSOGPort)
    {
        regValue = regValue & 0x8F;
        g_bInputSOGFlag=0;
       if(bInputVGAisYUV) //20150121
        {
            //printMsg(">>SetAnaPort>> ADC_TABLE_SOURCE_YUV_HV");
            drvADC_SetADCSource(ADC_TABLE_SOURCE_YUV_HV);  // input is YUV
        }
        else
        {
            //printMsg(">>SetAnaPort>> ADC_TABLE_SOURCE_RGB");
            drvADC_SetADCSource(ADC_TABLE_SOURCE_RGB);   // input is RGB
        }
        SC0_ADC_COAST_ENABLE(0x01);//msWriteByte(SC0_ED, 0x01);   // enable ADC coast
        SC0_ADC_COAST_START(0x03);//msWriteByte(SC0_EE, 0x03); //0x00);   // enable coast window start
        SC0_ADC_COAST_END(0x01);//msWriteByte(SC0_EF, 0x01); //0x00);   // enable coast window end
        SC0_GLITCH_REMOVAL_ENABLE(0);//msWriteByte(SC0_F3, 0x00 ); //RD suggest 20081008
        ADC_PLL_LOCKING_EDGE(0);//msWriteByteMask(REG_ADC_DTOP_07_L,0,BIT5); // 0:Hsync leading edge; 1: Hsync trailing edge
#if ENABLE_DEBUG
        printMsg("InputPort_____VGA");
#endif
    }
    else
    {
        regValue = regValue | 0x70;
        g_bInputSOGFlag=1;

        if(bInputVGAisYUV) //20150121
        {
            //printMsg(">>SetAnaPort>> ADC_TABLE_SOURCE_YUV");
            drvADC_SetADCSource(ADC_TABLE_SOURCE_YUV);  // input is YUV
        }
        else
        {
            //printMsg(">>SetAnaPort>> ADC_TABLE_SOURCE_SOG");
            drvADC_SetADCSource(ADC_TABLE_SOURCE_SOG);   // input is RGB
        }
        SC0_ADC_COAST_ENABLE(0x21);//msWriteByte(SC0_ED, 0x21);   // enable ADC coast
        SC0_ADC_COAST_START(0x0A);//msWriteByte(SC0_EE, 0x08);   // enable coast window start  //Al050814 Micro version need adjust this value
        SC0_ADC_COAST_END(0x0A);//msWriteByte(SC0_EF, 0x08 ); //0x05);   // enable coast window end //Jison 110317 follow CHIP_TSUMT
        SC0_GLITCH_REMOVAL_ENABLE(0);//msWriteByte( SC0_F3,  0x00 );
        ADC_PLL_LOCKING_EDGE(1);//msWriteByteMask(REG_ADC_DTOP_07_L,BIT5,BIT5); // 0:Hsync leading edge; 1: Hsync trailing edge
#if ENABLE_DEBUG
        printMsg("InputPort_____SOG");
#endif
    }
#else
    ToSOGPort=ToSOGPort;
    regValue = regValue & 0x8F;
    g_bInputSOGFlag = 0;
    drvADC_SetADCSource(ADC_TABLE_SOURCE_RGB);
#endif

    SC0_RECOVER_INPUT_SETTING(regValue);//msWriteByte(SC0_02, regValue);

    SC0_SCALER_RESET(GPR_B|ADCR_B); // enable software reset function to clear ADC & Graphic port RO register
    Delay1ms(2);
    SC0_SCALER_RESET(0); // disable software reset
    Delay1ms(80); // delay over 1 frame time to wait for status register is ready

}


//*******************************************************************
// Function Name: mStar_SetupInputPort
//
// Decscription: setup input port registers for
//               Analog/Digital/YCbCr(Video) input
//
// caller: mSar_WriteByte(), msReadByte() in ms_rwreg.c
// callee: mStar_MonitorInputTiming() in detect.c
//*******************************************************************
void mStar_SetupInputPort( void )
{
#if ENABLE_DEBUG
    printData("SetupInputPort : %d", SrcInputType);
#endif
    g_CountSwitchPortTimeFlag=TRUE;

#if CHIP_ID == CHIP_TSUMU
    if(!g_SetupPathInfo.bOverrideSCFmtIn)
        g_SetupPathInfo.ucSCFmtIn = SC_FMT_IN_NORMAL;
    g_SetupPathInfo.ucIPLRSel = IP_3D_LR_FRAME_DET;
#elif CHIP_ID == CHIP_TSUM2
    if(!g_SetupPathInfo.bOverrideSCFmtIn)
        g_SetupPathInfo.ucSCFmtIn = SC_FMT_IN_NORMAL;
#endif

    Set_ShowInputInfoFlag();

#if ENABLE_MHL
    MHLExtenCountFlag = 0;
#endif

    mStar_IPPowerControl();

#ifdef _HW_AUTO_NO_SIGNAL_
    msWriteByteMask(SC0_E7, _BIT4, _BIT4);
#endif

#if ((CHIP_ID == CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK) || (CHIP_ID == CHIP_TSUMD)||(CHIP_ID == CHIP_TSUM9)||(CHIP_ID == CHIP_TSUMF))
    if(!INPUT_IS_TMDS())
    {
         msWriteByteMask(REG_175D, 0x20, 0x20);//Mask HDMI IRQ
#if DHDMI_SIMPLAYHD_PATCH // SimplayHD CTS 8-18
         msWriteByteMask(REG_1509, 0x84, 0x84);
#endif
    }
    else
    {
        msWriteByteMask(REG_175D, 0x00, 0x20);
    }
#if ENABLE_DP_INPUT
    if(!CURRENT_INPUT_IS_DISPLAYPORT())
     {
             #if(CHIP_ID == CHIP_TSUM9||CHIP_ID == CHIP_TSUMF)
          DPRxOutputEnable( FALSE );
             #endif
            DPClearOffLineData();
            DPRxIRQEnable(FALSE);
            DPSetOffLine();
     }
#endif
#endif

    if(CURRENT_INPUT_IS_VGA())//( SrcInputType < Input_Digital )
    {
        drvmStar_SetupInputPort_VGA();

#if (CHIP_ID==CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID==CHIP_TSUMD || CHIP_ID==CHIP_TSUM9|| CHIP_ID==CHIP_TSUMF)
        ComboInputControl(COMBO_INPUT_ANALOG);
#endif
    }
    else if(CURRENT_INPUT_IS_DVI())//( SrcInputType == Input_Digital )
    {
        drvmStar_SetupInputPort_DVI();
#if ENABLE_MHL && (CHIP_ID == CHIP_TSUM2)
        mapi_mhl_CbusIsolate();
#endif
#if (CHIP_ID==CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID==CHIP_TSUMD || CHIP_ID==CHIP_TSUM9|| CHIP_ID==CHIP_TSUMF)
        ComboInputControl(COMBO_INPUT_DIGITAL);
#endif
    }
#if ENABLE_HDMI
    else if(CURRENT_INPUT_IS_HDMI())//(SrcInputType == Input_HDMI)
    {
        mstar_HDMIInitialVariable();
        drvmStar_SetupInputPort_HDMI();
#if (CHIP_ID==CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID==CHIP_TSUMD || CHIP_ID==CHIP_TSUM9|| CHIP_ID==CHIP_TSUMF)
        ComboInputControl(COMBO_INPUT_DIGITAL);
#endif
    }
#endif
#if ENABLE_DP_INPUT
    else if(CURRENT_INPUT_IS_DISPLAYPORT())//(SrcInputType == Input_Displayport)
    {
        drvmStar_SetupInputPort_DisplayPort();
#if (CHIP_ID==CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID==CHIP_TSUMD || CHIP_ID==CHIP_TSUM9|| CHIP_ID==CHIP_TSUMF)
        ComboInputControl(COMBO_INPUT_DIGITAL);
#endif
    }
#endif

#if ENABLE_MHL
#if ((CHIP_ID == CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK) ||(CHIP_ID == CHIP_TSUMD)||(CHIP_ID==CHIP_TSUMJ)||(CHIP_ID == CHIP_TSUM9)||(CHIP_ID == CHIP_TSUMF))
    mapi_mhl_SourceChange();
#endif
#endif

    ForceDelay1ms(10);//(20);
    SC0_SCALER_RESET(GPR_B|ADCR_B); // enable software reset function to clear ADC & Graphic port RO register
    ForceDelay1ms(20);//(80);
    SC0_SCALER_RESET(0); // disable software reset
#if ENABLE_HDCP
#if ENABLE_MHL
    if(!CURRENT_INPUT_MHL_CABLE_PLUGGED())
#endif
    if(CURRENT_INPUT_IS_TMDS())//( SrcInputType == Input_Digital || SrcInputType == Input_Digital2 )
        msEnableHDCP();
#endif

#if ShowSourceType
if (!FactoryModeFlag)
	 Set_DisplayInputMsgFlag();
#endif

}

#if Enable_Expansion
void IsSupportedAspRatioMode(WORD u16width, WORD u16height)
{
    WORD xdata M, N;
    WORD xdata u16OutputImgH, u16OutputImgV;

    //Frame buffer less mode
    if (CURRENT_IS_FBL())
    {   //4:3 & 16:9
        {
            M = 4;
            N = 3;
            if( ((DWORD)PanelWidth*N/M) < PanelHeight )
            {
                u16OutputImgH = PanelWidth;
                u16OutputImgV = ((DWORD)PanelWidth*N/M);
            }
            else
            {
                u16OutputImgH = ((DWORD)PanelHeight*M/N);
                u16OutputImgV = PanelHeight;
            }
            if(abs(u16OutputImgV-PanelHeight)>= 5)
            {
                Clr_Asp4_3EnableFlag();
                MST_printData(">>>>>>>--------------------Clr_Asp4_3EnableFlag==%x \r\n",0);
            }
            else
            {
                Set_Asp4_3EnableFlag();
                MST_printData(">>>>>>>--------------------Set_Asp4_3EnableFlag==%x \r\n",0);
            }
            M = 16;
            N = 9;
            if( ((DWORD)PanelWidth*N/M) < PanelHeight )
            {
                u16OutputImgH = PanelWidth;
                u16OutputImgV = ((DWORD)PanelWidth*N/M);
            }
            else
            {
                u16OutputImgH = ((DWORD)PanelHeight*M/N);
                u16OutputImgV = PanelHeight;
            }
            if(abs(u16OutputImgV-PanelHeight)>= 5)
            {
                Clr_Asp16_9EnableFlag();
                MST_printData(">>>>>>>--------------------Clr_Asp16_9EnableFlag==%x \r\n",0);

            }
            else
            {
                Set_Asp16_9EnableFlag();
                MST_printData(">>>>>>>--------------------Set_Asp16_9EnableFlag==%x \r\n",0);
            }
        }
        //1:1
        {
            u16OutputImgH = u16width;
            u16OutputImgV = u16height;
            if(abs(u16OutputImgV-PanelHeight)>= 5)
            {
                Clr_Asp1_1EnableFlag();
                MST_printData(">>>>>>>--------------------Clr_Asp1_1EnableFlag==%x \r\n",0);

            }
            else
            {
                Set_Asp1_1EnableFlag();
                MST_printData(">>>>>>>--------------------Set_Asp1_1EnableFlag==%x \r\n",0);
            }
        }
        //Fixed input ratio
        {
            if( (DWORD)PanelWidth*u16height >= (DWORD)PanelHeight*u16width )
            {
                u16OutputImgH = ((DWORD)u16width * PanelHeight / u16height);
                u16OutputImgV = PanelHeight;
            }
            else
            {
                u16OutputImgH = PanelWidth;
                u16OutputImgV = ((DWORD)u16height * PanelWidth / u16width);
            }
            if(abs(u16OutputImgV-PanelHeight)>= 5)
            {
                Clr_AspFixedInputRatioEnableFlag();
                MST_printData(">>>>>>>--------------------Clr_AspFixedInputRatioEnableFlag==%x \r\n",0);

            }
            else
            {
                Set_AspFixedInputRatioEnableFlag();
                MST_printData(">>>>>>>--------------------Set_AspFixedInputRatioEnableFlag==%x \r\n",0);
            }
        }
    }
    else
    {
        OverScanSetting.AspRatioEnableFlag=0x0F;
    };
    //MST_printData(">>>>>>>--------------------width==%x \r\n",u16width);
    //MST_printData(">>>>>>>--------------------height==%x \r\n",u16height);
    //MST_printData(">>>>>>>--------------------u16OutputImgH = %x \r\n", u16OutputImgH);
    //MST_printData(">>>>>>>--------------------u16OutputImgV = %x \r\n", u16OutputImgV);
}
#endif
void OverScanCheck(WORD CapWinHStart ,WORD CapWinVStart,WORD width, WORD height)
{
    WORD xdata OverScanHDisp, OverScanVDisp, OverScanHCap, OverScanVCap;
    WORD xdata M, N;

    if( (OverScanSetting.AspRatio == OVERSCAN_4_3)
     || (OverScanSetting.AspRatio == OVERSCAN_16_9) )
    {
        if(OverScanSetting.AspRatio == OVERSCAN_4_3)
        {
            M = 4;
            N = 3;
        }
        else if(OverScanSetting.AspRatio == OVERSCAN_16_9)
        {
            M = 16;
            N = 9;
        }
        if( ((DWORD)PanelWidth*N/M) < PanelHeight )
        {
            OverScanSetting.OverScanH = PanelWidth;
            OverScanSetting.OverScanV = ((DWORD)PanelWidth*N/M);
        }
        else
        {
            OverScanSetting.OverScanH = ((DWORD)PanelHeight*M/N);
            OverScanSetting.OverScanV = PanelHeight;
        }
        OverScanHDisp = OverScanSetting.OverScanH + OverScanSetting.ExtH;
        OverScanVDisp = OverScanSetting.OverScanV + OverScanSetting.ExtV;
        if( OverScanSetting.ExtH )
        {
            OverScanHCap = ((DWORD)width * OverScanSetting.OverScanH / OverScanHDisp);
            CapWinHStart = UserPrefHStart + (width - OverScanHCap) / 2 / PANEL_H_DIV;
            width = OverScanHCap;
        }

        if( OverScanSetting.ExtV )
        {
            OverScanVCap = ((DWORD)height * OverScanSetting.OverScanV / OverScanVDisp);
            CapWinVStart = UserPrefVStart + (height - OverScanVCap) / 2;
            height = OverScanVCap;
        }
    }
    else if( OverScanSetting.AspRatio == OVERSCAN_1_1 )
    {
        OverScanSetting.OverScanH = width;
        OverScanSetting.OverScanV = height;
    }
    else if(OverScanSetting.AspRatio == OVERSCAN_FIXED) //Fixed Input Aspect Ratio
    {
        if( (DWORD)PanelWidth*height >= (DWORD)PanelHeight*width )
        {
            OverScanSetting.OverScanH = ((DWORD)width * PanelHeight / height);
            OverScanSetting.OverScanV = PanelHeight;
        }
        else
        {
            OverScanSetting.OverScanH = PanelWidth;
            OverScanSetting.OverScanV = ((DWORD)height * PanelWidth / width);
        }
    }
    else if( OverScanSetting.AspRatio == OVERSCAN_FIXED_RATIO )
    {
        OverScanSetting.OverScanH = (PanelWidth > width)?( width + ((DWORD)(PanelWidth - width) * OverScanSetting.ImageRatio) / 100 ):PanelWidth ;
        OverScanSetting.OverScanV = (PanelHeight > height)?( height + ((DWORD)(PanelHeight - height) * OverScanSetting.ImageRatio) / 100 ):PanelHeight ;
        OverScanHCap = ((DWORD)width * OverScanSetting.ScanRatio / 100);
        CapWinHStart = UserPrefHStart + (width - OverScanHCap) / 2 / PANEL_H_DIV;
        width = OverScanHCap;

        OverScanVCap = ((DWORD)height * OverScanSetting.ScanRatio / 100);
        CapWinVStart = UserPrefVStart + (height - OverScanVCap) / 2;
        height = OverScanVCap;
    }
    else if( OverScanSetting.AspRatio == OVERSCAN_FULL )
    {
        OverScanSetting.OverScanV=PanelHeight;
        OverScanSetting.OverScanH=PanelWidth;
    }

    //if output image V size is smaller than panelHight with FBL mode , set output image size to panel size.
    if (CURRENT_IS_FBL())
    {
        if(abs(OverScanSetting.OverScanV-PanelHeight)>= 5)
        {
            OverScanSetting.OverScanH = PanelWidth;
            OverScanSetting.OverScanV = PanelHeight;
        }
    }
    width &= (~BIT0);
    OverScanSetting.FinalCapWinHST=CapWinHStart;
    OverScanSetting.FinalCapWinVST=CapWinVStart;
    OverScanSetting.FinalCapWinWidth=width;
    OverScanSetting.FinalCapWinHeight=height;

    MST_printData("--------------------CapWinHStart = %x", CapWinHStart);
    MST_printData("--------------------CapWinVStart = %x", CapWinVStart);
    MST_printData("--------------------width = %x", width);
    MST_printData("--------------------height = %x", height);
}
//*******************************************************************
// Function Name: mStar_SetCaptureWindow
//
// Decscription: setup input capture window for display
//
// caller: mSar_WriteWord(), mStar_ReadWord() in ms_rwreg.c
///
// callee: mStar_SetupMode() in mstar.c
//*******************************************************************
Bool mStar_SetCaptureWindow( void )
{
    WORD xdata width, height;
    MST_printMsg(">>>>>>>>mStar_SetCaptureWindow >>>>>>>>.");
    if(
        CURRENT_INPUT_IS_TMDS()//SrcInputType == Input_Digital || SrcInputType == Input_Digital2
#if ENABLE_DP_INPUT
        || CURRENT_INPUT_IS_DISPLAYPORT()//SrcInputType == Input_Displayport
#endif
    )
    {
#if ENABLE_CHECK_TMDS_DE
        if( CURRENT_INPUT_IS_TMDS() )
        {
            UserPrefHStart = SrcTMDSHStart; // get hsync DE start
            UserPrefVStart = SrcTMDSVStart; // get vsync DE start
        }
        else
        {
            UserPrefHStart = SC0_READ_AUTO_START_H();//msRead2Byte(SC0_80); // get hsync DE start
            UserPrefVStart = SC0_READ_AUTO_START_V();//msRead2Byte(SC0_7E); // get vsync DE start
        }
#else
        UserPrefHStart = SC0_READ_AUTO_START_H();//msRead2Byte(SC0_80); // get hsync DE start
        UserPrefVStart = SC0_READ_AUTO_START_V();//msRead2Byte(SC0_7E); // get vsync DE start
#endif

        width = GetImageWidth(); // get DE width
        height = GetImageHeight(); // get DE Height

         if(CURRENT_SOURCE_IS_INTERLACE_MODE())
            {
                if(SC0_DE_ONLY_MODE() == TRUE)//(scReadByte(SC0_04)&BIT6) //DE only mode
                {
                    if(SC0_VIDEO_FIELD_INVERSION() == FALSE)//((scReadByte(SC0_E9)&BIT3)==0) //video field invert
                        height++;  //height=(Vend ¡V Vstart + 2);
                    else
                        height-=3; //height=(Vend ¡V Vstart - 2);
                }
                else
                {
                    height--; //height=(Vend ¡V Vstart)
                }

            }
    }
    else
    {
        // input 1360x768 display on 1366 panel,
        // don't do sacle and get 1366 for hori. data
        if( PanelWidth == 1366 )
        {
            if( StandardModeGroup == Res_1360x768 )
                width = 1366;
            else
                width = StandardModeWidth;
        }
        else
        {
            width = StandardModeWidth;
        }


        height = SC0_READ_AUTO_HEIGHT();//msRead2Byte(SC0_82)-msRead2Byte(SC0_7E)+1; // get DE Height

        if( StandardModeGroup == Res_1152x864 )
        {
            height = 864;
        }
        else if((( StandardModeGroup == Res_720x480 ) && ( abs( StandardModeVFreq - 599 ) < 10 ) && ( abs( StandardModeHFreq - 157 ) < 10 ) )
                || (( StandardModeGroup == Res_720x576 ) && ( abs( StandardModeVFreq - 500 ) < 10 ) && ( abs( StandardModeHFreq - 156 ) < 10 ) )
                || (( StandardModeGroup == Res_1920x1080 ) && ( abs( StandardModeVFreq - 600 ) < 10 ) && ( abs( StandardModeHFreq - 337 ) < 10 ) )
                || (( StandardModeGroup == Res_1920x1080 ) && ( abs( StandardModeVFreq - 500 ) < 10 ) && ( abs( StandardModeHFreq - 281 ) < 10 ) ) ) // for VGA InterlaceMode
        {
            height = StandardModeHeight + 4;
        }
        else
        {
            height = StandardModeHeight;
        }
    }
#if DECREASE_V_SCALING
    //if(!OverScanFlag)
        height += DecVScaleValue;
#endif

    OverScanCheck(UserPrefHStart ,UserPrefVStart,width, height);

    if( width == PanelWidth && height == PanelHeight ) // set native mode flag
        SrcFlags |= bNativeMode;

    mStar_AdjustHPosition(OverScanSetting.FinalCapWinHST);
    mStar_AdjustVPosition(OverScanSetting.FinalCapWinVST);

    SC0_SET_IMAGE_HEIGHT(OverScanSetting.FinalCapWinHeight);
    SC0_SET_IMAGE_WIDTH(OverScanSetting.FinalCapWinWidth);

    return TRUE;
}
//*******************************************************************
// Function Name: mStar_SetupClampingByMode
//
// Decscription: When input timing is reduced blanking, we have to reduce the
//   duration of ADC calibration to avoid dark display.
//    The amount of calibration duration must be smaller than back-porch.
//    Currently, we set the calibration duration to H_Back_Porch/2.
// caller: mSar_WriteByte(), msReadByte() in ms_rwreg.c
///
// callee: mStar_SetupMode() in mstar.c
//*******************************************************************
void mStar_SetupClampingByMode(void)
{


    if (CURRENT_INPUT_IS_VGA())//(SrcInputType< Input_Digital)
    {
        if(g_bInputSOGFlag)  //Steven101221
        {
            switch(SrcModeIndex)
            {
                case MD_720x480I_60_YCbCr:
          case MD_720x480I_60_YCbCr_1:
                {
                    drvADC_AdjustCalDuring(15);  //Back-porch=57 pixel
                    break;
                }
                #if 0  //Steven110608, disable, Update TSUMT_ADC_Driver table 0.8 to fix some timing clamping issue.
                case MD_720X576I_50:
                {
                    drvADC_AdjustCalDuring(31);  //Back-porch=69 pixel
                    break;
                }
                case MD_720X480P_60:
                {
                    drvADC_AdjustCalDuring(60/2); //Back-porch=60 pixel
                    break;
                }
                case MD_640X480P_60:
                {
                    drvADC_AdjustCalDuring(48/2); //Back-porch=48 pixel
                    break;
                }
                case MD_720X576P_50:
                {
                    drvADC_AdjustCalDuring(31); //Back-porch=68 pixel
                    break;
                }
                #endif
                //================
                case MD_640x480_60_VESA:
                {
                    drvADC_AdjustCalDuring(7); //Back-porch=25 pixel
                    break;
                }
                case MD_640x350_70_IBM:
                {
                    drvADC_AdjustCalDuring(7); //Back-porch=48 pixel
                    break;
                }
                case MD_720x400_70:
                {
                    drvADC_AdjustCalDuring(15); //Back-porch=48 pixel
                    break;
                }
                #if 0  //Steven110608, disable, Update TSUMT_ADC_Driver table 0.8 to fix some timing clamping issue.
                case MD_800X600_72:
                {
                    drvADC_AdjustCalDuring(31); //Back-porch=64 pixel
                    break;
                }
                #endif
                case MD_1280x768R_60_CVT:
                {
                    drvADC_AdjustCalDuring(31); //Back-porch=80 pixel
                    break;
                }
                case MD_1280x800R_60_CVT:
                {
                    drvADC_AdjustCalDuring(31); //Back-porch=80 pixel
                    break;
                }
                case MD_1440x900R_60_CVT:
                {
                    drvADC_AdjustCalDuring(31); //Back-porch=80 pixel
                    break;
                }
        #if PANEL_WIDTH==1600&&PANEL_HEIGHT==900
                case MD_1600X900R_60_VESA:
                {
                    drvADC_AdjustCalDuring(31); //Back-porch=80 pixel
                    break;
                }
              #endif
                case MD_1680x1050R_60_CVT:
                {
                    drvADC_AdjustCalDuring(31); //Back-porch=80 pixel
                    break;
                }
                case MD_1920x1200R_60_CVT:
                {
                    drvADC_AdjustCalDuring(31); //Back-porch=80 pixel
                    break;
                }

        #if  0
         case MD_1920X1200_50R:
                {
                    drvADC_AdjustCalDuring(31); //Back-porch=80 pixel
                    break;
                }
          case MD_1600X1000_60R:
                {
                    drvADC_AdjustCalDuring(31); //Back-porch=80 pixel
                    break;
                }
                case MD_2048X1152_60R:
                {
                    drvADC_AdjustCalDuring(31); //Back-porch=80 pixel
                    break;
                }
                 case MD_1920X1080_60R:
                {
                    drvADC_AdjustCalDuring(31); //Back-porch=80 pixel
                    break;
                }
        #endif

            }
        }
    }
}
//*******************************************************************
// Function Name: mStar_SetupADC
//
// Decscription: setup ADC bandwidth/filter, clock, phase for sampling input data
//               and R/G/B gains, offsets
// caller: mSar_WriteByte(), msReadByte() in ms_rwreg.c
///
// callee: mStar_SetupMode() in mstar.c
//*******************************************************************
void mStar_SetupADC( void )
{
    WORD tempValue;

    if(!CURRENT_INPUT_IS_VGA())//(SrcInputType > Input_Analog1)
        return;

    tempValue = HFreq( SrcHPeriod );//(( DWORD )MST_CLOCK_MHZ * 10 + SrcHPeriod / 2 ) / SrcHPeriod; //calculate hfreq: round 5
    tempValue = (( DWORD )tempValue * UserPrefHTotal + 5000 ) / 10000; //dclk= hfreq * htotal

#if DADCPLLPROTECTION
    drvADC_SetModewithPLLProtection(g_bInputSOGFlag ? ADC_INPUTSOURCE_YPBPR : ADC_INPUTSOURCE_RGB, tempValue, UserPrefHTotal);
#else
    drvADC_AdjustHTotal(UserPrefHTotal);

    drvADC_SetADCModeSetting((g_bInputSOGFlag ? ADC_INPUTSOURCE_YPBPR : ADC_INPUTSOURCE_RGB), tempValue);
#endif
    //drvADC_SetPhaseCode((UPPHASE_GET_REAL_VALUE()+1)%MAX_PHASE_VALUE); //Jison 100818 patch for phase state machine reset
    drvADC_SetPhaseCode(UserPrefPhase); //Jison 100818
    drvADC_SetupHsyncPolarity((SrcFlags & bHSyncNegative)); //Jison 100818

    // wait for checking
    drvADC_SetRGBGainCode(UserPrefAdcRedGain, UserPrefAdcGreenGain, UserPrefAdcBlueGain);
    drvADC_SetRGBOffsetCode(UserPrefAdcRedOffset, UserPrefAdcGreenOffset, UserPrefAdcBlueOffset);
    //drvADC_AdjustCalDuring(7);

    mStar_SetupClampingByMode(); //Jison 110106
    //msADC_AdjustCalDuring( 80/2 );  // unit: pixel // coding temp setting
}

#if CHIP_ID == CHIP_TSUM2
void PowerDownDDR(void)
{
        //DDR DTOP
     msWrite2Byte(REG_1207, 0x3250);
     msWrite2Byte(REG_1247, 0xfffe);
     ForceDelay1ms(2);
     msWrite2Byte(REG_1219, 0x0400);
     msWrite2Byte(REG_1201, 0x002f);
     msWrite2Byte(REG_1201, 0x052e);
     msWrite2Byte(REG_1201, 0x002e);
     msWrite2Byte(REG_1201, 0x032e);
     msWrite2Byte(REG_1201, 0x002e);
     ForceDelay1ms(2);
     msWrite2Byte(REG_1247, 0xffff);
     msWrite2Byte(REG_1201, 0x202e);
}
#endif

//*******************************************************************
// Function Name: mStar_PowerUp
//
// Decscription: Power on chip from power down mode
//
// callee: msWriteByte() in ms_rwreg.c
//
// caller: Power_PowerOnSystem() in power.c
//*******************************************************************
void mStar_PowerUp( void )
{
#if ENABLE_MHL_CTS_TEST || ENABLE_DP_CTS_TEST
    BYTE ctstest = 1;
    if(ctstest)
        return;
#endif

#if !(MS_PM)
#if ENABLE_MHL
    if(!GET_MHL_CABLE_PLUGGED())
#endif
    {
        MPLL_POWER_UP(TRUE);
        LPLL_POWER_UP(TRUE);
        MPLL_CLOCK_ADC(TRUE);
        mcuSetSystemSpeed(SPEED_NORMAL_MODE);
    }
#endif

#if ENABLE_MHL
    mapi_mhl_PowerCtrl(MHL_POWER_ON);
#endif

    SC0_SCALER_POWER_DOWN(BIT6, 0xFF); // power on chip from power down mode

    SC0_SCALER_RESET( GPR_B | ADCR_B ); // reset graphic port RO register
    SC0_SCALER_RESET( 0 );


 #if ENABLE_DP_INPUT
#if (CHIP_ID == CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID == CHIP_TSUMD||CHIP_ID == CHIP_TSUM9||CHIP_ID == CHIP_TSUMF)
     DPRxInit_PM();
#endif
     DPRxFastTrainingInitial();
 #endif

#if MS_DAC // wait for coding
    msAudioDACPowerDown( FALSE );
#endif

#if ENABLE_LED_CONTROLLER
    msLED_BoostEnable(_ENABLE);
#endif

    // 120229 coding check tsum2
    WRITE_POWER_ON_TABLE();

    mStar_IPPowerControl();

#if ENABLE_DEBUG
    printMsg("PowerUp");
#endif

}

//*******************************************************************
// Function Name: mStar_PowerDown
//
// Decscription: Power down chip
//
// callee: msWriteByte() in ms_rwreg.c
//
// caller: Power_PowerOffSystem() in power.c
//*******************************************************************
void mStar_PowerDown( void )
{
#if ENABLE_MHL_CTS_TEST || ENABLE_DP_CTS_TEST
    BYTE ctstest = 1;
    if(ctstest)
        return;
#endif

#if (ENABLE_DP_INPUT) && ((CHIP_ID == CHIP_TSUMC)||(CHIP_ID==CHIP_TSUMK)||(CHIP_ID == CHIP_TSUMD)||(CHIP_ID == CHIP_TSUM9)||(CHIP_ID == CHIP_TSUMF))
     #if (CHIP_ID == CHIP_TSUM9||CHIP_ID == CHIP_TSUMF)
     #if !DP_AUX_NORMAL_FRO
     DPRxSetAuxClock(FRO);
     #endif
     #endif
     DPAux_Reset();
     //DPRxSetAuxClock(FRO);
     DPSetOffLine();
     DPRxIRQEnable(FALSE);
     msWriteByteMask( REG_2B19, ( _BIT7 ), _BIT7 );         // Disable DPISR
#endif

#if ENABLE_RTE // wait for coding
    msOverDriveOnOff( FALSE); // disable OD function , or you can set BK4_90h= 0 directly.
#endif

#if ENABLE_TNR
    msTNROnOff( FALSE );
#endif

#if AudioFunc
    mStar_AdjustVolume( 0 );
#endif

#if MS_DAC // wait for coding
#if DEPOP_DCONOFF_FLOW
if (u8PowerDownCount==0)
#endif
{
    msAudioDACPowerDown( TRUE );

}
	
#endif
    drvADC_PowerCtrl((PowerOnFlag ? ADC_POWER_STANDBY : ADC_POWER_DOWN));
    drvDVI_PowerCtrl((PowerOnFlag ? DVI_POWER_STANDBY : DVI_POWER_DOWN));

  #if UseINT
    mStar_EnableModeChangeINT(FALSE); //Jison: for DDCCI,DVI input, reduced power off,it wake up immediately.
  #endif

#if ENABLE_LED_CONTROLLER
    msLED_BoostEnable(_DISABLE);
#endif

#if (MS_PM)
    SC0_SCALER_POWER_DOWN( PDDS_B | BIT5 | BIT2 | 0x2 | BIT6, 0xFF ); // power down chip except mode detection
#else
    SC0_SCALER_POWER_DOWN( PDDS_B | BIT5 | BIT2 | 0x3 | BIT6, 0xFF ); // power down chip except mode detection
    //Jison, Don't enable if use PM mode, it will cause sometime DDCCI_ID is disabled by int 0
    //if (DDCCI_FORCE_POWER_SAVE_FLAG && (SRC_INPUT_TYPE==INPUT_DVI || SRC_INPUT_TYPE==INPUT_HDMI))
    //msWriteByte(SC0_CF, BIT2); //Enable DVI clock change int 090710 // 110928 coding check with Jison
#endif

#if ENABLE_MHL
    mapi_mhl_PowerCtrl(PowerOnFlag? MHL_POWER_STANDBY : MHL_POWER_DOWN);
#endif

#if CHIP_ID == CHIP_TSUM2
    PowerDownDDR();
#endif

#if !(MS_PM)
#if ENABLE_MHL
    if(!GET_MHL_CABLE_PLUGGED())
#endif
    {
        mcuSetSystemSpeed(SPEED_XTAL_MODE);     //111012 Modify
        MPLL_POWER_UP(FALSE);
        LPLL_POWER_UP(FALSE);
        MPLL_CLOCK_ADC(FALSE);
    }
#endif
 

#if BRIGHTNESS_INVERSE	 //+Duty power
			 drvGPIO_SetBacklightDuty(BrightnessPWM, 0xFF);
#else							 //-Duty power
			 drvGPIO_SetBacklightDuty(BrightnessPWM, 0);
#endif
#if !Enable_LED
	hw_ClrGreenLed();// xhq power key	off  led
#endif

#if ENABLE_DEBUG
printMsg("PowerDown");
#endif

//----------------------------------------------------
}
//*******************************************************************
// Function Name: mStar_InitADC
//
// Decscription: Initialize ADC bank registers
//
// callee: msWriteByte() in ms_rwreg.c
//
// caller: mStar_Init() in mStar.c
//*******************************************************************
void mStar_InitADC( void )
{

    drvADC_init(FALSE);
    drvADC_ADCOffsetGainMismatchCal();
#ifdef _DGAIN_CAL_WITHOUT_INPUT_
    drvADC_ADCAutoGainCalwithoutInput();
#endif
#if ENABLE_ADC_DITHERING // 120106 coding addition
    msWriteByteMask(REG_ADC_ATOP_51_L, 0x00, 0x03); // [1:0]: enable ADCA interleve H/V dither in the normal display
#else
    msWriteByteMask(REG_ADC_ATOP_51_L, 0x03, 0x03); // [1:0]: disable ADCA interleve H/V dither in the normal display
#endif
}


//==================================================================
extern BYTE code t_MWEDLC_Linear_Table[];
//extern BYTE code t_Normal_ColorSettingTable[];
extern code short t_Normal_ColorSettingTable[][3];

//*******************************************************************
// Function Name: mStar_SetUserPref
//
// Decscription: Restore user setting from NVRAM
//
// callee: msWriteByte() in ms_rwreg.c
//
// caller: mStar_SetupMode() in mStar.c
//*******************************************************************
extern BYTE mStar_ChangeAdjContrastValue(BYTE contrast);

void mStar_SetUserPref( void )
{
BYTE uccontrast;
    if( UserPrefDcrMode )
    {

        UserPrefECOMode = ECO_Standard;
        ReadColorTempSetting();
        /*
        msWriteByte( BK0_00, REGBANK3 );
        msWriteByte( BK3_72, 0x70 );
        msWriteByte( BK3_73, 0x70 );
        msWriteByte( BK3_74, 0x70 );
        msWriteByte( BK0_00, 0x00 );
        */
        //LoadACETable( t_Normal_ColorSettingTable, DefHue, DefSaturation, 0x80 ); // /UserPrefSubContrast
        //LoadACETable( MAIN_WINDOW, (BYTE *)t_Normal_ColorSettingTable, DefHue, DefSaturation, 0x80 ); // /UserPrefSubContrast

        msAccSetup( 0, PanelWidth, 0, PanelHeight );
        msDlcInit( PanelWidth, PanelHeight );
        msSetDlcStrength( g_LowStrength, g_HighStrength );
        LoadDLCTable( t_MWEDLC_Linear_Table );
        //mStar_WriteByteMask( BK0_5C, BIT3, BIT3 ); //msWriteByte( BK0_5C, msReadByte( BK0_5C ) | BIT3 );
        msAccOnOff( _DISABLE );     //111021 Rick check - B39434

        msDlcOnOff( _ENABLE );
    }
#if DDCCI_ENABLE
    mStar_AdjustUserPrefBlacklevel( UserprefRedBlackLevel, UserprefGreenBlackLevel, UserprefBlueBlackLevel );
#endif
	uccontrast = mStar_ChangeAdjContrastValue(UserPrefContrast);

    LoadACETable( MAIN_WINDOW, (BYTE *)t_Normal_ColorSettingTable, DefHue, DefSaturation, uccontrast); // /UserPrefSubContrast
    LoadACETable( SUB_WINDOW, (BYTE *)t_Normal_ColorSettingTable, DefHue, DefSaturation, uccontrast); // /UserPrefSubContrast
    msDCROnOff(UserPrefDcrMode, MAIN_WINDOW);
    SetECO(); //zhifeng.wu

#if ENABLE_R2_2DTO3D
    if (UserPref2Dto3D)
        msWriteByteMask(SC0_CE, BIT2, BIT2);    //enable vsync int
    if (UserPref2Dto3DGameMode)
        msWrite2Byte(REG_2102, 0x1111); //enable t3d game mode
#endif
}

#if !ENABLE_LED_CONTROLLER
void SetPWMFreq(WORD freq)
{
    drvGPIO_SetPWMFreq(BrightnessPWM, freq);
}
#endif
#if BrightFreqByVfreq
void SetFourtimesPWMFreq( void )
{
    WORD outVfreq;

#if CHIP_ID == CHIP_TSUMU || CHIP_ID == CHIP_TSUM2 || CHIP_ID == CHIP_TSUMD
    if(g_SetupPathInfo.bFBMode)
    {
        outVfreq = (WORD)GetVfreq()*4;
    }
    else
#endif
    {
        outVfreq = V_FREQ_IN*4;
    }

#if ENABLE_LED_CONTROLLER
    #if LED_CTRL_BRIGHTNESS_BY_CURRENT
        SetDimmingPWMFreq( BRIGHTNESS_VSYNC_ALIGN, PWM_DIMMING_FREQ );
    #else
        SetDimmingPWMFreq( BRIGHTNESS_VSYNC_ALIGN, outVfreq );
    #endif
#else
    SetPWMFreq( outVfreq );
#endif

#if ENABLE_DEBUG
    printData("  BrightFreqByVfreq:%d", outVfreq);
#endif
}
#endif

void mStar_BlackWhiteScreenCtrl(BYTE u8Ctrl)
{
    if (u8Ctrl==BW_SCREEN_WHITE)
        SC0_WHITE_SCREEN_ENABLE();//msWriteByteMask(SC0_43,BIT5,(BIT4|BIT5));
    else if (u8Ctrl==BW_SCREEN_BLACK)
        SC0_BLACK_SCREEN_ENABLE();//msWriteByteMask(SC0_43,BIT4,(BIT4|BIT5));
    else
        SC0_BLACK_WHITE_SCREEN_DISABLE();//msWriteByteMask(SC0_43,0,(BIT4|BIT5));
}

BYTE IsColorspaceRGBInput()
{
    BYTE  ColorspaceRGB=TRUE;

#if  ENABLE_DP_INPUT
    //if(CURRENT_INPUT_IS_DISPLAYPORT()&&(( gDPInfo.ucDPColorFormat  )!=InputColor_RGB))
    if(CURRENT_INPUT_IS_DISPLAYPORT()&&DPINPUT_COLORSPACEYUV())
        ColorspaceRGB=FALSE;
#endif

#if ENABLE_HDMI
    //if(SrcInputType==Input_HDMI&&(gScInfo.InputColor!=InputColor_RGB))
#if PANEL_3D_PASSIVE_4M
    if(CURRENT_INPUT_IS_HDMI())//(SrcInputType==Input_Digital)
#else
    if(CURRENT_INPUT_IS_TMDS() && (gScInfo.InputColor!=InputColor_RGB))
#endif
        ColorspaceRGB=FALSE;
#endif

#if CHIP_ID==CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID==CHIP_TSUMD
    msWriteByteMask(SCB_A0, (g_SetupPathInfo.bMRW422==1 || ColorspaceRGB==FALSE)?(BIT0):(0), BIT0);
#elif CHIP_ID==CHIP_TSUMJ||CHIP_ID==CHIP_TSUM9||CHIP_ID==CHIP_TSUMF
    msWriteByteMask(SCB_A0, (ColorspaceRGB==FALSE)?(BIT0):(0), BIT0);
#endif
    return ColorspaceRGB;
}

BYTE IsColorspaceRGB(void)
{
    BYTE ColorspaceRGB = TRUE;

    ColorspaceRGB = IsColorspaceRGBInput();

#if ENABLE_TNR
    if( TNROnFlag )
        ColorspaceRGB=FALSE;
#endif

    return  ColorspaceRGB;
}

//#if ENABLE_HDMI
void  SettingInputColorimetry(void)
{

#if  ENABLE_DP_INPUT
    if(CURRENT_INPUT_IS_DISPLAYPORT()&&DPINPUT_COLORIMETRY_ITU709())
    msACESetHDTVMode(YUV_Colorimetry_ITU709);
    else
#endif
#if ENABLE_HDMI
    if(CURRENT_INPUT_IS_TMDS() && (gScInfo.AVI_Colorimetry!=YUV_Colorimetry_ITU601))
    msACESetHDTVMode(YUV_Colorimetry_ITU709);
    else
#endif
    msACESetHDTVMode(YUV_Colorimetry_ITU601);
}
//#endif
BYTE code mStarNullData[] = {0};
void mStarDummy(void)
{
    BYTE xdata i = mStarNullData[0];
}


#if ((CHIP_ID==CHIP_TSUM9 ||CHIP_ID==CHIP_TSUMF)&& !defined(FPGA))
void msTrimFROFromEfuse(void)
{
    BYTE uctemp;

    uctemp = msEread_GetDataFromEfuse(0x0004);

    if((uctemp & BIT7) == BIT7)  // Trimming flag
    {
        msWriteByteMask(REG_018A, uctemp, 0x7F);
        g_bFROTrimResult = TRUE;

    }
    else
    {
        g_bFROTrimResult = FALSE;

    }
}

#if (ENABLE_XTAL_LESS)
//*******************************************************
//  RCOSC calibration counter.
//  If HIGH_RESOLUTION_EN = 0
//  >> TARGET frequency = REF_freq * COUNT_REPORT/ 512
//         => COUNT_REPORT = Target * 512  / REF_Freg = (OSC432M/16) * 512 / FRO12M  = 0x480
//  If HIGH_RESOLUTION_EN = 1
//  >> TARGET frequency = REF_freq * COUNT_REPORT/ 2048
//         => COUNT_REPORT = Target * 2048  / REF_Freg = (OSC432M/32) * 2048 / FRO12M = 0x900
//*******************************************************
Bool msStartRCOSC432MCal(void)
{
    #define WRITE_CAL_VALUE(A)    (msWriteByteMask(REG_1ECB, A,0xFF))
    #define RCOSC_HIGH_RESOLUTION_EN    0//BIT3
    #define RCOSC_COUNTER_MASK          0xFFF
    #if RCOSC_HIGH_RESOLUTION_EN
    #if CHIP_ID==CHIP_TSUMF
    #define OSC432_TARGET               0x900   //0x7C0 for 372M, 0x900 for 432M
    #else
    #define OSC432_TARGET               0x7C0   //0x7C0 for 372M, 0x900 for 432M
    #endif
    #else
    #if CHIP_ID==CHIP_TSUMF
    #define OSC432_TARGET               0x480   //0x3E0 for 372M, 0x480 for 432M
    #else
    #define OSC432_TARGET               0x3E0   //0x3E0 for 372M, 0x480 for 432M
    #endif
    #endif
    WORD ucCounter;
    BYTE i=0;

    msWriteByte(REG_1ECA,0x20);//0x1E65[7:0] = 0010_0000
    msWriteByteMask(REG_01B2, 0, BIT5|BIT6|BIT7);//0x0159[7:5] = 000 (default)
    msWriteBit(REG_1EEB, TRUE, BIT6);//0x1E75[14] = 1 //Enable test bus output.
    msWriteByteMask(REG_1EE8, 0x13, 0x1F);//0x1E74[4:0] = 10011 // Select TEST_CLK_OUT source.
    msWriteByteMask(REG_1EEB, 0x01, 0x1F);//0x1E75[12:8] = 00001 //ckg_tstclk
    #if RCOSC_HIGH_RESOLUTION_EN
    msWriteByteMask(REG_1EEC,0x05,0x07);//0x1E76[2:0] = 101
    #else
    msWriteByteMask(REG_1EEC,0x04,0x07);//0x1E76[2:0] = 100
    #endif
    Delay1ms(99);
    do
    {
        WRITE_CAL_VALUE(i++);
        Delay1ms(1);
        msWriteByte(REG_3A80, 0x80|RCOSC_HIGH_RESOLUTION_EN);//osc soft reset
        msWriteByte(REG_3A80, 0x03|RCOSC_HIGH_RESOLUTION_EN);// calculate & counter one time mode enable
        while( !( msReadByte(REG_3A83) & BIT4 ) );
        ucCounter = msRead2Byte(REG_3A82)&RCOSC_COUNTER_MASK;
    }
    while(( ucCounter < OSC432_TARGET ) &&  (i < 0xF0 ));
    msWriteBit(REG_1EEB, FALSE, BIT6); //close test bus
    return (BOOL)(ucCounter >= OSC432_TARGET);

    #undef WRITE_CAL_VALUE
    #undef RCOSC_HIGH_RESOLUTION_EN
    #undef RCOSC_COUNTER_MASK
    #undef OSC432_TARGET

}
#endif
#endif

WORD GetHSyncWidth(void)
{
    WORD u16HsyncWidth;
    WORD u16AutoHDEWidth = msRead2Byte(SC0_84)-msRead2Byte(SC0_80)+1;
    volatile BYTE u8BP_SC0_7A = msReadByte(SC0_7A);
    DWORD u32sclk;
    WORD u16hPeriod;
	
    if(CURRENT_INPUT_IS_VGA())
    {
	    if( SrcHPeriod > 511 )        // prevent counter overflow when input frequency is very low
	    {
	        u16hPeriod = SrcHPeriod * 16;
	    }
	    else
	    {
	        #if UseINT && Enable_Expansion
	        mStar_EnableModeChangeINT(FALSE);
	        #endif
	        u32sclk = GetVSyncTime() * 8; 
	        SC0_HPEROID_DETECT_MODE(TRUE);
	        Delay1ms( u32sclk );
	        u16hPeriod = SC0_READ_HPEROID();
	        SC0_HPEROID_DETECT_MODE(FALSE);
	        Delay1ms( u32sclk );
	        if( abs( u16hPeriod - SrcHPeriod * 16 ) > 80 )            
	        {
	            u16hPeriod = SrcHPeriod * 16;
	        }
	        #if UseINT && Enable_Expansion
	        mStar_EnableModeChangeINT(TRUE);//restore
	        #endif		
	    }
           u32sclk = (( DWORD )UserPrefHTotal * MST_HPeriod_UINT ) / (u16hPeriod );
	}

    msWriteByteMask(SC0_7A ,BIT3 ,BIT3);
    u16HsyncWidth = (msReadByte(SC0_EB)&0x0F)<<8|msReadByte(SC0_EA);
    msWriteByte(SC0_7A, u8BP_SC0_7A);


    if(CURRENT_INPUT_IS_VGA())
    {
	u16HsyncWidth=(((float)u16HsyncWidth*u32sclk+XTAL_CLOCK_KHZ/2)/XTAL_CLOCK_KHZ); //return clock count, DVI by idclk domain, ADC by xtal clk domain
    }
    u16HsyncWidth = (u16HsyncWidth>u16AutoHDEWidth)?(mSTar_GetInputHTotal()-u16HsyncWidth):(u16HsyncWidth);
    return u16HsyncWidth;  //return clock count, DVI by idclk domain, ADC by xtal clk domain
}

void SetInputColorFormat( void )
{
	BYTE uccontrast;
    if(g_bInputSOGFlag)
    {
        if(UserPrefInputColorFormat == INPUTCOLOR_YUV) //20150121
        {
            drvADC_SetADCSource(ADC_TABLE_SOURCE_YUV);  // input is YUV
        }
        else
        {
            drvADC_SetADCSource(ADC_TABLE_SOURCE_SOG);   // input is RGB
        }
    }
    else
    {
        if(UserPrefInputColorFormat == INPUTCOLOR_YUV) //20150121
        {
            drvADC_SetADCSource(ADC_TABLE_SOURCE_YUV_HV);  // input is YUV
        }
        else
        {
            drvADC_SetADCSource(ADC_TABLE_SOURCE_RGB);   // input is RGB
        }
    }

    if(IsColorspaceRGB()
#if ENABLE_DISPLAY_UNDERSCAN||CHIP_ID == CHIP_TSUMD
        && !g_SetupPathInfo.bMRWR2Y
#endif
        && (UserPrefInputColorFormat == INPUTCOLOR_RGB) //20150121
    )
    {
        mStar_AdjustContrast(UserPrefContrast);
    }
    else
    {
    	uccontrast = mStar_ChangeAdjContrastValue(UserPrefContrast);
        msAdjustVideoContrast(MAIN_WINDOW,uccontrast);
    }
}

