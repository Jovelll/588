
// for LGE LM201W01/ 20'1" WSXGA+ (1680 x 1050)
//#if ENABLE_GAMMA
#define PanelName   "M215HW01V6"

#define PanelDither     8

#define PanelTTL        0
#define PanelTCON       0
#define PanelLVDS       0xFF
#define PanelRSDS       0
#define PanelMiniLVDS     0
#define PanelMiniLVDS1     0
#define PanelMiniLVDS2     0

#define LVDS_TIMode     0xff
#define PanelDualPort       0xFF
#define PanelSwapPort       0xFF
#define PanelSwapOddML      0
#define PanelSwapEvenML     0
#define PanelSwapOddRB      0//xFF
#define PanelSwapEvenRB     0//xFF
#define PanelSwapMLSB       0
#if 0//SingleBoard
#define PanelSwapLCPS       0xFF
#else
#define PanelSwapLCPS       0
#endif

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

#define PanelOnTiming1      30 // time between panel & data while turn on power
#define PanelOnTiming2      400 // time between data & back light while turn on power
#define PanelOffTiming1     60 // time between back light & data while turn off power
#define PanelOffTiming2     10 // time between data & panel while turn off power

#define PanelHSyncWidth     34//32
#define PanelHSyncBackPorch 46//48

#define PanelVSyncWidth     2
#define PanelVSyncBackPorch 16

#define PanelHStart     (PanelHSyncWidth+PanelHSyncBackPorch)
#define PanelVStart     (PanelVSyncWidth+PanelVSyncBackPorch)
#define PanelWidth      1920
#define PanelHeight     1080
#define PanelHTotal     2200
#define PanelVTotal     1125

#define PanelMaxHTotal      2300
#define PanelMinHTotal      2100
#define PanelMaxVTotal      1136
#define PanelMinVTotal      1115
#define PanelDCLK           149
#define PanelMaxDCLK        196
#define PanelMinDCLK            117
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
#define DefCool_RedColor                     116
#define DefCool_GreenColor                 127
#define DefCool_BlueColor                    150
#define DefNormal_RedColor                132
#define DefNormal_GreenColor            140
#define DefNormal_BlueColor               150
#define DefWarm_RedColor                  141
#define DefWarm_GreenColor               148
#define DefWarm_BlueColor                 150
#define DefsRGB_RedColor                    141
#define DefsRGB_GreenColor                148
#define DefsRGB_BlueColor                   150

