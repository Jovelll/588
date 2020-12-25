#define PanelName                   "TPM200O1\x011L02"
#define _PanelColorDepth                0//0->6bit+FRC                      bPanelColorDepth->8Bit
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
#define _PanelOnTiming2             500 // time between data & back light while turn on power
#define _PanelOffTiming1                120// time between back light & data while turn off power
#define _PanelOffTiming2                25// time between data & panel while turn off power

#define _PanelHSyncWidth            45
#define _PanelHSyncBackPorch        40
#define _PanelVSyncWidth            3
#define _PanelVSyncBackPorch        21
#define _PanelHStart                    (_PanelHSyncWidth+_PanelHSyncBackPorch)
#define _PanelVStart                    (_PanelVSyncWidth+_PanelVSyncBackPorch)


#define _PanelWidth                 1600
#define _PanelHeight                    900
#define _PanelHTotal                    2112
#define _PanelVTotal                    934
#define _PanelMaxHTotal             2300
#define _PanelMinHTotal             1900
#define _PanelMaxVTotal             955
#define _PanelMinVTotal             906

#define _PanelDCLK                  119
#define _PanelMaxDCLK               150
#define _PanelMinDCLK               97


#define _DefBurstModeFreq           250//Hz
#define _DefMinDutyValue                0x01
#define _DefMaxDutyValue            0xFF


#define _STEPL                      0x44
#define _STEPH                      0x00
#define _SPANL                      0xCC
#define _SPANH                      0x01

//=================================================================
//Color Temp
#define DefCool_RedColor                    108
#define DefCool_GreenColor                 127
#define DefCool_BlueColor                   155
#define DefNormal_RedColor                128
#define DefNormal_GreenColor            138
#define DefNormal_BlueColor               155
#define DefWarm_RedColor                  138
#define DefWarm_GreenColor               144
#define DefWarm_BlueColor                 155
#define DefsRGB_RedColor                    138
#define DefsRGB_GreenColor                144
#define DefsRGB_BlueColor                   155