#define PanelName                   "M270HW01"
#define _PanelColorDepth                bPanelColorDepth//0->6bit+FRC                       bPanelColorDepth->8Bit
#define PanelTTL                        0
#define PanelTCON                   0
#define PanelLVDS                   0xff
#define PanelRSDS                   0


#define _ChangeModePanelVddOff      bChangeModePanelVddOff//0->Change Mode Panel Vdd On     bChangeModePanelVddOff->Change Mode Panel Vdd Off

#define PanelDualPort               0xff
#define PanelSwapPort               0xff

#define _PanelOutputControl1            0x10//Scaler Bank 0x42 Setting
#define _PanelOSContol              0x00//Scaler Bank 0xF2 Setting
#define _PanelODRriving             0x55//Scaler Bank 0xF5 Setting


#define _PanelOnTiming1             30// time between panel & data while turn on power
#define _PanelOnTiming2             250 // time between data & back light while turn on power
#define _PanelOffTiming1                250// time between back light & data while turn off power
#define _PanelOffTiming2                16// time between data & panel while turn off power

#define _PanelHSyncWidth            35
#define _PanelHSyncBackPorch        29
#define _PanelVSyncWidth            3
#define _PanelVSyncBackPorch        10
#define _PanelHStart                    (_PanelHSyncWidth+_PanelHSyncBackPorch)
#define _PanelVStart                    (_PanelVSyncWidth+_PanelVSyncBackPorch)

#define _PanelWidth                 1920
#define _PanelHeight                    1080

#define _PanelHTotal                    2120
#define _PanelVTotal                    1120

#define _PanelMaxHTotal             4094
#define _PanelMinHTotal             2068

#define _PanelMaxVTotal             2047
#define _PanelMinVTotal             1088

#define _PanelDCLK                  150
#define _PanelMaxDCLK               180
#define _PanelMinDCLK               80


#define _DefBurstModeFreq           250//Hz
#define _DefMinDutyValue                0x30
#define _DefMaxDutyValue            0xFF


#define _STEPL                      0x44
#define _STEPH                      0x00
#define _SPANL                      0xCC
#define _SPANH                      0x01

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

