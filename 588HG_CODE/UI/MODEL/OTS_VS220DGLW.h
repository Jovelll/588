#ifndef OTS_VS220DGLW_H
#define OTS_VS220DGLW_H
////////////////////////////////////////////////////////////
// BOARD SELECTION
////////////////////////////////////////////////////////////
//#define TSUMXXNX
#define CHIP_ID     CHIP_TSUMV
#define MainBoardType   MainBoard_3834_M02

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
#define ENABLE_DP_AUDIO                     0
#define MS_VGA_SOG_EN               0
////////////////////////////////////////////////////////////
// PANEL SELECTION
////////////////////////////////////////////////////////////
#define PanelType                           PanelLTM220M1L09//PanelLTM200KT07//PanelTPM200O1L02

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
#define AudioFunc                           1
#define VOLUME_INVERSE                  1
#define VOLUME_FREQ                        240 // unit: Hz
#define VOLUME_VSYNC_ALIGN              0
#define USE_DAC_ADJ                            0
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
#define MS_PM                                   0
#define PM_MCUSLEEP                     0
#define LED_PWM                             0

////////////////////////////////////////////////////////////
// SI
////////////////////////////////////////////////////////////
#define ODM_NAME                    ODM_AOC

////////////////////////////////////////////////////////////
// INFORMATION
////////////////////////////////////////////////////////////
#define ChipName                            "TSUMU58VHN"
#define Model                                   "VS220DGLW"
#define FWVersion                           "016"
#define ChangeDate                          "20111205"

///////////////////////////////////////////////////////////////
// SYSTEM DEBUG
//////////////////////////////////////////////////////////////
//#define ENABLE_DEBUG    1
//#define HDCPKEY_USE_CODE    1
///////////////////////////////////////////////////////////////

#endif

