#ifndef __PANEL_AU170N5E_
#define __PANEL_AU170N5E_
#define PanelName   "AU170N5E"


#define PanelDither         6

#define PanelTTL            0
#define PanelTCON           0
#define PanelLVDS           0xFF
#define PanelRSDS           0
#define PanelMiniLVDS3	    0

#define ShortLineCheck      0
#define LVDS_TIMode         0xFF
#define PanelDualPort       0
#define PanelSwapPort       0
#define PanelSwapOddML      0
#define PanelSwapEvenML     0
#define PanelSwapOddRB      0xFF
#define PanelSwapEvenRB     0
#define PanelSwapMLSB       0
#define PanelSwapPlusMinus  0



#define PanelDClkDelay      0
#define PanelInvDE          0
#define PanelInvDClk        0
#define PanelInvHSync       0
#define PanelInvVSync       0

// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA
#define PanelDCLKCurrent        1 // Dclk current
#define PanelDECurrent          1 // DE signal current
#define PanelOddDataCurrent     1 // Odd data current
#define PanelEvenDataCurrent    1 // Even data current

#define PanelOnTiming1          30  // time between panel & data while turn on power
#define PanelOnTiming2          100// time between data & back light while turn on power
#define PanelOffTiming1         20 // time between back light & data while turn off power
#define PanelOffTiming2         20//16  // time between data & panel while turn off power

#define PanelHSyncWidth       3//  128      // a multiple of 8
#define PanelHSyncBackPorch    22// 88//32      // a multiple of 8

#define PanelVSyncWidth       72//  4
#define PanelVSyncBackPorch    200// 20

#define PanelHStart     (PanelHSyncWidth+PanelHSyncBackPorch)
#define PanelVStart     (PanelVSyncWidth+PanelVSyncBackPorch)
#define PanelWidth      800
#define PanelHeight    1280// 600


#define PanelHTotal   831//  1056
#define PanelVTotal   1680// 1688// 630

#define PanelMaxHTotal  2000// 1500//1300
#define PanelMinHTotal  1352// 1035

#define PanelMaxVTotal  862// 750
#define PanelMinVTotal 803// 610

#define PanelDCLK       (((DWORD)PanelHTotal*PanelVTotal*60)/1000000)
#define PanelMaxDCLK    (((DWORD)PanelHTotal*PanelMaxVTotal*60)/1000000)//49
#define PanelMinDCLK    (((DWORD)PanelMinHTotal*PanelMinVTotal*60)/1000000)//39
//=================================================================
// TCON  setting for LVDS
//PVS / FSYNC
#define GPO9_VStart     0x7
#define GPO9_VEnd       0x705
#define GPO9_HStart     0x0
#define GPO9_HEnd       0x0
#define GPO9_Control    0x7

//HSYNC
#define GPOA_VStart     0x006
#define GPOA_VEnd       0x408
#define GPOA_HStart     0
#define GPOA_HEnd       0
#define GPOA_Control    0x4

#define PanelVfreq      (600)

//Color Temp
#define DefCool_RedColor        117//114//111
#define DefCool_GreenColor      110//114
#define DefCool_BlueColor       127//130//117
#define DefNormal_RedColor      126//127
#define DefNormal_GreenColor    121//125
#define DefNormal_BlueColor     128//127
#define DefWarm_RedColor        130
#define DefWarm_GreenColor      126//132
#define DefWarm_BlueColor       128//130
#define DefsRGB_RedColor        DefWarm_RedColor
#define DefsRGB_GreenColor      DefWarm_GreenColor
#define DefsRGB_BlueColor       DefWarm_BlueColor

#endif