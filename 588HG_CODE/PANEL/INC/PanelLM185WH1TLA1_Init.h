#define PanelName                   "LM185WH1TLA1"
#define _PanelColorDepth                bPanelColorDepth//0->6bit+FRC                       bPanelColorDepth->8Bit
#define PanelTTL                        0
#define PanelTCON                   0
#define PanelLVDS                   0xff
#define PanelRSDS                   0

#define _ChangeModePanelVddOff      bChangeModePanelVddOff//0->Change Mode Panel Vdd On     bChangeModePanelVddOff->Change Mode Panel Vdd Off

#define PanelDualPort               0xff
#define PanelSwapPort               0

#define _PanelOutputControl1            0x10//Scaler Bank 0x42 Setting
#define _PanelOSContol              0x83//Scaler Bank 0xF2 Setting
#define _PanelODRriving             0x55//Scaler Bank 0xF5 Setting


#define _PanelOnTiming1             25// time between panel & data while turn on power
#define _PanelOnTiming2             500 // time between data & back light while turn on power
#define _PanelOffTiming1                200// time between back light & data while turn off power
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
#define _PanelMaxHTotal             2046
#define _PanelMinHTotal             1520
#define _PanelMaxVTotal             1108
#define _PanelMinVTotal             776

#define _PanelDCLK                  86
#define _PanelMaxDCLK               108
#define _PanelMinDCLK               71


#define _DefBurstModeFreq           250//Hz
#define _DefMinDutyValue                0x3A//0x46
#define _DefMaxDutyValue            0xFF


#define _STEPL                      0x44
#define _STEPH                      0x00
#define _SPANL                      0xCC
#define _SPANH                      0x01

