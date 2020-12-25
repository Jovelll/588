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

#define _drvDPRXAPP_7_C_
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
#include "drvDPRxApp_C.h"
#include <msScaler.h>
#include "DDC2Bi.h"
#include "MsACE.h"
#include "NVRam.h"
#include "drvMcu.h"
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

#if DPENABLEMCCS
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
#if ENABLE_DP_INPUT
code RegTbl2Type tComboDPMuxTbl[]=
{

    {REG_1428, 0x7F00, 0xFFFF},  //  Mute Event
    {REG_1506, 0x0004, 0x0004},  // HDCP issue
    {REG_14D4, 0x2680, 0xFFFF},  //
    {REG_1400, 0x0080, 0x0080},  //DP RS code disable
    {REG_174A, 0x0040, 0x0040},  //BIt15 ECO u04.17
    {REG_18E0, 0x0000, 0x0006},  //Turn off Fast train


    {REG_1600, 0x1D00, 0xFFFF},
    {REG_1602, 0x020A, 0xFFFF},
    {REG_1606, 0x0DA8, 0xFFFF},
    {REG_1608, 0x0AD4, 0xFFFF},
    {REG_160A, 0x096A, 0xFFFF},
    {REG_1612, 0x0530, 0xFFFF},
    {REG_161C, 0x0700, 0xFFFF},
    {REG_1620, 0x0700, 0xFFFF},
    {REG_1624, 0x0700, 0xFFFF},
    {REG_1628, 0x0700, 0xFFFF},
    {REG_1646, 0x003F, 0xFFFF},
    {REG_165C, 0x0009, 0xFFFF},
    {REG_163C, 0x030F, 0xFFFF},   //20121016
    {REG_1644, 0x0077, 0x00FF},
    {REG_1648, 0x1DAA, 0xFFFF},   // 20121225
    {REG_163A, 0x8000, 0x8000},   //Disparit 163B[7]
    {REG_16AA, 0x0080, 0x0080},

    {REG_1700, 0x7040, 0xFFFF},
    {REG_1706, 0x0000, 0xFFFF},   //20121023
    {REG_1710, 0x0000, 0xFFFF},
    {REG_1714, 0x3030, 0xFFFF},
    {REG_1730, 0x0000, 0xFFFF},
    {REG_1752, 0x0004, 0xFFFF},
    {REG_1760, 0x0003, 0x000F},
    {REG_17A2, 0x8001, 0xFFFF},   // 17A3[7] wbdet reset while sterring unstable
  //{REG_17AE, 0x4080, 0xFFFF},
    {REG_17AE, 0x4000, 0xFFFF}, // 20130201 - follow RD new IP driver table
  #if 1
    {REG_17B0, 0x0C0C, 0xFFFF},   //EQ
    {REG_17B2, 0x0C0C, 0xFFFF},   //EQ
  #else
    {REG_17B0, 0x0F0F, 0xFFFF},     //EQ
    {REG_17B2, 0x0F0F, 0xFFFF},     //EQ
  #endif

    {REG_17BC, 0x8050 |DP_PLL_LDO_VALUE, 0xFFFF},     // wuby, add PLL LDO 10% setting

    {REG_1782, 0x8000, 0xFFFF},
    {REG_17A8, 0x0210, 0xFFFF},    // 20121101
    {REG_17AA, 0x4334, 0xFFFF},    // Ictrl Value  // 2013/1/28
    {REG_17AC, 0x02F1, 0xFFFF},    //Override EQ   // Override Ki  //Override Ictrl  // 20121105//2013/1/28
   #if 0
    {REG_17A4, 0x1100, 0xFFFF},    //HF
  #else
   {REG_17A4, 0x3100, 0xFFFF},
  #endif
    {REG_1740, 0x0006, 0xFFFF},
    {REG_17B4, 0x4010, 0xFFFF},   // [4]: for all mode,  phase dac vring LDO +5%
    {REG_174C, 0xFFFF, 0xFFFF},
    {REG_174E, 0x3FFF, 0xFFFF},
    {REG_17BE, 0x0000, 0xFFFF},
    {REG_175A, 0xFFFE, 0xFFFF},
    {REG_175A, 0x0000, 0xFFFF},
    {REG_175C, 0xFFFE, 0xFFFF},

    {REG_01C2, 0x0004, 0xFF1F},    // port mux couldn't be set in the initial table
    {REG_01C6, 0x0000, 0xFFFF},
    {REG_01C8, 0x09F8, 0xFFFF},     // Squelch
    {REG_01CA, 0x31F8, 0xFFFF},     // Override SQ
    {REG_01CC, 0x0000, 0xFFFF},
    {REG_01CE, 0x0000, 0xFFF0},     // Clock R-term couldn't be set in the initial table

    {REG_06C0, 0x1F08, 0xFFFF},     // SQ debounce.
    {REG_067C, 0x0008, 0x00FF},     // DPCD Program Disable
    {REG_01BB, 0x0030, 0x0030},     // enable DP differential Rterm

 //====================================================
//  Different setting (Russell and Junior)
//====================================================
  #if (CHIP_ID == CHIP_TSUMD)
    {REG_1542, 0x1189, 0xFFFF},     // Override Ch0 DAC
    {REG_17A0, 0x0246, 0xFFFF},
  #elif (CHIP_ID == CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK)
    {REG_1542, 0x1189, 0xFFFF},     // Override Ch0 DAC
  #endif


};
#endif
#if  DP_GUID_SW
BYTE  xdata GUIDData[16]={0x10,0xDE,0x90,0x70,0x00,0x00,0x00,0x02,0xC0,0x7C,0x4E,0x44,0x00,0x00,0x00,0x56};
//BYTE  xdata GUIDData[16]={0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xFE};
#endif
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
BYTE code  EDID[256] =       //Port B  EDID
{
#if !ENABLE_FREESYNC
0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x3E, 0x93, 0x08, 0xC0, 0x00, 0x00, 0x00, 0x00,
0x20, 0x15, 0x01, 0x04, 0xA5, 0x33, 0x1D, 0x78, 0x22, 0x1E, 0x55, 0xA0, 0x59, 0x56, 0x9F, 0x27,
0x0D, 0x50, 0x54, 0xBD, 0x4B, 0x00, 0xD1, 0xC0, 0x81, 0x80, 0x95, 0x0F, 0x95, 0x00, 0xB3, 0x00,
0x81, 0xC0, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
0x45, 0x00, 0xFD, 0x1E, 0x11, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x0A, 0x20, 0x20,
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x56,
0x53, 0x32, 0x33, 0x30, 0x50, 0x47, 0x4C, 0x57, 0x0A, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFD,
0x00, 0x38, 0x4C, 0x1E, 0x53, 0x11, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x6E,
0x02, 0x03, 0x0C, 0xC1, 0x23, 0x09, 0x17, 0x07, 0x83, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60
#else  //freesync EDID
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

BYTE code  EDID1[256] =         //Port C  EDID
{

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
    BYTE xdata i=0;
    BYTE xdata uctemp=0;

      //Get DP
     msComboRx_Init(&DPLEG, &DPNUM);
#if (CHIP_ID == CHIP_TSUMK)
        DP_printData(" ==> CHIP_ID=CHIP_TSUMK  %x",0);
        DPNUM =0x09;
        DPLEG=0x61;
#endif
    // ##############################################
    // ##                                        BANK 6/7  Setting                          ##
    // ##############################################
    if(g_bDoDPInit)
   {

    for(Aux=0;Aux<=0x100;Aux+=0x100)
     {
             msWriteByte( REG_0679+Aux, 0xFF);                                               // clear INT
             msWriteByteMask( REG_0692+Aux, 0x1F, 0x1F);                              // clear INT
             msWriteByteMask( REG_0696+Aux, BIT2, BIT2);                             // clear INT

             msWriteByte( REG_067B+Aux, 0xFF);                                               //Mask Int
             msWriteByteMask( REG_0691+Aux, 0xF0, 0xF0);                              //Mask Int
             msWriteByteMask( REG_0696+Aux, BIT7|BIT4, BIT7|BIT4);          //Mask Int

            #if 1  //Aux 12Mhz :   FRO_CLK_12MEG

                    if((Input_Displayport_C2 == Input_Nothing)&&(Aux==0))
                      continue;
                    else if((Input_Displayport_C3 == Input_Nothing)&&(Aux==0x100))
                      continue;

             DP_printData(" Aux=%x",Aux);
             msWriteByte( REG_0192, 0x00 );                                                      // Set Aux Clk = RCOSC 12Mhz
             msWriteByteMask( REG_0603+Aux, BIT3,BIT3|BIT1  );                      // DP Number of Receiver Ports // TP3 not support.
             msWriteByte( REG_0600+Aux, 0x11);                                               // DP11
             msWriteByteMask( REG_06A8+Aux, BIT2,BIT2|BIT1);                       // AuxEnable
             msWriteByte( REG_0612+Aux, 0x0E );                                              // Aux UI Count
             msWriteByte( REG_0613+Aux, 0x0B );                                              // Aux Sample Rate
             msWriteByte( REG_0614+Aux, 0x00 );                                              // Aux Time Out Low
             msWriteByte( REG_0615+Aux, 0x08 );                                              // Aux Time Out  High
             msWriteByteMask( REG_0615+Aux, 0, BIT5|BIT6);                          // Aux Clk Src
             msWriteByte( REG_062C+Aux, 0x0E );                                              // Aux UI Count
             msWriteByte( REG_062D+Aux, 0x0B );                                              // Aux Sample Rate
             msWriteByte( REG_062E+Aux, 0x00 );                                              // Aux Time Out Low
             msWriteByte( REG_062F+Aux, 0x08 );                                              // Aux Time Out  High
             msWriteByte( REG_0630+Aux, 0x00);                                               // Aux Reply BY HW
             msWriteByteMask( REG_0631+Aux, 0, BIT0|BIT1);                         // Aux Time out Reply
             msWriteByte( REG_0632+Aux, 0xD0);                                              // Aux EDID address

             msWriteByteMask( REG_06AA+Aux, BIT7,BIT7);                             // Aux Common mode turn on
             msWriteByteMask( REG_06AA+Aux, BIT0, BIT0|BIT1);                                // Aux Rx threshold
             msWriteByteMask( REG_06AB+Aux, BIT0|BIT1|BIT2, BIT0|BIT1|BIT2);       // Aux Tx Swimming
             msWriteByteMask( REG_06A9+Aux, BIT1, BIT0|BIT1|BIT2);                        // Aux Rterm

             #if DP_OUI
	      msWriteByteMask( REG_0604+Aux, BIT0, BIT0);
	      #endif
             if(((DPNUM >=0x04)&&(DPLEG==0x61))||((DPNUM >=0x02)&&(DPLEG==0x69)))
             msWriteByteMask( REG_06A8+Aux, BIT5|BIT7,BIT5|BIT7);            //MCCS CMD Issue and dpcd override

         if((DPNUM >=0x08)&&(DPLEG==0x61))
          {
             if(Aux ==0)
                msWriteByteMask( REG_01BB, BIT4,BIT4);                  //Common mode Voltage for Port 1
             else
                msWriteByteMask( REG_01BB, BIT5,BIT5);                  //Common mode Voltage for Port 2
          }

             #if DP_RBR_Mode
               msWriteByte( REG_0601+Aux, 0x06 );                                               // Support 1.62G
             #endif

            #else    //Aux 14Mhz :  live_xtal_clk

             msWriteByte( REG_0192, 0x04 );                                                       // Set Aux Clk = RCOSC 14Mhz
             msWriteByte( REG_0600+Aux, 0x11);                                                //DP11
             msWriteByteMask( REG_06A8+Aux, BIT2,BIT2|BIT1);                     // AuxEnable
             msWriteByte( REG_0612+Aux, 0x10 );                                              // Aux UI Count
             msWriteByte( REG_0613+Aux, 0x0D );                                              // Aux Sample Rate
             msWriteByte( REG_0614+Aux, 0x08 );                                              // Aux Time Out Low
             msWriteByte( REG_0615+Aux, 0x07 );                                              // Aux Time Out  High
             msWriteByteMask( REG_0615+Aux, 0, BIT5|BIT6);                         //Aux Clk Src
             msWriteByte( REG_062C+Aux, 0x10 );                                              // Aux UI Count
             msWriteByte( REG_062D+Aux, 0x0D );                                              // Aux Sample Rate
             msWriteByte( REG_062E+Aux, 0x08 );                                              // Aux Tim  e Out Low
             msWriteByte( REG_062F+Aux, 0x07 );                                              // Aux Time Out  High
             msWriteByte( REG_0630+Aux, 0x00);                                               // Aux Reply BY HW
             msWriteByteMask( REG_0631+Aux, 0, BIT0|BIT1);                         // Aux Time out Reply
             msWriteByte( REG_0632+Aux, 0xD0);                                               // Aux EDID address
            #endif
             msWriteByteMask( REG_06AE+Aux, 0, BIT0|BIT1|BIT2);               //Disable active Squelch
             msWriteByteMask( REG_06A9+Aux, 0, BIT5);                                  //Enable Aux
            // msWriteByteMask( REG_06A9+Aux, BIT0|BIT2, BIT0|BIT1|BIT2|BIT3);       // R-term  20150716  // R-term  20170607 mark by analog RD Chunyu Chiang
             //msWriteByteMask( REG_069C+Aux, BIT6|BIT4, 0xF0);              // Set Fake Swiming/preemphesis.
             msWriteByteMask( REG_069C+Aux, BIT6, 0xF0);                             // Set Fake Swiming/preemphesis.
             //msWriteByteMask( REG_069C+Aux, 0, 0xF0);                              // Set Fake Swiming.
            #if DPENABLEMCCS
             msWriteByteMask( REG_069A+Aux, BIT1, BIT1);                            // Enable Firmware  reply MCCS
            #endif

           // msWriteByteMask( REG_06AB+Aux, 0x05, 0x07);                             // Set AUX TX swing  Aux tx swiming  20170607 mark by analog RD Chunyu Chiang
                    msWriteByteMask( REG_0642+Aux, BIT0, BIT0);                             // Enable GUID Read/Write


    #if  DP_GUID_SW
            msWriteByte( REG_0600+Aux, 0x12);
        for(uctemp =0;uctemp<=0x0F;uctemp++)
        {
            ForceDelay1ms(1);
            msWriteByte( REG_0662+(uctemp)+Aux,  GUIDData[uctemp]);
        }
    #endif

            //===============================================
            //          Port B / C  HDCP Int
            //===============================================
              msWriteByteMask( REG_067D+Aux, BIT2, 0x3C );                                 //   Disable DPCD custom address 0.
              msWriteByteMask( REG_0691+Aux, 0xF0, 0xF0);                             //  Unmask  HDCP Mask INT            <=======Int Maxk
              msWriteByte( REG_0680+Aux,0x07);                                                //  HDCP AKSV Address
              msWriteByte( REG_0681+Aux,0x80);                                                //  HDCP AKSV Address
              msWriteByte( REG_0682+Aux,0x06);                                                //  HDCP AKSV Address
            //================================================

      #if  DP_GUID_SW
            //===============================================
            //          Port B / C  DPCD 30h  Int
            //===============================================
                msWriteByteMask( REG_067D+Aux, BIT4, BIT4 );                            //   Enable DPCD custom address 1.
                //msWriteByteMask( REG_0691+Aux, 0, BIT5);                                    //  Unmask  GUID Mask INT            <=======Int Maxk
                msWriteByte( REG_0688+Aux,0x30);                                                //
                msWriteByte( REG_0689+Aux,0x00);                                                //
                msWriteByte( REG_068A+Aux,0x00);                                                //
            //================================================
       #elif DP_PHY_CTS_7214
            //===============================================
            //          Port B / C  DPCD 30h  Int
            //===============================================
            msWriteByteMask( REG_067D+Aux, BIT4, BIT4 );                            //   Enable DPCD custom address 1.
            //msWriteByteMask( REG_0691+Aux, 0, BIT5);                                    //  Unmask  GUID Mask INT            <=======Int Maxk
            msWriteByte( REG_0688+Aux,0x10);                                                //
            msWriteByte( REG_0689+Aux,0x02);                                                //
            msWriteByte( REG_068A+Aux,0x00);                                                //
       #endif
		 #if DP_OUI
              msWriteByteMask( REG_067D+Aux, BIT5, BIT5 );                         //   Disable DPCD custom address 0.
              msWriteByteMask( REG_0691+Aux, 0xF0, 0xF0);                              //  Unmask  HDCP Mask INT            <=======Int Maxk
              msWriteByte( REG_068C+Aux,0x00);                                                //  HDCP AKSV Address
              msWriteByte( REG_068D+Aux,0x03);                                                //  HDCP AKSV Address
              msWriteByte( REG_068E+Aux,0x00);                                            //
            //================================================
       	#endif
        }
    }

    #if DP_AUX_PortB_PNSWAP                                         //Aux PortB PN SWAP
         msWriteByteMask( REG_0630, BIT0,BIT0);
    #endif
    #if DP_AUX_PortC_PNSWAP                                         //Aux PortC PN SWAP
         msWriteByteMask( REG_0730, BIT0,BIT0);
    #endif

    msWriteByteMask( REG_06C2, 0, BIT5);                                          //Enable Squelch Type ( Real time)

     // ##############################################
    // ##                                        BANK 15  Setting                            ##
    // ##############################################
    msWriteByteMask( REG_1509, 0x3F,0x3F );  // mask INT
    msWriteByteMask( REG_153C, BIT1,BIT1 );  // mask INT

    // ##############################################
    // ##                                        BANK 17  Setting                            ##
    // ##############################################

    msWriteByte( REG_175A, 0xFF );  // clear INT
    msWriteByte( REG_175B, 0xFF );  // clear INT
    msWriteByte( REG_175A, 0x00 );  // clear INT
    msWriteByte( REG_175B, 0x00);  // clear INT

    msWriteByte( REG_175C, 0xFF );  // mask INT
    msWriteByte( REG_175D, 0xFF );  // mask INT

    msWriteByteMask( REG_1760, 0x03,0x0F );  // Disable VBLK override
    msWriteByteMask( REG_1707, 0, BIT0);        // AUPLL_Power on
    // ##############################################
    // ##                                        BANK 13  Setting                           ##
    // ##############################################
    msWriteByte( REG_1320, 0x21 );                                                          // Htotal Tolerance
    msWriteByte( REG_1321, 0x08 );                                                                                                                //Htotal Learning Cnt
    msWriteByte( REG_1307, 0x3F );
    msWriteByte( REG_1308, 0x1F );
    msWriteByte( REG_1304, 0xCA );                                                                //DP Interlace mode // BIT0 =0 (640X480 issue)
    msWriteByteMask( REG_1326, BIT7|BIT6|BIT5, BIT7|BIT6|BIT5|BIT0 );
    msWriteByte( REG_1301, 0x61);                                                                 //(bit3=0 use VB_ID rising as frame flag (end of active))
    msWriteByte( REG_1302, 0x07);
  //  msWriteByte( REG_1303, 0x01);
    msWriteByte( REG_1303, 0x05); //abnormal display in YUV422 with AMD RX460  201707/27
    msWriteByteMask( REG_1304, 0, BIT4);
    msWriteByteMask( REG_1322, BIT5, BIT5 );
    if(((DPNUM >=0x04)&&(DPLEG==0x61))||((DPNUM >=0x02)&&(DPLEG==0x69)))
             msWriteByteMask( REG_1375,BIT7,BIT7);                                       //FIFO Reset issue

    msWriteByteMask( REG_1325, BIT4, BIT4 );	      // 20151029  HV Regen reset.
    // ##############################################
    // ##                                        BANK 14  Setting                            ##
    // ##############################################
    msWriteByte( REG_1418, 0xFF );  // mask INT
    msWriteByteMask( REG_1419, 0xFF,0xFF );  // mask INT
    msWriteByteMask( REG_146C, BIT2|BIT3|BIT4, BIT2|BIT3|BIT4);
    msWriteByteMask(REG_1410,BIT0,BIT0);                                                 //Set Video M/N
    msWriteByteMask( REG_1471, BIT4|BIT5, BIT4|BIT5);                           //A/V Big change update to syn
    msWriteByte( REG_1473, 0x07);                                                                //use VBID Config
    msWriteByte( REG_1474, 0x61);                                                                //use VBID Config
    msWriteByte( REG_1475, 0x0F);                                                                //use VBID Config

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
     msWriteByteMask( REG_18E4, 0 , BIT3 );                                                 //  Enable SQ check in fast training
     msWriteByte( REG_18E6,0x0E );                                                               //   Xtal clk for Fast train timer
    // ##############################################
    // ##                                        BANK 16 Setting                            ##
    // ##############################################
    msWriteByteMask( REG_1649, BIT6|BIT5|BIT3|BIT2, BIT6|BIT5|BIT3|BIT2|BIT1 );
    msWriteByte( REG_1644, 0x77);                                                                        //EQ done is equal to symbol lock
    msWrite2Byte( REG_16C0,0x0168);                                                                                     //CR lock value for RBR
    // ##############################################
    // ##                                        BANK 01 Setting                            ##
    // ##############################################
     msWriteByteMask( REG_01C9, BIT3, BIT3 );                                                   //enable dp setting for pm
    if(((DPNUM >=0x04)&&(DPLEG==0x61))||((DPNUM >=0x02)&&(DPLEG==0x69)))
     msWriteByteMask( REG_0181, BIT3, BIT3 );

    // ##############################################
    //  if(!g_bDoDPInit)
    // drvmStar_SetupInputPort_DisplayPort();
    // ##############################################
   if(g_bDoDPInit)
   {
    DPRxEDIDLoad();
#if !ENABLE_HDCP
    DPRxHDCPLoadKey();
#endif
    DPRxAuxInit();
    gDPInfo.ucDPColorFormat=InputColor_RGB;
    DPRxIRQEnable( FALSE );
    DP_EnableSquelch( TRUE );
#if DP_FastTraining
    DPRxFastTrainingInitial();
#endif
    DPSetOffLine();
	#if ENABLE_FREESYNC
		DPRxEnableDRRFunction(UserprefFreeSyncMode);
	#endif

    Delay1ms(1);
    SetPortCHPDHigh();
    SetPortBHPDHigh();
    Delay1ms(10);
   }
     g_bDoDPInit =0;

    #if 0
     {
         BYTE CCJ=1;

         while(CCJ)
            {};
      }
     #endif
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

      //Get DP
     msComboRx_Init(&DPLEG, &DPNUM);

    msWriteByteMask( REG_06C2, 0, BIT5);                                          //Enable Squelch Type ( Real time)
     // ##############################################
    // ##                                        BANK 15  Setting                            ##
    // ##############################################
    msWriteByteMask( REG_1509, 0x3F,0x3F );  // mask INT
    msWriteByteMask( REG_153C, BIT1,BIT1 );  // mask INT

    // ##############################################
    // ##                                        BANK 17  Setting                            ##
    // ##############################################

    msWriteByte( REG_175A, 0xFF );  // clear INT
    msWriteByte( REG_175B, 0xFF );  // clear INT
    msWriteByte( REG_175A, 0x00 );  // clear INT
    msWriteByte( REG_175B, 0x00);  // clear INT

    msWriteByte( REG_175C, 0xFF );  // mask INT
    msWriteByte( REG_175D, 0xFF );  // mask INT

    msWriteByteMask( REG_1760, 0x03,0x0F );  // Disable VBLK override
    msWriteByteMask( REG_1707, 0, BIT0);        // AUPLL_Power on
    // ##############################################
    // ##                                        BANK 13  Setting                           ##
    // ##############################################
    msWriteByte( REG_1320, 0x21 );                                                          // Htotal Tolerance
    //msWriteByte( REG_1320, 0x61 );
    msWriteByte( REG_1307, 0x3F );
    msWriteByte( REG_1308, 0x1F );
    msWriteByte( REG_1304, 0xCA );                                                                //DP Interlace mode // BIT0 =0 (640X480 issue)
    msWriteByteMask( REG_1326, BIT7|BIT6, BIT7|BIT6|BIT0 );
    msWriteByte( REG_1301, 0x61);                                                                 //(bit3=0 use VB_ID rising as frame flag (end of active))
    msWriteByte( REG_1302, 0x07);
    msWriteByte( REG_1303, 0x01);
    msWriteByteMask( REG_1304, 0, BIT4);
    msWriteByteMask( REG_1322, BIT5, BIT5 );
    if(((DPNUM >=0x04)&&(DPLEG==0x61))||((DPNUM >=0x02)&&(DPLEG==0x69)))
             msWriteByteMask( REG_1375,BIT7,BIT7);                                       //FIFO Reset issue


    // ##############################################
    // ##                                        BANK 14  Setting                            ##
    // ##############################################
    msWriteByte( REG_1418, 0xFF );  // mask INT
    msWriteByteMask( REG_1419, 0xFF,0xFF );  // mask INT
    msWriteByteMask( REG_146C, BIT2|BIT3|BIT4, BIT2|BIT3|BIT4);
    msWriteByteMask(REG_1410,BIT0,BIT0);                                                 //Set Video M/N
    msWriteByteMask( REG_1471, BIT4|BIT5, BIT4|BIT5);                           //A/V Big change update to syn
    msWriteByte( REG_1473, 0x07);                                                                //use VBID Config
    msWriteByte( REG_1474, 0x61);                                                                //use VBID Config
    msWriteByte( REG_1475, 0x0F);                                                                //use VBID Config

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
     msWriteByteMask( REG_18E4, 0 , BIT3 );                                                 //  Enable SQ check in fast training
     msWriteByte( REG_18E6,0x0E );                                                               //   Xtal clk for Fast train timer
    // ##############################################
    // ##                                        BANK 16 Setting                            ##
    // ##############################################
    msWriteByteMask( REG_1649, BIT6|BIT5|BIT3|BIT2, BIT6|BIT5|BIT3|BIT2|BIT1 );
    msWriteByte( REG_1644, 0x77);                                                                        //EQ done is equal to symbol lock
    msWrite2Byte( REG_16C0,0x0168);                                                                                     //CR lock value for RBR
    // ##############################################
    // ##                                        BANK 01 Setting                            ##
    // ##############################################
     msWriteByteMask( REG_01C9, BIT3, BIT3 );                                                   //enable dp setting for pm
    if(((DPNUM >=0x04)&&(DPLEG==0x61))||((DPNUM >=0x02)&&(DPLEG==0x69)))
     msWriteByteMask( REG_0181, BIT3, BIT3 );



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
    return;

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
    msWriteByteMask( REG_150F,BIT6|BIT7, BIT6|BIT7 );  // HDCP_Enable
    msWriteByteMask( REG_1508,BIT5, BIT5 );                     // HDCP_Enable CPU access
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
 /*
#if 1
#if DP_USE_INTERNAL_HDCP
    for( i = 0; i < 5; i++ )
     {
        msWriteByte( REG_1902, DPCPBKSV[i] );


     }
    for( i = 0; i < 284; i++ )
        msWriteByte( REG_1902, DPCPkey[i] );
#else
    for( i = 0; i < 5; i++ )
    {
        Flash_Read_Factory_KeySet( FLASH_KEY_HDCP, TRUE, i, 1, &ucValue, 1 );
        msWriteByte( REG_1902, ucValue );
    }
    for( i = 0; i < 284; i++ )
    {
        Flash_Read_Factory_KeySet( FLASH_KEY_HDCP, TRUE, i + HDCPKetAddr, 1, &ucValue, 1 );
        msWriteByte( REG_1902, ucValue );
    }
#endif
#endif
 */
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
     //DPClearOffLineData();

    if(!CURRENT_INPUT_IS_DISPLAYPORT())
    return;

    DPGetAuxInfo();

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
            if((!DPRxCheckSQTime(100)) ||( g_bDPTrainingP0T | g_bDPTrainingP1T ) )
                     m_ucState = eSTATE_STARTUP;
            else if (gDPInfo.bSquelchStable)
            {       DPRxClrSQTime();
                    if(DPFastLink(FALSE))
                    {  m_ucState = eSTATE_MEASURE;
                        break;
                    }
                    if(DPFastLink(TRUE))
                         m_ucState = eSTATE_MEASURE;
                    else
                         m_ucState = eSTATE_STARTUP;
            }
            break;
        case eSTATE_RETRAIN:
             if((CURRENT_INPUT_IS_DISPLAYPORT_PortC())&&(HDCPPortCFlag))
                    Clr_HDCPPortCFlag();
             else if ((CURRENT_INPUT_IS_DISPLAYPORT_PortB())&&(HDCPPortBFlag))
                    Clr_HDCPPortBFlag();
            DP_printMsg( "<RETRAIN> " );
            InputTimingStableCounter = 1;
            DPFastTrainOff();
            DPRxHPDDisable();                                                                           // HPD ouput enable and set Low
            ForceDelay1ms( 100 );
            DPRxHPDEnable();
            m_ucState = eSTATE_STARTUP;
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
        #if 1   //FAST Training
            else if( gDPInfo.bSquelchStable && ( !( g_bDPTrainingP0T || g_bDPTrainingP1T ) )&&( !DPRxCheckLock() ) )
            {
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
            	  DPRxHvFsmChk();
                Clr_VSC_FLAG();
                Clr_MSA_FLAG();
                DPFindModeCnt = 0;
                    if( DPRxCheckTiming(MSACnt))
                    {
                         msWriteByteMask( REG_1320,  BIT0, BIT1 | BIT0 );
                        #if DP_RESETHTT_PATCH
                DPMSA_Reset();
                msWriteByte( REG_170B, 0x0A );                  //set BW
                SetAudio_BWCnt(2000);
                //u16Audio_BWCnt =2000;                        //20130716
                //bAudio_BWCntFlag=1;
                        #endif
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

                Clr_BOOSTAUX_CM();

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
             //   DP_printMsg( " DP ---En for Power Saving" );
                g_bDPPowerDown = TRUE;
                //DPEventEnable(TRUE);
            };
            TrainingP1Cnt = 0;               // clear TrainingP1Cnt
            TrainingP2Cnt = 0;               // clear TrainingP2Cnt
            g_bDPTrainingP0T = FALSE; // clear flag
            g_bDPTrainingP1T = FALSE; // clear flag
            DPRxClrSQTime();
            m_ucState = eSTATE_DPIDLE;
#if 0
#if  !(MS_PM)
            if ( PowerSavingFlag )
            {
                mcuSetMcuSpeed(IDX_MCU_CLK_XTAL);
                msWriteByteMask(REG_1ED1, BIT0, BIT0);
            }
#endif
#endif
            break;
        case eSTATE_DPIDLE:
            if( g_bDPAUXVALID )
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
			ubHs_gen_fsm_status = msReadByte(REG_1348);

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

    if(SrcInputType == Input_Displayport3)

            Aux = 0x100;
    else

            Aux = 0x0;

    while( Times-- )
    {
        if(!((( msReadByte( REG_06E8 + Aux )&0x38)>>3)==AuxIdle))
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
    #if  !DP_DPCDREPLACE

          BYTE xdata i=0,j = 0;
          BYTE xdata FastLane[3] ={0x04,0x02,0x01};
          BYTE xdata FastRate[3] = {0x0A,0x06,0x1};

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

                                                msWriteByte( REG_1881, 0x80 + FastLane[i] );
                                                if( (msReadByte( REG_1882 )&0x03) == 0x00 )
                                                        {
                                                                      if(msReadByte( REG_1882)&BIT5)
                                                                      msWriteByteMask( REG_1882, 0 , BIT5  );
                                                                      DPRxFastTraining2Enable( TRUE );

                                                         }
                                                                     ForceDelay1ms( 5 );
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

                                     ForceDelay1ms( 50 );
                                     msWriteByte( REG_1446, 0xFF);
                                     DP_printMsg( " <Fast> - DP Lock!" );
                                     DP_printMsg( " <Fast> - DP Lock!--> Enable " );
                                    if(!DPRxCheckTiming(MSACnt))
                                    {          DP_printMsg( " <Fast> - DP Lock!--> Disable " );
                                                msWriteByteMask( REG_1882, BIT5, BIT5  );
                                              if(!DPRxCheckTiming(MSACnt))
                                                {
                                                    DP_printMsg( " <Fast> - DP Lock!--> Enable " );
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
                                    return TRUE;
                FastTraining_Exit:
                                    DP_printMsg( " <Fast> - DP Lose Lock!" );
                                    DPRxFastTraining2Enable( FALSE );
                                    Clr_DPPACKET_FLAG();
                                    DPCheckModeCnt=0;
                                    DPFindModeCnt =0;
                                   return TRUE;

     #else

          BYTE xdata i=0,j = 0;
          BYTE xdata FastLane[3] ={0x04,0x02,0x01};
          BYTE xdata FastRate[3] = {0x0A,0x06,0x1};

          msWriteByteMask( REG_17A2, BIT7 , BIT7 );                 // enable Control register to replace DPCD.

           if(bSearch == TRUE)
           {
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
                                                        msWriteByteMask( REG_17A3, FastLane[i] , 0x1F);
                                                       // if( (msReadByte( REG_1882 )&0x03) == 0x00 )
                                                        {
                                                                      if(msReadByte( REG_17AB)&BIT4)
                                                                      msWriteByteMask( REG_17AB, 0 , BIT4  );
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

                        msWriteByteMask( REG_17A3, OffFastLane, 0x1F);

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
                                     ForceDelay1ms( 50 );
                                     DP_printMsg( " <Fast> - DP Lock!" );
                                    if(!DPRxCheckTiming(MSACnt))
                                    {         msWriteByteMask( REG_17AB, BIT4, BIT4  );
                                               ForceDelay1ms( 50 );
                                              if(!DPRxCheckTiming(MSACnt))
                                                msWriteByteMask( REG_17AB, 0 , BIT4  );
                                    }
                                     DPRxFastTraining2Enable( FALSE );
                                    return TRUE;
                FastTraining_Exit:
                                    DP_printMsg( " <Fast> - DP Lose Lock!" );
                                    DPRxFastTraining2Enable( FALSE );
                                    Clr_DPPACKET_FLAG();
                                    DPCheckModeCnt=0;
                                    DPFindModeCnt =0;
                                   return FALSE;

     #endif

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
    return;
     if( bEnable )
    {
             DPRxClearAUXVliadStatus();
             msWriteByteMask( REG_067B + Aux , 0, BIT4 );                                             //[4]:Enable AUX IRQ
        #if DP_SQUELCH_IRQ
             DPRxClearAUXVliadStatus();
             msWriteByteMask( REG_067B + Aux, 0, BIT5 );                                               //[5]:Enable SQ
        #endif
    }
    else
    {
             msWriteByteMask( REG_067B + Aux , BIT4, BIT4 );                                        //[4]:Disable AUX IRQ
        #if DP_SQUELCH_IRQ
             msWriteByteMask( REG_067B + Aux , BIT5, BIT5 );                                        //[5]:Disable SQ
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
                    DPRxHPDDisable();
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

    if((!(msReadByte(REG_133E)&BIT2))&&(msRead2Byte(REG_1340)<400))
    msWriteByteMask( REG_1320, 0, BIT1 | BIT0 );

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




              if(( gDPInfo.uwDPHWidth < 2561 ) && ( gDPInfo.uwDPVWidth < 2501 ) && ( Pixel_Clock > 10) && ( Pixel_Clock < 500)
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

         DPGetAuxInfo();

        #if DP_SQUELCH_IRQ
            msRegs[REG_0679 + Aux] = BIT5; // clear interrupt
        #endif
            msRegs[REG_0679 + Aux] = BIT4; // clear interrupt

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
     BYTE xdata ucTemp=1;

        msWriteByte( REG_3EEC, 0x12);                                           // EDID Set Base address
        msWriteByte( REG_3EED, 0x03);

               msWriteByteMask( REG_3EEB, 0x10, 0xF0);

        #if DP_USE_INTERNAL_EDID

            for( i = 0; i < 256; i++ )
            {
                ucValue=EDID[i] ;
                 if(i >= 128)
                {
                    msWriteByteMask( REG_3EEB, 0x20, 0xF0);
                    msWriteByte(REG_3E4B, i-128);                                      //    Address[i]      25[14:8]
                }
                 else
                    msWriteByte( REG_3E4B, i );
                msWriteByte( REG_3E4C, ucValue);                                  //    EDID[i]           25[14:8]
                 msWriteByteMask( REG_3E43, BIT5, BIT5);

            }

             msWriteByteMask( REG_3EEB, 0x30, 0xF0);
            for( i = 0; i < 256; i++ )
            {
                 ucValue=EDID1[i] ;
                 if(i >= 128)
                 {
                     msWriteByteMask( REG_3EEB, 0x40, 0xF0);
                     msWriteByte(REG_3E4B, i-128);                                      //    Address[i]      25[14:8]
                  }
                 else
                    msWriteByte( REG_3E4B, i );
                 msWriteByte( REG_3E4C, ucValue);                                    //    EDID[i]           25[14:8]
                 msWriteByteMask( REG_3E43, BIT5, BIT5);

            }

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
        #endif

  //  }


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

     if(CURRENT_INPUT_IS_DISPLAYPORT_PortC())
     {
           SetPortCHPDHigh();

    DP_printMsg( "DPRxHPDEnable" );
		   // HPD ouput enable and set Low  GPIO31
     }
    else
     {
           SetPortBHPDHigh();                                                                           // HPD ouput enable and set Low   GPIO52
     }

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
     if(CURRENT_INPUT_IS_DISPLAYPORT_PortC())
     {


             SetPortCHPDLow();                                                                           // HPD ouput enable and set Low GPIO31
     }
    else
     {


             SetPortBHPDLow();                                                                           // HPD ouput enable and set Low GPIO52
     }

        //    DP_printMsg( "DPRxHPDDisable!" );
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
    //msWriteByteMask( REG_17BC, BIT4, BIT4 );
    //msWriteByteMask( REG_01CB, 0, BIT4 );
    msWriteByteMask( REG_18E4,BIT3, BIT3 );
    msWriteByte( REG_18E0, 0x51 );
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
       // DP_printMsg( " <Fast> - En " );
       // msWriteByte( REG_1644, 0x00);
        msWriteByteMask( REG_01CB, 0, BIT5);
        msWriteByteMask( REG_17B5, BIT6, BIT6 );
        msWriteByteMask( REG_18E0,  BIT1|BIT2, BIT1|BIT2 );
       // msWriteByteMask( REG_1649,   0, BIT5|BIT6 );
    }
    else
    {
        return;
       // DP_printMsg( " <Fast> - Dis" );
        msWriteByte( REG_1644, 0x77);
        msWriteByteMask( REG_01CB, BIT5, BIT5);
        msWriteByteMask( REG_17B5, 0, BIT6 );
        msWriteByteMask( REG_18E0, 0, BIT1|BIT2 );
       // msWriteByteMask( REG_1649,  BIT5, BIT5|BIT6 );
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
    //if(msReadByte(REG_1803)==0x10)
    //return;
     #if  !(MS_PM)
     MPLL_POWER_UP(TRUE);     // For Non_PM wake up.
     #endif

    if( bRate == DP_LBR )
    {
        msWriteByteMask( REG_17A8, BIT4, BIT3|BIT4|BIT5);                          //  DCDR KP Value

        msWrite2Byte( REG_1722, 0xaaab );
        msWriteByte( REG_1724, 0x2a );

        #if  !DP_DPCDREPLACE
        msWriteByte( REG_1880, 0x06 );
        #else
        msWriteByteMask( REG_17A3, BIT5|BIT6, BIT5|BIT6);                           // Replace DPCD
        #endif
    }
    else
    {
         msWriteByteMask( REG_17A8, BIT3|BIT4, BIT3|BIT4|BIT5);                 //  DCDR KP Value

         msWrite2Byte( REG_1722, 0x999a );
         msWriteByte( REG_1724, 0x19 );

         #if  !DP_DPCDREPLACE
         msWriteByte( REG_1880, 0x0A );
         #else
         msWriteByteMask( REG_17A3, BIT6, BIT5|BIT6);                                   //  Replace DPCD
         #endif
    }
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

    if(CURRENT_INPUT_IS_DISPLAYPORT_PortC())

            Temp = BIT7;
    else

            Temp = BIT6;


    while( Times-- )
    {
        if( !( msReadByte( REG_01CB )&Temp ) )
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
        if( !( msReadByte( REG_01CB )&BIT6 ) )
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
        if( !( msReadByte( REG_01CB )&BIT7 ) )
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
        msWriteByteMask( REG_01C2, BIT2, BIT2 );    //Enable Squelch
    }
    else
    {
        msWriteByteMask( REG_01C2, 0, BIT2 );         //Disable Squelch
    }

        msWriteByte( REG_01C8, 0x08 );                     //Set SQH Level
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
    return;
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
       // DP_printMsg( "\DP Output Enable!" );
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
        //DP_printMsg( "DP Output Disable!" );
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
                                                                                      //REG_1758 ~ 1759
  //   msWrite2Byte( REG_175A, 0xFFFF );                              // clear INT
  //   msWrite2Byte( REG_175A, 0x0000);                               // clear INT
  //   msWrite2Byte( REG_175B, 0xFFFF );                              // clear INT
  //   msWrite2Byte( REG_175B, 0x0000);                               // clear INT

         msWriteByteMask(REG_175B , BIT2 ,BIT2 );                   // Clear CDR Loss lock
         msWriteByteMask(REG_175B , 0       ,BIT2 );                   // Clear CDR Loss lock
         msWriteByteMask(REG_175B , BIT1 ,BIT1 );                   // Clear Interskew Lose
         msWriteByteMask(REG_175B , 0       ,BIT1 );                   // Clear Interskew Lose

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
          msWriteByteMask(REG_175D , BIT4 ,BIT4 );      // Video Change mode

        // #######    Enable interrupt   #########################
        //  DP_printMsg( "\r\nDP IRQ Enable!" );
        // msWriteByteMask( REG_2B19, ~( _BIT7 ), _BIT7 ); //DPISR

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
        // #######    disable interrupt   #########################
        //DP_printMsg( "\r\nDP IRQ Disable!" );
        //msWriteByteMask( REG_2B19, ( BIT7 ), _BIT7 ); // DPISR
    }

        DPRxAuxIRQEnable( bEnable );


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
        DPGetAuxInfo();
        msWriteByte( REG_0679+Aux, 0xFF);                                                // clear INT
        msWriteByteMask( REG_0692+Aux, 0x1E, 0x1E);                              // clear INT  // not clear DPCD
        msWriteByteMask( REG_0696+Aux, BIT2, BIT2);                               // clear INT
        msWriteByteMask( REG_0696+Aux, BIT4, BIT4);                               //Mask SQ ISR

    if( bEnable )
    {
        // #######    Enable interrupt   #########################
        //DP_printMsg( "\r\nDP IRQ Enable!" );
        //  msWriteByteMask( REG_067B,0, BIT0);         // Enable Aux Comd
        //  msWriteByteMask( REG_067B,0, BIT0);         // Enable Aux Comd
        #if DPENABLEMCCS
             msWriteByteMask( REG_067B+Aux,0, BIT7);                 // Enable MCCS
        #endif
        #if DP_Aux_TimeOut
          msWriteByteMask( REG_0696+Aux,0, BIT7);                 // Enable Aux Time out
     #endif
      #if  (DP_GUID_SW||DP_PHY_CTS_7214)
        if(DPLEG==0x61)
        {
            msWriteByteMask( REG_0691+Aux, 0, BIT5);
        }
        else if(DPLEG==0x69)
        {
            msWriteByteMask( REG_0691+Aux, 0, BIT6);
        }
      #endif
         #if ( DP_OUI)
	  	msWriteByteMask( REG_0691+Aux, 0, BIT6);
	  #endif

            if(CURRENT_INPUT_IS_DISPLAYPORT_PortC())
             {
                msWriteByteMask( REG_2B1A, _BIT2, _BIT3|_BIT2 );   //DPISR_AUX1    Enable  //DPISR_AUX0    Disbale
             }
             else
             {
                msWriteByteMask( REG_2B1A, _BIT3, _BIT3|_BIT2 );   //DPISR_AUX1    Disable  //DPISR_AUX0    Enable
             }
        // ################################

    }
    else
    {
        // #######    disable interrupt   #########################
        //DP_printMsg( "\r\nDP IRQ Disable!" );
         if(CURRENT_INPUT_IS_DISPLAYPORT_PortC())
          {
                msWriteByteMask( REG_2B1A, ( _BIT3 ), _BIT3 ); //DPISR_AUX1
          }
             else
          {
                msWriteByteMask( REG_2B1A, ( _BIT2 ), _BIT2 ); //DPISR_AUX0
           }

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
void DPGetAuxInfo( void )
{
      if(CURRENT_INPUT_IS_DISPLAYPORT_PortC())
     {
          Aux =  0x100;

     }
     else
     {
          Aux = 0;

     }

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

    memset(&gDPInfo, 0, sizeof(gDPInfo));
    DPGetAuxInfo();
#if 1
    if(msReadByte( REG_0690 + Aux)&BIT0)
    {        m_ucState = eSTATE_RETRAIN;

              if(msReadByte( REG_0690)&BIT0)
                  Set_HDCPPortBFlag();
              else
                  Set_HDCPPortCFlag();

             msWriteByte( REG_0692 + Aux, BIT0);
             msWriteByte( REG_0692 + Aux, 0);
             DP_printMsg( "#  ENABLE HDCP ==>1" );
    }
    else  if(msReadByte( REG_18B2) !=0)
    {
              DP_printMsg( "#  ENABLE HDCP ==>2" );
              m_ucState = eSTATE_RETRAIN;
    }
 #endif
    if(((HDCPPortBFlag)||(HDCPPortCFlag))||(msReadByte( REG_18B2) !=0))
        m_ucState = eSTATE_RETRAIN;
    else
    m_ucState = eSTATE_STARTUP;

    PowerSavingCnt = 0;
    LockFailCnt = 0;
    TrainingTimer = 0;
    g_DPDPCDPOWERSAVECNT = 0;
    TrainingP1Cnt = 0;                 // clear TrainingP1Cnt
    TrainingP2Cnt = 0;                 // clear TrainingP2Cnt
    g_bDPTrainingP0T = FALSE; // clear flag
    g_bDPTrainingP1T = FALSE; // clear flag
    gDPInfo.bAudioMute = 1;
    u16CDR_HPDCnt=0xFFFF;              // Clear counter
    u16Skew_HPDCnt=0xFFFF;            //Clear counter
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

        #if DP_DRRfunction
        DPRxEnableDRRFunction(FALSE);
        DPRxGetDRRFlag();
        DPRxGetPixelClk();
        DPRxGetHTotal();
        #endif
    }
	#if 0//ENABLE_FREESYNC
	DPRxEnableDRRFunction(UserprefFreeSyncMode);
	#endif
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
    #if 0
    if(CURRENT_INPUT_IS_DISPLAYPORT_PortC())
    {
        Aux =  0x100;
    }
    else
    {
        Aux = 0;
    }
    if(Enable)
    {
		DP_printMsg( "#  DRR Enable" );
		msWriteByteMask( REG_0603 + Aux, BIT6,BIT6);
    }
    else
    {
		DP_printMsg( "#  DRR Disable" );
		msWriteByteMask( REG_0603 + Aux,0 ,BIT6);
    }
    #endif

    if(Input_Displayport_C2 != Input_Nothing)
    {
         if(Enable)
         {
                  msWriteByteMask( REG_0603, BIT6,BIT6);
         }
         else
         {
                  msWriteByteMask( REG_0603,0 ,BIT6);
         }
    }

    if(Input_Displayport_C3 != Input_Nothing)
    {
         if(Enable)
         {
                  msWriteByteMask( REG_0703, BIT6,BIT6);
         }
         else
         {
                  msWriteByteMask( REG_0703,0 ,BIT6);
         }
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

    if(msReadByte( REG_1887)&BIT7)
    ucflag =TRUE;

    return ucflag;
 }//by wtx  freesync ON DP reset issue
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
    return msRead2Byte( REG_14B8 ) & 0x7FFF;
}

#endif

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

                        DPRxHPDIRQ();                                                                              //CCJ mark 20090925
                    g_bDPLoseCDRLockIRQ = FALSE;                                                       //clear flag
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

     DPPacket_Reset();

    #if DP_AudioMuteEvent
     if(CURRENT_INPUT_IS_DISPLAYPORT())//(SrcInputType == Input_Displayport)
     {
           if(PowerOnFlag)
           {
                    DPAudioSSC();
                DPAudioMuteCheck();
           }
           else
           {
                     HW_SET_AUDIO_MUTE_ENABLE();
           }
    }
   #endif

    gDPInfo.bDPNoAudioStrm = ( msReadByte( REG_136B ) & 0x10 ) ? 1 : 0; // Read BK36 A5 Bit4:1 NoAudioStrm

#if 1
    if( gDPInfo.bDPNoAudioStrm )
    {
        #if DP_AudioMN_FineTune
            g_bDPAUDIO_MN_FineTune = FALSE;
        #endif
         HW_SET_AUDIO_MUTE_ENABLE();
        return;
    }
    else
    {
        if((( msReadByte( REG_14F9 ) ) & 0x06) != 0)
        {
            DP_AudioDebunce_Off = 0;
             HW_SET_AUDIO_MUTE_DISABLE();

        }
        else
        {
            DP_AudioDebunce_Off++;
            if( DP_AudioDebunce_Off >= Off_Debunce )
            {
                DP_AudioDebunce_Off = 0;
                HW_SET_AUDIO_MUTE_ENABLE();
            }
        }
    }
#endif

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
                    // DP_printData( "\r\nDP_Audio_bigchange_Manual %x ", 0 );
             }

          if(u16Audio_BWCnt ==0)                               //20130716
          {
             msWriteByte( REG_170B, 0x2A );                  //set BW
          }

          ucCurInFreq = DPRxCheckAudioFreq();

             if( ucCurInFreq > DPAUD_FREQ_ERROR )
             {
                      if( g_bDPAUPLLBIGChange && ( !gDPInfo.bDPNoAudioStrm ) )
                        {
                                #if DP_AudioMN_FineTune
                                    g_bDPAUDIO_MN_FineTune = TRUE;
                                #endif

                                    msWriteByte( REG_1458, DPAUM0 ); //UPDATE MAUD_CENTER[7:0]
                                    msWriteByte( REG_1459, DPAUM1 ); //UPDATE MAUD_CENTER[15:8]
                                    msWriteByte( REG_145A, DPAUM2 ); //UPDATE MAUD_CENTER[23:16]
                                    msWriteByte( REG_1460, DPAUN0 ); //UPDATE NAUD_CENTER[7:0]
                                    msWriteByte( REG_1461, DPAUN1 ); //UPDATE NAUD_CENTER[15:8]
                                    msWriteByte( REG_1462, DPAUN2 ); //UPDATE NAUD_CENTER[23:16]

                              if((DPAUM_Temp == DPAUM)&&(DPAUN_Temp == DPAUN))
                             {
                                            HW_SET_AUDIO_MUTE_ENABLE();

                                DP_printData( "\r\nDP_Audio_bigchange%x ", 0 );

                                    g_bDPAUPLLBIGChange = FALSE;        // claer IRQ flag

                                    DPFBDIV0 = msReadByte( REG_1704 ); //FBDIV[7:0]
                                    DPFBDIV1 = msReadByte( REG_1705 ); //FBDIV[15:8]
                                    DPFBDIV = ( DPFBDIV1 << 8 ) + DPFBDIV0;
                                    DPKP = GetPow2(( msReadByte( REG_1700 ) >> 4 ) & 0x0F ); //KP
                                    DPKM = GetPow2((msReadByte( REG_1701 )  & 0x0F )); //KM

                                    DPFREQ = (64)  * ( 214.77 / LinkRate ) * ( DPKM * DPAUN ) / ( DPKP * DPAUM ) * DPFBDIV;

                                    DPFREQ0 = ( DPFREQ & 0x000000FF );
                                    DPFREQ1 = ( DPFREQ & 0x0000FF00 ) >> 8;
                                    DPFREQ2 = ( DPFREQ & 0x00FF0000 ) >> 16;

                                    msWriteByte( REG_1708, DPFREQ0 );           //Freq
                                    msWriteByte( REG_1709, DPFREQ1 );           //Freq
                                    msWriteByte( REG_170A, DPFREQ2 );           //Freq

                                  //  msWriteByte( REG_170B, 0x5A );                  //set BW

                                    msWriteByte( REG_1707, 0x20 );           //update Freq
                                    msWriteByte( REG_1707, 0x00 );           //update Freq
                                            gDPInfo.bAupllRun=1;

                                    msWriteByte( REG_170B, 0x0A );                  //set BW
                                    SetAudio_BWCnt(500);
                                    //u16Audio_BWCnt =500;                         //20130716
                                    //bAudio_BWCntFlag=1;

                                    HW_SET_AUDIO_MUTE_DISABLE();

                        #if 1
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
                        #endif

                                }
                            else
                            {
                                if(++DPAudio_Cnt >2)
                                {
                                    DPAUM_Temp = DPAUM;
                                    DPAUN_Temp = DPAUN;
                                    DPAudio_Cnt=0;
                                }
                            }

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
    WORD xdata wtemp;
    BYTE xdata uCnt =0;
    return;
      if(gDPInfo.bAupllRun &&(msReadByte( REG_14F9 )&(BIT1|BIT2))&&(msReadByte( REG_14EF)&(BIT0|BIT1)))
        {
               // msWriteByteMask(REG_1427, BIT4 , BIT4);
               // msWriteByteMask(REG_1427, 0 , BIT4);
              while(++ uCnt < 5)
              {     msWriteByteMask(REG_1410, BIT2 , BIT2);
                    if(msReadByte( REG_14EF)&BIT0)
                    {
                             wtemp =  msRead2Byte(REG_1708) + 0x80;
                             msWrite2Byte( REG_1708 , wtemp);
                             DP_printData( "FIFO UnderFlow", 0 );
                             DP_printData( " REG_1708=%x", msReadByte(REG_1708));
                             DP_printData( " REG_1709=%x", msReadByte(REG_1709));
                    }
                    else if(msReadByte( REG_14EF)&BIT1)
                    {
                             wtemp =  msRead2Byte(REG_1708) - 0x80;
                             msWrite2Byte( REG_1708 , wtemp);
                             DP_printData( "FIFO OverFlow", 0 );
                             DP_printData( " REG_1708=%x", msReadByte(REG_1708));
                             DP_printData( " REG_1709=%x", msReadByte(REG_1709));
                    }

                }
    }
        else
       {


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
    if(msReadByte(REG_14EE)&(BIT5|BIT4))
    {
            //DP_printMsg("\r\nAudio Sample / Parity Error ");
            msWriteByte(REG_14EF,msReadByte(REG_14EF));
            msWriteByteMask(REG_14EE,BIT5|BIT4,BIT5|BIT4);

    }
     if(msReadByte(REG_14EF)&(BIT0|BIT1))
    {
            //DP_printMsg("\r\nAudio FIFO Unde/OverRUN ");
            msWriteByte(REG_14EF,msReadByte(REG_14EF));
    }
    if(msReadByte(REG_1756)&(BIT2))
    {
            //DP_printMsg("\r\nNo Ls Clock");
            msWriteByte(REG_175A,BIT2);
    }
    if(msReadByte(REG_1757)&(BIT0))
    {
            //DP_printMsg("\r\n Linke Rate Change");
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

void DPISR( void )
{


                // ##############################################################
                // #############            Training P1          ################################
                // ##############################################################
                if( msRegs[REG_1756] & BIT7 )         // Training P1
                {
                     msRegs[REG_18E0] =(msRegs[REG_18E0] &(~(BIT2|BIT1)));          // Disable FT

                    Set_BOOSTAUX_CM();                                                                           //Set BOOST AUX CM

                    msRegs[REG_01CB] =   msRegs[REG_01CB] |BIT5;    // Switch Path for normal training
                    msRegs[REG_17B5] = (msRegs[REG_17B5] &(~BIT6));

                    if((msRegs[REG_1880]&0x0F)==0x0A)
                         msRegs[REG_17A8] =(msRegs[REG_17A8] |(BIT3));                //GCR_KP
                    else
                         msRegs[REG_17A8] =(msRegs[REG_17A8]&(~BIT3));

                 //      msRegs[REG_17A8] = msRegs[REG_17A8] & (~(BIT0|BIT2));      //Override Ki


                    g_bDPTrainingP0T = TRUE;
                    TrainingP1Cnt++;


                    CTSLevel = 0;
                    DPChkCnt = 0;
                    DPTP1Cnt = 0;
                    msRegs[REG_1802] = 0x00;
                    msRegs[REG_1803] = 0x00;


                    while((( msRegs[REG_1882] & 0x03 ) == 0x01 ) && ( DPTP1Cnt < 200000 ) ) // while pattern 1 is On-Going
                    {
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
                                                if( CTSLevel < 4 )
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
                                                if( CTSLevel < 4 )
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

                                                        if( CTSLevel < 4 )
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


                    msRegs[REG_175A] = BIT7; // clear IRQ
                    msRegs[REG_175A] = 0;       // clear IRQ
                }

                // ##############################################################
                // #############            Training P2          ################################
                // ##############################################################
                if( msRegs[REG_1756] & BIT6 )           // Training P2
                {
                    g_bDPTrainingP1T = TRUE;
                    TrainingP2Cnt++;
                    CTSLevel = 0;
                    DPTP2Cnt = 0;
                    msRegs[REG_1802] = msRegs[REG_1802] & 0x33;
                    msRegs[REG_1803] = msRegs[REG_1803] & 0x33;
                    while((( msRegs[REG_1882] & 0x03 ) == 0x02 ) && ( DPTP2Cnt < 50000 ) ) // while pattern 2 is On-Going
                    {
                        if(DPTP2Cnt > 49990)
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


                    msRegs[REG_175A] = BIT6;            // clear IRQ
                    msRegs[REG_175A] = 0;                 // clear IRQ
                }
            // ##############################################################
            // #############            Training Rate     ################################
            // ##############################################################

            if( msRegs[REG_1757] & BIT0 )       // Rate
            {
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

            //****************************************************

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

            //========================================




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
#if DP_PHY_CTS_7214
BYTE XDATA ucDataQueue[8] = {0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80}; //2017DPCTS
#endif
void DPISR_AUX0(void)
{
        if(_bit2_(msRegs[REG_2B2A]))
        {
                Aux = 0x0;
        }
        else
        {
                Aux = 0x100;
        }
            // ##############################################################
            // #############          MCCS Interrupt            ############################
            // ##############################################################

                #if DPENABLEMCCS
                 msDP_DDC2BI() ;
                #endif

            // ##############################################################
            // #############            AUX AKSV Interrupt   ############################
            // ##############################################################
 	     // ##############################################################
            // #############            OUI Interrupt   ############################
            // ##############################################################

               #if DP_OUI
                if( msRegs[REG_0690 + Aux] & BIT2 )
                {
                  	BYTE xdata RxCmd=0;
		       RxCmd = (msRegs[REG_0638 + Aux])&0x0F;

                     msRegs[REG_0630 + Aux ] = msRegs[REG_0630 + Aux ] |BIT1;
			msRegs[REG_06D3 + Aux ] =0x00;
		      msRegs[REG_0630 + Aux ] = msRegs[REG_0630 + Aux ]&(~BIT1);
                   // msRegs[REG_0692 + Aux ] = BIT2|BIT3;    //Clear
                    msRegs[REG_0692 + Aux ] = BIT2;    //Clear
                    msRegs[REG_0692 + Aux ] = 0;          //Clear
                }
            #endif
            // ##############################################################
            // #############            AUX GUID Interrupt  1 ############################
            // ##############################################################
              #if DP_GUID_SW
                if( msRegs[REG_0690 + Aux] & BIT1 )
                {
                    BYTE xdata Addre=0;
                BYTE xdata RxCmd=0;
                BYTE xdata Length=0;
                BYTE xdata Cnt=0;


               RxCmd = (msRegs[REG_0638 + Aux])&0x0F;
               Length = msRegs[REG_063A+ Aux];
               Addre=   msRegs[REG_0634+ Aux] - 0x30;
               //msRegs[REG_1838] =  msRegs[REG_1838] + 1;

                 if(RxCmd & _BIT0)    //AUX DP Tx
                     {
                            msRegs[REG_1839] =  msRegs[REG_1839] + 1;
                        msRegs[REG_0630 + Aux ] = msRegs[REG_0630 + Aux ] |BIT1;

                    while(Length--)
                    {
                        BYTE xdata temp1=10;
                        msRegs[REG_06D3 + Aux] = msRegs[ (REG_0671+Aux) - Addre - (Cnt++)];
                        while(temp1--){};
                                    temp1=10;
                    }
                    msRegs[REG_0630 + Aux ] = msRegs[REG_0630 + Aux ]&(~BIT1);

                 }
                  else    //DP Rx
                 {
                        BYTE xdata temp=10;
                        msRegs[REG_06FC + Aux]= msRegs[REG_06FC + Aux]|_BIT2;
                               while(temp--){};
                                   temp=10;
                        while(Length--)
                        {
                        BYTE xdata temp1=10;
                        msRegs[(REG_0662+Aux) + Addre + (Cnt++)] = msRegs[REG_0675 + Aux ];     // Read Rx FiFO
                        while(temp1--){};
                                    temp1=10;
                                       msRegs[REG_06FC + Aux]= msRegs[REG_06FC + Aux]|_BIT2;        // Enable read
                        }

                 }

                        msRegs[REG_0692 + Aux ] = BIT1;    //Clear
                        msRegs[REG_0692 + Aux ] = 0;          //Clear

                    }
              #elif DP_PHY_CTS_7214
                    if(((msRegs[REG_0690 + Aux] & BIT1 )&&(DPLEG==0x61))||(((msRegs[REG_0690 + Aux] & BIT2 )&&(DPLEG==0x69))))
                    {
                            WORD xdata Addre=0;
                            BYTE xdata RxCmd=0;
                            BYTE xdata Length=0;
                            BYTE xdata Cnt=0;

                           RxCmd = (msRegs[REG_0638 + Aux])&0x0F;
                           Length = msRegs[REG_063A+ Aux];
                           Addre=   ((msRegs[REG_0635+ Aux] << 8) + msRegs[REG_0634+ Aux]) - 0x210;


                             ucDataQueue[0] = msRegs[REG_16A0];
                             ucDataQueue[1] =0x80 |msRegs[REG_16A1];

                             ucDataQueue[2] = msRegs[REG_16A2];
                             ucDataQueue[3] = 0x80|msRegs[REG_16A3];

                             ucDataQueue[4] = msRegs[REG_16A4];
                             ucDataQueue[5] = 0x80|msRegs[REG_16A5];

                             ucDataQueue[6] = msRegs[REG_16A6];
                             ucDataQueue[7] = 0x80 |msRegs[REG_16A7];

                             if(RxCmd & _BIT0)    //AUX DP Tx
                             {
                                    msRegs[REG_1694] |= BIT4;
                                    msRegs[REG_1694] &= (~BIT4);
                                    msRegs[REG_1752] |= BIT1;
                                    msRegs[REG_1752] &= (~BIT1);
                             }

                             if(RxCmd & _BIT0)    //AUX DP Tx
                                 {
                                        msRegs[REG_1839] =  msRegs[REG_1839] + 1;
                                        msRegs[REG_0630 + Aux ] = msRegs[REG_0630 + Aux ] |BIT1;

                                while(Length--)
                                {
                                    BYTE xdata temp1=10;
                                    msRegs[REG_06D3 + Aux] = ucDataQueue[Addre + (Cnt++)];
                                    while(temp1--){};
                                                temp1=10;
                                }
                msRegs[REG_0630 + Aux ] = msRegs[REG_0630 + Aux ]&(~BIT1);

             }
              else    //DP Rx
             {
                    BYTE xdata temp=10;
                    msRegs[REG_06FC + Aux]= msRegs[REG_06FC + Aux]|_BIT2;
                           while(temp--){};
                               temp=10;
                    while(Length--)
                {
                    BYTE xdata temp1=10;
                    msRegs[(REG_0662+Aux) + Addre + (Cnt++)] = msRegs[REG_0675 + Aux ];     // Read Rx FiFO
                    while(temp1--){};
                                temp1=10;
                                   msRegs[REG_06FC + Aux]= msRegs[REG_06FC + Aux]|_BIT2;        // Enable read
                    }

             }

                        if(DPLEG==0x61)
                        {
                             msRegs[REG_0692 + Aux ] = BIT1;    //Clear
                                msRegs[REG_0692 + Aux ] = 0;          //Clear
                        }
                        else if(DPLEG==0x69)
                        {
                                msRegs[REG_0692 + Aux ] = BIT2;    //Clear
                                msRegs[REG_0692 + Aux ] = 0;          //Clear
                        }

                }
              #endif
            // ##############################################################
            // #############            AUX DPCD Interrupt  2 ############################
            // ##############################################################
             #if 0
                if( msRegs[REG_0690 + Aux] & BIT2 )
                {
                    msRegs[REG_06FC + Aux ] = BIT2;         // Enable read
                    msRegs[REG_0630 + Aux ] = msRegs[REG_0630 + Aux ] |BIT1;
                    msRegs[REG_06D3 + Aux ] = msRegs[REG_0634 + Aux ] ;
                    msRegs[REG_0692 + Aux ] = BIT2;    //Clear
                    msRegs[REG_0692 + Aux ] = 0;          //Clear
                    while( ! (((msRegs[REG_06E8 + Aux ])&0x38)==0x08))
                     {}
                    msRegs[REG_0630 + Aux ] = msRegs[REG_0630 + Aux ]&(~BIT1);
                }
             #endif
            // ##############################################################
            // #############            AUX Interrupt  Port1 ############################
            // ##############################################################

                if( msRegs[REG_0678 + Aux] & BIT0 )
                {
                    msRegs[REG_0679 + Aux ] = BIT0;    //Clear
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
                         msRegs[REG_065C + Aux]=0x00;
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
        msWriteByteMask( REG_1754,BIT0,BIT0);
        msWriteByteMask( REG_1754,0,BIT0);
        //=======================================================
       for(i=0;i<sizeof(tComboDPMuxTbl)/sizeof(RegTbl2Type);i++)
        msWrite2ByteMask(tComboDPMuxTbl[i].wReg,tComboDPMuxTbl[i].wValue,tComboDPMuxTbl[i].wMask );
        DP_printMsg("==>Load DPTBL");

        if(((DPNUM >=0x04)&&(DPLEG==0x61))||((DPNUM >=0x02)&&(DPLEG==0x69)))
          {
                msWrite2ByteMask(REG_170F,0x0008,0x0008);
                msWrite2ByteMask(REG_1791,0x0008,0x0008);
                msWrite2ByteMask(REG_0181,0x0008,0x0008);
                msWriteByteMask( REG_1375,BIT7,BIT7);
          }

        //=======================================================
        //      Scaler SC0 Setting
        //===============  =======================================
        msWriteByte(SC0_02, NIS_B|SCALER_STYPE_HV|SCALER_ISEL_DP);                              // Set HV seperate
        msWriteByteMask(SC0_03, _BIT7, _BIT7);                                                                         // Force Input Clock Divide Function.
        msWriteByte( SC0_04, BIT3 |BIT4| BIT7 );                                                                       //  Set HV mode
        msWriteByteMask(SC0_A6, 0, _BIT1);                                                                               // Dual in
        msWriteByteMask(SC0_A8, _BIT1|BIT0, _BIT1|BIT0);                                                     //Interlace method
        //msWriteByteMask(SC0_E6, _BIT6, _BIT6);                                                                         // DE Follow mode
        msWriteByteMask(SC0_E7, _BIT5, _BIT5);                                                                        // Htt Filter
        msWriteByteMask(SC0_E8, _BIT5|_BIT4|_BIT3|_BIT2, _BIT5|_BIT4|_BIT3|_BIT2);   // Override H/H sync
        msWriteByte( SC0_F3, 0xF0 );                                                                                              // enable DE glitch removal function & more tolerance for DE
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
        //=======================================================
        //      Scaler BK1E Setting
        //=======================================================
        msWriteByteMask( REG_1E22, 0, BIT3 );                                                                            // Disable DE-only mode for SC_TOP
        msWriteByteMask( REG_1E3E, (CLK_DVI_BUFF_DIV2<<4), 0xF0 );                                   //Clock source Select
        //=======================================================
        //      Scaler BK2E Setting
        //=======================================================
        msWriteByteMask( REG_2E02, BIT4, 0xF0 );                                                                      // IPmux 2    for DP
        msWriteByteMask( REG_2E04, BIT0, 0x0F );                                                                      // IPmux 3    for DP
        //=======================================================
        //=======================================================
        //msWriteByte( REG_0262, 0x24 );                                                                                       //For DP HPD 1M puLL High (MHL port will set this)
        //=======================================================

       #if  (CHIP_ID == CHIP_TSUMD)

       msWriteByteMask( REG_17AB, BIT4|BIT3, BIT1|BIT4|BIT3 );

       #endif

        //=======================================================
        //=======================================================
           if(SrcInputType == Input_Displayport)                                                                                       //(PortB)
         {
                #if OffLine
                while(--Count )                                                                                                                         // Check Port B active
                 {
                      if(((msReadByte(REG_06E2)&0x03)==0)&&((msReadByte(REG_06E3)&0x03)==0x01))
                      {
                             msWriteByteMask( REG_06C2, BIT3, BIT2|BIT3 );                                                                 // Port C Offline
                            if(HDCPPortBFlag)
                            {

                                    Clr_HDCPPortBFlag();
                            }
                            break;
                      }
                 }
                #else
                    while( --Count)
                     {      if((msReadByte(REG_06E2)&0x03)==0)
                             {
                                //printData("==>Input_DisplayportA =%x",msReadByte(REG_01C5));
                                //msWriteByteMask( REG_01C5,  BIT3|BIT2|BIT0 , 0x0F );                                                       // R-term
                                //msWriteByte( REG_01C4,  0x0D );
                                //msWriteByte( REG_01C5,  0x0D );
                                // _nop_();
                                //printData("==>Input_DisplayportB =%x",msReadByte(REG_01C5));
                                //msWriteByteMask( REG_01CE,  BIT2|BIT0 , BIT0|BIT1|BIT2 );                                             // R-term
                                msWriteByteMask( REG_PM_C2, BIT6, BIT7|BIT6|BIT5 );                                                          // Combo   for DP Port B      Port 1 is selected
                                msWriteByteMask( REG_06C2, BIT3, BIT2|BIT3 );                                                                      // Off Line
                               #if (CHIP_ID == CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK)
                                if(DPNUM >=0x04)
                    {
                                    #if  DP_Long_Cable
                                        msWrite2Byte( REG_17B0, 0x0E12 );                                                                                        //EQ
                                        msWrite2Byte( REG_17B2, 0x1212 );
                        #else
                            msWrite2Byte( REG_17B0, 0x0808 );                                                                                        //EQ
                            msWrite2Byte( REG_17B2, 0x0808 );
                        #endif
                    }
                                  else
                                    {
                            msWrite2Byte( REG_17B0, 0x0E12 );                                                                                        //EQ
                                        msWrite2Byte( REG_17B2, 0x1212 );

                                    }
                                #elif  (CHIP_ID == CHIP_TSUMD)
                                if(DPNUM >=2)
                                {
                                    #if  DP_Long_Cable
                        msWrite2Byte( REG_17B0, 0x0E12 );                                                                                        //EQ
                        msWrite2Byte( REG_17B2, 0x1212 );
                                    #else
                                        msWrite2Byte( REG_17B0, 0x0808 );                                                                                        //EQ
                                        msWrite2Byte( REG_17B2, 0x0808 );
                        #endif
                                }
                                else
                                {
                                msWrite2Byte( REG_17B0, 0x0606 );                                                                                         //EQ
                                msWrite2Byte( REG_17B2, 0x0606 );
                                }
                                #endif
                                msWriteByteMask( REG_077D, BIT2, BIT2 );
                                msWriteByteMask( REG_067D, 0, BIT2 );
                                msWriteByte( REG_18B2,0);
                               //msWriteByte( REG_01C4,  0x0D );
                               //msWriteByte( REG_01C5,  0x0D );
                               //msWriteByteMask( REG_01CE,  BIT2|BIT0 , BIT0|BIT1|BIT2 );
                               break;
                              }

                    }
                #endif
                // DP_printMsg("==>Input_Displayport");
         }
         else  if(SrcInputType == Input_Displayport3)                                                                              // (PortC)
         {

                #if OffLine
                while(--Count )                                                                                                                        // Check Port B active
                 {
                      if(((msReadByte(REG_07E2)&0x03)==0)&&((msReadByte(REG_07E3)&0x03)==0x01))
                      {
                             msWriteByteMask( REG_06C2, BIT2, BIT2|BIT3 );                                                                  // Port B    Offline
                             if(HDCPPortCFlag)
                            {

                                    Clr_HDCPPortCFlag();
                            }
                              break;
                      }
                 }
                #else
                   while( --Count)
                     {      if((msReadByte(REG_07E2)&0x03)==0)
                             {
                              //printData("==>Input_Displaypor3A=%x",msReadByte(REG_01C4));
                              //msWriteByteMask( REG_01C4,  BIT7|BIT6|BIT4 , 0xF0 );
                              //msWriteByte( REG_01C4,  0xDD);
                              //msWriteByte( REG_01C5,  0x00);
                              // _nop_();
                              //printData("==>Input_Displaypor3B =%x",msReadByte(REG_01C4));
                              //msWriteByteMask( REG_01CE,  BIT1|BIT0 , BIT1|BIT2|BIT0 );                                         // R-term
                              msWriteByteMask( REG_PM_C2, BIT7, BIT7|BIT6|BIT5 );                                                      // Combo   for DP Port C      Port 2 is selected
                              msWriteByteMask( REG_06C2, BIT2, BIT2|BIT3 );                                                                  // port 2 Off- line
                              #if (CHIP_ID == CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK)
                     if(DPNUM >=0x04)
                    {
                                    #if  DP_Long_Cable
                                    msWrite2Byte( REG_17B0, 0x0E17 );                                                                                         //EQ
                                    msWrite2Byte( REG_17B2, 0x1717 );
                        #else
                        msWrite2Byte( REG_17B0, 0x0808 );                                                                                        //EQ
                        msWrite2Byte( REG_17B2, 0x0808 );
                        #endif
                    }
                       else
                                    {
                        msWrite2Byte( REG_17B0, 0x0E17 );                                                                                         //EQ
                        msWrite2Byte( REG_17B2, 0x1717 );

                                    }
                              #elif  (CHIP_ID == CHIP_TSUMD)
                               if(DPNUM >=2)
                                {
                                            #if  DP_Long_Cable
                        msWrite2Byte( REG_17B0, 0x0E17 );                                                                                         //EQ
                                          msWrite2Byte( REG_17B2, 0x1717 );
                                        #else
                                        msWrite2Byte( REG_17B0, 0x0808 );                                                                                        //EQ
                                        msWrite2Byte( REG_17B2, 0x0808 );
                            #endif
                                }
                                else
                                {
                              msWrite2Byte( REG_17B0, 0x1008 );                                                                                         //EQ
                              msWrite2Byte( REG_17B2, 0x1010);
                                }
                              #endif
                              msWriteByteMask( REG_067D, BIT2, BIT2 );
                              msWriteByteMask( REG_077D, 0, BIT2 );
                              msWriteByte( REG_18B2,0);

                             //msWriteByte( REG_01C4,  0xDD);
                             //msWriteByte( REG_01C5,  0x00);
                             //msWriteByteMask( REG_01CE,  BIT1|BIT0 , BIT1|BIT2|BIT0 );
                              break;
                              }
                    }
                #endif
                //  DP_printMsg("==>Input_Displayport3");
         }

         DPRxInitState();
         DPRxIRQEnable(TRUE);
       //msWriteByteMask( REG_2B19, ~( _BIT7 ), _BIT7 ); //DPISR
       //DPRxIRQEnable(TRUE);
       //msWriteByte( REG_175C, 0xFF );  // mask INT
       //msWriteByte( REG_175D, 0xFF );  // mask INT
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
               if(((DPNUM >=0x04)&&(DPLEG==0x61))||((DPNUM >=0x02)&&(DPLEG==0x69)))
              {
                 msWriteByte(REG_06E3,0);
                 msWriteByte(REG_06E4,0);
                 msWriteByte(REG_06E5,0);
                 msWriteByte(REG_06E6,0);
                 msWriteByte(REG_07E3,0);
                 msWriteByte(REG_07E4,0);
                 msWriteByte(REG_07E5,0);
                 msWriteByte(REG_07E6,0);
              }
              else
              {
                 msWriteByteMask( REG_065C, 0,BIT0);
                 msWriteByteMask( REG_075C, 0,BIT0);
                 msWriteByteMask( REG_065C, BIT0,BIT0);
                 msWriteByteMask( REG_075C, BIT0,BIT0);
                 msWriteByte(REG_06E3,0);
                 msWriteByte(REG_06E4,0);
                 msWriteByte(REG_06E5,0);
                 msWriteByte(REG_06E6,0);
                 msWriteByte(REG_07E3,0);
                 msWriteByte(REG_07E4,0);
                 msWriteByte(REG_07E5,0);
                 msWriteByte(REG_07E6,0);
              }
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
