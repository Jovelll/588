/* For Sargent Compatible */
#ifndef __BD_MSTMCU__
#define __BD_MSTMCU__

#define BDName               "HG588V00R0XB05"


#if CHIP_ID==CHIP_TSUMF
#define MCU_SPEED_INDEX         IDX_MCU_CLK_144MHZ    
#else
#define MCU_SPEED_INDEX         IDX_MCU_CLK_MPLL_DIV    //IDX_MCU_CLK_MPLL_DIV // 172M
#endif
#define SPI_SPEED_INDEX         IDX_SPI_CLK_43MHZ
#define SPI_MODE                SPI_MODE_FR
#define HPD_INVERSE             1 // If it has BJT circuit on board, the HPD_INVERSE should be set to TRUE
#if CHIP_ID==CHIP_TSUMJ
#define SPI_SSC_EN              0//(SPI_SPEED_INDEX==IDX_SPI_CLK_SSC)// use memory clock source, need to enable memclk SSC, and divide it to the one closest & lower to SPI_SPEED_INDEX
#else
#define SPI_SSC_EN              (SPI_SPEED_INDEX==IDX_SPI_CLK_SSC)// use memory clock source, need to enable memclk SSC, and divide it to the one closest & lower to SPI_SPEED_INDEX
#endif
#define SPI_SSC_SOURCE          0 // 0:mempll, 1:lpll (Current code flow for mempll only)
#define SPI_SSC_DIVIDER         4 // Target Freq => (MEMORY_CLOCK_MHZ*(100+PERCENTAGE)/(SPI_SSC_DIVIDER+1))

#define DFT_MUX_Uart0           UART0_GPIO40_41//UART0_GPIO40_41 // default mux uart 0 - VGA
#define DFT_MUX_Uart1           UART1_GPIO32_33

// Mapping to InputPortType
//If using Solar board need to swap DVI <--swap-->  HDMI!!
#define Input_DVI_C1            Input_Nothing
#define Input_DVI_C2            Input_Nothing  //Input_Digital
#define Input_DVI_C3            Input_Nothing
#define Input_HDMI_C1           Input_Digital//Input_Nothing
#define Input_HDMI_C2           Input_Nothing
#define Input_HDMI_C3           Input_Nothing
#define Input_Displayport_C2    Input_Nothing
#define Input_Displayport_C3    Input_Nothing

// For Preprocessing
#define CInput_Nothing          0
#define CInput_Digital          1
#define CInput_Digital2         2
#define CInput_Digital3         3

#define CInput_DVI_C1           CInput_Nothing
#define CInput_DVI_C2           CInput_Nothing   //CInput_Digital
#define CInput_DVI_C3           CInput_Nothing
#define CInput_HDMI_C1          CInput_Digital  //CInput_Nothing
#define CInput_HDMI_C2          CInput_Nothing
#define CInput_HDMI_C3          CInput_Nothing
#define CInput_Displayport_C2   CInput_Nothing
#define CInput_Displayport_C3   CInput_Nothing

#define DVI_PORT                ( ((CInput_DVI_C1!=CInput_Nothing)?TMDS_PORT_A:TMDS_PORT_UNUSE) | ((CInput_DVI_C2!=CInput_Nothing)?TMDS_PORT_B:TMDS_PORT_UNUSE) | ((CInput_DVI_C3!=CInput_Nothing)?TMDS_PORT_C:TMDS_PORT_UNUSE) )
#define DVI_HPD_PORT            ( ((CInput_DVI_C1!=CInput_Nothing)?TMDS_PORT_A:TMDS_PORT_UNUSE) | ((CInput_DVI_C2!=CInput_Nothing)?TMDS_PORT_B:TMDS_PORT_UNUSE) | ((CInput_DVI_C3!=CInput_Nothing)?TMDS_PORT_C:TMDS_PORT_UNUSE) )
#define DVI_DDC_PORT            ( ((CInput_DVI_C1!=CInput_Nothing)?TMDS_PORT_A:TMDS_PORT_UNUSE) | ((CInput_DVI_C2!=CInput_Nothing)?TMDS_PORT_B:TMDS_PORT_UNUSE) | ((CInput_DVI_C3!=CInput_Nothing)?TMDS_PORT_C:TMDS_PORT_UNUSE) )
#define DVI_MASTER_PORT         ( ((CInput_DVI_C1!=CInput_Nothing)?TMDS_PORT_A:TMDS_PORT_UNUSE) | ((CInput_DVI_C2!=CInput_Nothing)?TMDS_PORT_B:TMDS_PORT_UNUSE) | ((CInput_DVI_C3!=CInput_Nothing)?TMDS_PORT_C:TMDS_PORT_UNUSE) )

//set to 0 if not DVI dual port enable
#define DVI_SLAVE_PORT          TMDS_PORT_UNUSE

#define HDMI_PORT               ( ((CInput_HDMI_C1!=CInput_Nothing)?TMDS_PORT_A:TMDS_PORT_UNUSE) | ((CInput_HDMI_C2!=CInput_Nothing)?TMDS_PORT_B:TMDS_PORT_UNUSE) | ((CInput_HDMI_C3!=CInput_Nothing)?TMDS_PORT_C:TMDS_PORT_UNUSE) )
#define HDMI_HPD_PORT           ( ((CInput_HDMI_C1!=CInput_Nothing)?TMDS_PORT_A:TMDS_PORT_UNUSE) | ((CInput_HDMI_C2!=CInput_Nothing)?TMDS_PORT_B:TMDS_PORT_UNUSE) | ((CInput_HDMI_C3!=CInput_Nothing)?TMDS_PORT_C:TMDS_PORT_UNUSE) )
#define HDMI_DDC_PORT           ( ((CInput_HDMI_C1!=CInput_Nothing)?TMDS_PORT_A:TMDS_PORT_UNUSE) | ((CInput_HDMI_C2!=CInput_Nothing)?TMDS_PORT_B:TMDS_PORT_UNUSE) | ((CInput_HDMI_C3!=CInput_Nothing)?TMDS_PORT_C:TMDS_PORT_UNUSE) )

// DDC port used
#define DDC_Port_D0             ((CInput_DVI_C1!=CInput_Nothing) || (CInput_HDMI_C1!=CInput_Nothing))
#define DDC_Port_D1             ((CInput_DVI_C2!=CInput_Nothing) || (CInput_HDMI_C2!=CInput_Nothing))
#define DDC_Port_D2             ((CInput_DVI_C3!=CInput_Nothing) || (CInput_HDMI_C3!=CInput_Nothing))
#define DDC_Port_Sel()          (MEM_MSWRITE_BYTE_MASK(REG_0260, 0, BIT3|BIT2)) // DDCD0 0x0260[3:2] = 00 : on GPIO32/GPIO33 ; 01 : on GPIO34/GPIO35 , 10/11: reserve


#define SERIAL_BAUD_RATE        9600

#define BrightnessPWM           _PWM0_ // Driving Non-LED BL Module
//#if (BrightnessPWM==_PWM7_)
#define Init_BrightnessPWM()    (MEM_MSWRITE_BYTE_MASK(REG_0208,BIT0,BIT0), MEM_MSWRITE_BYTE_MASK(REG_0280,BIT0,BIT0))
//#else  // _PWM6_
//#define Init_BrightnessPWM()    (MEM_MSWRITE_BYTE_MASK(REG_0208,BIT6,BIT6), MEM_MSWRITE_BYTE_MASK(REG_0281,BIT2,BIT2))
//#endif

#if AudioFunc
#define VolumePWM                // Not Support
#define Init_VolumePWM()        //(MEM_MSWRITE_BYTE_MASK(REG_0208,BIT5,BIT5), MEM_MSWRITE_BYTE_MASK(REG_0281,BIT2,BIT2))
#endif

#if ENABLE_XTAL_LESS // need to rework if use XTAL
#define PowerKey                (_bit3_(MEM_MSREAD_BYTE(_REG_GPIO1_IN)))
#define Init_PowerKey()       (MEM_MSWRITE_BIT(_REG_GPIO1_OEZ, 1, BIT3))
#define PM_POWERKEY_WAKEUP      0//GPIO20_INT
#else
#define PowerKey               (_bit4_(MEM_MSREAD_BYTE(_REG_GPIO0_IN)))// (_bit0_(MEM_MSREAD_BYTE(_REG_GPIO0_IN)))
#define Init_PowerKey()        (MEM_MSWRITE_BIT(_REG_GPIO0_OEZ, 1, BIT4))// (MEM_MSWRITE_BIT(_REG_GPIO0_OEZ, 1, BIT0))
#define PM_POWERKEY_WAKEUP      GPIO00_INT
#endif
#define NonUsepin
//================================================================================================================
//================================================================================================================
// 1.Write protection:
#define hwDDC_WP_Pin            //Pull to Ground in Schematic
#define hwEEPROM_WP             //GPIO03
#define hwFlash_WP_Pin          //GPIO10

// ddc eeprom wp be pulled high to 5V, set to input for output high level
#define hw_SetDDC_WP()          //(MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT, 1, BIT5), DDCWPActiveFlag=1)//(MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, 0, BIT6), MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, 0, BIT6), DDCWPActiveFlag=0)
#define hw_ClrDDC_WP()          //(MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT, 0, BIT5), DDCWPActiveFlag=0)
#define Init_hwDDC_WP_Pin()     //MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, 0, BIT5)//(MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, 1, BIT6)) // output disable

#if ENABLE_XTAL_LESS // need to rework if use XTAL
#define Set_EEPROM_WP()         MEM_MSWRITE_BIT(_REG_GPIO0_OUT, 1, BIT0)
#define Clr_EEPROM_WP()         MEM_MSWRITE_BIT(_REG_GPIO0_OUT, 0, BIT0)
#define Init_hwEEPROM_WP()      MEM_MSWRITE_BIT(_REG_GPIO0_OEZ, 0, BIT0)
#else
#define Set_EEPROM_WP()         
#define Clr_EEPROM_WP()         
#define Init_hwEEPROM_WP()      
#endif

#define hw_SetFlashWP()         MEM_MSWRITE_BIT(_REG_GPIO1_OUT, 0, BIT0)
#define hw_ClrFlashWP()         MEM_MSWRITE_BIT(_REG_GPIO1_OUT, 1, BIT0)
#define Init_hwFlash_WP_Pin()   MEM_MSWRITE_BIT(_REG_GPIO1_OEZ, 0, BIT0)

//================================================================================================================
//================================================================================================================
// 2.Backlight and panelon:
#define hwBlacklit_Pin          //GPIO01  //Driving Non-LED BL Module
#define hwPanel_Pin             //GPIO00
#define hw_SetBlacklit()       (MEM_MSWRITE_BIT(_REG_GPIO2_OUT, 1, BIT0), BackLightActiveFlag=1)// (MEM_MSWRITE_BIT(_REG_GPIO0_OUT, 0, BIT2), BackLightActiveFlag=1)
#define hw_ClrBlacklit()        (MEM_MSWRITE_BIT(_REG_GPIO2_OUT, 0, BIT0), BackLightActiveFlag=0)
#define Init_hwBlacklit_Pin()   (MEM_MSWRITE_BIT(_REG_GPIO2_OEZ, 0, BIT0)) // output enable

#define hw_SetPanel()           MEM_MSWRITE_BIT(_REG_GPIO2_OUT, 1, BIT1)
#define hw_ClrPanel()           MEM_MSWRITE_BIT(_REG_GPIO2_OUT, 0, BIT1)
#define Init_hwPanel_Pin()      (MEM_MSWRITE_BIT(_REG_GPIO2_OEZ, 0, BIT1)) // output enable

//================================================================================================================
//================================================================================================================
// 4.Detect Cable
#define CABLE_DETECT_VGA_USE_SAR        0
//#define CABLE_DETECT_DVI_USE_SAR        0
//#define CABLE_DETECT_HDMI_USE_SAR       1

#if CABLE_DETECT_VGA_USE_SAR
#define hwDSUBCable_Pin                 0
#define Init_hwDSUBCable_Pin()
#define CABLE_DET_VGA_SAR               0
#else
#define hwDSUBCable_Pin             (_bit2_(MEM_MSREAD_BYTE(_REG_GPIO1_IN)))//GPIO12  //  (_bit2_(MEM_MSREAD_BYTE(_REG_GPIOSAR_IN)))
#define Init_hwDSUBCable_Pin()        (MEM_MSWRITE_BIT(_REG_GPIO1_OEZ, 1, BIT2))// (MEM_MSWRITE_BIT(_REG_GPIOSAR_OEZ, 1, BIT3))// (MEM_MSWRITE_BIT(_REG_GPIOSAR_OEZ, 1, BIT3))
#define CABLE_DET_VGA_SAR               0
#endif


#if (CInput_HDMI_C1!=CInput_Nothing)||(CInput_DVI_C1!=CInput_Nothing)   // Digital Input Port 0
#define hwDVI0Cable_Pin                 0//(KEYPAD_SAR0)   // SAR0
#define Init_hwDVI0Cable_Pin()          //(MEM_MSWRITE_BIT(REG_3A68,1,BIT0))  // Enable SAR ch
#define CABLE_DET_DVI0_SAR             0 //SAR0_EN
#else
#define hwDVI0Cable_Pin                 0
#define Init_hwDVI0Cable_Pin()
#define CABLE_DET_DVI0_SAR              0
#endif

#if (CInput_HDMI_C2!=CInput_Nothing)||(CInput_DVI_C2!=CInput_Nothing)   // Digital Input Port 1
#define hwDVI1Cable_Pin                 (KEYPAD_SAR3)    // SAR3
#define Init_hwDVI1Cable_Pin()          (MEM_MSWRITE_BIT(REG_3A68,1,BIT3))  // Enable SAR ch
#define CABLE_DET_DVI1_SAR              SAR3_EN
#else
#define hwDVI1Cable_Pin                 0
#define Init_hwDVI1Cable_Pin()
#define CABLE_DET_DVI1_SAR              0
#endif

/* In fact, this input is not used in this board. */
#if (CInput_HDMI_C3!=CInput_Nothing)||(CInput_DVI_C3!=CInput_Nothing)
#define hwDVI2Cable_Pin                 0
#define Init_hwDVI2Cable_Pin()
#define CABLE_DET_DVI2_SAR              0
#else
#define hwDVI2Cable_Pin                 0
#define Init_hwDVI2Cable_Pin()
#define CABLE_DET_DVI2_SAR              0
#endif

#define DVI5V_GPIO_Sel()                (MEM_MSWRITE_BYTE_MASK(REG_0260, 0, BIT5)) // 5V Det source 0x0260[5] = 0 : GPIO30 ; 1 : GPIO37
#define CABLE_DET_SAR_EN                (CABLE_DET_VGA_SAR|CABLE_DET_DVI1_SAR)
#define CABLE_DET_SAR                   0
#define CABLE_DET_SAR2_EN               (CABLE_DET_DVI0_SAR|CABLE_DET_DVI2_SAR)

//================================================================================================================
//================================================================================================================
// 5.Led
#define hwGreenLed_Pin                  // GPIO22
#define hwAmberLed_Pin                  // GPIO25

#define hw_SetGreenLed()                 {MEM_MSWRITE_BIT(_REG_GPIO2_OEZ,0,BIT3);MEM_MSWRITE_BIT(_REG_GPIO2_OUT, 0, BIT3);MEM_MSWRITE_BIT(_REG_GPIO2_OEZ,0,BIT2);MEM_MSWRITE_BIT(_REG_GPIO2_OUT,1,BIT2);}
#define hw_ClrGreenLed()                 {MEM_MSWRITE_BIT(_REG_GPIO2_OEZ,0,BIT3);MEM_MSWRITE_BIT(_REG_GPIO2_OUT, 1, BIT3);MEM_MSWRITE_BIT(_REG_GPIO2_OEZ,0,BIT2);MEM_MSWRITE_BIT(_REG_GPIO2_OUT,0,BIT2);}
#define Init_hwGreenLed_Pin()            MEM_MSWRITE_BIT(_REG_GPIO2_OEZ, 0,BIT3)// output enable
#define hw_SetAmberLed()                {MEM_MSWRITE_BIT(_REG_GPIO2_OEZ,0,BIT2);MEM_MSWRITE_BIT(_REG_GPIO2_OUT,0,BIT2);MEM_MSWRITE_BIT(_REG_GPIO2_OEZ,0,BIT3);MEM_MSWRITE_BIT(_REG_GPIO2_OUT, 1, BIT3);}
#define hw_ClrAmberLed()                {MEM_MSWRITE_BIT(_REG_GPIO2_OEZ,0,BIT2);MEM_MSWRITE_BIT(_REG_GPIO2_OUT,1,BIT2);MEM_MSWRITE_BIT(_REG_GPIO2_OEZ,0,BIT3);MEM_MSWRITE_BIT(_REG_GPIO2_OUT, 0, BIT3);}
#define Init_hwAmberLed_Pin()           MEM_MSWRITE_BIT(_REG_GPIO2_OEZ, 0,BIT2)// output enable
//#define hw_SetGreenLed()                {MEM_MSWRITE_BIT(_REG_GPIO2_OEZ,0,BIT2);MEM_MSWRITE_BIT(_REG_GPIO2_OUT,1,BIT2);}
//#define hw_ClrGreenLed()                {MEM_MSWRITE_BIT(_REG_GPIO2_OEZ,0,BIT2);MEM_MSWRITE_BIT(_REG_GPIO2_OUT,0,BIT2);}
//#define Init_hwGreenLed_Pin()           MEM_MSWRITE_BIT(_REG_GPIO2_OEZ, 0,BIT2)// output enable

//#define hw_SetAmberLed()                {MEM_MSWRITE_BIT(_REG_GPIO2_OEZ,0,BIT3);MEM_MSWRITE_BIT(_REG_GPIO2_OUT, 0, BIT3);}
//#define hw_ClrAmberLed()                {MEM_MSWRITE_BIT(_REG_GPIO2_OEZ,0,BIT3);MEM_MSWRITE_BIT(_REG_GPIO2_OUT, 1, BIT3);}
//#define Init_hwAmberLed_Pin()           MEM_MSWRITE_BIT(_REG_GPIO2_OEZ, 0,BIT3)// output enable

//================================================================================================================
//================================================================================================================
// 6.Audio control:
#define hwMute_Pin                      //NO_USE_PIN
#define hw_SetMute()                    //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, FALSE, BIT4))
#define hw_ClrMute()                    //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, TRUE, BIT4))
#define Init_hwMute_Pin()               //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, FALSE, BIT4))

#define hw_SetAudio_SD()                //NO_USE_PIN
#define hw_ClrAudio_SD()
#define Init_Audio_SD_Pin()
#define Init_SPIFGPIO_Pin()   //{MEM_MSWRITE_BIT(REG_0206,BIT2,BIT2);}//[0]S/PDIF output control on GPIO01
                                                                    //[2]S/PDIF output control on GPIO_AU5
//Since GPIO01 is the power switch on demo board, output S/PDIF on this pad will cause latch up when AC on.
//The register definition is wrong.

//================================================================================================================
//================================================================================================================
// I2C
#if defined(UseVGACableReadWriteAllPortsEDID) //&&!defined(UseInternalDDCRam)
#define Set_i2c_SDA_VGA()           //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, TRUE, BIT2))
#define Clr_i2c_SDA_VGA()           //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, FALSE, BIT2), MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, FALSE, BIT2))
#define i2c_SDAHi_VGA()             //(_bit2_(MEM_MSREAD_BYTE(_REG_PMGPIO1_IN)))
#define i2c_SDALo_VGA()             //(!i2c_SDAHi())
#define Set_i2c_SCL_VGA()           //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, TRUE, BIT3))
#define Clr_i2c_SCL_VGA()           //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, FALSE, BIT3), MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, FALSE, BIT3))
#define i2c_SCLHi_VGA()             //(_bit3_(MEM_MSREAD_BYTE(_REG_PMGPIO1_IN)))
#define i2c_SCLLo_VGA()             //(!i2c_SCLHi())

#define Set_i2c_SDA_DVI()           //(MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, TRUE, BIT1))
#define Clr_i2c_SDA_DVI()           //(MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT, FALSE, BIT1), MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, FALSE, BIT1))
#define i2c_SDAHi_DVI()             //(_bit1_(MEM_MSREAD_BYTE(_REG_PMGPIO2_IN)))
#define i2c_SDALo_DVI()             //(!i2c_SDAHi())
#define Set_i2c_SCL_DVI()           //(MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, TRUE, BIT2))
#define Clr_i2c_SCL_DVI()           //(MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT, FALSE, BIT2), MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, FALSE, BIT2))
#define i2c_SCLHi_DVI()             //(_bit2_(MEM_MSREAD_BYTE(_REG_PMGPIO2_IN)))
#define i2c_SCLLo_DVI()             //(!i2c_SCLHi())

#define Set_i2c_SDA()               //((LoadEDIDSelectPort == DVI1)?(Set_i2c_SDA_DVI()):(Set_i2c_SDA_VGA()))
#define Clr_i2c_SDA()               //((LoadEDIDSelectPort == DVI1)?(Clr_i2c_SDA_DVI()):(Clr_i2c_SDA_VGA()))
#define i2c_SDAHi()                 0//((LoadEDIDSelectPort == DVI1)?(i2c_SDAHi_DVI()):(i2c_SDAHi_VGA()))
#define i2c_SDALo()                 0//((LoadEDIDSelectPort == DVI1)?(i2c_SDALo_DVI()):(i2c_SDALo_VGA()))

#define Set_i2c_SCL()               //((LoadEDIDSelectPort == DVI1)?(Set_i2c_SCL_DVI()):(Set_i2c_SCL_VGA()))
#define Clr_i2c_SCL()               //((LoadEDIDSelectPort == DVI1)?(Clr_i2c_SCL_DVI()):(Clr_i2c_SCL_VGA()))
#define i2c_SCLHi()                 0//((LoadEDIDSelectPort == DVI1)?(i2c_SCLHi_DVI()):(i2c_SCLHi_VGA()))
#define i2c_SCLLo()                 0//((LoadEDIDSelectPort == DVI1)?(i2c_SCLLo_DVI()):(i2c_SCLLo_VGA()))
#endif

#if !USEFLASH
#define hwI2C_SCL_Pin               // GPIO04
#define hwI2C_SDA_Pin               // GPIO05
#define Init_hwI2C_SCL_Pin()        (MEM_MSWRITE_BIT(_REG_GPIO0_OEZ, TRUE, BIT4))
#define Init_hwI2C_SDA_Pin()        (MEM_MSWRITE_BIT(_REG_GPIO0_OEZ, TRUE, BIT5))

#define Set_i2c_SDA()               (MEM_MSWRITE_BIT(_REG_GPIO0_OEZ, TRUE, BIT5))
#define Clr_i2c_SDA()               (MEM_MSWRITE_BIT(_REG_GPIO0_OUT, FALSE, BIT5), MEM_MSWRITE_BIT(_REG_GPIO0_OEZ, FALSE, BIT5))
#define i2c_SDAHi()                 (_bit5_(MEM_MSREAD_BYTE(_REG_GPIO0_IN)))
#define i2c_SDALo()                 (!i2c_SDAHi())
#define Set_i2c_SCL()               (MEM_MSWRITE_BIT(_REG_GPIO0_OEZ, TRUE, BIT4))
#define Clr_i2c_SCL()               (MEM_MSWRITE_BIT(_REG_GPIO0_OUT, FALSE, BIT4), MEM_MSWRITE_BIT(_REG_GPIO0_OEZ, FALSE, BIT4))
#define i2c_SCLHi()                 (_bit4_(MEM_MSREAD_BYTE(_REG_GPIO0_IN)))
#define i2c_SCLLo()                 (!i2c_SCLHi())
#endif

#if ENABLE_HDCP
#define hwHDCP_Hpd_Pin //GPIO36 = Port A, GPIO31 = Port B

#if HPD_INVERSE
#define hw_Set_HdcpHpd()      (MEM_MSWRITE_BIT(_REG_GPIO3_OUT, TRUE, BIT1), MEM_MSWRITE_BIT(_REG_GPIO3_OEZ, FALSE, BIT1))// (MEM_MSWRITE_BIT(_REG_GPIO3_OUT, FALSE, BIT1), MEM_MSWRITE_BIT(_REG_GPIO3_OEZ, FALSE, BIT1))
#define hw_Clr_HdcpHpd()       (MEM_MSWRITE_BIT(_REG_GPIO3_OUT, FALSE, BIT1), MEM_MSWRITE_BIT(_REG_GPIO3_OEZ, FALSE, BIT1))
#define Init_hwHDCP_Hpd_Pin()  (MEM_MSWRITE_BIT(_REG_GPIO3_OUT, FALSE, BIT1), MEM_MSWRITE_BIT(_REG_GPIO3_OEZ, FALSE, BIT1)) //default high
#define hw_Set_HdcpHpd2()        //(MEM_MSWRITE_BIT(_REG_GPIO3_OUT, FALSE, BIT6), MEM_MSWRITE_BIT(_REG_GPIO3_OEZ, FALSE, BIT6))
#define hw_Clr_HdcpHpd2()        //(MEM_MSWRITE_BIT(_REG_GPIO3_OUT, TRUE, BIT6), MEM_MSWRITE_BIT(_REG_GPIO3_OEZ, FALSE, BIT6))
#define Init_hwHDCP_Hpd_Pin2()   //(MEM_MSWRITE_BIT(_REG_GPIO3_OUT, FALSE, BIT6), MEM_MSWRITE_BIT(_REG_GPIO3_OEZ, FALSE, BIT6)) //default high
//#define hw_Set_HdcpHpd2()       (MEM_MSWRITE_BIT(_REG_GPIO3_OUT, FALSE, BIT1), MEM_MSWRITE_BIT(_REG_GPIO3_OEZ, FALSE, BIT1))
//#define hw_Clr_HdcpHpd2()       (MEM_MSWRITE_BIT(_REG_GPIO3_OUT, TRUE, BIT1), MEM_MSWRITE_BIT(_REG_GPIO3_OEZ, FALSE, BIT1))
//#define Init_hwHDCP_Hpd_Pin2()  (MEM_MSWRITE_BIT(_REG_GPIO3_OUT, FALSE, BIT1), MEM_MSWRITE_BIT(_REG_GPIO3_OEZ, FALSE, BIT1)) //default high
#define hw_Set_HdcpHpd3()        // no use in this Board.
#define hw_Clr_HdcpHpd3()
#define Init_hwHDCP_Hpd_Pin3()
#else
#define hw_Set_HdcpHpd()        //(MEM_MSWRITE_BIT(_REG_GPIO3_OUT, TRUE, BIT6), MEM_MSWRITE_BIT(_REG_GPIO3_OEZ, TRUE, BIT6), MEM_MSWRITE_BIT(REG_0262, FALSE, BIT3), MEM_MSWRITE_BIT(REG_0265, TRUE, BIT4))
#define hw_Clr_HdcpHpd()        //(MEM_MSWRITE_BIT(_REG_GPIO3_OUT, FALSE, BIT6), MEM_MSWRITE_BIT(_REG_GPIO3_OEZ, FALSE, BIT6), MEM_MSWRITE_BIT(REG_0262, FALSE, BIT3), MEM_MSWRITE_BIT(REG_0265, FALSE, BIT4))
#define Init_hwHDCP_Hpd_Pin()   //(MEM_MSWRITE_BIT(_REG_GPIO3_OUT, FALSE, BIT6), MEM_MSWRITE_BIT(_REG_GPIO3_OEZ, FALSE, BIT6), MEM_MSWRITE_BIT(REG_0262, FALSE, BIT3), MEM_MSWRITE_BIT(REG_0265, FALSE, BIT4)) //default low
#define hw_Set_HdcpHpd2()       (MEM_MSWRITE_BIT(_REG_GPIO3_OUT, TRUE, BIT1), MEM_MSWRITE_BIT(_REG_GPIO3_OEZ, TRUE, BIT1), MEM_MSWRITE_BIT(REG_0262, FALSE, BIT2), MEM_MSWRITE_BIT(REG_0265, TRUE, BIT3))
#define hw_Clr_HdcpHpd2()       (MEM_MSWRITE_BIT(_REG_GPIO3_OUT, FALSE, BIT1), MEM_MSWRITE_BIT(_REG_GPIO3_OEZ, FALSE, BIT1), MEM_MSWRITE_BIT(REG_0262, FALSE, BIT2), MEM_MSWRITE_BIT(REG_0265, FALSE, BIT3))
#define Init_hwHDCP_Hpd_Pin2()  (MEM_MSWRITE_BIT(_REG_GPIO3_OUT, FALSE, BIT1), MEM_MSWRITE_BIT(_REG_GPIO3_OEZ, FALSE, BIT1), MEM_MSWRITE_BIT(REG_0262, FALSE, BIT2), MEM_MSWRITE_BIT(REG_0265, FALSE, BIT3)) //default low
#define hw_Set_HdcpHpd3()        // no use in this Board.
#define hw_Clr_HdcpHpd3()
#define Init_hwHDCP_Hpd_Pin3()
#define hw_Set_MHLTriStateHpd()        //(MEM_MSWRITE_BIT(REG_0262, FALSE, BIT3), MEM_MSWRITE_BIT(REG_0265, FALSE, BIT4), MEM_MSWRITE_BIT(_REG_GPIO3_OUT, TRUE, BIT6), MEM_MSWRITE_BIT(_REG_GPIO3_OEZ, TRUE, BIT6))
#define hw_Set_MHLTriStateHpd2()        //(MEM_MSWRITE_BIT(REG_0262, FALSE, BIT2), MEM_MSWRITE_BIT(REG_0265, FALSE, BIT3), MEM_MSWRITE_BIT(_REG_GPIO3_OUT, TRUE, BIT1), MEM_MSWRITE_BIT(_REG_GPIO3_OEZ, TRUE, BIT1))
#define hw_Set_MHLTriStateHpd3()        // no use in this Board.
#endif //HPD_INVERSE
#else
#define hw_Set_HdcpHpd()
#define hw_Clr_HdcpHpd()
#define Init_hwHDCP_Hpd_Pin()
#define hw_Set_HdcpHpd2()
#define hw_Clr_HdcpHpd2()
#define Init_hwHDCP_Hpd_Pin2()
#define hw_Set_HdcpHpd3()
#define hw_Clr_HdcpHpd3()
#define Init_hwHDCP_Hpd_Pin3()
#define hw_Set_MHLTriStateHpd()
#define hw_Set_MHLTriStateHpd2()
#define hw_Set_MHLTriStateHpd3()
#endif //ENABLE_HDCP

#if ENABLE_USB_INPUT
#define hwUSB_Pin
#define hw_USB_HI()
#define hw_USB_LOW()
#define Init_hwUSB_Pin();
#endif

//SARKEY_ENABLE_BIT
#define SAR0_EN     BIT0
#define SAR1_EN     BIT1
#define SAR2_EN     BIT2
#define SAR3_EN     BIT3
#define SARKEY_EN   (SAR0_EN|SAR1_EN|SAR2_EN)

#define SAR4_EN     BIT0
#define SAR5_EN     BIT1
#define SAR6_EN     BIT2
#define SAR7_EN     BIT3

#define KEYPAD_ADC_A        KEYPAD_SAR0
#define KEYPAD_ADC_B        KEYPAD_SAR1
#define AOVDV_VALUE     4

//#define LVDS_CHANNEL    (MOD_PORTB|MOD_PORTC)//(MOD_PORTA|MOD_PORTB|MOD_PORTC|MOD_PORTD)
//#define I_GEN_CH        CH2

#if ENABLE_MHL // Not Finishhed Yet in CHIP_TSUMC
// SWI2C for Eland
#define SET_SWI2C_SCL()             //MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT, TRUE, BIT7)         //PMGPIO27
#define CLR_SWI2C_SCL()             //MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT,FALSE, BIT7)
#define IS_SWI2C_SCL_HI()           0//(_bit7_(MEM_MSREAD_BYTE(_REG_PMGPIO2_IN)))
#define IS_SWI2C_SCL_LO()           0//(!IS_SWI2C_SCL_HI())
#define Init_SWI2C_SCL()            //(MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, TRUE, BIT7))

#define SET_SWI2C_SDA()             //MEM_MSWRITE_BIT(_REG_PMGPIO3_OUT, TRUE, BIT6)         //PMGPIO36
#define CLR_SWI2C_SDA()             //MEM_MSWRITE_BIT(_REG_PMGPIO3_OUT,FALSE, BIT6)
#define IS_SWI2C_SDA_HI()           0//(_bit6_(MEM_MSREAD_BYTE(_REG_PMGPIO3_IN)))
#define IS_SWI2C_SDA_LO()           0//(!IS_SWI2C_SDA_HI())
#define Init_SWI2C_SDA()            //(MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, TRUE, BIT6))
//#define SET_SWI2C_BUS_OUT()   MEM_MSWRITE_BYTE_MASK(_REG_PMGPIO2_OEZ, 0x00, BIT6|BIT7)
// MHL cable detect
#define Init_MHL_CABLE_DETECT_Pin() //(MEM_MSWRITE_BIT(_REG_PMGPIO0_OEZ, TRUE, BIT1) )
#define SET_MHL_CABLE_DETECT()      //MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, TRUE, BIT4)    // PMGPIO14
#define IS_MHL_CABLE_HI()           0//(_bit4_(MEM_MSREAD_BYTE(_REG_PMGPIO1_IN)))
#define IS_MHL_CABLE_LO()           0//(!IS_MHL_CABLE_HI())


#endif

// MHL PS_CTRL
#define SET_MHL_PS_CTRL_CHARGE0()    //(MEM_MSWRITE_BIT(_REG_GPIO2_OEZ, 0, BIT1), MEM_MSWRITE_BIT(_REG_GPIO2_OUT, 0, BIT1))
#define CLR_MHL_PS_CTRL_CHARGE0()    //(MEM_MSWRITE_BIT(_REG_GPIO2_OEZ, 0, BIT1), MEM_MSWRITE_BIT(_REG_GPIO2_OUT, 1, BIT1))

// In 72 pins board, the PS_CTRL is inversed compared to C2
#define SET_MHL_PS_CTRL_CHARGE1()    (MEM_MSWRITE_BIT(_REG_GPIO2_OEZ, 0, BIT0), MEM_MSWRITE_BIT(_REG_GPIO2_OUT, 1, BIT0))
#define CLR_MHL_PS_CTRL_CHARGE1()    (MEM_MSWRITE_BIT(_REG_GPIO2_OEZ, 0, BIT0), MEM_MSWRITE_BIT(_REG_GPIO2_OUT, 0, BIT0))

#define SET_MHL_PS_CTRL_CHARGE2()    //(MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, TRUE, BIT7))
#define CLR_MHL_PS_CTRL_CHARGE2()    //(MEM_MSWRITE_BIT(_REG_PMGPIO3_OUT, FALSE, BIT7), MEM_MSWRITE_BIT(_REG_PMGPIO3_OEZ, FALSE, BIT7))

#if ENABLE_TOUCH_PANEL
#define Set_TOCUCH_PANEL_POWER()
#define Clr_TOCUCH_PANEL_POWER()
#define Init_TOCUCH_PANEL_POWER_Pin()
#define SPI_MISO_Pin                //(_bit2_(MEM_MSREAD_BYTE(_REG_PMGPIO1_IN)))
#define Init_SPI_MISO_Pin()         //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, TRUE, BIT2))   // PMGPIO12

#define Set_SPI_SCK()               //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, TRUE, BIT3))
#define Clr_SPI_SCK()               //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, FALSE, BIT3))
#define Init_SPI_SCK_Pin()          //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, FALSE, BIT3))  // PMGPIO13

#define Set_SPI_MOSI()              //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, TRUE, BIT4))
#define Clr_SPI_MOSI()              //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, FALSE, BIT4))
#define Init_SPI_MOSI_Pin()         //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, FALSE, BIT4)) // PMGPIO14

#define Set_SPI_SEL()               //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, TRUE, BIT5))
#define Clr_SPI_SEL()               //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, FALSE, BIT5))
#define Init_SPI_SEL_Pin()          //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, FALSE, BIT5))// PMGPIO15
#endif

#if HDMI_PRETEST
#define Init_hwCEC_Pin()            (MEM_MSWRITE_BIT(REG_0262, TRUE, BIT0))
#endif

//for LED function
// check h/w setting
#define Vin_Boost               12// Unit:V
#define L_value                  22 // Unit:uH
#define L_Variation                 30 // %,0~100%

#define ResistorRatio                       46  //(R2+R1)/R1  100K and 2.2K

#define CSA_gain      1 //for 100m
#define Rsense      100 //for 0.1ohm
#define Rsense_Variation                 25 // % ,0~100%
#endif


