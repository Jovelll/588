/******************************************************************************
 Copyright (c) 2005 MStar Semiconductor, Inc.
 All rights reserved.

 [Module Name]: Msflash.h
 [Date]:        25-Apr-2003
 [Comment]:
   MST Flash subroutines.
   1. Only for size 0x40000 BYTE = 2Mbit flash
   2. msFlash.c need include in bank 3 only
   3. Because msFlash.c include in bank3 , so address can use the LSB WORD
 [Reversion History]:
*******************************************************************************/

#include "board.h"

#ifdef _MSFLASH_C_
#define _MSFLASHDEC_
#else
#define _MSFLASHDEC_ extern
#endif

#ifndef     MSFLASH_H
#define     MSFLASH_H

#ifndef FLASH_READ_BYTE_BY_CODE_POINTER
#define FLASH_READ_BYTE_BY_CODE_POINTER     1
#endif

// TPV new request for that no matter user data in EEPROM or flash, HDCP key always located at 3F000
#define ENABLE_FLASH_UNPROTECT_3A_3D    1 

// /////////////////////////////////
// ENABLE_FLASH_UNPROTECT_3A_3D = 1
// HDCP key start at 3F000
// factory start at 3E000
// user data start at 3A000~ 3DFFF
////////////////////////////////////
// ENABLE_FLASH_UNPROTECT_3A_3D = 0
// HDCP key start at 3A000
// factory start at 3B000
// user data start at 3C000~ 3FFFF
////////////////////////////////////

#define UNPROTECT_MXIC2026_3A_3F     0x40
#define UNPROTECT_MXIC2026_3C_3F     0x60
#define UNPROTECT_MXIC2026_3A_3D    0x4C

#define UNPROTECT_WINBOND_X21_3A_3F     0xC0
#define UNPROTECT_WINBOND_X21_3C_3F     0xE0
#define UNPROTECT_WINBOND_X21_3A_3D    0xCC

#if ENABLE_FLASH_UNPROTECT_3A_3D
#define FLASH_MXIC2026_USER_AREA    UNPROTECT_MXIC2026_3A_3D
#define FLASH_WINBOND_X21_X41_USER_AREA    UNPROTECT_WINBOND_X21_3A_3D
#define FDATA_HDCPKEY_START_ADDR              0xF000 // 0x3F000
#define FDATA_FACTORY_ADDR          0xE000 // 0x3E000
#define FDATA_DYNAMIC_ADDR          0xA000 // 0x3A000
#else
#define FLASH_MXIC2026_USER_AREA    UNPROTECT_MXIC2026_3C_3F
#define FLASH_WINBOND_X21_X41_USER_AREA    UNPROTECT_WINBOND_X21_3C_3F
#define FDATA_HDCPKEY_START_ADDR              0xA000 // 0x3A000
#define FDATA_FACTORY_ADDR          0xB000 // 0x3B000
#define FDATA_DYNAMIC_ADDR          0xC000 // 0x3C000
#endif
#define FDATA_START_ADDR              0xA000 // 0x3A000

#define IDENT_NOT_FOUND 0xFFFF

typedef enum
{
    READ = 0x03,
    BYTE_WRITE = 0x02,
    H_READ = 0x0B,
    RDSR = 0x05,
    WRSR = 0x01,
    WREN = 0x06,
    WRDI = 0x04,
    EWSR = 0x50,
} FLASH_OP;

// 25VF020B
typedef enum
{
    SST_READ = 0x03,
    SST_H_READ = 0x0B,
    SST_ERASE_SECTOR = 0x20,
    SST_ERASE_BLOCK = 0x52,
    SST_ERASE_CHIP = 0x60,
    SST_BYTE_WRITE = 0x02,
    SST_AAI = 0xAF,
    SST_RDSR = 0x05,
    SST_EWSR = 0x50,
    SST_WRSR = 0x01,
    SST_WREN = 0x06,
    SST_WRDI = 0x04,
    SST_READ_ID = 0x9F // read out: 0xBF, 0x25, 0x8C
} FLASH_OP_SST;

typedef enum
{
    PMC_READ = 0x03, //
    PMC_H_READ = 0x0B, //
    PMC_ERASE_SECTOR = 0xD7,
    PMC_ERASE_BLOCK = 0xD8,
    PMC_ERASE_CHIP = 0xC7,
    PMC_BYTE_WRITE = 0x02,
    PMC_RDSR = 0x05, //
    PMC_WRSR = 0x01, //
    PMC_WREN = 0x06, //
    PMC_WRDI = 0x04, //
    PMC_READ_ID = 0x9F, //0xAB
    PMC_UNPROTECT = 0x26,
    PMC_PROTECT = 0x24
} FLASH_OP_PMC;
typedef enum
{
    MXIC_READ = 0x03,
    MXIC_H_READ = 0x0B,
    MXIC_ERASE_SECTOR = 0x20,
    MXIC_ERASE_BLOCK = 0xD8,
    MXIC_ERASE_CHIP = 0x60,
    MXIC_BYTE_WRITE = 0x02,
    //AAI=0xAF,
    MXIC_RDSR = 0x05,
    //EWSR=0x50,
    MXIC_WRSR = 0x01,
    MXIC_WREN = 0x06,
    MXIC_WRDI = 0x04,
    MXIC_READ_ID = 0x9F, // read output: 0xC2, 0x20, 0x12(2025 is same with 2026) 
    MXIC_RES = 0xAB // read output: 0x03(2026); 0x11(2025)
} FLASH_OP_MXIC;
typedef enum
{
    EON_READ = 0x03,
    EON_H_READ = 0x0B,
    EON_ERASE_SECTOR = 0x20,
    EON_ERASE_BLOCK = 0x52,
    EON_ERASE_CHIP = 0x60,
    EON_BYTE_WRITE = 0x02,
    //AAI=0xAF,
    EON_RDSR = 0x05,
    //EWSR=0x50,
    EON_WRSR = 0x01,
    EON_WREN = 0x06,
    EON_WRDI = 0x04,
    EON_READ_ID = 0x9F
} FLASH_OP_EON;
typedef enum
{
    AMIC_READ = 0x03,
    AMIC_H_READ = 0x0B,
    AMIC_ERASE_SECTOR = 0x20,
    AMIC_ERASE_BLOCK = 0xD8,
    AMIC_ERASE_CHIP = 0xC7,
    AMIC_BYTE_WRITE = 0x02,
    //AAI=0xAF,
    AMIC_RDSR = 0x05,
    //EWSR=0x50,
    AMIC_WRSR = 0x01,
    AMIC_WREN = 0x06,
    AMIC_WRDI = 0x04,
    AMIC_READ_ID = 0x9F
} FLASH_OP_AMIC;
typedef enum
{
    WINBOND_READ = 0x03,
    WINBOND_H_READ = 0x0B,
    WINBOND_ERASE_SECTOR = 0x20,
    WINBOND_ERASE_BLOCK = 0xD8,
    WINBOND_ERASE_CHIP = 0xC7,
    WINBOND_BYTE_WRITE = 0x02,
    //AAI=0xAF,
    WINBOND_RDSR = 0x05,
    //EWSR=0x50,
    WINBOND_WRSR = 0x01,
    WINBOND_WREN = 0x06,
    WINBOND_WRDI = 0x04,
    
    WINBOND_READ_ID = 0x9F,
    WINBOND_READ_UNIQUE_ID = 0x4B
} FLASH_OP_WINBOND;
typedef enum
{
    GIGADEVICE_READ = 0x03,
    GIGADEVICE_H_READ = 0x0B,
    GIGADEVICE_ERASE_SECTOR = 0x20,
    GIGADEVICE_ERASE_BLOCK = 0xD8,
    GIGADEVICE_ERASE_CHIP = 0xC7,
    GIGADEVICE_BYTE_WRITE = 0x02,
    GIGADEVICE_RDSR = 0x05,
    GIGADEVICE_WRSR = 0x01,
    GIGADEVICE_WREN = 0x06,
    GIGADEVICE_WRDI = 0x04,
    GIGADEVICE_READ_ID = 0x9F,
    GIGADEVICE_UNPROTECT = 0x4C,
    GIGADEVICE_PROTECT = 0x4D
} FLASH_OP_GIGADEVICE;

typedef enum
{
    FMSH_READ = 0x03,
    FMSH_H_READ = 0x0B,
    FMSH_ERASE_SECTOR = 0x20,
    FMSH_ERASE_BLOCK = 0xD8,
    FMSH_ERASE_CHIP = 0xC7,
    FMSH_BYTE_WRITE = 0x02,
    FMSH_RDSR = 0x05,
    FMSH_RDSR1 = 0x35,
    FMSH_WRSR = 0x01,
    FMSH_WREN = 0x06,
    FMSH_WRDI = 0x04,

    FMSH_INDIVIDUAL_SECTOR_UNLOCK = 0x39,
    FMSH_GLOBAL_SECTOR_LOCK = 0x7E,
    FMSH_READ_ID = 0x9F
} FLASH_OP_FMSH;

typedef enum
{
    BOYA_READ = 0x03,
    BOYA_H_READ = 0x0B,
    BOYA_ERASE_SECTOR = 0x20,
    BOYA_ERASE_BLOCK = 0xD8,
    BOYA_ERASE_CHIP = 0xC7,
    BOYA_BYTE_WRITE = 0x02,
    BOYA_RDSR = 0x05,
    BOYA_WRSR = 0x01,
    BOYA_WREN = 0x06,
    BOYA_WRDI = 0x04,

    BOYA_READ_ID = 0x9F
} FLASH_OP_BOYA;

typedef enum
{
    ZB_READ = 0x03,
    ZB_H_READ = 0x0B,
    ZB_ERASE_SECTOR = 0x20,
    ZB_ERASE_BLOCK = 0xD8,
    ZB_ERASE_CHIP = 0xC7,
    ZB_BYTE_WRITE = 0x02,
    ZB_RDSR = 0x05,
    ZB_WRSR = 0x01,
    ZB_WREN = 0x06,
    ZB_WRDI = 0x04,
    ZB_READ_ID = 0x9F
} FLASH_OP_ZB;

typedef enum
{
    MX_2025     = 0x11, // read by AB command // 2M bit without special unprotection, all command ID(RDID/RES/REMS) are same with MX2026E/KH2026E
    MX_4025     = 0x12, // read by AB command // 4M bit without special unprotecton, all command ID(RDID/RES/REMS) are same with MX4026E/KH4026E
    MX_2026     = 0x03, // read by AB command // 2M bit with special unprotection 
    MX_4026     = 0x04, // read by AB command // 4 M bit with special unprotection
    PM25_LV020  = 0x7D,
    PM25_LD020  = 0x22,
    PM25_LQ020  = 0x42,
    W25X05CL = 0x10, // 512K bit // 9F command ID15~0: 0x3010
    W25X10CL = 0x11, // 1M bit // 0x3011
    W25X20CL = 0x12, // 2M bit // 0x3012
    W25X10BV = W25X10CL, // 1M bit // 0x3011
    W25X20BV = W25X20CL, // 2M bit // 0x3012
    W25X40BV = 0x13, // 4M bit // 0x3013
    W25X21CL = 0x95, // 2M bit with special unprotection // 0x3012 by 9F command // 21CL: 0x95; 20CL: 0xD5 by 4B command
    W25X41CL = 0x97, // 4M bit with special unprotection // 0x3013 by 9F command // 41CL: 0x97; by 4B command // 120619 coding wait for x41 unique id
    GD25M21B = 0x12, // 2M bit, 9F command M7~M0: 0xC8, ID15~ID8: 0x41, ID7~0: 0x12
    GD25M41B = 0x13, // 4M bit, 9F command M7~M0: 0xC8, ID15~ID8: 0x41, ID7~0: 0x13
    FM25Q02  = 0x12, // 8M bit, 9F command M7~M0: 0xA1, ID15~ID8: 0x40, ID7~0: 0x12
    FM25Q04  = 0x13, // 8M bit, 9F command M7~M0: 0xA1, ID15~ID8: 0x40, ID7~0: 0x13
    BY25D20  = 0x12, // 8M bit, 9F command M7~M0: 0xA1, ID15~ID8: 0x40, ID7~0: 0x12
    BY25D40  = 0x13, // 8M bit, 9F command M7~M0: 0xA1, ID15~ID8: 0x40, ID7~0: 0x13

} _DEVICE_ID;

typedef enum
{
    FLASH_SST     = 0xBF,
    FLASH_PMC     = 0x9D,
    FLASH_MXIC    = 0xC2,
    FLASH_EON     = 0x1C,
    FLASH_WINBOND = 0xEF, // 90h
    FLASH_AMIC    = 0x37,
    FLASH_GIGADEVICE = 0xC8,
    FLASH_FMSH = 0xA1,
    FLASH_BOYA = 0x68,
	FLASH_ZB = 0x5E,
    FLASH_UNKNOW  = 0xFF
} _FLASH_ID;


typedef enum
{
    FBufType_HDCP_EDID          = 0xAA,
    FBufType_MonitorSetting     = 0x88,
    FBufType_MonitorSetting2    = 0x44,
    FBufType_TimingMode         = 0x22,
    FBufType_FactorySetting     = 0x11,
    FBufType_FullBuffer             = 0x00, // 091022 coding, if buffer type is 0, it should be free buffer.
    FBufType_EmptyBuffer        = 0xFF,
} FLASH_BUFFTER_TYPE;

//#define FLASH_SIZE                  0x40000
#define TOTAL_BANK_NUM              8//((g_FlashSize == FLASH_SIZE_4M)?(8):(4))// 4 // (TOTAL_BANK_NUM-1) : current code bank
#define FDATA_DYNAMIC_SECTOR_NUM    4
#define FDATA_SECTOR_SIZE           0x1000

#define FDATA_BUF_ADDR(Num)         (FDATA_DYNAMIC_ADDR+(Num<<12))//Flash Data Start

#define FLASH_IDENTDATA             0x55
#define FLASH_IDENTDATA_CLEAR       0x00
#define FLASH_CHECKSUM              0xAA
#define FLASH_EMPTYDATA             0xFF

#define HDCPKEY_SIZE                304

#define MONITORSET_START            (FLASH_MonitorSettingAddr+1)
#define MONITORSET_SIZE             MonitorSettingSize
#define MONITORSET_BUF_START        (FLASH_FreeBufferAddr+1)
#define MAX_MONITORSET_NUM          ((FDATA_SECTOR_SIZE-1)/MonitorSettingSize-1)

#define MONITORSET2_START           (FLASH_MonitorSetting2Addr+1)
#define MONITORSET2_SIZE            MonitorSetting2Size
#define MONITORSET2_BUF_START       (FLASH_FreeBufferAddr+1)
#define MAX_MONITORSET2_NUM         ((FDATA_SECTOR_SIZE-1)/MonitorSetting2Size-1)

#define MODESET_START               (FLASH_TimingModeAddr+1)
#define MODESET_SIZE                ModeSettingSize
#define MODESET_BUF_START           (FLASH_FreeBufferAddr+1)
#define MAX_MODESET_NUM             ((FDATA_SECTOR_SIZE-1)/ModeSettingSize-1)


typedef enum
{
    FLASH_KEY_FACTORY,
    FLASH_KEY_PANEL,
    FLASH_KEY_GAMMA,
    FLASH_KEY_HDCP,
    FLASH_KEY_DDCA,
    FLASH_KEY_DDCD,
    FLASH_KEY_DDCH,
    FLASH_KEY_DDCDP,
} FLASH_KEY_TYPE;

#if 1
#define BF3B_TYPR_ADDR              FLASH_FactorySettingAddr//0xB000 is buffer type
#define BF3B_STATUS_SIZE            0x100
#define BF3B_TYPR_RESERVE           0x10

#define FACTORY_SECTOR_SIZE         0x200
#define FACTORY_STATUS_SIZE         0x10
#define FACTORY_STATUS_ADDR         (BF3B_TYPR_ADDR          +BF3B_TYPR_RESERVE)
#define FACTORY_BUF_STATUS_ADDR     (FLASH_FreeBufferAddr    +BF3B_TYPR_RESERVE)
#define FACTORY_SIZE                FactorySettingSize
#define FACTORY_START               (BF3B_TYPR_ADDR          +BF3B_STATUS_SIZE)
#define FACTORY_BUF_START           (FLASH_FreeBufferAddr    +BF3B_STATUS_SIZE)

#define PANEL_SECTOR_SIZE           0x100
#define PANEL_STATUS_SIZE           0x10
#define PANEL_STATUS_ADDR           (FACTORY_STATUS_ADDR     +FACTORY_STATUS_SIZE)
#define PANEL_BUF_STATUS_ADDR       (FACTORY_BUF_STATUS_ADDR +FACTORY_STATUS_SIZE)
#define PANEL_SIZE                  PanelSettingSize
#define PANEL_START                 (FACTORY_START+FACTORY_SECTOR_SIZE)
#define PANEL_BUF_START             (FACTORY_BUF_START+FACTORY_SECTOR_SIZE)

#else
#define FACTORY_SECTOR_SIZE         (256-0x10)
#define FACTORY_STATUS_ADDR         (FLASH_FactorySettingAddr+1+FLASH_KEY_FACTORY)
#define FACTORY_BUF_STATUS_ADDR     (FLASH_FreeBufferAddr+1+FLASH_KEY_FACTORY)
#define FACTORY_SIZE                FactorySettingSize
#define FACTORY_START               (FLASH_FactorySettingAddr+0x10)//0x3B010
#define FACTORY_BUF_START           (FLASH_FreeBufferAddr+0x10)

#define PANEL_SECTOR_SIZE           256
#define PANEL_STATUS_ADDR           (FLASH_FactorySettingAddr+1+FLASH_KEY_PANEL)
#define PANEL_BUF_STATUS_ADDR       (FLASH_FreeBufferAddr+1+FLASH_KEY_PANEL)
#define PANEL_SIZE                  PanelSettingSize
#define PANEL_START                 (FACTORY_START+FACTORY_SECTOR_SIZE)//0x3B100
#define PANEL_BUF_START             (FACTORY_BUF_START+FACTORY_SECTOR_SIZE)
#endif
#define GAMMA_SECTOR_SIZE           1024
#define GAMMA_STATUS_ADDR           (FLASH_FactorySettingAddr+1+FLASH_KEY_GAMMA)
#define GAMMA_BUF_STATUS_ADDR       (FLASH_FreeBufferAddr+1+FLASH_KEY_GAMMA)
#define GAMMA_SIZE                  960
#define GAMMA_START                 (PANEL_START+PANEL_SECTOR_SIZE)//0x3B200
#define GAMMA_BUF_START             (PANEL_BUF_START+PANEL_SECTOR_SIZE)
#define RESERVE_SECTOR_SIZE         1024
#define RESERVE_START               (GAMMA_START+GAMMA_SECTOR_SIZE)//0x3B600
#define RESERVE_BUF_START           (GAMMA_BUF_START+GAMMA_SECTOR_SIZE)
#if 1   //111011 Rick modified flash address allocation to meet spec - A020
//////////////////////////////////////////////////////////////////////////////
//3A000 ======================================================================
//////////////////////////////////////////////////////////////////////////////
#define BF3A_TYPR_ADDR              FLASH_HDCP_EDIDAddr//0xA000 is buffer type
#define BF3A_STATUS_SIZE            0x100
#define BF3A_TYPR_RESERVE           0x10
#define HDCPKEY_CRC_H_ADDR          (BF3A_TYPR_ADDR+1)
#define HDCPKEY_CRC_L_ADDR          (BF3A_TYPR_ADDR+2)


#define HDCPKEY_SECTOR_SIZE         0x600//1536
#define HDCPKEY_STATUS_SIZE         0x10
#define HDCPKEY_STATUS_ADDR         (BF3A_TYPR_ADDR          +BF3A_TYPR_RESERVE)
#define HDCPKEY_BUF_STATUS_ADDR     (FLASH_FreeBufferAddr    +BF3A_TYPR_RESERVE)
#define HDCPKEYSET_START            (BF3A_TYPR_ADDR          +BF3A_STATUS_SIZE)
#define HDCPKEYSET_BUF_START        (FLASH_FreeBufferAddr    +BF3A_STATUS_SIZE)

#define DDCAKEY_SECTOR_SIZE         0x200//512
#define DDCAKEY_STATUS_SIZE         0x10
#define DDCAKEY_STATUS_ADDR         (HDCPKEY_STATUS_ADDR     +HDCPKEY_STATUS_SIZE)
#define DDCAKEY_BUF_STATUS_ADDR     (HDCPKEY_BUF_STATUS_ADDR +HDCPKEY_STATUS_SIZE)
#define DDCAKEY_SIZE                128
#define DDCAKEYSET_START            (HDCPKEYSET_START        +HDCPKEY_SECTOR_SIZE)
#define DDCAKEYSET_BUF_START        (HDCPKEYSET_BUF_START    +HDCPKEY_SECTOR_SIZE)

#define DDCDPKEY_SECTOR_SIZE        0x400
#define DDCDPKEY_STATUS_SIZE        0x10
#define DDCDPKEY_STATUS_ADDR        (DDCAKEY_STATUS_ADDR     +DDCAKEY_STATUS_SIZE)
#define DDCDPKEY_BUF_STATUS_ADDR    (DDCAKEY_BUF_STATUS_ADDR +DDCAKEY_STATUS_SIZE)
#define DDCDPKEY_SIZE               256
#define DDCDPKEYSET_START           (DDCAKEYSET_START        +DDCAKEY_SECTOR_SIZE)
#define DDCDPKEYSET_BUF_START       (DDCAKEYSET_BUF_START    +DDCAKEY_SECTOR_SIZE)

#define DDCDKEY_SECTOR_SIZE         0x300
#define DDCDKEY_STATUS_SIZE         0x10
#define DDCDKEY_STATUS_ADDR         (DDCDPKEY_STATUS_ADDR    +DDCDPKEY_STATUS_SIZE)
#define DDCDKEY_BUF_STATUS_ADDR     (DDCDPKEY_BUF_STATUS_ADDR+DDCDPKEY_STATUS_SIZE)
#define DDCDKEY_SIZE                128
#define DDCDKEYSET_START            (DDCDPKEYSET_START       +DDCDPKEY_SECTOR_SIZE)
#define DDCDKEYSET_BUF_START        (DDCDPKEYSET_BUF_START   +DDCDPKEY_SECTOR_SIZE)
#else
#define HDCPKEY_SIZE                304
#define HDCPKEYSET_START            0xA000


#define DDCAKEY_SECTOR_SIZE         256
#define DDCAKEY_STATUS_ADDR         (FLASH_FactorySettingAddr+1+FLASH_KEY_DDCA)
#define DDCAKEY_BUF_STATUS_ADDR     (FLASH_FreeBufferAddr+1+FLASH_KEY_DDCA)
#define DDCAKEY_SIZE                128
#define DDCAKEYSET_START            (RESERVE_START+RESERVE_SECTOR_SIZE)//0x3BA00
#define DDCAKEYSET_BUF_START        (RESERVE_BUF_START+RESERVE_SECTOR_SIZE)

#define DDCDKEY_SECTOR_SIZE         256
#define DDCDKEY_STATUS_ADDR         (FLASH_FactorySettingAddr+1+FLASH_KEY_DDCD)
#define DDCDKEY_BUF_STATUS_ADDR     (FLASH_FreeBufferAddr+1+FLASH_KEY_DDCD)
#define DDCDKEY_SIZE                128//256
#define DDCDKEYSET_START            (DDCAKEYSET_START+DDCAKEY_SECTOR_SIZE)//0x3BB00
#define DDCDKEYSET_BUF_START        (DDCAKEYSET_BUF_START+DDCAKEY_SECTOR_SIZE)

#define DDCHKEY_SECTOR_SIZE         256
#define DDCHKEY_STATUS_ADDR         (FLASH_FactorySettingAddr+1+FLASH_KEY_DDCH)
#define DDCHKEY_BUF_STATUS_ADDR     (FLASH_FreeBufferAddr+1+FLASH_KEY_DDCH)
#define DDCHKEY_SIZE                256
#define DDCHKEYSET_START            (DDCDKEYSET_START+DDCDKEY_SECTOR_SIZE)//0x3BC00
#define DDCHKEYSET_BUF_START        (DDCDKEYSET_BUF_START+DDCDKEY_SECTOR_SIZE)

#define DDCDPKEY_SECTOR_SIZE        256
#define DDCDPKEY_STATUS_ADDR        (FLASH_FactorySettingAddr+1+FLASH_KEY_DDCDP)
#define DDCDPKEY_BUF_STATUS_ADDR    (FLASH_FreeBufferAddr+1+FLASH_KEY_DDCDP)
#define DDCDPKEY_SIZE               256
#define DDCDPKEYSET_START           (DDCHKEYSET_START+DDCHKEY_SECTOR_SIZE)//0x3BD00
#define DDCDPKEYSET_BUF_START       (DDCHKEYSET_BUF_START+DDCHKEY_SECTOR_SIZE)
#endif


typedef enum
{
    FLASH_KEY_EMPTY     = 0xFF,
    FLASH_KEY_WRITE     = 0xAA,
    FLASH_KEY_VALID     = 0x22,
    FLASH_KEY_INVALID   = 0x00,
} FLASH_KEY_STATUS;

typedef enum
{
    FLASH_SIZE_2M = 2,
    FLASH_SIZE_4M = 4,        
}FLASH_SIZE;
//==================================================
_MSFLASHDEC_ BYTE xdata g_ucFlashID;
_MSFLASHDEC_ WORD xdata g_wKEYSET_START;
#if 1//USEFLASH
_MSFLASHDEC_ WORD xdata FLASH_MonitorSettingAddr;   //monitor Setting
_MSFLASHDEC_ WORD xdata FLASH_MonitorSetting2Addr;  //monitor Setting
_MSFLASHDEC_ WORD xdata FLASH_TimingModeAddr;   //Timing Mode
_MSFLASHDEC_ WORD xdata FLASH_FactorySettingAddr;   //Factory Setting
_MSFLASHDEC_ WORD xdata FLASH_FreeBufferAddr;   //Empty buffer
_MSFLASHDEC_ WORD xdata FLASH_HDCP_EDIDAddr;
#endif
//==================================================
///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Check current MsFlash.c code version
// <Returns> : MsFlash.c version number
///////////////////////////////////////////////////////////////////////////////
_MSFLASHDEC_ WORD FlashVersionCheck( void );
///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Read flash Manufacturer ID include PMC, MXIC, EON, and SST.
//                Return FLASH_UNKNOW if not found
// <Returns> : Manufacturer ID.
///////////////////////////////////////////////////////////////////////////////
_MSFLASHDEC_ BYTE ReadFlashID( void );
_MSFLASHDEC_ void drvFlashWaitSPINotBusy(void);
_MSFLASHDEC_ Bool FlashTypeIs_ZB( void );

#if 1//USEFLASH //------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Flash erase function
// <Parameter>:  -  <Flow>  -  <Description>
//-----------------------------------------------------------------------------
//  bDoWP   -   In  -   Doing FlashDisableWP at start and FlashEnableWP at end
//  wAddr   -   In  -   Erase 4K BYTE sector address
///////////////////////////////////////////////////////////////////////////////
_MSFLASHDEC_ void FlashSectorErase( Bool bDoWP, WORD wAddr );
///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Write one byte to flash
// <Parameter>: -  <Flow>   -   <Description>
//-----------------------------------------------------------------------------
//  bDoWP   -   In  -   Doing FlashDisableWP at start and FlashEnableWP at end
//  wAddr   -   In  -   Flash Address to be write, high byte address will be (TOTAL_BANK_NUM-1)
//                      wAddr < FDATA_FACTORY_ADDR will be limit, so address < 0x3B000 will be limit
//  value   -   In  -   The value write to flash
///////////////////////////////////////////////////////////////////////////////
_MSFLASHDEC_ void FlashWriteByte( Bool bDoWP, WORD wAddr, BYTE value );
_MSFLASHDEC_ void FlashWriteAnyByte( Bool bDoWP, DWORD dwAddr, BYTE value );
///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Read one byte from flash.
// <Returns> : Data value in the address
// <Parameter>:  -  <Flow>  -  <Description>
//-----------------------------------------------------------------------------
//  wAddr   -   In  -   Only in the same bank ot MsFlash.c
///////////////////////////////////////////////////////////////////////////////
_MSFLASHDEC_ void FlashHDCPWriteByte( WORD wAddr, BYTE value );

_MSFLASHDEC_ BYTE FlashReadByte( WORD wAddr );
_MSFLASHDEC_ BYTE FlashReadAnyByte( DWORD dwAddr );
//////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Write a table to flash
// <Parameter>:     -   <Flow>  -   <Description>
// ---------------------------------------------------------------------------
//  bDoWP   -   In  -   Doing FlashDisableWP at start and FlashEnableWP at end
//  wAddr   -   In  -   Flash Address to be write, high byte address will be (TOTAL_BANK_NUM-1)
//                      wAddr < FDATA_FACTORY_ADDR will be limit, so address < 0x3B000 will be limit
//  buffer  -   In  -   Data pointer of the table
//  count   -   In  -   Number of data
//////////////////////////////////////////////////////////////////////////////
_MSFLASHDEC_ void Flash_WriteTbl( Bool bDoWP, WORD dwAddr, BYTE *buffer, WORD count );
//////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Read a table from flash
// <Parameter>:     -   <Flow>  -   <Description>
// ---------------------------------------------------------------------------
//  wAddr   -   In  -   Flash Address
//  buffer  -   Out -   Data point of the table
//  count   -   In  -   Number of BYTE data
//////////////////////////////////////////////////////////////////////////////
_MSFLASHDEC_ void Flash_ReadTbl( WORD dwAddr, BYTE *buffer, WORD count );

//API start=================================================================================
//////////////////////////////////////////////////////////////////////////////
//  <API><Description>: Get each buffer address and assign to variable
//  FLASH_MonitorSettingAddr
//  FLASH_MonitorSetting2Addr
//  FLASH_TimingModeAddr
//  FLASH_FactorySettingAddr
//  FLASH_FreeBufferAddr
//////////////////////////////////////////////////////////////////////////////
_MSFLASHDEC_ void Flash_GetBufferAddr( void );
//////////////////////////////////////////////////////////////////////////////
// <API><Description>: After execute Flash_GetBufferAddr, makesure each address reasonable
//////////////////////////////////////////////////////////////////////////////
_MSFLASHDEC_ void Flash_AllocateBufferAddr( void );
//////////////////////////////////////////////////////////////////////////////
// <API><Description>: Check each buffer type correct or not,
//                Erase the sector when type wrong
//////////////////////////////////////////////////////////////////////////////
_MSFLASHDEC_ void Flash_CheckBufferType( void );
//////////////////////////////////////////////////////////////////////////////
// <API><Description>: Clear all timing mode save in flash
//////////////////////////////////////////////////////////////////////////////
_MSFLASHDEC_ void Flash_ClearModeSet( void );
//////////////////////////////////////////////////////////////////////////////
// <API><Description>:  Write Factory setting to flash,
//                  It will exchange buffer when not found empty space
//////////////////////////////////////////////////////////////////////////////
_MSFLASHDEC_ void Flash_WriteFactorySet( BYTE* pbuf, BYTE ucLen );
//////////////////////////////////////////////////////////////////////////////
// <API><Description>:  Read latest factory setting struct from flash.
// <Returns> : return TRUE when found factory setting
//////////////////////////////////////////////////////////////////////////////
_MSFLASHDEC_ Bool Flash_ReadFactorySet( BYTE* pbuf, BYTE ucLen );
//////////////////////////////////////////////////////////////////////////////
// <API><Description>:  Write HDCP ,DDCA or DDCD key to flash,
//                  Need set correct type of key.
//                  ucIndex from 0 to (key size / ucBufSize) or (key size / ucBufSize+1)
//                  The last write ucLen = (key size % ucBufSize)
//                  Will backup to free buffer if write to index = (MAX_HDCPKEYSET_NUM-1)
// <Parameter>:     -   <Flow>  -   <Description>
// ---------------------------------------------------------------------------
//  ucType          -    In     -   FLASH_KEY_HDCP, FLASH_KEY_DDCA or FLASH_KEY_DDCD
//  wIndex          -    In     -   From 0 ~ x
//  ucBufSize       -    In     -   Size of buf
//  *buf            -    In     -   point to the data of each set
//  wLen            -    In     -   Length of this time write buffer, ucBufSize>=ucLen
//////////////////////////////////////////////////////////////////////////////
_MSFLASHDEC_ void Flash_Write_Factory_KeySet( FLASH_KEY_TYPE ucType, WORD wIndex, WORD wBufSize, BYTE* buf, WORD wLen );

_MSFLASHDEC_ void Flash_Erase_FreeBuffer( FLASH_KEY_TYPE ucType, WORD wIndex ); // jeff add it in 2009 0907


//////////////////////////////////////////////////////////////////////////////
// <API><Description>:  Read HDCP ,DDCA or DDCD key each ucLen from flash,
//                  Need set correct ucType.
//                  ucIndex from 0 to (key size / ucBufSize) or (key size / ucBufSize+1)
//                  The last read ucLen = (key size % ucBufSize)
// <Returns> : return TRUE when found key ident and CheckSum
// <Parameter>:     -   <Flow>  -   <Description>
// ---------------------------------------------------------------------------
//  ucType          -    In     -   FLASH_KEY_HDCP, FLASH_KEY_DDCA or FLASH_KEY_DDCD
//  wIndex          -    In     -   From 0 ~ x
//  ucBufSize       -    In     -   Size of buf
//  *buf            -    In     -   point to the data of each set
//  wLen            -    In     -   Length of this time write buffer, ucBufSize>=ucLen
//////////////////////////////////////////////////////////////////////////////
_MSFLASHDEC_ Bool Flash_Read_Factory_KeySet( FLASH_KEY_TYPE ucType, Bool bReadVaild, WORD wIndex, WORD wBufSize, BYTE* buf, WORD wLen );
//////////////////////////////////////////////////////////////////////////////
// <API><Description>:  Write Monitor setting struct to flash,
//                  It will exchange buffer when buffer full
//////////////////////////////////////////////////////////////////////////////
_MSFLASHDEC_ void Flash_WriteMonitorSet( BYTE* buf, BYTE len );
//////////////////////////////////////////////////////////////////////////////
// <API><Description>:  Read latest Monitor setting struct from flash.
//                  1.Got monitor setting index by check flash identity
//                  2.If not found read CheckSum to got correct one
//                  3.read the Monitor setting struct
// <Returns> : return TRUE when found MonitorSetting
//////////////////////////////////////////////////////////////////////////////
_MSFLASHDEC_ Bool Flash_ReadMonitorSet( BYTE* buf, BYTE len );
//////////////////////////////////////////////////////////////////////////////
// <API><Description>:  Flash_WriteMonitorSet2 isues to save MonitorSetting2
//                  It will exchange buffer when buffer full
//////////////////////////////////////////////////////////////////////////////
_MSFLASHDEC_ void Flash_WriteMonitorSet2( BYTE* buf, BYTE len );
//////////////////////////////////////////////////////////////////////////////
// <API><Description>:  Read MonitorSetting2 struct from flash.
//                  1.Got monitor setting index by check flash identity
// <Returns> : return TRUE when found MonitorSetting2
//////////////////////////////////////////////////////////////////////////////
_MSFLASHDEC_ Bool Flash_ReadMonitorSet2( BYTE* buf, BYTE len );
//////////////////////////////////////////////////////////////////////////////
// <API><Description>:  Write timing mode with index to flash
//                  1.Search the timing mode from timing buffer
//                  2.Clear the timing ident
//                  3.Find empty timing mode bufer
//                  4.Exchange buffer if FLASH_TimingModeAddr buffer full
//                  5.Write timing struct to empty buffer
// <Parameter>:     -   <Flow>  -   <Description>
// ---------------------------------------------------------------------------
//  index           -    In     -   timing index, UserMode need |_BIT7 in index
//  *buf            -    In     -   point to the address of timing struct
//////////////////////////////////////////////////////////////////////////////
_MSFLASHDEC_ void Flash_WriteModeSet( BYTE index, BYTE *buf );
//////////////////////////////////////////////////////////////////////////////
// <API><Description>:  Read timing mode with index from flash
//                  1.Search the timing mode from timing buffer by compare FLASH_IDENTDATA&TimingIndex
//                  2.No update & return when not found
//                  3.Read timing mode from buffer
//                  4.upper level function check timing mode FLASH_IDENTDATA to makesure
// <Returns> : return TRUE when found the timing mode index
// <Parameter>:     -   <Flow>  -   <Description>
// ---------------------------------------------------------------------------
//  index           -    In     -   timing index, UserMode need |_BIT7 in index
//  *buf            -    In     -   point to the address of timing mode struct
//////////////////////////////////////////////////////////////////////////////
_MSFLASHDEC_ Bool Flash_ReadModeSet( BYTE index, BYTE *buf );

_MSFLASHDEC_ void FlashDisableWP( WORD wAddr );
_MSFLASHDEC_ void FlashEnableWP( void );


#endif

#ifndef USEFLASH_REDUCE_STACK_WRITE_FDATA
#define USEFLASH_REDUCE_STACK_WRITE_FDATA   0
#endif

#if USEFLASH_REDUCE_STACK_WRITE_FDATA
_MSFLASHDEC_ BYTE xdata FlashFData_ucKeyIndex;
_MSFLASHDEC_ WORD xdata FlashFData_wKEYSET_START;
_MSFLASHDEC_ WORD xdata FlashFData_wKEYSET_STATUS_ADDR;
_MSFLASHDEC_ Bool FlashFData_GetWriteKeyAddress(FLASH_KEY_TYPE ucType, Bool bWrite);
_MSFLASHDEC_ Bool FlashFData_BackupToFreeBuffer(FLASH_KEY_TYPE ucKeyType);
_MSFLASHDEC_ Bool FlashFData_BackupFromFreeBuffer(FLASH_KEY_TYPE ucKeyType);
_MSFLASHDEC_ void FlashFData_WriteBufferType(WORD addr);
_MSFLASHDEC_ void FlashFData_WriteKeyValidByte(void);
#endif



#endif//#ifndef     MSFLASH_H
