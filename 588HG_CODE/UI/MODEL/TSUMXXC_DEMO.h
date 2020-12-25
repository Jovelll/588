#ifndef TSUMXXC_DEMO_H
#define TSUMXXC_DEMO_H
////////////////////////////////////////////////////////////
// BOARD SELECTION
////////////////////////////////////////////////////////////
#define CHIP_ID         CHIP_TSUMC
//#define MainBoardType   MainBoard_TSUML88CMC2//MainBoard_TSUML88CMC2//MainBoard_TSUML887DC5//MainBoard_TSUML887DC4_1//MainBoard_TSUML88CMC2, MainBoard_TSUML887DC4_1, MainBoard_TSUMP387MT9_1, MainBoard_TSUMP387MT9_1_DP, MainBoard_tsuml88bdc2_1,MainBoard_TSUMO58CMG_H(72 pin),MainBoard_TSUMO58CMG_DP(72 pin)

////////////////////////////////////////////////////////////
// INPUT_TYPE SELECTION
////////////////////////////////////////////////////////////
#define DVI_USB                         0 // 1: DVI from USB
#define DVI_PN_SWAP                     1
#define HDMI_PN_SWAP                    1
#define ENABLE_AUTOEQ                   0
#if MainBoardType == MainBoard_TSUML887DC5
#define INPUT_TYPE                      INPUT_1A2C
#define ENABLE_HDMI                     1//((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_MHL_C1                   1
#define ENABLE_MHL_C2                   0
#define ENABLE_MHL_C3                   0
#define ENABLE_MHL                      (ENABLE_MHL_C1|ENABLE_MHL_C2|ENABLE_MHL_C3)
#define ENABLE_DVI                      0//((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 1//((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_LED_CONTROLLER           1//need to check difference between Solar and Russell
#elif MainBoardType == MainBoard_TSUMP387MT9_1
#define INPUT_TYPE                      INPUT_1A3C
#define ENABLE_HDMI                     1//((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_MHL_C1                   1
#define ENABLE_MHL_C2                   1
#define ENABLE_MHL_C3                   1
#define ENABLE_MHL                      (ENABLE_MHL_C1|ENABLE_MHL_C2|ENABLE_MHL_C3)
#define ENABLE_DVI                      0//((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 0//((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_LED_CONTROLLER           0
#elif MainBoardType == MainBoard_TSUMP387MT9_1_DP
#define INPUT_TYPE                      INPUT_1A3C
#define ENABLE_HDMI                     1//((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_MHL_C1                   1
#define ENABLE_MHL_C2                   0
#define ENABLE_MHL_C3                   0
#define ENABLE_MHL                      (ENABLE_MHL_C1|ENABLE_MHL_C2|ENABLE_MHL_C3)
#define ENABLE_DVI                      0//((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 1//((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_LED_CONTROLLER           0
#elif MainBoardType == MainBoard_tsuml88bdc2_1
#define INPUT_TYPE                      INPUT_1A2C
#define ENABLE_HDMI                     1//((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_MHL_C1                   0
#define ENABLE_MHL_C2                   0
#define ENABLE_MHL_C3                   0
#define ENABLE_MHL                      (ENABLE_MHL_C1|ENABLE_MHL_C2|ENABLE_MHL_C3)
#define ENABLE_DVI                      1//((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 0//((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_LED_CONTROLLER           1//need to check difference between Solar and Russell
#elif MainBoardType == MainBoard_TSUML88CMC2
#define INPUT_TYPE                    INPUT_1A1H//  INPUT_1A3C//INPUT_1A2C
#define ENABLE_HDMI                     1//((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_MHL_C1                   0
#define ENABLE_MHL_C2                   0
#define ENABLE_MHL_C3                   0
#define ENABLE_MHL                      (ENABLE_MHL_C1|ENABLE_MHL_C2|ENABLE_MHL_C3)
#define ENABLE_DVI                      0//((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT               0//  1//((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_LED_CONTROLLER           0//need to check difference between Solar and Russell
#elif MainBoardType == MainBoard_TSUMU88CDT9_V8811  // 128 pins 1A1D1H
#define INPUT_TYPE                      INPUT_1A1D1H
#define ENABLE_HDMI                     1       //((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_MHL_C1                   0
#define ENABLE_MHL_C2                   0
#define ENABLE_MHL_C3                   0
#define ENABLE_MHL                      (ENABLE_MHL_C1|ENABLE_MHL_C2|ENABLE_MHL_C3)
#define ENABLE_DVI                      1       //((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 0       //((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_LED_CONTROLLER           0
#elif MainBoardType == MainBoard_TSUMO58FDT9_VP58C11  // 128 pins 1A1HDMI
#define INPUT_TYPE                      INPUT_1A1H
#define ENABLE_HDMI                     1       //((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_MHL_C1                   0
#define ENABLE_MHL_C2                   0
#define ENABLE_MHL_C3                   0
#define ENABLE_MHL                      (ENABLE_MHL_C1|ENABLE_MHL_C2|ENABLE_MHL_C3)
#define ENABLE_DVI                      0       //((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 0       //((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_LED_CONTROLLER           0
#elif MainBoardType == MainBoard_TSUMO58FDT9_VSP58C31_3_IN_1  // 128 pins 1A1HDMI
#define INPUT_TYPE                      INPUT_1A1H
#define ENABLE_HDMI                     1       //((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_MHL_C1                   0
#define ENABLE_MHL_C2                   0
#define ENABLE_MHL_C3                   0
#define ENABLE_MHL                      (ENABLE_MHL_C1|ENABLE_MHL_C2|ENABLE_MHL_C3)
#define ENABLE_DVI                      0       //((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 0       //((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_LED_CONTROLLER           0
#define _NEW_SOG_DET_                   0

#elif MainBoardType == MainBoard_TSUML58CN3
#define INPUT_TYPE                      INPUT_1A1C2
#define ENABLE_HDMI                     0//((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_MHL_C1                   0
#define ENABLE_MHL_C2                   0
#define ENABLE_MHL_C3                   0
#define ENABLE_MHL                      (ENABLE_MHL_C1|ENABLE_MHL_C2|ENABLE_MHL_C3)
#define ENABLE_DVI                      0//((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 0//((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_LED_CONTROLLER           0//need to check difference between Solar and Russell
#elif MainBoardType == MainBoard_TSUML18CN2
#define INPUT_TYPE                      INPUT_1A
#define ENABLE_HDMI                     0//((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_MHL_C1                   0
#define ENABLE_MHL_C2                   0
#define ENABLE_MHL_C3                   0
#define ENABLE_MHL                      (ENABLE_MHL_C1|ENABLE_MHL_C2|ENABLE_MHL_C3)
#define ENABLE_DVI                      0//((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 0//((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_LED_CONTROLLER           1//need to check difference between Solar and Russell
#elif (MainBoardType == MainBoard_TSUMOP38CMT9_HHD) || (MainBoardType == MainBoard_TSUMOP38CMT9_HDH)//
#define INPUT_TYPE                      INPUT_1A3C
#define ENABLE_HDMI                     1//((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 1
#define ENABLE_MHL_C1                   1
#define ENABLE_MHL_C2                   0
#define ENABLE_MHL_C3                   0
#define ENABLE_MHL                      (ENABLE_MHL_C1|ENABLE_MHL_C2|ENABLE_MHL_C3)
#define ENABLE_DVI                      0//((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 1//((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_USB_INPUT                0
#define ENABLE_LED_CONTROLLER           0//need to check difference between Solar and Russell
#elif MainBoardType == MainBoard_TSUMO58CMG_H // 72 pins
#define INPUT_TYPE                      INPUT_1A1C2
#define ENABLE_HDMI                     1//((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_MHL_C1                   0
#define ENABLE_MHL_C2                   1
#define ENABLE_MHL_C3                   0
#define ENABLE_MHL                      (ENABLE_MHL_C1|ENABLE_MHL_C2|ENABLE_MHL_C3)
#define ENABLE_DVI                      0//((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 0//((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_LED_CONTROLLER           0//need to check difference between Solar and Russell
#define _NEW_SOG_DET_                   1//no SOG pin
#elif MainBoardType == MainBoard_TSUMO58CMG_DP // 72 pins
#define INPUT_TYPE                      INPUT_1A1C2
#define ENABLE_HDMI                     0//((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_MHL_C1                   0
#define ENABLE_MHL_C2                   0
#define ENABLE_MHL_C3                   0
#define ENABLE_MHL                      (ENABLE_MHL_C1|ENABLE_MHL_C2|ENABLE_MHL_C3)
#define ENABLE_DVI                      0//((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 1//((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_LED_CONTROLLER           0//need to check difference between Solar and Russell
#define _NEW_SOG_DET_                   1//no SOG pin

#else
#message "please define Russel new board type for Port_Func_Enable"
#endif

#define MS_VGA_SOG_EN               1
#ifndef _NEW_SOG_DET_
#define _NEW_SOG_DET_               0
#endif
#define ENABLE_USB_INPUT            0

//#define ENABLE_RTE  1

////////////////////////////////////////////////////////////
// PANEL SELECTION
////////////////////////////////////////////////////////////
#ifdef FPGA
#define PanelType                   PanelAU20SVGA//PanelCMIM236HGJ_L21//PanelAU20SVGA // 800x600
#else
//#define PanelType                   PanelCMIM236HGJ_L21//PanelCMOM215HGE_L10//PanelLM230WF5TLD1//PanelLM220WE1//PanelTPM215HW01_HGE//PanelLM230WF5TLD1//PanelLM270WF4TLA1
#endif
////////////////////////////////////////////////////////////
// BRIGHTNESS CONTROL
////////////////////////////////////////////////////////////
#define BrightFreqByVfreq               1 // set brightness freq. by input Vfreq.
#define BRIGHTNESS_INVERSE              0
#define BRIGHTNESS_FREQ                 240 // unit: Hz
#define BRIGHTNESS_VSYNC_ALIGN          1

////////////////////////////////////////////////////////////
// HDMI
////////////////////////////////////////////////////////////
#define DHDMI_SIMPLAYHD_PATCH    0 // SimplayHD CTS 8-18

////////////////////////////////////////////////////////////
// AUDIO
////////////////////////////////////////////////////////////
#define AudioFunc                       1
#if AudioFunc
#define AUDIDO_ADC_HPF_N                1   // high pass filter N setting
#endif
#define VOLUME_INVERSE                  0
#define VOLUME_FREQ                     30000 // unit: Hz
#define VOLUME_VSYNC_ALIGN              0
#define DAC_IDLE_TONE_IMPROVEMENT       0
#define I2S_gothrough_DPGA       0

////////////////////////////////////////////////////////////
// OSD
////////////////////////////////////////////////////////////
#ifndef DisplayLogo
#define DisplayLogo                     NoBrand //MstarLOGO
#endif
#define LANGUAGE_TYPE                   ASIA    //SIXTEEN

////////////////////////////////////////////////////////////
// SYSTEM
////////////////////////////////////////////////////////////
#define AUTO_DATA_PATH              0
#define ENABLE_DPS                  0
#define ENABLE_DSC                  0
#define ENABLE_SUPER_RESOLUTION     0
#define ENABLE_SHARPNESS            0
#define ENABLE_IR_CONTROL			0	/// NEC Format Only

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

//#define ENABLE_R2_2DTO3D                0
#ifndef ENABLE_USB_INPUT
#define ENABLE_USB_INPUT                   0
#endif
#define ENABLE_OSD_3D_CONTROL           ENABLE_R2_2DTO3D
#define ENABLE_R2_CODE_ON_DRAM          0//(ENABLE_USB_INPUT || ENABLE_R2_2DTO3D)
#define ENBLAE_BDMA_INT                 0
//#define ENABLE_HK_DATA_ON_DRAM    1

#define ENABLE_TOUCH_PANEL                  0
#define ENABLE_TOUCH_PANEL_DATA_FROM_USB    0
#define ENABLE_TOUCH_PANEL_CTRL_OSD         0

#define ENABLE_POWER_BOARD_CONTROL          0 // control power board enter to saving mode
#define ENABLE_FREESYNC                     1
// Instead of switching EDID, it use MCCS VPC Code to indicate the status of FreeSync support.
// Requirement: The Byte 4 of HDMI FreeSync VSD Block has to be 0xE6, not 0x00.
#define ENABLE_HDMI_DRR_MCCS            (1 && ENABLE_FREESYNC)//wtx add AMD MCCS
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
#define FWVersion               "001"
#define ChangeDate              "20190228"

//////////////////////////////////////////////////////////////
// MHL CTS test
//////////////////////////////////////////////////////////////
#define ENABLE_MHL_CTS_TEST         (0 &&ENABLE_MHL)

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
