#include "types.h"
#include "global.h"
#include "board.h"
#include "keypaddef.h"
#include "menudef.h"
#include "menutext.h"

//======================================================================




//vs ccflag  add
#define Icon4Color_AddrShift   2
#define Icon8Color_AddrShift   3

BYTE code strBrightnessIcon[7]=
{
  	FONT_8COLOR_ICON_START+0*Icon8Color_AddrShift,
  	FONT_8COLOR_ICON_START+1*Icon8Color_AddrShift,
  	FONT_8COLOR_ICON_START+2*Icon8Color_AddrShift,
  	FONT_8COLOR_ICON_START+3*Icon8Color_AddrShift,
  	FONT_8COLOR_ICON_START+4*Icon8Color_AddrShift,
  	FONT_8COLOR_ICON_START+5*Icon8Color_AddrShift,
 	 '\0'	
};	
BYTE code strImageIcon[7]=
{
  	FONT_4COLOR_ICON_START+0*Icon4Color_AddrShift,
  	FONT_4COLOR_ICON_START+1*Icon4Color_AddrShift,
  	FONT_4COLOR_ICON_START+2*Icon4Color_AddrShift,
  	FONT_4COLOR_ICON_START+3*Icon4Color_AddrShift,
  	FONT_4COLOR_ICON_START+4*Icon4Color_AddrShift,
  	FONT_4COLOR_ICON_START+5*Icon4Color_AddrShift,
 	 '\0'	
};

BYTE code strColorTempIcon[7]=
{
  	FONT_8COLOR_ICON_START+6*Icon8Color_AddrShift,
  	FONT_8COLOR_ICON_START+7*Icon8Color_AddrShift,
  	FONT_8COLOR_ICON_START+8*Icon8Color_AddrShift,
  	FONT_8COLOR_ICON_START+9*Icon8Color_AddrShift,
  	FONT_8COLOR_ICON_START+10*Icon8Color_AddrShift,
  	FONT_8COLOR_ICON_START+11*Icon8Color_AddrShift,
 	 '\0'	
};	

BYTE code strOSDIcon[7]=
{
  	FONT_8COLOR_ICON_START+12*Icon8Color_AddrShift,
  	FONT_8COLOR_ICON_START+13*Icon8Color_AddrShift,
  	FONT_8COLOR_ICON_START+14*Icon8Color_AddrShift,
  	FONT_8COLOR_ICON_START+15*Icon8Color_AddrShift,
  	FONT_8COLOR_ICON_START+16*Icon8Color_AddrShift,
  	FONT_8COLOR_ICON_START+17*Icon8Color_AddrShift,
 	 '\0'	
};

BYTE code strResetIcon[7]=
{
  	FONT_4COLOR_ICON_START+6*Icon4Color_AddrShift,
  	FONT_4COLOR_ICON_START+7*Icon4Color_AddrShift,
  	FONT_4COLOR_ICON_START+8*Icon4Color_AddrShift,
  	FONT_4COLOR_ICON_START+9*Icon4Color_AddrShift,
  	FONT_4COLOR_ICON_START+10*Icon4Color_AddrShift,
  	FONT_4COLOR_ICON_START+11*Icon4Color_AddrShift,
 	 '\0'	
};	

BYTE code strMiscIcon[7]=
{
  	FONT_4COLOR_ICON_START+12*Icon4Color_AddrShift,
  	FONT_4COLOR_ICON_START+13*Icon4Color_AddrShift,
  	FONT_4COLOR_ICON_START+14*Icon4Color_AddrShift,
  	FONT_4COLOR_ICON_START+15*Icon4Color_AddrShift,
  	FONT_4COLOR_ICON_START+16*Icon4Color_AddrShift,
  	FONT_4COLOR_ICON_START+17*Icon4Color_AddrShift,
 	 '\0'	
};
#if (EN_HotKeyECOMenuWithIcon||EN_HotKeyDCRMenuWithIcon)
BYTE code strHotKeyIcon[13]=
{
	FONT_4COLOR_ICON_START+0*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+1*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+2*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+3*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+4*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+5*Icon4Color_AddrShift,	 
	FONT_4COLOR_ICON_START+6*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+7*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+8*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+9*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+10*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+11*Icon4Color_AddrShift,
       '\0'	
};	
#endif

#if 0//EN_HotKeyXINGZHUN
BYTE code strXZIcon[17]=
{
	FONT_XZ4COLOR_ICON_START+0*Icon4Color_AddrShift,
	FONT_XZ4COLOR_ICON_START+1*Icon4Color_AddrShift,
	FONT_XZ4COLOR_ICON_START+2*Icon4Color_AddrShift,
	FONT_XZ4COLOR_ICON_START+3*Icon4Color_AddrShift,
	FONT_XZ4COLOR_ICON_START+4*Icon4Color_AddrShift,
	FONT_XZ4COLOR_ICON_START+5*Icon4Color_AddrShift,	 
	FONT_XZ4COLOR_ICON_START+6*Icon4Color_AddrShift,
	FONT_XZ4COLOR_ICON_START+7*Icon4Color_AddrShift,
	FONT_XZ4COLOR_ICON_START+8*Icon4Color_AddrShift,
	FONT_XZ4COLOR_ICON_START+9*Icon4Color_AddrShift,
	FONT_XZ4COLOR_ICON_START+10*Icon4Color_AddrShift,
	FONT_XZ4COLOR_ICON_START+11*Icon4Color_AddrShift,
	FONT_XZ4COLOR_ICON_START+12*Icon4Color_AddrShift,
	FONT_XZ4COLOR_ICON_START+13*Icon4Color_AddrShift,
	FONT_XZ4COLOR_ICON_START+14*Icon4Color_AddrShift,
	FONT_XZ4COLOR_ICON_START+15*Icon4Color_AddrShift,	 
       '\0'	
};	

BYTE code strJSIcon[17]=
{
	FONT_4COLOR_ICON_START+16*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+17*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+18*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+19*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+20*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+21*Icon4Color_AddrShift,	 
	FONT_4COLOR_ICON_START+22*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+23*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+24*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+25*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+26*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+27*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+28*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+29*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+30*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+31*Icon4Color_AddrShift,	 
       '\0'	
};	


BYTE code strXZ_XIcon[37]=
{
	FONT_XZ4COLOR_ICON_START+0*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+1*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+2*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+3*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+4*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+5*Icon4Color_AddrShift,	 
	FONT_4COLOR_ICON_START+6*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+7*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+8*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+9*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+10*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+11*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+12*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+13*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+14*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+15*Icon4Color_AddrShift,	 
	FONT_4COLOR_ICON_START+16*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+17*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+18*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+19*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+20*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+21*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+22*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+23*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+24*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+25*Icon4Color_AddrShift,	 
	FONT_4COLOR_ICON_START+26*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+27*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+28*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+29*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+30*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+31*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+32*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+33*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+34*Icon4Color_AddrShift,
	FONT_4COLOR_ICON_START+35*Icon4Color_AddrShift,	 
       '\0'	
};





#endif

//-----------------------------------------------------------
BYTE* BrightnessIcon(void)
{ return strBrightnessIcon;
}
BYTE* ImageIcon(void)
{ return strImageIcon;
}
BYTE* ColorTempIcon(void)
{ return strColorTempIcon;
}
BYTE* OSDIcon(void)
{ return strOSDIcon;
}
BYTE* ResetIcon(void)
{ return strResetIcon;
}
BYTE* MiscIcon(void)
{return strMiscIcon;
}
//-----------
#if Los_Select

BYTE code strCorssIcon[16]=
{
	MonoFontStart2+0,
	MonoFontStart2+1,
	MonoFontStart2+2,
	MonoFontStart2+3,
	MonoFontStart2+4,	 
	
	MonoFontStart2+5,
	MonoFontStart2+6,
	MonoFontStart2+7,
	MonoFontStart2+8,
	MonoFontStart2+9,
	
	MonoFontStart2+10,
	MonoFontStart2+11,
	MonoFontStart2+12,
	MonoFontStart2+13,
	MonoFontStart2+14,
       '\0'	
};	

BYTE* CorssIcon(void)
{return strCorssIcon;
}
#endif
//--------------------------------------
BYTE* MainMenu_BrightText(void)	
{ 
  return strMainMenu_BrightM7[UserPrefLanguage];
}
BYTE* MainMenu_ImageText(void)	
{ 
  return strMainMenu_ImageM7[UserPrefLanguage];
}
BYTE* MainMenu_ColorText(void)	
{ 
  return strMainMenu_ColorTempM7[UserPrefLanguage];
}

BYTE* MainMenu_OSDText(void)	
{ 
  return strMainMenu_OSDM7[UserPrefLanguage];
}

BYTE* MainMenu_ResetText(void)	
{ 
  return strMainMenu_ResetM7[UserPrefLanguage];
}
BYTE* MainMenu_MiscText(void)	
{ 
  return strMainMenu_MiscM7[UserPrefLanguage];
}
//------------
BYTE* BrtCon_BrightnessText(void)	
{ 
  return strBrtCon_BrightnessM8[UserPrefLanguage];
}
BYTE* BrtCon_ContrastText(void)	
{ 
  return strBrtCon_ContrastM8[UserPrefLanguage];
}
#if 1 //MWE_AP_Enable
BYTE* BrtCon_ECOText(void)	
{ 
  return strBrtCon_ECOM8[UserPrefLanguage];
}
//ECO Mode
BYTE* ECOModeText(void)
{ 
 if( UserPrefECOMode == ECO_Standard )
    {
        return strStandardM1[UserPrefLanguage];
    }
    else if( UserPrefECOMode == ECO_Game )
    {
        return strGameM1[UserPrefLanguage];
    }
    else if( UserPrefECOMode == ECO_Movie)
    {
        return strMovieM1[UserPrefLanguage];
    }
    else if( UserPrefECOMode == ECO_Text)
    {
        return strTextM1[UserPrefLanguage];
    }
    else
    {
        return strStandardM1[UserPrefLanguage];
    }
}
#endif
#if Enable_AdjustDCR
BYTE* BrtCon_DCRText(void)	
{ 
  return strBrtCon_DCRM8[UserPrefLanguage];
}
BYTE* DCRModeText(void)
{ // return strDCRMode[UserPrefDCRModeIndex%2][UserPrefLanguage];
	if (UserPrefDcrMode)
		return strDCR_OnMode[UserPrefLanguage];
	else
		return strDCR_OffMode[UserPrefLanguage];
}
#endif

#if OSD_OD
BYTE* BrtCon_ODText(void)	
{ 
  return strBrtCon_ODM8[UserPrefLanguage];
}
BYTE* ODModeText(void)
{ 
	if (UserprefOverDriveSwitch)
		return strOD_OnMode[UserPrefLanguage];
	else
		return strOD_OffMode[UserPrefLanguage];
}
#endif

#if LowBlueLight
BYTE* BlueLightText(void)
{
   return strBlueLightM11[UserPrefLanguage];
}

BYTE* BlueLightModeText(void)
{
	if(UserPrefLowBlueMode==BlueLight_Browsing)
		 return strBlueLight_BrowsingM11[UserPrefLanguage];
	else if (UserPrefLowBlueMode==BlueLight_Read)
		return strBlueLight_ReadM11[UserPrefLanguage];
	else if(UserPrefLowBlueMode==BlueLight_Video)
		return strBlueLight_VideoM11[UserPrefLanguage];
	else if (UserPrefLowBlueMode==BlueLight_lowlight)
		return strBlueLight_lowlightM11[UserPrefLanguage];
	else  //BlueLight_Off
		return strBlueLight_Off[UserPrefLanguage];
/*
	if (UserPrefLowBlueMode==BlueLight_Shield)
	   return strBlueLight_ShieldM11[UserPrefLanguage];
	else if (UserPrefLowBlueMode==BlueLight_Middle)
	   return strBlueLight_MiddleM11[UserPrefLanguage];
	else
	   return strBlueLight_StandardM11[UserPrefLanguage];
*/	   
}
#endif

//--------------
BYTE* HPositionText(void)
{ return strImage_HPositionM9[UserPrefLanguage];
}
BYTE* VPositionText(void)
{ return strImage_VPositionM9[UserPrefLanguage];
}
BYTE* ClockText(void)	
{ 
  return strImage_ClockM9[UserPrefLanguage];
}
BYTE* PhaseText(void)	
{ 
  return strImage_PhaseM9[UserPrefLanguage];
}
#if  Enable_Expansion
BYTE* AspectText(void)	
{ 
  return strImage_AspectM9[UserPrefLanguage];
}

BYTE* AspectMode_SelectText(void)	
{ 
	if(UserprefExpansionMode==Expansion_4_3)
  		return strAspectMode_4To3[UserPrefLanguage];
	else //if  (UserprefExpansionMode==Expansion_Full)
  		return strAspectMode_Wide[UserPrefLanguage];
}
#endif
//---------------------------------------
BYTE *CTemp_TempText(void)
{ 
	return strColor_CTempM10[UserPrefLanguage];
}
BYTE *CTemp_RedText(void)	
{ 
	return strColor_RedM10[UserPrefLanguage];
}

BYTE *CTemp_GreenText(void)	
{ 
	return strColor_GreenM10[UserPrefLanguage];
}

BYTE *CTemp_BlueText(void)	
{ 
	return strColor_BlueM10[UserPrefLanguage];
}
BYTE *ColorTempSelectText(void)	
{ 
	if (UserPrefColorTemp==CTEMP_USER)
		return strColorTemp_User[UserPrefLanguage];


	#if FactoryAuto_5400K
	if (UserPrefColorTemp==CTEMP_5400K)
		return strColorTemp_5400k[UserPrefLanguage];
	#endif

	#if FactoryAuto_6500K
	if (UserPrefColorTemp==CTEMP_6500K)
		return strColorTemp_6500k[UserPrefLanguage];
	#endif
	
	#if  FactoryAuto_9300K
	if (UserPrefColorTemp==CTEMP_9300K)
		return strColorTemp_9300k[UserPrefLanguage];
	#endif
	
		return strColorTemp_User[UserPrefLanguage];
}

//---------------------------------------
BYTE *OSD_LanguageText(void)
{ 
	return strOSD_LanguageM11[UserPrefLanguage];
}

BYTE* OSDHPositionText(void)
{
   return strOSD_HPosM11[UserPrefLanguage];
}
BYTE* OSDVPositionText(void)
{
    return strOSD_VPosM11[UserPrefLanguage];
}
BYTE* OSDHoldTimeText(void)
{
    return strOSD_TimerM11[UserPrefLanguage];
}

BYTE *OSD_TranslucenceText(void)
{ 
	return strOSD_TransM11[UserPrefLanguage];
}
//-------------------------------
//Language
#if Language_English
BYTE *Language_EnglishText(void)
{ 
	return strLanguage[0];
}
#endif
#if Language_France
BYTE *Language_FranceText(void)
{ 
	return strLanguage[1];
}
#endif
#if Language_German
BYTE *Language_GermanText(void)
{ 
	return strLanguage[2];
}
#endif
#if Language_SimChinese
BYTE *Language_SimChineseText(void)
{ 
	return strLanguage[3];
}
#endif
#if Language_Italian
BYTE *Language_ItalianText(void)
{ 
	return strLanguage[4];
}
#endif
#if Language_Spanish
BYTE *Language_SpanishText(void)
{ 
	return strLanguage[5];
}
#endif
#if Language_Portuguese
BYTE *Language_PortugueseText(void)
{ 
	return strLanguage[6];
}
#endif
#if Language_Turkish
BYTE *Language_TurkishText(void)
{ 
	return strLanguage[7];
}
#endif
#if Language_Polish
BYTE *Language_PolishText(void)
{ 
	return strLanguage[8];
}
#endif
#if Language_Dutch
BYTE *Language_DutchText(void)
{ 
	return strLanguage[9];
}
#endif
#if Language_Russian
BYTE *Language_RussianText(void)
{ 
	return strLanguage[10];
}
#endif

#if Language_Korean
BYTE *Language_KoreanText(void)
{ 
	return strLanguage[11];
}
#endif

#if Language_TraChinese
BYTE *Language_TraChineseText(void)
{ 
	return strLanguage[12];
}
#endif

//--------------------------------
BYTE *RST_AutoAdjText(void)
{
	return strRst_AutoAdjM12[UserPrefLanguage];
}
#if _AUTOCOLOR_IN_SETTING_MENU_ENABLE
BYTE *RST_ColorAdjText(void)
{
	return strRst_ColorAdjM12[UserPrefLanguage];
}
#endif
BYTE *RST_ResetText(void)
{
	return strRst_ResetM12[UserPrefLanguage];
}
//---------------------------------------
#if OSD_INPUT_SUBMENU_ENABLE
BYTE *Misc_InputSourceText(void)
{
	return strMisc_InputSourceM13[UserPrefLanguage];
}
#endif
BYTE *Misc_SourceSelectext(void)
{
	if(UserPrefSelectInputType==Input_DVI)
		return strInputType_DVI[UserPrefLanguage];
	else if(UserPrefSelectInputType==Input_HDMI)
		return strInputType_HDMI[UserPrefLanguage];
	else if(UserPrefSelectInputType==Input_Displayport)
		return strInputType_DP[UserPrefLanguage];
	else
		return strInputType_VGA[UserPrefLanguage];	
}
#if AudioFunc
BYTE *Misc_MuteText(void)
{
	return strMisc_MuteM13[UserPrefLanguage];
}

BYTE *Misc_MuteOnOfftext(void)
{
	if(UserOSDMuteOnFlag)
		return strMute_OnMode[UserPrefLanguage];
	else
		return strMute_OffMode[UserPrefLanguage];
}
BYTE *Misc_VolumeText(void)
{
	return strMisc_VolumeM13[UserPrefLanguage];
}
#if Audio_Switch
BYTE *HDMIAUDIOText(void)
{
	return strHDMIAUDIOText[UserPrefLanguage];
}

BYTE *HDMIAudioValueText(void)
{
	if((SrcInputType==Input_Analog1)||(SrcInputType==Input_DVI))
	{
		return strLineIn[UserPrefLanguage];
	}
	else
	{
		if (UserPrefHDMIAudioType)
		{
			return strAudio_HDMI[UserPrefLanguage];
		}
		else
		{
			return strLineIn[UserPrefLanguage];
		}
	}
}

#endif

#endif
//==============================
#if EN_HotKeyDCRMenuWithIcon

//HotKeyMenu Icon
BYTE* HotKeyDCROnIcon(void)
{return strHotKeyIcon;
}
BYTE* HotKeyDCROffIcon(void)
{return strHotKeyIcon;
}
#endif

#if EN_HotKeyECOMenuWithIcon
BYTE* HotKeyECOStandardIcon(void)
{return strHotKeyIcon;
}
BYTE* HotKeyECOGameIcon(void)
{return strHotKeyIcon;
}
BYTE* HotKeyECOWebIcon(void)
{return strHotKeyIcon;
}
BYTE* HotKeyECOMoveIcon(void)
{return strHotKeyIcon;
}
#endif

#if 0//EN_HotKeyXINGZHUN
BYTE* HotKeyXZIcon(void)
{return strXZIcon;
}
BYTE* HotKeyJSIcon(void)
{return strJSIcon;
}

BYTE* HotKeyXZ_XIcon(void)
{return strXZ_XIcon;
}

#endif
BYTE* HotKeyContrastText(void)
{ return strHotKeyContrast[UserPrefLanguage];
}
BYTE* HotKeyBrightnessText(void)
{ return strHotKeyBrightness[UserPrefLanguage];
}

#if KeyInputSelectMenu
#if(InputType_Analog1)
BYTE* HotKeyInputVGAText(void)
{ 
   return strHotKeyInputType_VGA[UserPrefLanguage];		
}
#endif
#if InputType_Digital
BYTE* HotKeyInputDVIText(void)
{ 
   return strHotKeyInputType_DVI[UserPrefLanguage];		
}
#endif
#if InputType_HDMI
BYTE* HotKeyInputHDMIText(void)
{ 
   return strHotKeyInputType_HDMI[UserPrefLanguage];		
}
#endif
#if ENABLE_DP_INPUT
BYTE* HotKeyInputDPText(void)
{ 
   return strHotKeyInputType_DP[UserPrefLanguage];		
}
#endif
#endif

#if AudioFunc
BYTE* HotKeyVolumeText(void)
{return strMisc_VolumeM13[UserPrefLanguage];
}
#endif

#if EN_HotKeyTimes

BYTE* HotKeyTimesText(void)
{ 
   return strHotKeyTimes[UserPrefLanguage];		
}

BYTE* HotKeyTimes10Text(void)
{ 
   return strHotKeyTimes_10[UserPrefLanguage];		
}

BYTE* HotKeyTimes20Text(void)
{ 
   return strHotKeyTimes_20[UserPrefLanguage];		
}
BYTE* HotKeyTimes30Text(void)
{ 
   return strHotKeyTimes_30[UserPrefLanguage];		
}
BYTE* HotKeyTimes40Text(void)
{ 
   return strHotKeyTimes_40[UserPrefLanguage];		
}

BYTE* HotKeyTimes50Text(void)
{ 
   return strHotKeyTimes_50[UserPrefLanguage];		
}
BYTE* HotKeyTimes60Text(void)
{ 
   return strHotKeyTimes_60[UserPrefLanguage];		
}
BYTE* HotKeyTimes90Text(void)
{ 
   return strHotKeyTimes_90[UserPrefLanguage];		
}

BYTE* HotKeyShowTimesText(void)
{ 
   return strHotKeyShowTimes[UserPrefLanguage];		
}
#endif

#if EN_HotKeyXINGZHUN
BYTE* HotKeyXINGZHUNText(void)
{ 
   return strXINGZHUN[UserPrefLanguage];		
}
BYTE* HotKeyXZCJText(void)
{ 
   return strHotKeycj[UserPrefLanguage];		
}



#endif

//---------------------------------------

BYTE* ProcessAutoText(void)	
{ 
	return strRst_AutoAdjM12[UserPrefLanguage];
}

#if Enable_Menu_LockOSD
BYTE* OsdLockedText(void)	
{ 	
  	return strOsdLockedText[UserPrefLanguage];
}
BYTE* OsdUnLockedText(void)	
{ 	
  	return strOsdUnLockedText[UserPrefLanguage];
}
#endif
#if Enable_LockPowerKey
BYTE* PowerLockedText(void)	
{ 	
  	return strPowerLockedText[UserPrefLanguage];
}
BYTE* PowerUnLockedText(void)	
{ 	
  	return strPowerUnLockedText[UserPrefLanguage];
}
#endif

#if ShowSourceType
BYTE* ShowInputTypeText(void)
{ 
return strShowInputType[UserPrefSelectInputType][UserPrefLanguage];
}
#endif

BYTE* NoSignalText(void)
{ return strNoSignal[UserPrefLanguage];
}
BYTE* OutOfRangeText(void)
{ return strOutOfRange[UserPrefLanguage];
}

//-------------------------------------------
BYTE *FactoryAutoColorText(void)
{
    return strFactoryAutoColor;
}
BYTE* FactoryPassText(void)
{ return strFactoryPass;
}
BYTE* FactoryFailText(void)
{ return strFactoryFail;
}

BYTE *FactoryBrightnessText(void)
{
    return strFactoryBrightness;
}
BYTE *FactoryContrastText(void)
{
    return strFactoryContrast;
}
BYTE *FactoryAdcGainText(void)
{
    return strFactoryAdcGain;
}
BYTE *FactoryAdcOffsetText(void)
{
    return strFactoryAdcOffset;
}
#if FactoryAuto_5400K
BYTE *Factory5400KText(void)
{
    return strFactory5400K;
}
#endif
#if FactoryAuto_6500K
BYTE *Factory6500kText(void)
{
    return strFactory6500k;
}
#endif
#if FactoryAuto_9300K
BYTE *Factory9300kText(void)
{
    return strFactory9300k;
}
#endif

				
BYTE *ExitText (void)
{
	return strExitText;
}
BYTE *SelectText (void)
{
	return strSelectText;
}

BYTE *ExitText1 (void)
{
	return strExit1Text;
}
BYTE *SelectText1 (void)
{
	return strSelect1Text;
}

BYTE *FactorResetText (void)
{
	return strFactoryResetText;
}
BYTE *BurninText(void)
{
    return strBurnin;
}
BYTE *BurninOnOffText(void)
{
    return (BurninModeFlag) ? (strOnOff[1]): (strOnOff[0]);
}
#if ENABLE_FACTORY_SSCADJ	//120305 Modify
BYTE *SpreadSpectrumText( void )
{
    return strSpreadSpectrum;
}
BYTE *FreqText( void )
{
    return strFreq;
}
BYTE *AmpText( void )
{
    return strAmp;
}
#endif
#if DisplayLogo!=NoBrand	//130613 Jun
BYTE *LogoText(void)
{
    return strLogo;
}
BYTE *LogoOnOffText(void)
{
    return (UserPrefLogoON) ? (strOnOff[1]): (strOnOff[0]);
}
#endif

#if DDCCI_ENABLE
BYTE *DDCCIText(void)
{
    return strDDCCI;
}
BYTE *DDCCIOnOffText(void)
{
    return (DDCciFlag) ? (strOnOff[1]): (strOnOff[0]);
}
#endif

#if !USEFLASH // 130829 larry
BYTE *EraseEEPROMText(void)
{
    return strEraseEEPROM;
}
#endif
BYTE *FactoryExitText(void)
{
	return strFactoryExitText;
}

//--------
BYTE *FactoryRedText(void)
{
    return strFactoryRed;
}
BYTE *FactoryGreenText(void)
{
    return strFactoryGreen;
}
BYTE *FactoryBlueText(void)
{
    return strFactoryBlue;
}
//----------------------------
BYTE *VersionDateText(void)
{
    return strVersionDate;
}
BYTE *ChangeDateText(void)
{
    return strChangeDate;
}
BYTE *VerText(void)
{
	return strVerText;
}

BYTE *PanelText(void)
{
    return strPanel;
}


BYTE *ChecksumText(void)
{
	return strChecksumText;
}
BYTE *TIMEText(void)
{
    return strPanelOn;
}

BYTE *Time_HText(void)
{
	return strTime_HText;
}

BYTE *Time_MText(void)
{
	return strTime_MText;
}

BYTE *Time_SText(void)
{
	return strTime_SText;
}

#if ENABLE_BURNIN_MENU
BYTE *EDIDText(void)
{
	return strEDID_SText;
}

BYTE *PanelWH_Text(void)
{
	return strPanelWH_SText;
}



#endif

