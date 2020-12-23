/******************************************************************************
 Copyright (c) 2003 MStar Semiconductor, Inc.
 All rights reserved.

 [Module Name]: msFb.h
 [Date]:        01-Dec-2003
 [Comment]:
   Memory access header file.
 [Reversion History]:
*******************************************************************************/
#if  CHIP_ID==CHIP_TSUMC || CHIP_ID==CHIP_TSUMK
#include "panel.h"

#ifndef     _MSFB_H
#define     _MSFB_H


#ifdef _MS_FB_C
#define INTERFACE
#else
#define INTERFACE   extern
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


/* Xdata Allocation - total 3KB in CHIP_TSUMC (0x4000 - 0x4BFF)
    -------------------
    |      Start      | 0x4000
    |                 |
    |      SWDB       | 0x4600
    |    DDCBuffer    | 0x4700
    |                 |
    |       END       | 0x4BFF
    -------------------
*/
#define XDATA_SIZE              0x0C00
#define XDATA_ADDR_START        0x4000
#define XDATA_MHL_CBUS_START    0x4000 // MHL_CBUS must keep at 0x4000!!
#define XDATA_SWDB_ADDR_START   0x4100 // SWDB - allocate maximum 256 Bytes >=  sizeof(StuSWDBCtrl)
#define XDATA_DDC_ADDR_START    0x4200 // DDC max = 256B*5=0x500, 0X4C00-0x0500=0x4700, In current driver code, maximum 256 Bytes needed.
#define XDATA_ADDR_END          0x4BFF

/* Memory Allocation
    -------------------
    |  R2  CODE       |
    |  DD             |
    |  DR             |
    |  FB             |
    |  OD             |
    |  HK DATA        |
    -------------------
*/
#define DRAM_SIZE_2MB       (0x200000UL)
#define DRAM_SIZE_8MB       (0x800000UL)
#define DRAM_SIZE_16MB      (0x1000000UL)
/*****************************************/
/*     DDR parameter                     */
/*****************************************/
#define DRAM_SIZE           DRAM_SIZE_2MB
#if DRAM_SIZE == DRAM_SIZE_2MB
#define RAM_TYPE            (_8COL|_2BK|_1PICS|_SDR)
#endif
#define MIU_ARBITOR_CLK     195ul  //MHZ
#define MEMORY_CLOCK_MHZ    MIU_ARBITOR_CLK
#define MPLL_FREQ_MHZ       429ul
#define MPLL_FREQ_KHZ       429540ul
#define DDRIP               3
#define DDRRP               0
#define IN_DIV              1
#define LOOP_DIV1           4
#define LOOP_DIV2           2
#define POST_DIV            2
#define DDFSET              ((DWORD)0x228F5C*200/MEMORY_CLOCK_MHZ)//(524288ul*(MPLL_FREQ_MHZ*2*LOOP_DIV1*LOOP_DIV2)/(IN_DIV*POST_DIV*MEMORY_CLOCK_MHZ) + 0.5))

#define ENABLE_DDR_SSC      (_ENABLE || SPI_SSC_EN)
#define F_MODULATION        20  // unit: KHZ
#define PERCENTAGE          1   // 1%
#define DDFSPAN             ((DWORD)(((float)MPLL_FREQ_MHZ*131072*1024)/(DDFSET*F_MODULATION)))
#define DDFSTEP             ((DWORD)(((float)PERCENTAGE*DDFSET+(float)50*DDFSPAN)/((float)100*DDFSPAN)))

/*****************************************/
/*     Memory Allocation                 */
/*****************************************/

#define MIU_FB_SIZE             (0UL)

#if (ENABLE_R2_CODE_ON_DRAM)
#define MCU_ON_DRAM_START_ADDR  (0UL)
#if ENABLE_R2_2DTO3D
#define MCU_CODE_SIZE            R2_CODE_2DTO3D_SIZE
#elif ENABLE_USB_INPUT
#define MCU_CODE_SIZE            R2_CODE_USB_SIZE
#else
#define MCU_CODE_SIZE            (0UL)
#endif
#else
#define MCU_ON_DRAM_START_ADDR   (0UL)
#define MCU_CODE_SIZE            (0UL)
#endif

#if (ENABLE_R2_2DTO3D)
#define MIU_DD_BUFFER_SIZE       (0x01FE00UL)//(0x200000ul) //((DWORD)PANEL_WIDTH/32+1)*((DWORD)PANEL_HEIGHT/32+1)*32*1  // ((DWORD)PANEL_WIDTH/32+1)*((DWORD)PANEL_HEIGHT/32+1)*32 *frame
#define MIU_DR_BUFFER_SIZE       (0x007F8UL) //((DWORD)PANEL_WIDTH/32+1)*((DWORD)PANEL_HEIGHT/32+1)*1*1  // ((DWORD)PANEL_WIDTH/32+1)*((DWORD)PANEL_HEIGHT/32+1)*1 *frame
#else
#define MIU_DD_BUFFER_SIZE       (0UL)//(0x200000ul) //((DWORD)PANEL_WIDTH/32+1)*((DWORD)PANEL_HEIGHT/32+1)*32*1  // ((DWORD)PANEL_WIDTH/32+1)*((DWORD)PANEL_HEIGHT/32+1)*32 *frame
#define MIU_DR_BUFFER_SIZE       (0UL) //((DWORD)PANEL_WIDTH/32+1)*((DWORD)PANEL_HEIGHT/32+1)*1*1  // ((DWORD)PANEL_WIDTH/32+1)*((DWORD)PANEL_HEIGHT/32+1)*1 *frame
#endif

#define MIU_DD_ADDR_START        (MCU_ON_DRAM_START_ADDR+MCU_CODE_SIZE)
#define MIU_DR_ADDR_START        (MIU_DD_ADDR_START+MIU_DD_BUFFER_SIZE)
#define MIU_AUDIO_ADDR_START     (MIU_DR_ADDR_START+MIU_DR_BUFFER_SIZE)
#define MIU_FB_ADDR_START        (MIU_AUDIO_ADDR_START)
#define MIU_FB_ADDR_END          (DRAM_SIZE - 1)//(MIU_FB_ADDR_START+MIU_FB_SIZE)

/* OD Frame Buffer */
#define MIU_OD_ADDR_START        (MIU_AUDIO_ADDR_START)
#define MIU_OD_SIZE              0x200000
#define MIU_OD_ADDR_END          (MIU_AUDIO_ADDR_START+MIU_OD_SIZE)

/* HK XDATA, allocate in 64KB boundary */
#define HK_XDATA_ADDR            (DRAM_SIZE - 0x10000UL) // last 64KB


enum
{
    MIU_PROTECT_0 = BIT0,
    MIU_PROTECT_1 = BIT1,
    MIU_PROTECT_2 = BIT2,
    MIU_PROTECT_3 = BIT3,
};

enum
{
    MIU_ID_MIU_CMD      = BIT0,
    MIU_ID_MIU_HK_CPU   = BIT1,
    MIU_ID_MIU_R2_CPU_I = BIT2,
    MIU_ID_MIU_R2_CPU_D = BIT3,
    MIU_ID_MIU_BDMA     = BIT4,
    MIU_ID_MIU_USB_MAC  = BIT5,
    MIU_ID_MIU_JPD_LL   = BIT6,
    MIU_ID_MIU_T3D_DD   = BIT7,
    MIU_ID_MIU_T3D_DR   = BIT8,
    MIU_ID_MIU_JPD_S    = BIT9,
    MIU_ID_MIU_MRW_W    = BIT10,
    MIU_ID_MIU_MRW_R    = BIT11,
    MIU_ID_MIU_OD_R     = BIT12,
    MIU_ID_MIU_OD_W     = BIT13,
    MIU_ID_MIU_USB_PHY  = BIT14,
    MIU_ID_MIU_MIU_TEST = BIT15,
};

///////////////////////////////////////////////////////////////////////////
// Subroutines
///////////////////////////////////////////////////////////////////////////

#if ENABLE_TNR || ENABLE_RTE
INTERFACE void msInitMemory(void);
INTERFACE Bool msMemoryBist(void);
#if CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK
INTERFACE void msMiuProtectCtrl(BYTE ucGroup, BOOL bCtrl, BYTE ucID, DWORD wAddrStart, DWORD wAddrEnd);
#endif
#endif


#if ENABLE_TNR || ENABLE_RTE || SPI_SSC_EN
INTERFACE void msSetupDDR(void);
INTERFACE void msSetupDDRSSC(void);
#endif

#undef INTERFACE

#endif

#endif
