
#if ENABLE_DP_INPUT

#ifndef _drvDPRXAPP_7_H_
#define _drvDPRXAPP_7_H_

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
#define XBYTE ((unsigned char volatile xdata *) 0)
#define mStar_ReadByte2(addr)	XBYTE[addr]
#define mStar_WriteByte2(addr, value) 		XBYTE[addr] = value

//==============================================
//    Must set 1   :   Keep  1  in your code  
//    Option       :   Decide by your project
//    For test     :   Keep  0  in your code
//==============================================
//                      Must set 1  in your code  
//==============================================
#define SupportDPAudio				        1 // Must set 1
#define DP_SQUELCH_IRQ				1    // Must set 1
#define DP_POWER_SAVING		      	        1    // Must set 1
#define DP_RESETHTT_PATCH			1    // Must set 1
#define DP_VPLLBIGChange			        1    // Must set 1
#define DP_AudioMuteEvent			        1    // Must set 1
#define DP_Aux_TimeOut				1    // Must set 1
//==============================================
//                        Option  :   Decide by your project
//==============================================
#define Displayport12					0    // If 1: Support DP 1.2(3D)            If 0: Support DP 1.1
#define DP54G						0    // If 1: Support 5.4G(TP3)             If 0: Only support 2.7G/1.62g(TP1/TP0)
#define DPENABLEMCCS				        1    // If 1: Support MCCS,                 If 0: Didn;t support MCCS
#define DP_FastTraining			                1    // If 1: Support FastTraining,         If 0: Didn;t support FastTraining
#define DP_AudioMN_FineTune		                0    // 120201 support audio should enable   // If 1: Support DP audio fine tune,   If 0: not support DP audio fine tune
#define DP_DPCDREPLACE                                  0    //// Enable Control register to replace DPCD.
#define DP_Audio_SSC                                    0    /////
#define DP_ATI_GRAPHIC_CARD_INTERLACE_PATCH             0      // Interlace 1080i  + Overscan issue in ATI vard
#define DP_DCOFFHPDLOW                                  0      // if 1:  DCOff HPD Low                   If 0:   DCOff HPD High 
#define DP_ASTRO_INTERLACE_PATCH               1
#define DP_RBR_Mode                                         0    //if 1: Only support 1.62G            if 0: support 2.7G
#define DP_Long_Cable                         		     0    // if 1: support 15m cable             if 0: don't support 15m cable 
#define DP_GUID_SW                                          (1&Displayport12)    // if 1: support GUID By SW reply   if 0 : don't support GUID by SW reply.
#define DP_DRRfunction                                     (1&&ENABLE_FREESYNC)  // if 1  :enable DDRFunction                       If 0:  disable DDR Function.
#define DP_OUI							      0
#define DP_PHY_CTS_7214                                (!DP_GUID_SW)
//==============================================
//			      For test  :   Keep  0  in your code
//==============================================
#define DP_REQUEST_VSWIM			        0    // For test 
#define DP_AUTO_EQ					0    // For test 
#define DP_SOFTEDID					0    // For test 
#define DP_USE_INTERNAL_EDID                  1
#define DP_USE_INTERNAL_HDCP                  1

//===============================================

#define Clr_LOSELOCK_FLAG()				//(msWriteByteMask(REG_1FEB,  _BIT5,  _BIT5))
#define Clr_DPPACKET_FLAG()				//(msWrite2Byte(REG_21AC, 0xFFFF))
#define Clr_MSA_FLAG()					//(msWriteByteMask(REG_21AD,  _BIT3,  _BIT3))
#define Clr_VSC_FLAG()					       // (msWriteByteMask(REG_21AD,  _BIT1,  _BIT1))

#define Get_LOSELOCK_FLAG()				//(_bit5_(msRegs[REG_1FEB]))
#define Get_MSA_FLAG()					//(_bit3_(msRegs[REG_21AB]))
#define Get_VSC_FLAG()					//(_bit1_(msRegs[REG_21AB]))

#define DPRxClrSQTime()                                         gDPInfo.bSquelchWaitStable=0;\
                                                                                gDPInfo.bSquelchStable=0;\
                                                                                bDPRxStableTimeoutFlag=0;\
                                                                                u16FastTrainingCntDown=0;\

#define AUX_RX()                                                   (mStar_WriteByte2(REG_06A8, (((mStar_ReadByte2(REG_06A8))|BIT3)&(~BIT4))))

#define AUX_TX()                                                   (mStar_WriteByte2(REG_06A8, (((mStar_ReadByte2(REG_06A8))|BIT4)&(~BIT3))))


#define DP_PLL_LDO_VALUE                HDMI_PLL_LDO_VALUE

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

typedef enum _DPAudioFreqType    //2012-05-16
{
    DPAUD_FREQ_ERROR,
    DPAUD_FREQ_32K,
    DPAUD_FREQ_44K,
    DPAUD_FREQ_48K,
    DPAUD_FREQ_88K,
    DPAUD_FREQ_96K,
    DPAUD_FREQ_176K,
    DPAUD_FREQ_192K,
} DPAudioFreqType;

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

 enum
{
    eSTATE_INITIAL,  //0
    eSTATE_RETRAIN,  // 1
    eSTATE_STARTUP,  // 2
    eSTATE_CKECKLOCK, // 3
    eSTATE_MEASURE, // 4
    eSTATE_NORMAL, // 5
    eSTATE_POWERSAVE, // 6
    eSTATE_DPIDLE  // 7
};


typedef struct
{
	
	WORD	uwDPHtotal;
	WORD	uwDPVtotal;
	WORD	uwDPHWidth;
	WORD	uwDPVWidth;
	WORD	uwDPHPWS;
	WORD	uwDPVPWS;
	WORD	uwDPHStart;
	WORD	uwDPVStart;
	Bool	        bDPHPol;
	Bool	        bDPVPol;
	BYTE	ucDPColorFormat;
	Bool	        bDPLock;
	Bool	        bDPInterlace;
	Bool	        bDPNoVideoStrm;
	Bool	        bDPNoAudioStrm;
	Bool	        bDPPowerSaving;
	Bool	        bAudioMute;
	Bool         bDPSave;
	Bool         bAupllRun;
        #if DP_FastTraining 
        Bool        bSquelchWaitStable;      
        Bool        bSquelchStable;
        #endif
	E_DP_3D_INPUT_MODE	DP_3DMode;

 } DP_INFO;

extern DP_INFO xdata gDPInfo;


#define bHDCPPortCbit                        BIT7    // 
#define bHDCPPortBbit                        BIT6    //

#define Set_HDCPPortBFlag()            (DPFlag|=bHDCPPortBbit)
#define Clr_HDCPPortBFlag()             (DPFlag&=~bHDCPPortBbit)
#define Set_HDCPPortCFlag()            (DPFlag|=bHDCPPortCbit)
#define Clr_HDCPPortCFlag()             (DPFlag&=~bHDCPPortCbit)

#define HDCPPortBFlag                       (DPFlag&bHDCPPortBbit)
#define HDCPPortCFlag                       (DPFlag&bHDCPPortCbit)                                 

//=======================================================================
#define HW_SET_AUDIO_MUTE_ENABLE()		     msWriteBit(REG_1427,TRUE,_BIT1);
                                                                                 


#define HW_SET_AUDIO_MUTE_DISABLE()	            msWriteBit(REG_1427,FALSE,_BIT1);
                                                                                    

#define DPINPUT_COLORIMETRY_ITU601()            !(msReadByte( REG_1369)& _BIT4)
#define DPINPUT_COLORIMETRY_ITU709()             (msReadByte( REG_1369)& _BIT4)
#define DPINPUT_COLORSPACEYUV()                       (((gDPInfo.ucDPColorFormat&(_BIT0|_BIT1)))!=InputColor_RGB)

 //============================================================================================
 //  Please add Cable GND pin detect in  below condition 
//((SrcInputType  == Input_Displayport)&&(msReadByte(REG_0232)&BIT0)&& (Cable GND pin Detect) )
#define DPPB_SPEEDUP_SDM_GND()                          ((SrcInputType  == Input_Displayport)&&(msReadByte(REG_0232)&BIT0))

#define DPPC_SPEEDUP_SDM_GND()                          ((SrcInputType  == Input_Displayport3)&&(msReadByte(REG_0232)&BIT4)) 


 #define EnablePBSDM()                                                  (msWriteByteMask(REG_022E, BIT0, BIT0));  
 #define EnablePCSDM()                                                  (msWriteByteMask(REG_022E, BIT4, BIT4));  
//============================================================================================

#define SetPortCHPDHigh()                                              (msWriteByteMask(REG_023D, BIT2, BIT2));\                                     
                                                                                          (msWriteByteMask(REG_023E, 0, BIT2));  


#define SetPortCHPDLow()                                              (msWriteByteMask(REG_023D, 0, BIT2));\                                          
                                                                                         (msWriteByteMask(REG_023E, 0, BIT2));        


#define SetPortBHPDHigh()                                             (msWriteByteMask(REG_0234, BIT1, BIT1));\                                   
                                                                                         (msWriteByteMask(REG_0236, 0, BIT1));


#define SetPortBHPDLow()                                              (msWriteByteMask(REG_0234, 0, BIT1));\                                             
                                                                                         (msWriteByteMask(REG_0236, 0, BIT1));


#define DPMSA_Reset()                                                    ( msWriteByteMask( REG_1753, BIT5,BIT5));\
                                                                                           ( msWriteByteMask( REG_1753, 0,BIT5));\
                                                                                           ( msWriteByteMask( REG_1753, BIT6,BIT6));\    
                                                                                           ( msWriteByteMask( REG_1753, 0,BIT6));




#define DPPacket_Reset()                                                (msWriteByteMask(REG_143A, BIT0, BIT0));\                                             
                                                                                            (msWriteByteMask(REG_143A, 0, BIT0));\
                                                                                            (msWriteByte(REG_14EF,0xFF));



#define Clr_BOOSTAUX_CM()                                             // (msWriteByteMask( REG_06AA, 0,BIT7));\
    
#define Set_BOOSTAUX_CM()                                            //  (msRegs[REG_06AA] = (msRegs[REG_06AA]|(BIT7)));\  

#define DPSetOffLine()                                                         (msWriteByteMask( REG_06C2, BIT2|BIT3, BIT2|BIT3 ));\
                                                                              		 (msWriteByteMask( REG_067D, BIT2, BIT2));\
                                                                              		 (msWriteByteMask( REG_077D, BIT2, BIT2))

#define DPFastTrainOff()                                                     (msWriteByteMask( REG_01CB, BIT5, BIT5));\
                                                                                            (msWriteByteMask( REG_17B5, 0, BIT6 ));\
  	                                                                                     (msWriteByteMask( REG_18E0, 0, BIT1|BIT2 ));

#define DPOffSQ()                                                                (msWriteByteMask( REG_01C2, 0, BIT2 ));\
  	                                                                        		 (msWriteByteMask( REG_01CA, BIT1, BIT1));       



#define DPSetMAudToZero() 							 (msWriteByte( REG_1458, 0 ));\    
                                   								 (msWriteByte( REG_1459, 0 ));\   
                                   								 (msWriteByte( REG_145A, 0 ));     


#define DPClearOffLineData() 							 (msWriteByte(REG_06E0,0));\
													 (msWriteByte(REG_06E1,0));\
                 											 (msWriteByte(REG_07E0,0));\
													 (msWriteByte(REG_07E1,0));\



#if 0
typedef struct _DP_3DDataType
{
    E_DP_3D_INPUT_MODE DP_3DMode;
    E_DP_3D_DATA_MODE  PAYLOAD;

}DP_3DDataType;
#endif
extern BYTE  XDATA ucFreqVaild_Old; 
//extern BYTE  XDATA   DP_Data[32] ;
extern BYTE _DPDATATYPE_ DPFlag;
extern BYTE _DPDATATYPE_ m_ucState;
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
extern bit g_bDPHDCPIRQ;

#if DP_FastTraining
extern WORD _DPDATATYPE_ u16DPRxStableTimeout;
extern bit bDPRxStableTimeoutFlag;
#define SetDPRxStableTimeout(x) ((x)?(bDPRxStableTimeoutFlag=0,u16DPRxStableTimeout=x,bDPRxStableTimeoutFlag=1):(bDPRxStableTimeoutFlag=0))

extern BYTE _DPDATATYPE_ OffFastRate;
extern BYTE _DPDATATYPE_ OffFastLane;
#endif
extern DWORD _DPDATATYPE_ DPTP1Cnt;
extern DWORD _DPDATATYPE_ DPTP2Cnt;
extern WORD _DPDATATYPE_ u16CDR_HPDCnt;
extern WORD _DPDATATYPE_ u16Skew_HPDCnt;// Add by CCJ
extern WORD _DPDATATYPE_ u16Audio_BWCnt; //   //20130716
extern WORD _DPDATATYPE_ u16HDCP_Cnt;   
extern bit bCDR_HPDCntFlag;
extern bit bSkew_HPDCntFlag;
extern bit bAudio_BWCntFlag;
extern bit bHDCP_CntFlag;
#define SetCDR_HPDCnt(x) ((x)?(bCDR_HPDCntFlag=0,u16CDR_HPDCnt=x,bCDR_HPDCntFlag=1):(bCDR_HPDCntFlag=0))
#define SetSkew_HPDCnt(x) ((x)?(bSkew_HPDCntFlag=0,u16Skew_HPDCnt=x,bSkew_HPDCntFlag=1):(bSkew_HPDCntFlag=0))
#define SetAudio_BWCnt(x) ((x)?(bAudio_BWCntFlag=0,u16Audio_BWCnt=x,bAudio_BWCntFlag=1):(bAudio_BWCntFlag=0))
#define SetHDCP_Cnt(x) ((x)?(bHDCP_CntFlag=0,u16HDCP_Cnt=x,bHDCP_CntFlag=1):(bHDCP_CntFlag=0))

//extern WORD _DPDATATYPE_ u16Audio_Cnt;  
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
extern WORD _DPDATATYPE_ Aux; 

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


extern void DPRxInit_PM( void );
#if DP_ASTRO_INTERLACE_PATCH
extern void DPRxInterlaceCheck(void);
#endif

#if DP_ATI_GRAPHIC_CARD_INTERLACE_PATCH
extern void DPRxATIInterlaceCheck(void);
#endif
extern void DPAux_Reset(void);
extern BYTE DPRxGetVB_ID(void);
extern void drvmStar_SetupInputPort_DisplayPort(void);
extern void DPAUXMeasure(void);
extern BOOL DPFastLink(BOOL bSearch);
extern void DPRxClearAUXVliadStatus(void);
extern BOOL DPCheck_Error(void);
extern void DPReset_Error(void);
extern void DPRxInit(void);
extern void DPRxHDCPLoadKey(void);
extern void DPRxEDIDLoad(void);
extern BOOL DPRxCheckTiming(BYTE Count);
extern void DPRxClearAUXVliadStatus(void);
extern void DPRxPollingDPCDPowerSave(void);
extern void DPRxMainStrmRst(void);
extern void DPRxMonitorHV(void);
extern void DPRxHPDEnable(void);
extern void DPRxHPDDisable(void);
extern void DPRxHPDIRQ(void);
extern void DPRxHPDRST(void);
extern void DPRxOutputEnable(BOOL bEnable);
extern void DPRxIRQEnable(BOOL bEnable);
extern void DPRxAuxIRQEnable(BOOL bEnable);
extern BOOL DPRxCheckLock(void);
extern void DPRxCheckInputFormat(void);
extern void DPPrintState(BYTE state);
extern void DPRxAudioHandle(void);
extern void DPRxHandle();
extern void DP_EDID_TEST(BYTE ucPara1,BYTE ucPara2);
extern void DPRxInitState(void);
#if SupportDPAudio
extern void DPRxAudioHandle(void);
extern void DPAudioMuteCheck(void);
extern  DPAudioFreqType DPRxCheckAudioFreq( void );    
extern void DPAudioSSC( void );    
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
extern BOOL DPRxCheckSQTime(WORD times);
extern void DPChkScramble(void);
#endif
extern void DPGetAuxInfo( void );
extern void DPRxAuxInit(void);
extern void DP_EnableSquelch(Bool bEnable);
extern Bool DP_GetSquelch(DWORD Times); 
extern void DPRxCDRLossHander(void);
#if DP_VPLLBIGChange
extern void DPRxVPLLHandle(void);
#endif
extern Bool DP_GetSquelchPortB(WORD Times); 
extern Bool DP_GetSquelchPortC(WORD Times); 
extern Bool DPRxGetInputDE(WORD* pHDE, WORD* pVDE);

#if DP_AUTO_EQ
void DPRxAutoEQEnable(BYTE bEnable);
void DPRxEQInform(void);
#endif
void DPISR_AUX0(void);
void DPRxHdcpReAuthen(void);
extern void DPRXPMForceEnter();
void DPEventEnable(Bool bEnable);
BOOL DPCheckAuxState(BYTE Times);
void DPRxHvFsmChk(void);
#if DP_DRRfunction
extern void DPRxEnableDRRFunction(Bool Enable );
extern Bool  DPRxGetDRRFlag(void);
extern Bool  DPRxGetDRRFlag_ISR(void);

extern WORD  DPRxGetPixelClk(void);
extern WORD  DPRxGetHTotal(void);
#endif 
#endif //_DPRXAPP_H_
#endif
