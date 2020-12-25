
#ifndef __BD_MSTMCU__
#define __BD_MSTMCU__

#define MCU_SPEED_INDEX     IDX_MCU_CLK_144MHZ
#define SPI_SPEED_INDEX     IDX_SPI_CLK_43MHZ
#define SPI_MODE            SPI_MODE_FR

#define DVI_PORT                TMDS_PORT_A//TMDS_PORT_UNUSE
#define DVI_DDC_PORT            TMDS_PORT_A//TMDS_PORT_UNUSE
#define DVI_MASTER_PORT         TMDS_PORT_A//TMDS_PORT_UNUSE
//set to 0 if not DVI dual port enable
#define DVI_SLAVE_PORT          TMDS_PORT_UNUSE

#if CHIP_ID == CHIP_TSUMB && ENABLE_HDMI // 88B
#define HDMI_PORT               TMDS_PORT_B
#define HDMI_HPD_PORT           TMDS_PORT_B
#define HDMI_DDC_PORT           TMDS_PORT_B
#else
#define HDMI_PORT               TMDS_PORT_UNUSE
#define HDMI_HPD_PORT           TMDS_PORT_UNUSE
#define HDMI_DDC_PORT           TMDS_PORT_UNUSE
#endif

#define SERIAL_BAUD_RATE           9600

#define BrightnessPWM          _PWM1_
#define Init_BrightnessPWM()    { MEM_MSWRITE_BYTE_MASK(REG_1B08,BIT1,BIT1);MEM_MSWRITE_BYTE_MASK(REG_0280,BIT4,BIT4);}

#if AudioFunc
#define VolumePWM               _PWM6_
#define Init_VolumePWM()
#endif

#define PowerKey               (_bit0_(MEM_MSREAD_BYTE(_REG_GPIO2_IN)))
#define Init_PowerKey()     (MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, BIT0, BIT0))

#define NonUsepin

//========================================================
// 1.Write protection:
#define hwDDC_WP_Pin
#define hwEEPROM_WP          // GPIO03
#define hwFlash_WP_Pin       // GPIO10

// ddc eeprom wp be pulled high to 5V, set to input for output high level
#define hw_ClrDDC_WP()
#define hw_SetDDC_WP()
#define Init_hwDDC_WP_Pin()

#define Set_EEPROM_WP()     (MEM_MSWRITE_BIT(_REG_GPIO0_OUT, 1, BIT3))
#define Clr_EEPROM_WP()     (MEM_MSWRITE_BIT(_REG_GPIO0_OUT, 0, BIT3))
#define Init_hwEEPROM_WP()  (MEM_MSWRITE_BIT(_REG_GPIO0_OEZ, 0, BIT3)) // output enable

#define hw_SetFlashWP()         (MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, 1, BIT0))
#define hw_ClrFlashWP()         (MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, 0, BIT0))
#define Init_hwFlash_WP_Pin()   (MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, 0, BIT0)) // output enable
//========================================================
// 2.Backlight and panelon:
#define hwBlacklit_Pin      // GPIO10
#define hwPanel_Pin         // GPIO12

#define hw_SetBlacklit()    (MEM_MSWRITE_BIT(_REG_GPIO0_OUT, 1, BIT1), BackLightActiveFlag=1)
#define hw_ClrBlacklit()    (MEM_MSWRITE_BIT(_REG_GPIO0_OUT, 0, BIT1), BackLightActiveFlag=0)
#define Init_hwBlacklit_Pin()   (MEM_MSWRITE_BIT(_REG_GPIO0_OEZ, 0, BIT1))// output enable

#define hw_SetPanel()       (MEM_MSWRITE_BIT(_REG_GPIO1_OUT, 1, BIT2))
#define hw_ClrPanel()       (MEM_MSWRITE_BIT(_REG_GPIO1_OUT, 0, BIT2))
#define Init_hwPanel_Pin()  (MEM_MSWRITE_BIT(_REG_GPIO1_OEZ, 0, BIT2)) // output enable
//========================================================
// 4.Detect Cable
#define CABLE_DETECT_VGA_USE_SAR        0
#define CABLE_DETECT_DVI_USE_SAR        0 // MHL
#define CABLE_DETECT_HDMI_USE_SAR       1 // pure HDMI

#define hwDSUBCable_Pin        (_bit2_(MEM_MSREAD_BYTE(_REG_PMGPIO0_IN)))
#define Init_hwDSUBCable_Pin()  (MEM_MSWRITE_BIT(_REG_PMGPIO0_OEZ,1,BIT2)) // input enable
#define CABLE_DET_VGA_SAR    0
#define hwDVICable_Pin      (_bit3_(MEM_MSREAD_BYTE(_REG_PMGPIO0_IN)))
#define Init_hwDVICable_Pin()   (MEM_MSWRITE_BIT(_REG_PMGPIO0_OEZ,1,BIT3)) // input enable

#if ENABLE_HDMI
#if CABLE_DETECT_HDMI_USE_SAR
#define hwHDMICable_Pin
#define Init_hwHDMICable_Pin()
#define CABLE_DET_HDMI_SAR              SAR3_EN // sar3 1.57V
#else
#define hwHDMICable_Pin                 //(_bit0_(MEM_MSREAD_BYTE(_REG_PMGPIOSAR_IN)))
#define Init_hwHDMICable_Pin()          //(MEM_MSWRITE_BIT(REG_0220, 1, BIT4)) // input enable,  [7:4] map to sar3~0
#define CABLE_DET_HDMI_SAR              0
#endif
#else
#define hwHDMICable_Pin
#define Init_hwHDMICable_Pin()
#define CABLE_DET_HDMI_SAR              0
#endif
#define CABLE_DET_SAR_EN                (CABLE_DET_VGA_SAR|CABLE_DET_HDMI_SAR)
#define CABLE_DET_SAR                   0
//========================================================
// 5.Led
#define hwGreenLed_Pin      // GPIO24
#define hwAmberLed_Pin     // GPIO25

#define hw_SetGreenLed()            (MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT, 1, BIT4))
#define hw_ClrGreenLed()            (MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT, 0, BIT4))
#define Init_hwGreenLed_Pin()   (MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, 0, BIT4)) // output enable

#define hw_SetAmberLed()           (MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT, 1, BIT5))
#define hw_ClrAmberLed()           (MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT, 0, BIT5))
#define Init_hwAmberLed_Pin()   (MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, 0, BIT5)) // output enable

//========================================================
// 6.Audio control:
#define hwMute_Pin
#define hw_SetMute()
#define hw_ClrMute()
#define Init_hwMute_Pin()

#define hwAudio_SD_Pin() //GPIO02
#define hw_SetAudio_SD()                    //MEM_MSWRITE_BIT(_REG_GPIO0_OUT, 0, BIT2)
#define hw_ClrAudio_SD()                //MEM_MSWRITE_BIT(_REG_GPIO0_OUT, 1, BIT2)
#define Init_Audio_SD_Pin()         //MEM_MSWRITE_BIT(_REG_GPIO0_OEZ, 0, BIT2)
#define Init_SPIFGPIO_Pin()

//========================================================
// I2C
#if defined(UseVGACableReadWriteAllPortsEDID) //&&!defined(UseInternalDDCRam)
#define Set_i2c_SDA_VGA()          (MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, TRUE, BIT2))
#define Clr_i2c_SDA_VGA()          (MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, FALSE, BIT2), MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, FALSE, BIT2))
#define i2c_SDAHi_VGA()     (_bit2_(MEM_MSREAD_BYTE(_REG_PMGPIO1_IN)))
#define i2c_SDALo_VGA()     (!i2c_SDAHi())
#define Set_i2c_SCL_VGA()          (MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, TRUE, BIT3))
#define Clr_i2c_SCL_VGA()          (MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, FALSE, BIT3), MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, FALSE, BIT3))
#define i2c_SCLHi_VGA()     (_bit3_(MEM_MSREAD_BYTE(_REG_PMGPIO1_IN)))
#define i2c_SCLLo_VGA()     (!i2c_SCLHi())

#define Set_i2c_SDA_DVI()          (MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, TRUE, BIT1))
#define Clr_i2c_SDA_DVI()          (MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT, FALSE, BIT1), MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, FALSE, BIT1))
#define i2c_SDAHi_DVI()     (_bit1_(MEM_MSREAD_BYTE(_REG_PMGPIO2_IN)))
#define i2c_SDALo_DVI()     (!i2c_SDAHi())
#define Set_i2c_SCL_DVI()          (MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, TRUE, BIT2))
#define Clr_i2c_SCL_DVI()          (MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT, FALSE, BIT2), MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, FALSE, BIT2))
#define i2c_SCLHi_DVI()     (_bit2_(MEM_MSREAD_BYTE(_REG_PMGPIO2_IN)))
#define i2c_SCLLo_DVI()     (!i2c_SCLHi())

#define Set_i2c_SDA()       ((LoadEDIDSelectPort == DVI1)?(Set_i2c_SDA_DVI()):(Set_i2c_SDA_VGA()))
#define Clr_i2c_SDA()       ((LoadEDIDSelectPort == DVI1)?(Clr_i2c_SDA_DVI()):(Clr_i2c_SDA_VGA()))
#define i2c_SDAHi()         ((LoadEDIDSelectPort == DVI1)?(i2c_SDAHi_DVI()):(i2c_SDAHi_VGA()))
#define i2c_SDALo()         ((LoadEDIDSelectPort == DVI1)?(i2c_SDALo_DVI()):(i2c_SDALo_VGA()))

#define Set_i2c_SCL()           ((LoadEDIDSelectPort == DVI1)?(Set_i2c_SCL_DVI()):(Set_i2c_SCL_VGA()))
#define Clr_i2c_SCL()           ((LoadEDIDSelectPort == DVI1)?(Clr_i2c_SCL_DVI()):(Clr_i2c_SCL_VGA()))
#define i2c_SCLHi()         ((LoadEDIDSelectPort == DVI1)?(i2c_SCLHi_DVI()):(i2c_SCLHi_VGA()))
#define i2c_SCLLo()         ((LoadEDIDSelectPort == DVI1)?(i2c_SCLLo_DVI()):(i2c_SCLLo_VGA()))
#endif

#if !USEFLASH
#define hwI2C_SCL_Pin       // GPIO03
#define hwI2C_SDA_Pin      // GPIO02
#define Init_hwI2C_SCL_Pin()    (MEM_MSWRITE_BIT(_REG_GPIO0_OEZ, TRUE, BIT3))
#define Init_hwI2C_SDA_Pin()    (MEM_MSWRITE_BIT(_REG_GPIO0_OEZ, TRUE, BIT2))

#define Set_i2c_SDA()          (MEM_MSWRITE_BIT(_REG_GPIO0_OEZ, TRUE, BIT2))
#define Clr_i2c_SDA()          (MEM_MSWRITE_BIT(_REG_GPIO0_OUT, FALSE, BIT2), MEM_MSWRITE_BIT(_REG_GPIO0_OEZ, FALSE, BIT2))
#define i2c_SDAHi()     (_bit2_(MEM_MSREAD_BYTE(_REG_GPIO0_IN)))
#define i2c_SDALo()     (!i2c_SDAHi())
#define Set_i2c_SCL()          (MEM_MSWRITE_BIT(_REG_GPIO0_OEZ, TRUE, BIT3))
#define Clr_i2c_SCL()          (MEM_MSWRITE_BIT(_REG_GPIO0_OUT, FALSE, BIT3), MEM_MSWRITE_BIT(_REG_GPIO0_OEZ, FALSE, BIT3))
#define i2c_SCLHi()     (_bit3_(MEM_MSREAD_BYTE(_REG_GPIO0_IN)))
#define i2c_SCLLo()     (!i2c_SCLHi())
#endif

#if ENABLE_HDCP
#if CHIP_ID == CHIP_TSUMB && ENABLE_HDMI // 88B
#define hwHDCP_Hpd_Pin      // HDMI GPIO36
#define hw_Set_HdcpHpd()            (MEM_MSWRITE_BIT(_REG_GPIO3_OUT, FALSE, BIT6))
#define hw_Clr_HdcpHpd()            (MEM_MSWRITE_BIT(_REG_GPIO3_OUT, TRUE, BIT6))
#define hw_Set_HdcpHpd2()
#define hw_Clr_HdcpHpd2()
#define hw_Set_HdcpHpd3()
#define hw_Clr_HdcpHpd3()
#define Init_hwHDCP_Hpd_Pin()   (MEM_MSWRITE_BIT(_REG_GPIO3_OEZ, FALSE, BIT6))
#else
#define hwHDCP_Hpd_Pin
#define hw_Set_HdcpHpd()
#define hw_Clr_HdcpHpd()
#define hw_Set_HdcpHpd2()
#define hw_Clr_HdcpHpd2()
#define hw_Set_HdcpHpd3()
#define hw_Clr_HdcpHpd3()
#define Init_hwHDCP_Hpd_Pin()
#endif
#endif

#if ENABLE_USB_INPUT
#define hwUSB_Pin
#define hw_USB_HI()
#define hw_USB_LOW()
#define Init_hwUSB_Pin();
#endif

// MHL PS_CTRL
#define SET_MHL_PS_CTRL_CHARGE0()    //(MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, TRUE, BIT7))
#define CLR_MHL_PS_CTRL_CHARGE0()    //(MEM_MSWRITE_BIT(_REG_PMGPIO3_OUT, FALSE, BIT7), MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, FALSE, BIT7))

#define SET_MHL_PS_CTRL_CHARGE1()    //(MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, TRUE, BIT7))
#define CLR_MHL_PS_CTRL_CHARGE1()    //(MEM_MSWRITE_BIT(_REG_PMGPIO3_OUT, FALSE, BIT7), MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, FALSE, BIT7))

#define SET_MHL_PS_CTRL_CHARGE2()    //(MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, TRUE, BIT7))
#define CLR_MHL_PS_CTRL_CHARGE2()    //(MEM_MSWRITE_BIT(_REG_PMGPIO3_OUT, FALSE, BIT7), MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, FALSE, BIT7))

#if HDMI_PRETEST
#define Init_hwCEC_Pin()             //no need
#endif

//SARKEY_ENABLE_BIT
#define SAR0_EN     BIT0
#define SAR1_EN     BIT1
#define SAR2_EN     BIT2
#define SAR3_EN     BIT3
#define SARKEY_EN   (SAR1_EN|SAR2_EN)

#define KEYPAD_ADC_A        KEYPAD_SAR1
#define KEYPAD_ADC_B        KEYPAD_SAR2
#define AOVDV_VALUE     4

#define LVDS_CHANNEL    (MOD_PORTB|MOD_PORTC)//(MOD_PORTA|MOD_PORTB|MOD_PORTC|MOD_PORTD)
#define I_GEN_CH        CH3_CLK

///////////////////////////////
// check h/w setting
#define Vin_Boost               12// Unit:V
#define L_value                  22 // Unit:uH
#define L_Variation                 30 // %,0~100%

#define ResistorRatio                       46  //(R2+R1)/R1  100K and 2.2K

#define CSA_gain      1 //for 100m
#define Rsense      100 //for 0.1ohm
#define Rsense_Variation                 25 // % ,0~100%

#define BOOST_CURRENT_LIMIT_VALUE		2  // 0:3A/1:4A/2:5A/3:6.5A
#endif


