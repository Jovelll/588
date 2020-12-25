#ifndef _BD_AOC_715G4663_TSUMU58P_
#define _BD_AOC_715G4663_TSUMU58P_

#define XTAL_CLOCK_KHZ          14318

#define CPU_CLOCK_KHZ           28636
#define CPU_CLOCK_MHZ           (CPU_CLOCK_KHZ/1000)
#define MST_CLOCK_MHZ           XTAL_CLOCK_KHZ

#define SERIAL_BAUD_RATE        9600
#define BrightnessPWM           PWM1C//    PWM0C
//#define BRIGHTNESS_USEPWM3

#if AudioFunc
#define VolumePWM                       PWM0C // 100617 coding, temp setting
//#define VOLUME_USEPWM3
#endif


//========================================================
// 1.Write protection:
#define hwDDC_WP_Pin        P1_5
#define hwEEPROM_WP         //P2_2
#define hwFlash_WP_Pin      P2_4

#define hw_SetDDC_WP()      hwDDC_WP_Pin=1
#define hw_ClrDDC_WP()      hwDDC_WP_Pin=0

#define Set_EEPROM_WP()
#define Clr_EEPROM_WP()

#define hw_SetFlashWP()         hwFlash_WP_Pin=0
#define hw_ClrFlashWP()         hwFlash_WP_Pin=1

//========================================================
// 2.Backlight and panelon:
#define hwBlacklit_Pin      P4
#define hwPanel_Pin         P4

#define hw_SetBlacklit()        P4&=~BIT2;\
    g_bBacklightOn=1;
#define hw_ClrBlacklit()        P4|=BIT2;\
    g_bBacklightOn=0;

#define hw_SetPanel()       P4|=BIT1
#define hw_ClrPanel()           P4&=~BIT1

//========================================================
// 4.Detect Cable
#define hwDSUBCable_Pin
#define hwDVICable_Pin
//========================================================
// 5.Led
#define hwGreenLed_Pin      P0_7
#define hwAmberLed_Pin      P0_6

#define hw_SetGreenLed()        hwGreenLed_Pin=0
#define hw_ClrGreenLed()        hwGreenLed_Pin=1

#define hw_SetAmberLed()    hwAmberLed_Pin=0
#define hw_ClrAmberLed()    hwAmberLed_Pin=1
//========================================================
// 6.Audio control:
#define hwMute_Pin
#define hw_SetMute()
#define hw_ClrMute()
#define hw_SetStandby()
#define hw_ClrStandby()

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

#define Set_ms_ResetZ()     hwMS_ResetZ_Pin=1
#define Clr_ms_ResetZ()     hwMS_ResetZ_Pin=0
#define Set_ms_CSZ()            hwMS_CSZ_Pin=1
#define Clr_ms_CSZ()            hwMS_CSZ_Pin=0
#define Set_ms_SDA()        hwMS_SDA_Pin=1
#define Clr_ms_SDA()            hwMS_SDA_Pin=0
#define ms_SDAHi()          hwMS_SDA_Pin==1
#define Set_ms_SCL()            hwMS_SCL_Pin=1
#define Clr_ms_SCL()            hwMS_SCL_Pin=0
// mStar DDR
#define Set_ms_ALE()            hwMS_ALE_Pin=1
#define Clr_ms_ALE()            hwMS_ALE_Pin=0
#define Set_ms_WRZ()        hwMS_WRZ_Pin=1
#define Clr_ms_WRZ()            hwMS_WRZ_Pin=0
#define Set_ms_RDZ()            hwMS_RDZ_Pin=1
#define Clr_ms_RDZ()            hwMS_RDZ_Pin=0

#if defined(UseVGACableReadWriteAllPortsEDID)&&!defined(UseInternalDDCRam)
#define hw_VGA_EDID_SCL_Pin             P3_0
#define hw_VGA_EDID_SDA_Pin             P3_1

#define hw_DVI_EDID_SCL_Pin             P2_0
#define hw_DVI_EDID_SDA_Pin             P2_1

#define hw_HDMI_EDID_SCL_Pin        P3_0
#define hw_HDMI_EDID_SDA_Pin        P3_1

#define Set_i2c_SDA()       ((LoadEDIDSelectPort == VGA1)?(hw_VGA_EDID_SDA_Pin=1):((LoadEDIDSelectPort == DVI1)?(hw_DVI_EDID_SDA_Pin=1):((LoadEDIDSelectPort == HDMI1)?(hw_HDMI_EDID_SDA_Pin=1):(hwI2C_SDA_Pin=1))))
#define Clr_i2c_SDA()           ((LoadEDIDSelectPort == VGA1)?(hw_VGA_EDID_SDA_Pin=0):((LoadEDIDSelectPort == DVI1)?(hw_DVI_EDID_SDA_Pin=0):((LoadEDIDSelectPort == HDMI1)?(hw_HDMI_EDID_SDA_Pin=0):(hwI2C_SDA_Pin=0))))
#define i2c_SDAHi()         ((LoadEDIDSelectPort == VGA1)?(hw_VGA_EDID_SDA_Pin):((LoadEDIDSelectPort == DVI1)?(hw_DVI_EDID_SDA_Pin):((LoadEDIDSelectPort == HDMI1)?(hw_HDMI_EDID_SDA_Pin):(hwI2C_SDA_Pin))))
#define i2c_SDALo()         ((LoadEDIDSelectPort == VGA1)?(!(hw_VGA_EDID_SDA_Pin)):((LoadEDIDSelectPort == DVI1)?(!(hw_DVI_EDID_SDA_Pin)):((LoadEDIDSelectPort == HDMI1)?(!(hw_HDMI_EDID_SDA_Pin)):(!(hwI2C_SDA_Pin)))))

#define Set_i2c_SCL()           ((LoadEDIDSelectPort == VGA1)?(hw_VGA_EDID_SCL_Pin=1):((LoadEDIDSelectPort == DVI1)?(hw_DVI_EDID_SCL_Pin=1):((LoadEDIDSelectPort == HDMI1)?(hw_HDMI_EDID_SCL_Pin=1):(hwI2C_SCL_Pin=1))))
#define Clr_i2c_SCL()           ((LoadEDIDSelectPort == VGA1)?(hw_VGA_EDID_SCL_Pin=0):((LoadEDIDSelectPort == DVI1)?(hw_DVI_EDID_SCL_Pin=0):((LoadEDIDSelectPort == HDMI1)?(hw_HDMI_EDID_SCL_Pin=0):(hwI2C_SCL_Pin=0))))
#define i2c_SCLHi()         ((LoadEDIDSelectPort == VGA1)?(hw_VGA_EDID_SCL_Pin):((LoadEDIDSelectPort == DVI1)?(hw_DVI_EDID_SCL_Pin):((LoadEDIDSelectPort == HDMI1)?(hw_HDMI_EDID_SCL_Pin):(hwI2C_SCL_Pin))))
#define i2c_SCLLo()         ((LoadEDIDSelectPort == VGA1)?(!(hw_VGA_EDID_SCL_Pin)):((LoadEDIDSelectPort == DVI1)?(!(hw_DVI_EDID_SCL_Pin)):((LoadEDIDSelectPort == HDMI1)?(!(hw_HDMI_EDID_SCL_Pin)):(!(hwI2C_SCL_Pin)))))
#else
#define Set_i2c_SDA()          hwI2C_SDA_Pin=1
#define Clr_i2c_SDA()           hwI2C_SDA_Pin=0
#define i2c_SDAHi()         (hwI2C_SDA_Pin)
#define i2c_SDALo()         (!(hwI2C_SDA_Pin))
#define Set_i2c_SCL()           hwI2C_SCL_Pin=1
#define Clr_i2c_SCL()           hwI2C_SCL_Pin=0
#define i2c_SCLHi()         (hwI2C_SCL_Pin)
#define i2c_SCLLo()         (!(hwI2C_SCL_Pin))
#endif
#if ENABLE_HDCP
#define hwHDCP_Hpd_Pin
#define hw_Set_HdcpHpd()
#define hw_Clr_HdcpHpd()
#endif


#define hw_DVISWITCH_Pin
#define hw_SEL_HDMI()
#define hw_SEL_DVI()

#endif

//==========================================================================================================================================================:
//output initial
#define MCU_P0_OUT_EN_INIT     		0
#define MCU_P0_FMD_INIT         		0
#define MCU_P0_ROD_EN_INIT          	(BIT7|BIT6)

#define MCU_P1_OUT_EN_INIT          	0
#define MCU_P1_FMD_INIT         		0
#define MCU_P1_ROD_EN_INIT          	0

#define MCU_P2_OUT_EN_INIT          	BIT4
#define MCU_P2_FMD_INIT         		BIT4
#define MCU_P2_ROD_EN_INIT          	0

#define MCU_P4_OUT_EN_INIT          	BIT1
#define MCU_P4_FMD_INIT         		BIT1
#define MCU_P4_ROD_EN_INIT          	0

#define GPIO 	0
#define SAR0 	BIT0
#define SAR1   	BIT1
#define SAR2      	BIT2
#define SAR3   	BIT3

#define SAR0_FUNC   		GPIO
#define SAR1_FUNC   		SAR1
#define SAR2_FUNC   		SAR2
#define SAR3_FUNC   		SAR3
#define SAR_FUNC    		SAR0_FUNC|SAR1_FUNC|SAR2_FUNC|SAR3_FUNC