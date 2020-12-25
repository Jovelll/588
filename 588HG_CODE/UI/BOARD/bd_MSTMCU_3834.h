#ifndef __MAINBOARD_3834__
#define __MAINBOARD_3834__
#include "types.h"
#define CPU_CLOCK_KHZ       28636
#define CPU_CLOCK_MHZ       (CPU_CLOCK_KHZ/1000)
#ifdef TSUMXXNX
#define XTAL_CLOCK_KHZ      14318
#define MST_CLOCK_MHZ       XTAL_CLOCK_KHZ
#else
#define MST_CLOCK_MHZ       28636
#endif
#define SERIAL_BAUD_RATE    9600
#define UsePWMBrightness    1
#define BrightnessPWM       PWM1C
#define VolumePWM                 PWM0C

#define PowerKey                   P1_4
//extern void SetScalerGPIO(BYTE pinX, bit HiLow);

//================================================
// Detect Cable
// DVI and HDMI detect by SAR4
#define hwDSUBCable_Pin     P1_5
#define hwDVICable_Pin  P1_3
//#define hwHDMICable_Pin   P1_4
//================================================
// Led
#define hwGreenLed_Pin      P0_6
#define hwAmberLed_Pin      P0_7
#define hwBlacklit_Pin      P1_2
#define hwPanel_Pin                   P2_7
#define hwEEPROM_WP     // useless
#define hwFLASH_WP              P2_4
//================================================
// mStar I2C
#define hwMS_ResetZ_Pin     P3_7

//#define UseInternalDDCRam
#ifdef UseInternalDDCRam
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
#define Set_i2c_SDA()       hwI2C_SDA_Pin=1
#define Clr_i2c_SDA()       hwI2C_SDA_Pin=0
#define i2c_SDAHi()     (hwI2C_SDA_Pin)
#define i2c_SDALo()     (!(hwI2C_SDA_Pin))
#define Set_i2c_SCL()       hwI2C_SCL_Pin=1
#define Clr_i2c_SCL()       hwI2C_SCL_Pin=0
#define i2c_SCLHi()     (hwI2C_SCL_Pin)
#define i2c_SCLLo()     (!(hwI2C_SCL_Pin))
#endif

#if ModelName==OTS_US200DALW
#define hw_SetGreenLed()        hwGreenLed_Pin=1
#define hw_ClrGreenLed()        hwGreenLed_Pin=0
#define hw_SetAmberLed()    hwAmberLed_Pin=1
#define hw_ClrAmberLed()    hwAmberLed_Pin=0
#else
#define hw_SetGreenLed()        hwGreenLed_Pin=0
#define hw_ClrGreenLed()        hwGreenLed_Pin=1
#define hw_SetAmberLed()    hwAmberLed_Pin=0
#define hw_ClrAmberLed()    hwAmberLed_Pin=1
#endif


#define hw_SetBlacklit()    hwBlacklit_Pin=0
#define hw_ClrBlacklit()    hwBlacklit_Pin=1

#define hw_SetPanel()       hwPanel_Pin =1
#define hw_ClrPanel()       hwPanel_Pin =0


#define hwMute_Pin                  P2_3
#define hw_SetMute()            hwMute_Pin=0
#define hw_ClrMute()                hwMute_Pin=1

#define hwStandby_Pin       P2_3
#define hw_SetStandby()         hwStandby_Pin=0
#define hw_ClrStandby()            hwStandby_Pin=1


#define hwDDC_WP_Pin        P2_2
#define hw_SetDDC_WP()      hwDDC_WP_Pin=0//(SetScalerGPIO(12, 0))
#define hw_ClrDDC_WP()      hwDDC_WP_Pin=1//(SetScalerGPIO(12, 1))

#define Set_EEPROM_WP()         // useless
#define Clr_EEPROM_WP()         // useless
#define hw_SetFlashWP()     hwFLASH_WP=0
#define hw_ClrFlashWP()     hwFLASH_WP=1

//#define hw_Set_AV_Mute()       UselessPin=1
//#define hw_Clr_AV_Mute()        UselessPin=1
//#define hw_Audio_HDMI()         UselessPin=1
//#define hw_Audio_PC()              UselessPin=1

#if ENABLE_HDCP

#define hwHDCP_Hpd_Pin             // P4
#define hw_Set_HdcpHpd()           // hwHDCP_Hpd_Pin&=~BIT0
#define hw_Clr_HdcpHpd()           //hwHDCP_Hpd_Pin|=BIT0

//#define hw_Set_HdcpHpd()  (SetScalerGPIO(12, 1))//hwHDCP_Hpd_Pin=1
//#define hw_Clr_HdcpHpd()  (SetScalerGPIO(12, 0))//hwHDCP_Hpd_Pin=0

//#define hw_Set_HdcpHpd_DVI()   (SetScalerGPIO(6, 1))
//#define hw_Clr_HdcpHpd_DVI()  (SetScalerGPIO(6, 0))
#endif

#define MCU_P0_OUT_EN_INIT     		( BIT6 | BIT7 )
#define MCU_P0_FMD_INIT         		( BIT6 | BIT7 )
#define MCU_P0_ROD_EN_INIT          	0

#define MCU_P1_OUT_EN_INIT          	( BIT2 | BIT6 )
#define MCU_P1_FMD_INIT         		( BIT2 | BIT6 )
#define MCU_P1_ROD_EN_INIT          	0

#define MCU_P2_OUT_EN_INIT          	( BIT2 | BIT3 | BIT4 | BIT7 )
#define MCU_P2_FMD_INIT         		( BIT2 | BIT3 | BIT4 | BIT7 )
#define MCU_P2_ROD_EN_INIT          	0

#define MCU_P4_OUT_EN_INIT          	( BIT0 | BIT1 | BIT2 )
#define MCU_P4_FMD_INIT         		( BIT0 | BIT1 | BIT2 )
#define MCU_P4_ROD_EN_INIT          	0
#endif

