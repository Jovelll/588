#define PanelName                   "LM215WF1"
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
#define _PanelOnTiming2             520 // time between data & back light while turn on power
#define _PanelOffTiming1                220// time between back light & data while turn off power
#define _PanelOffTiming2                20// time between data & panel while turn off power

#define _PanelHSyncWidth            45
#define _PanelHSyncBackPorch        20
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
#define _DefMinDutyValue                0x46
#define _DefMaxDutyValue            0xFF


#define _STEPL                      0x44
#define _STEPH                      0x00
#define _SPANL                      0xCC
#define _SPANH                      0x01
//=================================================================
//Color Temp
#define DefCool_RedColor            117//114//111
#define DefCool_GreenColor          110//114
#define DefCool_BlueColor           127//130//117
#define DefNormal_RedColor          126//127
#define DefNormal_GreenColor        121//125
#define DefNormal_BlueColor         128//127
#define DefWarm_RedColor            130
#define DefWarm_GreenColor      126//132
#define DefWarm_BlueColor           128//130
#define DefsRGB_RedColor      DefWarm_RedColor
#define DefsRGB_GreenColor  DefWarm_GreenColor
#define DefsRGB_BlueColor     DefWarm_BlueColor