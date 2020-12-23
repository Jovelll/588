// for CMO "
//#define PanelName   _C,_M,_O
#define PanelName  PanelCMOM220Z1_PS3
//#define PANEL_TYPE   PANEL_22_WIDE
#define PnlAUSN02           0
#define PnlHSD170ME13_A    0
#define PnlHT17E13_100       0
#define PnlQDI15XL02         0

#define PnlCPTEA02          0
#define PnlQDI              0
#define PnlAUN05            0
#define PnlCPTXG09          0
#define PnlSANYO19          0
#define PnlQDI15L           0
#define PnlM190E2_L01      0
#define PnlCPT170EA03             1

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





#if 0//CMO

#if 0//2006-03-13 Andy

#define PanelOnTiming0  35  //time between PanelVCC and DCDC            // William 20051011
#define PanelOnTiming1  80  // time between DCDC & TCON turn on         // William 20051011, for VGH 2 steps go High
#define PanelOnTiming2  260     // time between TCON & backlight            // William 20050908
#define PanelOnTiming3  700     // time between backlight & PWM         // WIlliam 20050908
#define PanelOnTiming4  30      // time between PWM & data

#define PanelOffTiming0 5   // time between data off & backlight off
#define PanelOffTiming1 15  // time between backlight off & PWM off
#define PanelOffTiming2 40  // time between PWM off & DCDC off      // WIlliam 20050819
#define PanelOffTiming3 80  // time between DCDC off & TCON IO
#define PanelOffTiming4 55  // time between TCON IO & PanelVCC
#else
///qx20061221

#define PanelOnTiming0  25  //time between PanelVCC and DCDC            // William 20051011
#define PanelOnTiming1  37  // time between DCDC & TCON turn on         // William 20051011, for VGH 2 steps go High
#define PanelOnTiming2  20  // time between TCON & backlight            // William 20050908
#define PanelOnTiming3  50  // time between backlight & PWM         // WIlliam 20050908
#define PanelOnTiming4  50      // time between PWM & data

#define PanelOffTiming0 5   // time between data off & backlight off
#define PanelOffTiming1 5   // time between backlight off & PWM off
#define PanelOffTiming2 5   // time between PWM off & DCDC off      // WIlliam 20050819
#define PanelOffTiming3 0   // time between DCDC off & TCON IO
#define PanelOffTiming4 60  // time between TCON IO & PanelVCC

#endif


#define PanelHSyncWidth     43 //40 //32
#define PanelHSyncBackPorch 41

#define PanelVSyncWidth 2
#define PanelVSyncBackPorch 38

#define PanelHStart     (PanelHSyncWidth+PanelHSyncBackPorch)
#define PanelVStart     (PanelVSyncWidth+PanelVSyncBackPorch)
#define PanelWidth      1680
#define PanelHeight     1050
#define PanelHTotal     1880
#define PanelVTotal     1089

#define PanelMaxHTotal      2240//1800
#define PanelMinHTotal      1880//2006-03-02 Andy 1688
#define PanelMaxVTotal      1600
#define PanelMinVTotal      1058//2006-03-10 900//2006-03-02 Andy 1042
#define PanelDCLK                   108
#define PanelMaxDCLK        170
#define PanelMinDCLK               100

//=================================================================
#define TCON_Format1    0x1 // OFC1: control polarity & inversion

#define ESP_AfterData   0 // Even Start Pulse after Data
#define ESP_Offset         1 // Even start pulse position
#define OSP_AfterData   0 // Odd Start Pulse after Data
#define OSP_Offset         1 // Odd start pulse position

// TCON  setting for RSDS
// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA


#define PanelOESPCurrent        3 // OSP/ESP drive current
#define PanelOEGCurrent     2 // OPOL/EPOL/GPOL drive current
#define PanelEINVCurrent        0 // EINV drive current
#define PanelOINVCurrent        0 // OINV drive current


#define OuputFormat     G0AT_B|DATI_B|PUA_B//(DATI_B)
#define RSBMLSW             0
#define RSAMLSW             0



//===========================
//2006-03-10 Suggest By AE
// POL  //ok     //nick 060807
#define GPO0_VStart          0x000
#define GPO0_VEnd            0x385
#define GPO0_HStart          0x168
#define GPO0_HEnd            0x168
#define GPO0_Control        0x92//0x12     //2006-03-15  : one line flicker:0x02  Two Line :0x12  Solve Flicker
//#define GPO0_Control        0x02     //2006-03-15  : one line flicker:0x02  Two Line :0x12  Solve Flicker
//old:0x82

// STB   //0k
#define GPO1_VStart     0x000
#define GPO1_VEnd       0x000
#define GPO1_HStart     0x350
#define GPO1_HEnd       0x364
#define GPO1_Control    0x00

// STV
#define GPO2_VStart     0x000
#define GPO2_VEnd       0x001
#define GPO2_HStart     0x180
#define GPO2_HEnd       0x180
#define GPO2_Control    0x00

// CLKV
#define GPO3_VStart     0x000
#define GPO3_VEnd       0x000
#define GPO3_HStart     0x180
#define GPO3_HEnd       0x2D0
#define GPO3_Control    0x01

// OE
#define GPO4_VStart     0x000
#define GPO4_VEnd       0x000
#define GPO4_HStart     0x280
#define GPO4_HEnd       0x310
#define GPO4_Control        0x14//0x00

//GV_ON     // no use for AOC Cmo  panel
#define GPO6_VStart         0x000               // 2006-03-13 Andy Polarity Reverse From GV_OFF
#define GPO6_VEnd           0x000
#define GPO6_HStart         0x200
#define GPO6_HEnd           0x2EF
#define GPO6_Control        0x00

//GV_OFF   //ok 060807 nick
#define GPO5_VStart        0x000
#define GPO5_VEnd          0x000
#define GPO5_HStart        0x198    //  0x196
#define GPO5_HEnd          0x2B6    //0x25E
#define GPO5_Control       0x01

//==========================================
#else
//#define PanelOnTiming0    35  //time between PanelVCC and DCDC            // William 20051011
//#define PanelOnTiming1    60  // time between DCDC & TCON turn on         // William 20051011, for VGH 2 steps go High
//#define PanelOnTiming2    550     // time between TCON & backlight            // William 20050908
//#define PanelOnTiming3    700     // time between backlight & PWM         // WIlliam 20050908
//#define PanelOnTiming4    30      // time between PWM & data

//#define PanelOffTiming0   5   // time between data off & backlight off
//#define PanelOffTiming1   100     // time between backlight off & PWM off
//#define PanelOffTiming2   50  // time between PWM off & DCDC off      // WIlliam 20050819
//#define PanelOffTiming3   80  // time between DCDC off & TCON IO
//#define PanelOffTiming4   55  // time between TCON IO & PanelVCC



#define PanelOnTiming0  35  //time between PanelVCC and DCDC              // William 20051011 //Amy2008.1.3   11:45:13
#define PanelOnTiming1  20//25  // time between DCDC & TCON turn on         // William 20051011, for VGH 2 steps go High    //Amy2008.1.3   17:30:07
#define PanelOnTiming2  450//350//260   // time between TCON & backlight            // William 20050908
#define PanelOnTiming3  700     // time between backlight & PWM         // WIlliam 20050908
#define PanelOnTiming4  30      // time between PWM & data

#define PanelOffTiming0 5   // time between data off & backlight off
#define PanelOffTiming1 100//5  // time between backlight off & PWM off
#define PanelOffTiming2 1//10//25//40  // time between PWM off & DCDC off      // WIlliam 20050819      //Amy2008.1.3   17:30:03
#define PanelOffTiming3 80  // time between DCDC off & TCON IO
#define PanelOffTiming4 55  // time between TCON IO & PanelVCC

/*#define PanelOnTiming0  35  //time between PanelVCC and DCDC            // William 20051011   1
#define PanelOnTiming1  25  // time between DCDC & TCON turn on         // William 20051011, for VGH 2 steps go High
#define PanelOnTiming2  450//350//260   // time between TCON & backlight            // William 20050908
#define PanelOnTiming3  700     // time between backlight & PWM         // WIlliam 20050908
#define PanelOnTiming4  30      // time between PWM & data

#define PanelOffTiming0 5   // time between data off & backlight off
#define PanelOffTiming1 100//5  // time between backlight off & PWM off
#define PanelOffTiming2 10//25//40  // time between PWM off & DCDC off      // WIlliam 20050819
#define PanelOffTiming3 80  // time between DCDC off & TCON IO
#define PanelOffTiming4 55  // time between TCON IO & PanelVCC*/






#define PanelHSyncWidth     32 //44
#define PanelHSyncBackPorch 40

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


// William 20051021 mark
// GV_On
/*                                  // William 20050922 reserve GPO5 for frame toggle, PCB A190A2-A02-H-S1-03
#define GPO5_VStart     0x0
#define GPO5_VEnd       0x0
#define GPO5_HStart     0x190       // William 20050915
#define GPO5_HEnd       0x2A9
#define GPO5_Control    0x00    //0x06
*/
/*
// GV_Off
#if Tsum
#define GPO0_VStart     0x00            // William 20050922, GV_OFF at GPO0 for PCB A190A2-A02-H-S1-03
#define GPO0_VEnd       0x00
#define GPO0_HStart     0x190
#define GPO0_HEnd       0x2A9
#define GPO0_Control    0x01
#else
#define GPO7_VStart     0x00
#define GPO7_VEnd       0x00
#define GPO7_HStart     0x300
#define GPO7_HEnd       0x570
#define GPO7_Control    0x01
#endif

// STB
#define GPO1_VStart     0x00
#define GPO1_VEnd       0x00
#define GPO1_HStart     0x2D6
#define GPO1_HEnd       0x2E0   // William 20051012, for scope test
#define GPO1_Control    0x00

// STV
#define GPO2_VStart     0x000
#define GPO2_VEnd       0x001
#define GPO2_HStart     0x032
#define GPO2_HEnd       0x032
#define GPO2_Control    0x00

// CLKV
#define GPO3_VStart     0
#define GPO3_VEnd       0
#define GPO3_HStart     0x0C5
#define GPO3_HEnd       0x28B
#define GPO3_Control    0x01

// OE
#define GPO4_VStart     0x0
#define GPO4_VEnd       0x0
#define GPO4_HStart     0x26D
#define GPO4_HEnd       0x2D0       // William 20050915
#define GPO4_Control    0x00

// POL
#if Tsum
#define GPO6_VStart     0x0         // William 20050922, POL at GPO6 for PCB A190A2-A02-H-S1-03
#define GPO6_VEnd       0x385//0x385    // 901
#define GPO6_HStart     0x168   // 360
#define GPO6_HEnd       0x168
#define GPO6_Control    0x02
#else
#define GPO8_VStart     0x0
#define GPO8_VEnd       0x385
#define GPO8_HStart     0x17f//0x196//kidd0629
#define GPO8_HEnd       0x000//0x25E//kidd0629
#define GPO8_Control    0x02
#endif                                  // William 20050830 end
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// William 20051107, follow 19A2 TSU56AK + TCON setting
#if 0

// GV_Off

#define GPO0_VStart     0x00            // William 20050922, GV_OFF at GPO0 for PCB A190A2-A02-H-S1-03
#define GPO0_VEnd       0x00
#define GPO0_HStart     0x198       // William 20051107
#define GPO0_HEnd       0x2B6
#define GPO0_Control        0x00//0x01


// STB
#define GPO1_VStart     0x00
#define GPO1_VEnd       0x00
#define GPO1_HStart     0x350//0x348 //0x2D8
#define GPO1_HEnd       0x364 //0x360 //0x2F5
#define GPO1_Control        0x00

// STV
#define GPO2_VStart     0x000
#define GPO2_VEnd       0x001
#define GPO2_HStart     0x180           // William 20051107
#define GPO2_HEnd       0x180
#define GPO2_Control        0x00

// CLKV
#define GPO3_VStart     0
#define GPO3_VEnd       0
#define GPO3_HStart     0x180           // William 20051107
#define GPO3_HEnd       0x2D0
#define GPO3_Control        0x01

// OE
#define GPO4_VStart     0x0
#define GPO4_VEnd       0x0
#define GPO4_HStart     0x280
#define GPO4_HEnd       0x310
#define GPO4_Control        0x01

// POL
#if 0
#define GPO6_VStart     0x0         // William 20050922, POL at GPO6 for PCB A190A2-A02-H-S1-03
#define GPO6_VEnd       0x385//0x385    // 901
#define GPO6_HStart     0x168   // 360
#define GPO6_HEnd       0x168
#define GPO6_Control    0x02
#else
#define GPO5_VStart     0x0         // William 20050922, POL at GPO6 for PCB A190A2-A02-H-S1-03
#define GPO5_VEnd       0x419//0x385    // 901
#define GPO5_HStart     0x350   // 360
#define GPO5_HEnd       0x350
#define GPO5_Control    0x02
#endif


#else
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
#if 0
#define GPO6_VStart     0x0         // William 20050922, POL at GPO6 for PCB A190A2-A02-H-S1-03
#define GPO6_VEnd       0x385//0x385    // 901
#define GPO6_HStart     0x168   // 360
#define GPO6_HEnd       0x168
#define GPO6_Control    0x02
#else
#define GPO0_VStart     0x0         // William 20050922, POL at GPO6 for PCB A190A2-A02-H-S1-03
#define GPO0_VEnd       0x419//0x385    // 901
#define GPO0_HStart     0x350   // 3600
#define GPO0_HEnd       0x350
#define GPO0_Control    0x82//0x12//0x02
#endif

#endif                          // William 20050830 end

//=================================================================
//Color Temp
#define DefCool_RedColor            140
#define DefCool_GreenColor          148
#define DefCool_BlueColor           164
#define DefNormal_RedColor          178
#define DefNormal_GreenColor        178
#define DefNormal_BlueColor         178
#define DefWarm_RedColor            150
#define DefWarm_GreenColor      154
#define DefWarm_BlueColor           139
#define DefsRGB_RedColor      DefWarm_RedColor
#define DefsRGB_GreenColor  DefWarm_GreenColor
#define DefsRGB_BlueColor     DefWarm_BlueColor
#endif

