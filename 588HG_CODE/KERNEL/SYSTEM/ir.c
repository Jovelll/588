/******************************************************************************
 Copyright (c) 2004 MStar Semiconductor, Inc.
 All rights reserved.

 [Module Name]: Ir.c
 [Date]:        04-Feb-2004
 [Comment]:
   Remote control subroutines.
 [Reversion History]:
*******************************************************************************/

#include "board.h"
#if ENABLE_IR_CONTROL
#define _IR_C_

// System
#include <intrins.h>
// Internal
#include "mode.h"
#include"UserPrefDef.h" 
#include "Global.h"

#include "MsReg.h"
#include "Ms_RWReg.h"

#include "menu.h"

#include "IRfunc.h"
#include "Ir.h"

#include "Ms_PM.h"



//BYTE TEST_XJ0=0;
//BYTE TEST_XJ1=0;
//BYTE TEST_XJ2=0;


BYTE LeadStart_flag=0;

BYTE data_flag=0;


BYTE XDATA LeadStart_flag_A=0;		  
BYTE XDATA LeadStart_flag_B=0;   
BYTE XDATA HL_Adjudge=0;


extern void putSIOChar(unsigned char sendData);
extern void printData(char *str, WORD value);

//extern xdata BYTE MenuItemIndex;

typedef enum
{
    IR_KEY_EXIT         =~BIT4,
    IR_KEY_MINUS        =~BIT3,
    IR_KEY_PLUS  		=~BIT2,
    IR_KEY_MENU 		=~BIT1,
    IR_KEY_POWER 		=~BIT0,    //BIT4
    IR_KEY_SELECT 		=~BIT5,
    IR_KEY_NOTHING        = 0xFF
} IRKeypadType;

BYTE code IRDecodeTable_Normal[]=
{
    IRKEY_POWER, IR_KEY_POWER,
    IRKEY_MENU, IR_KEY_MENU,
    IRKEY_EXIT, IR_KEY_EXIT,//IR_KEY_EXIT,
    IRKEY_OSD_RIGHT, IR_KEY_PLUS,// IR_KEY_PLUS,
    IRKEY_OSD_LEFT, IR_KEY_MINUS,// IR_KEY_MINUS,
    IRKEY_SELECT, IR_KEY_SELECT,
    0xFF
};

BYTE code IRDecodeTable_OPlay[]=
{
    IRKEY_POWER, IR_KEY_POWER,
    IRKEY_MENU, IR_KEY_NOTHING,
    IRKEY_EXIT, IR_KEY_NOTHING,//IR_KEY_EXIT,
    IRKEY_OSD_RIGHT, IR_KEY_NOTHING,// IR_KEY_PLUS,
    IRKEY_OSD_LEFT, IR_KEY_NOTHING,// IR_KEY_MINUS,
    IRKEY_SELECT, IR_KEY_SELECT,
    0xFF
};
#if(IR_TYPE==IR_SAMSUNG)
#define IR_DEBOUNCE()   (IRstateLowCount>30 || IRstateHighCount>30)
#define IR_LEADER_CODE()   ((IRstateLowCount>20) && (IRstateHighCount>20))
#define IR_REPEAT_CODE()  ((IRstateLowCount>20) && (IRstateHighCount>8))

#elif(IR_TYPE==IR_NEC_PRIMA_RC698)
//-------------------------------------------------------------------
#if 1
#define IR_DEBOUNCE()   ( IRstateHighCount>44)    //ding shi 214
#define IR_LEADER_CODE()   ((IRstateLowCount>39) && (IRstateHighCount>18))
#define IR_REPEAT_CODE()  ((IRstateLowCount>41) && (IRstateHighCount>10))
#endif
#if 0   //---------------------------220    //close print message  good status
#define IR_DEBOUNCE()   ( IRstateHighCount>42)    //ding shi 220
#define IR_LEADER_CODE()   ((IRstateLowCount>38) && (IRstateHighCount>17))
#define IR_REPEAT_CODE()  ((IRstateLowCount>41) && (IRstateHighCount>10))
#endif
#if 0   //---------------------------200
#define IR_DEBOUNCE()   ( IRstateHighCount>35)    
#define IR_LEADER_CODE()   ((IRstateLowCount>34) && (IRstateHighCount>16))//debonce 44  20--18  34/18  34/16 //debounce 35//L 34 H 16 no use
#define IR_REPEAT_CODE()  ((IRstateLowCount>41) && (IRstateHighCount>10))
#endif
#if 0   //---------------------------230
#define IR_DEBOUNCE()   ( IRstateHighCount>39)    
#define IR_LEADER_CODE()   ((IRstateLowCount>35) && (IRstateHighCount>16))
#define IR_REPEAT_CODE()  ((IRstateLowCount>41) && (IRstateHighCount>10))
#endif
#if 0   //---------------------------240
#define IR_DEBOUNCE()   ( IRstateHighCount>35)    
#define IR_LEADER_CODE()   ((IRstateLowCount>31) && (IRstateHighCount>14))
#define IR_REPEAT_CODE()  ((IRstateLowCount>41) && (IRstateHighCount>10))
#endif
#if 0   //---------------------------208
#define IR_DEBOUNCE()   ( IRstateHighCount>45)    
#define IR_LEADER_CODE()   ((IRstateLowCount>40) && (IRstateHighCount>19))
#define IR_REPEAT_CODE()  ((IRstateLowCount>41) && (IRstateHighCount>10))
#endif
#if 0   //---------------------------190
#define IR_DEBOUNCE()   ( IRstateHighCount>47)    
#define IR_LEADER_CODE()   ((IRstateLowCount>41) && (IRstateHighCount>19))
#define IR_REPEAT_CODE()  ((IRstateLowCount>41) && (IRstateHighCount>10))
#endif
#if 0   //---------------------------190
#define IR_DEBOUNCE()   ( IRstateHighCount>49)    
#define IR_LEADER_CODE()   ((IRstateLowCount>43) && (IRstateHighCount>20))
#define IR_REPEAT_CODE()  ((IRstateLowCount>41) && (IRstateHighCount>10))
#endif


#else
#define IR_DEBOUNCE()   (IRstateLowCount>16 || IRstateHighCount>16)
#define IR_LEADER_CODE()   ((IRstateLowCount>40) && (IRstateHighCount>20))
#define IR_REPEAT_CODE()  ((IRstateLowCount>40) && (IRstateHighCount>8))
#endif

#if 0    //-------------214
#define IR_DATA_CODE_1()  ((IRstateLowCount>=0) && (IRstateHighCount>5))
#define IR_DATA_CODE_0()  ((IRstateLowCount>=0) && (IRstateHighCount>0))
#define IR_DATA_COMPLETE()   (IR_Data_Count == 32)
#endif
#if 0    //-------------220
#define IR_DATA_CODE_1()  ((IRstateLowCount>=0) && (IRstateHighCount>5))
#define IR_DATA_CODE_0()  ((IRstateLowCount>=0) && (IRstateHighCount>0))
#define IR_DATA_COMPLETE()   (IR_Data_Count == 32)
#endif
#if 1    //-------------200
#define IR_DATA_CODE_1()  ((IRstateLowCount>=0) && (IRstateHighCount>5))  //1680us
#define IR_DATA_CODE_0()  ((IRstateLowCount>=0) && (IRstateHighCount>0))  //H 0-->1 5P    L  0-->1 no use
#define IR_DATA_COMPLETE()   (IR_Data_Count == 32)
#endif
#if 0    //-------------230   // 3 case
#define IR_DATA_CODE_1()  ((IRstateLowCount>=0) && (IRstateHighCount>4))
#define IR_DATA_CODE_0()  ((IRstateLowCount>=0) && (IRstateHighCount>0))
#define IR_DATA_COMPLETE()   (IR_Data_Count == 32)
#endif
#if 0    //-------------240   // 3 case
#define IR_DATA_CODE_1()  ((IRstateLowCount>=0) && (IRstateHighCount>3))
#define IR_DATA_CODE_0()  ((IRstateLowCount>=0) && (IRstateHighCount>0))
#define IR_DATA_COMPLETE()   (IR_Data_Count == 32)
#endif
#if 0    //-------------208   // 
#define IR_DATA_CODE_1()  ((IRstateLowCount>=0) && (IRstateHighCount>5))
#define IR_DATA_CODE_0()  ((IRstateLowCount>=0) && (IRstateHighCount>0))
#define IR_DATA_COMPLETE()   (IR_Data_Count == 32)
#endif
#if 0    //-------------200   // 6 CI
#define IR_DATA_CODE_1()  ((IRstateLowCount>=0) && (IRstateHighCount>5))
#define IR_DATA_CODE_0()  ((IRstateLowCount>=0) && (IRstateHighCount>0))
#define IR_DATA_COMPLETE()   (IR_Data_Count == 32)
#endif

#if  0   //-------------190   // no function
#define IR_DATA_CODE_1()  ((IRstateLowCount>=0) && (IRstateHighCount>5))
#define IR_DATA_CODE_0()  ((IRstateLowCount>=0) && (IRstateHighCount>0))
#define IR_DATA_COMPLETE()   (IR_Data_Count == 32)
#endif

void IR_INT1(void)
{

    if(IR_DEBOUNCE())
    {
      
            IRstateHighCount=0; 
            IRstateLowCount=0;
            LeadStart_flag=1;
       
    }
    if(LeadStart_flag==1)
    {
        if(IR_LEADER_CODE())
        {
        LeadStart_flag=0;
        IRstateHighCount=0; 	
        IRstateLowCount=0;
        IR_Data_Count=0;

        data_flag=1;		  
        }
    }

    else if(IR_REPEAT_CODE()&&IRCodeTranCompleteFlag)
    {
        IR_RepeatFlag ++;
        IRstateHighCount=0; 	
        IRstateLowCount=0;
    }	

    
    if(data_flag==1)
    {
        if(IR_DATA_CODE_1())   
        {
        IRstateHighCount=0; 	
        IRstateLowCount=0;
            IR_TempValue.valDW = (IR_TempValue.valDW <<1) | 0x01;
            IR_Data_Count++;
        }
        else if(IR_DATA_CODE_0()) 
        {
        IRstateHighCount=0; 	
        IRstateLowCount=0;
            IR_TempValue.valDW = (IR_TempValue.valDW <<1);
            IR_Data_Count++;            
        }
        

   


        if(IR_Data_Count==32)
        {
            IR_Value.valDW = IR_TempValue.valDW;
            IR_Value.valB[0]= IR_Value.valDW>>24;
            IR_Value.valB[1]= IR_Value.valDW>>16;
            IR_Value.valB[2]= IR_Value.valDW>>8; 
            IRCodeTranCompleteFlag=0;
            data_flag=0;



        }
    }

 
}

//#define PERIOD_200uS    (0x10000-(((unsigned long)CPU_CLOCK_KHZ*INT_PERIOD+30)/12/5)+100)

#if 0   // xj  use second way for IR  ---excuse function at timer  not  in EXTINT
void IR_timer_INT(void)
{
#if 1 

    if (pinIr)
    {
    HL_Adjudge=1;   //H Voltage
    IRstateHighCount++; 
    }
    else
    {
    HL_Adjudge=0;  // L Voltage
    IRstateLowCount++;
    }
        if((HL_Adjudge=1) &&(data_flag ==0))   //debouce   //higher volate   //(pinIr==0x0020)
        {           
        	if(IRstateHighCount >= 42)     // 200--->130--160---42
        	{
        	    IRstateHighCount = 0; 
        		LeadStart_flag_A = 1;
        	}
        }
        if(LeadStart_flag_A && (HL_Adjudge==0))        // low  9ms 
        {
        	if(IRstateLowCount >= 38)   //55--->38--40
        	{       
        	    IRstateLowCount  = 0;
        	    LeadStart_flag_A = 0;
        		LeadStart_flag_B = 1;
        	}	
        }    

if(LeadStart_flag_B && (HL_Adjudge == 1))   // higher  4.5ms   
	{
		if(IRstateHighCount >= 17)    // 25-->17--20
		{
			IRstateLowCount  = 0;
			IRstateHighCount = 0;
            LeadStart_flag_B = 0;
            data_flag = 1;
		}
	}

	if(data_flag)     // data translate   32 bit 
	{
		if(HL_Adjudge==0)   // run in interruption now
		{

			 if((IRstateHighCount >= 2) && (IRstateHighCount <= 4))  //logic 0    3-->2  6-->4
			{
				IRstateHighCount = 0;
				IRstateLowCount = 0;
                IR_TempValue.valDW = (IR_TempValue.valDW <<1);
				IR_Data_Count++; 
			}
			if(IRstateHighCount >= 5)    //  1   //7-->5--5
			{
				IRstateHighCount = 0;
				IRstateLowCount = 0;
                IR_TempValue.valDW = (IR_TempValue.valDW <<1) | 0x01;
            	IR_Data_Count++; 

			}
			if(IR_Data_Count==32)  //  32--->31
			{
            IR_Value.valDW = IR_TempValue.valDW;
            IR_Value.valB[0]= IR_Value.valDW>>24;
            IR_Value.valB[1]= IR_Value.valDW>>16;
            IR_Value.valB[2]= IR_Value.valDW>>8; 
				//flag = 1;           //data complete flag
			IR_Data_Count = 0;
			IR_TempValue.valDW = 0;
			data_flag = 0;
			}
        
		}
		
	}


#endif

}
#endif





void IR_InitVariables(void)
{
    IRstateHighCount = 0;
    IRstateLowCount = 0;	
    IR_Data_Count=0;
    LeadStart_flag=0;
    data_flag=0;
 //LeadStart_flag_A=0;		  
 //LeadStart_flag_B=0;   
 //HL_Adjudge=0;

#if 0//IR_ENABLE_DEBUG    
    IR_DebugKey = 0;
#endif    
    IRCodeTranCompleteFlag = 0;
}

void IR_Init(void)
{
    IR_InitVariables();
#if 0                                     //xj
    EXTINT3_EN_GPIO26(_ENABLE);
    INT_IRQ_SOURCE_1_ENABLE(TRUE);
    INT_IRQ_SEL_HL_TRIGGER(TRUE);
#endif  
   PMGPIO0_IRQ_Init() ;   // PMGPIO25  
   IR_Group_Enable()  ; 
  // IR_Group_Enable11()  ; 
   
    CIRQ_Mask_Enable() ; 
    //CIRQ_Mask_Enable11() ; 
    
    CIRQ_HL_TRIGGER()  ;  
    /* -------------initialize Timer 1 -----------------------------*/
    // remove to set mcu clock for normal/PM should re-calculate value
    //Timer1_TH1 = PERIOD_200uS >> 8;
    //Timer1_TL1 = PERIOD_200uS & 0xFF;
    //TH1=Timer1_TH1;  //200us reload
    //TL1=Timer1_TL1;  //200us reload	
    TMOD = 0x11;
    IT1 = 1; // edge trigger
    TR1 = 1;  // enable timer 1
    ET1 = 1;      //enable    TIMER1 

    PX0=0;  //
    PX1=0;  //

    PT0=0;  //
    PT1=1;  //

    IP0 = IP0 | _BIT2 |_BIT3;
    IP1 = IP1 | _BIT2 |_BIT3;

}

BYTE IR_Decode(BYTE inputkey )
{
    BYTE i, temp;

    i = 0;
    temp = IR_KEY_NOTHING;

    if (CURRENT_INPUT_IS_OPLAY())
    {
        while(IRDecodeTable_OPlay[i] != 0xFF)
        {
        	if(IRDecodeTable_OPlay[i] == inputkey)
        	{
                temp = IRDecodeTable_OPlay[i+1]; 
                break;
        	}
        	i+=2;
        }
    }
    else
    {
        while(IRDecodeTable_Normal[i] != 0xFF)
    	{
        	if(IRDecodeTable_Normal[i] == inputkey)
        	{
                temp = IRDecodeTable_Normal[i+1]; 
            break;
    	}
    	i+=2;
    }
    }

    return temp;
}

BYTE IR_GetIRKeypadStatus( void )
{
    BYTE i, temp = IR_KEY_NOTHING;
    BYTE IR_CustomCodeHigh=0, IR_CustomCodeLow=0;
    BYTE IR_TransData, IR_GotKey=0;
    BYTE xdata u8RepeatKeyDebounceCount;
     //   printMsg("IR---TEST----START----XUJUN");

    if (g_bMcuPMClock)
    {
        u8RepeatKeyDebounceCount = GETREPEAT_KEY_DEBOUNCE_COUNT_PM;
    }
    else if (IR_RepeatKey == IR_KEY_MINUS||IR_RepeatKey == IR_KEY_PLUS)
    {
        u8RepeatKeyDebounceCount = GETREPEAT_KEY_DEBOUNCE_COUNT_ADJITEM;
    }
    else
    {
        u8RepeatKeyDebounceCount = GETREPEAT_KEY_DEBOUNCE_COUNT_NORMAL;
    }

    IR_TransData = IR_Value.valB[0];
    for (i=0;i<8;i++)
    {
        IR_CustomCodeHigh <<= 1;
        if (IR_TransData&BIT0)
        {
            IR_CustomCodeHigh|=BIT0;
        }
        IR_TransData>>=1;
    }

     //  TEST_XJ0=IR_Value.valB[0];
    IR_TransData = IR_Value.valB[1];
    for (i=0;i<8;i++)
    {
        IR_CustomCodeLow <<= 1;
        if (IR_TransData&BIT0)
        {
            IR_CustomCodeLow|=BIT0;
        }
        IR_TransData>>=1;
    }
 //        TEST_XJ1=IR_Value.valB[1];

    if((IR_CustomCodeHigh<<8|IR_CustomCodeLow) == IR_CUSTOM_CODE)
    {
        IR_TransData = IR_Value.valB[2];
        for (i=0;i<8;i++)
        {
            IR_GotKey <<= 1;
            if (IR_TransData&BIT0)
            {
                IR_GotKey|=BIT0;
            }
            IR_TransData>>=1;
        }
        IRCodeTranCompleteFlag = 1;
    }

#if 1    // xj  for DC Down  debug IR


//if(flag_power_xj==1)  //used for power down
//flag_power_xj=0;

{
//MonitorSetting.RedColor=IR_Value.valB[0];
//TEST_XJ0=IR_Value.valB[0];
//TEST_XJ1=IR_Value.valB[1];
//TEST_XJ2=IR_Value.valB[2];
}
#endif
   // msWriteWord(REG_0204,IR_Value.valB[0]);//3274   //REG_0205  pm   251E
   // msWriteWord(REG_0205,IR_Value.valB[1]);
  //  msWriteWord(REG_3284,IR_Value.valB[0]);//3274   //REG_0205  pm   251E
  //  msWriteWord(REG_3285,IR_Value.valB[1]);
    
   // msWriteWord(REG_3276,IR_Value.valB[2]);
   // msWriteWord(REG_3277,IR_Value.valB[3]);

    
    if(IR_GotKey)
    {
    #if 1//IR_ENABLE_DEBUG
        printData("IR_CustomCodeHigh---=%x", IR_CustomCodeHigh);
        printData("IR_CustomCodeLow---=%x", IR_CustomCodeLow);
        printData("IR_GotKey---=%x", IR_GotKey);
    #endif
        temp = IR_Decode(IR_GotKey);
    printData("IR_Decode===---========%d",temp);
   
        if(temp==0xFE)//power key                       //problem remaid  xj 2016/3/28
            IR_RepeatKey = 0xFF;
        else// other key
            IR_RepeatKey = temp;

        
        IR_GotKey = 0;
        IR_Value.valDW=0;
        //IR_RepeatFlag = REPEAT_KEY_DEBOUNCE_COUNT + 1;          //20130523 Rick, avoid missing recieved repeat code , send repeat status directly
    }
    else if(IR_RepeatFlag > REPEAT_KEY_DEBOUNCE_COUNT)
    {
        temp = IR_RepeatKey;
        IR_RepeatFlag = REPEAT_KEY_DEBOUNCE_COUNT;
        IR_GetRepeatKey_DebounceCounter=0;
    }
    else if (IR_RepeatFlag&&(IR_GetRepeatKey_DebounceCounter<u8RepeatKeyDebounceCount))
    {
        temp = IR_RepeatKey;
        IR_GetRepeatKey_DebounceCounter++;
    }
    else
    {
        IR_RepeatFlag = 0;
    }
#if 0//IR_ENABLE_DEBUG
    if(1)//IR_DebugKey)
    {
        printData("IR_CUSTOM CODE:%x", IR_CUSTOM);
        printData("IR_DATA CODE:%x", IR_DATA);
        IR_DebugKey = 0;
    }
    if(temp != IR_KEY_NOTHING)
    {
        printData("IR:%x", temp);
    }
#endif
    return temp;
}
#else //for compiler

BYTE code dump[]={0};

#endif


