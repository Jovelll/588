#define PanelName                   "M190A1L02"

#define _PanelColorDepth                bPanelColorDepth//0->6bit+FRC                       bPanelColorDepth->8Bit
#define PanelTTL                        0
#define PanelTCON                   0
#define PanelLVDS                   0xff
#define PanelRSDS                   0

#define _ChangeModePanelVddOff      bChangeModePanelVddOff//0->Change Mode Panel Vdd On     bChangeModePanelVddOff->Change Mode Panel Vdd Off

#define PanelDualPort               0xff
#define PanelSwapPort               0xff

#define _PanelOutputControl1            0x10//Scaler Bank 0x42 Setting
#define _PanelOSContol              0x83//Scaler Bank 0xF2 Setting
#define _PanelODRriving             0x55//Scaler Bank 0xF5 Setting


#define _PanelOnTiming1             20// time between panel & data while turn on power
#define _PanelOnTiming2             500 // time between data & back light while turn on power
#define _PanelOffTiming1                150// time between back light & data while turn off power
#define _PanelOffTiming2                20// time between data & panel while turn off power

#define _PanelHSyncWidth            45
#define _PanelHSyncBackPorch        40
#define _PanelVSyncWidth            2
#define _PanelVSyncBackPorch        38
#define _PanelHStart                    (_PanelHSyncWidth+_PanelHSyncBackPorch)
#define _PanelVStart                    (_PanelVSyncWidth+_PanelVSyncBackPorch)


#define _PanelWidth                 1440
#define _PanelHeight                    900
#define _PanelHTotal                    1880
#define _PanelVTotal                    920
#define _PanelMaxHTotal             1800
#define _PanelMinHTotal             1540
#define _PanelMaxVTotal             2000
#define _PanelMinVTotal             880

#define _PanelDCLK                  89
#define _PanelMaxDCLK               112
#define _PanelMinDCLK               80


#define _DefBurstModeFreq           250//Hz
#define _DefMinDutyValue                0x2B
#define _DefMaxDutyValue            0xFF


#define _STEPL                      0x33
#define _STEPH                      0x00
#define _SPANL                      0xCC
#define _SPANH                      0x01

//=================================================================
//Color Temp
#define DefCool_RedColor            132
#define DefCool_GreenColor          140
#define DefCool_BlueColor           150
#define DefNormal_RedColor          144
#define DefNormal_GreenColor        150
#define DefNormal_BlueColor         141
#define DefWarm_RedColor            150
#define DefWarm_GreenColor      150
#define DefWarm_BlueColor           129
#define DefsRGB_RedColor      DefWarm_RedColor
#define DefsRGB_GreenColor  DefWarm_GreenColor
#define DefsRGB_BlueColor     DefWarm_BlueColor