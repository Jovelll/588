#ifndef TSUMXXU_DEMO_H
#define TSUMXXU_DEMO_H
////////////////////////////////////////////////////////////
// BOARD SELECTION
////////////////////////////////////////////////////////////
//#define TSUMXXNX
#define CHIP_ID     CHIP_TSUMU
//#define MainBoardType   MainBoard_MST125B // TSUMP58UHT5-1
#define MainBoardType   MainBoard_MST250B_D01A_S //TSUMU58UDT4-1 With eDP


////////////////////////////////////////////////////////////
// INPUT_TYPE SELECTION
////////////////////////////////////////////////////////////
#if MainBoardType == MainBoard_MST125B
#define INPUT_TYPE                      INPUT_1A1D1DP
#elif MainBoardType == MainBoard_MST250B_D01A_S
#define INPUT_TYPE                      INPUT_1A1D1H
#else
#define INPUT_TYPE                      INPUT_1A1D1DP
#endif

#define DVI_USB                         0 // 1: DVI from USB

#define ENABLE_HDMI                     (1&&(INPUT_TYPE == INPUT_1A1H || INPUT_TYPE == INPUT_1A1D1H || INPUT_TYPE == INPUT_1A2H||INPUT_TYPE == INPUT_1A1H1DP))
#define ENABLE_HDMI_1_4					(1&&ENABLE_HDMI)
#define ENABLE_DVI                      (1&&(INPUT_TYPE == INPUT_1A1D1H))
#define ENABLE_DP_INPUT                 (1&&(INPUT_TYPE == INPUT_1A1H1DP))
#define MS_VGA_SOG_EN                   1

#define ENABLE_EDP_OUTPUT				(1&&(MainBoardType == MainBoard_MST250B_D01A_S))
////////////////////////////////////////////////////////////
// PANEL SELECTION
////////////////////////////////////////////////////////////
#if ENABLE_EDP_OUTPUT
#define PanelType                        PaneleDP_LG_LP156WF6_FHD_60Hz//PaneleDP_PAN_VVX13F009G00_FHD_60Hz//PaneleDP_INL_N133HCE_FHD_60Hz///PaneleDP_AUO_B140HAN01_FHD_60Hz///PaneleDP_AUO_B116HAN03_FHD_60Hz///PaneleDP_AUO_B156HTN03_FHD_60Hz///PaneleDP_AUO_B116HAN03_FHD_60Hz///PaneleDPHestia_FHD_60HZ
#else
#define PanelType                        PanelLM220WE1   //    PanelLM230WF5TLD1//PanelLM270WF4TLA1
#endif

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
#define AudioFunc                       1///    0
#define VOLUME_INVERSE                  0
#define VOLUME_FREQ                        0 // unit: Hz
#define VOLUME_VSYNC_ALIGN              0
#define DAC_IDLE_TONE_IMPROVEMENT       0
////////////////////////////////////////////////////////////
// OSD
////////////////////////////////////////////////////////////
#define DisplayLogo                     MstarLOGO
#define LANGUAGE_TYPE                   ASIA//SIXTEEN
#define ENABLE_ROTATE_CLK_SET           0

////////////////////////////////////////////////////////////
// SYSTEM
////////////////////////////////////////////////////////////
#define ENABLE_DPS  1

#define FACTORYALIGN_TYPE               FACTORYALIGN_DDCCI

#define ENABLE_POWEROFF_1X_MCU_CLK          0

#define ENABLE_FREESYNC                     0
// Instead of switching EDID, it use MCCS VPC Code to indicate the status of FreeSync support.
// Requirement: The Byte 4 of HDMI FreeSync VSD Block has to be 0xE6, not 0x00.
#define ENABLE_HDMI_DRR_MCCS            (1 && ENABLE_FREESYNC)//wtx add AMD MCCS


#define ENABLE_FACTORY_SSCADJ           1
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
#define ChipName                            "TSUMU"
#define Model                                   "VS230PGLW"
#define FWVersion                           "002"
#define ChangeDate                          "20110905"

///////////////////////////////////////////////////////////////
// SYSTEM DEBUG
//////////////////////////////////////////////////////////////
#define ENABLE_DEBUG    1
//#define HDCPKEY_USE_CODE    1
///////////////////////////////////////////////////////////////
#endif

