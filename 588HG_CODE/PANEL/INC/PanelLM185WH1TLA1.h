#define PanelName               "LM185WH1TLA1"


#define PanelDither         8
#define PanelTTL            0
#define PanelTCON       0
#define PanelLVDS       0xff
#define PanelRSDS       0
#define LVDS_TIMode             0xff
#define PanelDualPort       0xFF
#define PanelSwapPort               0
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

#define PANEL_SWAP_PN   0xff
// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA
#define PanelDCLKCurrent        1 // Dclk current
#define PanelDECurrent              1 // DE signal current
#define PanelOddDataCurrent         1 // Odd data current
#define PanelEvenDataCurrent    1 // Even data current
#define PanelOnTiming1          25 // 0//40 // time between panel & data while turn on power
#define PanelOnTiming2          500 // 300//300 //30 // time between data & back light while turn on power  //20050701
#define PanelOffTiming1             200 // 300 // time between back light & data while turn off power
#define PanelOffTiming2             25 // 20 // time between data & panel while turn off power
#define PanelHSyncWidth         35//53//51//42
#define PanelHSyncBackPorch     21//19//21//21//30
#define PanelVSyncWidth         3//12
#define PanelVSyncBackPorch     29//28
#define PanelHStart         (PanelHSyncWidth+PanelHSyncBackPorch)
#define PanelVStart         (PanelVSyncWidth+PanelVSyncBackPorch)
#define PanelWidth      1366
#define PanelHeight         768
#define PanelHTotal     1560 // 070410 // 2006/4/25 5:38PM by Emily 1840 // 2240//2256
#define PanelVTotal         806 // 1080//1200//1250
#define PanelMaxHTotal  2046 // 070410
#define PanelMinHTotal  1520//1446 
#define PanelMaxVTotal  1108
#define PanelMinVTotal  776 // 070410 // 1065 // 
#define PanelDCLK       86  // 070410 modified
#define PanelMaxDCLK    108 // 070410 modified
#define PanelMinDCLK        71// 070410 modified
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

