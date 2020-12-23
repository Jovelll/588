#ifndef __BD_MSTMCU__
#define __BD_MSTMCU__
#define CPU_CLOCK_KHZ       28636//14318//28636
#define CPU_CLOCK_MHZ       (CPU_CLOCK_KHZ/1000)
#define MST_CLOCK_MHZ       28636//14318//28636
#define SERIAL_BAUD_RATE    9600
//================================================
// Detect Cable
#define hwDVICable_Pin      P1_5
#define hwDSUBCable_Pin     P2_2 //P69
//================================================
// Led
#define hwGreenLed_Pin      P0_6
#define hwAmberLed_Pin      P0_7
#define hwBlueLed_Pin       P0_6
#define hwBlacklit_Pin      P4//P1_2
#define hwPanel_Pin     P0_4//   P2_7
#define hwPanel12V_Pin      P0_3
#define hwAudio_Pin      P1_4 // P1_3   
#define hwMute_Pin             P1_6 //     P1_3
#define hwStandby_Pin       P0_3 //  P1_4 
//================================================
// mStar I2C
#define hwMS_ResetZ_Pin     P3_7  //
#define SetDDRPort  P3_4=ddrPort&BIT0;\
    P3_5=ddrPort&BIT1;\
    P3_6=ddrPort&BIT2;\
    P3_7=ddrPort&BIT3;
//================================================
// I2C
#define hwI2C_SCL_Pin       P1_0
#define hwI2C_SDA_Pin       P1_1
#define hwI2C_WP_Pin        P2_3
#if ENABLE_USB_INPUT
//Benz 2007.4.9   20:02:00
#define hwUSB_Pin           (P4&0x20)
#define hw_USB_HI()   (P4 |= 0x20)
#define hw_USB_LOW()   (P4 &= (~0x20))
#endif
#define hwHDCP_I2C_SCL_Pin       (P4 & BIT2)
#define hw_Set_Hdcp_SCL()   (P4 |= BIT2)
#define hw_Clr_Hdcp_SCL()   (P4 &= (~BIT2))
#define hwHDCP_I2C_SDA_Pin       (P4 & BIT3)
#define hw_Set_Hdcp_SDA()   (P4 |= BIT3)
#define hw_Clr_Hdcp_SDA()   (P4 &= (~BIT3))
#define hwDDC_WP_Pin        P2_5 //2006-06-01 Andy P2_2    
#define hwEEPROM_WP               P4 //2006-06-01 Andy 
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
#if ENABLE_HDCP
#define Set_i2c_SDA()       (HdcpNVRamFlag? (hw_Set_Hdcp_SDA()): (hwI2C_SDA_Pin=1))
#define Clr_i2c_SDA()       (HdcpNVRamFlag? (hw_Clr_Hdcp_SDA()): (hwI2C_SDA_Pin=0))
#define i2c_SDAHi()         (HdcpNVRamFlag? (hwHDCP_I2C_SDA_Pin): (hwI2C_SDA_Pin))
#define i2c_SDALo()         (HdcpNVRamFlag? (!(hwHDCP_I2C_SDA_Pin)): (!(hwI2C_SDA_Pin)))
#define Set_i2c_SCL()       (HdcpNVRamFlag? (hw_Set_Hdcp_SCL()): (hwI2C_SCL_Pin=1))
#define Clr_i2c_SCL()       (HdcpNVRamFlag? (hw_Clr_Hdcp_SCL()): (hwI2C_SCL_Pin=0))
#define i2c_SCLHi()         (HdcpNVRamFlag? (hwHDCP_I2C_SCL_Pin): (hwI2C_SCL_Pin))
#define i2c_SCLLo()         (HdcpNVRamFlag? (!(hwHDCP_I2C_SCL_Pin)): (!(hwI2C_SCL_Pin)))
#else
#define Set_i2c_SDA()       hwI2C_SDA_Pin=1
#define Clr_i2c_SDA()       hwI2C_SDA_Pin=0
#define i2c_SDAHi()     (hwI2C_SDA_Pin)
#define i2c_SDALo()     (!(hwI2C_SDA_Pin))
#define Set_i2c_SCL()       hwI2C_SCL_Pin=1
#define Clr_i2c_SCL()       hwI2C_SCL_Pin=0
#define i2c_SCLHi()     (hwI2C_SCL_Pin)
#define i2c_SCLLo()     (!(hwI2C_SCL_Pin))
#endif
#define Set_i2c_WP()        hwI2C_WP_Pin=1
#define Clr_i2c_WP()        hwI2C_WP_Pin=0
#define hw_SetGreenLed()    hwGreenLed_Pin=0;hwBlueLed_Pin=0
#define hw_ClrGreenLed()    hwGreenLed_Pin=1;hwBlueLed_Pin=1
#define hw_SetAmberLed()    hwAmberLed_Pin=0
#define hw_ClrAmberLed()    hwAmberLed_Pin=1
#define hw_SetBlueLed()     hwBlueLed_Pin=0
#define hw_ClrBlueLed()     hwBlueLed_Pin=1
#define hw_SetPanel()       hwPanel_Pin=0
#define hw_ClrPanel()       hwPanel_Pin=1
#define hw_SetBlacklit()                 hwBlacklit_Pin&=~BIT7
#define hw_ClrBlacklit()                  hwBlacklit_Pin|=BIT7
#define hw_SetMute()        hwMute_Pin=1
#define hw_ClrMute()        hwMute_Pin=0
#define hw_SetStandby()     hwStandby_Pin=1//Benz 2007.4.12   20:52:150 // 1    
#define hw_ClrStandby()     hwStandby_Pin=0//Benz 2007.4.12   20:52:191 // 0   
#define hw_SetPanel12V()                 hwPanel12V_Pin= 1 //0
#define hw_ClrPanel12V()                 hwPanel12V_Pin= 0 //1
#define hw_SetDDC_WP()       hwDDC_WP_Pin=0  //2005-04-14
#define hw_ClrDDC_WP()       hwDDC_WP_Pin=1  //2005-04-14
//#define Set_EEPROM_WP()     hwEEPROM_WP=1  //2006-03-20 Andy
//#define Clr_EEPROM_WP()     hwEEPROM_WP=0  //2006-03-20 Andy
#define Set_EEPROM_WP()     hwEEPROM_WP|=BIT6//2006-06-1 Andy
#define Clr_EEPROM_WP()     hwEEPROM_WP&=~BIT6//  2006-6-1 Andy 
#if ENABLE_HDCP
#define hwHDCP_Hpd_Pin        (P4 & BIT4)     // Annie 2006.8.21// Annie 2006.8.14  BIT5
#define hw_Set_HdcpHpd()   (P4 &= (~BIT4))  // Annie 2006.9.1 (P4 |= BIT4) // hwHDCP_Hpd_Pin=0  // 1 
#define hw_Clr_HdcpHpd()  (P4 |= BIT4) // Annie 2006.9.1 (P4 &= (~BIT4)) //  hwHDCP_Hpd_Pin=1  // 0 
#endif
#define hwFlash_WP_Pin   P2_4
#define hw_SetFlashWP()     hwFlash_WP_Pin=0
#define hw_ClrFlashWP()      hwFlash_WP_Pin=1
#endif
