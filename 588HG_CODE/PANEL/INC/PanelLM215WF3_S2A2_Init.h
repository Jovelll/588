#ifndef _PanelLM215WF3_S2A2_
#define _PanelLM215WF3_S2A2_

#define PanelName   "PanelLM215WF3"
#define _PanelColorDepth                1//0->6bit+FRC                      bPanelColorDepth->8Bit
#define PanelTTL                        0
#define PanelTCON                   0
#define PanelLVDS                   0xff
#define PanelRSDS                   0

#define _ChangeModePanelVddOff      bChangeModePanelVddOff//0->Change Mode Panel Vdd On     bChangeModePanelVddOff->Change Mode Panel Vdd Off

#define LVDS_TIMode               0xff
#define PANEL_SWAP_PN           0xff
#define PanelDualPort               0xff
#define PanelSwapPort               0xff
#define PanelSwapOddML         0
#define PanelSwapEvenML        0
#define PanelSwapOddRB         0
#define PanelSwapEvenRB        0
#define PanelSwapMLSB           0

#define _PanelOutputControl1            0x10//Scaler Bank 0x42 Setting
#define _PanelOutputControl2			( (PanelDualPort &BIT0) | (PanelSwapPort &BIT1))//Scaler Bank 0x43 Setting // Rosen 3043h Setting 2011.9.16 cc add
#define _PanelOSContol              0x80//Scaler Bank 0xF2 Setting
#define _PanelODRriving             0x55//Scaler Bank 0xF5 Setting

#define _PanelOnTiming1          40//60//20 // time between panel & data while turn on power
#define _PanelOnTiming2          550//550//350//220 // time between data & back light while turn on power
#define _PanelOffTiming1         250//350//220 // 2005/5/3// time between back light & data while turn off power 
#define _PanelOffTiming2         20//20//25 // 2005/5/3//10 // time between data & panel while turn off power 

#define _PanelHSyncWidth             10
#define _PanelHSyncBackPorch     32
#define _PanelVSyncWidth         2
#define _PanelVSyncBackPorch     10
#define _PanelHStart                    (_PanelHSyncWidth+_PanelHSyncBackPorch)
#define _PanelVStart                    (_PanelVSyncWidth+_PanelVSyncBackPorch)

#define PanelDClkDelay             0
#define PanelInvDE                  0
#define PanelInvDClk                0
#define PanelInvHSync            0
#define PanelInvVSync             0

#define _PanelWidth      1920
#define _PanelHeight     1080
#define _PanelHTotal     2176
#define _PanelVTotal     1100
#define _PanelMaxHTotal      2320
#define _PanelMinHTotal      2016
#define _PanelMaxVTotal      1160
#define _PanelMinVTotal      1090

#define _PanelDCLK           144
#define _PanelMaxDCLK        180//175
#define _PanelMinDCLK            115//113

#define _DefBurstModeFreq           250//Hz
#define _DefMinDutyValue                0x01
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

// 111003 Rick add
#define PanelVfreq          (600)
#define PanelMaxVfreq   (750+20) // 75Hz
#define PanelMinVfreq   (500-20) // 50Hz

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