///////////////////////////////////////////////////////////////////////////////
/// @file ms_rwreg.h
/// @brief Head file of ms_rwreg.c
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
#include "global.h"
#ifndef _MS_RWREG_H
#define _MS_RWREG_H

#ifdef _MS_RWREG_C
#define INTERFACE
#else
#define INTERFACE   extern
#endif
//#define _REG_DIRECT_ACCESS_

/////////////////////////////////////////////////////////////////////////////
//Must use sc series R/W function if input scaler bank address is BYTE data.
//Must use bank save/restore future while use sc series function.
/////////////////////////////////////////////////////////////////////////////
#define MAIN_WINDOW 0
#define SUB_WINDOW  1
#define msRegs        ((unsigned char volatile xdata *) 0x0000)
#define scRegs        ((unsigned char volatile xdata *) 0x2F00)
#define MAKEWORD(value1, value2)    (((WORD)(value1))<<8) + (value2)
//Jison 110308 for GPIO define and can be used in interrupt
#define MEM_MSREAD_BYTE(u16Addr)                        (msRegs[u16Addr])
#define MEM_MSWRITE_BYTE(u16Addr,u8Value)               (msRegs[u16Addr]=(u8Value))
#define MEM_MSWRITE_BYTE_MASK(u16Addr,u8Value,u8Mask)   ( msRegs[u16Addr] = ( (msRegs[u16Addr] & (~(u8Mask))) | ((u8Value) & (u8Mask))) )
#define MEM_MSWRITE_BIT(u16Addr,u8True,u8Bit)           ( msRegs[u16Addr] = ( (msRegs[u16Addr] & (~(u8Bit))) | ((u8True)?(u8Bit):0)) )
#define MEM_SCREAD_BYTE(u16Addr)                        (scRegs[(u16Addr)&0xFF])
#define MEM_SCWRITE_BYTE(u16Addr,u8Value)               (scRegs[(u16Addr)&0xFF]=(u8Value))
#ifdef _REG_DIRECT_ACCESS_
#define msReadByte(u16Addr)                     (((u16Addr)&0x8000) ?\
                                                ((scRegs[0]=((u16Addr)>>8)&0x7F), scRegs[(u16Addr)&0xFF]) :\
                                                (msRegs[(u16Addr)]))

#define msWriteByte(u16Addr,u8Value)            (((u16Addr)&0x8000) ?\
                                                ((scRegs[0]=((u16Addr)>>8)&0x7F) ? (scRegs[(u16Addr)&0xFF]=(u8Value)):(scRegs[(u16Addr)&0xFF]=(u8Value))) :\
                                                (msRegs[u16Addr] = (u8Value)))

//#define msWriteByteMask( wReg,  ucValue,  ucMask )  (msWriteByte( (wReg), ( (msReadByte( wReg ) & ( ~(ucMask) )) | ( (ucValue) & (ucMask) ) ) ))

#define msWriteBit(u16Addr,u8True,u8Bit)        msWriteByte((u16Addr),msReadByte(u16Addr)&(~(u8Bit))|((u8True)?(u8Bit):0))

#define msRead2Byte( u8RegLoAddr )              ((((WORD)msReadByte((u8RegLoAddr)+1))<<8)|msReadByte((u8RegLoAddr)))

//#define msWrite2Byte(wReg, wValue )       {msWriteByte( (wReg) - 1, (wValue)&0xFF ); msWriteByte( (wReg), ( (wValue) >> 8 ) );}

//#define scReadByte( u16Addr )                   (scRegs[(u16Addr)&0xFF])

//#define scWriteByte( u16Addr, u16Value )        (scRegs[(u16Addr)&0xFF] = (u16Value))

//#define scRead2Byte( u8RegLoAddr )              ((((WORD)scReadByte((u8RegLoAddr)+1))<<8)|scReadByte((u8RegLoAddr)))

//#define scWrite2Byte(u8RegLoAddr,u16Value)      {(scWriteByte((u8RegLoAddr),(u16Value)&0xFF)); scWriteByte((u8RegLoAddr)+1,((u16Value)>>8));}

//#define scWriteByteMask(u8Reg,u8Value,u8Mask)   (scWriteByte( (u8Reg), ( (scReadByte( u8Reg ) & ( ~(u8Mask) )) | ( (u8Value) & (u8Mask) ) ) ))

#else
INTERFACE BYTE msReadByte( WORD u16Reg );
INTERFACE void msWriteByte( WORD u16Reg, BYTE u8Val );
INTERFACE WORD msRead2Byte( WORD u8RegLoAddr );
INTERFACE void msWriteBit( WORD u16Reg, Bool bBit, BYTE u8BitPos );
INTERFACE BYTE scReadByte( BYTE u8Reg );
INTERFACE void scWriteByte( BYTE u8Reg, BYTE u8Value );
//INTERFACE WORD scRead2Byte( BYTE u8RegLoAddr );
//INTERFACE void scWrite2Byte( BYTE u8RegLoAddr, WORD u16Value );
//INTERFACE void scWriteByteMask( BYTE u8Reg, BYTE u8Value, BYTE u8Mask );


//INTERFACE void mStar_WriteByte(BYTE u8Addr, BYTE u8Value);
//INTERFACE BYTE mStar_ReadByte(BYTE u8Addr);
//INTERFACE WORD mStar_ReadWord(BYTE u8Addr);
//INTERFACE void mStar_WriteWord(BYTE u8Addr, WORD u16Value);
//INTERFACE void mStar_WriteTBL(BYTE u8Addr, BYTE *pu8Tbl, BYTE u8Count, BYTE u8Inc);
#endif
INTERFACE void msWrite2Byte( WORD u16RegLoAddr, WORD u16Value );
INTERFACE void msWriteByteMask( WORD u16Reg, BYTE u8Value, BYTE u8Mask );
INTERFACE void msWrite2ByteMask( WORD u16RegLoAddr, WORD wValue , WORD ucMask );
//INTERFACE void scWrite2ByteMask( BYTE u8RegLoAddr, WORD u16Value, WORD u16Mask );
//INTERFACE void scWrite3Byte( BYTE u8RegLoAddr, DWORD u32Value );
INTERFACE void msWrite3Byte( WORD u16RegLoAddr, DWORD u32Value );
//for Menu Load
typedef enum
{
    ML_MODE_NORMAL      = 0x00,
    ML_MODE_SUCCESSIVE  = 0x40,    
    ML_MODE_BURST       = 0x80,    
} ML_ModeType;
typedef enum
{
    ML_TRIG_SW          =0x00,
    ML_TRIG_OUT_VSYNC   =0x10,
    ML_TRIG_IN_VSYNC    =0x20,
    ML_TRIG_OUT_VDE_END =0x30
} ML_TrigType;
INTERFACE void msML_WaitReady(void);
INTERFACE void msML_WriteByte(ML_ModeType enML_Mode,WORD u16Reg, BYTE u8Value);
INTERFACE void msML_Trigger(ML_TrigType enTrigType);

#if 1//ENABLE_MENULOAD
#define ML_STS_BUSY     BIT7
#define ML_STS_TRIG_ERR BIT6
#define ML_STS_CMD_ERR  BIT5
#define ML_STS_DONE     BIT3
#endif



#if ENABLE_SW_DOUBLE_BUFFER    
#define DB_Mode(mode)                       msSWDBMode(mode)
#define DB_WB(wReg,ucVal)                   msSWDBWriteByte(wReg,ucVal)
#define DB_W2B(wReg,wVal)                   msSWDBWrite2Byte(wReg,wVal)
#define DB_W3B(wReg,wVal)                   msSWDBWrite3Byte(wReg,wVal)
#define DB_WBMask(wReg,ucVal,ucMask)        msSWDBWriteByteMask(wReg,ucVal,ucMask) 
#define DB_W2BMask(wReg,wVal,wMask)         msSWDBWrite2ByteMask(wReg,wVal,wMask)
#define DB_WBit(wReg,bBit,ucBitPos)         msSWDBWriteBit(wReg,bBit,ucBitPos)
#else
#define DB_Mode(mode)
#define DB_WB(wReg,ucVal)                   msWriteByte(wReg,ucVal)
#define DB_W2B(wReg,wVal)                   msWrite2Byte(wReg,wVal)
#define DB_W3B(wReg,wVal)                   msWrite3Byte(wReg,wVal)
#define DB_WBMask(wReg,ucVal,ucMask)        msWriteByteMask(wReg,ucVal,ucMask)
#define DB_W2BMask(wReg,wVal,wMask)         msWrite2ByteMask(wReg,wVal,wMask)
#define DB_WBit(wReg,bBit,ucBitPos)         msWriteBit(wReg,bBit,ucBitPos)
#endif

#if 1//ENABLE_SW_DOUBLE_BUFFER
#define SWDB_MAX_SIZE  99 // Beware of overflow problem in xdata allocation

typedef struct
{
    BYTE ucSWDB_OnFire;
    BYTE ucSWDB_Mode;
    BYTE ucSWDB_Mode_Log;
    BYTE ucSWDB_LastSCBank;
    BYTE ucSWDB_Num;
    BYTE ucSWDB_Counter;
    BYTE ucSWDB_Buffer[SWDB_MAX_SIZE];
} StuSWDBCtrl;

typedef enum _DBTYPE
{
    eDB_NORMAL_MODE     = 0x00,
    eDB_SUCCESSIVE_MODE = 0x40,
    eDB_BURST_MODE      = 0x80,
}DB_TYPE;



INTERFACE void msSWDBInit(void);
INTERFACE void msSWDBMode(DB_TYPE type);
INTERFACE void msSWDBWriteByte(WORD wReg,BYTE ucVal);
INTERFACE void msSWDBWrite2Byte(WORD wReg,WORD wVal);
INTERFACE void msSWDBWrite3Byte(WORD wReg, DWORD dwValue);
INTERFACE void msSWDBWriteByteMask(WORD wReg,BYTE ucVal,BYTE ucMask);
INTERFACE void msSWDBWrite2ByteMask(WORD wReg,WORD wVal,WORD wMask);
INTERFACE void msSWDBWriteBit(WORD wReg,BOOL bBit,BYTE ucBitPos);
INTERFACE void msSWDBWriteToRegister(void);
INTERFACE void msSWDBWriteToRegisterByOsdEnd(void);
INTERFACE void msSWDBWaitForRdy(void);
#endif




#undef INTERFACE


#endif   ///_MS_RWREG_H
