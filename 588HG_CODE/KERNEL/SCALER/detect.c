#include <math.h>
#include "types.h"
#include "board.h"
#include "global.h"
#include "ms_reg.h"
#include "ms_rwreg.h"
#include "power.h"
#include "debug.h"
#include "Common.h"
#include "misc.h"
#include "menu.h"
#include "UserPref.h"
#include "msOSD.h"
#include "mStar.h"
#include "MenuFunc.h"
//#include "msregTV.h"
//#include "Panel.h"
#include "adjust.h"
#include "mode.h"
#include "gpio_def.h"
#include "drvosd.h"
#include "drvadc.h"
#include "autofunc.h"
#include "detect.h"
#ifdef UseInternalDDCRam
#include "ddc.h"
#endif

#include "MsHDCP.h"
#include "MsACE.h"
#include "msdlc.h"
 #if ENABLE_DP_INPUT
#include "drvDPRxApp.h"
#endif

#if ENABLE_HDMI
#include "MsHDMI.h"
#endif

#if MS_DAC
#include "drvdac.h"
#endif


/////////////////////////////////////
#include "drvmStar.h"
//#include "halRwreg.h"
////////////////////////////////////


#define DETECT_DEBUG    0
#if ENABLE_DEBUG&&DETECT_DEBUG
    #define DETECT_printData(str, value)   printData(str, value)
    #define DETECT_printMsg(str)           printMsg(str)
#else
    #define DETECT_printData(str, value)
    #define DETECT_printMsg(str)
#endif

#define DEBUG_HDMI_MHL_CABLE_DETECT     0
#if ENABLE_DEBUG && DEBUG_HDMI_MHL_CABLE_DETECT
    #define DEBUG_HDMI_MHL_CABLE(str, value)   printData(str, value)
#else
    #define DEBUG_HDMI_MHL_CABLE(str, value)
#endif
////////////////////////////////////////////////////////////////////////
#define SyncValidCount_VGA     6//10  ²å°ÎVGAÏìÓ¦ËÙ¶È
#define SyncLossCount_VGA      20
#define SyncValidCount_DVI      20
#define SyncLossCount_DVI       60
#define SyncValidCount_HDMI   50
#define SyncLossCount_HDMI    250
#define SyncValidCount_DP           50
#define SyncLossCount_DP            200

 #if ENABLE_HDMI
#define HDMI_STAY_1S_COUNT      50 // unit: 20mS, 50x20=1000mS
#define HDMI_SPEEDUP_SWITCH_PORT()    (HDMICableNotConnect() && DVI_CLOCK_IS_MISSING() && (InputTimingStableCounter >= HDMI_STAY_1S_COUNT))
#endif

#if ENABLE_MHL
#define SyncLossExtenCount_MHL      ((SyncLossStableCount<=250)?(1):(SyncLossStableCount-250))  // unit: 20mS

#define MHL_EXTEND_SYNCLOSS_COUNT()   (SyncLossState() \
                                                                        && InputTimingStableCounter >= SyncLossStableCount \
                                                                        && !MHLExtenCountFlag\
                                                                        && CURRENT_INPUT_MHL_CABLE_PLUGGED() \
                                                                        && GET_MHL_CBUS_CONNECT())
#endif

////////////////////////////////////////////////////////////////////////

#if INPUT_TYPE == INPUT_1A1H
#define SyncValidCount_Digital  SyncValidCount_HDMI
#define SyncLossCount_Digital  SyncLossCount_HDMI
#elif INPUT_TYPE == INPUT_1A2D || INPUT_TYPE == INPUT_2A2D || INPUT_TYPE == INPUT_1A1D1H
#define SyncValidCount_Digital  ((CURRENT_INPUT_IS_HDMI())?(SyncValidCount_HDMI):(SyncValidCount_DVI))
#define SyncLossCount_Digital  ((CURRENT_INPUT_IS_HDMI())?(SyncLossCount_HDMI):(SyncLossCount_DVI))
#elif (INPUT_TYPE ==INPUT_1A1H1DP ||INPUT_TYPE == INPUT_1A1D1H1DP)
#define SyncValidCount_Digital  ((CURRENT_INPUT_IS_DISPLAYPORT())?(SyncValidCount_DP):(SyncValidCount_HDMI))
#define SyncLossCount_Digital  ((CURRENT_INPUT_IS_DISPLAYPORT())?(SyncLossCount_DP):(SyncLossCount_HDMI))
#elif INPUT_TYPE == INPUT_1A1D1DP
#define SyncValidCount_Digital  ((CURRENT_INPUT_IS_DISPLAYPORT())?(SyncValidCount_DP):(SyncValidCount_DVI))
#define SyncLossCount_Digital  ((CURRENT_INPUT_IS_DISPLAYPORT())?(SyncLossCount_DP):(SyncLossCount_DVI))
#elif INPUT_TYPE ==INPUT_1A2H
#define SyncValidCount_Digital  SyncValidCount_HDMI
#define SyncLossCount_Digital  SyncLossCount_HDMI
#elif INPUT_TYPE == INPUT_1A2C || INPUT_TYPE == INPUT_1A3C || INPUT_TYPE == INPUT_1A1C2
#define SyncValidCount_Digital  ((CURRENT_INPUT_IS_DISPLAYPORT())?(SyncValidCount_DP):((CURRENT_INPUT_IS_HDMI())?(SyncValidCount_HDMI):(SyncValidCount_DVI)))
#define SyncLossCount_Digital  ((CURRENT_INPUT_IS_DISPLAYPORT())?(SyncLossCount_DP):((CURRENT_INPUT_IS_HDMI())?(SyncLossCount_HDMI):(SyncLossCount_DVI)))
#else/// INPUT_1A1D
#define SyncValidCount_Digital  SyncValidCount_DVI
#define SyncLossCount_Digital  SyncLossCount_DVI
#endif

#define ValidTimingStableCount  ((CURRENT_INPUT_IS_VGA())?(SyncValidCount_VGA):(SyncValidCount_Digital))
#define SyncLossStableCount     ((CURRENT_INPUT_IS_VGA())?(SyncLossCount_VGA):(SyncLossCount_Digital))


#define TMDS_TOLERANCE     2

//#define HFreq_Torlance     12 // 15 // 1.5 KHz
//#define VFreq_Torlance     12 // 15 // 1.5 Hz


//OTS_UU215DGLW spec 100812

#if  ENABLE_DP_INPUT|| ENABLE_HDMI
#define MaxInputHFreq      ((CURRENT_INPUT_IS_DISPLAYPORT()||CURRENT_INPUT_IS_HDMI())? 1000:900)// 800  //FOR 3D
#define MinInputHFreq       ((CURRENT_INPUT_IS_DISPLAYPORT()||CURRENT_INPUT_IS_HDMI())? 100:100)  //for Dp interlace timing  //plz  HY conf spec
#else
#define MaxInputHFreq      900///800
#define MinInputHFreq       100//300 100831 code test
#endif
#define MaxInputVFreq      800//750

#define MinInputVFreq       230 // MHL/DP/HDMI

extern InputModeType code StandardMode[];
extern BYTE xdata InputTimingStableCounter;

#if Audio_Switch
extern Bool SetAudioType(AudioType Type);
#endif
extern Bool mStar_FindMode( void );
BYTE mStar_GetInputStatus( void );
Bool IsCableNotConnected( void );
Bool mStar_SyncLossStateDetect( void );
Bool mStar_ValidTimingDetect( void );
void SpeedupOrExtendSwitchPort(void);
#if ENABLE_FREESYNC
Bool xdata bHDMIFreesyncChk = FALSE;
#endif
#if ENABLE_HDMI
Bool HDMICableNotConnect(void);
#endif
#if ENABLE_MHL
    #if(CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID == CHIP_TSUMD || CHIP_ID == CHIP_TSUM9 || CHIP_ID == CHIP_TSUMF)
Bool CurrentInputMHLCablePlugged(void);
    #endif
#endif

//*******************************************************************
// Function Name: mStar_MonitorInputTiming
//
// Decscription: Detect input timing
//
// callee: mStar_SyncLossStateDetect(), mStar_ValidTimingDetect(), mStar_AutoDVI() in detect.c
//         mStar_SetInputPort(), mStar_SetAnalogInputPort() in mstar.c
//
// caller: main() in main.c
//*******************************************************************
void mStar_MonitorInputTiming( void )
{
    if(!PowerOnFlag)
    {
        return ;
    }

    // detect whether input signal is changing
    if( !InputTimingChangeFlag )
    {
        if( SyncLossState() )
        {

            if( !FreeRunModeFlag )
            {
                Set_InputTimingChangeFlag();
            }
            else if( mStar_SyncLossStateDetect() )                // detect mode while no Sync/suspend/standby/disconnect
            {
                Set_InputTimingChangeFlag();
            }
#if !DISABLE_AUTO_SWITCH
            else if( InputTimingStableCounter && ( SwitchPortCntr > Input_Nums ) && (UserPrefInputPriorityType==Input_Priority_Auto))
            {
                SpeedupOrExtendSwitchPort();

                if( InputTimingStableCounter % SyncLossStableCount == 0 )
                {
                    InputTimingStableCounter = 1; // 071224
                    SrcInputType = ( SrcInputType + 1 ) % Input_Nums;
                    mStar_SetupInputPort();
                    DETECT_printData(" PS_SwitchTo:%d", SrcInputType);
                }
            }
#endif
        }
        else            // valid timing
        {
            // check if input timing is changing
            if( mStar_ValidTimingDetect() )
            {
                SrcFlags |= SyncLoss;
                Set_InputTimingChangeFlag();
              //  if( !DisplayLogoFlag )
                //    hw_ClrBlacklit(); // 2006/9/7 8:39PM by Emily for changed mode had garbage

#if ENABLE_DP_INPUT
#if DP_RESETHTT_PATCH
            if(CURRENT_INPUT_IS_DISPLAYPORT())
                DPMSA_Reset();
#endif
#endif
            }
        }
    }
    //=============================================================================
    //=============================================================================
    if( InputTimingChangeFlag )        // input timing is unstable
    {
        if( g_bAccOnOff )
        {
            msAccOnOff( 0 );
        }
        // 2006/7/6 0:8AM by Emily for dec. dot clock
    #if DECREASE_V_SCALING
        DecVScaleValue = 0;
    #endif
#if AudioFunc //111027 Rick modified for avoid bo sound while timingchange - B39440
#if ENABLE_DP_AUDIO
    if((CURRENT_INPUT_IS_DISPLAYPORT())&&(UserPrefAudioSource == AUDIO_DIGITAL))
        hw_SetMute();
#endif
#endif
        Clr_InputTimingChangeFlag();
        Clr_DoModeSettingFlag();
        Clr_ReadyForChangeFlag();

#if ENABLE_DEBUG_TIMING_CHANGE_POINT
        if(TimingChangeFirstPoint)
           printData(" TimingChangeFirstPoint:%d", TimingChangeFirstPoint);
#endif
#if ENABLE_FREESYNC
        bHDMIFreesyncChk = TRUE;
        //g_bHDMIFreesyncEnable = FALSE;
#endif
        mStar_PrepareForTimingChange();
    }
    else        // input timing is stable
    {
        InputTimingStableCounter++; // counter timing stable times

        if( InputTimingStableCounter == 0 )
            InputTimingStableCounter = 1;

        if( !InputTimingStableFlag )
        {
            SpeedupOrExtendSwitchPort();

            if(( !SyncLossState() && InputTimingStableCounter >= ValidTimingStableCount ) ||
            ( SyncLossState() && InputTimingStableCounter >= SyncLossStableCount )||
            (  BurninModeFlag && SyncLossState() && InputTimingStableCounter >= 1 ) )
            {
                Set_InputTimingStableFlag(); // set flag to search for input mode
                Set_DoModeSettingFlag();

#if !DISABLE_AUTO_SWITCH
                g_CountSwitchPortTimeFlag=FALSE;
                InputTimingStableCounter = 1;

                if( !SyncLossState() )
                {
                    SwitchPortCntr = 0;
#if ENABLE_MHL
                    MHLExtenCountFlag = 0;
#endif

#if ENABLE_DEBUG_TIMING_CHANGE_POINT
                    ResetTimingChangeFirstPoint();
#endif
                }

                if( SyncLossState() && ( ++SwitchPortCntr <= Input_Nums ) && (UserPrefInputPriorityType==Input_Priority_Auto))
                {
                    #if 0//ENABLE_DEBUG
                    if(CURRENT_INPUT_IS_DVI())
                    {
                        msWriteByteMask(REG_0280, BIT4, BIT4);
                        while(msReadByte(REG_0280)&BIT4);
                    }
                    #endif
                    Clr_InputTimingStableFlag(); // set flag to search for input mode
                    Clr_DoModeSettingFlag();
                    SrcInputType = ( SrcInputType + 1 ) % Input_Nums;
                    mStar_SetupInputPort();
                }
#endif

            }


        }
    }
}
//*******************************************************************
// Function Name: mStar_ModeHandler
//
// Decscription: Programming scaler while input timing is changing
//
// callee: mStar_SyncLossStateDetect(), mStar_ValidTimingDetect(), mStar_AutoDVI() in detect.c
//         mStar_SetInputPort(), mStar_SetAnalogInputPort() in mstar.c
//
// caller: main() in main.c
//*******************************************************************
void mStar_CheckFailSafeMode( WORD Checkhfreq, WORD Checkvreq )
{
    if (( Checkhfreq > MaxInputHFreq || Checkhfreq < MinInputHFreq || Checkvreq > MaxInputVFreq || Checkvreq < MinInputVFreq )
        /*||(StandardModeWidth >= PanelWidth && StandardModeHeight > PanelHeight)
        ||(StandardModeWidth > PanelWidth && StandardModeHeight >= PanelHeight)*/)
    {
        SrcFlags |= bUnsupportMode;
    }
}
void mStar_ModeHandler( void )
{
#if ((CHIP_ID == CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK) || (CHIP_ID == CHIP_TSUMD)|| CHIP_ID==CHIP_TSUM9|| CHIP_ID == CHIP_TSUMF)
#if ENABLE_AUTOEQ
    if (!(_bit2_(msRegs[REG_0C01]))) // not MHL
    {
        extern Bool xdata AUTOEQ_FLAG;
        extern void msTMDS_AUTOEQ_TRIG(void);
        if(AUTOEQ_FLAG==1 && !(_bit4_(msRegs[REG_155D])))
            drv_TMDS_AUTOEQ_TRIG();
    }
#endif
#endif

    if( DisplayLogoFlag )
    {
        Menu_InitAction();
    }
    if( !DoModeSettingFlag )
        // while timing change & stable
    {
        return ;
    }

#if MS_DAC
    #if ((CHIP_ID == CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK) || (CHIP_ID == CHIP_TSUMD) || (CHIP_ID == CHIP_TSUM9)|| (CHIP_ID == CHIP_TSUMF))

    if(!SyncLossState() && w16DAC_DePopStep != INIT_FINISH)
        return;

    #endif
#endif

    //Clr_DoModeSettingFlag();  //0606 for Diaplay LOGO
    if( !SyncLossState() )
        // no sync
    {
#if ENABLE_HDMI
        if(CURRENT_INPUT_IS_TMDS())
        {
            if( mstar_HDMITmdsGetType() == TMDS_HDMI )
            {
                BYTE ucCount = 200; //25;  2009-10-13
                gScInfo.OutputDataState = HDMI_DATA_OUTPUT_ON;
                while( ucCount-- )
                {
                    gScInfo.InputAVIInfoReceived = 0;
                    mstar_HDMIGetPktStatus();
                    if( gScInfo.InputAVIInfoReceived )
                    {
                        mstar_HDMICheckAudio();
                        gScInfo.InputColor = mstar_HDMIPacketColor();
                        if(gScInfo.InputColor == INPUT_RGB)
                        {
                            InputColorFormat = INPUTCOLOR_RGB;
                            UserPrefInputColorFormat = INPUTCOLOR_RGB;
                        }
                        else
                        {
                            InputColorFormat = INPUTCOLOR_YUV;
                            UserPrefInputColorFormat = INPUTCOLOR_YUV;
                        }
                        gScInfo.ColorChanged = 1;
                        mstar_HDMIHandleColor();
                        break;
                    }
                    Delay1ms( 20 );
                }
#if ENABLE_HDMI_1_4
                msHDMI_Check_3D_Mode_Chg(0);
#endif
                mstar_HDMICheckAudio();
                gScInfo.InputTmdsType = TMDS_HDMI;
                SC0_INPUT_SELECT(ISEL_HDMI_444); //msWriteByteMask(SC0_02, SCALER_ISEL_HDMI, BIT2|BIT1|BIT0);
            }
            else if(mstar_HDMITmdsGetType()==TMDS_DVI)
            {
                gScInfo.InputTmdsType = TMDS_DVI;
                InputColorFormat = INPUTCOLOR_RGB;
                UserPrefInputColorFormat = INPUTCOLOR_RGB;

                SC0_INPUT_SELECT(ISEL_DVI); //msWriteByteMask(SC0_02, SCALER_ISEL_DVI, BIT2|BIT1|BIT0);
            }
        }
        //-------------------------------------------
#endif
		// vga xhq
			if(CURRENT_INPUT_IS_VGA())
			{
			
				InputColorFormat = INPUTCOLOR_RGB;
				UserPrefInputColorFormat = INPUTCOLOR_RGB;
				SetInputColorFormat();
			  }

        SettingInputColorimetry();
        if( !ReadyForChangeFlag )
        {
            if( !mStar_FindMode() )                // search mode index
            {
#if ENABLE_DEBUG_TIMING_CHANGE_POINT
                SetTimingChangeFirstPoint(TimingChange_FindMode);
#endif
                Set_InputTimingChangeFlag(); // search mode faile
                return ;
            }
        }

        if( !UnsupportedModeFlag )
            //supported mode
        {
            if( !ReadyForChangeFlag )
            {
                ReadModeSetting(); // restore user's settings from NVRAM for each mode
                Set_ReadyForChangeFlag();
            }

            if( DisplayLogoFlag )
            {
                return ;
            }

            if( mStar_SetupMode() == FALSE )                // setup scaler
            {
                Set_InputTimingChangeFlag(); // setup mode failed
                mStar_SetupFreeRunMode(); // setup freerun mode
                return ;
            }

        }

    }
    Set_ReadyForChangeFlag();

    if( DisplayLogoFlag )
    {
        return ;
    }

    if( SyncLossState() || UnsupportedModeFlag )
    {
        mStar_SetupFreeRunMode(); // setup freerun mode
#if Enable_LED
		Power_TurnOnAmberLed();//ºìµÆ
		printMsg("555555");
#else
		Power_TurnOnGreenLed();
#endif
    }
    else
    {
        WORD vfreq, hfreq;
#if ENABLE_FREESYNC
    if(IS_DP_FREESYNC())
    {
        WORD u16PixClk, u16Htt;

        u16PixClk = msAPI_combo_IPGetPixelClk();
        u16Htt = msAPI_combo_IPGetGetHTotal();
        if((u16Htt!=0)&&(SrcVTotal!=0))
        {
            hfreq = ((DWORD)u16PixClk * 100 + u16Htt/2)/u16Htt;
            vfreq = ((DWORD)hfreq * 1000 + SrcVTotal/2)/SrcVTotal;
        }
    }
    else
#endif
    {
            hfreq = HFreq( SrcHPeriod );
            vfreq = VFreq( hfreq, SrcVTotal );
    }
        if( CURRENT_SOURCE_IS_INTERLACE_MODE() )
            vfreq *= 2;

        //new OOR Spec.
        mStar_CheckFailSafeMode(hfreq, vfreq);    // 110919 Rick add for detect fail safe mode - A025 //110922 Rick Modified
        if( StandardModeHeight <= 400 ||
                ( StandardModeHeight == 480 && ( vfreq > 575 && vfreq < 650 ) && SrcModeIndex != 0 ) )
        {
            UserPrefAutoTimes = 1;
        }
        if( PowerOnFlag )
            // system is on power-off status
        {
            Power_TurnOnGreenLed(); // turn on green ledÂÌµÆ
            //Power_TurnOffAmberLed(); // turn off amber led
            //Power_TurnOnAmberLed();//ºìµÆ
        }
    }

    if( !CableNotConnectedFlag || !SyncLossState() )
    {
        if( DoBurninModeFlag )
        {
        #if 0
            if (!ProductModeFlag)                   //110916 Rick add condition for A027
            Clr_DoBurninModeFlag();
	 #endif		
            //msWriteByte( BK0_32, 0x00 ); // disable background color function.
            drvOSD_FrameColorEnable(FALSE);
        }
    }
    Clr_DoModeSettingFlag(); //0606
    #if AudioFunc
    #if ((CHIP_ID == CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK) || (CHIP_ID == CHIP_TSUMD))   // reset audio FIFO
    msWriteByteMask(REG_1427, 0x10, 0x10);
    msWriteByteMask(REG_1427, 0x00, 0x10);
    #endif
    #if MS_DAC
#if 1
if((!(SyncLossState()&&CableNotConnectedFlag&&DoBurninModeFlag))
  &&(CURRENT_INPUT_IS_DISPLAYPORT()
  #if ENABLE_HDMI
  ||( gScInfo.InputTmdsType == TMDS_HDMI && CURRENT_INPUT_IS_TMDS())
  #endif
  ))   //130909 petit //20131022 update
  {
  #if Audio_Switch
	  SetAudioType(UserPrefHDMIAudioType);
  #else
	  msAudioLineout_SourceSel( E_LINEOUT_FROM_DAC );
			  //	  msAudioDPGA_SetVolume( E_AUDIO_LEFT_RIGHT_CH, 0x0000 );  //20121004
	  msAudioLineOutGain( 0x3D );
  #endif	
  }
  else // line in to line out: 550mVrms
  {
	 msAudioLineout_SourceSel( E_LINEOUT_FROM_LINEIN0 );
   //	msAudioDPGA_SetVolume( E_AUDIO_LEFT_RIGHT_CH, 0x0000 );
	msAudioLineOutGain( 0x27 );
  }

#else	
    #if ENABLE_HDMI
    if( gScInfo.InputTmdsType == TMDS_HDMI && CURRENT_INPUT_IS_TMDS())
    {
        msAudioLineout_SourceSel( E_LINEOUT_FROM_DAC );
        if( !SyncLossState()&&!UnsupportedModeFlag)
            msAudioDPGA_SetVolume( E_AUDIO_LEFT_RIGHT_CH, 0x00 );
        else
            msAudioDPGA_Mute();

        #if !USE_DAC_ADJ
        msAudioLineOutGain( 0x3D );
        #endif
    }
    else
    #endif
#if ENABLE_DP_INPUT
    if(CURRENT_INPUT_IS_DISPLAYPORT() && (UserPrefAudioSource == AUDIO_DIGITAL))
    {
        msAudioLineout_SourceSel( E_LINEOUT_FROM_DAC );
        msAudioDPGA_SetVolume( E_AUDIO_LEFT_RIGHT_CH, 0x00 );
       #if !USE_DAC_ADJ
    msAudioLineOutGain( 0x3D );
       #endif
    }
    else
#endif
    {
        msAudioLineout_SourceSel( E_LINEOUT_FROM_LINEIN0 );
        if( !SyncLossState()&&!UnsupportedModeFlag)
            msAudioDPGA_SetVolume( E_AUDIO_LEFT_RIGHT_CH, 0x00 );
        else
            msAudioDPGA_Mute();
        #if !USE_DAC_ADJ
        msAudioLineOutGain( 0x27 );
        #endif

    }
#endif
    msAudioEARout_Mute( FALSE );

    msAudioGainForceToggle();
    #endif
	#if EarphoneDet_Enable
		if(PowerOnFlag&&!UserOSDMuteOnFlag&&(!SyncLossState())) 	
		{
			   if(HarphoneDet_Pin)//Ear Push In
			   {
					   msAudioLineEarSwitch(E_EAR_OUT_);
					Set_HarphoneDetHighFlag();
			   }
			 else
			 {
					 msAudioLineEarSwitch(E_LINE_OUT_);
					 Clr_HarphoneDetHighFlag();   
		
			 }
			
		 }
 	 #endif
	
    #endif

    #if ((CHIP_ID == CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK) || (CHIP_ID == CHIP_TSUMD)||(CHIP_ID==CHIP_TSUM9)||(CHIP_ID==CHIP_TSUMF))
    #if ENABLE_DP_INPUT
    #if DP_ATI_GRAPHIC_CARD_INTERLACE_PATCH//Mike 130204
    if (CURRENT_INPUT_IS_DISPLAYPORT())
        DPRxATIInterlaceCheck();
    #endif
    #endif
    #endif

#if ENABLE_SUPER_RESOLUTION
    mStar_SetupSuperResolution( UserPrefSuperResolutionMode );
#endif

    Power_TurnOnPanel();    // turn on panel
    Menu_InitAction();      // initial menu OSD state

#if AudioFunc
    ////2009-07-22  =======================================
    if( !InputTimingChangeFlag )
    {
        if( !SyncLossState()&&!UnsupportedModeFlag)
            {
                mStar_AdjustVolume( UserPrefVolume );
            }
    }
#endif
}

void mStar_PrepareForTimingChange( void )
{

    #if 0//DEBUG_PRINT_ENABLE
    printData( "PrepareForTimingChange SrcInputType=%d", SrcInputType );
    #endif

    #if ENABLE_HDMI // wait for checking
    if( gScInfo.InputTmdsType == TMDS_HDMI )
        mstar_HDMIAudioMute( HDMI_AUDIO_OFF );
    mstar_HDMIInitialVariable();
   //mstar_HDMIResetColorSpace();
    mstar_HDMIRst(Reset_HDMIStatus);
    #endif
#if MS_DAC
    msAudioDPGA_Mute();
#endif
    Clr_ForcePowerSavingFlag();
    PowerDownCounter = 0;
    InputColorFormat = INPUTCOLOR_RGB;
    if( !DisplayLogoFlag )
    {
        if( PowerSavingFlag )
        {
            Power_PowerOnSystem();
            mStar_SetupFreeRunMode();
        }
        else
        {
            mStar_SetupFreeRunMode(); // keep clock output then turn off panel
            Power_TurnOffPanel();
		#if Enable_LED
			Power_TurnOnAmberLed();//ºìµÆ
			printMsg("666666");
		#endif
        }
        Osd_Hide();

  #if ENABLE_DVI_DUAL_LINK //Jison 110429 reset to single to speed up dvi dual-->single timing stable
            if (CURRENT_INPUT_IS_DVI() && SyncLossState())
                msDVIDualLinkMode(FALSE);
  #endif

    }

#if UseINT
    mStar_EnableModeChangeINT(FALSE);
#endif

    if( InputTimingStableFlag )
    {
        SC0_SCALER_RESET(GPR_B);//msWriteByte(SC0_F1, GPR_B);  // Reset Graphic port to re-counter input hfreq & vtotal
        ForceDelay1ms(3);
        SC0_SCALER_RESET(0);//msWriteByte(SC0_F1, 0);
        ForceDelay1ms(10);
    }
    Clr_InputTimingStableFlag(); // reset input timing stable and wait for timing stable
    InputTimingStableCounter = 0;
    SrcFlags &= ~( bUnsupportMode | bUserMode | bNativeMode );
    //g_bInputSOGFlag = 1; // set to 1 for timing change switch to separate HV first
    //msWriteByte(SC0_01, 0x00);        // disable double bufer.
    mStar_ScalerDoubleBuffer(FALSE);
    //mStar_WriteByte(REGBK, REGBANKADC);
    //msWriteByte(SC0_01, 0x00);        // enable ADC's double bufer.
    //mStar_WriteByte(REGBK, REGBANKSCALER);
    msWriteByteMask(SC0_E8,0,(BIT0|BIT1));
    msWriteByteMask(SC0_E9,0,(BIT0|BIT1));

#if ENABLE_DP_INPUT
    if(CURRENT_INPUT_IS_DISPLAYPORT())
    {
        #if DP_ASTRO_INTERLACE_PATCH
             DPRxInterlaceCheck(); //Jison 090519
        #endif
        if (SyncLossState())//Mike 120215 For graphics card output interlace timing (even Vtotal).
        {
            #if ((CHIP_ID == CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK) || (CHIP_ID == CHIP_TSUMD)||(CHIP_ID==CHIP_TSUM9)||(CHIP_ID==CHIP_TSUMF))
            #if DP_ATI_GRAPHIC_CARD_INTERLACE_PATCH//Mike 130204
                 msWriteByteMask(REG_130A, 0, BIT0);
            #endif
            #endif
        }

#if 0//DP_RESETHTT_PATCH
         DPMSA_Reset();
#endif
    }
#endif

}
//==========================================================

#define DISALE_CABLEDETECT  (INPUT_TYPE!=INPUT_1A)//101215 Modify
Bool mStar_SyncLossStateDetect( void )
{
    BYTE fStatus;
    Bool result = FALSE;

    fStatus = mStar_GetInputStatus();
    //printData("status %xH", fStatus);//msReadByte(INSTA));
    //printData("stable counter %d", InputTimingStableCounter);
    if( fStatus & SyncLoss )
    {
        if(CURRENT_INPUT_IS_VGA())
        {
            if( InputTimingStableCounter && (InputTimingStableCounter % ValidTimingStableCount == 0) )
                mStar_SetAnalogInputPort((g_bInputSOGFlag?FALSE:TRUE));

            fStatus = mStar_GetInputStatus();
            if( !( fStatus & SyncLoss ) )
                goto SyncActive;
        }

        if( CableNotConnectedFlag )  // no Cable
        {
            if( !IsCableNotConnected() )
            {
                Clr_CableNotConnectedFlag();
#if DISALE_CABLEDETECT
#else
                result = TRUE;
#endif
            }
        }
        else if( IsCableNotConnected() ) // no Sync
        {
            Set_CableNotConnectedFlag();
#if DISALE_CABLEDETECT
#else
        //Power_TurnOnGreenLed();
		Power_TurnOnGreenLed();

            result = TRUE;
#endif
        }

        if( PowerSavingFlag )
        {
            if(CURRENT_INPUT_IS_TMDS())
            {
#if ((CHIP_ID == CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK) || (CHIP_ID == CHIP_TSUMD) || (CHIP_ID == CHIP_TSUMJ)|| (CHIP_ID == CHIP_TSUM9)|| (CHIP_ID == CHIP_TSUMF))
                if(DVI_CLK_STABLE())
#else
                if(!DVI_CLOCK_IS_MISSING())//(!(msReadByte(SC0_CA) & 0x40)) // coding check stable or exist
#endif
                {
                    //Delay1ms(150);
                    Delay1ms( 50 );
                    mStar_PowerUp();
                    Delay1ms( 50 );
                    fStatus = mStar_GetInputStatus();
                    if( fStatus & SyncLoss )
                    {
                        mStar_PowerDown();
                    }
                    else
                    {
                        result = TRUE;
                    }
                }

        #if ENABLE_MHL && (CHIP_ID == CHIP_TSUM2)
                mapi_mhl_WakeupDetect();
        #endif
            }
            /********** wmz 20051117
            else if (SrcInputType==Input_YPbPr && msReadByte(BK0_E1)&0x50)
            { if (abs(HFreq(mStar_ReadWord(BK0_E5))-156)<20)
            { fStatus&=~SyncLoss;
            result=TRUE;
            }
            }
             ***************/
        }
    }
    else
    {
    SyncActive:
        result = TRUE;
    }
    if( result )
    {
        SrcFlags = ( SrcFlags & 0xF0 ) | fStatus;
    } // clear others flags, except sync informat (polarity & w/o)
    return result;
}

/*
Bool mStar_IsStatusChanged( void )
{
    // prevent input status change when setup mode.
    if( msReadByte( SC0_E1 ) &INTM_B )
    {
        if( !( SrcFlags & bInterlaceMode ) )
        {
            Set_InputTimingChangeFlag();
            return TRUE;
        }
    }
    else if( SrcFlags & bInterlaceMode )
    {
        Set_InputTimingChangeFlag();
        return TRUE;
    }
    return FALSE;
}
*/
Bool mStar_ValidTimingDetect( void )
{
    BYTE fStatus;
    fStatus = mStar_GetInputStatus();
    if( fStatus & SyncLoss )        // no sync
    {
        SrcFlags = ( SrcFlags & 0xF0 ) | fStatus; // clear others flags, except sync informat (polarity & w/o)

        DETECT_printMsg( "Valid Timing : Sync Loss" );

        return TRUE;
    }
    else
    {
			#if 0
        if( SyncPolarity( SrcFlags ) != SyncPolarity( fStatus ) )            // Sync polarity changed
        {
            SrcFlags = ( SrcFlags & 0xF0 ) | fStatus; // clear others flags, except sync informat (polarity & w/o)

            
	    	#if ENABLE_FREESYNC
			if(!IS_HDMI_FREESYNC())
			#endif
			{
			DETECT_printMsg( "Valid Timing : Sync polarity changed" );
            return TRUE;
			}
        }
        else
			#endif
        {
            WORD tempPeriod;
            SrcFlags = ( SrcFlags & 0xF0 ) | fStatus; // clear others flags, except sync informat (polarity & w/o)

#if ENABLE_FREESYNC
            if(!IS_DP_FREESYNC())
#endif
            {
                tempPeriod = SC0_READ_HPEROID();//msRead2Byte(SC0_E4) & MASK_13BIT;

                #if DEBUG_PRINT_ENABLE
                // printData("HPeriod1 %d", tempPeriod);
                #endif
                if( InputTimingStableCounter == 0 )
                {
                    SrcHPeriod = tempPeriod;
                }
                if( labs( (DWORD)tempPeriod - SrcHPeriod ) > HPeriod_Torlance )
                    // HPeriod changed
                {
                    tempPeriod = SC0_READ_HPEROID();//msRead2Byte(SC0_E4) & MASK_13BIT;
                    if( labs( (DWORD)tempPeriod - SrcHPeriod ) > HPeriod_Torlance )
                    {
#if ENABLE_DEBUG_TIMING_CHANGE_POINT
                        SetTimingChangeFirstPoint(TimingChange_SrcHPeriod);
#endif
						DETECT_printMsg( "Valid Timing : HPeriod changed" );
                    return TRUE;
                    }
                }
            }
#if ENABLE_FREESYNC
            if(!IS_HDMI_FREESYNC())
#endif
            {
                tempPeriod = SC0_READ_VTOTAL();//msRead2Byte(SC0_E2) & MASK_11BIT;

                #if DEBUG_PRINT_ENABLE
                // printData("  VTotal1 %d", tempPeriod);
                #endif
                if( InputTimingStableCounter == 0 )
                {
                    // 100902 coding addition
                    Delay1ms( 100 );
                    tempPeriod = SC0_READ_VTOTAL();//msRead2Byte(SC0_E2) & MASK_11BIT;

                    SrcVTotal = tempPeriod;
                }
                if( abs( tempPeriod - SrcVTotal ) > VTotal_Torlance )
                // vtotal changed
                {
#if ENABLE_HDMI // 100902 coding addition
                    Delay1ms( 10 );
                    tempPeriod = SC0_READ_VTOTAL();//msRead2Byte(SC0_E2) & MASK_11BIT;
                    if( labs( (DWORD)tempPeriod - SrcVTotal ) > VTotal_Torlance ) // vtotal changed
#endif
                    {
#if ENABLE_DEBUG_TIMING_CHANGE_POINT
                        SetTimingChangeFirstPoint(TimingChange_SrcVTotal);
#endif
						DETECT_printMsg( "Valid Timing : VTotal changed" );
                        return TRUE;
                    }
                }
            }
#if ENABLE_CHECK_TMDS_DE
            if( CURRENT_INPUT_IS_TMDS() )
            {
                if(!InputTimingStableFlag && (InputTimingStableCounter >= (ValidTimingStableCount-1)))
                {
                    SrcTMDSWidth = SC0_READ_AUTO_WIDTH();
                    SrcTMDSHeight = SC0_READ_AUTO_HEIGHT();
                    SrcTMDSHStart = SC0_READ_AUTO_START_H();
                    SrcTMDSVStart = SC0_READ_AUTO_START_V();
                }

                if(InputTimingStableFlag)
                {
                    tempPeriod = SC0_READ_AUTO_WIDTH();
                    if( labs( (DWORD)tempPeriod - SrcTMDSWidth ) > TMDS_TOLERANCE )
                    {
                        tempPeriod = SC0_READ_AUTO_WIDTH();
                        if( labs( (DWORD)tempPeriod - SrcTMDSWidth ) > TMDS_TOLERANCE )
                        {
                            DETECT_printData( "SrcTMDSWidth changed%d", tempPeriod );

#if ENABLE_DEBUG_TIMING_CHANGE_POINT
                            SetTimingChangeFirstPoint(TimingChange_SrcTMDSWidth);
#endif
                            return TRUE;
                        }
                    }

                    tempPeriod = SC0_READ_AUTO_HEIGHT();
                    if( labs( (DWORD)tempPeriod - SrcTMDSHeight ) > TMDS_TOLERANCE )
                    {
#if ENABLE_HDMI
                        Delay1ms( 20 );
                        tempPeriod = SC0_READ_AUTO_HEIGHT();
                        if( labs( (DWORD)tempPeriod - SrcTMDSHeight ) > TMDS_TOLERANCE )
#endif
                        {
                            DETECT_printData( "SrcTMDSHeight changed%d", tempPeriod );
#if ENABLE_DEBUG_TIMING_CHANGE_POINT
                            SetTimingChangeFirstPoint(TimingChange_SrcTMDSHeight);
#endif
                            return TRUE;
                        }
                    }

                    tempPeriod = SC0_READ_AUTO_START_H();//msRead2Byte( SC0_80 ) & MASK_12BIT;
                    if( labs( (DWORD)tempPeriod - SrcTMDSHStart ) > TMDS_TOLERANCE )
                    {
                        DETECT_printData( "SrcTMDSHStart changed%d", tempPeriod );
#if ENABLE_DEBUG_TIMING_CHANGE_POINT
                        SetTimingChangeFirstPoint(TimingChange_SrcTMDSHStart);
#endif
                        return TRUE;
                    }

                    tempPeriod = SC0_READ_AUTO_START_V();//msRead2Byte( SC0_7E ) & MASK_11BIT;
                    if( labs( (DWORD)tempPeriod - SrcTMDSVStart ) > TMDS_TOLERANCE )
                    {
                        DETECT_printData( "SrcTMDSVStart changed%d", tempPeriod );
#if ENABLE_DEBUG_TIMING_CHANGE_POINT
                        SetTimingChangeFirstPoint(TimingChange_SrcTMDSVStart);
#endif
                        return TRUE;
                    }
                }

            }
#endif

        }
    }
    return FALSE;
}

#if  MS_VGA_SOG_EN
BYTE GetVSyncWidth(void)
{
    BYTE u8VSyncWidth;
    SC0_VSYNC_WIDTH_REPORT(TRUE);//msWriteByteMask(SC0_FA,BIT0,BIT0);
    u8VSyncWidth=SC0_READ_VSYNC_WIDTH();//msReadByte(SC0_E2);
    SC0_VSYNC_WIDTH_REPORT(FALSE);//msWriteByteMask(SC0_FA,0,BIT0);
    //PRINT_DATA("u8VSyncWidth=%d",u8VSyncWidth);
    return u8VSyncWidth;
}
#endif

BYTE mStar_GetInputStatus( void )
{
    BYTE fStatus = 0;
    WORD inputValue;
    BYTE status;
#if ENABLE_FREESYNC
    if(!IS_DP_FREESYNC())
#endif
    {
        inputValue = SC0_READ_HPEROID();//msRead2Byte(SC0_E4) & MASK_13BIT;
        if( inputValue == 0x1FFF || inputValue < 10 )
        {
            fStatus |= bHSyncLoss;
#if ENABLE_DEBUG_TIMING_CHANGE_POINT
            SetTimingChangeFirstPoint(TimingChange_Hperoid);
#endif
        }
    }
#if ENABLE_FREESYNC
    if(!IS_HDMI_FREESYNC())
#endif
    {
        inputValue = SC0_READ_VTOTAL();//msRead2Byte(SC0_E2) & MASK_11BIT;
        if( inputValue == 0x7FF || inputValue < 200 )
        {
            fStatus |= bVSyncLoss;
#if ENABLE_DEBUG_TIMING_CHANGE_POINT
            SetTimingChangeFirstPoint(TimingChange_Vtotal);
#endif
        }
    }

    status = SC0_READ_SYNC_STATUS();//msReadByte(SC0_E1);

    fStatus |= ( status & 0x03 ); // Get input timing polarity

    if(CURRENT_INPUT_IS_TMDS())    // input timing is valid while current state is no sync
    {
        if(DVI_DE_STABLE() == FALSE)
        {
            if(DVI_DE_STABLE() == FALSE)
            {
                fStatus |= SyncLoss;
                #if DETECT_DEBUG
                if((InputTimingStableCounter%5)==0)
                DETECT_printData("DVI DE Unstable", 1);
                #endif

#if ENABLE_DEBUG_TIMING_CHANGE_POINT
                SetTimingChangeFirstPoint(TimingChange_TMDS_DE);
#endif

            }
        }
    }
    else if( SyncLossState() && !( fStatus & SyncLoss ) )        // input timing is valid while current state is no sync
    {
        Delay1ms( 20 );
        if( InputTimingChangeFlag )
        {
            return fStatus;
        }
        status = status & SC0_READ_SYNC_STATUS();//msReadByte(SC0_E1);
        if(( status & 3 ) != ( fStatus & 3 ) )
            // polarity is stable
        {
            fStatus |= SyncLoss;
#if ENABLE_DEBUG_TIMING_CHANGE_POINT
            SetTimingChangeFirstPoint(TimingChange_SyncPol);
#endif
        }
        else if( status & 0x30 )
            // SOG or CSync input
        {
            Delay1ms( 20 );
            if( InputTimingChangeFlag )
            {
                return fStatus;
            }
            status = SC0_READ_SYNC_STATUS();//msReadByte(SC0_E1);           // Check if SOG/CSYNC is valid
            if( labs( (DWORD)SC0_READ_VTOTAL() - inputValue ) > 2 )//( abs( (msRead2Byte( SC0_E2 )&MASK_11BIT) - inputValue ) > 2 )
            {
                fStatus |= SyncLoss;
            }
#if  MS_VGA_SOG_EN

else if (status&SOGD_B)
            {
                #if 1
                if(!SyncLossState())
                 {
                    //BYTE i;
                    //for (i=0;i<10;i++)
                    {
                        inputValue=GetVSyncWidth();
                        if (inputValue>15||inputValue==0)
                        {
                            //printMsg("SOG Det Err");
                            fStatus |= SyncLoss;
                            //break;
                        }
                        //ForceDelay1ms(2);
                    }
                }
                #else
                {
                    BYTE u8Tmp0=SC0_READ_POWER_DOWN_STATUS();//msReadByte(SC0_F0);
                    BYTE u8Tmp1=SC0_READ_ADC_COAST_ENABLE_VALUE();//msReadByte(SC0_ED);
                    BYTE u8Tmp2=SC0_READ_ADC_COAST_START_VALUE();//msReadByte(SC0_EE);
                    BYTE u8Tmp3=SC0_READ_ADC_COAST_END_VALUE();//msReadByte(SC0_EF);
                    BYTE i;
                    //for get stable VSyncWidth
                    SC0_SCALER_POWER_DOWN(0, BIT0|BIT1); //to get vsyncWidth
                    SC0_ADC_COAST_ENABLE(0x21);//msWriteByte(SC0_ED, 0x21); //_NEW_SOG_WAKEUP_DET_
                    SC0_ADC_COAST_START(0x08);//msWriteByte(SC0_EE, 0x08);
                    SC0_ADC_COAST_END(0x05);//msWriteByte(SC0_EF, 0x05);
                    Delay1ms(20);
                    inputValue=0;
                    for (i=0;i<10;i++)
                    {
                        inputValue=GetVSyncWidth();
                        if (inputValue>15||inputValue=0)
                        {
                      //printMsg("SOG Det Err");
                            fStatus |= SyncLoss;
                            break;
                        }
                        ForceDelay1ms(2);
                    }
                    SC0_ADC_COAST_ENABLE(u8Tmp1);//msWriteByte(SC0_ED, u8Tmp1);
                    SC0_ADC_COAST_START(u8Tmp2);//msWriteByte(SC0_EE, u8Tmp2);
                    SC0_ADC_COAST_END(u8Tmp3);//msWriteByte(SC0_EF, u8Tmp3);
                    SC0_SCALER_POWER_DOWN(u8Tmp0, 0xFF);
                }
                #endif
            }

#endif
        }
    }
    return fStatus;
}

#ifndef CABLE_DET_SAR
#define CABLE_DET_SAR       0
#endif

#if (INPUT_TYPE == INPUT_1A && CABLE_DETECT_VGA_USE_SAR) ||\
    (INPUT_TYPE == INPUT_1A1D && CABLE_DETECT_VGA_USE_SAR && CABLE_DETECT_DVI_USE_SAR)||\
    (INPUT_TYPE == INPUT_1A1D1DP && CABLE_DETECT_VGA_USE_SAR && CABLE_DETECT_DVI_USE_SAR)
Bool IsCableNotConnected( void )
{
    BYTE retry_Key, temp, temp1;
    retry_Key = 3;
    while( retry_Key )
    {
        temp = CABLE_DET_SAR;
        Delay1ms( 2 );
        temp1 = CABLE_DET_SAR;
        if( abs( temp - temp1 ) < 3 )
            break;
        retry_Key--;
    }

    DETECT_printData( "CABEL ADC =%d", temp );

#if ENABLE_DP_INPUT
    if( temp > 190 )
        return (TRUE&&(DP_CABLE_NODET));
    else
        return (FALSE);
#else
    if( temp > 190 )
        return TRUE;
    else
        return FALSE;
#endif
}
#elif (INPUT_TYPE == INPUT_1A && !CABLE_DETECT_VGA_USE_SAR) || \
        (INPUT_TYPE == INPUT_1A1D && !CABLE_DETECT_VGA_USE_SAR && !CABLE_DETECT_DVI_USE_SAR)
Bool IsCableNotConnected( void )
{
#if CABLE_DETECT_DEBUG_ENABLE
    DETECT_printData( "hwDSUBCable_Pin %d", hwDSUBCable_Pin );
    DETECT_printData( "hwDVICable_Pin %d", hwDVICable_Pin );
#endif

    #if INPUT_TYPE==INPUT_1A
    return ( hwDSUBCable_Pin );
    #else
    #if (CHIP_ID == CHIP_TSUMJ) || (CHIP_ID == CHIP_TSUM9) || (CHIP_ID == CHIP_TSUMF)
    BYTE temp;
    temp = hwDSUBCable_Pin | hwDVI0Cable_Pin;
    if (temp && 0xFF)
        return TRUE;
    else
        return FALSE;
    #else
    return ( hwDSUBCable_Pin && hwDVICable_Pin );
    #endif
    #endif
}

#elif (INPUT_TYPE == INPUT_1A1D1H || INPUT_TYPE == INPUT_1A2H) // 120315 coding test
Bool IsCableNotConnected( void )
{
    BYTE retry_Key, temp, temp1;
    retry_Key = 3;
    while( retry_Key )
    {
        temp = CABLE_DET_SAR;
        Delay1ms( 2 );
        temp1 = CABLE_DET_SAR;
        if( abs( temp - temp1 ) < 3 )
            break;
        retry_Key--;
    }

    DETECT_printData( "CABEL ADC =%d", temp );

    if( temp > 0xF0 && hwDVICable_Pin)
        return TRUE;
    else
        return FALSE;
}
#elif (INPUT_TYPE == INPUT_1A2C || INPUT_TYPE == INPUT_1A3C || INPUT_TYPE == INPUT_1A1C2) // Combo port
Bool IsCableNotConnected( void ) // Not Finishhed Yet in CHIP_TSUMC
{
    BYTE temp;

#if (HDMI_PORT&TMDS_PORT_A)||(DVI_PORT&TMDS_PORT_A)
    temp = hwDVI0Cable_Pin;
#endif
#if (HDMI_PORT&TMDS_PORT_B)||(DVI_PORT&TMDS_PORT_B)
    temp = hwDVI1Cable_Pin;
#endif
#if (HDMI_PORT&TMDS_PORT_C)||(DVI_PORT&TMDS_PORT_C)
    temp = hwDVI2Cable_Pin;
#endif

    temp = 0xFF;

    if(hwDSUBCable_Pin && temp)
        return TRUE;
    else
        return FALSE;
}
#else
#message "no coding for  IsCableNotConnected"
Bool IsCableNotConnected( void )
{
    // wait for coding
    return FALSE;
}
#endif

#if ENABLE_HDMI
Bool HDMICableNotConnect(void)
{
    bit bresult = FALSE; // false means HDMI cable connect
#if MainBoardType == MainBoard_5603_M0B
#define HDMI_STAY_1S_COUNT      50
#define JUST_HDMI_PLUG   0x5D
#define JUST_VGA_PLUG   0x35
#define BOTH_VGA_HDMI_PLUG   0x26
#define NONE_PLUG    0xFF

    if(CURRENT_INPUT_IS_HYBRID_MHL_HDMI() && !GET_MHL_CABLE_PLUGGED())
    {
        if( !HYBRID_MHL_HDMI_CABLE_CONNECT())
        {
            Delay1ms(2);
            if( !HYBRID_MHL_HDMI_CABLE_CONNECT())
                bresult = TRUE;
        }
    }
    else if(CURRENT_INPUT_IS_PURE_HDMI())
    {
        BYTE tempValue = CABLE_DET_SAR;
        if(abs(tempValue-JUST_HDMI_PLUG) > 10 || abs(tempValue -BOTH_VGA_HDMI_PLUG) > 10)
        {
            bresult = TRUE;
        }
    }
#elif CHIP_ID == CHIP_TSUMC || CHIP_ID == CHIP_TSUMK || CHIP_ID == CHIP_TSUMD // demo board
#if ENABLE_MHL_C1
#define IS_HDMI_PORT0_CABLE_CONNECT()     (hwDVI0Cable_Pin <= 0x10) // 0V
#else
#define IS_HDMI_PORT0_CABLE_CONNECT()     hwDVI0Cable_Pin
#endif
#if ENABLE_MHL_C2
#define IS_HDMI_PORT1_CABLE_CONNECT()     (hwDVI1Cable_Pin <= 0x10) // 0V
#else
#define IS_HDMI_PORT1_CABLE_CONNECT()     hwDVI1Cable_Pin
#endif
#if ENABLE_MHL_C3
#define IS_HDMI_PORT2_CABLE_CONNECT()     (hwDVI2Cable_Pin <= 0x10) // 0V
#else
#define IS_HDMI_PORT2_CABLE_CONNECT()     hwDVI2Cable_Pin
#endif
#if (CInput_HDMI_C1 != CInput_Nothing)
    if(CURRENT_INPUT_IS_HDMI0())
    {
        if(!IS_HDMI_PORT0_CABLE_CONNECT())
        {
            bresult = TRUE;
#if DEBUG_HDMI_MHL_CABLE_DETECT
DEBUG_HDMI_MHL_CABLE( "HDMI0 CableNotConnect:%x", hwDVI0Cable_Pin );
#endif
        }
        else
        {
#if DEBUG_HDMI_MHL_CABLE_DETECT
DEBUG_HDMI_MHL_CABLE( "HDMI0 CableConnect:%x", hwDVI0Cable_Pin );
#endif
        }
    }
#endif

#if (CInput_HDMI_C2 != CInput_Nothing)
    if(CURRENT_INPUT_IS_HDMI1())
    {
        if(!IS_HDMI_PORT1_CABLE_CONNECT())
        {
            bresult = TRUE;
#if DEBUG_HDMI_MHL_CABLE_DETECT
DEBUG_HDMI_MHL_CABLE( "HDMI1 CableNotConnect:%x", hwDVI1Cable_Pin );
#endif
        }
        else
        {
#if DEBUG_HDMI_MHL_CABLE_DETECT
DEBUG_HDMI_MHL_CABLE( "HDMI1 CableConnect:%x", hwDVI1Cable_Pin );
#endif
        }
    }
#endif

#if (CInput_HDMI_C3 != CInput_Nothing)
    if(CURRENT_INPUT_IS_HDMI2())
    {
        if(!IS_HDMI_PORT2_CABLE_CONNECT())
        {
            bresult = TRUE;
#if DEBUG_HDMI_MHL_CABLE_DETECT
DEBUG_HDMI_MHL_CABLE( "HDMI2 CableNotConnect:%x", hwDVI2Cable_Pin );
#endif
        }
        else
        {
#if DEBUG_HDMI_MHL_CABLE_DETECT
DEBUG_HDMI_MHL_CABLE( "HDMI2 CableConnect:%x", hwDVI2Cable_Pin );
#endif
        }
    }
#endif
#else
#message "please coding HDMICableNotConnect fro speedup switch port"
#endif

    return bresult;
}
#endif

#if ENABLE_MHL
    #if(CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID == CHIP_TSUMD || CHIP_ID == CHIP_TSUM9 || CHIP_ID == CHIP_TSUMF)
Bool CurrentInputMHLCablePlugged(void)
{
    bit bresult = FALSE; // false means MHL cable NC

#if CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID == CHIP_TSUMD // demo board

#if ENABLE_MHL_C1
    if(CURRENT_INPUT_IS_HDMI0())
    {
        if(IS_MHL_PORT0_CABLE_CONNECT())
        {
            bresult = TRUE;
#if DEBUG_HDMI_MHL_CABLE_DETECT
DEBUG_HDMI_MHL_CABLE( "MHL0 CableConnect:%x", hwDVI0Cable_Pin );
#endif
        }
        else
        {
#if DEBUG_HDMI_MHL_CABLE_DETECT
DEBUG_HDMI_MHL_CABLE( "MHL0 CableNotConnect:%x", hwDVI0Cable_Pin );
#endif
        }
    }
#endif

#if ENABLE_MHL_C2
    if(CURRENT_INPUT_IS_HDMI1())
    {
        if(IS_MHL_PORT1_CABLE_CONNECT())
        {
            bresult = TRUE;
#if DEBUG_HDMI_MHL_CABLE_DETECT
DEBUG_HDMI_MHL_CABLE( "MHL1 CableConnect:%x", hwDVI0Cable_Pin );
#endif
        }
        else
        {
#if DEBUG_HDMI_MHL_CABLE_DETECT
DEBUG_HDMI_MHL_CABLE( "MHL1 CableNotConnect:%x", hwDVI1Cable_Pin );
#endif
        }
    }
#endif

#if ENABLE_MHL_C3
    if(CURRENT_INPUT_IS_HDMI2())
    {
        if(IS_MHL_PORT2_CABLE_CONNECT())
        {
#if DEBUG_HDMI_MHL_CABLE_DETECT
DEBUG_HDMI_MHL_CABLE( "MHL2 CableConnect:%x", hwDVI0Cable_Pin );
#endif
            bresult = TRUE;
        }
        else
        {
#if DEBUG_HDMI_MHL_CABLE_DETECT
DEBUG_HDMI_MHL_CABLE( "MHL2 CableNotConnect:%x", hwDVI2Cable_Pin );
#endif
        }
    }
#endif

#else
#message "please coding CurrentInputMHLCablePlugged for MHL port"
#endif

    return bresult;
}
    #endif
#endif

void SpeedupOrExtendSwitchPort(void)
{
#if ENABLE_MHL
    if(CURRENT_INPUT_MHL_CABLE_PLUGGED())
    {
        if(MHL_EXTEND_SYNCLOSS_COUNT())
        {
            InputTimingStableCounter = SyncLossExtenCount_MHL;
            MHLExtenCountFlag = 1;
        }
    }
    else
#endif

#if ENABLE_HDMI
    if(HDMI_SPEEDUP_SWITCH_PORT())
    {
        InputTimingStableCounter = SyncLossStableCount;
    }
#endif

#if (ENABLE_DP_INPUT&&(CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID == CHIP_TSUMD))
//============================================================================================
//  Please add Cable GND pin detect in  below condition
//  ((SrcInputType  == Input_Displayport)&&(msReadByte(REG_0232)&BIT0)&& (Cable GND pin Detect) )
//===================================================================================== =======
       if(Input_Displayport_C2 != Input_Nothing)
       {
              EnablePBSDM();                              // Set SDM Bit as Input
             if(DPPB_SPEEDUP_SDM_GND())     //  Check SDM bit Port B
            {
                InputTimingStableCounter = SyncLossStableCount;
            }
       }
       if(Input_Displayport_C3  != Input_Nothing)
       {
              EnablePCSDM();                                // Set SDM Bit as Input
              if(DPPC_SPEEDUP_SDM_GND())       // Check SDM bit Port C
             {
                InputTimingStableCounter = SyncLossStableCount;
             }
      }
#endif




}
WORD GetImageWidth(void)
{
    WORD u16ImageWidth;
    if( CURRENT_INPUT_IS_VGA() )
    {
        u16ImageWidth=StandardModeWidth;
    }
#if ENABLE_DP_INPUT
    else if( CURRENT_INPUT_IS_DISPLAYPORT() )
    {
    	#if CHIP_ID == CHIP_TSUMU
		u16ImageWidth=SC0_READ_AUTO_WIDTH();
		#else
        u16ImageWidth=gDPInfo.uwDPHWidth;
		#endif
    }
#endif
#if  ENABLE_CHECK_TMDS_DE
    else if( CURRENT_INPUT_IS_TMDS() )
    {
        u16ImageWidth=SrcTMDSWidth;
    }
#endif
    else
    {
        u16ImageWidth=SC0_READ_AUTO_WIDTH();
    }
    return u16ImageWidth;
}
WORD GetImageHeight(void)
{
    WORD u16ImageHeight;
    if( CURRENT_INPUT_IS_VGA() )
    {
        u16ImageHeight=StandardModeHeight;
    }
#if ENABLE_DP_INPUT
    else if( CURRENT_INPUT_IS_DISPLAYPORT() )
    {
        u16ImageHeight=gDPInfo.uwDPVWidth;
    }
#endif
#if  ENABLE_CHECK_TMDS_DE
    else if( CURRENT_INPUT_IS_TMDS() )
    {
        u16ImageHeight=SrcTMDSHeight;
    }
#endif
    else
    {
        u16ImageHeight=SC0_READ_AUTO_HEIGHT();
    }
    return u16ImageHeight;
}
