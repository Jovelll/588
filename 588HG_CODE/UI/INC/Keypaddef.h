
typedef enum
{ 
  KEY_POWER =BIT0, // power
  KEY_PLUS	 =BIT2,//BIT2, // right
  KEY_MINUS =BIT3,//BIT3, // left
  KEY_MENU	 =BIT1,//BIT1, // menu
  KEY_EXIT	 =BIT4,//BIT4, // exit/auto
  KEY_IncVal  =BIT5, //2006-02-22  source select 
  KEY_DecVal  =BIT6, //2006-02-22  source select 
  KEY_FACTORY	= KEY_POWER|KEY_EXIT, 
  KEY_LOCK		=KEY_PLUS|KEY_MINUS,
  KEY_MUTE		=KEY_EXIT|KEY_MENU,
#if ENABLE_FACTORY_AUTOTEST
 KEY_FACTORY_AUTOTEC =KEY_POWER|KEY_EXIT|KEY_PLUS, //KEY_MENU,//
#endif  
#if (Customer_Type_Sel==Customer_XINTAO)
	KEY_BRUNIN= KEY_POWER|KEY_PLUS,
#endif
  KEY_NOTHING	=0
} KeypadMaskType;

typedef enum
{ 
  BTN_Plus,
  BTN_Minus,
  BTN_Menu,
  BTN_Exit,
  BTN_Power,
  BTN_IncVal,	
  BTN_DecVal,	

  BTN_EndBTN,
  BTN_Repeat,

  BTN_Nothing
} ButtonType;

#if (SevenKey_Function)
#define KeypadMask (KEY_POWER|KEY_PLUS|KEY_MINUS|KEY_MENU|KEY_EXIT|KEY_DecVal|KEY_IncVal)
#else
#define KeypadMask (KEY_POWER|KEY_PLUS|KEY_MINUS|KEY_MENU|KEY_EXIT)
#endif

