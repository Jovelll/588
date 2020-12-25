#ifndef TSUMXX2_DEMO_H
#define TSUMXX2_DEMO_H
////////////////////////////////////////////////////////////
// BOARD SELECTION
////////////////////////////////////////////////////////////
#define CHIP_ID     CHIP_TSUM2
#define MainBoardType   MainBoard_MST144A_A02A//MainBoard_5598_M0A//MainBoard_MST144A_A02A

////////////////////////////////////////////////////////////
// INPUT_TYPE SELECTION
////////////////////////////////////////////////////////////
#define INPUT_TYPE                      INPUT_1A2H

#define DVI_USB                         0 // 1: DVI from USB

#if INPUT_TYPE == INPUT_1A1H || INPUT_TYPE == INPUT_1A1D1H || INPUT_TYPE == INPUT_1A2H||INPUT_TYPE == INPUT_1A1H1DP
#define ENABLE_HDMI                      1
#define ENABLE_HDMI_1_4                 1
#else
#define ENABLE_HDMI                     0
#endif
#define ENABLE_DVI                               0
#define ENABLE_DP_INPUT             0
#define MS_VGA_SOG_EN               1
#define ENABLE_USB_INPUT        0

#define ENABLE_MHL                          1

//#define ENABLE_RTE  1

////////////////////////////////////////////////////////////
// PANEL SELECTION
////////////////////////////////////////////////////////////
#define PanelType                         PanelLM220WE1//PanelTPM215HW01_HGE//PanelLM230WF5TLD1//PanelLM270WF4TLA1

////////////////////////////////////////////////////////////
// BRIGHTNESS CONTROL
////////////////////////////////////////////////////////////
#define BrightFreqByVfreq               1 // set brightness freq. by input Vfreq.
#define BRIGHTNESS_INVERSE          0
#define BRIGHTNESS_FREQ                     240 // unit: Hz
#define BRIGHTNESS_VSYNC_ALIGN       1
////////////////////////////////////////////////////////////

// AUDIO
////////////////////////////////////////////////////////////
#define AudioFunc                           1
#define VOLUME_INVERSE                  0
#define VOLUME_FREQ                        30000 // unit: Hz
#define VOLUME_VSYNC_ALIGN              0
#define DAC_IDLE_TONE_IMPROVEMENT       0
////////////////////////////////////////////////////////////
// OSD
////////////////////////////////////////////////////////////
#define DisplayLogo                         MstarLOGO
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

#ifndef ENABLE_R2_2DTO3D
#define ENABLE_R2_2DTO3D          0
#endif
#ifndef ENABLE_USB_INPUT
#define ENABLE_USB_INPUT             0
#endif
#define ENABLE_OSD_3D_CONTROL       ENABLE_R2_2DTO3D
#define ENABLE_R2_CODE_ON_DRAM    (ENABLE_USB_INPUT || ENABLE_R2_2DTO3D)
#define ENBLAE_BDMA_INT           1
//#define ENABLE_HK_DATA_ON_DRAM    1

#define ENABLE_TOUCH_PANEL     0
#define ENABLE_TOUCH_PANEL_DATA_FROM_USB     0
#define ENABLE_TOUCH_PANEL_CTRL_OSD     0

#define ENABLE_POWER_BOARD_CONTROL  0 // control power board enter to saving mode
#define ENABLE_FREESYNC         0
// Instead of switching EDID, it use MCCS VPC Code to indicate the status of FreeSync support.
// Requirement: The Byte 4 of HDMI FreeSync VSD Block has to be 0xE6, not 0x00.
#define ENABLE_HDMI_DRR_MCCS            (1 && ENABLE_FREESYNC)//wtx add AMD MCCS
// 120410 coding test for touch key
//#define ENABLE_TOUCH_KEY    1
//#define TOUCH_KEY_CTRL_LED  1
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
#define ChipName                            "TSUM2"
#define Model                                   "VS230PGLW"
#define FWVersion                           "002"
#define ChangeDate                          "20120205"

///////////////////////////////////////////////////////////////
// SYSTEM DEBUG
//////////////////////////////////////////////////////////////
#define ENABLE_DEBUG    1
//#define HDCPKEY_USE_CODE    1
#define DISABLE_AUTO_SWITCH     0 // for MHL test

///////////////////////////////////////////////////////////////
#endif

