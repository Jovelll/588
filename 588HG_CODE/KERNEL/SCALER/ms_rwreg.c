///////////////////////////////////////////////////////////////////////////////
/// @file ms_rwreg.h
/// @brief MStar Scaler register access.
/// @author MStarSemi Inc.
///
/// Functions for MStar scaler register access.
///
/// Features
///  -Write byte/word/bulk data
///  -Read byte/word data.
///  -
///  -
///////////////////////////////////////////////////////////////////////////////
#define _MS_RWREG_C

#include <intrins.h>
#include "types.h"
#include "board.h"
#include "ms_reg.h"
#include "misc.h"
#include "ms_rwreg.h"

#if ENABLE_SW_DOUBLE_BUFFER

#define DB_DEBUG    1 // default 1 to show SWDB error msg, i.e. timeout/full.
#if ENABLE_DEBUG&&DB_DEBUG
#define DB_DEBUG_printData(str, value)   printData(str, value)
#define DB_DEBUG_printMsg(str)           printMsg(str)
#else
#define DB_DEBUG_printData(str, value)
#define DB_DEBUG_printMsg(str)
#endif

enum
{
    SW_DB_SW_TRIG,
    SW_DB_OVSYNC_TRIG,
    SW_DB_IVSYNC_TRIG,
    SW_DB_OVDE_TRIG,
};

#if (CHIP_ID == CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK) || (CHIP_ID == CHIP_TSUMD) || (CHIP_ID == CHIP_TSUMJ) || (CHIP_ID == CHIP_TSUM9) || (CHIP_ID == CHIP_TSUMF)
XDATA StuSWDBCtrl SWDBCtrl _at_ XDATA_SWDB_ADDR_START;
#else
XDATA StuSWDBCtrl SWDBCtrl _at_ (0x45FF-sizeof(StuSWDBCtrl));
#endif

#if (CHIP_ID == CHIP_TSUMF)
#define SW_DB_TRIG_MODE     SW_DB_OVSYNC_TRIG
#else
#define SW_DB_TRIG_MODE     SW_DB_OVDE_TRIG
#endif

void msSWDBInit(void)
{
    SWDBCtrl.ucSWDB_Mode = eDB_NORMAL_MODE;
    SWDBCtrl.ucSWDB_Mode_Log = eDB_NORMAL_MODE;
    SWDBCtrl.ucSWDB_Num = 1;
    SWDBCtrl.ucSWDB_Counter = 0;
    SWDBCtrl.ucSWDB_OnFire = 0;
    SWDBCtrl.ucSWDB_LastSCBank = 0xFF;
    msWriteByteMask(REG_3C25,SW_DB_TRIG_MODE<<4,BIT5|BIT4);
    msWrite2ByteMask(REG_3C24,((WORD)&SWDBCtrl.ucSWDB_Buffer[0])&0x7FF,0x07FF);
    msWriteBit(REG_2B00,0,BIT5);
}

void msSWDBWaitForRdy(void)
{
    XDATA BYTE dlycnt=60;
    while(SWDBCtrl.ucSWDB_OnFire)
    {
        dlycnt--;
        if(!dlycnt)
        {
            DB_DEBUG_printMsg("ML Time out, switch to SW Trig Mode !!");
            msWriteByteMask(REG_3C25,SW_DB_SW_TRIG<<4,BIT5|BIT4);
            msWriteByteMask(REG_3C25,BIT6,BIT6);
            while(SWDBCtrl.ucSWDB_OnFire);
            msWriteByteMask(REG_3C25,0x00,0xC0);
            msWriteByteMask(REG_3C25,SW_DB_TRIG_MODE<<4,BIT5|BIT4);
            SWDBCtrl.ucSWDB_Counter = 0;
            SWDBCtrl.ucSWDB_OnFire = 0;
            break;
        }
        ForceDelay1ms(1);
    }
}

void msSWDBWriteToRegister(void)
{
    XDATA BYTE index;

    msSWDBWaitForRdy();

    if(SWDBCtrl.ucSWDB_Counter)
    {
        index = SWDBCtrl.ucSWDB_Counter;
        msWrite2ByteMask(REG_3C26,((WORD)&SWDBCtrl.ucSWDB_Buffer[index-1])&0x7FF,0x07FF);
        SWDBCtrl.ucSWDB_OnFire = 1;
        msWriteByte(REG_3C25,msReadByte(REG_3C25)|0xC0);
    }
}

#if CHIP_ID==CHIP_TSUMF // TSUMF no VDE end trig source, instead by OSD_VDE_End sw polling & trigger
void msSWDBWriteToRegisterByOsdEnd(void)
{
    XDATA BYTE index;
    WORD retry = 0xffff;

    msSWDBWaitForRdy();

    if(SWDBCtrl.ucSWDB_Counter)
    {
        index = SWDBCtrl.ucSWDB_Counter;
        msWrite2ByteMask(REG_3C26,((WORD)&SWDBCtrl.ucSWDB_Buffer[index-1])&0x7FF,0x07FF);
        msWriteByteMask(REG_3C25,SW_DB_SW_TRIG<<4,BIT5|BIT4);

        // wait OSD VDE end
        while( retry-- && ( MEM_MSREAD_BYTE(REG_2B24)&BIT0 ) && !InputTimingChangeFlag );
        retry = 0xffff;
        while( retry-- && !( MEM_MSREAD_BYTE(REG_2B24)&BIT0 ) && !InputTimingChangeFlag );

        // SWDB sw trigger
        MEM_MSWRITE_BYTE(REG_3C25,MEM_MSREAD_BYTE(REG_3C25) | 0xC0);

        while(SWDBCtrl.ucSWDB_OnFire);
        msWriteByteMask(REG_3C25,0x00,0xC0);
        msWriteByteMask(REG_3C25,SW_DB_TRIG_MODE<<4,BIT5|BIT4);
        SWDBCtrl.ucSWDB_Counter = 0;
        SWDBCtrl.ucSWDB_OnFire = 0;
    }
}
#endif

void msSWDBMode(DB_TYPE type)
{
    SWDBCtrl.ucSWDB_Mode = type;
}

static BOOL msSWDBNewModeBufChk(WORD wReg)
{
    BOOL NewMode = FALSE;
    XDATA BYTE size = 4;

    if((SWDBCtrl.ucSWDB_Mode_Log != SWDBCtrl.ucSWDB_Mode)
       || (SWDBCtrl.ucSWDB_Counter == 0))
    {
        NewMode = TRUE;
    }
    else
    {
        switch(SWDBCtrl.ucSWDB_Mode)
        {
            case eDB_SUCCESSIVE_MODE:
                if((wReg>>8) != (SWDBCtrl.ucSWDB_Buffer[SWDBCtrl.ucSWDB_Num - 1]&0x3F))
                    NewMode = TRUE;
                break;

            case eDB_BURST_MODE:
                if(((wReg>>8) != (SWDBCtrl.ucSWDB_Buffer[SWDBCtrl.ucSWDB_Num - 1]&0x3F))
                   || ((wReg&0xFF) != (SWDBCtrl.ucSWDB_Buffer[SWDBCtrl.ucSWDB_Num + 1])))
                   NewMode = TRUE;
                break;

            default:
                break;
        }
    }

    // if buffer size is not large enough then write to register first
    if(SWDBCtrl.ucSWDB_Mode == eDB_NORMAL_MODE)
        size = 3;
    else if(SWDBCtrl.ucSWDB_Mode == eDB_SUCCESSIVE_MODE)
        size = NewMode?4:2;
    else if(SWDBCtrl.ucSWDB_Mode == eDB_BURST_MODE)
        size = NewMode?4:1;

    if( (SWDBCtrl.ucSWDB_Counter+size) >= SWDB_MAX_SIZE )
    {
        DB_DEBUG_printMsg("SWDB buffer is Full !!");
        msSWDBWriteToRegister();
        msSWDBWaitForRdy();
        NewMode = TRUE;
    }

    if(NewMode)
        SWDBCtrl.ucSWDB_Num = SWDBCtrl.ucSWDB_Counter + 1;

    return NewMode;
}
void msSWDBWriteByte(WORD wReg,BYTE ucValue)
{
    XDATA BYTE index, SWDBModeChg;
    XDATA BYTE SCBank,SCAddr,SCVal;

    msSWDBWaitForRdy();
    if( wReg&_BIT15 ) // Scaler bank
    {
        SCBank = (wReg>>8)&0x7F;
        SCAddr = (wReg&0xFF);
        SCVal = ucValue;
        if(SWDBCtrl.ucSWDB_LastSCBank!= SCBank)
        {
            SWDBCtrl.ucSWDB_LastSCBank = SCBank;
            msSWDBWriteByte(0x2F00,SCBank);
            msSWDBWriteByte(0x2F00|SCAddr,SCVal);
        }
        else
            msSWDBWriteByte(0x2F00|SCAddr,SCVal);
    }
    else
    {
        SWDBModeChg = msSWDBNewModeBufChk(wReg);
        index  = SWDBCtrl.ucSWDB_Counter;

        switch(SWDBCtrl.ucSWDB_Mode)
        {
            case eDB_NORMAL_MODE:
                SWDBCtrl.ucSWDB_Mode_Log = SWDBCtrl.ucSWDB_Mode;
                SWDBCtrl.ucSWDB_Buffer[index] = (wReg>>8)|eDB_NORMAL_MODE;
                SWDBCtrl.ucSWDB_Buffer[index+1] = (wReg&0xFF);
                SWDBCtrl.ucSWDB_Buffer[index+2] = ucValue;
                SWDBCtrl.ucSWDB_Counter += 3;
                break;

            case eDB_SUCCESSIVE_MODE:
                if(SWDBModeChg)
                {
                    SWDBCtrl.ucSWDB_Mode_Log = SWDBCtrl.ucSWDB_Mode;
                    SWDBCtrl.ucSWDB_Buffer[SWDBCtrl.ucSWDB_Num - 1] = (wReg>>8)|eDB_SUCCESSIVE_MODE;
                    SWDBCtrl.ucSWDB_Buffer[SWDBCtrl.ucSWDB_Num] = 0;
                    SWDBCtrl.ucSWDB_Buffer[index + 2] = (wReg&0xFF);
                    SWDBCtrl.ucSWDB_Buffer[index + 3] = ucValue;
                    SWDBCtrl.ucSWDB_Counter += 4;
                }
                else
                {
                    SWDBCtrl.ucSWDB_Buffer[SWDBCtrl.ucSWDB_Num - 1] = (wReg>>8)|eDB_SUCCESSIVE_MODE;
                    SWDBCtrl.ucSWDB_Buffer[SWDBCtrl.ucSWDB_Num]++;
                    SWDBCtrl.ucSWDB_Buffer[index ] = (wReg&0xFF);
                    SWDBCtrl.ucSWDB_Buffer[index + 1] = ucValue;
                    SWDBCtrl.ucSWDB_Counter += 2;
                }
                break;

            case eDB_BURST_MODE:
                if(SWDBModeChg)
                {
                    SWDBCtrl.ucSWDB_Mode_Log = SWDBCtrl.ucSWDB_Mode;
                    SWDBCtrl.ucSWDB_Buffer[SWDBCtrl.ucSWDB_Num - 1] = (wReg>>8)|eDB_BURST_MODE;
                    SWDBCtrl.ucSWDB_Buffer[SWDBCtrl.ucSWDB_Num] = 0;
                    SWDBCtrl.ucSWDB_Buffer[SWDBCtrl.ucSWDB_Num + 1] = (wReg&0xFF);
                    SWDBCtrl.ucSWDB_Buffer[index + 3] = ucValue;
                    SWDBCtrl.ucSWDB_Counter += 4;
                }
                else
                {
                    SWDBCtrl.ucSWDB_Buffer[SWDBCtrl.ucSWDB_Num - 1] = (wReg>>8)|eDB_BURST_MODE;
                    SWDBCtrl.ucSWDB_Buffer[SWDBCtrl.ucSWDB_Num]++;
                    SWDBCtrl.ucSWDB_Buffer[SWDBCtrl.ucSWDB_Num + 1 ] = (wReg&0xFF);
                    SWDBCtrl.ucSWDB_Buffer[index] = ucValue;
                    SWDBCtrl.ucSWDB_Counter += 1;
                }
                break;
        }
    }
}


void msSWDBWrite2Byte(WORD wReg,WORD wValue)
{
    msSWDBWriteByte(wReg, wValue );
    msSWDBWriteByte(wReg + 1, wValue>>8);
}

void msSWDBWrite3Byte(WORD wReg, DWORD dwValue)
{
    msSWDBWrite2Byte( wReg, dwValue&0xFFFF );
    msSWDBWriteByte( wReg + 2, ( BYTE )( dwValue >> 16 ) );
}

void msSWDBWriteByteMask(WORD wReg,BYTE ucVal,BYTE ucMask)
{
    msSWDBWriteByte(wReg , (msReadByte( wReg ) & ( ~ucMask ) ) | (ucVal & ucMask) );
}

#if 1
void msSWDBWrite2ByteMask(WORD wReg,WORD wVal,WORD wMask)
{
    msSWDBWriteByteMask(wReg, (BYTE)wVal, (BYTE)wMask);
    msSWDBWriteByteMask(wReg+1, (BYTE)(wVal>>8), (BYTE)(wMask>>8));
}

void msSWDBWriteBit(WORD wReg,BOOL bBit,BYTE ucBitPos)
{
    XDATA BYTE u8Value;

    u8Value = msReadByte( wReg );
    if( bBit )
    {
        u8Value = u8Value | ucBitPos;
    }
    else
    {
        u8Value = u8Value & ( ~ucBitPos );
    }
    msSWDBWriteByte( wReg, u8Value );
}
#endif
#endif


#define DELAY1US()  _nop_()
#if 1//DIRECT_BUS
/////////////////////////////////////////////////////////////////////////
#ifndef _REG_DIRECT_ACCESS_
BYTE msReadByte( WORD u16Reg )
{
    if( u16Reg & 0x8000 )
    {
        scRegs[0] = ( u16Reg >> 8 ) & 0x7F;
        return scRegs[u16Reg&0xFF];
    }
    else
        return msRegs[u16Reg];
}
void msWriteByte( WORD u16Reg, BYTE u8Val )
{
    if( u16Reg & 0x8000 )
    {
        scRegs[0] = ( u16Reg >> 8 ) & 0x7F;
        scRegs[u16Reg&0xFF] = u8Val;
    }
    else
        msRegs[u16Reg] = u8Val;
}

void msWriteBit( WORD u16Reg, Bool bBit, BYTE u8BitPos )
{
    BYTE u8Value;

    u8Value = msReadByte( u16Reg );
    if( bBit )
    {
        u8Value = u8Value | u8BitPos;
    }
    else
    {
        u8Value = u8Value & ( ~u8BitPos );
    }
    msWriteByte( u16Reg, u8Value );
}

//Jison:110428 change msRead2Byte() u16Reg to low addr.
WORD msRead2Byte( WORD u8RegLoAddr )
{
    return ((( WORD )msReadByte( u8RegLoAddr+1 ) ) << 8 ) | msReadByte( u8RegLoAddr );
}



BYTE scReadByte( BYTE u8Reg )
{
    return scRegs[u8Reg];
}

void scWriteByte( BYTE u8Reg, BYTE u8Value )
{
    scRegs[u8Reg] = u8Value;
}
/*
//Jison:110428 u16Reg is low addr.
WORD scRead2Byte( BYTE u8RegLoAddr )
{
    return ((( WORD )scReadByte( u8RegLoAddr+1 ) ) << 8 ) | scReadByte( u8RegLoAddr );
}
//Jison:110428 u16Reg is low addr.
void scWrite2Byte( BYTE u8RegLoAddr, WORD u16Value )
{
    scWriteByte( u8RegLoAddr, ( BYTE )u16Value );
    scWriteByte( u8RegLoAddr + 1, ( BYTE )( u16Value >> 8 ) );
}
void scWriteByteMask( BYTE u8Reg, BYTE u8Value, BYTE u8Mask )
{
    scWriteByte( u8Reg, ( (scReadByte( u8Reg ) & ( ~u8Mask )) | ( u8Value & u8Mask ) ) );
}
*/
#endif
//Jison:110428 u16Reg is low addr.
void msWrite2Byte( WORD u16RegLoAddr, WORD u16Value )
{
    msWriteByte( u16RegLoAddr, ( BYTE )u16Value );
    msWriteByte( u16RegLoAddr + 1, ( BYTE )( u16Value >> 8 ) );
}
void msWriteByteMask( WORD u16Reg, BYTE u8Value, BYTE u8Mask )
{
    msWriteByte( u16Reg, ( (msReadByte( u16Reg ) & ( ~u8Mask )) | ( u8Value & u8Mask ) ) );
}

//Jison:110428 u16Reg is low addr.
void msWrite2ByteMask( WORD u16RegLoAddr, WORD u16Value , WORD u16Mask )
{
    msWriteByteMask( u16RegLoAddr, ( BYTE )u16Value, ( BYTE ) u16Mask );
    msWriteByteMask( u16RegLoAddr + 1, ( BYTE )( u16Value >> 8 ), ( BYTE )( u16Mask >> 8 ) );
}

#if (CHIP_ID == CHIP_TSUMU || CHIP_ID >= CHIP_TSUM2)
void msWrite3Byte( WORD u16RegLoAddr, DWORD u32Value )
{
    msWriteByte( u16RegLoAddr, ( BYTE )u32Value );
    msWriteByte( u16RegLoAddr + 1, ( BYTE )( u32Value >> 8 ) );
    msWriteByte( u16RegLoAddr + 2, ( BYTE )( u32Value >> 16 ) );
}
#endif

/*
//Jison:110428 u8Reg is low addr.
void scWrite2ByteMask( BYTE u8RegLoAddr, WORD u16Value, WORD u16Mask )
{
    scWriteByteMask( u8RegLoAddr, ( BYTE )u16Value, ( BYTE ) u16Mask );
    scWriteByteMask( u8RegLoAddr + 1, ( BYTE )( u16Value >> 8 ), ( BYTE )( u16Mask >> 8 ) );
}
//Jison:110428 u8Reg is low addr.
void scWrite3Byte( BYTE u8RegLoAddr, DWORD u32Value )
{
    scWriteByte( u8RegLoAddr, ( BYTE )u32Value );
    scWriteByte( u8RegLoAddr + 1, ( BYTE )( u32Value >> 8 ) );
    scWriteByte( u8RegLoAddr + 2, ( BYTE )( u32Value >> 16 ) );
}
*/

#if 0
BYTE mStar_ReadByte( BYTE addr )
{
    //static xdata WORD ucCntOldRB = 0;
    addr = addr;

    //ucCntOldRB++;

    return 0;
}

void mStar_WriteByte( BYTE addr, BYTE value )
{
    //static xdata WORD ucCntWB = 0;
    value = addr;

    //ucCntWB++;
}

WORD mStar_ReadWord( BYTE addr )
{
    //static xdata WORD ucCntOldRW = 0;
    addr = addr;

    //ucCntOldRW++;

    return 0;
}
#endif
#if 0
void mStar_WriteWord( BYTE addr, WORD value )
{
    //static xdata WORD ucCntOldWW = 0;
    value = addr;

    //ucCntOldWW++;
}
#endif
//===============================================================
#elif DDR_BUS
void mStar_Start(BYTE u8Addr)
{
#define DDR_PORT    u8Addr
    SET_DDR_PORT;
    DELAY1US();
    SET_MS_ALE();

    DDR_PORT >>= 4;
    SET_DDR_PORT;
    DELAY1US();
    CLR_MS_ALE();
#undef DDR_PORT
}

void mStar_SendByte(BYTE u8Value)
{
#define DDR_PORT    u8Value
    SET_DDR_PORT;
    DELAY1US();
    CLR_MS_WRZ();

    DDR_PORT >>= 4;
    SET_DDR_PORT;
    DELAY1US();
    SET_MS_WRZ();
#undef DDR_PORT
}

#if 0
BYTE mStar_ReceiveByte(void)
{
    BYTE    u8Value;
    BYTE    ddrPort = 0xF;

    CLR_MS_RDZ();
    SET_DDR_PORT;
    u8Value = (HWMS_DDR_PORT >> HWMS_DDR_SHIFT_BITS) & 0xF;
    SET_MS_RDZ();

    DELAY1US();
    CLR_MS_RDZ();
    SET_DDR_PORT;
    u8Value = u8Value | (((HWMS_DDR_PORT >> HWMS_DDR_SHIFT_BITS) & 0xF) << 4);
    SET_MS_RDZ();

    return u8Value;
}

#else
BYTE mStar_ReceiveByte(void)
{
    BYTE    u8Value;
    BYTE    ddrPort = 0xF;

    SET_DDR_PORT;
    DELAY1US();
    CLR_MS_RDZ();
    u8Value = (HWMS_DDR_PORT >> HWMS_DDR_SHIFT_BITS) & 0xF;
    SET_MS_RDZ();

    DELAY1US();
    SET_DDR_PORT;
    DELAY1US();
    CLR_MS_RDZ();
    u8Value = u8Value | (((HWMS_DDR_PORT >> HWMS_DDR_SHIFT_BITS) & 0xF) << 4);
    SET_MS_RDZ();

    return u8Value;
}
#endif
BYTE mStar_ReadByte(BYTE u8Addr)
{
    mStar_Start(u8Addr);
    return mStar_ReceiveByte();
}

void mStar_WriteByte(BYTE u8Addr, BYTE u8Value)
{
    mStar_Start(u8Addr);
    mStar_SendByte(u8Value);
}

WORD mStar_ReadWord(BYTE u8Addr)
{
    WORD    u16Value;

    u16Value = mStar_ReadByte(u8Addr);
    u16Value = (u16Value << 8) | mStar_ReadByte(u8Addr - 1);

    return u16Value;
}

void mStar_WriteWord(BYTE u8Addr, WORD u16Value)
{
    mStar_WriteByte(u8Addr - 1, u16Value & 0xFF);
    mStar_WriteByte(u8Addr, u16Value >> 8);
}

void mStar_WriteTBL(BYTE u8Addr, BYTE *pu8Tbl, BYTE u8Count, BYTE u8Inc)
{
    u8Inc = u8Inc;
    while (u8Count--)
        mStar_WriteByte(u8Addr, *(pu8Tbl++));
}

#else // !DIRECT_BUS
void mStar_Start(msBus_Direction direct)
{
    SET_MS_CSZ();       // CS=1
    CLR_MS_SCL();       // SCL=0
    if (direct)         // Read
        SET_MS_SDA();   //SDA=1
    else
        CLR_MS_SDA();   //SDA=0
    SET_MS_SCL();       //SCL=1
    CLR_MS_SCL();       //SCL=0
}

void mStar_SendByte(BYTE u8Value, Bool lastOne)
{
    BYTE    i;

    for (i = 0; i < 7; i++)
    {
        (u8Value & BIT7) ? (SET_MS_SDA()) : (CLR_MS_SDA());
        u8Value <<= 1;
        SET_MS_SCL();
        DELAY4US();
        CLR_MS_SCL();
    }

    if (lastOne)
        CLR_MS_CSZ();
    (u8Value & BIT7) ? (SET_MS_SDA()) : (CLR_MS_SDA());
    DELAY4US();
    SET_MS_SCL();
    DELAY4US();
    CLR_MS_SCL();

    if (lastOne)
    {
        SET_MS_SDA();
        SET_MS_SCL();
    }
}

BYTE mStar_ReceiveByte(Bool lastOne)
{
    BYTE    i;
    BYTE    u8Value;

    for (i = 0; i < 7; i++)
    {
        u8Value <<= 1;
        SET_MS_SCL();
        DELAY4US();
        if (IS_MS_SDA_HI())
            u8Value |= BIT0;
        CLR_MS_SCL();
    }

    if (lastOne)
        CLR_MS_CSZ();
    {
        u8Value <<= 1;
        SET_MS_SCL();
        DELAY4US();
        if (IS_MS_SDA_HI())
            u8Value |= BIT0;
        CLR_MS_SCL();
    }

    if (lastOne)
    {
        SET_MS_SDA();
        SET_MS_SCL();
    }

    return u8Value;
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
BYTE mStar_ReadByte(BYTE u8Addr)
{
    mStar_Start(MS_READ);
    mStar_SendByte(u8Addr, 0);
    SET_MS_SDA();

    return mStar_ReceiveByte(1);
}

WORD mStar_ReadWord(BYTE u8Addr)
{
    WORD    u16Value;

    u16Value = mStar_ReadByte(u8Addr);
    u16Value = (u16Value << 8) | mStar_ReadByte(u8Addr - 1);

    return u16Value;
}

void mStar_WriteByte(BYTE u8Addr, BYTE u8Value)
{
    mStar_Start(MS_WRITE);
    mStar_SendByte(u8Addr, 0);
    mStar_SendByte(u8Value, 1);
}

void mStar_WriteWord(BYTE u8Addr, WORD u16Value)
{
    mStar_WriteByte(u8Addr - 1, u16Value & 0xFF);
    mStar_WriteByte(u8Addr, u16Value >> 8);
}

void mStar_SendDDRByte(BYTE u8Value, Bool lastOne)
{
    BYTE    i;

    for (i = 0; i < 7; i++)
    {
        HWMS_SDA_PIN = (u8Value & BIT7);
        u8Value <<= 1;
        HWMS_SCL_PIN = !HWMS_SCL_PIN;
    }

    if (lastOne)
        CLR_MS_CSZ();
    (u8Value & BIT7) ? (SET_MS_SDA()) : (CLR_MS_SDA());
    HWMS_SCL_PIN = !HWMS_SCL_PIN;

    if (lastOne)
    {
        SET_MS_SDA();
        SET_MS_SCL();
    }
}

void mStar_WriteDDRByte(BYTE u8Addr, BYTE u8Value)
{
    CLR_MS_SCL();
    SET_MS_CSZ();   // CS=1
    CLR_MS_SDA();   //SDA=0
    SET_MS_SCL();
    mStar_SendDDRByte(u8Addr, 0);
    mStar_SendDDRByte(u8Value, 1);
}

void mStar_WriteTBL(BYTE u8Addr, BYTE *pu8Tbl, BYTE u8Count, BYTE u8Inc)
{
    if (u8Inc)
        mStar_WriteByte(REGBK, mStar_ReadByte(REGBK) &~AINC);
    else
        mStar_WriteByte(REGBK, mStar_ReadByte(REGBK) | AINC);

    mStar_Start(MS_WRITE);
    mStar_SendByte(u8Addr, 0);
    while (u8Count--)
        mStar_SendByte(*(pu8Tbl++), 0);
    mStar_SendByte(*pu8Tbl, 1);
}


#if 0
void mStar_SwitchRegBank(BYTE bank_index)
{
    BYTE    regValue = mStar_ReadByte(REGBK);
    mStar_WriteByte(REGBK, (regValue & REGBANK_MASK) | bank_index);
}
#endif
#endif

#if ENABLE_MENULOAD //111006 Rick add menuload function - C_FOS_015
//#include "DebugMsg.h"
//MST_MODULE(ms_rwreg)

static WORD xdata u16ML_Count = 0;
static BYTE xdata u8ML_Nums = 0;
static BYTE xdata u8ML_NumsIdx=0;
static BYTE xdata u8PrevML_Mode=0;
static WORD xdata u16PrevML_Reg=0;
// Note the xdata used by code should be < 500h=1280 bytes, 500h~5FFh for menu load function
// Compiler set xdata start at 0x4000 so the start addr =0x4500
// CHIP_TSUMV do not have the status bit to show the ML finished or not.
// Note SC registers do not write scaler bank(function like scWriteByte)
#define ML_START_ADDR       0x4500
#define ML_BUFFER           ((unsigned char volatile xdata *) ML_START_ADDR)
#define VAR_INC(u16Value)   {MST_ASSERT(u16Value<256);u16Value++;}
#if ML_USE_INT
volatile BYTE xdata u8ML_Status=ML_STS_DONE;
#endif
// To ensure the previous ML already finished while next ML process start.
void msML_WaitReady(void) //must call this function before 1st msML_WriteByte() of next ML write process.
{
    SetTimOutConter(50);
    //u16TimeOutCounter=50;
    //bTimeOutCounterFlag=1;
  #if ML_USE_INT
    MST_ASSERT((u8ML_Status&ML_STS_CMD_ERR)==0);
    //while(!(msReadByte(REG_3C27)&BIT3) && TimeOutCounter!=0)
    while(!(u8ML_Status&ML_STS_DONE) && u16TimeOutCounter!=0);

    //MST_ASSERT(u8ML_Status==ML_STS_DONE);

    #if 0
    printData("u8ML_Status=%xh", u8ML_Status);
    printData("REG_3C27=%xh\r\n", msReadByte(REG_3C27));
    #endif

    msWriteByteMask(REG_3C27, BIT3 ,BIT3);                  //clr menu load done
    u8ML_Status=0;
  #else
    while((msReadByte(REG_3C25)&BIT7) && u16TimeOutCounter!=0);
  #endif
    msWriteByteMask(REG_3C25, 0x00 ,0xC0);                  //Menu load disable
}
void msML_WriteByte(ML_ModeType enML_Mode,WORD u16Reg, BYTE u8Value)
{
  #if ML_USE_INT
    MST_ASSERT(u8ML_Status==0); //should call msML_WaitReady() first
  #endif
    if ((enML_Mode!=u8PrevML_Mode) //change mode
        || (enML_Mode==ML_MODE_SUCCESSIVE && (u16Reg&0xFF00)!=(u16PrevML_Reg&0xFF00)) //successice mode change bank
        || (enML_Mode==ML_MODE_BURST && (u16Reg)!=(u16PrevML_Reg))) //burst mode change bank/addr
    {
        u8ML_Nums=0; //reset for another successive/burst mode
    }

    u8PrevML_Mode=enML_Mode;
    u16PrevML_Reg=u16Reg;

    switch(enML_Mode)
    {   // Normal mode
        case ML_MODE_NORMAL:
            //bank
            if( u16Reg & 0x8000 )
                ML_BUFFER[u16ML_Count] = 0x2F|ML_MODE_NORMAL;
            else
                ML_BUFFER[u16ML_Count] = (BYTE)((u16Reg>>8)&0x3F);
            VAR_INC(u16ML_Count);
            //addr
            ML_BUFFER[u16ML_Count] = (BYTE)(u16Reg);
            VAR_INC(u16ML_Count);
            //value
            ML_BUFFER[u16ML_Count] = u8Value;
            VAR_INC(u16ML_Count);
            break;
#if 0
        case ML_MODE_SUCCESSIVE :
            // Successive mode
            if(u8ML_Nums == 0x00) //bank
            {
                if( u16Reg & 0x8000 )
                    ML_BUFFER[u16ML_Count] = (BYTE)(0x2F&0x3F)|ML_MODE_SUCCESSIVE;
                else
                    ML_BUFFER[u16ML_Count] = (BYTE)((u16Reg>>8)&0x3F)|ML_MODE_SUCCESSIVE;
                VAR_INC(u16ML_Count);
                //nums
                u8ML_NumsIdx=u16ML_Count; //save nums index in bufer
                VAR_INC(u16ML_Count);
            }

            //addr
            ML_BUFFER[u16ML_Count] = (BYTE)(u16Reg);
            VAR_INC(u16ML_Count);
            //value
            ML_BUFFER[u16ML_Count] = u8Value;
            VAR_INC(u16ML_Count);
            u8ML_Nums++;
            ML_BUFFER[u8ML_NumsIdx] = u8ML_Nums-1;
            break;

        case ML_MODE_BURST:
            // Burst mode
            if(u8ML_Nums == 0x00) //bank
            {
                if( u16Reg & 0x8000 )
                    ML_BUFFER[u16ML_Count] = (BYTE)(0x2F&0x3F)|ML_MODE_BURST;
                else
                    ML_BUFFER[u16ML_Count] = (BYTE)((u16Reg>>8)&0x3F)|ML_MODE_BURST;
                VAR_INC(u16ML_Count);
                //nums
                u8ML_NumsIdx=u16ML_Count; //save nums index in bufer
                VAR_INC(u16ML_Count);
                //addr
                ML_BUFFER[u16ML_Count] = (BYTE)(u16Reg);
                VAR_INC(u16ML_Count);
            }

            ML_BUFFER[u16ML_Count] = u8Value;
            VAR_INC(u16ML_Count);
            u8ML_Nums++;
            ML_BUFFER[u8ML_NumsIdx] = u8ML_Nums-1;
            break;
#endif
    }
}
#if 0
void msML_Write2Byte(WORD u16Reg, WORD u16Value)
{
    ML_WriteByte(ML_MODE_SUCCESSIVE,u16Reg , (BYTE)u16Value);
    ML_WriteByte(ML_MODE_SUCCESSIVE,u16Reg +1, (BYTE)(u16Value >> 8));
}

void msML_WriteByteMask(WORD u16Reg, BYTE u8Value, BYTE u8Mask)
{
    XDATA BYTE uc;

    uc = msReadByte(u16Reg) & (~u8Mask);
    ML_WriteByte( u16Reg, ( uc | (u8Value & u8Mask)) );
}
#endif
void msML_Trigger(ML_TrigType enTrigType)
{
    msWriteByteMask(REG_3C25, enTrigType ,0x30);                                    //trig by Vsync-op
    msWrite2ByteMask(REG_3C24, (ML_START_ADDR&0x7FF),0x7FF);                //Menu Load start addr
    #if !ML_USE_INT
    msML_WriteByte(ML_MODE_NORMAL, REG_3C25, msReadByte(REG_3C25)&(~(BIT7|BIT6)));  //Last menu load write byte(Disable menu load while finished)
    #endif
    // run after msML_WriteByte() that disable ML to get correct u16ML_Count.
    msWrite2ByteMask(REG_3C26, (ML_START_ADDR&0x7FF)+u16ML_Count-1,0x7FF);    //Menu Load end addr
    #if 0
    {
        WORD i;
        for(i=0;i<u16ML_Count;i++)
            printData("%x",ML_BUFFER[i]);
    }
    #endif

    if(enTrigType == ML_TRIG_SW)
        msWriteByteMask(REG_3C25, 0xC0 ,0xC0);              //Menu load Enable
    else
        msWriteByteMask(REG_3C25, 0x80 ,0xC0);              //Menu load Enable
    //PRINT_DATA("u16ML_Count=%d",u16ML_Count);
    u16ML_Count     = 0;
    u8ML_Nums       = 0;
    u8PrevML_Mode   = 0;
    u16PrevML_Reg   = 0;
}

#endif

