#define PanelName              "TPM185B1XW01"

#define PanelDither         6//8
#define PanelTTL            0
#define PanelTCON       0
#define PanelLVDS       0xff
#define PanelRSDS       0
#define LVDS_TIMode             0xff
#define PanelDualPort               0
#if MainBoardType == MainBoard_3244_USB
#define PanelSwapPort               0xFF
#else
#define PanelSwapPort               0
#endif
#define PanelSwapOddML          0
#define PanelSwapEvenML         0
#define PanelSwapOddRB          0
#define PanelSwapEvenRB         0
#define PanelSwapMLSB               0
#define PanelDClkDelay              8
#define PanelInvDE                  0
#define PanelInvDClk                0
#define PanelInvHSync               0xFF
#define PanelInvVSync               0xff
// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA
#define PanelDCLKCurrent        1 // Dclk current
#define PanelDECurrent              1 // DE signal current
#define PanelOddDataCurrent         1 // Odd data current
#define PanelEvenDataCurrent    1 // Even data current
#define PanelOnTiming1          20 // 0//40 // time between panel & data while turn on power
#define PanelOnTiming2          600 // 300//300 //30 // time between data & back light while turn on power  //20050701
#define PanelOffTiming1             600 // 300 // time between back light & data while turn off power
#define PanelOffTiming2             10 // 20 // time between data & panel while turn off power
#define PanelHSyncWidth     35//53//51//42
#define PanelHSyncBackPorch 21//19//21//21//30
#define PanelVSyncWidth         3//12
#define PanelVSyncBackPorch     29//28
#define PanelHStart         (PanelHSyncWidth+PanelHSyncBackPorch)
#define PanelVStart         (PanelVSyncWidth+PanelVSyncBackPorch)
#define PanelWidth      1366
#define PanelHeight         768
#define PanelHTotal     1606 // 070410 // 2006/4/25 5:38PM by Emily 1840 // 2240//2256
#define PanelVTotal         768 // 1080//1200//1250
#define PanelMaxHTotal  2047 // 070410
#define PanelMinHTotal  1416 // 070410 //1790 // 1820 // 1860// 1800 // 1880
#define PanelMaxVTotal  1023
#define PanelMinVTotal  776 // 070410 // 1065 //
#define PanelDCLK       78  // 070410 modified
#define PanelMaxDCLK    90 // 070410 modified
#define PanelMinDCLK        55// 070410 modified
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
#define DefCool_RedColor                    129
#define DefCool_GreenColor                136
#define DefCool_BlueColor                   148
#define DefNormal_RedColor               146
#define DefNormal_GreenColor           148
#define DefNormal_BlueColor              147
#define DefWarm_RedColor                 148
#define DefWarm_GreenColor              148
#define DefWarm_BlueColor                141
#define DefsRGB_RedColor                   148
#define DefsRGB_GreenColor               148
#define DefsRGB_BlueColor                  141