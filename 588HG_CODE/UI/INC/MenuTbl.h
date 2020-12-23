#include "MenuFontTbl.h"
#include "MenuTblDef.h"

//ccflag add
#if(DisplayLogo==TERRA)
#define LOGO_WIGHT     47
#define LOGO_HIGHT      11 
#elif (DisplayLogo==MstarLOGO)
#define LOGO_WIGHT     42
#define LOGO_HIGHT      11 
#elif(DisplayLogo==NANCHANG)
#define LOGO_WIGHT    25
#define LOGO_HIGHT     25
#elif(DisplayLogo==EPAI)
#define LOGO_WIGHT    42
#define LOGO_HIGHT     11
#elif(DisplayLogo==MAYA)
#define LOGO_WIGHT    42
#define LOGO_HIGHT     11
#elif(DisplayLogo==IMAGIC)
#define LOGO_WIGHT    42
#define LOGO_HIGHT     11
#elif(DisplayLogo==FANGZHENG)
#define LOGO_WIGHT    75
#define LOGO_HIGHT     11
#elif(DisplayLogo==GRC)
#define LOGO_WIGHT    42
#define LOGO_HIGHT     11
#elif(DisplayLogo==BUBALUS)
#define LOGO_WIGHT    75
#define LOGO_HIGHT     11
#elif(DisplayLogo==BUBALUSWHITE)
#define LOGO_WIGHT    75
#define LOGO_HIGHT     11
#elif(DisplayLogo==SAMPIN)
#define LOGO_WIGHT    58
#define LOGO_HIGHT     11
#elif(DisplayLogo==PCTOP)
#define LOGO_WIGHT    58
#define LOGO_HIGHT     16
#elif(DisplayLogo==FOXIN)
#define LOGO_WIGHT    42
#define LOGO_HIGHT     11
#elif(DisplayLogo==ZEBION)
#define LOGO_WIGHT    58
#define LOGO_HIGHT     11
#elif(DisplayLogo==SUGON)
#define LOGO_WIGHT    42
#define LOGO_HIGHT     11
#elif(DisplayLogo==SONGUAN)
#define LOGO_WIGHT    42
#define LOGO_HIGHT     21
#elif(DisplayLogo==INFORCE)
#define LOGO_WIGHT    42
#define LOGO_HIGHT     11
#elif(DisplayLogo==MAGIX)
#define LOGO_WIGHT    58
#define LOGO_HIGHT     11
#elif(DisplayLogo==DRAZILPC)
#define LOGO_WIGHT    58
#define LOGO_HIGHT     11
#elif(DisplayLogo==MEGAONIC)
#define LOGO_WIGHT    75
#define LOGO_HIGHT     10



#else
#define LOGO_WIGHT   42// 0
#define LOGO_HIGHT    11// 0
#endif

#if SevenKey_Function
//-----------------------------------------------7key
MenuItemActionType code RootKeyEvent[BTN_EndBTN]=
{
MIA_HOT43CHING169, 		//BTN_Plus
#if AudioFunc
MIA_HotVolume,		//BTN_Minus 
#else
MIA_Nothing,		//BTN_Minus 
#endif
MIA_GotoNext, 	//BTN_Menu
MIA_Auto,		//BTN_Exit
MIA_Power,		// BTN_Power
MIA_Nothing,		//BTN_IncVal
MIA_Nothing,		//BTN_DecVal
};
MenuItemActionType code OsdLockedKeyEvent[BTN_EndBTN]=
{
MIA_Nothing,  
MIA_Nothing,  
MIA_Exit,		//BTN_Exit
MIA_GotoNext, 	//BTN_Menu
MIA_Power,		// BTN_Power
MIA_Nothing,		//BTN_DecVal
MIA_Nothing,		//BTN_IncVal
};
MenuItemActionType code MainMenuKeyEvent[BTN_EndBTN]=
{ 
  MIA_PrevItem,	// BTN_Plus
  MIA_NextItem,	// BTN_Minus
  MIA_GotoNext,	// BTN_Menu
  MIA_GotoPrev,	// BTN_Exit
  MIA_Power, 	// BTN_Power    
  MIA_Nothing,	// BTN_IncVal   
  MIA_Nothing	// BTN_DecVal  
};
MenuItemActionType code SubMenuKeyEvent[BTN_EndBTN]=
{ 
  MIA_PrevItem,	// BTN_Plus
  MIA_NextItem,	// BTN_Minus
  MIA_JumpAdjustItem,	// BTN_Menu
  MIA_GotoPrevL1,	// BTN_Exit
  MIA_Power, 	// BTN_Power    
  MIA_Nothing,	// BTN_IncVal   
  MIA_Nothing	// BTN_DecVal  
};
MenuItemActionType code Adjuster2KeyEvent[BTN_EndBTN]=
{ 
  MIA_Nothing,	//    BTN_Plus
  MIA_Nothing,	//   BTN_Minus
  MIA_NextItem,	// BTN_Menu
  MIA_GotoPrevL1,		// BTN_Exit    
  MIA_Power, 	// BTN_Power    
  MIA_IncValue,		// BTN_IncVal
  MIA_DecValue,		// BTN_DecVal

};
#if Los_Select
MenuItemActionType code HotCorssMenuKeyEvent1[BTN_EndBTN]=
{ 
  MIA_Exit,	// BTN_Plus
  MIA_Exit,	// BTN_Minus
  MIA_Exit,	// BTN_Menu
  MIA_Exit,	// BTN_Exit
  MIA_Power, 	// BTN_Power    
  MIA_Nothing,	// BTN_IncVal   
  MIA_Nothing	// BTN_DecVal  
};
#endif

MenuItemActionType code SetAdjustKeyEvent[BTN_EndBTN]=
{ 
  MIA_PrevItem,	// BTN_Plus
  MIA_NextItem,	// BTN_Minus
  MIA_ExecFunc,	// BTN_Menu
  MIA_GotoPrevL1,	// BTN_Exit  
  MIA_Power, 	// BTN_Power    
  MIA_Nothing,	// BTN_IncVal   
  MIA_Nothing	// BTN_DecVal  
};
MenuItemActionType code SubMenu2KeyEvent[BTN_EndBTN]=
{ 
  MIA_PrevItem,	// BTN_Plus
  MIA_NextItem,	// BTN_Minus
  MIA_GotoNext,	// BTN_Menu
  MIA_GotoPrevL1,	// BTN_Exit  
  MIA_Power, 	// BTN_Power    
  MIA_Nothing,	// BTN_IncVal   
  MIA_Nothing	// BTN_DecVal  

};
MenuItemActionType code SubMenuAdjustKeyEvent[BTN_EndBTN]=
{ 
  MIA_Nothing,	//   BTN_Plus 
  MIA_Nothing,	//   BTN_Minus
  MIA_JumpNaviItem,	// BTN_Menu
  MIA_JumpNaviItem,	// BTN_Exit
  MIA_Power, 	// BTN_Power    
  MIA_IncValue,	//  BTN_IncVal
  MIA_DecValue,	//  BTN_DecVal

};
MenuItemActionType code SubMenuAdjustKeyEvent_H[BTN_EndBTN]=
{ 
  MIA_Nothing,	//   BTN_Plus 
  MIA_Nothing,	//   BTN_Minus
  MIA_JumpNaviItem,	// BTN_Menu
  MIA_JumpNaviItem,	// BTN_Exit
  MIA_Power, 	// BTN_Power    
  MIA_DecValue,	//  BTN_DecVal
  MIA_IncValue,	//  BTN_IncVal
};

MenuItemActionType code SwitchInputKeyEvent[BTN_EndBTN]=
{ 
  MIA_Nothing,	//    BTN_Plus
  MIA_Nothing,	//   BTN_Minus
  MIA_ExecFunc,	// BTN_Menu
  MIA_JumpNaviItem,	// BTN_Exit
  MIA_Power, 	// BTN_Power    
  MIA_IncValue,	// BTN_IncVal
  MIA_DecValue,	// BTN_DecVal

};

MenuItemActionType code LanguageMenuAdjustKeyEvent[BTN_EndBTN]=
{ 
  MIA_PrevItem,	// BTN_Plus
  MIA_NextItem,	// BTN_Minus
  MIA_ExecFunc,	// BTN_Menu
  MIA_GotoPrev,	// BTN_Exit  
  MIA_Power, 	// BTN_Power    
  MIA_Nothing,	// BTN_IncVal   
  MIA_Nothing	// BTN_DecVal  
};

MenuItemActionType code LogoKeyEvent[BTN_EndBTN]=
{ MIA_Nothing, 	// BTN_Plus 
  MIA_Nothing, 	// BTN_Minus
  MIA_Nothing, 	// BTN_Menu
  MIA_Nothing,	 // BTN_Exit
  MIA_Power, 		// BTN_Power
  MIA_Nothing,	//BTN_IncVal
  MIA_Nothing,	//BTN_DecVal
};

MenuItemActionType code NothingKeyEvent[BTN_EndBTN]=
{ 
  MIA_Nothing, 	// BTN_Plus 
  MIA_Nothing, 	// BTN_Minus
  MIA_Nothing,	// BTN_Menu
  MIA_Nothing, 	// BTN_Exit
  MIA_Power, 		// BTN_Power
  MIA_Nothing, 	// BTN_IncVal
  MIA_Nothing,	//BTN_DecVal
};
MenuItemActionType code NothingKeyEvent1[BTN_EndBTN]=
{ MIA_Nothing, 	// BTN_Plus
  MIA_Nothing,	 // BTN_Minus
  MIA_Nothing, 	// BTN_Menu
  MIA_Nothing,	 // BTN_Exit
  MIA_Power, 		// BTN_Power
  MIA_Nothing, 	//BTN_IncVal
  MIA_Nothing,	//BTN_DecVal
};
MenuItemActionType code HotMenuKeyEvent[BTN_EndBTN]=
{ 
   MIA_GotoNext,		// BTN_Plus
   MIA_GotoNext, 		// BTN_Minus
   MIA_GotoNext,		// BTN_Menu
   MIA_Exit,			// BTN_Exit	
   MIA_Power, 		// BTN_Power
   MIA_Nothing, 		//BTN_IncVal
   MIA_Nothing,		//BTN_DecVal
};
MenuItemActionType code AdjusterKeyEvent[BTN_EndBTN]=
{ 
   MIA_Nothing, 		// BTN_Plus
   MIA_Nothing,		// BTN_Minus
   MIA_GotoPrev,	       // BTN_Menu
   MIA_GotoPrev,		// BTN_Exit
   MIA_Power, 		// BTN_Power
   MIA_IncValue,		//  BTN_IncVal
   MIA_DecValue,		//  BTN_DecVal

};
//------------------------------------------------------------
MenuItemActionType code HOTExecKeyEvent[BTN_EndBTN]=
{ 
   MIA_PrevItem, 	// BTN_Plus
   MIA_NextItem, 	// BTN_Minus
   MIA_ExecFunc, 	// BTN_Menu
   MIA_GotoPrev,     // BTN_Exit 
   MIA_Power, 		// BTN_Power
   MIA_Nothing, 		//BTN_IncVal
   MIA_Nothing,		//BTN_DecVal
};
MenuItemActionType code NaviExecKeyEvent[BTN_EndBTN]=
{ 
   MIA_NextItem, 	// BTN_Plus
   MIA_PrevItem, 	// BTN_Minus
   MIA_ExecFunc, 	// BTN_Menu
   MIA_GotoPrev,     // BTN_Exit 
   MIA_Power, 		// BTN_Power
   MIA_Nothing, 		//BTN_IncVal
   MIA_Nothing,		//BTN_DecVal
};
MenuItemActionType code FacNaviKeyEvent[BTN_EndBTN]=
{ 
   MIA_NextItem , 	// BTN_Plus
   MIA_PrevItem, 	// BTN_Minus
   MIA_GotoNext,		// BTN_Menu
   MIA_GotoPrev,	 	// BTN_Exit
   MIA_Power, 		// BTN_Power
   MIA_Nothing, 		//BTN_IncVal
   MIA_Nothing,		//BTN_DecVal
  };
MenuItemActionType code FactoryNaviKeyEvent[BTN_EndBTN]=
{ 
  MIA_NextItem, 			// BTN_Plus
  MIA_PrevItem, 			// BTN_Minus
  MIA_JumpAdjustItem,		// BTN_Menu
  MIA_Exit,			      // BTN_Exit 
  MIA_Power, 				// BTN_Power
  MIA_Nothing,			//BTN_IncVal
  MIA_Nothing,			//BTN_DecVal
};

MenuItemActionType code FactoryAdjustKeyEvent[BTN_EndBTN]=
{ 
  MIA_Nothing,		//   BTN_Plus 
  MIA_Nothing,		// BTN_Minus
  MIA_JumpNaviItem,//MIA_JumpAdjustItem,		// BTN_Menu
  MIA_GotoPrev,	 	// BTN_Exit
  MIA_Power, 		// BTN_Power    
  MIA_IncValue, 	//  BTN_BSel
  MIA_DecValue, 	//  BTN_Ext7

  };

#else
//-----------------------------------------------5key
MenuItemActionType code RootKeyEvent[BTN_EndBTN]=
{
#if 1
  #if Customer_Type_Sel==Customer_HONGXUN
  	MIA_HotBrightness,
  #else
  	MIA_Nothing,
  #endif
#if (Customer_Type_Sel==Customer_XINTAO)
	MIA_HotInput,//MIA_HotBrightness, 
#elif Customer_Type_Sel==Customer_HONGXUN
	MIA_Auto,			//BTN_Exit
#else
	MIA_HotBrightness,
#endif
#if Customer_Type_Sel==Customer_HONGXUN
	MIA_HotInput,//MIA_HotBrightness, 
#else
	MIA_HotInput,//MIA_Auto,			//BTN_Exit
#endif
	MIA_GotoNext, 	//BTN_Menu
	MIA_Power,		// BTN_Power
	MIA_Nothing,		//BTN_DecVal
	MIA_Nothing,		//BTN_IncVal

#else
#if EN_HotKeyXINGZHUN
	MIA_HotXINGZHUN,
#elif EN_HotKeyTimes
	MIA_HotTimes,
#elif MainBoardType==MainBoard_TSUML88CMC2
	MIA_HotVolume,//MIA_Nothing, //MIA_HotVolume,		//BTN_Minus 
#else
	MIA_Nothing,
#endif
	MIA_HotBrightness,  
	MIA_HotInput,			//BTN_Exit
	MIA_GotoNext, 	//BTN_Menu
	MIA_Power,		// BTN_Power
	MIA_Nothing,		//BTN_DecVal
	MIA_Nothing,		//BTN_IncVal
#endif
};
MenuItemActionType code OsdLockedKeyEvent[BTN_EndBTN]=
{
MIA_Nothing,  
MIA_Nothing,  
MIA_Exit,		//BTN_Exit
MIA_GotoNext, 	//BTN_Menu
MIA_Power,		// BTN_Power
MIA_Nothing,		//BTN_DecVal
MIA_Nothing,		//BTN_IncVal
};

MenuItemActionType code MainMenuKeyEvent[BTN_EndBTN]=
{ 
  MIA_PrevItem,	// BTN_Plus
  MIA_NextItem,	// BTN_Minus
  MIA_GotoPrev,	// MIA_GotoNext,	// BTN_Menu
  MIA_GotoNext,	//  MIA_GotoPrev,	// BTN_Exit
  MIA_Power, 	// BTN_Power    
  MIA_Nothing,	// BTN_IncVal   
  MIA_Nothing	// BTN_DecVal  
};
MenuItemActionType code SubMenuKeyEvent[BTN_EndBTN]=
{ 
  MIA_PrevItem,	// BTN_Plus
  MIA_NextItem,	// BTN_Minus
  MIA_GotoPrevL1,	//MIA_JumpAdjustItem,	// BTN_Menu
 MIA_JumpAdjustItem,	//  MIA_GotoPrevL1,	// BTN_Exit
  MIA_Power, 	// BTN_Power    
  MIA_Nothing,	// BTN_IncVal   
  MIA_Nothing	// BTN_DecVal  
};
MenuItemActionType code Adjuster2KeyEvent[BTN_EndBTN]=
{ MIA_IncValue,		// BTN_Plus
  MIA_DecValue,		// BTN_Minus
  MIA_NextItem,	// BTN_Menu
  MIA_GotoPrevL1,		// BTN_Exit    
  MIA_Power, 	// BTN_Power    
  MIA_Nothing,	// BTN_IncVal   
  MIA_Nothing	// BTN_DecVal  
};
MenuItemActionType code SetAdjustKeyEvent[BTN_EndBTN]=
{ 
  MIA_PrevItem,	// BTN_Plus
  MIA_NextItem,	// BTN_Minus
 MIA_GotoPrevL1,	// MIA_ExecFunc,	// BTN_Menu
 MIA_ExecFunc,	// MIA_GotoPrevL1,	// BTN_Exit  
  MIA_Power, 	// BTN_Power    
  MIA_Nothing,	// BTN_IncVal   
  MIA_Nothing	// BTN_DecVal  
};
MenuItemActionType code SubMenu2KeyEvent[BTN_EndBTN]=
{ 
  MIA_PrevItem,	// BTN_Plus
  MIA_NextItem,	// BTN_Minus
 MIA_GotoPrevL1,	// MIA_GotoNext,	// BTN_Menu
  MIA_GotoNext,	//MIA_GotoPrevL1,	// BTN_Exit  
  MIA_Power, 	// BTN_Power    
  MIA_Nothing,	// BTN_IncVal   
  MIA_Nothing	// BTN_DecVal  

};
MenuItemActionType code SubMenuAdjustKeyEvent[BTN_EndBTN]=
{ 
  MIA_IncValue,	// BTN_Plus
  MIA_DecValue,	// BTN_Minus
  MIA_JumpNaviItem,	// BTN_Menu
  MIA_JumpNaviItem,	// BTN_Exit
  MIA_Power, 	// BTN_Power    
  MIA_Nothing,	// BTN_IncVal   
  MIA_Nothing	// BTN_DecVal  
};
MenuItemActionType code SubMenuAdjustKeyEvent_H[BTN_EndBTN]=
{ 
  MIA_DecValue,	// BTN_Minus
  MIA_IncValue,	// BTN_Plus
  MIA_JumpNaviItem,	// BTN_Menu
  MIA_JumpNaviItem,	// BTN_Exit
  MIA_Power, 	// BTN_Power    
  MIA_Nothing,	// BTN_IncVal   
  MIA_Nothing	// BTN_DecVal  
};
MenuItemActionType code SwitchInputKeyEvent[BTN_EndBTN]=
{ 
  MIA_IncValue,	// BTN_Plus
  MIA_DecValue,	// BTN_Minus
  MIA_JumpNaviItem,	//MIA_ExecFunc,	// BTN_Menu
  MIA_ExecFunc,	// MIA_JumpNaviItem,	// BTN_Exit
  MIA_Power, 	// BTN_Power    
  MIA_Nothing,	// BTN_IncVal   
  MIA_Nothing	// BTN_DecVal  
};

#if Los_Select
//5 key
MenuItemActionType code HotCorssMenuKeyEvent1[BTN_EndBTN]=
{ 
  MIA_Exit,		// BTN_Plus
  MIA_Exit,		// BTN_Minus
  MIA_GotoNext,		// BTN_Menu
  MIA_GotoPrev,		// BTN_Exit
  MIA_Power, 		// BTN_Power  
  MIA_Nothing,	// BTN_IncVal   
  MIA_Nothing		// BTN_DecVal  
};
#endif

MenuItemActionType code LanguageMenuAdjustKeyEvent[BTN_EndBTN]=
{ 
  MIA_PrevItem,	// BTN_Plus
  MIA_NextItem,	// BTN_Minus
 MIA_GotoPrev,	//  MIA_ExecFunc,	// BTN_Menu
 MIA_ExecFunc,	// MIA_GotoPrev,	// BTN_Exit  
  MIA_Power, 	// BTN_Power    
  MIA_Nothing,	// BTN_IncVal   
  MIA_Nothing	// BTN_DecVal  
};

MenuItemActionType code LogoKeyEvent[BTN_EndBTN]=
{ MIA_Nothing, 	// BTN_Plus 
  MIA_Nothing, 	// BTN_Minus
  MIA_Nothing, 	// BTN_Menu
  MIA_Nothing,	 // BTN_Exit
  MIA_Power, 		// BTN_Power
  MIA_Nothing,	//BTN_IncVal
  MIA_Nothing,	//BTN_DecVal
};

MenuItemActionType code NothingKeyEvent[BTN_EndBTN]=
{ 
  MIA_Nothing, 	// BTN_Plus 
  MIA_Nothing, 	// BTN_Minus
  MIA_Nothing,	// BTN_Menu
  MIA_Nothing, 	// BTN_Exit
  MIA_Power, 		// BTN_Power
  MIA_Nothing, 	// BTN_IncVal
  MIA_Nothing,	//BTN_DecVal
};
MenuItemActionType code NothingKeyEvent1[BTN_EndBTN]=
{ MIA_Nothing, 	// BTN_Plus
  MIA_Nothing,	 // BTN_Minus
  MIA_Nothing, 	// BTN_Menu
  MIA_Nothing,	 // BTN_Exit
  MIA_Power, 		// BTN_Power
  MIA_Nothing, 	//BTN_IncVal
  MIA_Nothing,	//BTN_DecVal
};




MenuItemActionType code HotMenuKeyEvent[BTN_EndBTN]=
{ 
   MIA_GotoNext,		// BTN_Plus
   MIA_GotoNext, 		// BTN_Minus
   MIA_GotoNext,		// BTN_Menu
   MIA_Exit,			// BTN_Exit	
   MIA_Power, 		// BTN_Power
   MIA_Nothing, 		//BTN_IncVal
   MIA_Nothing,		//BTN_DecVal
};
MenuItemActionType code AdjusterKeyEvent[BTN_EndBTN]=
{ 
   MIA_IncValue,		// BTN_Plus
   MIA_DecValue,		// BTN_Minus
   MIA_GotoPrev,	// BTN_Menu
   MIA_GotoPrev,		// BTN_Exit
   MIA_Power, 		// BTN_Power
   MIA_Nothing, 		//BTN_IncVal
   MIA_Nothing,		//BTN_DecVal
};
//------------------------------------------------------------
MenuItemActionType code HOTExecKeyEvent[BTN_EndBTN]=
{ 
   MIA_PrevItem, 	// BTN_Plus
   MIA_NextItem, 	// BTN_Minus
  MIA_GotoPrev,     // MIA_ExecFunc, 	// BTN_Menu
  MIA_ExecFunc, 	//  MIA_GotoPrev,     // BTN_Exit 
   MIA_Power, 		// BTN_Power
   MIA_Nothing, 		//BTN_IncVal
   MIA_Nothing,		//BTN_DecVal
};
#if EN_HotKeyTimes
MenuItemActionType code HOTTimesExecKeyEvent[BTN_EndBTN]=
{ 
   MIA_PrevItem, 	// BTN_Plus
   MIA_NextItem, 	// BTN_Minus
  MIA_GotoPrev,  //  MIA_HotShowTimes,// BTN_Menu
   MIA_HotShowTimes,//    MIA_GotoPrev,  //      // BTN_Exit 
   MIA_Power, 		// BTN_Power
   MIA_Nothing, 		//BTN_IncVal
   MIA_Nothing,		//BTN_DecVal
};
#endif
#if 0//EN_HotKeyXINGZHUN
MenuItemActionType code HOXZExecKeyEvent[BTN_EndBTN]=
{ 
	MIA_PrevItem,	 // BTN_Plus
	MIA_NextItem,	 // BTN_Minus
   MIA_GotoPrev,//MIA_ExecFunc, 	// BTN_Menu
   MIA_GotoPrev,  // MIA_ExecFunc, 	// MIA_HotShowTimes,// MIA_GotoPrev,     // BTN_Exit 
   MIA_Power, 		// BTN_Power
   MIA_Nothing, 		//BTN_IncVal
   MIA_Nothing,		//BTN_DecVal
};
#endif


MenuItemActionType code NaviExecKeyEvent[BTN_EndBTN]=
{ 
   MIA_PrevItem,//MIA_NextItem, 	// BTN_Plus
   MIA_NextItem,//MIA_PrevItem, 	// BTN_Minus
   MIA_GotoPrev,//MIA_ExecFunc, 	// BTN_Menu
   MIA_ExecFunc,//MIA_GotoPrev,     // BTN_Exit 
   MIA_Power, 		// BTN_Power
   MIA_Nothing, 		//BTN_IncVal
   MIA_Nothing,		//BTN_DecVal
};
MenuItemActionType code FacNaviKeyEvent[BTN_EndBTN]=
{ 
   MIA_PrevItem,//MIA_NextItem, 	// BTN_Plus
   MIA_NextItem,//MIA_PrevItem, 	// BTN_Minus
   MIA_GotoNext,		// BTN_Menu
   MIA_GotoPrev,	 	// BTN_Exit
   MIA_Power, 		// BTN_Power
   MIA_Nothing, 		//BTN_IncVal
   MIA_Nothing,		//BTN_DecVal
  };
MenuItemActionType code FactoryNaviKeyEvent[BTN_EndBTN]=
{ 
   MIA_PrevItem,//MIA_NextItem, 	// BTN_Plus
   MIA_NextItem,//MIA_PrevItem, 	// BTN_Minus
  MIA_Exit,//MIA_JumpAdjustItem,		// BTN_Menu
  MIA_JumpAdjustItem,//MIA_Exit,			      // BTN_Exit 
  MIA_Power, 				// BTN_Power
  MIA_Nothing,			//BTN_IncVal
  MIA_Nothing,			//BTN_DecVal
};

MenuItemActionType code FactoryAdjustKeyEvent[BTN_EndBTN]=
{ 
  MIA_IncValue, 	// BTN_Plus
  MIA_DecValue, 	// BTN_Minus
  MIA_JumpNaviItem,	// MIA_JumpAdjustItem,		// BTN_Menu //í?3??ü
  MIA_JumpNaviItem,	// MIA_GotoPrev,	 	// BTN_Exit //è·?¨?ü
  MIA_Power, 		// BTN_Power    
  MIA_Nothing,		// BTN_BSel   
  MIA_Nothing		// BTN_Ext7
  };
#endif

//============================================================
RadioTextType code DrawBrightnessTextRatio[]=
{ // Flags,					XPos,	YPos,	DisplayText
  {dwiEnd|DWI_CenterText,					1, 		1,		MainMenu_BrightText}
};
DrawRadioGroupType code DrawBrightnessTextRatioGroup[]=
{ //ForeColor,				BackColor,				GetValue	RadioText
  {0x07, CPC_HKCBlackColor, NULL,		DrawBrightnessTextRatio}
};
//----------
RadioTextType code DrawImageTextRatio[]=
{ // Flags,					XPos,	YPos,	DisplayText
  {dwiEnd|DWI_CenterText,					1, 		1,		MainMenu_ImageText}
};
DrawRadioGroupType code DrawImageTextRatioGroup[]=
{ //ForeColor,				BackColor,				GetValue	RadioText
  {0x07, CPC_HKCBlackColor, NULL,		DrawImageTextRatio}
};
//----------
RadioTextType code DrawColorTextRatio[]=
{ // Flags,					XPos,	YPos,	DisplayText
  {dwiEnd|DWI_CenterText,					1, 		1,		MainMenu_ColorText}
};
DrawRadioGroupType code DrawColorTextRatioGroup[]=
{ //ForeColor,				BackColor,				GetValue	RadioText
  {0x07, CPC_HKCBlackColor, NULL,		DrawColorTextRatio}
};
//----------
RadioTextType code DrawOSDTextRatio[]=
{ // Flags,					XPos,	YPos,	DisplayText
  {dwiEnd|DWI_CenterText,					1, 		1,		MainMenu_OSDText}
};
DrawRadioGroupType code DrawOSDTextRatioGroup[]=
{ //ForeColor,				BackColor,				GetValue	RadioText
  {0x07, CPC_HKCBlackColor, NULL,		DrawOSDTextRatio}
};
//----------
RadioTextType code DrawResetTextRatio[]=
{ // Flags,					XPos,	YPos,	DisplayText
  {dwiEnd|DWI_CenterText,					1, 		1,		MainMenu_ResetText}
};
DrawRadioGroupType code DrawResetTextRatioGroup[]=
{ //ForeColor,				BackColor,				GetValue	RadioText
  {0x07, CPC_HKCBlackColor, NULL,		DrawResetTextRatio}
};
//----------
RadioTextType code DrawMiscTextRatio[]=
{ // Flags,					XPos,	YPos,	DisplayText
  {dwiEnd|DWI_CenterText,					1, 		1,		MainMenu_MiscText}
};
DrawRadioGroupType code DrawMiscTextRatioGroup[]=
{ //ForeColor,				BackColor,				GetValue	RadioText
  {0x07, CPC_HKCBlackColor, NULL,		DrawMiscTextRatio}
};

//=========== MainMenuItems ================================
MenuItemType code MainMenuItems[]=
{
    // 1Brightness
  { MainMenuIcon_DrawXPos,MainMenuIcon_DrawYPos,		// XPos, YPos;
    7, Color_8, // ForeColor, BackColor;
    6, Color_8, // SelForeColor, SelBackColor;
    OSD_BriContrastMenu, 			// NextMenuPage;
    DWI_3x2Icon_2,						// DrawMenuItemType;
    BrightnessIcon, 						// DisplayText;
    MainMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     DrawBrightnessTextRatioGroup			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable 						// Flags
  },
   // 2 Image
  { MainMenuIcon_DrawXPos,MainMenuIcon_DrawYPos+2*1,		// XPos, YPos;
    0x05, Color_4, // ForeColor, BackColor;
    0x04, Color_4, // SelForeColor, SelBackColor;
    PictureMenu, 			// NextMenuPage;
    DWI_3x2Icon_2,						// DrawMenuItemType;
    ImageIcon, 						// DisplayText;
    MainMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     DrawImageTextRatioGroup			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable 						// Flags
  },
   // 3  ColorTemp
  { MainMenuIcon_DrawXPos,MainMenuIcon_DrawYPos+2*2,		// XPos, YPos;
    9, Color_8, // ForeColor, BackColor;
    8, Color_8, // SelForeColor, SelBackColor;
    RGBColorMenu, 			// NextMenuPage;
    DWI_3x2Icon_2,						// DrawMenuItemType;
    ColorTempIcon, 						// DisplayText;
    MainMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     DrawColorTextRatioGroup			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable 						// Flags
  },
    //  4 OSD
  { MainMenuIcon_DrawXPos,MainMenuIcon_DrawYPos+2*3,		// XPos, YPos;
    11, Color_8, // ForeColor, BackColor;
    10, Color_8, // SelForeColor, SelBackColor;
    OsdMenu, 			// NextMenuPage;
    DWI_3x2Icon_2,						// DrawMenuItemType;
    OSDIcon, 						// DisplayText;
    MainMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     DrawOSDTextRatioGroup			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable 						// Flags
  },
    //5 Reset
  { MainMenuIcon_DrawXPos,MainMenuIcon_DrawYPos+2*4,		// XPos, YPos;
    0x07, Color_4, // ForeColor, BackColor;
    0x06, Color_4, // SelForeColor, SelBackColor;
    SettingMenu, 			// NextMenuPage;
    DWI_3x2Icon_2,						// DrawMenuItemType;
    ResetIcon, 						// DisplayText;
    MainMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     DrawResetTextRatioGroup			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable 						// Flags
  },
    // 6 Misc
  { MainMenuIcon_DrawXPos,MainMenuIcon_DrawYPos+2*5,		// XPos, YPos;
    0x09, Color_4, // ForeColor, BackColor;
    0x08, Color_4, // SelForeColor, SelBackColor;
    OSD_MiscMenu, 			// NextMenuPage;
    DWI_3x2Icon_2,						// DrawMenuItemType;
    MiscIcon, 						// DisplayText;
    MainMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     DrawMiscTextRatioGroup			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable 						// Flags
  },

};


//=========== BriContrastMenuItems ================================
//--------------------BrightnessNumber--------------------------
NumberType code DrawBriContrastMenu_BrightnessNum[] =
{
    // Flags,   XPos,   YPos,   GetValue
    { dwiNumber|dwiEnd, MenuNumber_DrawStarXPos,MenuNumber_DrawStarYPos+SubMenu_DrawYPos_Shift*0,	GetBrightnessValue}
};
DrawNumberType code DrawBriContrastMenu_BrightnessNumber[] =
{
    // F ForeColor, BackColor,  GetValue
  {0x0C,  0x0E,  DrawBriContrastMenu_BrightnessNum} 
};
//--------------------BrightnessIconGuge--------------------------
GaugeType code DrawBriContrastMenu_BrightnessGuagee[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
   {  dwiGauge|dwiEnd,	MenuGuage_DrawStarXPos,	  MenuGuage_DrawStarYPos+SubMenu_DrawYPos_Shift*0,	GetBrightnessValue  }
};
DrawGuageType code DrawBriContrastMenu_BrightnessGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
   {0x0C,  0x0E,	MenuGuage_DrawLength, DrawBriContrastMenu_BrightnessGuagee	}

};
//--------------------ContrastNumber--------------------------
NumberType code DrawBriContrastMenu_ContrastNum[] =
{
    // Flags,   XPos,   YPos,   GetValue
    { dwiNumber|dwiEnd, MenuNumber_DrawStarXPos,MenuNumber_DrawStarYPos+SubMenu_DrawYPos_Shift*1,	GetContrastValue}
};
DrawNumberType code DrawBriContrastMenu_ContrastNumber[] =
{
    // F ForeColor, BackColor,  GetValue
  {0x0C,  0x0E,  DrawBriContrastMenu_ContrastNum} 
};
//--------------------ContrastIconGuge--------------------------
GaugeType code DrawBriContrastMenu_ContrastGuagee[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
   {  dwiGauge|dwiEnd,	MenuGuage_DrawStarXPos,	  MenuGuage_DrawStarYPos+SubMenu_DrawYPos_Shift*1,	GetContrastValue  }
};
DrawGuageType code DrawBriContrastMenu_ContrastGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
   {0x0C,  0x0E,	MenuGuage_DrawLength, DrawBriContrastMenu_ContrastGuagee	}

};
//--------------------RadioText--------------------------
#if 1//MWE_AP_Enable
RadioTextType code DrawBriContrastMenu_ECORatio[]=
{ // Flags,					XPos,	YPos,	DisplayText
  {dwiEnd,	MenuRatio_DrawStarXPos,	  MenuRatio_DrawStarYPos+SubMenu_DrawYPos_Shift*2,		ECOModeText}
};
DrawRadioGroupType code DrawBriContrastMenu_ECORatioGroup[]=
{ //ForeColor,				BackColor,				GetValue	RadioText
  {0x0C,  0x0E,	NULL,	DrawBriContrastMenu_ECORatio}
};
#endif
//--------------------RadioText--------------------------
#if Enable_AdjustDCR
RadioTextType code DrawBriContrastMenu_DCRRatio[]=
{ // Flags,					XPos,	YPos,	DisplayText
  {dwiEnd,	MenuRatio_DrawStarXPos,	  MenuRatio_DrawStarYPos+SubMenu_DrawYPos_Shift*3,		DCRModeText}
};
DrawRadioGroupType code DrawBriContrastMenu_DCRRatioGroup[]=
{ //ForeColor,				BackColor,				GetValue	RadioText
  {0x0C,  0x0E,	NULL, DrawBriContrastMenu_DCRRatio}
};
#endif
#if OSD_OD
RadioTextType code DrawBriContrastMenu_ODRatio[]=
{ // Flags,					XPos,	YPos,	DisplayText
  {dwiEnd,	MenuRatio_DrawStarXPos,	  MenuRatio_DrawStarYPos+SubMenu_DrawYPos_Shift*(3+Enable_AdjustDCR),		ODModeText}
};
DrawRadioGroupType code DrawBriContrastMenu_ODRatioGroup[]=
{ //ForeColor,				BackColor,				GetValue	RadioText
  {0x0C,  0x0E,	NULL, DrawBriContrastMenu_ODRatio}
};
#endif

//----------------------------------------------
#if LowBlueLight
RadioTextType code DrawRGBMenu_BlueLightRatio[]=
{ // Flags,					XPos,	YPos,	DisplayText
  {dwiEnd,	MenuRatio_DrawStarXPos,	  MenuRatio_DrawStarYPos+SubMenu_DrawYPos_Shift*(3+Enable_AdjustDCR+OSD_OD),		BlueLightModeText}
};
DrawRadioGroupType code DrawRGBMenu_BlueLightRatioGroup[]=
{ //ForeColor,				BackColor,				GetValue	RadioText
  {0x0C,  0x0E,	NULL,	DrawRGBMenu_BlueLightRatio}
};
#endif

//=============================================================
MenuItemType code BriContrastMenuItems[]=
{
    // 1Brightness
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*0,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    BrtCon_BrightnessText, 						// DisplayText;
    SubMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable 						// Flags
  },
  // Horizontal
    { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*0,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_None,						// DrawMenuItemType;
    NULL, 						// DisplayText;
    SubMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	AdjustBrightness,								// AdjustFunction
      	NULL								// ExecFunction
    },									// *DisplayValue;
    { 
      DrawBriContrastMenu_BrightnessNumber,								// DrawNumberType
      DrawBriContrastMenu_BrightnessGuage,								// DrawGuageType
      NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibJustShow 						// Flags
  },
    // 2 Contrast
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*1,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    BrtCon_ContrastText, 						// DisplayText;
    SubMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable 						// Flags
  },
  // Horizontal
    { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*1,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_None,						// DrawMenuItemType;
    NULL, 						// DisplayText;
    SubMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	AdjustContrast,								// AdjustFunction
      	NULL								// ExecFunction
    },									// *DisplayValue;
    { 
    	DrawBriContrastMenu_ContrastNumber,								// DrawNumberType
      DrawBriContrastMenu_ContrastGuage,								// DrawGuageType
      NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibJustShow 						// Flags
  },
 #if 1 //MWE_AP_Enable 
    // 3 ECO
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*2,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    BrtCon_ECOText, 						// DisplayText;
    SubMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable 						// Flags
  },
  // Horizontal
    { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*2,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_None,						// DrawMenuItemType;
    NULL, 						// DisplayText;
    SubMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	AdjustECOMode,								// AdjustFunction
      	NULL								// ExecFunction
    },									// *DisplayValue;
    { 
    	NULL,								// DrawNumberType
      NULL,								// DrawGuageType
      DrawBriContrastMenu_ECORatioGroup			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibJustShow 						// Flags
  },
#endif
#if Enable_AdjustDCR 
    // 4 DCR 
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*3,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    BrtCon_DCRText, 						// DisplayText;
    SubMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable 						// Flags
  },
  // Horizontal
    { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*3,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_None,						// DrawMenuItemType;
    NULL, 						// DisplayText;
    SubMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	AdjustDcrMode,								// AdjustFunction
      	NULL								// ExecFunction
    },									// *DisplayValue;
    { 
    	NULL,								// DrawNumberType
      NULL,								// DrawGuageType
      DrawBriContrastMenu_DCRRatioGroup			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibJustShow 						// Flags
  },
#endif
#if OSD_OD
    // 4 DCR 
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*(3+Enable_AdjustDCR),		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    BrtCon_ODText, 						// DisplayText;
    SubMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable 						// Flags
  },
  // Horizontal
    { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*(3+Enable_AdjustDCR),		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_None,						// DrawMenuItemType;
    NULL, 						// DisplayText;
    SubMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	AdjustOverDrive,								// AdjustFunction
      	NULL								// ExecFunction
    },									// *DisplayValue;
    { 
    	NULL,								// DrawNumberType
      NULL,								// DrawGuageType
      DrawBriContrastMenu_ODRatioGroup			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibJustShow 						// Flags
  },
#endif
#if LowBlueLight
    // 5 ASpect
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*(3+Enable_AdjustDCR+OSD_OD),		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    BlueLightText, 						// DisplayText;
    SubMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable 						// Flags
  },
  // ASpect
    { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*(3+Enable_AdjustDCR+OSD_OD),		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_None,						// DrawMenuItemType;
    NULL, 						// DisplayText;
    SubMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	AdjustBlueLightMode,				// AdjustFunction
      	NULL								// ExecFunction
    },									// *DisplayValue;
    { 
    	NULL,								// DrawNumberType
      NULL,								// DrawGuageType
      DrawRGBMenu_BlueLightRatioGroup			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibJustShow 						// Flags
  },
#endif

};

//=========== PictureMenuItems ================================
//--------------------PictureNumber--------------------------
NumberType code DrawImageMenu_HPosNum[] =
{
    // Flags,   XPos,   YPos,   GetValue
    { dwiNumber|dwiEnd, MenuNumber_DrawStarXPos,MenuNumber_DrawStarYPos+SubMenu_DrawYPos_Shift*0,	GetHPositionValue}
};
DrawNumberType code DrawImageMenu_HPosNumber[] =
{
    // F ForeColor, BackColor,  GetValue
  {0x0C,  0x0E,  DrawImageMenu_HPosNum} 
};
//--------------------PictureIconGuge--------------------------
GaugeType code DrawImageMenu_HPosGuagee[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
   {  dwiGauge|dwiEnd,	MenuGuage_DrawStarXPos,	  MenuGuage_DrawStarYPos+SubMenu_DrawYPos_Shift*0,	GetHPositionValue  }
};
DrawGuageType code DrawImageMenu_HPosGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
   {0x0C,  0x0E,	MenuGuage_DrawLength, DrawImageMenu_HPosGuagee	}

};
//--------------------PictureNumber--------------------------
NumberType code DrawImageMenu_VPosNum[] =
{
    // Flags,   XPos,   YPos,   GetValue
    { dwiNumber|dwiEnd, MenuNumber_DrawStarXPos,MenuNumber_DrawStarYPos+SubMenu_DrawYPos_Shift*1,	GetVPositionValue}
};
DrawNumberType code DrawImageMenu_VPosNumber[] =
{
    // F ForeColor, BackColor,  GetValue
  {0x0C,  0x0E,  DrawImageMenu_VPosNum} 
};
//--------------------PictureIconGuge--------------------------
GaugeType code DrawImageMenu_VPosGuagee[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
   {  dwiGauge|dwiEnd,	MenuGuage_DrawStarXPos,	  MenuGuage_DrawStarYPos+SubMenu_DrawYPos_Shift*1,	GetVPositionValue  }
};
DrawGuageType code DrawImageMenu_VPosGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
   {0x0C,  0x0E,	MenuGuage_DrawLength, DrawImageMenu_VPosGuagee	}

};
//--------------------PictureNumber--------------------------
NumberType code DrawImageMenu_ClockNum[] =
{
    // Flags,   XPos,   YPos,   GetValue
    { dwiNumber|dwiEnd, MenuNumber_DrawStarXPos,MenuNumber_DrawStarYPos+SubMenu_DrawYPos_Shift*2,	GetClockValue}
};
DrawNumberType code DrawImageMenu_ClockNumber[] =
{
    // F ForeColor, BackColor,  GetValue
  {0x0C,  0x0E,  DrawImageMenu_ClockNum} 
};
//--------------------PictureIconGuge--------------------------
GaugeType code DrawImageMenu_ClockGuagee[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
   {  dwiGauge|dwiEnd,	MenuGuage_DrawStarXPos,	  MenuGuage_DrawStarYPos+SubMenu_DrawYPos_Shift*2,	GetClockValue  }
};
DrawGuageType code DrawImageMenu_ClockGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
   {0x0C,  0x0E,	MenuGuage_DrawLength, DrawImageMenu_ClockGuagee	}

};
//--------------------PictureNumber--------------------------
NumberType code DrawImageMenu_PhaseNum[] =
{
    // Flags,   XPos,   YPos,   GetValue
    { dwiNumber|dwiEnd, MenuNumber_DrawStarXPos,MenuNumber_DrawStarYPos+SubMenu_DrawYPos_Shift*3,	GetFocusValue}
};
DrawNumberType code DrawImageMenu_PhaseNumber[] =
{
    // F ForeColor, BackColor,  GetValue
  {0x0C,  0x0E,  DrawImageMenu_PhaseNum} 
};
//--------------------PictureIconGuge--------------------------
GaugeType code DrawImageMenu_PhaseGuagee[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
   {  dwiGauge|dwiEnd,	MenuGuage_DrawStarXPos,	  MenuGuage_DrawStarYPos+SubMenu_DrawYPos_Shift*3,	GetFocusValue  }
};
DrawGuageType code DrawImageMenu_PhaseGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
   {0x0C,  0x0E,	MenuGuage_DrawLength, DrawImageMenu_PhaseGuagee	}

};
//-----------------------------------------
#if Enable_Expansion
RadioTextType code DrawImageMenu_AspectRatio[]=
{ // Flags,					XPos,	YPos,	DisplayText
  {dwiEnd,	MenuRatio_DrawStarXPos,	  MenuRatio_DrawStarYPos+SubMenu_DrawYPos_Shift*4,		AspectMode_SelectText}
};
DrawRadioGroupType code DrawImageMenu_AspectRatioGroup[]=
{ //ForeColor,					BackColor,						GetValue	RadioText
  {0x0C,  0x0E,	NULL,	DrawImageMenu_AspectRatio}
};
#endif
//------------------------------------------------------------------------------
MenuItemType code PictureMenuItems[]=
{
    // 1 HPosition
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*0,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    HPositionText, 						// DisplayText;
    SubMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable|mibDVIDisable 	 						// Flags
  },
  // HPosition
    { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*0,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_None,						// DrawMenuItemType;
    NULL, 						// DisplayText;
    SubMenuAdjustKeyEvent_H, 			// KeyEvent;
    { 
    	AdjustHPosition,								// AdjustFunction
      	NULL								// ExecFunction
    },									// *DisplayValue;
    { 
    	DrawImageMenu_HPosNumber,								// DrawNumberType
      DrawImageMenu_HPosGuage,								// DrawGuageType
      NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibJustShow|mibDVIDisable 						// Flags
  },
    // 2 VPosition
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*1,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    VPositionText, 						// DisplayText;
    SubMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable|mibDVIDisable 	 						// Flags
  },
  // VPosition
    { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*1,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_None,						// DrawMenuItemType;
    NULL, 						// DisplayText;
    SubMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	AdjustVPosition,								// AdjustFunction
      	NULL								// ExecFunction
    },									// *DisplayValue;
    { 
      DrawImageMenu_VPosNumber,								// DrawNumberType
      DrawImageMenu_VPosGuage,								// DrawGuageType
      NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibJustShow|mibDVIDisable 						// Flags
  },
    // 3 Clock
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*2,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    ClockText, 						// DisplayText;
    SubMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable|mibDVIDisable 	 						// Flags
  },
  // Clock
    { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*2,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_None,						// DrawMenuItemType;
    NULL, 						// DisplayText;
    SubMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	AdjustClock,								// AdjustFunction
      	NULL								// ExecFunction
    },									// *DisplayValue;
    { 
    	DrawImageMenu_ClockNumber,								// DrawNumberType
      DrawImageMenu_ClockGuage,								// DrawGuageType
      NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibJustShow|mibDVIDisable 						// Flags
  },
    // 4 Phase
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*3,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    PhaseText, 						// DisplayText;
    SubMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable|mibDVIDisable 	 						// Flags
  },
  // Phase
    { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*3,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_None,						// DrawMenuItemType;
    NULL, 						// DisplayText;
    SubMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	AdjustFocus,								// AdjustFunction
      	NULL								// ExecFunction
    },									// *DisplayValue;
    { 
    	DrawImageMenu_PhaseNumber,								// DrawNumberType
      DrawImageMenu_PhaseGuage,								// DrawGuageType
      NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibJustShow|mibDVIDisable 						// Flags
  },
  #if Enable_Expansion
    // 5 ASpect
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*4,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    AspectText, 						// DisplayText;
    SubMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable 						// Flags
  },
  // ASpect
    { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*4,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_None,						// DrawMenuItemType;
    NULL, 						// DisplayText;
    SubMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	AdjustExpansionMode,								// AdjustFunction
      	NULL								// ExecFunction
    },									// *DisplayValue;
    { 
    	NULL,								// DrawNumberType
      NULL,								// DrawGuageType
      DrawImageMenu_AspectRatioGroup			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibJustShow 						// Flags
  },
#endif
};
//----------------------------------------------------------------------------------------
RadioTextType code DrawColorTempMenu_ColorTempRatio[]=
{ // Flags,					XPos,	YPos,	DisplayText
  {dwiEnd,	MenuRatio_DrawStarXPos,	  MenuRatio_DrawStarYPos+SubMenu_DrawYPos_Shift*0,		ColorTempSelectText}//ColorTempSelectText
};
DrawRadioGroupType code DrawColorTempMenu_ColorTempRatioGroup[]=
{ //ForeColor,					BackColor,						GetValue	RadioText
  {0x0C,  0x0E,	NULL,  DrawColorTempMenu_ColorTempRatio}
};
//-------------
NumberType code DrawColorTempMenu_RedNum[]=
{ // Flags,	XPos,	YPos,	GetValue
  {dwiNumber|dwiEnd,	MenuNumber_DrawStarXPos,MenuNumber_DrawStarYPos+SubMenu_DrawYPos_Shift*1,	GetRedColorValue100}
};
DrawNumberType code DrawColorTempMenu_RedNumber[]=
{ // Flags,	ForeColor,					BackColor,								XPos,	YPos,	GetValue
  {	0x0C,  0x0E,	DrawColorTempMenu_RedNum}
};

GaugeType code DrawColorTempMenu_RedGuagee[]=
{ // Flags,	XPos,	YPos,	GetValue
  {dwiGauge|dwiEnd,	MenuGuage_DrawStarXPos,	  MenuGuage_DrawStarYPos+SubMenu_DrawYPos_Shift*1,	GetRedColorValue100}
};
DrawGuageType code DrawColorTempMenu_RedGuage[]=
{ //Flags,	ForeColor,	    			BackColor,						Length,	XPos,	YPos,	GetValue
  {	0x0C,  0x0E,	MenuGuage_DrawLength,   DrawColorTempMenu_RedGuagee	}
};
//----------------------------------
NumberType code DrawColorTempMenu_GreenNum[]=
{ // Flags,	XPos,	YPos,	GetValue
  {dwiNumber|dwiEnd,		MenuNumber_DrawStarXPos,MenuNumber_DrawStarYPos+SubMenu_DrawYPos_Shift*2,	GetGreenColorValue100}
};
DrawNumberType code DrawColorTempMenu_GreenNumber[]=
{ // Flags,	ForeColor,					BackColor,								XPos,	YPos,	GetValue
  {	0x0C,  0x0E,  DrawColorTempMenu_GreenNum}
};

GaugeType code DrawColorTempMenu_GreenGuagee[]=
{ // Flags,	XPos,	YPos,	GetValue
  {dwiGauge|dwiEnd,	MenuGuage_DrawStarXPos,	  MenuGuage_DrawStarYPos+SubMenu_DrawYPos_Shift*2,	GetGreenColorValue100}
};
DrawGuageType code DrawColorTempMenu_GreenGuage[]=
{ //Flags,	ForeColor,	    			BackColor,						Length,	XPos,	YPos,	GetValue
  {0x0C,  0x0E,	MenuGuage_DrawLength, DrawColorTempMenu_GreenGuagee	}
};
//----------------------------------

NumberType code DrawColorTempMenu_BlueNum[]=
{ // Flags,	XPos,	YPos,	GetValue
  {dwiNumber|dwiEnd,	MenuNumber_DrawStarXPos,MenuNumber_DrawStarYPos+SubMenu_DrawYPos_Shift*3,	GetBlueColorValue100}
};
DrawNumberType code DrawColorTempMenu_BlueNumber[]=
{ // Flags,	ForeColor,					BackColor,								XPos,	YPos,	GetValue
  {0x0C,  0x0E, DrawColorTempMenu_BlueNum	}
};
GaugeType code DrawColorTempMenu_BlueGuagee[]=
{ // Flags,	XPos,	YPos,	GetValue
  {dwiGauge|dwiEnd,	MenuGuage_DrawStarXPos,	  MenuGuage_DrawStarYPos+SubMenu_DrawYPos_Shift*3,	GetBlueColorValue100}
};
DrawGuageType code DrawColorTempMenu_BlueGuage[]=
{ //Flags,	ForeColor,	    			BackColor,						Length,	XPos,	YPos,	GetValue
  {0x0C,  0x0E,	MenuGuage_DrawLength,	DrawColorTempMenu_BlueGuagee}
};

//-------------------------------------------------------------------------------------------
MenuItemType code RGBMenuItems[]=
{
  // 1 ColorTemp
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*0,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    CTemp_TempText, 						// DisplayText;
#if 1//Customer_Type_Sel==Customer_GUANRONG
    SubMenuKeyEvent,
#else  
    Adjuster2KeyEvent, 			// KeyEvent;
 #endif   
    {
#if 1//Customer_Type_Sel==Customer_GUANRONG
		 NULL,
#else  
		 AdjustColorTempMode,								 // AdjustFunction
 #endif   
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
#if 1//Customer_Type_Sel==Customer_GUANRONG
			  NULL,
#else  
			  DrawColorTempMenu_ColorTempRatioGroup 		 // DrawRadioGroupType
 #endif   

    },  
     NULL, // Fonts
    mibSelectable 						// Flags
  },
 #if 1//Customer_Type_Sel==Customer_GUANRONG
{ SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*0,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_None,						// DrawMenuItemType;
    NULL, 						// DisplayText;
    SubMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	AdjustColorTempMode,								// AdjustFunction
      	NULL								// ExecFunction
    },									// *DisplayValue;
    { 
    	NULL,								// DrawNumberType
      NULL,								// DrawGuageType
      DrawColorTempMenu_ColorTempRatioGroup			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibJustShow 						// Flags
  },


#endif
  // 2  Red Color
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*1,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    CTemp_RedText, 						// DisplayText;
    SubMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable 						// Flags
  },
    // 2  RedColor guage
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*1,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_None,						// DrawMenuItemType;
    NULL, 						// DisplayText;
    SubMenuAdjustKeyEvent, 			// KeyEvent;
    {
      AdjustRedColor,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { 
      DrawColorTempMenu_RedNumber,								// DrawNumberType
      DrawColorTempMenu_RedGuage,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibJustShow 						// Flags
  },
  // 3  Green Color
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*2,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    CTemp_GreenText, 						// DisplayText;
    SubMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable 						// Flags
  },
    // 3  GreenColor guage
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*2,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_None,						// DrawMenuItemType;
    NULL, 						// DisplayText;
    SubMenuAdjustKeyEvent, 			// KeyEvent;
    { 
      AdjustGreenColor,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { 
    	DrawColorTempMenu_GreenNumber,								// DrawNumberType
      DrawColorTempMenu_GreenGuage,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibJustShow 						// Flags
  },
     // 4 Blue color
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*3,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    CTemp_BlueText, 						// DisplayText;
    SubMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable 						// Flags
  },
    // 4  BlueColor guage
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*3,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_None,						// DrawMenuItemType;
    NULL, 						// DisplayText;
    SubMenuAdjustKeyEvent, 			// KeyEvent;
    { 
      AdjustBlueColor,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { 
    	DrawColorTempMenu_BlueNumber,								// DrawNumberType
      DrawColorTempMenu_BlueGuage,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibJustShow 						// Flags
  },

};
//-------------------------------------------------------------------------------------------
NumberType code DrawOSDSettingMenu_HPosNum[]=
{ // Flags,	XPos,	YPos,	GetValue
  {dwiNumber|dwiEnd,MenuNumber_DrawStarXPos,MenuNumber_DrawStarYPos+SubMenu_DrawYPos_Shift*1,	GetOSDHPositionValue}
};
DrawNumberType code DrawOSDSettingMenu_HPosNumber[]=
{ // Flags,	ForeColor,					BackColor,								XPos,	YPos,	GetValue
  {	0x0C,  0x0E,	DrawOSDSettingMenu_HPosNum}
};

GaugeType code DrawOSDSettingMenu_HPosGuagee[]=
{ // Flags,	XPos,	YPos,	GetValue
  {dwiGauge|dwiEnd,		MenuGuage_DrawStarXPos,	  MenuGuage_DrawStarYPos+SubMenu_DrawYPos_Shift*1,	GetOSDHPositionValue}
};
DrawGuageType code DrawOSDSettingMenu_HPosGuage[]=
{ //Flags,	ForeColor,	    			BackColor,						Length,	XPos,	YPos,	GetValue
  {	0x0C,  0x0E,	MenuGuage_DrawLength,DrawOSDSettingMenu_HPosGuagee }
};
//--------------------
NumberType code DrawOSDSettingMenu_VPosNum[]=
{ // Flags,	XPos,	YPos,	GetValue
  {dwiNumber|dwiEnd,MenuNumber_DrawStarXPos,MenuNumber_DrawStarYPos+SubMenu_DrawYPos_Shift*2,	GetOSDVPositionValue}
};
DrawNumberType code DrawOSDSettingMenu_VPosNumber[]=
{ // Flags,	ForeColor,					BackColor,								XPos,	YPos,	GetValue
  {0x0C,  0x0E, DrawOSDSettingMenu_VPosNum	}
};

GaugeType code DrawOSDSettingMenu_VPosGuagee[]=
{ // Flags,	XPos,	YPos,	GetValue
  {dwiGauge|dwiEnd,		MenuGuage_DrawStarXPos,	  MenuGuage_DrawStarYPos+SubMenu_DrawYPos_Shift*2,	GetOSDVPositionValue}
};
DrawGuageType code DrawOSDSettingMenu_VPosGuage[]=
{ //Flags,	ForeColor,	    			BackColor,						Length,	XPos,	YPos,	GetValue
  {0x0C,  0x0E,	MenuGuage_DrawLength,DrawOSDSettingMenu_VPosGuagee}
};
//--------------------
NumberType code DrawOSDSettingMenu_TimerNum[]=
{ // Flags,	XPos,	YPos,	GetValue
  {dwiNumber|dwiEnd,MenuNumber_DrawStarXPos,MenuNumber_DrawStarYPos+SubMenu_DrawYPos_Shift*3,	GetOSDTimeValue}
};
DrawNumberType code DrawOSDSettingMenu_TimerNumber[]=
{ // Flags,	ForeColor,					BackColor,								XPos,	YPos,	GetValue 
  {	0x0C,  0x0E,	DrawOSDSettingMenu_TimerNum}
};

GaugeType code DrawOSDSettingMenu_TimerGuagee[]=
{ // Flags,	XPos,	YPos,	GetValue
  {dwiGauge|dwiEnd,	MenuGuage_DrawStarXPos,	  MenuGuage_DrawStarYPos+SubMenu_DrawYPos_Shift*3,	GetOSDTime100Value}
};
DrawGuageType code DrawOSDSettingMenu_TimerGuage[]=
{ //Flags,	ForeColor,	    			BackColor,						Length,	XPos,	YPos,	GetValue
  {0x0C,  0x0E,	MenuGuage_DrawLength, 	DrawOSDSettingMenu_TimerGuagee	}
};
//--------------------
NumberType code DrawOSDSettingMenu_TransparenceNum[]=
{ // Flags,	XPos,	YPos,	GetValue
  {dwiNumber|dwiEnd,MenuNumber_DrawStarXPos,MenuNumber_DrawStarYPos+SubMenu_DrawYPos_Shift*4,	GetOSDTransparencyValue}
};
DrawNumberType code DrawOSDSettingMenu_TransparenceNumber[]=
{ // Flags,	ForeColor,					BackColor,								XPos,	YPos,	GetValue
  {0x0C,  0x0E,	DrawOSDSettingMenu_TransparenceNum}
};

GaugeType code DrawOSDSettingMenu_TransparenceGuagee[]=
{ // Flags,	XPos,	YPos,	GetValue
  {dwiGauge|dwiEnd,		MenuGuage_DrawStarXPos,	  MenuGuage_DrawStarYPos+SubMenu_DrawYPos_Shift*4,	GetOSDTransparencyValue}
};
DrawGuageType code DrawOSDSettingMenu_TransparenceGuage[]=
{ //Flags,	ForeColor,	    			BackColor,						Length,	XPos,	YPos,	GetValue
  {0x0C,  0x0E,	MenuGuage_DrawLength, DrawOSDSettingMenu_TransparenceGuagee}
};

//======================================================================
MenuItemType code OSDMenuItems[]=
{
  // language
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*0,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    LanguageMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    OSD_LanguageText, 						// DisplayText;
    SubMenu2KeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable 						// Flags
  },
   // 1 OSD Hpos
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*1,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    OSDHPositionText, 						// DisplayText;
    SubMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable 						// Flags
  },
   // 1 OSD Hpos
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*1,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_None,						// DrawMenuItemType;
    NULL, 						// DisplayText;
    SubMenuAdjustKeyEvent, 			// KeyEvent;
    { 
      AdjustOSDHPosition,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { 
      DrawOSDSettingMenu_HPosNumber,								// DrawNumberType
      DrawOSDSettingMenu_HPosGuage,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibJustShow 						// Flags
  },
   // 2 OSD Vpos
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*2,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    OSDVPositionText, 						// DisplayText;
    SubMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable 						// Flags
  },
   // 2 OSD Vpos
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*2,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_None,						// DrawMenuItemType;
    NULL, 						// DisplayText;
    SubMenuAdjustKeyEvent, 			// KeyEvent;
    { 
      AdjustOSDVPosition,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { 
    	DrawOSDSettingMenu_VPosNumber,								// DrawNumberType
      DrawOSDSettingMenu_VPosGuage,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibJustShow 						// Flags
  },
   // 3 OSD Time
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*3,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    OSDHoldTimeText, 						// DisplayText;
    SubMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable 						// Flags
  },
   // 3 OSD Time
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*3,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_None,						// DrawMenuItemType;
    NULL, 						// DisplayText;
    SubMenuAdjustKeyEvent, 			// KeyEvent;
    { 
      AdjustOSDTime,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { 
      DrawOSDSettingMenu_TimerNumber,								// DrawNumberType
      DrawOSDSettingMenu_TimerGuage,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibJustShow 						// Flags
  },
   // 4 OSD Translucence
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*4,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    OSD_TranslucenceText, 						// DisplayText;
    SubMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable 						// Flags
  },
   // 4 OSD Translucence
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*4,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_None,						// DrawMenuItemType;
    NULL, 						// DisplayText;
    SubMenuAdjustKeyEvent, 			// KeyEvent;
    { 
      AdjustOSDTransparency,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { 
    	DrawOSDSettingMenu_TransparenceNumber,								// DrawNumberType
      DrawOSDSettingMenu_TransparenceGuage,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibJustShow 						// Flags
  },

};
//==========================================================
MenuItemType code LanguageMenuItems[]=
{

#if 0//(DisplayLogo==TERRA)
 #if Language_English
  // English
  { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*0,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    Language_EnglishText, 						// DisplayText;
    LanguageMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	NULL,                                   // AdjustFunction
    	AdjustLanguage							// ExecFunction	
    },									// *DisplayValue;
    { 
    	NULL,								// DrawNumberType
      NULL,								// DrawGuageType
      NULL,			// DrawRadioGroupType
    },
    NULL,
    mibSelectable 						// Flags
  },
  #endif
#if Language_France
  // France
  { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*1,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    Language_FranceText, 						// DisplayText;
    LanguageMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	NULL,                                   // AdjustFunction
    	AdjustLanguage							// ExecFunction	
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
      NULL,			// DrawRadioGroupType
    },
    NULL,
    mibSelectable 						// Flags
  },
  #endif
 #if Language_German
  // German
  { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*2,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    Language_GermanText, 						// DisplayText;
    LanguageMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	NULL,                                   // AdjustFunction
    	AdjustLanguage							// ExecFunction	
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
      NULL,			// DrawRadioGroupType
    },
    NULL,
    mibSelectable 						// Flags
  },
  #endif
  #if Language_Italian
  // Italian
  { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*3,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    Language_ItalianText, 						// DisplayText;
    LanguageMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	NULL,                                   // AdjustFunction
    	AdjustLanguage							// ExecFunction	
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
      NULL,			// DrawRadioGroupType
    },
    NULL,
    mibSelectable 						// Flags
  },
  #endif
  #if Language_Spanish
  // Spanish
  { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*4,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    Language_SpanishText, 						// DisplayText;
    LanguageMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	NULL,                                   // AdjustFunction
    	AdjustLanguage							// ExecFunction	
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
      NULL,			// DrawRadioGroupType
    },
    NULL,
    mibSelectable 						// Flags
  },
  #endif
  #if Language_Russian
  // Russian
  { SubMenuText_DrawStarXPos+SubMenu_DrawXPos_Shift,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*0,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    Language_RussianText, 						// DisplayText;
    LanguageMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	NULL,                                   // AdjustFunction
    	AdjustLanguage							// ExecFunction	
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
      NULL,			// DrawRadioGroupType
    },
    NULL,
    mibSelectable 						// Flags
  },
  #endif

#else
 #if Language_English
  // English
  { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*0,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    Language_EnglishText, 						// DisplayText;
    LanguageMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	NULL,                                   // AdjustFunction
    	AdjustLanguage							// ExecFunction	
    },									// *DisplayValue;
    { 
    	NULL,								// DrawNumberType
      NULL,								// DrawGuageType
      NULL,			// DrawRadioGroupType
    },
    NULL,
    mibSelectable 						// Flags
  },
  #endif
#if Language_France
  // France
  { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*1,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    Language_FranceText, 						// DisplayText;
    LanguageMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	NULL,                                   // AdjustFunction
    	AdjustLanguage							// ExecFunction	
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
      NULL,			// DrawRadioGroupType
    },
    NULL,
    mibSelectable 						// Flags
  },
  #endif
 #if Language_German
  // German
  { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*2,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    Language_GermanText, 						// DisplayText;
    LanguageMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	NULL,                                   // AdjustFunction
    	AdjustLanguage							// ExecFunction	
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
      NULL,			// DrawRadioGroupType
    },
    NULL,
    mibSelectable 						// Flags
  },
  #endif
  #if Language_SimChinese
  // SimChinese
  { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*3,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    Language_SimChineseText, 						// DisplayText;
    LanguageMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	NULL,                                   // AdjustFunction
    	AdjustLanguage							// ExecFunction	
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
      NULL,			// DrawRadioGroupType
    },
    NULL,
    mibSelectable 						// Flags
  },
  #endif
  #if Language_Italian
  // Italian
  { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*4,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    Language_ItalianText, 						// DisplayText;
    LanguageMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	NULL,                                   // AdjustFunction
    	AdjustLanguage							// ExecFunction	
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
      NULL,			// DrawRadioGroupType
    },
    NULL,
    mibSelectable 						// Flags
  },
  #endif
  #if Language_Spanish
  // Spanish
  { SubMenuText_DrawStarXPos+SubMenu_DrawXPos_Shift,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*0,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    Language_SpanishText, 						// DisplayText;
    LanguageMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	NULL,                                   // AdjustFunction
    	AdjustLanguage							// ExecFunction	
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
      NULL,			// DrawRadioGroupType
    },
    NULL,
    mibSelectable 						// Flags
  },
  #endif
  #if Language_Portuguese
  // Portuguese
  { SubMenuText_DrawStarXPos+SubMenu_DrawXPos_Shift,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*1,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    Language_PortugueseText, 						// DisplayText;
    LanguageMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	NULL,                                   // AdjustFunction
    	AdjustLanguage							// ExecFunction	
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
      NULL,			// DrawRadioGroupType
    },
    NULL,
    mibSelectable 						// Flags
  },
  #endif
  #if Language_Turkish
  // Turkish
  { SubMenuText_DrawStarXPos+SubMenu_DrawXPos_Shift,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*2,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    Language_TurkishText, 						// DisplayText;
    LanguageMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	NULL,                                   // AdjustFunction
    	AdjustLanguage							// ExecFunction	
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
      NULL,			// DrawRadioGroupType
    },
    NULL,
    mibSelectable 						// Flags
  },
  #endif
 #if Language_Polish
  // Polish
  { SubMenuText_DrawStarXPos+SubMenu_DrawXPos_Shift,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*3,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    Language_PolishText, 						// DisplayText;
    LanguageMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	NULL,                                   // AdjustFunction
    	AdjustLanguage							// ExecFunction	
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
      NULL,			// DrawRadioGroupType
    },
    NULL,
    mibSelectable 						// Flags
  },
  #endif
#if Language_Dutch
  // German
  { SubMenuText_DrawStarXPos+SubMenu_DrawXPos_Shift,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*4,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    Language_DutchText, 						// DisplayText;
    LanguageMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	NULL,                                   // AdjustFunction
    	AdjustLanguage							// ExecFunction	
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
      NULL,			// DrawRadioGroupType
    },
    NULL,
    mibSelectable 						// Flags
  },
  #endif
  #if Language_Russian
  // Russian
  { SubMenuText_DrawStarXPos+1+SubMenu_DrawXPos_Shift*2,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*0,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    Language_RussianText, 						// DisplayText;
    LanguageMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	NULL,                                   // AdjustFunction
    	AdjustLanguage							// ExecFunction	
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
      NULL,			// DrawRadioGroupType
    },
    NULL,
    mibSelectable 						// Flags
  },
  #endif
  #if Language_Korean
  // Korean
  { SubMenuText_DrawStarXPos+1+SubMenu_DrawXPos_Shift*2,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*1,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    Language_KoreanText, 						// DisplayText;
    LanguageMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	NULL,                                   // AdjustFunction
    	AdjustLanguage							// ExecFunction	
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
      NULL,			// DrawRadioGroupType
    },
    NULL,
    mibSelectable 						// Flags
  },
  #endif
  #if Language_TraChinese
  // TraChinese
  { SubMenuText_DrawStarXPos+1+SubMenu_DrawXPos_Shift*2,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*4,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    Language_TraChineseText, 						// DisplayText;
    LanguageMenuAdjustKeyEvent, 			// KeyEvent;
    { 
    	NULL,                                   // AdjustFunction
    	AdjustLanguage							// ExecFunction	
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
      NULL,			// DrawRadioGroupType
    },
    NULL,
    mibSelectable 						// Flags
  },
  #endif

#endif
};
//==========================================================
MenuItemType code SettingMenuItems[]=
{
   // AutoConfig
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*0,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    RST_AutoAdjText, 						// DisplayText;
    SetAdjustKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      AutoConfig								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable|mibDVIDisable 						// Flags
  },
#if _AUTOCOLOR_IN_SETTING_MENU_ENABLE  
 // 1  AutoColor
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*1,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    RST_ColorAdjText, 						// DisplayText;
    SetAdjustKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      AutoColor								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable|mibDVIDisable 						// Flags
  },
  #endif
 // 2  Reset
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*(1+_AUTOCOLOR_IN_SETTING_MENU_ENABLE),		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    RST_ResetText, 						// DisplayText;
    SetAdjustKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      ResetAllSetting								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable 						// Flags
  },
};
//=========================================================
RadioTextType code DrawMiscMenu_SourceTypeRatio[]=
{ // Flags,					XPos,	YPos,	DisplayText
  {dwiEnd,	MenuRatio_DrawStarXPos,	  MenuRatio_DrawStarYPos+SubMenu_DrawYPos_Shift*0,		Misc_SourceSelectext}//ColorTempSelectText
};
DrawRadioGroupType code DrawMiscMenu_SourceTypeRatioGroup[]=
{ //ForeColor,					BackColor,						GetValue	RadioText
  {0x0C,  0x0E,	NULL, 	DrawMiscMenu_SourceTypeRatio}
};
#if AudioFunc
#if MiscMenuMuteItem
#define Mute_one		1
#define Mute_two		2
#else
#define Mute_two		1
#endif
#if MiscMenuMuteItem
RadioTextType code DrawMiscMenu_MuteRatio[]=
{ // Flags,					XPos,	YPos,	DisplayText
  {dwiEnd,	MenuRatio_DrawStarXPos,	  MenuRatio_DrawStarYPos+SubMenu_DrawYPos_Shift*Mute_one,		Misc_MuteOnOfftext}
};
DrawRadioGroupType code DrawMiscMenu_MuteRatioGroup[]=
{ //ForeColor,					BackColor,						GetValue	RadioText
  {0x0C,  0x0E,	NULL, 	DrawMiscMenu_MuteRatio}
};
#endif

NumberType code DrawMiscMenu_VolumeNum[]=
{ // Flags,	XPos,	YPos,	GetValue
  {dwiNumber|dwiEnd,MenuNumber_DrawStarXPos,MenuNumber_DrawStarYPos+SubMenu_DrawYPos_Shift*Mute_two,	GetVolumeValue}
};
DrawNumberType code DrawMiscMenu_VolumeNumber[]=
{ // Flags,	ForeColor,					BackColor,								XPos,	YPos,	GetValue
  {0x0C,  0x0E, DrawMiscMenu_VolumeNum	}
};

GaugeType code DrawMiscMenu_VolumeGuagee[]=
{ // Flags,	XPos,	YPos,	GetValue
  {dwiGauge|dwiEnd,	MenuGuage_DrawStarXPos,	  MenuGuage_DrawStarYPos+SubMenu_DrawYPos_Shift*Mute_two,	GetVolumeValue}
};
DrawGuageType code DrawMiscMenu_VolumeGuage[]=
{ //Flags,	ForeColor,	    			BackColor,						Length,	XPos,	YPos,	GetValue
  {0x0C,  0x0E,	MenuGuage_DrawLength,	DrawMiscMenu_VolumeGuagee}
};

#if Audio_Switch
RadioTextType code DrawMiscMenu_HDMIAudioRatio[]=
{ // Flags,					XPos,	YPos,	DisplayText
  {dwiEnd,	MenuRatio_DrawStarXPos,	  MenuRatio_DrawStarYPos+SubMenu_DrawYPos_Shift*2,		HDMIAudioValueText}
};
DrawRadioGroupType code DrawMiscMenu_HDMIAudioRatioGroup[]=
{ //ForeColor,					BackColor,						GetValue	RadioText
  {0x0C,  0x0E,	NULL, 	DrawMiscMenu_HDMIAudioRatio}
};
#endif

#endif
//-------------------------------------------------------------------------------
MenuItemType code MiscMenuItems[]=
{
#if OSD_INPUT_SUBMENU_ENABLE
  // language
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*0,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    Misc_InputSourceText, 						// DisplayText;
    SubMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable 						// Flags
  },
  // 4 input
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*0,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_None,						// DrawMenuItemType;
    NULL, 						// DisplayText;
    SwitchInputKeyEvent, 			// KeyEvent;
    { 
	#if (!DualInput)
	NULL,
	NULL
	#else
	AdjustInputType,		// AdjustFunction
	SwitchInput// ExecFunction
	#endif
    },									// *DisplayValue;
    { 
    	NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     DrawMiscMenu_SourceTypeRatioGroup			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibJustShow 						// Flags
  },
#endif  
#if AudioFunc&&MainBoardType==MainBoard_TSUML88CMC2
#if MiscMenuMuteItem
  // language
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*Mute_one,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    Misc_MuteText, 						// DisplayText;
    SubMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable 						// Flags
  },
  // 4 OSD Translucence
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*Mute_one,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_None,						// DrawMenuItemType;
    NULL, 						// DisplayText;
    SubMenuAdjustKeyEvent, 			// KeyEvent;
    { 
      SwitchMute,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { 
    	NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     DrawMiscMenu_MuteRatioGroup			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibJustShow 						// Flags
  },
 #endif 
  // language
   { 
   SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*Mute_two,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    Misc_VolumeText, 						// DisplayText;
    SubMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable 						// Flags
  },
  // 4 OSD Translucence
   { 
   SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*Mute_two,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_None,						// DrawMenuItemType;
    NULL, 						// DisplayText;
    SubMenuAdjustKeyEvent, 			// KeyEvent;
    { 
      AdjustVolume,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { 
    	DrawMiscMenu_VolumeNumber,								// DrawNumberType
      DrawMiscMenu_VolumeGuage,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibJustShow 						// Flags
  },

#endif
#if Audio_Switch
  // language
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*2,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_Text,						// DrawMenuItemType;
    HDMIAUDIOText, 						// DisplayText;
    SubMenuKeyEvent, 			// KeyEvent;
    { NULL,								// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     NULL			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibSelectable 						// Flags
  },
  // 4 OSD Translucence
   { SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*2,		// XPos, YPos;
    0x0C, 0x0E, // ForeColor, BackColor;
    0x0F, 0x0E, // SelForeColor, SelBackColor;
    MainMenu, 			// NextMenuPage;
    DWI_None,						// DrawMenuItemType;
    NULL, 						// DisplayText;
    SubMenuAdjustKeyEvent, 			// KeyEvent;
    { 
      AdjustAudioType,					// AdjustFunction
      NULL								// ExecFunction
    },									// *DisplayValue;
    { 
    	NULL,								// DrawNumberType
      NULL,								// DrawGuageType
     DrawMiscMenu_HDMIAudioRatioGroup			// DrawRadioGroupType
    },  
     NULL, // Fonts
    mibJustShow 						// Flags
  },
#endif
};

//==========================================================zwtan add 20040812
NumberType code DrawHotKeyBrightnessIconNum[]=
{ // Flags,	XPos,	YPos,	GetValue
  {dwiNumber|dwiEnd,19,	4,	GetBrightnessValue}
};
DrawNumberType code DrawHotKeyBrightnessIconNumber[]=
{ // Flags,	ForeColor,	BackColor,	XPos,	YPos,	GetValue
  { 0x0C, 0x0E,	DrawHotKeyBrightnessIconNum}
};

GaugeType code DrawHotKeyBrightnessIconGuagee[]=
{ // Flags,	XPos,	YPos,	GetValue
  {dwiGauge|dwiEnd,5,	4,	GetBrightnessValue}
};
GaugeType code DrawHotKeyBrightnessIconGuage[]=
{ // Flags,	ForeColor,	BackColor,	Length,	XPos,	YPos,	GetValue
  {	0x0C, 0x0E,	15,	DrawHotKeyBrightnessIconGuagee}
};

NumberType code DrawHotKeyContrastIconNum[]=
{ // Flags,	XPos,	YPos,	GetValue
  {dwiEnd,	19,	4,	GetContrastValue}
};
DrawNumberType code DrawHotKeyContrastIconNumber[]=
{ // Flags,	ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {0x0C, 0x0E, DrawHotKeyContrastIconNum}
};

GaugeType code DrawHotKeyContrastIconGuagee[]=
{ // Flags,	XPos,	YPos,	GetValue
  {dwiEnd,	5,	4,	GetContrastValue}
};
GaugeType code DrawHotKeyContrastIconGuage[]=
{ // Flags,	ForeColor,	BackColor,	Length,	XPos,	YPos,	GetValue
  {0x0C, 0x0E,	15 , DrawHotKeyContrastIconGuagee}
};


MenuItemType code HotKeyContMenuItems[]=
{
	{ 0, 2, // XPos, YPos;
       0x0C, 0x0E, // ForeColor, BackColor;
       0x0F, 0x0E, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_FullCenterText,// DrawMenuItemType;
	HotKeyContrastText, // DisplayText;
	AdjusterKeyEvent,// KeyEvent
	{ AdjustContrast,	// AdjustFunction
	NULL			// ExecFunction
	},
	{ 
	DrawHotKeyContrastIconNumber,	// DrawNumberType
	DrawHotKeyContrastIconGuage,	// DrawGuageType
	NULL	// DrawRadioGroupType
	},
	NULL, // Fonts
	NULL // Flags
	}
};



MenuItemType code HotKeyBriteMenuItems[]=
{
	{ 0, 2, // XPos, YPos;
        0x0C, 0x0E, // ForeColor, BackColor;
        0x0F, 0x0E, // SelForeColor, SelBackColor;
    	RootMenu, //NextMenuPage;
    	DWI_FullCenterText,// DrawMenuItemType;
    	HotKeyBrightnessText, // DisplayText;
    	AdjusterKeyEvent,// KeyEvent
    	{ AdjustBrightness,	// AdjustFunction
    	NULL			// ExecFunction
    	},
    	{ 
    	DrawHotKeyBrightnessIconNumber,	// DrawNumberType
    	DrawHotKeyBrightnessIconGuage,	// DrawGuageType
    	NULL	// DrawRadioGroupType
    	},
    	NULL, // Fonts
    	NULL// Flags
	}
};
#if KeyInputSelectMenu
MenuItemType code HotKeyInputSelectMenuItems[]=
{
   #if(InputType_Analog1)
    { 3, 2, // XPos, YPos;
       0x0C, 0x0E, // ForeColor, BackColor;
       0x0F, 0x0E, // SelForeColor, SelBackColor;
   	    RootMenu, //NextMenuPage;
    	DWI_Text,// DrawMenuItemType;
    	HotKeyInputVGAText, // DisplayText;
    	HOTExecKeyEvent,// KeyEvent
    	{ NULL,	// AdjustFunction
    	SwitchInput			// ExecFunction
    	},
    	{ 
    	NULL,	// DrawNumberType
    	NULL,	// DrawGuageType
    	NULL	// DrawRadioGroupType
    	},
    	NULL, // Fonts
    	mibSelectable// Flags
	},
   #endif
#if InputType_Digital	
      { 3, 2+InputType_Analog1*2, // XPos, YPos;
       0x0C, 0x0E, // ForeColor, BackColor;
       0x0F, 0x0E, // SelForeColor, SelBackColor;
   	    RootMenu, //NextMenuPage;
    	DWI_Text,// DrawMenuItemType;
    	HotKeyInputDVIText, // DisplayText;
    	HOTExecKeyEvent,// KeyEvent
    	{ NULL,	// AdjustFunction
    	SwitchInput			// ExecFunction
    	},
    	{ 
    	NULL,	// DrawNumberType
    	NULL,	// DrawGuageType
    	NULL	// DrawRadioGroupType
    	},
    	NULL, // Fonts
    	mibSelectable// Flags
	},
#endif
#if InputType_HDMI
      { 3, 2+InputType_Analog1*2+InputType_Digital*2, // XPos, YPos;
        0x0C, 0x0E, // ForeColor, BackColor;
        0x0F, 0x0E, // SelForeColor, SelBackColor;
    	RootMenu, //NextMenuPage;
    	DWI_Text,// DrawMenuItemType;
    	HotKeyInputHDMIText, // DisplayText;
    	HOTExecKeyEvent,// KeyEvent
    	{ NULL,	// AdjustFunction
    	SwitchInput			// ExecFunction
    	},
    	{ 
    	NULL,	// DrawNumberType
    	NULL,	// DrawGuageType
    	NULL	// DrawRadioGroupType
    	},
    	NULL, // Fonts
    	mibSelectable// Flags
	},
#endif
#if ENABLE_DP_INPUT
      { 3, 2+InputType_Analog1*2+InputType_Digital*2+InputType_HDMI*2, // XPos, YPos;
        0x0C, 0x0E, // ForeColor, BackColor;
        0x0F, 0x0E, // SelForeColor, SelBackColor;
    	RootMenu, //NextMenuPage;
    	DWI_Text,// DrawMenuItemType;
    	HotKeyInputDPText, // DisplayText;
    	HOTExecKeyEvent,// KeyEvent
    	{ NULL,	// AdjustFunction
    	SwitchInput			// ExecFunction
    	},
    	{ 
    	NULL,	// DrawNumberType
    	NULL,	// DrawGuageType
    	NULL	// DrawRadioGroupType
    	},
    	NULL, // Fonts
    	mibSelectable// Flags
	},	
	#endif
};
#endif

#if EN_HotKeyTimes
MenuItemType code HotKeyHotKeyTimesShowMenuItems[]=
{
    { 4, 1, // XPos, YPos;
       0x0C, 0x0C, // ForeColor, BackColor;
       0x0F, 0x0E, // SelForeColor, SelBackColor;
   	    RootMenu, //NextMenuPage;
    	DWI_Text,// DrawMenuItemType;
    	HotKeyShowTimesText, // DisplayText;
    	HOTExecKeyEvent,// KeyEvent
    	{ NULL,	// AdjustFunction
    	NULL,//AdjustTimes			// ExecFunction
    	},
    	{ 
    	NULL,	// DrawNumberType
    	NULL,	// DrawGuageType
    	NULL	// DrawRadioGroupType
    	},
    	NULL, // Fonts
    	mibJustShow// Flags
	},

};



MenuItemType code HotKeyHotKeyTimesSelectMenuItems[]=
{
    { 3, 2, // XPos, YPos;
       0x0C, 0x0E, // ForeColor, BackColor;
       0x0F, 0x0E, // SelForeColor, SelBackColor;
   	    RootMenu, //NextMenuPage;
    	DWI_Text,// DrawMenuItemType;
    	HotKeyTimesText, // DisplayText;
    	HOTExecKeyEvent,// KeyEvent
    	{ NULL,	// AdjustFunction
    	NULL,//AdjustTimes			// ExecFunction
    	},
    	{ 
    	NULL,	// DrawNumberType
    	NULL,	// DrawGuageType
    	NULL	// DrawRadioGroupType
    	},
    	NULL, // Fonts
    	mibJustShow// Flags
	},
    { 3, 4, // XPos, YPos;
       0x0C, 0x0E, // ForeColor, BackColor;
       0x0F, 0x0E, // SelForeColor, SelBackColor;
   	    RootMenu, //NextMenuPage;
    	DWI_Text,// DrawMenuItemType;
    	HotKeyTimes10Text, // DisplayText;
    	HOTTimesExecKeyEvent,// KeyEvent
    	{ NULL,	// AdjustFunction
    	NULL,//AdjustTimes			// ExecFunction
    	},
    	{ 
    	NULL,	// DrawNumberType
    	NULL,	// DrawGuageType
    	NULL	// DrawRadioGroupType
    	},
    	NULL, // Fonts
    	mibSelectable// Flags
	},

      { 3, 6, // XPos, YPos;
       0x0C, 0x0E, // ForeColor, BackColor;
       0x0F, 0x0E, // SelForeColor, SelBackColor;
   	    RootMenu, //NextMenuPage;
    	DWI_Text,// DrawMenuItemType;
    	HotKeyTimes20Text, // DisplayText;
    	HOTTimesExecKeyEvent,// KeyEvent
    	{ NULL,	// AdjustFunction
    	NULL,//AdjustTimes			// ExecFunction
    	},
    	{ 
    	NULL,	// DrawNumberType
    	NULL,	// DrawGuageType
    	NULL	// DrawRadioGroupType
    	},
    	NULL, // Fonts
    	mibSelectable// Flags
	},

      { 3, 8, // XPos, YPos;
        0x0C, 0x0E, // ForeColor, BackColor;
        0x0F, 0x0E, // SelForeColor, SelBackColor;
    	RootMenu, //NextMenuPage;
    	DWI_Text,// DrawMenuItemType;
    	HotKeyTimes30Text, // DisplayText;
    	HOTTimesExecKeyEvent,// KeyEvent
    	{ NULL,	// AdjustFunction
    	NULL,//AdjustTimes			// ExecFunction
    	},
    	{ 
    	NULL,	// DrawNumberType
    	NULL,	// DrawGuageType
    	NULL	// DrawRadioGroupType
    	},
    	NULL, // Fonts
    	mibSelectable// Flags
	},

      { 3, 10, // XPos, YPos;
        0x0C, 0x0E, // ForeColor, BackColor;
        0x0F, 0x0E, // SelForeColor, SelBackColor;
    	RootMenu, //NextMenuPage;
    	DWI_Text,// DrawMenuItemType;
    	HotKeyTimes40Text, // DisplayText;
    	HOTTimesExecKeyEvent,// KeyEvent
    	{ NULL,	// AdjustFunction
    	NULL,//AdjustTimes			// ExecFunction
    	},
    	{ 
    	NULL,	// DrawNumberType
    	NULL,	// DrawGuageType
    	NULL	// DrawRadioGroupType
    	},
    	NULL, // Fonts
    	mibSelectable// Flags
	},	
	{ 3, 12, // XPos, YPos;
	   0x0C, 0x0E, // ForeColor, BackColor;
	   0x0F, 0x0E, // SelForeColor, SelBackColor;
		RootMenu, //NextMenuPage;
		DWI_Text,// DrawMenuItemType;
		HotKeyTimes50Text, // DisplayText;
		HOTTimesExecKeyEvent,// KeyEvent
		{ NULL, // AdjustFunction
		NULL,//AdjustTimes 		// ExecFunction
		},
		{ 
		NULL,	// DrawNumberType
		NULL,	// DrawGuageType
		NULL	// DrawRadioGroupType
		},
		NULL, // Fonts
		mibSelectable// Flags
	},
	
	  { 3, 14, // XPos, YPos;
	   0x0C, 0x0E, // ForeColor, BackColor;
	   0x0F, 0x0E, // SelForeColor, SelBackColor;
		RootMenu, //NextMenuPage;
		DWI_Text,// DrawMenuItemType;
		HotKeyTimes60Text, // DisplayText;
		HOTTimesExecKeyEvent,// KeyEvent
		{ NULL, // AdjustFunction
		NULL,//AdjustTimes 		// ExecFunction
		},
		{ 
		NULL,	// DrawNumberType
		NULL,	// DrawGuageType
		NULL	// DrawRadioGroupType
		},
		NULL, // Fonts
		mibSelectable// Flags
	},
	
	  { 3, 16, // XPos, YPos;
		0x0C, 0x0E, // ForeColor, BackColor;
		0x0F, 0x0E, // SelForeColor, SelBackColor;
		RootMenu, //NextMenuPage;
		DWI_Text,// DrawMenuItemType;
		HotKeyTimes90Text, // DisplayText;
		HOTTimesExecKeyEvent,// KeyEvent
		{ NULL, // AdjustFunction
		NULL,//AdjustTimes 		// ExecFunction
		},
		{ 
		NULL,	// DrawNumberType
		NULL,	// DrawGuageType
		NULL	// DrawRadioGroupType
		},
		NULL, // Fonts
		mibSelectable// Flags
	},
	
};
#endif

#if EN_HotKeyXINGZHUN

//============================================================
RadioTextType code DrawXINGZHUNTextRatio[]=
{ // Flags,					XPos,	YPos,	DisplayText
  {dwiEnd|DWI_CenterText,					3, 		8,		HotKeyXINGZHUNText}
};
DrawRadioGroupType code DrawXINGZHUNTextRatioGroup[]=
{ //ForeColor,				BackColor,				GetValue	RadioText
  {0x07, CPC_HKCBlackColor, NULL,		DrawXINGZHUNTextRatio}
};

RadioTextType code DrawjishiTextRatio[]=
{ // Flags,					XPos,	YPos,	DisplayText
  {dwiEnd|DWI_CenterText,					3, 		14,		HotKeyTimesText}
};
DrawRadioGroupType code DrawjishiTextRatioGroup[]=
{ //ForeColor,				BackColor,				GetValue	RadioText
  {0x07, CPC_HKCBlackColor, NULL,		DrawjishiTextRatio}
};

RadioTextType code DrawHotKeyXZCJTextTextRatio[]=
{ // Flags,					XPos,	YPos,	DisplayText
  {dwiEnd|DWI_CenterText,					3, 		2,		HotKeyXZCJText}
};

DrawRadioGroupType code DrawXZCJTTextRatioGroup[]=
{ //ForeColor,				BackColor,				GetValue	RadioText
  {0x07, CPC_HKCBlackColor, NULL,		DrawHotKeyXZCJTextTextRatio}
};



MenuItemType code HotKeyXZShowMenuItems[]=
{
	{ 3, 2, // XPos, YPos;
       0x0C, 0x0C, // ForeColor, BackColor;
       0x0F, 0x0E, // SelForeColor, SelBackColor;
   	    RootMenu, //NextMenuPage;
     	DWI_Nothing,//DWI_3x2Icon_2,// DrawMenuItemType;
    	NULL, //HotKeyXZIcon, // DisplayText;
    	HOTExecKeyEvent,// KeyEvent
    	{ NULL,	// AdjustFunction
    	NULL,//AdjustTimes			// ExecFunction
    	},
    	{ 
    	NULL,	// DrawNumberType
    	NULL,	// DrawGuageType
    	DrawXZCJTTextRatioGroup	// DrawRadioGroupType
    	},
    	NULL, // Fonts
    	mibJustShow// Flags
	},

    { 3, 6, // XPos, YPos;
       0x0C, 0x0C, // ForeColor, BackColor;
       0x0F, 0x0E, // SelForeColor, SelBackColor;
   	    RootMenu, //NextMenuPage;
     	DWI_Nothing,//DWI_3x2Icon_2,// DrawMenuItemType;
    	NULL, //HotKeyXZIcon, // DisplayText;
    	HOTTimesExecKeyEvent,// KeyEvent
    	{ NULL,	// AdjustFunction
    	NULL,//AdjustTimes			// ExecFunction
    	},
    	{ 
    	NULL,	// DrawNumberType
    	NULL,	// DrawGuageType
    	DrawXINGZHUNTextRatioGroup// DrawRadioGroupType
    	},
    	NULL, // Fonts
    	mibSelectable// Flags
	},
	
    	{ 3,12, // XPos, YPos;
    	   0x0C, 0x0C, // ForeColor, BackColor;
    	   0x0F, 0x0E, // SelForeColor, SelBackColor;
    		RootMenu, //NextMenuPage;
			DWI_Nothing,//DWI_3x2Icon_2,// DrawMenuItemType;
			NULL, //HotKeyJSIcon, // DisplayText;
    		HOTTimesExecKeyEvent,// KeyEvent
    		{ NULL, // AdjustFunction
    		NULL,//AdjustTimes			// ExecFunction
    		},
    		{ 
    		NULL,	// DrawNumberType
    		NULL,	// DrawGuageType
			DrawjishiTextRatioGroup// DrawRadioGroupType
    		},
    		NULL, // Fonts
    		mibSelectable// Flags
    	},


};

MenuItemType code HotKeyXZShowMenuItems1[]=
{
    { 1, 1, // XPos, YPos;
       0x0C, 0x0C, // ForeColor, BackColor;
       0x0F, 0x0E, // SelForeColor, SelBackColor;
   	    RootMenu, //NextMenuPage;
		DWI_Nothing,//DWI_3x2Icon_2,// DrawMenuItemType;
		NULL, //HotKeyJSIcon, // DisplayText;
    	HOTTimesExecKeyEvent,// KeyEvent
    	{ NULL,	// AdjustFunction
    	NULL,//AdjustTimes			// ExecFunction
    	},
    	{ 
    	NULL,	// DrawNumberType
    	NULL,	// DrawGuageType
			NULL	// DrawRadioGroupType
    	},
    	NULL, // Fonts
    	mibSelectable// Flags
	},
};
MenuItemType code HotKeyXZShowMenuItems2[]=
{
    { 1, 1, // XPos, YPos;
       0x0C, 0x0C, // ForeColor, BackColor;
       0x0F, 0x0E, // SelForeColor, SelBackColor;
   	    RootMenu, //NextMenuPage;
		DWI_Nothing,//DWI_3x2Icon_2,// DrawMenuItemType;
		NULL, //HotKeyJSIcon, // DisplayText;
    	HOTTimesExecKeyEvent,// KeyEvent
    	{ NULL,	// AdjustFunction
    	NULL,//AdjustTimes			// ExecFunction
    	},
    	{ 
    	NULL,	// DrawNumberType
    	NULL,	// DrawGuageType
			NULL	// DrawRadioGroupType
    	},
    	NULL, // Fonts
    	mibSelectable// Flags
	},
};
MenuItemType code HotKeyXZShowMenuItems3[]=
{
    { 1, 1, // XPos, YPos;
       0x0C, 0x0C, // ForeColor, BackColor;
       0x0F, 0x0E, // SelForeColor, SelBackColor;
   	    RootMenu, //NextMenuPage;
		DWI_Nothing,//DWI_3x2Icon_2,// DrawMenuItemType;
		NULL, //HotKeyJSIcon, // DisplayText;
    	HOTTimesExecKeyEvent,// KeyEvent
    	{ NULL,	// AdjustFunction
    	NULL,//AdjustTimes			// ExecFunction
    	},
    	{ 
    	NULL,	// DrawNumberType
    	NULL,	// DrawGuageType
			NULL	// DrawRadioGroupType
    	},
    	NULL, // Fonts
    	mibSelectable// Flags
	},
};
MenuItemType code HotKeyXZShowMenuItems4[]=
{
    { 1, 1, // XPos, YPos;
       0x0C, 0x0C, // ForeColor, BackColor;
       0x0F, 0x0E, // SelForeColor, SelBackColor;
   	    RootMenu, //NextMenuPage;
		DWI_Nothing,//DWI_3x2Icon_2,// DrawMenuItemType;
		NULL, //HotKeyJSIcon, // DisplayText;
    	HOTTimesExecKeyEvent,// KeyEvent
    	{ NULL,	// AdjustFunction
    	NULL,//AdjustTimes			// ExecFunction
    	},
    	{ 
    	NULL,	// DrawNumberType
    	NULL,	// DrawGuageType
			NULL	// DrawRadioGroupType
    	},
    	NULL, // Fonts
    	mibSelectable// Flags
	},
};
MenuItemType code HotKeyXZShowMenuItems5[]=
{
    { 1, 1, // XPos, YPos;
       0x0C, 0x0C, // ForeColor, BackColor;
       0x0F, 0x0E, // SelForeColor, SelBackColor;
   	    RootMenu, //NextMenuPage;
		DWI_Nothing,//DWI_3x2Icon_2,// DrawMenuItemType;
		NULL, //HotKeyJSIcon, // DisplayText;
    	HOTTimesExecKeyEvent,// KeyEvent
    	{ NULL,	// AdjustFunction
    	NULL,//AdjustTimes			// ExecFunction
    	},
    	{ 
    	NULL,	// DrawNumberType
    	NULL,	// DrawGuageType
			NULL	// DrawRadioGroupType
    	},
    	NULL, // Fonts
    	mibSelectable// Flags
	},
};
MenuItemType code HotKeyXZShowMenuItems6[]=
{
    { 1, 1, // XPos, YPos;
       0x0C, 0x0C, // ForeColor, BackColor;
       0x0F, 0x0E, // SelForeColor, SelBackColor;
   	    RootMenu, //NextMenuPage;
		DWI_Nothing,//DWI_3x2Icon_2,// DrawMenuItemType;
		NULL, //HotKeyJSIcon, // DisplayText;
    	HOTTimesExecKeyEvent,// KeyEvent
    	{ NULL,	// AdjustFunction
    	NULL,//AdjustTimes			// ExecFunction
    	},
    	{ 
    	NULL,	// DrawNumberType
    	NULL,	// DrawGuageType
    	NULL	// DrawRadioGroupType
    	},
    	NULL, // Fonts
    	mibSelectable// Flags
	},
};

#endif

#if AudioFunc

NumberType code DrawHotKeyVolumeIconNum[]=
{ // Flags,	XPos,	YPos,	GetValue
  {dwiEnd,	19,	4,	GetVolumeValue}
};
DrawNumberType code DrawHotKeyVolumeIconNumber[]=
{ // Flags,	ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {0x0C, 0x0E, DrawHotKeyVolumeIconNum}
};

GaugeType code DrawHotKeyVolumeIconGuagee[]=
{ // Flags,	XPos,	YPos,	GetValue
  {dwiEnd,	5,	4,	GetVolumeValue}
};
GaugeType code DrawHotKeyVolumeIconGuage[]=
{ // Flags,	ForeColor,	BackColor,	Length,	XPos,	YPos,	GetValue
  {0x0C, 0x0E,	15 , DrawHotKeyVolumeIconGuagee}
};

RadioTextType code DrawHotKeyMuteRatio[]=
{ // Flags,					XPos,	YPos,	DisplayText
  {dwiEnd,	9,	  3,		Misc_MuteOnOfftext}
};
DrawRadioGroupType code DrawHotKeyMuteRatioGroup[]=
{ //ForeColor,					BackColor,						GetValue	RadioText
  {0x0C,  0x0E,	NULL, 	DrawHotKeyMuteRatio}
};

MenuItemType code HotKeyVolumeMenuItems[]=
{
	{ 0, 2, // XPos, YPos;
       0x0C, 0x0E, // ForeColor, BackColor;
       0x0F, 0x0E, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_FullCenterText,// DrawMenuItemType;
	HotKeyVolumeText, // DisplayText;
	AdjusterKeyEvent,// KeyEvent
	{ AdjustVolume,	// AdjustFunction
	NULL			// ExecFunction
	},
	{ 
	DrawHotKeyVolumeIconNumber,//DrawHotKeyVolumeIconNumber,	// DrawNumberType
	DrawHotKeyVolumeIconGuage,//DrawHotKeyVolumeIconGuage,	// DrawGuageType
	NULL	// DrawRadioGroupType
	},
	NULL, // Fonts
	NULL // Flags
	}
};

MenuItemType code HotKeyMuteMenuItems[]=
{
	{ 3, 3, // XPos, YPos;
       0x0C, 0x0E, // ForeColor, BackColor;
       0x0F, 0x0E, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	Misc_MuteText, // DisplayText;
	AdjusterKeyEvent,// KeyEvent
	{
     	   SwitchMute,	// AdjustFunction
      	   NULL			// ExecFunction
	},
	{ 
	NULL,//DrawHotKeyVolumeIconNumber,	// DrawNumberType
	NULL,//DrawHotKeyVolumeIconGuage,	// DrawGuageType
	DrawHotKeyMuteRatioGroup	// DrawRadioGroupType
	},
	NULL, // Fonts
	NULL // Flags
	}
};
#endif

#if  LowBlueLight 

 RadioTextType code DrawHotBlueLightRatio[]=
{ // Flags,					XPos,	YPos,	DisplayText
  {dwiEnd,	17,	  3,		BlueLightModeText}
};
DrawRadioGroupType code DrawHotBlueLightRatioGroup[]=
{ //ForeColor,				BackColor,				GetValue	RadioText
  {0x0C,  0x0E,	NULL,	DrawHotBlueLightRatio}
};
 
MenuItemType code HotKeyLowBlueLightMenuItems[]=
{
	{ 1, 3, // XPos, YPos;
       0x0C, 0x0E, // ForeColor, BackColor;
       0x0C, 0x0E,// SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_FullText,// DrawMenuItemType;
	BlueLightText, // DisplayText;
	AdjusterKeyEvent,// KeyEvent
	{ 
    	AdjustBlueLightMode,				// AdjustFunction 
	NULL			// ExecFunction
	},
	{ 
	NULL	,	// DrawNumberType
	NULL	,	// DrawGuageType
        DrawHotBlueLightRatioGroup           // DrawRadioGroupType 
	},
	NULL, // Fonts
	mibSelectable // Flags
	}
};

#endif

//========================================================
NumberType code DrawFBrightness[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {dwiEnd,	11, 4,	GetBrightnessValue}
};
DrawNumberType code DrawFBrightnessNumber[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {0x0F,	0x0C,DrawFBrightness}
};
NumberType code DrawFContrast[]=
{ // Flags,  XPos,	YPos,	GetValue
  {dwiEnd, 11,5,	GetContrastValue}
};
DrawNumberType code DrawFContrastNumber[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {	0x0F,	0x0C,DrawFContrast}
};
//------------------------------------------------------------------------------------------------------------
RadioTextType code FactoryGainRedTextValue[]=
{ // Flags,	XPos,YPos,DisplayText
  {dwiEnd,	 7,	6,	FactoryRedText}
};
DrawRadioGroupType code DrawFactoryGainRedText[]=
{ //ForeColor,		BackColor,		GetValue	RadioText
  {0x0F,	0x0C,	NULL,	FactoryGainRedTextValue}
};
RadioTextType code FactoryGainGreenTextValue[]=
{ // Flags,	XPos,YPos,DisplayText
  {dwiEnd,	 12,	6,	FactoryGreenText}
};
DrawRadioGroupType code DrawFactoryGainGreenText[]=
{ //ForeColor,		BackColor,		GetValue	RadioText
  {0x0F,	0x0C,	NULL,	FactoryGainGreenTextValue}
};
RadioTextType code FactoryGainBlueTextValue[]=
{ // Flags,	XPos,YPos,DisplayText
  {dwiEnd,	 18,	6,	FactoryBlueText}
};
DrawRadioGroupType code DrawFactoryGainBlueText[]=
{ //ForeColor,		BackColor,		GetValue	RadioText
  {0x0F,	0x0C,	NULL,	FactoryGainBlueTextValue}
};
//----------------
NumberType code DrawGainR[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {dwiEnd,	7,	6,	GetRedGainValue}
};
DrawNumberType code DrawGainRNumber[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {	0x0F,	0x0C,DrawGainR}
};
NumberType code DrawGainG[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {dwiEnd,	12, 6,	GetGreenGainValue}
};
DrawNumberType code DrawGainGNumber[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {0x0F,	0x0C,	DrawGainG}
};
NumberType code DrawGainB[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {dwiEnd,	18, 6,	GetBlueGainValue}
};
DrawNumberType code DrawGainBNumber[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {0x0F,	0x0C,	DrawGainB}
};
//--------------------------------------------------------
RadioTextType code FactoryOffsetRedTextValue[]=
{ // Flags,	XPos,YPos,DisplayText
  {dwiEnd,	 7,	7,	FactoryRedText}
};
DrawRadioGroupType code DrawFactoryOffsetRedText[]=
{ //ForeColor,		BackColor,		GetValue	RadioText
  {0x0F,	0x0C ,	NULL,	FactoryOffsetRedTextValue}
};
RadioTextType code FactoryOffsetGreenTextValue[]=
{ // Flags,	XPos,YPos,DisplayText
  {dwiEnd,	 12,	7,	FactoryGreenText}
};
DrawRadioGroupType code DrawFactoryOffsetGreenText[]=
{ //ForeColor,		BackColor,		GetValue	RadioText
  {0x0F,	0x0C ,	NULL,	FactoryOffsetGreenTextValue}
};
RadioTextType code FactoryOffsetBlueTextValue[]=
{ // Flags,	XPos,YPos,DisplayText
  {dwiEnd,	 18,	7,	FactoryBlueText}
};
DrawRadioGroupType code DrawFactoryOffsetBlueText[]=
{ //ForeColor,		BackColor,		GetValue	RadioText
  {0x0F,	0x0C ,	NULL,	FactoryOffsetBlueTextValue}
};
//-------------------
NumberType code DrawOffsetR[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {dwiEnd,7,	7,	GetAdcRedOffsetValue}
};
DrawNumberType code DrawOffsetRNumber[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {0x0F,	0x0C,	DrawOffsetR}
};
NumberType code DrawOffsetG[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {dwiEnd,	12, 7,	GetAdcGreenOffsetValue}
};
DrawNumberType code DrawOffsetGNumber[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {	0x0F,	0x0C,	DrawOffsetG}
};
NumberType code DrawOffsetB[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {dwiEnd,	18, 7,	GetAdcBlueOffsetValue}
};
DrawNumberType code DrawOffsetBNumber[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {0x0F,	0x0C,	DrawOffsetB}
};
//---------------------
#if FactoryAuto_5400K
RadioTextType code Factory5400KRedTextValue[]=
{ // Flags,	XPos,YPos,DisplayText
  {dwiEnd,	 7,	8,	FactoryRedText}
};
DrawRadioGroupType code DrawFactory5400KRedText[]=
{ //ForeColor,		BackColor,		GetValue	RadioText
  {0x0F,	0x0C ,	NULL,	Factory5400KRedTextValue}
};
RadioTextType code Factory5400KGreenTextValue[]=
{ // Flags,	XPos,YPos,DisplayText
  {dwiEnd,	 12,	8,	FactoryGreenText}
};
DrawRadioGroupType code DrawFactory5400KGreenText[]=
{ //ForeColor,		BackColor,		GetValue	RadioText
  {0x0F,	0x0C ,	NULL,	Factory5400KGreenTextValue}
};
RadioTextType code Factory5400KBlueTextValue[]=
{ // Flags,	XPos,YPos,DisplayText
  {dwiEnd,	 18,	8,	FactoryBlueText}
};
DrawRadioGroupType code DrawFactory5400KBlueText[]=
{ //ForeColor,		BackColor,		GetValue	RadioText
  {0x0F,	0x0C ,	NULL,	Factory5400KBlueTextValue}
};
//---------------
NumberType code DrawRedColor5400K[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {dwiEnd,	7,	8,	GetRColorWarm1Value}
};
DrawNumberType code DrawRedColor5400KNumber[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {0x0F,	0x0C,	DrawRedColor5400K}
};
//---------------
NumberType code DrawGreenColor5400K[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {dwiEnd,	12, 8,	GetGColorWarm1Value}
};
DrawNumberType code DrawGreenColor5400KNumber[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {	0x0F,	0x0C,	DrawGreenColor5400K}
};
//---------------
NumberType code DrawBlueColor5400K[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {dwiEnd,18, 8,	GetBColorWarm1Value}
};
DrawNumberType code DrawBlueColor5400KNumber[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {	0x0F,	0x0C,	DrawBlueColor5400K}
};
#endif

//-------------------
#if FactoryAuto_6500K
RadioTextType code Factory6500KRedTextValue[]=
{ // Flags,	XPos,YPos,DisplayText
  {dwiEnd,	 7,	9,	FactoryRedText}
};
DrawRadioGroupType code DrawFactory6500KRedText[]=
{ //ForeColor,		BackColor,		GetValue	RadioText
  {0x0F,	0x0C ,	NULL,	Factory6500KRedTextValue}
};
RadioTextType code Factory6500KGreenTextValue[]=
{ // Flags,	XPos,YPos,DisplayText
  {dwiEnd,	 12,	9,	FactoryGreenText}
};
DrawRadioGroupType code DrawFactory6500KGreenText[]=
{ //ForeColor,		BackColor,		GetValue	RadioText
  {0x0F,	0x0C ,	NULL,	Factory6500KGreenTextValue}
};
RadioTextType code Factory6500KBlueTextValue[]=
{ // Flags,	XPos,YPos,DisplayText
  {dwiEnd,	 18,	9,	FactoryBlueText}
};
DrawRadioGroupType code DrawFactory6500KBlueText[]=
{ //ForeColor,		BackColor,		GetValue	RadioText
  {0x0F,	0x0C ,	NULL,	Factory6500KBlueTextValue}
};
//---------------
NumberType code DrawRedColor6500K[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {dwiEnd,	7,	9,	GetRColorNormalValue}
};
DrawNumberType code DrawRedColor6500KNumber[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {0x0F,	0x0C,	DrawRedColor6500K}
};
//---------------
NumberType code DrawGreenColor6500K[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {dwiEnd,	12, 9,	GetGColorNormalValue}
};
DrawNumberType code DrawGreenColor6500KNumber[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {	0x0F,	0x0C,	DrawGreenColor6500K}
};
//---------------
NumberType code DrawBlueColor6500K[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {dwiEnd,18, 9,	GetBColorNormalValue}
};
DrawNumberType code DrawBlueColor6500KNumber[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {	0x0F,	0x0C,	DrawBlueColor6500K}
};
#endif
//---------------------------------
#if FactoryAuto_9300K
RadioTextType code Factory9300KRedTextValue[]=
{ // Flags,	XPos,YPos,DisplayText
  {dwiEnd,	 7,	10,	FactoryRedText}
};
DrawRadioGroupType code DrawFactory9300KRedText[]=
{ //ForeColor,		BackColor,		GetValue	RadioText
  {0x0F,	0x0C ,	NULL,	Factory9300KRedTextValue}
};
RadioTextType code Factory9300KGreenTextValue[]=
{ // Flags,	XPos,YPos,DisplayText
  {dwiEnd,	 12,	10,	FactoryGreenText}
};
DrawRadioGroupType code DrawFactory9300KGreenText[]=
{ //ForeColor,		BackColor,		GetValue	RadioText
  { 0x0F,	0x0C ,	NULL,	Factory9300KGreenTextValue}
};
RadioTextType code Factory9300KBlueTextValue[]=
{ // Flags,	XPos,YPos,DisplayText
  {dwiEnd,	 18,	10,	FactoryBlueText}
};
DrawRadioGroupType code DrawFactory9300KBlueText[]=
{ //ForeColor,		BackColor,		GetValue	RadioText
  {0x0F,	0x0C ,	NULL,	Factory9300KBlueTextValue}
};
//-------------------------
NumberType code DrawRedColor9300K[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {dwiEnd,	7,	10,	GetRColorCool1Value}
};
DrawNumberType code DrawRedColor9300KNumber[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {0x0F,	0x0C,	DrawRedColor9300K}
};
//-----------
NumberType code DrawGreenColor9300K[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {dwiEnd,	12, 10,	GetGColorCool1Value}
};
DrawNumberType code DrawGreenColor9300KNumber[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {	0x0F,	0x0C,	DrawGreenColor9300K}
};
//----------
NumberType code DrawBlueColor9300K[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {dwiEnd,18, 10,	GetBColorCool1Value}
};
DrawNumberType code DrawBlueColor9300KNumber[]=
{ // Flags, ForeColor,	BackColor,	XPos,	YPos,	GetValue
  {	0x0F,	0x0C,	DrawBlueColor9300K}
};
#endif
RadioTextType code BurninRatioText[]=
{ // Flags, 	XPos,	YPos,	DisplayText
 // {dwiEnd,	10, 12, OffText},
  {dwiEnd,	12, 12, BurninOnOffText}
};
DrawRadioGroupType code DrawBurninRadioGroup[]=
{ //ForeColor,	BackColor,		GetValue	RadioText
  {0x0F,0x0C,	GetBurninValue, BurninRatioText}
};

#if ENABLE_FACTORY_SSCADJ	//120305 Modify
NumberType code FactorySSCModulationNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd, 4, 15, GetFactorySSCModulationValue}
};
DrawNumberType code DrawFactorySSCModulationNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   0x0F,    0x0C,       FactorySSCModulationNumber}
};
NumberType code FactorySSCPercentageNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd, 13, 15, GetFactorySSCPercentageValue}
};
DrawNumberType code DrawFactorySSCPercentageNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   0x0F,    0x0C,       FactorySSCPercentageNumber}
};
#endif
#if DisplayLogo!=NoBrand	//130613 Jun
RadioTextType code LogoRatioText[]=
{ // Flags, 	XPos,	YPos,	DisplayText
 // {dwiEnd,	10, 12, OffText},
  {dwiEnd,	10, 16, LogoOnOffText}
};
DrawRadioGroupType code DrawLogoRadioGroup[]=
{ //ForeColor,	BackColor,		GetValue	RadioText
  {0x0F,0x0C,	GetLogoValue, LogoRatioText}
};
#endif

#if DDCCI_ENABLE
RadioTextType code DDCCIRatioText[]=
{ // Flags, 	XPos,	YPos,	DisplayText
 // {dwiEnd,	10, 12, OffText},
  {dwiEnd,	10, 17, DDCCIOnOffText}
};
DrawRadioGroupType code DrawDDCCIRadioGroup[]=
{ //ForeColor,	BackColor,		GetValue	RadioText
  {0x0F,0x0C,	GetDDCCIValue, DDCCIRatioText}
};
#endif
//-------------------------------
RadioTextType code FactoryText[]=
{ // Flags, 	XPos,	YPos,	DisplayText
  { 0,		1,	 	0,	VersionDateText},
   {0,		16, 	0,	VerText},
  {0,		1,		1, 	  PanelText},
  { 0,	 	1,	 	2, 	  ChecksumText},
  {0,		1, 	11,	ChangeDateText},
  {0,       13 ,    19,     Time_HText},
  {0,       13 ,    18,     Time_MText},
  {0,       18 ,    18,     Time_SText},
  {dwiEnd,  1,	    18,     	  TIMEText},
 
};

DrawRadioGroupType code DrawFactoryRadioGroup[]=
{ //ForeColor,	BackColor,		GetValue	RadioText
  {0x0E,	0x0C,	NULL,	FactoryText}
};

//======================================================
MenuItemType code FactoryMenuItems[]=
{
// 0.Auto Color
  { 1, 3, // XPos, YPos;
	0x0E, 0x0C, // ForeColor, BackColor;
	0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	FactoryAutoColorText, // DisplayText;
	NaviExecKeyEvent,
	{ NULL, // AdjustFunction
	  AutoColor// ExecFunction
	},// DisplayValue;
	{ NULL, // DrawNumberType
	  NULL, // DrawGuageType
	  NULL	// DrawRadioGroupType
	},
      NULL, //Font
	mibSelectable // Flags
  },
	// 1 Brightness
	{ 1, 4, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	  RootMenu, //NextMenuPage;
	  DWI_Text,// DrawMenuItemType;
	  FactoryBrightnessText, // DisplayText;
	  FactoryNaviKeyEvent,
	  { NULL, // AdjustFunction
		NULL// ExecFunction
	  },// DisplayValue;
	  { NULL, // DrawNumberType
		NULL, // DrawGuageType
		NULL  // DrawRadioGroupType
	  },
         NULL, //Font
	  mibSelectable // Flags
	},
	// 2 Brightness adjust
	{ 10, 4, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	  RootMenu, //NextMenuPage;
	  DWI_Text,// DrawMenuItemType;
	  NULL, // DisplayText;
	  FactoryAdjustKeyEvent,
	  { AdjustBrightness, // AdjustFunction
		NULL// ExecFunction
	  },// DisplayValue;
	  { DrawFBrightnessNumber,	  // DrawNumberType
		NULL, // DrawGuageType
		NULL  // DrawRadioGroupType
	  },
         NULL, //Font
	  mibDrawValue//|mibSelectable// Flags
	},
	//  3 Contrast
	{ 1, 5, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	  RootMenu, //NextMenuPage;
	  DWI_Text,// DrawMenuItemType;
	  FactoryContrastText, // DisplayText;
	  FactoryNaviKeyEvent,
	  { NULL, // AdjustFunction
		NULL// ExecFunction
	  },// DisplayValue;
	  { NULL, // DrawNumberType
		NULL, // DrawGuageType
		NULL  // DrawRadioGroupType
	  },
        NULL, //Font
	  mibSelectable // Flags
	},
	//  4 Contrast adjust
	{ 10, 5, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	  RootMenu, //NextMenuPage;
	  DWI_Text,// DrawMenuItemType;
	  NULL, // DisplayText;
	  FactoryAdjustKeyEvent,
	  { AdjustContrast,   // AdjustFunction
		NULL// ExecFunction
	  },// DisplayValue;
	  { DrawFContrastNumber,  // DrawNumberType
		NULL, // DrawGuageType
		NULL  // DrawRadioGroupType
	  },
        NULL, //Font
	  mibDrawValue//|mibSelectable // Flags
	},
	//  5 GAIN  Text
	{ 1, 6, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	  RootMenu, //NextMenuPage;
	  DWI_Text,// DrawMenuItemType;
	  FactoryAdcGainText, // DisplayText;
	  FactoryNaviKeyEvent,
	  { NULL, // AdjustFunction
	     NULL// ExecFunction
	  },// DisplayValue;
	  { NULL, // DrawNumberType
		NULL, // DrawGuageType
		NULL  // DrawRadioGroupType
	  },
        NULL, //Font
	  mibSelectable // Flags
	},
  // 6 GAIN  R
  { 7, 6, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryNaviKeyEvent,
	{ NULL, // AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ NULL, // DrawNumberType
	  NULL, // DrawGuageType
	  DrawFactoryGainRedText	// DrawRadioGroupType
	},
      NULL, //Font
	mibSelectable // NULL // Flags
  },
  // 7 GAIN R adjust
  { 8, 6, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryAdjustKeyEvent,
	{ AdjustRedGain,	// AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ DrawGainRNumber,	// DrawNumberType
	  NULL, // DrawGuageType
	  NULL	// DrawRadioGroupType
	},
      NULL, //Font
	mibDrawValue // Flags
  },
  // 8 GAIN G
  { 12, 6, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryNaviKeyEvent,
	{ NULL, // AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ NULL, // DrawNumberType
	  NULL, // DrawGuageType
	  DrawFactoryGainGreenText	// DrawRadioGroupType
	},
      NULL, //Font
	mibSelectable //NULL// Flags
  },
  // 9.Gain G adjust
  { 13, 6, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryAdjustKeyEvent,
	{ AdjustGreenGain,	// AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ DrawGainGNumber,	// DrawNumberType
	  NULL, // DrawGuageType
	  NULL	// DrawRadioGroupType
	},
      NULL, //Font
	mibDrawValue // Flags
  },
  // 10.Gain B
  { 18, 6, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryNaviKeyEvent,
	{ NULL, // AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ NULL, // DrawNumberType
	  NULL, // DrawGuageType
	  DrawFactoryGainBlueText	// DrawRadioGroupType
	},
      NULL, //Font
	mibSelectable //NULL // Flags
  },
  // 11.Gain B adjust
  { 19, 6, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryAdjustKeyEvent,
	{ AdjustBlueGain,	// AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ DrawGainBNumber,	// DrawNumberType
	  NULL, // DrawGuageType
	  NULL	// DrawRadioGroupType
	},
      NULL, //Font
	mibDrawValue // Flags
  },
 //  12 OFFSET
	{ 1, 7, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	  RootMenu, //NextMenuPage;
	  DWI_Text,// DrawMenuItemType;
	  FactoryAdcOffsetText, // DisplayText;
	  FactoryNaviKeyEvent,
	  { NULL, // AdjustFunction
		NULL// ExecFunction
	  },// DisplayValue;
	  { NULL, // DrawNumberType
		NULL, // DrawGuageType
		NULL  // DrawRadioGroupType
	  },
        NULL, //Font
	  mibSelectable // Flags
	},
  // 13.Offset R
  {   7, 7, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	FactoryRedText, // DisplayText;
	FactoryNaviKeyEvent,
	{ NULL, // AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ NULL, // DrawNumberType
	  NULL, // DrawGuageType
	  DrawFactoryOffsetRedText	// DrawRadioGroupType
	},
      NULL, //Font
	mibSelectable //NULL // Flags
  },
  // 14.Offset R adjust
  {   8, 7, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryAdjustKeyEvent,
	{ AdjustAdcRedOffset,	// AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ DrawOffsetRNumber,	// DrawNumberType
	  NULL, // DrawGuageType
	  NULL	// DrawRadioGroupType
	},
      NULL, //Font
	mibDrawValue // Flags
  },
  // 15.Offset G
  {   12, 7, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryNaviKeyEvent,
	{ NULL, // AdjustFunction
	  NULL,//AutoColor// ExecFunction
	},// DisplayValue;
	{ NULL, // DrawNumberType
	  NULL, // DrawGuageType
	  DrawFactoryOffsetGreenText	// DrawRadioGroupType
	},
      NULL, //Font
	mibSelectable //NULL // Flags
  },
  // 16.Offset G adjust
  {   13, 7, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryAdjustKeyEvent,
	{ AdjustAdcGreenOffset, // AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ DrawOffsetGNumber,	// DrawNumberType
	  NULL, // DrawGuageType
	  NULL	// DrawRadioGroupType
	},
      NULL, //Font
	mibDrawValue // Flags
  },
  // 17.Offset B
  {   18, 7, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryNaviKeyEvent,
	{ NULL, // AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ NULL, // DrawNumberType
	  NULL, // DrawGuageType
	  DrawFactoryOffsetBlueText	// DrawRadioGroupType
	},
      NULL, //Font
	mibSelectable //NULL // Flags
  },
  // 18.Offset B adjust
  {   19, 7, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryAdjustKeyEvent,
	{ AdjustAdcBlueOffset,	// AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ DrawOffsetBNumber,	// DrawNumberType
	  NULL, // DrawGuageType
	  NULL	// DrawRadioGroupType
	},
      NULL, //Font
	mibDrawValue// Flags
  },
    #if FactoryAuto_5400K
 //  19 WARM1Text
	{ 1, 8, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	  RootMenu, //NextMenuPage;
	  DWI_Text,// DrawMenuItemType;
	  Factory5400KText, // DisplayText;
	  FactoryNaviKeyEvent,
	  { NULL, // AdjustFunction
		NULL// ExecFunction
	  },// DisplayValue;
	  { NULL, // DrawNumberType
		NULL, // DrawGuageType
		NULL  // DrawRadioGroupType
	  },
        NULL, //Font
	  mibSelectable // Flags
	},
  // 20 .WARM1  R
  { 7, 8, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL , // DisplayText;
	FactoryNaviKeyEvent,
	{ NULL, // AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ NULL, // DrawNumberType
	  NULL, // DrawGuageType
	  DrawFactory5400KRedText	// DrawRadioGroupType
	},
      NULL, //Font
	mibSelectable //NULL // Flags
  },
  // 21.WARM1_R adjust
  { 8, 8, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryAdjustKeyEvent,
	{ AdjustRedColorWarm1,	// AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ DrawRedColor5400KNumber,	// DrawNumberType
	  NULL, // DrawGuageType
	  NULL	// DrawRadioGroupType
	},
      NULL, //Font
	mibDrawValue// Flags
  },
  // 22 .WARM1_G
  { 12, 8, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryNaviKeyEvent,
	{ NULL, // AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ NULL, // DrawNumberType
	  NULL, // DrawGuageType
	  DrawFactory5400KGreenText	// DrawRadioGroupType
	},
      NULL, //Font
	mibSelectable //NULL // Flags
  },
  // 23.WARM1_G adjust
  { 13, 8, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryAdjustKeyEvent,
	{ AdjustGreenColorWarm1,	// AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ DrawGreenColor5400KNumber,	// DrawNumberType
	  NULL, // DrawGuageType
	  NULL	// DrawRadioGroupType
	},
      NULL, //Font
	mibDrawValue// Flags
  },
  // 24.WARM1 B
  { 18, 8, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryNaviKeyEvent,
	{ NULL, // AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ NULL, // DrawNumberType
	  NULL, // DrawGuageType
	  DrawFactory5400KBlueText	// DrawRadioGroupType
	},
      NULL, //Font
	mibSelectable //NULL // Flags
  },
  // 25.WARM1 B adjust
  { 19, 8, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryAdjustKeyEvent,
	{ AdjustBlueColorWarm1, // AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ DrawBlueColor5400KNumber, // DrawNumberType
	  NULL, // DrawGuageType
	  NULL	// DrawRadioGroupType
	},
      NULL, //Font
	mibDrawValue// Flags
  },
  #endif

  #if FactoryAuto_6500K
 //  19 WARM1Text
	{ 1, 9, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	  RootMenu, //NextMenuPage;
	  DWI_Text,// DrawMenuItemType;
	  Factory6500kText, // DisplayText;
	  FactoryNaviKeyEvent,
	  { NULL, // AdjustFunction
		NULL// ExecFunction
	  },// DisplayValue;
	  { NULL, // DrawNumberType
		NULL, // DrawGuageType
		NULL  // DrawRadioGroupType
	  },
        NULL, //Font
	  mibSelectable // Flags
	},
  // 20 .WARM1  R
  { 7, 9, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL , // DisplayText;
	FactoryNaviKeyEvent,
	{ NULL, // AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ NULL, // DrawNumberType
	  NULL, // DrawGuageType
	  DrawFactory6500KRedText	// DrawRadioGroupType
	},
      NULL, //Font
	mibSelectable //NULL // Flags
  },
  // 21.WARM1_R adjust
  { 8, 9, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryAdjustKeyEvent,
	{ AdjustRedColorNormal,	// AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ DrawRedColor6500KNumber,	// DrawNumberType
	  NULL, // DrawGuageType
	  NULL	// DrawRadioGroupType
	},
      NULL, //Font
	mibDrawValue// Flags
  },
  // 22 .WARM1_G
  { 12, 9, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryNaviKeyEvent,
	{ NULL, // AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ NULL, // DrawNumberType
	  NULL, // DrawGuageType
	  DrawFactory6500KGreenText	// DrawRadioGroupType
	},
      NULL, //Font
	mibSelectable //NULL // Flags
  },
  // 23.WARM1_G adjust
  { 13, 9, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryAdjustKeyEvent,
	{ AdjustGreenColorNormal,	// AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ DrawGreenColor6500KNumber,	// DrawNumberType
	  NULL, // DrawGuageType
	  NULL	// DrawRadioGroupType
	},
      NULL, //Font
	mibDrawValue// Flags
  },
  // 24.WARM1 B
  { 18, 9, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryNaviKeyEvent,
	{ NULL, // AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ NULL, // DrawNumberType
	  NULL, // DrawGuageType
	  DrawFactory6500KBlueText	// DrawRadioGroupType
	},
      NULL, //Font
	mibSelectable //NULL // Flags
  },
  // 25.WARM1 B adjust
  { 19, 9, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryAdjustKeyEvent,
	{ AdjustBlueColorNormal, // AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ DrawBlueColor6500KNumber, // DrawNumberType
	  NULL, // DrawGuageType
	  NULL	// DrawRadioGroupType
	},
      NULL, //Font
	mibDrawValue// Flags
  },
  #endif
#if FactoryAuto_9300K
   //  26 Cool1 Text
	{ 1, 10, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	  RootMenu, //NextMenuPage;
	  DWI_Text,// DrawMenuItemType;
	  Factory9300kText, // DisplayText;
	  FactoryNaviKeyEvent,
	  { NULL, // AdjustFunction
		NULL// ExecFunction
	  },// DisplayValue;
	  { NULL, // DrawNumberType
		NULL, // DrawGuageType
		NULL  // DrawRadioGroupType
	  },
        NULL, //Font
	  mibSelectable // Flags
	},
  // 27.Cool1_ R
  {	7, 10, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryNaviKeyEvent,
	{ NULL, // AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ NULL, // DrawNumberType
	  NULL, // DrawGuageType
	  DrawFactory9300KRedText	// DrawRadioGroupType
	},
      NULL, //Font
	mibSelectable //NULL // Flags
  },
  // 28.Cool1_R adjust
  { 	8, 10, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryAdjustKeyEvent,
	{ AdjustRedColorCool1,	// AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ DrawRedColor9300KNumber,	// DrawNumberType
	  NULL, // DrawGuageType
	  NULL	// DrawRadioGroupType
	},
      NULL, //Font
	mibDrawValue // Flags
  },
  // 29.Cool_G
  { 12, 10, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryNaviKeyEvent,
	{ NULL, // AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ NULL, // DrawNumberType
	  NULL, // DrawGuageType
	  DrawFactory9300KGreenText	// DrawRadioGroupType
	},
      NULL, //Font
	mibSelectable //NULL // Flags
  },
  // 30.Cool_G adjust
  { 13, 10, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryAdjustKeyEvent,
	{ AdjustGreenColorCool1,	// AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ DrawGreenColor9300KNumber,	// DrawNumberType
	  NULL, // DrawGuageType
	  NULL	// DrawRadioGroupType
	},
      NULL, //Font
	mibDrawValue // Flags
  },
  // 381.Cool1 B
  { 18,10, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryNaviKeyEvent,
	{ NULL, // AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ NULL, // DrawNumberType
	  NULL, // DrawGuageType
	  DrawFactory9300KBlueText	// DrawRadioGroupType
	},
      NULL, //Font
	mibSelectable //NULL // Flags
  },

  // 32.Cool1 B adjust
  { 19, 10, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryAdjustKeyEvent,
	{ AdjustBlueColorCool1, // AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ DrawBlueColor9300KNumber, // DrawNumberType
	  NULL, // DrawGuageType
	  NULL	// DrawRadioGroupType
	},
      NULL, //Font
	mibDrawValue// Flags
  },
#endif
  // 33 Burnin
  { 1, 12, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	BurninText, // DisplayText;
	FactoryNaviKeyEvent,
	{ NULL, // AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ NULL, // DrawNumberType
	  NULL, // DrawGuageType
	  NULL	// DrawRadioGroupType
	},
      NULL, //Font
	mibSelectable // Flags
  },
  // 34 Burnin Adjust
  { 	1, 12, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_Text,// DrawMenuItemType;
	NULL, // DisplayText;
	FactoryAdjustKeyEvent,
	{ AdjustBurnin, // AdjustFunction
	  NULL// ExecFunction
	},// DisplayValue;
	{ NULL, // DrawNumberType
	  NULL, // DrawGuageType
	  DrawBurninRadioGroup	// DrawRadioGroupType
	},
      NULL, //Font
	mibDrawValue //|mibSelectable// Flags
  },
  // 42 Factory Reset
  { 1, 13, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
    RootMenu, //NextMenuPage;
    DWI_Text,// DrawMenuItemType;
    FactorResetText, // DisplayText; 
    NaviExecKeyEvent,//NaviExecKeyEvent,
    { NULL,	// AdjustFunction
      ResetAllSetting//ResetAllSetting// ExecFunction
    },// DisplayValue;
    { NULL,	// DrawNumberType
      NULL,	// DrawGuageType
      NULL	// DrawRadioGroupType
    },
      NULL, //Font
    mibSelectable // Flags
  },
#if ENABLE_FACTORY_SSCADJ	//120305 Modify
	// 43 spread spectrum
	{
        1, 14, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        SpreadSpectrumText, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            NULL,// AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL,// DrawNumberType
            NULL, // DrawGuageType
            NULL,//DrawSpreadSpectrumRadioGroup//DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue // Flags
    },
	    //44 FreqText
    {
        1, 15, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FreqText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    //45 Freq Adjust
    {
        6, 15, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustFactorySSCModulation, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawFactorySSCModulationNumber, // DrawNumberType
            NULL, // DrawGuageType
            NULL// DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue //| mibSelectable // Flags
    },
        //46 AmpText
    {
        11, 15, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        AmpText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    //47 Amp Adjust
    {
        15, 15, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustFactorySSCPercentage, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawFactorySSCPercentageNumber, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue// | mibSelectable // Flags
    },
    #endif
#if DisplayLogo!=NoBrand	//130613 Jun
	//73 logo
	{ 1, 16, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	  RootMenu, //NextMenuPage;
	  DWI_Text,// DrawMenuItemType;
	  LogoText, // DisplayText;
	  FactoryNaviKeyEvent,
	  { NULL, // AdjustFunction
		NULL// ExecFunction
	  },// DisplayValue;
	  { NULL, // DrawNumberType
		NULL, // DrawGuageType
		NULL  // DrawRadioGroupType
	  },
		NULL, //Font
	  mibSelectable // Flags
	},
	// 74Burnin Adjust
	{ 1, 16, // XPos, YPos;
	  0x0E, 0x0C, // ForeColor, BackColor;
	  0x0F, 0x0C, // SelForeColor, SelBackColor;
	  RootMenu, //NextMenuPage;
	  DWI_Text,// DrawMenuItemType;
	  NULL, // DisplayText;
	  FactoryAdjustKeyEvent,
	  { AdjustLogo, // AdjustFunction
		NULL// ExecFunction
	  },// DisplayValue;
	  { NULL, // DrawNumberType
		NULL, // DrawGuageType
		DrawLogoRadioGroup  // DrawRadioGroupType
	  },
	  NULL, //Font
	  mibDrawValue //|mibSelectable// Flags
	},
	
#if !USEFLASH // 130829 larry
	{
		1, 17, // XPos, YPos;
   	       0x0E, 0x0C, // ForeColor, BackColor;
   	       0x0F, 0x0C, // SelForeColor, SelBackColor;
		RootMenu, //NextMenuPage;
		DWI_Text,// DrawMenuItemType;
		EraseEEPROMText,// ExecFunction
		NaviExecKeyEvent,
		{
			NULL, // AdjustFunction
			EraseAllEEPROMCell// ExecFunction
		},// DisplayValue;
		{
			NULL, // DrawNumberType
			NULL, // DrawGuageType
			NULL	// DrawRadioGroupType
		},
		NULL, //Font
		mibSelectable // Flags
	},
#endif
#endif

     #if DDCCI_ENABLE
     //73 logo
     { 1, 17, // XPos, YPos;
       0x0E, 0x0C, // ForeColor, BackColor;
       0x0F, 0x0C, // SelForeColor, SelBackColor;
       RootMenu, //NextMenuPage;
       DWI_Text,// DrawMenuItemType;
       DDCCIText, // DisplayText;
       FactoryNaviKeyEvent,
       { NULL, // AdjustFunction
         NULL// ExecFunction
       },// DisplayValue;
       { NULL, // DrawNumberType
         NULL, // DrawGuageType
         NULL  // DrawRadioGroupType
       },
         NULL, //Font
       mibSelectable // Flags
     },
     // 74Burnin Adjust
     { 1, 17, // XPos, YPos;
       0x0E, 0x0C, // ForeColor, BackColor;
       0x0F, 0x0C, // SelForeColor, SelBackColor;
       RootMenu, //NextMenuPage;
       DWI_Text,// DrawMenuItemType;
       NULL, // DisplayText;
       FactoryAdjustKeyEvent,
       { AdjustDDCCI, // AdjustFunction
         NULL// ExecFunction
       },// DisplayValue;
       { NULL, // DrawNumberType
         NULL, // DrawGuageType
         DrawDDCCIRadioGroup  // DrawRadioGroupType
       },
       NULL, //Font
       mibDrawValue //|mibSelectable// Flags
     },
     #endif
     
	{	 1, 19, // XPos, YPos;
   	       0x0E, 0x0C, // ForeColor, BackColor;
   	       0x0F, 0x0C, // SelForeColor, SelBackColor;
	    RootMenu, //NextMenuPage;
	    DWI_Text,// DrawMenuItemType;
	    FactoryExitText, // DisplayText; 
	    FacNaviKeyEvent,
	    { NULL,	// AdjustFunction
	      NULL//SaveUserPref// ExecFunction
	    },// DisplayValue;
	    { NULL,//NULL,	// DrawNumberType
	      NULL,	// DrawGuageType
	      NULL	// DrawRadioGroupType
	    },
	      NULL, //Font
	    mibSelectable // Flags
	  },

	  // Descript
	  { 	0, 0, // XPos, YPos;
   	       0x0E, 0x0C, // ForeColor, BackColor;
   	       0x0F, 0x0C, // SelForeColor, SelBackColor;
		RootMenu, //NextMenuPage;
		DWI_Text,// DrawMenuItemType;
		NULL, // DisplayText;
		FactoryAdjustKeyEvent,
		{ NULL,// AdjustFunction
		  NULL// ExecFunction
		},// DisplayValue;
		{ NULL,// DrawNumberType
		  NULL, // DrawGuageType
		  DrawFactoryRadioGroup//DrawRadioGroupType
		},
	      NULL, //Font
		mibDrawValue // Flags
	  },
};


MenuItemType code AutoMenuItems[]=
{
    {
        SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*0,		// XPos, YPos;
        0x0C, 0x0E, // ForeColor, BackColor;
        0x0C, 0x0E, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_FullCenterText,// DrawMenuItemType;
        ProcessAutoText,// DisplayText;
        NothingKeyEvent,// KeyEvent
        {
            NULL, // AdjustFunction
            NULL    // ExecFunction
        },// *DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    }
};
#if Enable_Menu_LockOSD
MenuItemType code OSDLockedMenuItems[]=
{
    {
        SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*0,		// XPos, YPos;
        0x0C, 0x0E, // ForeColor, BackColor;
        0x0C, 0x0E, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_FullCenterText,// DrawMenuItemType;
        OsdLockedText,// DisplayText;
        OsdLockedKeyEvent,// KeyEvent
        {
            NULL, // AdjustFunction
            NULL    // ExecFunction
        },// *DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    }
};

MenuItemType code OSDUnLockedMenuItems[]=
{
    {
        SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*0,		// XPos, YPos;
        0x0C, 0x0E, // ForeColor, BackColor;
        0x0C, 0x0E, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_FullCenterText,// DrawMenuItemType;
        OsdUnLockedText,// DisplayText;
        RootKeyEvent,// KeyEvent
        {
            NULL, // AdjustFunction
            NULL    // ExecFunction
        },// *DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    }
};

#endif
#if Enable_LockPowerKey
MenuItemType code PowerLockedMenuItems[]=
{
    {
        SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*0,		// XPos, YPos;
        0x0C, 0x0E, // ForeColor, BackColor;
        0x0C, 0x0E, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_FullCenterText,// DrawMenuItemType;
        PowerLockedText,// DisplayText;
        OsdLockedKeyEvent,// KeyEvent
        {
            NULL, // AdjustFunction
            NULL    // ExecFunction
        },// *DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    }
};

MenuItemType code PowerUnLockedMenuItems[]=
{
    {
        SubMenuText_DrawStarXPos,SubMenu_DrawStarYPos+SubMenu_DrawYPos_Shift*0,		// XPos, YPos;
        0x0C, 0x0E, // ForeColor, BackColor;
        0x0C, 0x0E, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_FullCenterText,// DrawMenuItemType;
        PowerUnLockedText,// DisplayText;
        RootKeyEvent,// KeyEvent
        {
            NULL, // AdjustFunction
            NULL    // ExecFunction
        },// *DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    }
};

#endif
#if ShowSourceType
MenuItemType code InputTypeMenuItems[]=
{
	{ 0, 2, // XPos, YPos;
        0x0C, 0x0E, // ForeColor, BackColor;
        0x0C, 0x0E, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_FullCenterText,// DrawMenuItemType;
	ShowInputTypeText, // DisplayText;
	NothingKeyEvent,// KeyEvent
	{ 
		NULL,	// AdjustFunction
		NULL			// ExecFunction
	},
	{ 
		NULL,	// DrawNumberType
		NULL,	// DrawGuageType
		NULL	// DrawRadioGroupType
	},
	NULL, // Fonts
	NULL // Flags
	}
};
#endif


//===============================================================
#if ENABLE_BURNIN_MENU

RadioTextType code BurninGroupText[]=
{ // Flags, 	XPos,	YPos,	DisplayText
  {0,			1,	 0,	VersionDateText},
  //{0,			16, 	 0,	EDIDText},
  {0,			1,	 1, 	PanelText},
  {0,			1,	 2, 	PanelWH_Text},
  {0,			1, 	 3, 	ChangeDateText},
  {0,       	20 ,   4,   Time_HText},
  {0,       	24 ,   4,   Time_MText},
  {dwiEnd,  	1,	 4,   TIMEText},
 
};

DrawRadioGroupType code DrawBurninGroup[]=
{ //ForeColor,	BackColor,		GetValue	RadioText
  {0x0f,	0x0C,	NULL,	BurninGroupText}
};
#endif

MenuItemType code BurninMenuItems[]=
{// Contrast Icon
  { 2, 1, // XPos, YPos;
	CP_RedColor, CP_WhiteColor, // ForeColor, BackColor;
	CP_RedColor, CP_WhiteColor, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	DWI_FullCenterText,// DrawMenuItemType;
	NULL, // DisplayText;
	NothingKeyEvent,// KeyEvent
	{ NULL, // AdjustFunction
	  NULL			// ExecFunction
	},// *DisplayValue;
	{ NULL, // DrawNumberType
	  NULL, // DrawGuageType
  #if ENABLE_BURNIN_MENU
    DrawBurninGroup
  #else  
	  NULL	// DrawRadioGroupType
  #endif
	},
      NULL, //Font
	mibSelectable // Flags
  }
};


MenuItemType code LogoMenuItems[]=
{//0 Reset Icon
  { 0, 9, // 8, // XPos, YPos;
	CP_BlackColor, CP_WhiteColor, // ForeColor, BackColor;
	CP_BlackColor, CP_BlueColor, // SelForeColor, SelBackColor;
	RootMenu, //NextMenuPage;
	NULL,//DWI_Text,// DrawMenuItemType;
	NULL,//LogoText, // DisplayText;
	LogoKeyEvent,// KeyEvent
	{ NULL, // AdjustFunction
	  NULL	// ExecFunction
	},// *DisplayValue;
	{ NULL, // DrawNumberType
	  NULL, // DrawGuageType
	  NULL	// DrawRadioGroupType
	},
      NULL, //Font
	mibSelectable // Flags
  }
};

//========================================================================
MenuItemType code PowerOffMenuItems[]=
{ { 0, 1, // XPos, YPos;
	0, 1, // ForeColor, BackColor;
	0, 1, // SelForeColor, SelBackColor;
	PowerOnMenu, //NextMenuPage;
	DWI_FullCenterText,// DrawMenuItemType;
	NULL, // DisplayText;
	NothingKeyEvent,//PowerOffKeyEvent, //KeyEvent
	{ NULL, // AdjustFunction
	  NULL			// ExecFunction
	},// DisplayValue;
	{ NULL, // DrawNumberType
	  NULL, // DrawGuageType
	  NULL	// DrawRadioGroupType
	},
      NULL, //Font
	mibSelectable // Flags
  }
};
MenuItemType code StandbyMenuItems[]=
{
    {
        0, 5, // XPos, YPos;
    	 0x0C, 0x0E, // ForeColor, BackColor;
    	 0x0C, 0x0E, // ForeColor, BackColor;
        RootMenu, //NextMenuPage;
        DWI_FullCenterText,// DrawMenuItemType;
        NoSignalText, // DisplayText;
        NothingKeyEvent,//PowerOffKeyEvent, //KeyEvent
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    }
    
};


MenuItemType code UnsupportedModeMenuItems[]=
{
    // Input Not
    {
        0, 5, // XPos, YPos;
    	 0x0C, 0x0E, // ForeColor, BackColor;
    	 0x0C, 0x0E, // ForeColor, BackColor;
        RootMenu, //NextMenuPage;
        DWI_FullCenterText,// DrawMenuItemType;
        OutOfRangeText, // DisplayText;
        NothingKeyEvent,  //KeyEvent
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

MenuItemType code RootMenuItems[]=
{ // virtual item
  { 0, 1, // XPos, YPos;
	0, 1, // ForeColor, BackColor;
	0, 1, // SelForeColor, SelBackColor;
	MainMenu, //NextMenuPage;
	NULL, //DWI_CenterText,// DrawMenuItemType;
	NULL, // DisplayText;
	RootKeyEvent,
	{ NULL, // AdjustFunction
	  NULL,//AutoConfig// ExecFunction
	},// DisplayValue;
	{ NULL, // DrawNumberType
	  NULL, // DrawGuageType
	  NULL	// DrawRadioGroupType
	},
      NULL, //Font
	mibSelectable // Flags
  }
};



//==============================================================================================
MenuPageType code tblMenus[]=
{ // 0 PowerOffMenu
  {    27, 9, //	XSize, YSize;
	PowerOnMenu,// PrevMenuPage;
	PowerOffMenuItems, // MenuItems;
	sizeof(PowerOffMenuItems)/sizeof(MenuItemType), // MenuItemCount;
	PowerOffSystem,// ExecFunction;
       NULL, // Fonts
	mpbInvisible|mpbStay // 	Flags;
  },
  // 1 PowerOnMenu
  {    27, 9, //	XSize, YSize;
	RootMenu,// PrevMenuPage;
	NULL, // MenuItems;
	NULL, // MenuItemCount;
	PowerOnSystem,// ExecFunction;
      NULL, // Fonts
      mpbInvisible // 	Flags;
  },
    // 2 StandbyMenu
    {
        0x19, 0x07, //  XSize, YSize;
        RootMenu,// PrevMenuPage;
        StandbyMenuItems, // MenuItems;
        sizeof( StandbyMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        EnablePowerDownCounter, // ExecFunction;
        NULL, // Fonts
        mpbStay  |mpbDrawFrame |mpbCenter // Flags;
    },
 
    // 4 UnsupportedModeMenu
  {    0x19, 0x07,  // 	XSize, YSize;
        RootMenu,// PrevMenuPage;
        UnsupportedModeMenuItems, // MenuItems;
        sizeof( UnsupportedModeMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // Fonts
        mpbStay|mpbCenter|mpbDrawFrame//   Flags;
    },
    // 5 RootMenu
    { 16, 3, // 	XSize, YSize;
        RootMenu,// PrevMenuPage;
        RootMenuItems, // MenuItems;
        sizeof( RootMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        EnterRootMenu, // ExecFunction;
        NULL, // Fonts
        mpbInvisible | mpbStay //   Flags;
    },
    //6 MainMenu
    { 0x29, 0x12, //24, //     XSize, YSize;//29
    	RootMenu,// PrevMenuPage;
    	MainMenuItems, // MenuItems;
    	sizeof(MainMenuItems)/sizeof(MenuItemType), // MenuItemCount;
    	NULL,   // ExecFunction;
        NULL,   // Fonts
      mpbStay|mpbDrawFrame|mpbItemFrame//   Flags;
    },
     //7 BriContrastMenuItems
    { 0x29, 0x12, //24, //     XSize, YSize;//29
    	MainMenu,// PrevMenuPage;
    	BriContrastMenuItems, // MenuItems;
    	sizeof(BriContrastMenuItems)/sizeof(MenuItemType), // MenuItemCount;
    	NULL,   // ExecFunction;
       NULL,   // Fonts
    	mpbStay//   Flags;
    },
    //8 PictureMenu
    { 0x29, 0x12, //24, //     XSize, YSize;//29
    	MainMenu,// PrevMenuPage;
    	PictureMenuItems, // MenuItems;
    	sizeof(PictureMenuItems)/sizeof(MenuItemType), // MenuItemCount;
    	NULL,   // ExecFunction;
       NULL,   // Fonts
    	mpbStay//   Flags;
    },
     //9  RGBColorMenu
    { 0x29, 0x12, //24, //     XSize, YSize;//29
    	MainMenu,// PrevMenuPage;
    	RGBMenuItems, // MenuItems;
    	sizeof(RGBMenuItems)/sizeof(MenuItemType), // MenuItemCount;
    	NULL,   // ExecFunction;
       NULL,   // Fonts
    	mpbStay//   Flags;
    },
    // 10 OsdMenu
    { 0x29, 0x12, //24, //     XSize, YSize;//29
    	MainMenu,// PrevMenuPage;
    	OSDMenuItems, // MenuItems;
    	sizeof(OSDMenuItems)/sizeof(MenuItemType), // MenuItemCount;
    	NULL,   // ExecFunction;
       NULL,   // Fonts
    	mpbStay//   Flags;
    },
    //11  LanguageMenu
    { 0x29, 0x12, //24, //     XSize, YSize;//29
    	OsdMenu,// PrevMenuPage;
    	LanguageMenuItems, // MenuItems;
    	sizeof(LanguageMenuItems)/sizeof(MenuItemType), // MenuItemCount;
    	NULL,   // ExecFunction;
       NULL,   // Fonts
    	mpbStay//   Flags;
    },
     //12  SettingMenu
    { 0x29, 0x12, //24, //     XSize, YSize;//29
    	MainMenu,// PrevMenuPage;
    	SettingMenuItems, // MenuItems;
    	sizeof(SettingMenuItems)/sizeof(MenuItemType), // MenuItemCount;
    	NULL,   // ExecFunction;
       NULL,   // Fonts
    	mpbStay//   Flags;
    },
     //13  OSD_MiscMenu
    { 0x29, 0x12, //24, //     XSize, YSize;//29
    	MainMenu,// PrevMenuPage;
    	MiscMenuItems, // MenuItems;
    	sizeof(MiscMenuItems)/sizeof(MenuItemType), // MenuItemCount;
    	NULL,   // ExecFunction;
       NULL,   // Fonts
    	mpbStay//   Flags;
    },
    
   #if EN_HotKeyDCRMenuWithIcon
     // 14 HotKeyDCROnMenu
	{ 0x19, 0x07, 		
	RootMenu,// PrevMenuPage;
	HotKeyDCROnMenuItems, // MenuItems;
	sizeof(HotKeyDCROnMenuItems)/sizeof(MenuItemType), // MenuItemCount;
	NULL, // ExecFunction;
	NULL, //Fonts
	mpbStay|mpbDrawFrame //   Flags;
	},
	// 15 HotKeyDCROffMenu
	{ 0x19, 0x07, 		
	RootMenu,// PrevMenuPage;
	HotKeyDCROffMenuItems, // MenuItems;
	sizeof(HotKeyDCROffMenuItems)/sizeof(MenuItemType), // MenuItemCount;
	NULL, // ExecFunction;
	NULL, //Fonts
	mpbStay|mpbDrawFrame //   Flags;
	},
	#endif
   #if EN_HotKeyECOMenuWithIcon
	// 16 HotKeyECOWebMenu
	{ 0x19, 0x07, 		
	RootMenu,// PrevMenuPage;
	HotKeyECOWebMenuItems, // MenuItems;
	sizeof(HotKeyECOWebMenuItems)/sizeof(MenuItemType), // MenuItemCount;
	NULL, // ExecFunction;
	NULL, //Fonts
	mpbStay|mpbDrawFrame //   Flags;
	},
	// 17 HotKeyECOStandardMenu
	{ 0x19, 0x07, 		
	RootMenu,// PrevMenuPage;
	HotKeyECOStandardMenuItems, // MenuItems;
	sizeof(HotKeyECOStandardMenuItems)/sizeof(MenuItemType), // MenuItemCount;
	NULL, // ExecFunction;
	NULL, //Fonts
	mpbStay|mpbDrawFrame //   Flags;
	},
	// 18 HotKeyECOGameMenu
	{ 0x19, 0x07, 		
	RootMenu,// PrevMenuPage;
	HotKeyECOGameMenuItems, // MenuItems;
	sizeof(HotKeyECOGameMenuItems)/sizeof(MenuItemType), // MenuItemCount;
	NULL, // ExecFunction;
	NULL, //Fonts
	mpbStay|mpbDrawFrame //   Flags;
	},
	// 19 HotKeyECOMoveMenu
	{0x19, 0x07, 		
	RootMenu,// PrevMenuPage;
	HotKeyECOMoveMenuItems, // MenuItems;
	sizeof(HotKeyECOMoveMenuItems)/sizeof(MenuItemType), // MenuItemCount;
	NULL, // ExecFunction;
	NULL, //Fonts
	mpbStay|mpbDrawFrame//   Flags;
	},
	#endif
	// 20  HotKeyContrastMenu
	{ 0x19, 0x07, 		
	RootMenu,// PrevMenuPage;
	HotKeyContMenuItems, // MenuItems;
	sizeof(HotKeyContMenuItems)/sizeof(MenuItemType), // MenuItemCount;
	NULL, // ExecFunction;
	NULL, //Fonts
	mpbStay|mpbDrawFrame//   Flags;
	},
	// 21  HotKeyBrightnessMenu
	{ 0x19, 0x07, 		
	RootMenu,// PrevMenuPage;
	HotKeyBriteMenuItems, // MenuItems;
	sizeof(HotKeyBriteMenuItems)/sizeof(MenuItemType), // MenuItemCount;
	NULL, // ExecFunction;
	NULL, //Fonts
	mpbStay|mpbDrawFrame //   Flags;
	},
 #if KeyInputSelectMenu
	// 22  HotKeyInputSelectMenu
	{ 12, 3+InputType_Analog1*2+InputType_Digital*2+InputType_HDMI*2+ENABLE_DP_INPUT*2, 		
	RootMenu,// PrevMenuPage;
	HotKeyInputSelectMenuItems, // MenuItems;
	sizeof(HotKeyInputSelectMenuItems)/sizeof(MenuItemType), // MenuItemCount;
	NULL, // ExecFunction;
	NULL, //Fonts
	mpbStay|mpbDrawFrame //   Flags;
	},
#endif	

 #if AudioFunc
	// 22 HotKeyVolumeMenu
	{ 0x19, 7, // 	XSize, YSize;
	RootMenu,// PrevMenuPage;
	HotKeyVolumeMenuItems, // MenuItems;
	sizeof(HotKeyVolumeMenuItems)/sizeof(MenuItemType), // MenuItemCount;
	NULL, // ExecFunction;
	NULL, //Fonts
	mpbStay|mpbDrawFrame//   Flags;
	},
     // 22+1 HotKeyMuteMenu
	{ 0x19, 7, // 	XSize, YSize;
	RootMenu,// PrevMenuPage;
	HotKeyMuteMenuItems, // MenuItems;
	sizeof(HotKeyMuteMenuItems)/sizeof(MenuItemType), // MenuItemCount;
	NULL, // ExecFunction;
	NULL, //Fonts
	mpbStay|mpbDrawFrame//   Flags;
	},
#endif
#if Enable_Menu_LockOSD
	// OSDLockedMenu
	{ 0x19, 0x07, // 	XSize, YSize;
	RootMenu,// PrevMenuPage;
	OSDLockedMenuItems, // MenuItems;
	sizeof(OSDLockedMenuItems)/sizeof(MenuItemType), // MenuItemCount;
	NULL, // ExecFunction;
	NULL, //Fonts
	mpbStay|mpbDrawFrame//   Flags;
	},
	// OSDUnLockedMenu
	{ 0x19, 0x07, // 	XSize, YSize;
	RootMenu,// PrevMenuPage;
	OSDUnLockedMenuItems, // MenuItems;
	sizeof(OSDUnLockedMenuItems)/sizeof(MenuItemType), // MenuItemCount;
	NULL, // ExecFunction;
	NULL, //Fonts
	mpbStay|mpbDrawFrame//   Flags;
	},
#endif
#if Enable_LockPowerKey
	// OSDLockedMenu
	{ 0x19, 0x07, // 	XSize, YSize;
	RootMenu,// PrevMenuPage;
	PowerLockedMenuItems, // MenuItems;
	sizeof(PowerLockedMenuItems)/sizeof(MenuItemType), // MenuItemCount;
	NULL, // ExecFunction;
	NULL, //Fonts
	mpbStay|mpbDrawFrame//   Flags;
	},
	// OSDUnLockedMenu
	{ 0x19, 0x07, // 	XSize, YSize;
	RootMenu,// PrevMenuPage;
	PowerUnLockedMenuItems, // MenuItems;
	sizeof(PowerUnLockedMenuItems)/sizeof(MenuItemType), // MenuItemCount;
	NULL, // ExecFunction;
	NULL, //Fonts
	mpbStay|mpbDrawFrame//   Flags;
	},
#endif
#if Los_Select
	// HotKeyCorssMenu
	{ 10, 6, // 	XSize, YSize;
	RootMenu,// PrevMenuPage;
	CorssMenuItems, // MenuItems;
	sizeof(CorssMenuItems)/sizeof(MenuItemType), // MenuItemCount;
	NULL, // ExecFunction;
	NULL, //Fonts
	mpbStay|mpbDrawFrame//   Flags;
	},
#endif
#if  LowBlueLight 
	// 22  HotKeyLowBlueLightMenu
	{ 0x1E, 0x07,		
	RootMenu,// PrevMenuPage;
	HotKeyLowBlueLightMenuItems, // MenuItems;
	sizeof(HotKeyLowBlueLightMenuItems)/sizeof(MenuItemType), // MenuItemCount;
	NULL, // ExecFunction;
	NULL, //Fonts
	mpbStay|mpbDrawFrame //   Flags;
	},
	
#endif
	// 23  AutoMenu,
  	{ 0x19, 0x07, // 	XSize, YSize;0x19, 0x07,//
	RootMenu,// PrevMenuPage;
	AutoMenuItems ,// MenuItems;
	sizeof( AutoMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
	AutoConfig, // ExecFunction;
	NULL, // Fonts
	mpbCenter|mpbDrawFrame //   Flags; //110301 Modify for AutoMenu Garbage
    	},
  #if ShowSourceType
	// 24 InputTypeMenu
  { 11, 5, // 	XSize, YSize;
    RootMenu,// PrevMenuPage;
    InputTypeMenuItems, // MenuItems;
    sizeof(InputTypeMenuItems)/sizeof(MenuItemType), // MenuItemCount;
    NULL, // ExecFunction;
     NULL, //Fonts
    mpbStay|mpbDrawFrame //   Flags;
  },
#endif

 // 25 BurninMenu   老化菜单画板大小
  { 26, 6, // 	XSize, YSize;
    RootMenu,// PrevMenuPage;
    BurninMenuItems, // MenuItems;
    sizeof(BurninMenuItems)/sizeof(MenuItemType), // MenuItemCount;
    EnableBurninMode, // ExecFunction;
    NULL, // Fonts
#if ENABLE_BURNIN_MENU
    mpbStay|mpbDrawFrame|mpbMoving//   Flags;
#else
   mpbStay|mpbInvisible//   Flags;
#endif	
  },
  // 26 FactoryMenu
   { 24, 20, //17,  	XSize, YSize;  wmz 20050106
    RootMenu,// PrevMenuPage;
    FactoryMenuItems, // MenuItems;
    sizeof(FactoryMenuItems)/sizeof(MenuItemType), // MenuItemCount;
    NULL, // ExecFunction;
    NULL, // Fonts
    mpbStay|mpbDrawFrame//   Flags;
  },
  // 27 LogoMenu
  {
   LOGO_WIGHT, LOGO_HIGHT, 	 // XSize, YSize;											// XSize, YSize;
   MainMenu,// PrevMenuPage;
   LogoMenuItems, // MenuItems;
   sizeof(LogoMenuItems)/sizeof(MenuItemType), // MenuItemCount;
   EnterDisplayLogo, // ExecFunction;
   NULL, // Fonts
   mpbStay|mpbCenter|mpbLogoFrame //	Flags;
  },
#if EN_HotKeyTimes
	   // 28   
	{ 12, 3+16, 	   
	   RootMenu,// PrevMenuPage;
	   HotKeyHotKeyTimesSelectMenuItems, // MenuItems;
	   sizeof(HotKeyHotKeyTimesSelectMenuItems)/sizeof(MenuItemType), // MenuItemCount;
	   NULL, // ExecFunction;
	   NULL, //Fonts
	   mpbStay|mpbDrawFrame //	 Flags;
	 },
	// 29
	{ 7, 3, 	 //XSize, YSize; 
	RootMenu,// PrevMenuPage;
	HotKeyHotKeyTimesShowMenuItems, // MenuItems;
	sizeof(HotKeyHotKeyTimesShowMenuItems)/sizeof(MenuItemType), // MenuItemCount;
	NULL, // ExecFunction;
	NULL, //Fonts
	mpbStay|mpbDrawFrame //   Flags;
	},
	   
#endif
#if EN_HotKeyXINGZHUN
   // 28   
{ 12, 3+16, 	   
   RootMenu,// PrevMenuPage;
   HotKeyXZShowMenuItems, // MenuItems;
   sizeof(HotKeyXZShowMenuItems)/sizeof(MenuItemType), // MenuItemCount;
   NULL, // ExecFunction;
   NULL, //Fonts
	mpbStay|mpbDrawFrame|mpbItemFrame//   Flags;
 },
// 29
{ 4, 4, 	 //XSize, YSize; 
RootMenu,// PrevMenuPage;
HotKeyXZShowMenuItems1, // MenuItems;
sizeof(HotKeyXZShowMenuItems1)/sizeof(MenuItemType), // MenuItemCount;
NULL, // ExecFunction;
NULL, //Fonts
	mpbStay|mpbDrawFrame|mpbItemFrame//   Flags;
},
// 29
{ 6, 4, 	 //XSize, YSize; 
RootMenu,// PrevMenuPage;
HotKeyXZShowMenuItems2, // MenuItems;
sizeof(HotKeyXZShowMenuItems2)/sizeof(MenuItemType), // MenuItemCount;
NULL, // ExecFunction;
NULL, //Fonts
	mpbStay|mpbDrawFrame|mpbItemFrame//   Flags;
},
// 29
{ 6, 4, 	 //XSize, YSize; 
RootMenu,// PrevMenuPage;
HotKeyXZShowMenuItems3, // MenuItems;
sizeof(HotKeyXZShowMenuItems3)/sizeof(MenuItemType), // MenuItemCount;
NULL, // ExecFunction;
NULL, //Fonts
	mpbStay|mpbDrawFrame|mpbItemFrame//   Flags;
},
// 29
{ 4, 4, 	 //XSize, YSize; 
RootMenu,// PrevMenuPage;
HotKeyXZShowMenuItems4, // MenuItems;
sizeof(HotKeyXZShowMenuItems4)/sizeof(MenuItemType), // MenuItemCount;
NULL, // ExecFunction;
NULL, //Fonts
	mpbStay|mpbDrawFrame|mpbItemFrame//   Flags;
},
// 29
{ 6, 4, 	 //XSize, YSize; 
RootMenu,// PrevMenuPage;
HotKeyXZShowMenuItems5, // MenuItems;
sizeof(HotKeyXZShowMenuItems5)/sizeof(MenuItemType), // MenuItemCount;
NULL, // ExecFunction;
NULL, //Fonts
	mpbStay|mpbDrawFrame|mpbItemFrame//   Flags;
},
{ 6, 4, 	 //XSize, YSize; 
RootMenu,// PrevMenuPage;
HotKeyXZShowMenuItems6, // MenuItems;
sizeof(HotKeyXZShowMenuItems6)/sizeof(MenuItemType), // MenuItemCount;
NULL, // ExecFunction;
NULL, //Fonts
mpbStay|mpbDrawFrame|mpbItemFrame//   Flags;
},
#endif
#if 0//ENABLE_BURNIN_MENU
{ 6, 4, 	 //XSize, YSize; 
RootMenu,// PrevMenuPage;
HotKeyXZShowMenuItems6, // MenuItems;
sizeof(HotKeyXZShowMenuItems6)/sizeof(MenuItemType), // MenuItemCount;
NULL, // ExecFunction;
NULL, //Fonts
mpbStay|mpbDrawFrame|mpbItemFrame//   Flags;
},

#endif
};




