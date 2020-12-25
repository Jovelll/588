/******************************************************************************
 Copyright (c) 2005 MStar Semiconductor, Inc.
 All rights reserved.

 [Module Name]: Msflash.c
 [Date]:        25-Apr-2003
 [Comment]:
   MST Flash subroutines.
 [Reversion History]:
*******************************************************************************/
#define _MSFLASH_C_

#include <intrins.h>
#include "types.h"
#include "board.h"
//#include "mode.h"
//#include "UserPrefDef.h"
#include "global.h"
#include "ms_reg.h"
#include "ms_rwreg.h"
#include "menudef.h"
#include "msOSD.h"
#include "debug.h"
#include "Common.h"
#include "misc.h"
#include "Mcu.h"
#include "msflash.h"
#include "gpio_def.h"
//#include "halRwreg.h"
#define _FLASH_CODE_VERSION_    0x200

#define FLASH_DEBUG    0
#if FLASH_DEBUG && ENABLE_DEBUG
#define FLASH_printData(str, value)   printData(str, value)
#define FLASH_printMsg(str)           printMsg(str)
#else
#define FLASH_printData(str, value)
#define FLASH_printMsg(str)
#endif

//#define CacheStatus()   (_bit3_(MEM_MSREAD_BYTE(REG_1018)))
//#define EnableCache()  (MEM_MSWRITE_BIT(REG_1018, BIT3, BIT3), MEM_MSWRITE_BIT(REG_2BA0, 0, BIT0))
//#define DisableCache()  (MEM_MSWRITE_BIT(REG_2BA0, BIT0, BIT0), MEM_MSWRITE_BIT(REG_1018, 0, BIT3))

Bool xdata bFlashWriteFactory = FALSE;
Bool xdata bFlashEraseBufferFinish = FALSE; // // jeff add it in 2009 0907
BYTE xdata g_ucFlashDeviceID = 0;
BYTE xdata g_FlashSize = FLASH_SIZE_2M;
///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Check current MsFlash.c code version
// <Returns> : MsFlash.c version number
///////////////////////////////////////////////////////////////////////////////
WORD FlashVersionCheck( void )
{
    BYTE ucTemp = 0;
    if( ucTemp )
    {
        Flash_GetBufferAddr();
        Flash_AllocateBufferAddr();
        Flash_CheckBufferType();
        Flash_ClearModeSet();
        Flash_WriteFactorySet( &ucTemp, 0 );
        Flash_ReadFactorySet( &ucTemp, 0 );
        Flash_WriteMonitorSet( &ucTemp, 0 );
        Flash_ReadMonitorSet( &ucTemp, 0 );
        Flash_WriteMonitorSet2( &ucTemp, 0 );
        Flash_ReadMonitorSet2( &ucTemp, 0 );
        Flash_WriteModeSet( 0, &ucTemp );
        Flash_ReadModeSet( 0, &ucTemp );
		FlashWriteAnyByte( 0, 0, 0);
		FlashReadAnyByte( 0 );		
    }
    return _FLASH_CODE_VERSION_;
}

void drvFlashWaitSPINotBusy(void)
{
    WORD u16Retry = 0xFFFF;
    while(!(SPI_STATUS&BIT0)&&u16Retry--)
        Delay4us();
    if (SPI_STATUS&BIT0)
        SPI_CLR_STATUS= 0x01; //clr int
    //printData("u8Retry=%d",u8Retry);
}

///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Return true when flash type is PMC .
///////////////////////////////////////////////////////////////////////////////
Bool FlashTypeIs_PMC( void )
{
    BYTE ucID = 0;

    // 1st
    SPI_SI_BUF[0] = PMC_READ_ID;
    
    // 2nd
    SPI_SI_BUF[1] = RDSR;

    SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
    SPI_CTRL1       = (EN_2ND_CMD|EN_AUTO_CHECK|RDSR_IN_2ND);
    SET_WBF_SIZE(1, 1, 0);
    SET_RBF_SIZE(3, 1, 0);

    SPI_TRIGGER  = 0x01; //start flash programming
    drvFlashWaitSPINotBusy();
    
    ucID = SPI_SO_BUF[1];

    if( ucID == FLASH_PMC )
    {
        if( SPI_SO_BUF[2] == PM25_LD020 )
            g_ucFlashDeviceID = PM25_LD020;
        else if( SPI_SO_BUF[2] == PM25_LV020 )
            g_ucFlashDeviceID = PM25_LV020;
        else if( SPI_SO_BUF[2] == PM25_LQ020 )
            g_ucFlashDeviceID = PM25_LQ020;
        return TRUE;
    }
    return FALSE;
}
///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Return true when flash type is SST.
///////////////////////////////////////////////////////////////////////////////
Bool FlashTypeIs_SST( void )
{
    BYTE ucID = 0;

    // 1st
    SPI_SI_BUF[0] = SST_READ_ID;
    
    // 2nd
    SPI_SI_BUF[1] = RDSR;

    SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
    SPI_CTRL1       = (EN_2ND_CMD|EN_AUTO_CHECK|RDSR_IN_2ND);
    SET_WBF_SIZE(1, 1, 0);
    SET_RBF_SIZE(3, 1, 0);

    SPI_TRIGGER  = 0x01; //start flash programming
    drvFlashWaitSPINotBusy();
    
    ucID = SPI_SO_BUF[0];

    if( ucID == FLASH_SST )
        return TRUE;
    return FALSE;
}
///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Return true when flash type is MXIC.
///////////////////////////////////////////////////////////////////////////////
Bool FlashTypeIs_MXIC( void )
{
    BYTE ucID = 0;

    // 1st
    SPI_SI_BUF[0] = MXIC_READ_ID;
    
    // 2nd
    SPI_SI_BUF[1] = RDSR;

    SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
    SPI_CTRL1       = (EN_2ND_CMD|EN_AUTO_CHECK|RDSR_IN_2ND);
    SET_WBF_SIZE(1, 1, 0);
    SET_RBF_SIZE(3, 1, 0);

    SPI_TRIGGER  = 0x01; //start flash programming
    drvFlashWaitSPINotBusy();
    
    ucID = SPI_SO_BUF[0];

    if( ucID == FLASH_MXIC )
        return TRUE;
    return FALSE;
}
///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Return true when flash type is EON.
///////////////////////////////////////////////////////////////////////////////
Bool FlashTypeIs_EON( void )
{
    BYTE ucID = 0;

    // 1st
    SPI_SI_BUF[0] = EON_READ_ID;
    
    // 2nd
    SPI_SI_BUF[1] = RDSR;

    SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
    SPI_CTRL1       = (EN_2ND_CMD|EN_AUTO_CHECK|RDSR_IN_2ND);
    SET_WBF_SIZE(1, 1, 0);
    SET_RBF_SIZE(3, 1, 0);

    SPI_TRIGGER  = 0x01; //start flash programming
    drvFlashWaitSPINotBusy();
    
    ucID = SPI_SO_BUF[0];

    if( ucID == FLASH_EON )
        return TRUE;
    return FALSE;
}
///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Return true when flash type is AMIC.
///////////////////////////////////////////////////////////////////////////////
Bool FlashTypeIs_AMIC( void )
{
    BYTE ucID = 0;

    // 1st
    SPI_SI_BUF[0] = AMIC_READ_ID;
    
    // 2nd
    SPI_SI_BUF[1] = RDSR;

    SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
    SPI_CTRL1       = (EN_2ND_CMD|EN_AUTO_CHECK|RDSR_IN_2ND);
    SET_WBF_SIZE(1, 1, 0);
    SET_RBF_SIZE(3, 1, 0);

    SPI_TRIGGER  = 0x01; //start flash programming
    drvFlashWaitSPINotBusy();
    
    ucID = SPI_SO_BUF[0];

    if( ucID == FLASH_AMIC )
        return TRUE;
    return FALSE;
}
///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Return true when flash type is WINBOND.
///////////////////////////////////////////////////////////////////////////////
Bool FlashTypeIs_WINBOND( void )
{
    BYTE ucID = 0;

    // 1st
    SPI_SI_BUF[0] = WINBOND_READ_ID;
    
    // 2nd
    SPI_SI_BUF[1] = RDSR;

    SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
    SPI_CTRL1       = (EN_2ND_CMD|EN_AUTO_CHECK|RDSR_IN_2ND);
    SET_WBF_SIZE(1, 1, 0);
    SET_RBF_SIZE(3, 1, 0);

    SPI_TRIGGER  = 0x01; //start flash programming
    drvFlashWaitSPINotBusy();
    
    ucID = SPI_SO_BUF[0];
    
    //FLASH_printData("W9F-2:%x",SPI_SO_BUF[2]);

    if( ucID == FLASH_WINBOND )
    {
        ucID = SPI_SO_BUF[2];
        if(ucID == W25X05CL)
            g_ucFlashDeviceID = W25X05CL;
        else if(ucID == W25X10CL)
            g_ucFlashDeviceID = W25X10CL;
        else if(ucID == W25X20CL)
            g_ucFlashDeviceID = W25X20CL;
        else if(ucID == W25X40BV)
            g_ucFlashDeviceID = W25X40BV;
            
        return TRUE;
    }
    return FALSE;
}

BYTE ReadFlashUniqueID_Winbond(void)
{
    // 1st
    SPI_SI_BUF[0] = WINBOND_READ_UNIQUE_ID; // with 4 dummy bytes
    
    // 2nd
    SPI_SI_BUF[5] = RDSR;

    SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
    SPI_CTRL1       = (EN_2ND_CMD|EN_AUTO_CHECK|RDSR_IN_2ND);
    SET_WBF_SIZE(5, 1, 0);
    SET_RBF_SIZE(1, 1, 0);

    SPI_TRIGGER  = 0x01; //start flash programming
    drvFlashWaitSPINotBusy();

    return SPI_SO_BUF[0];
}

BYTE ReadFlashDeviceID_MXIC( void )
{
    // 1st
    SPI_SI_BUF[0] = MXIC_RES;
    SPI_SI_BUF[1] = 0x00;
    SPI_SI_BUF[2] = 0x00;
    SPI_SI_BUF[3] = 0x00;
    // 2nd
    SPI_SI_BUF[4] = RDSR;

    SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
    SPI_CTRL1       = (EN_2ND_CMD|EN_AUTO_CHECK|RDSR_IN_2ND);
    SET_WBF_SIZE(4, 1, 0);
    SET_RBF_SIZE(1, 1, 0);
    SPI_TRIGGER  = 0x01; //start flash programming
    drvFlashWaitSPINotBusy();
    
    return SPI_SO_BUF[0];
}

///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Return true when flash type is GIGA DEVICE .
///////////////////////////////////////////////////////////////////////////////
Bool FlashTypeIs_GigaDevice( void )
{
    BYTE ucID = 0;

    // 1st
    SPI_SI_BUF[0] = GIGADEVICE_READ_ID;
    
    // 2nd
    SPI_SI_BUF[1] = RDSR;

    SPI_CTRL0       = SPI_CTRL_VALUE;
    SPI_CTRL1       = (EN_2ND_CMD|EN_AUTO_CHECK|RDSR_IN_2ND);
    SET_WBF_SIZE(1, 1, 0);
    SET_RBF_SIZE(3, 1, 0);

    SPI_TRIGGER  = 0x01;
    drvFlashWaitSPINotBusy();
    
    ucID = SPI_SO_BUF[0];
    
    if( ucID == FLASH_GIGADEVICE )
    {
        if( SPI_SO_BUF[2] == GD25M21B )
            g_ucFlashDeviceID = GD25M21B;
        else if( SPI_SO_BUF[2] == GD25M41B )
            g_ucFlashDeviceID = GD25M41B;
       
        return TRUE;
    }
    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Return true when flash type is FMSH.
///////////////////////////////////////////////////////////////////////////////
Bool FlashTypeIs_FMSH(void)
{
    BYTE ucID = 0;

    // 1st
    SPI_SI_BUF[0] = FMSH_READ_ID;
    
    // 2nd
    SPI_SI_BUF[1] = RDSR;

    SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
    SPI_CTRL1       = (EN_2ND_CMD|EN_AUTO_CHECK|RDSR_IN_2ND);
    SET_WBF_SIZE(1, 1, 0);
    SET_RBF_SIZE(3, 1, 0);

    SPI_TRIGGER  = 0x01; //start flash programming
    drvFlashWaitSPINotBusy();
    
    ucID = SPI_SO_BUF[0];

    if( ucID == FLASH_FMSH)
    {
       if( SPI_SO_BUF[2] == FM25Q04)
            g_ucFlashDeviceID = FM25Q04;
        else if( SPI_SO_BUF[2] == FM25Q02)
            g_ucFlashDeviceID = FM25Q02;

        return TRUE;
    }

    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Return true when flash type is BOYA.
///////////////////////////////////////////////////////////////////////////////
Bool FlashTypeIs_BOYA(void)
{
    BYTE ucID = 0;

    // 1st
    SPI_SI_BUF[0] = BOYA_READ_ID;
    
    // 2nd
    SPI_SI_BUF[1] = RDSR;

    SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
    SPI_CTRL1       = (EN_2ND_CMD|EN_AUTO_CHECK|RDSR_IN_2ND);
    SET_WBF_SIZE(1, 1, 0);
    SET_RBF_SIZE(3, 1, 0);

    SPI_TRIGGER  = 0x01; //start flash programming
    drvFlashWaitSPINotBusy();
    
    ucID = SPI_SO_BUF[0];

    if( ucID == FLASH_BOYA)
    {
        if( SPI_SO_BUF[2] == BY25D40)
            g_ucFlashDeviceID = BY25D40;
        else if( SPI_SO_BUF[2] == BY25D20)
            g_ucFlashDeviceID = BY25D20;

        return TRUE;
    }

    return FALSE;
}

Bool FlashTypeIs_ZB( void )
{
    BYTE ucID = 0;
	BYTE ucIDSize = 0;

    // 1st
    SPI_SI_BUF[0] = ZB_READ_ID;

    // 2nd
    SPI_SI_BUF[1] = RDSR;

    SPI_CTRL0       = SPI_CTRL_VALUE;
    SPI_CTRL1       = (EN_2ND_CMD|EN_AUTO_CHECK|RDSR_IN_2ND);
    SET_WBF_SIZE(1, 1, 0);
    SET_RBF_SIZE(3, 1, 0);

    SPI_TRIGGER  = 0x01;
    drvFlashWaitSPINotBusy();

    ucID = SPI_SO_BUF[0];
	ucIDSize = SPI_SO_BUF[2];
    if( ucID == FLASH_ZB )
    {
    return TRUE;
    }
    return FALSE;
}


#define FLASH_IS_MXIC(id)     (id == FLASH_MXIC)
#define FLASH_IS_WINBOND(id)    (id == FLASH_WINBOND)
#define FLASH_IS_GIGADEVICE(id)     (id == FLASH_GIGADEVICE)
#define FLASH_IS_FMSH(id)     (id == FLASH_FMSH)
#define FLASH_IS_BOYA(id)     (id == FLASH_BOYA)
#define MXIC_FLASH_4M()   (g_ucFlashDeviceID == MX_4026 || g_ucFlashDeviceID == MX_4025)
#define WINBOND_FLASH_4M()  (g_ucFlashDeviceID == W25X40BV || g_ucFlashDeviceID == W25X41CL)
#define GIGADEVICE_FLASH_4M()   (g_ucFlashDeviceID == GD25M41B)
#define FMSH_FLASH_4M()   (g_ucFlashDeviceID == FM25Q04)
#define BOYA_FLASH_4M()   (g_ucFlashDeviceID == BY25D40)

void CheckFlashSize(FlashID)
{
    if(FLASH_IS_MXIC(FlashID) && MXIC_FLASH_4M())
        g_FlashSize = FLASH_SIZE_4M;
    else if(FLASH_IS_WINBOND(FlashID) && WINBOND_FLASH_4M())
        g_FlashSize = FLASH_SIZE_4M;
    else if(FLASH_IS_GIGADEVICE(FlashID) && GIGADEVICE_FLASH_4M())
        g_FlashSize = FLASH_SIZE_4M;
    else if(FLASH_IS_FMSH(FlashID) && FMSH_FLASH_4M())
        g_FlashSize = FLASH_SIZE_4M;
    else if(FLASH_IS_BOYA(FlashID) && BOYA_FLASH_4M())
        g_FlashSize = FLASH_SIZE_4M;
    else
        g_FlashSize = FLASH_SIZE_2M;
}

///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Read flash Manufacturer ID include PMC, MXIC, EON, and SST.
//                Return FLASH_UNKNOW if not found
// <Returns> : Manufacturer ID.
///////////////////////////////////////////////////////////////////////////////
BYTE ReadFlashID( void )
{
    BYTE ucID;

    FlashVersionCheck();

    if( FlashTypeIs_PMC() )
        ucID = FLASH_PMC;
    else if( FlashTypeIs_MXIC() )
    {
        ucID = ReadFlashDeviceID_MXIC();
        if( ucID == MX_2026 )
            g_ucFlashDeviceID = MX_2026;
        else if( ucID == MX_4026 )
            g_ucFlashDeviceID = MX_4026;
        else if( ucID == MX_4025 )
            g_ucFlashDeviceID = MX_4025;
        else //if( ucID == MX_2025 )
            g_ucFlashDeviceID = MX_2025;
        ucID = FLASH_MXIC;
    }
    else if( FlashTypeIs_EON() )
        ucID = FLASH_EON;
    else if( FlashTypeIs_SST() )
        ucID = FLASH_SST;
    else if( FlashTypeIs_AMIC() )
        ucID = FLASH_AMIC;
    else if( FlashTypeIs_WINBOND() )
    {
        ucID = FLASH_WINBOND;
        if(g_ucFlashDeviceID == W25X20CL)
        {
            if(W25X21CL == ReadFlashUniqueID_Winbond())
                g_ucFlashDeviceID = W25X21CL;
        }
        else if(g_ucFlashDeviceID == W25X40BV)
        {
            if(W25X41CL == ReadFlashUniqueID_Winbond())
                g_ucFlashDeviceID = W25X41CL;
        }
    }
    else if(FlashTypeIs_GigaDevice())
        ucID = FLASH_GIGADEVICE;
    else if(FlashTypeIs_FMSH())
        ucID = FLASH_FMSH;
    else if(FlashTypeIs_BOYA())
        ucID = FLASH_BOYA;
	else if(FlashTypeIs_ZB())
		ucID = FLASH_ZB;
    else
        ucID = FLASH_UNKNOW;

    CheckFlashSize(ucID);

    return ucID;
}

#if 1//USEFLASH //------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Read flash status register.
//
// <Returns> : Flash status register.
///////////////////////////////////////////////////////////////////////////////
BYTE FlashReadSR( void )
{
    SPI_SI_BUF[0]   = RDSR;

    SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
    SPI_CTRL1       = (RDSR_IN_1ST|EN_AUTO_CHECK);
    SET_WBF_SIZE(1, 0, 0);
    SET_RBF_SIZE(1, 0, 0);
    SPI_TRIGGER     = 0x01; //start flash programming
    drvFlashWaitSPINotBusy();

    return SPI_SO_BUF[0];
}

///////////////////////////////////////////////////////////////////////////////
// <Description>: Read 2Byte flash status register.
//
// <Returns> : 2Byte Flash status register.
///////////////////////////////////////////////////////////////////////////////
WORD FlashRead2ByteSR( void )
{
    WORD u16TempSR = 0;
    
   if( g_ucFlashID == FLASH_FMSH && ((g_ucFlashDeviceID == FM25Q04)||(g_ucFlashDeviceID == FM25Q02)) )
    {
        SPI_SI_BUF[0]   = FMSH_RDSR1;
        SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
        SPI_CTRL1       = (RDSR_IN_1ST|EN_AUTO_CHECK);
        SET_WBF_SIZE(1, 0, 0);
        SET_RBF_SIZE(1, 0, 0);
        SPI_TRIGGER     = 0x01; //start flash programming
        drvFlashWaitSPINotBusy();

        u16TempSR=SPI_SO_BUF[0];

        SPI_SI_BUF[0]   = FMSH_RDSR;
        SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
        SPI_CTRL1       = (RDSR_IN_1ST|EN_AUTO_CHECK);
        SET_WBF_SIZE(1, 0, 0);
        SET_RBF_SIZE(1, 0, 0);
        SPI_TRIGGER     = 0x01; //start flash programming
        drvFlashWaitSPINotBusy();

        u16TempSR=(u16TempSR<<8)|SPI_SO_BUF[0];
    }
    
    return u16TempSR;
}


///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Flash Write enable
//Prior to any Byte-Program, AAI-Program, Sector-Erase, Block-Erase, or Chip-Erase operation, the Write-Enable (WREN)
//instruction must be executed.
//Wait WIP(SR[0]) clear then send WREN command
///////////////////////////////////////////////////////////////////////////////
//void FlashWriteEnable( void )
//{}

///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Write value to flash status register
// <Parameter>:  -  <Flow>  -  <Description>
//-----------------------------------------------------------------------------
//  value  -    In    -   which value write to flash status register
///////////////////////////////////////////////////////////////////////////////
void FlashWriteSR( BYTE value )
{
    if( g_ucFlashID == FLASH_SST )
    {
        // 1st
        SPI_SI_BUF[0]   = EWSR;
        // 2nd
        SPI_SI_BUF[1]   = WRSR;
        SPI_SI_BUF[2]   = value;
        // 3rd
        SPI_SI_BUF[3]   = RDSR;

        SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
        SPI_CTRL1       = (EN_2ND_CMD|EN_3RD_CMD|RDSR_IN_3RD|EN_AUTO_CHECK);
        SET_WBF_SIZE(1, 2, 1);
        SET_RBF_SIZE(0, 0, 1);
    }
    else if( g_ucFlashID == FLASH_AMIC )
    {
        BYTE SSR = FlashReadSR();
        if( SSR & 0x40 ) //_BIT6)
        {
            // 1st
            SPI_SI_BUF[0]   = WREN;
            // 2nd
            SPI_SI_BUF[1] = 0x90;
            SPI_SI_BUF[2] = 0x00;
            SPI_SI_BUF[3] = 0x28;
            SPI_SI_BUF[4] = 0x00;
            SPI_SI_BUF[5] = 0x3B;
            SPI_SI_BUF[6] = 0x00;
            SPI_SI_BUF[7] = 0x28;
            SPI_SI_BUF[8] = 0xFF;
            // 3rd
            SPI_SI_BUF[9]   = RDSR;

            SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
            SPI_CTRL1       = (EN_2ND_CMD|EN_3RD_CMD|RDSR_IN_3RD|EN_AUTO_CHECK);
            SET_WBF_SIZE(1, 8, 1);
            SET_RBF_SIZE(0, 0, 1);
            
            SPI_TRIGGER     = 0x01; //start flash programming
            drvFlashWaitSPINotBusy();
            
            // 1st
            SPI_SI_BUF[0]   = WRSR;
            SPI_SI_BUF[1]   = value;
            // 2nd
            SPI_SI_BUF[2]   = RDSR;
            
            SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
            SPI_CTRL1       = (EN_2ND_CMD|RDSR_IN_2ND|EN_AUTO_CHECK);
            SET_WBF_SIZE(2, 1, 0);
            SET_RBF_SIZE(0, 1, 0);
        }
        else
        {
            // 1st
            SPI_SI_BUF[0]   = WREN;
            // 2nd
            SPI_SI_BUF[1]   = WRSR;
            SPI_SI_BUF[2]   = value;
            // 3rd
            SPI_SI_BUF[3]   = RDSR;

            SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
            SPI_CTRL1       = (EN_2ND_CMD|EN_3RD_CMD|RDSR_IN_3RD|EN_AUTO_CHECK);
            SET_WBF_SIZE(1, 2, 1);
            SET_RBF_SIZE(0, 0, 1);
        }
    }
    else if( g_ucFlashID == FLASH_GIGADEVICE && (g_ucFlashDeviceID == GD25M21B || g_ucFlashDeviceID == GD25M41B))
    {
        BYTE SR15_8;

        SPI_SI_BUF[0]   = RDSR;

        SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
        SPI_CTRL1       = (RDSR_IN_1ST|EN_AUTO_CHECK);
        SET_WBF_SIZE(1, 0, 0);
        SET_RBF_SIZE(2, 0, 0);
        SPI_TRIGGER     = 0x01; //start flash programming
        drvFlashWaitSPINotBusy();

        // recover status register S15~S8
        SR15_8 = SPI_SO_BUF[1];

        // 1st
        SPI_SI_BUF[0]   = WREN;
        // 2nd
        SPI_SI_BUF[1]   = WRSR;
        SPI_SI_BUF[2]   = value;
        SPI_SI_BUF[3]   = SR15_8;
        // 3rd
        SPI_SI_BUF[4]   = RDSR;

        SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
        SPI_CTRL1       = (EN_2ND_CMD|EN_3RD_CMD|RDSR_IN_3RD|EN_AUTO_CHECK);
        SET_WBF_SIZE(1, 3, 1);
        SET_RBF_SIZE(0, 0, 1);
    }
    else
    {
        // 1st
        SPI_SI_BUF[0]   = WREN;
        // 2nd
        SPI_SI_BUF[1]   = WRSR;
        SPI_SI_BUF[2]   = value;
        // 3rd
        SPI_SI_BUF[3]   = RDSR;

        SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
        SPI_CTRL1       = (EN_2ND_CMD|EN_3RD_CMD|RDSR_IN_3RD|EN_AUTO_CHECK);
        SET_WBF_SIZE(1, 2, 1);
        SET_RBF_SIZE(0, 0, 1);
    }
    
    SPI_TRIGGER     = 0x01; //start flash programming
    drvFlashWaitSPINotBusy();
    
}

///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Write 2Byte value to flash status register
// <Parameter>:  -  <Flow>  -  <Description>
//-----------------------------------------------------------------------------
//  value  -    In    -   which value write to flash status register
///////////////////////////////////////////////////////////////////////////////
void FlashWrite2ByteSR(WORD u16SRValue)
{

   if( g_ucFlashID == FLASH_FMSH && ((g_ucFlashDeviceID == FM25Q04)||(g_ucFlashDeviceID == FM25Q02)) )
    {
        // 1st
        SPI_SI_BUF[0]   = WREN;
        // 2nd
        SPI_SI_BUF[1]   = WRSR;
        SPI_SI_BUF[2]   = u16SRValue;
        SPI_SI_BUF[3]   = u16SRValue>>8;
        // 3rd
        SPI_SI_BUF[4]   = RDSR;

        SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
        SPI_CTRL1       = (EN_2ND_CMD|EN_3RD_CMD|RDSR_IN_3RD|EN_AUTO_CHECK);
        SET_WBF_SIZE(1, 3, 1);
        SET_RBF_SIZE(0, 0, 1);
    }
    SPI_TRIGGER     = 0x01; //start flash programming
    drvFlashWaitSPINotBusy();
    
}

///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Only used in PMC type flash. To disable protect of specific sector.
//                     if wAddr < FDATA_FACTORY_ADDR will not unprotect
// <Parameter>:  <Description>
//  wAddr : Which sector address will be unportect
///////////////////////////////////////////////////////////////////////////////
void FlashPMCUnprotectEnable( WORD wAddr )
{
    if( g_ucFlashDeviceID == PM25_LD020 || g_ucFlashDeviceID == PM25_LQ020)
    {
        // 1st
        SPI_SI_BUF[0]   = WREN;
        // 2nd
        SPI_SI_BUF[1]   = PMC_UNPROTECT;
        SPI_SI_BUF[2]   = TOTAL_BANK_NUM - 1;
        SPI_SI_BUF[3]   = ( wAddr >> 8 ) & 0x0f0;
        SPI_SI_BUF[4]   = 0x00;
        // 3rd
        SPI_SI_BUF[5]   = RDSR;

        SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
        SPI_CTRL1       = (EN_2ND_CMD|EN_3RD_CMD|RDSR_IN_3RD|EN_AUTO_CHECK);
        SET_WBF_SIZE(1, 4, 1);
        SET_RBF_SIZE(0, 0, 1);

        /*
        SPI_SI_BUF[0] = WREN;
        SPI_CTRL = 0xF8;
        SPI_SI_BUF[0] = PMC_UNPROTECT;
        SPI_SI_BUF[1] = TOTAL_BANK_NUM - 1; //(wAddr>>16)&0x0ff;
        SPI_SI_BUF[2] = ( wAddr >> 8 ) & 0x0f0;
        SPI_SI_BUF[3] = 0x00; //wAddr&0x0ff;
        SPI_CTRL = 0xFB;
        */
    }
    else
    {
        hw_ClrFlashWP();
        Delay1ms( 1 );
    
        // 1st
        SPI_SI_BUF[0] = PMC_UNPROTECT;
        SPI_SI_BUF[1] = TOTAL_BANK_NUM - 1; //(wAddr>>16)&0x0ff;
        SPI_SI_BUF[2] = ( wAddr >> 8 ) & 0x0ff;
        SPI_SI_BUF[3] = wAddr & 0x0ff;
        // 2nd
        SPI_SI_BUF[4]   = RDSR;

        SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
        SPI_CTRL1       = (EN_2ND_CMD|RDSR_IN_2ND|EN_AUTO_CHECK);
        SET_WBF_SIZE(4, 1, 0);
        SET_RBF_SIZE(0, 1, 0);

        /*
        SPI_SI_BUF[0] = PMC_UNPROTECT;
        SPI_SI_BUF[1] = TOTAL_BANK_NUM - 1; //(wAddr>>16)&0x0ff;
        SPI_SI_BUF[2] = ( wAddr >> 8 ) & 0x0ff;
        SPI_SI_BUF[3] = wAddr & 0x0ff;
        SPI_CTRL = 0xBB;
        */
    }
    
    SPI_TRIGGER     = 0x01; //start flash programming
    drvFlashWaitSPINotBusy();
    
}
///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Only used in PMC type flash. To disable un-protect of specific sector.
///////////////////////////////////////////////////////////////////////////////
void FlashPMCUnprotectDisable( void )
{
    // 1st
    SPI_SI_BUF[0] = PMC_PROTECT;
    // 2nd
    SPI_SI_BUF[1] = RDSR;
    
    SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
    SPI_CTRL1       = (EN_2ND_CMD|RDSR_IN_2ND|EN_AUTO_CHECK);
    SET_WBF_SIZE(1, 1, 0);
    SET_RBF_SIZE(0, 1, 0);
    SPI_TRIGGER     = 0x01; //start flash programming
    drvFlashWaitSPINotBusy();
    /*
    SPI_SI_BUF[0] = PMC_PROTECT;
    SPI_CTRL = 0xF8;
    */
}

void FlashDisableStausProtect_MXIC2026( void )
{
    if( g_ucFlashID == FLASH_MXIC && (g_ucFlashDeviceID == MX_2026 || g_ucFlashDeviceID == MX_4026) )
    {
        hw_ClrFlashWP();
        Delay1ms( 1 );
        
        FlashWriteSR( 0x7F );
        if( bFlashWriteFactory )
            FlashWriteSR( UNPROTECT_MXIC2026_3A_3F );
        else
            FlashWriteSR( FLASH_MXIC2026_USER_AREA );
    }
}

void FlashWriteIndividualSectorUnlock_FMSH(DWORD address )
{
    BYTE retry=10;
    bit bcachestatus;

    if( address < FDATA_START_ADDR )
        return;

    bcachestatus = CACHE_STATUS();
    
    if( bcachestatus )
        CACHE_DISABLE();
	
    // 1st
    SPI_SI_BUF[0]   = WREN;
    // 2nd
    SPI_SI_BUF[1] = FMSH_INDIVIDUAL_SECTOR_UNLOCK;
    SPI_SI_BUF[2] = TOTAL_BANK_NUM - 1;
    SPI_SI_BUF[3] = ( address >> 8 ) & 0x0ff;
    SPI_SI_BUF[4] = address & 0x0ff;
    // 3rd
    SPI_SI_BUF[5]   = RDSR;

    SPI_CTRL0       = SPI_CTRL_VALUE;
    SPI_CTRL1       = (EN_2ND_CMD|EN_3RD_CMD|RDSR_IN_3RD|EN_AUTO_CHECK);
    SET_WBF_SIZE(1, 4, 1);
    SET_RBF_SIZE(0, 0, 1);
    SPI_TRIGGER     = 0x01; //start flash programming
    drvFlashWaitSPINotBusy();

    if( bcachestatus )
        CACHE_ENABLE();//EnableCache();
        
}

#define FlashEnableWP_PMC     FlashPMCUnprotectDisable

void FlashEnableWP_EON(BYTE SRValue)
{
    if(( SRValue & 0x94 ) != 0x94 )
    {
        //FLASH_printData("  EnableWP_EON_SR[%x] W_94",SSR);
        hw_ClrFlashWP();
        Delay1ms( 1 );
        FlashWriteSR( 0x94 );
    }
}

void FlashEnableWP_AMIC(BYTE SRValue)
{
    if(( SRValue & 0xFC ) != 0xE8 )
    {
        //FLASH_printData("  EnableWP_AMIC_SR[%x] E8_0",SSR);
        hw_ClrFlashWP();
        Delay1ms( 1 );
        FlashWriteSR( 0xE8 );
        //FLASH_printData("  EnableWP_AMIC_SR[%x] E8_1",SSR);
    }
}

void FlashEnableWP_ZB(BYTE SRValue)
{
    if(( SRValue & 0x80 ) != 0x80 )
    {
        //FLASH_printData("  EnableWP_ZB_SR[%x] W_80",SSR);
        hw_ClrFlashWP();
        Delay1ms( 1 );
        FlashWriteSR( 0x80 );
    }
}

void FlashEnableWP_WINBOND(BYTE SRValue)
{
    if(g_ucFlashDeviceID == W25X21CL || g_ucFlashDeviceID == W25X41CL)       
    {
        if((SRValue&FLASH_WINBOND_X21_X41_USER_AREA) != FLASH_WINBOND_X21_X41_USER_AREA)
        {
            hw_ClrFlashWP();
            Delay1ms( 1 );
            FlashWriteSR( FLASH_WINBOND_X21_X41_USER_AREA );
        }
    }
    else if(g_ucFlashDeviceID == W25X40BV)
    {
        if((SRValue&0xAC) != 0xAC)
        {
            hw_ClrFlashWP();
            Delay1ms( 1 );
            FlashWriteSR( 0xAC ); // protect lower 256K(0~0x3FFFF)
        }
    }
    else if(g_ucFlashDeviceID == W25X20BV)   
    {
        if((SRValue&0xA8) != 0xA8)      
        {
            hw_ClrFlashWP();
            Delay1ms( 1 );
            FlashWriteSR( 0xA8 ); // protect lower 128K(0~0x1FFFF)
        }
    }
    else if(g_ucFlashDeviceID == W25X10BV)   
    {
        if((SRValue&0xA4) != 0xA4)
        {
            hw_ClrFlashWP();
            Delay1ms( 1 );
            FlashWriteSR( 0xA4 ); // protect lower 64K(0~0x0FFFF)
        }
    }
    else
    {
        if((SRValue&0x8C) != 0x8C)
        {
            hw_ClrFlashWP();
            Delay1ms( 1 );
            FlashWriteSR( 0x8C ); // protect all
        }
    }
}

void FlashEnableWP_FMSH_FM25Q40_FM25Q20(WORD u16SRValue)
{
    // all block set to protect
    if(u16SRValue != 0x0090)
    {
        hw_ClrFlashWP();
        Delay1ms( 1 );
        FlashWrite2ByteSR( 0x0090 );
    }
}

void FlashEnableWP_BOYA_BY25D40_BY25D20(BYTE SRValue)
{
        if(( SRValue & 0xFC ) != 0xFC )
        {
            hw_ClrFlashWP();
            Delay1ms( 1 );
            FlashWriteSR( 0x7C );       //SRP status need individual set to 0 before write the protected bit(BP0~BP3)
            Delay1ms( 1 );
            FlashWriteSR( 0xFC );            
        }
}

#define FLASH_SR_PROTECT_WHOLE_AREA     ((g_FlashSize == FLASH_SIZE_2M)?(0x8C):(0x9C))
void FlashEnableWP_OTHERS(BYTE SRValue)
{
    if(( SRValue & FLASH_SR_PROTECT_WHOLE_AREA ) != FLASH_SR_PROTECT_WHOLE_AREA )
    {
        //FLASH_printData("  EnableWP__SR[%x] W_8C",SSR);
        hw_ClrFlashWP();
        Delay1ms( 1 );
        FlashWriteSR( FLASH_SR_PROTECT_WHOLE_AREA );
    }
}

void FlashEnableWP_GigaDevice_GD25M21B_GD25M41B(BYTE SRValue)
{
    // all block set to protect
    if((SRValue&0xFC) != 0xFC)
    {
        hw_ClrFlashWP();
        Delay1ms( 1 );
        FlashWriteSR( 0xFC );
    }

    // setor lock
    // 1st
    SPI_SI_BUF[0] = GIGADEVICE_PROTECT;
    // 2nd
    SPI_SI_BUF[1] = RDSR;
    
    SPI_CTRL0       = SPI_CTRL_VALUE;
    SPI_CTRL1       = (EN_2ND_CMD|RDSR_IN_2ND|EN_AUTO_CHECK);
    SET_WBF_SIZE(1, 1, 0);
    SET_RBF_SIZE(0, 1, 0);
    SPI_TRIGGER     = 0x01; //start flash programming
    drvFlashWaitSPINotBusy();
}

#define FlashDisableWP_PMC      FlashPMCUnprotectEnable
#define FlashDisableWP_MXIC_MX2026      FlashDisableStausProtect_MXIC2026
void FlashDisableWP_EON(BYTE SRValue)
{
    hw_ClrFlashWP();
    Delay1ms( 1 );
    if( bFlashWriteFactory )
    {
        //FLASH_printData("  _EON_SR[%x] W_00",SSR);
        FlashWriteSR( 0x00 );
    }
    else
    {
        if(( SRValue & 0x94 ) != 0x94 )
        {
            //FLASH_printData("  _EON_SR[%x] W_94",SSR);
            FlashWriteSR( 0x94 );
        }
    }
}
void FlashDisableWP_WINBOND_W25X21CL(void)
{
    // X21CL: // 0xC0: protect range 0~0x39FFF; 0xE0: 0~0x3BFFF
    // X41CL: // 0xC0: protect range 0~0x79FFF; 0xE0: 0~0x7BFFF

    hw_ClrFlashWP();
    Delay1ms( 1 );
    if( bFlashWriteFactory )
    {
        FlashWriteSR( UNPROTECT_WINBOND_X21_3A_3F );
    }
    else
    {
        FlashWriteSR( FLASH_WINBOND_X21_X41_USER_AREA );
    }
}

void FlashDisableWP_ZB(BYTE SRValue)
{
    hw_ClrFlashWP();
    Delay1ms( 1 );
    if( bFlashWriteFactory )
    {
        //FLASH_printData("  ZB_SR[%x] W_00",SSR);
        FlashWriteSR( 0x00 );
    }
    else
    {
        if(( SRValue & 0x80 ) != 0x80 )
        {
            //FLASH_printData("  ZB_SR[%x] W_80",SSR);
            FlashWriteSR( 0x80 );
        }
    }
}

void FlashDisableWP_OTHERS(BYTE SRValue)
{
    hw_ClrFlashWP();
    Delay1ms( 1 );
    if( SRValue != 0x00 )
    {
        FlashWriteSR( 0x00 );
    }
}

// use sector unlock to disable write protection
void FlashDisableWP_GigaDevice_GD25M21B_GD25M41B(WORD wAddr)
{
    // 1st
    SPI_SI_BUF[0]   = WREN;
    // 2nd
    SPI_SI_BUF[1]   = GIGADEVICE_UNPROTECT;
    SPI_SI_BUF[2]   = TOTAL_BANK_NUM - 1;
    SPI_SI_BUF[3]   = ( wAddr >> 8 ) & 0x0f0;
    SPI_SI_BUF[4]   = 0x00;
    // 3rd
    SPI_SI_BUF[5]   = RDSR;

    SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
    SPI_CTRL1       = (EN_2ND_CMD|EN_3RD_CMD|RDSR_IN_3RD|EN_AUTO_CHECK);
    SET_WBF_SIZE(1, 4, 1);
    SET_RBF_SIZE(0, 0, 1);

    SPI_TRIGGER     = 0x01; //start flash programming
    drvFlashWaitSPINotBusy();
}

//BOYA Flash will protect after write/erase command excuted, so disable SR value should writen before every wirte/erase command.
void FlashDisableWP_BOYA_BY25D40_BY25D20(WORD wAddr)
{
    BYTE u8SRValue=0;

    if( g_ucFlashID == FLASH_BOYA && (g_ucFlashDeviceID == BY25D20 || g_ucFlashDeviceID == BY25D40) )
    {
       hw_ClrFlashWP();
       Delay1ms( 1 );
       
       if (wAddr >= 0xF000)
           u8SRValue = 0xF8;    //Protect Area: 0x000000H-0x03EFFFH/0x07EFFFH
       else if (wAddr >= 0xE000)
           u8SRValue = 0xF0;    //Protect Area: 0x000000H-0x03DFFFH/0x07DFFFH
       else if (wAddr >= 0xC000)
           u8SRValue = 0xE0;    //Protect Area: 0x000000H-0x03BFFFH/0x07BFFFH
       else// if (wAddr >= 0xA000)
           u8SRValue = 0xC0;
       
       FlashWriteSR( 0x7C );       //SRP status need individual set to 0 before set the protected bit(BP0~BP3)
       Delay1ms( 1 );
       FlashWriteSR( u8SRValue );            
    }
}

void FlashDisableWP_FMSH_FM25Q40_FM25Q20(WORD wAddr)
{
    DWORD u32UnlockAddr=0;
    //Protect Area: 0x000000H-FDATA_START_ADDR-1
    hw_ClrFlashWP();
    Delay1ms( 1 );
    
    FlashWrite2ByteSR ( 0x2080 );
    for (u32UnlockAddr=wAddr; u32UnlockAddr<=0x0FFFF; u32UnlockAddr+=0x1000)
    {
        Delay1ms(1);
        FlashWriteIndividualSectorUnlock_FMSH(u32UnlockAddr);
    }
}

///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Enable flash write protect
///////////////////////////////////////////////////////////////////////////////
void FlashEnableWP( void )
{
    BYTE SSR;

    SSR = FlashReadSR();

    if( g_ucFlashID == FLASH_PMC )
    {
        FlashEnableWP_PMC();//FlashPMCUnprotectDisable();
    }
    else if( g_ucFlashID == FLASH_EON )
    {
        FlashEnableWP_EON(SSR);        
    }
    else if( g_ucFlashID == FLASH_AMIC )
    {
        FlashEnableWP_AMIC(SSR);
    }
    else if( g_ucFlashID == FLASH_MXIC && (g_ucFlashDeviceID == MX_2026 || g_ucFlashDeviceID == MX_4026) )
    {
        //SRAM type auto protect when write finish
    }
    else if( g_ucFlashID == FLASH_WINBOND )
    {
        FlashEnableWP_WINBOND(SSR);
    }
    else if( g_ucFlashID == FLASH_GIGADEVICE && (g_ucFlashDeviceID == GD25M21B || g_ucFlashDeviceID == GD25M41B) )
    {
        FlashEnableWP_GigaDevice_GD25M21B_GD25M41B(SSR);
    }
    else if( g_ucFlashID == FLASH_FMSH && ((g_ucFlashDeviceID == FM25Q04)||(g_ucFlashDeviceID == FM25Q02)) )
    {
        WORD u16SSR;
        
        u16SSR = FlashRead2ByteSR();
        FlashEnableWP_FMSH_FM25Q40_FM25Q20(u16SSR);
    }
    else if( g_ucFlashID == FLASH_BOYA && ((g_ucFlashDeviceID == BY25D20)||(g_ucFlashDeviceID == BY25D40)) )
    {
        FlashEnableWP_BOYA_BY25D40_BY25D20(SSR);
    }
	else if( g_ucFlashID == FLASH_ZB )
    {
        FlashEnableWP_ZB(SSR);
    }
    else
    {
        FlashEnableWP_OTHERS(SSR);
    }
    
    hw_SetFlashWP();

}

///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Disable flash write protect
///////////////////////////////////////////////////////////////////////////////
void FlashDisableWP( WORD wAddr )
{
    BYTE SSR;

    if( wAddr < FDATA_START_ADDR )
        return;

    SSR = FlashReadSR();

    if( g_ucFlashID == FLASH_PMC )
    {
        FlashDisableWP_PMC(wAddr);//FlashPMCUnprotectEnable( wAddr );
    }
    else if( g_ucFlashID == FLASH_EON )
    {
        FlashDisableWP_EON(SSR);
    }
    else if( g_ucFlashID == FLASH_AMIC )
    {
        ;
    }
    else if( g_ucFlashID == FLASH_MXIC && (g_ucFlashDeviceID == MX_2026 || g_ucFlashDeviceID == MX_4026) )
    {
        FlashDisableWP_MXIC_MX2026();//FlashDisableStausProtect_MXIC2026();
    }
    else if( g_ucFlashID == FLASH_WINBOND && (g_ucFlashDeviceID == W25X21CL || g_ucFlashDeviceID == W25X41CL)) // just 21CL needs to change SR
    {
        FlashDisableWP_WINBOND_W25X21CL();
    }
    else if( g_ucFlashID == FLASH_GIGADEVICE && (g_ucFlashDeviceID == GD25M21B || g_ucFlashDeviceID == GD25M41B) )
    {
        FlashDisableWP_GigaDevice_GD25M21B_GD25M41B(wAddr);
    }
    else if( g_ucFlashID == FLASH_FMSH  && ((g_ucFlashDeviceID == FM25Q04)||(g_ucFlashDeviceID == FM25Q02)) )
    {
        FlashDisableWP_FMSH_FM25Q40_FM25Q20(wAddr);
    }
    else if( g_ucFlashID == FLASH_BOYA && ((g_ucFlashDeviceID == BY25D20)||(g_ucFlashDeviceID == BY25D40)) )
    {
        FlashDisableWP_BOYA_BY25D40_BY25D20(wAddr);
    }
	else if( g_ucFlashID == FLASH_ZB )
    {
        FlashDisableWP_ZB(SSR);
    }
    else
    {
        FlashDisableWP_OTHERS(SSR);
    }

}

///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Flash erase function
// <Parameter>:  -  <Flow>  -  <Description>
//-----------------------------------------------------------------------------
//  bDoWP   -   In  -   Doing FlashDisableWP at start and FlashEnableWP at end
//  wAddr   -   In  -   Erase 4K BYTE sector address
///////////////////////////////////////////////////////////////////////////////
void FlashSectorErase( Bool bDoWP, WORD wAddr )
{

    if( wAddr < FDATA_START_ADDR )
        return;

    if( bDoWP )
        FlashDisableWP( wAddr );

    // 1st
    SPI_SI_BUF[0]   = WREN;
    // 2nd
    if( g_ucFlashID == FLASH_SST )
        SPI_SI_BUF[1] = SST_ERASE_SECTOR;
    else if( g_ucFlashID == FLASH_PMC )
        SPI_SI_BUF[1] = PMC_ERASE_SECTOR;
    else if( g_ucFlashID == FLASH_MXIC )
        SPI_SI_BUF[1] = MXIC_ERASE_SECTOR;
    else if( g_ucFlashID == FLASH_EON )
        SPI_SI_BUF[1] = EON_ERASE_SECTOR;
    else if( g_ucFlashID == FLASH_AMIC )
        SPI_SI_BUF[1] = AMIC_ERASE_SECTOR;
    else if( g_ucFlashID == FLASH_WINBOND )
        SPI_SI_BUF[1] = WINBOND_ERASE_SECTOR;
    else if( g_ucFlashID == FLASH_BOYA)
        SPI_SI_BUF[1] = BOYA_ERASE_SECTOR;
	else if( g_ucFlashID == FLASH_ZB )
        SPI_SI_BUF[1] = ZB_ERASE_SECTOR;
    else
        SPI_SI_BUF[1] = SST_ERASE_SECTOR;

    SPI_SI_BUF[2] = TOTAL_BANK_NUM - 1; //(wAddr>>16)&0x0ff;
    SPI_SI_BUF[3] = ( wAddr >> 8 ) & 0x0ff;
    SPI_SI_BUF[4] = wAddr & 0x0ff;
    // 3rd
    SPI_SI_BUF[5]   = RDSR;

    SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
    SPI_CTRL1       = (EN_2ND_CMD|EN_3RD_CMD|RDSR_IN_3RD|EN_AUTO_CHECK);
    SET_WBF_SIZE(1, 4, 1);
    SET_RBF_SIZE(0, 0, 1);
    SPI_TRIGGER     = 0x01; //start flash programming
    drvFlashWaitSPINotBusy();

    /*
    FlashWriteEnable();
    if( g_ucFlashID == FLASH_SST )
        SPI_SI_BUF[0] = SST_ERASE_SECTOR;
    else if( g_ucFlashID == FLASH_PMC )
        SPI_SI_BUF[0] = PMC_ERASE_SECTOR;
    else if( g_ucFlashID == FLASH_MXIC )
        SPI_SI_BUF[0] = MXIC_ERASE_SECTOR;
    else if( g_ucFlashID == FLASH_EON )
        SPI_SI_BUF[0] = EON_ERASE_SECTOR;
    else if( g_ucFlashID == FLASH_AMIC )
        SPI_SI_BUF[0] = AMIC_ERASE_SECTOR;
    else
        SPI_SI_BUF[0] = SST_ERASE_SECTOR;

    SPI_SI_BUF[1] = TOTAL_BANK_NUM - 1; //(wAddr>>16)&0x0ff;
    SPI_SI_BUF[2] = ( wAddr >> 8 ) & 0x0ff;
    SPI_SI_BUF[3] = wAddr & 0x0ff;

    SPI_CTRL = 0xFB; //0xFC;
    */

    if( bDoWP )
        FlashEnableWP();

}

///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Write one byte to flash
// <Parameter>: -  <Flow>   -   <Description>
//-----------------------------------------------------------------------------
//  bDoWP   -   In  -   Doing FlashDisableWP at start and FlashEnableWP at end
//  wAddr   -   In  -   Flash Address to be write, high byte address will be (TOTAL_BANK_NUM-1)
//                      wAddr < FDATA_FACTORY_ADDR will be limit, so address < 0x3B000 will be limit
//  value   -   In  -   The value write to flash
///////////////////////////////////////////////////////////////////////////////
void FlashWriteByte( Bool bDoWP, WORD wAddr, BYTE value )
{

    if( wAddr < FDATA_START_ADDR )
        return;

    if( bDoWP )
        FlashDisableWP( wAddr );
    else if( g_ucFlashID == FLASH_BOYA && (g_ucFlashDeviceID == BY25D20 || g_ucFlashDeviceID == BY25D40) )
        FlashDisableWP_BOYA_BY25D40_BY25D20(wAddr);
    else
        FlashDisableStausProtect_MXIC2026();

    // 1st
    SPI_SI_BUF[0]   = WREN;
    // 2nd
    SPI_SI_BUF[1] = BYTE_WRITE;
    SPI_SI_BUF[2] = TOTAL_BANK_NUM - 1; //(wAddr>>16)&0x0ff;
    SPI_SI_BUF[3] = ( wAddr >> 8 ) & 0x0ff;
    SPI_SI_BUF[4] = wAddr & 0x0ff;
    SPI_SI_BUF[5] = value;
    // 3rd
    SPI_SI_BUF[6]   = RDSR;

    SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
    SPI_CTRL1       = (EN_2ND_CMD|EN_3RD_CMD|RDSR_IN_3RD|EN_AUTO_CHECK);
    SET_WBF_SIZE(1, 5, 1);
    SET_RBF_SIZE(0, 0, 1);
    SPI_TRIGGER     = 0x01; //start flash programming
    drvFlashWaitSPINotBusy();
    
    /*
    FlashWriteEnable();
    //CleanSPI_SI_BUF();
    //MCU_CACHE_CTL = 0x00;
    SPI_SI_BUF[0] = BYTE_WRITE;
    SPI_SI_BUF[1] = TOTAL_BANK_NUM - 1; //(wAddr>>16)&0x0ff;
    SPI_SI_BUF[2] = ( wAddr >> 8 ) & 0x0ff;
    SPI_SI_BUF[3] = wAddr & 0x0ff;
    SPI_SI_BUF[4] = value;
    SPI_CTRL = 0xFC;
    //MCU_CACHE_CTL = 0x20;
    //retry=10;
    //while((SPI_CTRL&BIT7)&&retry--)
    //  ForceDelay1ms(1);
    */

    if( bDoWP )
        FlashEnableWP();
    //FLASH_printData("value=%x\n",value);
    //FLASH_printData("S4=%x",SPI_SI_BUF[4]);

}
void FlashWriteAnyByte( Bool bDoWP, DWORD dwAddr, BYTE value )
{
    if( bDoWP )
        FlashDisableWP( dwAddr );
    else if( g_ucFlashID == FLASH_BOYA && (g_ucFlashDeviceID == BY25D20 || g_ucFlashDeviceID == BY25D40) )
        FlashDisableWP_BOYA_BY25D40_BY25D20(dwAddr);
    else
        FlashDisableStausProtect_MXIC2026();

    // 1st
    SPI_SI_BUF[0]   = WREN;
    // 2nd
    SPI_SI_BUF[1] = BYTE_WRITE;
    SPI_SI_BUF[2] = (dwAddr>>16)&0x0ff; //USER_DATA_BANK; //TOTAL_BANK_NUM - 1; //(wAddr>>16)&0x0ff;
    SPI_SI_BUF[3] = ( dwAddr >> 8 ) & 0x0ff;
    SPI_SI_BUF[4] = dwAddr & 0x0ff;
    SPI_SI_BUF[5] = value;
    // 3rd
    SPI_SI_BUF[6]   = RDSR;

    SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
    SPI_CTRL1       = (EN_2ND_CMD|EN_3RD_CMD|RDSR_IN_3RD|EN_AUTO_CHECK);
    SET_WBF_SIZE(1, 5, 1);
    SET_RBF_SIZE(0, 0, 1);
    SPI_TRIGGER     = 0x01; //start flash programming
    drvFlashWaitSPINotBusy();

    /*
    FlashWriteEnable();
    //CleanSPI_SI_BUF();
    //MCU_CACHE_CTL = 0x00;
    SPI_SI_BUF[0] = BYTE_WRITE;
    SPI_SI_BUF[1] = USER_DATA_BANK; //TOTAL_BANK_NUM - 1; //(wAddr>>16)&0x0ff;
    SPI_SI_BUF[2] = ( wAddr >> 8 ) & 0x0ff;
    SPI_SI_BUF[3] = wAddr & 0x0ff;
    SPI_SI_BUF[4] = value;
    SPI_CTRL = 0xFC;
    //MCU_CACHE_CTL = 0x20;
    //retry=10;
    //while((SPI_CTRL&BIT7)&&retry--)
    //  ForceDelay1ms(1);
    */

    if( bDoWP )
        FlashEnableWP();
    //FLASH_printData("value=%x\n",value);
    //FLASH_printData("S4=%x",SPI_SI_BUF[4]);
}

#if 0//HDCPKEY_IN_Flash
void FlashHDCPWriteByte( WORD wAddr, BYTE value )
{

    if( wAddr < FDATA_START_ADDR )
        return;

    FlashDisableStausProtect_MXIC2026();//For MXIC 2026

    // 1st
    SPI_SI_BUF[0]   = WREN;
    // 2nd
    SPI_SI_BUF[1] = BYTE_WRITE;
    SPI_SI_BUF[2] = TOTAL_BANK_NUM - 1; //(wAddr>>16)&0x0ff;
    SPI_SI_BUF[3] = ( wAddr >> 8 ) & 0x0ff;
    SPI_SI_BUF[4] = wAddr & 0x0ff;
    SPI_SI_BUF[5] = value;
    // 3rd
    SPI_SI_BUF[6]   = RDSR;

    SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
    SPI_CTRL1       = (EN_2ND_CMD|EN_3RD_CMD|RDSR_IN_3RD|EN_AUTO_CHECK);
    SET_WBF_SIZE(1, 5, 1);
    SET_RBF_SIZE(0, 0, 1);
    SPI_TRIGGER     = 0x01; //start flash programming
    drvFlashWaitSPINotBusy();
    
    /*
    FlashWriteEnable();
    SPI_SI_BUF[0] = BYTE_WRITE;
    SPI_SI_BUF[1] = TOTAL_BANK_NUM - 1; //(wAddr>>16)&0x0ff;
    SPI_SI_BUF[2] = ( wAddr >> 8 ) & 0x0ff;
    SPI_SI_BUF[3] = wAddr & 0x0ff;
    SPI_SI_BUF[4] = value;
    SPI_CTRL = 0xFC;
    */

}
#endif
///////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Read one byte from flash.
// <Returns> : Data value in the address
// <Parameter>:  -  <Flow>  -  <Description>
//-----------------------------------------------------------------------------
//  wAddr   -   In  -   Only in the same bank ot MsFlash.c
///////////////////////////////////////////////////////////////////////////////
#if FLASH_READ_BYTE_BY_CODE_POINTER
#define xfr_FlashCode ((BYTE code*)0x0000)
BYTE FlashReadByte( WORD wAddr )
{
    return xfr_FlashCode[wAddr];
}
#else
BYTE FlashReadByte( WORD wAddr )
{
    //1st
    SPI_SI_BUF[0]   = (SPI_MODE == SPI_MODE_FR) ? H_READ:READ;
    SPI_SI_BUF[1]   = TOTAL_BANK_NUM - 1;
    SPI_SI_BUF[2]   = (wAddr>>8)&0xff;
    SPI_SI_BUF[3]   = wAddr&0xff;
    //2nd
    SPI_SI_BUF[4]   = RDSR;
    if(SPI_MODE == SPI_MODE_FR)
        SPI_SI_BUF[5]   = RDSR;
    SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
    SPI_CTRL1       = (EN_2ND_CMD|RDSR_IN_2ND|EN_AUTO_CHECK);
    SET_WBF_SIZE( (SPI_MODE == SPI_MODE_FR) ? 5:4, 1, 0);
    SET_WBF_SIZE( 5, 1, 0);
    SET_RBF_SIZE(1, 1, 0);
    SPI_TRIGGER     = 0x01; //start flash programming
    
    drvFlashWaitSPINotBusy();
    
    return SPI_SO_BUF[0];
}
#endif

// dwAddr : Bank + 4B address
BYTE FlashReadAnyByte( DWORD dwAddr )
{
    //1st
    SPI_SI_BUF[0]   = (SPI_MODE == SPI_MODE_FR) ? H_READ:READ;
    SPI_SI_BUF[1]   = (dwAddr>>16)&0xff; //USER_DATA_BANK; //TOTAL_BANK_NUM - 1;
    SPI_SI_BUF[2]   = (dwAddr>>8)&0xff;
    SPI_SI_BUF[3]   = dwAddr&0xff;
    //2nd
    SPI_SI_BUF[4]   = RDSR;
    if(SPI_MODE == SPI_MODE_FR)
        SPI_SI_BUF[5]   = RDSR;
    SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
    SPI_CTRL1       = (EN_2ND_CMD|RDSR_IN_2ND|EN_AUTO_CHECK);
    SET_WBF_SIZE( (SPI_MODE == SPI_MODE_FR) ? 5:4, 1, 0);
    SET_WBF_SIZE( 5, 1, 0);
    SET_RBF_SIZE(1, 1, 0);
    SPI_TRIGGER     = 0x01; //start flash programming

    drvFlashWaitSPINotBusy();

    return SPI_SO_BUF[0];
}

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
void Flash_WriteTbl( Bool bDoWP, WORD wAddr, BYTE *buffer, WORD count )
{
    BYTE i = 0;

    if( wAddr < FDATA_START_ADDR )
        return;

    if( bDoWP )
        FlashDisableWP( wAddr ); // 2005/5/11 上午 10:19:39 by keng
    //MCU_CACHE_CTL = 0x00;
    while( count > 0 )
    {
        if( g_ucFlashID == FLASH_BOYA && (g_ucFlashDeviceID == BY25D20 || g_ucFlashDeviceID == BY25D40) )
            FlashDisableWP_BOYA_BY25D40_BY25D20(wAddr);
        else
            FlashDisableStausProtect_MXIC2026();

        // 1st
        SPI_SI_BUF[0]   = WREN;
        // 2nd
        SPI_SI_BUF[1] = BYTE_WRITE;
        SPI_SI_BUF[2] = TOTAL_BANK_NUM - 1; //(wAddr>>16)&0x0ff;
        SPI_SI_BUF[3] = ( wAddr >> 8 ) & 0x0ff;
        SPI_SI_BUF[4] = wAddr & 0x0ff;
        SPI_SI_BUF[5] = *( buffer + ( i++ ) );
        // 3rd
        SPI_SI_BUF[6]   = RDSR;

        SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
        SPI_CTRL1       = (EN_2ND_CMD|EN_3RD_CMD|RDSR_IN_3RD|EN_AUTO_CHECK);
        SET_WBF_SIZE(1, 5, 1);
        SET_RBF_SIZE(0, 0, 1);
        SPI_TRIGGER     = 0x01; //start flash programming
        drvFlashWaitSPINotBusy();
        
        /*
        FlashWriteEnable();
        SPI_SI_BUF[0] = BYTE_WRITE;
        SPI_SI_BUF[1] = TOTAL_BANK_NUM - 1; //(wAddr>>16)&0x0ff;
        SPI_SI_BUF[2] = ( wAddr >> 8 ) & 0x0ff;
        SPI_SI_BUF[3] = wAddr & 0x0ff;
        SPI_SI_BUF[4] = *( buffer + ( i++ ) );
        SPI_CTRL = 0xFC;
        */
        
        wAddr += 1;
        count -= 1;

    }
    //MCU_CACHE_CTL = 0x20;


    if( bDoWP )
        FlashEnableWP();// 2005/5/11 上午 10:19:43 by keng
}
void Flash_MoveTbl( Bool bDoWP, WORD wSrcAddr, WORD wDestAddr, WORD count )
{
    BYTE ucTemp;

    if( wDestAddr < FDATA_START_ADDR )
        return;

    if( bDoWP )
        FlashDisableWP( wDestAddr ); // 2005/5/11 上午 10:19:39 by keng
    while( count > 0 )
    {
        ucTemp = FlashReadByte( wSrcAddr );
        if( g_ucFlashID == FLASH_BOYA && (g_ucFlashDeviceID == BY25D20 || g_ucFlashDeviceID == BY25D40) )
            FlashDisableWP_BOYA_BY25D40_BY25D20(wSrcAddr);
        else
            FlashDisableStausProtect_MXIC2026();

        // 1st
        SPI_SI_BUF[0]   = WREN;
        // 2nd
        SPI_SI_BUF[1] = BYTE_WRITE;
        SPI_SI_BUF[2] = TOTAL_BANK_NUM - 1; //(wAddr>>16)&0x0ff;
        SPI_SI_BUF[3] = ( wDestAddr >> 8 ) & 0x0ff;
        SPI_SI_BUF[4] = wDestAddr & 0x0ff;
        SPI_SI_BUF[5] = ucTemp;
        // 3rd
        SPI_SI_BUF[6]   = RDSR;

        SPI_CTRL0       = SPI_CTRL_VALUE; //ctrl en FSP/reset
        SPI_CTRL1       = (EN_2ND_CMD|EN_3RD_CMD|RDSR_IN_3RD|EN_AUTO_CHECK);
        SET_WBF_SIZE(1, 5, 1);
        SET_RBF_SIZE(0, 0, 1);
        SPI_TRIGGER     = 0x01; //start flash programming
        drvFlashWaitSPINotBusy();

        /*
        FlashWriteEnable();
        SPI_SI_BUF[0] = BYTE_WRITE;
        SPI_SI_BUF[1] = TOTAL_BANK_NUM - 1; //(wDestAddr>>16)&0x0ff; // bank 3
        SPI_SI_BUF[2] = ( wDestAddr >> 8 ) & 0x0ff;
        SPI_SI_BUF[3] = wDestAddr & 0x0ff;
        SPI_SI_BUF[4] = ucTemp;
        SPI_CTRL = 0xFC;
        */
        
        wSrcAddr += 1;
        wDestAddr += 1;
        count -= 1;
    }
    if( bDoWP )
        FlashEnableWP();// 2005/5/11 上午 10:19:43 by keng
}
//////////////////////////////////////////////////////////////////////////////
// <DRV><Description>: Read a table from flash
// <Parameter>:     -   <Flow>  -   <Description>
// ---------------------------------------------------------------------------
//  wAddr   -   In  -   Flash Address
//  buffer  -   Out -   Data point of the table
//  count   -   In  -   Number of BYTE data
//////////////////////////////////////////////////////////////////////////////
void Flash_ReadTbl( WORD wAddr, BYTE *buffer, WORD count )
{
    WORD i;
    for(i = 0; i < count; i++)
#if FLASH_READ_BYTE_BY_CODE_POINTER
        *( buffer + i ) = xfr_FlashCode[wAddr+i];
#else
        *(buffer+i) = FlashReadByte(wAddr+i);
#endif
}


//////////////////////////////////////////////////////////////////////////////
// <API><Description>: Search specific FLASH_BUFFTER_TYPE form FDATA_BUF_ADDR(x)
// <Returns> : Specific buffer address when found; 0 when not found
// <Parameter>:     -   <Flow>  -   <Description>
// ---------------------------------------------------------------------------
//  ucType  -   In  -   buffer type, such as FBufType_MonitorSetting, FBufType_TimingMode ..
//////////////////////////////////////////////////////////////////////////////
WORD Flash_SearchBufferAddr( FLASH_BUFFTER_TYPE ucType )
{
    BYTE ucTemp;

    for( ucTemp = 0; ucTemp < FDATA_DYNAMIC_SECTOR_NUM; ucTemp++ )
    {
        if( FlashReadByte( FDATA_BUF_ADDR( ucTemp ) ) == ucType )
            return FDATA_BUF_ADDR( ucTemp );
    }
    return 0;
}
//////////////////////////////////////////////////////////////////////////////
// <API><Description>: Compare the FlashReadByte(wAddr + i*wRange)==ucIdent
//                                 FlashReadByte(wAddr + i*wRange + wRange - 1) == ucChecksum
//                     return i if equal ,else return IDENT_NOT_FOUND
// Search index from wStartIdx to wEndIdx to replace i. Can from 0 search to 255(include)
// Also can from 255 to 0(include)
// <Returns> : Specific buffer address when found; IDENT_NOT_FOUND when not found
// <Parameter>:     -   <Flow>  -   <Description>
// ---------------------------------------------------------------------------
//  wAddr       -   In  -   Flash start Address
//  wStartIdx   -   In  -   wAddr + wStartIdx*wRange
//  wEndIdx     -   In  -   wAddr + wEndIdx*wRange
//  wRange      -   In  -   Each search range
//  ucIdent     -   In  -   Data of want to compare
//  ucChecksum  -   In  -   Data of want to compare
//////////////////////////////////////////////////////////////////////////////
WORD Flash_SearchIdentChecksum( WORD wAddr, WORD wStartIdx, WORD wEndIdx, WORD wRange, BYTE ucIdent, BYTE ucChecksum )
{
    WORD i;

    if( wStartIdx < wEndIdx )
    {
        i = wStartIdx;
        while( i >= wStartIdx && i <= wEndIdx )
        {
            if( FlashReadByte( wAddr + i * wRange ) == ucIdent && FlashReadByte( wAddr + i * wRange + wRange - 1 ) == ucChecksum )
                return i;
            i++;
        }
    }
    else
    {
        i = wStartIdx;
        while( i >= wEndIdx && i <= wStartIdx )
        {
            if( FlashReadByte( wAddr + i * wRange ) == ucIdent && FlashReadByte( wAddr + i * wRange + wRange - 1 ) == ucChecksum )
                return i;
            i--;
        }

    }
    //FLASH_printMsg("------- IDENT_CHECKSUM_NOT_FOUND -------");
    return IDENT_NOT_FOUND;
}

//////////////////////////////////////////////////////////////////////////////
//  <API><Description>: Get each buffer address and assign to variable
//  FLASH_MonitorSettingAddr
//  FLASH_MonitorSetting2Addr
//  FLASH_TimingModeAddr
//  FLASH_FactorySettingAddr
//  FLASH_FreeBufferAddr
//////////////////////////////////////////////////////////////////////////////
void Flash_GetBufferAddr( void )
{
    WORD wAddr;
    FLASH_HDCP_EDIDAddr = FDATA_HDCPKEY_START_ADDR;
    FLASH_FactorySettingAddr = FDATA_FACTORY_ADDR;
    FLASH_MonitorSettingAddr = 0;
    FLASH_MonitorSetting2Addr = 0;
    FLASH_TimingModeAddr = 0;
    FLASH_FreeBufferAddr = 0;

    FLASH_printData( "_FGBA_ FLASH_FactorySettingAddr %x ", ( WORD )( FLASH_FactorySettingAddr ) );

    wAddr = Flash_SearchBufferAddr( FBufType_MonitorSetting );
    if( wAddr )
    {
        FLASH_MonitorSettingAddr = wAddr;

        FLASH_printData( "_FGBA_ FLASH_MonitorSettingAddr %x ", ( WORD )( FLASH_MonitorSettingAddr ) );

    }

    wAddr = Flash_SearchBufferAddr( FBufType_MonitorSetting2 );
    if( wAddr )
    {
        FLASH_MonitorSetting2Addr = wAddr;

        FLASH_printData( "_FGBA_ FLASH_MonitorSetting2Addr %x ", ( WORD )( FLASH_MonitorSetting2Addr ) );

    }

    wAddr = Flash_SearchBufferAddr( FBufType_TimingMode );
    if( wAddr )
    {
        FLASH_TimingModeAddr = wAddr;

        FLASH_printData( "_FGBA_ FLASH_TimingModeAddr %x ", ( WORD )( FLASH_TimingModeAddr ) );

    }

    wAddr = Flash_SearchBufferAddr( FBufType_EmptyBuffer );
    if( wAddr )
    {
        FLASH_FreeBufferAddr = wAddr;

        FLASH_printData( "FLASH_FreeBufferAddr %x ", ( WORD )( FLASH_FreeBufferAddr ) );

    }
    // 091022 coding, free buffer type should be 0xff or 0x00
    else
    {
        if(( wAddr = Flash_SearchBufferAddr( FBufType_FullBuffer ) ) >= FDATA_DYNAMIC_ADDR )
            FLASH_FreeBufferAddr = wAddr;
    }

    return;
}

//////////////////////////////////////////////////////////////////////////////
// <API><Description>: After execute Flash_GetBufferAddr, makesure each address reasonable
//////////////////////////////////////////////////////////////////////////////
void Flash_AllocateBufferAddr( void )
{
    BYTE ucTemp;

    if( FLASH_MonitorSettingAddr == 0 )
    {
        for( ucTemp = 0; ucTemp < FDATA_DYNAMIC_SECTOR_NUM; ucTemp++ )
        {
#if 0
            FLASH_printData( "Flash_Allocate 01 %x ", ( WORD )( FLASH_TimingModeAddr >> 8 ) );
            FLASH_printData( "Flash_Allocate 02 %x ", ( WORD )( FLASH_FreeBufferAddr >> 8 ) );
            FLASH_printData( "Flash_Allocate 03 %x ", ( WORD )( FDATA_BUF_ADDR( ucTemp ) >> 8 ) );
#endif
            if( FDATA_BUF_ADDR( ucTemp ) != FLASH_TimingModeAddr
                    && FDATA_BUF_ADDR( ucTemp ) != FLASH_FreeBufferAddr
                    && FDATA_BUF_ADDR( ucTemp ) != FLASH_MonitorSetting2Addr )
            {
                FLASH_MonitorSettingAddr = FDATA_BUF_ADDR( ucTemp );
                break;
            }
        }

        FLASH_printData( "_FABA_ FLASH_MonitorSettingAddr %x ", ( WORD )( FLASH_MonitorSettingAddr ) );

    }
    if( FLASH_MonitorSetting2Addr == 0 )
    {
        for( ucTemp = 0; ucTemp < FDATA_DYNAMIC_SECTOR_NUM; ucTemp++ )
        {
            if( FDATA_BUF_ADDR( ucTemp ) != FLASH_MonitorSettingAddr
                    && FDATA_BUF_ADDR( ucTemp ) != FLASH_TimingModeAddr
                    && FDATA_BUF_ADDR( ucTemp ) != FLASH_FreeBufferAddr )
            {
                FLASH_MonitorSetting2Addr = FDATA_BUF_ADDR( ucTemp );
                break;
            }
        }

        FLASH_printData( "_FABA_ FLASH_MonitorSetting2Addr %x ", ( WORD )( FLASH_MonitorSetting2Addr ) );

    }
    if( FLASH_TimingModeAddr == 0 )
    {
        for( ucTemp = 0; ucTemp < FDATA_DYNAMIC_SECTOR_NUM; ucTemp++ )
        {
            if( FDATA_BUF_ADDR( ucTemp ) != FLASH_MonitorSettingAddr
                    && FDATA_BUF_ADDR( ucTemp ) != FLASH_FreeBufferAddr
                    && FDATA_BUF_ADDR( ucTemp ) != FLASH_MonitorSetting2Addr )
            {
                FLASH_TimingModeAddr = FDATA_BUF_ADDR( ucTemp );
                break;
            }
        }

        FLASH_printData( "_FABA_ FLASH_TimingModeAddr %x ", ( WORD )( FLASH_TimingModeAddr ) );

    }
    if( FLASH_FreeBufferAddr == 0 )
    {
        for( ucTemp = 0; ucTemp < FDATA_DYNAMIC_SECTOR_NUM; ucTemp++ )
        {
            if( FDATA_BUF_ADDR( ucTemp ) != FLASH_MonitorSettingAddr
                    && FDATA_BUF_ADDR( ucTemp ) != FLASH_TimingModeAddr
                    && FDATA_BUF_ADDR( ucTemp ) != FLASH_MonitorSetting2Addr )
            {
                FLASH_FreeBufferAddr = FDATA_BUF_ADDR( ucTemp );
                break;
            }
        }

        FLASH_printData( "_FABA_ FLASH_FreeBufferAddr %x ", ( WORD )( FLASH_FreeBufferAddr ) );

    }

    if( FLASH_MonitorSettingAddr == 0 || FLASH_MonitorSetting2Addr == 0 || FLASH_TimingModeAddr == 0 || FLASH_FreeBufferAddr == 0 )
    {
        FLASH_MonitorSettingAddr = FDATA_BUF_ADDR( 0 );
        FLASH_MonitorSetting2Addr = FDATA_BUF_ADDR( 1 );
        FLASH_TimingModeAddr = FDATA_BUF_ADDR( 2 );
        FLASH_FreeBufferAddr = FDATA_BUF_ADDR( 3 );

        FLASH_printMsg( "_FABA_ Addr NEW" );
        FLASH_printData( "_FABA_ FLASH_MonitorSettingAddr %x ", ( WORD )( FLASH_MonitorSettingAddr ) );
        FLASH_printData( "_FABA_ FLASH_MonitorSetting2Addr %x ", ( WORD )( FLASH_MonitorSetting2Addr ) );
        FLASH_printData( "_FABA_ FLASH_TimingModeAddr %x ", ( WORD )( FLASH_TimingModeAddr ) );
        FLASH_printData( "_FABA_ FLASH_FactorySettingAddr %x ", ( WORD )( FLASH_FactorySettingAddr ) );
        FLASH_printData( "_FABA_ FLASH_FreeBufferAddr %x ", ( WORD )( FLASH_FreeBufferAddr ) );
        FLASH_printData( "_FABA_ FLASH_HDCP_EDIDAddr %x ", ( WORD )( FLASH_HDCP_EDIDAddr) );
    }

}
#if 0
Bool HDCP_BKSVIsValid(WORD addr)
{
    BYTE i, j, Count_1=0, Count_0=0, ReadValue;
    for(i = 0; i < 5; i++)
    {
        ReadValue = FlashReadByte(addr+i);
        for(j = 0; j < 8; j++)
        {
            if(_bit7_(ReadValue<<j))
                Count_1++;
            else
                Count_0++;
        }
    }
                
    if(Count_0 == 20 && Count_1 == 20)
        return TRUE;
    else
        return FALSE;
    
}

Bool EDID_HeaderIsValid(WORD addr)
{
    if((FlashReadByte(addr) == 0x00) &&
        (FlashReadByte(addr+1) == 0xFF) &&
        (FlashReadByte(addr+2) == 0xFF) &&
        (FlashReadByte(addr+3) == 0xFF) &&
        (FlashReadByte(addr+4) == 0xFF) &&
        (FlashReadByte(addr+5) == 0xFF) &&
        (FlashReadByte(addr+6) == 0xFF) &&
        (FlashReadByte(addr+7) == 0x00))
        return TRUE;
    else
        return FALSE;
}
#endif
//////////////////////////////////////////////////////////////////////////////
// <API><Description>: Check each buffer type correct or not,
//                Erase the sector when type wrong
//////////////////////////////////////////////////////////////////////////////
void Flash_CheckBufferType( void )
{
    if( FBufType_HDCP_EDID!= FlashReadByte( FLASH_HDCP_EDIDAddr) )
    {
#if 1
        bFlashWriteFactory= TRUE;
        FlashSectorErase( TRUE, FLASH_HDCP_EDIDAddr );
        FlashWriteByte( TRUE, FLASH_HDCP_EDIDAddr, FBufType_HDCP_EDID);
        bFlashWriteFactory = FALSE;
        FLASH_printMsg( "Flash_CheckBufferType SetHDCPType" );
#else
        bFlashWriteFactory= TRUE;
        if(HDCP_BKSVIsValid(HDCPKEYSET_START))
        {
            FlashWriteByte( TRUE, HDCPKEY_STATUS_ADDR, FLASH_KEY_VALID );
        }

        if(EDID_HeaderIsValid(DDCDPKEYSET_START))
        {
            FlashWriteByte( TRUE, DDCDPKEY_STATUS_ADDR, FLASH_KEY_VALID );
        }
        FlashWriteByte( TRUE, FLASH_HDCP_EDIDAddr, FBufType_HDCP_EDID);
        bFlashWriteFactory = FALSE;
        FLASH_printMsg( "Flash_CheckBufferType SetHDCPType" );
#endif

    }
    if( FBufType_FactorySetting != FlashReadByte( FLASH_FactorySettingAddr ) )
    {
        bFlashWriteFactory = TRUE;
        FlashSectorErase( TRUE, FLASH_FactorySettingAddr );
        FlashWriteByte( TRUE, FLASH_FactorySettingAddr, FBufType_FactorySetting );
        bFlashWriteFactory = FALSE;
        FLASH_printMsg( "Flash_CheckBufferType SetFactoryType" );
    }
    if( FBufType_MonitorSetting != FlashReadByte( FLASH_MonitorSettingAddr ) )
    {
        FlashSectorErase( TRUE, FLASH_MonitorSettingAddr );
        FlashWriteByte( TRUE, FLASH_MonitorSettingAddr, FBufType_MonitorSetting );
        FLASH_printMsg( "Flash_CheckBufferType SetMonitorType" );
    }
    if( FBufType_MonitorSetting2 != FlashReadByte( FLASH_MonitorSetting2Addr ) )
    {
        FlashSectorErase( TRUE, FLASH_MonitorSetting2Addr );
        FlashWriteByte( TRUE, FLASH_MonitorSetting2Addr, FBufType_MonitorSetting2 );
        FLASH_printMsg( "Flash_CheckBufferType SetMonitorSetting2Type" );
    }
    if( FBufType_TimingMode != FlashReadByte( FLASH_TimingModeAddr ) )
    {
        FlashSectorErase( TRUE, FLASH_TimingModeAddr );
        FlashWriteByte( TRUE, FLASH_TimingModeAddr, FBufType_TimingMode );
        FLASH_printMsg( "Flash_CheckBufferType SetTimingModeAddrType" );
    }

}

//////////////////////////////////////////////////////////////////////////////
// <API><Description>: Clear all timing mode save in flash
//////////////////////////////////////////////////////////////////////////////
void Flash_ClearModeSet( void )
{
    FLASH_printMsg( "_FCMS_ Flash_ClearModeSet" );
    FlashSectorErase( TRUE, FLASH_TimingModeAddr );
    FlashWriteByte( TRUE, FLASH_TimingModeAddr, FBufType_TimingMode );
}

//////////////////////////////////////////////////////////////////////////////
// <API><Description>:  Write Factory setting, HDCP Key, DDCA, DDCD key to free buffer,
//                  Exchange buffer address FLASH_FreeBufferAddr and FLASH_FactorySettingAddr
//////////////////////////////////////////////////////////////////////////////
BYTE Flash_GetKeyIndex(FLASH_KEY_TYPE ucType, Bool bWrite)
{
    BYTE ucIndex, ucStatus, ucSize=0;
    WORD wAddr;

    if(ucType == FLASH_KEY_FACTORY)
    {
        ucSize = FACTORY_SECTOR_SIZE/FACTORY_SIZE;
        wAddr  = FACTORY_STATUS_ADDR;
    }
    else if(ucType == FLASH_KEY_PANEL)
    {
        ucSize = PANEL_SECTOR_SIZE/PANEL_SIZE;
        wAddr  = PANEL_STATUS_ADDR;
    }
#if 1    
    else if(ucType == FLASH_KEY_GAMMA)
    {
        ucSize = GAMMA_SECTOR_SIZE/GAMMA_SIZE;
        wAddr  = GAMMA_STATUS_ADDR;
    }
    
    else if(ucType == FLASH_KEY_HDCP)
    {
        ucSize = HDCPKEY_SECTOR_SIZE/HDCPKEY_SIZE;
        wAddr  = HDCPKEY_STATUS_ADDR;
    }
    
    else if(ucType == FLASH_KEY_DDCA)
    {
        ucSize = DDCAKEY_SECTOR_SIZE/DDCAKEY_SIZE;
        wAddr  = DDCAKEY_STATUS_ADDR;
    }
    else if(ucType == FLASH_KEY_DDCD)
    {
        ucSize = DDCDKEY_SECTOR_SIZE/DDCDKEY_SIZE;
        wAddr  = DDCDKEY_STATUS_ADDR;
    }
#endif    
    else if(ucType == FLASH_KEY_DDCDP)
    {
        ucSize = DDCDPKEY_SECTOR_SIZE/DDCDPKEY_SIZE;
        wAddr  = DDCDPKEY_STATUS_ADDR;
    }

    for(ucIndex=0;ucIndex<ucSize;ucIndex++)
    {
        ucStatus = FlashReadByte(wAddr+ucIndex);
        if(bWrite)
        {
            if( ucStatus == FLASH_KEY_EMPTY || ucStatus == FLASH_KEY_WRITE)
                break;
        }
        else
        {
            if( ucStatus == FLASH_KEY_VALID)
                break;
        }
    }

    if( ucIndex >= ucSize )
    {
        if(bWrite)
            ucIndex = ucSize - 1;
        else
            ucIndex = 0;
    }

    return ucIndex;
}
//////////////////////////////////////////////////////////////////////////////
// <API><Description>:  Write Factory setting, HDCP Key, DDCA, DDCD key to free buffer,
//                  Exchange buffer address FLASH_FreeBufferAddr and FLASH_FactorySettingAddr
//////////////////////////////////////////////////////////////////////////////
void Flash_BackupFactorySetToBuffer( void )
{
    BYTE xdata ucKeyIndex;
    WORD wAddr;

    FLASH_printMsg( "  Flash_BackupFactorySetToBuffer " );

    FlashDisableWP( FLASH_FreeBufferAddr );
    ucKeyIndex = Flash_GetKeyIndex(FLASH_KEY_FACTORY, FALSE);
    if(FlashReadByte(FACTORY_STATUS_ADDR+ucKeyIndex) == FLASH_KEY_VALID
        && FlashReadByte(FACTORY_BUF_STATUS_ADDR) == FLASH_KEY_EMPTY)
    {
        Flash_MoveTbl(FALSE, FACTORY_START+(WORD)ucKeyIndex*FACTORY_SIZE, FACTORY_BUF_START, FACTORY_SIZE);
        FlashWriteByte(FALSE, FACTORY_BUF_STATUS_ADDR, FLASH_KEY_VALID);
        FLASH_printMsg( "  _FACT_ " );
    }

    ucKeyIndex = Flash_GetKeyIndex(FLASH_KEY_PANEL, FALSE);
    if(FlashReadByte(PANEL_STATUS_ADDR+ucKeyIndex) == FLASH_KEY_VALID
        && FlashReadByte(PANEL_BUF_STATUS_ADDR) == FLASH_KEY_EMPTY)
    {
        Flash_MoveTbl(FALSE, PANEL_START+(WORD)ucKeyIndex*PANEL_SIZE, PANEL_BUF_START, PANEL_SIZE);
        FlashWriteByte(FALSE, PANEL_BUF_STATUS_ADDR, FLASH_KEY_VALID);
        FLASH_printMsg( "  _PANEL_ " );
    }

#if 0
    ucKeyIndex = Flash_GetKeyIndex(FLASH_KEY_GAMMA, FALSE);
    if(FlashReadByte(GAMMA_STATUS_ADDR+ucKeyIndex) == FLASH_KEY_VALID
        && FlashReadByte(GAMMA_BUF_STATUS_ADDR) == FLASH_KEY_EMPTY)
    {
        Flash_MoveTbl(FALSE, GAMMA_START+(WORD)ucKeyIndex*GAMMA_SIZE, GAMMA_BUF_START, GAMMA_SIZE);
        FlashWriteByte(FALSE, GAMMA_BUF_STATUS_ADDR, FLASH_KEY_VALID);
        FLASH_printMsg("  _GAMMA_ ");
    }
    
    ucKeyIndex = Flash_GetKeyIndex(FLASH_KEY_HDCP, FALSE);
    if(FlashReadByte(HDCPKEY_STATUS_ADDR+ucKeyIndex) == FLASH_KEY_VALID
        && FlashReadByte(HDCPKEY_BUF_STATUS_ADDR) == FLASH_KEY_EMPTY)
    {
        Flash_MoveTbl(FALSE, HDCPKEYSET_START+(WORD)ucKeyIndex*HDCPKEY_SIZE, HDCPKEYSET_BUF_START, HDCPKEY_SIZE);
        FlashWriteByte(FALSE, HDCPKEY_BUF_STATUS_ADDR, FLASH_KEY_VALID);
        FLASH_printMsg("  _HDCP_ ");
    }
    
    ucKeyIndex = Flash_GetKeyIndex(FLASH_KEY_DDCA, FALSE);
    if(FlashReadByte(DDCAKEY_STATUS_ADDR+ucKeyIndex) == FLASH_KEY_VALID
        && FlashReadByte(DDCAKEY_BUF_STATUS_ADDR) == FLASH_KEY_EMPTY)
    {
        Flash_MoveTbl(FALSE, DDCAKEYSET_START+(WORD)ucKeyIndex*DDCAKEY_SIZE, DDCAKEYSET_BUF_START, DDCAKEY_SIZE);
        FlashWriteByte(FALSE, DDCAKEY_BUF_STATUS_ADDR, FLASH_KEY_VALID);
        FLASH_printMsg("  _DDCA_ ");
    }
    
    ucKeyIndex = Flash_GetKeyIndex(FLASH_KEY_DDCD, FALSE);
    if(FlashReadByte(DDCDKEY_STATUS_ADDR+ucKeyIndex) == FLASH_KEY_VALID
        && FlashReadByte(DDCDKEY_BUF_STATUS_ADDR) == FLASH_KEY_EMPTY)
    {
        Flash_MoveTbl(FALSE, DDCDKEYSET_START+(WORD)ucKeyIndex*DDCDKEY_SIZE, DDCDKEYSET_BUF_START, DDCDKEY_SIZE);
        FlashWriteByte(FALSE, DDCDKEY_BUF_STATUS_ADDR, FLASH_KEY_VALID);
        FLASH_printMsg("  _DDCD_ ");
    }
    
    if( FlashReadByte( DDCHKEY_STATUS_ADDR ) == FLASH_KEY_VALID )
    {
        Flash_MoveTbl( FALSE, DDCHKEYSET_START, DDCHKEYSET_BUF_START, DDCHKEY_SIZE );
        FlashWriteByte( FALSE, DDCHKEY_BUF_STATUS_ADDR, FLASH_KEY_VALID );
        FLASH_printMsg( "  _DDCH_ " );
    }
#endif

    ucKeyIndex = Flash_GetKeyIndex(FLASH_KEY_DDCDP, FALSE);
    if(FlashReadByte(DDCDPKEY_STATUS_ADDR+ucKeyIndex) == FLASH_KEY_VALID
        && FlashReadByte(DDCDPKEY_BUF_STATUS_ADDR) == FLASH_KEY_EMPTY)
    {
        Flash_MoveTbl(FALSE, DDCDPKEYSET_START+(WORD)ucKeyIndex*DDCDPKEY_SIZE, DDCDPKEYSET_BUF_START, DDCDPKEY_SIZE);
        FlashWriteByte(FALSE, DDCDPKEY_BUF_STATUS_ADDR, FLASH_KEY_VALID);
        FLASH_printMsg("  _DDCDP_ ");
    }

    FlashWriteByte( FALSE, FLASH_FreeBufferAddr, FBufType_FactorySetting );
    FlashEnableWP();
    FlashWriteByte( TRUE, FLASH_FactorySettingAddr, 0x00 );
    wAddr = FLASH_FreeBufferAddr;
    FLASH_FreeBufferAddr = FLASH_FactorySettingAddr;
    FLASH_FactorySettingAddr = wAddr;

}

//////////////////////////////////////////////////////////////////////////////
// <API><Description>:  Write HDCP ,DDCA or DDCD key to flash,
//                  Need set correct type of key.
//                  wIndex from 0 to (key size / ucBufSize) or (key size / ucBufSize+1)
//                  The last write wLen = (key size % ucBufSize)
//                  Will backup to free buffer if write to index = (MAX_HDCPKEYSET_NUM-1)
// <Parameter>:     -   <Flow>  -   <Description>
// ---------------------------------------------------------------------------
//  ucType          -    In     -   FLASH_KEY_HDCP, FLASH_KEY_DDCA or FLASH_KEY_DDCD
//  wIndex          -    In     -   From 0 ~ x
//  ucBufSize       -    In     -   Size of buf
//  *buf            -    In     -   point to the data of each set
//  wLen            -    In     -   Length of this time write buffer, wBufSize>=wLen
//////////////////////////////////////////////////////////////////////////////
void Flash_Write_Factory_KeySet( FLASH_KEY_TYPE ucType, WORD wIndex, WORD wBufSize, BYTE* buf, WORD wLen )
{
    BYTE xdata ucKeyIndex;
    WORD xdata wKEY_SIZE;
    WORD xdata wKEYSET_START;
    WORD xdata wKEYSET_BUF_START;
    WORD xdata wKEYSET_STATUS_ADDR;
    WORD xdata wKEYSET_BUF_STATUS_ADDR;

    ucKeyIndex = Flash_GetKeyIndex(ucType, TRUE);
    
    if(ucType == FLASH_KEY_FACTORY)
    {
        wKEY_SIZE               = FACTORY_SIZE;
        wKEYSET_START           = FACTORY_START + (WORD)ucKeyIndex*FACTORY_SIZE;
        wKEYSET_BUF_START       = FACTORY_BUF_START;
        wKEYSET_STATUS_ADDR     = FACTORY_STATUS_ADDR + ucKeyIndex;
        wKEYSET_BUF_STATUS_ADDR = FACTORY_BUF_STATUS_ADDR;
    }
    else if(ucType == FLASH_KEY_PANEL)
    {
        wKEY_SIZE               = PANEL_SIZE;
        wKEYSET_START           = PANEL_START + (WORD)ucKeyIndex*PANEL_SIZE;
        wKEYSET_BUF_START       = PANEL_BUF_START;
        wKEYSET_STATUS_ADDR     = PANEL_STATUS_ADDR + ucKeyIndex;
        wKEYSET_BUF_STATUS_ADDR = PANEL_BUF_STATUS_ADDR;
    }
#if 1    
    else if(ucType == FLASH_KEY_GAMMA)
    {
        wKEY_SIZE               = GAMMA_SIZE;
        wKEYSET_START           = GAMMA_START + (WORD)ucKeyIndex*GAMMA_SIZE;
        wKEYSET_BUF_START       = GAMMA_BUF_START;
        wKEYSET_STATUS_ADDR     = GAMMA_STATUS_ADDR + ucKeyIndex;
        wKEYSET_BUF_STATUS_ADDR = GAMMA_BUF_STATUS_ADDR;
    }
    else if(ucType == FLASH_KEY_HDCP)
    {
        wKEY_SIZE               = HDCPKEY_SIZE;
        wKEYSET_START           = HDCPKEYSET_START + (WORD)ucKeyIndex*HDCPKEY_SIZE;
        wKEYSET_BUF_START       = HDCPKEYSET_BUF_START;
        wKEYSET_STATUS_ADDR     = HDCPKEY_STATUS_ADDR + ucKeyIndex;
        wKEYSET_BUF_STATUS_ADDR = HDCPKEY_BUF_STATUS_ADDR;
    }
    else if(ucType == FLASH_KEY_DDCA)
    {
        wKEY_SIZE               = DDCAKEY_SIZE;
        wKEYSET_START           = DDCAKEYSET_START + (WORD)ucKeyIndex*DDCAKEY_SIZE;
        wKEYSET_BUF_START       = DDCAKEYSET_BUF_START;
        wKEYSET_STATUS_ADDR     = DDCAKEY_STATUS_ADDR + ucKeyIndex;;
        wKEYSET_BUF_STATUS_ADDR = DDCAKEY_BUF_STATUS_ADDR;
    }
    else if(ucType == FLASH_KEY_DDCD)
    {
        wKEY_SIZE               = DDCDKEY_SIZE;
        wKEYSET_START           = DDCDKEYSET_START + (WORD)ucKeyIndex*DDCDKEY_SIZE;
        wKEYSET_BUF_START       = DDCDKEYSET_BUF_START;
        wKEYSET_STATUS_ADDR     = DDCDKEY_STATUS_ADDR + ucKeyIndex;;
        wKEYSET_BUF_STATUS_ADDR = DDCDKEY_BUF_STATUS_ADDR;
    }
#endif    
    else if(ucType == FLASH_KEY_DDCDP)
    {
        wKEY_SIZE               = DDCDPKEY_SIZE;
        wKEYSET_START           = DDCDPKEYSET_START + (WORD)ucKeyIndex*DDCDPKEY_SIZE;
        wKEYSET_BUF_START       = DDCDPKEYSET_BUF_START;
        wKEYSET_STATUS_ADDR     = DDCDPKEY_STATUS_ADDR + ucKeyIndex;;
        wKEYSET_BUF_STATUS_ADDR = DDCDPKEY_BUF_STATUS_ADDR;
    }
    else
        return;

    bFlashWriteFactory = TRUE;

    if( FlashReadByte( wKEYSET_STATUS_ADDR ) == FLASH_KEY_EMPTY || FlashReadByte( wKEYSET_STATUS_ADDR ) == FLASH_KEY_WRITE )
    {
        if( wIndex == 0x00 )
        {
            // FLASH_printData("  _FWFK_StatusAddr [%x] ",wKEYSET_STATUS_ADDR);
             FlashWriteByte(TRUE, wKEYSET_STATUS_ADDR, FLASH_KEY_WRITE);
        }
        Flash_WriteTbl( TRUE, wKEYSET_START + wIndex * wBufSize, buf, wLen );
        if(( wIndex * wBufSize + wLen ) >= wKEY_SIZE ) // check write end
        {
            FlashWriteByte( TRUE, wKEYSET_STATUS_ADDR, FLASH_KEY_VALID );
            if(ucKeyIndex>0)
                FlashWriteByte(TRUE, wKEYSET_STATUS_ADDR-1, FLASH_KEY_INVALID);
        }
    }
    else
    {
        if( wIndex == 0x00 )
        {
            FlashSectorErase(TRUE, FLASH_FreeBufferAddr);
            FlashWriteByte(TRUE, wKEYSET_BUF_STATUS_ADDR, FLASH_KEY_WRITE);
        }

        Flash_WriteTbl( TRUE, wKEYSET_BUF_START + wIndex * wBufSize, buf, wLen );
        if(( wIndex * wBufSize + wLen ) >= wKEY_SIZE ) // check write end
        {
            FlashWriteByte( TRUE, wKEYSET_BUF_STATUS_ADDR, FLASH_KEY_VALID );
            //FLASH_printData("  _FWFK_buffer write End [%x] ",ucType);
            Flash_BackupFactorySetToBuffer();
            FlashSectorErase( TRUE, FLASH_FreeBufferAddr );
            Flash_BackupFactorySetToBuffer();
            FlashSectorErase( TRUE, FLASH_FreeBufferAddr );    //charles test
        }

    }
    bFlashWriteFactory = FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// <API><Description>:  Read HDCP ,DDCA or DDCD key each wLen from flash,
//                  Need set correct ucType.
//                  wIndex from 0 to (key size / wBufSize) or (key size / wBufSize+1)
//                  The last read wLen = (key size % wBufSize)
// <Returns> : return TRUE when found key ident and CheckSum
// <Parameter>:     -   <Flow>  -   <Description>
// ---------------------------------------------------------------------------
//  ucType          -    In     -   FLASH_KEY_HDCP, FLASH_KEY_DDCA or FLASH_KEY_DDCD
//  wIndex          -    In     -   From 0 ~ x
//  wBufSize        -    In     -   Size of buf
//  *buf            -    In     -   point to the data of each set
//  wLen            -    In     -   Length of this time write buffer, wBufSize>=wLen
//////////////////////////////////////////////////////////////////////////////
Bool Flash_Read_Factory_KeySet( FLASH_KEY_TYPE ucType, Bool bReadVaild, WORD wIndex, WORD wBufSize, BYTE* buf, WORD wLen )
{
    BYTE xdata ucKeyIndex;
    WORD xdata wKEY_SIZE;
    WORD xdata wKEYSET_START;
    WORD xdata wKEYSET_STATUS_ADDR;

    if(bReadVaild)
        ucKeyIndex = Flash_GetKeyIndex(ucType, FALSE);
    else
        ucKeyIndex = Flash_GetKeyIndex(ucType, TRUE);

    if( ucType == FLASH_KEY_FACTORY )
    {
        wKEY_SIZE           = FACTORY_SIZE;
        wKEYSET_START       = FACTORY_START + (WORD)ucKeyIndex*FACTORY_SIZE;
        wKEYSET_STATUS_ADDR = FACTORY_STATUS_ADDR + ucKeyIndex;
    }
    else if( ucType == FLASH_KEY_PANEL )
    {
        wKEY_SIZE           = PANEL_SIZE;
        wKEYSET_START       = PANEL_START + (WORD)ucKeyIndex*PANEL_SIZE;
        wKEYSET_STATUS_ADDR = PANEL_STATUS_ADDR + ucKeyIndex;
    }
#if 1    
    else if( ucType == FLASH_KEY_GAMMA )
    {
        wKEY_SIZE           = GAMMA_SIZE;
        wKEYSET_START       = GAMMA_START + (WORD)ucKeyIndex*GAMMA_SIZE;
        wKEYSET_STATUS_ADDR = GAMMA_STATUS_ADDR + ucKeyIndex;
    }
    else if(ucType == FLASH_KEY_HDCP)
    {
        wKEY_SIZE           = HDCPKEY_SIZE;
        wKEYSET_START       = HDCPKEYSET_START + (WORD)ucKeyIndex*HDCPKEY_SIZE;
        wKEYSET_STATUS_ADDR = HDCPKEY_STATUS_ADDR + ucKeyIndex;
    }
    else if( ucType == FLASH_KEY_DDCA )
    {
        wKEY_SIZE           = DDCAKEY_SIZE;
        wKEYSET_START       = DDCAKEYSET_START + (WORD)ucKeyIndex*DDCAKEY_SIZE;;
        wKEYSET_STATUS_ADDR = DDCAKEY_STATUS_ADDR + ucKeyIndex;
    }
    else if( ucType == FLASH_KEY_DDCD )
    {
        wKEY_SIZE       = DDCDKEY_SIZE;
        wKEYSET_START       = DDCDKEYSET_START + (WORD)ucKeyIndex*DDCDKEY_SIZE;;
        wKEYSET_STATUS_ADDR = DDCDKEY_STATUS_ADDR + ucKeyIndex;
    }
#endif    
    else if( ucType == FLASH_KEY_DDCDP )
    {
        wKEY_SIZE       = DDCDPKEY_SIZE;
        wKEYSET_START       = DDCDPKEYSET_START + (WORD)ucKeyIndex*DDCDPKEY_SIZE;;
        wKEYSET_STATUS_ADDR = DDCDPKEY_STATUS_ADDR + ucKeyIndex;
    }
    else
        return FALSE;

    if( FlashReadByte( wKEYSET_STATUS_ADDR ) == FLASH_KEY_WRITE || FlashReadByte( wKEYSET_STATUS_ADDR ) == FLASH_KEY_VALID )
    {
        //FLASH_printData("  Flash_ReadDDCAKeySet Succ [%x]",wKEYSET_START+i*wKEY_SIZE+1+wIndex*wBufSize);
        if(bReadVaild && FlashReadByte(wKEYSET_STATUS_ADDR) != FLASH_KEY_VALID)
            return FALSE;
        else
            Flash_ReadTbl(wKEYSET_START+wIndex*wBufSize, buf, wLen);
        
        return TRUE;
    }
    else
    {
        // FLASH_printData("  Flash_ReadKeySet Fail ucType[%x h]",ucType);
        return FALSE;
    }
}

//////////////////////////////////////////////////////////////////////////////
// <API><Description>:  Write Factory setting to flash,
//                  It will exchange buffer when not found empty space
//////////////////////////////////////////////////////////////////////////////
void Flash_WriteFactorySet( BYTE* pbuf, BYTE ucLen )
{
    Flash_Write_Factory_KeySet( FLASH_KEY_FACTORY, 0, FACTORY_SIZE, pbuf, ucLen );
}
//////////////////////////////////////////////////////////////////////////////
// <API><Description>:  Read latest factory setting struct from flash.
// <Returns> : return TRUE when found factory setting
//////////////////////////////////////////////////////////////////////////////
Bool Flash_ReadFactorySet( BYTE* pbuf, BYTE ucLen )
{
    return Flash_Read_Factory_KeySet( FLASH_KEY_FACTORY, TRUE, 0, FACTORY_SIZE, pbuf, ucLen );
}
//////////////////////////////////////////////////////////////////////////////
// <API><Description>:  Write Monitor setting struct to flash,
//                  It will exchange buffer when buffer full
//////////////////////////////////////////////////////////////////////////////
void Flash_WriteMonitorSet( BYTE* buf, BYTE len )
{
    WORD wAddr;
    WORD i;

    i = Flash_SearchIdentChecksum( MONITORSET_START, 0, MAX_MONITORSET_NUM - 1, MONITORSET_SIZE, FLASH_EMPTYDATA, FLASH_EMPTYDATA );
    if( i != IDENT_NOT_FOUND ) // Found the empty index
    {
        //MonitorSetting.Ident= FLASH_IDENTDATA;
        //MonitorSetting.CheckSum   = FLASH_CHECKSUM;
        wAddr = MONITORSET_START + i * MONITORSET_SIZE;
        Flash_WriteTbl( TRUE, wAddr, ( BYTE* )buf, len );
        FLASH_printData( "_FWMS_ wAddr %x", wAddr );
        if( i > 0 )
        {
            wAddr = MONITORSET_START + ( i - 1 ) * MONITORSET_SIZE;
            FlashWriteByte( TRUE, wAddr, FLASH_IDENTDATA_CLEAR );
        }
    }
    else
    {
        FlashSectorErase( TRUE, FLASH_FreeBufferAddr );
        Flash_WriteTbl( TRUE, MONITORSET_BUF_START, ( BYTE* )buf, len );
        FLASH_printData( "Exchange_FWMS_ wAddr %x", MONITORSET_BUF_START );
        FlashWriteByte( TRUE, FLASH_FreeBufferAddr, FBufType_MonitorSetting );
        FlashWriteByte( TRUE, FLASH_MonitorSettingAddr, 0x00 );

        wAddr = FLASH_FreeBufferAddr;
        FLASH_FreeBufferAddr = FLASH_MonitorSettingAddr;
        FLASH_MonitorSettingAddr = wAddr;

    }
}
//////////////////////////////////////////////////////////////////////////////
// <API><Description>:  Read latest Monitor setting struct from flash.
//                  1.Got monitor setting index by check flash identity
//                  2.If not found read CheckSum to got correct one
//                  3.read the Monitor setting struct
// <Returns> : return TRUE when found MonitorSetting
//////////////////////////////////////////////////////////////////////////////
Bool Flash_ReadMonitorSet( BYTE* buf, BYTE len )
{
    WORD i;

    i = Flash_SearchIdentChecksum( MONITORSET_START, ( MAX_MONITORSET_NUM - 1 ), 0, MONITORSET_SIZE, FLASH_IDENTDATA, FLASH_CHECKSUM );
    if( i != IDENT_NOT_FOUND )
    {
        Flash_ReadTbl( MONITORSET_START + i * MONITORSET_SIZE, buf, len );
        FLASH_printData( "_FRMS_ wAddr %x", MONITORSET_START + i * MONITORSET_SIZE );
        return TRUE;
    }
    else
    {
        i = Flash_SearchIdentChecksum( MONITORSET_START, ( MAX_MONITORSET_NUM - 1 ), 0, MONITORSET_SIZE, FLASH_IDENTDATA_CLEAR, FLASH_CHECKSUM );
        if( i != IDENT_NOT_FOUND )
        {
            Flash_ReadTbl( MONITORSET_START + i * MONITORSET_SIZE, buf, len );
            //   FLASH_printMsg("_FRMS_ Only Checksum Correct! ");
            //  FLASH_printData("_FRMS_ wAddr %x",MONITORSET_START + i*MONITORSET_SIZE);
            return TRUE;
        }
        else
            return FALSE;
    }
}


//////////////////////////////////////////////////////////////////////////////
// <API><Description>:  Flash_WriteMonitorSet2 isues to save MonitorSetting2
//                  It will exchange buffer when buffer full
//////////////////////////////////////////////////////////////////////////////
void Flash_WriteMonitorSet2( BYTE* buf, BYTE len )
{
    WORD wAddr;
    WORD i;

    i = Flash_SearchIdentChecksum( MONITORSET2_START, 0, MAX_MONITORSET2_NUM - 1, MONITORSET2_SIZE, FLASH_EMPTYDATA, FLASH_EMPTYDATA );
    if( i != IDENT_NOT_FOUND )
    {
        wAddr = MONITORSET2_START + i * MONITORSET2_SIZE;
        Flash_WriteTbl( TRUE, wAddr, ( BYTE* )buf, len );
        //FLASH_printData("_FWMS2_ wAddr %x",wAddr);
    }
    else
    {
        FlashSectorErase( TRUE, FLASH_FreeBufferAddr );
        Flash_WriteTbl( TRUE, MONITORSET2_BUF_START, ( BYTE* )buf, len );
        //FLASH_printData("_FWMS2_ wAddr %x",MONITORSET2_BUF_START);
        FlashWriteByte( TRUE, FLASH_FreeBufferAddr, FBufType_MonitorSetting2 );
        FlashWriteByte( TRUE, FLASH_MonitorSetting2Addr, 0x00 );
        wAddr = FLASH_FreeBufferAddr;
        FLASH_FreeBufferAddr = FLASH_MonitorSetting2Addr;
        FLASH_MonitorSetting2Addr = wAddr;
    }
}

//////////////////////////////////////////////////////////////////////////////
// <API><Description>:  Read MonitorSetting2 struct from flash.
//                  1.Got monitor setting index by check flash identity
// <Returns> : return TRUE when found MonitorSetting2
//////////////////////////////////////////////////////////////////////////////
Bool Flash_ReadMonitorSet2( BYTE* buf, BYTE len )
{
    Bool bFound = FALSE;
    WORD i;

    i = Flash_SearchIdentChecksum( MONITORSET2_START, ( MAX_MONITORSET2_NUM - 1 ), 0, MONITORSET2_SIZE, FLASH_IDENTDATA, FLASH_CHECKSUM );
    if( i != IDENT_NOT_FOUND )
    {
        Flash_ReadTbl( MONITORSET2_START + i * MONITORSET2_SIZE, buf, len );
        //FLASH_printData("_FRMS2_ wAddr %x",MONITORSET2_START + i*MONITORSET2_SIZE);
        return TRUE;
    }
    else
    {
        FLASH_printMsg( "  Flash_ReadMonitorSet2 Fail" );
        return FALSE;
    }
}


//////////////////////////////////////////////////////////////////////////////
// <API><Description>:  Check exit timing mode number and compare to MAX_MODESET_NUM.
// <Returns> : return TRUE when saved timing mode size equal to MAX_MODESET_NUM
//////////////////////////////////////////////////////////////////////////////
Bool Flash_CheckModeFull( void )
{
    BYTE i, wExitModeCount = 0;
    //check buffer full
    for( i = 0; i < MAX_MODESET_NUM; i++ )
    {
        if( FlashReadByte( MODESET_START + i * MODESET_SIZE ) == FLASH_IDENTDATA && FlashReadByte( MODESET_START + i * MODESET_SIZE + MODESET_SIZE - 1 ) == FLASH_CHECKSUM )
            wExitModeCount++;
    }
    if( wExitModeCount == MAX_MODESET_NUM )
        return TRUE;
    else
        return FALSE;
}
//////////////////////////////////////////////////////////////////////////////
// <API><Description>:  Clear all user mode ident.
//
//////////////////////////////////////////////////////////////////////////////
void Flash_ClearUserModeSpace( void )
{
    BYTE i, ucIdent = 0;
    //check buffer full
    for( i = 0; i < MAX_MODESET_NUM; i++ )
    {
        ucIdent = FlashReadByte( MODESET_START + i * MODESET_SIZE );
        if( ucIdent == FLASH_IDENTDATA )
        {
            if(( FlashReadByte( MODESET_START + i * MODESET_SIZE + 1 ) & 0x80 ) == 0x80 )
                FlashWriteByte( TRUE, MODESET_START + i * MODESET_SIZE, 0x00 ); // Clear Identdata.
        }
    }
    FLASH_printMsg( "Flash_ClearUserModeSpace" );
}

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
void Flash_WriteModeSet( BYTE index, BYTE *buf )
{
    WORD i = 0, j = 0;
    WORD wAddr;
    xdata ModeSettingType modeSetting;

    //FLASH_printData("_FWMS_ Target index 0x%x",index);
    // find orginal mode index in flash
    for( i = 0; i < MAX_MODESET_NUM; i++ )
    {
        if( FlashReadByte( MODESET_START + i * MODESET_SIZE ) == FLASH_IDENTDATA && FlashReadByte( MODESET_START + i * MODESET_SIZE + MODESET_SIZE - 1 ) == FLASH_CHECKSUM )
        {
            if( FlashReadByte( MODESET_START + i * MODESET_SIZE + 1 ) == index )
            {
                FlashWriteByte( TRUE, MODESET_START + i * MODESET_SIZE, 0x00 ); // Clear Identdata.
                FLASH_printData( "_FWMS_ Clear index %x", i );
            }
        }
    }

    // find empty position
    i = Flash_SearchIdentChecksum( MODESET_START, 0, ( MAX_MODESET_NUM - 1 ), MODESET_SIZE, FLASH_EMPTYDATA, FLASH_EMPTYDATA );
    if( i != IDENT_NOT_FOUND )
    {
        FLASH_printData( "_FWMS_ write to empty Addr %x", MODESET_START + i * MODESET_SIZE );
        Flash_WriteTbl( TRUE, MODESET_START + i * MODESET_SIZE, buf, MODESET_SIZE );
    }
    else
    {
        if( Flash_CheckModeFull() )
            Flash_ClearUserModeSpace();

        FlashSectorErase( TRUE, FLASH_FreeBufferAddr );
        for( i = 0; i < MAX_MODESET_NUM; i++ )
        {
            if( FlashReadByte( MODESET_START + i * MODESET_SIZE ) == FLASH_IDENTDATA  && FlashReadByte( MODESET_START + i * MODESET_SIZE + MODESET_SIZE - 1 ) == FLASH_CHECKSUM )
            {
                Flash_ReadTbl( MODESET_START + i * MODESET_SIZE, ( BYTE* )&modeSetting, MODESET_SIZE );
                Flash_WriteTbl( TRUE, MODESET_BUF_START + j * MODESET_SIZE, ( BYTE* )&modeSetting, MODESET_SIZE );
                j++;
            }
        }
        if( j < ( MAX_MODESET_NUM - 1 ) )
        {
            FLASH_printData( "_FWMS_ write to empty Addr %x", MODESET_BUF_START + j * MODESET_SIZE );
            Flash_WriteTbl( TRUE, MODESET_BUF_START + j * MODESET_SIZE, buf, MODESET_SIZE );
        }

        //  Exchange Buffer
        FlashWriteByte( TRUE, FLASH_FreeBufferAddr, FBufType_TimingMode );
        FlashWriteByte( TRUE, FLASH_TimingModeAddr, 0x00 );
        wAddr = FLASH_FreeBufferAddr;
        FLASH_FreeBufferAddr = FLASH_TimingModeAddr;
        FLASH_TimingModeAddr = wAddr;
        //i=j;//Write empty one index

        FLASH_printData( "_FWMS_ Buffer exchange TimingAddr %x ", ( WORD )( FLASH_TimingModeAddr >> 8 ) );
        FLASH_printData( "_FWMS_ Buffer exchange FreeAddr %x ", ( WORD )( FLASH_FreeBufferAddr >> 8 ) );
    }
}


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
Bool Flash_ReadModeSet( BYTE index, BYTE *buf )
{
    WORD i;

    for( i = 0; i < MAX_MODESET_NUM; i++ )
    {
        if( FlashReadByte( MODESET_START + i * MODESET_SIZE ) == FLASH_IDENTDATA
                && FlashReadByte( MODESET_START + i * MODESET_SIZE + MODESET_SIZE - 1 ) == FLASH_CHECKSUM
                && FlashReadByte( MODESET_START + i * MODESET_SIZE + 1 ) == index )
        {
            break;
        }
    }

    if( i >= MAX_MODESET_NUM )
    {
        FLASH_printMsg( "_FRMS_ RModeSet Not Found" );
        return FALSE;
    }

    FLASH_printData( "_FRMS_ RModeSet=%x", i );
    FLASH_printData( "_FRMS_ ModeR=%x", ( MODESET_START + i * MODESET_SIZE ) & 0xFFFF );
    Flash_ReadTbl( MODESET_START + i * MODESET_SIZE, buf, MODESET_SIZE );
    return TRUE;
}

#if USEFLASH_REDUCE_STACK_WRITE_FDATA
Bool FlashFData_GetWriteKeyAddress(FLASH_KEY_TYPE ucType, Bool bWrite)
{
    Bool bresult = TRUE;
    BYTE ucStatus, ucSize;
    WORD wdatasize;

    switch (ucType)
    {
        case FLASH_KEY_FACTORY:
        {
            ucSize = FACTORY_STATUS_SIZE;
            wdatasize = FACTORY_SIZE;
            FlashFData_wKEYSET_STATUS_ADDR  = FACTORY_STATUS_ADDR;
            FlashFData_wKEYSET_START = FACTORY_START;
            break;
        }
#if INPUT_TYPE != INPUT_1A
        case FLASH_KEY_HDCP:
        {
            ucSize = HDCPKEY_STATUS_SIZE;
            wdatasize = HDCPKEY_SIZE;
            FlashFData_wKEYSET_STATUS_ADDR  = HDCPKEY_STATUS_ADDR;
            FlashFData_wKEYSET_START = HDCPKEYSET_START;
            break;
        }
#endif        
#if ENABLE_DP_INPUT        
        case FLASH_KEY_DDCDP:
        {
            ucSize = DDCDPKEY_STATUS_SIZE;
            wdatasize = DDCDPKEY_SIZE;
            FlashFData_wKEYSET_STATUS_ADDR  = DDCDPKEY_STATUS_ADDR;
            FlashFData_wKEYSET_START = DDCDPKEYSET_START;
            break;
        }
#endif        

        default:
            break;
        
    }

    for(FlashFData_ucKeyIndex=0; FlashFData_ucKeyIndex<ucSize; FlashFData_ucKeyIndex++)
    {
        ucStatus = FlashReadByte(FlashFData_wKEYSET_STATUS_ADDR+FlashFData_ucKeyIndex);
        if(bWrite)
        {
            if( ucStatus == FLASH_KEY_EMPTY)
                break;
        }
        else
        {
            if( ucStatus == FLASH_KEY_VALID)
                break;
        }
    }

    if( FlashFData_ucKeyIndex >= ucSize )
    {
        bresult = FALSE;
    }

    FlashFData_wKEYSET_START += ((WORD)FlashFData_ucKeyIndex*wdatasize);
    FlashFData_wKEYSET_STATUS_ADDR += FlashFData_ucKeyIndex;

    if(bresult == TRUE)
    {
        bFlashWriteFactory = TRUE;
        FlashDisableWP(FlashFData_wKEYSET_START);        
    }

    return bresult;
}

Bool FlashFData_BackupToFreeBuffer(FLASH_KEY_TYPE ucKeyType)
{
    BYTE ucKeyIndex; 
    WORD wStartAddr, wStatusAddr, wKeySize;
    WORD wBufStatusAddr = ucKeyType;

    ucKeyIndex = Flash_GetKeyIndex(ucKeyType, FALSE);

    switch (ucKeyType)
    {
        case FLASH_KEY_FACTORY:
        {
            wStatusAddr = FACTORY_STATUS_ADDR + ucKeyIndex;
            wStartAddr = FACTORY_START + (WORD)ucKeyIndex*FACTORY_SIZE;
            wKeySize = FACTORY_SIZE;
            break;
        }
#if INPUT_TYPE != INPUT_1A
        case FLASH_KEY_HDCP:
        {
            wStatusAddr = HDCPKEY_STATUS_ADDR + ucKeyIndex;
            wStartAddr = HDCPKEYSET_START + (WORD)ucKeyIndex*HDCPKEY_SIZE;
            wKeySize = HDCPKEY_SIZE;
            break;
        }
#endif        
#if ENABLE_DP_INPUT        
        case FLASH_KEY_DDCDP:
        {
            wStatusAddr = DDCDPKEY_STATUS_ADDR + ucKeyIndex;
            wStartAddr = DDCDPKEYSET_START + (WORD)ucKeyIndex*DDCDPKEY_SIZE;
            wKeySize = DDCDPKEY_SIZE;
            break;
        }
#endif        

        default:
            break;
        
    } 
    
    // prevent different sector data move to current sector
    if((wStartAddr&0xF000) != (FlashFData_wKEYSET_START&0xF000))
        return FALSE;

    wBufStatusAddr = FLASH_FreeBufferAddr+wBufStatusAddr*0x200;

    if (FlashReadByte(wStatusAddr) == FLASH_KEY_VALID)
    {
        Flash_MoveTbl(TRUE, wStartAddr, wBufStatusAddr+0x10, wKeySize);
        FlashWriteByte(TRUE, wBufStatusAddr, FLASH_KEY_VALID);
        return TRUE;
    }
    
    return FALSE;

}

Bool FlashFData_BackupFromFreeBuffer(FLASH_KEY_TYPE ucKeyType)
{
    WORD wStartAddr, wStatusAddr, wKeySize;
    WORD wBufStatusAddr = ucKeyType;

    wBufStatusAddr = FLASH_FreeBufferAddr+wBufStatusAddr*0x200;

    switch (ucKeyType)
    {
        case FLASH_KEY_FACTORY:
        {
            wStatusAddr = FACTORY_STATUS_ADDR;
            wStartAddr = FACTORY_START;
            wKeySize = FACTORY_SIZE;
            break;
        }
#if INPUT_TYPE != INPUT_1A
        case FLASH_KEY_HDCP:
        {
            wStatusAddr = HDCPKEY_STATUS_ADDR;
            wStartAddr = HDCPKEYSET_START;
            wKeySize = HDCPKEY_SIZE;
            break;
        }
#endif        
#if ENABLE_DP_INPUT        
        case FLASH_KEY_DDCDP:
        {
            wStatusAddr = DDCDPKEY_STATUS_ADDR;
            wStartAddr = DDCDPKEYSET_START;
            wKeySize = DDCDPKEY_SIZE;
            break;
        }
#endif        

        default:
            break;
        
    } 
    
    if (FlashReadByte(wBufStatusAddr) == FLASH_KEY_VALID)
    {
        bFlashWriteFactory = TRUE;
        Flash_MoveTbl(TRUE, wBufStatusAddr+0x10, wStartAddr, wKeySize);
        FlashWriteByte(TRUE, wStatusAddr, FLASH_KEY_VALID);
        bFlashWriteFactory = FALSE;
        return TRUE;
    }    

    return FALSE;
}

void FlashFData_WriteBufferType(WORD addr)
{
    BYTE ucBufferType;

    if(addr < FDATA_KEY_ADDR)
        return;

    ucBufferType = FlashReadByte(addr);
    bFlashWriteFactory = TRUE;
    FlashSectorErase(TRUE, FlashFData_wKEYSET_START);
    FlashWriteByte(TRUE, addr, ucBufferType);
    bFlashWriteFactory = FALSE;
}

void FlashFData_WriteKeyValidByte(void)
{
    bFlashWriteFactory = TRUE;
    FlashWriteByte(TRUE, FlashFData_wKEYSET_STATUS_ADDR, FLASH_KEY_VALID);
    if(FlashFData_ucKeyIndex)
        FlashWriteByte(TRUE, FlashFData_wKEYSET_STATUS_ADDR-1, FLASH_KEY_INVALID);
    FlashEnableWP();
    bFlashWriteFactory = FALSE;
}

//////////////////////////////////////////////////////
/*                              how to use

step 1: get start address of factory area data (0x3A000~0x3B000)
           for FlashFData_wKEYSET_START, 
           FlashFData_wKEYSET_STATUS_ADDR, 
           FlashFData_ucKeyIndex
step 2: write HDCP-key data to flash

if(FACTORY_SEND_HDCP_KEY_START) // step 1
{
    if(FlashFData_GetWriteKeyAddress(FLASH_KEY_HDCP, TRUE) == FALSE)
    {
        FlashSectorErase(TRUE, FLASH_FreeBufferAddr);
        FlashFData_BackupToFreeBuffer(FLASH_KEY_DDCDP);
        FlashSectorErase(TRUE, FlashFData_wKEYSET_START);
        FlashFData_BackupFromFreeBuffer(FLASH_KEY_DDCDP);
        FlashFData_WriteBufferType(FLASH_HDCP_EDIDAddr);
    }
}
else if(FACTORY_SEND_HDCP_KEY_DATA) // step 2
{
    if(HDCP_KEY_LENGTH < 304)
    {
        
    }
    else if(HDCP_KEY_LENGTH >= 304)
    {
        FlashFData_WriteKeyValidByte();
    }
}



*/
/////////////////////////////////////////////////////

#endif



#endif//USEFLASH
