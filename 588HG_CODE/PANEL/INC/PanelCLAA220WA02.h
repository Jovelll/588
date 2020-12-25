#ifndef _PanelCLAA220WA02
#define _PanelCLAA220WA02
#define PanelName "CLAA220WA02"

#define PanelDither     6  //2006-07-27 Andy 8//2006-07-14 Andy 6//2006-03-02

#define PanelTTL        0
#define PanelTCON       0
#define PanelLVDS       0
#define PanelRSDS       0xFF

#define ShortLineCheck      0
#define LVDS_TIMode         0x00
#define PanelDualPort       0xFF
#define PanelSwapPort       0x0
#define PanelSwapOddML      0x00
#define PanelSwapEvenML     0x00
#define PanelSwapOddRB      0xFF
#define PanelSwapEvenRB     0xFF
#define PanelSwapMLSB       0xFF

#define PanelDClkDelay      8//0
#define PanelInvDClk               0
#define PanelInvDE             0
#define PanelInvHSync       0
#define PanelInvVSync       0

// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA
#define PanelDCLKCurrent     1 // Dclk current
#define PanelDECurrent       1 // DE signal current
#define PanelOddDataCurrent  1 // Odd data current
#define PanelEvenDataCurrent 1 // Even data current




#define PanelOnTiming1  20//25  // time between DCDC & TCON turn on         // William 20051011, for VGH 2 steps go High    //Amy2008.1.3   17:30:07
#define PanelOnTiming2  300//350//260   // time between TCON & backlight            // William 20050908

#define PanelOffTiming1 250//5  // time between backlight off & PWM off
#define PanelOffTiming2 0//20//10//25//40  // time between PWM off & DCDC off      // WIlliam 20050819      //Amy2008.1.3   17:30:03




#define PanelHSyncWidth     26 //44
#define PanelHSyncBackPorch 28

#define PanelVSyncWidth     3 // 2
#define PanelVSyncBackPorch 10 //38

#define PanelHStart     (PanelHSyncWidth+PanelHSyncBackPorch)
#define PanelVStart     (PanelVSyncWidth+PanelVSyncBackPorch)
#define PanelWidth      1680 //1440
#define PanelHeight     1050 //900
#define PanelHTotal     2240 //1800 //2240 //1900 // 20060310, Danny // 20060424, Danny
#define PanelVTotal     1089 //1150 //1080 //950 // 20060310, Danny // 20060424, Danny

#define PanelMaxHTotal      2800 //2000//1800
#define PanelMinHTotal      1800 //1700//1700 //1700 // 20060310, Danny
#define PanelMaxVTotal      1900
#define PanelMinVTotal      1080 //800
#define PanelDCLK           118//146 //108 // william, 20070713
#define PanelMaxDCLK        165 //160
#define PanelMinDCLK        109 //100


//=================================================================
// TCON  setting for RSDS
#define TCON_Format1    0x1 // OFC1: control polarity & inversion

#define ESP_AfterData   0 // Even Start Pulse after Data
#define ESP_Offset      1 // Even start pulse position
#define OSP_AfterData   0 // Odd Start Pulse after Data
#define OSP_Offset      1 // Odd start pulse position

// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA
#define PanelOESPCurrent    2 // OSP/ESP drive current
#define PanelOEGCurrent     1 // OPOL/EPOL/GPOL drive current
#define PanelEINVCurrent    0 // EINV drive current
#define PanelOINVCurrent    0 // OINV drive current

//define swport

#define RSBMLSW         0x00
#define RSAMLSW         0x00
#define OuputFormat     (G0AT_B|DATI_B)//Enable Auto Toggle GPO0 & data invert


// William 20051107, follow 19A2 TSU56AK + TCON setting
// GV_Off

#define GPO5_VStart     0x00
#define GPO5_VEnd       0x00
#define GPO5_HStart     0x2c0 //0x198 // 071113
#define GPO5_HEnd       0x390//0x2B6 // 071113
#define GPO5_Control    0x00


// STB
#define GPO1_VStart     0x00
#define GPO1_VEnd       0x00
#define GPO1_HStart     0x378 //0x348 //0x2D8
#define GPO1_HEnd       0x390 //0x360 //0x2F5
#define GPO1_Control    0x00

// STV
#define GPO2_VStart     0x000
#define GPO2_VEnd       0x001
#define GPO2_HStart     0x180           // William 20051107
#define GPO2_HEnd       0x180
#define GPO2_Control    0x00

// CLKV
#define GPO3_VStart     0
#define GPO3_VEnd       0
#define GPO3_HStart     0x180           // William 20051107
#define GPO3_HEnd       0x2D0
#define GPO3_Control    0x01

// OE
#define GPO4_VStart     0x0
#define GPO4_VEnd       0x0
#define GPO4_HStart     0x280
#define GPO4_HEnd       0x310
#define GPO4_Control        0x00

// POL
#define GPO0_VStart     0x0         // William 20050922, POL at GPO6 for PCB A190A2-A02-H-S1-03
#define GPO0_VEnd       0x419//0x385    // 901
#define GPO0_HStart     0x350   // 3600
#define GPO0_HEnd       0x350
#define GPO0_Control    0x82//0x12//0x02

//PVS / FSYNC
#define GPO9_VStart     0//x002
#define GPO9_VEnd       0//x306
#define GPO9_HStart     0//x00A
#define GPO9_HEnd       0//x005
#define GPO9_Control        1//0x08
#endif
