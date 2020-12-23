#ifndef _PanelLM215WF3_S2A2_
#define _PanelLM215WF3_S2A2_


#define PanelName   "PanelLM215WF3"

#define PanelDither   8

#define PanelTTL    0
#define PanelTCON   0
#define PanelLVDS   0xff
#define PanelRSDS   0
#define PanelminiLVDS      0


#define LVDS_TIMode   0xff
#define PanelDualPort   0xff
#define PanelSwapPort   0xff
#define PanelSwapOddML    0
#define PanelSwapEvenML   0
#define PanelSwapOddRB    0
#define PanelSwapEvenRB   0
#define PanelSwapMLSB   0


#define PANEL_SWAP_PN       0xFF
#define PanelDClkDelay    0
#define PanelInvDE      0
#define PanelInvDClk    0
#define PanelInvHSync   0
#define PanelInvVSync   0
// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA
#define PanelDCLKCurrent  1 // Dclk current
#define PanelDECurrent    1 // DE signal current
#define PanelOddDataCurrent 1 // Odd data current
#define PanelEvenDataCurrent  1 // Even data current


#define PanelOnTiming1    25  // time between panel & data while turn on power
#define PanelOnTiming2    550 // time between data & back light while turn on power  //20050701
#define PanelOffTiming1    100 // time between back light & data while turn off power
#define PanelOffTiming2    25 // time between data & panel while turn off power

#define PanelHSyncWidth         10
#define PanelHSyncBackPorch 32
#define PanelVSyncWidth         2
#define PanelVSyncBackPorch 10 
#define PanelHStart   (PanelHSyncWidth+PanelHSyncBackPorch)
#define PanelVStart   (PanelVSyncWidth+PanelVSyncBackPorch)
#define PanelWidth    1920
#define PanelHeight   1080

#define PanelHTotal   2176
#define PanelVTotal   1100
#define PanelMaxHTotal     2320
#define PanelMinHTotal     2016
#define PanelMaxVTotal     1160
#define PanelMinVTotal    1090

#define PanelDCLK            144 // 74.25
#define PanelMaxDCLK      180 // 98
#define PanelMinDCLK       115 // 58.54
#define HV_OUTPUT_TYPE          HSRM_B


// 110929 Rick add
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
// Color Temp
//111103 Rick modified
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
