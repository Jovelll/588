#ifndef __PANEL_LTM220M1L09__
#define __PANEL_LTM220M1L09__
#define PanelName   "TPM215HW01 HGE-L01"

#define PanelDither     	8

#define PanelTTL        	0
#define PanelTCON       	0
#define PanelLVDS       	0xFF
#define PanelRSDS       	0
#define PanelMiniLVDS     	0
#define PanelMiniLVDS1     	0
#define PanelMiniLVDS2     	0

#define LVDS_TIMode     	0xFF
#define PanelDualPort       0xFF
#define PanelSwapPort       0//0xFF  //默认为0xFF，字体发虚或者重影时可以调节0或0xFF
#define PanelSwapOddML      0
#define PanelSwapEvenML     0
#define PanelSwapOddRB      0//xFF
#define PanelSwapEvenRB     0//xFF
#define PanelSwapMLSB       0
#if 0//SingleBoard
#define PanelSwapLCPS       0xFF
#else
//#define PanelSwapLCPS       0
#endif

#define PanelDClkDelay      0
#define PanelInvDE      	0
#define PanelInvDClk        0
#define PanelInvHSync       0
#define PanelInvVSync       0

#define PANEL_SWAP_PN   0xff
#define LVDS_TI_BitModeSel          2// TI_10BIT:1 ,TI_8BIT:2,TI_6BIT:3


// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA
#define PanelDCLKCurrent    	1 // Dclk current
#define PanelDECurrent      	1 // DE signal current
#define PanelOddDataCurrent 	1 // Odd data current
#define PanelEvenDataCurrent    1 // Even data current

#define PanelOnTiming1      	25 // time between panel & data while turn on power
#define PanelOnTiming2      	550//400 // time between data & back light while turn on power
#define PanelOffTiming1     	150//60 // time between back light & data while turn off power
#define PanelOffTiming2     	25//10 // time between data & panel while turn off power

#define PanelHSyncWidth     	34//32
#define PanelHSyncBackPorch 	46//48

#define PanelVSyncWidth     	2
#define PanelVSyncBackPorch  	16

#define PanelHStart     	(PanelHSyncWidth+PanelHSyncBackPorch)
#define PanelVStart     	(PanelVSyncWidth+PanelVSyncBackPorch)
#define PanelWidth      	1680
#define PanelHeight     	1050
#define PanelHTotal     	1840
#define PanelVTotal     	1080

#define PanelMaxHTotal      2008
#define PanelMinHTotal      1856
#define PanelMaxVTotal      1100
#define PanelMinVTotal      1059
#define PanelDCLK           119
#define PanelMaxDCLK        151
#define PanelMinDCLK       	106

// 110929 Rick add
//#define PanelVfreq          (600)
//#define PanelMaxVfreq   	(770+20) // 77Hz
//#define PanelMinVfreq   	(530-20) // 53Hz

//=================================================================
// TCON  setting for LVDS
//PVS / FSYNC
#define GPO9_VStart     	0x7
#define GPO9_VEnd       	0x705
#define GPO9_HStart     	0x0
#define GPO9_HEnd       	0x0
#define GPO9_Control        0x7

//HSYNC
#define GPOA_VStart     	0x006
#define GPOA_VEnd       	0x408
#define GPOA_HStart     	0
#define GPOA_HEnd       	0
#define GPOA_Control        0x4

//=================================================================
// Color Temp
#define DefCool_RedColor            108
#define DefCool_GreenColor          128
#define DefCool_BlueColor           128
#define DefNormal_RedColor          118
#define DefNormal_GreenColor        128
#define DefNormal_BlueColor         128
#define DefWarm_RedColor            128
#define DefWarm_GreenColor      128
#define DefWarm_BlueColor           128
#define DefsRGB_RedColor      DefWarm_RedColor
#define DefsRGB_GreenColor  DefWarm_GreenColor
#define DefsRGB_BlueColor     DefWarm_BlueColor
#endif
