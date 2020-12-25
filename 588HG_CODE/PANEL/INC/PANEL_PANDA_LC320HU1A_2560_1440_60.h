#ifndef __PANEL_PANDA_LC320HU1A_2560_1440_60__
#define __PANEL_PANDA_LC320HU1A_2560_1440_60__
#define PanelName   "PANEL_PANDA_LC320HU1A_2560_1440_60"


#define PanelDither     8

#define PanelTTL        0
#define PanelTCON       0
#define PanelLVDS       0xFF
#define PanelRSDS       0

#define LVDS_TIMode         0xFF
#define LVDS_TI_BitModeSel      LVDS_TI_10BIT

#define PANEL_QUAD_PORT		1
#define PanelDualPort       0//0xFF
#define PanelSwapPort       0//0xFF
#define PanelSwapOddML      0
#define PanelSwapEvenML     0
#define PanelSwapOddRB      0
#define PanelSwapEvenRB     0
#define PanelSwapMLSB       0
#define PANEL_SWAP_PN         0

#define LVDS_CH_A_SWAP      LVDS_CH_A
#define LVDS_CH_B_SWAP      LVDS_CH_B
#define LVDS_CH_C_SWAP      LVDS_CH_C
#define LVDS_CH_D_SWAP      LVDS_CH_D
#define PANEL_SWAP_AB_PORT  0 //0: non-swap, 0xFF: AB swap
#define PANEL_SWAP_CD_PORT  0//0: non-swap ,0xFF: CD swap

//    panel output type select
#define PANEL_OUTPUT_FMT_3D_LR      1
#define PANEL_OUTPUT_FMT_3D_CB      0


#define PanelDClkDelay      0
#define PanelInvDE                  0
#define PanelInvDClk        0
#define PanelInvHSync       0
#define PanelInvVSync       0

// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA
#define PanelDCLKCurrent    1 // Dclk current
#define PanelDECurrent      1 // DE signal current
#define PanelOddDataCurrent 1 // Odd data current
#define PanelEvenDataCurrent    1 // Even data current

#define PanelOnTiming1    30 // 0//40 // time between panel & data while turn on power
#define PanelOnTiming2    250 // 300//300 //30 // time between data & back light while turn on power

#define PanelOffTiming1  250 // time between back light & data while turn off power
#define PanelOffTiming2  16 // 20 // time between data & panel while turn off power

#define PanelHSyncWidth         10
#define PanelHSyncBackPorch    48

#define PanelVSyncWidth         5
#define PanelVSyncBackPorch    26

#define PanelHStart         (PanelHSyncWidth+PanelHSyncBackPorch)
#define PanelVStart         (PanelVSyncWidth+PanelVSyncBackPorch)

#define PanelWidth              2560
#define PanelHeight             1440
#define PanelHTotal             2720
#define PanelVTotal             1481
#define PanelVsyncFreq      60

#define PanelMaxHTotal        2728
#define PanelMinHTotal         2712
#define PanelMaxVTotal         1683
#define PanelMinVTotal         1479
#define PanelDCLK               (((DWORD)PanelHTotal*PanelVTotal*PanelVsyncFreq)/1000000)
#define PanelMaxDCLK           304
#define PanelMinDCLK           180

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
#define DefCool_RedColor                    97
#define DefCool_GreenColor                105
#define DefCool_BlueColor                   128
#define DefNormal_RedColor               114
#define DefNormal_GreenColor           116
#define DefNormal_BlueColor              128
#define DefWarm_RedColor                 122
#define DefWarm_GreenColor              123
#define DefWarm_BlueColor                128
#define DefsRGB_RedColor                   122
#define DefsRGB_GreenColor               123
#define DefsRGB_BlueColor                  128


#endif
