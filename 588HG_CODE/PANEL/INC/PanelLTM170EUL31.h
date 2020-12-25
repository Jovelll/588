//For HSD190ME12-A
#define PanelName   "LTM170EUL31"
#define PanelDither     8  //0109
#define PanelTTL        0
#define PanelTCON       0
#define PanelLVDS       0xFF
#define PanelRSDS       0
#define LVDS_TIMode         0xFF
#define PanelDualPort       0xFF
#define PanelSwapPort       0xFF
#define PanelSwapOddML      0
#define PanelSwapEvenML     0
#define PanelSwapOddRB      0//xFF
#define PanelSwapEvenRB     0//xFF
#define PanelSwapMLSB       0
#define PanelDClkDelay      0
#define PanelInvDE          0
#define PanelInvDClk               0
#define PanelInvHSync       0
#define PanelInvVSync       0
// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA
#define PanelDCLKCurrent           1 // Dclk current
#define PanelDECurrent      1 // DE signal current
#define PanelOddDataCurrent 1 // Odd data current
#define PanelEvenDataCurrent    1 // Even data current
#define PanelOnTiming1      20//for V2 version 80 // time between panel & data while turn on power   
#define PanelOnTiming2      550//for V2 version 80 // time between data & back light while turn on power 
#define PanelOffTiming1        120//for V2 version  80 // time between back light & data while turn off power 
#define PanelOffTiming2        10// for V2 version  80 // time between data & panel while turn off power 

#define PanelHSyncWidth     45//35//45//32
#define PanelHSyncBackPorch 20//40

#define PanelVSyncWidth     2
#define PanelVSyncBackPorch 38

#define PanelHStart         (PanelHSyncWidth+PanelHSyncBackPorch)
#define PanelVStart            (PanelVSyncWidth+PanelVSyncBackPorch)

#define PanelWidth             1280
#define PanelHeight            1024

#define PanelHTotal             1688//1840
#define PanelMaxHTotal          2260//1920
#define PanelMinHTotal          1352//1800//1760

#define PanelVTotal             1066//940
#define PanelMaxVTotal          1250//960
#define PanelMinVTotal          1031//908

#define PanelDCLK               108
#define PanelMaxDCLK            140
#define PanelMinDCLK                80
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
#define GPOA_Control        0x4

