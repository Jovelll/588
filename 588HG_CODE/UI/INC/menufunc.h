extern WORD GetNonFuncValue( void );
extern WORD GetBlueColorValue( void );
extern WORD GetGreenColorValue( void );
extern WORD GetRedColorValue( void );
extern Bool ChangeSource( void );
extern void MenuFuncuncall(void);





//==============ready================


#if 0

extern WORD GetSharpnessValue( void );
extern WORD GetSharpnessRealValue( void );
extern Bool AdjustSharpness( MenuItemActionType action );


extern Bool SwitchSource( void );

extern Bool ChangeSourceToDigital( void );
extern Bool ChangeSourceToHDMI( void );


#if Enable_Gamma
extern Bool AdjustGamaMode( MenuItemActionType action );
#endif

extern Bool SetDCRmode(void);

#if ENABLE_DSC
extern Bool AdjustDSCMode( MenuItemActionType action );
extern Bool SetDSCmode(void);

#endif
#if ENABLE_DLC
extern Bool AdjustDLCMode( MenuItemActionType action );
extern Bool SetDLCmode(void);
#endif
#if ENABLE_3DLUT
extern Bool Adjust3DLUTMode( MenuItemActionType action );
#endif
#if ENABLE_OSD_3D_CONTROL
extern Bool AdjustT3DMode( MenuItemActionType action );
extern Bool AdjustT3DGameMode(MenuItemActionType action);
extern Bool AdjustT3DGain( MenuItemActionType action );
extern WORD GetT3DGainValue( void );
extern Bool AdjustT3DOffset( MenuItemActionType action );
extern WORD GetT3DOffsetValue( void );

#endif
#if ENABLE_DPS
extern Bool AdjustDPSMode( MenuItemActionType action );
extern Bool SetDPSmode(void);
#endif
#if ENABLE_SUPER_RESOLUTION
extern Bool AdjustSuperResolutionMode( MenuItemActionType action );
#endif
#if ENABLE_TNR
extern Bool AdjustNRMode( MenuItemActionType action );
#endif
#if ENABLE_DeBlocking
extern Bool AdjustDeBlockingMode( MenuItemActionType action );
extern void msDeBlockingOnOff( BYTE ucSwitch, BYTE win);
extern void msInitDeBlocking( void );
#endif

#if MWEFunction
extern Bool AdjustColorBoostMode( MenuItemActionType action );
extern Bool AdjustSkinProtectMode( MenuItemActionType action );
extern Bool AdjustGreenEnhanceMode( MenuItemActionType action );
extern Bool AdjustBlueEnhanceMode( MenuItemActionType action );
extern Bool AdjustAutoDetectMode( MenuItemActionType action );
extern Bool AdjustDemoMode( MenuItemActionType action );
#endif
extern Bool SetToEnglish( void );
#if 1

/*#if USEFLASH
    extern Bool EraseFlashSettingCell(void);
#else*/
#if !USEFLASH
extern Bool EraseAllEEPROMCell( void );
#else
extern Bool EraseFlashUserData(void); //2011.9.13 14:54 CC
#endif


extern Bool SetFactoryColorTempCool2( void );

extern Bool SetFactoryColorTempWarm2( void );

extern Bool SetFactoryColorTempSRGB( void );

extern Bool AdjustBrightnessWarm1( MenuItemActionType action );
extern Bool AdjustContrastWarm1( MenuItemActionType action );

extern WORD GetBrightnessWarm1Value( void );
extern WORD GetContrastWarm1Value( void );
extern Bool AdjustRedColorWarm2( MenuItemActionType action );
extern Bool AdjustGreenColorWarm2( MenuItemActionType action );
extern Bool AdjustBlueColorWarm2( MenuItemActionType action );
extern Bool AdjustBrightnessWarm2( MenuItemActionType action );
extern Bool AdjustContrastWarm2( MenuItemActionType action );
extern WORD GetBColorWarm2Value( void );
extern WORD GetGColorWarm2Value( void );
extern WORD GetRColorWarm2Value( void );
extern WORD GetBrightnessWarm2Value( void );
extern WORD GetContrastWarm2Value( void );


extern Bool AdjustBrightnessCool1( MenuItemActionType action );
extern Bool AdjustContrastCool1( MenuItemActionType action );
extern WORD GetBrightnessCool1Value( void );
extern WORD GetContrastCool1Value( void );
extern Bool AdjustRedColorCool2( MenuItemActionType action );
extern Bool AdjustGreenColorCool2( MenuItemActionType action );
extern Bool AdjustBlueColorCool2( MenuItemActionType action );
extern Bool AdjustBrightnessCool2( MenuItemActionType action );
extern Bool AdjustContrastCool2( MenuItemActionType action );
extern WORD GetBColorCool2Value( void );
extern WORD GetGColorCool2Value( void );
extern WORD GetRColorCool2Value( void );
extern WORD GetBrightnessCool2Value( void );
extern WORD GetContrastCool2Value( void );

extern Bool AdjustBrightnessNormal( MenuItemActionType action );
extern Bool AdjustContrastNormal( MenuItemActionType action );

extern WORD GetBrightnessNormalValue( void );
extern WORD GetContrastNormalValue( void );
extern Bool AdjustRedColorSRGB( MenuItemActionType action );
extern Bool AdjustGreenColorSRGB( MenuItemActionType action );
extern Bool AdjustBlueColorSRGB( MenuItemActionType action );
extern Bool AdjustBrightnessSRGB( MenuItemActionType action );
extern Bool AdjustContrastSRGB( MenuItemActionType action );
extern WORD GetBColorSRGBValue( void );
extern WORD GetGColorSRGBValue( void );
extern WORD GetRColorSRGBValue( void );
extern WORD GetBrightnessSRGBValue( void );
extern WORD GetContrastSRGBValue( void );
extern Bool AdjustYColor( MenuItemActionType action );
extern Bool AdjustCColor( MenuItemActionType action );
extern Bool AdjustMColor( MenuItemActionType action );
extern WORD GetYColorValue( void );
extern WORD GetCColorValue( void );
extern WORD GetMColorValue( void );


//extern WORD GetSSCStepValue( void ); //CC 2011.9.9 15:26
//extern WORD GetSSCSpanValue( void ); //CC 2011.9.9 15:26
extern WORD GetProductionValue( void ); //CC 2011.9.9 10:00
extern Bool AdjustProduction(MenuItemActionType action); //CC 2011.9.9 10:00
extern WORD GetDDCWPActiveValue( void ); //CC 2011.9.9 16:00
extern Bool AdjustDDCWP( MenuItemActionType action ); //CC 2011.9.9 16:00
extern Bool AdjustBankNo( MenuItemActionType action );
extern Bool AdjustAddr( MenuItemActionType action );
extern WORD GetAddrValue( void );
extern Bool AdjustRegValue( MenuItemActionType action );
extern WORD GetAddrRegValue( void );
extern WORD GetRegValue( void );
extern Bool WriteRegValue( void );
//extern WORD GetOSDTime100Value(void);


extern void SaveFactorySettingByItem( BYTE itemIndex );
extern void SaveFactorySetting( void );
extern Bool EnterISPMode( void );
#endif




//extern Bool DrawMenuTitle(void);
//extern Bool DrawTimingInfo( void );

#if DDCCI_ENABLE
//    extern Bool ExecDDC2Bi(void);

extern Bool ResetColorTemp( void );
extern void AdjustTempColorTemp( BYTE ctemp );
#endif

extern Bool AdjustBlue( MenuItemActionType action );
extern Bool AdjustGreen( MenuItemActionType action );
extern Bool AdjustMWE( MenuItemActionType action );
#if MWEFunction
extern Bool AdjustSubContrast( MenuItemActionType action );
extern Bool AdjustSubBrightness( MenuItemActionType action );
extern Bool AdjustBFSize( MenuItemActionType action );
extern Bool AdjustBFHstart( MenuItemActionType action );
extern WORD GetBFHstartValue( void );
extern Bool AdjustBFVstart( MenuItemActionType action );
extern WORD GetBFVstartValue( void );
extern WORD GetBFSizeValue( void );
extern WORD GetSubContrast100Value( void );
extern WORD GetSubBrightness100Value( void );
extern Bool AdjustBF_Switch( MenuItemActionType action );
extern WORD GetBF_SwitchValue( void );
extern Bool SetBFSize( BYTE ucSize );
extern void SetColorEnhance( bit enable );
#endif
extern WORD GetPanelOnTimeValue( void );
extern Bool ResetGeometry( void );
#if 0//ENABLE_RTE
extern  WORD GetOverDriveValue( void );
#endif
#if ENABLE_TNR
extern  Bool AdjustTNR( MenuItemActionType action );
#endif
#if PanelRSDS
extern WORD GetVCOMValue( void );
extern Bool AdjustVCOMValue( MenuItemActionType action );
#endif
#if ENABLE_SUPER_RESOLUTION
extern void mStar_SetupSuperResolution( BYTE SuperResolutionMode );
extern Bool SetSuperResolutionMode(void);
#endif
#if ENABLE_SHARPNESS
extern Bool AdjustSharpness( MenuItemActionType action );
extern WORD GetSharpnessValue( void );
#endif
#if ENABLE_TNR
extern void mStar_SetupNRmode( BYTE NRMode );
#endif


extern Bool SetColorFormat(void);


#if ENABLE_OSD_ROTATION
extern Bool SetOSDRotateMode(void);
#endif

extern Bool OSD_SetColorTemp(void);
extern Bool AdjustVideoSaturation(MenuItemActionType enAction);
extern WORD GetSaturationValue( void );
 
extern Bool AdjustVideoHUE(MenuItemActionType enAction);
extern WORD GetHueValue( void );

extern Bool SetGammaOnOff(void);

#endif
//vsadd ccflag






#if ENABLE_FREESYNC
BYTE GetSrcFreesyncFPS(void);
extern Bool SetDRRFunction(void);
#endif


extern void SetOSDTimer(BYTE osdtime);
extern Bool ResetOsdTimer( void );
extern Bool EnablePowerDownCounter( void );
extern Bool PowerOffSystem( void );
extern Bool PowerOnSystem( void );
extern Bool EnterRootMenu( void );
extern Bool AdjustContrast( MenuItemActionType action );
extern WORD GetContrastValue( void );
extern Bool AdjustBrightness( MenuItemActionType action );
extern WORD GetBrightnessValue( void );

extern Bool AdjustFocus( MenuItemActionType action );
extern WORD GetFocusValue( void );
extern Bool AdjustClock( MenuItemActionType action );
extern WORD GetClockValue( void );
extern Bool AdjustHPosition( MenuItemActionType action );
extern WORD GetHPositionValue( void );
extern Bool AdjustVPosition( MenuItemActionType action );
extern WORD GetVPositionValue( void );

#if DualInput
#if OSD_INPUT_SUBMENU_ENABLE
extern Bool AdjustInputType(MenuItemActionType action);
#endif
extern Bool SwitchInput(void);
#endif

#if 0//EN_HotKeyTimes
extern Bool AdjustTimes(void);
#endif

#if AudioFunc
extern Bool AdjustVolume( MenuItemActionType action );
extern WORD GetVolumeValue( void );
#if (ENABLE_HDMI || ENABLE_DP_INPUT) && MS_DAC
extern Bool AdjustAudioSource( void );
#endif
extern Bool SwitchMute(void);
#endif

#if Audio_Switch
extern Bool AdjustAudioType(MenuItemActionType action);
#endif
extern Bool AdjustOSDHPosition( MenuItemActionType action );
extern WORD GetOSDHPositionValue( void );
extern Bool AdjustOSDVPosition( MenuItemActionType action );
extern WORD GetOSDVPositionValue( void );
extern Bool AdjustOSDTime( MenuItemActionType action );
extern WORD GetOSDTimeValue( void );
extern WORD GetOSDTime100Value( void );
extern Bool AdjustOSDTransparency( MenuItemActionType action );
extern WORD GetOSDTransparencyValue( void );

extern Bool AdjustRedColor( MenuItemActionType action );
extern Bool AdjustGreenColor( MenuItemActionType action );
extern Bool AdjustBlueColor( MenuItemActionType action );
extern WORD GetRedColorValue100(void);
extern WORD GetGreenColorValue100(void);
extern WORD GetBlueColorValue100(void);
extern Bool SetColorTemp( void );
extern Bool AdjustColorTempMode( MenuItemActionType action );
extern Bool AdjustLanguage(void);
extern Bool ResetAllSetting( void );
extern Bool AutoConfig( void );
#if DDCCI_ENABLE
extern Bool ResetGeometry( void );
#endif

extern void SaveUserPref(void);
extern void SaveUserPrefSetting(BYTE menuPageIndex);

extern Bool AutoColor( void );
extern Bool AdjustBlueGain( MenuItemActionType action );
extern WORD GetBlueGainValue( void );
extern Bool AdjustGreenGain( MenuItemActionType action );
extern WORD GetGreenGainValue( void );
extern Bool AdjustRedGain( MenuItemActionType action );
extern WORD GetRedGainValue( void );
extern Bool AdjustAdcBlueOffset( MenuItemActionType action );
extern WORD GetAdcBlueOffsetValue( void );
extern Bool AdjustAdcGreenOffset( MenuItemActionType action );
extern WORD GetAdcGreenOffsetValue( void );
extern Bool AdjustAdcRedOffset( MenuItemActionType action );
extern WORD GetAdcRedOffsetValue( void );

extern void ReadColorTempSetting( void );
extern void SetECO( void );
extern Bool AdjustECOMode( MenuItemActionType action );
extern Bool AdjustDcrMode( MenuItemActionType action );

extern Bool SetFactoryColorTempCool1( void );
extern Bool SetFactoryColorTempNormal( void );
extern Bool SetFactoryColorTempWarm1( void );

extern Bool AdjustRedColorWarm1( MenuItemActionType action );
extern Bool AdjustGreenColorWarm1( MenuItemActionType action );
extern Bool AdjustBlueColorWarm1( MenuItemActionType action );
extern WORD GetBColorWarm1Value( void );
extern WORD GetGColorWarm1Value( void );
extern WORD GetRColorWarm1Value( void );

extern Bool AdjustRedColorNormal( MenuItemActionType action );
extern Bool AdjustGreenColorNormal( MenuItemActionType action );
extern Bool AdjustBlueColorNormal( MenuItemActionType action );
extern WORD GetBColorNormalValue( void );
extern WORD GetGColorNormalValue( void );
extern WORD GetRColorNormalValue( void );

extern Bool AdjustRedColorCool1( MenuItemActionType action );
extern Bool AdjustGreenColorCool1( MenuItemActionType action );
extern Bool AdjustBlueColorCool1( MenuItemActionType action );
extern WORD GetBColorCool1Value( void );
extern WORD GetGColorCool1Value( void );
extern WORD GetRColorCool1Value( void );

#if ENABLE_FACTORY_SSCADJ
extern  Bool AdjustFactorySSCModulation( MenuItemActionType action );//CC 2011.9.13 18:51
extern  WORD GetFactorySSCModulationValue( void );//CC 2011.9.13 18:51
extern  Bool AdjustFactorySSCPercentage( MenuItemActionType action );//CC 2011.9.13 18:51
extern  WORD GetFactorySSCPercentageValue( void );//CC 2011.9.13 18:51
#endif

extern Bool EnableBurninMode( void );
extern WORD GetBurninValue( void );
extern Bool AdjustBurnin( MenuItemActionType action );

#if DisplayLogo!=NoBrand	
extern Bool AdjustLogo(MenuItemActionType action);
extern WORD GetLogoValue(void);
#endif

#if DDCCI_ENABLE
extern Bool AdjustDDCCI( MenuItemActionType action );
extern WORD GetDDCCIValue(void);
#endif

extern Bool EnterDisplayLogo( void );
#if Enable_Expansion
extern void SetExpansionMode( void );
extern Bool AdjustExpansionMode(void);
#endif
#if OSD_OD 
//extern  WORD GetOverDriveValue(void);
extern Bool AdjustOverDrive(MenuItemActionType action);
#endif
#if LowBlueLight
extern Bool AdjustBlueLightMode(MenuItemActionType action);
#endif


