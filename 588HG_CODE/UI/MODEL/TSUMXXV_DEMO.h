#ifndef TSUMXXV_DEMO_H
#define TSUMXXV_DEMO_H
////////////////////////////////////////////////////////////
// BOARD SELECTION
////////////////////////////////////////////////////////////
#define CHIP_ID     CHIP_TSUMV
#define MainBoardType   MainBoard_MST135B

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
#define ENABLE_DVI                      1
#define ENABLE_DP_INPUT                 0
////////////////////////////////////////////////////////////
// PANEL SELECTION
////////////////////////////////////////////////////////////
#define PanelType                           PanelLD2150010//PanelCMOM215HGE_L10//PanelTPM215HW01_HGE//PanelLTM200KT07//PanelAUOM215HW01//PanelCLAA185WA03//PanelLTM200KT07//PanelTPM200O1L02

////////////////////////////////////////////////////////////
// BRIGHTNESS CONTROL
////////////////////////////////////////////////////////////
#define BrightFreqByVfreq               1 // set brightness freq. by input Vfreq.
// just for coding
//#if PanelType == PanelCLAA185WA03
#define BRIGHTNESS_INVERSE          0
//#else
//#define BRIGHTNESS_INVERSE        0
//#endif

#define BRIGHTNESS_FREQ                     250 // unit: Hz
#define BRIGHTNESS_VSYNC_ALIGN       1
////////////////////////////////////////////////////////////

// AUDIO
////////////////////////////////////////////////////////////
#define AudioFunc                           0
#define VOLUME_INVERSE                  0
#define VOLUME_FREQ                        0 // unit: Hz
#define VOLUME_VSYNC_ALIGN              0

////////////////////////////////////////////////////////////
// OSD
////////////////////////////////////////////////////////////
#define DisplayLogo                     MstarLOGO
#define LANGUAGE_TYPE                   ASIA//SIXTEEN

////////////////////////////////////////////////////////////
// SYSTEM
////////////////////////////////////////////////////////////
#define ENABLE_DPS  1

#define FACTORYALIGN_TYPE               FACTORYALIGN_DDCCI

#define ENABLE_POWEROFF_1X_MCU_CLK          0


#define ENABLE_FACTORY_SSCADJ           1
#if INPUT_TYPE > INPUT_1A
#define ENABLE_HDCP                     1
#define HDCPKEY_IN_Flash                1
#else
#define ENABLE_HDCP                     0
#define HDCPKEY_IN_Flash                0
#endif
#define ENABLE_FREESYNC                 0
// Instead of switching EDID, it use MCCS VPC Code to indicate the status of FreeSync support.
// Requirement: The Byte 4 of HDMI FreeSync VSD Block has to be 0xE6, not 0x00.
#define ENABLE_HDMI_DRR_MCCS            (1 && ENABLE_FREESYNC)//wtx add AMD MCCS
//#define HDCPKEY_USE_CODE    1 // coding for test

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
#define Model                                   "VS215DGLW"
#define FWVersion                           "010"
#define ChangeDate                          "20101117"

///////////////////////////////////////////////////////////////
// SYSTEM DEBUG
//////////////////////////////////////////////////////////////
#define ENABLE_DEBUG    1
//#define HDCPKEY_USE_CODE    1
///////////////////////////////////////////////////////////////
#define UART1   0

#endif

