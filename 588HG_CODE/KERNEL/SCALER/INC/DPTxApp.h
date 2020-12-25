#ifndef _DPTXAPP_H_
#define _DPTXAPP_H_

#define _DPTXDATATYPE_  xdata

#if ENABLE_EDP_OUTPUT
#ifndef eDPTXTiming
#define eDPTXTiming 4 //DP_1080P
#endif
#ifndef eDPTXMinRate 
#define eDPTXMinRate 1 //DPTXHBR
#endif
#ifndef eDPTXMinLane
#define eDPTXMinLane 2 //DPTX4L
#endif

// #####  PingPong FIFO Value ####################################
#if eDPTXMinRate==DPTXHBR
#define eDPTXLinkRate  270
#elif eDPTXMinRate==DPTXRBR
#define eDPTXLinkRate  162
#endif

#if eDPTXMinLane==DPTX1L
#define eDPTXPingPongN  6  //6
#elif eDPTXMinLane==DPTX2L
#define eDPTXPingPongN  9  //10
#elif eDPTXMinLane==DPTX4L
#define eDPTXPingPongN  10 //14
#endif

#define eDPTXPingPongV   (((DWORD)eDPTXLinkRate*eDPTXPingPongN)/PanelDCLK)

#define ENABLE_ADJUST_PRENPHSIS		(1&&(PanelType == PaneleDP_AUO_B156HTN03_FHD_60Hz || PanelType == PaneleDP_PAN_VVX13F009G00_FHD_60Hz ))
//#######################################################

#define DP32G 0
#define DP29G 0
#define DP27G 0
#define FASTTRINING  0
#define eDPTXU03 1
#define EN_DPTX_SSC   0
#define EN_CRCTEST     0

typedef struct
{
        BYTE   ucLane;
        BYTE   ucRate;
        BYTE   ucVSwing;
        BYTE   ucEmphasis;
        WORD uwDPHtotal;
        WORD uwDPVtotal;
        WORD uwDPHWidth;
        WORD uwDPVWidth;
        WORD uwDPHPWS;
        WORD uwDPVPWS;
        WORD uwDPHStart;
        WORD uwDPVStart;
        Bool    bDPHPol;
        Bool    bDPVPol;
        BYTE   ucDPColorFormat;
        Bool  bCDRLock;
        Bool  bEQSYMLock;
        Bool  bDPLock;
        Bool  bTrainingDone;
     Bool  bDPInterlace;
        Bool  bDPNoVideoStrm;
        Bool  bDPNoAudioStrm;
        Bool  bDPPowerSaving;
        Bool  bAudioMute;
	 BYTE  bHPD;
	 BYTE  bPlugOut;
        BYTE  bLinkRate;
        BYTE  bLinkLane;
        BYTE  bReTraining;
 } DPTX_INFO;

extern void DPTXCheckCap(void);
extern void DPCDWRITEBYTES (BYTE ADDR2, BYTE ADDR1, BYTE ADDR0,  BYTE LEN , BYTE *TXDATA);
extern BOOL DPTxCheckLockCDR(BYTE LaneCnt);
extern void  DPTXTraining(void);
extern void  DPTX_CheckHPD(void);
extern DPTX_INFO xdata gDPTXInfo;
extern void  DPISR_TX(void);
extern void DPTxIRQEnable(BOOL bEnable);
extern void DPTxInit(void);
void DPTxOutputEnable(Bool bEnable);
BOOL DPTxCheckLock(BYTE LaneCnt);
extern void DPTxEDIDRead(void);
void DPTXPrintState(BYTE state);
extern void DPTxHandle(void);
extern void DPTxCheckCRC(void);
extern void DPAUX_TEST(void);


#else
extern void msDpTxDummy();
#endif
/*
enum
{
DP_800X600_60HZ,
DP_1024X768_60HZ,
DP_480P,
DP_720P,
DP_1080P,
DP_1280X800_60HZ,
DP_1360X768_60HZ
};

enum
{
DPTXRBR,
DPTXHBR,
DPTXHBRx11,
DPTXHBRx12
};

enum
{
DPTX1L,
DPTX2L,
DPTX4L
};
*/
#endif
