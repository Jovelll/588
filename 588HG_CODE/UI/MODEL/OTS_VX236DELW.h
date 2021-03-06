#ifndef OTS_VX236DELW_H
#define OTS_VX236DELW_H
////////////////////////////////////////////////////////////
// BOARD SELECTION
////////////////////////////////////////////////////////////
//#define TSUMXXNX
#define CHIP_ID     CHIP_TSUMY
#define MainBoardType   MainBoard_5269_M0A

#define ENABLE_LED_CONTROLLER   1
#define ENABLE_SUPER_RESOLUTION     0

// 120119 coding test for TPV
// 0: brightness control by PWM duty
// 1: brightness control by string current and fix PWM duty to max
#define LED_CTRL_BRIGHTNESS_BY_CURRENT  1
////////////////////////////////////////////////////////////
// INPUT_TYPE SELECTION
////////////////////////////////////////////////////////////
#define INPUT_TYPE                      INPUT_1A1D

#define DVI_USB                         0 // 1: DVI from USB

#if INPUT_TYPE == INPUT_1A1H || INPUT_TYPE == INPUT_1A1D1H || INPUT_TYPE == INPUT_1A2H
#define ENABLE_HDMI                     1
#else
#define ENABLE_HDMI                     0
#endif
#define ENABLE_DVI                               1
#define ENABLE_DP_INPUT             0
////////////////////////////////////////////////////////////
// PANEL SELECTION
////////////////////////////////////////////////////////////
#define PanelType                           PanelCMIM236HGJ_L21

////////////////////////////////////////////////////////////
// BRIGHTNESS CONTROL
////////////////////////////////////////////////////////////
#define BrightFreqByVfreq               1 // set brightness freq. by input Vfreq.
#define BRIGHTNESS_INVERSE          0
#define BRIGHTNESS_FREQ                     250 // unit: Hz
#define BRIGHTNESS_VSYNC_ALIGN       1
////////////////////////////////////////////////////////////

// AUDIO
////////////////////////////////////////////////////////////
#define AudioFunc                           0
#define VOLUME_INVERSE                  0
#define VOLUME_FREQ                        0 // unit: Hz
#define VOLUME_VSYNC_ALIGN              0
#define USE_DAC_ADJ                              0
////////////////////////////////////////////////////////////
// OSD
////////////////////////////////////////////////////////////
#define DisplayLogo                         NoBrand
#define LANGUAGE_TYPE                   ASIA//SIXTEEN

////////////////////////////////////////////////////////////
// SYSTEM
////////////////////////////////////////////////////////////
#define FACTORYALIGN_TYPE               FACTORYALIGN_DDCCI

#define ENABLE_POWEROFF_1X_MCU_CLK          0


#define ENABLE_FACTORY_SSCADJ           1       //201109.13 19:02 CC

#if INPUT_TYPE > INPUT_1A
#define ENABLE_HDCP                     1
#define HDCPKEY_IN_Flash                1
#else
#define ENABLE_HDCP                     0
#define HDCPKEY_IN_Flash                0
#endif

////////////////////////////////////////////////////////////
// POWERMANAGEMENT
////////////////////////////////////////////////////////////
#define MS_PM                                   1
#define PM_MCUSLEEP                     0
#define LED_PWM                             0

////////////////////////////////////////////////////////////
// SI
////////////////////////////////////////////////////////////
#define ODM_NAME                    ODM_AOC

////////////////////////////////////////////////////////////
// INFORMATION
////////////////////////////////////////////////////////////
#define ChipName                            "TSUMY"
#define Model                                   "VX236DELW"
#define FWVersion                           "016"
#define ChangeDate                          "20111207"

///////////////////////////////////////////////////////////////
// SYSTEM DEBUG
//////////////////////////////////////////////////////////////
//#define ENABLE_DEBUG    1
//#define HDCPKEY_USE_CODE    1
///////////////////////////////////////////////////////////////

#endif

