#ifndef __PANEL__
#define __PANEL__
//#include "Board.h"
#include "Panel_Define.h"
#include "Types.h"
#include "Ms_reg.h"

#if ( PanelType == PanelM156B1L01 )
#include "PanelM156B1L01.h"

#elif ( PanelType   == PanelM190A1L02)
#include "PanelCMOM190A1L02.h"

#elif ( PanelType == PanelM185B1L01)
#include "PanelM185B1L01.h"

#elif ( PanelType == PanelLM185WH1TLA1)
#include "PanelLM185WH1TLA1.h"

#elif ( PanelType == PanelM216H1L01)
#include "PanelM216H1L01.h"

#elif (PanelType==PanelLM215WF1)
#include "PanelLM215WF1.h"

#elif(PanelType==PanelLM220WE1)
#include "PanelLM220WE1.h"

#elif (PanelType==PanelLTM200KT01)
#include "PanelLTM200KT01.h"

#elif ( PanelType == PanelLM230WF1 )
#include "PanelLM230WF1.h"

#elif ( PanelType   == PanelTPM200O1L02)
#include "PanelTPM200O1L02.h"

#elif ( PanelType   == PanelCLAA185W)
#include "PanelCLAA185W.h"

#elif ( PanelType   == PanelCLAA170WA02)
#include "PanelCLAA170WA02.h"

#elif ( PanelType   == PanelLM200WD3)
#include "PanelLM200WD3.h"

#elif ( PanelType   == PanelM270HW01)
#include "PanelM270HW01.h"
#elif ( PanelType   == PanelLTM230HT03)
#include "PanelLTM230HT03.h"

#elif ( PanelType   == PanelLTM200KT07)
#include "PanelLTM200KT07.h"

#elif (PanelType == PanelTPM215HW01_HGE)      //2011.9.16 cc add
#include "PanelTPM215HW01_HGE.h"

#elif (PanelType == PanelLM230WF5TLD1)
#include "PanelLM230WF5TLD1.h"

#elif (PanelType==PanelAUOM215HW01VB)
#include "PanelAUOM215HW01VB.h"

#elif (PanelType==PanelCMIM236HGJ_L21)
#include "PanelCMIM236HGJ_L21.h"

#elif (PanelType==PanelLTM220M1L09) //111012 Modify
#include "PanelLTM220M1L09.h"

#elif (PanelType==PanelLM215WF3_S2A2)
#include "PanelLM215WF3_S2A2.h"


#elif (PanelType==PanelBM230WF3_SJC2)
#include "PanelBM230WF3_SJC2.h"

#elif (PanelType==PanelAUOM240UW01)
#include "PanelAUOM240UW01.h"

#elif (PanelType==PanelAU20SVGA)
#include "PanelAU20SVGA.h"

#elif (PanelType==PanelCMO190)
#include "PanelCMO190.h"

#elif (PanelType==PanelAUOM185XW01)
#include "PanelAUOM185XW01.h"
#elif (PanelType==PanelCMOM215HGE_L10)//add by smc.lmw 20160421
#include "PanelCMOM215HGE.h"
#elif (PanelType==PanelAUOM238HVN01)//add by smc.lmw 20160421
#include "PanelAUOM238HVN01.h"
#elif (PanelType==PanelBOEMV238FHM_N30)//add by vs 20190122
#include "PanelBOEMV238FHM_N30.h"
#elif (PanelType==PanelM185BGE_L22)//add by vs 20190122
#include "PanelM185BGE_L22.h"
#elif (PanelType==PanelLD2150010)//add by smc.lmw 20160421
#include "PanelLD2150010.h"
#elif (PanelType == PaneleDPHestia_FHD_60HZ)
#include "Panel_eDP_Hestia_FHD_60HZ.h"
#elif (PanelType == PaneleDP_AUO_B116HAN03_FHD_60Hz)
#include "Panel_eDP_AUO_B116HAN03_FHD_60Hz.h"
#elif (PanelType == PaneleDP_AUO_B156HTN03_FHD_60Hz)
#include "Panel_eDP_AUO_B156HTN03_FHD_60Hz.h"
#elif (PanelType == PaneleDP_AUO_B140HAN01_FHD_60Hz)
#include "Panel_eDP_AUO_B140HAN01_FHD_60Hz.h"
#elif (PanelType == PaneleDP_INL_N133HCE_FHD_60Hz)
#include "Panel_eDP_INL_N133HCE_FHD_60Hz.h"
#elif (PanelType == PaneleDP_PAN_VVX13F009G00_FHD_60Hz)
#include "Panel_eDP_PAN_VVX13F009G00_FHD_60Hz.h"
#elif (PanelType == PaneleDP_LG_LP156WF6_FHD_60Hz)
#include "Panel_eDP_LG_LP156WF6_FHD_60Hz.h"

#elif (PanelType == PANEL_AUO_M238DAN01_2560_1440_60)
#include "PANEL_AUO_M238DAN01_2560_1440_60.h"
#elif (PanelType == PANEL_PANDA_LC320HU1A_2560_1440_60)
#include "PANEL_PANDA_LC320HU1A_2560_1440_60.h"
#elif ( PanelType   == PanelHT150X02_100_SI8L_XGA_3V)
#include "PanelHT150X02_100_SI8L_XGA_3V.h"
#elif ( PanelType   == PanelLQ150U1LH22_DO6L_1600X1200_3V)
#include "PanelLQ150U1LH22_DO6L_1600X1200_3V.h"
#elif ( PanelType   == PanelLTN150P2_L01_DO6L_1400X1050_3V)
#include "PanelLTN150P2_L01_DO6L_1400X1050_3V.h"
#elif ( PanelType   == PanelLTN150P2_1024X768_S6)
#include "PanelLTN150P2_1024X768_S6.h"
#elif ( PanelType   == PNL_LQ150X1LGN2A_1024X768_SI8L_3V)
#include "PNL_LQ150X1LGN2A_1024X768_SI8L_3V.h"
#elif ( PanelType   == PanelM190A1L02_1)
#include "PanelM190A1L02_1.h"
#elif ( PanelType   == PanelN150X4_1024X768_S8)
#include "PanelN150X4_1024X768_S8.h"
#elif ( PanelType   == PanelHT15X34_S8)
#include "PanelHT15X34_S8.h"
#elif ( PanelType   == PanelCMO190W)
#include "PanelCMO190W.h"
#elif ( PanelType   == PanelCMOM220J1L01)
#include "PanelCMOM220J1L01.h"
#elif ( PanelType   == PanelQD15AL_1680X1050_D08_3V)
#include "PanelQD15AL_1680X1050_D08_3V.h"
#elif ( PanelType   == PanelLTN154U2_L04_1920X1200_D08)
#include "PanelLTN154U2_L04_1920X1200_D08.h"
#elif ( PanelType   == PanelLTM190BT03_1440X900_D08_5V)
#include "PanelLTM190BT03_1440X900_D08_5V.h"
#elif ( PanelType   == PanelM190Z1_L01_1680X1050_D08_5V)
#include "PanelM190Z1_L01_1680X1050_D08_5V.h"



#endif


#ifndef PanelVfreq
#define PanelVfreq          (600)
#endif

#ifndef PanelMaxVfreq
#define PanelMaxVfreq   (750+12) // 75Hz
#endif

#ifndef PanelMinVfreq
#define PanelMinVfreq   (500-12) // 50Hz
#endif

#ifndef PanelMiniLVDS3
#define PanelMiniLVDS3	0
#endif

#ifndef PANEL_VCOM_ADJUST
#define PANEL_VCOM_ADJUST    0
#endif

#ifndef DefCool_RedColor
#define DefCool_RedColor            99//103//95//126
#define DefCool_GreenColor          111//115//105//126
#define DefCool_BlueColor           128//130//126//143
#define DefNormal_RedColor          116//117//115//105//140
#define DefNormal_GreenColor        122//125//123//109//135
#define DefNormal_BlueColor         128//130//143
#define DefWarm_RedColor            126//128//123//115//143
#define DefWarm_GreenColor      128//133//129//114//137
#define DefWarm_BlueColor           128//130//132
#define DefsRGB_RedColor      DefWarm_RedColor
#define DefsRGB_GreenColor  DefWarm_GreenColor
#define DefsRGB_BlueColor     DefWarm_BlueColor
//#message "please measure panel color temp setting"
#endif

#if (PanelType==PanelLTM190M2)
#define DITHER_COEFF    0xE4
#define DITHER_METHOD    0x12
#elif PanelType==PanelCLAA220WA02
#define DITHER_COEFF    0xE4
#define DITHER_METHOD    0x42
#elif PanelType==PanelTPM200O1L02
#define DITHER_COEFF    0xE4
#define DITHER_METHOD    0x40
#else
#define DITHER_COEFF    0x2D
#define DITHER_METHOD    0x42
#endif


#ifndef PANEL_SSC_MODULATION_DEF
#define PANEL_SSC_MODULATION_DEF    40 // unit: 1KHz, range 0~40 means 0~40KHz
#endif

#ifndef PANEL_SSC_PERCENTAGE_DEF
#define PANEL_SSC_PERCENTAGE_DEF      20  // unit: 0.1%, range 0~30 means 0~3%
#endif

#ifndef PANEL_SSC_MODULATION_MAX
#define PANEL_SSC_MODULATION_MAX    40 // unit: 1KHz, range 0~40 means 0~40KHz
#endif

#ifndef PANEL_SSC_PERCENTAGE_MAX
#define PANEL_SSC_PERCENTAGE_MAX      30 // unit: 0.1%, range 0~30 means 0~3%
#endif

#ifndef LVDS_VOLTAGE_TARGET
#define LVDS_VOLTAGE_TARGET  IGT_TARGET_250
#endif

#ifndef LVDS_SWING_TARGET
#define LVDS_SWING_TARGET  250 //unit:mV 
#endif

#ifndef ENABLE_UP_LOW_BOUNDARY_PROTECTION
#define ENABLE_UP_LOW_BOUNDARY_PROTECTION   1
#endif

#ifdef ENABLE_UP_LOW_BOUNDARY_PROTECTION    //do I_GEN_Tuning() before Turn on backlight
	#if CHIP_ID==CHIP_TSUMD||CHIP_ID==CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID==CHIP_TSUMJ||CHIP_ID==CHIP_TSUM9||CHIP_ID==CHIP_TSUMF
		#define ICON_Upper_Boundary  36
		#define ICON_Lower_Boundary  6
	#elif CHIP_ID==CHIP_TSUMU
		#define ICON_Upper_Boundary  47
		#define ICON_Lower_Boundary  0
	#elif CHIP_ID==CHIP_TSUML||CHIP_ID==CHIP_TSUMB||CHIP_ID==CHIP_TSUMY
		#define ICON_Upper_Boundary  20
		#define ICON_Lower_Boundary  0
	#elif CHIP_ID==CHIP_TSUM2 ||CHIP_ID==CHIP_TSUMV
		#define ICON_Upper_Boundary  32
		#define ICON_Lower_Boundary  7
	#elif CHIP_ID==CHIP_TSUMK
		#define ICON_Upper_Boundary  35
		#define ICON_Lower_Boundary  15
	//#elif CHIP_ID==CHIP_TSUME  //ROSE
	//	#define ICON_Upper_Boundary  32
	//	#define ICON_Lower_Boundary  7
	#else
		#warning "--------------------not define ICON_Upper/Lower_Boundary---------------------------"
	#endif
#endif

#ifndef LVDS_FAIL_TARGET
	#ifdef ENABLE_UP_LOW_BOUNDARY_PROTECTION
		#if CHIP_ID==CHIP_TSUMD||CHIP_ID==CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID==CHIP_TSUMJ||CHIP_ID==CHIP_TSUM9||CHIP_ID==CHIP_TSUMF
			#define LVDS_FAIL_TARGET  21
		#elif CHIP_ID==CHIP_TSUMU
			#define LVDS_FAIL_TARGET  23
		#elif CHIP_ID==CHIP_TSUML||CHIP_ID==CHIP_TSUMB||CHIP_ID==CHIP_TSUMY
			#define LVDS_FAIL_TARGET  8
		#elif CHIP_ID==CHIP_TSUM2||CHIP_ID==CHIP_TSUMV
			#define LVDS_FAIL_TARGET  19
		#elif CHIP_ID==CHIP_TSUMK
			#define LVDS_FAIL_TARGET  25			
		//#elif CHIP_ID==CHIP_TSUME  //ROSE
		//	#define LVDS_FAIL_TARGET  19
		#else
			#warning "--------------------not define LVDS_FAIL_TARGET---------------------------"
		#endif
	#else
		#define LVDS_FAIL_TARGET  0x2A // 0x3F*2/3
	#endif
#endif

#ifndef DISABLE_SHORT_LINE_TUNE
#define DISABLE_SHORT_LINE_TUNE  0//	1
#endif


#ifndef DISABLE_SHORT_FRAME
#define DISABLE_SHORT_FRAME    0//	1
#endif

#ifndef PANEL_SWAP_PN
#define PANEL_SWAP_PN     0XFF//  0
#endif

#ifndef PAIR_SWAP_B
#define PAIR_SWAP_B         0
#endif
#ifndef PANEL_SWAP_AB_PORT
#define PANEL_SWAP_AB_PORT  0 //0: non-swap, 0xFF: AB swap
#endif
#ifndef PANEL_SWAP_CD_PORT
#define PANEL_SWAP_CD_PORT  0//0: non-swap ,0xFF: CD swap
#endif

#ifndef PANEL_3D_PASSIVE
#define PANEL_3D_PASSIVE            0
#endif

#ifndef PANEL_3D_PASSIVE_4M
#define PANEL_3D_PASSIVE_4M         0
#endif

#ifndef PANEL_OUTPUT_FMT_3D_LR
#define PANEL_OUTPUT_FMT_3D_LR      0
#endif

#ifndef PANEL_OUTPUT_FMT_3D_CB
#define PANEL_OUTPUT_FMT_3D_CB      0
#endif

#ifndef PANEL_OUTPUT_FMT_3D_FS
#define PANEL_OUTPUT_FMT_3D_FS      0
#endif

#ifndef PANEL_OUTPUT_FMT_3D_TB
#define PANEL_OUTPUT_FMT_3D_TB      0
#endif

#ifndef PANEL_OUTPUT_FMT_3D_SBS
#define PANEL_OUTPUT_FMT_3D_SBS      0
#endif

#ifndef PANEL_QUAD_PORT
#define PANEL_QUAD_PORT      0
#endif

#ifndef LVDS_TI_BitModeSel
#define LVDS_TI_BitModeSel          2 // TI_10BIT:1 ,TI_8BIT:2,TI_6BIT:3
#endif

//#ifndef LVDS_CHANNEL
//#define LVDS_CHANNEL (MOD_PORTA|MOD_PORTB|MOD_PORTC|MOD_PORTD)
//#endif

#ifndef PANEL_V_SYNC_FREQ
#define PANEL_V_SYNC_FREQ   60
#endif

#ifndef LVDS_CH_A
#define LVDS_CH_A           0
#endif

#ifndef LVDS_CH_B
#define LVDS_CH_B           1
#endif

#ifndef LVDS_CH_C
#define LVDS_CH_C           2
#endif

#ifndef LVDS_CH_D
#define LVDS_CH_D           3
#endif

#ifndef LVDS_CH_A_SWAP
#define LVDS_CH_A_SWAP              LVDS_CH_A
#endif

#ifndef LVDS_CH_B_SWAP
#define LVDS_CH_B_SWAP              LVDS_CH_A
#endif

#ifndef LVDS_CH_C_SWAP
#define LVDS_CH_C_SWAP              LVDS_CH_C
#endif

#ifndef LVDS_CH_D_SWAP
#define LVDS_CH_D_SWAP              LVDS_CH_D
#endif

 // 111227 coding addition, if V(80pin) with dual LVDS output, enable the setting.
//#define DOUBLE_LVDS_CLK_SWING   (PanelDualPort && MainBoardType == MainBoard_5270_M0A)
 // 120920 coding modified, remove the setting of double LVDS clock swing to main board
 #ifndef DOUBLE_LVDS_CLK_SWING
 #define DOUBLE_LVDS_CLK_SWING      0
 #endif

// 120119 coding addition for Y / B type LED light bar control
// "00= 87mV 01= 127mV 10=160mV 11= 212mV"
#ifndef BOOST_CURRENT_LIMIT_VALUE
#define BOOST_CURRENT_LIMIT_VALUE   2
#endif

#define PANEL_VTT_3D_120    1588
#define PANEL_VTT_3D_110    1732
#define PANEL_VTT_3D_100    1905
#define PANEL_VTT_3D        PANEL_VTT_3D_120
#define LVDS_TI_10BIT       1
#define LVDS_TI_8BIT        2
#define LVDS_TI_6BIT        3

#if (PanelMaxVTotal>SC_MASK_V)
    #warning "--------------------PanelMaxVTotal Overs Scaler Limit---------------------------"
#endif

#endif
