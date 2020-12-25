
#ifndef __PANEL_HSD170MGW_H__
#define __PANEL_HSD170MGW_H__


#define PanelNumber             1800
#define PanelName               "LTN150P2_L01_DO6L_1400X1050_3V"
//#define ModelName               "MSTAR UXGA"
#define PanelDither                  6

#define PanelTTL                    0
#define PanelTCON                   0
#define PanelLVDS                   0xff
#define PanelRSDS                   0

#define ShortLineCheck              0
#define LVDS_TIMode             0xff // 070301
#define PanelDualPort               0xff
#define PanelSwapPort             0//  0xff // 070301
#define PanelSwapOddML          0
#define PanelSwapEvenML         0
#define PanelSwapOddRB          0
#define PanelSwapEvenRB         0
#define PanelSwapMLSB               0

#define LVDS_TI_BitModeSel          3 // TI_10BIT:1 ,TI_8BIT:2,TI_6BIT:3


#define PanelDClkDelay              8
#define PanelInvDE                  0
#define PanelInvDClk                0
#define PanelInvHSync               0
#define PanelInvVSync               0


#define PANEL_SWAP_PN   		0xff
// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA
#define PanelDCLKCurrent        1 // Dclk current
#define PanelDECurrent              1 // DE signal current
#define PanelOddDataCurrent         1 // Odd data current
#define PanelEvenDataCurrent    1 // Even data current



#define PanelOnTiming0    0 // time between 3.3V - 12V
#define PanelOnTiming1      20 //80 // time between panel & data while turn on power
#define PanelOnTiming2      250//500 //80 // time between data & back light while turn on power 
#define PanelOffTiming1     250//150 //250 //80 // time between back light & data while turn off power 
#define PanelOffTiming2     20 //5 //80 // time between data & panel while turn off power 
#define PanelOffTiming0 0 // time between 12V - 3.3V


#define PanelHSyncWidth     32
#define PanelHSyncBackPorch 50

#define PanelVSyncWidth     4
#define PanelVSyncBackPorch 12

#define PanelHStart     (PanelHSyncWidth+PanelHSyncBackPorch)
#define PanelVStart     (PanelVSyncWidth+PanelVSyncBackPorch)

#define PanelWidth     1400// 1440
#define PanelHeight    1050// 900

#define PanelHTotal    2300//2050//1720// 1904//1728
#define PanelVTotal    1100// 934//1066

#define PanelMaxHTotal    2800//2240//1920//  2100//2000//1800
#define PanelMinHTotal     1950//2100//1950//1660// 1820//1700//1728

#define PanelMaxVTotal   1160//   968//942//2000
#define PanelMinVTotal    1090//  800//934//900 

#define PanelDCLK         90//  106//108
#define PanelMaxDCLK      170//  136//155 
#define PanelMinDCLK       80// 90//100



#define HV_OUTPUT_TYPE          HSRM_B
//=================================================================
// TCON  setting for RSDS
// TCON  setting for RSDS

#define TCON_Format1  0x1 // OFC1: control polarity & inversion

#define ESP_AfterData   0 // Even Start Pulse after Data
#define ESP_Offset  1 // Even start pulse position
#define OSP_AfterData   0 // Odd Start Pulse after Data
#define OSP_Offset  1 // Odd start pulse position
// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA
#define PanelOESPCurrent  2 // OSP/ESP drive current
#define PanelOEGCurrent   2 // OPOL/EPOL/GPOL drive current
#define PanelEINVCurrent  0 // EINV drive current
#define PanelOINVCurrent  0 // OINV drive current

#define OuputFormat   (BIT3|BIT5)//Enable Auto Toggle GPO0 & data invert
// POL
#define GPO0_VStart   0x000
#define GPO0_VEnd   0x000
#define GPO0_HStart   0x186 //0x068
#define GPO0_HEnd   0x186 //0x222
#define GPO0_Control    0x02 //0x02 //0x82
//LP
#define GPO1_VStart   0
#define GPO1_VEnd   0
#define GPO1_HStart   0x406 //0x407
#define GPO1_HEnd   0x41b //0x428
#define GPO1_Control  0x00

// STV
#define GPO2_VStart   0x000
#define GPO2_VEnd   0x001
#define GPO2_HStart   0x166 //x170
#define GPO2_HEnd   0x166 //x170
#define GPO2_Control  0x00

// CLKV
#define GPO3_VStart   0
#define GPO3_VEnd   0x00 //0x300
#define GPO3_HStart   0x166 //0x173
#define GPO3_HEnd   0x406 //0x407
#define GPO3_Control  0x01

// OE
#define GPO4_VStart   0
#define GPO4_VEnd   0
#define GPO4_HStart   0x384 //0x357
#define GPO4_HEnd   0x442 //0x427
#define GPO4_Control  0x00

//PVS / FSYNC
#define GPO9_VStart   0 //0x002
#define GPO9_VEnd   0 //0x306
#define GPO9_HStart   0 //0x00A
#define GPO9_HEnd   0 //0x005
#define GPO9_Control  0 //0x08

//HSYNC
#define GPOA_VStart   0
#define GPOA_VEnd   0
#define GPOA_HStart   0
#define GPOA_HEnd   0
#define GPOA_Control  0

//=================================================================
//Color Temp
#define DefCool_RedColor            132
#define DefCool_GreenColor          132
#define DefCool_BlueColor           150
#define DefNormal_RedColor          141
#define DefNormal_GreenColor        143
#define DefNormal_BlueColor         150
#define DefWarm_RedColor            143
#define DefWarm_GreenColor          150
#define DefWarm_BlueColor           133
#define DefsRGB_RedColor      DefWarm_RedColor
#define DefsRGB_GreenColor  DefWarm_GreenColor
#define DefsRGB_BlueColor     DefWarm_BlueColor
#endif

