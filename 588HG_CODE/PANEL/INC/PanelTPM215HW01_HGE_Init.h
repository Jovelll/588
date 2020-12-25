#ifndef _PanelTPM215HW01_HGE_
#define _PanelTPM215HW01_HGE_

#define PanelName   "TPM215HW01 HGE-L01"
#define _PanelColorDepth                1//0->6bit+FRC                      bPanelColorDepth->8Bit
#define PanelTTL                        0
#define PanelTCON                   0
#define PanelLVDS                   0xff
#define PanelRSDS                   0

#define _ChangeModePanelVddOff      bChangeModePanelVddOff//0->Change Mode Panel Vdd On     bChangeModePanelVddOff->Change Mode Panel Vdd Off

#define LVDS_TIMode               0xff
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

#define _PanelOnTiming1             30// time between panel & data while turn on power
#define _PanelOnTiming2             500 // time between data & back light while turn on power
#define _PanelOffTiming1                140// time between back light & data while turn off power
#define _PanelOffTiming2                5//20// time between data & panel while turn off power

#define _PanelHSyncWidth            34
#define _PanelHSyncBackPorch        46
#define _PanelVSyncWidth            2
#define _PanelVSyncBackPorch        16
#define _PanelHStart                    (_PanelHSyncWidth+_PanelHSyncBackPorch)
#define _PanelVStart                    (_PanelVSyncWidth+_PanelVSyncBackPorch)

#define PanelDClkDelay             0
#define PanelInvDE                  0
#define PanelInvDClk                0
#define PanelInvHSync            0
#define PanelInvVSync             0

#define _PanelWidth      1920
#define _PanelHeight     1080
#define _PanelHTotal     2200
#define _PanelVTotal     1125
#define _PanelMaxHTotal      2300
#define _PanelMinHTotal      2100
#define _PanelMaxVTotal      1136
#define _PanelMinVTotal      1115
#define _PanelDCLK           149
#define _PanelMaxDCLK        196
#define _PanelMinDCLK            117

// 110929 Rick add
#define PanelVfreq          (600)
#define PanelMaxVfreq   (750+20) // 75Hz
#define PanelMinVfreq   (500-20) // 50Hz

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
#endif