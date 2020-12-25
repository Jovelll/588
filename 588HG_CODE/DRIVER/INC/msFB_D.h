/******************************************************************************
 Copyright (c) 2003 MStar Semiconductor, Inc.
 All rights reserved.

 [Module Name]: msFb.h
 [Date]:        01-Dec-2003
 [Comment]:
   Memory access header file.
 [Reversion History]:
*******************************************************************************/
#if  CHIP_ID==CHIP_TSUMD
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


/* Xdata Allocation - total 5KB in CHIP_TSUMD (0x4000 - 0x53FF)
    -------------------
    |      Start      | 0x4000
    |                 |
    |      SWDB       | 0x4600
    |    DDCBuffer    | 0x4700
    |                 |
    |       END       | 0x53FF
    |       USB       | 0xF000
    -------------------
*/
#define XDATA_SIZE              0x1400
#define XDATA_ADDR_START        0x4000 // keep 0x4000.
#define XDATA_MHL_CBUS_START    0x4000 // MHL_CBUS must keep at 0x4000!!
#define XDATA_SWDB_ADDR_START   0x4100 // SWDB - allocate maximum 256 Bytes >=  sizeof(StuSWDBCtrl)
#define XDATA_DDC_ADDR_START    0x4200 // DDC max = 256B*5=0x500, 0X4C00-0x0500=0x4700, In current driver code, maximum 256 Bytes needed.
#define XDATA_ADDR_END          0x53FF
#define XDATA_USB_ADDR_START    0xF000
/* Memory Allocation
    -------------------
    |  R2  CODE       |
    |  DD             |
    |  DR             |
    |  FB             |
    |  OD             |
    |  HK DATA        |
    |  USB            |
    -------------------
*/
#define DRAM_SIZE_2MB       (0x200000UL)
#define DRAM_SIZE_6MB       (0x600000UL)
#define DRAM_SIZE_8MB       (0x800000UL)
#define DRAM_SIZE_16MB      (0x1000000UL)
/*****************************************/
/*     DDR parameter                     */
/*****************************************/
#define DRAM_SIZE           DRAM_SIZE_2MB
#if DRAM_SIZE == DRAM_SIZE_2MB
#define RAM_TYPE            (_8COL|_2BK|_1PICS|_SDR)
#endif
#define MIU_ARBITOR_CLK     225ul  //MHZ
#define MEMORY_CLOCK_MHZ    MIU_ARBITOR_CLK
#define MPLL_FREQ_MHZ       429ul
#define MPLL_FREQ_KHZ       429540ul
#define DDRIP               2
#define IN_DIV              1
#define LOOP_DIV1           2
#define LOOP_DIV2           3
#define DDFSET              ((DWORD)(524288ul*(MPLL_FREQ_MHZ*LOOP_DIV1*LOOP_DIV2)/(IN_DIV*2*MEMORY_CLOCK_MHZ) + 0.5))

#define ENABLE_DDR_SSC      (_ENABLE || SPI_SSC_EN)
#define F_MODULATION        20  // unit: KHZ
#define PERCENTAGE          1   // 1%
#define DDFSPAN             ((DWORD)(((float)MPLL_FREQ_MHZ*131072*1024)/(DDFSET*F_MODULATION)))
#define DDFSTEP             ((DWORD)(((float)PERCENTAGE*DDFSET+(float)50*DDFSPAN)/((float)100*DDFSPAN)))

/*****************************************/
/*     Memory Allocation                 */
/*****************************************/
#if ENABLE_VC_8_5
    #if (PANEL_WIDTH == 1920 && PANEL_HEIGHT == 1200) // due to 1080P50 memory size is not enough in 1920x1080 60 panel, use this size for temp solution
    #define MIU_FB_SIZE             (0x62E080UL)// 1920*1200*3*5/8(compression ratio)*1.5
    #else // (PANEL_WIDTH == 1920 && PANEL_HEIGHT == 1080)
    #define MIU_FB_SIZE             (0x58FD40UL)// 1920*1080*3*5/8(compression ratio)*1.5
    #endif
#else // VC_8_6
    #if (PANEL_WIDTH == 1920 && PANEL_HEIGHT == 1200)
    #define MIU_FB_SIZE             (0x76A700UL)// 1920*1200*3*6/8(compression ratio)*1.5
    #else // (PANEL_WIDTH == 1920 && PANEL_HEIGHT == 1080)
    #define MIU_FB_SIZE             (0x6AC980UL)// 1920*1080*3*6/8(compression ratio)*1.5
    #endif
#endif

#if (ENABLE_TNR)
//#define MIU_FB_SIZE             DRAM_SIZE_2MB
#define MOTION_BIT_WIDTH    2
#define MIU_MOTION_SIZE         (0xBDD80UL)//1920*1080/8*2*1.5
//#define MIU_MOTION_SIZE         (0xD2F00UL)//1920*1200/8*2*1.5
#else
#define MIU_MOTION_SIZE         (0UL)
#endif

#define MCU_ON_DRAM_START_ADDR  (0UL)
#if (ENABLE_R2_CODE_ON_DRAM) && (ENABLE_R2_2DTO3D)
#define MCU_CODE_SIZE            R2_CODE_2DTO3D_SIZE
#else
#define MCU_CODE_SIZE           (0UL)
#endif

#if ENABLE_USB_INPUT
#define MIU_USB_SIZE            (0x10000UL)
#else
#define MIU_USB_SIZE            (0UL)
#endif

#if ENABLE_RTE
    #if (PANEL_WIDTH == 1920 && PANEL_HEIGHT == 1200)
    #define MIU_OD_SIZE              (0x13C680UL)// 1920*1200*3*3/8/2(OD mode:333+H scaling down)
    #else // (PANEL_WIDTH == 1920 && PANEL_HEIGHT == 1080)
    #define MIU_OD_SIZE              (0x11CC40UL)// 1920*1080*3*3/8/2(OD mode:333+H scaling down)
    #endif
#else
#define MIU_OD_SIZE              (0UL)
#endif

#if (ENABLE_R2_2DTO3D)
#define MIU_DD_BUFFER_SIZE       (0x012000UL) //(PANEL_WIDTH/BLOCK_WIDTH)*(PANEL_HEIGHT/BLOCK_HEIGHT) * 32 bytes
#define MIU_DRBLK_BUFFER_SIZE    (0x001000UL) //(PANEL_WIDTH/BLOCK_WIDTH)*(PANEL_HEIGHT/BLOCK_HEIGHT) * 1 bytes
#define MIU_DRPIX_BUFFER_SIZE    (0x001000UL) //(PANEL_WIDTH/BLOCK_WIDTH)*(PANEL_HEIGHT/BLOCK_HEIGHT) * 1 bytes
#else
#define MIU_DD_BUFFER_SIZE       (0UL)
#define MIU_DRBLK_BUFFER_SIZE    (0UL)
#define MIU_DRPIX_BUFFER_SIZE    (0UL)
#endif

#define MIU_DD_ADDR_START        (MCU_ON_DRAM_START_ADDR+MCU_CODE_SIZE)//(0x00040000UL)//(MCU_ON_DRAM_START_ADDR+MCU_CODE_SIZE)
#define MIU_DRBLK_ADDR_START     (MIU_DD_ADDR_START+MIU_DD_BUFFER_SIZE)
#define MIU_DRPIX_ADDR_START     (MIU_DRBLK_ADDR_START+MIU_DRBLK_BUFFER_SIZE)
#define MIU_AUDIO_ADDR_START     (MIU_DRPIX_ADDR_START+MIU_DRPIX_BUFFER_SIZE)
#define MIU_FB_ADDR_START        (MIU_AUDIO_ADDR_START)
#define MIU_FB_ADDR_END          (MIU_FB_ADDR_START+MIU_FB_SIZE)
#define MIU_MOTION_ADDR_START    (MIU_FB_ADDR_END)
#define MIU_MOTION_ADDR_END      (MIU_MOTION_ADDR_START+MIU_MOTION_SIZE)
/* OD Frame Buffer */
#define MIU_OD_ADDR_START        (MIU_MOTION_ADDR_END)
#define MIU_OD_ADDR_END          (MIU_OD_ADDR_START+MIU_OD_SIZE)
/* HK XDATA, allocate in 64KB boundary */
#define HK_XDATA_ADDR            (MIU_OD_ADDR_END+0x10000-MIU_OD_ADDR_END%0x10000)//align with 64k
#define HK_USB_XDATA_ADDR        (HK_XDATA_ADDR)
#define HK_USB_XDATA_ADDR_PORT2  (HK_USB_XDATA_ADDR+MIU_USB_SIZE)

#if (HK_USB_XDATA_ADDR_PORT2) > (DRAM_SIZE_8MB)
#message "please re-arrange FB, the usage of Dram is over 8MB"
#endif
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
    MIU_ID_MIU_DEBUG    = BIT2,
    MIU_ID_MIU_R2_CPU_I = BIT3,
    MIU_ID_MIU_OD_R     = BIT4,
    MIU_ID_MIU_OD_W     = BIT5,
    MIU_ID_MIU_R2_CPU_D = BIT7,
    MIU_ID_MIU_BDMA     = BIT8,
    MIU_ID_MIU_IPR_SC   = BIT9,
    MIU_ID_MIU_MRW_W    = BIT10,
    MIU_ID_MIU_MRW_R    = BIT11,
    MIU_ID_MIU_USB_0    = BIT12,
    MIU_ID_MIU_USB_1    = BIT13,
    MIU_ID_MIU_T3D_RNDR = BIT14,
    MIU_ID_MIU_T3D_DPTH = BIT15,

};

///////////////////////////////////////////////////////////////////////////
// Subroutines
///////////////////////////////////////////////////////////////////////////

INTERFACE void msInitMemory(void);
INTERFACE Bool msMemoryBist(void);
INTERFACE void msMiuProtectCtrl(BYTE ucGroup, BOOL bCtrl, WORD ucID, DWORD wAddrStart, DWORD wAddrEnd);
INTERFACE void msSetupDDR(void);
INTERFACE void msSetupDDRSSC(void);

#undef INTERFACE

#endif

#endif
