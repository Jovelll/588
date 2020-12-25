#include "types.h"
#include "board.h"
#include "global.h"
#include "ms_reg.h"
//#include "panel.h"
#include "misc.h"
#include "ms_rwreg.h"
#include "mstar.h"
#include "debug.h"
#include "Common.h"
#include "msOsd.h"
#include "adjust.h"
//#include "msADC.h"
//#include "drvadc.h"
#include "MsHDCP.h"
#include "drvmsOVD.h"
#include "gpio_def.h"
//#if ENABLE_LED_CONTROLLER
//#include "LedControl.h"
//#endif
#include "Power.h"

#if (MS_PM)
#include "ms_PM.h"
#endif

////////////////////////////////////////////////////
//#include "drvPower.h"
//#include "drvmStar.h"
///////////////////////////////////////////////////
#if ENABLE_DPS
#include "msDPS_Setting.h"
#endif

#define POWER_DEBUG    0
#if ENABLE_DEBUG&&POWER_DEBUG
    #define POWER_printData(str, value)   printData(str, value)
    #define POWER_printMsg(str)           printMsg(str)
#else
    #define POWER_printData(str, value)
    #define POWER_printMsg(str)
#endif
#define TurnOffOSD      1

#if ENABLE_FACTORY_AUTOTEST
extern bit BFactoryAutoTest;
#endif

extern BYTE xdata MenuPageIndex;


void Power_TurnOffLed( void );
void Power_TurnOffPanel( void );
void Power_PowerOffSystem( void );

void Power_TurnOnAmberLed( void );
extern Bool PowerOffSystem( void );


//=============================================
// Control system power status
// caller : main()
void Power_PowerHandler( void )
{
    if( ForcePowerSavingFlag )
    {
        Clr_ForcePowerSavingFlag(); // clear force power saving flag
        if( !PowerSavingFlag )
        {
            if( PowerOnFlag )
            {
            
                #if ENABLE_DP_INPUT
					#if CHIP_ID==CHIP_TSUM9 || CHIP_ID == CHIP_TSUMF
					if(DPRXCheckDPMS()) // Do not into power saving when CTS
					{
	                	Clr_ForcePowerSavingFlag();
	                	return;
					}	
					#endif
                #endif
                
                Power_PowerOffSystem();
#if (MS_PM)
                msPM_SetFlag_Standby();
#endif
                Power_TurnOnAmberLed();//red
                printMsg("99999");
            }
            else
            {
                PowerOffSystem();
            }

        }
    }
}

extern void mStar_AdjustVolume( BYTE volume );
void Power_PowerOnSystem( void )
{
#if ENABLE_USB_INPUT
    hw_USB_HI();
#endif
#if FACTORY_CLEAR_EDID_WP_ENABLE
    if(FactoryModeFlag||BurninModeFlag||DoBurninModeFlag)
	hw_ClrDDC_WP();
    else
	hw_SetDDC_WP();
#endif
    Set_ShowInputInfoFlag();
    gBoolVisualKey = 0;
    gByteVisualKey = 0xff;
    Power_TurnOffPanel(); //modify by smc.lmw 20160617 for solution DC can not wake up
    mStar_PowerUp();
    Clr_PowerSavingFlag();
    Set_InputTimingChangeFlag();
    //Clr_ForcePowerDownFlag();
    SrcFlags |= SyncLoss;
    SwitchPortCntr = 0;
#if AudioFunc
    hw_ClrAudio_SD();
#endif

    mStar_SetupInputPort();
}

void Power_PowerOffSystem( void )
{
#if ENABLE_USB_INPUT
    if( !PowerOnFlag )
        hw_USB_LOW();
#endif

#if AudioFunc
    hw_SetMute();
    hw_SetAudio_SD();
#endif

    Power_TurnOffPanel();
    mStar_PowerDown();
    Set_PowerSavingFlag();
    Clr_InputTimingChangeFlag();
    OsdCounter = 0;
    PowerDownCounter = 0;
    Clr_OsdTimeoutFlag();
}

void Power_TurnOffLed( void )
{
#if ENABLE_TOUCH_KEY
    #if TOUCH_KEY_CTRL_LED
    //TouchKeySetLEDStatus = TouchKeyLEDStatus_OFF;
    TouchKey_CtrlLED(TouchKeyLEDStatus_OFF);
    #endif
#else
#if Enable_LED

	hw_ClrGreenLed();

	hw_SetAmberLed();
	printMsg("1111");
	
#else
	hw_ClrGreenLed();
    hw_ClrAmberLed();
	printMsg("159789");
#endif

#endif
}

void Power_TurnOnGreenLed( void )
{
#if ENABLE_TOUCH_KEY
    #if TOUCH_KEY_CTRL_LED
    //TouchKeySetLEDStatus = TouchKeyLEDStatus_GREEN_ON;
    TouchKey_CtrlLED(TouchKeyLEDStatus_GREEN_ON);
    #endif
#else
    hw_SetGreenLed();
    hw_ClrAmberLed();
#endif
}

void Power_TurnOnAmberLed( void )
{
#if ENABLE_TOUCH_KEY
    #if TOUCH_KEY_CTRL_LED
    //TouchKeySetLEDStatus = TouchKeyLEDStatus_AMBER_ON;
    TouchKey_CtrlLED(TouchKeyLEDStatus_AMBER_ON);
    #endif
#else
    hw_ClrGreenLed();
    hw_SetAmberLed();
#endif
}

void Power_TurnOnPanel( void )
{

    while( bPanelOffOnFlag )
    {
        if(InputTimingChangeFlag && !DisplayLogoFlag)
            return;
    }

    bPanelOnFlag = 1;

    hw_SetPanel();
#if ENABLE_EDP_OUTPUT
    //eDP don't need this code
#else // LVDS
	mStar_BlackWhiteScreenCtrl(BW_SCREEN_BLACK);
#if ENABLE_LED_CONTROLLER
    #if ENABLE_REDUCE_BOOST_VOLTAGE
    LEDBoostDutyCtrl(LED_DISPLAY_CURRENT_SETTING);
    #endif
#endif

    ForceDelay1ms( PanelOnTiming1 );

#if PANEL_VCOM_ADJUST
    HW_SET_VCOM_I2C_SDA();
    HW_SET_VCOM_I2C_SCL();
#endif

    Power_ModCtrl(_ENABLE);

#ifndef ENABLE_UP_LOW_BOUNDARY_PROTECTION
    if( !iGenTuningFinished )
    {
        I_Gen_Tuning();
    }
#endif

    ForceDelay1ms( PanelOnTiming2 );

    mStar_BlackWhiteScreenCtrl(BW_SCREEN_OFF);

#ifdef ENABLE_UP_LOW_BOUNDARY_PROTECTION
    if( !iGenTuningFinished )
    {
        I_Gen_Tuning();
        //printMsg("------------------------ENABLE_UP_LOW_BOUNDARY_PROTECTION");
    }
#endif

#endif //ENABLE_EDP_OUTPUT
    if( InputTimingChangeFlag && !DisplayLogoFlag )                     // for mode change
        Power_TurnOffPanel();
    else
    {

	mStar_AdjustBrightness(UserPrefBrightness);
	
  #if ((CHIP_ID==CHIP_TSUMB || CHIP_ID==CHIP_TSUMY || CHIP_ID==CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID==CHIP_TSUMD || CHIP_ID==CHIP_TSUMJ) && (ENABLE_LED_CONTROLLER))
        msLED_On();
  #else
        hw_SetBlacklit();
  #endif
    }

    POWER_printData("  Power_TurnOnPanel", 1);
}

void Power_TurnOffPanel( void )
{
    if( !bPanelOnFlag )
        return;

    bPanelOnFlag = 0;

#if TurnOffOSD
    Osd_DoubleBuffer(FALSE);
    Osd_Hide();
    mStar_ScalerDoubleBuffer(FALSE);
    Delay4us();
#endif

  #if ((CHIP_ID==CHIP_TSUMB || CHIP_ID==CHIP_TSUMY || CHIP_ID==CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID==CHIP_TSUMD  || CHIP_ID==CHIP_TSUMJ) && (ENABLE_LED_CONTROLLER))
    msLED_Off();
  #else
   // if (!DisplayLogoFlag)

  #if ENABLE_FACTORY_AUTOTEST
	  if (BFactoryAutoTest==0)
#endif 
    {
    hw_ClrBlacklit();
    }
#endif

#if BRIGHTNESS_INVERSE	 //+Duty power
				 drvGPIO_SetBacklightDuty(BrightnessPWM, 0xFF);
#else							 //-Duty power
				 drvGPIO_SetBacklightDuty(BrightnessPWM, 0);
#endif



    ForceDelay1ms( PanelOffTiming1 );

#if PanelType==PanelLTM230HT03
    mStar_BlackWhiteScreenCtrl(BW_SCREEN_BLACK);
#else
    mStar_BlackWhiteScreenCtrl(BW_SCREEN_WHITE);
#endif
    ForceDelay1ms( 80 );
    mStar_BlackWhiteScreenCtrl(BW_SCREEN_OFF);

    Power_ModCtrl(_DISABLE);

#if PANEL_VCOM_ADJUST
    HW_CLR_VCOM_I2C_SCL();
    HW_CLR_VCOM_I2C_SDA();
#endif

    ForceDelay1ms( PanelOffTiming2 );
  #if ENABLE_FACTORY_AUTOTEST
		  if (BFactoryAutoTest==0)
#endif 
		{
    hw_ClrPanel();
     }
#if TurnOffOSD
    Osd_DoubleBuffer(TRUE);
#endif
    SetPanelOffOnCounter(1200);
    //u16PanelOffOnCounter = 1200;
    //bPanelOffOnFlag = 1;

    POWER_printData("Power_TurnOffPanel", 1);
}
