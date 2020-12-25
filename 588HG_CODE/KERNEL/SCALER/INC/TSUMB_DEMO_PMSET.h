#ifndef _MS_PMDEF_H
#define _MS_PMDEF_H

#define PM_CLOCK                RCOSC   //modified clock rate on PM mode - A059
#define PM_SUPPORT_AC2DC                    (0) // (1)supports to AC2DC;                                                 (0)doesn't support to AC2DC
#define PM_SUPPORT_SOG_TIME_SHARE       (0) // (1)supports to SoG time sharing function to reduce power consumption  (0)doesn't support to SoG time sharing function
#define PM_SUPPORT_DVI_TIME_SHARE       (0) // (1)supports to DVI time sharing function to reduce power consumption  (0)doesn't support to DVI time sharing function
#define PM_SUPPORT_WAKEUP_DVI               (1) // (1)supports to DVI wakeup;                                            (0)doesn't support to DVI wakeup
#define PM_SUPPORT_WAKEUP_DP                (0) 
#define PM_SUPPORT_DVI5V_TurnonRterm        (0)
#define PM_POWERkEY_GETVALUE                (0)
#define PM_CABLEDETECT_USE_GPIO         (0)
#define PM_CABLE_DETECT_USE_SAR         (CABLE_DETECT_VGA_USE_SAR||CABLE_DETECT_VGA_USE_SAR)

#define PM_0W_Mode_Enable         (0)    // for TXUMD only




#define PM_POWERSAVING_WAKEUP_GPIO              0
#define PM_POWERSAVING_WAKEUP_SAR               0
#define PM_POWERSAVING_WAKEUP_SOG               1
#define PM_POWERSAVING_WAKEUP_MCCS              1
#define PM_POWERSAVING_SARmode                  ePMSAR_SAR3

#define PM_POWEROFF_WAKEUP_GPIO                     0
#define PM_POWEROFF_WAKEUP_SAR                      0
#define PM_POWEROFF_WAKEUP_SOG                  0
#define PM_POWEROFF_WAKEUP_MCCS                     1
#define PM_POWEROFF_SARmode                     ePMSAR_SAR3
#if PM_SUPPORT_WAKEUP_DVI
#define DVI5VP0_GPIO 1
#define DVI5VP1_GPIO 0
#endif

#define XBYTE       ((unsigned char volatile xdata *) 0)
//-------------------------------------------------------------------------------------------------
//  PM CLOCK
//-------------------------------------------------------------------------------------------------
#define   RCOSC         0
#define   XTAL          1
#define   XTALDIV2      2
#define   XTALDIV4      3

//-------------------------------------------------------------------------------------------------
//  VGA
//-------------------------------------------------------------------------------------------------
#define REG_SYNC_DET    REG_PM_A0

#define EN_HSYNC_DET    _BIT6
#define EN_VSYNC_DET    _BIT5
#define EN_SOG_DET      _BIT4
#define EN_HVSYNC_DET   _BIT3
#define EN_HSYNC_LOST   _BIT2
#define EN_VSYNC_LOST   _BIT1
#define EN_SOG_LOST     _BIT0

#define EN_SYNC_DET_SET     EN_HVSYNC_DET
#define EN_SYNC_DET_MASK    EN_HSYNC_DET|EN_VSYNC_DET|EN_HVSYNC_DET
//-------------------------------------------------------------------------------------------------
//  DVI
//-------------------------------------------------------------------------------------------------
#define REG_DVI_DET         REG_PM_9F
#define EN_DVI_LOST_A       _BIT5
#define EN_DVI_LOST_BC      _BIT4
#define EN_DVI_DET_A        _BIT1
#define EN_DVI_DET_BC       _BIT0

#define EN_DVI_DET_SET      EN_DVI_DET_BC
#define EN_DVI_DET_MASK     EN_DVI_DET_A|EN_DVI_DET_BC
//---------------------------------------
#define REG_DVI_CTRL        REG_PM_BE
#define EN_DVI_CTRL             _BIT7
#define EN_DVI_ALWAYS_ON        _BIT6
#define DVI_CTRL_PERIOD         0x00    // <Janus>2010/07/14: valid range 0~7. control period = (DVI_CTRL_PERIOD+1) * 131.2ms
#define DVI_ON_PERIOD           0x03 //0x07    // <Janus>2010/07/14: valid range 1~7. Period = DVI_ON_PERIOD * 8.2 ms

#define EN_DVI_CTRL_SET         EN_DVI_CTRL|(DVI_CTRL_PERIOD<<3)|DVI_ON_PERIOD
#define EN_DVI_CTRL_MASK        0xFF
//----------------------------------------
#define REG_DVI_CHEN        REG_PM_BF
#define EN_DVI_RTERM        _BIT3
#define EN_DVI0             _BIT0
#define EN_DVI1             _BIT1
#define EN_DVI0_DVI1        _BIT1|_BIT0

#define EN_DVI_CHEN_SET     EN_DVI_RTERM|EN_DVI0_DVI1
#define EN_DVI_CHEN_MASK    EN_DVI_RTERM|EN_DVI0_DVI1

//-------------------------------------------------------------------------------------------------
//  SAR
//-------------------------------------------------------------------------------------------------
#define REG_SAR_GPIO        REG_PM_84

#define EN_SAR_DET          _BIT7
#define INV_SAR_POL         _BIT6

#define EN_SAR_DET_SET      EN_SAR_DET
#define EN_SAR_DET_MASK     EN_SAR_DET|INV_SAR_POL

//-- GPIO ---------------------------------
#define REG_PMGPIO_EN        REG_PM_60  // use word access to this register
#define EN_GPIO00_DET      _BIT0      // enable wakeup detect of PM GPIO 00
#define EN_GPIO01_DET      _BIT1      // enable wakeup detect of PM GPIO 01
#define EN_GPIO02_DET      _BIT2      // enable wakeup detect of PM GPIO 02
#define EN_GPIO11_DET      _BIT3      // enable wakeup detect of PM GPIO 03
#define EN_GPIO12_DET      _BIT4      // enable wakeup detect of PM GPIO 04
#define EN_GPIO13_DET      _BIT5      // enable wakeup detect of PM GPIO 05
#define EN_GPIO20_DET      _BIT6      // enable wakeup detect of PM GPIO 06
#define EN_GPIO21_DET      _BIT7      // enable wakeup detect of PM GPIO 07
#define EN_GPIO24_DET      _BIT8      // enable wakeup detect of PM GPIO 24
#define EN_GPIO25_DET      _BIT9      // enable wakeup detect of PM GPIO 25
#define EN_GPIO26_DET      _BIT10     // enable wakeup detect of PM GPIO 26
#define EN_GPIO27_DET      _BIT11     // enable wakeup detect of PM GPIO 27
#define EN_GPIO30_DET      _BIT12     // enable wakeup detect of PM GPIO 30
#define EN_GPIO31_DET      _BIT13     // enable wakeup detect of PM GPIO 31
#define EN_GPIO36_DET      _BIT14     // enable wakeup detect of PM GPIO 36
#define EN_GPIO37_DET      _BIT15    // enable wakeup detect of PM GPIO 37


#define REG_PMGPIO_POL       REG_PM_62 // use word access to this register
#define INV_GPIO00_POL     _BIT0      // invert input signal polarity of PM GPIO 00
#define INV_GPIO01_POL     _BIT1      // invert input signal polarity of PM GPIO 01
#define INV_GPIO02_POL     _BIT2      // invert input signal polarity of PM GPIO 02
#define INV_GPIO11_POL     _BIT3      // invert input signal polarity of PM GPIO 03
#define INV_GPIO12_POL     _BIT4      // invert input signal polarity of PM GPIO 04
#define INV_GPIO13_POL     _BIT5      // invert input signal polarity of PM GPIO 05
#define INV_GPIO20_POL     _BIT6      // invert input signal polarity of PM GPIO 06
#define INV_GPIO21_POL     _BIT7      // invert input signal polarity of PM GPIO 07
#define INV_GPIO24_POL     _BIT8      // invert input signal polarity of PM GPIO 24
#define INV_GPIO25_POL     _BIT9      // invert input signal polarity of PM GPIO 25
#define INV_GPIO26_POL     _BIT10     // invert input signal polarity of PM GPIO 26
#define INV_GPIO27_POL     _BIT11     // invert input signal polarity of PM GPIO 27
#define INV_GPIO30_POL     _BIT12     // invert input signal polarity of PM GPIO 30
#define INV_GPIO31_POL     _BIT13    // invert input signal polarity of PM GPIO 31
#define INV_GPIO36_POL     _BIT14     // invert input signal polarity of PM GPIO 36
#define INV_GPIO37_POL     _BIT15     // invert input signal polarity of PM GPIO 37



#define REG_PMGPIO_STS       REG_PM_64   // use word access to this register
#define GPIO00_INT         _BIT0      // wakeup status of PM GPIO 00
#define GPIO01_INT         _BIT1      // wakeup status of PM GPIO 01
#define GPIO02_INT         _BIT2      // wakeup status of PM GPIO 02
#define GPIO11_INT         _BIT3      // wakeup status of PM GPIO 03
#define GPIO12_INT         _BIT4      // wakeup status of PM GPIO 04
#define GPIO13_INT         _BIT5      // wakeup status of PM GPIO 05
#define GPIO20_INT         _BIT6      // wakeup status of PM GPIO 06
#define GPIO21_INT         _BIT7      // wakeup status of PM GPIO 07
#define GPIO24_INT         _BIT8      // wakeup status of PM GPIO 24
#define GPIO25_INT         _BIT9      // wakeup status of PM GPIO 25
#define GPIO26_INT         _BIT10     // wakeup status of PM GPIO 26
#define GPIO27_INT         _BIT11     // wakeup status of PM GPIO 27
#define GPIO30_INT         _BIT12    // wakeup status of PM GPIO 30
#define GPIO31_INT         _BIT13     // wakeup status of PM GPIO 31
#define GPIO36_INT         _BIT14     // wakeup status of PM GPIO 36
#define GPIO37_INT         _BIT15     // wakeup status of PM GPIO 37


#define PM_POWERKEY_INT     GPIO27_INT
#define EN_GPIO_DET_SET     EN_GPIO00_DET
                            
#define EN_GPIO_DET_MASK   ( EN_GPIO00_DET\                              
                                                    | EN_GPIO01_DET\      
                                                    | EN_GPIO02_DET\      
                                                    | EN_GPIO11_DET\      
                                                    | EN_GPIO12_DET \      
                                                    | EN_GPIO13_DET \      
                                                    | EN_GPIO20_DET \      
                                                    | EN_GPIO21_DET \      
                                                    | EN_GPIO24_DET \     
                                                    | EN_GPIO25_DET \    
                                                    | EN_GPIO26_DET \    
                                                    | EN_GPIO27_DET\     
                                                    | EN_GPIO30_DET \     
                                                    | EN_GPIO31_DET \     
                                                    | EN_GPIO36_DET\     
                                                    |EN_GPIO37_DET )
                                                
#define INV_GPIO_POL_SET    ( INV_GPIO00_POL \
                            )
#define INV_GPIO_POL_MASK ( INV_GPIO00_POL \                           
                                                    |INV_GPIO01_POL     \      
                                                    |INV_GPIO02_POL     \     
                                                    |INV_GPIO11_POL     \      
                                                    |INV_GPIO12_POL     \      
                                                    |INV_GPIO13_POL     \      
                                                    |INV_GPIO20_POL     \      
                                                    |INV_GPIO21_POL     \      
                                                    |INV_GPIO24_POL     \      
                                                    | INV_GPIO25_POL     \    
                                                    |INV_GPIO26_POL     \    
                                                    |INV_GPIO27_POL     \    
                                                    |INV_GPIO30_POL     \     
                                                    |INV_GPIO31_POL     \    
                                                    |INV_GPIO36_POL     \    
                                                    |INV_GPIO37_POL          )


#define PM_VGACBL_DET            0
#define PM_DVICBL_DET            0
#define PM_HDMIACBL_DET       0
#define PM_HDMIBCBL_DET       0
#define PM_HDMICCBL_DET       0
#define PM_DPCBL_DET             INV_GPIO00_POL
//----------------------------------------
#define REG_SINGLE_KEY          REG_3A60
#define SAR_CH0                 0
#define SAR_CH1                 1
#define SAR_CH2                 2
#define SAR_CH3                 3
#define SAR_CHMASK          BIT1|BIT0
#define EN_SAR_SINGLE           _BIT4
#define SINGLE_KEY_SEL          SAR_CH0

#define EN_SAR_SINGLE_SET       EN_SAR_SINGLE|SINGLE_KEY_SEL
#define EN_SAR_SINGLE_MASK      EN_SAR_SINGLE|SAR_CHMASK
//----------------------------------------
#define REG_SAR_CMP             REG_3A64
#define EN_SAR_05V              0x00
#define EN_SAR_14V              0x40
#define EN_SAR_25V              0xC0

#define EN_SAR_CMP_MASK         0xC0
//----------------------------------------
#define REG_SAR_ANYKEY          REG_3A24
#define EN_ANY_KEY              _BIT7
#define EN_TWO_CH               _BIT6
#define SAR_SWITCH_CNT          0x02    // SAR channel switch period

#define EN_SAR_3CH_SET          EN_ANY_KEY|SAR_SWITCH_CNT
#define EN_SAR_2CH_SET          EN_ANY_KEY|EN_TWO_CH|SAR_SWITCH_CNT

#define EN_SAR_ANYKEY_MASK      0xFF

//-------------------------------------------------------------------------------------------------
//  MCCS
//-------------------------------------------------------------------------------------------------
#define REG_D2B_EN              REG_PM_90

#define D2B_EN_A0               _BIT3
#define D2B_EN_A1               _BIT2
#define D2B_EN_D0               _BIT1
#define D2B_EN_D1               _BIT0
#define REG_D2B_EN_MASK         D2B_EN_A0|D2B_EN_A1|D2B_EN_D0|D2B_EN_D1
#define REG_D2B_EN_SET          D2B_EN_A0|D2B_EN_A1|D2B_EN_D0|D2B_EN_D1

//-------------------------------------------------------------------------------------------------
//  DDC
//-------------------------------------------------------------------------------------------------
#define REG_DDC_EN              REG_PM_91

#define DDC_EN_A0               _BIT3
#define DDC_EN_A1               _BIT2
#define DDC_EN_D0               _BIT1
#define DDC_EN_D1               _BIT0
#define REG_DDC_EN_MASK         DDC_EN_A0|DDC_EN_A1|DDC_EN_D0|DDC_EN_D1
#define REG_DDC_EN_SET          DDC_EN_A0|DDC_EN_A1|DDC_EN_D0|DDC_EN_D1

//-------------------------------------------------------------------------------------------------
//  Others
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Wakeup Event
//-------------------------------------------------------------------------------------------------
#define REG_EVENT1          REG_PM_85

#define HSYNC_DET_0     BIT7
#define VSYNC_DET_0     BIT6
#define SOG_DET_0           BIT5
#define HV_DET_0            BIT4
#define HSYNC_DET_1     BIT3
#define VSYNC_DET_1     BIT2
#define SOG_DET_1           BIT1
#define HV_DET_1            BIT0
//----------------------------------------
#define REG_EVENT2          REG_PM_86

#define D2B_WAKEUP          _BIT7
#define DVI_CLK_DET_0       _BIT5
#define DVI_CLK_DET_1       0
#define SAR_IN_DET          _BIT4
#define CS_DET          _BIT2   // composite sync detect status
#define CEC_WAKEUP          _BIT1


//-------------------------------------------------------------------------------------------------
//  AC2DC function
//-------------------------------------------------------------------------------------------------
#define AC2DC_PAD                   AC2DC_PAD_GPIOP25    /*BY HW SETTING */
#define AC2DC_PAD_Disable           0x00
#define AC2DC_PAD_GPIOP25           BIT0
#define AC2DC_PAD_GPIOP24           BIT1
#define AC2DC_MODE                  VL_MODE
#define OFF_MODE                    0x04
#define OFF_PERIOD                  0x3E                                    /* OFF period = (OFF_PERIOD+1)X32.8mS , should >1S */   /*BY HW SETTING */
#define VL_MODE                     0xA4
#define VH_MODE                     0x91
#define VHL_MODE                    0xB5

//-------------------------------------------------------------------------------------------------
//  msPM_StartRCOSCCal function
//-------------------------------------------------------------------------------------------------
#define AC2DC_RCOSC_CLKOFF          0xBB
#define AC2DC_CLKOFF                0x0B
#define RCOSC_CLKOFF                0xB0
#endif
