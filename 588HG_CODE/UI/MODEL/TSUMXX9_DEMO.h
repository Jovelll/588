#ifndef TSUMXX9_DEMO_H
#define TSUMXX9_DEMO_H
////////////////////////////////////////////////////////////
// BOARD SELECTION
////////////////////////////////////////////////////////////
#define CHIP_ID         CHIP_TSUM9
//#define MainBoardType   MainBoard_TSUMU589MT9_H
#define ENABLE_XTAL_LESS                0
#define ENABLE_XTAL_LESS_FPLL           (0 && ENABLE_XTAL_LESS)     // Note: FPLL will stay disabled when the input is DP
#define ENABLE_XTAL_LESS_SW_SLT         (0 && ENABLE_XTAL_LESS)
#define ENABLE_RESET_PIN                0
#define ENABLE_CABLE_5V_EDID            1
//#define FPGA

////////////////////////////////////////////////////////////
// INPUT_TYPE SELECTION
////////////////////////////////////////////////////////////
#define DVI_USB                         0       // 1: DVI from USB
#define DVI_PN_SWAP                     0
#define HDMI_PN_SWAP                    1
#define ENABLE_AUTOEQ                   0
#if MainBoardType == MainBoard_TSUMP589MG_DP    // 72 pins 1A1DP
#define INPUT_TYPE                      INPUT_1A1C2
#define ENABLE_HDMI                     0       //((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_MHL_C1                   0
#define ENABLE_MHL_C2                   0
#define ENABLE_MHL_C3                   0
#define ENABLE_MHL                      0//(ENABLE_MHL_C1|ENABLE_MHL_C2|ENABLE_MHL_C3)
#define ENABLE_DVI                      0       //((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 1       //((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_DP_AUDIO                 1
#define ENABLE_LED_CONTROLLER           0
#define AudioFunc                       0       // not support in 72 pins
#define _NEW_SOG_DET_                   1
#elif MainBoardType == MainBoard_TSUMO58CMG_H // 72 pins
#define INPUT_TYPE                      INPUT_1A1C2
#define ENABLE_HDMI                     1       //((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_MHL_C1                   0
#define ENABLE_MHL_C2                   0
#define ENABLE_MHL_C3                   0
#define ENABLE_MHL                      (ENABLE_MHL_C1|ENABLE_MHL_C2|ENABLE_MHL_C3)
#define ENABLE_DVI                      0       //((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 0       //((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_LED_CONTROLLER           0       //need to check difference between Solar and Russell
#define AudioFunc                       0       // not support in 72 pins
#define _NEW_SOG_DET_                   1
#elif MainBoardType == MainBoard_TSUMU589MT9_H  // 128 pins 1A1HDMI
#define INPUT_TYPE                      INPUT_1A1C2
#define ENABLE_HDMI                     1       //((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_MHL_C1                   1
#define ENABLE_MHL_C2                   0
#define ENABLE_MHL_C3                   0
#define ENABLE_MHL                      (ENABLE_MHL_C1|ENABLE_MHL_C2|ENABLE_MHL_C3)
#define ENABLE_DVI                      0       //((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 0       //((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_LED_CONTROLLER           0
#define _NEW_SOG_DET_                   0
#elif MainBoardType == MainBoard_TSUMP589HT9_DP // 128 pins 1A1DP
#define INPUT_TYPE                      INPUT_1A1C2
#define ENABLE_HDMI                     0       //((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_MHL_C1                   0
#define ENABLE_MHL_C2                   0
#define ENABLE_MHL_C3                   0
#define ENABLE_MHL                      0//(ENABLE_MHL_C1|ENABLE_MHL_C2|ENABLE_MHL_C3)
#define ENABLE_DVI                      0       //((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 1       //((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_DP_AUDIO                 1
#define ENABLE_LED_CONTROLLER           0
#define _NEW_SOG_DET_                   0
#elif MainBoardType == MainBoard_TSUMP589MG_H||MainBoardType == ZD_588HGV00R0XCOOB05 // new board 72 pins
#define INPUT_TYPE                      INPUT_1A1H//INPUT_1A1C2
#define ENABLE_HDMI                     1//0       //((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_MHL_C1                   0
#define ENABLE_MHL_C2                   0
#define ENABLE_MHL_C3                   0
#define ENABLE_MHL                      (ENABLE_MHL_C1|ENABLE_MHL_C2|ENABLE_MHL_C3)
#define ENABLE_DVI                      0//1       //((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 0       //((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_LED_CONTROLLER           0       //need to check difference between Solar and Russell
//#define AudioFunc                       0       // not support in 72 pins
#define _NEW_SOG_DET_                   1
#else
#message "please define Rockman new board type"
#endif

#define MS_VGA_SOG_EN                   1
#define ENABLE_USB_INPUT                0   //Alwasy set to 0

////////////////////////////////////////////////////////////
// PANEL SELECTION
////////////////////////////////////////////////////////////
#ifdef FPGA
#define PanelType                   PanelAU20SVGA   // 800x600
#else
//#define PanelType                   PanelCMIM236HGJ_L21
#endif

////////////////////////////////////////////////////////////
// BRIGHTNESS CONTROL
////////////////////////////////////////////////////////////
#define BrightFreqByVfreq               1   // set brightness freq. by input Vfreq.
#define BRIGHTNESS_INVERSE              0
#define BRIGHTNESS_FREQ                 240 // unit: Hz
#define BRIGHTNESS_VSYNC_ALIGN          1
#define BRIGHTNESS_HSYNC_ALIGN          0

////////////////////////////////////////////////////////////
// HDMI
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// AUDIO
////////////////////////////////////////////////////////////
#ifndef AudioFunc
#define AudioFunc                       (1 && (!ENABLE_XTAL_LESS))
#endif
#if AudioFunc
#define AUDIDO_ADC_HPF_N                0       // high pass filter N setting
#endif
#define VOLUME_INVERSE                  0
#define VOLUME_FREQ                     30000   // unit: Hz
#define VOLUME_VSYNC_ALIGN              0
#define DAC_IDLE_TONE_IMPROVEMENT       0
#define DEPOP_DCONOFF_FLOW       1

////////////////////////////////////////////////////////////
// OSD
////////////////////////////////////////////////////////////
//#define DisplayLogo                     MstarLOGO
#define LANGUAGE_TYPE                   ASIA    //SIXTEEN

////////////////////////////////////////////////////////////
// SYSTEM
////////////////////////////////////////////////////////////
#define AUTO_DATA_PATH                  0
#define ENABLE_DLC                    0//  1
#define ENABLE_DPS                      1
#define ENABLE_DSC                      1
#define ENABLE_SUPER_RESOLUTION         0
#define ENABLE_SHARPNESS                1

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

#define ENABLE_TOUCH_PANEL                  0
#define ENABLE_TOUCH_PANEL_DATA_FROM_USB    0
#define ENABLE_TOUCH_PANEL_CTRL_OSD         0

#define ENABLE_POWER_BOARD_CONTROL          0 // control power board enter to saving mode
#define ENABLE_FREESYNC                     0
// Instead of switching EDID, it use MCCS VPC Code to indicate the status of FreeSync support.
// Requirement: The Byte 4 of HDMI FreeSync VSD Block has to be 0xE6, not 0x00.
#define ENABLE_HDMI_DRR_MCCS            (1 && ENABLE_FREESYNC)//wtx add AMD MCCS

////////////////////////////////////////////////////////////
// POWERMANAGEMENT
////////////////////////////////////////////////////////////
#define MS_PM                           1
#define PM_MCUSLEEP                     0
#define LED_PWM                         0

////////////////////////////////////////////////////////////
// SI
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// INFORMATION
////////////////////////////////////////////////////////////
#define ChipName                "TSUM9"
#define Model                   "XXXXXXXXX"
#define FWVersion               "000"
#define ChangeDate              "20141028"

//////////////////////////////////////////////////////////////
// MHL CTS test
//////////////////////////////////////////////////////////////
#define ENABLE_MHL_CTS_TEST         (0&&ENABLE_MHL)

//////////////////////////////////////////////////////////////
// DP CTS test
// if enable, system will keep at normal speed without entering PM,
// and SrcInputType stays at FIXED_PORT
//////////////////////////////////////////////////////////////
#define ENABLE_DP_CTS_TEST         (0 &&ENABLE_DP_INPUT)


///////////////////////////////////////////////////////////////
// SYSTEM DEBUG
//////////////////////////////////////////////////////////////
#define ENABLE_DEBUG            1

#if ENABLE_MHL_CTS_TEST
#define DISABLE_AUTO_SWITCH     1 // for MHL test
#define FIXED_PORT      Input_HDMI
#elif ENABLE_DP_CTS_TEST
#define DISABLE_AUTO_SWITCH     1 // for DP test
#define FIXED_PORT      Input_Displayport
#else
#define DISABLE_AUTO_SWITCH   0
#define FIXED_PORT      Input_HDMI
#endif


#define ENABLE_3DLUT            0
#define LVDS_CLK_1TO2_OFFSET    4

#endif

