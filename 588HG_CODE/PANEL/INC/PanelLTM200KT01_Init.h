#define PanelName                   "LTM200KT01"
#if INPUT_TYPE!=INPUT_1A
#define _PanelColorDepth                0//bPanelColorDepth//0->6bit+FRC                        bPanelColorDepth->8Bit
#else
#define _PanelColorDepth                bPanelColorDepth//0->6bit+FRC                       bPanelColorDepth->8Bit
#endif
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

#define _PanelOnTiming1             20// time between panel & data while turn on power
#define _PanelOnTiming2             450 // time between data & back light while turn on power
#define _PanelOffTiming1                100// time between back light & data while turn off power
#define _PanelOffTiming2                20// time between data & panel while turn off power

#define _PanelHSyncWidth            48
#define _PanelHSyncBackPorch        78
#define _PanelVSyncWidth            3
#define _PanelVSyncBackPorch        12
#define _PanelHStart                    (_PanelHSyncWidth+_PanelHSyncBackPorch)
#define _PanelVStart                    (_PanelVSyncWidth+_PanelVSyncBackPorch)


#define _PanelWidth                 1600
#define _PanelHeight                    900
#define _PanelHTotal                    2112
#define _PanelVTotal                    934
#define _PanelMaxHTotal             2260
#define _PanelMinHTotal             1976
#define _PanelMaxVTotal             956
#define _PanelMinVTotal             920

#define _PanelDCLK                  104
#define _PanelMaxDCLK               180
#define _PanelMinDCLK               88


#define _DefBurstModeFreq           250//Hz
#define _DefMinDutyValue                0x1F
#define _DefMaxDutyValue            0xFF


#define _STEPL                      0x44
#define _STEPH                      0x00
#define _SPANL                      0xCC
#define _SPANH                      0x01

//=================================================================
//Color Temp
#if ModelName==OTS_US200DALW
#define DefCool_RedColor            111
#define DefCool_GreenColor          118
#define DefCool_BlueColor           125
#define DefNormal_RedColor          120
#define DefNormal_GreenColor        123
#define DefNormal_BlueColor         121
#define DefWarm_RedColor            125
#define DefWarm_GreenColor      124
#define DefWarm_BlueColor           117
#define DefsRGB_RedColor      DefWarm_RedColor
#define DefsRGB_GreenColor  DefWarm_GreenColor
#define DefsRGB_BlueColor     DefWarm_BlueColor
#elif ModelName==OTS_UU200AGLW
#define DefCool_RedColor            105//111
#define DefCool_GreenColor          118
#define DefCool_BlueColor           128//125
#define DefNormal_RedColor          118//120
#define DefNormal_GreenColor        125//123
#define DefNormal_BlueColor         128//121
#define DefWarm_RedColor            128//125
#define DefWarm_GreenColor      128//124
#define DefWarm_BlueColor           128//117
#define DefsRGB_RedColor      DefWarm_RedColor
#define DefsRGB_GreenColor  DefWarm_GreenColor
#define DefsRGB_BlueColor     DefWarm_BlueColor
#else
#define DefCool_RedColor            129
#define DefCool_GreenColor          130
#define DefCool_BlueColor           155
#define DefNormal_RedColor          143
#define DefNormal_GreenColor        142
#define DefNormal_BlueColor         155
#define DefWarm_RedColor            155
#define DefWarm_GreenColor      155
#define DefWarm_BlueColor           155
#define DefsRGB_RedColor      DefWarm_RedColor
#define DefsRGB_GreenColor  DefWarm_GreenColor
#define DefsRGB_BlueColor     DefWarm_BlueColor
#endif