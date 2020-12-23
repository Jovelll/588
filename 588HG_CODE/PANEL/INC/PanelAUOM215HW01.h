#ifndef __PANEL_AUOM215HW01_V7__
#define __PANEL_AUOM215HW01_V7__
#define PanelNumber             1800
#define PanelName               "AUOM215HW01"

#define PanelDither   8
#define PanelTTL    0
#define PanelTCON   0
#define PanelLVDS   0xff
#define PanelRSDS   0
#define PanelminiLVDS      0

#define ShortLineCheck    0
#define LVDS_TIMode   0xff
#define PanelDualPort   0xff
#define PanelSwapPort   0xff
#define PanelSwapOddML    0
#define PanelSwapEvenML   0
#define PanelSwapOddRB    0
#define PanelSwapEvenRB   0
#define PanelSwapMLSB   0
#define PanelDClkDelay    8
#define PanelInvDE      0
#define PanelInvDClk    0
#define PanelInvHSync   0
#define PanelInvVSync   0


// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA
#define PanelDCLKCurrent  1 // Dclk current
#define PanelDECurrent    1 // DE signal current
#define PanelOddDataCurrent 1 // Odd data current
#define PanelEvenDataCurrent  1 // Even data current
#define PanelOnTiming0    2 // time between 3.3V - 12V
#define PanelOnTiming1    25 // 0//40 // time between panel & data while turn on power
#define PanelOnTiming2    550//300//300 //30 // time between data & back light while turn on power  //20050701
#define PanelOffTiming1    400//300 // time between back light & data while turn off power
#define PanelOffTiming2    16 // time between data & panel while turn off power
#define PanelOffTiming0 0 // time between 12V - 3.3V
#define PanelHSyncWidth   38//22//24
#define PanelHSyncBackPorch 41
#define PanelVSyncWidth         3
#define PanelVSyncBackPorch 21 // 46
#define PanelHStart   (PanelHSyncWidth+PanelHSyncBackPorch)
#define PanelVStart   (PanelVSyncWidth+PanelVSyncBackPorch)
#define PanelWidth    1920
#define PanelHeight   1080
#define PanelHTotal   2120
#define PanelVTotal   1120
#define PanelMaxHTotal     4094
#define PanelMinHTotal     2020//2080
#define PanelMaxVTotal     2047
#define PanelMinVTotal    1088
#define PanelDCLK            150
#define PanelMaxDCLK          180
#define PanelMinDCLK          80
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
#define DefCool_RedColor                    100
#define DefCool_GreenColor                  113
#define DefCool_BlueColor                   120
#define DefNormal_RedColor                  110
#define DefNormal_GreenColor                115
#define DefNormal_BlueColor                 115
#define DefWarm_RedColor                    115
#define DefWarm_GreenColor              115
#define DefWarm_BlueColor                   111
#define DefsRGB_RedColor                    115
#define DefsRGB_GreenColor                  115
#define DefsRGB_BlueColor                   111
#endif

