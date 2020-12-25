#define PanelName                   "LM220WE1"
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


#define _PanelOnTiming1             25// time between panel & data while turn on power
#define _PanelOnTiming2             550 // time between data & back light while turn on power
#define _PanelOffTiming1                250// time between back light & data while turn off power
#define _PanelOffTiming2                25// time between data & panel while turn off power

#define _PanelHSyncWidth            39
#define _PanelHSyncBackPorch        5
#define _PanelVSyncWidth            3
#define _PanelVSyncBackPorch        21
#define _PanelHStart                    (_PanelHSyncWidth+_PanelHSyncBackPorch)
#define _PanelVStart                    (_PanelVSyncWidth+_PanelVSyncBackPorch)


#define _PanelWidth                 1680
#define _PanelHeight                    1050
#define _PanelHTotal                    1840
#define _PanelVTotal                    1080
#define _PanelMaxHTotal             2400
#define _PanelMinHTotal             1760
#define _PanelMaxVTotal             1300
#define _PanelMinVTotal             1060

#define _PanelDCLK                  120
#define _PanelMaxDCLK               150
#define _PanelMinDCLK               120


#define _DefBurstModeFreq           250//Hz
#define _DefMinDutyValue                0x14
#define _DefMaxDutyValue            0xFF


#define _STEPL                      0x44
#define _STEPH                      0x00
#define _SPANL                      0xCC
#define _SPANH                      0x01