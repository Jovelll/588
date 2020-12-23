
#define PanelName                "CMOM236H1L07"
#define PANEL_TYPE                PANEL_23_WIDE

#define PanelDither                8

#define PanelTTL                    0
#define PanelTCON                0
#define PanelLVDS                0xFF
#define PanelRSDS                0

#define LVDS_TIMode                0xFF
#define PanelDualPort            0xFF
#define PanelSwapPort            0xFF
#define PanelSwapOddML            0
#define PanelSwapEvenML            0
#define PanelSwapOddRB            0
#define PanelSwapEvenRB            0
#define PanelSwapMLSB            0

#define PanelDClkDelay            8
#define PanelInvDE                0
#define PanelInvDClk                0
#define PanelInvHSync            0
#define PanelInvVSync            0

// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA
#define PanelDCLKCurrent            1        // Dclk current
#define PanelDECurrent            1        // DE signal current
#define PanelOddDataCurrent        1        // Odd data current
#define PanelEvenDataCurrent        1        // Even data current

#define PanelOnTiming1        40//60//20 // time between panel & data while turn on power
#define PanelOnTiming2        500//350//220 // time between data & back light while turn on power
#define PanelOffTiming1        380//350//220 // 2005/5/3// time between back light & data while turn off power 
#define PanelOffTiming2        20//20//25 // 2005/5/3//10 // time between data & panel while turn off power 
//    #define PanelOffOnDelay        700     // >0.5
#define PanelHSyncWidth            24
#define PanelHSyncBackPorch     28//25  //85

#define PanelVSyncWidth            3
#define PanelVSyncBackPorch        13

#define PanelHStart                (PanelHSyncWidth+PanelHSyncBackPorch)
#define PanelVStart                (PanelVSyncWidth+PanelVSyncBackPorch)

#define PanelWidth                  1920
#define PanelHeight                1080
#define PanelHTotal                2200
#define PanelVTotal                1125
#define PanelMaxHTotal            2300
#define PanelMinHTotal            2048            //2100
#define PanelMaxVTotal            1136
#define PanelMinVTotal            1115

#define PanelDCLK                  149
#define PanelMaxDCLK            196
#define PanelMinDCLK                117

//HSYNC
#define GPOA_VStart                0
#define GPOA_VEnd                0
#define GPOA_HStart                0
#define GPOA_HEnd                0
#define GPOA_Control                0

//=================================================================
//Color Temp
#define DefCool_RedColor                120
#define DefCool_GreenColor              127
#define DefCool_BlueColor               150
#define DefNormal_RedColor              160
#define DefNormal_GreenColor            160
#define DefNormal_BlueColor         160
#define DefWarm_RedColor                149
#define DefWarm_GreenColor          147
#define DefWarm_BlueColor               150
#define DefsRGB_RedColor                149
#define DefsRGB_GreenColor              147
#define DefsRGB_BlueColor               150