#ifndef __MAINBOARD_3836__
#define __MAINBOARD_3836__

#define CPU_CLOCK_KHZ       28636
#define CPU_CLOCK_MHZ       (CPU_CLOCK_KHZ/1000)
#define MST_CLOCK_MHZ       28636
#define SERIAL_BAUD_RATE    9600
#define UsePWMBrightness    1
#define BrightnessPWM       PWM2C
#define BRIGHTNESS_USEPWM3

#define VolumePWM                 PWM0C
//#define VOLUME_USEPWM3

#define PowerKey                   P0_3

//================================================
// Detect Cable
// DVI and HDMI detect by SAR4
#define hwDSUBCable_Pin     P1_3
#define hwDVICable_Pin  P2_7
#define hwHDMICable_Pin     P1_4
//================================================
// Led
//efine hwGreenLed_Pin      P0_6
//efine hwAmberLed_Pin      P0_7
#define hwBlacklit_Pin      P1_5
#define hwPanel_Pin                   P0_5
#define hwEEPROM_WP     // useless
#define hwFLASH_WP              P2_4
//================================================
// mStar I2C
#define hwMS_ResetZ_Pin     P3_7

//#define ReduceDDC
#ifdef ReduceDDC
#define hwSystemVcc_Pin     P0_0
#endif


#define SetDDRPort  P3_4=ddrPort&BIT0;\
    P3_5=ddrPort&BIT1;\
    P3_6=ddrPort&BIT2;\
    P3_7=ddrPort&BIT3;
//================================================
// I2C
#define hwI2C_SCL_Pin       P1_0
#define hwI2C_SDA_Pin       P1_1

//Audio
#define Set_ms_ResetZ()     hwMS_ResetZ_Pin=1
#define Clr_ms_ResetZ()     hwMS_ResetZ_Pin=0
#define Set_ms_CSZ()        hwMS_CSZ_Pin=1
#define Clr_ms_CSZ()        hwMS_CSZ_Pin=0
#define Set_ms_SDA()        hwMS_SDA_Pin=1
#define Clr_ms_SDA()        hwMS_SDA_Pin=0
#define ms_SDAHi()          hwMS_SDA_Pin==1
#define Set_ms_SCL()        hwMS_SCL_Pin=1
#define Clr_ms_SCL()        hwMS_SCL_Pin=0
// mStar DDR
#define Set_ms_ALE()        hwMS_ALE_Pin=1
#define Clr_ms_ALE()        hwMS_ALE_Pin=0
#define Set_ms_WRZ()        hwMS_WRZ_Pin=1
#define Clr_ms_WRZ()        hwMS_WRZ_Pin=0
#define Set_ms_RDZ()        hwMS_RDZ_Pin=1
#define Clr_ms_RDZ()        hwMS_RDZ_Pin=0
#define Set_i2c_SDA()       hwI2C_SDA_Pin=1
#define Clr_i2c_SDA()       hwI2C_SDA_Pin=0
#define i2c_SDAHi()     (hwI2C_SDA_Pin)
#define i2c_SDALo()     (!(hwI2C_SDA_Pin))
#define Set_i2c_SCL()       hwI2C_SCL_Pin=1
#define Clr_i2c_SCL()       hwI2C_SCL_Pin=0
#define i2c_SCLHi()     (hwI2C_SCL_Pin)
#define i2c_SCLLo()     (!(hwI2C_SCL_Pin))

#if ENABLE_USB_INPUT
//#define hwUSB_Pin         (P4&0x20)
#define hw_USB_HI()   (P4 |= 0x20)
#define hw_USB_LOW()   (P4 &= (~0x20))
#endif

#ifdef ScanKey
#define hw_SK_SDA      P0_2
#define hw_SK_SCL       P0_1
extern void SK_CtrlGreenLed( bit ctrl_on );
extern void SK_CtrlAmberLed( bit ctrl_on );

#define hw_SetGreenLed()            (SK_CtrlGreenLed(1))
#define hw_ClrGreenLed()            (SK_CtrlGreenLed(0))
#define hw_SetAmberLed()        (SK_CtrlAmberLed(1))
#define hw_ClrAmberLed()        (SK_CtrlAmberLed(0))
#define Set_SK_SCL()    hw_SK_SCL = 1
#define Clr_SK_SCL()     hw_SK_SCL = 0
#define SK_SCLHi()     (hw_SK_SCL)
#define SK_SCLLo()     (!(hw_SK_SCL))

#define Set_SK_SDA()    hw_SK_SDA = 1
#define Clr_SK_SDA()     hw_SK_SDA = 0
#define SK_SDAHi()      (hw_SK_SDA)
#define SK_SDALo()      (!(hw_SK_SDA))

#else

#define hwGreenLed_Pin      P0_6
#define hwAmberLed_Pin      P0_7
#define hw_SetGreenLed()    hwGreenLed_Pin=1
#define hw_ClrGreenLed()    hwGreenLed_Pin=0
#define hw_SetAmberLed()           hwAmberLed_Pin=1
#define hw_ClrAmberLed()           hwAmberLed_Pin=0
#endif

#ifndef SCALAR_IO
#define SCALAR_IO
#endif
#define hw_SetBlacklit()    (SetScalerGPIO(12, 0))//hwBlacklit_Pin=0
#define hw_ClrBlacklit()    (SetScalerGPIO(12, 1))//hwBlacklit_Pin=1

#define hw_SetPanel()       hwPanel_Pin =1
#define hw_ClrPanel()       hwPanel_Pin =0

//Audio
//#define hwMute_Pin            // GPIO_06
#define hw_SetMute()            P1_7=0// (SetScalerGPIO(6, 0))
#define hw_ClrMute()            P1_7=1//(SetScalerGPIO(6, 1))
//#define hwStandby_Pin     hwMute_Pin
#define hw_SetStandby()     //(hw_SetMute())
#define hw_ClrStandby()     //(hw_ClrMute())

//#define hwDDC_WP_Pin      // GPIO_12
#define hw_SetDDC_WP()      P1_6=1//(SetScalerGPIO(12, 0))
#define hw_ClrDDC_WP()      P1_6=0//(SetScalerGPIO(12, 1))

#define Set_EEPROM_WP()         // useless
#define Clr_EEPROM_WP()         // useless
#define hw_SetFlashWP()     hwFLASH_WP=0
#define hw_ClrFlashWP()     hwFLASH_WP=1

#if ENABLE_HDCP
#define hw_Set_HdcpHpd()    //(SetScalerGPIO(12, 1))//hwHDCP_Hpd_Pin=1
#define hw_Clr_HdcpHpd()    //(SetScalerGPIO(12, 0))//hwHDCP_Hpd_Pin=0
#endif

#if ENABLE_TOUCH_PANEL_CTRL
#define hw_TouchPanel_Pin       // p45
#define hw_Set_TouchPanel()     P4|=BIT5
#define hw_Clr_TouchPanel()     P4&=~BIT5
#endif

#endif
