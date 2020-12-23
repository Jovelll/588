#include <math.h>    //MST_MCU
#include "types.h"
#include "board.h"
#include "keypaddef.h"
#include "ms_reg.h"
#include "global.h"
#include "debug.h"
#include "Common.h"
#include "misc.h"
#include "ms_rwreg.h"
#include "mcu.h"
#include "menudef.h"
#include "gpio_def.h"
#if USEFLASH
#include "userpref.h"
#endif

//VS CCFLAG ADD
#if TimerComboKey
bit bDoComboKeyFlag=0;
bit bEnableComboKeyFlag=0;
BYTE XDATA ComboCounter=0;
BYTE XDATA ComboKey=0;
BYTE XDATA PreComboKey=0;
//BYTE XDATA ComboKeyType=0;
#endif
BYTE xdata KeyDebug=0;

#if IR_FUNC_EN	//20130824
extern bit bIrHoldFlag;
extern bit bIrEndFlag;
extern WORD idata g_IRRawdata[2];
extern unIrDataType idata unIrData;
WORD xdata g_IRdata[2];
BYTE idata byMixKeyBak = 0xff; // 复合键存储单元;
WORD xdata Backup_IRdata[2];
bit IRSourceFlag=0;
bit IRInputSourceFlag=0;
#endif

#if ENABLE_FACTORY_AUTOTEST
extern bit BFactoryAutoTest;
#endif

extern void Osd_Hide( void );
extern void SaveMonitorSetting( void );
extern Bool ExecuteKeyEvent( MenuItemActionType menuAction );


//================================================================
extern BYTE xdata MenuPageIndex;
//extern BYTE xdata MenuItemIndex;

#define  adckey1(temp) 		((temp>=0x00 && temp<0x0A))
#define  adckey2(temp)		((temp>=0x42 && temp<0x4F))  //40--4A,   00,  48,  70
//#define  adckey3(temp)		((temp>=0x69 && temp<0x75))  //v3811  ok
#define  adckey3(temp)		((temp>=0x69&& temp<0x85))   //v3811+v8811  ok

typedef enum
	   {   
		   ADC1key1   =KEY_EXIT,		
		   ADC1key2   =KEY_IncVal,
		   ADC1key3   =KEY_PLUS,	   	 //  + up  
		   ADC2key1   =KEY_MINUS,	 //  - down 
		   ADC2key2   =KEY_MENU,
		   ADC2key3   =KEY_DecVal,	  
		   IOkey1	 =KEY_POWER,	  
	   } KeypadType;

//2006-07-10

BYTE Key_GetKeypadStatus( void )
{ 

	BYTE keypad = 0xFF;
	BYTE temp;
	BYTE loop=0;
	#if ENABLE_FACTORY_AUTOTEST
	BYTE FACTORY_AUTOTESTflash=0;
	#endif
   temp = KEYPAD_ADC_A&0xFF;//sar1	
	

	if(temp!=0xFF)
	{

  	 Delay1ms(2);
	while((abs(temp-(KEYPAD_ADC_A&0xFF))<2)&&(loop<15))
	{
			if(temp!=0xFF)
        #if 0//(MainBoardType==MainBoard_TSUML88CMC2)
				  Delay1ms(2);
        #else
				  Delay1ms(1);
        #endif
		loop++;
	}
	if(loop<15)
	return 0xFF;
	 }
  
#if 0//ENABLE_DEBUG 
	if(temp<0xf0)
	printData("ADC2=%d",temp);
#endif
		//if (adckey1(temp))
	//	keypad &= ~ADC2key3;
		//else if(adckey2(temp))
		//keypad &= ~ADC1key2;
		//else if(adckey3(temp))
		//keypad &=~ADC1key1;
   #if 0
       if(temp < 90) // 0
			 keypad &= ~KEY_POWER;
		 else if(temp <140) // 0x48
			 keypad &=~KEY_MENU;//
  #elif Customer_Type_Sel==Customer_GUANRONG
  
       if(temp < 0x20) // 0
     	  keypad &=~KEY_MINUS;//
       else if(temp < 0xf0) // 0x48
     	  keypad &=  ~KEY_PLUS;//~KEY_EXIT;//
   #else
#if Customer_Type_Sel==Customer_HONGXUN
	//鸿讯按键
		if(temp < 0x20) // 0
		   keypad &=~KEY_EXIT;//~KEY_MINUS;// 
	   else if(temp < 0xf0) // 0x48
		   keypad &= ~KEY_MINUS;//~KEY_EXIT;//
#elif	0//DisplayLogo ==SONGUAN	
		
	if(temp < 0x20) // 0
		   keypad &=~KEY_PLUS;//~KEY_MINUS;// ~KEY_EXIT;
	else if(temp < 0xf0) // 0x48
		   keypad &= ~KEY_EXIT;//~KEY_MINUS;//~KEY_MINUS;
#elif Enable_4LINE_KEY////Enable_ONE_LED//1       
	if(temp < 90) // 0    
		keypad &= ~KEY_POWER;   
	else if(temp <140) // 0x48    
		keypad &=~KEY_EXIT;//~KEY_MENU;//
#else
  // 原来的按键
	if(temp < 0x20) // 0
		   keypad &=~KEY_MINUS;// ~KEY_EXIT;
	   else if(temp < 0xf0) // 0x48
		   keypad &= ~KEY_EXIT;//~KEY_MINUS;
#endif
   #endif

#if ENABLE_FACTORY_AUTOTEST
   if(temp < 0x20) // 0x48
      FACTORY_AUTOTESTflash++;
#endif	   

	loop=0;
	temp = KEYPAD_ADC_B&0xFF;
	if(temp!=0xFF)
	{
		  Delay1ms(2);
	while((abs(temp-(KEYPAD_ADC_B&0xFF))<2)&&(loop<15))
	{
			if(temp!=0xFF)
		Delay1ms(1);
		loop++;
	}
	if(loop<15)
	return 0xFF;
	 }
#if 0//ENABLE_DEBUG 
	if(temp<0xf0)
	printData("ADC3=%d",temp);
#endif	
		//if(adckey1(temp))
		//keypad &= ~ADC2key1;
		//else if(adckey2(temp))
		//keypad &= ~ADC1key3;
		//else if(adckey3(temp))
		//keypad &=~ADC2key2;

  #if 0
 			if(temp < 20) // 0
				keypad &=~KEY_PLUS;//
			else if(temp < 80) // 0x48
				keypad &= ~KEY_EXIT;
			else if(temp < 120) // 0x48
				keypad &=~KEY_MINUS;
#elif Customer_Type_Sel==Customer_GUANRONG
      if(temp < 0x20) // 0
    		  keypad &=~KEY_MENU;//  ~KEY_PLUS;//
    	  else if(temp < 0x60) // 0x48
    		  keypad &= ~KEY_MINUS;
    	  else if(temp < 0x80) // 0x48
    		  keypad &=~KEY_EXIT;//~KEY_MENU;//
    	  
      if(PowerKey == 0 )
       {
    	  keypad &= ~IOkey1;
       }

  #else

#if Customer_Type_Sel==Customer_HONGXUN
	//鸿讯按键		
			if(temp < 0x20) // 0
				keypad &= ~KEY_PLUS;//~KEY_MENU;
			else if(temp < 0x60) // 0x48
				keypad &=~KEY_MENU;// ~KEY_MINUS;
			else if(temp < 0x80) // 0x48
				keypad &=~KEY_EXIT;//;//~KEY_MENU;//  ~KEY_SELECT;
#elif	0//DisplayLogo ==SONGUAN	
			if(temp < 0x20) // 0
				keypad &= ~KEY_PLUS;//~KEY_MENU;// ~KEY_PLUS;
			else if(temp < 0x60) // 0x48
				keypad &= ~KEY_MINUS;
			else if(temp < 0x80) // 0x48
				keypad &=~KEY_MENU;
#elif Enable_4LINE_KEY//    
		if(temp < 20) // 0    
			keypad &=~KEY_PLUS;//   
		else if(temp < 80) // 0x48    
			keypad &= ~KEY_MENU;//~KEY_EXIT;   
		else if(temp < 120) // 0x48    
			keypad &=~KEY_MINUS;	
 #else
//原来的按键
		if(temp < 0x20) // 0
				keypad &=  ~KEY_PLUS;//~KEY_MENU;
			else if(temp < 0x60) // 0x48
				keypad &= ~KEY_MINUS;
			else if(temp < 0x80) // 0x48
				keypad &=~KEY_MENU;//  ~KEY_SELECT;
#endif			
		if(PowerKey == 0 )
		 {
		 #if ENABLE_FACTORY_AUTOTEST
		FACTORY_AUTOTESTflash++;
		 #endif
			keypad &= ~IOkey1;
		 }
  #endif

#if ENABLE_FACTORY_AUTOTEST
	 if(temp < 0x20) // 0x48
		FACTORY_AUTOTESTflash++;



      if(FACTORY_AUTOTESTflash==3)
	  	keypad=~KEY_FACTORY_AUTOTEC;

	 
#endif

		//if(keypad!=0xff)
		//printData("keypad=%d",keypad);

		return keypad;
}

void Key_ScanKeypad(void)
{
    BYTE keypadStatus=0;

#if ENABLE_FACTORY_AUTOTEST

	  if (BFactoryAutoTest==1)
			{ 
			return;
            }
#endif

#if IR_FUNC_EN	//20130824	
	
				if (bIrHoldFlag || bIrEndFlag)
				{ // Ir key;
	
				BYTE i;
				BYTE  tempdata,temp11,temp22,temp33,temp44,RepeatFlag=0;
				
					temp11=g_IRRawdata[0]>>8;
					temp22=(BYTE)(g_IRRawdata[0]&0xff);
					temp33=g_IRRawdata[1]>>8;
					temp44=(BYTE)(g_IRRawdata[1]&0xff);
					
	
					tempdata =temp11;
					temp11 =0;
					for(i =0;i<7;i++)
					{
						temp11=(temp11 | (tempdata&0x01));
						tempdata=tempdata>>1;
						temp11 =temp11 <<1;
					}
					temp11=(temp11 | (tempdata&0x01));
					
	
					tempdata =temp22;
					temp22 =0;
					for(i =0;i<7;i++)
					{
						temp22=(temp22 | (tempdata&0x01));
						tempdata=tempdata>>1;
						temp22 =temp22 <<1;
					}
					temp22=(temp22 | (tempdata&0x01));
					
	
					tempdata =temp33;
					temp33 =0;
					for(i =0;i<7;i++)
					{
						temp33=(temp33 | (tempdata&0x01));
						tempdata=tempdata>>1;
						temp33 =temp33 <<1;
					}
					temp33=(temp33 | (tempdata&0x01));
									
	
					tempdata =temp44;
					temp44 =0;
					for(i =0;i<7;i++)
					{
						temp44=(temp44 | (tempdata&0x01));
						tempdata=tempdata>>1;
						temp44 =temp44 <<1;
					}
					temp44=(temp44 | (tempdata&0x01));				
					
	
					g_IRdata[0]=(temp11<<8)|temp22;
					g_IRdata[1]=(temp33<<8)|temp44;
					if(bIrHoldFlag)
					{					
						bIrHoldFlag = 0;
						IRClearCounter++;
											RepeatFlag=1;
							if(IRClearCounter>5)
								{
									g_IRdata[0]=Backup_IRdata[0];
									g_IRdata[1]=Backup_IRdata[1];
								}
					}
					if(bIrEndFlag)
					{
						bIrEndFlag = 0;
						IRClearCounter=0;
						Backup_IRdata[0]=g_IRdata[0];
						Backup_IRdata[1]=g_IRdata[1];
					}
			#if ENABLE_DEBUG
					printData("ccflag1111=%x", temp11 );
					printData("ccflag222=%x", temp22 );
					printData("ccflag333=%x", temp33 );
					printData("ccflag444=%x", temp44 );
					printData("g_IRdata0=%x",g_IRdata[0]);
					printData("g_IRdata1=%x", g_IRdata[1]);
					printData("IRClearCounter=%x", IRClearCounter);
		//			printData("IRFLAG=%x", IRFLAG);
			#endif	
		//				IRFLAG = 0;
					switch (g_IRdata[1])
					{
						case IR_KEY_POWER: // IR Power key;
						{
							if (g_IRdata[0]==IR_CUSTOMER_CODE)
							{
								keypadStatus = ~KEY_POWER;
								byMixKeyBak &= ~KEY_POWER;
								{
									g_IRdata[1] = 0; // 使该键无连续动作;
								}
							}
						}
						break;
						
					case IR_KEY_MENU:
					case IR_KEY_ENTRY:
						{
							if (g_IRdata[0]==IR_CUSTOMER_CODE)
							{
								keypadStatus = ~ KEY_MENU;
								byMixKeyBak &= ~ KEY_MENU;
								{
									g_IRdata[1] = 0; // 使该键无连续动作;
								}
							}
						}
						break;
					case IR_KEY_RIGHT:
						{
							if (g_IRdata[0]==IR_CUSTOMER_CODE)
							{
								keypadStatus = ~KEY_IncVal;
								byMixKeyBak &= ~KEY_IncVal;
								{
									g_IRdata[1] = 0; // 使该键无连续动作;
								}
							}
			
						}
						break;
					case IR_KEY_LEFT:
						{	
							if (g_IRdata[0]==IR_CUSTOMER_CODE)
							{
									keypadStatus = ~KEY_DecVal;
								byMixKeyBak &= ~KEY_DecVal;
								{
									g_IRdata[1] = 0; // 使该键无连续动作;
								}
							}
						}
						break;
					case IR_KEY_DOWN:
						{	
							if (g_IRdata[0]==IR_CUSTOMER_CODE)
							{
									keypadStatus = ~KEY_MINUS;
								byMixKeyBak &= ~KEY_MINUS;
								{
									g_IRdata[1] = 0; // 使该键无连续动作;
								}
							}
						}
						break;
						case IR_KEY_UP:
						{	
							if (g_IRdata[0]==IR_CUSTOMER_CODE)
							{
									keypadStatus = ~KEY_PLUS;
								byMixKeyBak &= ~KEY_PLUS;
								{
									g_IRdata[1] = 0; // 使该键无连续动作;
								}
							}
						}
						break;
					case IR_KEY_EXIT:
						{
							if ((g_IRdata[0]==IR_CUSTOMER_CODE))
							{
									keypadStatus = ~ KEY_EXIT;
								byMixKeyBak &= ~ KEY_EXIT;
								{
									g_IRdata[1] = 0; // 使该键无连续动作;
								}
							}
						}
						break;
					case IR_Hot43CHING169:
						{
							if (g_IRdata[0]==IR_CUSTOMER_CODE)
							{
								ExecuteKeyEvent(MIA_HOT43CHING169);
								{
									g_IRdata[1] = 0; // 使该键无连续动作;
									Delay1ms(500);
								}
							}
	
						}
						break;
					case IR_HotChangeSource:
						{
							if (g_IRdata[0]==IR_CUSTOMER_CODE)
							{
								ExecuteKeyEvent(MIA_HotInput);
								{
									g_IRdata[1] = 0; // 使该键无连续动作;
									Delay1ms(500);
								}
							}
							
						}
						break;
			#if	AudioFunc
					   case IR_HotMUTE:
						{
							if (g_IRdata[0]==IR_CUSTOMER_CODE)
							{
								ExecuteKeyEvent(MIA_HotMute);
								{
									g_IRdata[1] = 0; // 使该键无连续动作;
									Delay1ms(500);
								}
							}
							
						}
						break;
			#endif
	//----------------------------------------------------------------------------	
						default:
						{
							keypadStatus = 0;
							byMixKeyBak = 0xff;
							IRSourceFlag=0;
							IRInputSourceFlag=0;
						}
						break;
					}		
					g_IRdata[0]=0;
					
					if (PowerOnFlag == 0)
					{
						keypadStatus = byMixKeyBak;
					}
					keypadStatus=(keypadStatus^KeypadMask)&KeypadMask;
					
				}
				else				
#endif

    if (gBoolVisualKey == 0)
    	{
		keypadStatus = (Key_GetKeypadStatus() ^ KeypadMask) &KeypadMask;
    	}
    else if (gBoolVisualKey == 1)
     {
        gBoolVisualKey = 0;
        keypadStatus = (gByteVisualKey ^ KeypadMask) &KeypadMask;
     }
   
    if (keypadStatus)
    {
	//	printData("keypad112233=%d", keypadStatus );
		#if 0//ENABLE_DEBUG&&DEBUG_SCANKEY// 090624 coding test
		printData("  keypadStatus:%d", keypadStatus);
		
		#endif
        {
            if (keypadStatus == KEY_LOCK )
            {
                // 091019
                #if 0//ENABLE_DEBUG
               // printData("  keypadStatus == KEY_LOCK", 1);
                #endif
                if (!PowerOnFlag && !(MonitorFlags &OsdLockModeFlag))
                {
                    Set_OsdLockModeFlag();
                    //Set_PowerOnFlag();
                    Set_ShowOsdLockFlag();
                    KeypadButton = BTN_Power;
                }
                else if ((!PowerOnFlag) && (MonitorFlags &OsdLockModeFlag))
                {
                    Clr_OsdLockModeFlag();
                    //Set_PowerOnFlag();
                    Clr_ShowOsdLockFlag();
                    KeypadButton = BTN_Power;
                }
    		    else if(PowerOnFlag)
    		        KeypadButton = BTN_Repeat;

    	        #if USEFLASH
                UserPref_EnableFlashSaveBit(bFlashSaveMonitorBit);
                #else
                SaveMonitorSetting();
                #endif

                
            }
            else if (keypadStatus == KEY_PLUS)
		{
                if (MenuPageIndex == RootMenu)
                {
                    if (KeypadButton == BTN_Plus || KeypadButton == BTN_Repeat)
                            KeypadButton = BTN_Repeat;
                    else
                        KeypadButton = BTN_Plus;
                }
                else
                    KeypadButton = BTN_Plus;
			#if ENABLE_DEBUG
			printMsg("press KEY_PLUS");
			#endif
            }
            
            else if (keypadStatus == KEY_MINUS)
            {
                if (MenuPageIndex == RootMenu)
                {
                    if (KeypadButton == BTN_Minus || KeypadButton == BTN_Repeat)
                            KeypadButton = BTN_Repeat;
                    else
                        KeypadButton = BTN_Minus;
                }
                else
                    KeypadButton = BTN_Minus;
			#if ENABLE_DEBUG
             		printMsg("press KEY_MINUS");
			#endif
            }
            else if (keypadStatus == KEY_MENU)
            {
                if ((KeypadButton == BTN_Menu || KeypadButton == BTN_Repeat))
                    KeypadButton = BTN_Repeat;
                else
	                KeypadButton = BTN_Menu;

				
		#if ENABLE_DEBUG
			printMsg("press KEY_MENU");
		#endif
		
#if LOCKPOWER_USE_OTHER_KEY
		#if TimerComboKey && Enable_LockPowerKey
				ComboKey=BTN_Menu;
		#endif
#endif		 	
            }
            else if (keypadStatus == KEY_EXIT)
            {
                if (KeypadButton == BTN_Exit || KeypadButton == BTN_Repeat)	//120420 Modify
                    KeypadButton = BTN_Repeat;
                else
                    KeypadButton = BTN_Exit;
            	  
		#if TimerComboKey && Enable_Menu_LockOSD
			ComboKey=BTN_Exit;
		#endif
		#if ENABLE_DEBUG
                printMsg("press KEY_EXIT");
		#endif
            }
	#if (SevenKey_Function)
            else if (keypadStatus == KEY_DecVal)
            {
                if (KeypadButton == BTN_Repeat)
                    KeypadButton = BTN_Repeat;
                else
                {
                    KeypadButton = BTN_DecVal;
                }
            }
	    else if (keypadStatus == KEY_IncVal)
            {
                if (KeypadButton == BTN_Repeat)
                    KeypadButton = BTN_Repeat;
                else
                {
                    KeypadButton = BTN_IncVal;
                }
            }
	    #endif
            else if (keypadStatus == KEY_POWER)
            {
                {
                    if (KeypadButton == BTN_Power || KeypadButton == BTN_Repeat)
                    {
                	    KeypadButton = BTN_Repeat;
                    }
                    else	
                    {
				KeypadButton = BTN_Power;
		#if (Customer_Type_Sel==Customer_XINTAO)
				if (FactoryModeFlag||BurninModeFlag)
		#else
				if (FactoryModeFlag)
		#endif
				{
				Clr_SaveSettingFlag();

				Clr_FactoryModeFlag();
				Clr_BurninModeFlag();
				Clr_DoBurninModeFlag();	//120112 Modify for TPV Request
				}
                    }
                }	
		#if ENABLE_DEBUG
			printMsg("press KEY_POWER");
		#endif

#if !LOCKPOWER_USE_OTHER_KEY
		#if TimerComboKey && Enable_LockPowerKey
			ComboKey=BTN_Power;
		#endif
#endif
            }
	else  if (keypadStatus==KEY_FACTORY)
		{ 
			#if ENABLE_DEBUG
			printMsg(" key FACTORY");
			#endif
			
			if(FactoryModeFlag)   
		    	   KeypadButton=BTN_Repeat;
			else
	    		   KeypadButton=BTN_Power;
			
		     if(!PowerOnFlag)
		    	{
		    		Set_FactoryModeFlag();

		    		if (SyncLossState())
		    			{
						Set_BurninModeFlag();
						Set_DoBurninModeFlag();		//120703 Modify for TPV Request
					}	
		    	}	

		}	
#if(Customer_Type_Sel==Customer_XINTAO)
	else  if (keypadStatus==KEY_BRUNIN)
		{ 
			#if ENABLE_DEBUG
			printMsg(" key BRUNIN");
			#endif
			
			if(FactoryModeFlag)   
		    	   KeypadButton=BTN_Repeat;
			else
	    		   KeypadButton=BTN_Power;
			
		     if(!PowerOnFlag)
		    	{
		    		
						Set_BurninModeFlag();
						Set_DoBurninModeFlag();		//120703 Modify for TPV Request
						
		    	}	

		}	
#endif
#if ENABLE_FACTORY_AUTOTEST
   else  if (keypadStatus==KEY_FACTORY_AUTOTEC)
	  	{ 
	  	
#if ENABLE_DEBUG
		printMsg(" KEY_FACTORY_AUTOTEC");
#endif
		//if(BFactoryAutoTest)   
		//	   KeypadButton=BTN_Repeat;
		//else
			   KeypadButton=BTN_Nothing;
		
	  	   BFactoryAutoTest=1;
	  		#if 0//ENABLE_DEBUG
	  		printMsg(" key FACTORY");
	  		#endif	  		
               //Clr_FactoryModeFlag();
               //Clr_BurninModeFlag();    
               //Clr_DoBurninModeFlag();	
			   Second=0;

		   
			   if (SrcInputType!= Input_VGA)
				   {				   
			   SrcInputType=UserPrefSelectInputType=Input_VGA;  
			   mStar_SetupInputPort();
			   Set_InputTimingChangeFlag();
			   SrcFlags |= SyncLoss;
				   }



			   
	  	}	

#endif	
            else
            {
                KeypadButton = BTN_Nothing;
            }                                
        }
		//--------------------------
#if TimerComboKey
      #if Enable_Menu_LockOSD
	 if(ComboKey==BTN_Exit && keypadStatus==KEY_EXIT)	
		{
                if (PowerOnFlag)
                {
                    bEnableComboKeyFlag=1;
                }
        }
       #endif
	  #if Enable_LockPowerKey
	  #if LOCKPOWER_USE_OTHER_KEY
		else if (ComboKey==BTN_Menu && keypadStatus==KEY_MENU)
	  #else
		else if (ComboKey==BTN_Power && keypadStatus==KEY_POWER)
	   #endif
            {
                if (PowerOnFlag)
                {
                    bEnableComboKeyFlag=1;
                }
            }
	#endif
        else
        {
            bEnableComboKeyFlag=0;
            ComboKey=0;
            PreComboKey=0;
            ComboCounter=0;
        }
  #endif

    }
//======================================================================================	
    else //check when key release
    {
        #if TimerComboKey
        bEnableComboKeyFlag=0;
        ComboKey=0;
        PreComboKey=0;
        ComboCounter=0;
        #endif

	#if (ENABLE_SUPERRESOLUTION)
		if( PressMinusFlag )
		{
			if( MenuPageIndex == RootMenu )
			{
				KeypadButton = BTN_Plus;
				HotKeyMinusCounter = 0;
				Clr_EnableClrVisionDemoFlag();
			}
			if( SyncLossState() )
				Clr_PressMinusFlag();
		}
	#endif

        KeypadButton = BTN_Nothing;
    }
   KeyDebug = 0;	
}

//================================================================
