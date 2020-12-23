/******************************************************************************
 Copyright (c) 2003 MStar Semiconductor, Inc.
 All rights reserved.

 [Module Name]: msFb.h
 [Date]:        01-Dec-2003
 [Comment]:
   Memory access header file.
 [Reversion History]:
*******************************************************************************/
#ifndef _MSFB_H_
#define _MSFB_H_

#ifdef _MSFB_C_
#define _MSFBDEC_
#else
#define _MSFBDEC_ extern
#endif
#include "panel.h"

/* Memory Allocation
    ----------
    |   OD   |
    ----------
*/

#define MIU_OD_ADDR             0x0

#define MIU_OD_SIZE             0x200000UL
#define MIU_OD_LIMIT_SIZE       MIU_OD_SIZE

#define MIU_OD_ADDR_EVEN        (MIU_OD_ADDR)
#define MIU_OD_EVEN_LIMIT_ADDR  (MIU_OD_LIMIT_SIZE+MIU_OD_ADDR_EVEN)    // 2M

/****************************************************************************/
//        DRAM parameters
/****************************************************************************/

#define DDR3_NANYA_NT5CB64M16   0x00
#define DDR3_H5TQ1G63BFR        0x01
#define DDR3_DEFAULT            0xFF

#define DDR_SELECT              DDR3_H5TQ1G63BFR

#define tRC             60      //unit:ns
#define tRAS            40      //unit:ns
#define tRP             15      //unit:ns
#define tRCD            15      //unit:ns
#define tWR             10      //unit:ns
#define tRRD            15      //unit:ns
#define tRTP            15      //unit:ns
#define tMRD            2       //unit:CLK
#define tWTR            5       //unit:ns
#define tRTW            35
#define tRFC            70      //unit:ns
#define tWL             10      //unit:ns
#define tCCD            1
#define tCKE            4       //unit:ns
#define tREFI           2560    //unit:ns
#define MR0             0x0031
#define MR1             0x4004
#define MR2             0x8018
#define MR3             0xC000
enum
{
    _SDR        = 0x00,
    _DDR        = BIT0,
    _DDR2       = BIT1,
    _DDR3       = BIT0|BIT1,
};

enum
{
    _1PICS      = 0x00,
    _2PICS      = BIT2,
};

enum
{
    _2BK        = 0x00,
    _4BK        = BIT4,
    _8BK        = BIT5,
};

enum
{
    _8COL       = 0x00,
    _9COL       = BIT6,
    _10COL      = BIT7,
};


#define MIU_ARBITOR_CLK         200ul  //MHZ
#define MEMORY_CLOCK_MHZ        MIU_ARBITOR_CLK // MHz //issue, decrease to 250
#define MPLL_FREQ_MHZ           429ul
#define DDRIP                   3
#define IN_DIV                  1
#define LOOP_DIV1               2
#define LOOP_DIV2               3
#define POST_DIV                8
#define RAM_TYPE                (_8COL|_2BK|_1PICS|_SDR)
#define MEM_CLK_PHASE_CTRL      0x03
#define MEM_DQ_PHASE_CTRL_1     0x77
#define MEM_DQ_PHASE_CTRL_2     0x00
#define MEM_DQS_PHASE_CTRL_1    0x33
#define MEM_DQS_PHASE_CTRL_2    0x00

#define DDFSET                  ((DWORD)(524288ul*(MPLL_FREQ_MHZ*2*LOOP_DIV1*LOOP_DIV2)/(IN_DIV*POST_DIV*MEMORY_CLOCK_MHZ) + 0.5))

#define ENABLE_DDR3_SSC        _DISABLE

#define F_MODULATION            20
#define PERCENTAGE              1
#define DDFSPAN                 ((DWORD)(((float)MPLL_FREQ_MHZ*131072*1024)/(DDFSET*F_MODULATION)))
#define DDFSTEP                 ((DWORD)(((float)PERCENTAGE*DDFSET+(float)50*DDFSPAN)/((float)100*DDFSPAN)))




///////////////////////////////////////////////////////////////////////////
// Subroutines
///////////////////////////////////////////////////////////////////////////

_MSFBDEC_ void msInitMemory(void);
_MSFBDEC_ Bool msMemoryBist(void);
_MSFBDEC_ void msFBDummy(void);

#endif
