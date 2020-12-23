/******************************************************************************
 Copyright (c) 2003 MStar Semiconductor, Inc.
 All rights reserved.

 [Module Name]: msFB.h
 [Date]:        01-Dec-2003
 [Comment]:
   Memory access header file.
 [Reversion History]:
*******************************************************************************/

#if  CHIP_ID==CHIP_TSUMF

#ifndef     _MSFB_H
#define     _MSFB_H

#ifdef _MS_FB_C
#define INTERFACE
#else
#define INTERFACE   extern
#endif

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
#define XDATA_SWDB_ADDR_START   0x4100 // SWDB - allocate maximum 256 Bytes >=  sizeof(StuSWDBCtrl)
#define XDATA_DDC_ADDR_START    0x4200 // DDC max = 256B*5=0x500, 0X4C00-0x0500=0x4700, In current driver code, maximum 256 Bytes needed.
#define XDATA_DDC_TX_ADDR_START  XDATA_DDC_ADDR_START + D2B_FIFO_XdataSize
#define XDATA_ADDR_END          0x4BFF

///////////////////////////////////////////////////////////////////////////
// Subroutines
///////////////////////////////////////////////////////////////////////////

#undef INTERFACE

#endif

#endif

