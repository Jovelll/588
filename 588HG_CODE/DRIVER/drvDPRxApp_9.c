//******************************************************************************
//  Copyright (c) 2003-2008 MStar Semiconductor, Inc.
//  All rights reserved.
//
//  [Module Name]:
//      DPRxApp.c
//  [Abstract]:
//      This module contains code for DisplayPort receiver's application level
//      procedure and subroutin
//  [Author(s)]:
//      Vincent Kuo
//  [Reversion History]:
//      Initial release:    06 May, 2018
//*******************************************************************************

#define _drvDPRXAPP_9_C_
#include "board.h"
#if ENABLE_DP_INPUT
#if ENABLE_FREESYNC
#include "ComboApp.h"
#endif
#include <string.h>
#include <math.h>
#include "types.h"
#include "board.h"
#include "ms_reg.h"
#include "mode.h"
#include "UserPrefDef.h"
#include "userpref.h"
#include "global.h"
#include "panel.h"
#include "ms_rwreg.h"
#include "debug.h"
#include "detect.h"
#include "mStar.h"
#include "misc.h"
#include "drvDPRxApp.h"
#include "msScaler.h"
#include "DDC2Bi.h"
#include "MsACE.h"
#include "NVRam.h"
#include "drvMcu.h"
#include "common.h"
#include "msflash.h"
#include "GPIO_Def.h"
#include "Adjust.h"
#if MS_DAC
#include "drvDAC.h"
#endif
//-------------------------------------------------------------------------------------------------
//  Local Structures & define
//-------------------------------------------------------------------------------------------------
#include "msComboRx.h"

DP_INFO _DPDATATYPE_ gDPInfo;

#define  DP_DEBUG  1

#if (DP_DEBUG&&ENABLE_DEBUG)
#define DP_printData(str, value)   printData(str, value)
#define DP_printMsg(str)               printMsg(str)
#else
#define DP_printData(str, value)
#define DP_printMsg(str)
#endif
//BYTE  XDATA   DP_Data[32] _at_ 0x4A00;

BYTE _DPDATATYPE_ DPNUM=0;
WORD  _DPDATATYPE_ DPLEG=0;

#if DP_SOFTEDID
BYTE EDIDADDR, EDIDOffSetAddr, Length = 0;
#endif

#if 0
E_DP_AutoEq _DPDATATYPE_  EQ_Done = INITIAL;
BYTE _DPDATATYPE_  EQ_Value = 0;
#endif

#if DPCTS
BYTE _DPDATATYPE_ CTSLevel = 0;
BYTE _DPDATATYPE_ DPChkCnt = 0;
#endif

#if DPENABLEINTERLANESKEW
bit g_bDPInterlaneSkewIRQ = 0;
#endif
#if DPENABLELOSECDRLOCK
bit g_bDPLoseCDRLockIRQ = 0;
#endif
bit g_bDPHDCPIRQ = 0;
BYTE _DPDATATYPE_ ucFreqVaild_Old=0;
BYTE _DPDATATYPE_ DPFlag=0;

#if DP_FastTraining
WORD volatile xdata u16DPRxStableTimeout=0;
bit bDPRxStableTimeoutFlag = 0;
WORD _DPDATATYPE_ u16FastTrainingCntDown=0;
#endif

DWORD _DPDATATYPE_ DPTP1Cnt = 0;
DWORD _DPDATATYPE_ DPTP2Cnt = 0;
WORD _DPDATATYPE_ u16CDR_HPDCnt = 0;
WORD _DPDATATYPE_ u16Skew_HPDCnt = 0; // Add by CCJ
WORD _DPDATATYPE_ u16Audio_BWCnt = 0; // Add by CCJ
WORD _DPDATATYPE_ u16HDCP_Cnt = 0;
bit bCDR_HPDCntFlag=0;
bit bSkew_HPDCntFlag=0;
bit bAudio_BWCntFlag = 0;
bit bHDCP_CntFlag = 0;
#define SetCDR_HPDCnt(x) ((x)?(bCDR_HPDCntFlag=0,u16CDR_HPDCnt=x,bCDR_HPDCntFlag=1):(bCDR_HPDCntFlag=0))
#define SetSkew_HPDCnt(x) ((x)?(bSkew_HPDCntFlag=0,u16Skew_HPDCnt=x,bSkew_HPDCntFlag=1):(bSkew_HPDCntFlag=0))
#define SetAudio_BWCnt(x) ((x)?(bAudio_BWCntFlag=0,u16Audio_BWCnt=x,bAudio_BWCntFlag=1):(bAudio_BWCntFlag=0))
#define SetHDCP_Cnt(x) ((x)?(bHDCP_CntFlag=0,u16HDCP_Cnt=x,bHDCP_CntFlag=1):(bHDCP_CntFlag=0))

//WORD _DPDATATYPE_ u16Audio_Cnt = 0;

#if DP_REQUEST_VSWIM
BYTE _DPDATATYPE_ DP_Train_Fix = 0;
BYTE _DPDATATYPE_ DP_Count = 0;
#endif
BYTE _DPDATATYPE_ DPState = 20;
WORD _DPDATATYPE_ SCHPeriod = 0;
WORD _DPDATATYPE_ SCHPeriod1S = 0;
WORD _DPDATATYPE_ SCVTotal = 0;
BYTE _DPDATATYPE_ DPFindModeCnt = 0;
BYTE _DPDATATYPE_ DPCheckModeCnt = 0;
WORD _DPDATATYPE_ DPMVID = 0;
WORD _DPDATATYPE_ DPMVID1S = 0;
BYTE _DPDATATYPE_ g_bDPTrainingP0T = 0;
BYTE _DPDATATYPE_ g_bDPTrainingP1T = 0;
WORD _DPDATATYPE_ LockFailCnt = 0;
WORD _DPDATATYPE_ PowerSavingCnt = 0;
BYTE _DPDATATYPE_ NoVscPktCnt = 0;
BYTE _DPDATATYPE_ TrainingP1Cnt = 0;
BYTE _DPDATATYPE_ TrainingP2Cnt = 0;
WORD _DPDATATYPE_ TrainingTimer = 0;
bit g_bDPVPLLBIGChange = 0;
bit g_bDPAUPLLBIGChange = 0;
bit g_bDPAUXVALID = 0;
bit g_bDPDPCDPOWERSAVE = 0;
BYTE _DPDATATYPE_ g_DPDPCDPOWERSAVECNT = 0;
bit g_bDPPowerDown = 0;

bit g_bDPLANELOCK = 0;
bit g_bDPCDRLOCK = 0;
#if DP_FastTraining
BYTE _DPDATATYPE_ OffFastRate = 0;
BYTE _DPDATATYPE_ OffFastLane = 0;
#endif
WORD _DPDATATYPE_ LinkRate = 0;
BYTE _DPDATATYPE_ HLBR = 0;
#if SupportDPAudio
BYTE _DPDATATYPE_ DP_AudioDebunce_On = 0;
BYTE _DPDATATYPE_ DP_AudioDebunce_Off = 0;
DWORD _DPDATATYPE_ DPAUM0 = 0;
DWORD _DPDATATYPE_ DPAUM1 = 0;
DWORD _DPDATATYPE_ DPAUM2 = 0;
DWORD _DPDATATYPE_ DPAUN0 = 0;
DWORD _DPDATATYPE_ DPAUN1 = 0;
DWORD _DPDATATYPE_ DPAUN2 = 0;
BYTE _DPDATATYPE_ DPFBDIV0 = 0;
BYTE _DPDATATYPE_ DPFBDIV1 = 0;
BYTE _DPDATATYPE_ DPKP = 0;
BYTE _DPDATATYPE_ DPKM = 0;
BYTE _DPDATATYPE_ DPFREQ0 = 0;
BYTE _DPDATATYPE_ DPFREQ1 = 0;
BYTE _DPDATATYPE_ DPFREQ2 = 0;
DWORD _DPDATATYPE_ DPAUM = 0;
DWORD _DPDATATYPE_ DPAUN = 0;

DWORD _DPDATATYPE_ DPAUM_OLD = 0;
DWORD _DPDATATYPE_ DPAUN_OLD = 0;

DWORD _DPDATATYPE_ DPFREQ = 0;
DWORD _DPDATATYPE_ DPFBDIV = 0;

DWORD _DPDATATYPE_ DPAUM_Temp = 0;
DWORD _DPDATATYPE_ DPAUN_Temp= 0;
BYTE   _DPDATATYPE_ DPAudio_Cnt=0;
#endif

#if DP_VPLLBIGChange
BYTE _DPDATATYPE_ g_bDPVPLLMNBIGCHANGE = 0;
#endif

 WORD _DPDATATYPE_ Aux=0;
BYTE xdata DDCData[16] ={0};
#if DPENABLEMCCS
BYTE _DPDATATYPE_ MCCSRxCount = 0;
BYTE _DPDATATYPE_ DPMCCSOffSetAddr = 0;
BYTE _DPDATATYPE_ DPMCCSRXCNT = 0;
BYTE _DPDATATYPE_ DPMCCSRXBUF[16];
BYTE _DPDATATYPE_ DPMCCSTXBUF[16] =
{0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x36, 0x74, 0x22, 0x10, 0x86, 0x69, 0x32, 0x01};
#endif

///#define HFreq_Torlance      5
///#define VFreq_Torlance      5
#define DPDPCDPOWERSAVE_COUNTER 8
#define MSACnt     2
//-------------------------------------------------------------------------------------------------
//  External Variables
//-------------------------------------------------------------------------------------------------
//extern InputModeType code StandardMode;
//extern ModeSettingType xdata DPModeSetting;

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------

 BYTE _DPDATATYPE_ m_ucState = eSTATE_STARTUP;

#if DP_EDID_384
BYTE code  EDID[384] =
{
       0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x4D, 0xD9, 0x01, 0xFD, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x14, 0x01, 0x03, 0x80, 0x90, 0x51, 0x78, 0x0A, 0x0D, 0xC9, 0xA0, 0x57, 0x47, 0x98, 0x27,
    0x12, 0x48, 0x4C, 0x21, 0x08, 0x00, 0x81, 0x80, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
    0x45, 0x00, 0xA0, 0x2A, 0x53, 0x00, 0x00, 0x1E, 0x04, 0x74, 0x00, 0x30, 0xF2, 0x70, 0x5A, 0x80,
    0xB0, 0x58, 0x8A, 0x00, 0xA0, 0x2A, 0x53, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFD, 0x00, 0x3A,
    0x3E, 0x0F, 0x46, 0x0F, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFC,
    0x00, 0x53, 0x4F, 0x4E, 0x59, 0x20, 0x54, 0x56, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x02, 0x34,

    0x02, 0x03, 0x34, 0xF0, 0x4A, 0x10, 0x04, 0x05, 0x03, 0x02, 0x07, 0x06, 0x20, 0x01, 0x3C, 0x26,
    0x09, 0x07, 0x07, 0x15, 0x07, 0x50, 0x83, 0x01, 0x00, 0x00, 0x76, 0x03, 0x0C, 0x00, 0x20, 0x00,
    0xB8, 0x3C, 0x2F, 0x88, 0x0C, 0x20, 0x90, 0x08, 0x10, 0x18, 0x10, 0x28, 0x10, 0x78, 0x10, 0x06,
    0x26, 0xE2, 0x00, 0x7B, 0x01, 0x1D, 0x80, 0x18, 0x71, 0x1C, 0x16, 0x20, 0x58, 0x2C, 0x25, 0x00,
    0xA0, 0x2A, 0x53, 0x00, 0x00, 0x9E, 0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E,
    0x96, 0x00, 0xA0, 0x2A, 0x53, 0x00, 0x00, 0x18, 0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10,
    0x10, 0x3E, 0x96, 0x00, 0x38, 0x2A, 0x43, 0x00, 0x00, 0x18, 0x8C, 0x0A, 0xA0, 0x14, 0x51, 0xF0,
    0x16, 0x00, 0x26, 0x7C, 0x43, 0x00, 0x38, 0x2A, 0x43, 0x00, 0x00, 0x98, 0x00, 0x00, 0x00, 0x8F,

    0x70, 0x11, 0x79, 0x06, 0x00, 0x03, 0x00, 0x3C, 0x03, 0x74, 0x00, 0x04, 0xFF, 0x0E, 0x9F, 0x05,
    0x1F, 0x84, 0x57, 0x00, 0x6F, 0x08, 0x59, 0x00, 0x07, 0x80, 0x09, 0x00, 0x03, 0x74, 0x00, 0x04,
    0xFF, 0x0E, 0x7B, 0x06, 0xFB, 0x84, 0x57, 0x00, 0x6F, 0x08, 0x59, 0x00, 0x07, 0x80, 0x09, 0x00,
    0x03, 0x74, 0x00, 0x04, 0xFF, 0x0F, 0x7B, 0x05, 0xFB, 0x83, 0x57, 0x00, 0x6F, 0x08, 0x59, 0x00,
    0x07, 0x80, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2E, 0x90,
};
#else

BYTE code  EDID[256] =       //Port B  EDID
{
#if !ENABLE_FREESYNC
0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x3E, 0x93, 0x08, 0xC0, 0x00, 0x00, 0x00, 0x00,
0x20, 0x15, 0x01, 0x04, 0xB5, 0x33, 0x1D, 0x78, 0x22, 0x1E, 0x55, 0xA0, 0x59, 0x56, 0x9F, 0x27,
0x0D, 0x50, 0x54, 0xBD, 0x4B, 0x00, 0xD1, 0xC0, 0x81, 0x80, 0x95, 0x0F, 0x95, 0x00, 0xB3, 0x00,
0x81, 0xC0, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
0x45, 0x00, 0xFD, 0x1E, 0x11, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x0A, 0x20, 0x20,
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x56,
0x53, 0x32, 0x33, 0x30, 0x50, 0x47, 0x4C, 0x57, 0x0A, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFD,
0x00, 0x38, 0x4C, 0x1E, 0x53, 0x11, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x5E,
0x02, 0x03, 0x0C, 0xC1, 0x23, 0x09, 0x17, 0x07, 0x83, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60
#else   //freesync EDID
0x00    ,0xFF   ,0xFF   ,0xFF   ,0xFF   ,0xFF   ,0xFF   ,0x00   ,0x4C   ,0x2D   ,0xCC   ,0x0B   ,0x00   ,0x00   ,0x00   ,0x00,
0x0D    ,0x18   ,0x01   ,0x04   ,0x80   ,0x47   ,0x28   ,0x78   ,0x3B   ,0xE6   ,0x91   ,0xA7   ,0x55   ,0x4E   ,0xA0   ,0x25,
0x0C    ,0x50   ,0x54   ,0xBF   ,0xEF   ,0x80   ,0x71   ,0x4F   ,0x81   ,0x00   ,0x81   ,0xC0   ,0x81   ,0x80   ,0xA9   ,0xC0,
0xB3    ,0x00   ,0x95   ,0x00   ,0x01   ,0x01   ,0x83   ,0x48   ,0x80   ,0x18   ,0x71   ,0x38   ,0x2D   ,0x40   ,0x58   ,0x2C,
0x45    ,0x00   ,0xFD   ,0x1E   ,0x11   ,0x00   ,0x00   ,0x1E   ,0xF2   ,0x12   ,0x80   ,0x18   ,0x71   ,0x38   ,0x2D   ,0x40,
0x58    ,0x2C   ,0x45   ,0x00   ,0xFD   ,0x1E   ,0x11   ,0x00   ,0x00   ,0x1E   ,0x00   ,0x00   ,0x00   ,0xFD   ,0x00   ,0x31,
0x4B    ,0x70   ,0x70   ,0x12   ,0x01   ,0x0A   ,0x20   ,0x20   ,0x20   ,0x20   ,0x20   ,0x20   ,0x00   ,0x00   ,0x00   ,0xFC,
0x00    ,0x55   ,0x32   ,0x38   ,0x45   ,0x35   ,0x39   ,0x30   ,0x0A   ,0x20   ,0x20   ,0x20   ,0x20   ,0x20   ,0x01   ,0x80,
0x02    ,0x03   ,0x0E   ,0xF2   ,0x41   ,0x10   ,0x23   ,0x09   ,0x07   ,0x07   ,0x83   ,0x01   ,0x00   ,0x00   ,0x56   ,0x5E,
0x00    ,0xA0   ,0xA0   ,0xA0   ,0x29   ,0x50   ,0x30   ,0x20   ,0x35   ,0x00   ,0xC4   ,0x8E   ,0x21   ,0x00   ,0x00   ,0x1A,
0x02    ,0x3A   ,0x80   ,0x18   ,0x71   ,0x38   ,0x2D   ,0x40   ,0x58   ,0x2C   ,0x45   ,0x00   ,0xC4   ,0x8E   ,0x21   ,0x00,
0x00    ,0x1E   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00,
0x00    ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00,
0x00    ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00,
0x00    ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00,
0x00    ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x00   ,0x89,
#endif
};
#endif




#if DP_USE_INTERNAL_HDCP
BYTE code DPCPBKSV[5] =
{
    0x89, 0x38, 0xae, 0x0d, 0xed
};

//////////////////////////////////////////////////////////////
// HDCP production key
//////////////////////////////////////////////////////////////
BYTE code DPCPkey[284] =
{
    0x00, 0xE6, 0xD2, 0xD0, 0xA4, 0x0A, 0xF9, 0xAB, 0x2F, 0x9A, 0x2D, 0x3A,
    0xB5, 0xFF, 0x9D, 0x37, 0xB3, 0x63, 0x23, 0x6E, 0xA3, 0x4E, 0x58, 0xEF,
    0xD2, 0x78, 0x6D, 0x3B, 0xDE, 0xFD, 0x7E, 0x4D, 0xAA, 0xB6, 0x0C, 0x59,
    0x4F, 0x74, 0x3C, 0xF8, 0xDA, 0x65, 0x6E, 0x2F, 0xB4, 0xAB, 0xAD, 0x7F,
    0x89, 0xA1, 0x92, 0x25, 0x1A, 0xCD, 0x50, 0x93, 0x50, 0x00, 0x63, 0x88,
    0x7B, 0xD0, 0xD8, 0x4D, 0x46, 0x95, 0xA8, 0xFC, 0x9B, 0x0D, 0xD1, 0x08,
    0x06, 0xB2, 0x67, 0xBE, 0xA2, 0x90, 0xA7, 0x84, 0x2A, 0x4B, 0x28, 0xC8,
    0xDD, 0x8F, 0xC9, 0x48, 0xA3, 0x0A, 0x7D, 0xCA, 0xDA, 0xBC, 0xCE, 0xFA,
    0x9B, 0x9F, 0x61, 0x90, 0x9E, 0x46, 0x24, 0x2B, 0xA6, 0x7B, 0xB1, 0x6F,
    0xC4, 0x4C, 0x63, 0xAF, 0x44, 0xE1, 0x3A, 0xDD, 0x0D, 0xED, 0xF4, 0x45,
    0x36, 0x47, 0x0B, 0x48, 0x0A, 0xB1, 0xE6, 0xE9, 0xB6, 0xED, 0x78, 0x37,
    0xDC, 0xB0, 0x07, 0x72, 0x83, 0xDC, 0x73, 0x26, 0x31, 0x49, 0x03, 0x52,
    0x7C, 0xE6, 0xC6, 0xE8, 0x3C, 0xD1, 0x62, 0xFE, 0xF0, 0x01, 0x73, 0x23,
    0x90, 0x70, 0xA9, 0x52, 0x8C, 0x75, 0x1C, 0xE5, 0x63, 0x7C, 0xA9, 0x98,
    0x5F, 0xD9, 0x7C, 0x62, 0x25, 0x6C, 0x61, 0x6C, 0xB5, 0xF9, 0xDB, 0xD8,
    0x91, 0x2F, 0x25, 0xF6, 0x3E, 0x37, 0xBE, 0xE2, 0x92, 0x08, 0x07, 0x3C,
    0xE6, 0xC9, 0xE3, 0xB0, 0x4D, 0xF6, 0xAD, 0x54, 0x3C, 0xED, 0x92, 0x5A,
    0x97, 0x93, 0xC0, 0x48, 0xEA, 0x23, 0x6C, 0xDD, 0x9A, 0x00, 0xBA, 0xF1,
    0xB4, 0xE7, 0x7E, 0x84, 0x9A, 0xB5, 0xD5, 0xF4, 0xB0, 0x42, 0x49, 0x5E,
    0x15, 0x5A, 0x43, 0x2D, 0xA9, 0xB0, 0x0A, 0x1C, 0x75, 0xDE, 0x2E, 0x08,
    0xF7, 0xDF, 0x38, 0x03, 0xEB, 0x6A, 0x75, 0xEB, 0xDA, 0x1C, 0xA0, 0xC9,
    0x2F, 0x2A, 0x69, 0x3F, 0x13, 0xFB, 0xA4, 0x23, 0x0A, 0xE5, 0x0A, 0xA5,
    0xEF, 0xCC, 0x8D, 0xB6, 0x5E, 0x6C, 0xB6, 0xE8, 0x84, 0xEF, 0x03, 0xDC,
    0xC5, 0x8A, 0xA3, 0x4B, 0xEF, 0x44, 0x12, 0x7F
};
#endif

//**************************************************************************
//  [Function Name]:
//                  DPRxInit
//  [Description]
//                  DPRxInit
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxInit( void )
{
	DP_printMsg("=====DPRxInit  ========2=====");
    DPRxEFuseInit();
    DPRxInitVariable();
    DPRxAuxInit();
    DPRxProgramDPCDInit();
    DPRxPHYInit();
    DPRxMACInit();
    DPRxAudioInit();
    #if DP_AUTOEQ
    DPRxAutoEQInit();
    #endif
    DPRxOutputEnable( TRUE );
    DPRxEDIDLoad();
    #if !ENABLE_HDCP
    DPRxHDCPLoadKey();
    #endif
  //  DPRxIRQEnable( TRUE );
   // DPRxAuxIRQEnable( TRUE );
    DP_EnableSquelch( TRUE );
    DPRxFastTrainingInitial();
    DPSetOffLine();
	#if ENABLE_FREESYNC
		DPRxEnableDRRFunction(UserprefFreeSyncMode);
	#endif
    DPRxHPDEnable();
    DPRxPower5VEnable(TRUE);
}
//**************************************************************************
//  [Function Name]:
//                  DPRxInit_PM
//  [Description]
//                  DPRxInit_PM
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxInit_PM( void )
{
    BYTE xdata i=0;

    DP_printMsg("=====DPRxInit_PM  =============");
    DPRxPHYInit();
    DPRxMACInit();
    DPRxAudioInit();
    DPRxAutoEQInit();
    DPRxOutputEnable( TRUE );
    //DPRxEDIDLoad();
    #if !ENABLE_HDCP
    DPRxHDCPLoadKey();
    #endif
   // DPRxIRQEnable( TRUE );
    DPRxAuxIRQEnable( TRUE );
    DP_EnableSquelch( TRUE );
    DPRxFastTrainingInitial();
    DPRxAsynReset();
}
//**************************************************************************
//  [Function Name]:
//                  DPRxAutoEQInit
//  [Description]
//                  DPRxAutoEQInit
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxAutoEQInit()
{

       DP_printMsg("=====DPRxAutoEQInit  =============");
       msWriteByteMask(REG_1F3D,0,BIT7);
// Coarse Tune
    msWrite2Byte(REG_1B52,0x0004);
    msWriteByte(REG_1644,0x50);
    msWriteByteMask(REG_1F20,0x07,0x1F);
    msWriteByteMask(REG_1F21,0x1F,0x1F);
    msWriteByteMask(REG_1B51,0x30,0x70);
    msWriteByteMask(REG_1F22,0,BIT0);
	msWriteByteMask(REG_1B67,0x07,0x3F);        //auto eq  init value
	//msWriteByteMask(REG_1B67,0x03,0x3F);        //auto eq init value
    msWriteByteMask(REG_1B2B,0,BIT6|BIT7);
    msWriteByteMask(REG_1B4D,BIT7,BIT7);
    msWriteByteMask(REG_1B5E,0x20,0x3F);
// Fine Tune
    msWriteByteMask(REG_1F23,0,0x3F);
    msWriteByteMask(REG_1B50,BIT7,BIT7);
    msWrite2Byte(REG_1B56,0x0007);
    msWrite2Byte(REG_1B58,0x0002);
    msWriteByteMask(REG_1B4F,0x05,0x3F);
    msWriteByteMask(REG_1B51,0x03,BIT0|BIT1);
    msWriteByteMask(REG_1F93,0,BIT1);

    msWriteByteMask(REG_1FA2,0,0x3F);

    msWriteByteMask(REG_1FA3,BIT2,0x07);

    msWriteByteMask(REG_1B4D,0x0a,0x3F);
    msWriteByteMask(REG_1B51,BIT2,BIT2);
    msWrite2Byte(REG_1B5A,0x0028);
    msWriteByteMask(REG_1B50,0,0x7F);
    msWriteByteMask(REG_1F22,BIT4,BIT4);
    msWriteByteMask(REG_1FA0,0,0x1F);
    msWriteByteMask(REG_1FA0,0,BIT7);
// CR lose/lock threshold for AEQ
//	msWriteByteMask(REG_1F66,BIT0,BIT0);  //[0] = 0  base on Atop lose lock (1B90/91 ),   [0] =1 base on Digital AEQ lose lock  (1F64/65 )
	msWriteByteMask(REG_1F66,0,BIT0); 
    msWriteByte(REG_1F64,0x05);				//lock
    msWriteByte(REG_1F65,0x09);				//Lose

// choice eq result 0 : min 1: original 2: mean 3:max
    msWriteByteMask(REG_1B51,BIT1,BIT0|BIT1);

msWriteByteMask(REG_1B4A,BIT2,BIT2);
msWriteByteMask(REG_164A,BIT2,BIT2);
msWriteByteMask(REG_1411,BIT6,BIT6);

//////////////////////
//        MAC           //
//////////////////////
    msWriteByte(REG_1666,0x0F);
    msWriteByte(REG_1667,0x28);

    msWriteByte(REG_1668,0x10);
    msWriteByte(REG_1669,0x10);

    msWriteByteMask(REG_166A,BIT6,BIT7|BIT6);

    msWriteByte(REG_1660,0x91);
    msWriteByteMask(REG_1619,BIT6,BIT6);

    msWrite2Byte(REG_1688,0x3210);

    msWrite2Byte(REG_168A,0x6654);

    msWrite2Byte(REG_168C,0x9998);

    msWriteByte(REG_1684,0x26);
    msWriteByte(REG_1685,0x0a);

    msWrite2Byte(REG_1686,0x0000);

    msWriteByte(REG_1682,0x00);
    msWriteByteMask(REG_1683,0,0x0F);
#if 0
// New 1 pre-emphasis AEQ with SW adjust
    msWrite2Byte(REG_1680,0xe744);
    msWrite2Byte(REG_1694,0x1001);


// New 2 pre-emphasis AEQ with SW adjust
    msWrite2Byte(REG_1680,0xF744);
    msWrite2Byte(REG_1694,0x1000);

// New 3 pre-emphasis AEQ with SW adjust
    msWrite2Byte(REG_1680,0xE744);
    msWrite2Byte(REG_1694,0x1000);

// New 1 pre-emphasis AEQ without SW adjust
    msWrite2Byte(REG_1680,0xE740);
    msWrite2Byte(REG_1694,0x1001);

// New 2 pre-emphasis AEQ without SW adjust
    msWrite2Byte(REG_1680,0xF740);
    msWrite2Byte(REG_1694,0xa1000);

// New 3 pre-emphasis AEQ without SW adjust
    msWrite2Byte(REG_1680,0xe740);
    msWrite2Byte(REG_1694,0x1000);
#endif
#if 1   //MP used
// New 2 pre-emphasis AEQ without SW adjust
    msWrite2Byte(REG_1680,0xB740);
    msWrite2Byte(REG_1694,0x1000);

#else
    // New 1 pre-emphasis AEQ without SW adjust
    msWrite2Byte(REG_1680,0xA740);
    msWrite2Byte(REG_1694,0x1001);
#endif

    msWriteByteMask(REG_1850,BIT3,BIT3|BIT2);
}
//**************************************************************************
//  [Function Name]:
//                  DPRxAuxInit
//  [Description]
//                  DPRxAuxInit
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxAuxInit(void)
{
	   WORD temp;
  	   temp = msRead2Byte(REG_1EF0);
    	   msWrite2Byte(REG_1EF0,0xC83E);
   	   gDPInfo.ucDPStates=msReadByte(REG_1EF4);
	   msWrite2Byte(REG_1EF0,temp);	

		#if 0
        #if 1//ENABLE_XTAL_LESS
           msWriteByteMask( REG_1A90, 0, BIT1|BIT2 );                     // Combo AUX
           msWriteByteMask( REG_0192, 0, BIT2|BIT3);                      // Aux Clock SRC : FRO
           msWriteByte( REG_0612, 0x0E );                                                     // Aux UI Count
        msWriteByte( REG_0613, 0x0B );                                                         // Aux Sample Rate
        msWriteByte( REG_0614, 0x08 );                                                        // Aux Time Out Low
        msWriteByte( REG_0615, 0x07 );                                                    // Aux Time Out  High
		DP_printMsg( " ====Aux=FRO=====  " );
           #else
           msWriteByteMask( REG_1A90, BIT1,BIT1|BIT2 );                       // Combo AUX
        msWriteByteMask( REG_0192, BIT2, BIT2|BIT3);                      // Aux Clock SRC : Live clk
        msWriteByte( REG_0612, 0x10 );                                                    // Aux UI Count
        msWriteByte( REG_0613, 0x0D );                                                         // Aux Sample Rate
        msWriteByte( REG_0614, 0x63 );                                                        // Aux Time Out Low
        msWriteByte( REG_0615, 0x08 );                                                    // Aux Time Out  High
		DP_printMsg( " ====Aux=Crysatl=====  " );
 	       #endif
 	       #endif

		#if ENABLE_XTAL_LESS		
 	       DPRxSetAuxClock(FRO);
		#else
			#if DP_AUX_NORMAL_FRO
			 DPRxSetAuxClock(FRO);
			#else
			 DPRxSetAuxClock(Xstal);
           #endif
		#endif
		
	#if (CHIP_ID == CHIP_TSUMF)
        msWriteByteMask( REG_06FA, 0, BIT3|BIT2);       // AUX 
	#endif
		
           msWriteByteMask( REG_06FC, BIT3, BIT3);                        //Mask EQ whenTP1
        msWriteByte( REG_0600, 0x11);                                     // DP11
        msWriteByte( REG_0601, 0x0A );                                    // cyc : 2.7G
        msWriteByteMask( REG_0603, 0x89,0xFF&(~BIT6));                                     // Not supprot TP3
        msWriteByteMask( REG_0615, 0, BIT5|BIT6);                                     // Aux Clk Src
        msWriteByte( REG_062C, 0x0E );                                                    // Aux UI Count
        msWriteByte( REG_062D, 0x0B );                                                    // Aux Sample Rate
        msWriteByte( REG_062E, 0x00 );                                                        // Aux Time Out Low
        msWriteByte( REG_062F, 0x08 );                                                        // Aux Time Out  High
        msWriteByte( REG_0630, 0x00);                                                         // Aux Reply BY HW
        msWriteByteMask( REG_0631, 0, BIT0|BIT1);                                     // Aux Time out Reply
        msWriteByte( REG_0632, 0xD0);                                                     // Aux EDID address
        msWriteByteMask( REG_0642, BIT0,BIT1);                                        // GUID EN
        msWrite2Byte( REG_06A8, 0x1044 );                                                 // ENABLE TX
        msWrite2Byte( REG_06C2, 0x0000 );                                                   // Off-line EN = 0
        msWrite2Byte( REG_06C0, 0x2020 );                                                   // SQ Debunce
        msWriteByteMask( REG_0642, BIT0, BIT0 );                                        // Enable GUID Read/Write
        msWriteByteMask( REG_069C, BIT7|BIT4, BIT4|BIT5|BIT6|BIT7 );     // V oltage Swim = 2 and PRE-emph =1
        msWriteByteMask( REG_069A, BIT1, BIT1 );                                       // MCCS reply By MCU
        msWrite2Byte(REG_06C2 , 0x0000 );                                                  // Off - line =0
        //==============================================================================
        msWriteByte( REG_0679, 0xFF );                                           // Clear Interrupt States
        msWriteByte( REG_0679, 0x00 );

        msWriteByte( REG_0692, 0x0F );                                           // Clear dpcd0_irq_mask Interrupt States
        msWriteByte( REG_0692, 0x00 );

        msWriteByteMask( REG_0693, BIT7, BIT7);                           // Clear __aksv_irq_mask
        msWriteByteMask( REG_0693, 0, BIT7);

        msWriteByteMask( REG_0696, BIT2, BIT2);                           // Clear_SQ
        msWriteByteMask( REG_0696, 0, BIT2);
           msWriteByteMask( REG_06C2, 0, BIT5);                               //Enable Squelch Type ( Real time)
        // ##############################################
        // ##                                        BANK 01 Setting                            ##
        // ##############################################
        msWriteByteMask( REG_01C9, BIT3, BIT3 );                                                   //enable dp setting for pm
        // if(((DPNUM >=0x04)&&(DPLEG==0x61))||((DPNUM >=0x02)&&(DPLEG==0x69)))
        msWriteByteMask( REG_0181, BIT3, BIT3 );
}
//**************************************************************************
//  [Function Name]:
//                  DPRProgramDPCDInit
//  [Description]
//                  DPRProgramDPCDInit
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxProgramDPCDInit(void)
{
        msWrite2Byte( REG_1418, 0xFFFF );                    //MASK
        msWriteByte( REG_1509, 0xFF );                    //MASK
        msWriteByteMask(REG_153C , BIT1 ,BIT1);
        //REG_1758 ~ 1759
        msWrite2Byte( REG_175A, 0xFFFF );                              // clear INT
        msWrite2Byte( REG_175A, 0x0000);                               // clear INT
        msWrite2Byte( REG_175B, 0xFFFF );                              // clear INT
        msWrite2Byte( REG_175B, 0x0000);                               // clear INT
        msWrite2Byte( REG_141C, 0xFFFF );                              // clear INT
        msWrite2Byte( REG_141C, 0x0000);                               // clear INT
        msWriteByte( REG_1539, 0x3F );                                 // clear INT
        msWriteByteMask( REG_153C, BIT0,BIT0);                     // clear INT
        msWriteByteMask(REG_175B , BIT2 ,BIT2 );                   // Clear CDR Loss lock
        msWriteByteMask(REG_175B , 0       ,BIT2 );                   // Clear CDR Loss lock
        msWriteByteMask(REG_175B , BIT1 ,BIT1 );                    // Clear Interskew Lose
        msWriteByteMask(REG_175B , 0       ,BIT1 );                   // Clear Interskew Lose

    //====================
    //          1
    //====================
        //msWriteByteMask( REG_067D, BIT2, BIT2 );      //Enable Program_0
            //  msWrite2Byte( REG_0680,0x0270);             //Address
                //msWriteByte( REG_0682,0x00);
        //msWriteByteMask( REG_0691, 0, BIT4 );         //Unmask

	//===============================================
	//          Port BHDCP Int
	//===============================================
	msWriteByteMask( REG_067D, BIT2, 0x3C );                             //   Disable DPCD custom address 0.
	msWriteByteMask( REG_0691, 0xF0, 0xF0);                              //  Unmask  HDCP Mask INT            <=======Int Maxk
	msWriteByte( REG_0680,0x07);                                                //  HDCP AKSV Address
	msWriteByte( REG_0681,0x80);                                                //  HDCP AKSV Address
	msWriteByte( REG_0682,0x06);                                                //  HDCP AKSV Address
    
}
//**************************************************************************
//  [Function Name]:
//                  DPRxPHYInit
//  [Description]
//                  DPRxPHYInit
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxPHYInit(void)
{
    //  ##############################################
    // ##                                        BANK 1a  Setting                              ##
    // ##############################################
    msWrite2Byte( REG_1A70, 0xFFFF );                  //  PLL clkgen enable
    msWriteByte(REG_1A72,0xFF);

    msWrite2Byte( REG_1A86, 0xFFFF );                  //  MISC clkgen enable
    msWrite2Byte( REG_1A88, 0xFFFF);

    msWrite2Byte( REG_1A92, 0xFFFF );                  //  Clkgen clkgen enable
    msWrite2Byte( REG_1A94, 0xFFFF);

    msWriteByteMask( REG_1A63, 0,BIT0 );            //  VPLL PD
    msWriteByteMask( REG_1A06, 0,BIT0 );                // APLL DIV
    msWriteByteMask( REG_1A07, 0,BIT0 );                // APLL PD

    msWriteByteMask( REG_1A11, BIT7,BIT7 );             // APLL bypass mode
    msWrite2Byte( REG_1AE8, 0x404D );
    msWrite2Byte( REG_1AC4, 0xFFFF );              //zero cross
    msWriteByte( REG_1AD5, 0xC0 );                 //audio fading gain
       msWriteByteMask( REG_1A84, BIT2|BIT1,BIT2|BIT1 );            //AVmute
    msWriteByteMask( REG_1A62, 0,BIT0 );            //
    msWriteByteMask( REG_1AA6, BIT0,BIT0 );            // For Interrupt

    msWriteByteMask( REG_1A2A, BIT4,BIT7|BIT4 );                //  Freq_Ctrol _Select from New mode

    msWrite2Byte( REG_1AEE, 0x100);      				   // The period for checking audio sample in
   	
    //  ##############################################
    // ##                                        BANK 1B  Setting                              ##
    // ##############################################
    msWriteByteMask( REG_1BD7, 0,BIT0 );            //reg_atop_pd_bg
    msWriteByteMask( REG_1BD6, BIT3 ,BIT3|BIT2|BIT1 );      //reg_atop_pd_clkin
    msWriteByteMask( REG_1BB4,0,BIT2);
    msWriteByteMask( REG_1BB6, 0 , 0x0F );          //Power Down Lane
    msWriteByteMask( REG_1BB2, 0 , BIT1 );          //reg_atop_ove2[1]
    msWriteByteMask( REG_1BD7, 0,0xF0 );                //reg_atop_pd_phdac_ov
    msWriteByteMask( REG_1BF4, 0,0x0F);             //reg_atop_pd_pll
    msWriteByteMask( REG_1B00, 0,BIT7);             //DP mode
#if DP_AUTOEQ
    msWriteByteMask( REG_1B68, BIT0|BIT1,BIT0|BIT1);            //auto eq time out
#endif
    msWriteByte( REG_1B88, 0x6A );

#if ENABLE_XTAL_LESS   // 12Mhz
    msWrite2Byte( REG_1B84, 0x2D0 );                    // HBR Lock
    msWrite2Byte( REG_1B86, 0x2D2 );                    // RBR Lock
	msWrite2Byte( REG_1B90, 0x200F); 				// threshold
    #if (CHIP_ID == CHIP_TSUMF)
    msWrite2Byte( REG_1B90, 0x0903); 				// threshold
    #endif
#else           //14.318
    msWrite2Byte( REG_1B84, 0x25B );                    // HBR Lock
    msWrite2Byte( REG_1B86, 0x25D );                    // RBR Lock
    msWrite2Byte( REG_1B90, 0x1006);                // threshold
#endif

	msWriteByteMask( REG_1BBE, BIT1|BIT0,BIT1|BIT0);			//Afifo safe Zone Enable1 and Enable 6/7

	#if (CHIP_ID == CHIP_TSUMF)
	#if ENABLE_XTAL_LESS  
	msWriteByteMask( REG_1BBE, BIT4,BIT4);					      //Auto synth ECO  CCJ)
	#endif
	#endif
    //  ##############################################
    // ##                                        BANK 1F  Setting                              ##
    // ##############################################
    msWriteByteMask( REG_1FBA, 0,BIT7 );            //reg_pd_adata
    msWriteByteMask( REG_1FB2, 0,BIT4|BIT5 );       //reg_pd_cal_ov_en  //reg_pd_cal_ov
    msWriteByteMask( REG_1F3F, 0,BIT6 );                //
       msWriteByteMask( REG_1FB7, 0 ,BIT0);				//r
 
	msWriteByteMask( REG_1F93, BIT0 ,BIT0);							//reg_cr_train_en
	msWriteByteMask( REG_1FA3, BIT4|BIT5 ,BIT4|BIT5);				//Synth_Auto_En / SEL

	//20150514
	#if 0
	msWriteByte( REG_1B62, 0x1F); 				      // DiV
	msWrite2Byte( REG_1B84, 0x168 );					// HBR Lock
	msWriteByte( REG_1B5D, 0x3E);    	  // CDR lock throhs  1cnt 10u
	#else 
	//20150518  
	 msWriteByte( REG_1B5D, 0x10);    	 			           // CDR lock throhs  1cnt 10u
        msWriteByte( REG_1FAF, 0x1B); 			                 // 0.002  STEP
        msWriteByte( REG_1FB0, 0x0D); 			                 // 0.002  STEP
        msWriteByte( REG_1FB1, 0x00);   				          // 0.002  STEP
	 msWrite2Byte( REG_1FA4, 0xF8);       				   //Time out      
	#endif
}


//**************************************************************************
//  [Function Name]:
//                  DPRxMACInit
//  [Description]
//                  DPRxMACInit
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxMACInit(void)
{

		// ##############################################
		// ##                                        BANK 15  Setting                            ##
		// ##############################################
    msWriteByteMask( REG_1509, 0x3F,0x3F );  // mask INT
    msWriteByteMask( REG_153C, BIT1,BIT1 );  // mask INT
    msWriteByteMask( REG_1506, BIT2,BIT2 );  //  R0 Selecct

    // ##############################################
    // ##                                        BANK 17  Setting                            ##
    // ##############################################
    msWriteByteMask( REG_1740, 0x00 , BIT0);        // Select DP.
    msWriteByte( REG_175A, 0xFF );  // clear INT
    msWriteByte( REG_175B, 0xFF );  // clear INT
    msWriteByte( REG_175A, 0x00 );  // clear INT
    msWriteByte( REG_175B, 0x00);  // clear INT

    msWriteByte( REG_175C, 0xFF );  // mask INT
    msWriteByte( REG_175D, 0xFF );  // mask INT
    msWriteByteMask( REG_1760, 0 , BIT2|BIT3);   //0112 //
    msWriteByteMask( REG_1752, BIT2, BIT2 );  // dvi_decoder_sw_reset
    msWriteByteMask( REG_17AB, BIT0, BIT0 );  //  reg_en_ft_eq_done_sw
    // ##############################################
    // ##                                        BANK 13  Setting                           ##
    // ##############################################
    msWriteByteMask( REG_1303, 0 , BIT4);                               //For CTS
    msWriteByte( REG_1320, 0x71 );
    msWriteByteMask( REG_1322, 0x60 , 0x60);
    msWriteByteMask( REG_130A, 0x06 , 0x06 );
    msWrite2ByteMask( REG_138C, 0x3B30 ,0x3B30&(~BIT5));
     msWriteByteMask( REG_1325, BIT5 , BIT5 );                          // Auto Clear Vsyn re-gen state FSM (2030215)
    msWriteByteMask( REG_1326, BIT7|BIT5, BIT7|BIT5);							//FST reset HTT  , HTT by BE 20170104
    msWriteByteMask( REG_1375, BIT6 , BIT6); 							// 1366 CRC
    // ##############################################
    // ##                                        BANK 14  Setting                            ##
    // ##############################################

    msWriteByteMask( REG_142A, 0x14 , 0x14);                               //[4]: AVG FIFO enable  //[2]: AVG FIFO bypass
    msWriteByte( REG_1418, 0xFF );                                      // mask INT
    msWriteByteMask( REG_1419, 0xFF,0xFF );  // mask INT
    msWriteByteMask( REG_146C, BIT2|BIT3|BIT4, BIT2|BIT3|BIT4);
   // msWrite2Byte( REG_1418, 0xFFF );  // SDP enable

    msWriteByteMask(REG_1410,BIT0,BIT0);                                                 //Set Video M/N
    msWriteByteMask( REG_1471, BIT4|BIT5, BIT4|BIT5);                              //A/V Big change update to syn

     msWriteByte( REG_1469, 0x00);         	    //Attribute Data Change mode mask	
     msWriteByteMask( REG_1411, BIT5,BIT5);    // MSA New Mode      

     msWriteByte( REG_1429, 0x7F);  	  // Audio Mute Function.	
    
    msWriteByte( REG_1473, 0x07);           //use VBID Config
    msWriteByte( REG_1474, 0x61);           //use VBID Config
    msWriteByte( REG_1475, 0x0F);           //use VBID Config

    msWriteByte( REG_1448, 0x68 );           //MVID_CENTER[7:0]
    msWriteByte( REG_1449, 0x46 );           //MVID_CENTER[15:8]
    msWriteByte( REG_144A, 0x00 );           //MVID_CENTER[23:16]

    msWriteByte( REG_144C, 0x10 );           //MVID_Range[7:0]
    msWriteByte( REG_144D, 0x00 );           //MVID_Range[15:8]
    msWriteByte( REG_144E, 0x00 );           //MVID_Range[23:16]

    msWriteByte( REG_1450, 0x00 );           //NVID_CENTER[7:0]
    msWriteByte( REG_1451, 0x80 );           //NVID_CENTER[15:8]
    msWriteByte( REG_1452, 0x00 );           //NVID_CENTER[23:16]

    msWriteByte( REG_1454, 0x10 );           //NVID_Range[7:0]
    msWriteByte( REG_1455, 0x00 );           //NVID_Range[15:8]
    msWriteByte( REG_1456, 0x00 );           //NVID_Range[23:16]

    msWriteByte( REG_1458, 0x00 );           //MAUD_CENTER[7:0]
    msWriteByte( REG_1459, 0x40 );           //MAUD_CENTER[15:8]
    msWriteByte( REG_145A, 0x00 );           //MAUD_CENTER[23:16]

    msWriteByte( REG_145C, 0x10 );           //MAUD_Range[7:0]
    msWriteByte( REG_145D, 0x00 );           //MAUD_Range[15:8]
    msWriteByte( REG_145E, 0x00 );           //MAUD_Range[23:16]

    msWriteByte( REG_1460, 0x00 );           //NAUD_CENTER[7:0]
    msWriteByte( REG_1461, 0x80 );           //NAUD_CENTER[15:8]
    msWriteByte( REG_1462, 0x00 );           //NAUD_CENTER[23:16]

    msWriteByte( REG_1465, 0x10 );           //NAUD_Range[7:0]
    msWriteByte( REG_1466, 0x00 );           //NAUD_Range[15:8]
    msWriteByte( REG_1467, 0x00 );           //NAUD_Range[23:16]
    // ##############################################
    // ##                                        BANK 18 Setting                            ##
    // ##############################################
     msWriteByte( REG_1800, 0x03);                                                                // [0]local DPCD 1.2 En, [1] CP_ready
     msWriteByte( REG_1802, 0x00);                                                                // Set Level 0
     msWriteByte( REG_1803, 0x00);                                                                // Set Level 0
     msWriteByteMask( REG_18E4, 0 , BIT3 );                                                  //  Enable SQ check in fast training
     #if ENABLE_XTAL_LESS
     msWriteByte( REG_18E6,0x0C );                                        //  12MHz  Xtal clk for Fast train timer
     #else
     msWriteByte( REG_18E6,0x0E );                                                               //  14Mhz   Xtal clk for Fast train timer
     #endif

     #if DP_AUTOCR
      msWriteByteMask( REG_1800,0x10,0x10 );                              // Auto CR
      msWrite2Byte( REG_18FA,0x3c04 );                                    // [15:4]: reg_cr_done_mask_time
      #endif
    // ##############################################
    // ##                                        BANK 16 Setting                            ##
    // ##############################################
    msWrite2Byte( REG_161C, 0x0F00 );  // wb det TH
    msWrite2Byte( REG_1620, 0x0F00 );  // wb det TH
       msWrite2Byte( REG_1624, 0x0F00 );  // wb det TH
       msWrite2Byte( REG_1628, 0x0F00 );  // wb det TH
       msWriteByte( REG_1648, 0xAA );       // //Max Swing, only [1:0] useful
    msWrite2Byte( REG_1666, 0x203C );  //  reg_error_cnt_mask_thd,reg_timer_thd
       msWrite2Byte( REG_1668, 0x1010 );  //[15:8]: reg_eq_ph_err_cnt_th //[7:0]:  reg_eq_center_err_cnt_th
    msWriteByteMask( REG_166A, 0x80, 0xC0 );     //[7:6]: EQ phase number
    msWriteByteMask( REG_166B, 0x04, 0x04 );     //reg_en_req_pre_emph_align


	//================================================
	//
	//================================================
	#if 0
         msWriteByteMask( REG_163B, BIT3, BIT3 );		 //deinterskew setting   CCJ  U2.06
	#else
      	       #if DP_INTERSKEWMORE
		msWriteByteMask( REG_163B, 0, BIT3 );
		msWriteByteMask( REG_1619, BIT3 , BIT3 );        //u 06
		#endif
		msWriteByteMask( REG_1619, BIT4 , BIT4 );        //u 03
	#endif
		msWriteByteMask( REG_1BBE, BIT3 , BIT3);        //u 08
	
    // msWriteByteMask( REG_1649, BIT6|BIT5|BIT3|BIT2, BIT6|BIT5|BIT3|BIT2|BIT1 );
    //msWriteByte( REG_1644, 0x77);                                                                                        //EQ done is equal to symbol lock
    // msWrite2Byte( REG_16C0,0x0168);                                                   //CR lock value for RBR
}

//**************************************************************************
//  [Function Name]:
//                  DPRxAudioInit
//  [Description]
//                  DPRxAudioInit
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxAudioInit( void )
{
    BYTE mode = 1;


       msWriteByte( REG_1AC3, 0xFF);                                   //Mute Event
       msWriteByteMask( REG_1AD0, BIT3, BIT3|BIT2);           //Auto Fading
       msWriteByteMask( REG_1AC0, BIT0, BIT0);
       msWriteByteMask( REG_1AC1, BIT4, BIT4);
       msWriteByteMask( REG_1A22, 0x0A, 0x0F);
       msWriteByteMask( REG_1AE1, BIT7, BIT7);
       msWriteByteMask( REG_1A28, 0, 0x3F);
       msWriteByteMask( REG_1A2A, BIT7, BIT7);			  //192K issue

    if(gDPInfo.ucAudioMode ==1)   // mode 1
    {
        DP_printMsg("=====Mode 1");
        msWriteByteMask( REG_1AE9, BIT6, BIT6 );
        msWriteByteMask( REG_1AE0, 0, BIT7 );
        msWriteByteMask( REG_1AE9, 0, BIT7 );
        msWriteByteMask( REG_1A40, BIT0|BIT1,BIT0|BIT1 );
        msWriteByteMask( REG_1A22, 0, BIT6 );
        msWriteByteMask( REG_1AC1, 0, BIT4);
        msWriteByteMask( REG_1A41, 0, BIT4);
        msWriteByteMask( REG_1AEB, 0, BIT7);
        msWriteByteMask( REG_1A41, 0, BIT3);
        msWriteByteMask( REG_1A28, 0, BIT7);
        msWriteByteMask( REG_1A30, BIT7, BIT7);
        msWriteByteMask( REG_1AE0, 0, BIT7);
        msWriteByteMask( REG_1AE1, 0, BIT7);
        //msWriteByteMask( REG_1A2A, BIT7, BIT7);
    }
    else        //Mode 3
    {
        DP_printMsg("=====Mode 3");
        msWriteByteMask( REG_1AE9, 0, BIT6 );
        msWriteByteMask( REG_1AE0, BIT7, BIT7 );
        msWriteByteMask( REG_1AE9, 0, BIT7 );
        msWriteByteMask( REG_1A40, BIT0|BIT1,BIT0|BIT1 );
        msWriteByteMask( REG_1A22, BIT6, BIT6 );

        msWriteByteMask( REG_1A41, BIT4, BIT4);
        msWriteByteMask( REG_1AEB, 0, BIT7);
        msWriteByteMask( REG_1A41, 0, BIT3);
        msWriteByteMask( REG_1A28, 0, BIT7);
        msWriteByteMask( REG_1A30, BIT7, BIT7);
        msWriteByteMask( REG_1AE0, BIT7, BIT7);

        //msWriteByteMask( REG_1A2A, BIT7, BIT7);


    }
}
//**************************************************************************
//  [Function Name]:
//                  DPRxInitVariable
//  [Description]
//                  DPRxInitVariable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxInitVariable( void )
{
      memset(&gDPInfo, 0, sizeof(gDPInfo));
      #if 1
      gDPInfo.ucAudioMode =0;   // New mode
      #else
      gDPInfo.ucAudioMode =1;   // old mode
      #endif
      gDPInfo.ucDPColorFormat=InputColor_RGB;

      DP_printData("ucAudioMode==>%x" , gDPInfo.ucAudioMode );
}


//**************************************************************************
//  [Function Name]:
//                  DPRxEFuseInit
//  [Description]
//                  DPRxEFuseInit
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxEFuseInit( void )
{
    BYTE bEfuseData =0;
       DP_printMsg( " =====EFuse======  " );
      msRegs[REG_3F89] = 0x10;
      msRegs[REG_3F84] = (0x18 + 0x03);   //ADDRESS=0x03
      msRegs[REG_3F85] = 0x00;
      Delay1ms(1);
      bEfuseData = msRegs[REG_3F86];
      if(bEfuseData&BIT7)
      msRegs[REG_0948] = bEfuseData&0x0F;
      DP_printData( "bEfuseData = %x" ,bEfuseData );
}
//**************************************************************************
//  [Function Name]:
//                  DPRxHDCPLoadKey
//  [Description]
//                  DPRxHDCPLoadKey
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************

#if !ENABLE_HDCP
#define HDCP_XIU  1
void DPRxHDCPLoadKey( void )
{
    WORD xdata  i;

    #if !DP_USE_INTERNAL_HDCP
    BYTE xdata ucValue;
    #endif
    msWriteByteMask( REG_174F,BIT4,BIT4);                      //Trun on the Crystal
#if  !HDCP_XIU
// Use the same HDCP key shared with  HDMI port
    msWriteByte( REG_150C, 0x00);
    msWriteByte( REG_150D, 0x00);
    msWriteByteMask( REG_153A,BIT4, BIT4 );
    msWriteByteMask( REG_153A,0, BIT4 );
    Delay1ms(1);
    msWriteByteMask( REG_150F,BIT6|BIT7, BIT6|BIT7 );      // HDCP_Enable
    msWriteByteMask( REG_1508,BIT5, BIT5 );                       // HDCP_Enable CPU access
    msWriteByte( REG_1500, 0x00 );                                      // HDCP_SRAM_INIT_ADDR[7:0]
    msWriteByte( REG_1501, 0x00);                                       // HDCP_SRAM_INIT_ADDR[9:8]

     for( i = 0; i < 5; i++ )
     {
            msWriteByte( REG_150E, DPCPBKSV[i] );
            msWriteByteMask( REG_153A,BIT3, BIT3 );
            msWriteByte( REG_06A0 + i, DPCPBKSV[i] );           //Off-Line Key
            msWriteByte( REG_07A0 + i, DPCPBKSV[i] );           //Off-LineKey

     }

     for( i = 0; i < 284; i++ )
    {
        msWriteByte( REG_150E, DPCPkey[i] );
        msWriteByteMask( REG_153A,BIT3, BIT3 );

    }
#else  //HDCP XIU Write HDCP key

    msWriteByteMask( REG_150F,BIT6|BIT7, BIT6|BIT7 );  // enable MCU write HDCP key into SRAM
    msWriteByteMask( REG_1508,BIT5, BIT5 );                     // HDCP_Enable CPU access
    msWriteByte( REG_1900, 0x00 );                                      // HDCP__INIT_ADDR[7:0]
    msWriteByteMask( REG_1901,0,BIT0|BIT1);                   // HDCP__INIT_ADDR[9:8]
    for( i = 0; i < 5; i++ )
     {
            msWriteByte( REG_1902, DPCPBKSV[i] );                // On-Line BKSV

            msWriteByte( REG_06A0 + i, DPCPBKSV[i] );           //Off-Line Key
            msWriteByte( REG_07A0 + i, DPCPBKSV[i] );           //Off-LineKey
     //   DP_printData( "%x", msReadByte( REG_18AB+i ) );
        }
     for( i = 0; i < 284; i++ )
    {
        msWriteByte( REG_1902, DPCPkey[i] );                     //On-Line KEY
    }
#endif
  DP_printMsg( "DPRxHDCPLoadKey OK!" );
}
#endif

//**************************************************************************
//  [Function Name]:
//                  DPRxHandle()
//  [Description]
//                  DPRxHandle
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************

void DPRxHandle( void )
{
    BYTE i=0;

        if(msRegs[REG_183E] == 1)
            {
            for(i =0;i<16;i++)
                DP_printData(" DDC =>%x ",DDCData[i]);
                msRegs[REG_183E] =0;
           }

     if(!CURRENT_INPUT_IS_DISPLAYPORT())
     return;

      if( m_ucState==eSTATE_NORMAL)
      {
        DPRXGetPktStatus();
      }

    #if DP_POWER_SAVING
        DPRxPollingDPCDPowerSave();
    #endif

        if( m_ucState!=eSTATE_POWERSAVE)
        {
               DPRxHdcpReAuthen();
               if(!SyncLossState() )
                DPRxCheckInputFormat();
        }

    #if DPCTS
         if(m_ucState!=eSTATE_POWERSAVE)
        {
               DPRxCDRLossHander();                         //CCJ 20110321
        }
    #endif

#if 0//DP_POWER_SAVING
    if( g_bDPDPCDPOWERSAVE )
    {
        if( g_DPDPCDPOWERSAVECNT <= DPDPCDPOWERSAVE_COUNTER )
            g_DPDPCDPOWERSAVECNT++;
        if( g_DPDPCDPOWERSAVECNT == DPDPCDPOWERSAVE_COUNTER )
        {
            if( !g_bDPPowerDown )
                m_ucState = eSTATE_POWERSAVE;

        }
    }
#endif

    if( DPState != m_ucState )
    {
        DPPrintState( m_ucState );
    }

   

    DPState = m_ucState;

    switch( m_ucState )
    {
        case eSTATE_INITIAL:
             m_ucState = eSTATE_STARTUP;
            break;
        case eSTATE_RETRAIN:
            DP_printMsg( "<RETRAIN> " );
            DPRxHPDDisable();                                                                           // HPD ouput enable and set Low
            ForceDelay1ms( 100 );
            DPRxHPDEnable();
             m_ucState = eSTATE_STARTUP;
            break;
        case eSTATE_STARTUP:
            DPRxClrSQTime();
            DPEventEnable(FALSE);
            LockFailCnt = 0;
            PowerSavingCnt = 0;
            DPFindModeCnt = 0;
            m_ucState = eSTATE_CKECKLOCK;
            break;
        case eSTATE_CKECKLOCK:  // Do training & Chk Lock
            DPSetMAudToZero();
            DPRxCheckSQTime(1000);
            if( g_bDPTrainingP0T || g_bDPTrainingP1T || ( DPRxCheckLock() ) )
            {
                        DP_printMsg( "<Normal> " );
                        DPRxFastTraining2Enable( FALSE );
                        if( DPRxCheckLock() )
                        {
                            DPRxMainStrmRst();
                            Clr_DPPACKET_FLAG();
                            DP_printMsg( "<Normal> DP LOCK!" );
                            g_bDPTrainingP0T = FALSE; // clear flag
                            g_bDPTrainingP1T = FALSE; // clear flag
                            TrainingP1Cnt = 0;               // clear TrainingP1Cnt
                            TrainingP2Cnt = 0;               // clear TrainingP2Cnt
                            LockFailCnt = 0;                    // clear ChkLockCnt
                            PowerSavingCnt = 0;
                            DPCheckModeCnt=0;
                            DPFindModeCnt =0;
                            m_ucState = eSTATE_MEASURE;
                        }
                        else
                        {
                            DP_printMsg( "<Normal> DP Un_LOCK!" );
                            Delay1ms( 1 );
                            LockFailCnt++;
                            if( LockFailCnt > 10 ) // ChkLock Fail too much time => HotPlug  worstcase:1msX250= 500ms
                            {
                                LockFailCnt = 0;
                                PowerSavingCnt = 0;
                                g_bDPTrainingP0T=FALSE;  // clear flag
                                g_bDPTrainingP1T=FALSE;  // clear flag

                            }
                        }
            }
        #if 1 //FAST Training
            else if( gDPInfo.bSquelchStable && ( !( g_bDPTrainingP0T || g_bDPTrainingP1T ) )&&( !DPRxCheckLock() ) )
            {
                        if((msRegs[REG_1834]&BIT7)||gDPInfo.bPhyTest)    //Auto Test
                        {
                    return;
                        }
                            DPRxClrSQTime();
                            if(DPFastLink(TRUE))
                            m_ucState = eSTATE_MEASURE;
                else
                            m_ucState = eSTATE_RETRAIN;
            }
       #endif
               break;
        case eSTATE_MEASURE:
               DPRxClrSQTime();
            if(1)
            {
             // DPRXAutoEQCheck();
                    DPFindModeCnt = 0;
                    if( DPRxCheckTiming(MSACnt))
                    {
				
                        DPRxHvFsmChk();
                       // #if DP_RESETHTT_PATCH
              //    DPMSA_Reset();
                       // #endif
                        ForceDelay1ms(1);
                        m_ucState = eSTATE_NORMAL;
                        NoVscPktCnt = 0;
                    }
                    else if(++DPCheckModeCnt > 2)
                    {
                        g_bDPTrainingP0T=FALSE;  // clear flag
                        g_bDPTrainingP1T=FALSE;  // clear flag
                        DPCheckModeCnt =0;
                        m_ucState = eSTATE_CKECKLOCK;
                    }
            }
            else
            {
                    DPCheckModeCnt =0;
                    DPFindModeCnt++;
                    if( DPFindModeCnt > 100 )
                    {
                        DPFindModeCnt = 0;
                        m_ucState = eSTATE_CKECKLOCK;
                    }
            }
            break;
        case eSTATE_NORMAL:
            if( !DPRxCheckLock() )
            {
                ucFreqVaild_Old =0;
                g_bDPTrainingP0T = FALSE; // clear flag
                g_bDPTrainingP1T = FALSE; // clear flag
                m_ucState = eSTATE_CKECKLOCK;
            }
            else
            {
                m_ucState = eSTATE_NORMAL;
                DPRxTiming();
                //DPRx3DInform();
#if SupportDPAudio
                DPRxAudioHandle();
#endif
#if DP_VPLLBIGChange
                DPRxVPLLHandle();
#endif
                DPRxCheckInputFormat();
              }
              #if DP_FastTraining
                if(m_ucState != eSTATE_NORMAL)
                {
                        DPRxClrSQTime();
                }
               #endif
            break;
        case eSTATE_POWERSAVE:
            if( !g_bDPPowerDown )
            {
                      DPRxPower5VEnable(FALSE);
                    DP_printMsg( " DP ---En for Power Saving" );
                    g_bDPPowerDown = TRUE;
                    //DPEventEnable(TRUE);
            };
            TrainingP1Cnt = 0;               // clear TrainingP1Cnt
            TrainingP2Cnt = 0;               // clear TrainingP2Cnt
            g_bDPTrainingP0T = FALSE; // clear flag
            g_bDPTrainingP1T = FALSE; // clear flag
            DPRxClrSQTime();
            m_ucState = eSTATE_DPIDLE;
            break;
        case eSTATE_DPIDLE:
            if( g_bDPAUXVALID )
            {
                DPRxClearAUXVliadStatus();
                m_ucState = eSTATE_STARTUP;
                DPRxOutputEnable( TRUE );
                //msWriteByte(REG_1F5A,0x01);   // overwrite DPCD 600h as POWER ON
                g_bDPDPCDPOWERSAVE = 0;      // clear flag
                g_DPDPCDPOWERSAVECNT = 0;  // clear powersave conuter
                break;
            }
    }
}

//**************************************************************************
//  [Function Name]:
//                  DPRxHvFsmChk(void)
//  [Description]
//                  DPRxHvFsmChk
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxHvFsmChk(void)
{
	BYTE ubHs_gen_fsm_status;
	BYTE ubCheck_fsm_Times;
	
	
		ubHs_gen_fsm_status = 0x0;
		ubCheck_fsm_Times = 0x2;  //RD suggest we should check FSM 2 times
				
		//make sure hs_gen_fsm_status[3:0] only has 1 bit as 1, otherwise reset reg_hs_gen_fsm_rst
		while(ubCheck_fsm_Times > 0) 
		{
			ubHs_gen_fsm_status = (msRead2Byte(REG_1350) >> 7)&0x000F;

			if(((ubHs_gen_fsm_status & (ubHs_gen_fsm_status - 1)) != 0x0) ||
				(ubHs_gen_fsm_status == 0x0)) 
			{

			       msWriteByteMask(REG_130D, BIT0, BIT0);//reg_hs_gen_fsm_rst
				Delay1ms(1);
				msWriteByteMask(REG_130D, 0, BIT0);//reg_hs_gen_fsm_rst
			}
			
		    ubCheck_fsm_Times --;
		}
			

}
//**************************************************************************
//  [Function Name]:
//                  DPRXCheckDPMS(void)
//  [Description]
//                  DPRXCheckDPMS
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL DPRXCheckDPMS(void)
{
    BOOL bIgnoreDPMS = FALSE;

        if(((msReadByte(REG_136B) & 0x08 ) ? 1 : 0)&&DPRxCheckLock()&&(msReadByte(REG_0672 )&0x01))
        {
            bIgnoreDPMS = TRUE;
        }

    return bIgnoreDPMS;
}

//**************************************************************************
//  [Function Name]:
//                  DPRXAutoEQCheck()
//  [Description]
//                  DPRXAutoEQCheck
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
#define DELAY1US()                  _nop_()

void DPRXAutoEQCheck()
{
    BYTE i_loop=0;
    BYTE ucLans=0;

	#if DP_INTERSKEWMORE
	return;
	#endif
	
       ucLans = msReadByte( REG_1881 ) & 0x07;

      switch( ucLans )
     {

        case 0x04:
       if(!((msReadByte(REG_166F)&BIT7)&&((msReadByte(REG_1670)&(BIT0|BIT1)) ==(BIT0|BIT1))))
           {

            #if 1
                msWriteByteMask(REG_18E7, BIT0, BIT0);
                msRegs[REG_1B1C] = BIT0;                              //FIFO reset
                msRegs[REG_1B1C] = 0;                                //FIFO reset
                Delay1ms(1);
                msWriteByteMask(REG_18E7, 0, BIT0);
                DP_printMsg("==============================Patch");
                #endif
            #if 0
                if(msReadByte(REG_163B)&BIT3)
                msWriteByteMask(REG_163B, 0,BIT3);
                else
                msWriteByteMask(REG_163B, BIT3,BIT3);
                DP_printMsg("==============================Patch");

            #endif
         break;
         case 0x02:
          if(!((msReadByte(REG_166F)&BIT7)))
           {
            #if 1
                msWriteByteMask(REG_18E7, BIT0, BIT0);
                msRegs[REG_1B1C] = BIT0;                              //FIFO reset
                msRegs[REG_1B1C] = 0;                                //FIFO reset
                Delay1ms(1);
                msWriteByteMask(REG_18E7, 0, BIT0);
                DP_printMsg("==============================Patch");
                #endif
          }
         break;
         case 0x01:
         default:


         break;
              }
              }

}

//**************************************************************************
//  [Function Name]:
//                  DPRXGetPktStatus()
//  [Description]
//                  DPRXGetPktStatus
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRXGetPktStatus()
{

     if(gDPInfo.bPacketTimer==0)
    {
        gDPInfo.uwPacketStatus = msReadByte(REG_14F9);
        gDPInfo.bPacketTimer = 100;
        DPPacket_Reset();
    }

}
//**************************************************************************
//  [Function Name]:
//                  DPCheckAuxState()
//  [Description]
//                  DPCheckAuxState
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
#define AuxIdle    0x02
BOOL DPCheckAuxState(BYTE Times)
{
     BOOL xdata bStstus = FALSE;
     BYTE xdata Temp=0;

    while( Times-- )
    {
        if(!((( msReadByte( REG_06E8  )&0x38)>>3)==AuxIdle))
        {
            return FALSE;               //  not idle
        }
    }

    return TRUE;

}
//**************************************************************************
//  [Function Name]:
//                  DPRxCheckSQTime()
//  [Description]
//                  DPRxCheckSQTime
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL DPRxCheckSQTime(WORD Times)
{
                   if (DP_GetSquelch(1) )
                    {
                        if (bDPRxStableTimeoutFlag==0 && !gDPInfo.bSquelchStable)
                        {
                            if (!gDPInfo.bSquelchWaitStable)
                            {
                                SetDPRxStableTimeout(Times);
                                //u16DPRxStableTimeout=Times;
                                //bDPRxStableTimeoutFlag=1;
                                gDPInfo.bSquelchWaitStable=1;
                                gDPInfo.bSquelchStable=0;
                            }
                            else
                            {

                                gDPInfo.bSquelchWaitStable=0;
                                gDPInfo.bSquelchStable=1;
                                u16FastTrainingCntDown=0;
                            }
                        }
                        return TRUE;
                    }
                    else
                    {
                        gDPInfo.bSquelchWaitStable=0;
                        gDPInfo.bSquelchStable=0;
                        SetDPRxStableTimeout(0);
                        //u16DPRxStableTimeout=0;
                        //bDPRxStableTimeoutFlag=1;
                        u16FastTrainingCntDown=0;
                        return FALSE;
                    }

}
//**************************************************************************
//  [Function Name]:
//                  DPFastLink()
//  [Description]
//                  DPFastLink
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL DPFastLink(BOOL bSearch)
{
          BYTE xdata i=0,j=0,k=0;
          BYTE xdata CounterChk=0;
	#if 1
          BYTE xdata FastLane[3] ={0x04,0x02,0x01};
          BYTE xdata FastRate[3] = {0x0A,0x06,0x1};
       #else
	BYTE xdata FastRate[3] = {0x0A,0x0A,0xA};
       BYTE xdata FastLane[3] ={0x04,0x04,0x04};
	#endif

	#if (CHIP_ID==CHIP_TSUMF)
        Set_DPAutoSythNewMode();					
   	#endif
	#if ENABLE_XTAL_LESS
	#if (CHIP_ID==CHIP_TSUM9)
	msRegs[REG_1B90] =  0x0F;					   			    // Lock Loss threhold
	msRegs[REG_1B91] =  0x20;
	#elif (CHIP_ID==CHIP_TSUMF)
	msRegs[REG_1B90] =  0x03;					   			    // Lock Loss threhold
	msRegs[REG_1B91] =  0x09;
	#endif
	#endif

	msRegs[REG_1B68] = msRegs[REG_1B68] &(~(BIT1));   			    // AUTO EQ Time Out
	
        #if DP_INTERSKEWMORE
 	msRegs[REG_1619] = msRegs[REG_1619] |BIT3;				    //Deinterskew setting
 	msRegs[REG_163B] = msRegs[REG_163B] &(~BIT3);	  
 	#endif
    #if DP_AUTOEQ
         msWriteByteMask( REG_1660, 0,BIT0);            // Auto EQ off (Mac)
    #endif

           if(bSearch == TRUE)
           {
                                    DP_printMsg( "------>>  <Fast> " );
                                    for(j=0;j<3;j++)
                                    {
                                                if((j==2)|| g_bDPTrainingP0T || g_bDPTrainingP1T )
                                                {
                                                        goto FastTraining_Exit;
                                                }
                                                DP_printData( "---Rate--->>%x ", FastRate[j]);
                            if( FastRate[j] == 0x06 )
                                 DP_SetSyntheizer( DP_LBR );
                            else
                                 DP_SetSyntheizer( DP_HBR );

                                            for(i=0;i<3;i++)
                                            {
							  if( g_bDPTrainingP0T || g_bDPTrainingP1T )
                                                   {
                                                        goto FastTraining_Exit;
                                                   }	
                                                msWriteByte( REG_1881, 0x80 + FastLane[i] );
                                                if( (msReadByte( REG_1882 )&0x03) == 0x00 )
                                                        {
                                                                      if(msReadByte( REG_1882)&BIT5)
                                                                      msWriteByteMask( REG_1882, 0 , BIT5  );
                                                                      DPRxFastTraining2Enable( TRUE );

                                                         }
                                                                     ForceDelay1ms( 100 );
                                                                     DP_printData( "---Lane--->>%x ",FastLane[i] );

                                                     if( !DPRxCheckLock() )
                                                      {
                                                                DPRxFastTraining2Enable(FALSE);
                                                      }
                                                     else
                                                     {
                                                            BYTE xdata Counter=0;

                                                            while(DPRxCheckLock())
                                                            {
                                                                if(++Counter>5)
                                                                {
                                                                	     
                                                                	     #if ENABLE_XTAL_LESS
                                                                	      if(i)
                                                                	      {           
                                                                	      		DP_printData( " i= >>%x ", i );
												DPRxFastTraining2Enable(FALSE);												  
												
											   for(k=0;k<=3;k++)
                                   						   {									    
												  												  					   			 
												      if((k==3)|| g_bDPTrainingP0T || g_bDPTrainingP1T )
						                                                {
						                                                        goto FastTraining_Exit;
						                                                }												      
												        msWriteByte( REG_1881, 0x80 + FastLane[k] );

													 DPRxFastTraining2Enable( TRUE );

													 ForceDelay1ms( 100 );									

												        DP_printData( "Re_Check Lane--->>%x ",FastLane[k] );
												        
												      if( !DPRxCheckLock() )
					                                                       {
					                                                                DPRxFastTraining2Enable(FALSE);
					                                                       }
					                                                      else
					                                                      {														
					                                                      		 while(DPRxCheckLock())
					                                                      		 {														
																													  
															 if(++CounterChk>5)
	                                                              						  {           
	                                                              						  	 DP_printMsg( "-Pass3 <Fast> " ); 
																 goto FastTraining_OK;
															  }
					                                                      		 }
					                                                      }	      
											   }

                                                                	     }
                                                                	     #endif
                                                                	     
                                                                            goto FastTraining_OK;
                                                                }
                                                            }
                                                     }
                                           }
                                        }

                                      goto FastTraining_Exit;

            }
            else
            {
                     OffFastRate = msReadByte(REG_06E0 + Aux);

                     if( OffFastRate== 0x06 )

                 DP_SetSyntheizer( DP_LBR );
            else

                 DP_SetSyntheizer( DP_HBR );


                     OffFastLane = msReadByte(REG_06E1 + Aux)&0x0F;

                      msWriteByte( REG_1881, 0x80 + OffFastLane );

                      if( (msReadByte( REG_1882 )&0x03) == 0x00 )
                      {
                                if(msReadByte( REG_1882)&BIT5)
                                  msWriteByteMask( REG_1882, 0 , BIT5  );
                                  DPRxFastTraining2Enable( TRUE );
                      }
                          ForceDelay1ms( 5 );

                            if( !DPRxCheckLock() )
                            {
                                    DPRxFastTraining2Enable(FALSE);
                            }
                            else
                            {
                                BYTE xdata Counter=0;
                                while(DPRxCheckLock())
                                {
                                    if(++Counter>5)
                                    {
                                                goto FastTraining_OK;
                                    }
                                }
                            }

                            goto FastTraining_Exit;

             }

                FastTraining_OK:

                         DPRXAutoEQCheck();
                                     ForceDelay1ms( 50 );
                                     msWriteByte( REG_1446, 0xFF);
                                     DP_printMsg( " <Fast> - DP Lock!" );
                                     DP_printMsg( " <Fast> - DP Lock!--> Enable_Scram=0 " );
                                    if(!DPRxCheckTiming(MSACnt))
                                    {          DP_printMsg( " <Fast> - DP Lock!--> Disable_Scram=1 " );
                                                msWriteByteMask( REG_1882, BIT5, BIT5  );
                                              if(!DPRxCheckTiming(MSACnt))
                                                {
                                                    DP_printMsg( " <Fast> - DP Lock!--> EnableScram=0  " );
                                                msWriteByteMask( REG_1882, 0 , BIT5  );
                                                          if(!DPRxCheckTiming(MSACnt))
                                                            {
                                                                  msWriteByte( REG_1446, 0x00);
                                                                  return FALSE;
                                                            }
                                    }
                                    }
                                     msWriteByte( REG_1446, 0x00);
                                     DPRxFastTraining2Enable(FALSE);
                                     #if (CHIP_ID==CHIP_TSUM9)
                                     #if ENABLE_XTAL_LESS
                                     if(  msRegs[REG_1880] == 0x0A )
					 {
						msRegs[REG_1B84] = msRegs[REG_1B78];
						msRegs[REG_1B85] = msRegs[REG_1B79];
					 }
					 else
					 {
						msRegs[REG_1B86] = msRegs[REG_1B78];
						msRegs[REG_1B87] = msRegs[REG_1B79];	
					 }
						 #endif
					 #endif
					 
                                    return TRUE;
                                    
                FastTraining_Exit:
                                    DP_printMsg( " <Fast> - DP Lose Lock!" );
                                    DPRxFastTraining2Enable( FALSE );
                                    Clr_DPPACKET_FLAG();
                                    DPCheckModeCnt=0;
                                    DPFindModeCnt =0;
                                   return FALSE;

}

//**************************************************************************
//  [Function Name]:
//                  DPEventEnable()
//  [Description]
//                  DPEventEnable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPEventEnable(Bool bEnable)
{

     if( bEnable )
    {
             DPRxClearAUXVliadStatus();
             msWriteByteMask( REG_067B , 0, BIT4 );                                             //[4]:Enable AUX IRQ
        #if DP_SQUELCH_IRQ
             DPRxClearAUXVliadStatus();
             msWriteByteMask( REG_067B , 0, BIT5 );                                               //[5]:Enable SQ
        #endif
    }
    else
    {
             msWriteByteMask( REG_067B  , BIT4, BIT4 );                                        //[4]:Disable AUX IRQ
        #if DP_SQUELCH_IRQ
             msWriteByteMask( REG_067B  , BIT5, BIT5 );                                        //[5]:Disable SQ
        #endif
    }
}
//**************************************************************************
//  [Function Name]:
//                  DPRxHdcpReAuthen()
//  [Description]
//                  DPRxHdcpReAuthen
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxHdcpReAuthen(void)
{

      if(((msReadByte(REG_1882)&0x03)==0) && DPRxCheckLock()&&(msReadByte(REG_18C0)&BIT2))
      {
                if(g_bDPHDCPIRQ ==0)
               {
                    SetHDCP_Cnt(1000);
                    //u16HDCP_Cnt = 1000;
                    //g_bDPHDCPIRQ = 1;
               }

               if(g_bDPHDCPIRQ && (u16HDCP_Cnt==0))
            {
                    DPRxHPDEnable();
                    Delay1ms( 5);
                    DPRxHPDEnable();
                    g_bDPHDCPIRQ=0;
                }
             }
      else
      {
               g_bDPHDCPIRQ = 0;

      }
}

//**************************************************************************
//  [Function Name]:
//                  DPRxTiming()
//  [Description]
//                  DPRxTiming
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
#if DP_VPLLBIGChange
void DPRxVPLLHandle( void )
{
    if( g_bDPVPLLBIGChange )
    {
        msWriteByteMask(REG_1410,BIT0,BIT0);
        m_ucState = eSTATE_MEASURE;
        g_bDPVPLLBIGChange = FALSE;                               // clear IRQ Flag
        msWriteByte( REG_1450, msReadByte( REG_14C3 ) ); // update NVID
        msWriteByte( REG_1451, msReadByte( REG_14C4 ) ); // update NVID
        msWriteByte( REG_1452, msReadByte( REG_14C5 ) ); // update NVID
        msWriteByte( REG_1448, msReadByte( REG_14C0 ) ); // update MVID
        msWriteByte( REG_1449, msReadByte( REG_14C1 ) ); // update MVID
        msWriteByte( REG_144A, msReadByte( REG_14C2 ) ); // update MVID
        DP_printMsg( "\r\nVIDEO MN BIG CHANGE" );
    }

}
#endif
//**************************************************************************
//  [Function Name]:
//                  DPRxPacketColor()
//  [Description]
//                  DPRxPacketColor
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE DPRxPacketColor( void )
{
    BYTE xdata temp;

    temp = InputColor_RGB;
    switch(( msReadByte( REG_1369 ) & 0x06 ) >> 1 )
    {
        case 0:
            temp = InputColor_RGB;
            break;
        case 1:
            temp = InputColor_YPBPR_422;
            break;
        case 2:
            temp = InputColor_YPBPR_444;
            break;
    }

    return temp;
}
//**************************************************************************
//  [Function Name]:
//                  DPRxCheckInputFormat()
//  [Description]
//                  DPRxCheckInputFormat
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxCheckInputFormat( void )
{
    // DP MSA MISC[7:0]
    // [2:1]        00:RGB              ,             01:YCbCr 422        ,   10:YCbCr 444
    // [3]          0:VESA Range     ,               1:CEA Range
    // [4]          0:BT601              ,               1:BT709
    // [7:5]     000:6bits
    //             001:8bits
    //             010:10bits
    //             011:12bits
    //             100:16bits
    BYTE u8Temp=DPRxPacketColor();
    if( u8Temp != gDPInfo.ucDPColorFormat )
    {
        //DP_printData("gDPInfo.ucDPColorFormat=%d",gDPInfo.ucDPColorFormat);
        gDPInfo.ucDPColorFormat = u8Temp;
        Set_InputTimingChangeFlag();
        //DP_printData("DP color=%d",u8Temp);
#if ENABLE_DEBUG_TIMING_CHANGE_POINT
        SetTimingChangeFirstPoint(TimingChange_DPColorFormatChange);
#endif
    }
}


//**************************************************************************
//  [Function Name]:
//                  DPRxTiming()
//  [Description]
//                  DPRxTiming
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************

void DPRxTiming( void )
{
        WORD xdata Blanking = 0;
        WORD xdata Width, Front_Porch = 0, Back_Porch = 0;
        BYTE xdata temp = 0;


       gDPInfo.bDPInterlace = ( msReadByte( REG_136B ) & 0x04 ) ? TRUE : FALSE; // Read BK36 A5 Bit2 Interlance mode

    if( gDPInfo.bDPInterlace )
    {
        gDPInfo.uwDPVtotal = ( msRead2Byte( REG_14B6 ) & 0x7FFF ) << 1;
        gDPInfo.uwDPVWidth =( msRead2Byte( REG_14BE) & 0x7FFF ) <<1;

    }
    else
    {
        gDPInfo.uwDPVtotal = msRead2Byte( REG_14B6 ) & 0x7FFF;
        gDPInfo.uwDPVWidth = msRead2Byte( REG_14BE) & 0x7FFF;
    }

           gDPInfo.uwDPHWidth = msRead2Byte( REG_1366 ) & 0x7FFF;

       #if DP_ASTRO_INTERLACE_PATCH//Mike 130207
        DPRxInterlaceCheck();//Mike 120216 the abnormal image issue for ASTRO 480i&576i
       #endif
        msWriteByteMask( REG_1310, BIT1 | BIT0, BIT1 | BIT0 );
        if( gDPInfo.bDPInterlace )
         Blanking = (gDPInfo.uwDPVtotal - gDPInfo.uwDPVWidth) >> 1;
    else
        Blanking = gDPInfo.uwDPVtotal - gDPInfo.uwDPVWidth;
        Width =  Blanking / 2;
        Front_Porch = ( Blanking - Width ) / 2;
        Back_Porch = Blanking - Width - Front_Porch ;
        msWrite2Byte( REG_1314, Width );
        msWriteByte( REG_130B, Front_Porch );
        msWriteByte( REG_130E, ( Back_Porch ));
        msWrite2Byte( REG_1312, msRead2Byte( REG_1312 ) );


}

#if 0
//**************************************************************************
//  [Function Name]:
//                  DPRxEQInform()
//  [Description]
//                  DPRxEQInform
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxEQInform( void )
{
    if( EQ_Done != INITIAL )
    {
        if( EQ_Done == FINISH )
        {
           // DP_printData( "\r\nAuto EQ Pass %x ", 0 );
        }
        else if( EQ_Done == TIME_OUT )
        {
            //DP_printData( "\r\nAuto EQ Fail %x ", 0 );
        }
        EQ_Done = INITIAL;
        //DP_printData( "\r\nBest Pre-Setting  => %x  ", msReadByte( REG_3714 ) & 0x03 );
        //DP_printData( "\r\nEQ_Value  = %x  ", msReadByte( REG_3568 ) );
    }
}
//**************************************************************************
//  [Function Name]:
//                  DPRxAutoEQEnable()
//  [Description]
//                  DPRxAutoEQEnable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************

void DPRxAutoEQEnable( BYTE bEnable )
{
    if( bEnable )
    {
        msWriteByteMask( REG_3700, _BIT0, _BIT0 );
    }
    else
    {
        msWriteByteMask( REG_3700, 0, _BIT0 );
    }
}
#endif

//**************************************************************************
//  [Function Name]:
//                  DPRxCheckTiming()
//  [Description]
//                  DPRxCheckTiming
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL DPRxCheckTiming( BYTE Count )
{
    Bool xdata found = FALSE;
    WORD xdata LS_Clock=0;
    WORD xdata Pixel_Clock=0;
    DWORD xdata Base_N=0;
    DWORD xdata Base_M=0;

    while(Count--)
    {
        #if  !DP_DEBUG
        ForceDelay1ms( 20 );
        #endif
        LS_Clock = 27 * msReadByte(REG_1880);
        DP_printData( "\r\DP Input : LS_Clock %d ", LS_Clock );
        DP_printData( "\r\DP Input : Lane %d ", msReadByte(REG_1881)&0x0F);
        Base_M = ((DWORD) msRead2Byte( REG_14C1 ) <<8) + msReadByte(REG_14C0);
        DP_printData( "\r\DP Input : Base_M %x ", Base_M );
        Base_N = ((DWORD)msRead2Byte( REG_14C4 ) <<8) + msReadByte(REG_14C3);
        DP_printData( "\r\DP Input : Base_N %x ", Base_N );
        Pixel_Clock = ((DWORD) Base_M * LS_Clock ) / Base_N ;
        DP_printData( "\r\DP Input : Pixel_Clock %d ", Pixel_Clock );

                gDPInfo.bDPInterlace = ( msReadByte( REG_136B ) & 0x04 ) ? TRUE : FALSE; // Read BK36 A5 Bit2 Interlance mode


                gDPInfo.uwDPHtotal = msRead2Byte( REG_14B8 ) & 0x7FFF;

                if( gDPInfo.bDPInterlace )
                    gDPInfo.uwDPVtotal = ( msRead2Byte( REG_14B6 ) & 0x7FFF ) << 1;
                else
                    gDPInfo.uwDPVtotal = msRead2Byte( REG_14B6 ) & 0x7FFF;

                    gDPInfo.uwDPHWidth = msRead2Byte( REG_1366 ) & 0x7FFF;


                if( gDPInfo.bDPInterlace )
                {
                    gDPInfo.uwDPVWidth = ( msRead2Byte( REG_14BE ) & 0x7FFF ) << 1;
                }
                else
                {
                    gDPInfo.uwDPVWidth = msRead2Byte( REG_14BE ) & 0x7FFF;
                    gDPInfo.uwDPHStart = msRead2Byte( REG_14BA ) & 0x7FFF;
                    gDPInfo.uwDPVStart = msRead2Byte( REG_14BC ) & 0x7FFF;
                    gDPInfo.uwDPHPWS = msRead2Byte( REG_14B2 ) & 0x7FFF;
                    gDPInfo.bDPHPol = ( msReadByte( REG_14B2 ) & 0x80 ) ? 1 : 0;
                    gDPInfo.uwDPVPWS = msRead2Byte( REG_14B4 ) & 0x7FFF;
                    gDPInfo.bDPVPol = ( msReadByte( REG_14B4 ) & 0x80 ) ? 1 : 0;
                }

                  DP_printData( "\r\DP Input : DPHtotal   %d ", gDPInfo.uwDPHtotal );
                  DP_printData( "\r\DP Input : DPVtotal   %d ", gDPInfo.uwDPVtotal );
                  DP_printData( "\r\DP Input : DPHWidth   %d ", gDPInfo.uwDPHWidth );
                  DP_printData( "\r\DP Input : DPVWidth   %d ", gDPInfo.uwDPVWidth );




              if(( gDPInfo.uwDPHWidth < 4000 ) && ( gDPInfo.uwDPVWidth < 2501 ) && ( Pixel_Clock > 10) && ( Pixel_Clock < 500)
              &&( gDPInfo.uwDPHtotal > gDPInfo.uwDPHWidth) && ( gDPInfo.uwDPVtotal > gDPInfo.uwDPVWidth))
               {
                    found = TRUE;
                    DP_printMsg( "\r\===> DP timing Pass:" );
                }
                else
                {   DP_printMsg( "\r\===> DP timing Fail:" );
                     found = FALSE;
                 break;
                }
    }
     return found;

}



//**************************************************************************
//  [Function Name]:
//                  DPRxClearAUXVliadStatus()
//  [Description]
//                 DPRxClearAUXVliadStatus
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxClearAUXVliadStatus( void )
{

        #if DP_SQUELCH_IRQ
            msRegs[REG_0678 + Aux] = BIT5; // clear interrupt
        #endif
            msRegs[REG_0678 + Aux] = BIT4; // clear interrupt

         g_bDPAUXVALID = FALSE;
}

//**************************************************************************
//  [Function Name]:
//                  DPRxEDIDLoad()
//  [Description]
//                  DPRxEDIDLoad
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxSetEQCalValue( void )
{
	if(gDPInfo.ucDPStates != 0)
	return; 	

	if(msRegs[REG_1880]==0x0A)			 // EQ Calbra 
	{
		MEM_MSWRITE_BYTE(REG_1FB4,wHF1_L0);
		MEM_MSWRITE_BYTE(REG_1FB5,wHF1_L0>>8);
		MEM_MSWRITE_BYTE(REG_1FB6,wHF1_L1);
		MEM_MSWRITE_BYTE(REG_1FB7,wHF1_L1>>8);
	}
	else
	{
		MEM_MSWRITE_BYTE(REG_1FB4,wHF2_L0);
		MEM_MSWRITE_BYTE(REG_1FB5,wHF2_L0>>8);
		MEM_MSWRITE_BYTE(REG_1FB6,wHF2_L1);
		MEM_MSWRITE_BYTE(REG_1FB7,wHF2_L1>>8);
	}

}

//**************************************************************************
//  [Function Name]:
//                  DPRxEDIDLoad()
//  [Description]
//                  DPRxEDIDLoad
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxEDIDLoad( void )
{

     BYTE xdata ucValue;
     WORD xdata i;
     BYTE xdata ucTemp=1;
#if DP_USE_INTERNAL_EDID

    #if DP_EDID_384
        msWriteByte( REG_3EF8, 0x01);                                // D0 Edid Sram address
        msWriteByteMask( REG_3EEB, 0x01, 0x0F);                      // EDID Set Base address


            for( i = 0; i < 384; i++ )
            {
                 ucValue = EDID[i];

                 if(i>=256)
                 {
             msWriteByteMask( REG_3EEB, 0x03, 0x0F);
             msWriteByte(REG_3E4B, i-256);
                 }
                 else if(i >= 128)
                 {
                     msWriteByteMask( REG_3EEB, 0x02, 0x0F);
                     msWriteByte(REG_3E4B, i-128);                                        //    Address[i]      25[14:8]
                 }
                 else
                 {
                      msWriteByte( REG_3E4B, i );
                 }
                      msWriteByte( REG_3E4C, ucValue);                                       //    EDID[i]           25[14:8]
                      msWriteByteMask( REG_3E43, BIT5, BIT5);
            }
      #else
           msWriteByte( REG_3EF8, 0x01);                                 // D0 Edid Sram address
            msWriteByteMask( REG_3EEB, 0x01, 0x0F);                      // EDID Set Base address


            for( i = 0; i < 256; i++ )
            {
                 ucValue=EDID[i];

                 if(i >= 128)
                {
                     msWriteByteMask( REG_3EEB, 0x02, 0x0F);
                    msWriteByte(REG_3E4B, i-128);                                         //    Address[i]      25[14:8]
                }
                 else
                  msWriteByte( REG_3E4B, i );
                  msWriteByte( REG_3E4C, ucValue);                                       //    EDID[i]           25[14:8]
                  msWriteByteMask( REG_3E43, BIT5, BIT5);
            }



         #endif
                          //===============MCU Read======================
                          #if 0
                           for( i = 0; i < 256; i++ )
                            {
                                  msWriteByteMask( REG_3EEB, 0x10, 0xF0);
                                     if(i >= 128)
                                     msWriteByte(REG_3E4B, i-128);                                      //    Address[i]      25[14:8]
                                 else
                                     msWriteByte( REG_3E4B, i );

                                  msWriteByteMask( REG_3E43, BIT4, BIT4);
                                  printData("  %x",msReadByte( REG_3E0E));
                                  if(i >= 128)
                                  msWriteByteMask( REG_3EEB, 0x20, 0xF0);
                            }
                         #endif
                          //============================================

                        #else
                         #if 0
                            for( i = 0; i < 256; i++ )
                            {
                                Flash_Read_Factory_KeySet( FLASH_KEY_DDCDP, TRUE, i, 1, &ucValue, 1 );
                                msWriteByte( REG_3E4B, i );                                              //     Address[i]
                                msWriteByte( REG_3E4C, ucValue);                                    //    EDID[i]
                                 msWriteByteMask( REG_3E43, BIT5, BIT5);
                                if(i&BIT7)
                                msWriteByteMask( REG_3EEB, ((++ucTemp) <<4), 0xF0);
                            }
                        #endif


  //  }

 #endif
}

//**************************************************************************
//  [Function Name]:
//                  DPRxPolingDPCDPowerSave()
//  [Description]
//                 DPRxPolingDPCDPowerSave
//  [Arguments]:
//
//  [Return]:
//
//*****************************************************************
#if DP_POWER_SAVING
void DPRxPollingDPCDPowerSave( void )
{
    g_bDPDPCDPOWERSAVE = (( msReadByte( REG_0672 + Aux ) & 0x03 ) == 0x02 ) ? 1 : 0; // Read BKB 5A Bit1:0 for DPCD 600h PM mode 10=PWD 01=normal

    if( g_bDPDPCDPOWERSAVE && ( g_DPDPCDPOWERSAVECNT == ( DPDPCDPOWERSAVE_COUNTER - 1 ) ) )
        DP_printData( "DPCD 600H PWD!CNT%d", g_DPDPCDPOWERSAVECNT );

    if( g_bDPDPCDPOWERSAVE == 0 )
        g_DPDPCDPOWERSAVECNT = 0;

}
#endif


//**************************************************************************
//  [Function Name]:
//                  DPRxPower5VEnable()
//  [Description]
//                  DPRxPower5VEnable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxPower5VEnable(Bool bEnable )
{
         if( bEnable )
        {
                  hw_Set_DPCable5V();
        }
        else
        {
                 hw_Clr_DPCable5V();
        }
}
//**************************************************************************
//  [Function Name]:
//                  DPRxHPDEnable()
//  [Description]
//                  DPRxHPDEnable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxHPDEnable( void )
{

            hw_Set_DPHpd0();                                                                 // HPD ouput enable and set Low GPIO52

            DP_printMsg( "DPRxHPDEnable!" );
}

//**************************************************************************
//  [Function Name]:
//                  DPRxHPDDisable()
//  [Description]
//                  DPRxHPDDisable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxHPDDisable( void )
{
        DP_printMsg( "DPRxHPDDisable!" );

            hw_Clr_DPHpd0();                                                                         // HPD ouput enable and set Low GPIO52

}

//**************************************************************************
//  [Function Name]:
//                  DPRxFastTrainingInitial()
//  [Description]
//                  DPRxFastTrainingInitial
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxFastTrainingInitial( void )
{
    DP_SetSyntheizer( DP_LBR );
    msWriteByteMask( REG_174A, 0, BIT7 );
    msWriteByteMask( REG_17B5, BIT6, BIT6 );
    msWriteByteMask( REG_18E4,BIT3, BIT3 );
    msWriteByte( REG_18E0, 0x51 );

// AUto Syth Init
	//msWrite2Byte( REG_1FA6, 0x9999); 	
	//msWrite2Byte( REG_1FA8, 0x19); 

	//msWrite2Byte( REG_1FA9, 0x5E35); 	
	//msWrite2Byte( REG_1FAB, 0x1A); 

	//msWrite2Byte( REG_1FAC, 0xD4FD); 	
	//msWrite2Byte( REG_1FAE, 0x18); 	
        
    
}
//**************************************************************************
//  [Function Name]:
//                  DPRxFastTraining2Enable()
//  [Description]
//                  DPRxFastTraining2Enable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxFastTraining2Enable( Bool bEnable )
{
    if( bEnable )
    {
        DP_printMsg( " <Fast> - En " );
        msWriteByte( REG_1644, 0x00);
        msWriteByteMask( REG_18E4,  BIT3, BIT3 );
        msWriteByteMask( REG_18E0,  BIT0|BIT1|BIT2, BIT0|BIT1|BIT2 );
        msWriteByteMask( REG_1BD7,  BIT3|BIT2|BIT1, BIT3|BIT2|BIT1 );
    }
    else
    {
         msWriteByteMask( REG_1BD7,  0, BIT3|BIT2|BIT1 );
        return;
        DP_printMsg( " <Fast> - Dis" );
        msWriteByte( REG_1644, 0x77);
     msWriteByteMask( REG_18E4,  0, BIT3 );
        msWriteByteMask( REG_18E0,  0, BIT0|BIT1|BIT2 );

    }
}
//**************************************************************************
//  [Function Name]:
//                  DPRxFastTraining1Enable()
//  [Description]
//                  DPRxFastTraining1Enable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
#if 0
void DPRxFastTraining1Enable( Bool bEnable )
{
    if( bEnable )
    {
        msWriteByteMask( REG_368E, BIT0, BIT0 );
    }
    else
    {
        msWriteByteMask( REG_368E, 0, BIT0 );
    }
}
#endif
//**************************************************************************
//  [Function Name]:
//                  DPRxHPDIRQ()
//  [Description]
//                  DPRxHPDIRQ
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DP_SetSyntheizer( E_DP_Rate bRate )
{

    if( bRate == DP_LBR )
    {
       // msWriteByteMask( REG_17A8, BIT4, BIT3|BIT4|BIT5);                          //  DCDR KP Value
       // msWrite2Byte( REG_1722, 0xaaab );
       // msWriteByte( REG_1724, 0x2a );
        msWriteByte( REG_1880, 0x06 );

	 msWrite2Byte( REG_1FA6, 0x5555); 	
	  msWrite2Byte( REG_1FA8, 0x15); 

	 msWrite2Byte( REG_1FA9, 0xF92C); 	
	 msWrite2Byte( REG_1FAB, 0x15); 

	 msWrite2Byte( REG_1FAC, 0xB17E); 	
 	 msWrite2Byte( REG_1FAE, 0x14); 
      
    }
    else
    {
         // msWriteByteMask( REG_17A8, BIT3|BIT4, BIT3|BIT4|BIT5);                 //  DCDR KP Value
         //msWrite2Byte( REG_1722, 0x999a );
         //msWriteByte( REG_1724, 0x19 );
         msWriteByte( REG_1880, 0x0A );

	  msWrite2Byte( REG_1FA6, 0x9999); 	
	  msWrite2Byte( REG_1FA8, 0x19); 

	  msWrite2Byte( REG_1FA9, 0x5E35); 	
	  msWrite2Byte( REG_1FAB, 0x1A); 

	  msWrite2Byte( REG_1FAC, 0xD4FD); 	
	  msWrite2Byte( REG_1FAE, 0x18); 	
        
    }

     DPRxSetEQCalValue();
	
         // msWrite2Byte( REG_1FAF, 0x00A7); 	   // 0.025  STEP
	  //msWriteByte( REG_1FB1, 0x00);   
	  //msWrite2Byte( REG_1FA4, 0xFF);          //Time out	  
	  //msWriteByte( REG_1B5D, 0x1E);    	  // CDR lock throhs  1cnt 10u
        
}
//**************************************************************************
//  [Function Name]:
//                  DP_GetSquelch()
//  [Description]
//                  DP_GetSquelch
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool DP_GetSquelch( DWORD Times)
{
    BOOL xdata bStstus = FALSE;
    BYTE xdata Temp=0;

    while( Times-- )
    {
        if( !( msReadByte( REG_0963 )&BIT6 ) )
        {
            return FALSE;
        }

    }  //DP_printMsg( "SQ Active" );

    return TRUE;

}

//**************************************************************************
//  [Function Name]:
//                  DP_GetSquelchPortB()
//  [Description]
//                  DP_GetSquelchPortB
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool DP_GetSquelchPortB( WORD Times)
{
    BOOL xdata bStstus = FALSE;

    while( Times-- )
    {
        if( !( msReadByte( REG_0963 )&BIT6 ) )
        {
            return FALSE;
        }

    }  //DP_printMsg( "SQ Active" );

    return TRUE;

}
//**************************************************************************
//  [Function Name]:
//                  DP_GetSquelchPortC()
//  [Description]
//                  DP_GetSquelchPortC
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool DP_GetSquelchPortC( WORD Times)
{
    BOOL xdata bStstus = FALSE;

    while( Times-- )
    {
        if( !( msReadByte( REG_0963 )&BIT6 ) )
        {
            return FALSE;
        }

    }  //DP_printMsg( "SQ Active" );

    return TRUE;

}
//**************************************************************************
//  [Function Name]:
//                  DP_EnableSquelch()
//  [Description]
//                  DP_EnableSquelch
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************

void DP_EnableSquelch( Bool bEnable )
{
    if( bEnable )
    {
        msWriteByteMask( REG_0961, BIT7|BIT6, BIT7|BIT6 );              //Enable Squelch
    }
    else
    {
        msWriteByteMask( REG_0961, BIT7, BIT7|BIT6 );                    //Disable Squelch
    }

	#if (CHIP_ID==CHIP_TSUM9)
        msWriteByteMask( REG_0961, 0x05 ,0x0F);                                  //Set SQH Level
       #elif (CHIP_ID==CHIP_TSUMF)
	 msWriteByteMask( REG_0961, 0x04 ,0x0F);                                  //Set SQH Level
       #endif

}

//**************************************************************************
//  [Function Name]:
//                  DPRxHPDIRQ()
//  [Description]
//                  DPRxHPDIRQ
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************

void DPRxHPDIRQ( void )
{
        DPRxHPDDisable();                                                                           // HPD ouput enable and set Low
        TH0 = g_ucTimer0_TH0;
        TL0 = g_ucTimer0_TL0;
        ForceDelay1ms( 1 );
        DPRxHPDEnable();                                                                            // HPD ouput enable and set High
      //  DP_printMsg( "DPRxHPDIRQ!" );
}


//**************************************************************************
//  [Function Name]:
//                  DPRxOutputEnable()
//  [Description]
//                  DPRxOutputEnable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxOutputEnable( Bool bEnable )
{

    if( bEnable )
    {

    msWriteByteMask( REG_1BD7, 0,BIT0 );            //reg_atop_pd_bg
    msWriteByteMask( REG_1BD6, BIT3 ,BIT3 );            //reg_atop_pd_clkin
    msWriteByteMask( REG_1BB6, 0 , 0x0F );          //Power Down Lane
    //msWriteByteMask( REG_1BB2, 0 , BIT1 );            //reg_atop_ove2[1]
    //msWriteByteMask( REG_1BD7, 0,0xF0 );              //reg_atop_pd_phdac_ov
    msWriteByteMask( REG_1BF4, 0,0x0F);             //reg_atop_pd_pll
    //msWriteByteMask( REG_1B00, 0,BIT7);               //DP mode


    msWrite2Byte( REG_1A70, 0xFFFF );                  //  PLL clkgen enable
    msWriteByte(REG_1A72,0xFF);

    msWrite2Byte( REG_1A86, 0xFFFF );                  //  MISC clkgen enable
    msWrite2Byte( REG_1A88, 0xFFFF);

    msWrite2Byte( REG_1A92, 0xFFFF );                  //  Clkgen clkgen enable
    msWrite2Byte( REG_1A94, 0xFFFF);

       msWriteByteMask( REG_1A63, 0,BIT0 );             //  VPLL PD
    msWriteByteMask( REG_1A06, 0,BIT0 );                // APLL DIV
    msWriteByteMask( REG_1A07, 0,BIT0 );                // APLL PD

     msWriteByteMask( REG_0940, 0x00 ,0x0F);            //R-term
     msWriteByteMask( REG_0941, BIT4 ,BIT4);            //NODIE_PD_CLKIN
     msWriteByteMask( REG_0941, 0 ,BIT5);               //NODIE_PD_PORT_SW
     msWriteByteMask( REG_0907, 0 ,BIT4);               //reg_cbus_ctrl_p0
     msWriteByteMask( REG_0928, 0 ,BIT0);               //reg_pm_dvi_5v_det_en
     msWriteByteMask( REG_0924, 0 ,BIT7);               //reg_p0_pd_rck
     msWriteByteMask( REG_0950, BIT0 ,BIT0);                //reg_nodie_en_lvrt


        g_bDPPowerDown = FALSE;
        DP_printMsg( "\DP Output Enable!" );
    }
    else
    {

           msWriteByteMask( REG_1BD7, BIT0,BIT0 );                //reg_atop_pd_bg
        //msWriteByteMask( REG_1BD6, 0 ,BIT3 );                  //reg_atop_pd_clkin       (table)
        msWriteByteMask( REG_1BB6, 0x0F , 0x0F );             //Power Down Lane
        msWriteByteMask( REG_1BF4, 0x0F,0x0F);                    //reg_atop_pd_pll
        //msWrite2Byte( REG_1A70, 0 );                            //  PLL clkgen enable
        //msWriteByte(REG_1A72,0);
        //msWrite2Byte( REG_1A86, 0 );                             //  MISC clkgen enable
        //msWrite2Byte( REG_1A88, 0);
        //msWrite2Byte( REG_1A92, 0 );                              //  Clkgen clkgen enable
        //msWrite2Byte( REG_1A94, 0);
        msWriteByteMask( REG_1A63, BIT0,BIT0 );                    //  VPLL PD
        msWriteByteMask( REG_1A06, BIT0,BIT0 );                    // APLL DIV
        msWriteByteMask( REG_1A07, BIT0,BIT0 );                        // APLL PD
        //msWriteByteMask( REG_0940, 0x0F ,0x0F);               // power Rterm               (table)
        //msWriteByteMask( REG_0941, 0 ,BIT4);                  //NODIE_PD_CLKIN           (table)
        msWriteByteMask( REG_0941, BIT5 ,BIT5);                      //NODIE_PD_PORT_SW         (table)
        //msWriteByteMask( REG_0907, BIT4 ,BIT4);               //reg_cbus_ctrl_p0            (table)
        //msWriteByteMask( REG_0928, BIT0 ,BIT0);               //reg_pm_dvi_5v_det_en   (table)
        //msWriteByteMask( REG_0924, BIT7 ,BIT7);               //reg_p0_pd_rck               (table)
        //msWriteByteMask( REG_0950, BIT0 ,BIT0);               //reg_nodie_en_lvrt

     g_bDPPowerDown = TRUE;
        DP_printMsg( "DP Output Disable!" );

    }
}

//**************************************************************************
//  [Function Name]:
//                  DPRxIRQEnable(Bool bEnable)
//  [Description]
//                  DPRxIRQEnable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxIRQEnable( Bool bEnable )
{


    if( bEnable )
    {
          msWriteByteMask(REG_175C , 0 ,BIT0 );            // APLL
          msWriteByteMask(REG_175C , 0 ,BIT1 );            // VPLL
          msWriteByteMask(REG_175C , BIT2 ,BIT2 );      // No Input Detect
          msWriteByteMask(REG_175C , BIT3 ,BIT3 );      // HDCP
          msWriteByteMask(REG_175C , BIT4 ,BIT4 );      // LS Clock Big Change
          msWriteByteMask(REG_175C , BIT5 ,BIT5 );      // OP Ring Buffer
          msWriteByteMask(REG_175C , 0 ,BIT6 );      // TP2
          msWriteByteMask(REG_175C , 0 ,BIT7 );      // TP1

          msWriteByteMask(REG_175D , 0 ,BIT0 );      // Link Rate Change
          msWriteByteMask(REG_175D , 0 ,BIT1 );      // Interskew Lose
          msWriteByteMask(REG_175D , 0 ,BIT2 );      // CR Loss
          msWriteByteMask(REG_175D , BIT3 ,BIT3 );      // Symbol Loss
          msWriteByteMask(REG_175D , BIT4 ,BIT4 );           // MSA Chg
          msWriteByteMask(REG_175D , 0 ,BIT7 );          // 270
	  	
	   
        // #######    Enable interrupt   #########################
          DP_printMsg( "\r\nDP IRQ Enable!" );

          msWriteByteMask( REG_2B19, ~( _BIT7 ), _BIT7 ); //DPISR

    }
    else
    {
          msWriteByteMask(REG_175C , BIT0 ,BIT0 );      // APLL
          msWriteByteMask(REG_175C , BIT1 ,BIT1 );      // VPLL
          msWriteByteMask(REG_175C , BIT2 ,BIT2 );      // No Input Detect
          msWriteByteMask(REG_175C , BIT3 ,BIT3 );      // HDCP
          msWriteByteMask(REG_175C , BIT4 ,BIT4 );      // LS Clock Big Change
          msWriteByteMask(REG_175C , BIT5 ,BIT5 );      // OP Ring Buffer
          msWriteByteMask(REG_175C , BIT6 ,BIT6 );      // TP2
          msWriteByteMask(REG_175C , BIT7 ,BIT7 );      // TP1

          msWriteByteMask(REG_175D , BIT0 ,BIT0 );      // Link Rate Change
          msWriteByteMask(REG_175D , BIT1 ,BIT1 );      // Interskew Lose
          msWriteByteMask(REG_175D , BIT2 ,BIT2 );      // CR Loss
          msWriteByteMask(REG_175D , BIT3 ,BIT3 );      // Symbol Loss
          msWriteByteMask(REG_175D , BIT4 ,BIT4 );      // Video Change mode
          msWriteByteMask(REG_175D , BIT7 ,BIT7 );          // 270
            
        // #######    disable interrupt   #########################
        DP_printMsg( "\r\nDP IRQ Disable!" );
        msWriteByteMask( REG_2B19, ( BIT7 ), _BIT7 ); // DPISR
    }




}
//**************************************************************************
//  [Function Name]:
//                  DPRxAuxIRQEnable(Bool bEnable)
//  [Description]
//                  DPRxAuxIRQEnable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxAuxIRQEnable( Bool bEnable )
{
    msWriteByte( REG_067B, 0xFF );
    msWriteByteMask( REG_0691, 0xF0, 0xF0);
    msWriteByteMask( REG_0696, BIT7|BIT4, BIT7|BIT4);
    msWriteByteMask( REG_0693, BIT6|BIT2, BIT6|BIT2);
    msWriteByte( REG_0695, 0x0F );
    msWriteByteMask( REG_06F9, BIT6|BIT2, BIT6|BIT2);

    msWriteByte( REG_0679, 0xFF );                                           // Clear Interrupt States
    msWriteByte( REG_0679, 0x00 );

    msWriteByte( REG_0692, 0x0F );                                           // Clear dpcd0_irq_mask Interrupt States
    msWriteByte( REG_0692, 0x00 );

    msWriteByteMask( REG_0693, BIT7, BIT7);                           // Clear __aksv_irq_mask
    msWriteByteMask( REG_0693, 0, BIT7);

    msWriteByteMask( REG_0696, BIT2, BIT2);                           // Clear_SQ
    msWriteByteMask( REG_0696, 0, BIT2);

    if( bEnable )
    {
        // #######    Enable interrupt   #########################
               DP_printMsg( "\r\nDP IRQ Enable!" );
         //   msWriteByteMask( REG_0691,0, BIT4);      // Enable ProgramDPCD
         //   msWriteByteMask( REG_067B,0, BIT4);      // Enable Aux Comd
         //   msWriteByteMask( REG_067B,0, BIT0);         // Enable Aux Comd
	#if DP_Aux_TimeOut
	        msWriteByteMask( REG_0696,0, BIT7);                 // Enable Aux Time out
	#endif   
        #if DPENABLEMCCS
               msWriteByteMask( REG_067B,0, BIT7);                 // Enable MCCS
        #endif
             msWriteByteMask( REG_2B1A, 0, _BIT2 );   //DPISR_AUX1    Enable  //DPISR_AUX0    Disbale
        // ################################

    }
    else
    {
        // #######    disable interrupt   #########################
        DP_printMsg( "\r\nDP IRQ Disable!" );
        msWriteByteMask( REG_2B1A, (_BIT2 ), _BIT2 );    //DPISR_AUX0
        // ################################
    }

}

//**************************************************************************
//  [Function Name]:
//                  DPRxCheckLock()
//  [Description]
//                  DPRxCheckLock
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL DPRxCheckLock( void )
{

    BYTE xdata ucLans = 0;
    BOOL xdata bStstus = FALSE;
    BYTE xdata ucStatus0 = 0;
    BYTE xdata ucStatus1 = 0;
    BYTE xdata uctemp = 0;

    uctemp =  (msReadByte( REG_18C3 )&0x38 ) << 1;
    ucStatus0 =  (uctemp | (msReadByte( REG_18C3 )&0x07));

    uctemp =  (msReadByte( REG_18C4 )&0x38 ) << 1;
    ucStatus1 =  (uctemp | (msReadByte( REG_18C4 )&0x07));

    g_bDPLANELOCK = FALSE;
    g_bDPCDRLOCK = FALSE;

    ucLans = msReadByte( REG_1881 ) & 0x07;

#if 1
   // DP_printData( " ucStatus0=%x", msReadByte( REG_1F10 ) );
  //  DP_printData( " ucStatus1=%x", msReadByte( REG_36A8 ) );
  //  DP_printData( " 8E=%x", msReadByte( REG_368E ) );
#endif

    switch( ucLans )
    {
        case 0x01:
            if( (ucStatus0 & 0x07) == 0x07 )
            {
                bStstus = TRUE;
                g_bDPLANELOCK = TRUE;
                g_bDPCDRLOCK = TRUE;

            }
            break;
        case 0x02:
            if( ucStatus0 == 0x77 )
            {
                bStstus = TRUE;
                g_bDPLANELOCK = TRUE;
                g_bDPCDRLOCK = TRUE;
            }
            break;
        case 0x04:
            if(( ucStatus0 == 0x77 ) && ( ucStatus1 == 0x77 ) )
            {
                bStstus = TRUE;
                g_bDPLANELOCK = TRUE;
                g_bDPCDRLOCK = TRUE;
            }
            break;
        default:
                g_bDPLANELOCK = FALSE;
                g_bDPCDRLOCK = FALSE;
            break;
    }

    if( !( msReadByte( REG_18C4 )&_BIT7 ) ) // lost alignment
    {
              g_bDPLANELOCK = FALSE;
    }

    gDPInfo.bDPLock = g_bDPCDRLOCK;
    if(bStstus == FALSE)
   {  //DP_printData( "\r\C3=%x ", msReadByte( REG_18C3 ) );
       //DP_printData( "\r\C4=%x ",   msReadByte( REG_18C4 ) );
          if( m_ucState == eSTATE_NORMAL)
            {
                  DP_printData( " ucStatus0=%x", ucStatus0 );
                  DP_printData( " ucStatus1=%x", ucStatus1 );
            }
   }
    return bStstus;
}




//**************************************************************************
//  [Function Name]:
//                  DPRxMainStrmRst()
//  [Description]
//                  DPRxMainStrmRst
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxMainStrmRst( void )
{
    return;

  //  DP_printMsg( "\r\nDPRxMainStrmRst" );

}
//**************************************************************************
//  [Function Name]:
//                  DP_EnableDRRFunction()
//  [Description]
//                  DP_EnableDRRFunction
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************

#if DP_DRRfunction
void DPRxEnableDRRFunction(Bool Enable )
{

    if(Enable)
    {
		DP_printMsg( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>#  DRR Enable" );
		msWriteByteMask( REG_0603 , BIT6,BIT6);
		msWriteByteMask( REG_138C , 0,BIT5);		
    }
    else
    {
		DP_printMsg( " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>#  DRR Disable" );
		msWriteByteMask( REG_0603 ,0 ,BIT6);
		msWriteByteMask( REG_138C , BIT5,BIT5);
    }

    //DPRxHPDDisable();                                                                           // HPD ouput enable and set Low
    //ForceDelay1ms( 100 );
    //DPRxHPDEnable();
}
//**************************************************************************
//  [Function Name]:
//                  DPRxGetDRRFlag_ISR()
//  [Description]
//                  DPRxGetDRRFlag_ISR
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool  DPRxGetDRRFlag_ISR(void)
{
    Bool ucflag= FALSE;

    if(msReadByte( REG_1887)&BIT7)
    ucflag =TRUE;

    return ucflag;
 }
//**************************************************************************
//  [Function Name]:
//                  DPRxGetDRRFlag()
//  [Description]
//                  DPRxGetDRRFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool  DPRxGetDRRFlag(void)
{
    Bool ucflag= FALSE;

    if(msReadByte( REG_1887)&BIT7)
    ucflag =TRUE;

    return ucflag;
}
//**************************************************************************
//  [Function Name]:
//                  DPRxGetPixelClk()
//  [Description]
//                  DPRxGetPixelClk
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
WORD  DPRxGetPixelClk(void)
{
    WORD xdata LS_Clock=0;
    WORD xdata Pixel_Clock=0;
    DWORD xdata Base_N=0;
    DWORD xdata Base_M=0;

    LS_Clock = 27 * msReadByte(REG_1880);
    DP_printData( "\r\DP Input : LS_Clock %d ", LS_Clock );
    DP_printData( "\r\DP Input : Lane %d ", msReadByte(REG_1881)&0x0F);
    Base_M = ((DWORD) msRead2Byte( REG_14C1 ) <<8) + msReadByte(REG_14C0);
    DP_printData( "\r\DP Input : Base_M %x ", Base_M );
    Base_N = ((DWORD)msRead2Byte( REG_14C4 ) <<8) + msReadByte(REG_14C3);
    DP_printData( "\r\DP Input : Base_N %x ", Base_N );
    Pixel_Clock = ((DWORD) Base_M * LS_Clock *100 ) / Base_N ;
    DP_printData( "\r\DP Input : Pixel_Clock %d ", Pixel_Clock );

    return Pixel_Clock;
}

//**************************************************************************
//  [Function Name]:
//                  DPRxGetPixelClk()
//  [Description]
//                  DPRxGetPixelClk
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
WORD  DPRxGetHTotal(void)
{
    return  msRead2Byte( REG_14B8 ) & 0x7FFF;
}

#endif
//**************************************************************************
//  [Function Name]:
//                  DPPrintState(BYTE state)
//  [Description]
//                  DPPrintState
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPPrintState( BYTE state )
{

    switch( state )
    {
        case eSTATE_STARTUP:
           DP_printMsg( "##  eSTATE_STARTUP! ##" );
            break;
        case eSTATE_CKECKLOCK:
            DP_printMsg( "## eSTATE_CKECKLOCK! ##" );
            break;
        case eSTATE_MEASURE:
            DP_printMsg( "## eSTATE_MEASURE!##" );
            break;
        case eSTATE_NORMAL:
            DP_printMsg( "## eSTATE_NORMAL! ##" );
            break;
        case  eSTATE_POWERSAVE:
             DP_printMsg( "## eSTATE_POWERSAVE! ##" );
            break;
        case eSTATE_DPIDLE:
          DP_printMsg( "## eSTATE_DPIDLE! ##" );
            break;
    }

}


//**************************************************************************
//  [Function Name]:
//                  DPRxSetAuxClock()
//  [Description]
//                  DPRxSetAuxClock
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxSetAuxClock(DP_AuxClock temp)
{
	WORD Timeout=0;

	//return;
	#if 0
	//msWriteByteMask( REG_022E, 0, BIT3 );        //OEN  GPIO_23
	//msWriteByteMask( REG_022C, BIT3, BIT3 );   //Set GPIO_23 High
	//msWriteByteMask( REG_022C, 0, BIT3 );        //Set GPIO_23 Low
       //msWriteByteMask( REG_022C, BIT3, BIT3 );   //Set GPIO_23 Hgh

	while(msReadByte(REG_06E9) !=0)
	{		
		if(++Timeout > 49990)    //49990  = 80msec
		{			
			break;	
		}	
	}
	#endif
	
	if(temp == FRO )//ENABLE_XTAL_LESS
	{	
	msWriteByteMask( REG_1A90, 0, BIT1|BIT2 ); 					  // Combo AUX
	msWriteByteMask( REG_0192, 0, BIT2|BIT3); 					  // Aux Clock SRC : FRO
	msWriteByte( REG_0612, 0x0E );                                                 	  // Aux UI Count
	msWriteByte( REG_0613, 0x0B );                                                         // Aux Sample Rate
	msWriteByte( REG_0614, 0x08 );                                              		  // Aux Time Out Low
	msWriteByte( REG_0615, 0x07 );                                                 	  // Aux Time Out  High 	     
	DP_printMsg( "## FRO ##" );
	}	
	else	 if(temp == Xstal)
	{
	msWriteByteMask( REG_1A90, BIT1,BIT1|BIT2 ); 					  // Combo AUX
	msWriteByteMask( REG_0192, BIT2, BIT2|BIT3); 					  // Aux Clock SRC : Live clk	
	msWriteByte( REG_0612, 0x10 );                                                 	  // Aux UI Count
	msWriteByte( REG_0613, 0x0D );                                                         // Aux Sample Rate
	msWriteByte( REG_0614, 0x63 );                                              		  // Aux Time Out Low
	msWriteByte( REG_0615, 0x08 );                                                 	  // Aux Time Out  High		
	DP_printMsg( "## Xstal ##" );
	}

	
}
//**************************************************************************
//  [Function Name]:
//                  DPRxInitState()
//  [Description]
//                  DPRxInitState
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRxInitState( void )
{
    BYTE xdata uncall = 1;

    #if 1 
     if(msReadByte( REG_0690)&BIT0)
    {        m_ucState = eSTATE_RETRAIN;

            //  if(msReadByte( REG_0690)&BIT0)
                  Set_HDCPPortBFlag();
            //  else
            //      Set_HDCPPortCFlag();

             msWriteByte( REG_0692 , BIT0);
             msWriteByte( REG_0692 , 0);
             DP_printMsg( "#  ENABLE HDCP ==>1" );
    }
    else  if(msReadByte( REG_18B2) !=0)
    {
              DP_printMsg( "#  ENABLE HDCP ==>2" );
              m_ucState = eSTATE_RETRAIN;
    }
    #endif
    if((HDCPPortBFlag)||(msReadByte( REG_18B2) !=0))
    {
    		 DP_printMsg( "#  ENABLE HDCP =a=>1" );
        m_ucState = eSTATE_RETRAIN;
    }	
    else
    {
    		 DP_printMsg( "#  ENABLE HDCP =a=>2" );
    m_ucState = eSTATE_STARTUP;
    }
    PowerSavingCnt = 0;
    LockFailCnt = 0;
    TrainingTimer = 0;
    g_DPDPCDPOWERSAVECNT = 0;
    TrainingP1Cnt = 0;                            // clear TrainingP1Cnt
    TrainingP2Cnt = 0;                            // clear TrainingP2Cnt
    g_bDPTrainingP0T = FALSE;             // clear flag
    g_bDPTrainingP1T = FALSE;             // clear flag
    gDPInfo.bAudioMute = 1;
    u16CDR_HPDCnt=0xFFFF;                         // Clear counter
    u16Skew_HPDCnt=0xFFFF;                        //Clear counter
    if( uncall == 0 )
    {
        WORD HDisplay;
        WORD VDisplay;
              DPCheckAuxState(10);
              DPRxClearAUXVliadStatus();
              DPRxHPDIRQ();
              DPRxAuxIRQEnable(TRUE);
              DP_GetSquelchPortB(1);
              DP_GetSquelchPortC(1);
              DPRxGetVB_ID();
              DPRxGetInputDE(&HDisplay, &VDisplay);
              DPFastLink(TRUE);
              DPAudioSSC();
              DPRxInitState();
        #if DP_DRRfunction
        DPRxEnableDRRFunction(FALSE);
        DPRxGetDRRFlag();
        DPRxGetPixelClk();
        DPRxGetHTotal();
        #endif


    }
}
//**************************************************************************
//  [Function Name]:
//                  DPRxCDRLossHander()
//  [Description]
//                  DPRxCDRLossHander
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
#if DPCTS
void DPRxCDRLossHander( void )
{

   if( g_bDPLoseCDRLockIRQ ) // Lock fail have occured
    {
        if( !DPRxCheckLock() )
        {
             if(( u16CDR_HPDCnt == 0 ) && ( (msReadByte( REG_1882 )&0x03) == 0 ) )
            {
			 msWriteByteMask( REG_1BB6, 0x0F , 0x0F );            //Power Down Lane
                       //msRegs[REG_1B1C] = BIT1|BIT2;                            //FIFO reset
                       //msRegs[REG_1B1C] = 0;                                      //FIFO reset
                         msRegs[REG_1660] = msRegs[REG_1660] |BIT1;                      //Auto EQ reset
                         msRegs[REG_1660] = msRegs[REG_1660] & (~BIT1);
                         DPRxHPDIRQ();                                                                              //CCJ mark 20090925
                         g_bDPLoseCDRLockIRQ = FALSE;                                                       //clear flag
			 msWriteByteMask( REG_1BB6, 0x00 , 0x0F );           //Power Down Lane
                         DP_printMsg("CDR_Loss");
            }
        }
        else
        {
            g_bDPLoseCDRLockIRQ = FALSE;         //clear flag
        }

    }
    if( g_bDPInterlaneSkewIRQ ) // Lock fail have occured
    {
        if( DPRxCheckLock() )
        {
        if(( msReadByte( REG_1882 )&0x03) == 0 )
            {
            //DPRXAutoEQCheck();
            }
            if(( u16Skew_HPDCnt == 0 ) && (( msReadByte( REG_1882 )&0x03) == 0 ) )
            {
                if( ! ( msReadByte( REG_18C4 )&_BIT7 ) ) // now is not in traning stage
                {
                  // if( (msReadByte( REG_0672 + Aux)&0x07)!=0x02)      //CCJ 20110727
                        DPRxHPDIRQ();                             //CCJ mark 20090925
                        DP_printMsg("Interlane_Loss");

                    g_bDPInterlaneSkewIRQ = FALSE; //clear flag
                }
                else
                {
                    g_bDPInterlaneSkewIRQ = FALSE; //clear flag

                }
            }
        }
        else
        {
            g_bDPInterlaneSkewIRQ = FALSE;      //clear flag

        }
    }

}
#endif
//**************************************************************************
//  [Function Name]:
//                  DPRxAudioHandle()
//  [Description]
//                  DPRxAudioHandle
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
#if SupportDPAudio
#define On_Debunce  3
#define Off_Debunce  80
void DPRxAudioHandle( void )
{
     DPAudioFreqType xdata ucCurInFreq;


    gDPInfo.bDPNoAudioStrm = ( msReadByte( REG_136B ) & 0x10 ) ? 1 : 0; // Read BK36 A5 Bit4:1 NoAudioStrm

#if 1
    if( gDPInfo.bDPNoAudioStrm )
    {
         HW_SET_AUDIO_MUTE_ENABLE();
        return;
    }
    else
    {
        if(( gDPInfo.uwPacketStatus & 0x06) ==  0x06)
        {
                HW_SET_AUDIO_MUTE_DISABLE();
        }
        else
        {
                HW_SET_AUDIO_MUTE_ENABLE();
                return;
        }
    }
#endif

      DPAudioMuteCheck();

    if( g_bDPLANELOCK == TRUE )
    {
              HLBR = msReadByte( REG_1880);

            if(( HLBR & 0x0F ) == 0x06 )
            {
                LinkRate = 162;
            }
            else
            {
                LinkRate = 270;
            }
         //################  Audio M/N example ###################
            //M=512*fs*N/LinkRate
            //N=0x8000=32768
            //32K,2.7G   => M=512*32K*32768/270000K=1988.4=0x07_C4
            //48K,2.7G   => M=512*48K*32768/270000K=2982.6=0x0B_A6
            //32K,1.62G => M=512*32K*32768/162000K=3314.0=0x0C_F2
            //48K,1.62G => M=512*48K*32768/162000K=4971.0=0x13_6B
            // ################################################

            msWriteByteMask(REG_1410,0,BIT0);
            DPAUM0 = msReadByte( REG_14C0 ); //MSA MAUD_CENTER[7:0]
            DPAUM1 = msReadByte( REG_14C1 ); //MSA MAUD_CENTER[15:8]
            DPAUM2 = msReadByte( REG_14C2 ); //MSA MAUD_CENTER[23:16]
            DPAUM = ( DPAUM2 << 16 ) + ( DPAUM1 << 8 ) + ( DPAUM0 );
            DPAUN0 = msReadByte( REG_14C3 ); //MSA NAUD_CENTER[7:0]
            DPAUN1 = msReadByte( REG_14C4 ); //MSA NAUD_CENTER[15:8]
            DPAUN2 = msReadByte( REG_14C5 ); //MSA NAUD_CENTER[23:16]
            DPAUN = ( DPAUN2 << 16 ) + ( DPAUN1 << 8 ) + ( DPAUN0 );
            msWriteByteMask(REG_1410,BIT0,BIT0);

            DPAUM_OLD =  (msReadByte( REG_145A ) <<16)  + (msReadByte( REG_1459 )<<8) + msReadByte( REG_1458 );
            DPAUN_OLD =  (msReadByte( REG_1462 ) <<16)  + (msReadByte( REG_1461 )<<8) + msReadByte( REG_1460 );

            if(abs( DPAUM - DPAUM_OLD) > 0x80)
             {
                    g_bDPAUPLLBIGChange = TRUE;
				//DP_printData( "\r\nDP_Audio_bigchange_Manual %x ", 0 );
             }

         ucCurInFreq = DPRxCheckAudioFreq();

             if( ucCurInFreq > DPAUD_FREQ_ERROR )
             {
                     if( g_bDPAUPLLBIGChange && ( !gDPInfo.bDPNoAudioStrm ) )
                       {
                                    msWriteByte( REG_1458, DPAUM0 ); //UPDATE MAUD_CENTER[7:0]
                                    msWriteByte( REG_1459, DPAUM1 ); //UPDATE MAUD_CENTER[15:8]
                                    msWriteByte( REG_145A, DPAUM2 ); //UPDATE MAUD_CENTER[23:16]
                                    msWriteByte( REG_1460, DPAUN0 ); //UPDATE NAUD_CENTER[7:0]
                                    msWriteByte( REG_1461, DPAUN1 ); //UPDATE NAUD_CENTER[15:8]
                                    msWriteByte( REG_1462, DPAUN2 ); //UPDATE NAUD_CENTER[23:16]

                                   HW_SET_AUDIO_MUTE_ENABLE();

                                   DP_printData( "\r\nDP_Audio_bigchange%x ", 0 );

                                    g_bDPAUPLLBIGChange = FALSE;        // claer IRQ flag

                                    DPFBDIV0 = msReadByte( REG_1A04 ); //FBDIV[7:0]
                                    DPFBDIV1 = msReadByte( REG_1A05 ); //FBDIV[15:8]
                                    DPFBDIV = ( DPFBDIV1 << 8 ) + DPFBDIV0;
                                    DPKP = GetPow2(( msReadByte( REG_1A00 ) >> 4 ) & 0x0F ); //KP
                                    DPKM = GetPow2((msReadByte( REG_1A01 )  & 0x0F )); //KM

            if(gDPInfo.ucAudioMode ==1)
            {
                                    DPFREQ = (64)  * ( 214.77 / LinkRate ) * ( DPKM * DPAUN ) / ( DPKP * DPAUM ) * DPFBDIV;
                                    DPFREQ0 = ( DPFREQ & 0x000000FF );
                                    DPFREQ1 = ( DPFREQ & 0x0000FF00 ) >> 8;
                                    DPFREQ2 = ( DPFREQ & 0x00FF0000 ) >> 16;
                                    msWriteByte( REG_1A08, DPFREQ0 );           //Freq
                                    msWriteByte( REG_1A09, DPFREQ1 );           //Freq
                                    msWriteByte( REG_1A0A, DPFREQ2 );           //Freq
                     msWriteByte( REG_1A07, 0x20 );           //update Freq
                                    msWriteByte( REG_1A07, 0x00 );           //update Freq
                                    msWriteByte( REG_1A0B, 0x0A );                  //set BW
            }
            else
            {
                     DPFREQ= (((( (DWORD) 432 * DPAUN )/LinkRate) *1024)/( DPAUM ))*128;
                     DPFREQ0 = ( DPFREQ & 0x000000FF );
                                    DPFREQ1 = ( DPFREQ & 0x0000FF00 ) >> 8;
                                    DPFREQ2 = ( DPFREQ & 0x00FF0000 ) >> 16;
                     msWriteByte( REG_1A44, DPFREQ0 );                     //Freq
                                    msWriteByte( REG_1A45, DPFREQ1 );                     //Freq
                                    msWriteByte( REG_1A46, DPFREQ2 );                     //Freq
                     msWriteByteMask( REG_1A41, BIT2,BIT2 );             //update Freq
                                    msWriteByteMask( REG_1A41, 0,BIT2 );                  //update Freq
                      }

                                    HW_SET_AUDIO_MUTE_DISABLE();
                                    DP_printData( "\AUD M0 %x", DPAUM0 );
                                    DP_printData( "AUD M1 %x", DPAUM1 );
                                    DP_printData( "AUD M2 %x", DPAUM2 );
                                    DP_printData( "AUD N0 %x", DPAUN0 );
                                    DP_printData( "AUD N1 %x", DPAUN1 );
                                    DP_printData( "AUD N2 %x", DPAUN2 );

                                    DP_printData( "DPFBDIV %x", DPFBDIV );
                                    DP_printData( "DPKP %x", DPKP );
                                    DP_printData( "DPKM %x", DPKM );

                                    DP_printData( "AUD Freq0 %x", DPFREQ0 );
                                    DP_printData( "AUD Freq1 %x", DPFREQ1 );
                                    DP_printData( "AUD Freq2 %x", DPFREQ2 );

                     }

                }


    }



 }

//**************************************************************************
//  [Function Name]:
//                  DPSSC()
//  [Description]
//                  DPSSC
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPAudioSSC(void)
{

}
//**************************************************************************
//  [Function Name]:
//                  DPRxCheckAudioFreq()
//  [Description]
//                  DPRxCheckAudioFreq
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
DPAudioFreqType DPRxCheckAudioFreq( void )    //2012-05-16
{
    DPAudioFreqType ucFreqVaild;
    WORD xdata wDPAudioFreq;

    wDPAudioFreq = ((((LinkRate*1000ul)+256ul)/512ul)*DPAUM)/DPAUN;
    //DP_printData("\r\n DP wDPAudioFreq %d", wDPAudioFreq);

    if( abs( wDPAudioFreq - 32 ) < 4 )
        ucFreqVaild = DPAUD_FREQ_32K;
    else if( abs( wDPAudioFreq - 44 ) < 4 )
        ucFreqVaild = DPAUD_FREQ_44K;
    else if( abs( wDPAudioFreq - 48 ) < 4 )
        ucFreqVaild = DPAUD_FREQ_48K;
    else if( abs( wDPAudioFreq - 88 ) < 4 )
        ucFreqVaild = DPAUD_FREQ_88K;
    else if( abs( wDPAudioFreq - 96 ) < 4 )
        ucFreqVaild = DPAUD_FREQ_96K;
    else if( abs( wDPAudioFreq - 176 ) < 4 )
        ucFreqVaild = DPAUD_FREQ_176K;
    else if( abs( wDPAudioFreq - 192 ) < 4 )
        ucFreqVaild = DPAUD_FREQ_192K;
    else
        ucFreqVaild = DPAUD_FREQ_ERROR;

        if((abs(ucFreqVaild_Old -wDPAudioFreq) >3)&&(ucFreqVaild > DPAUD_FREQ_ERROR))
       {
           //  DP_printData("  DP Audio Fs =  %d (KHZ)", wDPAudioFreq);
       }
        ucFreqVaild_Old = wDPAudioFreq;

    return ucFreqVaild;
}
//**************************************************************************
//  [Function Name]:
//                  DPAudioMuteCheck()
//  [Description]
//                  DPAudioMuteCheck
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
#if DP_AudioMuteEvent
void  DPAudioMuteCheck(void)
{
    if(msReadByte(REG_1ADE)&(BIT0|BIT1))
    {
             DP_printMsg("\r\nAudio Sample / Parity Error ");
             msWriteByteMask(REG_1ADE,BIT0|BIT1,BIT0|BIT1);

    }
     if(msReadByte(REG_1ADF)&(BIT0|BIT1))
    {
              DP_printMsg("\r\nAudio FIFO Unde/OverRUN ");
              msWriteByteMask(REG_1ADF,BIT0|BIT1,BIT0|BIT1);
    }
    if(msReadByte(REG_1756)&(BIT2))
    {
             DP_printMsg("\r\nNo Ls Clock");
             msWriteByte(REG_175A,BIT2);
    }
    if(msReadByte(REG_1757)&(BIT0))
    {
             DP_printMsg("\r\n Linke Rate Change");
             msWriteByte(REG_175B,BIT0);
    }

}
 #endif // DP_AudioMuteEvent
#endif
//**************************************************************************
//  [Function Name]:
//                  DPISR()
//  [Description]
//                  DPISR
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
//    SC_FMT_IN_NORMAL,
//    SC_FMT_IN_3D_PF,        /* page flipping */
//    SC_FMT_IN_3D_FP,        /* frame packing */
//    SC_FMT_IN_3D_FPI,       /* frame packing interlace */
//    SC_FMT_IN_3D_TB,        /* top bottom */
//    SC_FMT_IN_3D_SBSH,      /* side by side half */
//    SC_FMT_IN_3D_LA,        /* line interleave */
//    SC_FMT_IN_3D_SBSF,      /* side by side full */
//    SC_FMT_IN_3D_CB,        /* chess board */

 /*
void DPRx3DInform( void )
{
    BYTE ucSCFmtIn = SC_FMT_IN_NORMAL;

    gDPInfo.DP_3DMode = E_DP_3D_INPUT_NO_STEREO;

#if 0//Displayport12

    NoVscPktCnt++;

    if( msReadByte( REG_1F59) & BIT1 )
    {
          gDPInfo.DP_3DMode = E_DP_3D_INPUT_FRAME_SEQUENTIAL;
      ucSCFmtIn = SC_FMT_IN_3D_PF;
      NoVscPktCnt=0;
    }
    else if( Get_VSC_FLAG() )
    {
        if(( msReadByte( REG_21A4 ) & 0x0F ) == 0 )               //E_DP_3D_INPUT_NO_STEREO
        {
            gDPInfo.DP_3DMode = E_DP_3D_INPUT_NO_STEREO;
            ucSCFmtIn = SC_FMT_IN_NORMAL;
        }
        else if(( msReadByte( REG_21A4 ) & 0x0F ) == 0x01 )       //E_DP_3D_INPUT_FRAME_SEQUENTIAL
        {
            gDPInfo.DP_3DMode = E_DP_3D_INPUT_FRAME_SEQUENTIAL;
            ucSCFmtIn = SC_FMT_IN_3D_PF;
        }
        else if(( msReadByte( REG_21A4 ) & 0x0F ) == 0x02 )      //E_DP_3D_INPUT_STACK_FRAME
        {
            gDPInfo.DP_3DMode = E_DP_3D_INPUT_STACK_FRAME;
            ucSCFmtIn = SC_FMT_IN_3D_FP;
        }
        else if(( msReadByte( REG_21A4 ) & 0x0F ) == 0x03 )      //E_DP_3D_INPUT_PIXEL_INTERLEAVED
        {
            gDPInfo.DP_3DMode = E_DP_3D_INPUT_PIXEL_INTERLEAVED;
            ucSCFmtIn = SC_FMT_IN_3D_LA;

            if((( msReadByte( REG_21A4 ) >> 4 ) & 0x0F ) < 5 )
                gDPInfo.PAYLOAD = ( msReadByte( REG_21A4 ) >> 4 ) & 0x0F;
            else
                gDPInfo.PAYLOAD = E_DP_3D_DATA_RESERVED;

            if( gDPInfo.PAYLOAD == 0x02 )
            {
                ucSCFmtIn = SC_FMT_IN_3D_CB;
            }
        }
        else if(( msReadByte( REG_21A4 ) & 0x0F ) == 0x04 )          //E_DP_3D_INPUT_SIDE_BY_SIDE_FULL
        {
            gDPInfo.DP_3DMode = E_DP_3D_INPUT_SIDE_BY_SIDE_FULL;
            ucSCFmtIn = SC_FMT_IN_3D_SBSF;

            if((( msReadByte( REG_21A4 ) >> 4 ) & 0x0F ) < 2 )
                gDPInfo.PAYLOAD = ( msReadByte( REG_21A4 ) >> 4 ) & 0x03;
            else
                gDPInfo.PAYLOAD = E_DP_3D_DATA_RESERVED;
        }
        else
        {
            gDPInfo.DP_3DMode = E_DP_3D_RESERVED;
            ucSCFmtIn = SC_FMT_IN_NORMAL;
        }
        NoVscPktCnt = 0;
    }
 //DP_printData( " (ucSCFmtIn=%x)", ucSCFmtIn);
 //DP_printData( " ( g_SetupPathInfo.ucSCFmtIn=%x)", g_SetupPathInfo.ucSCFmtIn );
 //DP_printData( " (NoVscPktCnt=%x)", NoVscPktCnt);

    if((( ucSCFmtIn != g_SetupPathInfo.ucSCFmtIn ) && (( NoVscPktCnt == 0 ) || ( NoVscPktCnt >= 10 ) ) ) )
    {

        DP_printData( "DP 3D FMT CHG : %d", ucSCFmtIn );
        NoVscPktCnt = 0;
        g_SetupPathInfo.ucSCFmtIn = ucSCFmtIn;
        Set_InputTimingChangeFlag();
    }
#else
    if(!g_SetupPathInfo.bOverrideSCFmtIn)
    g_SetupPathInfo.ucSCFmtIn = SC_FMT_IN_NORMAL;
#endif

} */

//**************************************************************************
//  [Function Name]:
//                  DPISR()
//  [Description]
//                  DPISR
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
#define CTS_Patch  1
#define FRO_Patch  0
void DPISR( void )
{
	#if FRO_Patch
	BYTE uctemp=0,ucStatus0=0,ucStatus1=0;
	WORD CDR_Cnt=0;
	WORD  Delta= 0;
	#endif
    // ##############################################################
    // #############            Training P1          ################
    // ##############################################################
    if( msRegs[REG_1756] & BIT7 )         // Training P1
    {

    	#if (CHIP_ID==CHIP_TSUMF)
        Clr_DPAutoSythNewMode();					
   	#endif
	 DPRxSetEQCalValue();

	 msRegs[REG_1B68] = msRegs[REG_1B68]|((BIT1));   // AUTO EQ Time Out

	#if ((CHIP_ID==CHIP_TSUM9)|| (CHIP_ID==CHIP_TSUMF))
	 #if ENABLE_XTAL_LESS
	 msRegs[REG_1B90] =  0x0F;					   // Lock Loss threhold
	 msRegs[REG_1B91] =  0x20;
    	 #endif
    	 #endif
    	 
	 #if FRO_Patch
	 msRegs[REG_1B84] = 0xD0;
	 msRegs[REG_1B85] = 0x02;		
	 msRegs[REG_183B] = msRegs[REG_183B] + 1;
	 #endif
   		
         msRegs[REG_18E0] =(msRegs[REG_18E0] &(~(BIT2|BIT1)));          // Disable FT
           #if DP_INTERSKEWMORE
         msRegs[REG_1619] = msRegs[REG_1619] |BIT3;				    //Deinterskew setting
	  msRegs[REG_163B] = msRegs[REG_163B] &(~BIT3);	  
          #endif
#if DP_AUTOEQ
      msRegs[REG_1660] =msRegs[REG_1660]|BIT0;                      //Enable AUto MAC
    #if CTS_Patch
       msRegs[REG_1B4A] = msRegs[REG_1B4A]|(BIT2);
	   msRegs[REG_18F8] = msRegs[REG_18F8] |BIT2;				    //DPCD  symbol/eq?unlock
       msRegs[REG_18F8] = msRegs[REG_18F8] & (~BIT3);
    #endif
#endif
        g_bDPTrainingP0T = TRUE;
        TrainingP1Cnt++;

        CTSLevel = 0;
        DPChkCnt = 0;
        DPTP1Cnt = 0;

        msRegs[REG_1802] = 0x00;
        msRegs[REG_1803] = 0x00;
 

#if !DP_AUTOCR
#if 1// !DP_AUTOEQ
        while((( msRegs[REG_1882] & 0x03 ) == 0x01 ) && ( DPTP1Cnt < 200000 ) ) // while pattern 1 is On-Going
        {
		#if FRO_Patch
		uctemp =  (msRegs[REG_18C3]&0x38 ) << 1;
		ucStatus0 =  (uctemp | (msRegs[ REG_18C3 ]&0x07));
		uctemp =  (msRegs[ REG_18C4 ]&0x38 ) << 1;
		ucStatus1 =  (uctemp | (msRegs[ REG_18C4 ]&0x07));
		 if(( ucStatus0 == 0x11 ) && ( ucStatus1 == 0x11 ) )
		 {
			msRegs[REG_183C] = msRegs[REG_183C] + 1;
		 }
		 else
		 {
		 	msRegs[REG_183D] = msRegs[REG_183D] + 1;

				CDR_Cnt = msRegs[REG_1B79] <<8 + msRegs[REG_1B78]; 

				if(CDR_Cnt >= 0x2D0)
				{				 
					Delta = CDR_Cnt - 0x2D0; 	
				}
				else
				{
					Delta = 0x2D0 - CDR_Cnt;
				}
					msRegs[REG_1830] = Delta;
					msRegs[REG_1831] =( Delta>>8);	
				
				if(Delta < 0x15)
				{		
						msRegs[REG_183A] = msRegs[REG_183A] + 1;
						msRegs[REG_1B84] = msRegs[REG_1B78];
						msRegs[REG_1B85] = msRegs[REG_1B79];
				}
				else
				{
						msRegs[REG_1839] = msRegs[REG_1839] + 1;
				}
				
		 }
		#endif

        
            if(DPTP1Cnt > 199990)
         msRegs[REG_1882] = 0x00;

            DPChkCnt++;
            DPTP1Cnt++;
            switch( msRegs[REG_1881] & 0x07 )
            {
                case 0x01:  // 1 lane
                    if(( msRegs[REG_1883] & 0x03 ) == ( msRegs[REG_1802] & 0x03 ) ) // request reach
                        {
                            if(( msRegs[REG_18C3] & 0x01 ) != 0x01 ) // 1 lane LOCK Fail
                            {
                                {
                                    if( CTSLevel < 3 )
                                        CTSLevel++;  // update swing level
                                    msRegs[REG_1802] =( msRegs[REG_1802] & 0x0C) | ( CTSLevel & 0x03 );
                                }
                            }
                        }
                    break;
                case 0x02:  // 2 lanes
                    if(( msRegs[REG_1883] & 0x03 ) == ( msRegs[REG_1802] & 0x03 ) ) // request reach
                        {
                        if(( msRegs[REG_1884] & 0x03 ) == (( msRegs[REG_1802] & 0x30 ) >> 4 ) ) // request reach
                         {
                            if(( msRegs[REG_18C3] & 0x09 ) != 0x09 ) // 2 lanes CDR LOCK Fail
                            {
                                {
                                    if( CTSLevel < 3 )
                                        CTSLevel++;  // update swing level
                                    msRegs[REG_1802] =( msRegs[REG_1802] & 0xCC) | ((( CTSLevel & 0x03 ) << 4 ) | ( CTSLevel & 0x03 ) );

                                }
                            }
                            }
                        }
                    break;
                case 0x04:  // 4 lanes

                    if(( msRegs[REG_1883] & 0x03 ) == ( msRegs[REG_1802] & 0x03 ) ) // request reach
                      {
                        if(( msRegs[REG_1884] & 0x03 ) == (( msRegs[REG_1802] & 0x30 ) >> 4 ) ) // request reach
                          {
                            if(( msRegs[REG_1885] & 0x03 ) == ( msRegs[REG_1803] & 0x03 ) ) // request reach
                              {
                                if(( msRegs[REG_1886] & 0x03 ) == (( msRegs[REG_1803] & 0x30 ) >> 4 ) ) // request reach
                                  {
                                    if((( msRegs[REG_18C3] & 0x09 ) != 0x09 ) | (( msRegs[REG_18C4] & 0x09 ) != 0x09 ) ) // 4 lanes CDR LOCK Fail
                                    {

                                            if( CTSLevel < 3 )
                                                CTSLevel++;  // update swing level
                                            msRegs[REG_1802] =(msRegs[REG_1802] & 0xCC) | ((( CTSLevel & 0x03 ) << 4 ) | ( CTSLevel & 0x03 ) );
                                            msRegs[REG_1803] = (msRegs[REG_1803] & 0xCC) | ((( CTSLevel & 0x03 ) << 4 ) | ( CTSLevel & 0x03 ) );

                                        }
                                    }
                                 }
                             }
                          }
                    break;
                default:

                    break;
            }
        }
   #endif
 #endif
     DPRxClrSQTime();

   	#if DP_INTERSKEWMORE
     msRegs[REG_1B1C] = BIT0;                             //FIFO reset
     msRegs[REG_1B1C] = 0;                        //FIFO reset
	#endif

        msRegs[REG_175A] = BIT7; // clear IRQ
        msRegs[REG_175A] = 0;       // clear IRQ
    }

    // ##############################################################
    // #############            Training P2          ################
    // ##############################################################
    if( msRegs[REG_1756] & BIT6 )           // Training P2
    {
        BYTE AEQ_done = 0;
        BYTE uctemp = 0;
        g_bDPTrainingP1T = TRUE;
        TrainingP2Cnt++;
        CTSLevel = 0;
        DPTP2Cnt = 0;

	#if ((CHIP_ID==CHIP_TSUM9)|| (CHIP_ID==CHIP_TSUMF))
	 #if ENABLE_XTAL_LESS
	if(  msRegs[REG_1880] == 0x0A )
	{
		msRegs[REG_1B84] = msRegs[REG_1B78];
		msRegs[REG_1B85] = msRegs[REG_1B79];
	}
	else
	{
		msRegs[REG_1B86] = msRegs[REG_1B78];
		msRegs[REG_1B87] = msRegs[REG_1B79];	
	}
	#endif
	#if (CHIP_ID==CHIP_TSUM9)
	msRegs[REG_1B90] =  0x06;   // Lock Loss threhold
	msRegs[REG_1B91] =  0x10;
	#elif (CHIP_ID==CHIP_TSUMF)
	msRegs[REG_1B90] =  0x06;   // Lock Loss threhold
	msRegs[REG_1B91] =  0x10;
	#endif		
	#endif


        #if !DP_AUTOEQ

        msRegs[REG_1802] = msRegs[REG_1802] & 0x33;
        msRegs[REG_1803] = msRegs[REG_1803] & 0x33;
        while((( msRegs[REG_1882] & 0x03 ) == 0x02 ) && ( DPTP2Cnt < 200000 ) ) // while pattern 2 is On-Going
        {
            if(DPTP2Cnt > 199990)
            msRegs[REG_1882] = 0x00;

            DPChkCnt++;
            DPTP2Cnt++;
            switch( msRegs[REG_1881] & 0x07 )
            {
                case 0x01:  // 1 lane
                    if((( msRegs[REG_1883] & 0x18 ) >> 3 ) == (( msRegs[REG_1802] & 0x0C ) >>2) ) // request reach
                      {
                            if(( msRegs[REG_18C3] & 0x06 ) != 0x06 ) // 1 lane SYMBOL & EQ LOCK Fail
                            {
                                if( CTSLevel < 4 )
                                    CTSLevel++;  // update swing level
                                    msRegs[REG_1802] = (msRegs[REG_1802] & 0x03) | (( CTSLevel & 0x03 ) << 2 );

                            }
                        }
                    break;
                case 0x02:  // 2 lanes
                    if((( msRegs[REG_1883] & 0x18 ) >> 3 ) == (( msRegs[REG_1802] & 0x0C )>>2) ) // request reach
                     {
                            if((( msRegs[REG_1884] & 0x18 ) >> 3 ) == (( msRegs[REG_1802] & 0xC0 ) >> 6 ) ) // request reach
                              {
                                if(( msRegs[REG_18C3] & 0x36 ) != 0x36 ) // 2 lanes SYMBOL & EQ LOCK Fail
                                {
                                    if( CTSLevel < 4 )
                                        CTSLevel++;  // update swing level
                                        msRegs[REG_1802] = (msRegs[REG_1802] & 0x33) | ((( CTSLevel & 0x03 ) << 6 ) | (( CTSLevel & 0x03 ) << 2 ) );
                                }
                            }
                        }
                    break;
                case 0x04:  // 4 lanes
                    if((( msRegs[REG_1883] & 0x18 ) >> 3 ) == (( msRegs[REG_1802] & 0x0C )>>2 )) // request reach
                     {
                        if((( msRegs[REG_1884] & 0x18 ) >> 3 ) == (( msRegs[REG_1802] & 0xC0 ) >> 6 ) ) // request reach
                          {
                            if((( msRegs[REG_1885] & 0x18 ) >> 3 ) == (( msRegs[REG_1803] & 0x0C )>>2 )) // request reach
                              {
                                if((( msRegs[REG_1886] & 0x18 ) >> 3 ) == (( msRegs[REG_1803] & 0xC0 ) >> 6 ) ) // request reach
                                  {
                                    if((( msRegs[REG_18C3]&0x36 ) != 0x36 ) || (( msRegs[REG_18C4]&0x36) != 0x36 ) ) // 4 lanes SYMBOL & EQ LOCK Fail
                                    {
                                        if( CTSLevel < 4 )
                                            CTSLevel++;  // update swing level
                                        msRegs[REG_1802] = (msRegs[REG_1802] & 0x33) | ((( CTSLevel & 0x03 ) << 6 ) | (( CTSLevel & 0x03 ) << 2 ) );
                                        msRegs[REG_1803] = (msRegs[REG_1803] & 0x33) | ((( CTSLevel & 0x03 ) << 6 ) | (( CTSLevel & 0x03 ) << 2 ) );
                                    }
                               }
                             }
                         }
                        }
                    break;
                default:

                    break;
            }
        }
    #else
    #if CTS_Patch
           while((( msRegs[REG_1882] & 0x03 ) == 0x02 ) && ( DPTP2Cnt < 50000 )) // while pattern 2 is On-Going
         {

                if(DPTP2Cnt > 49990)
                msRegs[REG_1882] = 0x00;
                   DPTP2Cnt++;


        if( msRegs[REG_1671] & BIT7 )   //MAC Auto EQ done
        {
                AEQ_done = 0x01;

                    switch( msRegs[REG_1881] & 0x07 )
                    {
                        case 0x01:  // 1 lane

                            if(msRegs[REG_18C3] & BIT2)
                            {
		                    		
		                       	  //msRegs[REG_18F8] = msRegs[REG_18F8] & (~(BIT2|BIT3));
                            }
                            else
                            {

                          msRegs[REG_1660] = msRegs[REG_1660]&(~BIT0);
                          uctemp = 0x03 - msRegs[REG_1883]&0x03;
                          msRegs[REG_1802] =   msRegs[REG_1802] | (uctemp << 2);
						  //msRegs[REG_18F8] = msRegs[REG_18F8] & (~(BIT2|BIT3));
                            }

                        break;

                        case 0x02:  // 2 lanes

                            if((msRegs[REG_18C3] & BIT2)&& (msRegs[REG_18C3] & BIT5))
                            {
		                       	   //msRegs[REG_18F8] = msRegs[REG_18F8] & (~(BIT2|BIT3));
                            }
                            else
                            {
                          msRegs[REG_1660] = msRegs[REG_1660]&(~BIT0);
                          uctemp = 0x03 - msRegs[REG_1883]&0x03;
                          msRegs[REG_1802] =msRegs[REG_1802] |( uctemp << 2);
                          msRegs[REG_1802] =msRegs[REG_1802] |( uctemp << 6);
 					         //msRegs[REG_18F8] = msRegs[REG_18F8] & (~(BIT2|BIT3));	
                            }



                        break;

                        case 0x04:  // 4 lanes
                           if((msRegs[REG_18C3] & BIT2)&&(msRegs[REG_18C3] & BIT5)&&(msRegs[REG_18C4] & BIT2)&&(msRegs[REG_18C4] & BIT5))
                            {
		                       	 //msRegs[REG_18F8] = msRegs[REG_18F8] & (~(BIT2|BIT3));
                            }
                            else
                            {
                          msRegs[REG_1660] = msRegs[REG_1660]&(~BIT0);
                          msRegs[REG_1B4A] = msRegs[REG_1B4A]&(~BIT2);
                          uctemp = 0x03 - msRegs[REG_1883]&0x03;
                          msRegs[REG_1802] =msRegs[REG_1802] |( uctemp << 2);
                          msRegs[REG_1802] =msRegs[REG_1802] |( uctemp << 6);
                          msRegs[REG_1803] =msRegs[REG_1803] |( uctemp << 2);
                          msRegs[REG_1803] =msRegs[REG_1803] |( uctemp << 6);
						  //msRegs[REG_18F8] = msRegs[REG_18F8] & (~(BIT2|BIT3));
                            }

                        break;

                        default:

                            AEQ_done = 0x00;

                        break;
                    }
                    msRegs[REG_18F8] = msRegs[REG_18F8] & (~(BIT2|BIT3));
        }
        }

    #endif
    #endif



     DPRxClrSQTime();
        msRegs[REG_175A] = BIT6;            // clear IRQ
        msRegs[REG_175A] = 0;                 // clear IRQ
    }

    // ##############################################################
    // #############            Training Rate     ################################
    // ##############################################################
    if( msRegs[REG_1757] & BIT0 )       // Rate
    {
	      #if (CHIP_ID==CHIP_TSUM9)		
		
		if(gDPInfo.ucDPStates == 0)
		{
		       if(msRegs[REG_1880]==0x0A)
			{
				MEM_MSWRITE_BYTE(REG_1FB4,wHF1_L0);
				MEM_MSWRITE_BYTE(REG_1FB5,wHF1_L0>>8);
				MEM_MSWRITE_BYTE(REG_1FB6,wHF1_L1);
				MEM_MSWRITE_BYTE(REG_1FB7,wHF1_L1>>8);
			}
			else
			{
				MEM_MSWRITE_BYTE(REG_1FB4,wHF2_L0);
				MEM_MSWRITE_BYTE(REG_1FB5,wHF2_L0>>8);
				MEM_MSWRITE_BYTE(REG_1FB6,wHF2_L1);
				MEM_MSWRITE_BYTE(REG_1FB7,wHF2_L1>>8);
			}
		}	
    		#endif
        msRegs[REG_175B] = BIT0;
        msRegs[REG_175B] = 0;
    }

    // ***************************************************
    // ****************   Video MN Big Change  ***************
    // ***************************************************
    if( msRegs[REG_1756] & BIT1 ) // VPLL MN BIG CHANGE
    {
        // DP_Data[9] =DP_Data[9]+1;
        g_bDPVPLLBIGChange = TRUE;
        msRegs[REG_175A] = BIT1;            // clear IRQ
        msRegs[REG_175A] = 0;
    }

    // ***************************************************
    // ****************   Audio MN Big Change  **************
    // ***************************************************
    if( msRegs[REG_1756] & BIT0 ) // AU MN BIG CHANGE
    {
        //DP_Data[9] =DP_Data[9]+1;
        g_bDPAUPLLBIGChange = TRUE;
        msRegs[REG_175A] = BIT0;                     // clear IRQ
        msRegs[REG_175A] = 0;                           // clear IRQ
    }

	// ***************************************************
	// ****************   MSA  Change  **************
	// ***************************************************
	if( msRegs[REG_1757] & BIT4 ) // AU MN BIG CHANGE
	{
	    // msRegs[REG_183B] = msRegs[REG_183B] + 1;

	     msRegs[REG_175B] = BIT4;                     // clear IRQ
	     msRegs[REG_175B] = 0;                           // clear IRQ
	
		msRegs[REG_1411] = msRegs[REG_1411] |BIT4;       		              // clear IRQ
		msRegs[REG_1411] = msRegs[REG_1411] & (~BIT4);                             // clear IRQ

	}	
    //****************************************************
    // ***************************************************
    // ****************   DPENABLEINTERLANESKEW ***********
    // ***************************************************
    if( msRegs[REG_1757] & BIT1 ) // InterlaneSkew
    {
        g_bDPInterlaneSkewIRQ = TRUE;
        SetSkew_HPDCnt(1500);
        //u16Skew_HPDCnt = 1500;// Add by CCJ
        //bSkew_HPDCntFlag=1;
	   // msRegs[REG_183B] = msRegs[REG_183B] + 1;
	#if (CHIP_ID==CHIP_TSUM9)
            #if DP_INTERSKEWMORE
           msRegs[REG_1619] = msRegs[REG_1619] |BIT3;				    //DPCD  symbol/eq?unlock
	    msRegs[REG_163B] = msRegs[REG_163B] |BIT3;	  
           #endif

	    #if !DP_INTERSKEWMORE
          switch( msRegs[REG_1881] & 0x07)
          {

            case 0x04:
         if(!((msRegs[REG_166F]&BIT7)&&((msRegs[REG_1670]&(BIT0|BIT1)) ==(BIT0|BIT1))))
           {
		   		//msRegs[REG_183A] = msRegs[REG_183A]+1 ;   
                msRegs[REG_18E7] = msRegs[REG_18E7] |BIT0;
                    msRegs[REG_1B1C] = BIT0;                              //FIFO reset
                msRegs[REG_1B1C] = 0;                                //FIFO reset
                msRegs[REG_18E7] = msRegs[REG_18E7] & (~BIT0);
           }
         break;
         case 0x02:
          if(!((msRegs[REG_166F]&BIT7)))
           {
		   		//msRegs[REG_183B] = msRegs[REG_183B]+1 ;   
                msRegs[REG_18E7] = msRegs[REG_18E7] |BIT0;
                    msRegs[REG_1B1C] = BIT0;                              //FIFO reset
                msRegs[REG_1B1C] = 0;                                //FIFO reset
                msRegs[REG_18E7] = msRegs[REG_18E7] & (~BIT0);
           }
         break;
         case 0x01:
         default:


         break;
              }
	  
		   	#endif 
	   #endif 


        msRegs[REG_175B] = BIT1;            // clear IRQ
        msRegs[REG_175B] = 0;                 // clear IRQ
     }

    //========================================
    // ***************************************************
    // ****************   DPENABLELOSECDRLOCK **************
    // ***************************************************
    if( msRegs[REG_1757] & BIT2 ) // CDRLoseLock
    {
        g_bDPLoseCDRLockIRQ = TRUE;
        SetCDR_HPDCnt(1500);
        //u16CDR_HPDCnt = 1500;
        //bCDR_HPDCntFlag=1;
        msRegs[REG_175B] = BIT2;            // clear IRQ
        msRegs[REG_175B] = 0;
    }


    // ***************************************************
    // ****************   270h                  **************
    // ***************************************************

    if( msRegs[REG_1757] & BIT7 ) // CDRLoseLock
    {

		  //msRegs[REG_183A] = msRegs[REG_183A] +1;
        if( msRegs[REG_1834]&BIT7)
        {
               gDPInfo.bPhyTest =1;
               msRegs[REG_1680] = 0x40;
               msRegs[REG_1681] =    0xA7;
               msRegs[REG_1694] = 0x01;
               msRegs[REG_1695] = 0x10;
               msRegs[REG_1800] =msRegs[REG_1800]&(~BIT4);
               msRegs[REG_1649] =msRegs[REG_1649]&(~(BIT2|BIT3));
               msRegs[REG_1680] =msRegs[REG_1680]&(~BIT6);

               msRegs[REG_1660] =msRegs[REG_1660]|BIT0;                     //Recovery
               msRegs[REG_18E4] =msRegs[REG_18E4]&(~BIT3);                  //Recovery
               msRegs[REG_18E0] =msRegs[REG_18E0]&(~(BIT0|BIT1|BIT2));      //Recovery
               msRegs[REG_1BD7] =msRegs[REG_1BD7]&(~(BIT3|BIT1|BIT2));      //Recovery


        }
        else
        {

               gDPInfo.bPhyTest =0;
               msRegs[REG_1680] = 0x40;
               msRegs[REG_1681] =    0xB7;
               msRegs[REG_1694] = 0x00;
               msRegs[REG_1695] = 0x10;
               msRegs[REG_1800] =msRegs[REG_1800]|BIT4;
               msRegs[REG_1649] =msRegs[REG_1649]|((BIT2|BIT3));
               msRegs[REG_1680] =msRegs[REG_1680]|BIT6;
        }

        msRegs[REG_175B] = BIT7;            // clear IRQ
        msRegs[REG_175B] = 0;
    }



}

//**************************************************************************
//  [Function Name]:
//                  DPAUXMeasure()
//  [Description]
//                  DPAUXMeasure
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPISR_AUX0(void)
{


            // ##############################################################
            // #############          MCCS Interrupt            ############################
            // ##############################################################

                #if DPENABLEMCCS
                 msDP_DDC2BI() ;
                #endif

            // ##############################################################
            // #############            AUX DPCD Interrupt  1 ############################
            // ##############################################################
              #if 0
                if( msRegs[REG_0690 ] & BIT0 )
                {
                    Set_HDCPPortBFlag();
                    msRegs[REG_183A] =     msRegs[REG_183A] +1;	
                    msRegs[REG_0692 ] = BIT0;    //Clear
                    msRegs[REG_0692 ] = 0;          //Clear
                }
              #endif
            // ##############################################################
            // #############            AUX DPCD Interrupt  2 ############################
            // ##############################################################
             #if 0
                if( msRegs[REG_0690 ] & BIT1 )
                {
                    BYTE xdata  i=0,Length=0;
              Length = msRegs[REG_063A];

              for(i=0;i<Length;i++)
              {
                        msRegs[REG_06F3 ] = i;
                        msRegs[REG_06F2 ] = BIT0;
                         while( ! (((msRegs[REG_06F2])&BIT2)))      // Wait
                         {}
                        msRegs[REG_06F1 ] = BIT1;                      // Clear
              }
                    msRegs[REG_0692 ] = BIT1;    //Clear
                }
             #endif
            // ##############################################################
            // #############            AUX Interrupt  Port1 ############################
            // ##############################################################

                if( msRegs[REG_0678 + Aux] & BIT4 )
                {
                   // msRegs[REG_1839] =     msRegs[REG_1839] +1;
                    msRegs[REG_0679 + Aux ] = BIT4;    //Clear
                    msRegs[REG_0679 + Aux ] = 0;          //Clear
                }

            // ##############################################################
            // #############            Aux Time Out             ############################
            // ##############################################################

             #if DP_Aux_TimeOut
                if( msRegs[REG_0690 + Aux] & BIT4 )
                {
                       if(msRegs[REG_06E8 + Aux]&0x07)
                      {
                         msRegs[REG_1839] =     msRegs[REG_1839] +1;	
                         msRegs[REG_065C+ Aux]=0;
                         msRegs[REG_065C+ Aux]=BIT0;
                      }
                     msRegs[REG_0692 + Aux] = BIT4;     // Clear IRQ
                     msRegs[REG_0692 + Aux] = 0;           // Clear IRQ
     }
    #endif
}

//**************************************************************************
//  [Function Name]:
//                  DPRxGetInputDE()
//  [Description]
//                  DPRxGetInputDE
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool DPRxGetInputDE(WORD* pHDE, WORD* pVDE)     //111027 Rick add for display DP InputDE on information menu - B39608
{
    Bool xdata bstatus = FALSE;


    if(m_ucState == eSTATE_NORMAL)
    {
        bstatus = TRUE;

        *pHDE = msRead2Byte(REG_1366)&0x7FFF;
        *pVDE = msRead2Byte(REG_14BE)&0x7FFF;

        //DP_printData(" [DP HDE] =%d", *pHDE );
        //DP_printData(" [DP VDE] =%d", *pVDE );
    }
    return bstatus;


}
//**************************************************************************
//  [Function Name]:
//                  drvmStar_SetupInputPort_DisplayPort()
//  [Description]
//                  drvmStar_SetupInputPort_DisplayPort
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
#if ENABLE_DP_INPUT
#define OffLine 0
void drvmStar_SetupInputPort_DisplayPort(void)
{
    BYTE xdata i=0;
    BYTE xdata Count=20;

        if(msReadByte(REG_1806)&BIT0)      //For TESTing
        return;
        msWriteByteMask(REG_1A99, 0, BIT6); // [6]: reg_hdmi_sd_pix_clk_sel
        DPRxOutputEnable( TRUE );
        #if 0
        msWriteByteMask( REG_1754,BIT0,BIT0);     //reset
        msWriteByteMask( REG_1754,0,BIT0);
        //=======================================================
        for(i=0;i<sizeof(tComboDPMuxTbl)/sizeof(RegTbl2Type);i++)
        msWrite2ByteMask(tComboDPMuxTbl[i].wReg,tComboDPMuxTbl[i].wValue,tComboDPMuxTbl[i].wMask );
        DP_printMsg("==>Load DPTBL");
        //=======================================================
        //      Scaler BK14 Setting
        //=======================================================
        msWriteByteMask( REG_14D5, BIT3, BIT3|BIT2 );                                                            // Set I2S sample width
        msWriteByteMask( REG_1427, BIT0 , BIT0);                                                                      // Set DP Audio Source (MHL)
        msWriteByteMask( REG_142A, BIT4 , BIT4);                                                                      //FiFo en
        msWriteByteMask( REG_1403, 0 , BIT3);                                                                            // AV mute Disable
        msWriteByteMask( REG_1410,BIT0,BIT0);                                                                          //Set Video M/N
        // ##############################################
        //      BANK 13  Setting
        // ##############################################
        msWriteByte( REG_1320, 0x21 );                                                                              // Htotal Tolerance
        msWriteByte( REG_1307, 0x3F );
        msWriteByte( REG_1308, 0x1F );
        msWriteByte( REG_1304, 0xCA );                                                                                      //DP Interlace mode // BIT0 =0 (640X480 issue)
        msWriteByteMask( REG_1326, BIT7|BIT6, BIT7|BIT6|BIT0);
        msWriteByte( REG_1301, 0x61);                                                                                       //(bit3=0 use VB_ID rising as frame flag (end of active))
        msWriteByte( REG_1302, 0x07);
        msWriteByte( REG_1303, 0x01);
        msWriteByteMask( REG_1304, 0, BIT4);
        msWriteByteMask( REG_1322, BIT5, BIT5 );
        // ##############################################
        //     BANK 16 Setting
        // ##############################################
        msWriteByteMask( REG_1649, BIT6|BIT5|BIT3|BIT2, BIT6|BIT5|BIT3|BIT2|BIT1 );
        msWriteByte( REG_1644, 0x77);                                                                                         //EQ done is equal to symbol lock
        msWrite2Byte( REG_16C0,0x0168);                                                                                     //CR lock value for RBR
    #endif
     //=======================================================
        //      Scaler SC0 Setting
        //===============  =======================================
        msWriteByte(SC0_02, NIS_B|SCALER_STYPE_HV|SCALER_ISEL_DP);                                   // Set HV seperate
        msWriteByteMask(SC0_03, _BIT7, _BIT7);                                                                         // Force Input Clock Divide Function.
        msWriteByte( SC0_04, BIT3 |BIT4| BIT7 );                                                                         //  Set HV mode
        msWriteByteMask(SC0_A6, 0, _BIT1);                                                                               // Dual in
        msWriteByteMask(SC0_A8, _BIT1|BIT0, _BIT1|BIT0);                                                        // Interlace method
        //msWriteByteMask(SC0_E6, _BIT6, _BIT6);                                                                         // DE Follow mode
        msWriteByteMask(SC0_E7, _BIT5, _BIT5);                                                                         // Htt Filter
        msWriteByteMask(SC0_E8, _BIT5|_BIT4|_BIT3|_BIT2, _BIT5|_BIT4|_BIT3|_BIT2);            // Override H/H sync
        msWriteByte( SC0_F3, 0xF4 );                                                                                            // enable DE glitch removal function & more tolerance for DE
       // msWriteByteMask(SC0_7A, 0, _BIT6);        											//AVmute for HDMO\I  and  Vsyn deglitch  //20150304 for Bench 

        //=======================================================
        //      Scaler BK1E Setting
        //=======================================================
        msWriteByteMask( REG_1E22, 0, BIT3 );                                                                            // Disable DE-only mode for SC_TOP
        msWriteByteMask( REG_1E3E, (CLK_DVI_BUFF_DIV2<<4), 0xF0 );                                       //Clock source Select
        //=======================================================
        //      Scaler BK2E Setting
        //=======================================================
        msWriteByteMask( REG_2E02, BIT4, 0xF0 );                                                                      // IPmux 2    for DP
        msWriteByteMask( REG_2E04, BIT0, 0x0F );                                                                      // IPmux 3    for DP
        //=======================================================
        //=======================================================
         msWriteByteMask( REG_067D, 0, BIT2 );
         msWriteByteMask( REG_06C2, 0, BIT2|BIT3 );  //Set Offline off
         //=======================================================
         //       // FIX EQ
         //=======================================================
         msWrite2Byte( REG_1BDC, 0x03C0 );                       // FIX EQ
         msWrite2Byte( REG_1BC0, 0x0808 );                        //EQ ==08
         msWrite2Byte( REG_1BC2, 0x0808 );


         DPRxInitState();
         DPRxIRQEnable(TRUE);
         DPRxAuxIRQEnable( TRUE );
}
#endif

//**************************************************************************
//  [Function Name]:
//                  DPRxInterlaceCheck()
//  [Description]
//                  DPRxInterlaceCheck
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
#if DP_ASTRO_INTERLACE_PATCH
void DPRxInterlaceCheck(void)//Mike 130208 Modify it.
{
    WORD uwHDE,uwVDE;

    if(m_ucState == eSTATE_NORMAL)
    {
        uwHDE = msRead2Byte(REG_1366)&0x7FFF;
        uwVDE = msRead2Byte(REG_14BE)&0x7FFF;

        if ((DPRxGetVB_ID()&BIT2)==0
            && ((uwHDE%720==0 && uwVDE==480/2 ) || (uwHDE%720==0 && uwVDE==576/2) || (uwHDE==1920 && uwVDE==1080/2))) //480i/576i/108oi
        {//override
            if (msReadByte(SC0_E8)&(_BIT1|_BIT0)) //Chroma 2233 Patch
            {
                msWriteByteMask(SC0_A8, BIT0|BIT1, BIT0|BIT1);
                msWriteByteMask(SC0_E8, BIT0|BIT1, BIT0|BIT1);
            }
            else
            {
                msWriteByteMask(REG_1326, BIT1|BIT0, BIT1|BIT0);// set interlace ov //Mike 130208 confirm with CCJ.
                msWriteByteMask(SC0_A8, 0, BIT0|BIT1);
            }
        }
        else if (//((DPRxGetVB_ID()&BIT2)!=0 ) //Chroma 2233 Patch
             ((uwHDE%720==0 && uwVDE==480/2 ) || (uwHDE%720==0 && uwVDE==576/2) || (uwHDE==1920 && uwVDE==1080/2))
            )
        {   //override 480i issue
            if (!(msReadByte(SC0_E8)&(_BIT1|_BIT0)))
            {
                msWriteByteMask(SC0_A8, BIT0|BIT1, BIT0|BIT1);
                msWriteByteMask(SC0_E8, BIT0|BIT1, BIT0|BIT1);
            }
        }
        else
        {//restore
            msWriteByteMask(REG_1326, 0, BIT1|BIT0);// clr interlace ov //Mike 130208 confirm with CCJ.
            msWriteByteMask(SC0_A8, BIT0|BIT1, BIT0|BIT1);
        }
    }
}
#endif






//**************************************************************************
//  [Function Name]:
//                  DPRxATIInterlaceCheck()
//  [Description]
//                  DPRxATIInterlaceCheck
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
#if DP_ATI_GRAPHIC_CARD_INTERLACE_PATCH//Mike 130204 For the abnormal image when user use 1080i timing on ATI graphic card.
void DPRxATIInterlaceCheck(void)
{
    WORD uwHDE,uwVDE;

    if(m_ucState == eSTATE_NORMAL)
    {
        uwHDE = msRead2Byte(REG_1366)&0x7FFF;
        uwVDE = msRead2Byte(REG_14BE)&0x7FFF;

        if ((DPRxGetVB_ID()&BIT2) && ((uwHDE==1920 && uwVDE==1080/2)))
        {
            msWriteByteMask(REG_130A, 0, BIT0);
            scWriteByte(SC0_00, 0x00);
            scWriteByte(SC0_00, 0x0D);
            scWriteByte(SCD_64,(scReadByte(SCD_64)|BIT2));
            scWriteByte(SCD_65,(scReadByte(SCD_65)|BIT2));
            Delay1ms(10);
            scWriteByte(SCD_64,scReadByte(SCD_64)&(~BIT2));
            scWriteByte(SCD_65,scReadByte(SCD_65)&(~BIT2));
            Delay1ms(50);
            if ((scReadByte(SCD_64)&BIT1) || (scReadByte(SCD_65)&BIT1))
                msWriteByteMask(REG_130A, BIT0, BIT0);
            scWriteByte(SC0_00, 0x00);
        }
    }
}
#endif
//**************************************************************************
//  [Function Name]:
//                  DPRxGetVB_ID()
//  [Description]
//                  DPRxGetVB_ID
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE DPRxGetVB_ID(void)
{
    return msReadByte(REG_136B);// REG_1420); ????
}

//**************************************************************************
//  [Function Name]:
//                  DPAux_Reset()
//  [Description]
//                  DPAux_Reset
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPAux_Reset(void)
{
               msWriteByteMask( REG_065C, 0,BIT0);
               msWriteByteMask( REG_065C, BIT0,BIT0);
}

//**************************************************************************
//  [Function Name]:
//                  DPRXPMForceEnter()
//  [Description]
//                  DPRXPMForceEnter
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
#if  MS_PM
void DPRXPMForceEnter()
{
    g_bDPPowerDown = FALSE;
    m_ucState = eSTATE_POWERSAVE;
    DPRxHandle();
}
#endif
#else
BYTE code msdrvDPRxAppNullData[] = {0};
void msdrvDPRxAppDummy(void)
{
    BYTE xdata i = msdrvDPRxAppNullData[0];
}
#endif

