#ifndef _BOARD_H_
#define _BOARD_H_

#define CHIP_TSUML      0//seraut
#define CHIP_TSUMU      1//leger
#define CHIP_TSUMB      2//sargent
#define CHIP_TSUMY      3//solar
#define CHIP_TSUMV      4//rosen
#define CHIP_TSUM2      5//juno
#define CHIP_TSUMC      6//russell
#define CHIP_TSUMD      7//junier
#define CHIP_TSUMJ      8
#define CHIP_TSUM9      9 //rockman
#define CHIP_TSUMF      10//rolex
#define CHIP_TSUMK      11
////////////////////////////////////////////////////////////
// INPUT_TYPE DEFINE
////////////////////////////////////////////////////////////
#define INPUT_1A            0
#define INPUT_1A1D          1
#define INPUT_1A2D          2
#define INPUT_1A1H          3
#define INPUT_1A2H          4
#define INPUT_2A2D          5
#define INPUT_1A1D1H        6
#define INPUT_1A1D1DP       7
#define INPUT_1A1H1DP       8
#define INPUT_1A2C          9 // 1A, combo portx2
#define INPUT_1A3C         	10 // 1A, combo portx3
#define INPUT_1A1C2        	11 // 1A1C, C = C2 port
#define INPUT_1A1D1H1DP		12// For MST9570S DVI DUAL LINK
//////////////////////////////////////////////////////////////
// KEY SELECTION
//////////////////////////////////////////////////////////////
#define KEY_TYPE_IOKEY                          0
#define KEY_TYPE_ADKEY                          1
#define KEY_TYPE_IOWITHADKEY                2
#define KEY_TYPE_I2CKEY_CAPACTIVE       3

////////////////////////////////////////////////////////////
// ODM_NAME DEFINE
////////////////////////////////////////////////////////////
#define ODM_AOC                     0
#define ODM_INL                     1
#define ODM_BENQ                    1

////////////////////////////////////////////////////////////
// FACTORYALIGN_TYPE DEFINE
////////////////////////////////////////////////////////////
#define FACTORYALIGN_RS232              0
#define FACTORYALIGN_DDCCI              1

////////////////////////////////////////////////////////////
// HDCPKEY SAVE DEFINE
////////////////////////////////////////////////////////////
#define HDCPKEY_USEFLASH            0
#define HDCPKEY_USE24C04            1
#define HDCPKEY_USE24C16            2

////////////////////////////////////////////////////////////
// MAINBOARD TYPE DEFINE
////////////////////////////////////////////////////////////
#define MainBoard_3661                  1
#define MainBoard_3598                  2  //TSUMU58NHL-LF-1,TSUMU58NHL-LF,TSUM5PLHL(Pin85->GPIO_P12/PWM3 is different with N Type Chip)
#define MainBoard_3836                  3
#define MainBoard_4432                  4  // N-type
#define MainBoard_4663                  5  // P Type
#define MainBoard_3893                  6  // TSUMO58QWDT3-LF-1
#define MainBoard_4266                  7  // TSUMW88MWDT3-LF-1  // Rex 100705 ASUS 2Dto3D demo
#define MainBoard_3834                  8  // 58NHL
#define MainBoard_3959                  9  // P -type (1A)
#define MainBoard_3244_USB              10 // TSUM1PLR-LF
#define MainBoard_4534                  11 // TSUMU58PWHL 100909

////////////////////////////////////////////////////////
#define MainBoard_MST135B               12 // TSUMU58VHN
#define MainBoard_5270_M0A              13 // TSUMU58VHN(80pin), 1A1D without audio
#define MainBoard_MST125B               14 // TSUMP58UHT5-1
#define MainBoard_5281_M0A              15 // TSUMU58UHN, 1A1D1DP +audio
#define MainBoard_MST139A               16 // TSUML58YDC2, 1A1D1HDMI
#define MainBoard_5269_M0A              17 // TSUML58YDC2, 1A1D
#define MainBoard_3834_M02              18 // TSUMV
#define MainBoard_MST144A_A02A          19 // TSUM2 1A2H
#define MainBoard_5598_M0A              20 // TSUM2 1A2H with MHL
#define MainBoard_TSUML887DC5           21 // TSUMC 1A1H1DP1X_LED
#define MainBoard_TSUMP387MT9_1_DP      22 // TSUMC 1A1H1DP1DP
#define MainBoard_TSUMP387MT9_1         23 // TSUMC 1A1H1H1H
#define MainBoard_tsuml88bdc2_1         24 // TSUMC 1A1D1H1X_LED, For Sargent Compatible, Sargent_tsum(o)L88bdc2(-1)
#define MainBoard_TSUML88CMC2           25 // TSUMC 1A2MHL_LED
#define MainBoard_TSUML18CN2            26 // TSUMC 1A_LED
#define MainBoard_5603_M0B              27 // TSUM2 1A2H with MHL for AOC_E2357
#define MainBoard_TSUML887DS_1          28 // TSUMD
#define MainBoard_TSUMOP38CMT9_HHD      29 // TSUMC/TSUMD
#define MainBoard_TSUMOP38CMT9_HDH      30 // TSUMC/TSUMD
#define MainBoard_TSUML58CN3            31 // TSUMC/TSUMD
#define MainBoard_TSUMO58CMG_H          32 // TSUMC/TSUM9
#define MainBoard_TSUMO58CMG_DP         33 // TSUMC
#define MainBoard_TSUMP589MG_DP         34 // TSUM9/TSUMF
#define MainBoard_TSUMU589MT9_H         35 // TSUM9/TSUMF
#define MainBoard_TSUMP589HT9_DP        36 // TSUM9/TSUMF
#define MainBoard_TSUMP589MG_H          37 // TSUM9/TSUMF
#define MainBoard_TSUMO58FMG_H_         38 // TSUMF
#define MainBoard_TSUMOP38CMT9_1A3C     39 // TSUMK 1A2DP1H
#define MainBoard_TSUMOLP88CDC5			40 // TSUMK 1A2D

#define ZD_588HGV00R0XCOOB05          41// TSUM9/TSUMF

#define MainBoard_MST125A_A01B_S		101// MST8556T linebuffer / MST9570S
#define MainBoard_MST250B_D01A_S		102// TSUMU58UDT4-1
#define MainBoard_TSUMO58FDT9_1A3C      103 // TSUMF
#define MainBoard_TSUMO58FDT9_VP58C11      104 // TSUMF /TSUMC
#define MainBoard_TSUMO58FDT9_VP58C41_B      105// TSUMF /TSUMC
#define MainBoard_TSUMU88CDT9_V8811      106 // TSUMC
#define MainBoard_TSUMO58FDT9_VSP58C31_3_IN_1      107 // TSUMC /TSUMF






#define MainBoard_Max                   255

//********************LogoType Define*******************************
#define NoBrand                       0
#define MstarLOGO                   1

#define TERRA						4
#define NEWONE						5
#define MLOGO                                        6
#define VIO4LOGO					7
#define VIOTEKLOGO					8
#define LONGWINLOGO               		9
#define NANCHANG              		       10
#define EPAI             		       		11
#define MAYA             		       		12
#define IMAGIC            		       	13
#define FANGZHENG            		       14
#define GRC            		                	15
#define BUBALUS           		              16
#define BUBALUSWHITE           		       17
#define SAMPIN           		       	18
#define PCTOP           		       	19
#define FOXIN           		       	20
#define ZEBION							21
#define SUGON           		       	22
#define SONGUAN           		       	23
#define INFORCE           		       	24
#define MAGIX							25
#define DRAZILPC						26
#define MEGAONIC						27

//*****************************************************************
//*******************Language Define********************************
#define ASIA                                1
//English,Spanish,France,Portugues,Russia,SChina,TChina,Korea,
#define EUROPE                          2
//English,Spanish,France,German,Italian,Russia,Swedish,Polish,Czech,
#define SIXTEEN                                 3
//English,German,France,Italian,Spanish,SChina,Japanese,Norsk,Russia,Swedish,Finnish,Dansk,Polish,Dutch,Portugues,Korea,
#define AllLanguage                                 4
//TChina,English,France,German,Italian,Japanese,Korea,Russia,Spanish,Swedish,Turkish,SChina,Portugues,Czech,Dutch,Finnish,Polish,

//*****************************************************************
//*****************************************************************
#include "Keypad_define.h"
#include "Customer_config.h"
#include "MODEL.H"
#include "DefaultCompilerOption.h"

#include "VS_CompilerOption.h"

#if (VGA_EDID_Write_To_24C02||DVI_EDID_Write_To_24C02||HDMI_EDID_Write_To_24C02)
 #define Internal_EDID_Write_To_24C02
#endif

#ifndef ENABLE_FACTORY_SSCADJ
#define ENABLE_FACTORY_SSCADJ               0
#endif

#ifndef ENABLE_PANELSERVICEMENU_SSCADJ
#define ENABLE_PANELSERVICEMENU_SSCADJ      0
#endif

#ifndef ENABLE_MENULOAD
#define ENABLE_MENULOAD                     0
#endif

#ifndef ML_USE_INT
#define ML_USE_INT                          0
#endif

#ifndef ENABLE_TIME_MEASUREMENT
#define ENABLE_TIME_MEASUREMENT             (1 && ENABLE_DEBUG)
#endif
#ifndef ENABLE_MCUTOPTEST
#define ENABLE_MCUTOPTEST                   0
#endif

#define DCRClearBacklight             // when drc is on in black pattern clear the backlight
//#if !defined(UseInternalDDCRam)
//#define UseVGACableReadWriteAllPortsEDID
//#endif

#if AudioFunc && (CHIP_ID==CHIP_TSUMU || CHIP_ID==CHIP_TSUMB || CHIP_ID==CHIP_TSUM2 || CHIP_ID==CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID==CHIP_TSUMD )
#define MS_DAC                      1
#define InitialDACByISR             1&MS_DAC
#define CheckInitialDACTime         0&MS_DAC
#define DAC_support_LineoutOnly     0&MS_DAC
#else   //CHIP_ID==CHIP_TSUML ,CHIP_TSUMY,CHIP_TSUMV,CHIP_TSUMJ
#define MS_DAC        0
#endif

#ifndef I2S_Earout_Simultaneously
#define I2S_Earout_Simultaneously       0
#endif
#ifndef I2S_gothrough_DPGA
#define I2S_gothrough_DPGA       0
#endif
#ifndef CASE1_LineIn_To_I2S
#define CASE1_LineIn_To_I2S       0// for TSUMD
#endif

#define CompressPropFont        1
//*****************************************************************
//*****************************************************************
#if DisplayLogo==NoBrand
#define DisplayPowerOnLogo          0
#else
#define DisplayPowerOnLogo          1
#endif

#define HotInputSelect          1
 
#if  ENABLE_HDMI
#define G2_G3_HDMI_AUDIO_FADING             0 // 1
#define EN_HDMI_TOSHIBA_DVD_PATCH           0 //When Toshiba HD_SA1 no disk DVD-P sends the Non-PCM format
#define ENABLE_CEC                              0
#define ENABLE_CEC_INT                          0
#define ENABLE_CTSNSYNC                     1
#define ENABLE_CTS_INT                          1
#define ENABLE_CTSN_FILTER                  1
#define ENABLE_CTS_HPD_PD100K       0
#define ENABLE_HDMI_SW_AUDCLK_CHECK     0
#define ENABLE_HDMI_SW_CTS_STABLE       0
#define ENABLE_HDMI_SW_AUDCLK_CHECK     0
#define ENABLE_AUDIO_AUTO_MUTE              1 // auto mute setting
#define ENABLE_AUDIO_AUTO_FADING        1 // auto fading setting
#define ENABLE_HAYDN_FADING_BY_HDMI_STATUS        0
#else
#define G2_G3_HDMI_AUDIO_FADING             0
#define ENABLE_CEC                              0
#define ENABLE_CEC_INT                          0
#define ENABLE_CTSNSYNC                     0
#define ENABLE_CTS_INT                          0
#define ENABLE_CTSN_FILTER                  0
#define ENABLE_CTS_HPD_PD100K       0
#define ENABLE_HDMI_1_4                 0
#define ENABLE_HDMI_SW_AUDCLK_CHECK     0
#define ENABLE_HDMI_SW_CTS_STABLE       0
#define ENABLE_HDMI_SW_AUDCLK_CHECK     0
#define ENABLE_AUDIO_AUTO_MUTE              0
#define ENABLE_AUDIO_AUTO_FADING        0
#define ENABLE_HAYDN_FADING_BY_HDMI_STATUS        0
#endif

#define Enable_CascadeMode              0
#define Enable_Gamma                    1
#define USE_NORMAL_GAMMA_TBL            (1&Enable_Gamma)

#define DDCCI_ENABLE                    1

#define PANEL_WIDTH                 PanelWidth
#define PANEL_HEIGHT                PanelHeight
#include "PANEL.H"

#define Enable_Expansion                1

#define AOC_RSerialOSD                  1

#define MWEFunction                     0

#define Enable_Cache                    1

#define DATA_LENGTH_20_BYTE//For HDCP Tool Send 20Byte Data One Block
//*****************************************************************

#define HotExpansion        Enable_Expansion&&!AudioFunc

#define PropFontUseCommonArea           1

#define HDMI_PRETEST                    0

#include "reg52.h"
#if (MainBoardType == MainBoard_5270_M0A)
#define USEFLASH    1
#include "bd_5270_M0A.h"
#elif (MainBoardType ==MainBoard_5281_M0A)
#define USEFLASH    1
#include "bd_5281_M0A.h"
#elif (MainBoardType ==MainBoard_5269_M0A)
#define USEFLASH    1
#include "bd_5269_M0A.h"
#elif (MainBoardType ==MainBoard_MST125B)
#define USEFLASH    1
#include "bd_MST125B.h"
#elif (MainBoardType == MainBoard_MST135B)
#define USEFLASH    1
#include "bd_MST135B.h"
#elif (MainBoardType == MainBoard_MST139A)
#define USEFLASH    1
#include "bd_MST139A.h"
#elif (MainBoardType == MainBoard_3834_M02) //111012 Modify
#define USEFLASH    1
#include "bd_3834_M02.h"
#elif (MainBoardType == MainBoard_MST144A_A02A)
#define USEFLASH    1
#include "bd_MST144A_A02A.h"
#elif (MainBoardType == MainBoard_5598_M0A)
#define USEFLASH    1
#include "bd_5598_M0A.h"
#elif (MainBoardType == MainBoard_TSUML887DC5)
#define USEFLASH    1
#include "bd_TSUML887DC4-1.h"
#elif (MainBoardType == MainBoard_TSUMP387MT9_1_DP)
#define USEFLASH    1
#include "bd_TSUMP387MT9-1_DP.h"
#elif (MainBoardType == MainBoard_TSUMP387MT9_1)
#define USEFLASH    1
#include "bd_TSUMP387MT9-1.h"
#elif (MainBoardType == MainBoard_tsuml88bdc2_1)
#define USEFLASH    1
#include "bd_tsuml88bdc2-1.h"
#elif (MainBoardType == MainBoard_TSUML88CMC2)
#define USEFLASH   1
#include "bd_TSUML88CMC2.h"
#elif (MainBoardType == MainBoard_TSUML18CN2)
#define USEFLASH    1
#include "bd_TSUML18CN2.h"
#elif (MainBoardType == MainBoard_TSUML58CN3)
#define USEFLASH    1
#include "bd_TSUML58CN3.h"
#elif (MainBoardType == MainBoard_TSUML887DS_1)
#define USEFLASH    1
#include "bd_TSUML887DS_1.h"
#elif (MainBoardType == MainBoard_TSUMOP38CMT9_HHD)
#define USEFLASH    1
#include "bd_TSUMOP38CMT9_HHD.h"
#elif (MainBoardType == MainBoard_TSUMOP38CMT9_HDH)
#define USEFLASH    1
#include "bd_TSUMOP38CMT9_HDH.h"
#elif (MainBoardType == MainBoard_TSUMO58CMG_H)
#define USEFLASH    1
#include "bd_TSUMO58CMG_H.h"
#elif (MainBoardType == MainBoard_TSUMO58CMG_DP)
#define USEFLASH    1
#include "bd_TSUMO58CMG_DP.h"
#elif (MainBoardType == MainBoard_TSUMP589MG_DP)
#define USEFLASH    1
#include "bd_TSUMP589MG_DP.h"
#elif (MainBoardType == MainBoard_TSUMU589MT9_H)
#define USEFLASH    1
#include "bd_TSUMU589MT9_H.h"
#elif (MainBoardType == MainBoard_TSUMO58FDT9_VP58C11)
#define USEFLASH    1
#include "bd_TSUMO58FDT9_VP58C11.h"
#elif (MainBoardType == MainBoard_TSUMO58FDT9_VSP58C31_3_IN_1)
#define USEFLASH    1
#include "bd_TSUMO58FDT9_VSP58C31_3_IN_1.h"
#elif (MainBoardType == MainBoard_TSUMO58FDT9_VP58C41_B)
#define USEFLASH    1
#include "bd_TSUMO58FDT9_VP58C41_B.h"
#elif (MainBoardType == MainBoard_TSUMU88CDT9_V8811)
#define USEFLASH    1
#include "bd_TSUMU88CDT9_V8811.h"
#elif (MainBoardType == MainBoard_TSUMP589HT9_DP)
#define USEFLASH    1
#include "bd_TSUMP589HT9_DP.h"
#elif (MainBoardType == MainBoard_TSUMP589MG_H)
#define USEFLASH    1
#include "bd_TSUMP589MG_H.h"
#elif (MainBoardType == ZD_588HGV00R0XCOOB05)
#define USEFLASH    1
#include "ZD_588HGV00R0XCOOB05.h"
#elif (MainBoardType == MainBoard_TSUMO58FMG_H_)
#define USEFLASH    1
#include "bd_TSUMO58FMG_H.h"
#elif (MainBoardType == MainBoard_TSUMOP38CMT9_1A3C)
#define USEFLASH    1
#include "bd_TSUMOP38CMT9_1A3C.h"
#elif (MainBoardType == MainBoard_TSUMOLP88CDC5)
#define USEFLASH    1
#include "bd_TSUMOLP88CDC5.h"

#elif (MainBoardType == MainBoard_MST125A_A01B_S)
#define USEFLASH    1
#include "bd_MST125A_A01B_S.h"
#elif (MainBoardType == MainBoard_MST250B_D01A_S)
#define USEFLASH    1
#include "bd_MST250B_D01A_S.h"
#elif (MainBoardType == MainBoard_TSUMO58FDT9_1A3C)
#define USEFLASH    1
#include "bd_TSUMO58FDT9_1A3C.h"

#endif

// to reduce power consumption during monitor active
#define _IP_POWER_CTRL_

#ifndef Init_hwHDCP_Hpd_Pin2
#define Init_hwHDCP_Hpd_Pin2()
#endif

#ifndef Init_hwHDCP_Hpd_Pin3
#define Init_hwHDCP_Hpd_Pin3()
#endif

#ifndef CABLE_DETECT_VGA_USE_SAR
#define CABLE_DETECT_VGA_USE_SAR    0
#endif

#ifndef CABLE_DETECT_DVI_USE_SAR
#define CABLE_DETECT_DVI_USE_SAR    0
#endif

#ifndef CABLE_DETECT_HDMI_USE_SAR
#define CABLE_DETECT_HDMI_USE_SAR   0
#endif

#ifndef CABLE_DET_SAR_EN
#define CABLE_DET_SAR_EN            0
#endif

// Chip definition
#ifndef CHIP_ID
#define CHIP_ID                     CHIP_TSUMV
#endif

#ifndef ENABLE_DP_INPUT
#define ENABLE_DP_INPUT             0
#endif

#ifndef ENABLE_DP_AUDIO
#define ENABLE_DP_AUDIO             0
#endif

#ifndef ENABLE_DVI_DUAL_LINK
#define ENABLE_DVI_DUAL_LINK        0
#endif

#ifndef ENABLE_XIU_WRITE_KEY
#define ENABLE_XIU_WRITE_KEY        0
#endif

#ifndef  ENABLE_LED_CONTROLLER
#define ENABLE_LED_CONTROLLER       0
#endif

#ifndef ENABLE_SUPER_RESOLUTION
#define ENABLE_SUPER_RESOLUTION     0
#endif

#ifndef ENABLE_SHARPNESS
#define ENABLE_SHARPNESS            0
#endif

#ifndef USE_DAC_ADJ
#define USE_DAC_ADJ                 0
#endif

#ifndef ENABLE_DLC
#define ENABLE_DLC                  0
#endif

#ifndef ENABLE_DPS
#define ENABLE_DPS                  0
#endif

#ifndef ENABLE_DSC
#define ENABLE_DSC                  0
#endif

#ifndef ENABLE_DeBlocking
#define ENABLE_DeBlocking           0
#endif

// keep using DOUBLE_LVDS_CLK_SWING, removed this new define
//#ifndef LVDS_CLK_1TO2
//#define LVDS_CLK_1TO2               0
//#endif

// 120106 coding addition, default set adc dithering to disable
#if 0 // move to DefaultComplierOption.h
#ifndef ENABLE_ADC_DITHERING
#define ENABLE_ADC_DITHERING    0
#endif
#endif

//I_Gen_Tuning Target Select
#if CHIP_ID == CHIP_TSUMV || CHIP_ID == CHIP_TSUM2
#define IGT_TARGET_250  0
#define IGT_TARGET_300  1
#define IGT_TARGET_200  2
#define IGT_TARGET_350  3
#elif CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK ||CHIP_ID == CHIP_TSUMD || CHIP_ID == CHIP_TSUMJ || CHIP_ID == CHIP_TSUM9 || CHIP_ID == CHIP_TSUMF
#define IGT_TARGET_250  0
#define IGT_TARGET_350  1
#define IGT_TARGET_300  2
#define IGT_TARGET_200  3
#else
#define IGT_TARGET_250  0
#define IGT_TARGET_300  1
#define IGT_TARGET_350  2
#define IGT_TARGET_200  3
#endif
//I_Gen_Tuning CH Source Select
#if (CHIP_ID == CHIP_TSUMC) || CHIP_ID==CHIP_TSUMK ||(CHIP_ID == CHIP_TSUMD)||(CHIP_ID == CHIP_TSUMJ)||(CHIP_ID == CHIP_TSUM9)||(CHIP_ID == CHIP_TSUMF)
#define CH2             0
#define CH6             1
#define CH8             2
#else
#define CH3_CLK         0
#define CH9_CLK         1
#define CH15_CLK        2
#define CH21_CLK        3
#endif
//LVDS_CHANNEL select
#define MOD_PORTA       BIT0
#define MOD_PORTB       BIT1
#define MOD_PORTC       BIT2
#define MOD_PORTD       BIT3

//TMDS PORT
#define TMDS_PORT_UNUSE 0
#define TMDS_PORT_A     BIT0
#define TMDS_PORT_B     BIT1
#define TMDS_PORT_C     BIT2
////////////////////////////////////////////////////////
// Frame buffer setting
////////////////////////////////////////////////////////
#define FRAME_BUFFER        0
#define FRAME_BFFLESS       1
#if 0 // 120403 coding reserved
#if 1//MS_OVD // check it again
    #ifndef FRAME_BFF_SEL
        #if (CHIP_ID==CHIP_TSUMU)
            #if   PANEL_3D_PASSIVE_4M
            #define FRAME_BFF_SEL       FRAME_BUFFER
            #define ENABLE_RTE        1
            #else
            #define FRAME_BFF_SEL       FRAME_BFFLESS
            #define ENABLE_RTE        1
            #endif
     #elif CHIP_ID==CHIP_TSUM2
            #define FRAME_BFF_SEL       FRAME_BUFFER
            #define ENABLE_RTE        0
        #else
            #define FRAME_BFF_SEL       FRAME_BFFLESS
            #define ENABLE_RTE        0
        #endif
    #endif
#else
  #define FRAME_BFF_SEL       FRAME_BFFLESS
  #define ENABLE_RTE        0
#endif
#endif

#ifndef PANEL_3D_PASSIVE_4M
#define PANEL_3D_PASSIVE_4M     0
#endif

#ifndef FRAME_BFF_SEL
#define FRAME_BFF_SEL   FRAME_BFFLESS
#endif

#define DOWN_COUNTER_V_SYNC     0 // 120403 coding

//OD mode
#if (CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK)
#define OD_MODE_444             0
#define OD_MODE_565             1
#define OD_MODE_666             4
#define OD_MODE_485             5
#define OD_MODE_555             6
#define OD_MODE_888             7
#elif (CHIP_ID == CHIP_TSUMF)
#define OD_MODE_444             0
#define OD_MODE_565             1
#define OD_MODE_666             4
#define OD_MODE_485        		5
#define OD_MODE_555             6
#else
#if CHIP_ID != CHIP_TSUMD            //Junior does not support 444 mode
#define OD_MODE_444             0
#endif
#define OD_MODE_565             1
#define OD_MODE_333             5   //compress mode
#define OD_MODE_555             6
#define OD_MODE_485             8
#endif

#ifndef OD_MODE
#if CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK ||(CHIP_ID == CHIP_TSUMF)
#define OD_MODE                                         OD_MODE_485
//#elif (CHIP_ID == CHIP_TSUMU && (FRAME_BFF_SEL == FRAME_BFFLESS))
//#define OD_MODE                                         OD_MODE_333
#elif (CHIP_ID == CHIP_TSUMU && (FRAME_BFF_SEL == FRAME_BUFFER))
#define OD_MODE                                         OD_MODE_565
#else
#define OD_MODE                                         OD_MODE_333
#endif
#endif

#define OD_HSCAL_EN             (_ENABLE && (OD_MODE == OD_MODE_565 || OD_MODE == OD_MODE_485))// MST9570S USE OD_MODE_333 -- MST8757T USE OD_MODE_565
#if (CHIP_ID == CHIP_TSUMF)
#define OD_VSCAL_EN             _ENABLE
#else
#define OD_VSCAL_EN             _DISABLE
#endif

// 3D glasses type
#define GLASSES_NONE    0
#define GLASSES_IR      1
#define GLASSES_INTEL   2
#define GLASSES_NVIDIA  3

#ifndef GLASSES_TYPE
#define GLASSES_TYPE          GLASSES_NONE
#endif


#if CHIP_ID == CHIP_TSUM2 || CHIP_ID == CHIP_TSUMC ||CHIP_ID == CHIP_TSUMD ||CHIP_ID == CHIP_TSUMJ ||CHIP_ID == CHIP_TSUM9 ||CHIP_ID == CHIP_TSUMF
#define ENABLE_SW_DOUBLE_BUFFER   1
#else
#define ENABLE_SW_DOUBLE_BUFFER   0
#endif

#ifndef ENABLE_HK_CODE_ON_DRAM
#define ENABLE_HK_CODE_ON_DRAM  0
#endif

#ifndef ENABLE_HK_XDATA_ON_DRAM
#define ENABLE_HK_XDATA_ON_DRAM     0
#endif

#if CHIP_ID==CHIP_TSUMU
#define PANEL_H_DIV             2 // H relative parameters(display timing) divided by 1 or 2
#else
#define PANEL_H_DIV             1 // H relative parameters(display timing) divided by 1 or 2
#endif

#if PANEL_H_DIV ==2
#define ACC_HDIV_BIT    BIT15
#else
#define ACC_HDIV_BIT    0
#endif

#ifndef _TMDS_DE_MODE_EN_
#define _TMDS_DE_MODE_EN_   1
#endif

#define EN_V4TAP_SCALING            0 //(CHIP_ID==CHIP_TSUMB || CHIP_ID==CHIP_TSUMY) //for vertical 4-tap scaling mode

#ifndef ENABLE_3D_FUNCTION
#define ENABLE_3D_FUNCTION  0
#endif

#ifndef MST_ASSERT
#define MST_ASSERT(_test_)
#endif

#ifndef EnableTime1Interrupt
#define EnableTime1Interrupt  	0
#endif

#ifndef EnableTime2Interrupt
#define EnableTime2Interrupt  0
#endif

#if (ENABLE_XTAL_LESS)
#define XTAL_CLOCK_KHZ              (12000ul)
#define CRYSTAL_CLOCK               (12000000ul)
#else
#define XTAL_CLOCK_KHZ              (14318ul)
#define CRYSTAL_CLOCK               (14318000ul)
#endif
#define DEF_FORCE_DPMS      0///30 // unit: Second


#define WATCH_DOG_RESET_TIME        (10)    // unit: second
#define WATCH_DOG_Interrupt_TIME    (3)     // unit: second
#define EXT_TIMER0_1MS              (0)     // external timer 0 for 1ms interrupt
#define EXT_TIMER0_CounterTime      (1)     //unit: ms default set to 1ms

// 110905 coding, addition for getting more V to reduce output dclk
#define DECREASE_V_SCALING  1
#define INT_PERIOD          1// timer interrupt: unit = 1ms
//#define BaudRatePeriod    (0x10000 - (((unsigned long)CPU_CLOCK_MHZ * 1000000)/((unsigned long) SERIAL_BAUD_RATE / 10 * 32) + 5) / 10)
#define SystemTick  1000    // 1 second
#define UseINT      1
#define UsesRGB     0

#define ClockPeriod (0x10000-(((unsigned long)XTAL_CLOCK_KHZ*INT_PERIOD)/12))    // Interrupt every 1ms for Time 0
#define BaudRatePeriod  (0x10000-(((unsigned long)XTAL_CLOCK_KHZ * 1000+(DWORD)SERIAL_BAUD_RATE*16)/((DWORD)SERIAL_BAUD_RATE*32)))
#define T1BaudRatePeriod    (0x100-(((unsigned long)XTAL_CLOCK_KHZ * 1000+(DWORD)SERIAL_BAUD_RATE*192)/((DWORD)SERIAL_BAUD_RATE*384)))
#define HFreq(hPeriod)  ((hPeriod)?(((DWORD)XTAL_CLOCK_KHZ*10+hPeriod/2)/hPeriod):(1)) // 120330 coding modified to avoid divide 0
#define VFreq(hFreq, vTotal)    ((SrcVTotal)?(((DWORD)hFreq*1000+SrcVTotal/2)/SrcVTotal):(1)) // 120330 coding modified to avoid divide 0
#define MST_HPeriod_UINT    (XTAL_CLOCK_KHZ*16)
#define VFreqIn         (((VFreq( HFreq( SrcHPeriod ), SrcVTotal )+5)/10) * ((SrcFlags&bInterlaceMode)?2:1))

////////////////////////////////////////////////////////////
// TSUM9 AC POWER ON FLAG
////////////////////////////////////////////////////////////
#if (CHIP_ID == CHIP_TSUM9||CHIP_ID == CHIP_TSUMF)
#define AC_POWER_ON_FLAG    (_bit7_(MEM_MSREAD_BYTE(REG_0183)))
#endif

//define DEFAULT_BACKLIGHT_CURRENT			400
//#define ENABLE_POWER_PORTECT			 1// ENABLE

//ÏÞ¹¦ÂÊ
#if 0//(MainBoardType==MainBoard_TSUMP589MG_H||MainBoardType==ZD_588HGV00R0XCOOB05)
   #undef ENABLE_POWER_PORTECT
   #define ENABLE_POWER_PORTECT				1// ENABLE
  
 #define MAX_CURRENT_VALUE				400// 
		
 #define PowerProtect_v0	0
 #define PowerProtect_w0	20
		 
 #define PowerProtect_v1	60
 #define PowerProtect_w1	12
		 
 //#define PowerProtect_v2	40
 //#define PowerProtect_w2	20
		
 //#define PowerProtect_v3	50
 //#define PowerProtect_w3	18
		 
 //#define PowerProtect_v4	60
 //#define PowerProtect_w4	12
 #elif(MainBoardType==MainBoard_TSUML88CMC2)
   #undef ENABLE_POWER_PORTECT
   #define ENABLE_POWER_PORTECT				1// ENABLE
  
 #define MAX_CURRENT_VALUE				480// 
		 
 #define PowerProtect_v0	0
 #define PowerProtect_w0	20
		  
 #define PowerProtect_v1	60
 #define PowerProtect_w1	12

#else
#define ENABLE_POWER_PORTECT			 _DISABLE

#endif

#endif


#if (Customer_Type_Sel==Customer_XINTAO) || (Customer_Type_Sel==Customer_KAINENG)
#define Enable_LED				1
#else
#define Enable_LED				0
#endif



