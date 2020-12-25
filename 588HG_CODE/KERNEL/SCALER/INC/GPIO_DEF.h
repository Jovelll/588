#ifndef _GPIO_DEF_H
#define _GPIO_DEF_H


#if CHIP_ID==CHIP_TSUM2
#define _REG_PMGPIO0_OUT    REG_0200  //PMGPIO00 ~ PMGPIO07 output value
#define _REG_PMGPIO0_OEZ    REG_0201  //PMGPIO00 ~ PMGPIO07 OEN control
#define _REG_PMGPIO1_OUT    REG_0202  //PMGPIO12 ~ PMGPIO17 output value
#define _REG_PMGPIO1_OEZ    REG_0203 //PMGPIO12 ~ PMGPIO17 OEN control
#define _REG_PMGPIO2_OUT    REG_0204 //PMGPIO20 ~ PMGPIO27 output value
#define _REG_PMGPIO2_OEZ    REG_0205 //PMGPIO20 ~ PMGPIO27 OEN control
#define _REG_PMGPIO3_OUT    REG_0206 //PMGPIO20 ~ PMGPIO27 output value
#define _REG_PMGPIO3_OEZ    REG_0207 //PMGPIO20 ~ PMGPIO27 OEN control
#define _REG_PMGPIO0_IN     REG_0208   //PMGPIO00 ~ PMGPIO07 input value
#define _REG_PMGPIO1_IN     REG_0209  //PMGPIO10 ~ PMGPIO17 input value
#define _REG_PMGPIO2_IN     REG_020A  //PMGPIO20 ~ PMGPIO27 input value
#define _REG_PMGPIO3_IN     REG_020B
#define _REG_CEC_EN         REG_020E

#define _REG_PMGPIOSAR_OUT  REG_0220  //PMGPIO_SAR0 ~ PMGPIO_SAR3 output value
#define _REG_PMGPIOSAR_OEZ  REG_0220  //PMGPIO_SAR0 ~ PMGPIO_SAR3 OEN control
#define _REG_PMGPIOSAR_IN   REG_0221  //PMGPIO_SAR0 ~ PMGPIO_SAR3 input value

#elif (CHIP_ID==CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK) || (CHIP_ID==CHIP_TSUMD) || (CHIP_ID==CHIP_TSUMJ) || (CHIP_ID==CHIP_TSUM9) || (CHIP_ID==CHIP_TSUMF)
#define _REG_GPIO0_OUT      REG_0220 //GPIO00 ~ GPIO06 output value
#define _REG_GPIO0_OEZ      REG_0222 //GPIO00 ~ GPIO06 OEN control
#define _REG_GPIO0_IN       REG_0224 //GPIO00 ~ GPIO06 input value
#define _REG_GPIO1_OUT      REG_0226 //GPIO10 ~ GPIO11 output value
#define _REG_GPIO1_OEZ      REG_0228 //GPIO10 ~ GPIO11 OEN control
#define _REG_GPIO1_IN       REG_022A //GPIO10 ~ GPIO11 input value
#define _REG_GPIO2_OUT      REG_022C //GPIO20 ~ GPIO27 output value
#define _REG_GPIO2_OEZ      REG_022E //GPIO20 ~ GPIO27 OEN control
#define _REG_GPIO2_IN       REG_0232 //GPIO20 ~ GPIO27 input value
#define _REG_GPIO3_OUT      REG_0234 //GPIO30 ~ GPIO37 output value
#define _REG_GPIO3_OEZ      REG_0236 //GPIO30 ~ GPIO37 OEN control
#define _REG_GPIO3_IN       REG_0238 //GPIO30 ~ GPIO37 input value
#define _REG_GPIO4_OUT      REG_023A //GPIO40 ~ GPIO42 output value
#define _REG_GPIO4_OEZ      REG_023B //GPIO40 ~ GPIO42 OEN control
#define _REG_GPIO4_IN       REG_023C //GPIO40 ~ GPIO42 input value
#define _REG_GPIO5_OUT      REG_023D //GPIO50 ~ GPIO56 output value
#define _REG_GPIO5_OEZ      REG_023E //GPIO50 ~ GPIO56 OEN control
#define _REG_GPIO5_IN       REG_023F //GPIO50 ~ GPIO56 input value
//MOD
#define _REG_GPIOM0_OUT     REG_0240 //GPIO_MOD0 ~ GPIO_MOD7 output value
#define _REG_GPIOM0_OEZ     REG_0242 //GPIO_MOD0 ~ GPIO_MOD7 OEN control
#define _REG_GPIOM0_IN      REG_0244 //GPIO_MOD0 ~ GPIO_MOD7 input value
#define _REG_GPIOM1_OUT     REG_0241 //GPIO_MOD8 ~ GPIO_MOD15 output value
#define _REG_GPIOM1_OEZ     REG_0243 //GPIO_MOD8 ~ GPIO_MOD15 OEN control
#define _REG_GPIOM1_IN      REG_0245 //GPIO_MOD8 ~ GPIO_MOD15 input value
#define _REG_GPIOM2_OUT     REG_0246 //GPIO_MOD16 ~ GPIO_MOD19 output value
#define _REG_GPIOM2_OEZ     REG_0247 //GPIO_MOD16 ~ GPIO_MOD19 OEN control
#define _REG_GPIOM2_IN      REG_0248 //GPIO_MOD16 ~ GPIO_MOD19 input value
// LED
#define _REG_GPIOL_SEL_L    REG_024C //GPIO_LED0 ~ GPIO_LED15 enable ,0: enable GPIO function ,1: disable GPIO function
#define _REG_GPIOL_SEL_H    REG_024D //GPIO_LED0 ~ GPIO_LED15 enable ,0: enable GPIO function ,1: disable GPIO function
#define _REG_GPIOL_OUT_L    REG_024E //GPIO_LED0 ~ GPIO_LED15 output value
#define _REG_GPIOL_OUT_H    REG_024F //GPIO_LED0 ~ GPIO_LED15 output value
#define _REG_GPIOL_OEZ_L    REG_0250 //GPIO_LED0 ~ GPIO_LED15 OEN control
#define _REG_GPIOL_OEZ_H    REG_0251 //GPIO_LED0 ~ GPIO_LED15 OEN control
#define _REG_GPIOL_IN_L     REG_0252 //GPIO_LED0 ~ GPIO_LED15 input value
#define _REG_GPIOL_IN_H     REG_0253 //GPIO_LED0 ~ GPIO_LED15 input value
// SAR
#define _REG_GPIOSAR_OUT    REG_0254 //[7:4] GPIO_SAR0 ~ GPIO_SAR3 output value
#define _REG_GPIOSAR_OEZ    REG_0255 //[3:0] GPIO_SAR0 ~ GPIO_SAR3 OEN control
#define _REG_GPIOSAR_IN     REG_0255 //[7:4] GPIO_SAR0 ~ GPIO_SAR3 input value
// AUDIO
#define _REG_GPIOAU_SEL     REG_025A //GPIO_AU0 ~ GPIO_AU6 enable ,0: enable GPIO function ,1: disable GPIO function
#define _REG_GPIOAU_OUT     REG_025B //GPIO_AU0 ~ GPIO_AU6 output value
#define _REG_GPIOAU_OEZ     REG_025C //GPIO_AU0 ~ GPIO_AU6 OEN control
#define _REG_GPIOAU_IN      REG_025F //GPIO_AU0 ~ GPIO_AU6 input value

#else
//for CHIP_TSUMU && CHIP_TSUML //CHIP_TSUMV
#define _REG_GPIO0_OUT      REG_1B20 //GPIO00 ~ GPIO07 output value
#define _REG_GPIO0_OEZ      REG_1B22 //GPIO00 ~ GPIO07 OEN control
#define _REG_GPIO0_IN       REG_1B24  //GPIO00 ~ GPIO07input value
#define _REG_GPIO1_OUT      REG_1B26 //GPIO10 ~ GPIO13 output value     //[CHIP_TSUMV] GPIO10 ~ GPIO16 output value
#define _REG_GPIO1_OEZ      REG_1B28 //GPIO10 ~ GPIO13 OEN control      //[CHIP_TSUMV] GPIO10 ~ GPIO16 OEN control
#define _REG_GPIO1_IN       REG_1B2A //GPIO10 ~ GPIO13 input value      //[CHIP_TSUMV] GPIO10 ~ GPIO16 input value
#define _REG_GPIO2_OUT      REG_1B2C //GPIO20 ~ GPIO27 output value     //[CHIP_TSUMV] GPIO20 ~ GPIO26 output value
#define _REG_GPIO2_OEZ      REG_1B2E //GPIO20 ~ GPIO27 OEN control      //[CHIP_TSUMV] GPIO20 ~ GPIO26 OEN control
#define _REG_GPIO2_IN       REG_1B32 //GPIO20 ~ GPIO27 input value      //[CHIP_TSUMV] GPIO20 ~ GPIO26 input value
#define _REG_GPIO3_OUT      REG_1B34 //GPIO30 ~ GPIO35 output value     //[CHIP_TSUMV] GPIO30 ~ GPIO31 output value
#define _REG_GPIO3_OEZ      REG_1B36 //GPIO30 ~ GPIO35 OEN control      //[CHIP_TSUMV] GPIO30 ~ GPIO31 OEN control
#define _REG_GPIO3_IN       REG_1B38 //GPIO30 ~ GPIO35 input value      //[CHIP_TSUMV] GPIO30 ~ GPIO31 input value
#if CHIP_ID==CHIP_TSUMV || CHIP_ID==CHIP_TSUMY // 20111006.  Gary.Wang
#define _REG_GPIO4_OUT      REG_1B3A //GPIO40 ~ GPIO47 output value
#define _REG_GPIO4_OEZ      REG_1B3C //GPIO40 ~ GPIO47 OEN control
#define _REG_GPIO4_IN       REG_1B3E //GPIO40 ~ GPIO47 input value
#endif
#define _REG_GPIOAU_SEL     REG_1B5A // GPIO_AU0 ~ GPIO_AU5 enable ,0: enable GPIO function ,1: disable GPIO function
#define _REG_GPIOAU_OUT     REG_1B5B // GPIO_AU0 ~ GPIO_AU5 output value
#define _REG_GPIOAU_OEZ     REG_1B5C //GPIO_AU0 ~ GPIO_AU5 OEN control
#define _REG_GPIOAU_IN      REG_1B5F    //GPIO_AU0 ~ GPIO_AU5 input value


#define _REG_PMGPIO0_OUT    REG_0200  //PMGPIO00 ~ PMGPIO07 output value
#define _REG_PMGPIO0_OEZ    REG_0201  //PMGPIO00 ~ PMGPIO07 OEN control
#define _REG_PMGPIO1_OUT    REG_0202  //PMGPIO12 ~ PMGPIO17 output value
#define _REG_PMGPIO1_OEZ    REG_0203 //PMGPIO12 ~ PMGPIO17 OEN control
#define _REG_PMGPIO2_OUT    REG_0204 //PMGPIO20 ~ PMGPIO27 output value
#define _REG_PMGPIO2_OEZ    REG_0205 //PMGPIO20 ~ PMGPIO27 OEN control
#define _REG_PMGPIO0_IN     REG_0206   //PMGPIO00 ~ PMGPIO07 input value
#define _REG_PMGPIO1_IN     REG_0207  //PMGPIO10 ~ PMGPIO17 input value
#define _REG_PMGPIO2_IN     REG_0208  //PMGPIO20 ~ PMGPIO27 input value

#define _REG_GPIO_OEN_MODE  REG_020C  //Power GPIO Enable , 0 : Normal PM GPIO , 1 : Power GPIO

#define _REG_CEC_EN         REG_020E
                                                                                              //REG_020E[0] 1:Enable CEC, 0: Disable CEC,
                                                                                              //REG_020E[1] 1:Enable CEC2 , 0:Disable CEC2
                                                                                              //REG_020E[7:6], [6]: PMGPIO0[4] driving strength control,[7]: PMGPIO0[5] driving strength control
#if CHIP_ID<CHIP_TSUMV
#define _REG_EXTINT_GROUP0_EN   REG_020F    //[11:8]
                                                                                              // [3] Enable PMGPIO03 external interrupt function
                                                                                              // [2] Enable PMGPIO04 external interrupt function
                                                                                              // [1] Enable PMGPIO05 external interrupt function
                                                                                              // [0] Enable PMGPIO06 external interrupt function
#endif
#define _REG_EXTINT_GROUP1_EN   REG_020F    //[15:12]
                                                                                              //[3] Enable PMGPIO27 external interrupt function
                                                                                              //[2] Enable PMGPIO26 external interrupt function
                                                                                              //[1] Enable PMGPIO25 external interrupt function
                                                                                              //[0] Enable PMGPIO24 external interrupt function

#define _REG_PMGPIOSAR_OUT      REG_0220  //PMGPIO_SAR0 ~ PMGPIO_SAR3 output value
#define _REG_PMGPIOSAR_OEZ      REG_0220  //PMGPIO_SAR0 ~ PMGPIO_SAR3 OEN control
#define _REG_PMGPIOSAR_IN       REG_0221  //PMGPIO_SAR0 ~ PMGPIO_SAR3 input value

#if CHIP_ID<CHIP_TSUMV
#define _REG_SPI_PAD            REG_0222    //[5:0] //SPI pads driving control registers.
#endif
#define _REG_PAD_PMPWM_EN       REG_0223    //[10:8]       //PM_PWM function output enable (1: enable)
#define _REG_PAD_PMPWM_OD_EN    REG_0223    //[13:11] //PM_PWM function output opendrain enable (1: enable)

#define _REG_SPI_QUAD_EN        REG_0223    //[14]            //0: Disable SPI quad mode, 1: Enable SPI quad mode

#define _REG_DBUS_XEN_PAD_EN    REG_0223    //[15]       //0: Disable DBUS external control 1: DBUS can be disabled temporarily by pad DBUS_XEN=0
#if 0
#define REG_EXTINT_GROUP1_EN_BIT(b) ( ( b ) << 4 )
#define REG_PMGPIOSAR_OEZ_BIT(b)    ( ( b ) << 4 )
#define REG_PAD_PMPWM_OD_EN_BIT(b)  ( ( b ) << 3 )
#define REG_SPI_QUAD_EN_BIT(b)      ( ( b ) << 6 )
#define REG_DBUS_XEN_PAD_EN_BIT(b)  ( ( b ) << 7 )
#endif

#endif

#endif
