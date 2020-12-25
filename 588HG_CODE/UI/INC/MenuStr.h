

//===================================================================
extern BYTE* BrightnessIcon(void);
extern BYTE* ImageIcon(void);
extern BYTE* ColorTempIcon(void);
extern BYTE* OSDIcon(void);
extern BYTE* ResetIcon(void);
extern BYTE* MiscIcon(void);
#if Los_Select
extern BYTE* CorssIcon(void);
#endif
//----------------
extern BYTE* MainMenu_BrightText(void);	
extern BYTE* MainMenu_ImageText(void);	
extern BYTE* MainMenu_ColorText(void);	
extern BYTE* MainMenu_OSDText(void);	
extern BYTE* MainMenu_ResetText(void);	
extern BYTE* MainMenu_MiscText(void);	
//----------------
extern BYTE* BrtCon_BrightnessText(void);
extern BYTE* BrtCon_ContrastText(void);
#if 1 //MWE_AP_Enable
extern BYTE* BrtCon_ECOText(void);
extern BYTE* ECOModeText(void);
#endif
#if Enable_AdjustDCR
extern BYTE* BrtCon_DCRText(void);
extern BYTE* DCRModeText(void);
#endif
#if OSD_OD
extern BYTE* BrtCon_ODText(void);
extern BYTE* ODModeText(void);
#endif
#if LowBlueLight
extern BYTE* BlueLightText(void);
extern BYTE* BlueLightModeText(void);
#endif
//----------------
extern BYTE* HPositionText(void);
extern BYTE* VPositionText(void);
extern BYTE* ClockText(void);
extern BYTE* PhaseText(void);
#if Enable_Expansion
extern BYTE* AspectText(void);
extern BYTE* AspectMode_SelectText(void);
#endif
//-----------------------------
extern BYTE* CTemp_TempText(void);
extern BYTE* CTemp_RedText(void);
extern BYTE* CTemp_GreenText(void);
extern BYTE* CTemp_BlueText(void);
extern BYTE* ColorTempSelectText(void);
//-----------------------------
extern BYTE* OSD_LanguageText(void);
extern BYTE* OSDHPositionText(void);
extern BYTE* OSDVPositionText(void);
extern BYTE* OSDHoldTimeText(void);
extern BYTE* OSD_TranslucenceText(void);
//------------------------------
#if Language_English
extern BYTE *Language_EnglishText(void);
#endif
#if Language_France
extern BYTE *Language_FranceText(void);
#endif
#if Language_German
extern BYTE *Language_GermanText(void);
#endif
#if Language_SimChinese
extern BYTE *Language_SimChineseText(void);
#endif
#if Language_Italian
extern BYTE *Language_ItalianText(void);
#endif
#if Language_Spanish
extern BYTE *Language_SpanishText(void);
#endif
#if Language_Portuguese
extern BYTE *Language_PortugueseText(void);
#endif
#if Language_Turkish
extern BYTE *Language_TurkishText(void);
#endif
#if Language_Polish
extern BYTE *Language_PolishText(void);
#endif
#if Language_Dutch
extern BYTE *Language_DutchText(void);
#endif
#if Language_TraChinese
extern BYTE *Language_TraChineseText(void);
#endif
#if Language_Russian
extern BYTE *Language_RussianText(void);
#endif
#if Language_Korean
extern BYTE *Language_KoreanText(void);
#endif
//-----------------------------
extern BYTE* RST_AutoAdjText(void);
#if _AUTOCOLOR_IN_SETTING_MENU_ENABLE
extern BYTE* RST_ColorAdjText(void);
#endif
extern BYTE* RST_ResetText(void);
//-----------------------------
#if OSD_INPUT_SUBMENU_ENABLE
extern BYTE* Misc_InputSourceText(void);
#endif
extern BYTE* Misc_SourceSelectext(void);
#if AudioFunc
extern BYTE* Misc_MuteText(void);
extern BYTE* Misc_MuteOnOfftext(void);
extern BYTE* Misc_VolumeText(void);
#if Audio_Switch
extern BYTE* HDMIAUDIOText(void);
extern BYTE* HDMIAudioValueText(void);
#endif
#endif
//-------------------------------------
extern BYTE* HotKeyDCROnIcon(void);
extern BYTE* HotKeyDCROffIcon(void);
extern BYTE* HotKeyECOStandardIcon(void);
extern BYTE* HotKeyECOGameIcon(void);
extern BYTE* HotKeyECOWebIcon(void);
extern BYTE* HotKeyECOMoveIcon(void);

extern BYTE* HotKeyContrastText(void);
extern BYTE* HotKeyBrightnessText(void);
#if KeyInputSelectMenu
#if(InputType_Analog1)
extern BYTE* HotKeyInputVGAText(void);
#endif
#if InputType_Digital
extern BYTE* HotKeyInputDVIText(void);
#endif
#if InputType_HDMI
extern BYTE* HotKeyInputHDMIText(void);
#endif
#if ENABLE_DP_INPUT
extern BYTE* HotKeyInputDPText(void);
#endif
#endif
#if AudioFunc
extern BYTE* HotKeyVolumeText(void);
#endif

#if EN_HotKeyTimes
extern BYTE* HotKeyTimesText(void);
extern BYTE* HotKeyTimes10Text(void);
extern BYTE* HotKeyTimes20Text(void);
extern BYTE* HotKeyTimes30Text(void);
extern BYTE* HotKeyTimes40Text(void);
extern BYTE* HotKeyTimes50Text(void);
extern BYTE* HotKeyTimes60Text(void);
extern BYTE* HotKeyTimes90Text(void);
extern BYTE* HotKeyShowTimesText(void);
#endif

#if EN_HotKeyXINGZHUN
extern BYTE* HotKeyXINGZHUNText(void);
extern BYTE* HotKeyXZCJText(void);



//extern BYTE* HotKeyjishiText(void);

//extern BYTE* HotKeyXZIcon(void);
//extern BYTE* HotKeyJSIcon(void);
//extern BYTE* HotKeyXZ_XIcon(void);

#endif
//-------------------------------------
extern BYTE *ProcessAutoText(void);
#if Enable_Menu_LockOSD
extern BYTE *OsdLockedText(void);
extern BYTE *OsdUnLockedText(void);
#endif
#if Enable_LockPowerKey
extern BYTE *PowerLockedText(void);
extern BYTE *PowerUnLockedText(void);
#endif
#if ShowSourceType
extern BYTE* ShowInputTypeText(void);
#endif
extern BYTE* NoSignalText(void);
extern BYTE* OutOfRangeText(void);
//------------------------------------------
extern BYTE* FactoryAutoColorText(void);
extern BYTE* FactoryPassText(void);
extern BYTE* FactoryFailText(void);
extern BYTE* FactoryBrightnessText(void);
extern BYTE* FactoryContrastText(void);
extern BYTE* FactoryAdcGainText(void);
extern BYTE* FactoryAdcOffsetText(void);
#if FactoryAuto_5400K
extern BYTE* Factory5400KText(void);
#endif
#if FactoryAuto_6500K
extern BYTE* Factory6500kText(void);
#endif
#if FactoryAuto_9300K
extern BYTE* Factory9300kText(void);
#endif
extern BYTE* FactorResetText(void);
			
extern BYTE* ExitText(void);
extern BYTE* SelectText(void);
extern BYTE* ExitText1(void);
extern BYTE* SelectText1(void);

extern BYTE* BurninText(void);
extern BYTE* BurninOnOffText(void);
extern BYTE* FactoryRedText(void);
extern BYTE* FactoryGreenText(void);
extern BYTE* FactoryBlueText(void);
#if ENABLE_FACTORY_SSCADJ	//120305 Modify
extern BYTE *SpreadSpectrumText( void );
extern BYTE *FreqText( void );
extern BYTE *AmpText( void );
#endif
#if DisplayLogo!=NoBrand	//130613 Jun
extern BYTE *LogoText( void );
extern BYTE *LogoOnOffText( void );
#endif

#if DDCCI_ENABLE
extern BYTE *DDCCIText( void );
extern BYTE *DDCCIOnOffText(void);
#endif

#if !USEFLASH // 130829 larry
extern BYTE *EraseEEPROMText( void );
#endif
extern BYTE *FactoryExitText( void );
//-----------------
extern BYTE *VersionDateText(void);
extern BYTE *ChangeDateText(void);
extern BYTE *ChecksumText(void);
extern BYTE *VerText(void);
extern BYTE *Time_HText(void);
extern BYTE *Time_MText(void);
extern BYTE *Time_SText(void);
extern BYTE *PanelText(void);
extern BYTE *TIMEText(void);


#if ENABLE_BURNIN_MENU

extern BYTE *EDIDText(void);
extern BYTE *PanelWH_Text(void);

#endif


