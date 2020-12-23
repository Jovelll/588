#ifndef _PanelLTM200KT01_
#define _PanelLTM200KT01_
#define PanelName               "LTM200KT01"

#define PanelDither     8

#define PanelTTL            0
#define PanelTCON       0
#define PanelLVDS       0xFF
#define PanelRSDS       0

#define LVDS_TIMode     0xff
#define PanelDualPort       0xFF //˫8
#define PanelSwapPort      0// 0xFF
#define PanelSwapOddML      0
#define PanelSwapEvenML     0
#define PanelSwapOddRB      0//xFF
#define PanelSwapEvenRB     0//xFF
#define PanelSwapMLSB       0

#define PanelDClkDelay          0
#define PanelInvDE              0
#define PanelInvDClk            0
#define PanelInvHSync           0
#define PanelInvVSync           0

#define LVDS_TI_BitModeSel          3 // TI_10BIT:1 ,TI_8BIT:2,TI_6BIT:3
#define PANEL_SWAP_PN   		0xff
// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA
#define PanelDCLKCurrent        1 // Dclk current
#define PanelDECurrent              1 // DE signal current
#define PanelOddDataCurrent         1 // Odd data current
#define PanelEvenDataCurrent    1 // Even data current

#define PanelOnTiming1          20//60//20 // time between panel & data while turn on power
#define PanelOnTiming2          450//350//220 // time between data & back light while turn on power
#define PanelOffTiming1         100//350//220 // 2005/5/3// time between back light & data while turn off power 
#define PanelOffTiming2         20//20//25 // 2005/5/3//10 // time between data & panel while turn off power 

#define PanelHSyncWidth        24// 48
#define PanelHSyncBackPorch    65// 78

#define PanelVSyncWidth        12// 3
#define PanelVSyncBackPorch   24//  12

#define PanelHStart                 (PanelHSyncWidth+PanelHSyncBackPorch)
#define PanelVStart                 (PanelVSyncWidth+PanelVSyncBackPorch)

#define PanelWidth      1600
#define PanelHeight     1200//900

#define PanelHTotal  1950// 2100//  2112
#define PanelVTotal   1254// 1250// 934

#define PanelMaxHTotal   2212// 2400//  2260
#define PanelMinHTotal   1758// 1680//  1976

#define PanelMaxVTotal   1280//  1580// 956
#define PanelMinVTotal    1202//  1340//    920

#define PanelDCLK             120//  104
#define PanelMaxDCLK          170//  180
#define PanelMinDCLK             88
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
#if 0//ModelName==OTS_US200DALW
#define DefCool_RedColor            111
#define DefCool_GreenColor          118
#define DefCool_BlueColor           125
#define DefNormal_RedColor          120
#define DefNormal_GreenColor        123
#define DefNormal_BlueColor         121
#define DefWarm_RedColor            125
#define DefWarm_GreenColor      124
#define DefWarm_BlueColor           117
#define DefsRGB_RedColor      DefWarm_RedColor
#define DefsRGB_GreenColor  DefWarm_GreenColor
#define DefsRGB_BlueColor     DefWarm_BlueColor
#elif 0//ModelName==OTS_UU200AGLW
#define DefCool_RedColor            105//111
#define DefCool_GreenColor          118
#define DefCool_BlueColor           128//125
#define DefNormal_RedColor          118//120
#define DefNormal_GreenColor        125//123
#define DefNormal_BlueColor         128//121
#define DefWarm_RedColor            128//125
#define DefWarm_GreenColor      128//124
#define DefWarm_BlueColor           128//117
#define DefsRGB_RedColor      DefWarm_RedColor
#define DefsRGB_GreenColor  DefWarm_GreenColor
#define DefsRGB_BlueColor     DefWarm_BlueColor
#else
#define DefCool_RedColor            129
#define DefCool_GreenColor          130
#define DefCool_BlueColor           155
#define DefNormal_RedColor          143
#define DefNormal_GreenColor        142
#define DefNormal_BlueColor         155
#define DefWarm_RedColor            155
#define DefWarm_GreenColor      155
#define DefWarm_BlueColor           155
#define DefsRGB_RedColor      DefWarm_RedColor
#define DefsRGB_GreenColor  DefWarm_GreenColor
#define DefsRGB_BlueColor     DefWarm_BlueColor
#endif

#endif
