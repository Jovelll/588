#include "types.h"
#include "board.h"
#include "global.h"
#include "mcu.h"
#include "detect.h"
#include "menu.h"
#include "ms_reg.h"
#include "power.h"
#include "debug.h"
#include "Common.h"
#include "ms_rwreg.h"
#include "msosd.h"
#include "misc.h"
#include "NVRam.h"
#include "mstar.h"
#include "UserPref.h"
#include "keypad.h"
//#include "Panel.h"
#include "msflash.h"  //2006-03-20
#include "ddc2bi.h"
#include "gpio_def.h"
#if ENABLE_RTE
#include "drvmsovd.h"
#endif
#if ENABLE_DP_INPUT
#include "drvDPRxApp.h"
#endif

#if (MS_PM)
#include "Ms_PM.h"
#endif
#if DDCCI_ENABLE||AudioFunc
#include "adjust.h"
#endif
#include "MsDLC.h"
#include "MsACE.h"

#if ENABLE_HDMI
#include "MsHDMI.h"
#if ENABLE_CEC
#include "apiCEC.h"
#endif
#endif
#if ENABLE_EDP_OUTPUT
#include "DPTxApp.h"
#endif

#if MS_DAC
#include "drvDAC.h"
#endif

#if ENABLE_LED_CONTROLLER
#include "LEDControl.h"
#endif

#include "DDC.h"

////////////////////////////////////
//#include "drvMcu.h"
///////////////////////////////////

#if ENABLE_DPS
#include "msDPS_Handler.h"
#include "userprefdef.h"
#include "msdps_setting.h"
#endif
#if ENABLE_DSC
#include "msDSC_Handler.h"
//#include "userprefdef.h"
#include "msdsc_setting.h"
#endif

#define Main_DEBUG    1
#if ENABLE_DEBUG&&Main_DEBUG
    #define Main_printData(str, value)   printData(str, value)
    #define Main_printMsg(str)           printMsg(str)
#else
    #define Main_printData(str, value)
    #define Main_printMsg(str)
#endif
#if EN_HotKeyTimes
extern BYTE xdata HotKeyTimes_Minutes;
extern BYTE xdata HotKeyTimes_Second;

#endif
#if ENABLE_FACTORY_AUTOTEST
extern bit BFactoryAutoTest;
#endif

void Init_Device( void );
void Main_SlowTimerHandler(void);
#if EarphoneDet_Enable
void HarphoneDetFunc(void);
#endif


extern void Init_GlobalVariables( void );
extern void i2C_Intial( void ); //2006/03/20
#if ENABLE_HDCP&&HDCPKEY_IN_Flash
extern void msInitHDCPProductionKey( void );
#endif
extern Bool ResetAllSetting( void );

extern BYTE xdata PowerDownCounter;

BYTE xdata mainloop = 0;

typedef unsigned int  __u16;    // 2 bytes
typedef unsigned char   __u8;     // 1 byte

#ifndef U8
#define U8    BYTE
#endif

#ifndef U32
#define U32    DWORD
#endif

#ifndef BOOLEAN
#define BOOLEAN    BYTE
#endif
//extern __u8*  MDrv_Get_HID_Report_Raw_Data(void);
//extern U8 drvUSBHost_HID_Initial(void);
extern U8 drvUSBHost_HID_Initial_Port2(U8 u8Interface);
extern void MDrv_USB_Init(DWORD USBAdr);
extern void MDrv_USB_Init_Port2(DWORD USBAdr);
extern BOOLEAN  MDrv_Usb_Device_Enum(void);
extern BOOLEAN  MDrv_Usb_Device_Enum_Port2(void);
extern BOOLEAN MDrv_UsbDeviceConnect(void);
extern BOOLEAN MDrv_UsbDeviceConnect_Port2(void);
extern void drvUSBHost_UTMIInitial();
extern void  MDrv_Get_Keyboard_Staus(U8 u8Interface);
extern void  MDrv_Get_Mouse_Staus(U8 u8Interface);
extern void  MDrv_Get_Mouse_Staus_Port2(U8 u8Interface);
extern void  MDrv_Get_Keyboard_Staus_Port2(U8 u8Interface);
#if CHIP_ID == CHIP_TSUMU
extern void msInitInternalEDID(void);
#endif

#define PORT
#define PORT2
#define HOST20_INTERFACE_NUM_MAX    5
extern U8 xdata PORT_DEVICE[HOST20_INTERFACE_NUM_MAX];//support 5 interface
extern U8 xdata PORT2_DEVICE[HOST20_INTERFACE_NUM_MAX];

#if ENABLE_FREESYNC
extern xdata Bool bHDMIFreesyncChk;
#endif

BYTE code msMainNullData[] = {0};
#if ENABLE_POWER_PORTECT
 static unsigned int max_current;
float xdata  voltmp[256]={
	 0.000,0.005,0.008,0.013,0.015,0.020,0.025,0.028,0.030,0.035,0.038,0.043,0.045,0.051,0.056,0.058,
	 0.063,0.066,0.071,0.073,0.076,0.081,0.083,0.088,0.091,0.096,0.098,0.104,0.106,0.111,0.116,0.119,
	 0.124,0.126,0.131,0.134,0.139,0.141,0.146,0.149,0.154,0.159,0.162,0.167,0.169,0.174,0.177,0.182,
	 0.184,0.189,0.194,0.197,0.202,0.205,0.210,0.212,0.217,0.222,0.225,0.230,0.232,0.237,0.240,0.245,
	 0.250,0.253,0.258,0.260,0.265,0.268,0.273,0.275,0.280,0.283,0.288,0.290,0.295,0.301,0.303,0.308,
	 0.311,0.316,0.321,0.323,0.328,0.333,0.336,0.341,0.343,0.348,0.351,0.356,0.361,0.364,0.369,0.374,
	 0.376,0.381,0.384,0.389,0.391,0.396,0.399,0.404,0.409,0.412,0.417,0.422,0.424,0.429,0.432,0.437,
	 0.439,0.444,0.447,0.455,0.457,0.460,0.465,0.470,0.472,0.477,0.480,0.485,0.487,0.492,0.497,0.500,
	 0.505,0.508,0.513,0.515,0.520,0.525,0.528,0.533,0.538,0.540,0.545,0.548,0.553,0.558,0.561,0.566,
	 0.571,0.573,0.578,0.581,0.586,0.588,0.593,0.596,0.601,0.606,0.609,0.614,0.619,0.621,0.626,0.631,
	 0.634,0.636,0.641,0.644,0.649,0.654,0.657,0.662,0.664,0.669,0.674,0.677,0.682,0.684,0.687,0.692,
	 0.697,0.699,0.705,0.707,0.712,0.717,0.720,0.725,0.727,0.732,0.737,0.740,0.742,0.747,0.753,0.755,
	 0.760,0.763,0.768,0.770,0.775,0.778,0.783,0.788,0.790,0.795,0.798,0.803,0.806,0.811,0.813,0.816,
	 0.821,0.826,0.828,0.833,0.838,0.841,0.846,0.848,0.854,0.856,0.861,0.864,0.869,0.871,0.876,0.879,
	 0.884,0.886,0.889,0.894,0.896,0.902,0.907,0.909,0.914,0.917,0.919,0.922,0.929,0.932,0.934,0.939,
	 0.944,0.947,0.952,0.955,0.960,0.962,0.965,0.970,0.972,0.977,0.980,0.985,0.987,0.992,0.995,1.000,
 
	};
U8 getCurrentPWM(unsigned int RealCurrent)
	   {
		   U8 i;
		   
		   if (RealCurrent > MAX_CURRENT_VALUE)
		   {
			   RealCurrent = MAX_CURRENT_VALUE;
		   }
		   for (i = 0; i < 255; i++)
		   {
			   if (RealCurrent < (unsigned int)(voltmp[i]*MAX_CURRENT_VALUE)) //
			   {
				   break;
			   }
		   }
	   
		   return i;	
	   }
						   
 unsigned int getRealCurrent(U8 pwm)
    {
    
	  return (unsigned int)(MAX_CURRENT_VALUE* voltmp[pwm]); // 
	}
							

unsigned int getVoltage(void)
{
    U8 i;
    U8 adc = 0;
    U32 voltage = 0;
    U32 sum = 0;

    // 平均法滤波
    for (i = 0; i < 16; i++)
    {
        adc=LED_ADC_C;
        sum += adc;
    }
    sum = sum >> 4;
	
       voltage = (1086 * sum) >> 8;
    
    return (unsigned int)voltage;
}


void PowerProtectInit(void)
{
    U8 init_current_pwm = 0;

	if(UserPrefBrightness>50) //DefBrightness
		 {
			// brightness=((DWORD)(brightness-DefBrightness)*(MaxDutyValue-MidDutyValue))/(MaxBrightnessValue-DefBrightness)+MidDutyValue;
			 
			init_current_pwm=((DWORD)(UserPrefBrightness-50)*(MaxDutyValue-MidDutyValue))/50+MidDutyValue;
		 }
	 else
		 {
			// brightness=((DWORD)(brightness)*(MidDutyValue-MinDutyValue))/DefBrightness+MinDutyValue;			
			init_current_pwm=((DWORD)(UserPrefBrightness)*(MidDutyValue-MinDutyValue))/50+MinDutyValue;
		 }
   max_current = getRealCurrent(init_current_pwm);
}

void PowerProtectTask(void)
	{
		unsigned int voltage;
		BYTE Backlightpwm = 128;//U8 i;
		U32 max_watts = 0;
		Bool b_BLchange=FALSE;
	
			voltage = getVoltage();
			
		//	printData("\n voltage is %d \n",voltage);
			
      #ifdef PowerProtect_v4			
           if(voltage>PowerProtect_v4* 10)
           	{
			   max_watts=PowerProtect_w4;
           	}else 
     #endif
    #ifdef PowerProtect_v3	  
			if(voltage>PowerProtect_v3* 10)
           	{
			   max_watts=PowerProtect_w3;
           	}else
    #endif
   #ifdef PowerProtect_v2	  
			if(voltage>PowerProtect_v2* 10)
           	{
			   max_watts=PowerProtect_w2;
           	}else
    #endif
  #ifdef PowerProtect_v1	  
			if(voltage>PowerProtect_v1* 10)
           	{
			   max_watts=PowerProtect_w1;
           	}else
   #endif
			if(voltage>PowerProtect_v0* 10)
           	{
			   max_watts=PowerProtect_w0;
           	}	

		//	printData("\n max_watts is %d \n",max_watts);
			// 电压放大10倍   电流放大1000倍
	
	
			 if(voltage==0)
				{
				// printf("error voltage shoule not 0,pleas check it ");
				 return;
	
				}
			 
			while (max_current > ((unsigned long)(max_watts * 10000) / voltage))
			{
				max_current--;
				b_BLchange=TRUE;
			}
	
	
			//printData("\n max_current is %d \n",max_current);
			//printData("\n UserPrefBrightness is %d \n",UserPrefBrightness);
      if(b_BLchange==TRUE)
      	{
	  Backlightpwm=getCurrentPWM((unsigned int)((max_current * UserPrefBrightness) / 100));
	   //printData("\n BacklightNow is %d \n",Backlightpwm);

       #if BRIGHTNESS_INVERSE	//+Duty power
       			drvGPIO_SetBacklightDuty(BrightnessPWM, 0xFF-Backlightpwm);
       #else							//-Duty power
       			drvGPIO_SetBacklightDuty(BrightnessPWM, Backlightpwm);
       #endif
      	}
		
	}
#endif

void msMainDummy(void)
{
    BYTE xdata i = msMainNullData[0];
}



void main( void )
{
    BYTE UsbConnect = 0, UsbConnect_Port2 = 0;

#if (CHIP_ID == CHIP_TSUM9 ||CHIP_ID == CHIP_TSUMF)
    drvmStar_PowerOnInit();
#if (CHIP_ID == CHIP_TSUMF)
    MPLL_CLOCK_ADC(TRUE);
#endif
Main_printMsg("!!!!!!1111");
#else
    #if (MS_PM)
    msPM_Exit();    // for WDT reset in PM mode
    #endif
#endif

#if ENABLE_DP_INPUT
    g_bDoDPInit = TRUE;
#endif
    Init_MCU();
Main_printMsg("!!!!!!2222");
#if (CHIP_ID == CHIP_TSUM9||CHIP_ID == CHIP_TSUMF)
	if (!g_bFROTrimResult)
		Main_printMsg("FRO trim failed!");
#endif

#if( ENABLE_WATCH_DOG )
    WDT_CLEAR();
#endif
Main_printMsg("!!!!!!3333");
    ScalerReset();
    mcuSetSystemSpeed(SPEED_NORMAL_MODE);
    ForceDelay1ms( 100 );
    g_ucFlashID = ReadFlashID();

#if !USEFLASH
    i2C_Intial();
#endif
Main_printMsg("!!!!!!4444");
    ReadMonitorSetting();
#ifdef Internal_EDID_Write_To_24C02
		  if(!LoadHDMIEDIDFlag)
		  Mstar_Init_VGA_DVI_HDMI_24C02_EDID();
#endif
Main_printMsg("!!!!!!5555");
    mStar_Init();
    mStar_ACOnInit();
#if  (MS_PM)
    msPM_Init();
#endif
Main_printMsg("!!!!!!6666");
#ifdef ENABLE_INTERNAL_HDMIEDID
	msInitInternalEDID();
#endif

    Init_GlobalVariables();
    Menu_InitVariable();
    Init_Device();
    DDC2Bi_Init();

    Main_printData("  g_u8SystemSpeedMode:%d", g_u8SystemSpeedMode);
	
#if ENABLE_USB_INPUT
#ifdef PORT
    MDrv_USB_Init(HK_USB_XDATA_ADDR);
#endif
#ifdef PORT2
    MDrv_USB_Init_Port2(HK_USB_XDATA_ADDR_PORT2);
#endif
#endif

#if ENABLE_POWER_PORTECT

	PowerProtectInit();
#endif
    while( 1 )
    {
    
#if ENABLE_POWER_PORTECT
	  if((Second%2) == 0)
	  	{
	      PowerProtectTask();
	  	}
#endif
#if (MS_PM)
        msPM_Handler();
        if(!msPM_IsState_IDLE())
            continue;
#endif

        Main_SlowTimerHandler();

        DebugHandler();

#if( ENABLE_WATCH_DOG )
        WDT_CLEAR();
#endif

#if ENABLE_DEBUG
        if( DebugOnlyFlag )
            continue;
#endif

        DDC2Bi_CommandHandler();

#if ENABLE_HDCP&&HDCPKEY_IN_Flash
        if( WriteHDCPcodeFlag )
            continue;
#endif

#if ENABLE_HDCP&&HDCPKEY_IN_Flash
        if( LoadHDCPKeyFlag )
        {
            msInitHDCPProductionKey();
            Clr_LoadHDCPKeyFlag();
        }
#endif

#if ENABLE_LED_CONTROLLER
        msLED_Hander();
#endif

#if ENABLE_DP_INPUT
        DPRxHandle();             // DP hander
#endif

        Power_PowerHandler();
#if (MS_PM)
        if(!msPM_IsState_IDLE())
            continue;
#endif

        if( !ModeDetectCounter || InputTimingChangeFlag )
        {
            mStar_MonitorInputTiming();
            ModeDetectCounter = 20;
        }

        mStar_ModeHandler();

#if (ENABLE_HDMI || ENABLE_DVI)
#if(CHIP_ID==CHIP_TSUM9)
        drv_TMDS_DATA_ALIGN();
#endif
#endif

#if ENABLE_HDMI
        mstar_HDMIHandler();
#if ENABLE_CEC
        api_CEC_Handler();
#endif
#endif

#if ENABLE_DPS
        if(bRunToolFlag)
            continue;
        //printMsg( "msDPS_Handler" );
        msDPS_Handler();
#endif
#if ENABLE_DSC
#if !DEBUG_DSC_withoutInterrupt
        //printMsg( "msDSC_Handler" );
        msDSC_Handler();
        //ForceDelay1ms(1000);
#endif
#endif
#if ENABLE_DLC
    xx    msDlcHandler();
#endif

#if AudioFunc
#if ((CHIP_ID==CHIP_TSUMC || CHIP_ID==CHIP_TSUMD || CHIP_ID==CHIP_TSUM9|| CHIP_ID==CHIP_TSUMF) && MS_DAC)
        msDACInitStep();
#endif
#if (MS_DAC)
        msAudioGainToggle();
		#if ENABLE_DAC_DEPOP
        msAudioDACHandler();
		#endif
#endif
#if EarphoneDet_Enable
       HarphoneDetFunc();
#endif
#if AudioIC_CS8003_Enable
	msAudioDPGA_SetVolume ( E_AUDIO_LEFT_RIGHT_CH, 0x00);
	msWrite2ByteMask(  REG_2CA4, 0xFF76    , 0xFF7F);  //[15:8] DPGA_1_TIMEOUT_N; [6:4] DPGA_1_STEP=3'd7->N=1;  [2] DPGA_1_ZERO_DET_EN; [1] DPGA_1_FADING_EN;   [0] DPGA_1_DPGA_EN;
#endif

#endif

        Menu_OsdHandler();
#if ENABLE_EDP_OUTPUT
		//if( !SyncLossState() )
		DPTxHandle();
#endif

#if (MS_PM)
        if(!msPM_IsState_IDLE())
            continue;
#endif

#if MWEFunction
        if(( ColorAutoDetectFlag || UserPrefDcrMode ) && PowerOnFlag && !SyncLossState() && InputTimingStableFlag && ( !DisplayLogoFlag ) )
#else
        if(( UserPrefDcrMode ) && PowerOnFlag && !SyncLossState() && InputTimingStableFlag && ( !DisplayLogoFlag ) )
#endif
        {
            msDlcHandler();
            msDCRHandler();
        }

#if ENABLE_MHL
#if(MHL_HW_RTERM_SUPPORT)
        mapi_mhl_RtermControlHWMode(FALSE);
#endif

        mapi_mhl_handler();

#if(MHL_HW_RTERM_SUPPORT)
        mapi_mhl_RtermControlHWMode(TRUE);
#endif
#endif

#if ENABLE_TOUCH_PANEL||ENABLE_TOUCH_PANEL_CTRL_OSD
        TouchPanelHandler();
#endif

#if USEFLASH
        UserPref_FlashDataSaving();
#endif
#if ENABLE_USB_INPUT
#ifdef PORT
        if (MDrv_UsbDeviceConnect()==1)
        {
            if (UsbConnect == 0)  // do usb device enumeration
            {
                Main_printMsg("USB device plug-in!!\n");
                if (MDrv_Usb_Device_Enum() == 1)
                {
                    UsbConnect = 1;
                    Main_printMsg("1111 USB enumeration success!!\n");
                }
                else
                    Main_printMsg("1111 USB enumeration fail!!\n");
            }
            else    // polling usb device input data, ex. move mouse, press buttoms
            {
                U8 i;
                for(i=0;i<HOST20_INTERFACE_NUM_MAX;i++)
                {
                    if(PORT_DEVICE[i]==1)  //Keyboard
                        MDrv_Get_Keyboard_Staus(i);
                    else if(PORT_DEVICE[i]==2) //Mouse
                        MDrv_Get_Mouse_Staus(i);
                }
            }
        }
        else
        {
            if (UsbConnect == 1)
                Main_printMsg("USB device plug-out!!\n");
            UsbConnect = 0;
        }
#endif
#ifdef PORT2
        if (MDrv_UsbDeviceConnect_Port2()==1)
        {
            if (UsbConnect_Port2 == 0)  // do usb device enumeration
            {
                Main_printMsg("USB device plug-in!!\n");
                if (MDrv_Usb_Device_Enum_Port2() == 1)
                {
                    UsbConnect_Port2 = 1;
                    Main_printMsg("2222 USB enumeration success!!\n");
    }
                else
                    Main_printMsg("2222 USB enumeration fail!!\n");
            }
            else    // polling usb device input data, ex. move mouse, press buttoms
            {
                U8 j;
                for(j=0;j<HOST20_INTERFACE_NUM_MAX;j++)
                {
                    if(PORT2_DEVICE[j]==1)  //Keyboard
                        MDrv_Get_Keyboard_Staus_Port2(j);
                    else if(PORT2_DEVICE[j]==2) //Mouse
                        MDrv_Get_Mouse_Staus_Port2(j);
                }
            }
        }
        else
        {
            if (UsbConnect_Port2 == 1)
                Main_printMsg("USB device plug-out!!\n");
            UsbConnect_Port2 = 0;
        }
#endif
#endif
    }

}

void Init_Device()
{
#if ENABLE_USB_INPUT
    hw_USB_HI();
#endif

#if ENABLE_TOUCH_KEY
    TouchKeyRestortCounter = TouchKey_Init();
#endif

#if AudioFunc
    mStar_AdjustVolume( 0 );
#endif

#if ENABLE_TOUCH_PANEL||ENABLE_TOUCH_PANEL_CTRL_OSD
    TPL_InitSetting();
#endif

    if( PowerOnFlag )
    {
    	#if Enable_LED
			Power_TurnOnAmberLed();
		#else
			Power_TurnOnGreenLed();//�̵�
		#endif
        
    }
    else
    {
    
		Power_TurnOffLed();      
    }
}

void Main_SlowTimerHandler(void)
{
    if(ms10Flag)
    {
        if(ModeDetectCounter)
        {
            if(ModeDetectCounter > 10)
                ModeDetectCounter -= 10;
            else
                ModeDetectCounter = 0;
        }

        if(TPDebunceCounter)
        {
            if(TPDebunceCounter > 10)
            {
                TPDebunceCounter -= 10;
            }
            else
            {
                TPDebunceCounter = 0;
                Set_StartScanKeyFlag();
            }
        }

#if ENABLE_HDMI
        if(HdmiPollingCounter)
        {
            if( HdmiPollingCounter > 10)
                HdmiPollingCounter -= 10;
            else
                HdmiPollingCounter = 0;
        }

        if( gScInfo.InputTmdsType == TMDS_HDMI && ( !( SrcFlags&( SyncLoss | bUnsupportMode ) ) ) )
        {
            if( gScInfo.wAviPktLossCnt < AviPktLossBound )
                gScInfo.wAviPktLossCnt += 10;
        }
#endif

        if(!FreeRunModeFlag)
        {
#if ENABLE_TOUCH_PANEL || ENABLE_TOUCH_PANEL_CTRL_OSD
            if(TPL_PollingCounter)
            {
                if(TPL_PollingCounter > 10)
                    TPL_PollingCounter -= 10;
                else
                    TPL_PollingCounter = 0;
            }
#endif
        }

#if ENABLE_MHL
    #if ((CHIP_ID == CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK) || (CHIP_ID == CHIP_TSUMD) || (CHIP_ID == CHIP_TSUM9)|| (CHIP_ID == CHIP_TSUMF))
        mapi_mhl_PollingTimerhandler();
    #endif
#endif

#if MS_DAC && ((CHIP_ID == CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK) || (CHIP_ID == CHIP_TSUMD) || (CHIP_ID == CHIP_TSUM9)|| (CHIP_ID == CHIP_TSUMF))
#if (!InitialDACByISR)
    if( w16DAC_DePopCounter )
    {
        if(w16DAC_DePopCounter > 10)
        {
            w16DAC_DePopCounter -= 10;
        }
        else
        {
            w16DAC_DePopCounter = 0;
            bDAC_DePopFlag = 1;
        }
    }
#endif
#endif

    #if ENABLE_LED_CONTROLLER
        LEDProcess_10mS();
    #endif

	#if ENABLE_DAC_DEPOP
	if ( ucDePOP4SecCounter )
	{
		if ( --ucDePOP4SecCounter == 0 )
		{
			EARDePopFlag = 1;
		}
	}
	#endif

        Clr_ms10Flag();
    }

    if(ms50Flag)
    {
#if 0//MS_DAC && (CHIP_ID==CHIP_TSUMC ||CHIP_ID==CHIP_TSUMD)
#if CheckInitialDACTime
    Main_printData("w16DAC_DePopStep==%d",w16DAC_DePopStep);
    Main_printData("w16DAC_DePopCounter==%d",w16DAC_DePopCounter);

    //TIME_MEASURE_END();
#endif
#endif

#if MS_DAC
        if( ToggleGainCntr )
        {
            if(ToggleGainCntr > 50)
                ToggleGainCntr -= 50;
            else
            {
                ToggleGainCntr = 0;
                bToggleGainFlag = 1;
            }
        }
#endif

#if ENABLE_TOUCH_KEY
        if(TouchKeyRestortCounter)
        {
            TouchKeyRestortCounter--;
            if(!TouchKeyRestortCounter && (TouchKey_InitStatus() == PRE_RESTORT))
                TouchKey_Restort_Reg();

        }

#if ENABLE_LOW_CONTACT
        if(TouchKeyLowContactDelay)
        {
            TouchKeyLowContactDelay--;
            if(!TouchKeyLowContactDelay && (TouchKey_InitStatus() == PRE_RESTORT))
            {
                TouchKey_Restort_Reg();
                TouchKeyLowContactDetectCntr = 0;
            }
        }
#endif

#if TOUCH_KEY_POWER_KEY_DEBOUNCE
        if( bPowerKeyPressed )
        {
            if( PowerKeyDebounceCntr < POWER_KEY_DEBOUNCE_PEROID )
                PowerKeyDebounceCntr++;
        }
#endif
#endif

#if ENABLE_FREESYNC
        if(IS_HDMI_FREESYNC())
        {
            if(bHDMIFreesyncChk)
                msWriteByte(SC0_E8,(msReadByte(SC0_E8)&(~BIT1))|BIT0);    //Enable force P mode
            bHDMIFreesyncChk = FALSE;
        }
#endif

        Clr_ms50Flag();
    }

    if(SecondFlag)
    {
    #if ENABLE_DEBUG && 0
        if(Second%1 == 0)
        {
        #if  ENABLE_WATCH_DOG_INT
            BYTE  final_status;                 // use idata to speedup
            final_status = msRegs[REG_2B10];
            msRegs[REG_2B08]=0x00;
            if( final_status & _BIT1 )
                Main_printMsg("WDT interrupt");

            Main_printData("u8WDT_Status==%x",u8WDT_Status);
            //u8WDT_Status=0;
        #endif
        }
        if(Second%5 == 0)
         Main_printData("Second==%d",Second);
    #endif
        Second++;
#if ENABLE_FACTORY_AUTOTEST
   if(BFactoryAutoTest==1)
		{
		
		if(Second==6)
			{
            #if InputType_HDMI
				 UserPrefSelectInputType=Input_HDMI;
            #endif

			}
			else if(Second==10)
			{
				BFactoryAutoTest=0;

			}
	
		
			if (SrcInputType!= UserPrefSelectInputType)
				{
				
			SrcInputType=UserPrefSelectInputType;  
			//UserPrefInputType=SrcInputType = Input_Analog1;	
			//Set_SaveSettingFlag();
			mStar_SetupInputPort();
			Set_InputTimingChangeFlag();
			SrcFlags |= SyncLoss;
				}
			
		}

#endif

#if EN_HotKeyTimes
//if(MenuPageIndex==HotKeyShowTimes)
{
	if(HotKeyTimes_Second>0)
	{
		HotKeyTimes_Second--;
	}
	else if(HotKeyTimes_Minutes>0)
	{
		HotKeyTimes_Minutes--;
		HotKeyTimes_Second=59;
	}

}

#endif
#if USEFLASH
        UserPref_FlashSaveFlagCheck();
#endif

#if  ENABLE_LED_CONTROLLER
        LEDProcess_1S();
#endif

#if ENABLE_BURNIN_MENU
	if( BurninModeFlag && !PowerSavingFlag )
	  {
		  if( BurninTime < 0xFFFFFFFFul )
		  {
			  BurninTime += 1;
		  }
		  else
			  BurninTime = 0;
	
		  if( BurninTime > 0 && ( BurninTime % 1800 ) == 0)
			  SaveBurninTimeFlag = 1;
	  }
	else
#endif

        if( PowerOnFlag && !PowerSavingFlag )
        {
            if( BlacklitTime < 0xFFFFFFFFul )
            {
                BlacklitTime += 1;
            }
            else
                BlacklitTime = 0;

            if( BlacklitTime > 0 && ( BlacklitTime % 1800 ) == 0 )
                SaveBlacklitTimeFlag = 1;
        }

        if( OsdCounter )
        {
            if( --OsdCounter == 0 )
            {
                Set_OsdTimeoutFlag();
            }
        }

#if HotInputSelect
        if( HotKeyCounter )
        {
            if( --HotKeyCounter == 0 )
            {
                if( PressExitFlag )
                    Set_EnableShowAutoFlag();
            }
        }
#endif

        if( PowerDownCounter )
        {
            if( --PowerDownCounter == 0 )
            {
                Set_ForcePowerSavingFlag();
            }
        }
        Set_ChangePatternFlag();

		if (g_SwitchSec && g_CountSwitchPortTimeFlag)
		{
            if( --g_SwitchSec == 0 )
            {
                g_CountSwitchPortTimeFlag=FALSE;
				g_SwitchSec=DEF_FORCE_DPMS;
                Set_ForcePowerSavingFlag();
            }
		}

#if ENABLE_TOUCH_KEY

#if ENABLE_LOW_CONTACT
        if(TouchKeyLowContactDetectCntr)
            TouchKeyLowContactDetectCntr--;
#endif

        if((Second%2) == 0)
        {
            if(!TouchKeyRestortCounter) // 120417 coding addition
                TouchKeyRestortCounter = TouchKey_Check_ESD();
        }
#endif
        Clr_SecondFlag();
    }

    if( SaveBlacklitTimeFlag )
    {
        SaveBlacklitTimeFlag = 0;
#if USEFLASH
        UserprefBacklighttime = BlacklitTime;
        UserPref_EnableFlashSaveBit( bFlashForceSaveMonitor2Bit );
#else
        SaveBlacklitTime();
#endif
    }

#if ENABLE_BURNIN_MENU
	else if( SaveBurninTimeFlag )
	{
	SaveBurninTimeFlag = 0;
	UserprefBurnintime=BurninTime;
	UserPref_EnableFlashSaveBit( bFlashForceSaveMonitor2Bit );
	
	}
#endif


#if 0//ENABLE_TOUCH_KEY
#if TOUCH_KEY_CTRL_LED
    if(TouchKey_GetLEDStatus() != TouchKeySetLEDStatus)
        TouchKey_CtrlLED(TouchKeySetLEDStatus);
#endif
#endif

}

#if EarphoneDet_Enable
void HarphoneDetFunc(void)
{

   if((!InputTimingChangeFlag)&&InputTimingStableFlag&&(!SyncLossState())&&(!PowerSavingFlag)&&bPanelOnFlag)
    ;
   else
   	return;


 	  if(PowerOnFlag&&!UserOSDMuteOnFlag)	  
 	  {
                 if(HarphoneDet_Pin)//Ear Push In
                 {
                    if((!HarphoneDetHighFlag))
                	{
            
                        msAudioLineEarSwitch(E_EAR_OUT_);
		#if AudioIC_CS8003_Enable

			 if (UserPrefVolume==0)
				msAudioDPGA_Mute();
			   else
			   {

			   	msAudioDPGA_SetVolume ( E_AUDIO_LEFT_RIGHT_CH, 0xFF-UserPrefEarVolume);//*/
			   	msWrite2ByteMask(  REG_2CA4, 0xFF77    , 0xFF7F);  //[15:8] DPGA_1_TIMEOUT_N; [6:4] DPGA_1_STEP=3'd7->N=1;  [2] DPGA_1_ZERO_DET_EN; [1] DPGA_1_FADING_EN;   [0] DPGA_1_DPGA_EN;
                           }
		
		#endif
	               Set_HarphoneDetHighFlag();	
                	}
                 }
		   else
		   {
		      if(HarphoneDetHighFlag)
		   	{
            		           msAudioLineEarSwitch(E_LINE_OUT_);
                         #if AudioIC_CS8003_Enable
                         msAudioDPGA_SetVolume ( E_AUDIO_LEFT_RIGHT_CH, 0x00);
                         msWrite2ByteMask(  REG_2CA4, 0xFF76    , 0xFF7F);  //[15:8] DPGA_1_TIMEOUT_N; [6:4] DPGA_1_STEP=3'd7->N=1;  [2] DPGA_1_ZERO_DET_EN; [1] DPGA_1_FADING_EN;   [0] DPGA_1_DPGA_EN;
                         #endif
			
			     Clr_HarphoneDetHighFlag();	
		   	}
		   }
 	   }

}
#endif

