
#if ENABLE_DP_INPUT

#ifndef _DPRXAPP_H_
#define _DPRXAPP_H_

#ifdef _DPRXAPP_C_
#define _DPRXAPP_DEC_
#else
#define _DPRXAPP_DEC_ extern
#endif

#define _DPDATATYPE_            xdata
#define _DPDATATYPEEDID_		xdata
// ***************    For Display Port CTS  *******************
#define DPCTS                                          1
#define DPENABLEINTERLANESKEW   	DPCTS
#define DPENABLELOSECDRLOCK            DPCTS
#define NEWCRCMODE                               1
#define DPDEBUG                                      0
// ******************************************************
#define U02		 0					
#define U03           1
//==============================================
//    Must set 1   :   Keep  1  in your code  
//    Option       :   Decide by your project
//    For test     :   Keep  0  in your code
//==============================================
//                      Must set 1  in your code  
//==============================================
#define SupportDPAudio				        1    // Must set 1
#define DP_SQUELCH_IRQ				1    // Must set 1
#define DP_POWER_SAVING		      	        1    // Must set 1
#define DP_RESETHTT_PATCH			1    // Must set 1
#define DP_VPLLBIGChange			        1    // Must set 1
#define DP_AudioMuteEvent			        1    // Must set 1
#define DP_Aux_TimeOut				1    // Must set 1
//==============================================
//                        Option  :   Decide by your project
//==============================================
#define Displayport12					1    // If 1: Support DP 1.2(3D)            If 0: Support DP 1.1
#define DP54G						0    // If 1: Support 5.4G(TP3)             If 0: Only support 2.7G/1.62g(TP1/TP0)
#define DPENABLEMCCS				        1   // If 1: Support MCCS,                 If 0: Didn;t support MCCS
#define DP_FastTraining				1    // If 1: Support FastTraining,         If 0: Didn;t support FastTraining
#define DP_AudioMN_FineTune		        1 // 120201 support audio should enable   // If 1: Support DP audio fine tune,   If 0: not support DP audio fine tune
//==============================================
//			      For test  :   Keep  0  in your code
//==============================================
#define DP_REQUEST_VSWIM			        0    // For test 
#define DP_AUTO_EQ					0    // For test 
#define DP_SOFTEDID					0    // For test 
#define DP_USE_INTERNAL_EDID                  0
#define DP_USE_INTERNAL_HDCP                  0
//===============================================

#define Clr_LOSELOCK_FLAG()				(msWriteByteMask(REG_1FEB,  _BIT5,  _BIT5))
#define Clr_DPPACKET_FLAG()				(msWrite2Byte(REG_21AC, 0xFFFF))
#define Clr_MSA_FLAG()					(msWriteByteMask(REG_21AD,  _BIT3,  _BIT3))
#define Clr_VSC_FLAG()					        (msWriteByteMask(REG_21AD,  _BIT1,  _BIT1))

#define Get_LOSELOCK_FLAG()				(_bit5_(msRegs[REG_1FEB]))
#define Get_MSA_FLAG()					(_bit3_(msRegs[REG_21AB]))
#define Get_VSC_FLAG()					(_bit1_(msRegs[REG_21AB]))



typedef enum
{
    //range [0000 ~ 1111] reserved for DP 3D spec
    E_DP_3D_INPUT_NO_STEREO                                   = 0x00, //0000
    E_DP_3D_INPUT_FRAME_SEQUENTIAL                     = 0x01, //0001    /* page flipping */
    E_DP_3D_INPUT_STACK_FRAME                               = 0x02, //0002    //Frame Packing
    E_DP_3D_INPUT_PIXEL_INTERLEAVED                     = 0x03, //003     /* line interleave */
    E_DP_3D_INPUT_SIDE_BY_SIDE_FULL                     = 0x04, //004     /* side by side full */
    E_DP_3D_RESERVED                                                  = 0x05, // 005, 101 ~ 1111
}  E_DP_3D_INPUT_MODE;


   // SC_FMT_IN_NORMAL,
   //   SC_FMT_IN_3D_PF,        /* page flipping */
   //   SC_FMT_IN_3D_FP,        /* frame packing */
   //   SC_FMT_IN_3D_FPI,       /* frame packing interlace */
   //   SC_FMT_IN_3D_TB,        /* top bottom */
   //   SC_FMT_IN_3D_SBSH,      /* side by side half */
   //   SC_FMT_IN_3D_LA,        /* line interleave */
   //   SC_FMT_IN_3D_SBSF,      /* side by side full */
   //   SC_FMT_IN_3D_CB,        /* chess board */



typedef enum
{
    E_DP_PIXELINTERLEAVE_LLRR                                  = 0x00, //0000
    E_DP_PIXELINTERLEAVE_RRLL                                 = 0x01, //0001
    E_DP_PIXELINTERLEAVE_LRRL                                  = 0x02, //0002
    E_DP_PIXELINTERLEAVE_LRLR                              = 0x03, //0003
    E_DP_PIXELINTERLEAVE_RLRL                            = 0x04, //0004
    E_DP_SIDEBYSIDE_LR                                             = 0x05, //0005
    E_DP_SIDEBYSIDE_RL                                             = 0x06, //0005
    E_DP_3D_DATA_RESERVED
}  E_DP_3D_DATA_MODE;

typedef enum
{
    DP_LBR                              = 0x00, //0000
    DP_HBR                             = 0x01, //0001

}  E_DP_Rate;


typedef enum
{
    NORMAL_TRAINING               = 0x00, //0000
    FAST_TRAINING2              = 0x01, //0001
    FAST_TRAINING1           = 0x02, //0002

}  E_DP_TrainingMode;

typedef enum
{
    FINISH               = 0x00, //0000
    TIME_OUT           = 0x01, //0001
    INITIAL            = 0x02, //0002

}  E_DP_AutoEq;




typedef struct
{
	BYTE	ucLane;
	BYTE	ucRate;
	BYTE	ucVSwing;
	BYTE	ucEmphasis;
	WORD	uwDPHtotal;
	WORD	uwDPVtotal;
	WORD	uwDPHWidth;
	WORD	uwDPVWidth;
	WORD	uwDPHPWS;
	WORD	uwDPVPWS;
	WORD	uwDPHStart;
	WORD	uwDPVStart;
	Bool	bDPHPol;
	Bool	bDPVPol;
	BYTE	ucDPColorFormat;
	Bool	bCDRLock;
	Bool	bEQSYMLock;
	Bool	bDPLock;
	Bool	bTrainingDone;
	Bool	bDPInterlace;
	Bool	bDPNoVideoStrm;
	Bool	bDPNoAudioStrm;
	Bool	bDPPowerSaving;
	Bool	bAudioMute;
	Bool    bDPSave;
	Bool    bDPDCoff;
		
	E_DP_3D_INPUT_MODE	DP_3DMode;
	E_DP_3D_DATA_MODE	PAYLOAD;
	E_DP_TrainingMode	bDPTrainMode;

 } DP_INFO;

extern DP_INFO xdata gDPInfo;

#define HW_SET_AUDIO_MUTE_ENABLE()			 msWriteBit(REG_05CC,TRUE,_BIT4);
                                                                                    //msWriteBit(REG_1B06,TRUE,_BIT5);
                                                                                    //msWriteBit(REG_05CC,TRUE,_BIT4);


#define HW_SET_AUDIO_MUTE_DISABLE()	            msWriteBit(REG_05CC,FALSE,_BIT4);
                                                                              //msWriteBit(REG_1B06,FALSE,_BIT5);
											//msWriteBit(REG_05CC,FALSE,_BIT4);

#define DPINPUT_COLORIMETRY_ITU601()            !(gDPInfo.ucDPColorFormat& _BIT4)
#define DPINPUT_COLORIMETRY_ITU709()             (gDPInfo.ucDPColorFormat& _BIT4)

#define DPINPUT_COLORSPACEYUV()                     (((gDPInfo.ucDPColorFormat&(_BIT2|_BIT1))>>1)!=InputColor_RGB)

#if 0
typedef struct _DP_3DDataType
{
    E_DP_3D_INPUT_MODE DP_3DMode;
    E_DP_3D_DATA_MODE  PAYLOAD;

}DP_3DDataType;
#endif
#if DP_SOFTEDID
extern BYTE EDIDADDR;
extern BYTE EDIDOffSetAddr;
extern BYTE Length;
#endif
#if DPCTS
extern BYTE _DPDATATYPE_ CTSLevel;
extern BYTE _DPDATATYPE_ DPChkCnt;
#if DPENABLEINTERLANESKEW
extern bit g_bDPInterlaneSkewIRQ;
#endif
#if DPENABLELOSECDRLOCK
extern bit g_bDPLoseCDRLockIRQ;
#endif
#endif
#if DP_FastTraining
extern BYTE _DPDATATYPE_ FastRate;
extern BYTE _DPDATATYPE_ FastLane;
#endif
extern DWORD _DPDATATYPE_ DPTP1Cnt;
extern DWORD _DPDATATYPE_ DPTP2Cnt;
extern WORD _DPDATATYPE_ u16CDR_HPDCnt;
extern WORD _DPDATATYPE_ u16Skew_HPDCnt;// Add by CCJ
extern bit bCDR_HPDCntFlag;
extern bit bSkew_HPDCntFlag;
#define SetCDR_HPDCnt(x) ((x)?(bCDR_HPDCntFlag=0,u16CDR_HPDCnt=x,bCDR_HPDCntFlag=1):(bCDR_HPDCntFlag=0))
#define SetSkew_HPDCnt(x) ((x)?(bSkew_HPDCntFlag=0,u16Skew_HPDCnt=x,bSkew_HPDCntFlag=1):(bSkew_HPDCntFlag=0))
 

#if DP_REQUEST_VSWIM
extern BYTE _DPDATATYPE_ DP_Train_Fix;
extern BYTE _DPDATATYPE_ DP_Count;
#endif

#if DP_VPLLBIGChange
extern BYTE _DPDATATYPE_ g_bDPVPLLMNBIGCHANGE; // for DisplayPort
#endif

extern WORD _DPDATATYPE_ SCHPeriod;
extern WORD _DPDATATYPE_ SCHPeriod1S;
extern WORD _DPDATATYPE_ SCVTotal;
extern BYTE _DPDATATYPE_ DPFindModeCnt;
extern BYTE _DPDATATYPE_ DPCheckModeCnt;
extern BYTE _DPDATATYPE_ DPState;
extern BYTE _DPDATATYPE_ g_bDPTrainingP0T;
extern BYTE _DPDATATYPE_ g_bDPTrainingP1T;
extern WORD _DPDATATYPE_ LockFailCnt;
extern WORD _DPDATATYPE_  PowerSavingCnt;
extern BYTE _DPDATATYPE_ TrainingP1Cnt;
extern BYTE _DPDATATYPE_ TrainingP2Cnt;
extern WORD _DPDATATYPE_ TrainingTimer;
extern bit g_bDPVPLLBIGChange;
extern bit g_bDPAUPLLBIGChange;
extern bit g_bDPAUXVALID;
extern bit g_bDPDPCDPOWERSAVE;
extern BYTE _DPDATATYPE_ g_DPDPCDPOWERSAVECNT;
extern bit g_bDPPowerDown;

extern bit g_bDPLANELOCK;
extern WORD _DPDATATYPE_ LinkRate;
extern BYTE _DPDATATYPE_ HLBR;


#if SupportDPAudio
extern BYTE _DPDATATYPE_ DP_AudioDebunce_On;
extern BYTE _DPDATATYPE_ DP_AudioDebunce_Off;
extern DWORD _DPDATATYPE_ DPAUM0;
extern DWORD _DPDATATYPE_ DPAUM1;
extern DWORD _DPDATATYPE_ DPAUM2;
extern DWORD _DPDATATYPE_ DPAUN0;
extern DWORD _DPDATATYPE_ DPAUN1;
extern DWORD _DPDATATYPE_ DPAUN2;
extern BYTE _DPDATATYPE_ DPFBDIV0;
extern BYTE _DPDATATYPE_ DPFBDIV1;
extern BYTE _DPDATATYPE_ DPKP;
extern BYTE _DPDATATYPE_ DPKM;
//extern BYTE xdata DPKPV;
//extern BYTE xdata DPKMV;
extern BYTE _DPDATATYPE_ DPFREQ0;
extern BYTE _DPDATATYPE_ DPFREQ1;
extern BYTE _DPDATATYPE_ DPFREQ2;
extern DWORD _DPDATATYPE_ DPAUM;
extern DWORD _DPDATATYPE_ DPAUN;
extern DWORD _DPDATATYPE_ DPFREQ;
extern DWORD _DPDATATYPE_ DPFBDIV;
#endif
extern void DPRxClearAUXVliadStatus(void);
extern BOOL DPCheck_Error(void);
extern void DPReset_Error(void);
extern void DPRxInit(void);
extern void DPRxHDCPLoadKey(void);
extern void DPRxEDIDLoad(void);
extern BOOL DPRxCheckTiming(void);
extern void DPRxClearAUXVliadStatus(void);
extern void DPRxPollingDPCDPowerSave(void);
extern void DPRxMainStrmRst(void);
//extern void DPRxCDRLossHander(void);
extern void DPRxMonitorHV(void);
extern void DPRxHPDEnable(void);
extern void DPRxHPDDisable(void);
extern void DPRxHPDIRQ(void);
extern void DPRxDCHPD( Bool enable );
extern void DPRxHPDRST(void);
extern void DPRxOutputEnable(BOOL bEnable);
extern void DPRxIRQEnable(BOOL bEnable);
extern BOOL DPRxCheckLock(void);
extern void DPRxCheckInputFormat(void);
extern void DPPrintState(BYTE state);
extern void DPRxAudioHandle(void);
extern void DPRxHandle();
extern void DP_EDID_TEST(BYTE ucPara1,BYTE ucPara2);
extern void msDPInitState(void);
#if SupportDPAudio
extern void DPRxAudioHandle(void);
extern void DPAudioMuteCheck(void);
#endif
extern void DPISR(void);
extern void DPRx3DInform(void);
extern void DPRxTiming(void);
extern void DPRxCheckInputFormat(void);
#if DP_FastTraining
extern void DPRxFastTraining2Enable(Bool bEnable);
extern void DPRxFastTraining1Enable(Bool bEnable);
extern void DP_SetSyntheizer(E_DP_Rate bRate);
extern void DPRxFastTrainingInitial(void);
extern BOOL DPRxCheckTiming1( BYTE Count );
#endif

extern void DPRxAuxInit(void);
extern void DP_EnableSquelch(Bool bEnable);
extern Bool DP_GetSquelch(DWORD Times);
extern void DPRxCDRLossHander(void);
#if DP_VPLLBIGChange
extern void DPRxVPLLHandle(void);
#endif
#if DP_AUTO_EQ
void DPRxAutoEQEnable(BYTE bEnable);
void DPRxEQInform(void);
#endif
void DPRxHdcpReAuthen(void);
extern void DPRXPMForceEnter();
#endif //_DPRXAPP_H_

#endif
