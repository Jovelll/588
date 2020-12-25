
#ifndef __BD_MSTMCU__
#define __BD_MSTMCU__

#define MCU_SPEED_INDEX     IDX_MCU_CLK_108MHZ
#define SPI_SPEED_INDEX     IDX_SPI_CLK_43MHZ
#define SPI_MODE            SPI_MODE_FR

#define DVI_PORT                TMDS_PORT_A
#define DVI_HPD_PORT           TMDS_PORT_A
#define DVI_DDC_PORT            TMDS_PORT_A
#define DVI_MASTER_PORT         TMDS_PORT_A
//set to 0 if not DVI dual port enable
#define DVI_SLAVE_PORT          TMDS_PORT_UNUSE

#define HDMI_PORT               TMDS_PORT_B//TMDS_PORT_UNUSE
#define HDMI_HPD_PORT           TMDS_PORT_B//TMDS_PORT_UNUSE
#define HDMI_DDC_PORT           TMDS_PORT_B//TMDS_PORT_UNUSE

#define SERIAL_BAUD_RATE           9600

#define BrightnessPWM          _PWM4_
#define Init_BrightnessPWM()    (MEM_MSWRITE_BYTE_MASK(REG_0213,BIT3,BIT3), MEM_MSWRITE_BYTE_MASK(REG_0280,BIT0,BIT0))

#if AudioFunc
#define VolumePWM               _PWM5_
#define Init_VolumePWM()    (MEM_MSWRITE_BYTE_MASK(REG_0213,BIT5,BIT5), MEM_MSWRITE_BYTE_MASK(REG_0280,BIT1,BIT1))
#endif

#define PowerKey               (_bit0_(MEM_MSREAD_BYTE(_REG_PMGPIO0_IN)))
#define Init_PowerKey()     (MEM_MSWRITE_BIT(_REG_PMGPIO0_OEZ, 1, BIT0))

#define NonUsepin           

//========================================================
// 1.Write protection:
#define hwDDC_WP_Pin           // PMGPIO25
#define hwEEPROM_WP          // PMGPIO20
#define hwFlash_WP_Pin       // PMGPIO10

// ddc eeprom wp be pulled high to 5V, set to input for output high level
#define hw_SetDDC_WP()      (MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT, 0, BIT5), MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, 0, BIT5), DDCWPActiveFlag=0)//(MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, 1, BIT5), DDCWPActiveFlag=1)
#define hw_ClrDDC_WP()      (MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT, 0, BIT5), MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, 0, BIT5), DDCWPActiveFlag=0)
#define Init_hwDDC_WP_Pin() MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, 1, BIT5) // output disable

#define Set_EEPROM_WP()     MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT, 1, BIT0)
#define Clr_EEPROM_WP()      MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT, 0, BIT0)
#define Init_hwEEPROM_WP()  MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, 0, BIT0) // output enable

#define hw_SetFlashWP()         MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, 0, BIT0)
#define hw_ClrFlashWP()          MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, 1, BIT0)
#define Init_hwFlash_WP_Pin()   MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, 0, BIT0) // output enable
//========================================================
// 2.Backlight and panelon:
#define hwBlacklit_Pin      // GPIO03
#define hwPanel_Pin         // GPIO26

#define hw_SetBlacklit()    (MEM_MSWRITE_BIT(_REG_PMGPIO0_OUT, 0, BIT3), BackLightActiveFlag=1)
#define hw_ClrBlacklit()    (MEM_MSWRITE_BIT(_REG_PMGPIO0_OUT, 1, BIT3), BackLightActiveFlag=0)
#define Init_hwBlacklit_Pin()   MEM_MSWRITE_BIT(_REG_PMGPIO0_OEZ, 0, BIT3) // output enable

#define hw_SetPanel()          MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT, 1, BIT6)
#define hw_ClrPanel()          MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT, 0, BIT6)
#define Init_hwPanel_Pin()     MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, 0, BIT6)
//========================================================
// 4.Detect Cable
#define CABLE_DETECT_VGA_USE_SAR    1
#define CABLE_DETECT_DVI_USE_SAR    0 // MHL
#define CABLE_DETECT_HDMI_USE_SAR    1 // pure HDMI

#if CABLE_DETECT_VGA_USE_SAR
#define hwDSUBCable_Pin        
#define Init_hwDSUBCable_Pin()  
#define CABLE_DET_VGA_SAR    SAR3_EN // 2.8V
#else
#define hwDSUBCable_Pin        //(_bit2_(MEM_MSREAD_BYTE(_REG_PMGPIO0_IN)))
#define Init_hwDSUBCable_Pin()  //(MEM_MSWRITE_BIT(_REG_PMGPIO0_OEZ,1,BIT2)) // input enable
#define CABLE_DET_VGA_SAR    0
#endif

#if CABLE_DETECT_DVI_USE_SAR
#define hwDVICable_Pin      
#define Init_hwDVICable_Pin()  
#define CABLE_DET_DVI_SAR    SAR0_EN
#else
#define hwDVICable_Pin      (_bit0_(MEM_MSREAD_BYTE(_REG_PMGPIOSAR_IN)))
#define Init_hwDVICable_Pin()   (MEM_MSWRITE_BIT(REG_0220, 1, BIT4)) // input enable,  [7:4] map to sar3~0
#define CABLE_DET_DVI_SAR    0
#endif

//#define hwDSUBCable_Pin        // sar 3 2.8V //(_bit3_(MEM_MSREAD_BYTE(_REG_PMGPIOSAR_IN)))
//#define Init_hwDSUBCable_Pin()  //(MEM_MSWRITE_BIT(_REG_PMGPIOSAR_OEZ,1,BIT3)) // input enable

//#define hwDVICable_Pin      //(_bit3_(MEM_MSREAD_BYTE(_REG_PMGPIO0_IN)))
//#define Init_hwDVICable_Pin()   //(MEM_MSWRITE_BIT(_REG_PMGPIO0_OEZ,1,BIT3)) // input enable

#if ENABLE_HDMI
//#define hwHDMICable_Pin // sar3 1.57V // both VGA/HDMI cable in: 0.9V
//#define Init_hwHDMICable_Pin()
#if CABLE_DETECT_HDMI_USE_SAR
#define hwHDMICable_Pin      
#define Init_hwHDMICable_Pin()  
#define CABLE_DET_HDMI_SAR    SAR3_EN // sar3 1.57V
#else
#define hwHDMICable_Pin      //(_bit0_(MEM_MSREAD_BYTE(_REG_PMGPIOSAR_IN)))
#define Init_hwHDMICable_Pin()   //(MEM_MSWRITE_BIT(REG_0220, 1, BIT4)) // input enable,  [7:4] map to sar3~0
#define CABLE_DET_HDMI_SAR    0
#endif

#endif

#define CABLE_DET_SAR_EN    (CABLE_DET_VGA_SAR|CABLE_DET_HDMI_SAR)
#define CABLE_DET_SAR           KEYPAD_SAR3

//========================================================
// 5.Led
#define hwGreenLed_Pin      // PMGPIO22 65pin
#define hwAmberLed_Pin     // PMGPIO21 64pin

#define hw_SetGreenLed()            (MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT, 0, BIT2))
#define hw_ClrGreenLed()            MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT, 1, BIT2)
#define Init_hwGreenLed_Pin()   MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, 0, BIT2) // output enable

#define hw_SetAmberLed()           MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT, 0, BIT1)
#define hw_ClrAmberLed()           MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT, 1, BIT1)
#define Init_hwAmberLed_Pin()   MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, 0, BIT1) // output enable

//========================================================
// 6.Audio control:
#define hwMute_Pin      // PMGPIO14       
#define hw_SetMute()    (MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, FALSE, BIT4))
#define hw_ClrMute()    (MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, TRUE, BIT4))
#define Init_hwMute_Pin()   (MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, FALSE, BIT4))

#define hw_SetAudio_SD()    
#define hw_ClrAudio_SD()    
#define Init_Audio_SD_Pin()
#define Init_SPIFGPIO_Pin() 

//========================================================
// I2C
#if defined(UseVGACableReadWriteAllPortsEDID)
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
#define hwI2C_SCL_Pin       // PMGPIO16
#define hwI2C_SDA_Pin      // PMGPIO17
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
#define hwHDCP_Hpd_Pin // PMGPIO_32 for HDMI2    
#define hw_Set_HdcpHpd()    	(MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, TRUE, BIT2))	
#define hw_Clr_HdcpHpd() 		(MEM_MSWRITE_BIT(_REG_PMGPIO3_OUT, FALSE, BIT2), MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, FALSE, BIT2))
#define Init_hwHDCP_Hpd_Pin()   (MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, TRUE, BIT2))
#endif

#if ENABLE_USB_INPUT
#define hwUSB_Pin
#define hw_USB_HI()
#define hw_USB_LOW()
#define Init_hwUSB_Pin();
#endif

#if ENABLE_TOUCH_KEY
#define SK_SCL_Pin       // GPIO_SAR2
#define SK_SDA_Pin      // GPIO_SAR1
#define SK_INT_Pin       // PMGPIO00

#define Init_SK_SCL()    (MEM_MSWRITE_BIT(_REG_PMGPIOSAR_OEZ, TRUE, BIT6))
#define Init_SK_SDA()    (MEM_MSWRITE_BIT(_REG_PMGPIOSAR_OEZ, TRUE, BIT5))
#define Init_SK_INT()    (MEM_MSWRITE_BIT(_REG_PMGPIO0_OEZ, TRUE, BIT0))

#define SK_INT_ACIVE()     (_bit0_(MEM_MSREAD_BYTE(_REG_PMGPIO0_IN)))

#define Set_SK_SCL()    (MEM_MSWRITE_BIT(_REG_PMGPIOSAR_OEZ, TRUE, BIT6))
#define Clr_SK_SCL()     (MEM_MSWRITE_BIT(_REG_PMGPIOSAR_OUT, 0, BIT2), MEM_MSWRITE_BIT(_REG_PMGPIOSAR_OEZ, 0, BIT6))
#define SK_SCLHi()     (_bit2_(MEM_MSREAD_BYTE(_REG_PMGPIOSAR_IN)))
#define SK_SCLLo()     (!SK_SCLHi())

#define Set_SK_SDA()    (MEM_MSWRITE_BIT(_REG_PMGPIOSAR_OEZ, TRUE, BIT5))
#define Clr_SK_SDA()     (MEM_MSWRITE_BIT(_REG_PMGPIOSAR_OUT, 0, BIT1), MEM_MSWRITE_BIT(_REG_PMGPIOSAR_OEZ, 0, BIT5))
#define SK_SDAHi()      (_bit1_(MEM_MSREAD_BYTE(_REG_PMGPIOSAR_IN)))
#define SK_SDALo()      (!SK_SDAHi())
#endif

//////////////////////////////////////////////////////
// power board control
//////////////////////////////////////////////////////
#if ENABLE_POWER_BOARD_CONTROL
//  PMGPIO11
#define Set_PowerBoardSaving_Pin()            (MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, 1, BIT1)) // power board saving mode
#define Clr_PowerBoardSaving_Pin()            (MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, 0, BIT1)) // power board active mode
#define Init_PowerBoardSaving_Pin()   (MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, 0, BIT1)) // output enable
#endif

//SARKEY_ENABLE_BIT
#define SAR0_EN     BIT0
#define SAR1_EN     BIT1
#define SAR2_EN     BIT2
#define SAR3_EN     BIT3
#define SARKEY_EN   (0)

#define KEYPAD_ADC_A        0xff//KEYPAD_SAR1
#define KEYPAD_ADC_B        0xff//KEYPAD_SAR2
#define AOVDV_VALUE     4

#define LVDS_CHANNEL    (MOD_PORTB|MOD_PORTC)
#define I_GEN_CH        CH3_CLK

#if ENABLE_MHL
#define SET_SWI2C_SCL()       MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, TRUE, BIT4) //PMGPIO24
#define CLR_SWI2C_SCL()       (MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT,FALSE, BIT4), MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ,FALSE, BIT4))
#define IS_SWI2C_SCL_HI()     (_bit4_(MEM_MSREAD_BYTE(_REG_PMGPIO2_IN)))
#define IS_SWI2C_SCL_LO()     (!IS_SWI2C_SCL_HI())
#define Init_SWI2C_SCL()        (MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, TRUE, BIT4))

#define SET_SWI2C_SDA()       MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, TRUE, BIT3) //PMGPIO23
#define CLR_SWI2C_SDA()       (MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT,FALSE, BIT3), MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ,FALSE, BIT3))
#define IS_SWI2C_SDA_HI()     (_bit3_(MEM_MSREAD_BYTE(_REG_PMGPIO2_IN)))
#define IS_SWI2C_SDA_LO()     (!IS_SWI2C_SDA_HI())
#define Init_SWI2C_SDA()        (MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, TRUE, BIT3))

//#define SET_SWI2C_BUS_OUT()   MEM_MSWRITE_BYTE_MASK(_REG_PMGPIO2_OEZ, 0x00, BIT6|BIT7)

// MHL cable detect
#define SET_MHL_CABLE_DETECT()    MEM_MSWRITE_BIT(_REG_PMGPIO0_OEZ, TRUE, BIT1)    // PMGPIO01
#define IS_MHL_CABLE_HI()     (_bit1_(MEM_MSREAD_BYTE(_REG_PMGPIO0_IN)))
#define IS_MHL_CABLE_LO()     (!IS_MHL_CABLE_HI())
#define Init_MHL_CABLE_DETECT_Pin() (MEM_MSWRITE_BIT(_REG_PMGPIO0_OEZ, TRUE, BIT1) )

#endif

// MHL PS_CTRL
//#define SET_MHL_PS_CTRL_OUT()    MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, FALSE, BIT7)    // PMGPIO37
#if 0 // 120323 ok after plugging cable, fail in monitor ac power on
#define SET_MHL_PS_CTRL_CHARGE0()    (MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, TRUE, BIT7))
#define CLR_MHL_PS_CTRL_CHARGE0()    (MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, TRUE, BIT7))//(MEM_MSWRITE_BIT(_REG_PMGPIO3_OUT, FALSE, BIT7), MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, FALSE, BIT7))

#define SET_MHL_PS_CTRL_CHARGE1()    (MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, TRUE, BIT7))
#define CLR_MHL_PS_CTRL_CHARGE1()    (MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, TRUE, BIT7))//(MEM_MSWRITE_BIT(_REG_PMGPIO3_OUT, FALSE, BIT7), MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, FALSE, BIT7))

#define SET_MHL_PS_CTRL_CHARGE2()    (MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, TRUE, BIT7))
#define CLR_MHL_PS_CTRL_CHARGE2()    (MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, TRUE, BIT7))//(MEM_MSWRITE_BIT(_REG_PMGPIO3_OUT, FALSE, BIT7), MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, FALSE, BIT7))

#else
#define SET_MHL_PS_CTRL_CHARGE0()    (MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, TRUE, BIT7))
#define CLR_MHL_PS_CTRL_CHARGE0()    (MEM_MSWRITE_BIT(_REG_PMGPIO3_OUT, FALSE, BIT7), MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, FALSE, BIT7))

#define SET_MHL_PS_CTRL_CHARGE1()    (MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, TRUE, BIT7))
#define CLR_MHL_PS_CTRL_CHARGE1()    (MEM_MSWRITE_BIT(_REG_PMGPIO3_OUT, FALSE, BIT7), MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, FALSE, BIT7))

#define SET_MHL_PS_CTRL_CHARGE2()    (MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, TRUE, BIT7))
#define CLR_MHL_PS_CTRL_CHARGE2()    (MEM_MSWRITE_BIT(_REG_PMGPIO3_OUT, FALSE, BIT7), MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, FALSE, BIT7))

#endif

#if ENABLE_TOUCH_PANEL || ENABLE_TOUCH_PANEL_DATA_FROM_USB || ENABLE_TOUCH_PANEL_CTRL_OSD

#if ENABLE_TOUCH_PANEL_DATA_FROM_USB
#define USB_INT // PMGPIO33
#define Init_USB_INT()  (MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, TRUE, BIT3))
#define USB_INT_ACTIVE()    (_bit3_(MEM_MSREAD_BYTE(_REG_PMGPIO3_IN)))
#endif

#define Set_TOCUCH_PANEL_POWER()
#define Clr_TOCUCH_PANEL_POWER()
#define Init_TOCUCH_PANEL_POWER_Pin()

#define SPI_MISO_Pin   (_bit2_(MEM_MSREAD_BYTE(_REG_PMGPIO1_IN))) 
#define Init_SPI_MISO_Pin() (MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, TRUE, BIT2))	// PMGPIO12				

#define Set_SPI_SCK()   (MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, TRUE, BIT3))
#define Clr_SPI_SCK()   (MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, FALSE, BIT3))
#define Init_SPI_SCK_Pin()  (MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, FALSE, BIT3))  // PMGPIO13

#define Set_SPI_MOSI()  (MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, TRUE, BIT4))
#define Clr_SPI_MOSI()  (MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, FALSE, BIT4))
#define Init_SPI_MOSI_Pin() (MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, FALSE, BIT4)) // PMGPIO14

#define Set_SPI_SEL()  (MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, TRUE, BIT5))
#define Clr_SPI_SEL()   (MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, FALSE, BIT5))
#define Init_SPI_SEL_Pin() (MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, FALSE, BIT5))// PMGPIO15
#endif

#if HDMI_PRETEST
#define Init_hwCEC_Pin()    //no need
#endif

#endif


