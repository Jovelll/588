
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

#define PowerKey                   P1_5

#define NonUsepin           P0_6

//========================================================
// 1.Write protection:
#define hwDDC_WP_Pin           P2_2//    NonUsepin
#define hwEEPROM_WP               P2_3// NonUsepin
#define hwFlash_WP_Pin       P2_4//  NonUsepin//    P2_7
/*
#define hw_ClrDDC_WP()      msWriteByte(BK0_00, REGBANK1); \
                                                    msWriteByte(0xA3, msReadByte(0xA3)&(~BIT6)); \
                                                    msWriteByte(BK0_00, REGBANK0)
#define hw_SetDDC_WP()      msWriteByte(BK0_00, REGBANK1); \
                                                    msWriteByte(0xA3, msReadByte(0xA3) |BIT6); \
                                                    msWriteByte(BK0_00, REGBANK0)

#define Clr_EEPROM_WP()     msWriteByte(BK0_00, REGBANK1); \
                                                    msWriteByte(0xA3, msReadByte(0xA3)&(~BIT5)); \
                                                    msWriteByte(BK0_00, REGBANK0)
#define Set_EEPROM_WP()     msWriteByte(BK0_00, REGBANK1); \
                                                    msWriteByte(0xA3, msReadByte(0xA3) |BIT5); \
                                                    msWriteByte(BK0_00, REGBANK0)
*/
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
/*
#define hw_SetBlacklit()           hwBlacklit_Pin=1  // 1 OLD
#define hw_ClrBlacklit()                 hwBlacklit_Pin=0  //0  OLD
#define hw_SetPanel()              hwPanel_Pin=0     //反向
#define hw_ClrPanel()              hwPanel_Pin=1     //反向
*/
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
#define hwGreenLed_Pin      P0_6
#define hwAmberLed_Pin      P0_7
//#define hwBlueLed_Pin            P1_6

#define hw_SetGreenLed()    hwGreenLed_Pin=0
#define hw_ClrGreenLed()    hwGreenLed_Pin=1

#define hw_SetAmberLed()           hwAmberLed_Pin=0//  1
#define hw_ClrAmberLed()           hwAmberLed_Pin=1 // 0
//#define hw_SetBlueLed()       hwBlueLed_Pin=1
//#define hw_ClrBlueLed()       hwBlueLed_Pin=0


//========================================================
// 6.Audio control:
//#define hwPanel12V_Pin        P0_4
//#define hwStandby_Pin            P4
//#define hwAudio_Pin           P4
#define hwMute_Pin             P1_4
#define hw_SetMute()            hwMute_Pin=0
#define hw_ClrMute()            hwMute_Pin=1
/*
    #define hw_SetMute()               msWriteByte(BK0_00, REGBANK1); \
                                                        msWriteByte(0xA0, msReadByte(0xA0)&(~BIT6)); \
                                                        msWriteByte(0xA1, msReadByte(0xA1)&(~BIT6)); \
                                                        msWriteByte(BK0_00, REGBANK0)
    #define hw_ClrMute()               msWriteByte(BK0_00, REGBANK1); \
                                                        msWriteByte(0xA0, msReadByte(0xA0)&(~BIT6)); \
                                                        msWriteByte(0xA1, msReadByte(0xA1) |BIT6); \
                                                        msWriteByte(BK0_00, REGBANK0)
*/
//#define hw_SetPanel12V()         hwPanel12V_Pin= 0
//#define hw_ClrPanel12V()         hwPanel12V_Pin= 1
//#define hw_SetStandby()       hwStandby_Pin&= ~(BIT2)
//#define hw_ClrStandby()       hwStandby_Pin|=(BIT2)

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
#if defined(UseVGACableReadWriteAllPortsEDID)&&!defined(UseInternalDDCRam)
#define hw_VGA_EDID_SCL_Pin          P3_0
#define hw_VGA_EDID_SDA_Pin         P3_1

#define hw_DVI_EDID_SCL_Pin          P2_0
#define hw_DVI_EDID_SDA_Pin         P2_1

#define hw_HDMI_EDID_SCL_Pin          P3_0
#define hw_HDMI_EDID_SDA_Pin         P3_1

#define Set_i2c_SDA()       ((LoadEDIDSelectPort == VGA1)?(hw_VGA_EDID_SDA_Pin=1):((LoadEDIDSelectPort == DVI1)?(hw_DVI_EDID_SDA_Pin=1):((LoadEDIDSelectPort == HDMI1)?(hw_HDMI_EDID_SDA_Pin=1):(hwI2C_SDA_Pin=1))))
#define Clr_i2c_SDA()       ((LoadEDIDSelectPort == VGA1)?(hw_VGA_EDID_SDA_Pin=0):((LoadEDIDSelectPort == DVI1)?(hw_DVI_EDID_SDA_Pin=0):((LoadEDIDSelectPort == HDMI1)?(hw_HDMI_EDID_SDA_Pin=0):(hwI2C_SDA_Pin=0))))
#define i2c_SDAHi()         ((LoadEDIDSelectPort == VGA1)?(hw_VGA_EDID_SDA_Pin):((LoadEDIDSelectPort == DVI1)?(hw_DVI_EDID_SDA_Pin):((LoadEDIDSelectPort == HDMI1)?(hw_HDMI_EDID_SDA_Pin):(hwI2C_SDA_Pin))))
#define i2c_SDALo()         ((LoadEDIDSelectPort == VGA1)?(!(hw_VGA_EDID_SDA_Pin)):((LoadEDIDSelectPort == DVI1)?(!(hw_DVI_EDID_SDA_Pin)):((LoadEDIDSelectPort == HDMI1)?(!(hw_HDMI_EDID_SDA_Pin)):(!(hwI2C_SDA_Pin)))))

#define Set_i2c_SCL()           ((LoadEDIDSelectPort == VGA1)?(hw_VGA_EDID_SCL_Pin=1):((LoadEDIDSelectPort == DVI1)?(hw_DVI_EDID_SCL_Pin=1):((LoadEDIDSelectPort == HDMI1)?(hw_HDMI_EDID_SCL_Pin=1):(hwI2C_SCL_Pin=1))))
#define Clr_i2c_SCL()           ((LoadEDIDSelectPort == VGA1)?(hw_VGA_EDID_SCL_Pin=0):((LoadEDIDSelectPort == DVI1)?(hw_DVI_EDID_SCL_Pin=0):((LoadEDIDSelectPort == HDMI1)?(hw_HDMI_EDID_SCL_Pin=0):(hwI2C_SCL_Pin=0))))
#define i2c_SCLHi()         ((LoadEDIDSelectPort == VGA1)?(hw_VGA_EDID_SCL_Pin):((LoadEDIDSelectPort == DVI1)?(hw_DVI_EDID_SCL_Pin):((LoadEDIDSelectPort == HDMI1)?(hw_HDMI_EDID_SCL_Pin):(hwI2C_SCL_Pin))))
#define i2c_SCLLo()         ((LoadEDIDSelectPort == VGA1)?(!(hw_VGA_EDID_SCL_Pin)):((LoadEDIDSelectPort == DVI1)?(!(hw_DVI_EDID_SCL_Pin)):((LoadEDIDSelectPort == HDMI1)?(!(hw_HDMI_EDID_SCL_Pin)):(!(hwI2C_SCL_Pin)))))
#else
#define Set_i2c_SDA()          hwI2C_SDA_Pin=1
#define Clr_i2c_SDA()          hwI2C_SDA_Pin=0
#define i2c_SDAHi()     (hwI2C_SDA_Pin)
#define i2c_SDALo()     (!(hwI2C_SDA_Pin))
#define Set_i2c_SCL()          hwI2C_SCL_Pin=1
#define Clr_i2c_SCL()          hwI2C_SCL_Pin=0
#define i2c_SCLHi()     (hwI2C_SCL_Pin)
#define i2c_SCLLo()     (!(hwI2C_SCL_Pin))
#endif
#if ENABLE_HDCP
#define hwHDCP_Hpd_Pin        NonUsepin//P2_7     // Annie 2006.8.21// Annie 2006.8.14  BIT5
#define hw_Set_HdcpHpd()    hwHDCP_Hpd_Pin=1  // Annie 2006.9.1 (P4 |= BIT4) // hwHDCP_Hpd_Pin=0  // 1
#define hw_Clr_HdcpHpd()     hwHDCP_Hpd_Pin=0 // Annie 2006.9.1 (P4 &= (~BIT4)) //  hwHDCP_Hpd_Pin=1  // 0
#endif
#endif

#define hw_DVISWITCH_Pin        P2_4
#define hw_SEL_HDMI()           //hw_DVISWITCH_Pin=0
#define hw_SEL_DVI()            //hw_DVISWITCH_Pin=1
/*

#if ENABLE_HDMI
#define hw_DVISWITCH_Pin        P2_4
#define hw_SEL_HDMI()           hw_DVISWITCH_Pin=0
#define hw_SEL_DVI()            hw_DVISWITCH_Pin=1

#define hw_AUDIOSWITCH_Pin      P2_4
#define hw_Audio_HDMI()         hw_AUDIOSWITCH_Pin=0
#define hw_Audio_PC()               hw_AUDIOSWITCH_Pin=1
#else
#define hwAudio_Sel_Pin            P2_4
#define hw_Audio_Sel_A()        hwAudio_Sel_Pin=1
#define hw_Audio_Sel_D()        hwAudio_Sel_Pin=0
#endif

#define hwAV_Mute_Pin           P4
#define hw_Set_AV_Mute()        hwAV_Mute_Pin|=(BIT3)
#define hw_Clr_AV_Mute()        hwAV_Mute_Pin&= ~(BIT3)


#define BriPWM                PWM2C

*/

