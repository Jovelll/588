// for AU24_M240UW01_WUXGA
#define PanelName   "eDP_1080p@60"

#define PanelDither     0

#define PanelTTL        0
#define PanelTCON       0
#define PanelLVDS       1 //0xFF
#define PanelRSDS       0

#define PANEL_SWAP_AB_PORT      0 //0: non-swap, 0xFF: AB swap
#define PANEL_SWAP_CD_PORT      0 //0: non-swap ,0xFF: CD swap

#define LVDS_CH_B_SWAP         LVDS_CH_C // LVDS_CH_A

#define PAIR_SWAP_B             0

#define ShortLineCheck          0xFF
#define LVDS_TIMode             0xFF  //Thine mode:disable TI mode

#define LVDS_TI_BitModeSel      LVDS_TI_8BIT

#define PanelQuadPort           0
#define PanelDualPort           0 //FUNC_ENABLE
#define PanelSwapPort       0
#define PanelSwapOddML      0
#define PanelSwapEvenML     0
#define PanelSwapOddRB      0
#define PanelSwapEvenRB     0
#define PanelSwapMLSB       0
#define PanelSwapPN         0

#define PanelDClkDelay      0
#define PanelInvDE      0
#define PanelInvDClk        0
#define PanelInvHSync       0
#define PanelInvVSync       0

// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA
#define PanelDCLKCurrent    1 // Dclk current
#define PanelDECurrent      1 // DE signal current
#define PanelOddDataCurrent 1 // Odd data current
#define PanelEvenDataCurrent    1 // Even data current

#define PanelOnTiming1      40 // time between panel & data while turn on power
#define PanelOnTiming2      300 // time between data & back light while turn on power
#define PanelOffTiming1     300 // time between back light & data while turn off power
#define PanelOffTiming2     30 // time between data & panel while turn off power

#define PanelHSyncWidth     10///48//36 //32
#define PanelHSyncBackPorch 8///80//40

#define PanelVSyncWidth     5
#define PanelVSyncBackPorch 23

#define PanelHStart     (PanelHSyncWidth+PanelHSyncBackPorch)
#define PanelVStart     (PanelVSyncWidth+PanelVSyncBackPorch)
#define PanelWidth      1920
#define PanelHeight     1080
#define PanelHTotal     2108
#define PanelVTotal     1118

#define PanelMaxHTotal      2320
#define PanelMinHTotal      2100
#define PanelMaxVTotal      1380
#define PanelMinVTotal      1084
#define PanelVsyncFreq      60

#define Panel_VTT_3D_120    PanelVTotal
#define Panel_VTT_3D_110    PanelVTotal
#define Panel_VTT_3D_100    PanelVTotal
#define Panel_VTT_3D        PanelVTotal

#define PanelDCLK           (((DWORD)PanelHTotal*PanelVTotal*PanelVsyncFreq)/1000000)
#define PanelMaxDCLK        170
#define PanelMinDCLK        120

// eDP Timing define 
#define DP_800X600_60HZ     0
#define DP_1024X768_60HZ   1
#define DP_1280X800_60HZ   2
#define DP_1360X768_60HZ   3
#define DP_480P                     4
#define DP_720P                     5
#define DP_1080P                   6
#define DP_1080P_120HZ       7
#define DP_2560X1440_60Hz    8
#define DP_1366X768_60HZ	9
#define DP_PANEL_TYPE		10


// eDP Link Rate define 
#define DPTXRBR        0
#define DPTXHBR        1
#define DPTXHBRx11  2
#define DPTXHBRx12  3
// eDP Lane Count define 
#define DPTX1L          0
#define DPTX2L          1
#define DPTX4L          2

//set eDP TX Timing
#define eDPTXTiming        DP_PANEL_TYPE
//set eDP TX Min Link Rate
#define eDPTXMinRate      DPTXHBR
// set eDP TX Min Lane Count
#define eDPTXMinLane      DPTX2L

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
