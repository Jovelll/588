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
//      Initial release:    06 May, 2008
//*******************************************************************************

#define _DPRXAPP_C_
#include "board.h"
#if ENABLE_DP_INPUT
#if ENABLE_FREESYNC
#include "ComboApp.h"
#endif
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
#include <msScaler.h>
#include "DDC2Bi.h"
#include "MsACE.h"
#include "NVRam.h"
#include "Mcu.h"
#include "common.h"
////////////////////////////////////
//#include "drvMcu.h"
///////////////////////////////////
#include "msflash.h"
#include "GPIO_Def.h"
#include "Adjust.h"
#if MS_DAC
#include "drvDAC.h"
#endif
//-------------------------------------------------------------------------------------------------
//  Local Structures & define
//-------------------------------------------------------------------------------------------------
 WORD _DPDATATYPE_ Aux=0;
enum
{

    eSTATE_STARTUP,  //0
    eSTATE_CKECKLOCK, // 1
    eSTATE_MEASURE, // 2
    eSTATE_NORMAL, // 3
    eSTATE_POWERSAVE, // 4
    eSTATE_DPIDLE  // 5
};
DP_INFO _DPDATATYPE_ gDPInfo;

#define  DP_DEBUG  0

#if (DP_DEBUG&&ENABLE_DEBUG)
#define DP_printData(str, value)   printData(str, value)
#define DP_printMsg(str)               printMsg(str)
#else
#define DP_printData(str, value)
#define DP_printMsg(str)
#endif

#if DP_SOFTEDID
BYTE EDIDADDR, EDIDOffSetAddr, Length = 0;
#endif

#if DP_AUTO_EQ
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


DWORD _DPDATATYPE_ DPTP1Cnt = 0;
DWORD _DPDATATYPE_ DPTP2Cnt = 0;
WORD _DPDATATYPE_ u16CDR_HPDCnt = 0;
WORD _DPDATATYPE_ u16Skew_HPDCnt = 0; // Add by CCJ
bit bCDR_HPDCntFlag = 0;
bit bSkew_HPDCntFlag =0;
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
BYTE _DPDATATYPE_ FastRate = 0;
BYTE _DPDATATYPE_ FastLane = 0;
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
DWORD _DPDATATYPE_ DPFREQ = 0;
DWORD _DPDATATYPE_ DPFBDIV = 0;

DWORD _DPDATATYPE_ DPAUM_Temp = 0;
DWORD _DPDATATYPE_ DPAUN_Temp= 0;
BYTE   _DPDATATYPE_ DPAudio_Cnt=0;
#endif

#if DP_VPLLBIGChange
BYTE _DPDATATYPE_ g_bDPVPLLMNBIGCHANGE = 0;
#endif


#if DPENABLEMCCS
bit g_bSoftMCCS = 0;
BYTE _DPDATATYPE_ MCCSRxCount = 0;
BYTE _DPDATATYPE_ DPMCCSOffSetAddr = 0;
BYTE _DPDATATYPE_ DPMCCSRXCNT = 0;
BYTE _DPDATATYPE_ DPMCCSRXBUF[16];
BYTE _DPDATATYPE_ DPMCCSTXBUF[16] =
{0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x36, 0x74, 0x22, 0x10, 0x86, 0x69, 0x32, 0x01};
#endif

#if DP_AudioMN_FineTune
BYTE _DPDATATYPE_ g_bDPAUDIO_MN_FineTune = 0;
#endif
#define ENABLE_DPAUDIO      0
#define DELAY_COUNT     80
///#define HFreq_Torlance      5
///#define VFreq_Torlance      5
#define DPDPCDPOWERSAVE_COUNTER 8
#define NoVscPktBound       5 //unit:pkt
//-------------------------------------------------------------------------------------------------
//  External Variables
//-------------------------------------------------------------------------------------------------
//extern InputModeType code StandardMode;
//extern ModeSettingType xdata DPModeSetting;

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static BYTE _DPDATATYPE_ m_ucState = eSTATE_STARTUP;

#if DP_SOFTEDID
BYTE code EDID[128] =
{
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x36, 0x74, 0x22, 0x10, 0x86, 0x69, 0x32, 0x01,
    0x2B, 0x12, 0x01, 0x04, 0x80, 0x22, 0x1B, 0x78, 0xE2, 0x8F, 0x95, 0xAD, 0x4F, 0x32, 0xB2, 0x25,
    0x0F, 0x50, 0x54, 0xA5, 0x4B, 0x00, 0x81, 0x80, 0xA9, 0x40, 0xD1, 0x00, 0xD1, 0x40, 0x71, 0x4F,
    0x81, 0x00, 0x01, 0x01, 0x01, 0x01, 0x30, 0x2A, 0x00, 0x98, 0x51, 0x00, 0x2A, 0x40, 0x30, 0x70,
    0x13, 0x00, 0x54, 0x0E, 0x11, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x4D, 0x53, 0x54,
    0x44, 0x50, 0x31, 0x39, 0x57, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x4D,
    0x53, 0x54, 0x41, 0x52, 0x20, 0x44, 0x45, 0x4D, 0x4F, 0x0A, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFD,
    0x00, 0x31, 0x56, 0x1D, 0x71, 0x1C, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x05
};
#else
#if DP_USE_INTERNAL_EDID
BYTE code  EDID[256] =
{
	#if ENABLE_FREESYNC
	#if 1
	0x00  ,0xFF  ,0xFF  ,0xFF  ,0xFF  ,0xFF  ,0xFF  ,0x00  ,0x05  ,0xA4  ,0x30  ,0x00  ,0x02  ,0x00  ,0x00  ,0x00,
	0x01  ,0x18  ,0x01  ,0x04  ,0x80  ,0x73  ,0x41  ,0x78  ,0x0B  ,0xCF  ,0x74  ,0xA7  ,0x55  ,0x46  ,0x98  ,0x24,
	0x10  ,0x49  ,0x4B  ,0x21  ,0x08  ,0x00  ,0x81  ,0x80  ,0x95  ,0x00  ,0x90  ,0x40  ,0xA9  ,0xC0  ,0xA9  ,0x40,
	0xB3  ,0x00  ,0x61  ,0x40  ,0x71  ,0x40  ,0x37  ,0x45  ,0x80  ,0x4A  ,0x71  ,0x38  ,0x2D  ,0x40  ,0x30  ,0x20,
	0x35  ,0x00  ,0x35  ,0xAD  ,0x10  ,0x00  ,0x00  ,0x1E  ,0x00  ,0x00  ,0x00  ,0x10  ,0x00  ,0x00  ,0x00  ,0x00,
	0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0xFC  ,0x00  ,0x46,
	0x52  ,0x45  ,0x45  ,0x53  ,0x59  ,0x4E  ,0x43  ,0x37  ,0x30  ,0x2D  ,0x34  ,0x30  ,0x00  ,0x00  ,0x00  ,0xFD,
	0x00  ,0x28  ,0x46  ,0x6B  ,0x6B  ,0x1E  ,0x01  ,0x0A  ,0x20  ,0x20  ,0x20  ,0x20  ,0x20  ,0x20  ,0x01  ,0x0C,
	0x02  ,0x03  ,0x31  ,0xF3  ,0x4D  ,0x01  ,0x03  ,0x84  ,0x05  ,0x07  ,0x10  ,0x12  ,0x93  ,0x14  ,0x16  ,0x1F,
	0x20  ,0x22  ,0x29  ,0x09  ,0x07  ,0x07  ,0x01  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x83  ,0x01  ,0x00  ,0x00,
	0x67  ,0x03  ,0x0C  ,0x00  ,0x10  ,0x00  ,0x00  ,0x44  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00,
	0x00  ,0x8C  ,0x0A  ,0xD0  ,0x8A  ,0x20  ,0xE0  ,0x2D  ,0x10  ,0x10  ,0x3E  ,0x96  ,0x00  ,0xC4  ,0x8E  ,0x21,
	0x00  ,0x00  ,0x18  ,0x02  ,0x3A  ,0x80  ,0x18  ,0x71  ,0x38  ,0x2D  ,0x40  ,0x58  ,0x2C  ,0x45  ,0x00  ,0xC4,
	0x8E  ,0x21  ,0x00  ,0x00  ,0x1E  ,0x01  ,0x1D  ,0x00  ,0xBC  ,0x52  ,0xD0  ,0x1E  ,0x20  ,0xB8  ,0x28  ,0x55,
	0x40  ,0xC4  ,0x8E  ,0x21  ,0x00  ,0x00  ,0x1E  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00,
	0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x07,

	#else
	0x00	,0xFF	,0xFF	,0xFF	,0xFF	,0xFF	,0xFF	,0x00	,0x4C	,0x2D	,0xCC	,0x0B	,0x00	,0x00	,0x00	,0x00,
	0x0D	,0x18	,0x01	,0x04	,0x80	,0x47	,0x28	,0x78	,0x3B	,0xE6	,0x91	,0xA7	,0x55	,0x4E	,0xA0	,0x25,
	0x0C	,0x50	,0x54	,0xBF	,0xEF	,0x80	,0x71	,0x4F	,0x81	,0x00	,0x81	,0xC0	,0x81	,0x80	,0xA9	,0xC0,
	0xB3	,0x00	,0x95	,0x00	,0x01	,0x01	,0x83	,0x48	,0x80	,0x18	,0x71	,0x38	,0x2D	,0x40	,0x58	,0x2C,
	0x45	,0x00	,0xFD	,0x1E	,0x11	,0x00	,0x00	,0x1E	,0xF2	,0x12	,0x80	,0x18	,0x71	,0x38	,0x2D	,0x40,
	0x58	,0x2C	,0x45	,0x00	,0xFD	,0x1E	,0x11	,0x00	,0x00	,0x1E	,0x00	,0x00	,0x00	,0xFD	,0x00	,0x31,
	0x4B	,0x70	,0x70	,0x12	,0x01	,0x0A	,0x20	,0x20	,0x20	,0x20	,0x20	,0x20	,0x00	,0x00	,0x00	,0xFC,
	0x00	,0x55	,0x32	,0x38	,0x45	,0x35	,0x39	,0x30	,0x0A	,0x20	,0x20	,0x20	,0x20	,0x20	,0x01	,0x80,
	0x02	,0x03	,0x0E	,0xF2	,0x41	,0x10	,0x23	,0x09	,0x07	,0x07	,0x83	,0x01	,0x00	,0x00	,0x56	,0x5E,
	0x00	,0xA0	,0xA0	,0xA0	,0x29	,0x50	,0x30	,0x20	,0x35	,0x00	,0xC4	,0x8E	,0x21	,0x00	,0x00	,0x1A,
	0x02	,0x3A	,0x80	,0x18	,0x71	,0x38	,0x2D	,0x40	,0x58	,0x2C	,0x45	,0x00	,0xC4	,0x8E	,0x21	,0x00,
	0x00	,0x1E	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
	0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
	0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
	0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00,
	0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	,0x89,
	#endif
	#elif 0
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x38, 0xA3, 0x0E, 0x68, 0x00, 0x00, 0x00, 0x00,
    0x24, 0x14, 0x01, 0x04, 0xA5, 0x26, 0x1E, 0x78, 0xE2, 0xB7, 0xF5, 0xA0, 0x58, 0x56, 0x9F, 0x28,
    0x0B, 0x50, 0x54, 0xBF, 0xEF, 0x80, 0x71, 0x4F, 0x81, 0x40, 0x81, 0x80, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x30, 0x2A, 0x00, 0x98, 0x51, 0x00, 0x2A, 0x40, 0x30, 0x70,
    0x13, 0x00, 0x78, 0x2D, 0x11, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFD, 0x00, 0x32, 0x4C, 0x1F,
    0x52, 0x0E, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x45,
    0x41, 0x31, 0x39, 0x32, 0x4D, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x38, 0x31, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x54, 0x42, 0x0A, 0x20, 0x20, 0x01, 0x74,
    0x02, 0x03, 0x11, 0xC1, 0x44, 0x84, 0x13, 0x03, 0x12, 0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00,
    0x00, 0x01, 0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20, 0x6E, 0x28, 0x55, 0x00, 0x78, 0x2D, 0x11,
    0x00, 0x00, 0x1E, 0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E, 0x96, 0x00, 0x78,
    0x2D, 0x11, 0x00, 0x00, 0x18, 0x8C, 0x0A, 0xD0, 0x90, 0x20, 0x40, 0x31, 0x20, 0x0C, 0x40, 0x55,
    0x00, 0x78, 0x2D, 0x11, 0x00, 0x00, 0x18, 0x01, 0x1D, 0x00, 0xBC, 0x52, 0xD0, 0x1E, 0x20, 0xB8,
    0x28, 0x55, 0x40, 0x78, 0x2D, 0x11, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55
	#elif 0// For Nvidia EDID 1080P120hx
    0x00,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0x00,   0x36,   0x74,   0x00,   0x01,   0x00,   0x00,   0x00,   0x00,
    0x16,   0x13,   0x01,   0x04,   0x80,   0x33,   0x1C,   0x78,   0xEE,   0xEF,   0x91,   0xA3,   0x54,   0x4C,   0x9B,   0x26,
    0x0F,   0x50,   0x54,   0x00,   0x00,   0x00,   0x01,   0x01,   0x01,   0x01,   0x01,   0x01,   0x01,   0x00,   0x01,   0x01,
    0x01,   0x01,   0x01,   0x01,   0x01,   0x01,   0x86,   0x6F,   0x80,   0xA0,   0x70,   0x38,   0x40,   0x40,   0x30,   0x20,
    0x35,   0x00,   0xFE,   0x1F,   0x11,   0x00,   0x00,   0x1E,   0x00,   0x00,   0x00,   0xFF,   0x00,   0x30,   0x30,   0x30,
    0x30,   0x30,   0x31,   0x0A,   0x20,   0x20,   0x20,   0x20,   0x20,   0x20,   0x00,   0x00,   0x00,   0xFC,   0x00,   0x4D,
    0x53,   0x74,   0x61,   0x72,   0x20,   0x46,   0x48,   0x44,   0x0A,   0x20,   0x20,   0x20,   0x00,   0x00,   0x00,   0xFD,
    0x00,   0x3B,   0x79,   0x3E,   0x8E,   0x1E,   0x00,   0x0A,   0x20,   0x20,   0x20,   0x20,   0x20,   0x20,   0x00,   0x16,
	#elif 0
    0x00,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0x00,   0x36,   0x74,   0x00,   0x01,   0x00,   0x00,   0x00,   0x00,
    0x16,   0x13,   0x01,   0x04,   0x80,   0x33,   0x1C,   0x78,   0xEE,   0xEF,   0x91,   0xA3,   0x54,   0x4C,   0x9B,   0x26,
    0x0F,   0x50,   0x54,   0x00,   0x00,   0x00,   0x01,   0x01,   0x01,   0x01,   0x01,   0x01,   0x01,   0x00,   0x01,   0x01,
    0x01,   0x01,   0x01,   0x01,   0x01,   0x01,   0x86,   0x6F,   0x80,   0xA0,   0x70,   0x38,   0x40,   0x40,   0x30,   0x20,
    0x35,   0x00,   0xFE,   0x1F,   0x11,   0x00,   0x00,   0x3E,   0x00,   0x00,   0x00,   0xFF,   0x00,   0x30,   0x30,   0x30,
    0x30,   0x30,   0x31,   0x0A,   0x20,   0x20,   0x20,   0x20,   0x20,   0x20,   0x00,   0x00,   0x00,   0xFC,   0x00,   0x4D,
    0x53,   0x74,   0x61,   0x72,   0x20,   0x46,   0x48,   0x44,   0x0A,   0x20,   0x20,   0x20,   0x00,   0x00,   0x00,   0xFD,
    0x00,   0x3B,   0x79,   0x3E,   0x8E,   0x1E,   0x00,   0x0A,   0x20,   0x20,   0x20,   0x20,   0x20,   0x20,   0x00,   0xF6,
	#elif 0 //(2560 120Hz) (4 lane)
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x10, 0xAC, 0x57, 0xA0, 0x53, 0x31, 0x31, 0x31,
    0x30, 0x13, 0x01, 0x04, 0xB5, 0x3C, 0x22, 0x78, 0x3A, 0x8E, 0x05, 0xAD, 0x4F, 0x33, 0xB0, 0x26,
    0x0D, 0x50, 0x54, 0xA5, 0x4B, 0x00, 0x81, 0x00, 0xB3, 0x00, 0xD1, 0x00, 0x71, 0x4F, 0xA9, 0x40,
    0x81, 0x80, 0x01, 0x01, 0x01, 0x01, 0xAC, 0xBC, 0x00, 0xA0, 0xA0, 0xA0, 0x29, 0x50, 0x30, 0x20,
    0x35, 0x00, 0x55, 0x50, 0x21, 0x00, 0x00, 0x1A, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x48, 0x48, 0x31,
    0x31, 0x37, 0x38, 0x32, 0x39, 0x31, 0x31, 0x31, 0x53, 0x0A, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x44,
    0x45, 0x4C, 0x4C, 0x20, 0x55, 0x32, 0x37, 0x31, 0x31, 0x0A, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFD,
    0x00, 0x31, 0x79, 0x1D, 0x71, 0x0B, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0xE1,
    0x02, 0x03, 0x1D, 0xF1, 0x50, 0x90, 0x05, 0x04, 0x03, 0x02, 0x07, 0x16, 0x01, 0x06, 0x11, 0x12,
    0x15, 0x13, 0x14, 0x1F, 0x20, 0x23, 0x0D, 0x7F, 0x07, 0x83, 0x0F, 0x00, 0x00, 0x02, 0x3A, 0x80,
    0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C, 0x25, 0x00, 0x55, 0x50, 0x21, 0x00, 0x00, 0x1E, 0x01,
    0x1D, 0x80, 0x18, 0x71, 0x1C, 0x16, 0x20, 0x58, 0x2C, 0x25, 0x00, 0x55, 0x50, 0x21, 0x00, 0x00,
    0x9E, 0x01, 0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20, 0x6E, 0x28, 0x55, 0x00, 0x55, 0x50, 0x21,
    0x00, 0x00, 0x1E, 0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E, 0x96, 0x00, 0x55,
    0x50, 0x21, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4B
	#elif 0 //=>2560X1600@ 60
    0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x04,0x43,0x14,0x80,0x89,0xDA,0x07,0x00,0x0A,0x12,0x01,0x04,0x80,0x41,0x29,0x78,0xE2,0x8F,0x95,0xAD,0x4F,0x32,0xB2,0x25,
    0x0F,0x50,0x54,0xA5,0x4B,0x00,0x81,0x80,0xA9,0x40,0xD1,0x00,0xD1,0x40,0x71,0x4F,0x81,0x00,0x01,0x01,0x01,0x01,0xB0,0x68,0x00,0xA0,0xA0,0x40,0x2E,0x60,0x30,0x20,
    0x36,0x00,0x81,0x90,0x21,0x00,0x00,0x3E,0x00,0x00,0x00,0xFF,0x00,0x44,0x50,0x31,0x39,0x30,0x35,0x32,0x38,0x38,0x37,0x45,0x54,0x0A,0x00,0x00,0x00,0xFC,0x00,0x41,
    0x42,0x43,0x20,0x33,0x30,0x32,0x30,0x30,0x38,0x57,0x53,0x0A,0x00,0x00,0x00,0xFD,0x00,0x31,0x56,0x1D,0x71,0x1C,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x01,0xC9,
    0x70,0x11,0x06,0x03,0x00,0x10,0x00,0x03,0x02,0x00,0x01,0xD0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x90
    #elif 0//DELL_1680X1050 // 20111007.  Gary.Wang
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x10,0xAC,0x4D,0x40,0x00,0x00,0x00,0x00,
	0x07,0x12,0x01,0x04,0xA5,0x2F,0x1E,0x78,0x3E,0xEE,0x95,0xA3,0x54,0x4C,0x99,0x26,
	0x0F,0x50,0x54,0xA5,0x4B,0x00,0x71,0x4F,0x81,0x80,0xB3,0x00,0x01,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x7C,0x2E,0x90,0xA0,0x60,0x1A,0x1E,0x40,0x30,0x20,
	0x36,0x00,0xDA,0x28,0x11,0x00,0x00,0x1C,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0x00,0x44,
	0x45,0x4C,0x4C,0x20,0x50,0x32,0x32,0x31,0x30,0x0A,0x20,0x20,0x00,0x00,0x00,0xFD,
	0x00,0x38,0x4B,0x1E,0x53,0x0F,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x85,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
   #elif 0  //1080P
    0x00,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0x00,   0x4C,   0x2D,   0x9F,   0x07,   0x00,   0x00,   0x00,   0x00,
    0x03,   0x15,   0x01,   0x04,   0xA5,   0x3C,   0x22,   0x78,   0x22,   0x60,   0x41,   0xA6,   0x56,   0x4A,   0x9C,   0x25,
    0x12,   0x50,   0x54,   0x00,   0x00,   0x00,   0x01,   0x01,   0x01,   0x01,   0x01,   0x01,   0x01,   0x01,   0x01,   0x01,
    0x01,   0x01,   0x01,   0x01,   0x01,   0x01,   0x86,   0x6F,   0x80,   0xA0,   0x70,   0x38,   0x40,   0x40,   0x30,   0x20,
    0x35,   0x00,   0x56,   0x50,   0x21,   0x00,   0x00,   0x3E,   0xFE,   0x5B,   0x80,   0xA0,   0x70,   0x38,   0x35,   0x40,
    0x30,   0x20,   0x35,   0x00,   0x56,   0x50,   0x21,   0x00,   0x00,   0x5A,   0x00,   0x00,   0x00,   0xFD,   0x00,   0x18,
    0x78,   0x1A,   0x8C,   0x1E,   0x00,   0x0A,   0x20,   0x20,   0x20,   0x20,   0x20,   0x20,   0x00,   0x00,   0x00,   0xFC,
    0x00,   0x53,   0x32,   0x37,   0x41,   0x39,   0x35,   0x30,   0x44,   0x0A,   0x20,   0x20,   0x20,   0x20,   0x01,   0x3B,
    0x02,   0x03,   0x08,   0xC0,   0x23,   0x09,   0x07,   0x03,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
    0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
    0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
    0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
    0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
    0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
    0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
    0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0xFD
    #elif 1 //TPV VS230PGLW // 20111007.  Gary.Wang
	#warning "--------------------VS230PGLW internal EDID enable----------------------------"
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x04,0x43,0x14,0x80,0x89,0xDA,0x07,0x00,0x0A,0x12,0x01,0x04,0x80,0x41,0x29,0x78,0xE2,0x8F,0x95,0xAD,0x4F,0x32,0xB2,0x25,
	0x0F,0x50,0x54,0xA5,0x4B,0x00,0x81,0x80,0xA9,0x40,0xD1,0x00,0xD1,0x40,0x71,0x4F,0x81,0x00,0x01,0x01,0x01,0x01,0xB0,0x68,0x00,0xA0,0xA0,0x40,0x2E,0x60,0x30,0x20,
	0x36,0x00,0x81,0x90,0x21,0x00,0x00,0x3E,0x00,0x00,0x00,0xFF,0x00,0x44,0x50,0x31,0x39,0x30,0x35,0x32,0x38,0x38,0x37,0x45,0x54,0x0A,0x00,0x00,0x00,0xFC,0x00,0x41,
	0x42,0x43,0x20,0x33,0x30,0x32,0x30,0x30,0x38,0x57,0x53,0x0A,0x00,0x00,0x00,0xFD,0x00,0x31,0x56,0x1D,0x71,0x1C,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x01,0xC9,
	0x70,0x11,0x06,0x03,0x00,0x10,0x00,0x03,0x02,0x00,0x01,0xD0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x90

#endif

};
#endif
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



void DPRxInit( void )
{
    DPRxHPDDisable();
    msWriteByte( REG_0226, msReadByte( REG_0226 )&( ~_BIT0 ) ); // 120201
    msDPInitState();

    msWriteByte( REG_1FE0, 0xFF );  // mask INT
    msWriteByte( REG_1FE1, 0xFF );  // mask INT
    msWriteByte( REG_1FE2, 0xFF );  // mask INT
    msWriteByte( REG_1FE3, 0xFF );  // mask INT
    msWriteByte( REG_1FE8, 0xFF );  // clear INT
    msWriteByte( REG_1FE9, 0xFF );  // clear INT
    msWriteByte( REG_1FEA, 0xFF );  // clear INT
    msWriteByte( REG_1FEB, 0xFF );  // clear INT
    // ##############################################
    // ##                                        BANK (0X21)                                    ##
    // ##############################################
#if SupportDPAudio
    msWriteByte( REG_21A8, 0x0C );  //[2]: ignore Audio ID [3] Enable RS code sync reset
    msWriteByte( REG_21EC, 0xE0 );  //[5]:Enable Audio time Stamp [6]Audio Stream [7] Audio Protect
    #if DP_AudioMuteEvent

    // 120418 coding, addition for mute and fading event
    msWriteByte( REG_05CE, 0x40 ); // [6:5]: Automatic fading mode
    msWriteByte( REG_05D2, 0xC0); // [7:6]: fading speed select 8X
    msWriteByte( REG_05D0, 0xFF ); // Zero crossing threshold = 0xFF
    msWriteByte( REG_05D1, 0xFF ); // Zero crossing threshold = 0xFF

    msWriteByte(REG_21F0, 0x7F);                     //[0]~[7] Enable Audio Mute Event.
    msWriteByte(REG_21F1, 0x7F);                     //[0]~[7] Enable Audio Fading Event.
    #endif
#endif
    msWriteByteMask( REG_21ED, _BIT1 | _BIT3, _BIT1 | _BIT3 );   //Enable VSC Packet  // receive Packet Update

    // ##############################################
    // ##                                        BANK (0X20)                                    ##
    // ##############################################

#if DP54G
    msWriteByte( REG_2002, 0xC4 );
#else
    msWriteByte( REG_2002, 0x84 );
#endif

    msWriteByte( REG_2003, 0x04 );               // I2C speed

    // ##############################################
    // ##                                        BANK 1F(0X36)                                ##
    // ##############################################
    msWriteByte( REG_3680, 0x21 );                       // Htotal Tolerance
    msWriteByte( REG_36A2, _BIT7 | _BIT6 | _BIT1 | _BIT0 );         // Aux Clock Src,// Set AUX Debounce clk
    msWriteByte( REG_366E, 0xFF );                           // New 8/10b Fifo
    msWriteByte( REG_36C8, 0x0F );                              // select new 10b/8b decoder
    msWriteByte( REG_36A0, 0x40 );                           // Enable AUX Debounce
    msWriteByte( REG_360D, 0 );                          // set V paulse  small
    msWriteByte( REG_3618, 0x1F );
    msWriteByte( REG_3619, 0x3F );
    msWriteByte( REG_3607, 0xE5 );                   //DP Interlace mode
    msWriteByte( REG_367F, 0x01 );                   //Reset Word Boundary
    msWriteByteMask( REG_3641, BIT3, BIT3 );	      // 20151029  HV Regen reset.
#if DP_RESETHTT_PATCH
    msWriteByteMask( REG_3681, BIT3 | BIT2 |BIT1, BIT3 | BIT2 |BIT1);  // DP HTT detect select by BE
    msWriteByteMask( REG_3641, BIT3, BIT3 );
    //msWriteByte( REG_3683,0x03);                      //Learning threshold
#endif
    // ##############################################
    // ##                                        BANK 18(0X37)                                ##
    // ##############################################
#if DP_AUTO_EQ
    msWriteByte( REG_3700, 0x90 );
    msWriteByte( REG_3702, 0x03 );        //EQ 1
    msWriteByte( REG_3704, 0x06 );        //EQ 2
    msWriteByte( REG_3706, 0x08 );        //EQ 3
    msWriteByte( REG_370C, 0x1F );
    msWriteByte( REG_370D, 0x10 );
#endif

    msWriteByteMask( REG_3765, 0, _BIT0 );                        // check
    msWriteByteMask( REG_3701, _BIT4, _BIT4 );           // U02 ECO item  high pixel rate

#if 0//DP_PLL_ATOP
    msWriteByteMask( REG_3779, BIT7, BIT7 );             //ATOP detect logic
    msWriteByteMask( REG_3566, 0, 0xF0 );
    msWriteByteMask( REG_356A, 0, BIT4 );
#else
    msWriteByteMask( REG_3779, 0, BIT7 );            //ATOP detect logic
    msWriteByteMask( REG_3566, 0xF0, 0xF0 );
    msWriteByteMask( REG_356A, BIT4, BIT4 );
#endif

    msWriteByte( REG_37E8, 0 );                          //CH0 AFE power down settings
    msWriteByte( REG_37E9, 0 );                          //CH1 AFE power down settings
    msWriteByte( REG_37EA, 0 );                          //CH2 AFE power down settings
    msWriteByte( REG_37EB, 0 );                          //CH3 AFE power down settings

    // ##############################################
    // ##                                        BANK 36(0X1F)                                ##
    // ##############################################

#if DP_SOFTEDID
    msWriteByte( REG_1FF0, 0x02 ); // EDID Replay by MCU , TX reply according to Source request
#else
    msWriteByte( REG_1FF0, 0x00 ); // AUX TIME OUT REPLY ACK FOR CTS ISSUE
#endif

#if Displayport12
    msWriteByte( REG_1F00, 0x12 );           // DPCD 1.2 Version
    msWriteByte( REG_1F02, 0x84 );           // DP Enhanced Frame capability & Max Lane Count
#else
    msWriteByte( REG_1F00, 0x11 );           // DPCD 1.1 Version
    msWriteByte( REG_1F02, 0x84 );           // DP Enhanced Frame capability & Max Lane Count
#endif

#if DP54G
      msWriteByte( REG_1F01, 0x14 );           // DP main link rate 5.4G
#else
    msWriteByte( REG_1F01, 0x0A );           // DP main link rate
#endif

#if ENABLE_FREESYNC
	if(UserprefFreeSyncMode)
	msWriteByte( REG_1F04, 0x01|BIT3);            // DP Number of Receiver Ports
	else
#endif
    msWriteByte( REG_1F04, 0x01 );            // DP Number of Receiver Ports
    msWriteByte( REG_1F1C, 0x80 );           // DP Linkstatus andadjust requestupdated
    msWriteByte( REG_1F1E, 0x00 );            // LANE0,1 Pre-emphasis & VoltageSwing
    msWriteByte( REG_1F1F, 0x00 );            // LANE2,3 Pre-emphasis & VoltageSwing
    msWriteByte( REG_1FD4, 0x01 );           // RS_SWAP & DP receiever enable

    msWriteByte( REG_1FDC, 0x80 );           // USE Lanes count mask lock status
    msWriteByte( REG_1FF1, 0xD0 );            // AUX length compare mask


#if DP_VPLLBIGChange
    msWriteByte( REG_1FD2, 0xFF );           // Video PLL freeze disable    Audio Enable Frezzen
    msWriteByte( REG_1FD3, 0x72 );           // [3]:=> 1:Dither output 8 bits , 0: 10 bits
#else
    msWriteByte( REG_1FD2, 0x33 );           // Video PLL freeze disable    Audio Enable Frezzen
    msWriteByte( REG_1FD3, 0x72 );           // [3]:=> 1:Dither output 8 bits , 0: 10 bits
#endif

    msWriteByte( REG_1FDD, 0x20 );           //  Bit5: DPCD 0204h Update bit enable

#if DPENABLEINTERLANESKEW
    //msWriteByte(REG_1FDD,0xA0);               //  Bit5: DPCD 0204h Update bit enable , Bit7: InterlaneSkew mask by CDR
    msWriteByte( REG_1FDD, 0x80 );      //  Add by CCJ
#endif

    msWriteByte( REG_1FD6, 0x10 );            // Set 0x10
    msWriteByte( REG_1FD8, 0x20 );            // FIFO Delay for Reduce Blanking issue
    msWriteByte( REG_1FB5, 0x08 );            // Auto Reply Defer when Auto EQ.

#if 0//DP_BUG_PATCH
    msWriteByteMask( REG_1FBD, BIT1, BIT1 );  // Read MSA by Counting BS
#endif

#if SupportDPAudio
    // audio setting
    msWriteByte( REG_1FAE, 0x10 );           //MAUD_CENTER[7:0]
    msWriteByte( REG_1FAF, 0x00 );           //MAUD_CENTER[15:8]
    msWriteByte( REG_1FB0, 0x00 );           //MAUD_CENTER[23:16]
    msWriteByte( REG_1FB2, 0x10 );           //MAUD_Range[7:0]
    msWriteByte( REG_1FB3, 0x00 );           //MAUD_Range[15:8]
    msWriteByte( REG_1FB4, 0x00 );           //MAUD_Range[23:16]
    msWriteByte( REG_1FB6, 0x00 );           //NAUD_CENTER[7:0]
    msWriteByte( REG_1FB7, 0x10 );           //NAUD_CENTER[15:8]
    msWriteByte( REG_1FB8, 0x00 );           //NAUD_CENTER[23:16]
    msWriteByte( REG_1FB9, 0x10 );           //NAUD_Range[7:0]
    msWriteByte( REG_1FBA, 0x00 );           //NAUD_Range[15:8]
    msWriteByte( REG_1FBB, 0x00 );           //NAUD_Range[23:16]

#endif

#if 1
    //video setting
    msWriteByte( REG_1FA2, 0x20 );           //MVID_Range[7:0]
    msWriteByte( REG_1FA3, 0x00 );           //MVID_Range[15:8]
    msWriteByte( REG_1FA4, 0x00 );           //MVID_Range[23:16]
#endif

#if 1
    msWriteByte( REG_1FD1, 0x30 );           // Bit5: Video M/N Bit4: Audio M/N
    msWriteByte( REG_1FC1, msReadByte( REG_1FC1 ) | _BIT0 );
    msWriteByte( REG_1FC2, 0x98 );           // BS-> RE_GEN HPWS delay for Songlu U02
    msWriteByte( REG_1FC6, 0x04 );
    msWriteByte( REG_1FCF, 0x07 );
    msWriteByte( REG_1F99, msReadByte( REG_1F99 ) | _BIT7 ); //vblank config enable
    msWriteByte( REG_1F97, 0x03 );           //  Glitch filter use old mode  (Warning: HLBR ISSUE)  No gltch
    msWriteByte( REG_1F96, 0xFF );           //  Glitch filter use old mode  (Warning: HLBR ISSUE)  No gltch
#endif

#if NEWCRCMODE
    msWriteByte( REG_1FCC, 0x61 );          //(bit3=0 use VB_ID rising as frame flag (end of active))
#else
    msWriteByte( REG_1FCC, 0x29 );
#endif

    // ##############################################
    // ##                                        BANK 37(0x35)                                 ##
    // ##############################################

    msWriteByte( REG_35E0, 0xCE );           // clk enable  Bit0=0 EQ issue

#if DP_AUTO_EQ
    msWriteByte( REG_35E1, 0x7F );           // Bit 4: Phase DAC clk enable // Bit 5: For Squelch clock
#else
    msWriteByte( REG_35E1, 0x6F );           // Bit 5: For Squelch clock
#endif

    msWriteByte( REG_356C, 0x00 );           // DP PLL POWER DOWN
    msWriteByte( REG_356E, 0x01 );           // Training PFD Divide 4
#if 1//DPPHYUPDATE
    msWriteByte( REG_356D, 0x00 );           // DP_SEL_R50_VCM 3.3V & close powerdown
    msWriteByte( REG_356F, 0xF4 );
    msWriteByte( REG_3574, 0x30 );           // EQ common mode, TX_OFF
    msWriteByte( REG_3576, 0x38 );           // DP_AUX 6ma 10ns 800mv
    msWriteByte( REG_3580, 0x00 );           // DP PHY Resistor power down.  Enable Rterm
    msWriteByte( REG_3581, 0x36 );           // DP PHY Resistor Control Overwrite enable. DP PHY Resistor Control. Rterm 50ohm
    msWriteByte( REG_358C, 0x10 );           // DP EQ threshold set 0 for EQ lose lock issue : DP EQ threshold set 10h
#endif

#if 0//DPAUXCLK60M
    msWriteByte( REG_358A, 0x04 );           // DP main link clock lock threshold when 162MHz.
    msWriteByte( REG_358B, 0x13 );           // DP main link clock lose threshold when 162MHz.
    msWriteByte( REG_3592, 0x40 );           // DP main link clock lock when 270MHz.
    msWriteByte( REG_3593, 0x01 );           // DP main link clock lock when 270MHz.
    msWriteByte( REG_3594, 0x03 );           // DP main link clock lock threshold when 270MHz.
    msWriteByte( REG_3595, 0x0F );           // DP main link clock lose threshold when 270MHz.
    msWriteByte( REG_3596, 0x80 );           // DP main link clock lock when 162MHz.
    msWriteByte( REG_3597, 0x01 );           // DP main link clock lock when 162MHz.
#else
#if 0
    msWriteByte( REG_358A, 0x0E );           // 12M DP main link clock lock threshold when 162MHz.
    msWriteByte( REG_358B, 0x48 );           // 12M DP main link clock lose threshold when 162MHz.
    msWriteByte( REG_3592, 0xA0 );           // 12M DP main link clock lock when 270MHz.  0x5A0
    msWriteByte( REG_3593, 0x05 );           // 12M DP main link clock lock when 270MHz.
    msWriteByte( REG_3594, 0x0C );           // 12M DP main link clock lock threshold when 270MHz.
    msWriteByte( REG_3595, 0x3C );           // 12M DP main link clock lose threshold when 270MHz.
    msWriteByte( REG_3596, 0xC0 );           // 12M DP main link clock lock when 162MHz.    0x6C0
    msWriteByte( REG_3597, 0x06 );           // 12M DP main link clock lock when 162MHz.
#else
    msWriteByte( REG_358A, 0x04 );           // 12M DP main link clock lock threshold when 162MHz.
    msWriteByte( REG_358B, 0x10 );           // 12M DP main link clock lose threshold when 162MHz.
    msWriteByte( REG_3592, 0x2D );           // 12M DP main link clock lock when 270MHz.  0x5A0
    msWriteByte( REG_3593, 0x01 );           // 12M DP main link clock lock when 270MHz.
    msWriteByte( REG_3594, 0x04 );           // 12M DP main link clock lock threshold when 270MHz.
    msWriteByte( REG_3595, 0x12 );           // 12M DP main link clock lose threshold when 270MHz.
    msWriteByte( REG_3596, 0x6A );           // 12M DP main link clock lock when 162MHz.    0x6C0
    msWriteByte( REG_3597, 0x01 );           // 12M DP main link clock lock when 162MHz.

#if DP_REQUEST_VSWIM
    msRegs[REG_3592] = 0x0;                    // 12M DP main link clock lock when 270MHz.
    msRegs[REG_3593] = 0x0;                    // 12M DP main link clock lock when 270MHz.
    msRegs[REG_3596] = 0x0;                    // 12M DP main link clock lock when 162MHz.
    msRegs[REG_3597] = 0x0;                    // 12M DP main link clock lock when 162MHz.
#endif
#endif

#endif

    msWriteByte( REG_3598, 0x10 );           // 12M DP AUX PHY UI counter threshold.
    msWriteByte( REG_3599, 0x0D );           // 12M DP AUX PHY over sample rate.
    msWriteByte( REG_359A, 0x08 );           // 12M DP AUX timeout threshold.
    msWriteByte( REG_359B, 0x07 );           // 12M DP AUX timeout threshold.  AUX CLK select 50Mhz
    // msWriteByteMask(REG_359B, BIT7, BIT7);     //DPCD New mode

    msWriteByte( REG_3568, 0x6 );           // DP EQ0 SET   3Ch
    msWriteByte( REG_3569, 0x6 );           // DP EQ1 SET   3Ch
    msWriteByte( REG_3587, 0x6 );           // DP EQ2 SET   3Ch
    msWriteByte( REG_3589, 0x6 );           // DP EQ3 SET   3Ch

    msWriteByte( REG_355E, 0x04 );            // BIT4 ignore Audio ID
    msWriteByte( REG_359E, 0x0C );            // BIT2 Align Word Boundary Overwrite Enable
    msWriteByte( REG_359C, 0x3f );           // DP main link lock Divider 2.7G.
    msWriteByte( REG_359D, 0x7f );           // DP main link lock Divider 1.62G
#if SupportDPAudio
    #if DP_AudioMuteEvent
       msWriteByte(REG_35FA,0xCF);         //  Bit1: Disable AUPLL Frezze interrupt , Bit6: CLK HDCP_EN , Bit7: SD clk
    #else
       msWriteByte(REG_35FA,0xCA);
    #endif
#else
    msWriteByte( REG_35FA, 0x4A );           //  Bit1: Disable AUPLL Frezze interrupt , Bit6: CLK HDCP_EN
#endif
    msWriteByte( REG_35D5, 0x1C );           // AUX SYNC de-glitch 4 sync's
    msWriteByte( REG_35D1, 0x00 );           // VPLL_PWDWN
    msWriteByte( REG_35B3, 0x00 );           // APLL disable PWD
    msWriteByte( REG_35B8, 0x22 );           // APLL 512*Fs at 1.62Ghz 162Mhz
    msWriteByte( REG_35C6, 0x09 );           // BW more speed up
    // 120201
    //msWriteByte( REG_35D5, 0x1F );           // [1:0] option enable Training P1 P2 IRQ To BankB 0xE8[0:1]
    msWriteByte( REG_35D5, 0x0F );           // [1:0] option enable Training P1 P2 IRQ To BankB 0xE8[0:1]

    msWriteByte( REG_356E, 0x00 );           //  set default
    msWriteByte( REG_356F, 0x00 );           //  set default
    msWriteByteMask( REG_3575, BIT7,BIT7 );     // AUX won¡¦t be powerdown

    DPRxEDIDLoad();
    DPRxHDCPLoadKey();

    DPRxAuxInit();

    DPRxOutputEnable( TRUE );
    m_ucState = eSTATE_STARTUP;
    gDPInfo.ucDPColorFormat=InputColor_RGB;
    DPRxIRQEnable( TRUE );
    DP_EnableSquelch( TRUE );
#if DP_FastTraining
    DPRxFastTrainingInitial();
#endif

    DPRxHPDEnable();
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
    Bool bCID;

    BYTE ucRID, ucDbyte;

    msWriteByteMask(REG_21ED, 0xF0, 0xF0);                                   // =>  Issue 3, U03.37

    bCID = (msRead2Byte(REG_1EF2) == 0x384D ? TRUE : FALSE);

    msWrite2Byte(REG_1EF0, 0xC83E);

    ucRID = msReadByte(REG_1EF4);

    if(bCID && (ucRID == 0x02))         //111107 Rick modified DP init setting - A047
    {

        ucDbyte = msReadByte(REG_1EFA);

        if(ucDbyte & _BIT0) // CDR with SQ                                              //  =>  Issue 2, U03.24
        {

            msWriteByteMask(REG_35F7, _BIT5, _BIT5);

        }
        if(ucDbyte & _BIT1) // K28.5+-                                                   //  =>  Issue 2, U03.30
        {

            msWriteByteMask(REG_377B, _BIT7, _BIT7);

        }
    }
    msWrite2Byte(REG_1EF0, 0x0000);
    #if DP54G
    msWriteByteMask(REG_35F7, _BIT4, _BIT4);
    #endif
       msWriteByteMask(REG_35F7, _BIT3, _BIT3);                 // Simply Fast
    msWriteByteMask(REG_35F7, _BIT2, _BIT2);                    // Power Down PDF LA
       msWriteByteMask(REG_35F7, _BIT1, _BIT1);                 // Fast Trianing Turn off
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
void DPRxHDCPLoadKey( void )
{
    WORD xdata  i;
    BYTE xdata ucValue;
    msWriteByte( REG_358E, 0x00 ); // HDCP_SRAM_INIT_ADDR[7:0]
    //msWriteByte(REG_358F,0x00);// HDCP_SRAM_INIT_ADDR[9:8]
    msWriteByteMask( REG_358F, 0, 0x3 ); // HDCP_SRAM_INIT_ADDR[9:8]
#if DP_USE_INTERNAL_HDCP
    for( i = 0; i < 5; i++ )
        msWriteByte( REG_3588, DPCPBKSV[i] );

    for( i = 0; i < 284; i++ )
        msWriteByte( REG_3588, DPCPkey[i] );
#else
    for( i = 0; i < 5; i++ )
    {
        Flash_Read_Factory_KeySet( FLASH_KEY_HDCP, TRUE, i, 1, &ucValue, 1 );
        msWriteByte( REG_3588, ucValue );
    }
    for( i = 0; i < 284; i++ )
    {
        Flash_Read_Factory_KeySet( FLASH_KEY_HDCP, TRUE, i + HDCPKetAddr, 1, &ucValue, 1 );
        msWriteByte( REG_3588, ucValue );
    }
#endif
    DP_printMsg( "DPRxHDCPLoadKey OK!" );
}
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

#if DP_POWER_SAVING
    DPRxPollingDPCDPowerSave();
#endif

    if( m_ucState!=eSTATE_POWERSAVE)
    DPRxHdcpReAuthen();


#if DPCTS
     if(m_ucState!=eSTATE_POWERSAVE)
    {
            DPRxCDRLossHander();                         //CCJ 20110321
    }
#endif

#if DP_POWER_SAVING
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

   // msWriteByte( REG_1F2F, m_ucState);

    DPState = m_ucState;

    switch( m_ucState )
    {

        case eSTATE_STARTUP:
            msWriteByte( REG_1FE0, msReadByte( REG_1FE0 ) | 0x10 );              //[4]:Disable AUX IRQ
#if DP_SQUELCH_IRQ
            msWriteByte( REG_1FE3, msReadByte( REG_1FE3 ) | 0x80 );              //[4]:Enable AUX
#endif
            LockFailCnt = 0;
            PowerSavingCnt = 0;
            DPFindModeCnt = 0;
            DPRxOutputEnable( TRUE );
            DPRxHPDEnable();
            m_ucState = eSTATE_CKECKLOCK;
            break;
        case eSTATE_CKECKLOCK:  // Do training & Chk Lock
            if( g_bDPTrainingP0T | g_bDPTrainingP1T | ( DPRxCheckLock() ) )
            {
                DP_printMsg( "<Normal> " );
#if DP_FastTraining
                DPRxFastTraining2Enable( FALSE );
#endif
                if( DPRxCheckLock() )
                {
                    DPRxMainStrmRst();
                    Clr_DPPACKET_FLAG();
                    DP_printMsg( "<Normal> - DP LOCK!" );
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
                    DP_printMsg( "<Normal> - DP Un_LOCK!" );
                    Delay1ms( 1 );
                    LockFailCnt++;
                    if( LockFailCnt > 3000 ) // ChkLock Fail too much time => HotPlug  worstcase:1msX250= 500ms
                    {
                        LockFailCnt = 0;
                        PowerSavingCnt = 0;
                        //g_bDPTrainingP0T=FALSE;  // clear flag
                        //g_bDPTrainingP1T=FALSE;  // clear flag
                        //if((msReadByte(REG_1F5A)&0x03)!=0x02)
                        //DPRxHPDIRQ();
                    }
                }
            }
#if DP_FastTraining
            else if( DP_GetSquelch( 0x2FFFE ) && ( !( g_bDPTrainingP0T | g_bDPTrainingP1T ) )&&( !DPRxCheckLock() ) )
            {
          #if  0
                BYTE xdata i = 0;
              #if U02
                msWriteByteMask( REG_1FE0, BIT0 | BIT1, BIT0 | BIT1 );
#endif
          if(msReadByte( REG_1FA1)&BIT0)
                 msWriteByteMask( REG_1FA1, 0 , BIT0  );
                DP_printMsg( "------>>  <Fast> " );
                msWriteByte( REG_1F0F, 0x80 + FastLane );
                if( FastRate == 0x06 )
                    DP_SetSyntheizer( DP_LBR );
                else
                    DP_SetSyntheizer( DP_HBR );
                    Clr_LOSELOCK_FLAG();
                if( msReadByte( REG_1F10 ) == 0x00 )
                    DPRxFastTraining2Enable( TRUE );
                    Delay1ms( 5 );
                if( !DPRxCheckLock() || Get_LOSELOCK_FLAG() )
                {
                    //DPRxFastTraining2Enable(FALSE);
                }
                else
                {
                    DPRxFastTraining2Enable( FALSE );
              #if U02
                    msWriteByte( REG_366F, 0x0F );
                    msWriteByte( REG_366F, 0x00 );
        #endif
                    DP_printMsg( " <Fast> - DP LOCK!" );
                    Clr_DPPACKET_FLAG();
                    m_ucState = eSTATE_MEASURE;
                }
             #if U02
                msWriteByte( REG_1FE8, 0x03 );
                msWriteByteMask( REG_1FE0, 0, BIT0 | BIT1 );
             #endif
                #else
                {
                    BYTE xdata i=0,j = 0;
                      BYTE xdata FastLane[3] ={0x04,0x02,0x01};
                      BYTE xdata FastRate[3] = {0x0A,0x06,0x1};
                  #if U02
                    msWriteByteMask( REG_1FE0, BIT0 | BIT1, BIT0 | BIT1 );
            #endif
              if(msReadByte( REG_1FA1)&BIT0)
                    msWriteByteMask( REG_1FA1, 0 , BIT0  );
                 DP_printMsg( "------>>  <Fast> " );
                    for(j=0;j<3;j++)
                    {
                        if((j==2)|| g_bDPTrainingP0T || g_bDPTrainingP1T )
                {
                    goto FastTraining_Exit;
                        }
            if( FastRate[j] == 0x06 )
                DP_SetSyntheizer( DP_LBR );
            else
                DP_SetSyntheizer( DP_HBR );
                         for(i=0;i<3;i++)
                            {
                                  BYTE temp=0;
#if 0
                if( FastRate[j] == 0x06 )
                DP_SetSyntheizer( DP_LBR );
                else
                DP_SetSyntheizer( DP_HBR );
#endif
                            ForceDelay1ms( 5 );
                                  msWriteByte( REG_1F0F, 0x80 + FastLane[i] );
                            Clr_LOSELOCK_FLAG();
                            if( (msReadByte( REG_1F10 )&0x03) == 0x00 )
                            {       if(msReadByte( REG_1F10)&BIT5)
                              msWriteByteMask( REG_1F10, 0 , BIT5  );
                                    DPRxFastTraining2Enable( TRUE );
                            }
                            ForceDelay1ms( 20 );
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
                             }
                            }
FastTraining_OK:
                    if(!DPRxCheckTiming1(3))
                    {
                         msWriteByteMask( REG_1F10, BIT5 , BIT5  );
            }
FastTraining_Exit:
                            DPRxFastTraining2Enable( FALSE );
                      #if U02
                            msWriteByte( REG_366F, 0x0F );
                            msWriteByte( REG_366F, 0x00 );
                #endif
                            DP_printMsg( " <Fast> - DP LOCK!" );
                            Clr_DPPACKET_FLAG();
                            DPCheckModeCnt=0;
                            DPFindModeCnt =0;
                            m_ucState = eSTATE_MEASURE;

                #if U02
                    msWriteByte( REG_1FE8, 0x03 );
                    msWriteByteMask( REG_1FE0, 0, BIT0 | BIT1 );
                #endif
                }//end of else
             #endif
            }
#endif
            else
            {
                Delay1ms( 1 );
                PowerSavingCnt++;
                if( PowerSavingCnt > 5000 ) // ChkLock Fail too much time => HotPlug  worstcase:5000m
                {
                    PowerSavingCnt = 0;
                    g_bDPTrainingP0T = FALSE; // clear flag
                    g_bDPTrainingP1T = FALSE; // clear flag
#if DP_POWER_SAVING
                    m_ucState = eSTATE_POWERSAVE;
#endif
                }
            }
            break;
        case eSTATE_MEASURE:
            if( Get_MSA_FLAG() )
            {
                DPRxHvFsmChk();
                Clr_VSC_FLAG();
                Clr_MSA_FLAG();
                DPFindModeCnt = 0;
                if( DPRxCheckTiming() )
                {
                	msWriteByteMask( REG_3680, BIT0, BIT1 | BIT0 ); // Add for AMD R390  1680x1050@60 no signal when DP Input
                    m_ucState = eSTATE_NORMAL;
                    NoVscPktCnt = 0;
                }
                    else if(++DPCheckModeCnt > 20)
                    {
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
#if DP_AUTO_EQ
                DPRxAutoEQEnable( TRUE );
#endif
                if( !gDPInfo.bAudioMute )
                {
                    // 111207 check it again
                  //  hw_SetMute();      //111027 Rick modified for avoid sound and video asynchronously - B39615
                  //  if(UserPrefAudioSource==AUDIO_DIGITAL)    //111118 Modify
                    {
                       // mStar_AdjustVolume(0);
                        HW_SET_AUDIO_MUTE_ENABLE();
                    }
                    gDPInfo.bAudioMute = 1;
                }
                g_bDPTrainingP0T = FALSE; // clear flag
                g_bDPTrainingP1T = FALSE; // clear flag
                m_ucState = eSTATE_CKECKLOCK;
            }
            else
            {
                m_ucState = eSTATE_NORMAL;
#if DP_AUTO_EQ
                DPRxEQInform();
#endif
                Clr_VSC_FLAG();
                DPRxTiming();
                DPRx3DInform();
#if SupportDPAudio
                DPRxAudioHandle();
#endif
#if DP_VPLLBIGChange
                DPRxVPLLHandle();
#endif
                DPRxCheckInputFormat();
            }
            break;
        case eSTATE_POWERSAVE:
            if( !g_bDPPowerDown )
            {

                  DP_printMsg( " DP ---En for Power Saving" );
                g_bDPPowerDown = TRUE;

                msWriteByteMask(REG_2B19, BIT6, BIT6); // disable DP interrupt
                DPRxClearAUXVliadStatus();
                msWriteByte( REG_1FE0, msReadByte( REG_1FE0 ) & 0xEF );               //[4]:Enable AUX IRQ
#if DP_SQUELCH_IRQ
                DPRxClearAUXVliadStatus();
                msWriteByte( REG_1FE3, msReadByte( REG_1FE3 ) & 0x7F );                //[4]:Enable SQUELCH IRQ
#endif
                msWriteByteMask(REG_2B19, 0, BIT6); // enable DP interrupt
            };
            TrainingP1Cnt = 0;               // clear TrainingP1Cnt
            TrainingP2Cnt = 0;               // clear TrainingP2Cnt
            g_bDPTrainingP0T = FALSE; // clear flag
            g_bDPTrainingP1T = FALSE; // clear flag
            m_ucState = eSTATE_DPIDLE;
#if  !(MS_PM)
            if ( PowerSavingFlag )
            {
                mcuSetMcuSpeed(IDX_MCU_CLK_XTAL);
                msWriteByteMask(REG_1ED1, BIT0, BIT0);
            }
#endif
            break;
        case eSTATE_DPIDLE:
            if(( g_bDPAUXVALID ) || ( DP_GetSquelch( 15 ) ) )
            {
                //DPRxClearAUXVliadStatus();
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
			ubHs_gen_fsm_status = msReadByte(REG_3624);

			if(((ubHs_gen_fsm_status & (ubHs_gen_fsm_status - 1)) != 0x0) ||
				(ubHs_gen_fsm_status == 0x0)) 
			{

			       msWriteByteMask(REG_3621, BIT0, BIT0);//reg_hs_gen_fsm_rst
				Delay1ms(1);
				msWriteByteMask(REG_3621, 0, BIT0);//reg_hs_gen_fsm_rst
			}
			
		    ubCheck_fsm_Times --;
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
    if((msReadByte(REG_1F10)&0x03)==0)
    {
        if(msReadByte(REG_1F74)&BIT2)
        {
                DPRxHPDDisable();
                Delay1ms( 1 );
                DP_printMsg("  DP_HDCP_LinkFail & ReHPD");
                DPRxHPDEnable();
         }
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
        m_ucState = eSTATE_MEASURE;
        g_bDPVPLLBIGChange = FALSE;                               // clear IRQ Flag
        msWriteByte( REG_1FA8, msReadByte( REG_1F8C ) ); // update NVID
        msWriteByte( REG_1FA7, msReadByte( REG_1F8B ) ); // update NVID
        msWriteByte( REG_1FA6, msReadByte( REG_1F8A ) ); // update NVID
        msWriteByte( REG_1F9E, msReadByte( REG_1F86 ) ); // update MVID
        msWriteByte( REG_1F9F, msReadByte( REG_1F87 ) ); // update MVID
        msWriteByte( REG_1FA0, msReadByte( REG_1F88 ) ); // update MVID
        DP_printMsg( "\r\nVIDEO MN BIG CHANGE" );

    }

}
#endif
BYTE DPRxPacketColor( void )
{
    BYTE temp;

    temp = InputColor_RGB;
    msWriteByteMask( REG_1FBC, 0, BIT6 );
    switch(( msReadByte( REG_1F59 ) & 0x06 ) >> 1 )
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
    msWriteByteMask( REG_1FBC, BIT6, BIT6 );
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
        DP_printData("gDPInfo.ucDPColorFormat=%d",gDPInfo.ucDPColorFormat);
        gDPInfo.ucDPColorFormat = u8Temp;
        Set_InputTimingChangeFlag();
        DP_printData("DP color=%d",u8Temp);
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
    if(CURRENT_INPUT_IS_DISPLAYPORT())//( SrcInputType == Input_Displayport)
    {
        WORD xdata Blanking = 0;
        WORD xdata Width, Front_Porch = 0, Back_Porch = 0;
        BYTE xdata temp = 0;

		// Add for AMD R390  1680x1050@60 no signal when DP Input
        if((!(msReadByte(REG_3684)&BIT2))&&(msRead2Byte(REG_3686)<400))		
                msWriteByteMask( REG_3680, 0, BIT1 | BIT0 );		
		
        gDPInfo.bDPInterlace = ( msReadByte( REG_1FA5 ) & 0x04 ) ? TRUE : FALSE;
#if 0
        if( gDPInfo.bDPInterlace == TRUE )
        {
            scWriteByteMask( SC0_E8, _BIT1 | _BIT0, _BIT1 | _BIT0 );
        }
        else
        {
            scWriteByteMask( SC0_E8, _BIT0, _BIT1 | _BIT0 );
        }
#endif
        gDPInfo.uwDPVtotal = msRead2Byte( REG_1F78 ) & 0x7FFF;
        gDPInfo.uwDPVWidth = msRead2Byte( REG_1F7C) & 0x7FFF;
        msWriteByteMask( REG_3608, BIT1 | BIT0, BIT1 | BIT0 );
        Blanking = gDPInfo.uwDPVtotal - gDPInfo.uwDPVWidth;
        Width =  Blanking / 2;
        Front_Porch = ( Blanking - Width ) / 2;
        Back_Porch = Blanking - Width - Front_Porch ;
        msWrite2Byte( REG_360C, Width );
        msWriteByte( REG_3640, Front_Porch );
        msWriteByte( REG_36A7, ( Back_Porch ) << 1 );
#if 0
        if( Width != msReadWord( REG_360D ) )
        {
            DP_printData( "gDPInfo.uwDPVtotal =%d", gDPInfo.uwDPVtotal );
            DP_printData( "gDPInfo.uwDPVWidth =%d", gDPInfo.uwDPVWidth );
            DP_printData( "Width =%d", Width );
            DP_printData( "Blanking =%d", Blanking );
            DP_printData( "Front_Porch =%d", Front_Porch );
            DP_printData( "Back_Porch =%d", Back_Porch );
            msWriteWord( REG_360D, Width );
        }
        if( Front_Porch != msReadByte( REG_3640 ) )
        {
            msWriteByte( REG_3640, Front_Porch );
        }
        if( Back_Porch != ( msReadByte( REG_36A7 ) ) >> 1 )
            msWriteByte( REG_36A7, ( Back_Porch ) << 1 );
#endif
        msWriteByteMask( REG_3601, BIT4, BIT4 );
        msWrite2Byte( REG_360A, msRead2Byte( REG_360A ) );
    }

}

#if DP_AUTO_EQ
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
            DP_printData( "\r\nAuto EQ Pass %x ", 0 );
        }
        else if( EQ_Done == TIME_OUT )
        {
            DP_printData( "\r\nAuto EQ Fail %x ", 0 );
        }
        EQ_Done = INITIAL;
        DP_printData( "\r\nBest Pre-Setting  => %x  ", msReadByte( REG_3714 ) & 0x03 );
        DP_printData( "\r\nEQ_Value  = %x  ", msReadByte( REG_3568 ) );
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
BOOL DPRxCheckTiming( void )
{
    Bool found = FALSE;
    WORD xdata LS_Clock=0;
    WORD xdata Pixel_Clock=0;
    DWORD xdata Base_N=0;
    DWORD xdata Base_M=0;


   LS_Clock = 27 * msReadByte(REG_1F0E);
   DP_printData( "\r\DP Input : LS_Clock %d ", LS_Clock );
   Base_M = ((DWORD) msRead2Byte( REG_1F87 ) <<8) + msReadByte(REG_1F86);
   DP_printData( "\r\DP Input : Base_M %x ", Base_M );
   Base_N = ((DWORD)msRead2Byte( REG_1F8B ) <<8) + msReadByte(REG_1F8A);
   DP_printData( "\r\DP Input : Base_N %x ", Base_N );
   Pixel_Clock = ((DWORD) Base_M * LS_Clock ) / Base_N ;
   DP_printData( "\r\DP Input : Pixel_Clock %d ", Pixel_Clock );

    DPRx3DInform();

    gDPInfo.bDPInterlace = ( msReadByte( REG_1FA5 ) & 0x04 ) ? TRUE : FALSE; // Read BK36 A5 Bit2 Interlance mode

    gDPInfo.uwDPHtotal = msRead2Byte( REG_1F76 ) & 0x7FFF;

    if( gDPInfo.bDPInterlace )
        gDPInfo.uwDPVtotal = ( msRead2Byte( REG_1F78 ) & 0x7FFF ) << 1;
    else
        gDPInfo.uwDPVtotal = msRead2Byte( REG_1F78 ) & 0x7FFF;

    gDPInfo.uwDPHWidth = msRead2Byte( REG_1F7A ) & 0x7FFF;

    if( gDPInfo.bDPInterlace )
    {
        gDPInfo.uwDPVWidth = ( msRead2Byte( REG_1F7C ) & 0x7FFF ) << 1;
    }
    else
    {
        gDPInfo.uwDPVWidth = msRead2Byte( REG_1F7C ) & 0x7FFF;
        gDPInfo.uwDPHStart = msRead2Byte( REG_1F7E ) & 0x7FFF;
        gDPInfo.uwDPVStart = msRead2Byte( REG_1F80 ) & 0x7FFF;
        gDPInfo.uwDPHPWS = msRead2Byte( REG_1F82 ) & 0x7FFF;
        gDPInfo.bDPHPol = ( msReadByte( REG_1F83 ) & 0x80 ) ? 1 : 0;
        gDPInfo.uwDPVPWS = msRead2Byte( REG_1F84 ) & 0x7FFF;
        gDPInfo.bDPVPol = ( msReadByte( REG_1F85 ) & 0x80 ) ? 1 : 0;
    }

    if(( gDPInfo.uwDPHWidth < 2561 ) && ( gDPInfo.uwDPVWidth < 2501 ) && ( Pixel_Clock > 10) && ( Pixel_Clock < 500)
    &&( gDPInfo.uwDPHtotal > gDPInfo.uwDPHWidth) && ( gDPInfo.uwDPVtotal > gDPInfo.uwDPVWidth))
    {
    #if DP_FastTraining
    #if USEFLASH
        UserPref_EnableFlashSaveBit( bFlashSaveMonitorBit );
    #else
         //NVRam_WriteByte(  nvrMonitorAddr( DP_Speed ), FastRate);
         //NVRam_WriteByte(  nvrMonitorAddr( DP_Lane ),  FastLane);
        #endif
    #endif
        found = TRUE;
    }

    if( found )
        Delay1ms( 60 );

    return found;
}

//**************************************************************************
//  [Function Name]:
//                  DPRxCheckTiming1()
//  [Description]
//                 DPRxCheckTiming1
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
#if DP_FastTraining
BOOL DPRxCheckTiming1( BYTE Count )
{
       Bool found = FALSE;
    WORD xdata LS_Clock=0;
    WORD xdata Pixel_Clock=0;
    DWORD xdata Base_N=0;
    DWORD xdata Base_M=0;

    while(Count--)
    {
           LS_Clock = 27 * msReadByte(REG_1F0E);
           DP_printData( "\r\DP Input : LS_Clock %d ", LS_Clock );
           Base_M = ((DWORD) msRead2Byte( REG_1F87 ) <<8) + msReadByte(REG_1F86);
           DP_printData( "\r\DP Input : Base_M %x ", Base_M );
           Base_N = ((DWORD)msRead2Byte( REG_1F8B) <<8) + msReadByte(REG_1F8A);
           DP_printData( "\r\DP Input : Base_N %x ", Base_N );
           Pixel_Clock = ((DWORD) Base_M * LS_Clock ) / Base_N ;
           DP_printData( "\r\DP Input : Pixel_Clock %d ", Pixel_Clock );

            gDPInfo.bDPInterlace = ( msReadByte( REG_1FA5 ) & 0x04 ) ? TRUE : FALSE; // Read BK36 A5 Bit2 Interlance mode

            gDPInfo.uwDPHtotal = msRead2Byte( REG_1F76 ) & 0x7FFF;

            if( gDPInfo.bDPInterlace )
                gDPInfo.uwDPVtotal = ( msRead2Byte( REG_1F78) & 0x7FFF ) << 1;
            else
                gDPInfo.uwDPVtotal = msRead2Byte( REG_1F78) & 0x7FFF;

            gDPInfo.uwDPHWidth = msRead2Byte( REG_1F7A ) & 0x7FFF;

            if( gDPInfo.bDPInterlace )
            {
                gDPInfo.uwDPVWidth = ( msRead2Byte( REG_1F7C ) & 0x7FFF ) << 1;
            }
            else
            {
                gDPInfo.uwDPVWidth = msRead2Byte( REG_1F7C ) & 0x7FFF;
                gDPInfo.uwDPHStart = msRead2Byte( REG_1F7E ) & 0x7FFF;
                gDPInfo.uwDPVStart = msRead2Byte( REG_1F80 ) & 0x7FFF;
                gDPInfo.uwDPHPWS = msRead2Byte( REG_1F82 ) & 0x7FFF;
                gDPInfo.bDPHPol = ( msReadByte( REG_1F83 ) & 0x80 ) ? 1 : 0;
                gDPInfo.uwDPVPWS = msRead2Byte( REG_1F84) & 0x7FFF;
                gDPInfo.bDPVPol = ( msReadByte( REG_1F85 ) & 0x80 ) ? 1 : 0;
            }

            if(( gDPInfo.uwDPHWidth < 2561 ) && ( gDPInfo.uwDPVWidth < 2501 ) && ( Pixel_Clock > 10) && ( Pixel_Clock < 500)
            &&( gDPInfo.uwDPHtotal > gDPInfo.uwDPHWidth) && ( gDPInfo.uwDPVtotal > gDPInfo.uwDPVWidth))
            {
                found = TRUE;
            }
            else
            {
                found = FALSE;
             break;
            }
    }
     return found;

}


#endif
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
    // BANK38
    // msWriteByte(REG_01B3,msReadByte(REG_01B3)|0x80); // clear status

#if DP_SQUELCH_IRQ
    msRegs[REG_1FEB] = 0x80; // clear interrupt
#endif
    msRegs[REG_1FE8] = 0x10; // clear interrupt

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
void DPRxEDIDLoad( void )
{
    BYTE xdata ucValue;
    WORD xdata i;
    msWriteByte( REG_1FD8, msReadByte( REG_1FD8 ) | _BIT6 ); // EDID write enable
    msWriteByte( REG_1FFE, 0x00 );                                          // EDID PORT ADDR[7:0]

#if DP_USE_INTERNAL_EDID
    for( i = 0; i < 256; i++ )
    {
        ucValue=EDID[i] ;
        msWriteByte( REG_1FFB,ucValue);// EDID[i] );
    }
#else
    for( i = 0; i < 256; i++ )
    {
        Flash_Read_Factory_KeySet( FLASH_KEY_DDCDP, TRUE, i, 1, &ucValue, 1 );
        //ucValue = FlashReadByte( HDCPKEYSET_START + BKSVDataAddr + wCount );
        msWriteByte( REG_1FFB, ucValue );
    }
#endif
    msWriteByte( REG_1FD8, msReadByte( REG_1FD8 )&( ~_BIT6 ) ); // EDID write disable
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
    // BANK36(B)
    g_bDPDPCDPOWERSAVE = (( msReadByte( REG_1F5A ) & 0x03 ) == 0x02 ) ? 1 : 0; // Read BKB 5A Bit1:0 for DPCD 600h PM mode 10=PWD 01=normal

    if( g_bDPDPCDPOWERSAVE && ( g_DPDPCDPOWERSAVECNT == ( DPDPCDPOWERSAVE_COUNTER - 1 ) ) )
        DP_printData( "DPCD 600H PWD!CNT%d", g_DPDPCDPOWERSAVECNT );

    if( g_bDPDPCDPOWERSAVE == 0 )
        g_DPDPCDPOWERSAVECNT = 0;

}
#endif
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

    msWriteByte( REG_0226, msReadByte( REG_0226 ) | _BIT2 );     // HPD ouput enable and set high
    DP_printMsg( "DPRxHPDEnable" );
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
    msWriteByte( REG_0226, msReadByte( REG_0226 )&( ~_BIT2 ) ); // HPD ouput enable and set Low
    msWriteByte( REG_0226, msReadByte( REG_0226 )&( ~_BIT0 ) );
    DP_printMsg( "DPRxHPDDisable!" );
}
#if DP_FastTraining
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
    msWriteByte( REG_3670, 0x00 );          //111216 modified DP fast training initial value
    msWriteByte( REG_3671, 0x04 );
    msWriteByte( REG_3674, 0x00 );
    msWriteByte( REG_3675, 0x04 );
    msWriteByte( REG_3678, 0x00 );
    msWriteByte( REG_3679, 0x04 );
    msWriteByte( REG_367C, 0x00 );
    msWriteByte( REG_367D, 0x04 );
    msWriteByteMask( REG_377B, BIT2, BIT2 );               // CDR input reference clock select
    msWriteByte( REG_3570, 0x00 );
    msWriteByte( REG_368E, 0x58 );
    msWriteByte( REG_367F, 0x01 );

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
    #if U02
        msWriteByte( REG_358C, 0x00 );                       // EQ done is equal to symbol lock
        msWriteByteMask( REG_3570, BIT0, BIT0 );
        msWriteByteMask( REG_37E0, 0x0F, 0x0F );
        msWriteByteMask( REG_368E, BIT1, BIT1 );
        msWriteByteMask( REG_356D, 0xF0, 0xF0);
    #endif
       msWriteByteMask( REG_368E, BIT1, BIT1 );

    }
    else
    {
        DP_printMsg( " <Fast> - Dis" );
       #if U02
        msWriteByte( REG_358C, 0x10 );
        msWriteByteMask( REG_3570, 0, BIT0 );
        msWriteByteMask( REG_37E0, 0x00, 0x0F );
        msWriteByteMask( REG_356D, 0x00, 0xF0);
    #endif
        msWriteByteMask( REG_368E, 0, BIT1 );
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
        msWrite2Byte( REG_3790, 0x2222 );
        msWriteByte( REG_3792, 0x22 );
        msWriteByte( REG_1F0E, 0x06 );
    }
    else
    {
        msWrite2Byte( REG_3790, 0x7AE1 );
        msWriteByte( REG_3792, 0x14 );
        msWriteByte( REG_1F0E, 0x0A );
    }
}
#endif
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
Bool DP_GetSquelch( DWORD Times )
{
    BOOL xdata bStstus = FALSE;

    while( Times-- )
    {
        if( !( msReadByte( REG_36A8 )&BIT1 ) )
        {
            return FALSE;
        }
    }  DP_printMsg( "------Squelch Ready " );
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
        msWriteByteMask( REG_368C, 0x0F, 0x0F );//( REG_368C, BIT3, BIT3 );
    }
    else
    {
        msWriteByteMask( REG_368C, 0x00, 0x0F );//( REG_368C, 0, BIT3 );
    }
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
    msWriteByte( REG_0226, msReadByte( REG_0226 )&( ~_BIT2 ) ); // HPD ouput enable and set Low
    Delay1ms( 1 );
    msWriteByte( REG_0226, msReadByte( REG_0226 ) | _BIT2 ); // HPD ouput enable and set high
    DP_printMsg( "DPRxHPDIRQ!" );
}

//**************************************************************************
void DPRxOutputEnable( Bool bEnable )
{

    if( bEnable )
    {
#if 1//DPPHYUPDATE
        msWriteByte( REG_356D, 0x00 ); // Band gap POWER ON
#else
        msWriteByte( REG_356D, 0x10 ); // Band gap POWER ON
#endif
        msWriteByte( REG_356C, 0x00 ); // Lane0~3 clk POWER ON
        msWriteByte( REG_35D1, 0x00 ); // VPLL   POWER ON
        msWriteByte( REG_35B3, 0x00 ); // AUPLL POWER ON
        g_bDPPowerDown = FALSE;
        DP_printMsg( "\DP Output Enable!" );
    }
    else
    {
#if 0//DPPOWERDOWN
        msWriteByte( REG_356C, 0x0F ); // Lane0~3 clk POWER DOWN
        msWriteByte( REG_35D1, 0x01 ); // VPLL   POWER DOWN
        msWriteByte( REG_35B3, 0x01 ); // AUPLL POWER DOWN
#if 1//DPPHYUPDATE
                msWriteByte(REG_356D, 0x03); // Band gap POWER DOWN
#else
        msWriteByte( REG_356D, 0x15 ); // Band gap POWER DOWN
#endif
        DP_printMsg( "DP Output Disable!" );
#endif
        g_bDPPowerDown = TRUE;
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
#if  DP_SOFTEDID
        msWriteByte( REG_1FE0, 0xEC );            //[4]:AUX IRQ [7]: Lose CDR lock  [4]:AUX IRQ [6]: CRC Start INT , [1:0] Training P1 P2 IRQ dis-mask
#else
        msWriteByte( REG_1FE0, 0xFC );            //[4]:AUX IRQ [7]: Lose CDR lock  [4]:AUX IRQ
#endif

#if DPENABLELOSECDRLOCK
     msWriteByte(REG_1FE1,0xFE);               // [0]: CDR Loss EN
#else
        msWriteByte(REG_1FE1,0xFF);               // [0]: CDR Loss DIS
#endif

#if DPENABLEMCCS
        msWriteByte( REG_1FE2, 0x7F );          // Enable  MCCS INT Add By Jonson 20090919
#else
        msWriteByte( REG_1FE2, 0xFF );            // [7]: MCCS dis-mask  [3]: interlane skew [2] AUX TimeOut
#endif

#if DPENABLEINTERLANESKEW
     msWriteByte(REG_1FE2,msReadByte(REG_1FE2)&0xF7);
#endif

#if  DP_Aux_TimeOut
     msWriteByte(REG_1FE2,msReadByte(REG_1FE2)&0xFB);    // Aux Time out.
#endif

#if SupportDPAudio
        msWriteByte( REG_1FE3, msReadByte( REG_1FE3 ) & 0xFD ); // AUPLL fressen dis-mask
#else
        msWriteByte( REG_1FE3, msReadByte( REG_1FE3 ) & 0xF7 );
#endif

#if DP_VPLLBIGChange
        msWriteByte( REG_1FE3, msReadByte( REG_1FE3 ) & 0xF7 ); // DP_VPLLBIGChange dis-mask
#endif

#if DP54G
        msWriteByte( REG_1FE3, msReadByte( REG_1FE3 ) & 0xDF ); // DP_DP54GINT dis-mask
#endif

        DP_printMsg( "\r\nDP IRQ Enable!" );

#if DP54G
        msWriteByte( REG_1FE2, msReadByte(REG_1FE2)&0xFD );
#endif

        msWriteByteMask( REG_2B19, ~( _BIT6 ), _BIT6 ); //DP
    }
    else
    {
        // #######    disable interrupt   #########################
#if 0//DPPOWERDOWN
        msWriteByte( REG_1FE0, 0xFF );
        msWriteByte( REG_1FE1, 0xFF );
        msWriteByte( REG_1FE2, 0xFF );
        msWriteByte( REG_1FE3, 0xFF );
#endif
        DP_printMsg( "\r\nDP IRQ Disable!" );

        msWriteByteMask( REG_2B19, ( _BIT6 ), _BIT6 ); // DP
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

#if 0
    DP_printData( " 10=%x", msReadByte( REG_1F10 ) );
    DP_printData( " A8=%x", msReadByte( REG_36A8 ) );
    DP_printData( " 8E=%x", msReadByte( REG_368E ) );
#endif

    g_bDPLANELOCK = FALSE;
    g_bDPCDRLOCK = FALSE;
    ucLans = msReadByte( REG_1F0F ) & 0x07;
    ucStatus0 = msReadByte( REG_1F1A );
    ucStatus1 = msReadByte( REG_1F1B );

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

    if( !( msReadByte( REG_1F1C )&_BIT0 ) ) // lost alignment
    {
        g_bDPLANELOCK = FALSE;
             //   bStstus = FALSE;                          //CCJ
    }
#if 0
    DP_printData( " 1C=%x", msReadByte( REG_1F1C ) );
    DP_printData( " 1A=%x", msReadByte( REG_1F1A ) );
#endif
    gDPInfo.bDPLock = g_bDPCDRLOCK;

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
    msWriteByte( REG_1FA1, 0x06 );           // reset MainStr Block [1] & SS Packet [2]
    Delay1ms( 5 );
    msWriteByte( REG_1FA1, 0x00 );
    DP_printMsg( "\r\nDPRxMainStrmRst" );
    Delay1ms( 10 ); //Jison 091010 for 720p23/p24/p25/p30 keeping mode change
}

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
//                  msDPInitState()
//  [Description]
//                  msDPInitState
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msDPInitState( void )
{
    BYTE xdata uncall = 1;

    m_ucState = eSTATE_STARTUP;
    PowerSavingCnt = 0;
    LockFailCnt = 0;
    TrainingTimer = 0;
    g_DPDPCDPOWERSAVECNT = 0;
    TrainingP1Cnt = 0;               // clear TrainingP1Cnt
    TrainingP2Cnt = 0;               // clear TrainingP2Cnt
    g_bDPTrainingP0T = FALSE; // clear flag
    g_bDPTrainingP1T = FALSE; // clear flag
    gDPInfo.bAudioMute = 1;
    gDPInfo.bDPDCoff = 0;
    if( uncall == 0 )
    {
        WORD HDisplay;
        WORD VDisplay;
        DPRxHPDIRQ();
        DPRxCheckAudioFreq();
        DPRxGetInputDE(&HDisplay, &VDisplay);
	#if DP_DRRfunction
	DPRxEnableDRRFunction(FALSE);
	DPRxGetDRRFlag();	
	DPRxGetPixelClk();
	DPRxGetHTotal();
	#endif
    }
	#if ENABLE_FREESYNC
	DPRxEnableDRRFunction(UserprefFreeSyncMode);
	#endif

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
            if(( u16CDR_HPDCnt == 0 ) && ( msReadByte( REG_1F10 ) == 0 ) )
            {
                if( msReadByte( REG_1F10 ) == 0 ) // now is not in traning stage
                {
                     // msWriteByte( REG_1FA1, 0x01 );
                    msWriteByte( REG_1F1C, 0x00 );
                    msWriteByte( REG_1F1C, 0x80 );
                    if( (msReadByte( REG_1F5A )&0x03)!=0x02)   //CCJ 20110727
                        DPRxHPDIRQ();                             //CCJ mark 20090925
                    g_bDPLoseCDRLockIRQ = FALSE;  //clear flag
                }
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
            if(( u16Skew_HPDCnt == 0 ) && ( msReadByte( REG_1F10 ) == 0 ) )
            {
                if( !( msReadByte( REG_1F1C )&_BIT0 ) ) // now is not in traning stage
                {

                    msWriteByte( REG_1F1C, 0x00 );
                    msWriteByte( REG_1F1C, 0x80 );
                    if( (msReadByte( REG_1F5A )&0x03)!=0x02)    //CCJ 20110727
                        DPRxHPDIRQ();                             //CCJ mark 20090925
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

    #if DP_AudioMuteEvent
     if(CURRENT_INPUT_IS_DISPLAYPORT())//(SrcInputType == Input_Displayport)
     {
           if(PowerOnFlag)
           {
                DPAudioMuteCheck();
            msWriteByteMask( REG_05CC, 0 , BIT4 );
           }
           else
           {
                 msWriteByteMask( REG_05CC,BIT4 , BIT4 );
           }
    }
   #endif

    gDPInfo.bDPNoAudioStrm = ( msReadByte( REG_1FA5 ) & 0x10 ) ? 1 : 0; // Read BK36 A5 Bit4:1 NoAudioStrm

    if( gDPInfo.bDPNoAudioStrm )
    {
        #if DP_AudioMN_FineTune
            g_bDPAUDIO_MN_FineTune = FALSE;
        #endif
        return;
    }
    else
    {
        if((( msReadByte( REG_21AA ) ) & 0x60 ) == 0x60 )
        {
            msWriteByte( REG_21AC, BIT5 | BIT6 );
            DP_AudioDebunce_Off = 0;
            // 111207 check it again
        //  mStar_AdjustVolume(UserPrefVolume);
      //  if(bPanelOnFlag&&(!hw_MutePin_States())&&(UserPrefVolume!=0))
            {
             HW_SET_AUDIO_MUTE_DISABLE();
       //      mStar_AdjustVolume(UserPrefVolume);
             gDPInfo.bAudioMute = 0;
             //hw_ClrMute();
            }

        }
        else
        {
            DP_AudioDebunce_Off++;
            if( DP_AudioDebunce_Off >= Off_Debunce )
            {
                DP_AudioDebunce_Off = 0;
                // 111207 check it again
               // hw_SetMute();
          //     mStar_AdjustVolume(0);             //Set audio mute to avoid bo sound while TimingChange - B39440
                HW_SET_AUDIO_MUTE_ENABLE();
                gDPInfo.bAudioMute = 1;
            }
        }

    }

    if( g_bDPLANELOCK == TRUE )
    {
        if( g_bDPAUPLLBIGChange && ( !gDPInfo.bDPNoAudioStrm ) )
        {
        #if DP_AudioMN_FineTune
            g_bDPAUDIO_MN_FineTune = TRUE;
        #endif
            g_bDPAUPLLBIGChange = FALSE;        // claer IRQ flag
            HLBR = msReadByte( REG_1F0E );

            if(( HLBR & 0x0F ) == 0x06 )
            {
                //msWriteByte(REG_37B8,0x22);              // APLL 512*Fs at 1.62Ghz 162Mhz MCKSEL=0
                LinkRate = 162;
            }
            else
            {
                //msWriteByte(REG_37B8,0x23);              // APLL 512*Fs at 2.7Ghz 270Mhz  MCKSEL=0
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

            DP_printData( "\r\nDP_Audio_bigchange%x ", 0 );
            Delay1ms( 100 );
            DPAUM0 = msReadByte( REG_1F8E ); //MSA MAUD_CENTER[7:0]
            DPAUM1 = msReadByte( REG_1F8F ); //MSA MAUD_CENTER[15:8]
            DPAUM2 = msReadByte( REG_1F90 ); //MSA MAUD_CENTER[23:16]
            DPAUM = ( DPAUM2 << 16 ) + ( DPAUM1 << 8 ) + ( DPAUM0 );
            DPAUN0 = msReadByte( REG_1F92 ); //MSA NAUD_CENTER[7:0]
            DPAUN1 = msReadByte( REG_1F93 ); //MSA NAUD_CENTER[15:8]
            DPAUN2 = msReadByte( REG_1F94 ); //MSA NAUD_CENTER[23:16]
            DPAUN = ( DPAUN2 << 16 ) + ( DPAUN1 << 8 ) + ( DPAUN0 );

            msWriteByte( REG_1FAE, DPAUM0 ); //UPDATE MAUD_CENTER[7:0]
            msWriteByte( REG_1FAF, DPAUM1 ); //UPDATE MAUD_CENTER[15:8]
            msWriteByte( REG_1FB0, DPAUM2 ); //UPDATE MAUD_CENTER[23:16]
            msWriteByte( REG_1FB6, DPAUN0 ); //UPDATE NAUD_CENTER[7:0]
            msWriteByte( REG_1FB7, DPAUN1 ); //UPDATE NAUD_CENTER[15:8]
            msWriteByte( REG_1FB8, DPAUN2 ); //UPDATE NAUD_CENTER[23:16]

            DPFBDIV0 = msReadByte( REG_35B0 ); //FBDIV[7:0]
            DPFBDIV1 = msReadByte( REG_35B1 ); //FBDIV[15:8]
            DPFBDIV = ( DPFBDIV1 << 8 ) + DPFBDIV0;

            // 120112 coding, modified to reduce common area code size
            //DPKM = pow( 2, ( msReadByte( REG_35AD ) & 0x0F ) ); //KM
            //DPKP = pow( 2, ( msReadByte( REG_35AC ) >> 4 ) & 0x0F ); //KP
            DPKM=(1<<( msReadByte( REG_35AD ) & 0x0F ));
            DPKP=(1<<(( msReadByte( REG_35AC ) >> 4 ) & 0x0F));

            DPFREQ = 64 * ( 214.77 / LinkRate ) * ( DPKM * DPAUN ) / ( DPKP * DPAUM ) * DPFBDIV;

            DPFREQ0 = ( DPFREQ & 0x000000FF );
            DPFREQ1 = ( DPFREQ & 0x0000FF00 ) >> 8;
            DPFREQ2 = ( DPFREQ & 0x00FF0000 ) >> 16;

            msWriteByte( REG_35B4, DPFREQ0 );           //Freq
            msWriteByte( REG_35B5, DPFREQ1 );           //Freq
            msWriteByte( REG_35B6, DPFREQ2 );           //Freq

            msWriteByte( REG_35C6, 0x2D );                  //set BW 0x2D


            msWriteByte( REG_35B3, 0x20 );           //update Freq
            msWriteByte( REG_35B3, 0x00 );           //update Freq
            //msWriteByte(0x5E,msReadByte(0xFC)); // change ID

#if 0
            DP_printData( "\r\nAUD M0 %x", DPAUM0 );
            DP_printData( "\r\nAUD M1 %x", DPAUM1 );
            DP_printData( "\r\nAUD M2 %x", DPAUM2 );
            DP_printData( "\r\nAUD N0 %x", DPAUN0 );
            DP_printData( "\r\nAUD N1 %x", DPAUN1 );
            DP_printData( "\r\nAUD N2 %x", DPAUN2 );
            DP_printData( "\r\nAUD Freq0 %x", DPFREQ0 );
            DP_printData( "\r\nAUD Freq1 %x", DPFREQ1 );
            DP_printData( "\r\nAUD Freq2 %x", DPFREQ2 );
#endif

        }
    }



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
    WORD wDPAudioFreq;

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
    else if( abs( wDPAudioFreq - 192 ) < 4 )
        ucFreqVaild = DPAUD_FREQ_192K;
    else
        ucFreqVaild = DPAUD_FREQ_ERROR;

   // DP_printData("\r\n DP ucFreqVaild %d", ucFreqVaild);

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
    if((msReadByte(REG_0509)&(BIT5|BIT4))||(msReadByte(REG_0509)&(BIT0|BIT1)))
    {
      #if DP_AudioMN_FineTune
        if(g_bDPAUDIO_MN_FineTune)
        {
           if(msReadByte(REG_0509) & BIT0)
            DPFREQ += 0x100;
           else
            DPFREQ -= 0x100;

            DPFREQ0 = ( DPFREQ & 0x000000FF );
            DPFREQ1 = ( DPFREQ & 0x0000FF00 ) >> 8;
            DPFREQ2 = ( DPFREQ & 0x00FF0000 ) >> 16;

            msWriteByte( REG_35B4, DPFREQ0 );           //Freq
            msWriteByte( REG_35B5, DPFREQ1 );           //Freq
            msWriteByte( REG_35B6, DPFREQ2 );           //Freq
            msWriteByte( REG_35B3, 0x20 );           //update Freq
            Delay1ms(5);
        }
      #endif
            //DP_printMsg("\r\nAudio Sample / Parity Error ");
            msWriteByte(REG_0509,msReadByte(REG_0509));
            msWriteByteMask(REG_05E7,BIT5|BIT6,BIT5|BIT6);
            msWriteByteMask(REG_05E7,0,BIT5|BIT6);
    }
    if(msReadByte(REG_1FE7)&(BIT0))
    {
            //DP_printMsg("\r\nNo Ls Clock");
            msWriteByte(REG_1FEB,BIT0);
    }
    if(msReadByte(REG_1FE7)&(BIT2))
    {
            //DP_printMsg("\r\n Linke Rate Change");
            msWriteByte(REG_1FEB,BIT2);
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


void DPRx3DInform( void )
{
    BYTE ucSCFmtIn = SC_FMT_IN_NORMAL;

    gDPInfo.DP_3DMode = E_DP_3D_INPUT_NO_STEREO;

#if Displayport12

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

}

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

void DPISR( void ) // CCJ1
{
#if DP_AUTO_EQ
    BYTE Temp = 0;
    WORD Time_Out = 0;
    WORD EQ_Value = 0;
#endif
#if DP_REQUEST_VSWIM
    BYTE REQ_SW_LV = 0;
#endif

#if DPENABLEMCCS // 120201
    msDP_DDC2BI();
#endif

#if DP_SQUELCH_IRQ
    if(( msRegs[REG_1FEB] & 0x80 ) || ( msRegs[REG_1FE8] & 0x10 ) )
#else
    if( msRegs[REG_1FE8] & 0x10 )        // AUX IRQ
#endif
    {

#if DPENABLEMCCS
     BYTE AUX_CMD=TRUE;
     if(( msRegs[REG_1FE8] & 0x10 )&&(msRegs[REG_1FF2] ==0x37))
        {
            AUX_CMD = FALSE;
        }
#endif

#if DP_SOFTEDID

        Length = msRegs[REG_1FF6];
        EDIDADDR = msRegs[REG_1FF2];

        if( EDIDADDR == 0x50 ) // EDID Addr
        {
            if( msRegs[REG_1FF4] == 0x50 ) // EDID READ
            {

                if( Length )
                {
                    while( Length-- )
                    {
                        if( EDIDOffSetAddr <= 0x80 )
                            msRegs[REG_1FF9] = EDID[EDIDOffSetAddr];
                        else
                            msRegs[REG_1FF9] = 0;
                        EDIDOffSetAddr++;
                    }
                }

                msRegs[REG_1FFA] = 0;
            }
            else if( msRegs[REG_1FF4] == 0x10 )
            {
                //msRegs[REG_1FFA] = 0;
            }
            else if(( msRegs[REG_1FF4] == 0x40 ) || ( msRegs[REG_1FF4] == 0x00 ) ) // EDID WRITE
            {
                msRegs[REG_1FFA] = msRegs[REG_1FFA] | BIT2;
                EDIDOffSetAddr = msRegs[REG_1FF8];

                msRegs[REG_1FFA] = 0;

            }
            else if( msRegs[REG_1FF4] == 0x00 )
            {
                msRegs[REG_1FFA] = 0;
            }
            else
            {
                msRegs[REG_1FFA] = 0x03; //DEFF

            }
        }
#endif
#if DPENABLEMCCS
    if(AUX_CMD == TRUE)
#endif
    {
        if( g_bDPAUXVALID == FALSE )
        {
            msRegs[REG_35D1] = msRegs[REG_35D1] & ( ~( _BIT0 ) );
            msRegs[REG_1ED1] = msRegs[REG_1ED1]& 0xFE;
            //mcuSetMcuSpeedIsr(MCU_SPEED_INDEX);
            g_bDPAUXVALID = TRUE;
    //        msRegs[REG_1F2E] = msRegs[REG_1F2E]+1;
            msRegs[REG_356D] = msRegs[REG_356D] & ( ~( _BIT1 | _BIT0 ) );
            msRegs[REG_35B3] = msRegs[REG_35B3] & ( ~( _BIT0 ) );
            msRegs[REG_356C] = 0;
            //msRegs[REG_3580]=0;   this BYTE no use
            msRegs[REG_37E8] = 0;
            msRegs[REG_37E9] = 0;
            msRegs[REG_37EA] = 0;
            msRegs[REG_37EB] = 0;
#if DP_SQUELCH_IRQ
            msRegs[REG_1FEB] = 0x80; // clear interrupt
            msRegs[REG_1FE3] = msRegs[REG_1FE3] | 0x80; // Mask Interrupt.
#endif
        }
        msRegs[REG_1FE8] =  0x10; // clear interrupt
        msRegs[REG_1FE0] = msRegs[REG_1FE0] | 0x10; // Mask Interrupt.
    }
         msRegs[REG_1FE8] =  0x10; // clear interrupt
    }
    // ##############################################################
    // #############            Training Rate     ################################
    // ##############################################################
#if DP54G

    if(( msRegs[REG_1FEB] & 0x20 ) || ( msRegs[REG_1FE7] & 0x20 ))       // Rate
    {
         //   msRegs[REG_1F2D] = msRegs[REG_1F2D]+1;

        if(msRegs[REG_1F0E] ==0x14)
        {
            msRegs[REG_1FB5]= msRegs[REG_1FB5] | (BIT4|BIT1);
            msRegs[REG_1F1E]= 0x66;   //0x66
            msRegs[REG_1F1F]= 0x66;
            msRegs[REG_35D7]= 0x33;
            msRegs[REG_1FE0]= msRegs[REG_1FE0] |(BIT0|BIT1);
        }
        else
        {
            msRegs[REG_1FB5]= msRegs[REG_1FB5] & (~(BIT4));
            msRegs[REG_1F1E]= 0x00;
            msRegs[REG_1F1F]= 0x00;
            msRegs[REG_35D7]= 0x77;
            msRegs[REG_1FE0]= msRegs[REG_1FE0]&(~(BIT0|BIT1));
        }
        msRegs[REG_1FEB] = 0x20;
    }
#endif

    // ##############################################################
    // #############            Training P1          ################################
    // ##############################################################
#if 1

    if(( msRegs[REG_1FE4] & 0x01 ) || ( msRegs[REG_1FE8] & 0x01 ) )    // Training P1
    {
       #if DP_FastTraining
         gDPInfo.bDPSave =0;
        if(msRegs[REG_368E]&BIT1)
        msRegs[REG_368E] =msRegs[REG_368E] & 0xFD;
         FastRate = msRegs[REG_1F0E];
         FastLane = msRegs[REG_1F0F] & 0x0F;
    #endif

#if DP_AUTO_EQ
        msRegs[REG_3700] = msRegs[REG_3700] | 0x01;
#endif

        msRegs[REG_1FA1] = 0x00;                 // CCJ 20091029
        g_bDPTrainingP0T = TRUE;
        TrainingP1Cnt++;

#if DP54G
     if(msRegs[REG_1F0E] ==0x14)
        {
            msRegs[REG_35D7]= 0x33;
            goto TP1_Exit;
         }
        else
            msRegs[REG_35D7]= 0x77;
#endif

#if DPCTS
        CTSLevel = 0;
        DPChkCnt = 0;
        DPTP1Cnt = 0;
       if(msRegs[REG_1F0E] !=0x14)
       {
        msRegs[REG_1F1E] = 0x00;
        msRegs[REG_1F1F] = 0x00;
       }
#if DP_REQUEST_VSWIM
        DP_Train_Fix = 1;
        if( DP_Train_Fix == 1 )
        {
            REQ_SW_LV = 3; //=msRegs[REG_361E]&0x03;  // SET WHAT SWING LV YOU WANT
            msRegs[REG_3592] = 0x0;                    // 12M DP main link clock lock when 270MHz.
            msRegs[REG_3593] = 0x0;                    // 12M DP main link clock lock when 270MHz.
            msRegs[REG_3596] = 0x0;                    // 12M DP main link clock lock when 162MHz.
            msRegs[REG_3597] = 0x0;                    // 12M DP main link clock lock when 162MHz.
        }
        else
        {
            msRegs[REG_3592] = 0x2D;                  // 12M DP main link clock lock when 270MHz.
            msRegs[REG_3593] = 0x01;                  // 12M DP main link clock lock when 270MHz.
            msRegs[REG_3596] = 0x6A;                    // 12M DP main link clock lock when 162MHz.
            msRegs[REG_3597] = 0x01;
        }
#endif

        while((( msRegs[REG_1FE4] & 0x03 ) == 0x01 ) && ( DPTP1Cnt < 200000 ) ) // while pattern 1 is On-Going
        {
            if(DPTP1Cnt > 199990)
         msRegs[REG_1F10] = 0x00;

            DPChkCnt++;
            DPTP1Cnt++;
            switch( msRegs[REG_1F0F] & 0x07 )
            {
                case 0x01:  // 1 lane
                    if(( msRegs[REG_1F11] & 0x03 ) == ( msRegs[REG_1F1E] & 0x03 ) ) // request reach
                        {
                        if(( msRegs[REG_1F1A] & 0x01 ) != 0x01 ) // 1 lane LOCK Fail
                        {
#if DP_REQUEST_VSWIM
                            if( DP_Train_Fix == 1 )
                            {
                                if( CTSLevel >= REQ_SW_LV )
                                {
                                    // 12M DP main link clock lock when 162MHz.
                                    msRegs[REG_3592] = 0x2D;                  // 12M DP main link clock lock when 270MHz.
                                    msRegs[REG_3593] = 0x01;                  // 12M DP main link clock lock when 270MHz.
                                    msRegs[REG_3596] = 0x6A;                    // 12M DP main link clock lock when 162MHz.
                                    msRegs[REG_3597] = 0x01;
                                    CTSLevel++;
                                }
                                else if( CTSLevel < REQ_SW_LV )
                                    CTSLevel++;  // update swing level
                                msRegs[REG_1F1E] = ( CTSLevel & 0x03 );
                            }
                            else
#endif
                            {
                                if( CTSLevel < 4 )
                                    CTSLevel++;  // update swing level
                                msRegs[REG_1F1E] =( msRegs[REG_1F1E] & 0x0C) | ( CTSLevel & 0x03 );
                            }

                            msRegs[REG_1F1C] = msRegs[REG_1F1C] & 0x7F; // updata
                            msRegs[REG_1F1C] = msRegs[REG_1F1C] | 0x80; // updata
                        }
                        }
                    break;
                case 0x02:  // 2 lanes
                    if(( msRegs[REG_1F11] & 0x03 ) == ( msRegs[REG_1F1E] & 0x03 ) ) // request reach
                        {
                        if(( msRegs[REG_1F12] & 0x03 ) == (( msRegs[REG_1F1E] & 0x30 ) >> 4 ) ) // request reach
                         {
                            if(( msRegs[REG_1F1A] & 0x11 ) != 0x11 ) // 2 lanes CDR LOCK Fail
                            {
#if DP_REQUEST_VSWIM
                                if( DP_Train_Fix == 1 )
                                {
                                    if( CTSLevel >= REQ_SW_LV )
                                    {
                                        msRegs[REG_3592] = 0x2D;                  // 12M DP main link clock lock when 270MHz.
                                        msRegs[REG_3593] = 0x01;                  // 12M DP main link clock lock when 270MHz.
                                        msRegs[REG_3596] = 0x6A;                    // 12M DP main link clock lock when 162MHz.
                                        msRegs[REG_3597] = 0x01;
                                        CTSLevel++;
                                    }
                                    else if( CTSLevel < REQ_SW_LV )
                                        CTSLevel++;  // update swing level
                                    msRegs[REG_1F1E] = ((( CTSLevel & 0x03 ) << 4 ) | ( CTSLevel & 0x03 ) );
                                }
                                else
#endif
                                {
                                    if( CTSLevel < 4 )
                                        CTSLevel++;  // update swing level
                                    msRegs[REG_1F1E] =( msRegs[REG_1F1E] & 0xCC) | ((( CTSLevel & 0x03 ) << 4 ) | ( CTSLevel & 0x03 ) );

                                }

                                msRegs[REG_1F1C] = msRegs[REG_1F1C] & 0x7F; // updata
                                msRegs[REG_1F1C] = msRegs[REG_1F1C] | 0x80; // updata
                            }
                            }
                        }
                    break;
                case 0x04:  // 4 lanes
                    if(( msRegs[REG_1F11] & 0x03 ) == ( msRegs[REG_1F1E] & 0x03 ) ) // request reach
                      {
                        if(( msRegs[REG_1F12] & 0x03 ) == (( msRegs[REG_1F1E] & 0x30 ) >> 4 ) ) // request reach
                          {
                            if(( msRegs[REG_1F13] & 0x03 ) == ( msRegs[REG_1F1F] & 0x03 ) ) // request reach
                              {
                                if(( msRegs[REG_1F14] & 0x03 ) == (( msRegs[REG_1F1F] & 0x30 ) >> 4 ) ) // request reach
                                  {
                                    if((( msRegs[REG_1F1A] & 0x11 ) != 0x11 ) | (( msRegs[REG_1F1B] & 0x11 ) != 0x11 ) ) // 4 lanes CDR LOCK Fail
                                    {

#if DP_REQUEST_VSWIM
                                        if( DP_Train_Fix == 1 )
                                        {
                                            if( CTSLevel >= REQ_SW_LV )
                                            {
                                                msRegs[REG_3592] = 0x2D;                  // 12M DP main link clock lock when 270MHz.
                                                msRegs[REG_3593] = 0x01;                  // 12M DP main link clock lock when 270MHz.
                                                msRegs[REG_3596] = 0x6A;                    // 12M DP main link clock lock when 162MHz.
                                                msRegs[REG_3597] = 0x01;
                                                CTSLevel++;
                                            }
                                            else if( CTSLevel < REQ_SW_LV )
                                                CTSLevel++;  // update swing level
                                            msRegs[REG_1F1E] = ((( CTSLevel & 0x03 ) << 4 ) | ( CTSLevel & 0x03 ) );
                                            msRegs[REG_1F1F] = ((( CTSLevel & 0x03 ) << 4 ) | ( CTSLevel & 0x03 ) );
                                        }
                                        else
#endif
                                        {
                                            if( CTSLevel < 4 )
                                                CTSLevel++;  // update swing level
                                            msRegs[REG_1F1E] =(msRegs[REG_1F1E] & 0xCC) | ((( CTSLevel & 0x03 ) << 4 ) | ( CTSLevel & 0x03 ) );
                                            msRegs[REG_1F1F] = (msRegs[REG_1F1F] & 0xCC) | ((( CTSLevel & 0x03 ) << 4 ) | ( CTSLevel & 0x03 ) );

                                        }
                                        msRegs[REG_1F1C] = msRegs[REG_1F1C] & 0x7F; // updata
                                        msRegs[REG_1F1C] = msRegs[REG_1F1C] | 0x80; // updata
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
#if DP54G
TP1_Exit:
#endif
        msRegs[REG_1FE8] = 0x01; // clear IRQ

    }


    // ##############################################################
    // #############            Training P2          ################################
    // ##############################################################
    if(( msRegs[REG_1FE4] & 0x02 ) || ( msRegs[REG_1FE8] & 0x02 ) )           // Training P2
    {
   #if 0
         if(1)
        {

        msRegs[REG_1247]=((msRegs[REG_1247] & ~0xFF)|0X07);
        msRegs[REG_1246]=((msRegs[REG_1246] & ~0xFF)|0XFC);
           msRegs[REG_35F5]=((msRegs[REG_35F5] & ~0xFF)|0X00);
              //msRegs[REG_21EE]=((msRegs[REG_21EE] & ~0xFF)|0X12);
              msRegs[REG_21EE]=((msRegs[REG_21EE] & ~0xFF)|0X17);
           msRegs[REG_1231]=((msRegs[REG_1231] & ~0x40)|0x40);
           msRegs[REG_12C0]=((msRegs[REG_12C0] & ~0xFF)|0X00);
           msRegs[REG_12C1]=((msRegs[REG_12C1] & ~0xFF)|0X00);
           msRegs[REG_1E46]=((msRegs[REG_1E46] & ~0xFF)|0X00);//
           msRegs[REG_1E47]=((msRegs[REG_1E47] & ~0xFF)|0x40);
           msRegs[REG_1EE6]=((msRegs[REG_1EE6] & ~0xFF)|0x02);//end addr
           msRegs[REG_1EE7]=((msRegs[REG_1EE7] & ~0xFF)|0X00);//start addr
           msRegs[REG_1EE4]=((msRegs[REG_1EE4] & ~0xFF)|0X80);
           msRegs[REG_1EE5]=((msRegs[REG_1EE5] & ~0xFF)|0X08);
           msRegs[REG_1EE4]=((msRegs[REG_1EE4] & ~0xFF)|0X00);
           msRegs[REG_1EE5]=((msRegs[REG_1EE5] & ~0xFF)|0X08);
           msRegs[REG_1EE4]=((msRegs[REG_1EE4] & ~0xFF)|0X40); //enable dump
           msRegs[REG_1EE5]=((msRegs[REG_35F5] & ~0xFF)|0X08);

           while(1)
           {}
        }
#endif

      //  msRegs[REG_1F2C] = msRegs[REG_1F2C]+1;

        g_bDPTrainingP1T = TRUE;
        TrainingP2Cnt++;


#if DP_AUTO_EQ
        EQ_Done = INITIAL;
        while( 1 )
        {
            if( ++Time_Out > 8000 )
            {
                msRegs[REG_3700] = msRegs[REG_3700] & 0xFE;
                EQ_Done = TIME_OUT;
                break;
            }
            if(( msRegs[REG_1FE7]&_BIT6 ) | ( msRegs[REG_1FEB]&_BIT6 ) )
            {
                EQ_Done = FINISH;
                Temp = msRegs[REG_3714] & 0x03; //
                if( Temp == 0 )
                {
                    EQ_Value = msRegs[REG_3702];
                    msRegs[REG_3568] = msRegs[REG_3702];
                    msRegs[REG_3569] = msRegs[REG_3702];
                    msRegs[REG_3587] = msRegs[REG_3702];
                    msRegs[REG_3589] = msRegs[REG_3702];
                }
                else if(( Temp == 1 ) | ( Temp == 3 ) )
                {
                    EQ_Value =  msRegs[REG_3704];
                    msRegs[REG_3568] = msRegs[REG_3704];
                    msRegs[REG_3569] = msRegs[REG_3704];
                    msRegs[REG_3587] = msRegs[REG_3704];
                    msRegs[REG_3589] = msRegs[REG_3704];
                }
                else if( Temp == 2 )
                {
                    EQ_Value =  msRegs[REG_3706];
                    msRegs[REG_3568] = msRegs[REG_3706];
                    msRegs[REG_3569] = msRegs[REG_3706];
                    msRegs[REG_3587] = msRegs[REG_3706];
                    msRegs[REG_3589] = msRegs[REG_3706];
                }
                else
                    EQ_Value =  0xFF;
                msRegs[REG_1FEB] = 0x40;  // Clear Flag

                msRegs[REG_3700] = msRegs[REG_3700] & 0xFE;
                break;

            }

        }
         msRegs[REG_1FE8] = 0x02;
        return;
#endif
#if DP54G
     if(msRegs[REG_1F0E] ==0x14)
        {
            goto TP2_Exit;
        }
#endif

#if DPCTS
        CTSLevel = 0;
        DPTP2Cnt = 0;
        msRegs[REG_1F1E] = msRegs[REG_1F1E] & 0x33;
        msRegs[REG_1F1F] = msRegs[REG_1F1F] & 0x33;
        while((( msRegs[REG_1FE4] & 0x03 ) == 0x02 ) && ( DPTP2Cnt < 50000 ) ) // while pattern 2 is On-Going
        {
            if(DPTP2Cnt > 49990)
         msRegs[REG_1F10] = 0x00;

            DPChkCnt++;
            DPTP2Cnt++;
            switch( msRegs[REG_1F0F] & 0x07 )
            {
                case 0x01:  // 1 lane
                    if((( msRegs[REG_1F11] & 0x18 ) >> 3 ) == ( msRegs[REG_1F1E] & 0x0C ) ) // request reach
                      {
                            if(( msRegs[REG_1F1A] & 0x06 ) != 0x06 ) // 1 lane SYMBOL & EQ LOCK Fail
                            {
                                if( CTSLevel < 4 )
                                    CTSLevel++;  // update swing level
                                msRegs[REG_1F1E] = (msRegs[REG_1F1E] & 0x03) | (( CTSLevel & 0x03 ) << 2 );
                                msRegs[REG_1F1C] = msRegs[REG_1F1C] & 0x7F; // updata
                                msRegs[REG_1F1C] = msRegs[REG_1F1C] | 0x80; // updata
                            }
                            else
                            {
                            }
                        }
                    break;
                case 0x02:  // 2 lanes
                    if((( msRegs[REG_1F11] & 0x18 ) >> 3 ) == ( msRegs[REG_1F1E] & 0x0C ) ) // request reach
                     {
                        if((( msRegs[REG_1F12] & 0x18 ) >> 3 ) == (( msRegs[REG_1F1E] & 0xC0 ) >> 6 ) ) // request reach
                          {
                                if(( msRegs[REG_1F1A] & 0x66 ) != 0x66 ) // 2 lanes SYMBOL & EQ LOCK Fail
                                {
                                    if( CTSLevel < 4 )
                                        CTSLevel++;  // update swing level
                                    msRegs[REG_1F1E] = (msRegs[REG_1F1E] & 0x33) | ((( CTSLevel & 0x03 ) << 6 ) | (( CTSLevel & 0x03 ) << 2 ) );
                                    msRegs[REG_1F1C] = msRegs[REG_1F1C] & 0x7F; // updata
                                    msRegs[REG_1F1C] = msRegs[REG_1F1C] | 0x80; // updata
                                }
                                else
                                {
                                }
                            }
                        }
                    break;
                case 0x04:  // 4 lanes
                    if((( msRegs[REG_1F11] & 0x18 ) >> 3 ) == ( msRegs[REG_1F1E] & 0x0C ) ) // request reach
                     {
                        if((( msRegs[REG_1F12] & 0x18 ) >> 3 ) == (( msRegs[REG_1F1E] & 0xC0 ) >> 6 ) ) // request reach
                          {
                            if((( msRegs[REG_1F13] & 0x18 ) >> 3 ) == ( msRegs[REG_1F1F] & 0x0C ) ) // request reach
                              {
                                if((( msRegs[REG_1F14] & 0x18 ) >> 3 ) == (( msRegs[REG_1F1F] & 0xC0 ) >> 6 ) ) // request reach
                                  {
                                    if((( msRegs[REG_1F1A]&0x66 ) != 0x66 ) || (( msRegs[REG_1F1B]&0x66) != 0x66 ) ) // 4 lanes SYMBOL & EQ LOCK Fail
                                    {
                                        if( CTSLevel < 4 )
                                            CTSLevel++;  // update swing level
                                        msRegs[REG_1F1E] = (msRegs[REG_1F1E] & 0x33) | ((( CTSLevel & 0x03 ) << 6 ) | (( CTSLevel & 0x03 ) << 2 ) );
                                        msRegs[REG_1F1F] = (msRegs[REG_1F1F] & 0x33) | ((( CTSLevel & 0x03 ) << 6 ) | (( CTSLevel & 0x03 ) << 2 ) );
                                        msRegs[REG_1F1C] = msRegs[REG_1F1C] & 0x7F; // updata
                                        msRegs[REG_1F1C] = msRegs[REG_1F1C] | 0x80; // updata

                                    }
                                    else
                                    {
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
#if DP54G
TP2_Exit:
#endif
        msRegs[REG_1FE8] = 0x02;            // clear IRQ

    }

#endif

#if DP54G
    // ##############################################################
    // #############            Training P3         ################################
    // ##############################################################
    if(( msRegs[REG_1FE6] & 0x02 ) || ( msRegs[REG_1FEA] & 0x02 ) )           // Training P2
    {
        g_bDPTrainingP1T = TRUE;
        TrainingP2Cnt++;
#if DPCTS
        CTSLevel = 0;
        DPTP2Cnt = 0;
#if DP54G
     if(msRegs[REG_1F0E] ==0x14)
        {
            goto TP3_Exit;
        }
#endif

        while((( msRegs[REG_1FE6] & 0x03 ) == 0x02 ) && ( DPTP2Cnt < 50000 ) ) // while pattern 2 is On-Going
        {
            if(DPTP2Cnt > 49990)
         msRegs[REG_1F10] = 0x00;

            DPChkCnt++;
            DPTP2Cnt++;
            switch( msRegs[REG_1F0F] & 0x07 )
            {
                case 0x01:  // 1 lane
                    if(( msRegs[REG_204F] & 0x03 ) == ( msRegs[REG_2081] & 0x03 ) ) // request reach
                    {
                       if(( msRegs[REG_1F1A] & 0x06 ) != 0x06 ) // 1 lane SYMBOL & EQ LOCK Fail
                        {
                            if( CTSLevel < 4 )
                                CTSLevel++;  // update swing level
                            msRegs[REG_2081] =(msRegs[REG_2081]&0xFC) | ( CTSLevel & 0x03 );
                            msRegs[REG_1F1C] = msRegs[REG_1F1C] & 0x7F; // updata
                            msRegs[REG_1F1C] = msRegs[REG_1F1C] | 0x80; // updata
                        }
                        else
                        {
                        }
                        }
                    break;
                case 0x02:  // 2 lanes
                    if((( msRegs[REG_204F] & 0x03 ) ) == ( msRegs[REG_2081] & 0x03 ) ) // request reach
                     {
                        if((( msRegs[REG_204F] & 0x30 ) ) == (( msRegs[REG_2081] & 0x0C ) ) ) // request reach     //LAN1
                         {
                            if(( msRegs[REG_1F1A] & 0x66 ) != 0x66 ) // 2 lanes SYMBOL & EQ LOCK Fail
                            {
                                if( CTSLevel < 4 )
                                    CTSLevel++;  // update swing level
                                msRegs[REG_2081] =(msRegs[REG_2081]&0xF0) | ((( CTSLevel & 0x03 ) ) | (( CTSLevel & 0x03 ) << 2 ) );
                                msRegs[REG_1F1C] = msRegs[REG_1F1C] & 0x7F; // updata
                                msRegs[REG_1F1C] = msRegs[REG_1F1C] | 0x80; // updata
                            }
                            else
                            {
                            }
                         }
                        }
                    break;
                case 0x04:  // 4 lanes
                    if((( msRegs[REG_204F] & 0x03 ) ) == ( msRegs[REG_2081] & 0x03 ) ) // request reach             //LAN0
                     {
                        if((( msRegs[REG_204F] & 0x30 ) ) == ( msRegs[REG_2081] & 0x0C ) ) // request reach         //LAN1
                         {
                            if((( msRegs[REG_2050] & 0x03 ) ) == ( msRegs[REG_2081] & 0x30 ) ) // request reach     //LAN2
                             {
                                if((( msRegs[REG_2050] & 0x30 ) ) == ( msRegs[REG_2081] & 0xC0 ) ) // request reach     //LAN3
                                 {
                                    if((( msRegs[REG_1F1A] & 0x66 ) != 0x66 ) | (( msRegs[REG_1F1B] & 0x66 ) != 0x66 ) ) // 4 lanes SYMBOL & EQ LOCK Fail
                                    {
                                        if( CTSLevel < 4 )
                                            CTSLevel++;  // update swing level
                                        msRegs[REG_2081] =  ((( CTSLevel & 0x03 ) << 6 ) | (( CTSLevel & 0x03 ) << 4 ) | (( CTSLevel & 0x03 ) << 2 ) | (( CTSLevel & 0x03 ) ) );
                                        msRegs[REG_1F1C] = msRegs[REG_1F1C] & 0x7F; // updata
                                        msRegs[REG_1F1C] = msRegs[REG_1F1C] | 0x80; // updata

                                    }
                                    else
                                    {
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

#if DP54G
TP3_Exit:
#endif
        msRegs[REG_1FEA] = 0x02;            // clear IRQ

    }

#endif

    // ***************************************************
    // ****************   Aux Time Out   ********************
    // ***************************************************
#if DP_Aux_TimeOut
    if( msRegs[REG_1FEA] & BIT2 )
    {
              if(msRegs[REG_35AB]&0x07)
              {
                 msRegs[REG_1FA1]=BIT5;
                 msRegs[REG_1FA1]=0x00;
              }

         msRegs[REG_1FEA] = 0x04;     // clear IRQ
    }
#endif
    // ***************************************************
    // ****************   Video MN Big Change  ***************
    // ***************************************************
#if DP_VPLLBIGChange
    if( msRegs[REG_1FEB] & 0x08 ) // VPLL MN BIG CHANGE
    {
        g_bDPVPLLBIGChange = TRUE;
        msRegs[REG_1FEB] = 0x08;            // clear IRQ
    }
#endif
    //****************************************************

    // ***************************************************
    // ****************   Audio MN Big Change  **************
    // ***************************************************
    if( msRegs[REG_1FEB] & 0x02 ) // AU MN BIG CHANGE
    {
        g_bDPAUPLLBIGChange = TRUE;
        msRegs[REG_1FEB] = 0x02;                     // clear IRQ
    }
    //****************************************************

#if DPCTS
#if DPENABLEINTERLANESKEW
    if( msRegs[REG_1FEA] & 0x08 ) // InterlaneSkew
    {
       g_bDPInterlaneSkewIRQ = TRUE;
       SetSkew_HPDCnt(1500);	   
        //u16Skew_HPDCnt = 1500;// Add by CCJ
        //bSkew_HPDCntFlag = 1;
        msRegs[REG_1FEA] = 0x08;            // clear IRQ
    }
#endif
#endif

#if DPCTS
#if DPENABLELOSECDRLOCK
    if( msRegs[REG_1FE9] & 0x01 ) // CDRLoseLock
    {
        g_bDPLoseCDRLockIRQ = TRUE;
        SetCDR_HPDCnt(1500);
        //u16CDR_HPDCnt = 1500;
        //bCDR_HPDCntFlag = 1;
        msRegs[REG_1FE9] = 0x01;            // clear IRQ
    }
#endif
#endif

    //========================================
}

#if DP_DRRfunction
void DPRxEnableDRRFunction(Bool Enable )
{	
	if(Enable)
	{		
		DP_printMsg( "#  DRR Enable" );
		msWriteByteMask( REG_1F04 , BIT3,BIT3);
		//DPRxHPDDisable();                                                                           // HPD ouput enable and set Low
		//ForceDelay1ms( 100 );
		//DPRxHPDEnable();
	}
	else
	{
		DP_printMsg( "#  DRR Disable" );
		msWriteByteMask( REG_1F04 , 0,BIT3);
	}
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

	if(msReadByte( REG_1F15)&BIT5)
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

	if(msReadByte( REG_1F15)&BIT5)
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

	
	LS_Clock = 27 * msReadByte(REG_1F0E);
	DP_printData( "\r\DP Input : LS_Clock %d ", LS_Clock );
	Base_M = ((DWORD) msRead2Byte( REG_1F87 ) <<8) + msReadByte(REG_1F86);
	DP_printData( "\r\DP Input : Base_M %x ", Base_M );
	Base_N = ((DWORD)msRead2Byte( REG_1F8B ) <<8) + msReadByte(REG_1F8A);
	DP_printData( "\r\DP Input : Base_N %x ", Base_N );
	Pixel_Clock = ((DWORD) Base_M * LS_Clock * 100 ) / Base_N ;
	DP_printData( "\r\DP Input : Pixel_Clock %d Mhz", Pixel_Clock/100 );
	
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
	#if 0 // need test
	return ((msRead2Byte(REG_1F76)&0x7FFF)/2);
	#else
	WORD xdata HTotal;
	HTotal = msRead2Byte(REG_3686);
	HTotal *= PANEL_H_DIV;
	DP_printData( "\r\DP Input : HTotal %d ", HTotal );
	return HTotal;
	#endif
}

#endif

Bool DPRxGetInputDE(WORD* pHDE, WORD* pVDE)     //111027 Rick add for display DP InputDE on information menu - B39608
{
    Bool bstatus = FALSE;


    if(m_ucState == eSTATE_NORMAL)
    {
        bstatus = TRUE;
        *pHDE = msRead2Byte(REG_1F7A)&0x7FFF;
        *pVDE = msRead2Byte(REG_1F7C)&0x7FFF;
        DP_printData(" [DP HDE] =%d", *pHDE );
        DP_printData(" [DP VDE] =%d", *pVDE );
    }
    return bstatus;
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
