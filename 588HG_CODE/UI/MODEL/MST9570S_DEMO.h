#ifndef MST9570S_DEMO_H
#define MST9570S_DEMO_H
////////////////////////////////////////////////////////////
// BOARD SELECTION
////////////////////////////////////////////////////////////
//#define TSUMXXNX
#define CHIP_ID     CHIP_TSUMU
#define MainBoardType   MainBoard_MST125A_A01B_S //MST9570S/8556T Line buffer

////////////////////////////////////////////////////////////
// INPUT_TYPE SELECTION
////////////////////////////////////////////////////////////
#define INPUT_TYPE                      INPUT_1A1D1H1DP///INPUT_1A1H1DP

#define DVI_USB                         0 // 1: DVI from USB
#define DVI_PN_SWAP                     0
#define HDMI_PN_SWAP                    0
#define ENABLE_AUTOEQ                   0

#define ENABLE_HDMI                     (1&&(INPUT_TYPE == INPUT_1A1H || INPUT_TYPE == INPUT_1A1D1H || INPUT_TYPE == INPUT_1A2H||INPUT_TYPE == INPUT_1A1H1DP || INPUT_TYPE == INPUT_1A1D1H1DP))
#define ENABLE_HDMI_1_4                 (1&&ENABLE_HDMI)
#define ENABLE_DVI                      1
#define ENABLE_DP_INPUT                 1
#define MS_VGA_SOG_EN                   1
#define ENABLE_DVI_DUAL_LINK			(1&&ENABLE_DVI)// dvi dual link
#define DL_PXL_RAT_DET      			(1&&ENABLE_DVI_DUAL_LINK)   /* DVI DUAL Link detect by pixel rate */
////////////////////////////////////////////////////////////
// PANEL SELECTION
////////////////////////////////////////////////////////////
#define PanelType                        PANEL_PANDA_LC320HU1A_2560_1440_60///PanelLM230WF5TLD1//PANEL_AUO_M238DAN01_2560_1440_60   //    PanelLM230WF5TLD1//PanelLM270WF4TLA1

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
#define VOLUME_FREQ                        0 // unit: Hz
#define VOLUME_VSYNC_ALIGN              0
#define DAC_IDLE_TONE_IMPROVEMENT       0
#define ENABLE_DAC_DEPOP				1
////////////////////////////////////////////////////////////
// OSD
////////////////////////////////////////////////////////////
#define DisplayLogo                     MstarLOGO
#define LANGUAGE_TYPE                   ASIA//SIXTEEN
#define ENABLE_ROTATE_CLK_SET           0

////////////////////////////////////////////////////////////
// SYSTEM
////////////////////////////////////////////////////////////
#define ENABLE_DPS  0///1

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

#define ENABLE_CTS_HPD_PD100K			0
#define ENABLE_FREESYNC                 1
#define ENABLE_RTE             			1
// Instead of switching EDID, it use MCCS VPC Code to indicate the status of FreeSync support.
// Requirement: The Byte 4 of HDMI FreeSync VSD Block has to be 0xE6, not 0x00.
#define ENABLE_HDMI_DRR_MCCS            (1 && ENABLE_FREESYNC)//wtx add AMD MCCS

#define ENABLE_INTERNAL_HDMIEDID
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

