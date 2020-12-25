#ifndef __PANEL_LTM220M1L09__
#define __PANEL_LTM220M1L09__
#define PanelName   "TPM215HW01 HGE-L01"
#define _PanelColorDepth                1//0->6bit+FRC                      bPanelColorDepth->8Bit
#define PanelTTL                        0
#define PanelTCON                   	0
#define PanelLVDS                   	0xff
#define PanelRSDS                   	0

#define _ChangeModePanelVddOff      	bChangeModePanelVddOff//0->Change Mode Panel Vdd On     bChangeModePanelVddOff->Change Mode Panel Vdd Off

#define LVDS_TIMode               		0xff
#define PanelDualPort               	0xff
#define PanelSwapPort               	0xff
#define PanelSwapOddML         			0
#define PanelSwapEvenML        			0
#define PanelSwapOddRB         			0
#define PanelSwapEvenRB        			0
#define PanelSwapMLSB           		0

#define _PanelOutputControl1            0x10//Scaler Bank 0x42 Setting
#define _PanelOutputControl2			( (PanelDualPort &BIT0) | (PanelSwapPort &BIT1))//Scaler Bank 0x43 Setting // Rosen 3043h Setting 2011.9.16 cc add
#define _PanelOSContol              	0x80//Scaler Bank 0xF2 Setting
#define _PanelODRriving             	0x55//Scaler Bank 0xF5 Setting

#define _PanelOnTiming1             	25// time between panel & data while turn on power
#define _PanelOnTiming2             	550 // time between data & back light while turn on power
#define _PanelOffTiming1                150// time between back light & data while turn off power
#define _PanelOffTiming2                25//20// time between data & panel while turn off power

#define _PanelHSyncWidth            	34
#define _PanelHSyncBackPorch        	46
#define _PanelVSyncWidth            	2
#define _PanelVSyncBackPorch        	16
#define _PanelHStart                    (_PanelHSyncWidth+_PanelHSyncBackPorch)
#define _PanelVStart                    (_PanelVSyncWidth+_PanelVSyncBackPorch)

#define PanelDClkDelay             		0
#define PanelInvDE                  	0
#define PanelInvDClk                	0
#define PanelInvHSync            		0
#define PanelInvVSync             		0

#define _PanelWidth      			1680
#define _PanelHeight     			1050
#define _PanelHTotal     			1840
#define _PanelVTotal     			1080
#define _PanelMaxHTotal      		2008
#define _PanelMinHTotal      		1856//1826
#define _PanelMaxVTotal      		1100
#define _PanelMinVTotal      		1059
#define _PanelDCLK           		119
#define _PanelMaxDCLK        		151
#define _PanelMinDCLK            	106

// 110929 Rick add
#define PanelVfreq          		(600)
#define PanelMaxVfreq   			(770+20) // 77Hz
#define PanelMinVfreq   			(530-20) // 53Hz

#define _DefBurstModeFreq           250//Hz
#define _DefMinDutyValue         	0x01
#define _DefMaxDutyValue            0xFF


#define _STEPL                      0x44
#define _STEPH                      0x00
#define _SPANL                      0xCC
#define _SPANH                      0x01

//=======================================================================
#define PanelDither                 8

#define _PanelLVDSSwing2			1// 0:242.37mV    1:339.33mV    2:387mV    3:169.67mV    //For TSUMXXNXX&TSUMXXQXX type ic auto tune target voltage
#define _PanelLVDSSwing1			0x15// 283mV //For old chip
#define _PanelLVDSSwing				(_PanelLVDSSwing1|_PanelLVDSSwing2<<6)

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

