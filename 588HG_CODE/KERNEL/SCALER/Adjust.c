#include <math.h>
#include "types.h"
#include "global.h"
#include "ms_reg.h"
#include "ms_rwreg.h"
#include "debug.h"
#include "Common.h"
#include "msosd.h"
#include "Board.h"
#include "menudef.h"
#include "msdlc.h"
#include "msace.h"
#if ENABLE_HDMI
#include "msHDMI.h"
#include "msHDMIACE.h"
#endif
#include "misc.h"
#include "drvosd.h"
#include "drvgpio.h"
#include "adjust.h"
#if ENABLE_LED_CONTROLLER
#include "LEDControl.h"
#endif
#if 1//USE_DAC_ADJ
#include "drvDAC.H"
#endif
//2020-12-16 add jove
#include "Customer_config.h"
#include "Customer_CEHUA.h"
////////////////////////////////////////
//#include "drvAdjust.h"
//#include "halRwreg.h"
///////////////////////////////////////
#if ENABLE_DPS
#include "msdps_setting.h"
#endif
//===================================================
// Local function declaration
#if UsesRGB
void mStar_AdjustRedColor( BYTE rColor, BYTE gColor, BYTE bColor );
void mStar_AdjustGreenColor( BYTE rColor, BYTE gColor, BYTE bColor );
void mStar_AdjustBlueColor( BYTE rColor, BYTE gColor, BYTE bColor );
#else
void mStar_AdjustRedColor( BYTE color, BYTE contrast );
void mStar_AdjustGreenColor( BYTE color, BYTE contrast );
void mStar_AdjustBlueColor( BYTE color, BYTE contrast );
#endif

extern void GetColorTempRGB(ColorType *pstColor);
extern Bool IsColorspaceRGB(void);
#if ENABLE_POWER_PORTECT
extern void PowerProtectInit(void);
#endif

void mStar_AdjustBlackLevel( BYTE BlackLevel );

#ifndef MinDutyValue
#define MinDutyValue        0x00
#endif
#ifndef MidDutyValue
#define MidDutyValue        0x60  
#endif
#ifndef MaxDutyValue
#define MaxDutyValue       0xFF 
#endif

//For User Brightness
#ifndef RealMaxBrightnessValue
#define RealMaxBrightnessValue	MaxDutyValue
#endif
#ifndef RealMinBrightnessValue
#define RealMinBrightnessValue	MinDutyValue
#endif


#define RealAbsoluteMinBrightnessValue  0x10


#define LowBoundDCRRealBriRatio          20     // CCFL: 30%, LED:50%
#define RealMinDcrBrightnessValue    ((((RealMaxBrightnessValue - RealMinBrightnessValue) * LowBoundDCRRealBriRatio) / 100) + RealMinBrightnessValue)

//****************************************************
#define MaxBacklightNowValue    240  // HongYi suggest 100712 Rex
#define MinBacklightNowValue      20//17//29  //39  //19//17

#if MainBoardType==MainBoard_3598 || MainBoardType == MainBoard_4432 || MainBoardType == MainBoard_4663
#define AbsMostDarkest1  ((CURRENT_INPUT_IS_VGA())?(19):(17))  // 16  //20081201
#define AbsNearDarkest1  ((CURRENT_INPUT_IS_VGA())?(24):(20))  // 18
#define AbsMostDarkest2  ((CURRENT_INPUT_IS_VGA())?(19):(17))  // 17
#define AbsNearDarkest2  ((CURRENT_INPUT_IS_VGA())?(25):(22))  // 20
#else
#define AbsMostDarkest1  ((CURRENT_INPUT_IS_VGA())?(16):(16))  // 16  //20081201
#define AbsNearDarkest1  ((CURRENT_INPUT_IS_VGA())?(19):(17))  // 18
#define AbsMostDarkest2  ((CURRENT_INPUT_IS_VGA())?(16):(16))  // 17
#define AbsNearDarkest2  ((CURRENT_INPUT_IS_VGA())?(21):(19))  // 20
#endif

#define GET_MAP_VALUE(_X_,_Y_,_Z_)  ((DWORD)(_X_) * (_Y_) / (_Z_))

WORD UserAdjust_NonlinerSetCurve(WORD level, WORD code *p_curve, WORD num)
{
    DWORD     value1=0,temp1=0,temp2=0;
    DWORD    i=0;
//	printf("\r\n level  is %d",level);
    for(i=2;i<num;i+=2)
    {
        if(level==p_curve[i])
        {
            value1=p_curve[i+1];
            break;
        }
        if(level>=p_curve[i-2] && level<=p_curve[i])
        {

            temp1 = p_curve[i] - p_curve[i-2];
		if(p_curve[i+1]>p_curve[i-1])
			{
            temp2 =p_curve[i+1]- p_curve[i-1];
			
			value1=p_curve[i-1]+(DWORD)GET_MAP_VALUE(((DWORD)level-p_curve[i-2]),temp2,temp1);
			}
		else
	           {
	            temp2 =p_curve[i-1]- p_curve[i+1];		
		value1= p_curve[i-1]-(DWORD)GET_MAP_VALUE(((DWORD)level-p_curve[i-2]),temp2,temp1);
			}	 
            break;
        }
    }
    return value1;
}

//****************************************************
BYTE code msAdjustNullData[] = {0};
void msAdjustDummy(void)
{
    BYTE xdata i = msAdjustNullData[0];
}
void mStar_AdjustDcrBrightness( void )
{
    static BYTE xdata brightness;
    static BYTE xdata DcrCutoffFlag = 0;    //DCR new rule 081128

#if ENABLE_DEBUG
static BYTE xdata PreValue;
#endif

    if( BacklightNow < MinBacklightNowValue )
        BacklightNow = MinBacklightNowValue;

    if( BacklightNow > MaxBacklightNowValue )
    {
        brightness = RealMaxBrightnessValue;
        DcrCutoffFlag = 0;    //20081201
    }
    else if( BacklightNow <= MinBacklightNowValue )
    {
        if( SetDarkestFlag ) // the black pattern
        {
            SetDarkestFlag = 0;
            PreBrightData = 0; // to make sure if bright data exist, it will update brightness.
            //brightness = RealAbsoluteMinBrightnessValue;  // set logical absolute dark value, // RealMinBrightnessValue;
            if( DcrCutoffFlag == 0 ) // DcrCutoffFlag nitial value is 0
            {
                if(( DarkData <= AbsMostDarkest1 ) && ( BrightData <= AbsNearDarkest1 ) )   // >>>  logical absolute dark condition  <<< 20081128
                    // set logical absolute dark value, // RealMinBrightnessValue;
                {
                    //brightness = RealMinDcrBrightnessValue;  // set logical absolute dark value, // RealMinBrightnessValue;
                    DcrCutoffFlag = 1;
                }
                brightness = RealMinDcrBrightnessValue;  // set logical absolute dark value, // RealMinBrightnessValue;
            }
            else
            {
                if(( DarkData <= AbsMostDarkest2 ) && ( BrightData <= AbsNearDarkest2 ) )   // set logical absolute dark value,
                {
                    if( brightness > RealAbsoluteMinBrightnessValue )
                        brightness--;  // set logical absolute dark value, // RealMinBrightnessValue;
#if defined(DCRClearBacklight)
                     if(OSD_MENU_EXIST() && ( brightness < RealMinBrightnessValue ) )
                        brightness = RealMinBrightnessValue;
                     ForceDelay1ms( 100 );
#endif
                }
                else
                {
                    DcrCutoffFlag = 0;
                    brightness = RealMinDcrBrightnessValue;
                }
            }
        }    // if(SetDarkestFlag)
        else // RealMinDcrBrightnessValue=98 -> brightness >=98 (min)
        {
            DcrCutoffFlag = 0;
            brightness = RealMinDcrBrightnessValue;
        }
    }
    else  // RealMinDcrBrightnessValue=98 -> brightness >=98 (min)
    {
        DcrCutoffFlag = 0;
        brightness = ((( DWORD )( BacklightNow - MinBacklightNowValue ) * ( RealMaxBrightnessValue - RealMinDcrBrightnessValue ) ) / ( MaxBacklightNowValue - MinBacklightNowValue ) ) + RealMinDcrBrightnessValue;
    }

    #if 0//DEBUG_PRINT_ENABLE
    printData( "---BacklightNow =%d", BacklightNow );
    printData( "---brightness =%d", brightness );
    #endif

    #if ENABLE_DEBUG
    if(abs(PreValue-BacklightNow) > 3)
    {
        PreValue = BacklightNow;
    printData( "---BacklightNow =%d", BacklightNow );
    }
    #endif

#if ENABLE_LED_CONTROLLER
    msLED_DimmingPWMAdjust(brightness);
    drvGPIO_SetBacklightDuty(0,0); // wait for coding, prevent complier warning
#else
    #if BRIGHTNESS_INVERSE  //+Duty power
    drvGPIO_SetBacklightDuty(BrightnessPWM, 0xFF-brightness);
    #else                           //-Duty power
    drvGPIO_SetBacklightDuty(BrightnessPWM, brightness);
    #endif
#endif

    #if defined(DCRClearBacklight)
    if( brightness == RealAbsoluteMinBrightnessValue )
    {

        if( BackLightActiveFlag )
            hw_ClrBlacklit();
    }
    else
    {
        if( !BackLightActiveFlag )
            hw_SetBlacklit();
    }
    #endif
}

void mStar_AdjustBrightness( BYTE brightness )
{
		if(brightness>=MaxBrightnessValue)
		brightness=MaxBrightnessValue;
		else if(brightness<=MinBrightnessValue)
		brightness=MinBrightnessValue;
	
	#if Enable_DIGITAL_DIMMING
		ICE_MAIN_BRI_CTRL(TRUE);
	#endif
	
	
		 if(brightness>50) //DefBrightness
			{
			   // brightness=((DWORD)(brightness-DefBrightness)*(MaxDutyValue-MidDutyValue))/(MaxBrightnessValue-DefBrightness)+MidDutyValue;
				
			   brightness=((DWORD)(brightness-50)*(MaxDutyValue-MidDutyValue))/50+MidDutyValue;
    #if Enable_DIGITAL_DIMMING
			   brightness=brightness-64;
			  // ICE_BRIGHTNESS_RGBCMY(0x00);
	
	
			ICE_BRIGHTNESS_RGBCMY(brightness);	
	
	#endif
	
			}
		else
			{
			   // brightness=((DWORD)(brightness)*(MidDutyValue-MinDutyValue))/DefBrightness+MinDutyValue;
			   
			   brightness=((DWORD)(brightness)*(MidDutyValue-MinDutyValue))/50+MinDutyValue;
	#if Enable_DIGITAL_DIMMING
				//ICE_BRIGHTNESS_RGBCMY(0x40);
				brightness=64-brightness;
				brightness=brightness|0x40;
			 ICE_BRIGHTNESS_RGBCMY(brightness); 
	
	#endif
	
			}
	
		BacklightNow = BacklightPrev = brightness;
	
#if ENABLE_LED_CONTROLLER
		msLED_DimmingPWMAdjust(brightness);
#else
    #if BRIGHTNESS_INVERSE  //+Duty power
		drvGPIO_SetBacklightDuty(BrightnessPWM, 0xFF-brightness);
		//drvGPIO_SetBacklightDuty(BrightnessPWM, 0);
    #else                           //-Duty power
		//drvGPIO_SetBacklightDuty(BrightnessPWM, 0xff);
		drvGPIO_SetBacklightDuty(BrightnessPWM, brightness);
		
    #endif
#endif
#if ENABLE_POWER_PORTECT
		PowerProtectInit();
#endif
}

//=================================================================================
//volume=((DWORD)volume*63)/100+57;    //LM929 2W for Lai
//volume=((DWORD)volume*84)/100+93;    //LM929 3W && Carrefour for Lai
//volume=((DWORD)volume*92)/100+93;    //-0923-LM929 3W && Carrefour for Lai
//volume=((DWORD)volume*170)/100+57;   //LM729 && Carrefour for Lu
//volume = ( ( DWORD ) volume * 70) / 20 +90; //qx.long20061213
//volume = ( ( DWORD ) volume * 34) / 100 +160; //qx.long20061213
// volume=((DWORD)volume*146)/100+51;   //1127 for Lai TESTING 19" AUDIO
//=================================================================================
#if AudioFunc

#ifndef DACMinVolumeValue
#define DACMinVolumeValue       0
#endif
#ifndef DACMaxVolumeValue
#define DACMaxVolumeValue       0xC0//255
#endif
#ifndef DACVolume50Value
#define DACVolume50Value            ((DACMaxVolumeValue-DACMinVolumeValue)*50/100+DACMinVolumeValue)
#endif


#ifndef _CUSTOMER_VOLUME_TBL
#define _CUSTOMER_VOLUME_TBL           { 0,      0x390, \
									   1,     0x16c,\
									  66,    0x68,\
									 100,  	0x3d,\
										}

#endif


WORD code U32VolumeCurv[]= _CUSTOMER_VOLUME_TBL;

//==============
void mStar_AdjustVolume(BYTE volume)
{
	int tempvolume;
	WORD u8Num=0;
   	u8Num= sizeof(U32VolumeCurv)/sizeof(WORD);

#if AudioIC_CS8003_Enable


#if EarphoneDet_Enable	
     BYTE tempearVolume;
#endif

	if (volume > MaxVolumeValue)
		volume = MaxVolumeValue;
	else if (volume < MinVolumeValue)
		volume = MinVolumeValue;

         if(volume==0)
        {
            hw_SetMute();
            Delay1ms(2);
         }
		 
//--------------------------------------------------------------------------------------------
#if EarphoneDet_Enable	
	if (HarphoneDet_Pin)
	{
		if ( volume )
		{
		       tempearVolume=volume;

	            if(volume<50)
		          tempearVolume=((DWORD)volume*(DACVolume50Value-DACMinVolumeValue))/50+DACMinVolumeValue;
	            else
		          tempearVolume=((DWORD)(volume-50)*(DACMaxVolumeValue-DACVolume50Value))/(100-50)+DACVolume50Value;
				
			UserPrefEarVolume=tempearVolume;
			//msAudioDPGA_SetVolume ( E_AUDIO_LEFT_RIGHT_CH, 0xFF-UserPrefEarVolume);
		}
		else
		{
			msAudioDPGA_Mute();
		}
	}
#endif
//-------------------------------------------------------------------
tempvolume = 0;	
        if(volume)
        {		
                volume= UserAdjust_NonlinerSetCurve(volume,&U32VolumeCurv[0],u8Num);
        }
        else
        {
                 volume = 0;
        }

    #if VOLUME_INVERSE	//+Duty power
    	drvGPIO_SetBacklightDuty(VolumePWM, 0xFF-volume);
    #else				//-Duty power
    	drvGPIO_SetBacklightDuty(VolumePWM, volume);
    #endif

#if EarphoneDet_Enable	
     if((volume!=0)&&(!UserOSDMuteOnFlag)&&(!HarphoneDet_Pin)&&bPanelOnFlag)
     	{
		msWrite2ByteMask(  REG_2CA4, 0xFF76    , 0xFF7F);  //[15:8] DPGA_1_TIMEOUT_N; [6:4] DPGA_1_STEP=3'd7->N=1;  [2] DPGA_1_ZERO_DET_EN; [1] DPGA_1_FADING_EN;   [0] DPGA_1_DPGA_EN;
		hw_ClrMute();
     	}
     else
     	{
		msWrite2ByteMask(  REG_2CA4, 0xFF77    , 0xFF7F);  //[15:8] DPGA_1_TIMEOUT_N; [6:4] DPGA_1_STEP=3'd7->N=1;  [2] DPGA_1_ZERO_DET_EN; [1] DPGA_1_FADING_EN;   [0] DPGA_1_DPGA_EN;
		hw_SetMute(); 
     	}
#else	 
        if ((volume!=0)&&(!UserOSDMuteOnFlag))
        {
              hw_ClrMute();
        }
#endif



#else   //cs8563

	if (volume > MaxVolumeValue)
		volume = MaxVolumeValue;
	else if (volume < MinVolumeValue)
		volume = MinVolumeValue;

         if(volume==0)
        {
            hw_SetMute();
            Delay1ms(2);
         }
		 
//-------------------------------------------------------------------	
//tempvolume=AudVolumeTab[100 - volume];  
	tempvolume= UserAdjust_NonlinerSetCurve(volume,&U32VolumeCurv[0],u8Num);
#if  MainBoardType==MainBoard_TSUML88CMC2
	msAudioDPGA_SetVolume( E_AUDIO_LEFT_RIGHT_CH, tempvolume );
#endif
   if ((volume!=0)&&(!UserOSDMuteOnFlag))
        {
		#if EarphoneDet_Enable
		if(!HarphoneDet_Pin)
		#endif
			  hw_ClrMute();
        }
#endif
}
//====================================================

#if 0 //MS_DAC&&IIYAMASeries
#define DEFAULT_LINEOUT_GAIN          	0x27
#define DEFAULT_HDMI_LINEOUT_GAIN  		0x36
#define DEFAULT_DP_LINEOUT_GAIN    		0x3D

#define DEF_DP_DAC_GAIN       	(0)
#define DEF_LINEIN_DAC_GAIN   	(0)
#define DEF_HDMI_DAC_GAIN    	(0)

void ChangeAudioSourceToLineOut( void )
{
    msAudioLineout_SourceSel( E_LINEOUT_FROM_LINEIN0 );
    #if !USE_DAC_ADJ
    //msAudioDPGA_SetVolume( E_AUDIO_LEFT_RIGHT_CH, DEF_LINEIN_DAC_GAIN );	//111118 Modify
    msAudioLineOutGain( DEFAULT_LINEOUT_GAIN );
    #endif
}

#if ENABLE_HDMI || ENABLE_DP_INPUT
void ChangeAudioSourceToDigital( void )
{
#if ENABLE_HDMI
    if( CURRENT_INPUT_IS_TMDS()
		 && gScInfo.InputTmdsType == TMDS_HDMI 
		)
    {
        msAudioLineout_SourceSel( E_LINEOUT_FROM_DAC );
	#if !USE_DAC_ADJ
        //msAudioDPGA_SetVolume( E_AUDIO_LEFT_RIGHT_CH, DEF_HDMI_DAC_GAIN );	//111118 Modify
        msAudioLineOutGain( DEFAULT_HDMI_LINEOUT_GAIN );
    #endif
    }
#endif
#if ENABLE_DP_INPUT
    if(CURRENT_INPUT_IS_DISPLAYPORT())
    {
        msAudioLineout_SourceSel( E_LINEOUT_FROM_DAC );
	#if !USE_DAC_ADJ
       // msAudioDPGA_SetVolume( E_AUDIO_LEFT_RIGHT_CH, DEF_DP_DAC_GAIN );	//111118 Modify
        msAudioLineOutGain( DEFAULT_DP_LINEOUT_GAIN );
    #endif
    }
#endif
}
#endif
#endif

#endif 

void mStar_AdjustRGBColor(BYTE u8Contrast, BYTE u8Red, BYTE u8Green, BYTE u8Blue)
{
    if(IsColorspaceRGB()
#if ENABLE_DISPLAY_UNDERSCAN||CHIP_ID == CHIP_TSUMD
        && !g_SetupPathInfo.bMRWR2Y
#endif
        && (UserPrefInputColorFormat == INPUTCOLOR_RGB) //20150121
        )
    {
#if (CHIP_ID == CHIP_TSUMC)||(CHIP_ID==CHIP_TSUMK)||(CHIP_ID == CHIP_TSUMD)||(CHIP_ID == CHIP_TSUM9)||(CHIP_ID == CHIP_TSUMF)
        msWriteByteMask(SC0_02, 0, _BIT3);
#endif
        msAdjustPCContrastRGB(MAIN_WINDOW, u8Contrast, u8Red, u8Green, u8Blue);
    }
    else
    {
#if (CHIP_ID == CHIP_TSUMC)||(CHIP_ID==CHIP_TSUMK)||(CHIP_ID == CHIP_TSUMD)||(CHIP_ID == CHIP_TSUM9)||(CHIP_ID == CHIP_TSUMF)
        msWriteByteMask(SC0_02, _BIT3, _BIT3);
#endif
        msAdjustVideoContrastRGB(MAIN_WINDOW,u8Contrast, u8Red, u8Green, u8Blue);
        //printMsg("\r\n YUV");
    }
}

#define CONTRAST_MIN		 30
//2020-12-16 add jove 
#if ((Customer_Type_Sel    == Customer_CEHUA) &&  (PanelType == PanelCLAA170WA02))
#define CONTRAST_MID		 136
#elif ((Customer_Type_Sel    == Customer_CEHUA) &&  (PanelType == PanelM190A1L02_1))
#define CONTRAST_MID		 124
#else
#define CONTRAST_MID		 148  
#endif

#define CONTRAST_MAX		 200 //178 

BYTE mStar_ChangeAdjContrastValue(BYTE contrast)
{
	if (contrast > DefContrast)
		contrast = ((((WORD)(contrast-DefContrast) * (CONTRAST_MAX -CONTRAST_MID)/(100-DefContrast))) + CONTRAST_MID);//128~(Color+30)
	else
	       contrast= ((((WORD)contrast * (CONTRAST_MID - CONTRAST_MIN)) / DefContrast)+CONTRAST_MIN);//88~128

       return contrast;

}

void mStar_AdjustContrast( BYTE contrast )
{
    xdata ColorType CT;

    GetColorTempRGB(&CT);
    contrast = mStar_ChangeAdjContrastValue(contrast);
    mStar_AdjustRGBColor(contrast, CT.u8Red, CT.u8Green, CT.u8Blue);
}

void mStar_AdjustRedColor( BYTE color, BYTE contrast )
{
    xdata ColorType CT;

    GetColorTempRGB(&CT);
    contrast = mStar_ChangeAdjContrastValue(contrast);	
    mStar_AdjustRGBColor(contrast, color, CT.u8Green, CT.u8Blue);
}
void mStar_AdjustGreenColor( BYTE color, BYTE contrast )
{
    xdata ColorType CT;

    GetColorTempRGB(&CT);
    contrast = mStar_ChangeAdjContrastValue(contrast);
    mStar_AdjustRGBColor(contrast, CT.u8Red, color, CT.u8Blue);
}
void mStar_AdjustBlueColor( BYTE color, BYTE contrast )
{
    xdata ColorType CT;

    GetColorTempRGB(&CT);
    contrast = mStar_ChangeAdjContrastValue(contrast);
    mStar_AdjustRGBColor(contrast, CT.u8Red, CT.u8Green, color);
}

void mStar_AdjustHPosition( WORD position )
{
    SC0_SET_IMAGE_START_H(position);//msWrite2Byte(SC0_07, position);
}

void mStar_AdjustVPosition( WORD position )
{
#if 1
#if DECREASE_V_SCALING
    position = (position>VSTART_OFFSET)?(position-VSTART_OFFSET):(0);
#endif
    SC0_SET_IMAGE_START_V(position);//msWrite2Byte( SC0_05, position );
#else
    if( DecVScaleFlag )
    {
        if( position <= ( VScaleDecValue / 2 ) )
            position = VScaleDecValue / 2 + 1;
        msWrite2Byte( SC0_05, position - ( VScaleDecValue / 2 ) );
    }
    else
    {
        if( position == 0 )
            position = 1;
        msWrite2Byte( SC0_05, position );
    }
#endif
	#if CHIP_ID==CHIP_TSUMD
    if((g_SetupPathInfo.bFBMode)&& msRead2Byte(REG_3818)&BIT3)
		mStar_SetIVS();
	#endif

}

// 100702 coding, modify to wait complete blanking
void mStar_WaitForDataBlanking( void )
{
    WORD retry = 0xffff;

    if( SyncLossState() || !bPanelOnFlag )
        return;

    while( retry-- && ( msReadByte(SC0_90)&BIT7 ) && !InputTimingChangeFlag );

    retry = 0xffff;

    while( retry-- && !( msReadByte(SC0_90)&BIT7 ) && !InputTimingChangeFlag );
}

void mStar_AdjustBackgoundColor( BYTE pattern )
{
    BYTE rColor = 0;
    BYTE gColor = 0;
    BYTE bColor = 0;

#if Customer_Type_Sel==Customer_GUANRONG 
     if(pattern == 6)
     	{
		 gColor = 0x78;
		 bColor = 0xD7;
		 mStar_WaitForDataBlanking();
		 drvOSD_FrameColorRGB(rColor, gColor, bColor);
		// hw_ClrGreenLed();
		// ForceDelay1ms( 1000 );
		// hw_SetGreenLed();
		// ForceDelay1ms( 1000 );
		// hw_ClrGreenLed();
		// ForceDelay1ms( 1000 );
		// hw_SetGreenLed();
		// ForceDelay1ms( 1000 );
		// hw_ClrGreenLed();
		// ForceDelay1ms( 1000 );
		 //hw_SetGreenLed();
		 //ForceDelay1ms( 1000 );
		// hw_SetGreenLed();
		// ForceDelay1ms( 10000 );
      
     	}	 
	  else if( pattern == 1 )
	 {
	 
	 rColor = 0xFF;
	 gColor = 0xFF;
	 bColor = 0xFF;
		
	 }
    //else if( pattern == 2 ) //
    else if( pattern == 3 )
    {
        rColor = 0xFF;
    }
	   else if( pattern ==4 )
    {
        gColor = 0xFF;
    }
 	   else if( pattern ==5 )
    {
        bColor = 0xFF;
    }         
#else
#if Enable_LED
	hw_ClrAmberLed();
#else
	hw_SetGreenLed();
#endif	
    if( pattern == 0 )
    {
        rColor = 0xFF;
		hw_ClrGreenLed();
		
    }
    else if( pattern == 1 )
    {
        gColor = 0xFF;
#if Enable_LED
	hw_ClrAmberLed();
#else
	hw_SetGreenLed();
#endif	
    }
    else if( pattern == 2 )
    {
        bColor = 0xFF;
		hw_ClrGreenLed();
		
    }
    else if( pattern == 4 )
    {
        rColor = 0xFF;
        gColor = 0xFF;
        bColor = 0xFF;
		hw_ClrGreenLed();
    }
#endif
	
    mStar_WaitForDataBlanking();
    drvOSD_FrameColorRGB(rColor, gColor, bColor);
#if Customer_Type_Sel==Customer_GUANRONG 	
	//ForceDelay1ms( 3000 );
#endif
}
