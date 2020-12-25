#define PanelName                   "CLAA185W"

#define _PanelColorDepth                0//0->6bit+FRC                      bPanelColorDepth->8Bit
#define PanelTTL                        0
#define PanelTCON                   0
#define PanelLVDS                   0xff
#define PanelRSDS                   0

#define _ChangeModePanelVddOff      bChangeModePanelVddOff//0->Change Mode Panel Vdd On     bChangeModePanelVddOff->Change Mode Panel Vdd Off

#define PanelDualPort               0
#define PanelSwapPort               0

#define _PanelOutputControl1            0x10//Scaler Bank 0x42 Setting
#define _PanelOSContol              0x83//Scaler Bank 0xF2 Setting
#define _PanelODRriving             0x55//Scaler Bank 0xF5 Setting


#define _PanelOnTiming1             5// time between panel & data while turn on power
#define _PanelOnTiming2             250 // time between data & back light while turn on power
#define _PanelOffTiming1                250// time between back light & data while turn off power
#define _PanelOffTiming2                25// time between data & panel while turn off power

#define _PanelHSyncWidth            35
#define _PanelHSyncBackPorch        21
#define _PanelVSyncWidth            3
#define _PanelVSyncBackPorch        29
#define _PanelHStart                    (_PanelHSyncWidth+_PanelHSyncBackPorch)
#define _PanelVStart                    (_PanelVSyncWidth+_PanelVSyncBackPorch)


#define _PanelWidth                 1366
#define _PanelHeight                    768
#define _PanelHTotal                    1560
#define _PanelVTotal                    806
#define _PanelMaxHTotal             1936
#define _PanelMinHTotal             1446
#define _PanelMaxVTotal             888
#define _PanelMinVTotal             778

#define _PanelDCLK                  75
#define _PanelMaxDCLK               90
#define _PanelMinDCLK               50


#define _DefBurstModeFreq           250//Hz
#define _DefMinDutyValue                0x2B
#define _DefMaxDutyValue            0xFF


#define _STEPL                      0x33
#define _STEPH                      0x00
#define _SPANL                      0xCC
#define _SPANH                      0x01

//=================================================================
//Color Temp
#define DefCool_RedColor            109
#define DefCool_GreenColor          111
#define DefCool_BlueColor           128
#define DefNormal_RedColor          122
#define DefNormal_GreenColor        124
#define DefNormal_BlueColor         128
#define DefWarm_RedColor            128
#define DefWarm_GreenColor      128
#define DefWarm_BlueColor           128
#define DefsRGB_RedColor      DefWarm_RedColor
#define DefsRGB_GreenColor  DefWarm_GreenColor
#define DefsRGB_BlueColor     DefWarm_BlueColor