/******************************************************************************
 Copyright (c) 2003 MStar Semiconductor, Inc.
 All rights reserved.

 [Module Name]: msFb.h
 [Date]:        01-Dec-2003
 [Comment]:
   Memory access header file.
 [Reversion History]:
*******************************************************************************/
#if  CHIP_ID==CHIP_TSUMU
#include "panel.h"

#ifndef     _MSFB_H
#define     _MSFB_H


#ifdef _MS_FB_C
#define INTERFACE
#else
#define INTERFACE   extern
#endif

/* Memory Allocation
    ----------
    |RESERVED|
    |HK CODE |
    |   FB   |
    |   OD   |
    | OD MTB |
    | HK DATA|
    ----------
*/
/* Frame Buffer */
#if (FRAME_BFF_SEL == FRAME_BFFLESS)
#define MIU_FB_SIZE_MSB         0
#define MIU_FB_SIZE_LSB         0
#else
#define MIU_FB_SIZE_MSB         (((DWORD)PANEL_HEIGHT/16+1)*16*PANEL_WIDTH*2)
#define MIU_FB_SIZE_LSB         (MIU_FB_SIZE_MSB/2)                       // in case of 444
#endif

#if PANEL_3D_PASSIVE_4M || (FRAME_BFF_SEL == FRAME_BFFLESS)
#define RESERVERD_AREA      0
#define HK_CODE_ADDR        (RESERVERD_AREA)
#define HK_CODE_SIZE        0
#define MIU_FB_ADDR_MSB_0   (HK_CODE_ADDR+HK_CODE_SIZE)
#define MIU_FB_ADDR_MSB_1   ((MIU_FB_ADDR_MSB_0+MIU_FB_SIZE_MSB)/2)
#define MIU_FB_ADDR_MSB_2   0
#define MIU_FB_ADDR_MSB_3   0
#define MIU_FB_ADDR_MSB_4   0
#define MIU_FB_ADDR_MSB_5   0
#define MIU_FB_ADDR_MSB_6   0
#define MIU_FB_ADDR_MSB_7   0

#define MIU_FB_ADDR_LSB_0   0   /* 422 mode, LSB unused */
#define MIU_FB_ADDR_LSB_1   0
#define MIU_FB_ADDR_LSB_2   0
#define MIU_FB_ADDR_LSB_3   0
#define MIU_FB_ADDR_LSB_4   0
#define MIU_FB_ADDR_LSB_5   0
#define MIU_FB_ADDR_LSB_6   0
#define MIU_FB_ADDR_LSB_7   0
#else
#if ENABLE_HK_CODE_ON_DRAM
/* HK XDATA, allocate in 64KB boundary */
#define RESERVERD_AREA          0x800000UL
#define HK_CODE_ADDR            (((0UL)+RESERVERD_AREA+0xFFFFUL)&0xFFF0000L) // allocate in 64KB boundary// // allocate in 64KB boundary
#define HK_CODE_SIZE            (1024ul*64)*16 // 64K bytes * 16 banks
#else
#define RESERVERD_AREA          0UL
#define HK_CODE_ADDR            0UL
#define HK_CODE_SIZE            0
#endif

#define MIU_FB_ADDR_MSB_0   (HK_CODE_ADDR+HK_CODE_SIZE)
#define MIU_FB_ADDR_MSB_1   (MIU_FB_ADDR_MSB_0+MIU_FB_SIZE_MSB)
#define MIU_FB_ADDR_MSB_2   (MIU_FB_ADDR_MSB_1+MIU_FB_SIZE_MSB)
#define MIU_FB_ADDR_MSB_3   (MIU_FB_ADDR_MSB_2+MIU_FB_SIZE_MSB)
#define MIU_FB_ADDR_MSB_4   (MIU_FB_ADDR_MSB_3+MIU_FB_SIZE_MSB)
#define MIU_FB_ADDR_MSB_5   (MIU_FB_ADDR_MSB_4+MIU_FB_SIZE_MSB)
#define MIU_FB_ADDR_MSB_6   (MIU_FB_ADDR_MSB_5+MIU_FB_SIZE_MSB)
#define MIU_FB_ADDR_MSB_7   (MIU_FB_ADDR_MSB_6+MIU_FB_SIZE_MSB)

#define MIU_FB_ADDR_LSB_0   (MIU_FB_ADDR_MSB_7+MIU_FB_SIZE_MSB)
#define MIU_FB_ADDR_LSB_1   (MIU_FB_ADDR_LSB_0+MIU_FB_SIZE_LSB)
#define MIU_FB_ADDR_LSB_2   (MIU_FB_ADDR_LSB_1+MIU_FB_SIZE_LSB)
#define MIU_FB_ADDR_LSB_3   (MIU_FB_ADDR_LSB_2+MIU_FB_SIZE_LSB)
#define MIU_FB_ADDR_LSB_4   (MIU_FB_ADDR_LSB_3+MIU_FB_SIZE_LSB)
#define MIU_FB_ADDR_LSB_5   (MIU_FB_ADDR_LSB_4+MIU_FB_SIZE_LSB)
#define MIU_FB_ADDR_LSB_6   (MIU_FB_ADDR_LSB_5+MIU_FB_SIZE_LSB)
#define MIU_FB_ADDR_LSB_7   (MIU_FB_ADDR_LSB_6+MIU_FB_SIZE_LSB)
#endif
#define MIU_FB_ADDR_END     (MIU_FB_ADDR_LSB_7+MIU_FB_SIZE_LSB)

/* OD Frame Buffer */
#define MIU_OD_ADDR             (MIU_FB_ADDR_END)
#if ENABLE_RTE
    #if (FRAME_BFF_SEL==FRAME_BFFLESS)
        #if OD_HSCAL_EN
        #define MIU_OD_SIZE             0x200000UL
        #define MIU_OD_LIMIT_SIZE       MIU_OD_SIZE
        #else
        #define MIU_OD_SIZE             0x400000UL
        #define MIU_OD_LIMIT_SIZE       (MIU_OD_SIZE/2)     // even, odd
        #endif
    #elif 0 //(OD_MODE == OD_MODE_444) && (FRAME_BFF_SEL==FRAME_BFFLESS)
        #define MIU_OD_SIZE             0x400000UL
        #define MIU_OD_LIMIT_SIZE       (MIU_OD_SIZE/2)
    #elif 1 //(OD_MODE == OD_MODE_565)
        #if 1
        #define MIU_OD_SIZE             MIU_FB_SIZE_MSB
        #define MIU_OD_LIMIT_SIZE       (MIU_OD_SIZE)
        #else
        #define MIU_OD_SIZE             0x800000UL          // (8M) 2560x1600
        #define MIU_OD_LIMIT_SIZE       (MIU_OD_SIZE/2)
        #endif
    #endif
#else
    #define MIU_OD_SIZE             0x000000UL
    #define MIU_OD_LIMIT_SIZE       0x000000UL
#endif

#define MIU_OD_ADDR_EVEN        (MIU_OD_ADDR)
#define MIU_OD_ADDR_ODD         (MIU_OD_ADDR_EVEN+MIU_OD_SIZE/2)

#define MIU_OD_EVEN_LIMIT_ADDR  (MIU_OD_LIMIT_SIZE+MIU_OD_ADDR_EVEN)    // 2M
#define MIU_OD_ODD_LIMIT_ADDR   (MIU_OD_LIMIT_SIZE+MIU_OD_ADDR_ODD)     // 6M

/* OD Multitable, allocate in 64KB boundary */
#define MIU_OD_MTB_ADDR         ((MIU_OD_ADDR+MIU_OD_SIZE+0x10000UL)&0xFFFF0000)
#ifdef _MULTI_OD_
#define MIU_OD_MTB_SIZE         0x10000UL
#else
#define MIU_OD_MTB_SIZE         0UL
#endif

/* HK XDATA, allocate in 64KB boundary */
#define HK_XDATA_ADDR           (MIU_OD_MTB_ADDR+MIU_OD_MTB_SIZE)
#if ENABLE_HK_XDATA_ON_DRAM
#define HK_XDATA_SIZE           0x10000UL                               // 64K bytes
#else
#define HK_XDATA_SIZE           0UL
#endif


/****************************************************************************/
//        DRAM parameters
/****************************************************************************/

#define DDR3_NANYA_NT5CB64M16        0x00
#define DDR3_H5TQ1G63BFR             0x01
#define SDR_2M                       0x10
#define SDR_4M                       0x11
#define DDR_NONE                     0xFF

#if PANEL_3D_PASSIVE_4M                                 // Passive 4M
#define DDR_SELECT                  SDR_4M
#elif ENABLE_RTE && (FRAME_BFF_SEL==FRAME_BFFLESS)
#if (MIU_OD_SIZE == 0x200000UL)
#define DDR_SELECT                  SDR_2M              // QFP OD 2M
#else
#define DDR_SELECT                  SDR_4M              // QFP OD 4M
#endif
#elif (FRAME_BFF_SEL==FRAME_BUFFER)
#define DDR_SELECT                  DDR3_H5TQ1G63BFR    // BGA DDR3
#else
#define DDR_SELECT                  DDR_NONE
#endif

#if (DDR_SELECT == SDR_2M) || (DDR_SELECT == SDR_4M)
#define tRC             60      //unit:ns
#define tRAS            40      //unit:ns
#define tRP             15      //unit:ns
#define tRCD            15      //unit:ns
#define tWR             10      //unit:ns
#define tRRD            15      //unit:ns
#define tRTP            15     //unit:ns
#define tMRD            2       //unit:CLK
#define tWTR            5     //unit:ns
#define tRTW            35
#define tRFC            70     //unit:ns
#define tWL             10       //unit:ns
#define tCCD            1
#define tCKE            4       //unit:ns
#define tREFI           2560    //unit:ns
#define MR0             0x0031
#define MR1             0x4004
#define MR2             0x8018
#define MR3             0xC000

#elif DDR_SELECT == DDR3_NANYA_NT5CB64M16
#include "DDR3_NANYA_NT5CB64M16.h"
#elif DDR_SELECT == DDR3_H5TQ1G63BFR
#include "DDR3_HYNIX_H5TQ1G63BFR.h"
#else
#define tRC             60      //unit:ns
#define tRAS            35      //unit:ns
#define tRP             15      //unit:ns
#define tRCD            15      //unit:ns
#define tWR             15      //unit:ns
#define tRRD            10      //unit:ns
#define tRTP            7.5     //unit:ns
#define tWTR            7.5     //unit:ns
#define tRTW            12
#define tRFC            110     //unit:ns
#define tWL             12      //unit:ns
#define tCCD            4
#define tCKE            4       //unit:ns
#define tREFI           3900    //unit:ns 0~85 degree:7800; 85~95 degree:3900
#define MR0             0x1C70
#define MR1             0x4004
#define MR2             0x8018
#define MR3             0xC000

#endif

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

#if (DDR_SELECT == SDR_2M) || (DDR_SELECT == SDR_4M)
#define MIU_ARBITOR_CLK     200ul  //MHZ
#define MEMORY_CLOCK_MHZ    MIU_ARBITOR_CLK // MHz //issue, decrease to 250
#define MPLL_FREQ_MHZ   429ul
#define DDRIP           3
#define IN_DIV          1
#define LOOP_DIV1       2
#define LOOP_DIV2       3
#define POST_DIV        8
#if (DDR_SELECT == SDR_2M)
#define RAM_TYPE        (_8COL|_2BK|_1PICS|_SDR)
#define MEM_CLK_PHASE_CTRL     0x03
#else
#define RAM_TYPE        (_8COL|_2BK|_2PICS|_SDR)
#define MEM_CLK_PHASE_CTRL     0x00
#endif
#define MEM_DQ_PHASE_CTRL_1    0x77
#define MEM_DQ_PHASE_CTRL_2    0x00
#define MEM_DQS_PHASE_CTRL_1   0x33
#define MEM_DQS_PHASE_CTRL_2   0x00
#else
#define MIU_ARBITOR_CLK     200ul  //MHZ
#define MEMORY_CLOCK_MHZ    (MIU_ARBITOR_CLK*4) // MHz //issue, decrease to 250
#define MPLL_FREQ_MHZ   429ul
#define DDRIP           3
#if( MEMORY_CLOCK_MHZ >= 800 )
#define IN_DIV          1
#define LOOP_DIV1       2
#define LOOP_DIV2       3
#define POST_DIV        2
#elif( MEMORY_CLOCK_MHZ >= 400 )
#define IN_DIV          1
#define LOOP_DIV1       2
#define LOOP_DIV2       3
#define POST_DIV        4
#else
#define IN_DIV          1
#define LOOP_DIV1       2
#define LOOP_DIV2       3
#define POST_DIV        8
#endif
#define RAM_TYPE        (_10COL|_8BK|_1PICS|_DDR3)
#define MEM_CLK_PHASE_CTRL     0x01
#define MEM_DQ_PHASE_CTRL_1    0x77
#define MEM_DQ_PHASE_CTRL_2    0x00
#define MEM_DQS_PHASE_CTRL_1   0x33
#define MEM_DQS_PHASE_CTRL_2   0x00


#endif


#define DDFSET          ((DWORD)(524288ul*(MPLL_FREQ_MHZ*2*LOOP_DIV1*LOOP_DIV2)/(IN_DIV*POST_DIV*MEMORY_CLOCK_MHZ) + 0.5))

#define ENABLE_DDR3_SSC        _DISABLE

#define F_MODULATION    20
#define PERCENTAGE      1
#define DDFSPAN         ((DWORD)(((float)MPLL_FREQ_MHZ*131072*1024)/(DDFSET*F_MODULATION)))
#define DDFSTEP         ((DWORD)(((float)PERCENTAGE*DDFSET+(float)50*DDFSPAN)/((float)100*DDFSPAN)))




///////////////////////////////////////////////////////////////////////////
// Subroutines
///////////////////////////////////////////////////////////////////////////

#if (FRAME_BFF_SEL == FRAME_BUFFER) || ENABLE_RTE
INTERFACE void msInitMemory(void);
INTERFACE Bool msMemoryBist(void);
#endif
#undef INTERFACE
#endif //_MSFB_H


#elif  CHIP_ID==CHIP_TSUM2

//add to here for Juno


#endif
