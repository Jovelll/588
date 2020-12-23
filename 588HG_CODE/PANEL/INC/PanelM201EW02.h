#define PanelName   "M201EW02"

#define PanelDither     6

#define PanelTTL        0
#define PanelTCON       0
#define PanelLVDS       0xFF
#define PanelRSDS       0

#define LVDS_TIMode         0xff
#define PanelDualPort           0xFF
#define PanelSwapPort            0xFF
#define PanelSwapOddML           0
#define PanelSwapEvenML     0
#define PanelSwapOddRB      0
#define PanelSwapEvenRB     0
#define PanelSwapMLSB       0
#define PanelSwapPlusMinus      0//0xff



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

#define PanelOnTiming1      20//2006-06-01 Andy 30 // time between panel & data while turn on power
#define PanelOnTiming2      350//2006-06-01 Andy 250//2006-03-01 Andy 650 // time between data & back light while turn on power
#define PanelOffTiming1     350 //2006-06-01 Andy 250  time between back light & data while turn off power
#define PanelOffTiming2     20 // time between data & panel while turn off power

#define PanelHSyncWidth      44//42//32
#define PanelHSyncBackPorch   32//10//40

#define PanelVSyncWidth     6
#define PanelVSyncBackPorch 51

#define PanelHStart     (PanelHSyncWidth+PanelHSyncBackPorch)
#define PanelVStart     (PanelVSyncWidth+PanelVSyncBackPorch)
#define PanelWidth      1680
#define PanelHeight     1050
#define PanelHTotal     1840
#define PanelVTotal     1220


#define PanelMaxHTotal      2000
#define PanelMinHTotal      1900//Benz 2007.4.13   16:53:09 1800 //2006-06-27 Andy For OutPut DLK TOO HIGH 1840
#define PanelMaxVTotal      2000
#define PanelMinVTotal      1066
#define PanelDCLK                   120
#define PanelMaxDCLK        170//170        
#define PanelMinDCLK        62
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
#define DefCool_RedColor            107
#define DefCool_GreenColor          110
#define DefCool_BlueColor           145
#define DefNormal_RedColor          125
#define DefNormal_GreenColor        131
#define DefNormal_BlueColor         145
#define DefWarm_RedColor            137
#define DefWarm_GreenColor      145
#define DefWarm_BlueColor           145
#define DefsRGB_RedColor      DefWarm_RedColor
#define DefsRGB_GreenColor  DefWarm_GreenColor
#define DefsRGB_BlueColor     DefWarm_BlueColor