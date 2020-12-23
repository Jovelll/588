#ifndef _PanelLTM200KT07_
#define _PanelLTM200KT07_
#define PanelName               "LTM200KT07"

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

#define PanelOnTiming1          20//60//20 // time between panel & data while turn on power
#define PanelOnTiming2          650//550//350//220 // time between data & back light while turn on power
#define PanelOffTiming1         150//350//220 // 2005/5/3// time between back light & data while turn off power 
#define PanelOffTiming2         20//20//25 // 2005/5/3//10 // time between data & panel while turn off power 

#define PanelHSyncWidth         16//48
#define PanelHSyncBackPorch     16//78

#define PanelVSyncWidth         2// 3
#define PanelVSyncBackPorch     16//12

#define PanelHStart                 (PanelHSyncWidth+PanelHSyncBackPorch)
#define PanelVStart                 (PanelVSyncWidth+PanelVSyncBackPorch)

#define PanelWidth      1600
#define PanelHeight     900
#define PanelHTotal     2112
#define PanelVTotal     934

#define PanelMaxHTotal      2300
#define PanelMinHTotal      1900
#define PanelMaxVTotal      955
#define PanelMinVTotal          906

#define PanelDCLK               120
#define PanelMaxDCLK            153
#define PanelMinDCLK             97
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
#if ModelName==OTS_US200DALW
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
#elif ModelName==OTS_UU200AGLW
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

