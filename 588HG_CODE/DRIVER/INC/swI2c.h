///////////////////////////////////////////////////////////////////////////////
/// @file swI2c.h
/// @brief Head file of swI2c.c
/// @author MStarSemi Inc.
///
/// S/W I2C functions for outside I2C device control.
///
/// Features
///  - I2C interface is a two-wire interface which is designed for communicating between ICs.
///  - Use S/W to control GPIO for producing I2C signal.
///  - Provide I2C Init/Read/Write function.
///
///////////////////////////////////////////////////////////////////////////////
#ifndef _SWI2C_H
#define _SWI2C_H

#if ENABLE_MHL
/********************************************************************************/
/*                                Define                                        */
/********************************************************************************/
#ifdef _SWI2C_C
#define INTERFACE
#else
#define INTERFACE   extern
#endif

/********************************************************************************/
/*                                 Macro                                        */
/********************************************************************************/


/********************************************************************************/
/*                                 Enum                                        */
/********************************************************************************/
/*
typedef enum I2C_Direction
{ I2C_WRITE,
  I2C_READ
} I2C_Direction;
*/
/********************************************************************************/
/*                           Function prototypes                                */
/********************************************************************************/

//INTERFACE void swi2c_ReadTBL(BYTE deviceID, WORD addr, BYTE *buffer, BYTE count);
INTERFACE Bool swi2cElandConfigBytes (BYTE ucConfigAdr, BYTE ucConfigDataA, BYTE ucConfigDataB);
INTERFACE Bool i2cElandWriteWord(BYTE ucSRID, WORD waddr, WORD wdata);
INTERFACE Bool i2cElandWriteByte(BYTE ucSRID, WORD waddr, BYTE ucdata);
INTERFACE WORD i2cElandReadWord(BYTE ucSRID, WORD waddr);
INTERFACE BYTE i2cElandReadByte(BYTE ucSRID, WORD waddr);

#undef INTERFACE

#endif // #if ENABLE_MHL

#endif // _SWI2C_H
