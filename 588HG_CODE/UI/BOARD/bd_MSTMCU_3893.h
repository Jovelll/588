
#ifndef __BD_MSTMCU__
#define __BD_MSTMCU__
#define CPU_CLOCK_KHZ       28636
#define CPU_CLOCK_MHZ       (CPU_CLOCK_KHZ/1000)
#ifdef TSUMXXQX
#define XTAL_CLOCK_KHZ      14318
#define MST_CLOCK_MHZ       XTAL_CLOCK_KHZ
#else
#define MST_CLOCK_MHZ       28636
#endif

#define SERIAL_BAUD_RATE           9600
#define BrightnessPWM          PWM3
#define BRIGHTNESS_USEPWM3

#define VolumePWM               PWM2C
//#define VOLUME_USEPWM3

#define PowerKey            P1_4
#define NonUsepin           P1_6

//========================================================
// 1.Write protection:
#define hwDDC_WP_Pin           // p47
#define hwEEPROM_WP            // p46
#define hwFlash_WP_Pin         P2_4

#define hw_ClrDDC_WP()      (P4&=~BIT7)
#define hw_SetDDC_WP()      (P4|=BIT7)

#define Clr_EEPROM_WP()     (P4&=~BIT6)
#define Set_EEPROM_WP()     (P4|=BIT6)

#define hw_SetFlashWP()         hwFlash_WP_Pin=0
#define hw_ClrFlashWP()         hwFlash_WP_Pin=1

//========================================================
// 2.Backlight and panelon:
#define hwBlacklit_Pin      P1_7
#define hwPanel_Pin     //p45

#define hw_SetBlacklit()    hwBlacklit_Pin=0
#define hw_ClrBlacklit()            hwBlacklit_Pin=1

#define hw_SetPanel()       (P4|=BIT5)
#define hw_ClrPanel()       (P4&=~BIT5)

//========================================================
// 4.Detect Cable
//#define hwDVICable_Pin
#define hwDSUBCable_Pin     P2_7
#define hwHDMICable_Pin         P2_6
//========================================================
// 5.Led
#define hwGreenLed_Pin      P0_6
#define hwAmberLed_Pin      P0_7

#define hw_SetGreenLed()    hwGreenLed_Pin=0
#define hw_ClrGreenLed()    hwGreenLed_Pin=1

#define hw_SetAmberLed()           hwAmberLed_Pin=0
#define hw_ClrAmberLed()           hwAmberLed_Pin=1

//========================================================
// 6.Audio control:
#define hwMute_Pin             //P43
#define hw_SetMute()              (P4&=~BIT3)
#define hw_ClrMute()              (P4|=BIT3)
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
#define hwHDCP_Hpd_Pin        // p44
#define hw_Set_HdcpHpd()    (P4|=BIT4)//hwHDCP_Hpd_Pin=1  // Annie 2006.9.1 (P4 |= BIT4) // hwHDCP_Hpd_Pin=0  // 1
#define hw_Clr_HdcpHpd()     (P4&=BIT4)//hwHDCP_Hpd_Pin=0 // Annie 2006.9.1 (P4 &= (~BIT4)) //  hwHDCP_Hpd_Pin=1  // 0
#endif

#define hw_DVISWITCH_Pin
#define hw_SEL_HDMI()
#define hw_SEL_DVI()


#endif




