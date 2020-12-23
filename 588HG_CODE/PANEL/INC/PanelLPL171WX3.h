// for CPT 20
#define PanelName               "LPL171WX3"
#define PanelDither             8

#define PanelTTL                0
#define PanelTCON               0
#define PanelLVDS               0xFF
#define PanelRSDS               0
#define LVDS_TIMode     0xFF

#define PanelDualPort           0xFF
#if 0  //VA111111
#define PanelSwapPort           0x00
#else
#define PanelSwapPort           0xFF
#endif
#define PanelSwapOddML          0
#define PanelSwapEvenML         0
#define PanelSwapOddRB          0
#define PanelSwapEvenRB         0
#define PanelSwapMLSB       0

#define PanelDClkDelay          0
#define PanelInvDClk            0
#define PanelInvDE      0
#define PanelInvHSync           0
#define PanelInvVSync           0

// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA
#define PanelDCLKCurrent    1 // Dclk current
#define PanelDECurrent      1 // DE signal current
#define PanelOddDataCurrent 1 // Odd data current
#define PanelEvenDataCurrent    1 // Even data current

#define PanelOnTiming0      0 // time between 3.3V - 12V
#define PanelOnTiming1      30 // time between panel & data while turn on power
#define PanelOnTiming2      500//250//220//300 //30 // time between data & back light while turn on power   //20050701

#define PanelOffTiming1 250 // time between back light & data while turn off power
#define PanelOffTiming2 30 //50 // time between data & panel while turn off power
#define PanelOffTiming0 0 // time between 12V - 3.3V

#define PanelHSyncWidth     43 //40 //32
#define PanelHSyncBackPorch 41

#define PanelVSyncWidth 2
#define PanelVSyncBackPorch 38

#define PanelHStart     (PanelHSyncWidth+PanelHSyncBackPorch)
#define PanelVStart     (PanelVSyncWidth+PanelVSyncBackPorch)
#define PanelWidth      1440
#define PanelHeight     900
#define PanelHTotal     1904//1688
#define PanelVTotal     934//1066

#define PanelMaxHTotal      1960//1800
#define PanelMinHTotal      1600//2006-03-02 Andy 1688
#define PanelMaxVTotal      942//2000
#define PanelMinVTotal      926//1042//2006-03-10 900//2006-03-02 Andy 1042
#define PanelDCLK                   108
#define PanelMaxDCLK        200
#define PanelMinDCLK               89


//=================================================================
// TCON  setting for LVDS
//Input Format Control
#define GPO7_ClockBlank         1

//PVS / FSYNC
#define GPO9_VStart             0x7
#define GPO9_VEnd               0x705
#define GPO9_HStart             0x0
#define GPO9_HEnd               0x0
#define GPO9_Control            0x7

//HSYNC
#define GPOA_VStart             0x006
#define GPOA_VEnd               0x408
#define GPOA_HStart             0
#define GPOA_HEnd               0
#define GPOA_Control            0x4

