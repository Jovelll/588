#ifndef _PanelBM230WF3_
#define _PanelBM230WF3_
#define PanelName               "PanelBM230WF3"

#define PanelDither     8

#define PanelTTL            0
#define PanelTCON       0
#define PanelLVDS       0xFF
#define PanelRSDS       0

#define LVDS_TIMode     0xff
#define PanelDualPort       0xFF
#define PanelSwapPort       0xFF
#define PanelSwapOddML      0
#define PanelSwapEvenML     0
#define PanelSwapOddRB      0//xFF
#define PanelSwapEvenRB     0//xFF
#define PanelSwapMLSB       0

#define PANEL_SWAP_PN       0xFF
#define PanelDClkDelay          0
#define PanelInvDE              0
#define PanelInvDClk            0
#define PanelInvHSync           0
#define PanelInvVSync           0
// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA
#define PanelDCLKCurrent        1 // Dclk current
#define PanelDECurrent              1 // DE signal current
#define PanelOddDataCurrent         1 // Odd data current
#define PanelEvenDataCurrent    1 // Even data current

#define PanelOnTiming1          40//60//20 // time between panel & data while turn on power
#define PanelOnTiming2          550//550//350//220 // time between data & back light while turn on power
#define PanelOffTiming1         250//350//220 // 2005/5/3// time between back light & data while turn off power 
#define PanelOffTiming2         20//20//25 // 2005/5/3//10 // time between data & panel while turn off power 

#define PanelHSyncWidth       32 // 20//48
#define PanelHSyncBackPorch   48//  28//78

#define PanelVSyncWidth         2// 3
#define PanelVSyncBackPorch     14//12

#define PanelHStart                 (PanelHSyncWidth+PanelHSyncBackPorch)
#define PanelVStart                 (PanelVSyncWidth+PanelVSyncBackPorch)

#define PanelWidth      1920
#define PanelHeight     1080
#define PanelHTotal     2176
#define PanelVTotal     1100

#define PanelMaxHTotal      2240
#define PanelMinHTotal      2048
#define PanelMaxVTotal      1160
#define PanelMinVTotal          1090

#define PanelDCLK               144
#define PanelMaxDCLK            180//175
#define PanelMinDCLK             120//113

// 111003 Rick add
#define PanelVfreq          (600)
#define PanelMaxVfreq   (750+20) // 75Hz
#define PanelMinVfreq   (500-20) // 50Hz
//=================================================================
// TCON  setting for LVDS
//PVS / FSYNC
#define GPO9_VStart     0x7
#define GPO9_VEnd       0x705
#define GPO9_HStart     0x0
#define GPO9_HEnd       0x0
#define GPO9_Control        0x7

//HSYNC
#define GPOA_VStart     0x006
#define GPOA_VEnd       0x408
#define GPOA_HStart     0
#define GPOA_HEnd       0
#define GPOA_Control        0x4

//=================================================================
//Color Temp
#define DefCool_RedColor            117//114//111
#define DefCool_GreenColor          110//114
#define DefCool_BlueColor           127//130//117
#define DefNormal_RedColor          126//127
#define DefNormal_GreenColor        121//125
#define DefNormal_BlueColor         128//127
#define DefWarm_RedColor            130
#define DefWarm_GreenColor      126//132
#define DefWarm_BlueColor           128//130
#define DefsRGB_RedColor      DefWarm_RedColor
#define DefsRGB_GreenColor  DefWarm_GreenColor
#define DefsRGB_BlueColor     DefWarm_BlueColor

//==================================================================
//LED Backlight Control
#define LED_CH_0    1
#define LED_CH_1    1
#define LED_CH_2    1
#define LED_CH_3    1
#define LEDBL_PanelDefaultCurrent        	50			//need set by panel spec
#define PanelLightBarDefaultVoltage    		44			//need set by panel spec
#define PanelLightBarMaximumVoltage    	48			//need set by panel spec
#endif

