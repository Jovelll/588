#define PanelName                   "M216H1L01"
#define _PanelColorDepth                bPanelColorDepth//0->6bit+FRC                       bPanelColorDepth->8Bit
#define PanelTTL                        0
#define PanelTCON                   0
#define PanelLVDS                   0xff
#define PanelRSDS                   0

#define _ChangeModePanelVddOff      bChangeModePanelVddOff//0->Change Mode Panel Vdd On     bChangeModePanelVddOff->Change Mode Panel Vdd Off

#define PanelDualPort               0xff
#define PanelSwapPort               0xff

#define _PanelOutputControl1            0x10//Scaler Bank 0x42 Setting
#define _PanelOSContol              0x80//Scaler Bank 0xF2 Setting
#define _PanelODRriving             0x55//Scaler Bank 0xF5 Setting

#define _PanelOnTiming1             20// time between panel & data while turn on power
#define _PanelOnTiming2             450 // time between data & back light while turn on power
#define _PanelOffTiming1                100// time between back light & data while turn off power
#define _PanelOffTiming2                20// time between data & panel while turn off power

#define _PanelHSyncWidth            46
#define _PanelHSyncBackPorch        19
#define _PanelVSyncWidth            3
#define _PanelVSyncBackPorch        13
#define _PanelHStart                    (_PanelHSyncWidth+_PanelHSyncBackPorch)
#define _PanelVStart                    (_PanelVSyncWidth+_PanelVSyncBackPorch)


#define _PanelWidth                 1920
#define _PanelHeight                    1080
#define _PanelHTotal                    2100
#define _PanelVTotal                    1130
#define _PanelMaxHTotal             2176
#define _PanelMinHTotal             2036
#define _PanelMaxVTotal             2047
#define _PanelMinVTotal             1088

#define _PanelDCLK                  140
#define _PanelMaxDCLK               170
#define _PanelMinDCLK               116


#define _DefBurstModeFreq           250//Hz
#define _DefMinDutyValue                0x18
#define _DefMaxDutyValue            0xFF


#define _STEPL                      0x44
#define _STEPH                      0x00
#define _SPANL                      0xCC
#define _SPANH                      0x01

//=================================================================
//Color Temp
#define DefCool_RedColor            136
#define DefCool_GreenColor          138
#define DefCool_BlueColor           150
#define DefNormal_RedColor          148
#define DefNormal_GreenColor        150
#define DefNormal_BlueColor         142
#define DefWarm_RedColor            150
#define DefWarm_GreenColor      150
#define DefWarm_BlueColor           127
#define DefsRGB_RedColor      DefWarm_RedColor
#define DefsRGB_GreenColor  DefWarm_GreenColor
#define DefsRGB_BlueColor     DefWarm_BlueColor