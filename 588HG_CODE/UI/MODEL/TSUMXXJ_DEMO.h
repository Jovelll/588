#ifndef TSUMXXJ_DEMO_H
#define TSUMXXJ_DEMO_H
////////////////////////////////////////////////////////////
// BOARD SELECTION
////////////////////////////////////////////////////////////
#define CHIP_ID         CHIP_TSUMJ
#define MainBoardType   MainBoard_TSUML88CMC2

//#define FPGA


////////////////////////////////////////////////////////////
// INPUT_TYPE SELECTION
////////////////////////////////////////////////////////////
#define DVI_USB                         0 // 1: DVI from USB
#define DVI_PN_SWAP                     0
#define HDMI_PN_SWAP                    1
#define ENABLE_AUTOEQ                   0
#if MainBoardType == MainBoard_TSUML887DC5  //C5 board
#define INPUT_TYPE                      INPUT_1A1D
#define ENABLE_HDMI                     0
#define ENABLE_HDMI_1_4                 0
#define ENABLE_MHL_C1                   0
#define ENABLE_MHL_C2                   0
#define ENABLE_MHL_C3                   0
#define ENABLE_MHL                      0
#define ENABLE_DVI                      1
#define ENABLE_DP_INPUT                 0
#define ENABLE_LED_CONTROLLER           1
#elif MainBoardType == MainBoard_TSUML88CMC2   //C2 board
#define INPUT_TYPE                      INPUT_1A1D
#define ENABLE_HDMI                     0
#define ENABLE_HDMI_1_4                 0
#define ENABLE_MHL_C1                   0
#define ENABLE_MHL_C2                   0
#define ENABLE_MHL_C3                   0
#define ENABLE_MHL                      0
#define ENABLE_DVI                      1
#define ENABLE_DP_INPUT                 0
#define ENABLE_LED_CONTROLLER           1
#elif MainBoardType == MainBoard_TSUML18CN2 // N2 board
#define INPUT_TYPE                      INPUT_1A
#define ENABLE_HDMI                     0
#define ENABLE_HDMI_1_4                 0
#define ENABLE_MHL_C1                   0
#define ENABLE_MHL_C2                   0
#define ENABLE_MHL_C3                   0
#define ENABLE_MHL                      0
#define ENABLE_DVI                      0
#define ENABLE_DP_INPUT                 0
#define ENABLE_LED_CONTROLLER           1
#elif MainBoardType == MainBoard_TSUML58CN3   // N3 board
#define INPUT_TYPE                      INPUT_1A1D
#define ENABLE_HDMI                     0
#define ENABLE_HDMI_1_4                 0
#define ENABLE_MHL_C1                   0
#define ENABLE_MHL_C2                   0
#define ENABLE_MHL_C3                   0
#define ENABLE_MHL                      0
#define ENABLE_DVI                      1
#define ENABLE_DP_INPUT                 0
#define ENABLE_LED_CONTROLLER           0
#else
#message "please define Russel new board type for Port_Func_Enable"
#endif

#define MS_VGA_SOG_EN               1
#define _NEW_SOG_DET_               0
#define ENABLE_USB_INPUT            0   //Alwasy set to 0

//#define ENABLE_RTE  1

////////////////////////////////////////////////////////////
// PANEL SELECTION
////////////////////////////////////////////////////////////
#ifdef FPGA
#define PanelType                   PanelAU20SVGA//PanelCMIM236HGJ_L21//PanelAU20SVGA // 800x600
#else
#define PanelType                   PanelCMIM236HGJ_L21//PanelLM230WF5TLD1//PanelLM220WE1//PanelTPM215HW01_HGE//PanelLM230WF5TLD1//PanelLM270WF4TLA1
#endif
////////////////////////////////////////////////////////////
// BRIGHTNESS CONTROL
////////////////////////////////////////////////////////////
#define BrightFreqByVfreq               1 // set brightness freq. by input Vfreq.
#define BRIGHTNESS_INVERSE              0
#define BRIGHTNESS_FREQ                 240 // unit: Hz
#define BRIGHTNESS_VSYNC_ALIGN          1
#define BRIGHTNESS_HSYNC_ALIGN          0

////////////////////////////////////////////////////////////
// HDMI
////////////////////////////////////////////////////////////
//efine DHDMI_SIMPLAYHD_PATCH    0 // SimplayHD CTS 8-18

////////////////////////////////////////////////////////////
// AUDIO
////////////////////////////////////////////////////////////
#define AudioFunc                       0   //Alwasy set to 0
#if AudioFunc
#define AUDIDO_ADC_HPF_N                0   // high pass filter N setting
#endif
#define VOLUME_INVERSE                  0
#define VOLUME_FREQ                     30000 // unit: Hz
#define VOLUME_VSYNC_ALIGN              0
#define DAC_IDLE_TONE_IMPROVEMENT       0
////////////////////////////////////////////////////////////
// OSD
////////////////////////////////////////////////////////////
#define DisplayLogo                     NoBrand
#define LANGUAGE_TYPE                   ASIA//SIXTEEN

////////////////////////////////////////////////////////////
// SYSTEM
////////////////////////////////////////////////////////////
#define AUTO_DATA_PATH              0
#define ENABLE_DPS                  1
#define ENABLE_DSC                  1
#define ENABLE_SUPER_RESOLUTION     0
#define ENABLE_SHARPNESS            1


#define FACTORYALIGN_TYPE               FACTORYALIGN_DDCCI

#define ENABLE_POWEROFF_1X_MCU_CLK      0


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
//#ifndef ENABLE_USB_INPUT
//#define ENABLE_USB_INPUT                   0   //Alwasy set to 0
//#endif
//#define ENABLE_OSD_3D_CONTROL           0//Alwasy set to 0
//#define ENABLE_R2_CODE_ON_DRAM          0//Alwasy set to 0
//#define ENBLAE_BDMA_INT                 0   //Alwasy set to 0
//#define ENABLE_HK_DATA_ON_DRAM    1

#define ENABLE_TOUCH_PANEL                  0
#define ENABLE_TOUCH_PANEL_DATA_FROM_USB    0
#define ENABLE_TOUCH_PANEL_CTRL_OSD         0

#define ENABLE_POWER_BOARD_CONTROL          0 // control power board enter to saving mode

// 120410 coding test for touch key
//#define ENABLE_TOUCH_KEY    1
//#define TOUCH_KEY_CTRL_LED  1
////////////////////////////////////////////////////////////
// POWERMANAGEMENT
////////////////////////////////////////////////////////////
#define MS_PM                   1
#define PM_MCUSLEEP             0
#define LED_PWM                 0

////////////////////////////////////////////////////////////
// SI
////////////////////////////////////////////////////////////
#define ODM_NAME                ODM_AOC

////////////////////////////////////////////////////////////
// INFORMATION
////////////////////////////////////////////////////////////
#define ChipName                "TSUMC"
#define Model                   "XXXXXXXXX"
#define FWVersion               "000"
#define ChangeDate              "20120612"

///////////////////////////////////////////////////////////////
// SYSTEM DEBUG
//////////////////////////////////////////////////////////////
#define ENABLE_DEBUG            1
//#define HDCPKEY_USE_CODE         1
#define DISABLE_AUTO_SWITCH     0 // for MHL test

///////////////////////////////////////////////////////////////
#define ENABLE_3DLUT            0
#define LVDS_CLK_1TO2_OFFSET    4

#endif

