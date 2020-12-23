// for CPT RSDS 17"
//#define   PanelName   _C,_L,_A,_A,_1,_7,_0,_E,_A,_0,_7,_P//"CLAA170EA07P"
#define PanelName       "CLAA170EA07P"
#define PANEL_TYPE          PANEL_17

#define PanelDither     6

#define PanelTTL        0
#define PanelTCON       0
#define PanelLVDS   0xff
#define PanelRSDS   0

#define LVDS_TIMode   0xff
#define PanelDualPort   0xff
#define PanelSwapPort   0xff
#define PanelSwapOddML    0
#define PanelSwapEvenML   0
#define PanelSwapOddRB    0
#define PanelSwapEvenRB   0
#define PanelSwapMLSB       0
#define PanelSwapLCPS            0

#define PanelDClkDelay      8
#define PanelInvDClk        0
#define PanelInvDE      0
#define PanelInvHSync       0
#define PanelInvVSync       0

// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA
#define PanelDCLKCurrent    1 // Dclk current
#define PanelDECurrent      1 // DE signal current
#define PanelOddDataCurrent 1 // Odd data current
#define PanelEvenDataCurrent    1 // Even data current

// 070125 modify period of PanelOnTiming1 and PanelOnTiming2
#define PanelOnTiming1      5//50//20//15//20//Delay1ms * 2 already. 60//10 //80 // time between panel & data while turn on power
#define PanelOnTiming2      250//180 time between data & back light while turn on power
//#define PanelOnTiming3        100
#define PanelOffTiming1     250//10//80//Delay1ms * 2 already. 60//200 // time between back light & data while turn off power
#define PanelOffTiming2     25//10//80//Delay1ms * 2 already. 60//10 // time between data & panel while turn off power
//#define PanelOffTiming3       10

#define PanelHSyncWidth        15//32
#define PanelHSyncBackPorch 15//40

#define PanelVSyncWidth        2
#define PanelVSyncBackPorch 34

#define PanelHStart     (PanelHSyncWidth+PanelHSyncBackPorch)
#define PanelVStart     (PanelVSyncWidth+PanelVSyncBackPorch)
#define PanelWidth      1280
#define PanelHeight     1024
#define PanelHTotal     1688
#define PanelVTotal     1066

#define PanelMaxHTotal      1600
#define PanelMinHTotal      1400//1440//1520//1400//1340
#define PanelMaxVTotal      770
#define PanelMinVTotal      730
#define PanelDCLK           120
#define PanelMaxDCLK        140
#define PanelMinDCLK            112
#define HV_OUTPUT_TYPE          HSRM_B
//=================================================================
#define TCON_Format1    0x1 // OFC1: control polarity & inversion

#define ESP_AfterData   0 // Even Start Pulse after Data
#define ESP_Offset  1 // Even start pulse position
#define OSP_AfterData   0 // Odd Start Pulse after Data
#define OSP_Offset  1 // Odd start pulse position

// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA
#define OuputFormat     (DATI_B|G0AT_B)//Enable Auto Toggle GPO0 & data invert

#define PanelOESPCurrent    2 // OSP/ESP drive current
#define PanelOEGCurrent     2 // OPOL/EPOL/GPOL drive current
#define PanelEINVCurrent    0 // EINV drive current
#define PanelOINVCurrent    0 // OINV drive current

// POL
#define GPO0_VStart   0x000
#define GPO0_VEnd   0x000
#define GPO0_HStart   0x186 //0x068
#define GPO0_HEnd   0x186 //0x222
#define GPO0_Control    0x02 //0x02 //0x82
//HSYNC
#define GPOA_VStart   0
#define GPOA_VEnd   0
#define GPOA_HStart   0
#define GPOA_HEnd   0
#define GPOA_Control  0

