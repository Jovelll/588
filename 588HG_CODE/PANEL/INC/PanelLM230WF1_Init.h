#define PanelName               "LM230W"

#define PanelTTL                    0
#define PanelTCON                   0
#define PanelLVDS               0xff
#define PanelRSDS               0

#if INPUT_TYPE!=INPUT_1A
#define _PanelColorDepth            0//bPanelColorDepth
#else
#define _PanelColorDepth            bPanelColorDepth
#endif
#define _ChangeModePanelVddOff  bChangeModePanelVddOff


#define PanelDualPort               0xff
#define PanelSwapPort               0xff

#define _PanelOutputControl1        0x10
#define _PanelOSContol          0x80
#define _PanelODRriving         0x55


#define _PanelOnTiming1         20
#define _PanelOnTiming2         550
#define _PanelOffTiming1            250
#define _PanelOffTiming2            25

#define _PanelHSyncWidth        35
#define _PanelHSyncBackPorch    29//61
#define _PanelVSyncWidth        3
#define _PanelVSyncBackPorch    13
#define _PanelHStart            (_PanelHSyncWidth+_PanelHSyncBackPorch)
#define _PanelVStart            (_PanelVSyncWidth+_PanelVSyncBackPorch)


#define _PanelWidth             1920
#define _PanelHeight                1080
#define _PanelHTotal                2100
#define _PanelVTotal                1130
#define _PanelMaxHTotal         2176
#define _PanelMinHTotal         2030//2050//2030//2100
#define _PanelMaxVTotal         2047
#define _PanelMinVTotal         1088


#define _PanelDCLK              144
#define _PanelMaxDCLK           175
#define _PanelMinDCLK           120


#define _DefBurstModeFreq       250
#define _DefMinDutyValue            0x18
#define _DefMaxDutyValue        0xFF


#define _STEPL                  0x44
#define _STEPH                  0x00
#define _SPANL                  0xCC
#define _SPANH                  0x01

//=================================================================
//Color Temp
#define DefCool_RedColor            107
#define DefCool_GreenColor          110
#define DefCool_BlueColor           130
#define DefNormal_RedColor          120
#define DefNormal_GreenColor        120
#define DefNormal_BlueColor         130
#define DefWarm_RedColor            130
#define DefWarm_GreenColor      130
#define DefWarm_BlueColor           130
#define DefsRGB_RedColor      DefWarm_RedColor
#define DefsRGB_GreenColor  DefWarm_GreenColor
#define DefsRGB_BlueColor     DefWarm_BlueColor