#define PanelName                   "LTM230HT3"

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
#define _PanelOnTiming2             550 // time between data & back light while turn on power
#define _PanelOffTiming1                150// time between back light & data while turn off power
#define _PanelOffTiming2                20// time between data & panel while turn off power

#define _PanelHSyncWidth            20
#define _PanelHSyncBackPorch        28
#define _PanelVSyncWidth            3
#define _PanelVSyncBackPorch        12
#define _PanelHStart                    (_PanelHSyncWidth+_PanelHSyncBackPorch)
#define _PanelVStart                    (_PanelVSyncWidth+_PanelVSyncBackPorch)


#define _PanelWidth                 1920
#define _PanelHeight                    1080
#define _PanelHTotal                    2020
#define _PanelVTotal                    1111
#define _PanelMaxHTotal             2080
#define _PanelMinHTotal             2000
#define _PanelMaxVTotal             1118
#define _PanelMinVTotal             1105

#define _PanelDCLK                  135
#define _PanelMaxDCLK               175
#define _PanelMinDCLK               108


#define _DefBurstModeFreq           250//Hz
#define _DefMinDutyValue                0x2B
#define _DefMaxDutyValue            0xFF


#define _STEPL                      0x33
#define _STEPH                      0x00
#define _SPANL                      0xCC
#define _SPANH                      0x01

//=================================================================
//Color Temp
#define DefCool_RedColor                    105
#define DefCool_GreenColor                113
#define DefCool_BlueColor                   130
#define DefNormal_RedColor               116
#define DefNormal_GreenColor           122
#define DefNormal_BlueColor              130
#define DefWarm_RedColor                 130
#define DefWarm_GreenColor              130
#define DefWarm_BlueColor                130
#define DefsRGB_RedColor                   130
#define DefsRGB_GreenColor               130
#define DefsRGB_BlueColor                  130