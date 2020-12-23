
#ifndef __BD_MSTMCU__
#define __BD_MSTMCU__
#define CPU_CLOCK_KHZ       28636
#define CPU_CLOCK_MHZ       (CPU_CLOCK_KHZ/1000)
#ifdef TSUMXXNX
#define XTAL_CLOCK_KHZ      14318
#define MST_CLOCK_MHZ       XTAL_CLOCK_KHZ
#else
#define MST_CLOCK_MHZ       28636
#endif

#define SERIAL_BAUD_RATE           9600
#define BrightnessPWM          PWM1C//    PWM0C
//#define BRIGHTNESS_USEPWM3

#define VolumePWM               PWM3
#define VOLUME_USEPWM3

#define PowerKey                   P1_4
#define NonUsepin           P1_6

//========================================================
// 1.Write protection:
#define hwDDC_WP_Pin           P2_2//    NonUsepin
#define hwEEPROM_WP               P2_3// NonUsepin
#define hwFlash_WP_Pin       P2_4//  NonUsepin//    P2_7

#define hw_ClrDDC_WP()      hwDDC_WP_Pin=1
#define hw_SetDDC_WP()      hwDDC_WP_Pin=0

#define Clr_EEPROM_WP()     hwEEPROM_WP=0
#define Set_EEPROM_WP()     hwEEPROM_WP=1

#define hw_SetFlashWP()         hwFlash_WP_Pin=0
#define hw_ClrFlashWP()         hwFlash_WP_Pin=1

//========================================================
// 2.Backlight and panelon:
#define hwBlacklit_Pin      P1_2
#define hwPanel_Pin         P2_7//P0_5

#define hw_SetBlacklit()    hwBlacklit_Pin=0
#define hw_ClrBlacklit()    hwBlacklit_Pin=1

#define hw_SetPanel()       hwPanel_Pin=1//  0
#define hw_ClrPanel()       hwPanel_Pin=0     // 1

//========================================================
// 4.Detect Cable
#define hwDVICable_Pin      0
#define hwDSUBCable_Pin NonUsepin// P2_3
//#define hwHDMICable_Pin
//========================================================
// 5.Led
#ifdef ScanKey
#define hw_SK_SDA      P0_1
#define hw_SK_SCL       P0_0
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

//========================================================
// 6.Audio control:
#define hwMute_Pin             P4
#define hw_SetMute()              hwMute_Pin|=(BIT2)
#define hw_ClrMute()              hwMute_Pin&= ~(BIT2)
#define hw_SetStandby()     //hwStandby_Pin&= ~(BIT2)
#define hw_ClrStandby()     //hwStandby_Pin|=(BIT2)

//========================================================
// mStar I2C
#define hwMS_ResetZ_Pin     P3_7  //
#define SetDDRPort  P3_4=ddrPort&BIT0;\
    P3_5=ddrPort&BIT1;\
    P3_6=ddrPort&BIT2;\
    P3_7=ddrPort&BIT3;
//========================================================
// I2C
#define hwI2C_SCL_Pin       P1_0
#define hwI2C_SDA_Pin       P1_1
#define Set_ms_ResetZ() hwMS_ResetZ_Pin=1
#define Clr_ms_ResetZ() hwMS_ResetZ_Pin=0
#define Set_ms_CSZ()        hwMS_CSZ_Pin=1
#define Clr_ms_CSZ()        hwMS_CSZ_Pin=0
#define Set_ms_SDA()        hwMS_SDA_Pin=1
#define Clr_ms_SDA()        hwMS_SDA_Pin=0
#define ms_SDAHi()      hwMS_SDA_Pin==1
#define Set_ms_SCL()        hwMS_SCL_Pin=1
#define Clr_ms_SCL()        hwMS_SCL_Pin=0
// mStar DDR
#define Set_ms_ALE()        hwMS_ALE_Pin=1
#define Clr_ms_ALE()        hwMS_ALE_Pin=0
#define Set_ms_WRZ()        hwMS_WRZ_Pin=1
#define Clr_ms_WRZ()        hwMS_WRZ_Pin=0
#define Set_ms_RDZ()        hwMS_RDZ_Pin=1
#define Clr_ms_RDZ()        hwMS_RDZ_Pin=0
#define Set_i2c_SDA()          hwI2C_SDA_Pin=1
#define Clr_i2c_SDA()          hwI2C_SDA_Pin=0
#define i2c_SDAHi()     (hwI2C_SDA_Pin)
#define i2c_SDALo()     (!(hwI2C_SDA_Pin))
#define Set_i2c_SCL()          hwI2C_SCL_Pin=1
#define Clr_i2c_SCL()          hwI2C_SCL_Pin=0
#define i2c_SCLHi()     (hwI2C_SCL_Pin)
#define i2c_SCLLo()     (!(hwI2C_SCL_Pin))
#if ENABLE_HDCP
#define hwHDCP_Hpd_Pin        NonUsepin//P2_7     // Annie 2006.8.21// Annie 2006.8.14  BIT5
#define hw_Set_HdcpHpd()    hwHDCP_Hpd_Pin=1  // Annie 2006.9.1 (P4 |= BIT4) // hwHDCP_Hpd_Pin=0  // 1
#define hw_Clr_HdcpHpd()     hwHDCP_Hpd_Pin=0 // Annie 2006.9.1 (P4 &= (~BIT4)) //  hwHDCP_Hpd_Pin=1  // 0
#endif

#define hw_DVISWITCH_Pin
#define hw_SEL_HDMI()
#define hw_SEL_DVI()

#endif




