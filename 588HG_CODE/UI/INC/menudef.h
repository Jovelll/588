
#ifndef MENUDEF_H
#define MENUDEF_H
#include "Board.h"
#define Multi_Number    1
#define Multi_Gauge 1
#define OnlyDrawColor   BIT7
#define OnlyDrawCode    BIT6

//ccflag  vsadd
#define SpaceFont			0x00

#define MainMenuIcon_DrawXPos	0x01
#define MainMenuIcon_DrawYPos	0x03

#define SubMenuText_DrawStarXPos 0x06
#define SubMenu_DrawStarYPos	0x04

#define MenuGuage_DrawStarXPos	0x16
#define MenuGuage_DrawLength	0x0E
#define MenuGuage_DrawStarYPos 	SubMenu_DrawStarYPos

#define MenuNumber_DrawStarXPos  0x23
#define MenuNumber_DrawStarYPos 	SubMenu_DrawStarYPos

#define MenuRatio_DrawStarXPos	0x17
#define MenuRatio_DrawStarYPos 	SubMenu_DrawStarYPos

#define SubMenu_DrawYPos_Shift	0x02

#define SubMenu_DrawXPos_Shift	0x0A


#define MenuFrameStart 		0x08

#define MenuFrame_LeftTop     MenuFrameStart
#define MenuFrame_RightTop     MenuFrameStart+1
#define MenuFrame_LeftBottom     MenuFrameStart+2
#define MenuFrame_RightBottom     MenuFrameStart+3

#define MenuFrame_TopSide	MenuFrameStart+4
#define MenuFrame_BottomSide	MenuFrameStart+5
#define MenuFrame_LeftSide	MenuFrameStart+6
#define MenuFrame_RightSide	MenuFrameStart+7

#define MenuFrame_LeftSide_1	MenuFrameStart+8
#define MenuFrame_LeftSide_2	MenuFrameStart+9
#define MenuFrame_LeftSide_3	MenuFrameStart+(0x1D-0x08)

#define MenuFrame_RightSide_1	MenuFrameStart+(0x1B-0x08)
#define MenuFrame_RightSide_2	MenuFrame_RightSide
#define MenuFrame_RightSide_3	MenuFrameStart+(0x1C-0x08)

#define AutoMenuFrameLeftSide_1		MenuFrame_LeftSide_1
#define AutoMenuFrameLeftSide_2		MenuFrame_LeftSide_2
#define AutoMenuFrameLeftSide_3		MenuFrame_LeftSide_3
#define AutoMenuFrameRightSide_1		MenuFrame_RightSide_1
#define AutoMenuFrameRightSide_2		MenuFrame_RightSide_2
#define AutoMenuFrameRightSide_3		MenuFrame_RightSide_3
#define AutoMenuFrameMide_Top			MenuFrameStart+(0x1F-0x08)
#define AutoMenuFrameMide_Bottom		MenuFrameStart+(0x1E-0x08)

#if Customer_Type_Sel==Customer_PENGUAN
#define CPC_HKCBlackColor  0x03 //BLUE 
#else
#define CPC_HKCBlackColor  0x08 //BLACK
#endif
 
#define MonoFontStart       0x00   //94   5E
#define FONT_DYNIC_START		0x60  //langue   max91  5b
			
#if EN_HotKeyXINGZHUN 
#define MonoFontXZStart      0xC0 // 0xEC     // 8
#else
#define MonoFontSmallLogoStart       0xEC     // 8
#endif

#if Los_Select
#define MonoFontStart2       0xE6
#endif


#define LogoMonoFontStart       0x01
#define Logo_4C         0x02

#define FONT_4COLOR_ICON_START 	0x14

#if 0//EN_HotKeyXINGZHUN   
#define FONT_XZ4COLOR_ICON_START 	0x30

#endif
#define FONT_8COLOR_ICON_START	0x3F//0x2E


#define Color_2  0x01    //
#define Color_4  0x02   //Ëìù
#define Color_8  0x03
#define Color_2T  0x04    //Transparent
#define Color_4T  0x05    //Transparent
#define Color_8T  0x06    //Transparent  ÈÄèÊòé
#define Color_2G  0x07  //Gradual
#define Color_8G  0x08  //Gradual

typedef enum
{
		PowerOffMenu,				// 0
		PowerOnMenu,				// 1
		StandbyMenu,					// 2
		UnsupportedModeMenu,	  		// 3
		RootMenu,					// 4
	
		MainMenu,					// 5
		OSD_BriContrastMenu,			// 6
		PictureMenu,					// 7
		RGBColorMenu,			//	8
		OsdMenu,					//	9
		LanguageMenu,			//	
		SettingMenu,				//	  
		OSD_MiscMenu,			 // 	12
		
     #if EN_HotKeyDCRMenuWithIcon
		 HotKeyDCROnMenu,		// 14
		 HotKeyDCROffMenu,		// 15
     #endif  
     #if EN_HotKeyECOMenuWithIcon
		 HotKeyECOWebMenu,		// 16
		 HotKeyECOStandardMenu, // 17
		 HotKeyECOGameMenu, 	// 18
		 HotKeyECOMoveMenu, 	// 19
     #endif 
		HotKeyContrastMenu, 	// 13
		HotKeyBrightnessMenu,	// 14
    #if KeyInputSelectMenu
		HotKeyInputSelectMenu,   //15
    #endif
	
#if AudioFunc
	  HotKeyVolumeMenu, 	// 16
	  HotKeyMuteMenu,	//17
#endif
#if Enable_Menu_LockOSD
		OSDLockedMenu,		  
		OSDUnLockedMenu,	
#endif
#if Enable_LockPowerKey
	   PowerLockedMenu, 	   
	   PowerUnLockedMenu,	  //21 
#endif
#if Los_Select
	   HotKeyCorssMenu,
#endif
#if  LowBlueLight
	HotKeyLowBlueLightMenu,
#endif
		AutoMenu,					//	22
#if ShowSourceType
		InputTypeMenu,				//	23
#endif
	
		BurninMenu, 			// 24
	
		FactoryMenu,				//	25
		LogoMenu,				//	26
#if EN_HotKeyTimes
		   HotKeyTimes,	//27
	        HotKeyShowTimes, //28
		   
 #endif
#if EN_HotKeyXINGZHUN
		HotXINGZHUN,
		HotXINGZHUN1,
		HotXINGZHUN2,
		 HotXINGZHUN3,
		 HotXINGZHUN4,
		 HotXINGZHUN5,
		 HotXINGZHUN6,
#endif
#if 0//ENABLE_BURNIN_MENU
         BurninMenu,				//	26
#endif		
		
		MaxMenu,

} MenuPageIndexType;

//----------------------------------------------------
#if Audio_Switch
typedef enum
{
	Audio_LineIn,
	Audio_HDMI,
	AUdioType_Nums
}AudioType;
#endif


typedef enum _BriConMenuItems_
{
	mBrightTextItem,
	mBrightGuageItem,

	mContrastTextItem,
	mContrastGuageItem,
	#if 1 //MWE_AP_Enable
	mEcoTextItem,
	mEcoSelectItem,
	#if Enable_AdjustDCR
	mDcrTextItem,
	mDcrSelectItem,
	#endif
	#endif
	#if LowBlueLight
	mBlueLightTextItem,
	mBlueLightSelectItem,
	#endif
	mItemNums,
}BriConMenuItems;

typedef enum _ImageMenuItems_
{
	mHPositionTextItem,
	mHPositionGuageItem,

	mVPositionTextItem,
	mVPositionGuageItem,

	mClockTextItem,
	mClockGuageItem,

	mPhaseTextItem,
	mPhaseGuageItem,

	mAspectTextItem,
	mAspectSelectItem,

	mImageItemNums,
}ImageMenuItems_;

typedef enum _ColorTempMenuItemNames_
{
	mColorTempTextItem,
#if 1//Customer_Type_Sel==Customer_GUANRONG //20201027
   mColorTempSelectItem,
#endif
	mRedTextItem,
	mRedGuageItem,
	mGreenTextItem,
	mGreenGuageItem,
	mBlueTextItem,
	mBlueGuageItem,

	mColortempItemNums,

}ColorTempMenuItemNames;
typedef enum _ResetMenuItemNames_
{
	mAutoAdjTextItem,
#if _AUTOCOLOR_IN_SETTING_MENU_ENABLE 		
	mColorAdjTextItem,
#endif	
	mResetTextItem,

	mResetItemNums,

}ResetMenuItemNames;

typedef enum _MiscMenuItemNames_
{	
	mInputSourceTextItem,
	mInputSourceSelectItem,
	#if MiscMenuMuteItem
	mMuteTextItem,
	mMuteSelectItem,
	#endif
	mVolumeTextItem,
	mVolumeGuageItem,
	#if Audio_Switch
	mHdmiAudioTextItem,
	mHdmiAudioSelectItem,
	#endif

	mMiscMenuItemNums,

}MiscMenuItemNames;

#if KeyInputSelectMenu
typedef enum _InputSelectMenuItemNames_
{
  #if(InputType_Analog1)
     mVGATextItem,
  #endif
  #if InputType_Digital
	mDVITextItem,
  #endif
  #if InputType_HDMI
	mHDMITextItem,
  #endif
  #if ENABLE_DP_INPUT
	mDPTextItem,
  #endif
	mInputItemNums,

}InputSelectMenuItemNames;
#endif 

#if EN_HotKeyTimes
typedef enum _HotKeyTimesMenuItemNames_
{
     Times10TextItem=1,
	 Times20TextItem,
	 Times30TextItem,
	 Times40TextItem,
	 Times50TextItem,
	 Times60TextItem,
	 Times90TextItem,
	TimesItemNums,

}HotKeyTimesMenuItemNames;
#endif

typedef enum _FactoryMenuItemIndex_
{	
	fAutoColorItem,

	fBrightnessTextItem,
	fBrightnessAdjustItem,
	fContrastTextItem,
	fContrastAdjustItem,

	#if 1 //FactoryAuto_ADC
	fADCGainTextItem,
	fADCRedTextItem,
	fGainRAdjustItem,
	fADCGreenTextItem,
	fGainGAdjustItem,
	fADCBlueTextItem,
	fGainBAdjustItem,

	fADCOffsetTextItem,
	fOffsetRedTextItem,
	fOffsetRAdjustItem,
	fOffsetGreenTextItem,
	fOffsetGAdjustItem,
	fOffsetBlueTextItem,
	fOffsetBAdjustItem,
	#endif

	#if FactoryAuto_5400K
	f5400KTextItem,
	f5400KRedTextItem,
	fRedColor5400KAdjustItem,
	f5400KGreenTextItem,
	fGreenColor5400KAdjustItem,
	f5400KBlueTextItem,
	fBlueColor5400KAdjustItem,
	#endif

	#if 1 //FactoryAuto_6500K
	f6500KTextItem,
	f6500KRedTextItem,
	fRedColor6500KAdjustItem,
	f6500KGreenTextItem,
	fGreenColor6500KAdjustItem,
	f6500KBlueTextItem,
	fBlueColor6500KAdjustItem,
	#endif
	
	#if 1 //FactoryAuto_9300K
	f9300KTextItem,
	f9300KRedTextItem,
	fRedColor9300KAdjustItem,
	f9300KGreenTextItem,
	fGreenColor9300KAdjustItem,
	f9300KBlueTextItem,
	fBlueColor9300KAdjustItem,
	#endif
	
	#if 1 //FactoryAuto_BurnIn
	fBurnInTextItem,
	fBurnAdjustItem,
	#endif
	
	fResetTextItem,

       #if 1 //FactoryAuto_BurnIn
	fFREQTextItem,
	fFREQAdjustItem,
	fAMPTextItem,
	fAMPAdjustItem,
	#endif

	fLogoSWTextItem,
	fLogoSWAdjustItem,
		
}FactoryMenuItemIndex;




typedef enum
{
    COLORBOOST_FULL,       //4 //1
    COLORBOOST_HALF,    // 2
    COLORBOOST_OFF,   // 3
    COLORBOOST_Nums

} ColorBoostType;


typedef enum
{
    ECO_Standard,       //4 //1
    ECO_Text,          // 2
    //ECO_Internet,   // 3
    ECO_Game,        // 4
    ECO_Movie,        // 5
    //ECO_Sports,       //4 // 6

    ECO_Nums

} ECOModeType;

#if LowBlueLight
typedef enum
{
  BlueLight_Off,
  BlueLight_Browsing,
  BlueLight_Read,
  BlueLight_Video,
  BlueLight_lowlight,
  BlueLight_Nums,
  BlueLight_Shield,       
  BlueLight_Middle,          
  BlueLight_Standard,        

} BlueLightModeType;
#endif

#if Enable_Expansion
typedef enum
{
    Expansion_Full,
    Expansion_4_3,
    Expansion_Nums
} ExpansionType;
#endif

#if ENABLE_SUPER_RESOLUTION
typedef enum
{
    SRMODE_OFF,//SRMODE1,          // 0   high
    SRMODE_WEAK,//SRMODE2,   // 1     middle
    SRMODE_MIDDLE,//SRMODE3,        // 2    low
    SRMODE_STRONG,//SRMODE_OFF,     // 3

    SRMODE_Nums
} SRModeType;
#endif

#if ENABLE_TNR
typedef enum
{
    NRMODE1,          // 0   high
    NRMODE2,   	  // 1     middle
    NRMODE3,          // 2    low
    NRMODE_OFF,     // 3

    NRMODE_Nums
} NRModeType;
#endif

typedef enum
{
#if  FactoryAuto_5400K
	CTEMP_5400K,	
#endif
	CTEMP_6500K,	
	CTEMP_9300K,  
	CTEMP_USER, //	USER
	CTEMP_Nums, 
	CTEMP_SRGB,   //  SRGB    	
} ColorTempType;



#if Enable_Gamma
typedef enum
{
    GAMA1,          // 0
    GAMA2,   // 1
    GAMA3,        // 2

    GAMA_Nums
} GAMAModeType;
#endif


//language select
#define Language_English		       1
#define Language_German		1
#define Language_France			1
#define Language_Spanish		1
#define Language_Italian		       1

#define Language_Swedish		0
#define Language_Finland		       0

#define Language_Russian		1

#define Language_Portuguese		1//0
#define Language_Polish			1//0
#define Language_Greece			0
#define Language_Korean			0
#define Language_Japanese		0
#define Language_SimChinese		1 //0

#define Language_Dutch		1//	0
#define Language_TraChinese		0
#define Language_Ukrainian           0
#define Language_Turkish          1//    0

#define Language_Russian_Lowercase       (1&&Language_Russian)

typedef enum
{ 

	#if Language_English
  	HKC_LANG_English,
	#endif
	#if Language_France
  	HKC_LANG_France,
	#endif	
	#if Language_German
  	HKC_LANG_German,
	#endif
	#if Language_SimChinese
  	HKC_LANG_SimChinese,
	#endif
	#if Language_Italian
  	HKC_LANG_Italian,
	#endif
	#if Language_Spanish
  	HKC_LANG_Spanish,
	#endif
	#if Language_Portuguese
  	HKC_LANG_Portuguese,
	#endif	
       #if Language_Turkish
  	HKC_LANG_Turkish,
	#endif   
	#if Language_Polish
  	HKC_LANG_Polish,
	#endif
	#if Language_Dutch
  	HKC_LANG_Dutch,
	#endif
	#if Language_Russian
  	HKC_LANG_Russian,
	#endif
	#if Language_Korean
  	HKC_LANG_Korean,    
	#endif
	#if Language_TraChinese
  	HKC_LANG_TraChinese,
	#endif
	
	#if Language_Swedish
  	HKC_LANG_Swedish,
	#endif
	#if Language_Finland
  	HKC_LANG_Finland,
	#endif	
      #if Language_Ukrainian
  	HKC_LANG_Ukrainian,	  
      #endif	
	#if Language_Greece
  	HKC_LANG_Greece,
	#endif
	#if Language_Japanese
  	HKC_LANG_Japanese,    
	#endif
	
  	HKC_LANG_Nums,

	
	#if !Language_English
  	HKC_LANG_English,
	#endif
	#if !Language_German
  	HKC_LANG_German,
	#endif
	#if !Language_Dutch
  	HKC_LANG_Dutch,
	#endif
	
	#if !Language_France
  	HKC_LANG_France,
	#endif
	#if !Language_Spanish
  	HKC_LANG_Spanish,
	#endif
	#if !Language_Italian
  	HKC_LANG_Italian,
	#endif
	#if !Language_Swedish
  	HKC_LANG_Swedish,
	#endif
	#if !Language_Finland
  	HKC_LANG_Finland,
	#endif
	#if !Language_Portuguese
  	HKC_LANG_Portuguese,
	#endif
	#if !Language_Polish
  	HKC_LANG_Polish,
	#endif
	#if !Language_Russian
  	HKC_LANG_Russian,
	#endif
	#if !Language_Ukrainian //rafe lau add for Ukraiaian060823
  	HKC_LANG_Ukrainian,
	#endif	
	#if !Language_Greece
  	HKC_LANG_Greece,
	#endif
	#if !Language_Korean
  	HKC_LANG_Korean,    
	#endif
	#if !Language_Japanese
  	HKC_LANG_Japanese,    
	#endif
	#if !Language_SimChinese
  	HKC_LANG_SimChinese,
	#endif

	
	 #if !Language_TraChinese
  	HKC_LANG_TraChinese,
	#endif

       #if !Language_Turkish
  	HKC_LANG_Turkish,
	#endif   

	HKC_LANG_Max


} LanguageType;

#define LANG_English		       HKC_LANG_English//”¢”Ô
#define LANG_German		HKC_LANG_German//µ¬”Ô
#define LANG_Franch			HKC_LANG_France//∑®”Ô
#define LANG_Spanish		HKC_LANG_Spanish//Œ˜∞‡—¿”Ô
#define LANG_Italian		       HKC_LANG_Italian//“‚¥Û¿˚”Ô
#define LANG_Swedish		HKC_LANG_Swedish//»µ‰”Ô
#define LANG_Finland		       HKC_LANG_Finland//∑“¿º”Ô
#define LANG_Portuguese		HKC_LANG_Portuguese//∆œÃ——¿”Ô
#define LANG_Polish			HKC_LANG_Polish//≤®¿º”Ô
#define LANG_Ukrainian           HKC_LANG_Ukrainian //  Œ⁄øÀ¿º”Ô   
#define LANG_Russian		HKC_LANG_Russian//∂Ì”Ô
#define LANG_Greece			HKC_LANG_Greece//œ£¿∞”Ô
#define LANG_Korean			HKC_LANG_Korean//∫´”Ô
#define LANG_Japanese		HKC_LANG_Japanese//»’Œƒ
#define LANG_SimChinese		HKC_LANG_SimChinese//ºÚÃÂ÷–Œƒ
 
#define LANG_Turkish              	HKC_LANG_Turkish//Õ¡∂˙∆‰”Ô

#define LANG_Dutch			HKC_LANG_Dutch//∫…¿º”Ô
#define LANG_TraChinese		HKC_LANG_TraChinese//∑±ÃÂ÷–Œƒ

#define LANG_Nums			HKC_LANG_Nums

//----------------------------------------------
typedef enum
{ 
  MIA_Nothing,
  MIA_GotoNext, // goto next
  MIA_GotoPrev, // goto prev
  MIA_PrevItem,
  MIA_NextItem,
  MIA_JumpNaviItem,
  MIA_JumpAdjustItem,
  MIA_GotoPrevL1,
  MIA_Exit, // Exit
  MIA_IncValue, // increase value
  MIA_DecValue, // decrease value
  MIA_ExecFunc,
  MIA_Auto,
  MIA_InvIncValue,
  MIA_InvDecValue,
  MIA_RedrawMenu,

  #if AudioFunc
  MIA_HotVolume,
  MIA_HotMute,
  #endif
  #if  LowBlueLight
   MIA_HotLowBlueLightKey,
   #endif
  MIA_HotDCR,
  MIA_HotBrightness,
  MIA_HOT43CHING169,
  MIA_HotContrast,
  
  MIA_SwitchInput,
  #if Enable_Menu_LockOSD
  MIA_Menu_LockKey, 
  #endif
  #if Enable_LockPowerKey
  MIA_power_LockKey, 
  #endif
#if KeyInputSelectMenu
  MIA_HotInput,
#endif
  #if Los_Select
  MIA_HotCross,
  #endif
#if EN_HotKeyTimes
   MIA_HotTimes,
    MIA_HotShowTimes,
#endif 
#if EN_HotKeyXINGZHUN
	  MIA_HotXINGZHUN,
	  MIA_HotXINGZHUN1,
	  MIA_HotXINGZHUN2,
	  MIA_HotXINGZHUN3,
	 MIA_HotXINGZHUN4,
	 MIA_HotXINGZHUN5,
	 MIA_HotXINGZHUN6,
#endif


  MIA_Power // power control
  
} MenuItemActionType;


typedef enum
{ 
  DWI_Nothing,
  DWI_Icon,
  DWI_Text,
  DWI_3x2Icon_2,
  DWI_4x3Icon,
  #if Los_Select
  DWI_5x3Icon,
  #endif
  DWI_FullText,
  DWI_CenterText,
  DWI_FullCenterText,
  DWI_CenterPropText,
  DWI_None
} DrawMenuItemType;

typedef BYTE *( *fpDisplayText )( void );
typedef Bool( *fpExecFunc )( void );
typedef BYTE( *fpItemIndex )( void );
typedef Bool( *fpAdjustValue )( MenuItemActionType action );
typedef WORD( *fpGetValue )( void );
typedef WORD( *fpGetMaxValue )( void );
typedef WORD( *fpSetMaxValue )( void );
#define mpbStay         0x001 // for stay in menu page
#define mpbInvisible    0x002 // menu page visible or not
#define mpbItemFrame	0x04
#define mpbDrawFrame	0x10

#define mpbBrowse       0x200
#define mpbAdjust       0x008
#define mpbRedraw       0x100
#define mpbMoving       0x020
#define mpbCenter       0x040
#define mpbLogoFrame    0x080

#define mibSelectable   0x01 // for menu item selectable
#define mibInvisible    0x02 // for menu item selectable
#define mibDVIDisable   0x04
#define mibFactoryItem  0x08


#define mibJustShow  0x10
#define mibsRGBDisable 	0x20
#define mibDrawValue    0x40
#define mibmissSelectable                     0x80 



#define mibStdEnable    0x0400
#define mibDCRDisable   0x0800
#define mibDLCDisable           0x1000
#define mibBFEnable             0x4000
#define mibDemoDisable      	0x8000
#if Enable_Expansion
#define mibExpansionDisable4_3    			0x10000
#define mibExpansionDisable16_9     		0x20000
#define mibExpansionDisable1_1     			0x40000
#define mibExpansionDisableFixInputRatio    0x80000
#endif


#define dwiNumber                   0x01
#define dwiGauge	    	     0x02
#define dwiRadioGroup               0x04
#define dwiEnd                      0x08
#define dwiHex                          0x10
#define dwiOptionBar                    0x20
#define dwiUserColor   			        0x40
#define dwiVideoOption   0x80

//#define dwiRightAlign                   0x80
#define dwiCenterArrowAlign          0x100
#define dwiHotKeyCenterArrowAlign   0x200

typedef struct
{
    fpAdjustValue AdjustFunction;
    fpExecFunc ExecFunction;
} MenuItemFuncType;
#if Multi_Number //multi-number

typedef struct
{
    BYTE Flags;
    BYTE XPos, YPos;
    fpGetValue GetValue;
} NumberType;

typedef struct
{
    // BYTE Flags;
    BYTE ForeColor, BackColor;
    NumberType *DisplayNumber;
} DrawNumberType;
#else // single number
typedef struct
{
    BYTE Flags;
    BYTE ForeColor, BackColor;
    BYTE XPos, YPos;
    fpGetValue GetValue;
} DrawNumberType;
#endif
#if Multi_Gauge // multi-gauge
typedef struct
{
    BYTE Flags;
    BYTE XPos, YPos;
    fpGetValue GetValue;
} GaugeType;
typedef struct
{
    //BYTE Flags;
    BYTE ForeColor, BackColor;
    BYTE Length;
    GaugeType *DisplayGauge;
} DrawGuageType;
#else // single gauge
typedef struct
{
    BYTE Flags;
    BYTE ForeColor, BackColor;
    BYTE Length;
    BYTE XPos, YPos;
    fpGetValue GetValue;
} DrawGuageType;
#endif
typedef struct
{
    WORD Flags;
    BYTE XPos, YPos;
    fpDisplayText DisplayText;
} RadioTextType;
typedef struct
{
    // BYTE Flags;
    BYTE ForeColor, BackColor;
    fpGetValue GetValue;
    RadioTextType *RadioText;
} DrawRadioGroupType;
typedef struct
{
    DrawNumberType *DrawNumber;
    DrawGuageType *DrawGuage;
    DrawRadioGroupType *DrawRadioGroup;
} DrawValueType;
typedef struct
{
    BYTE *Fonts;
    WORD FontCount;
} MenuFontType;
#if 0
typedef struct
{
    BYTE XPos, YPos;
    BYTE ForeColor, BackColor;
    BYTE SelForeColor, SelBackColor;
    BYTE NextPage;
    DrawMenuItemType DrawItemMethod;
    fpDisplayText DisplayText;
    MenuItemActionType *KeyEvents;
    MenuItemFuncType KeyFunction;
    DrawValueType DisplayValue;
    BYTE Flags;
} MenuItemType;
#endif
typedef struct
{
    BYTE XPos, YPos;
    BYTE ForeColor, BackColor;
    BYTE SelForeColor, SelBackColor;
    BYTE NextPage;
    DrawMenuItemType DrawItemMethod;
    fpDisplayText DisplayText;
    MenuItemActionType *KeyEvents;
    MenuItemFuncType KeyFunction;
    DrawValueType DisplayValue;
    MenuFontType *Fonts;
    DWORD Flags; // 2006/9/6 5:41PM by Emily BYTE Flags;
} MenuItemType;
#if 0
typedef struct
{
    BYTE SpaceWidth;
    WORD LineData[18];
} PropFontType;
#endif
#if 0
typedef struct
{
    BYTE XSize, YSize;
    BYTE PrevPage;
    MenuItemType *MenuItems;
    BYTE ItemCount;
    fpExecFunc ExecFunction;
    BYTE Flags;
} MenuPageType;
#endif
typedef struct
{
    BYTE XSize, YSize;
    BYTE PrevPage;
    MenuItemType *MenuItems;
    BYTE ItemCount;
    fpExecFunc ExecFunction;
    MenuFontType *Fonts;
    WORD Flags;
} MenuPageType;
#endif
