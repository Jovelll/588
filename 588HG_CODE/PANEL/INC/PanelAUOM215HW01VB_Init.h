#ifndef __PANEL_AUOM215HW01_VB__
#define __PANEL_AUOM215HW01_VB__
#define PanelNumber             1800
#define PanelName               "AUOM215HW01VB"

#define _PanelColorDepth                1//0->6bit+FRC                      bPanelColorDepth->8Bit
#define PanelTTL    0
#define PanelTCON   0
#define PanelLVDS   0xff
#define PanelRSDS   0
#define PanelminiLVDS      0

#define _ChangeModePanelVddOff      bChangeModePanelVddOff//0->Change Mode Panel Vdd On     bChangeModePanelVddOff->Change Mode Panel Vdd Off

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

#define PANEL_SWAP_PN       0xFF

#define _PanelOutputControl1            0x10//Scaler Bank 0x42 Setting
#define _PanelOutputControl2			( (PanelDualPort &BIT0) | (PanelSwapPort &BIT1))//Scaler Bank 0x43 Setting // Rosen 3043h Setting 2011.9.16 cc add
#define _PanelOSContol              0x80//Scaler Bank 0xF2 Setting
#define _PanelODRriving             0x55//Scaler Bank 0xF5 Setting

// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA
#define PanelDCLKCurrent  1 // Dclk current
#define PanelDECurrent    1 // DE signal current
#define PanelOddDataCurrent 1 // Odd data current
#define PanelEvenDataCurrent  1 // Even data current
#define PanelOnTiming0    2 // time between 3.3V - 12V
#define _PanelOnTiming1    25  // time between panel & data while turn on power
#define _PanelOnTiming2    550 // time between data & back light while turn on power  //20050701
#define _PanelOffTiming1    250 // time between back light & data while turn off power
#define _PanelOffTiming2    25 // time between data & panel while turn off power
#define PanelOffTiming0 0 // time between 12V - 3.3V
#define _PanelHSyncWidth   38//22//24
#define _PanelHSyncBackPorch 41
#define _PanelVSyncWidth         3
#define _PanelVSyncBackPorch 21 // 46
#define _PanelHStart   (PanelHSyncWidth+PanelHSyncBackPorch)
#define _PanelVStart   (PanelVSyncWidth+PanelVSyncBackPorch)
#define _PanelWidth    1920
#define _PanelHeight   1080
#define _PanelHTotal   2120
#define _PanelVTotal   1120
#define _PanelMaxHTotal     4094
#define _PanelMinHTotal     2068
#define _PanelMaxVTotal     2047
#define _PanelMinVTotal    1088
#define _PanelDCLK            150
#define _PanelMaxDCLK          184
#define _PanelMinDCLK          80
#define HV_OUTPUT_TYPE          HSRM_B

//=====================================================================// 111003 Rick add
#define PanelVfreq          (600)
#define PanelMaxVfreq   (750+20) // 75Hz
#define PanelMinVfreq   (500-20) // 50Hz

#define _DefBurstModeFreq           250//Hz
#define _DefMinDutyValue                0x01
#define _DefMaxDutyValue            0xFF

#define _STEPL                      0x44
#define _STEPH                      0x00
#define _SPANL                      0xCC
#define _SPANH                      0x01

#define PanelDither                 8

#define _PanelLVDSSwing2			1// 0:242.37mV    1:339.33mV    2:387mV    3:169.67mV    //For TSUMXXNXX&TSUMXXQXX type ic auto tune target voltage
#define _PanelLVDSSwing1			0x15// 283mV //For old chip
#define _PanelLVDSSwing				(_PanelLVDSSwing1|_PanelLVDSSwing2<<6)

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
#endif

