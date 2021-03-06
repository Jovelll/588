
#include "board.h"

#if  ENABLE_LED_CONTROLLER

#define _LEDCONTROL_C_
#include <math.h>
#include "types.h"
#include "ms_reg.h"
#include "mode.h"
#include "UserPrefDef.h"
#include "global.h"
#include "panel.h"
#include "ms_rwreg.h"
//#include "debugMsg.h"
#include "detect.h"
#include "mStar.h"
#include "misc.h"
#include "LedControl.h"


WORD code Stream_S[LED_STRING_NUM][2]=
{
    {LED_CH_S0, STRING_NUM_4  },
    {LED_CH_S1, STRING_NUM_5  },
    {LED_CH_S2, STRING_NUM_6  },
    {LED_CH_S3, STRING_NUM_7  },
};
WORD code Stream_D[LED_STRING_NUM][2]=
{
    {LED_CH_D0, STRING_NUM_0  },
    {LED_CH_D1, STRING_NUM_1  },
    {LED_CH_D2, STRING_NUM_2  },
    {LED_CH_D3, STRING_NUM_3  },
};


LED_INFO xdata gLEDInfo;
LED_Status xdata LEDSatus;
LED_Alert_Status xdata gLEDAlert;
static BYTE xdata g_u8PWMValue = 0;
//BYTE xdata u8States_5C=0;
//BYTE xdata u8States_5D=0;
//BYTE idata u8Inter_Count=0;
BYTE xdata u8SymState=eLEDSTATE_INITIAL;
BYTE xdata u8LEDState  =eLED_IDLE;
WORD xdata u16TotalPower=0;

#if LED_AUTO_RECOVER
BYTE xdata StringShortOpenStatus = 0;
BYTE xdata StringErrorCounter = 0;
BYTE xdata StringRecoverCounter = 0;
bit StringShortOpenChecking = 0;
bit StringShortOpenFlag = 0;
bit StringRecoverCounterFlag = 0;
bit StringRebootFlag = 0;
#define LED_AUTO_RECOVER_DEBUG      1
#endif

#if ENABLE_LED_OPEN_SHORT_DEBOUNCE
BYTE xdata LEDOpenShortCounter = 0;
BYTE xdata LEDOpenShortString = 0; // record light bar open and short status in ISR, bit7~4: LED 3~0 open, bit3~0: LED 3~0 short
bit bLEDOpenShortFlag = 0; // indicate string open or short happened in ISR
BYTE xdata LEDShutDownCounter = 0; // open/short string will be shut down after LEDShutDownCounter up count to LED_SHUT_DOWN_COUNT
bit bLEDOpenShortIntEnFlag = 0;
#endif

#if ENABLE_LED_INT_DELAY
BYTE xdata LEDINTEnableCounter = 0;
BYTE xdata LEDINTEnableFlag = 0;
#endif

#if ENABLE_DETECT_LED_CH_0 || ENABLE_DETECT_LED_CH_1 || ENABLE_DETECT_LED_CH_2 || ENABLE_DETECT_LED_CH_3
BYTE xdata StringDetectAndEnable = 0; // bit7~4 indicate which string detection be enabled; bit3~0 indicate which string active
#endif

#define  LED_DEBUG  0
#if (ENABLE_DEBUG&&LED_DEBUG)
#define LED_printData(str, value)   printData(str, value)
#define LED_printMsg(str)               printMsg(str)
#define LED1_printData(str, value)
#define LED1_printMsg(str)
#elif (ENABLE_DEBUG&&ENABLE_LED_OPEN_SHORT_DEBOUNCE)
#define LED_printData(str, value)
#define LED_printMsg(str)
#define LED1_printData(str, value)   printData(str, value)
#define LED1_printMsg(str)               printMsg(str)
#else
#define LED_printData(str, value)
#define LED_printMsg(str)
#define LED1_printData(str, value)
#define LED1_printMsg(str)
#endif
#define  LED_DEBUG_PROTECTION 1&LED_DEBUG
#define  LED_DEBUG_CurrentAdjust 1&LED_DEBUG

extern void mStar_WaitForDataBlanking( void );
//==============================================================================
void Init_LED_Setting()
{
    WORD xdata temp=0;
#if LED_CTRL_BRIGHTNESS_BY_CURRENT
    #if CHIP_ID==CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID==CHIP_TSUMD||CHIP_ID==CHIP_TSUMJ
        msWriteByteMask(REG_0A84,BIT7|0x01,0x8F);    // [7]:new mode to impare tuning speed [3:0] : 256T before Neg PWM  Prepare timt
    #else
        msWriteByteMask(REG_0A84,0x00,0x0F);    // 256T before Neg PWM  Prepare timt
       #endif
#else
    msWriteByteMask(REG_0A84,0x01,0x0F);    // 256T before Neg PWM  Prepare timt
#endif
    msWrite2Byte(REG_0A88, VOL_TO_SAR(NORMAL_UP));   // 1V = 0x12
    msWrite2Byte(REG_0A8A, VOL_TO_SAR(NORMAL_LOW));  // 0.55V = 0x0A
    msWrite2Byte(REG_0A8C, VOL_TO_SAR(SKEW_LOW));        // 1.55V = 0x1C
    msWrite2Byte(REG_0A8E,  VOL_TO_SAR(SKWE_UP));        // 3.5V = 0x41

    msWrite2Byte(REG_1B56,  0x00FF);        //disable GPIO_L0 ~ GPIO_L7 enable to let SAR detection is ok //20110517
}

//==============================================================================
void  msLED_ClearFlag(void)
{
    //CLR_SAR_Flag();
    msWrite2Byte(REG_0AC4, 0xFFFF);                          // Clear LED states
    msWrite2Byte(REG_0A4A, 0xFFFF);                        //Clear SAR INT 1 Flag
    //msWrite2Byte(REG_0A4A, 0xFFFF);                        //Clear SAR INT 1 Flag
    msWrite2Byte(REG_0A4C, 0xFFFF);                      //Clear SAR INT 2 Flag
    //gLEDInfo.bTime_Out =100;       //20110302
    gLEDInfo.bMEASURE_Delay =0;      //20110302                 //TIME_SAR_SAMPLE_DELAY;
}
//==============================================================================
void  Init_SAR()
{

    msWriteByteMask(REG_0A60,0 ,BIT5|BIT7|BIT6);                              //Power On sar //One shot mode//
#if LED_CTRL_BRIGHTNESS_BY_CURRENT
    msWriteByte(REG_0A61,0x00 );                                                    //Sample delay time
#else
    msWriteByte(REG_0A61,0x05 );                                                    //Sample delay time
#endif
    msWriteByteMask(REG_0A63, BIT2|BIT1 , BIT4|BIT3|BIT2|BIT1|BIT0 );      //Sar Input Clock

    msWrite2Byte(REG_0A40,  VOL_TO_SAR_DET(LED_2Short));         
    msWrite2Byte(REG_0A42,  VOL_TO_SAR_DET(LED_1Short));		
    msWrite2Byte(REG_0A44,  VOL_TO_SAR_STR_2(LED_Open));		 
	//printData("LED_2Short==%d",VOL_TO_SAR_DET(LED_2Short));
	//printData("LED_2Short==%d",VOL_TO_SAR_STR(LED_Open));
    //msWrite2Byte(REG_0A40,  SAR_V_8p00);         // > 8.0 V //2 LED short protection
    //msWrite2Byte(REG_0A42,  SAR_V_4p50);         // > 4.5 V  1 led short protection
    //msWrite2Byte(REG_0A44,  SAR_V_0p10);         // <.0.1 V
}
//==============================================================================
void msLED_BoostEnable( BYTE ucStatus)
{
    if ( ucStatus == _ENABLE)
    {
        gLEDInfo.bBoostOnOff = 1;
        msWriteByteMask(REG_0A95,BIT7,BIT7);            //Enable Boost pwm
        msWriteByteMask(REG_0A87,BIT7,BIT7);            //Enable Boost Clk
        SetBoostCounter(BOOST_COUNT);        
        //u16BoostCounter = BOOST_COUNT;
        //bBoostCounterFlag = 1;
    }
    else
    {
        gLEDInfo.bBoostOnOff = 0;
        msWriteByteMask(REG_0A95,0 ,BIT7);              //Disable Boost pwm
        msWriteByteMask(REG_0A87,0,BIT7);               //Disable Boost Clk
        SetBoostCounter(0);        
        //u16BoostCounter = 0;
        //bBoostCounterFlag = 0;
    }
}
//==============================================================================
void msLED_DimmingEnable( BYTE ucStatus, WORD channels)
{
    if ( ucStatus == _ENABLE)
    {
        while(bBoostCounterFlag);// 111116 coding test
        //mStar_WaitForDataBlanking();  //sometime  have flash  black line at  turn on panel
        //LED_printData(" ===========>>Dimming ON => Channel = %x " , channels);

	#if LED_SAR_VOLTAGE_MEASURE_ENABLE
		msWrite2ByteMask(REG_0AD0,(STREAM_VALUE_D<<4|STREAM_VALUE_D),channels&0x00FF);
	#endif
	#if LED_limitOCPtoreduceCurrentPeak //use OCP to limit current peak 20120726
	        {
	            BYTE xdata u8OCPVoltage;
	            msWriteByteMask(REG_0AE3,0,BIT4|BIT5);                  // OCP 10b =REG_0AE3/IFB_R Current
	            msWrite2ByteMask(REG_0A80,STREAM_VALUE_D,channels&0x000F);
	            //LED_printData(" @@@@ REG_0A80 = %x " , msRead2Byte(REG_0A80));
	            for( u8OCPVoltage = 0; u8OCPVoltage <= BOOST_CURRENT_LIMIT_VALUE; u8OCPVoltage ++ )
	            {
		            ForceDelay1ms(5);
		            msWriteByteMask(REG_0AE3,(u8OCPVoltage<<4),BIT4|BIT5);                   // OCP 10b =REG_0AE3/IFB_R Current
	            }
	            msWriteByteMask(REG_0AC4,BIT6,BIT6);
	        }
	#else
	        msWrite2ByteMask(REG_0A80,STREAM_VALUE_D,channels&0x000F);
	#endif
	#if LED_DETECT_STRN
		msWrite2ByteMask(REG_0A68, STREAM_VALUE_S,channels&0x00F0);
	#else
		msWrite2ByteMask(REG_0A68, STREAM_VALUE_D,channels&0x000F);
	#endif
	#if CHIP_ID==CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID==CHIP_TSUMD||CHIP_ID==CHIP_TSUMJ
		msWriteByteMask(REG_0AF1,BIT6|BIT4,BIT6|BIT4);
		msWriteByteMask(REG_0AE1,BIT6|BIT7,BIT6|BIT7);
	#endif
	//msWriteByteMask(REG_0A64,BIT0 ,BIT0); 
#if Enable_MultiSensing||LED_CTRL_BRIGHTNESS_BY_CURRENT
	BoostReadyCounter=EnableMultiSensingAfterBoostReady;
	//msWriteByteMask(REG_022C,BIT2,BIT2);
	//ForceDelay1ms(1);
	//msWriteByteMask(REG_022C,0,BIT2);
#endif
    }
    else
    {
        //LED_printData(" Dimming OFF => Channel = %x " , channels);
        msWrite2ByteMask(REG_0A80,0x0000,channels);
        msWrite2ByteMask(REG_0A68,0x0000,channels);
     #if LED_SAR_VOLTAGE_MEASURE_ENABLE
        msWrite2ByteMask(REG_0AD0,0x0000,channels);
     #endif
     #if CHIP_ID==CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID==CHIP_TSUMD||CHIP_ID==CHIP_TSUMJ
	 msWriteByteMask(REG_0AF1,0x00,BIT6|BIT4);
        msWriteByteMask(REG_0AE1,0x00,BIT6|BIT7);
     #endif
    }
}

#if ENABLE_TSUML_NEWOVP
void  msLED_OVP2_LevelSelection(Bool bflag)
{
    BYTE xdata u8count=0;
    BYTE code OVP2_THRESHOLD[16][2]=
    {
        { 55, 35}, //VH and VL
        { 60, 40},
        { 65, 45},
        { 70, 50},
        { 75, 55},
        { 80, 60},
        { 85, 65},
        { 90, 70},
        { 95, 75},
        { 105, 80},
        { 115, 90},
        { 125, 100},
        { 135, 105},
        { 150, 120},
        { 165, 135},
        { 180, 145}
    };
    if (bflag)
    {
        for (u8count=0; u8count<sizeof(OVP2_THRESHOLD)/2; u8count++)
        {
            //WORD wOVP2Voltage_TH = (OVP2_THRESHOLD[u8count][0] * ResistorRatio /100);
            //WORD wOVP2Voltage_TL = (OVP2_THRESHOLD[u8count][1] * ResistorRatio /100);
            WORD wOVP2Voltage_TH = ((WORD)(OVP2_THRESHOLD[u8count][0] * ResistorRatio) /100);
            WORD wOVP2Voltage_TL = ((WORD)(OVP2_THRESHOLD[u8count][1] * ResistorRatio) /100);

            if ((wOVP2Voltage_TH>OVP2_ProtVoltage)&&(wOVP2Voltage_TL<OVP2_ProtVoltage))
            {
                #if LED_DEBUG
                    LED_printData(" @@@@ u8count ===%d", u8count&0x0F);
                    LED_printData(" OVP2_TH ===%d", wOVP2Voltage_TH);
                    LED_printData(" OVP2_TL ===%d", wOVP2Voltage_TL);
                    LED_printData(" OVP2_ProtVoltage ===%d", OVP2_ProtVoltage);
                #endif

                msWriteByteMask(REG_0AE0, BIT7,BIT7);       //Enable OVP protection
                msWriteByteMask(REG_0AE2, u8count,0x0F);    //OVP2 level selection
				
        #if CHIP_ID==CHIP_TSUMD ||CHIP_ID==CHIP_TSUMJ
                msWriteByteMask(REG_0AFA, u8count,0x0F);        //OVP2 THR1
                msWriteByteMask(REG_0AFA, u8count<<4,0xF0); //OVP2 THR2
        #endif
                break;
            }
        }
    }
    else
    {
        msWriteByteMask(REG_0AE0, 0x00,BIT7);       //Disable OVP protection
        msWriteByteMask(REG_0AE2, 0x0F,0x0F);       //OVP2 level selection
#if CHIP_ID==CHIP_TSUMD ||CHIP_ID==CHIP_TSUMJ
        msWriteByteMask(REG_0AFA, 0x0F,0x0F);       //OVP2 THR1
        msWriteByteMask(REG_0AFA, 0x0F<<4,0xF0);    //OVP2 THR2
#endif
    }
}
#endif
#if !LED_limitOCPtoreduceCurrentPeak //use OCP to limit current peak 20120726
void msSetOCPLevel( void )
{
    DWORD xdata w16Ipeak=IPeak;
    BYTE xdata u8OCPLevel=0;

    if(w16Ipeak<eLED_OCPLV_2)
    {
        u8OCPLevel=2;
    }
    else
    {
        u8OCPLevel=3;
    }

    LED_printData("\n\  OCP  TotalCurrent = %d ", TotalCurrent);
    LED_printData("\n\r OCP GateDuty = %d ", GateDuty);
    LED_printData("\n\r OCP w16IpeakA = %d ", w16Ipeak);
    LED_printData("\n\r  OCP u8OCPLevel  = %d ", u8OCPLevel);

    // "00= 3A 01= 4V 10=5A 11= 6.5"
    msWriteByteMask(REG_0AE3, (u8OCPLevel<<4), BIT4|BIT5);
}
#endif
void msSetSlopeCompensation( void )
{
	//=====================================================
	//SC > 0.5 * (55V + 0.7V - 12V)/33uH(×(100-B)/100))*1*0.1 (×(100+25)/100)= 0.066V/us
	//From the equation above, we can see higher vout requires larger SC. 
	//It looks SC setting=1 (0.1V/us) is enough, but in real case, inductor might have +/- 30% variation and SC would have +/- 25% variation. Therefore, a better setting for this panel would be 
	//SC > 0.066V/us /(1-30%)/(1-25%) = 0.126V/us
	//=====================================================
	BYTE u8I=0;
	BYTE xdata u8Ratio=0;
	DWORD xdata w32SC_IdealValue=0;
	
	#define BST_Slcomp_maxSEL 7
	u8Ratio=(WORD)(1000000ul/((100-L_Variation)*(100-Rsense_Variation)));  //NN %
	w32SC_IdealValue=((5ul*(PanelLightBarDefaultVoltage*100+70-Vin_Boost*100)*CSA_gain*Rsense/L_value)*u8Ratio)/100/1000;
	LED_printData("\n\r   w32SC_IdealValue= %d ", u8Ratio);		
	LED_printData("\n\r   w32SC_IdealValue= %x ", w32SC_IdealValue);
	LED_printData("\n\r   w32SC_IdealValue= %x ", w32SC_IdealValue>>16);

	while(w32SC_IdealValue>0)
	{	
		u8I++;
		LED_printData("\n\r  w32SC_IdealValue= %d ", w32SC_IdealValue);
		LED_printData("\n\r  Slope compensation setting to  [ 0.%x V/uS ]", u8I);
		
		if (w32SC_IdealValue>=100) 
			w32SC_IdealValue-=100;
		else 
			w32SC_IdealValue=0;
		
		if (w32SC_IdealValue == 0x00)
		{
			break;
		}
	}
	msWriteByteMask(REG_0AE1, ((u8I >= BST_Slcomp_maxSEL)?(BST_Slcomp_maxSEL):(u8I)), BIT2|BIT1|BIT0);
}
//==============================================================================
void  Init_BoostPWM()
{
    msWrite2Byte(REG_0AC0,   0xFFFF);                                   //Mask All Interrupt // LED INT Enable
    //internal LPF filter setting(2M+16P)	
    msWriteByteMask(REG_0AEE,   BIT5,   BIT5|BIT4);                                   //Mask All Interrupt // LED INT Enable

#if ENABLE_TSUML_NEWOVP
    msWrite2Byte(REG_0A90, PnlVOL_TO_SAR(PanelLightBarDefaultVoltage));         //initial boost duty = panel default voltage
#if LED_CTRL_BRIGHTNESS_BY_CURRENT
    msWrite2Byte(REG_0A92, PnlVOL_TO_SAR(PanelLightBarDefaultVoltage*6/10));    //minimum boost duty = panel default voltage x 90%
#else
    msWrite2Byte(REG_0A92, PnlVOL_TO_SAR(PanelLightBarDefaultVoltage*9/10));    //minimum boost duty = panel default voltage x 90%
#endif
    msWrite2Byte(REG_0AD2, PnlVOL_TO_SAR(OVP1_ProtVoltage)<<4);                 //maximum boost duty = panel default voltage x 110% = OVP1_ProtVoltage
    msWriteByteMask(REG_0AD3, BIT7 ,BIT7);      // Enable BST maximum setting
    #if CHIP_ID==CHIP_TSUMB||CHIP_ID==CHIP_TSUMY
    msWriteByteMask(REG_0AE0, BIT5 ,BIT5);       // Boost Feedback Select
    #endif
    msLED_OVP2_LevelSelection(TRUE);

    msSetSlopeCompensation();
	
    #if LED_DEBUG
    LED_printData(" @@@ initial BST duty ==%x ",PnlVOL_TO_SAR(PanelLightBarDefaultVoltage));
    LED_printData(" @@@ minimum BST duty ==%x ",PnlVOL_TO_SAR(PanelLightBarDefaultVoltage*9/10));
    LED_printData(" @@@ maximum BST duty ==%x ",PnlVOL_TO_SAR(OVP1_ProtVoltage));
    #endif
#else
    msWrite2Byte(REG_0A90, DUTY_TO_SAR(INIT_BOOST_DUTY));          //Initial PWM       // LED Initial Duty
    msWrite2Byte(REG_0A92, DUTY_TO_SAR(MIN_BOOST_DUTY));           //Min PWM       // LED Min Duty
    #if CHIP_ID==CHIP_TSUMB||CHIP_ID==CHIP_TSUMY
    msWriteByteMask(REG_0AE0,   BIT5|BIT7,BIT5|BIT7);                       // Boost Feedback Select
    #elif CHIP_ID==CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID==CHIP_TSUMD||CHIP_ID==CHIP_TSUMJ
    msWriteByteMask(REG_0AE0,   BIT7,       BIT7);
    #endif
#endif


#if CHIP_ID==CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID ==CHIP_TSUMD||CHIP_ID==CHIP_TSUMJ   
	msWriteByteMask(REG_0AEF, (CSA_gain==1)?(0x00):(BIT5), BIT5|BIT4);   	//100m→1x,  33m→3x   //[5:4]CSA gain selection 2'10->33mΩ 2'b01->100mΩ
#elif CHIP_ID==CHIP_TSUMB||CHIP_ID==CHIP_TSUMY
	msWriteByteMask(REG_0AE1,BIT6|BIT7|BIT5|BIT4,BIT6|BIT7|BIT5|BIT4);        // [6]Bias Enable ,[4]CSA gain selection, [7]Rext_Enable.Digital 1.5
#endif
    //msWriteByteMask(REG_0AE2,BIT5,BIT4|BIT5);                       // Boost Current limit level 10b
#if !LED_limitOCPtoreduceCurrentPeak //use OCP to limit current peak 20120726
    //msWriteByteMask(REG_0AE3, (BOOST_CURRENT_LIMIT_LEVEL<<4), BIT4|BIT5);    //111216 set boost current limit level setting for each panel  //20111209_FOR_CAE                  //msWriteByteMask(REG_0AE3,BIT5|BIT4,BIT4|BIT5);                  // OCP 10b =REG_0AE3/IFB_R Current
    msSetOCPLevel();    //follow formula to set OCP level 20130307
#endif

    //msWriteByteMask(REG_0AE1,   BIT0,   BIT2|BIT1|BIT0);                // Slope compensation

#if CHIP_ID==CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID==CHIP_TSUMD||CHIP_ID==CHIP_TSUMJ
    //msWriteByteMask(REG_0AE3, BIT2, BIT2);    //Enable Soft Start
    msWriteByteMask(REG_0AEF, BIT0, BIT1|BIT0);    //Enable Soft Start to long period   //00:26,01:14,10:7,11:3mS
#elif CHIP_ID==CHIP_TSUMB||CHIP_ID==CHIP_TSUMY
    msWriteByteMask(REG_0AE3, BIT3|BIT2, BIT3|BIT2);    //Enable Soft Start and set to Slow
#endif
    //msWriteByteMask(REG_0AE3,BIT5|BIT4,BIT4|BIT5);                // OCP 10b =REG_0AE3/IFB_R Current

    msWriteByteMask(REG_0A96,0x0B,0x1F);                        // Boost  Settle time  N // delay time for boost stable
    msWriteByteMask(REG_0A97,0x06,0x07);                        // Boost  Settle time  M// delay time for boost stable

    if (gLEDInfo.bBoostClkODClk ==Boost_ODClk) // Panel Dclk
    {
        msWriteByteMask(REG_0A87,BIT6,BIT6);      // Set OD clock
        msWriteByte(REG_0A86,CLKDIV_OD_N);
    }
    else
    {
        msWriteByteMask(REG_0A87,0,BIT6);       // Set Xtal clock
        msWriteByteMask(REG_0A87,CLKDIV_216M_N,BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);       // Divider
        LED_printData("CLKDIV_216M_N===%x",CLKDIV_216M_N);
    }
	
    #if LED_SW_MODE
    if (gLEDInfo.bControlModeHW == SW_Mode)
    {
        msWriteByteMask(REG_0A95,BIT6,BIT6);                //Software Mode
    }
    else
    #endif			
    {
        msWriteByteMask(REG_0A95,0,BIT6);                //Hardware Mode
        //msWriteByte(REG_0A94, gLEDInfo.ucBoostPwmStep);    //Hardware Mode Boost Step
        msLED_Set_BoostPwmStep(gLEDInfo.ucBoostPwmStep);
    }

    msLED_ReloadBoostPWM();
    msLED_BoostEnable(_ENABLE);
}
//==============================================================================
#if CHIP_ID == CHIP_TSUMJ
#define DIVISIOR_MIN_SETTING    2
#else
#define DIVISIOR_MIN_SETTING    18
#endif
#define DIVISIOR_M_MIN_SETTING      0
#define DIVISIOR_M_MAX_SETTING      7
#define DIVISIOR_N_MAX_SETTING      255

#define PWM_FREQ_M_1645HZ     3
#define PWM_FREQ_N_1645HZ     2
void SetDimmingPWMFreq(Bool enValign, WORD freq)       //111216 modified LED blacklit dimming PWMfreq setting
{
    DWORD xdata SAR_T;
    BYTE divisior_m;
    WORD xdata DimmingFreq, divisior;

    for(divisior_m=DIVISIOR_M_MIN_SETTING; divisior_m<=DIVISIOR_M_MAX_SETTING; divisior_m++)
    {
        DimmingFreq=(((28000ul+(freq)/2)/(freq))-1 )/(1<<divisior_m);
        if(DimmingFreq > DIVISIOR_N_MAX_SETTING)
            continue;
        
        divisior = DimmingFreq*(1<<divisior_m)+1;
        if(divisior >= DIVISIOR_MIN_SETTING)
            break;
    }
        
    if(divisior_m > DIVISIOR_M_MAX_SETTING)
    {
        divisior_m = FQC_DIV_M;
        DimmingFreq=((28000ul/(freq))-1 )/(1<<divisior_m);
    }
        
    msWriteByteMask(REG_0A83, divisior_m<<4, BIT6|BIT5|BIT4);

    SAR_T = (2250+1000000/PWM_BOOST_FREQ)/70/16;    // (sar_t+1) >(2.2U+1/boost_CLK)/16*XTAL
	
    if (SAR_T <= (1748/freq-1))    //SAR+1<Divisor/(28K*512*16)*XTAL=1748/Dimming_Freq
    {
        msWriteByteMask(REG_0A84, (SAR_T<1)?(1):(SAR_T), 0x0F);
        msWriteByteMask(REG_0A64,BIT0 ,BIT0); 
    }
    else
    {
        msWriteByteMask(REG_0A84, 0x01 ,0x0F);
        msWriteByteMask(REG_0A64, 0x00 ,BIT0); 
    }

    msWriteBit(REG_0A83, enValign, BIT2);
    msWriteByte(REG_0A82,DimmingFreq);       //should be larger than preparing time of SAR(REG_0A82 >REG_0A84[3:0])        //  n
    msWriteByteMask(REG_0A85,BIT0,BIT0);                            // Upload PWM with Double buffer
}

#if LED_CTRL_BRIGHTNESS_BY_CURRENT
void BrightnessPWMSettingToCurrentSetting(BYTE u8PWM)
{
    BYTE i;

    // brightness PWM 0~255 mapping to real current
    u8PWM=(WORD)u8PWM*(LEDBL_PanelDefaultCurrent-LEDBL_PanelMinCurrent)/255+LEDBL_PanelMinCurrent;

    // real current mapping to register setting
    u8PWM = AnalogCurrent(u8PWM);

    for(i=0; i<LED_STRING_NUM; i++)
    {
        msWrite2Byte(REG_0AE4+i*2, u8PWM);
    }
}

void BrightnessOSDSettingToCurrentSetting(BYTE value)
{
    BYTE current, i;

    // brightness 0~100 mapping to real current
    if(value >= 100)
        current = LEDBL_PanelDefaultCurrent;
    else if(value == 0)
        current = LEDBL_PanelMinCurrent;
    else
        current = (WORD)value*(LEDBL_PanelDefaultCurrent-LEDBL_PanelMinCurrent)/100+LEDBL_PanelMinCurrent;

    // real current mapping to register setting
    current = AnalogCurrent(current);

    for(i=0; i<LED_STRING_NUM; i++)
    {
        msWrite2Byte(REG_0AE4+i*2, current);
    }
}
#endif

void  Init_DimmingPWM()
{
    WORD xdata i=0;
    WORD xdata channel=0x01;
    BYTE xdata j=0;
    WORD xdata temp[5]={0,0,0,0,0};

    LED_printMsg("======Initial DimmingPWM ========");

#if 0 //for 64 pin N2 package , need get FT sample to fetch Rext setting from efuse
    msWriteByteMask(REG_0AF0,	BIT5	,0x3F);   //set RINT_TRIM12[5:0]=100000 (trim bits)
    msWriteByteMask(REG_0AF1,	BIT5	,BIT5);   //internal resistor enable
#endif

    msLED_DimmingEnable(_DISABLE,0xFFFF);                                   //Disable Dimming PWM

    //LED dimming PWM frequency setting
    msWriteByteMask(REG_0A83,FQC_DIV_M<<4, BIT4|BIT5|BIT6);     // m = 3
    #if !BrightFreqByVfreq||LED_CTRL_BRIGHTNESS_BY_CURRENT
    msWriteByte(REG_0A82,FQC_DIV_N);       //should be larger than preparing time of SAR(REG_0A82 >REG_0A84[3:0])        //  n
    #endif
    msWriteByteMask(REG_0A85,BIT0,BIT0);                            // Upload PWM with Double buffer
	
    //msWriteByteMask(REG_0A83,FQC_DIV_M<<4, BIT4|BIT5|BIT6);             // m = 3
   // msWriteByte(REG_0A82,FQC_DIV_N);       //should be larger than preparing time of SAR(REG_0A82 >REG_0A84[3:0])           //  n

    msWriteByteMask(REG_0AE1,((gLEDInfo.bDimCtlModeMultiPhase == MULTI_Phase)?(0):(BIT3)),BIT3); // multi phase enable

#if LED_CTRL_BRIGHTNESS_BY_CURRENT
    //SetDimmingCurrent_INIT(50);

    //SetDimmingCurrent_INIT(UserPrefBrightness);
    // set Dimming duty to max
    msWrite2Byte(REG_0AB0, 0x1FF);
    msWrite2Byte(REG_0AB2, 0x1FF);
    msWrite2Byte(REG_0AB4, 0x1FF);
    msWrite2Byte(REG_0AB6, 0x1FF);
    //Reload Dimming Duty
    msLED_ReloadDimmingPWM();
#else

    for (i=1,j=0; j<LED_STRING_NUM; j++,i=i*2)
    {
        msLED_SetDimCurrent(i, gLEDInfo.bCurrent[j]);// need to check this function
    }

#endif

    //msLED_DimmingPWMAdjust(0x86);   // PWM duty cycle               // LED dimming controller  DUTY,  Initial Duty =50%
    if (gLEDInfo.bDimCtlModeMultiPhase == MULTI_Phase)
    {
        msWrite2Byte(REG_0AA0,0x0010); //bit 0~9
        msWrite2Byte(REG_0AA2,0x0040);   //45 degree
        msWrite2Byte(REG_0AA4,0x0080);   //90 degree
        msWrite2Byte(REG_0AA6,0x00C0);   //135 degree
    }
    else
    {
        msWrite2Byte(REG_0AA0,0x0000); //bit 0~9
        msWrite2Byte(REG_0AA2,0x0000);
        msWrite2Byte(REG_0AA4,0x0000); //bit 0~9
        msWrite2Byte(REG_0AA6,0x0000);
    }
#if 0 //20110725
    if(bPanelOnFlag)//(PANEL_ON_FLAG)
        msLED_DimmingEnable(_ENABLE,0xFFFF);                                     // Enable 16 Dimming PWM
#endif
    msWriteByteMask(REG_0A85,BIT0,BIT0);                            // Upload PWM with Double buffer
}
//==============================================================================

#if ENABLE_LED_INT
#if LED_DEBUG_PROTECTION
bit bEnterOpen_Interrput=0;
bit bEnterShort_Interrput=0;
BYTE xdata ucShortCheckCounter=0;
WORD xdata wINT_CHECK_4F4E=0; //INT0 MASK
WORD xdata wINT_CHECK_5150=0; //INT1 MASK
WORD xdata wINT_CHECK_5352=0; //INT2 MASK

WORD xdata wINT_CHECK_5B5A=0; //INT0 STATUS
WORD xdata wINT_CHECK_5D5C=0; //INT1 STATUS
WORD xdata wINT_CHECK_5F5E=0; //INT2 STATUS

WORD xdata wINT_CHECK_6968=0; //Boost calibration Enable
WORD xdata wINT_CHECK_8180=0; //Dimming PWM Enable
WORD xdata wINT_CHECK_D1D0=0; //SAR Detection switch
#endif
#if ENABLE_TSUML_NEWOVP
#if OVP2_INTERRUPT
BYTE xdata OVP2_Counter=0;
#endif
#endif

//BYTE xdata opencheck=0;

void msLED_Interrupt(void)
{
    WORD i=0;
    BYTE Temp=0;

#if ENABLE_TSUML_NEWOVP
    #if OVP2_INTERRUPT

    if (msRegs[REG_0AC6]&BIT5)
    {
        if(OVP2_Counter<3)
        {
            OVP2_Counter++;
            msRegs[REG_0AC4]=msRegs[REG_0AC4]|(BIT5);/// clr interrupt flag
        }
        else
        {
            msRegs[REG_0A80]=msRegs[REG_0A80]&(0x00);//disable dimming
            msRegs[REG_0A68]=msRegs[REG_0A68]&(0x00);
#if LED_SAR_VOLTAGE_MEASURE_ENABLE
            msRegs[REG_0AD0]=msRegs[REG_0AD0]&(0x00);
#endif
            msRegs[REG_0A52]=msRegs[REG_0A52]|(0xFF);//disable open interrupt
            msRegs[REG_0A4E]=msRegs[REG_0A4E]|(0xFF);//disable two short interrupt
            gLEDInfo.bBoostOnOff = 0;
            msRegs[REG_0A95]=msRegs[REG_0A95]&(~BIT7);//Disable Boost pwm
            msRegs[REG_0A87]=msRegs[REG_0A87]&(~BIT7);//Disable Boost Clk
            OVP2_Counter=0;
            msRegs[REG_0AC4]=msRegs[REG_0AC4]|(BIT5);/// clr interrupt flag
            msRegs[REG_0AC0]=msRegs[REG_0AC0]&(~BIT5);  // disable interrupt mask
        }
    }
    else
    #endif
#endif
#if OCP_INTERRUPT
    if (msRegs[REG_0AC6]&BIT6)
    {
        msRegs[REG_0A80]=msRegs[REG_0A80]&(0x00);   //disable dimming
        msRegs[REG_0A68]=msRegs[REG_0A68]&(0x00);
#if LED_SAR_VOLTAGE_MEASURE_ENABLE
        msRegs[REG_0AD0]=msRegs[REG_0AD0]&(0x00);
#endif
        msRegs[REG_0A52]=msRegs[REG_0A52]|(0xFF);   //disable open interrupt
        msRegs[REG_0A4E]=msRegs[REG_0A4E]|(0xFF);   //disable two short interrupt
        gLEDInfo.bBoostOnOff = 0;
        msRegs[REG_0A95]=msRegs[REG_0A95]&(~BIT7);  //Disable Boost pwm
        msRegs[REG_0A87]=msRegs[REG_0A87]&(~BIT7);  //Disable Boost Clk
        msRegs[REG_0AC4]=msRegs[REG_0AC4]|(BIT6);  // clr interrupt flag
        msRegs[REG_0AC0]=msRegs[REG_0AC0]&(~BIT6);  // disable interrupt mask
    }

#endif

#if ENABLE_LED_OPEN_SHORT_DEBOUNCE
    if ((msRegs[REG_0A5E])|(msRegs[REG_0A5F]))
    {
        bLEDOpenShortFlag = 0;
        LEDOpenShortCounter = LED_OPEN_SHORT_COUNT;
        bLEDOpenShortFlag = 1;
        bLEDOpenShortIntEnFlag = 0;

    #if LED_CH_0
        if (msRegs[REG_0A5E]&BIT4)
        {
        #if ENABLE_DETECT_LED_CH_0        
            LEDOpenShortString |= BIT4;
        #endif
            msRegs[REG_0A52] |= BIT4;   // enable interrupt mask
            msRegs[REG_0A4C] |= BIT4; // clear interrupt status
        }
    #endif
    
    #if LED_CH_1
        if (msRegs[REG_0A5E]&BIT5)
        {
        #if ENABLE_DETECT_LED_CH_1      
            LEDOpenShortString |= BIT5;
        #endif
            msRegs[REG_0A52] |= BIT5; // enable interrupt mask
            msRegs[REG_0A4C] |= BIT5; // clear interrupt status
        }
    #endif
    
    #if LED_CH_2
        if (msRegs[REG_0A5E]&BIT6)
        {
        #if ENABLE_DETECT_LED_CH_2     
            LEDOpenShortString |= BIT6;
        #endif
            msRegs[REG_0A52] |= BIT6; // enable interrupt mask
            msRegs[REG_0A4C] |= BIT6; // clear interrupt status
        }
    #endif

    #if LED_CH_3
        if (msRegs[REG_0A5E]&BIT7)
        {
        #if ENABLE_DETECT_LED_CH_3
            LEDOpenShortString |= BIT7;
        #endif
            msRegs[REG_0A52] |= BIT7; // enable interrupt mask
            msRegs[REG_0A4C] |= BIT7; // clear interrupt status
        }
    #endif
    }

    if ((msRegs[REG_0A5A])|(msRegs[REG_0A5B]))
    {
        bLEDOpenShortFlag = 0;
        LEDOpenShortCounter = LED_OPEN_SHORT_COUNT;
        bLEDOpenShortFlag = 1;
        bLEDOpenShortIntEnFlag = 0;
        
    #if LED_CH_0
        if (msRegs[REG_0A5A]&BIT0)
        {
        #if ENABLE_DETECT_LED_CH_0
            LEDOpenShortString |= BIT0;
        #endif
            msRegs[REG_0A4E] |= BIT0; // enable interrupt mask
            msRegs[REG_0A48] |= BIT0; // clear interrupt status
        }
    #endif
    
    #if LED_CH_1
        if (msRegs[REG_0A5A]&BIT1)
        {
        #if ENABLE_DETECT_LED_CH_1
            LEDOpenShortString |= BIT1;
        #endif
            msRegs[REG_0A4E] |= BIT1; // enable interrupt mask
            msRegs[REG_0A48] |= BIT1; // clear interrupt status
        }
    #endif

    #if LED_CH_2
        if (msRegs[REG_0A5A]&BIT2)
        {
        #if ENABLE_DETECT_LED_CH_2
            LEDOpenShortString |= BIT2;
        #endif
            msRegs[REG_0A4E] |= BIT2; // enable interrupt mask
            msRegs[REG_0A48] |= BIT2; // clear interrupt status
        }
    #endif

    #if LED_CH_3
        if (msRegs[REG_0A5A]&BIT3)
        {
        #if ENABLE_DETECT_LED_CH_3
            LEDOpenShortString |= BIT3;
        #endif
            msRegs[REG_0A4E] |= BIT3; // enable interrupt mask
            msRegs[REG_0A48] |= BIT3; // clear interrupt status
        }
    #endif
    }
    
#else // !ENABLE_LED_OPEN_SHORT_DEBOUNCE

  #ifdef _OPEN_INTERRUPT_ENABLE_
    if ((msRegs[REG_0A5E])|(msRegs[REG_0A5F]))
    {
    #if LED_CH_0

        if (msRegs[REG_0A5E]&BIT4)   // 2
        {
            //msRegs[REG_0A80]=msRegs[REG_0A80]&(~BIT0);
            msRegs[REG_0A68]=msRegs[REG_0A68]&(~BIT4);
            #if LED_SAR_VOLTAGE_MEASURE_ENABLE
            msRegs[REG_0AD0]=msRegs[REG_0AD0]&(~(BIT0|BIT4));
            #endif
            msRegs[REG_0A52]=msRegs[REG_0A52]|BIT4;   //disable interrupt
        }

    #endif
    #if LED_CH_1

        if (msRegs[REG_0A5E]&BIT5)  // 7
        {
            //msRegs[REG_0A80]=msRegs[REG_0A80]&(~BIT1);
            msRegs[REG_0A68]=msRegs[REG_0A68]&(~BIT5);
            #if LED_SAR_VOLTAGE_MEASURE_ENABLE
            msRegs[REG_0AD0]=msRegs[REG_0AD0]&(~(BIT1|BIT5));
            #endif
            msRegs[REG_0A52]=msRegs[REG_0A52]|BIT5;
        }

    #endif
    #if LED_CH_2

        if (msRegs[REG_0A5E]&BIT6)
        {
            //msRegs[REG_0A80]=msRegs[REG_0A80]&(~BIT2);
            msRegs[REG_0A68]=msRegs[REG_0A68]&(~BIT6);
            #if LED_SAR_VOLTAGE_MEASURE_ENABLE
            msRegs[REG_0AD0]=msRegs[REG_0AD0]&(~(BIT2|BIT6));
            #endif
            msRegs[REG_0A52]=msRegs[REG_0A52]|BIT6;
        }

#endif
    #if LED_CH_3

        if (msRegs[REG_0A5E]&BIT7)
        {
            //msRegs[REG_0A80]=msRegs[REG_0A80]&(~(BIT3));
            msRegs[REG_0A68]=msRegs[REG_0A68]&(~(BIT7));
            #if LED_SAR_VOLTAGE_MEASURE_ENABLE
            msRegs[REG_0AD0]=msRegs[REG_0AD0]&(~(BIT3|BIT7));
            #endif
            msRegs[REG_0A52]=msRegs[REG_0A52]|BIT7;
        }

    #endif
#if LED_CH_0||LED_CH_1||LED_CH_2||LED_CH_3

	if (msRegs[REG_0A68]==0x00 && msRegs[REG_0AD0] ==0x00 && msRegs[REG_0A80] !=0x00)
	{
		msRegs[REG_0A80]=0x00;
		msRegs[REG_0A68]=0x00;
	#if LED_SAR_VOLTAGE_MEASURE_ENABLE
		msRegs[REG_0AD0]=0x00;
	#endif
        	msRegs[REG_0A52]=msRegs[REG_0A52]|(0xFF);   //disable open interrupt
	}

#endif
    }

  #endif

    if ((msRegs[REG_0A5A])|(msRegs[REG_0A5B]))
    {
      //  u8Inter_Count++;
        #if LED_CH_0
        if (msRegs[REG_0A5A]&BIT0)   // 2
        {
            msRegs[REG_0A80]=msRegs[REG_0A80]&(~BIT0);
            msRegs[REG_0A68]=msRegs[REG_0A68]&(~BIT4);
        #if LED_SAR_VOLTAGE_MEASURE_ENABLE
            msRegs[REG_0AD0]=msRegs[REG_0AD0]&(~(BIT0|BIT4));
        #endif
            msRegs[REG_0A4E]=msRegs[REG_0A4E]|BIT0;   //disable interrupt
        }

        #endif
        #if LED_CH_1

        if (msRegs[REG_0A5A]&BIT1)  // 7
        {
            msRegs[REG_0A80]=msRegs[REG_0A80]&(~BIT1);
            msRegs[REG_0A68]=msRegs[REG_0A68]&(~BIT5);
        #if LED_SAR_VOLTAGE_MEASURE_ENABLE
            msRegs[REG_0AD0]=msRegs[REG_0AD0]&(~(BIT1|BIT5));
        #endif
            msRegs[REG_0A4E]=msRegs[REG_0A4E]|BIT1;
        }

#endif
        #if LED_CH_2

        if (msRegs[REG_0A5A]&BIT2)
        {
            msRegs[REG_0A80]=msRegs[REG_0A80]&(~BIT2);
            msRegs[REG_0A68]=msRegs[REG_0A68]&(~BIT6);
        #if LED_SAR_VOLTAGE_MEASURE_ENABLE
            msRegs[REG_0AD0]=msRegs[REG_0AD0]&(~(BIT2|BIT6));
        #endif
            msRegs[REG_0A4E]=msRegs[REG_0A4E]|BIT2;
        }

#endif
        #if LED_CH_3

        if (msRegs[REG_0A5A]&BIT3)
        {
            msRegs[REG_0A80]=msRegs[REG_0A80]&(~(BIT3));
            msRegs[REG_0A68]=msRegs[REG_0A68]&(~(BIT7));
        #if LED_SAR_VOLTAGE_MEASURE_ENABLE
            msRegs[REG_0AD0]=msRegs[REG_0AD0]&(~(BIT3|BIT7));
        #endif
            msRegs[REG_0A4E]=msRegs[REG_0A4E]|BIT3;
        }

        #endif
    }
#endif // end of ENABLE_LED_OPEN_SHORT_DEBOUNCE

#if LED_AUTO_RECOVER

        if(StringShortOpenChecking)
        {
            StringShortOpenStatus = (msRegs[REG_0A5A]&0x0F)|(msRegs[REG_0A5E]&0xF0);

            if(StringShortOpenStatus)
            {
                StringShortOpenFlag = 1;
                StringRebootFlag = 0;
            }
        }

#endif
    #if LED_DEBUG_PROTECTION
    bEnterShort_Interrput=TRUE;
    wINT_CHECK_4F4E=msRegs[REG_0A4F]<<7|msRegs[REG_0A4E];
    wINT_CHECK_5B5A=msRegs[REG_0A5B]<<7|msRegs[REG_0A5A];
    wINT_CHECK_6968=msRegs[REG_0A69]<<7|msRegs[REG_0A68];
    wINT_CHECK_8180=msRegs[REG_0A81]<<7|msRegs[REG_0A80];
    wINT_CHECK_D1D0=msRegs[REG_0AD1]<<7|msRegs[REG_0AD0];
    #endif

    #if LED_DEBUG_PROTECTION
    bEnterOpen_Interrput=TRUE;
    wINT_CHECK_5352=msRegs[REG_0A53]<<7|msRegs[REG_0A52];
    wINT_CHECK_5F5E=msRegs[REG_0A5F]<<7|msRegs[REG_0A5E];
    wINT_CHECK_6968=msRegs[REG_0A69]<<7|msRegs[REG_0A68];
    wINT_CHECK_8180=msRegs[REG_0A81]<<7|msRegs[REG_0A80];
    wINT_CHECK_D1D0=msRegs[REG_0AD1]<<7|msRegs[REG_0AD0];
    #endif
}

//==============================================================================
void Init_LEDIRQEnable(BOOL bEnable)
{
	INT_IRQ_LEDSARINT_ENABLE(FALSE);
	//==============================mask=========
	msWrite2Byte(REG_0A4E,0xFFFF);
	msWrite2Byte(REG_0A50,0xFFFF);					   //Disable SAR INT 1
	#ifndef _OPEN_INTERRUPT_ENABLE_
	msWrite2Byte(REG_0A52,0xFFFF);						 //Disable SAR INT 2
	#endif
	msWrite2Byte(REG_0AC0,0xFFFF);						 //disable LED status INT
	
	msWrite2Byte(REG_0A48, 0xFFFF);                      //Clear SAR INT 0 Flag
	msWrite2Byte(REG_0A4A, 0xFFFF);                      //Clear SAR INT 1 Flag
	msWrite2Byte(REG_0A4C, 0xFFFF);                      //Clear SAR INT 2 Flag
	msWrite2Byte(REG_0AC4,0xFFFF);                       //Clear LED INT

	//LED_printData("!!!!!!!!! REG_0A2D==%x",msRead2Byte(REG_0A5A));
	//LED_printData("!!!!!!!!! REG_0A2E==%x",msRead2Byte(REG_0A5C));
	//LED_printData("!!!!!!!!! REG_0A2F==%x",msRead2Byte(REG_0A5E));

#if ENABLE_LED_INT_DELAY
    if(bEnable)
    {
        if(!LEDINTEnableFlag) // enable interrupt just when LEDINTEnableFlag be set
        {
            LEDINTEnableCounter = LED_INT_ENABLE_COUNT;
            return;
        }
    }
#endif

	INT_IRQ_LEDSARINT_ENABLE(bEnable);

	if (bEnable)
	{
#if ENABLE_LED_INT_DELAY
            if(LEDINTEnableFlag)
                LEDINTEnableFlag = 0;
#endif
	        //LED_printMsg("=Interrupt--On=");
	        msWrite2Byte(REG_0A4E, (0x00FF-STREAM_VALUE_D));  // INT1 Mask
	        
	#ifdef _OPEN_INTERRUPT_ENABLE_
		msWrite2Byte(REG_0A52, (0x00FF-STREAM_VALUE_S));  // INT1 Mask
	#endif

	#if OVP2_INTERRUPT
		msWrite2ByteMask(REG_0AC0,   0x00   ,BIT5); //enable OVP2 interrupt
	#endif

		LightBarCounter = OCP_Stable_COUNT;
			
		#if 0//OCP_INTERRUPT
			msWriteByteMask(REG_0AC4,   BIT6        ,BIT6);
			msWrite2ByteMask(REG_0AC0,   0x00   ,BIT6); //enable OCP interrupt
			LED_printData("%%%%%    REG_0AC0======>>%x",msRead2Byte(REG_0AC0));
		#endif

		//LED_printData("!!!!!!!!! REG_0A27==%x",msRead2Byte(REG_0A4E));
		//LED_printData("!!!!!!!!! REG_0A28==%x",msRead2Byte(REG_0A50));
		//LED_printData("!!!!!!!!! REG_0A29==%x",msRead2Byte(REG_0A52));

		//LED_printData("!!!!!!!!! REG_0A40==%x",msRead2Byte(REG_0A80));
		//LED_printData("!!!!!!!!! REG_0A34==%x",msRead2Byte(REG_0A68));
		//LED_printData("!!!!!!!!! REG_0A68==%x",msRead2Byte(REG_0AD0));
	}
	else
	{
#if ENABLE_LED_INT_DELAY
            LEDINTEnableCounter = 0;
            LEDINTEnableFlag = 0;
#endif
	        //LED_printMsg("=Interrupt--Off=");
	        msWrite2Byte(REG_0A4E,0xFFFF);
	        #ifdef _OPEN_INTERRUPT_ENABLE_
			msWrite2Byte(REG_0A52, 0xFFFF);  // INT1 Mask
	        #endif
	        #if OVP2_INTERRUPT
			msWrite2ByteMask(REG_0AC0,   BIT5   ,BIT5); //disable OVP2 interrupt
	        #endif
	        #if OCP_INTERRUPT
			msWriteByteMask(REG_0AC4,   BIT6        ,BIT6); // clear OCP interrupt status
			msWrite2ByteMask(REG_0AC0,   BIT6   ,BIT6); //disable OCP interrupt
			//LED_printData("%%%%%    REG_0AC0======>>%x",msRead2Byte(REG_0AC0));
	        #endif
	}
}
#endif
//==============================================================================
void Init_LEDVariable()
{
    LED_printMsg("===Init_LEDVariable===");

    u8SymState=eLEDSTATE_INITIAL;
    u8LEDState  =eLED_IDLE;

    gLEDAlert.bBOOST_PWM_States =KEEP_PWM;
    gLEDAlert.bBOOST_Warning =0;
    gLEDAlert.wLED_TwoFail =0;
    gLEDAlert.wLED_OneFail =0;
    gLEDAlert.wLED_Open =0;
    gLEDAlert.wHiBOOST =0;
    gLEDAlert.wLowBOOST =0;
    gLEDAlert.wSkew_Error=0;
    gLEDAlert.bBOOST_Warning=0;
    gLEDAlert.bSYStemFail =0;
}
//==============================================================================
#if (CHIP_ID == CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID==CHIP_TSUMD||CHIP_ID==CHIP_TSUMJ)
Bool msCheckGDRVVoltage(void)
{
#if LED_CTRL_BRIGHTNESS_BY_CURRENT||Enable_MultiSensing
    return TRUE;
#else
    WORD u16DetectedGDRV;
    BYTE waits = 5;
    BOOL result = FALSE;
    msWriteByteMask(REG_0AE1,BIT6,BIT6);    // [6]Bias Enable ;

    while (waits-- )
    {
        if(( msReadByte( REG_0A17 ) & BIT7 ))
        {

        u16DetectedGDRV=msRead2Byte(REG_0A16)&0x3FF;
        LED_printData("  $$$  u16DetectedGDRV===%x", u16DetectedGDRV);
        if (u16DetectedGDRV > GDRV_THRD)
       {
            result = TRUE;
        break;
            }
        }
    ForceDelay1ms(100);
    };
    return result;
#endif
}
void msLED_GDRV5VEnable( void)
{
    if (msCheckGDRVVoltage()==TRUE)
    {
        LED_printMsg("  $$$  msLED_GDRV5VEnable");
#if LED_Direct5VBoostCircuit
        msWriteByteMask(REG_0AF1,BIT6|BIT4,         BIT6|BIT4);             //[6]:boost error amp enable; [4]:RDAC enable;
        msWriteByteMask(REG_0AF4,BIT7|BIT4|UVLO_TH,BIT7|BIT4|UVLO_TH);   //[7]:UVLO enable; [4]:GDRV bias enable; [3:0]Set UVL TH to 4.875V；
        msWriteByteMask(REG_0AF2,0x7F,              0x7F);                  //[7:0]:GDRV 5V enable;
#else
        msWriteByteMask(REG_0AF1,BIT6|BIT4,         BIT6|BIT4);             //[6]:boost error amp enable; [4]:RDAC enable;
        msWriteByteMask(REG_0AF4,BIT4,              BIT7|BIT4|UVLO_TH); //[7]:UVLO enable; [4]:GDRV bias enable; [3:0]Set UVL TH to 4.875V；
        msWriteByteMask(REG_0AF2,0x00,              0x7F);              //[7:0]:GDRV 5V enable;
#endif
    }
    else
    {
            msWriteByteMask(REG_0AF1,0x00,              BIT6|BIT4);             //[6]:boost error amp enable; [4]:RDAC enable;
            msWriteByteMask(REG_0AF4,0x00,              BIT7|BIT4|UVLO_TH);  //[7]:UVLO enable; [4]:GDRV bias enable; [3:0]Set UVL TH to 4.875V；
            msWriteByteMask(REG_0AF2,0x00,              0x7F);                  //[7:0]:GDRV 5V enable;
    }
}
#endif
//==============================================================================
void  msLED_Init(void)
{
    BYTE xdata i=0;

#if ENABLE_DETECT_LED_CH_0 || ENABLE_DETECT_LED_CH_1 || ENABLE_DETECT_LED_CH_2 || ENABLE_DETECT_LED_CH_3
    StringDetectAndEnable = STREAM_VALUE_D;
    #if ENABLE_DETECT_LED_CH_0
    StringDetectAndEnable |= DETECT_LED_CH_0;
    #endif
    #if ENABLE_DETECT_LED_CH_1
    StringDetectAndEnable |= DETECT_LED_CH_1;
    #endif
    #if ENABLE_DETECT_LED_CH_2
    StringDetectAndEnable |= DETECT_LED_CH_2;
    #endif
    #if ENABLE_DETECT_LED_CH_3
    StringDetectAndEnable |= DETECT_LED_CH_3;
    #endif
#endif

    Init_SAR();
#if Enable_MultiSensing
    //msWriteByteMask(REG_0ABF,BIT7,BIT7);
    msWrite2ByteMask(REG_0ABE,0x80,0x1FF);
    msWrite2ByteMask(REG_0ABC,0x00,0x3FF);
#endif	 	

#if LED_CTRL_BRIGHTNESS_BY_CURRENT||Enable_MultiSensing
    msWriteByteMask(REG_0A6F,0x00   ,BIT7); //[7]:disable GDRV voltage measurement to enhance ADC report time
#else
    msWriteByteMask(REG_0A6F,BIT7|BIT5   ,BIT7|BIT5); 
#endif
msWrite2ByteMask(REG_0A6E,	509 ,0x1FF); //GDRV voltage wdog,18us x (0~511)，最大週期約為9ms


#if CHIP_ID == CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID==CHIP_TSUMD||CHIP_ID==CHIP_TSUMJ
    msLED_GDRV5VEnable();
#endif

#if ENABLE_HW_SHORT_PROTECTION
    msWriteByteMask(REG_0A83,BIT3,BIT3);
#endif

#if LED_THERMAL_SENSOR_PROTECT
    msWriteByteMask(REG_0A65,BIT6|BIT4,BIT6|BIT4|BIT3|BIT2); //[6]:OTP enable Set to 3'b100 TEMP Range will be 133~112 degree
    #if LED_CTRL_BRIGHTNESS_BY_CURRENT||Enable_MultiSensing
    msWriteByteMask(REG_0A67,0x00   ,BIT7); //[7]:disable Thermal  measurement to enhance ADC report time
    #else
    msWriteByteMask(REG_0A67,BIT7|BIT5	,BIT7|BIT5); 
    #endif
#endif
    msWrite2ByteMask(REG_0A66,  511	,0x1FF); //thermal wdog,18us x (0~511)，最大週期約為9ms

    msWrite2Byte(REG_0AD0,(STREAM_VALUE_D<<4|STREAM_VALUE_D));                            // Enable all SAR

#if ENABLE_LED_INT //20110302
    Init_LEDIRQEnable(FALSE);
#endif
//    LED_printData("\n\r====Version = %d =======", Version);
    msLED_BoostEnable(_DISABLE);
    msLED_DimmingEnable(_DISABLE,0xFFFF);

    for (i=0; i<LED_STRING_NUM; i++)
#if 0//LED_CTRL_BRIGHTNESS_BY_CURRENT
        gLEDInfo.bCurrent[i] = UserPrefBrightness; // UserPrefBrightness 0~100
#else
        gLEDInfo.bCurrent[i] = LEDBL_PanelDefaultCurrent;
#endif

    gLEDInfo.bBoostClkODClk = Boost_216MClk;//Boost_ODClk; //Boost_ODClk;      // different ???
    gLEDInfo.bControlModeHW = HW_Mode;

    gLEDInfo.ucBoostPwmStep = BOOST_PWM_TUNING_STEP;//0x05;
    gLEDInfo.WCurrentPWM = MIN_BOOST_PWM;               // different ???
    gLEDInfo.bDimCtlModeMultiPhase = SINGLE_Phase;                     // different ???

    //gLEDInfo.bDimCtlModeMultiPhase = MULTI_Phase;                     // different ???



    if (gLEDInfo.bControlModeHW ==HW_Mode)
    {
        LED_printMsg("ControlMode = HW_Mode");
    }
    #if LED_SW_MODE	
    else
    {
        LED_printMsg("ControlMode = SW_Mode");
    }
    #endif

    Init_LEDVariable();
    //Init_SAR();
    Init_LED_Setting(); // set LED Normal range and skew range

    Init_DimmingPWM(); // backlight Freq

    Init_BoostPWM();
    gLEDInfo.bPREPARE_Delay  = 0;                    // 3;    (Sec) // 120327 coding, from trunk
    u8SymState = eLEDSTATE_PREPARE; // 120327 coding, from trunk
}
//==============================================================================
void msLED_SetDimCurrent(WORD Channels, BYTE Current)// need to check this function
{
    BYTE xdata i=0,j=0;

    Current = AnalogCurrent(Current);

    for (i=0; i<LED_STRING_NUM; i++)
    {
        if((Channels>>i)&0x01)
        {
            msWrite2Byte(REG_0AE4+i*2,  Current);
            //LED_printData("\n\r 11111  Channels==%x",Channels);
            //LED_printData("\n\r 22222  ((Channels&0x000F)>>i)==%x",((Channels&0x000F)>>i));
            //LED_printData("\n\r 33333   msRead2Byte(REG_0AE4+i*2)==%x", msRead2Byte(REG_0AE4+i*2));
        }
    }
}

//==============================================================================
void msLED_ReloadBoostPWM(void)
{
    msWriteByteMask(REG_0A85,BIT1,BIT1);        //Reload PWM Duty
}

void msLED_ReloadDimmingPWM(void)
{
    msWriteByteMask(REG_0A85,BIT0,BIT0);        //Reload Dimming Duty
}
//==============================================================================
void msLED_CheckFlag(void)
{
    BYTE xdata reg_value;

    reg_value = msReadByte(REG_0AC6);                            //High Byte
    //LED_printData(" ==>REG_0AC6 =%x", reg_value);
    LEDSatus.Separate.bBoost_Under_Voltage =GET_BIT7(reg_value);            //Separate[0]
    LEDSatus.Separate.bBoost_Over_Current = GET_BIT6(reg_value);     //Separate[1]
    LEDSatus.Separate.bBoost_Over_Voltage = GET_BIT5(reg_value);     //Separate[2]
    LEDSatus.Separate.bBoost_Over_Temperature = GET_BIT4(reg_value);    //Separate[3]
    LEDSatus.Separate.bBoost_Over_Skew = GET_BIT3(reg_value);            //Separate[4]
    LEDSatus.Separate.bNonuse2=0;
    LEDSatus.Separate.bNonuse1=0;
    LEDSatus.Separate.bNonuse=0;

    //if(gLEDInfo.bControlModeHW == 1)
    {
        reg_value = msReadByte(REG_0AC7);                                         //Low Byte
        if (reg_value!=0x06)
        LED_printData(" ==>REG_0AC7 =%x", reg_value);
#if ENABLE_LED_OPEN_SHORT_DEBOUNCE      
        LEDSatus.Separate.bHigh_Boost = 0;      //Separate[0]
        LEDSatus.Separate.bLow_Boost = 0;     //Separate[1]
        LEDSatus.Separate.bBoost_High_low_Error = 0;    //Separate[4]
#else
        LEDSatus.Separate.bHigh_Boost = GET_BIT7(reg_value);             //Separate[0]
        LEDSatus.Separate.bLow_Boost = GET_BIT6(reg_value);              //Separate[1]
        LEDSatus.Separate.bBoost_High_low_Error = GET_BIT3(reg_value);       //Separate[4]
#endif
        LEDSatus.Separate.bSkew_Error = GET_BIT5(reg_value);             //Separate[2]
        LEDSatus.Separate.bCross_Boundary_Error = GET_BIT4(reg_value);       //Separate[3]
        LEDSatus.Separate.bBoost_Tuning_Finish= GET_BIT2(reg_value);     //Separate[5]
        LEDSatus.Separate.bBoost_Data_Fnish = GET_BIT1(reg_value);           //Separate[6]
        LEDSatus.Separate.bBoost_Clock_Fail = GET_BIT0(reg_value);           //Separate[7]
    }
}
//==============================================================================
#if LED_SW_MODE
BOOST_STATE msLED_BoostPWMAdjust(BOOST_PWM_ADJUST UpDown)
{
    WORD xdata temp=0;
    BYTE   xdata Flag=Normal_PWM;


    if (UpDown == UP_PWM)
    {
        gLEDInfo.WCurrentPWM  =  msRead2Byte(REG_0A90) +(WORD) gLEDInfo.ucBoostPwmStep;

    }
    else if (UpDown == DOWM_PWM)
    {
        gLEDInfo.WCurrentPWM =  msRead2Byte(REG_0A90) - (WORD) gLEDInfo.ucBoostPwmStep;
    }
    else
        gLEDInfo.WCurrentPWM =  msRead2Byte(REG_0A90);


    if (gLEDInfo.WCurrentPWM < MIN_BOOST_PWM)
    {
        gLEDInfo.WCurrentPWM = MIN_BOOST_PWM;

        Flag= Min_PWM;
    }
    else if (gLEDInfo.WCurrentPWM > MAX_BOOST_PWM)
    {
        gLEDInfo.WCurrentPWM = MAX_BOOST_PWM;

        Flag= Max_PWM;
    }
    else
    {
        Flag= Normal_PWM;
    }
    msWrite2Byte(REG_0A90,gLEDInfo.WCurrentPWM);
    msLED_ReloadBoostPWM();
    return Flag;
}
#endif
//==============================================================================
void msLED_DimmingPWMAdjust(BYTE PwmValue)
{
    BYTE xdata i=0;

#if 0//ENABLE_SUPER_RESOLUTION
    msWrite2Byte(REG_0AB0, 0x1FF);
    msWrite2Byte(REG_0AB2, 0x1FF);
    msWrite2Byte(REG_0AB4, 0x1FF);
    msWrite2Byte(REG_0AB6, 0x1FF);
    msLED_ReloadDimmingPWM();  //未開有辦法 update dimmingpwm嗎？ //20110303
#elif LED_CTRL_BRIGHTNESS_BY_CURRENT
    // 0~0xFF to 0~100
    //PwmValue = (WORD)PwmValue*100/255;
    #if ENABLE_LED_INT
    Init_LEDIRQEnable(FALSE);
    #endif
    BrightnessPWMSettingToCurrentSetting(PwmValue);
    #if ENABLE_LED_INT
    Init_LEDIRQEnable(TRUE);
    #endif
    PwmValue = 0xFF;
#else
    for (i = 0; i < LED_STRING_NUM ; i++)
    {
	(PwmValue==0xFF)?(msWrite2Byte(REG_0AB0+i*2, 0x1FF)):(msWrite2Byte(REG_0AB0+i*2, (WORD)(PwmValue<<1)));
        //msWrite2Byte(REG_0AB0+i*2, (WORD)(PwmValue<<1)); //512 step
    }
    msLED_ReloadDimmingPWM();  //未開有辦法 update dimmingpwm嗎？ //20110303
#endif
    g_u8PWMValue = PwmValue;
}
//==============================================================================
#if LED_SW_MODE
void msLED_Control(void)
{
    switch (u8LEDState)
    {

        case eLED_ADJUST_BOOST_HIGH:
            gLEDAlert.bBOOST_PWM_States=msLED_BoostPWMAdjust(UP_PWM);
            break;

        case eLED_ADJUST_BOOST_LOW:
            gLEDAlert.bBOOST_PWM_States=msLED_BoostPWMAdjust(DOWM_PWM);
            break;

        case eLED_ADJUST_DIMCURRENT:
            //msLED_SetDimCurrent()
            break;
        case  eLED_IDLE:

            break;

    }
}
#endif
//==============================================================================
#if 0
void msLED_CheckLED(void)
{

        printMsg(" msLED_CheckLED");

// Check One Led Short
    if (msReadWord( REG_0A5D)&STREAM_VALUE_S)
    {
        gLEDAlert.wLED_OneFail =(msReadWord( REG_0A5B)|gLEDAlert.wLED_OneFail)&STREAM_VALUE_S;

        LED_printData(" HW:gLEDAlert.wLED_OneFail = %x " , gLEDAlert.wLED_OneFail );

        msLED_SetDimCurrent((msReadWord( REG_0A5D))&STREAM_VALUE_S, DIM_CURRENT_ONE_FAIL);
    }

// Check Two Led Short
    if (msReadWord( REG_0A5B)&STREAM_VALUE_S)
    {
        gLEDAlert.wLED_TwoFail =(msReadWord( REG_0A5B )|gLEDAlert.wLED_TwoFail)&STREAM_VALUE_S;

        LED_printData(" HW:gLEDAlert.wLED_TwoFail = %x " , gLEDAlert.wLED_TwoFail );

        msLED_DimmingEnable(_DISABLE, (msReadWord( REG_0A5B))&STREAM_VALUE_S);

        msLED_SetDimCurrent((msReadWord( REG_0A5B))&STREAM_VALUE_S, 0);
    }

// Check LED OPEN
    if (msReadWord( REG_0A5F)&STREAM_VALUE_S)
    {
        gLEDAlert.wLED_Open =(msReadWord( REG_0A5F )|gLEDAlert.wLED_Open)&STREAM_VALUE_S;

        LED_printData(" HW:gLEDAlert.wLED_Open = %x " , gLEDAlert.wLED_Open );

        msLED_DimmingEnable(_DISABLE, (msReadWord( REG_0A5F ))&STREAM_VALUE_S);
    }
}
#endif
//----------------------------------------------------------------------------------------------------
void msLED_Set_BoostPwmStep(BYTE value)
{
    msWriteByte(REG_0A94, value);
}
WORD msLED_Get_LEDSAR_Data(WORD channels)
{
    return (msRead2Byte(REG_0A20 + channels * 2))&0x3FF;     // gete V Data
}

void msLED_Get_Led_Status(void)
{
    BYTE xdata i=0;
    WORD xdata SAR_VD_DATA[LED_STRING_NUM] = {0,0,0,0};
    WORD xdata SAR_VS_DATA[LED_STRING_NUM] = {0,0,0,0};
        //if ((msReadByte(REG_0A83)&0x01)==0x01)  //Clear Flag  //?????? //20110302
        {
            gLEDAlert.wLED_TwoFail =0;  //
            gLEDAlert.wLED_OneFail =0;
            gLEDAlert.wLED_Open =0;
            gLEDAlert.wSkew_Error=0;
        }

    //---------------------------------------------------------------------
    for (i = 0; i < LED_STRING_NUM ; i++)     //Read LED string Data（SAR detected voltage）
    {
        SAR_VD_DATA[i] = msLED_Get_LEDSAR_Data(Stream_D[i][1]);
        SAR_VS_DATA[i] = msLED_Get_LEDSAR_Data(Stream_S[i][1]);
    }
    //---------------------------------------------------------------------
    if ( LEDSatus.Separate.bCross_Boundary_Error )   //REG_0AC7[4] ,step>V3-V4(Ex 0.55-0.54)
    {
        gLEDInfo.ucBoostPwmStep = 1;
        LED_printMsg("====>Set PwmStep = 1 ");
            msLED_Set_BoostPwmStep(gLEDInfo.ucBoostPwmStep);

    }
    else if (LEDSatus.Separate.bBoost_Data_Fnish)  //REG_0AC7[1] , tunning
    {
        gLEDInfo.ucBoostPwmStep = BOOST_PWM_TUNING_STEP;
            msLED_Set_BoostPwmStep(gLEDInfo.ucBoostPwmStep);
        //LED_printMsg("====>Tuning_Data_Ready");
    }
    if (LEDSatus.Separate.bBoost_Tuning_Finish)  //REG_0AC7[2] , adjustment is finished
    {
        //LED_printMsg("====>Tuning_Finish(No Error ---)");
    }
#if LED_THERMAL_SENSOR_PROTECT
    //---------------------------------------------------------------------
    if (LEDSatus.Separate.bBoost_Over_Temperature)
    {
        msLED_DimmingEnable(_DISABLE, 0xFFFF);
        msLED_SetDimCurrent(0xFFFF, 0);
        LED_printMsg("====>bBoost_Over_Temperature ");
    }
#endif
#if 1
#if 0
    //---------------------------------------------------------------------
    if ( LEDSatus.Separate.bSkew_Error)              //REG_0AC7[5] 0.55>Vmin>0.54 and Vmax>1.55(skew min voltage)
    {
        for (i = 0; i < LED_STRING_NUM ; i++)
        {
            if(Stream_D[i][0]==0)   // this CH is Close
                continue;

            if (SAR_VD_DATA[i] > SAR_V_8p00)                    //Two LED fail  led short
            {
                gLEDAlert.wLED_TwoFail |= Stream_D[i][0]; //
                gLEDAlert.wLED_TwoFail |= Stream_S[i][0]; //
                LED_printData("FW: gLEDAlert.wLED_TwoFail = %x " , gLEDAlert.wLED_TwoFail );
                msLED_DimmingEnable(_DISABLE, gLEDAlert.wLED_TwoFail);
                msLED_SetDimCurrent(gLEDAlert.wLED_TwoFail, 0);
            }
            if ((SAR_VD_DATA[i] < SAR_V_8p00)&&(SAR_VD_DATA[i] >SAR_V_4p50))  // 1 LED fail   led short
            {
                gLEDAlert.wLED_OneFail |= Stream_D[i][0]; //
                gLEDAlert.wLED_OneFail |= Stream_S[i][0]; //
                LED_printData("FW: gLEDAlert.wLED_OneFail = %x " , gLEDAlert.wLED_OneFail );
#if LED_CAL_POWER
                msLED_PowerCal((WORD *)SAR_VS_DATA, _DISABLE);
#else
                msLED_SetDimCurrent(gLEDAlert.wLED_OneFail, DIM_CURRENT_ONE_FAIL);
#endif
            }
            if ((SAR_VD_DATA[i] > SAR_V_1p55)&&(SAR_VD_DATA[i] < SAR_V_4p50))                     // Led Skew error
            {
                gLEDAlert.wSkew_Error |= Stream_D[i][0]; //
                gLEDAlert.wSkew_Error |= Stream_S[i][0]; //
                LED_printData("FW: gLEDAlert.wSkew_Error = %x " , gLEDAlert.wSkew_Error );
            }
        }
    }
#endif
    //---------------------------------------------------------------------
    if ( LEDSatus.Separate.bLow_Boost)// LED open
    {
        // Check LED Open
        if (msRead2Byte( REG_0A5E)&STREAM_VALUE_D)
        {
            gLEDAlert.wLED_Open =(msRead2Byte( REG_0A5E )|gLEDAlert.wLED_Open)&STREAM_VALUE_D;//&STREAM_VALUE_D;
            for (i = 0; i < LED_STRING_NUM ; i++)
            {
                //LED_printData("FW: SAR_DATA[i] = %x " , SAR_DATA[i] );
                if(Stream_D[i][0]==0)   // this CH is Close
                    continue;
                if(gLEDAlert.wLED_Open & Stream_D[i][0])
                {
                    gLEDAlert.wLED_Open |= Stream_S[i][0];
                }
            }
            msLED_DimmingEnable(_DISABLE, gLEDAlert.wLED_Open);
                    LED_printData("\n\r HW: bLow_Boost.wLED_Open = %x " , gLEDAlert.wLED_Open );
        }
        else   //isnt open , so increase current
        {
#if LED_CAL_POWER
            msLED_PowerCal((WORD *)SAR_VS_DATA, _DISABLE);  //??????
#else
            msLED_SetDimCurrent(gLEDAlert.wLED_OneFail, DIM_CURRENT_ONE_FAIL); //?????
#endif
                    LED_printMsg("\n\r  ====HW: bLow_Boost.Cal_Power ===== " );
        }
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    if ( LEDSatus.Separate.bHigh_Boost)
    {
        // Check 2 LED SHORT
        if (msRead2Byte( REG_0A5A)&STREAM_VALUE_D)
        {
            gLEDAlert.wLED_TwoFail =(msRead2Byte( REG_0A5A )|gLEDAlert.wLED_TwoFail)&STREAM_VALUE_D;
            for (i = 0; i < LED_STRING_NUM ; i++)
            {
                //LED_printData("FW: SAR_DATA[i] = %x " , SAR_DATA[i] );
                if(Stream_D[i][0]==0)   // this CH is Close
                    continue;
                if(gLEDAlert.wLED_TwoFail & Stream_D[i][0])
                {
                    gLEDAlert.wLED_TwoFail |= Stream_S[i][0];
                }
            }
            msLED_DimmingEnable(_DISABLE, gLEDAlert.wLED_TwoFail);
            msLED_SetDimCurrent(gLEDAlert.wLED_TwoFail, 0);
            LED_printData("Dimming_DISABLE:  = %x " , gLEDAlert.wLED_TwoFail );
        }
        else
        {
#if LED_CAL_POWER
            msLED_PowerCal((WORD *)SAR_VS_DATA, _DISABLE);   //??????
#else
            msLED_SetDimCurrent(gLEDAlert.wLED_OneFail, DIM_CURRENT_ONE_FAIL); //??????
#endif
        }
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    #if 0
    if ( LEDSatus.Separate.bBoost_Over_Skew)
    {
        // Check 1 LED SHORT
        if (msRead2Byte( REG_0A5C)&STREAM_VALUE_D)
        {
            gLEDAlert.wLED_OneFail =(msRead2Byte( REG_0A5C )|gLEDAlert.wLED_OneFail)&STREAM_VALUE_D;
            for (i = 0; i < LED_STRING_NUM ; i++)
            {
                //LED_printData("FW: SAR_DATA[i] = %x " , SAR_DATA[i] );
                if(Stream_D[i][0]==0)   // this CH is Close
                    continue;
                if(gLEDAlert.wLED_OneFail & Stream_D[i][0])
                {
                    gLEDAlert.wLED_OneFail |= Stream_S[i][0];
                }
            }
#if LED_CAL_POWER
            msLED_PowerCal((WORD *)SAR_VS_DATA, _DISABLE);
#else
            msLED_SetDimCurrent(gLEDAlert.wLED_OneFail, DIM_CURRENT_ONE_FAIL);
#endif
            LED_printData("SetDimCurrent:  = %x " , gLEDAlert.wLED_OneFail );
        }
        // Check 2 LED SHORT
        if (msRead2Byte( REG_0A5A)&STREAM_VALUE_D)
        {
            gLEDAlert.wLED_TwoFail =(msRead2Byte( REG_0A5A )|gLEDAlert.wLED_TwoFail)&STREAM_VALUE_D;
            for (i = 0; i < LED_STRING_NUM ; i++)
            {
                //LED_printData("FW: SAR_DATA[i] = %x " , SAR_DATA[i] );
                if(Stream_D[i][0]==0)   // this CH is Close
                    continue;
                if(gLEDAlert.wLED_TwoFail & Stream_D[i][0])
                {
                    gLEDAlert.wLED_TwoFail |= Stream_S[i][0];
                }
            }
            msLED_DimmingEnable(_DISABLE, gLEDAlert.wLED_TwoFail);
            msLED_SetDimCurrent(gLEDAlert.wLED_TwoFail, 0);
            LED_printData("Dimming_DISABLE:  = %x " , gLEDAlert.wLED_TwoFail );
        }
    }
	#endif
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    if ( LEDSatus.Separate.bBoost_High_low_Error)
    {
        // Check LED Open
        if (msRead2Byte( REG_0A5E)&STREAM_VALUE_D)
        {
            gLEDAlert.wLED_Open =(msRead2Byte( REG_0A5E )|gLEDAlert.wLED_Open)&STREAM_VALUE_D;
            for (i = 0; i < LED_STRING_NUM ; i++)
            {
                //LED_printData("FW: SAR_DATA[i] = %x " , SAR_DATA[i] );
                if(Stream_D[i][0]==0)   // this CH is Close
                    continue;
                if(gLEDAlert.wLED_Open & Stream_D[i][0])
                {
                    gLEDAlert.wLED_Open |= Stream_S[i][0];
                }
            }
            msLED_DimmingEnable(_DISABLE, gLEDAlert.wLED_Open);
            LED_printData("Dimming_DISABLE:  = %x " , gLEDAlert.wLED_Open );
        }
        // Check 1 LED SHORT
        if (msRead2Byte( REG_0A5C)&STREAM_VALUE_D)
        {
            gLEDAlert.wLED_OneFail =(msRead2Byte( REG_0A5C )|gLEDAlert.wLED_OneFail)&STREAM_VALUE_D;
            for (i = 0; i < LED_STRING_NUM ; i++)
            {
                //LED_printData("FW: SAR_DATA[i] = %x " , SAR_DATA[i] );
                if(Stream_D[i][0]==0)   // this CH is Close
                    continue;
                if(gLEDAlert.wLED_OneFail & Stream_D[i][0])
                {
                    gLEDAlert.wLED_OneFail |= Stream_S[i][0];
                }
            }
#if LED_CAL_POWER
            msLED_PowerCal((WORD *)SAR_VS_DATA, _DISABLE);
#else
            msLED_SetDimCurrent(gLEDAlert.wLED_OneFail, DIM_CURRENT_ONE_FAIL);
#endif
            LED_printData("SetDimCurrent:  = %x " , gLEDAlert.wLED_OneFail );
        }
        // Check 2 LED SHORT
        if (msRead2Byte( REG_0A5A)&STREAM_VALUE_D)
        {
            gLEDAlert.wLED_TwoFail =(msRead2Byte( REG_0A5A )|gLEDAlert.wLED_TwoFail)&STREAM_VALUE_D;
            for (i = 0; i < LED_STRING_NUM ; i++)
            {
                //LED_printData("FW: SAR_DATA[i] = %x " , SAR_DATA[i] );
                if(Stream_D[i][0]==0)   // this CH is Close
                    continue;
                if(gLEDAlert.wLED_TwoFail & Stream_D[i][0])
                {
                    gLEDAlert.wLED_TwoFail |= Stream_S[i][0];
                }
            }
            msLED_DimmingEnable(_DISABLE, gLEDAlert.wLED_TwoFail);
            msLED_SetDimCurrent(gLEDAlert.wLED_TwoFail, 0);
            LED_printData("Dimming_DISABLE:  = %x " , gLEDAlert.wLED_TwoFail );
        }
    }
    #endif
}

//==============================================================================
void msLED_CheckHW(void)
{

    //-----------------------------------------------------
    //get fail led status
    msLED_Get_Led_Status();
    //-----------------------------------------------------
    //setting by status
    //msLED_Set_Led_Setting();
    //-----------------------------------------------------

}
//==============================================================================
#if LED_SW_MODE||LED_CAL_POWER
void msLED_PowerCal(WORD *Data, Bool value)
{
    BYTE xdata i=0,Num=0;
    BYTE xdata Fail[2]=0;
    Bool xdata Low_Current=0;
    WORD xdata Temp=0;
    WORD xdata Temp1=0;
    //WORD xdata wFail_VS_Temp=0;

    for (i = 0; i < LED_STRING_NUM ; i++)
    {
        //LED_printData(" ===>%x", i);
        //LED_printData(" %x", *(Data+i));
#if LED_SKIP_DIMMING   // skip Dimming Off
        if (!(((WORD)(Stream_S[i][0]))&msRead2Byte(REG_0A80)))
            continue;
#endif
        if (value == _DISABLE)
        {
            if (((WORD)(Stream_S[i][0]))&(gLEDAlert.wLED_OneFail|gLEDAlert.wSkew_Error))
            {
                if (Num ==0)
                    Fail[0] = Stream_S[i][1];           // First LED fail Number
                else
                    Fail[1] = Stream_S[i][1];           // SEC LED fail Number
                Num++;
                continue;
            }
        }
        Temp = SAR_TO_VOL(*(Data+i));
        Temp1 = (Temp) * gLEDInfo.bCurrent[Stream_S[i][1]]*2 + Temp1;
        LED_printData(" VSpad Power==>%d (mW)", Temp1/10);
    }

    u16TotalPower = Temp1;

    //LED_printData(" u16TotalPower==>%d (mW)", u16TotalPower/10);

    if ((Num!=0)&&(value == _DISABLE))
    {
        if (u16TotalPower >= LIMIT_POWER)
        {
            Low_Current =1;
            LED_printData("\n\r Low_Current =1   && Leave power=%d" , Temp/10);
        }
        else
        {
            Temp = (LIMIT_POWER - u16TotalPower);
            LED_printData("\n\r Low_Current =0   && Leave power=%d" , Temp/10);
        }


        //wFail_VS_Temp = SAR_TO_VOL(*(Data+Fail[0]));
        //LED_printData("\n\r wFail_VS_Temp=%d" , wFail_VS_Temp);

        if (Temp > LEAVE_POWER) //
            Temp = LEAVE_POWER;

        gLEDInfo.bCurrent[Fail[0]] = Temp/(57); //one led short , VD=5.74V
        LED_printData("\n\r gLEDInfo.bCurrent[Fail[0]]=%d" , gLEDInfo.bCurrent[Fail[0]] );

        if (gLEDInfo.bCurrent[Fail[0]]  > LEDBL_PanelDefaultCurrent*2)
            gLEDInfo.bCurrent[Fail[0]]   = LEDBL_PanelDefaultCurrent*2;

        //LED_printData("\n\r Temp=%d" , Temp);
        //LED_printData("\n\r Num=%x" , Num);
        //LED_printData("\n\r Fail[0]=%x" , Fail[0] );
        //LED_printData("\n\r Current =%x" , gLEDInfo.bCurrent[Fail[0]] );

#if 1
        switch (Num)
        {
            case 1:
                if (Low_Current ==0)
                {
                    msLED_SetDimCurrent((WORD)(1<<Fail[0]),  gLEDInfo.bCurrent[Fail[0]] );
                    LED_printData("\n\r 1 msLED_SetDimCurrent.[Fail[0]]=%d" , gLEDInfo.bCurrent[Fail[0]] );
                }
                else
                    msLED_SetDimCurrent((WORD)(1<<Fail[0]), DIM_CURRENT_LOW);
                break;

            case 2:
                gLEDInfo.bCurrent[Fail[0]] = gLEDInfo.bCurrent[Fail[0]] /2 ;
                if (Low_Current ==0)
                {
                    msLED_SetDimCurrent((WORD)(1<<Fail[0]),  gLEDInfo.bCurrent[Fail[0]]);
                    msLED_SetDimCurrent((WORD)(1<<Fail[1]),  gLEDInfo.bCurrent[Fail[0]]);
                    LED_printData("\n\r 2 SetDimCurrent.[Fail[0]]=%d" , gLEDInfo.bCurrent[Fail[0]] );
                }
                else
                {
                    msLED_SetDimCurrent((WORD)(1<<Fail[0]),  DIM_CURRENT_LOW);
                    msLED_SetDimCurrent((WORD)(1<<Fail[1]),  DIM_CURRENT_LOW);
                }
                break;

            case 3:


            default:
                gLEDAlert.bSYStemFail = 1;
                gLEDInfo.bTurnOn_Delay = TIME_TURN_ON_DELAY;
                u8SymState = eLEDSTATE_Reset;
                break;
        }
     #if LED_SAR_VOLTAGE_MEASURE_ENABLE
    msWrite2ByteMask(REG_0AD0,0x0000,gLEDAlert.wLED_OneFail|gLEDAlert.wSkew_Error);
    LED_printData("\n\r wSkew_Error|wLED_OneFail=%x" , gLEDAlert.wLED_OneFail|gLEDAlert.wSkew_Error );
     #endif
#endif

    }
}
#endif
//==============================================================================
#if LED_SW_MODE
void msLED_CheckSW(void)
{
    WORD xdata SAR_DATA[LED_STRING_NUM] = {0,0,0,0};
    BYTE xdata Flag=5;
    BYTE xdata i =0;
    WORD xdata j=0;
    WORD xdata Min = 0x3FF;

    //Read 16 SAR Data
    //LED_printData("   2: PWM=%x", msReadWord(REG_0A99));

    for (i = 0; i < LED_STRING_NUM ; i++)
    {
        SAR_DATA[i] = msRead2Byte(REG_0A20 + i * 2)&0x3FF;

#if LED_SKIP_DIMMING
        if (!(((WORD)(1<<i))&msRead2Byte(REG_0A80)))
            continue;
#endif

        LED_printData("i = %x " , i);

        LED_printData("SAR_DATA[i] = %x " , SAR_DATA[i] );

#if 0
        LED_printData("=>%x" ,i);
#endif

        if (SAR_DATA[i] < Min)
        {
            Min = SAR_DATA[i];

            u8LED_Channel = i;
        }
    }

    LED_printData(" String Min = (%x) " , Min);

    for (i = 0; i < LED_STRING_NUM ; i++,Flag=5)
    {

        LED_printData("i = %x " , i);

        LED_printData("SAR_DATA[i] = %x " , SAR_DATA[i] );

#if LED_SKIP_DIMMING
        if (!(((WORD)(1<<i))&msRead2Byte(REG_0A80)))
            continue;
#endif

#if LED_SKIP_LOW_BOOST
        if (((WORD)(1<<i))&gLEDAlert.wLowBOOST)
            continue;
#endif


        if (SAR_DATA[i]  < SAR_V_0p54 )
        {
            Flag = 1;                                                    // Error
        }

        if (Flag !=1)
        {

            if ((Min<= (SAR_V_0p55 + TOLERANCE_SAR))&&(Min>=(SAR_V_0p54 - TOLERANCE_SAR)))    ///Min = { SAR_V_0p54 ; SAR_V_0p55 }
            {
#if 0
                LED_printData("+++> = %x " , i);

                LED_printData("SAR_DATA[i] = %x " , SAR_DATA[i] );
#endif

                if (SAR_DATA[i] > SAR_V_4p50)                         // 1 or 2 LED Error
                {
                    Flag = 2;

                }
                else if (SAR_DATA[i] > SAR_V_1p55)                    // Led Skew error
                {
                    Flag = 4;
                }
                else
                {
                    Flag = 5;
                }

            }
            else
            {
                Flag = 3;
            }

        }

        LED_printData("  Flag=>%x", Flag );


        if (Flag ==1)  //SAR_DATA[i]  < SAR_V_0p54
        {

            if (SAR_DATA[i] > SAR_V_5p00)                                   //Two LED fail
            {
                gLEDAlert.wLED_TwoFail =SET_BIT(gLEDAlert.wLED_TwoFail,i);

                msLED_DimmingEnable(_DISABLE, (1<<i));

                msLED_SetDimCurrent(i, 0);

                u8LEDState = eLED_IDLE;

            }
            if ((SAR_DATA[i] < SAR_V_5p00)&&(SAR_DATA[i] >SAR_V_4p50))  // 1 LED fail
            {
                gLEDAlert.wLED_OneFail =SET_BIT(gLEDAlert.wLED_OneFail,i);

#if LED_CAL_POWER
                msLED_PowerCal((WORD *)SAR_DATA, _DISABLE);
#else
                msLED_SetDimCurrent((WORD)(1<<i), DIM_CURRENT_ONE_FAIL);
#endif

                u8LEDState = eLED_IDLE;

            }

            if (gLEDAlert.bBOOST_PWM_States == Max_PWM)
            {

                if (SAR_DATA[i] < SAR_V_0p10)                   //LED Open
                {

                    LED_printData("=Led Open==>%x", i );

                    LED_printData("=>%x", SAR_DATA[i] );

                    gLEDAlert.wLED_Open = SET_BIT(gLEDAlert.wLED_Open , i);

                    msLED_DimmingEnable(_DISABLE, (1<<i));

                }
                else
                {

                    LED_printData("=Low Boost==>%x", i );

                    LED_printData("=>%x", SAR_DATA[i] );

                    gLEDAlert.wLowBOOST =SET_BIT(gLEDAlert.wLowBOOST,i);
                }

            }
            else
            {
                if (u8LED_Channel ==i)
                {
                    u8LEDState = eLED_ADJUST_BOOST_HIGH;
                }
                else
                {
                    u8LEDState = eLED_IDLE;
                }
            }



        }
        else if (Flag ==3)
        {
            LED_printData(" bBOOST_PWM_States==>%x " , gLEDAlert.bBOOST_PWM_States );

            if (gLEDAlert.bBOOST_PWM_States == Min_PWM)
            {
                gLEDAlert.wHiBOOST =    SET_BIT(gLEDAlert.wHiBOOST,i);

                u8LEDState = eLED_IDLE;

                LED_printMsg("----1----");
            }
            else
            {

                if (u8LED_Channel ==i)
                {
                    u8LEDState = eLED_ADJUST_BOOST_LOW;
                }
                else
                {
                    u8LEDState = eLED_IDLE;
                }
                LED_printMsg("----2----");
            }

        }
        else if (Flag == 2)
        {

            if (SAR_DATA[i] > SAR_V_5p00)    //Two LED fail
            {
                gLEDAlert.wLED_TwoFail =SET_BIT(gLEDAlert.wLED_TwoFail,i);

                msLED_DimmingEnable(_DISABLE, (1<<i));

                msLED_SetDimCurrent(i, 0);

                u8LEDState = eLED_IDLE;

            }

            if ((SAR_DATA[i] < SAR_V_5p00)&&(SAR_DATA[i] >SAR_V_4p50))  // 1 LED fail
            {
                gLEDAlert.wLED_OneFail =SET_BIT(gLEDAlert.wLED_OneFail,i);

                msLED_PowerCal((WORD *)SAR_DATA, _DISABLE);

                u8LEDState = eLED_IDLE;

            }

        }
        else if (Flag == 4) // Skew Error
        {
            gLEDAlert.wSkew_Error =SET_BIT(gLEDAlert.wSkew_Error,i);

            msLED_SetDimCurrent((WORD)(1<<i), DIM_CURRENT_OVERSKEW);

            LED_printMsg(" =Skew_Error=");

            u8LEDState = eLED_IDLE;

        }
        else if (Flag == 5) // Normal
        {
            LED_printMsg(" =Normal=");

            u8LEDState = eLED_IDLE;
        }


#if 1
        if (u8LEDState ==0)
            LED_printMsg("=======> PWM Normal");
        else if (u8LEDState ==1)
            LED_printMsg("=======> PWM Up");
        else if (u8LEDState ==2)
            LED_printMsg("=======> PWM Low");
#endif

        msLED_Control();

        LED_printData("=======> PWM Value = %x " ,msRead2Byte(REG_0A90));

    }
}
#endif
//==============================================================================
void msLED_CheckAlert(void)
{
    WORD xdata temp = 1;

    if (++gLEDInfo.bPrint_Delay ==0)
    {
        LEDSatus.Separate.bBoost_Tuning_Finish = 0;
        LEDSatus.Separate.bBoost_Data_Fnish =0;
        temp = LEDSatus.total;
#if 0
        LED_printData("u8States_5C= %x ", u8States_5C);
        LED_printData("u8States_5D= %x ", u8States_5D);
        LED_printData("u8Inter_Count= %x ", u8Inter_Count);
#endif

        if (gLEDInfo.bControlModeHW ==1)
        {
            if (temp)
            {
                if (LEDSatus.Separate.bBoost_Under_Voltage)
                    LED_printMsg("==>Boost_Under_Voltage");
                if (LEDSatus.Separate.bBoost_Over_Current)
                    LED_printMsg("==>Boost_Over_Current");
                if (LEDSatus.Separate.bBoost_Over_Voltage)
                    LED_printMsg("==>Boost_Over_Voltage");
                if (LEDSatus.Separate.bBoost_Over_Temperature)
                    LED_printMsg("==>Boost_Over_Temperature");
                if (LEDSatus.Separate.bBoost_Over_Skew)
                    LED_printMsg("==>Boost_Over_Skew");
                if (LEDSatus.Separate.bHigh_Boost )
                    LED_printMsg("==>High_Boost");
                if ( LEDSatus.Separate.bLow_Boost )
                    LED_printMsg("==>Low_Boost ");
                if (LEDSatus.Separate.bSkew_Error)
                    LED_printMsg("==>Skew_Error");
                if ( LEDSatus.Separate.bCross_Boundary_Error )
                    LED_printMsg("==>Cross_Boundary_Error");
                if (LEDSatus.Separate.bBoost_High_low_Error )
                    LED_printMsg("==>Boost_High_low_Error");
                if (LEDSatus.Separate.bBoost_Clock_Fail)
                    LED_printMsg("==>Boost_Clock_Fail");
            }
        }
        else
        {
            if (gLEDAlert.wLED_TwoFail ||gLEDAlert.wLED_OneFail ||gLEDAlert.wLED_Open||
                    gLEDAlert.wHiBOOST||gLEDAlert.wLowBOOST||gLEDAlert.wSkew_Error||
                    gLEDAlert.bBOOST_Warning||gLEDAlert.bSYStemFail)
            {
                LED_printData(" => TwoFail = %x ", gLEDAlert.wLED_TwoFail );
                LED_printData(" => OneFail = %x ",  gLEDAlert.wLED_OneFail );
                LED_printData(" => LED_Open = %x ", gLEDAlert.wLED_Open);
                LED_printData(" => HiBOOST = %x ", gLEDAlert.wHiBOOST);
                LED_printData(" => LowBOOST = %x ", gLEDAlert.wLowBOOST);
                LED_printData(" => Skew_Error = %x ", gLEDAlert.wSkew_Error);
                LED_printData(" => BOOST_Warning = %x ", gLEDAlert.bBOOST_Warning);
            }
        }

        if ((msReadByte(REG_0A83)&0x01)==0x01)  //Clear Flag  //?????? //20110302
        {
            gLEDAlert.wLED_TwoFail =0;
            gLEDAlert.wLED_OneFail =0;
            gLEDAlert.wLED_Open =0;
            gLEDAlert.wSkew_Error=0;
            gLEDAlert.wHiBOOST =0;
            gLEDAlert.wLowBOOST =0;
            gLEDAlert.bBOOST_Warning=0;

            msLED_DimmingEnable(_ENABLE,0xFFFF);

            //msWriteByteMask(REG_0A83, 0, BIT0);
        }
    }
}
//==============================================================================
void msLED_Hander(void)
{
    //BYTE temp=1; //20110302
    //LED_printData("\n\rSymState===%d",u8SymState);
#if ENABLE_LED_INT
    #if LED_DEBUG_PROTECTION
        if (bEnterShort_Interrput==TRUE)
        {
            LED_printData("1111 SHORT_MASK_27===%x",wINT_CHECK_4F4E);
            LED_printData("2222 SHORT_STATUS_2D===%x",wINT_CHECK_5B5A);
            LED_printData("3333 BSTCAL_EN_34===%x",wINT_CHECK_6968);
            LED_printData("4444 DIM_ENABLE_40===%x",wINT_CHECK_8180);
            LED_printData("5555 Which STRING is SHORT===%x",wINT_CHECK_5B5A&0x00F);
            bEnterShort_Interrput=FALSE;
            //LED_printData("AAAAAAAAAAAAAAAAAa opencheck===%x",opencheck);
			
        }
        else if (bEnterOpen_Interrput==TRUE)
        {
            LED_printData("1111 OPEN_MASK_29===%x",wINT_CHECK_5352);
            LED_printData("2222 OPEN_STATUS_2F===%x",wINT_CHECK_5F5E);
            LED_printData("3333 BSTCAL_EN_34===%x",wINT_CHECK_6968);
            LED_printData("4444 DIM_ENABLE_40===%x",wINT_CHECK_8180);
            LED_printData("5555 Which STRING is OPEN===%x",wINT_CHECK_5F5E&0x00F);
            bEnterOpen_Interrput=FALSE;

            //LED_printData("BBBBBBBBBBBBBBBBBBBB opencheck===%x",opencheck);
        }
    #endif
#endif

    switch (u8SymState)
    {
        case eLEDSTATE_INITIAL:  //0
            msLED_Init();
            gLEDInfo.bPREPARE_Delay  = 3;                    // 3;    (Sec)
            u8SymState = eLEDSTATE_PREPARE;
            //printMsg("eLEDSTATE_INITIAL");

            break;

        case eLEDSTATE_PREPARE: // 1
            #if 0//def _OPEN_INTERRUPT_ENABLE_
            if(gLEDInfo.bPREPARE_Delay ==2)
            {
                msLED_CheckStringOpen();
            }
            #endif
            if (gLEDInfo.bPREPARE_Delay ==0)
            {
            //printMsg("eLEDSTATE_PREPARE");

                msLED_ClearFlag(); // clear LED&SAR INT Flag
                msLED_ReloadBoostPWM();                // update flag
                #if 0//ENABLE_LED_INT
                Init_LEDIRQEnable(TRUE);                   //  TRUE  FALSE
                #endif
                gLEDInfo.bMEASURE_Delay =1;               //  1(Sec)
                u8SymState = eLEDSTATE_MEASURE;
            }
            break;

        case eLEDSTATE_MEASURE:    // 2
            if (gLEDInfo.bMEASURE_Delay ==0)
            {
            //printMsg("eLEDSTATE_MEASURE");

                if (GET_BIT1(msReadByte(REG_0AC7)))  //[9]: Boost sample data compare finish interrupt flag.
                {
                    msLED_CheckFlag(); // get LED INT Status
                    if (gLEDInfo.bControlModeHW ==HW_Mode)
                    {
                        //BYTE temp=0;
                        //if (temp)
                            msLED_CheckHW();
                    }
                    #if LED_SW_MODE
                    else
                    {
                        msLED_CheckSW();
                    }
                    #endif
                    u8SymState = eLEDSTATE_Check;

        #if LED_AUTO_RECOVER
            if(LEDSatus.Separate.bBoost_Data_Fnish && LEDSatus.Separate.bBoost_Tuning_Finish)
            {
                StringShortOpenChecking = 1;
            }
        #endif
                }
                else
                {
                    u8SymState = eLEDSTATE_MEASURE;
                    break;
                }
            }
            //if (u8SymState != eLEDSTATE_Warn)   //20110302
            //    u8SymState = eLEDSTATE_Check;
            break;

        case eLEDSTATE_NORMAL:     // 3
            //printMsg("eLEDSTATE_NORMAL");

            u8SymState = eLEDSTATE_NORMAL;
            break;

        case eLEDSTATE_Check:          // 4
            //printMsg("eLEDSTATE_Check");

            msLED_CheckAlert();
            msLED_ClearFlag(); // clear SAR & LED INT Flag
            msLED_ReloadBoostPWM();         // update flag
            u8SymState = eLEDSTATE_MEASURE;
            break;

        case eLEDSTATE_Warn:       //5
                    //printMsg("eLEDSTATE_Warn");

            gLEDInfo.bTurnOn_Delay = TIME_TURN_ON_DELAY;
            msLED_BoostEnable(_DISABLE);
            msLED_DimmingEnable(_DISABLE,0xFFFF);
            LED_printMsg("=========System Restert=========");
            u8SymState = eLEDSTATE_Reset;
            break;

        case eLEDSTATE_Reset:          // 6
                    //printMsg("eLEDSTATE_Reset");

            msLED_CheckAlert();
            if (gLEDInfo.bTurnOn_Delay == 0)
                u8SymState = eLEDSTATE_INITIAL;
            break;
#if 1 //20110302
        case eLEDSTATE_Off: // 7
            //msLED_Off(); //20110725
            break;
#endif
    }
}

//==============================================================================
void msLED_Off(void)
{
    LED_printMsg("\r\n  ========msLED_Off=========");

#if ENABLE_LED_OPEN_SHORT_DEBOUNCE
    bLEDOpenShortFlag = 0;
#endif
    
#if ENABLE_LED_INT
    Init_LEDIRQEnable(FALSE);
#endif

    msLED_DimmingEnable(_DISABLE,0xFFFF);
    u8SymState = eLEDSTATE_Off;
	
}

void msLED_On(void)
{
    LED_printMsg("\r\n =========msLED_On=======");

#if ENABLE_LED_OPEN_SHORT_DEBOUNCE
    LEDOpenShortCounter = 0;
    LEDOpenShortString = 0; // bit7~4: LED 3~0 open, bit3~0: LED 3~0 short
    bLEDOpenShortFlag = 0;
    LEDShutDownCounter = 0;
#endif
    
#if Enable_MultiSensing||LED_CTRL_BRIGHTNESS_BY_CURRENT
	msWriteByteMask(REG_0A96,0x0B,0x1F);						// Boost  Settle time  N // delay time for boost stable
	msWriteByteMask(REG_0A97,0x06,0x07);						// Boost  Settle time  M// delay time for boost stable
	BoostReadyCounter=EnableMultiSensingAfterBoostReady;
#endif	

#if LED_CTRL_BRIGHTNESS_BY_CURRENT
    BrightnessOSDSettingToCurrentSetting(LED_DISPLAY_CURRENT_SETTING);
#endif

    msLED_DimmingEnable(_ENABLE,0xFFFF);

    #if 0//CHIP_ID==CHIP_TSUMC
        msWriteByteMask(REG_0AE1,BIT6|BIT7,BIT6|BIT7);
        msWriteByteMask(REG_0AF1,BIT6|BIT4,BIT6|BIT4);
    #endif

#if ENABLE_REDUCE_BOOST_VOLTAGE
    msWriteByteMask(REG_0A95, 0, BIT6);             //back to HW mode
#endif
    
    u8SymState = eLEDSTATE_MEASURE;//eLEDSTATE_INITIAL;
    #if ENABLE_LED_INT
        Init_LEDIRQEnable(TRUE);
    #endif
}

void LEDProcess_10mS(void)
{
        if(LightBarCounter)
        {
            if(LightBarCounter > 10)
            {
                LightBarCounter -= 10;
            }
            else
            {
                LightBarCounter = 0;

    #if OCP_INTERRUPT
                msWriteByteMask(REG_0AC4,   BIT6        ,BIT6); // clear OCP interrupt status
                msWrite2ByteMask(REG_0AC0,   0   ,BIT6); //enable OCP interrupt
    #endif
            }
        }
        
    #if Enable_MultiSensing||LED_CTRL_BRIGHTNESS_BY_CURRENT
        if(BoostReadyCounter)
        {
            if(BoostReadyCounter > 10)
            {
                BoostReadyCounter -= 10;
            }
            else
            {
                BoostReadyCounter = 0;
                msWriteByteMask(REG_0A96, 0, 0x1F); // Boost  Settle time  N // delay time for boost stable
                msWriteByteMask(REG_0A97, 0x06, 0x07); // Boost  Settle time  M// delay time for boost stable
#if Enable_MultiSensing 	
                msWriteByteMask(REG_0ABF, BIT7, BIT7); // enable multi sensing
#endif
            }
        }
    #endif

#if ENABLE_LED_OPEN_SHORT_DEBOUNCE
    LEDOpenShortDebounceProcess();
#endif
}

void LEDProcess_1S(void)
{
    if (gLEDInfo.bPREPARE_Delay)
        gLEDInfo.bPREPARE_Delay--;
    if (gLEDInfo.bMEASURE_Delay)
        gLEDInfo.bMEASURE_Delay --;
    if (gLEDInfo.bTurnOn_Delay)
        gLEDInfo.bTurnOn_Delay--;

#if LED_AUTO_RECOVER
    LEDAutoRecoverProcess();
#endif

#if ENABLE_LED_INT_DELAY
    LEDIntDelayProcess();
#endif
}

#if LED_AUTO_RECOVER
void msLED_InitRecoveryVariables(void)
{
    StringShortOpenChecking = 0;
    StringShortOpenFlag = 0;
    StringRecoverCounterFlag = 0;
    StringRebootFlag = 0;
    StringErrorCounter = 0;
    StringRecoverCounter = 0;
    StringRebootNormalCounter = 0;
}

void msLED_FastReboot(void)
{
    if(InputTimingChangeFlag)
        return;

#if UseINT
    mStar_EnableModeChangeINT(FALSE);
#endif

#if ENABLE_LED_INT
    Init_LEDIRQEnable(FALSE);
#endif

    LED_printMsg(">>>>>>>>>>>>>>>msLED_FastReboot");
    msLED_DimmingEnable(_ENABLE,0xFFFF);
#if ENABLE_LED_INT
    Init_LEDIRQEnable(TRUE);
#endif

    StringRebootFlag = 1;
    StringRebootNormalCounter = 0;
	
#if UseINT
    mStar_EnableModeChangeINT(TRUE);
#endif

}

void LEDAutoRecoverProcess(void)
{
    if(StringShortOpenFlag)
    {
        StringShortOpenFlag = 0;
        StringRebootNormalCounter = 0;
        StringRecoverCounter = 0; // reset recover counter when short/open occurs

        if(StringErrorCounter < STRING_ERROR_COUNT)
        {
            StringRecoverCounterFlag = 1;
        }
    }

    if(StringRecoverCounterFlag)
    {
        if(StringRecoverCounter < STRING_ERROR_RECOVER_PERIOD)
        {
            StringRecoverCounter++;
        }
        else
        {
            StringRecoverCounterFlag = 0;
            StringRecoverCounter = 0;
            StringErrorCounter ++;
            msLED_FastReboot();
        }
    }

    if(StringRebootFlag)
    {
        if(StringRebootNormalCounter < STRING_REBOOT_NORMAL_PERIOD)
        {
            StringRebootNormalCounter++;
        }
        else
        {
            StringRebootFlag = 0;
            StringErrorCounter = 0;
        }
    }
}
#endif

#if ENABLE_LED_OPEN_SHORT_DEBOUNCE
void LEDDisableOpenShortString(BYTE ch)
{
    LED1_printData("LEDShutDownString:%x", ch);
    
#if LED_CH_0
    #if ENABLE_DETECT_LED_CH_0
    if(_bit0_(ch)) // string0 short
    {
        if(ENABLE_4_STRING_WITH_CH0_DETECT) // enable ch0~3, use ch0 to detect
        {
            msRegs[REG_0A80] = 0x00; // disable all dimming
            msRegs[REG_0A68] = 0x00; // disable all boost ch 
        #if LED_SAR_VOLTAGE_MEASURE_ENABLE
            msRegs[REG_0AD0] = 0x00; // disable all sar
        #endif
            msRegs[REG_0A4E] = 0xFF;   // enable all interrupt mask
            
            LED1_printData("short dis ch0123:%x", msRead2Byte(REG_0A20)&0x3FF);
        }
        else if(ENABLE_4_STRING_WITH_CH0_CH2_DETECT) // enable ch0~1, use ch0 to detect
        {
            msRegs[REG_0A80] &= (~(BIT0|BIT1)); // disable dimming0, 1
            msRegs[REG_0A68] &= (~(BIT4|BIT5)); // disable boost ch0, 1 
        #if LED_SAR_VOLTAGE_MEASURE_ENABLE
            msRegs[REG_0AD0] &= (~(BIT0|BIT4|BIT1|BIT5)); // disable sar0, 1
        #endif
            msRegs[REG_0A4E] |= (BIT0|BIT1);   // enable interrupt mask ch0, 1
            
            LED1_printData("short dis ch01:%x", msRead2Byte(REG_0A20)&0x3FF);
        }
        else // enable ch0, use ch0 to detect
        {
            msRegs[REG_0A80] &= (~BIT0); // disable dimming
            msRegs[REG_0A68] &= (~BIT4); // disable boost ch 
        #if LED_SAR_VOLTAGE_MEASURE_ENABLE
            msRegs[REG_0AD0] &= (~(BIT0|BIT4)); // disable sar
        #endif
            msRegs[REG_0A4E] |= BIT0;   // enable interrupt mask
            
            LED1_printData("short dis ch0:%x", msRead2Byte(REG_0A20)&0x3FF);
        }
    }

    if(_bit4_(ch)) // string0 open
    {
        if(ENABLE_4_STRING_WITH_CH0_DETECT) // enable ch0~3, use ch0 to detect
        {
            msRegs[REG_0A80] = 0x00; // disable all dimming
            msRegs[REG_0A68] = 0x00; // disable all boost ch 
        #if LED_SAR_VOLTAGE_MEASURE_ENABLE
            msRegs[REG_0AD0] = 0x00; // disable all sar
        #endif
            msRegs[REG_0A52] = 0xFF;   // enable all interrupt mask
            
            LED1_printData("open dis ch0123:%x", msRead2Byte(REG_0A28)&0x3FF);
        }
        else if(ENABLE_4_STRING_WITH_CH0_CH2_DETECT) // enable ch0~1, use ch0 to detect
        {
            msRegs[REG_0A80] &= (~(BIT0|BIT1)); // disable dimming0, 1
            msRegs[REG_0A68] &= (~(BIT4|BIT5)); // disable boost ch0, 1 
        #if LED_SAR_VOLTAGE_MEASURE_ENABLE
            msRegs[REG_0AD0] &= (~(BIT0|BIT4|BIT1|BIT5)); // disable sar0, 1
        #endif
            msRegs[REG_0A52] |= (BIT4|BIT5);   // enable interrupt mask ch0, 1
            
            LED1_printData("open dis ch01:%x", msRead2Byte(REG_0A28)&0x3FF);
        }
        else // enable ch0, use ch0 to detect
        {
            msRegs[REG_0A80] &= (~BIT0);
            msRegs[REG_0A68] &= (~BIT4);
        #if LED_SAR_VOLTAGE_MEASURE_ENABLE
            msRegs[REG_0AD0] &= (~(BIT0|BIT4));
        #endif
            msRegs[REG_0A52] |= BIT4;   // enable interrupt mask
            
            LED1_printData("open dis ch0:%x", msRead2Byte(REG_0A28)&0x3FF);
        }
    }
    
    #else
    if(_bit0_(ch)) // string0 short
    {
        msRegs[REG_0A80] &= (~BIT0); // disable dimming
        msRegs[REG_0A68] &= (~BIT4); // disable boost ch 
    #if LED_SAR_VOLTAGE_MEASURE_ENABLE
        msRegs[REG_0AD0] &= (~(BIT0|BIT4)); // disable sar
    #endif
        msRegs[REG_0A4E] |= BIT0;   // enable interrupt mask
        
        LED1_printData("short dis ch0:%x", msRead2Byte(REG_0A20)&0x3FF);
    }

    if(_bit4_(ch)) // string0 open
    {
        msRegs[REG_0A80] &= (~BIT0);
        msRegs[REG_0A68] &= (~BIT4);
    #if LED_SAR_VOLTAGE_MEASURE_ENABLE
        msRegs[REG_0AD0] &= (~(BIT0|BIT4));
    #endif
        msRegs[REG_0A52] |= BIT4;   // enable interrupt mask
        
        LED1_printData("open dis ch0:%x", msRead2Byte(REG_0A28)&0x3FF);
    }
    #endif
#endif

#if LED_CH_1
    if(_bit1_(ch)) // string1 short
    {
        msRegs[REG_0A80] &= (~BIT1); 
        msRegs[REG_0A68] &= (~BIT5);
    #if LED_SAR_VOLTAGE_MEASURE_ENABLE
        msRegs[REG_0AD0] &= (~(BIT1|BIT5));
    #endif
        msRegs[REG_0A4E] |= BIT1;
    
        LED1_printData("short dis ch1:%x", msRead2Byte(REG_0A22)&0x3FF);
    }

    if(_bit5_(ch)) // string1 open
    {
        msRegs[REG_0A80] &= (~BIT1);
        msRegs[REG_0A68] &= (~BIT5);
    #if LED_SAR_VOLTAGE_MEASURE_ENABLE
        msRegs[REG_0AD0] &= (~(BIT1|BIT5));
    #endif
        msRegs[REG_0A52] |= BIT5;
    
        LED1_printData("open dis ch1:%x", msRead2Byte(REG_0A2A)&0x3FF);
    }
#endif

#if LED_CH_2
    #if ENABLE_DETECT_LED_CH_2
    if(_bit2_(ch)) // string2 short
    {
        if(ENABLE_4_STRING_WITH_CH0_CH2_DETECT) // enable ch2~3, use ch2 to detect
        {
            msRegs[REG_0A80] &= (~(BIT2|BIT3)); // disable dimming2, 3
            msRegs[REG_0A68] &= (~(BIT6|BIT7)); // disable boost ch2, 3
        #if LED_SAR_VOLTAGE_MEASURE_ENABLE
            msRegs[REG_0AD0] &= (~(BIT2|BIT6|BIT3|BIT7)); // disable sar2, 3
        #endif
            msRegs[REG_0A4E] |= (BIT2|BIT3); // enable interrupt mask ch2, 3
        
            LED1_printData("short dis ch23:%x", msRead2Byte(REG_0A24)&0x3FF);
        }
        else // enable ch2, use ch2 to detect
        {
            msRegs[REG_0A80] &= (~BIT2);
            msRegs[REG_0A68] &= (~BIT6);
        #if LED_SAR_VOLTAGE_MEASURE_ENABLE
            msRegs[REG_0AD0] &= (~(BIT2|BIT6));
        #endif
            msRegs[REG_0A4E] |= BIT2;
        
            LED1_printData("short dis ch2:%x", msRead2Byte(REG_0A24)&0x3FF);
        }
    }

    if(_bit6_(ch)) // string2 open
    {
        if(ENABLE_4_STRING_WITH_CH0_CH2_DETECT) // enable ch2~3, use ch2 to detect
        {
            msRegs[REG_0A80] &= (~(BIT2|BIT3)); // disable dimming2, 3
            msRegs[REG_0A68] &= (~(BIT6|BIT7)); // disable boost ch2, 3
        #if LED_SAR_VOLTAGE_MEASURE_ENABLE
            msRegs[REG_0AD0] &= (~(BIT2|BIT6|BIT3|BIT7)); // disable sar2, 3
        #endif
            msRegs[REG_0A52] |= (BIT6|BIT7); // enable interrupt mask ch2, 3
        
            LED1_printData("open dis ch23:%x", msRead2Byte(REG_0A2C)&0x3FF);
        }
        else // enable ch2, use ch2 to detect
        {
            msRegs[REG_0A80] &= (~BIT2);
            msRegs[REG_0A68] &= (~BIT6);
        #if LED_SAR_VOLTAGE_MEASURE_ENABLE
            msRegs[REG_0AD0] &= (~(BIT2|BIT6));
        #endif
            msRegs[REG_0A52] |= BIT6;
        
            LED1_printData("open dis ch2:%x", msRead2Byte(REG_0A2C)&0x3FF);
        }
    }
    #else
    if(_bit2_(ch)) // string2 short
    {
        msRegs[REG_0A80] &= (~BIT2);
        msRegs[REG_0A68] &= (~BIT6);
    #if LED_SAR_VOLTAGE_MEASURE_ENABLE
        msRegs[REG_0AD0] &= (~(BIT2|BIT6));
    #endif
        msRegs[REG_0A4E] |= BIT2;
    
        LED1_printData("short dis ch2:%x", msRead2Byte(REG_0A24)&0x3FF);
    }

    if(_bit6_(ch)) // string2 open
    {
        msRegs[REG_0A80] &= (~BIT2);
        msRegs[REG_0A68] &= (~BIT6);
    #if LED_SAR_VOLTAGE_MEASURE_ENABLE
        msRegs[REG_0AD0] &= (~(BIT2|BIT6));
    #endif
        msRegs[REG_0A52] |= BIT6;
    
        LED1_printData("open dis ch2:%x", msRead2Byte(REG_0A2C)&0x3FF);
    }
    #endif
#endif

#if LED_CH_3
    if(_bit3_(ch)) // string3 short
    {
        msRegs[REG_0A80] &= (~BIT3);
        msRegs[REG_0A68] &= (~BIT7);
    #if LED_SAR_VOLTAGE_MEASURE_ENABLE
        msRegs[REG_0AD0] &= (~(BIT3|BIT7));
    #endif
        msRegs[REG_0A4E] |= BIT3;
    
        LED1_printData("short dis ch3:%x", msRead2Byte(REG_0A26)&0x3FF);
    }

    if(_bit7_(ch)) // string3 open
    {
        msRegs[REG_0A80] &= (~BIT3);
        msRegs[REG_0A68] &= (~BIT7);
    #if LED_SAR_VOLTAGE_MEASURE_ENABLE
        msRegs[REG_0AD0] &= (~(BIT3|BIT7));
    #endif
        msRegs[REG_0A52] |= BIT7;

        LED1_printData("open dis ch3:%x", msRead2Byte(REG_0A2E)&0x3FF);
    }
#endif
}

void LEDStringEnableInterrupt(BYTE ch)
{
#if LED_CH_0
    if(_bit0_(ch)) // string0 short
    {
        msRegs[REG_0A48] |= BIT0;
        msRegs[REG_0A4E] &= (~BIT0);
    }

    if(_bit4_(ch)) // string0 open
    {
        msRegs[REG_0A4C] |= BIT4;
        msRegs[REG_0A52] &= (~BIT4);
    }
#endif

#if LED_CH_1
    if(_bit1_(ch))// string1 short
    {
        msRegs[REG_0A48] |= BIT1;
        msRegs[REG_0A4E] &= (~BIT1);
    }

    if(_bit5_(ch)) // string1 open
    {
        msRegs[REG_0A4C] |= BIT5;
        msRegs[REG_0A52] &= (~BIT5);
    }
#endif

#if LED_CH_2
    if(_bit2_(ch)) // string2 short
    {
        msRegs[REG_0A48] |= BIT2;
        msRegs[REG_0A4E] &= (~BIT2);
    }

    if(_bit6_(ch)) // string2 open
    {
        msRegs[REG_0A4C] |= BIT6;
        msRegs[REG_0A52] &= (~BIT6);
    }
#endif

#if LED_CH_3
    if(_bit3_(ch)) // string3 short
    {
        msRegs[REG_0A48] |= BIT3;
        msRegs[REG_0A4E] &= (~BIT3);
    }

    if(_bit7_(ch)) // string3 open
    {
        msRegs[REG_0A4C] |= BIT7;
        msRegs[REG_0A52] &= (~BIT7);
    }
#endif

    bLEDOpenShortIntEnFlag = 1;
}

void LEDOpenShortDebounceProcess(void)    
{
    if(!bLEDOpenShortFlag)
        return;
    
    if(LEDShutDownCounter < LED_SHUT_DOWN_COUNT)
    {
        LEDShutDownCounter++;

        if(LEDShutDownCounter >= (LED_SHUT_DOWN_COUNT/2) && (LEDOpenShortString&0xF0))
        {
            if(LED_BOOST_VOLTAGE_REACH_PANEL_MAX1P1_VOLTAGE())
            {
                
                LEDDisableOpenShortString(LEDOpenShortString&0xF0);
                LEDBoostDutyCtrl(LED_DISPLAY_CURRENT_SETTING);
                msWriteByteMask(REG_0A95, BIT6, BIT6); //sw mode
                msWriteByteMask(REG_0A85, BIT1, BIT1); //trigger
                ForceDelay1ms(10);
                msWriteByteMask(REG_0A95, 0, BIT6); //hw mode
                LEDShutDownCounter = 0; // reset shut down counter
            }
        }
        
    }
    else // turn off open/short string after LED_SHUT_DOWN_COUNT*10mS
    {
        // disable open/short string
        LEDDisableOpenShortString(LEDOpenShortString);
        LEDOpenShortString = 0;
        bLEDOpenShortFlag = 0;
        LEDShutDownCounter = 0;
    }
}
#endif

#if ENABLE_LED_INT_DELAY
void LEDIntDelayProcess(void)
{
    if(LEDINTEnableCounter)
    {
        LEDINTEnableCounter--;
        if(!LEDINTEnableCounter)
        {
            LEDINTEnableFlag = 1;
            Init_LEDIRQEnable(TRUE);
        }
    }
}
#endif

#if ENABLE_REDUCE_BOOST_VOLTAGE || ENABLE_LED_OPEN_SHORT_DEBOUNCE
void LEDBoostDutyCtrl(brightness)
{
#if LED_CTRL_BRIGHTNESS_BY_CURRENT
    if(brightness < 25)
    {
        LED_SET_INIT_BOOST_DUTY_14();
    }
    else if(brightness < 50)
    {
        LED_SET_INIT_BOOST_DUTY_24();
    }
    else if(brightness < 75)
    {
        LED_SET_INIT_BOOST_DUTY_34();
    }
    else
    {
        LED_SET_INIT_BOOST_DUTY();
    }
#else
    brightness = brightness;
    LED_SET_INIT_BOOST_DUTY();
#endif

    msWriteByteMask(REG_0A95, BIT6, BIT6);          //sw mode
    msWriteByteMask(REG_0A85, BIT1, BIT1);          //trigger
}
#endif

#else
BYTE code msLEDControllerNullData[] = {0};
void msLEDControllerDummy()
{
    BYTE xdata x = msLEDControllerNullData[0];
}

#endif
