///////////////////////////////////////////////////////////////////////////////
/// @file swI2c.c
/// @brief S/W I2C functions
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

#define _SWI2C_C

/******************************************************************************/
/*                    Header Files                        */
/* ****************************************************************************/
#include "types.h"
#include "board.h"
//#include "DebugMsg.h"
//#include "misc.h"
#include "swI2c.h"
#include "Global.h"
#include <intrins.h>


/********************************************************************************/
/*                           Function prototypes                                */
/********************************************************************************/
//====================================
// Setup i2c Start condition
#if ENABLE_MHL

#define SWI2C_ACCESS_DUMMY_TIME       3

void Delay1us(void)
{
    BYTE i; // Add by Jonson 20100917

    if(g_bMcuPMClock)
        return;
    
#if 1 //asm code better
    i=5; // 315K // 20: 135K
    while(i--)
    {
        _nop_();
    }
#else
    for(i=0;i<u8Loop4us;i++)
    {
        _nop_();
    }
#endif
}

static Bool swi2c_Start(void)
{
    SET_SWI2C_SDA();
    Delay1us();//Delay4us();
    SET_SWI2C_SCL();
    Delay1us();//Delay4us();
    if (IS_SWI2C_SDA_LO() || IS_SWI2C_SCL_LO())
        return FALSE;
    CLR_SWI2C_SDA();
    Delay1us();//Delay4us();
    CLR_SWI2C_SCL();
    return TRUE;
}
//=============================================
// Setup i2c Stop condition
void swi2c_Stop(void)
{
    // SCL=L, SDA=L, Stop condition.
    CLR_SWI2C_SCL();
    Delay1us();//Delay4us();
    CLR_SWI2C_SDA();
    Delay1us();//Delay4us();
    SET_SWI2C_SCL();
    Delay1us();//Delay4us();
    SET_SWI2C_SDA();
}

//============================================
Bool swi2c_SendByte(BYTE u8Value)
{
    BYTE i;
    Bool result;

    for (i=0; i<8; i++) // Send data via i2c pin
    {
        if (u8Value&BIT7)
            SET_SWI2C_SDA();
        else
            CLR_SWI2C_SDA();
        Delay1us();//Delay4us();
        SET_SWI2C_SCL();
        Delay1us();//Delay4us();  //ygp  //simon 070118 change for not use CSI 24C04
        u8Value<<=1;
        CLR_SWI2C_SCL();
    }
    SET_SWI2C_SDA();
    Delay1us();//Delay4us();
    SET_SWI2C_SCL();
    Delay1us();//Delay4us();  //ygp//simon 070118 change for not use CSI 24C04
    result=IS_SWI2C_SDA_LO(); // see if ACK
    CLR_SWI2C_SCL();
    Delay1us();//Delay4us();
    CLR_SWI2C_SDA();

    return result;
}

//============================================
BYTE swi2c_ReceiveByte(const Bool ack)
{
    BYTE i;
    BYTE u8Value = 0;

    for (i=0; i<8; i++)
    {
        u8Value<<=1;
        SET_SWI2C_SDA();
        SET_SWI2C_SCL();
        Delay1us();//Delay4us();
        if (IS_SWI2C_SDA_HI())
            u8Value|=BIT0;
        CLR_SWI2C_SCL();
        Delay1us();//Delay4us();//20051101
    }
    if (ack)
        CLR_SWI2C_SDA();
    else
        SET_SWI2C_SDA();
    Delay1us();//Delay4us();
    SET_SWI2C_SCL();
    Delay1us();//Delay4us();
    CLR_SWI2C_SCL();

    return u8Value;
}

#if 0
static Bool swi2c_BurstWrite(BYTE u8Count, BYTE *pu8Buffer)
{
    while (u8Count--)
    {
        if (swi2c_SendByte(*(pu8Buffer++))==FALSE)
            return FALSE;
    }
    return TRUE;
}
static Bool swi2c_BurstRead(BYTE u8Count, BYTE * pu8Buffer)
{
    BYTE i;

    for (i=0; i<u8Count-1; i++)
        *(pu8Buffer+i)=swi2c_ReceiveByte(1);
    *(pu8Buffer+i)=swi2c_ReceiveByte(0);
    swi2c_Stop();
    return TRUE;
}
#endif

Bool swi2c_MasterStart(I2C_Direction enDirect, BYTE u8Addr)//Sky111213 modify
{
    BYTE u8Retry=SWI2C_ACCESS_DUMMY_TIME;
    if (enDirect==I2C_READ) // Set I2C direction bit.
        u8Addr|=BIT0;
    else
        u8Addr&=~BIT0;

    while (u8Retry--)
    {
        if (swi2c_Start()==FALSE)
        {
            ForceDelay1ms(1); //tony 24/11/03
            continue;
        }
        if (swi2c_SendByte(u8Addr)==TRUE) // send address success
            return TRUE;
        swi2c_Stop();
        ForceDelay1ms(1);
    }
    return FALSE;
}

#if 0
void swi2c_ReadTBL(BYTE u8DeviceID, WORD u16Addr, BYTE *pu8Buffer, BYTE u8Count)
{
    BYTE  u8Retry=SWI2C_ACCESS_DUMMY_TIME;
    BYTE  u8WordAddr;

    while (u8Retry--)
    {
        if (swi2c_MasterStart(I2C_WRITE, u8DeviceID)==FALSE)
            continue;
        u8WordAddr = HIBYTE(u16Addr);
        if (swi2c_BurstWrite(1, &u8WordAddr)==FALSE)
            continue;
        u8WordAddr=LOBYTE(u16Addr);
        if (swi2c_BurstWrite(1, &u8WordAddr)==FALSE)
            continue;
        if (swi2c_MasterStart(I2C_READ, u8DeviceID)==FALSE)
            continue;
        if (swi2c_BurstRead(u8Count, pu8Buffer)==FALSE)
            continue;
        //swi2c_Stop();
        return;
    }
}
#endif

/////////////////// Eland use only /////////////////////////////////
// I2C Set bytes to device.
//
// Arguments: ucConfigAdr - Config address
//            ucConfigDataA - Config Data A
//            ucConfigDataB - Config Data B
/////////////////////////////////////////////////////////////////
Bool swi2cElandConfigBytes (BYTE ucConfigAdr, BYTE ucConfigDataA, BYTE ucConfigDataB)
{
    BYTE ucDummy; // loop dummy

    ucDummy = SWI2C_ACCESS_DUMMY_TIME;
    while (--ucDummy)
    {
        if (swi2c_MasterStart(I2C_WRITE, ucConfigAdr) == FALSE)
            continue;

        if (swi2c_SendByte(ucConfigDataA) == FALSE) // check non-acknowledge
            continue;

        swi2c_SendByte(ucConfigDataB);
        break;
    } // while
		
    swi2c_Stop();

    return ((ucDummy)?(TRUE):(FALSE));
}


/////////////////// Eland use only /////////////////////////////////
// I2C write bytes to device.
//
// Arguments: ucSRID - SRID
//                   waddr   - write addr
//                   wdata   - write data
/////////////////////////////////////////////////////////////////
Bool i2cElandWriteWord(BYTE ucSRID, WORD waddr, WORD wdata)
{
    BYTE ucDummy; // loop dummy

    ucDummy = SWI2C_ACCESS_DUMMY_TIME;
    while (--ucDummy)
    {
        if (swi2c_MasterStart(I2C_WRITE, ucSRID) == FALSE)
            continue;

        if (swi2c_SendByte(waddr>>8) == FALSE) // check non-acknowledge
            continue;

        swi2c_SendByte((waddr&0x00FF)) ;	
        swi2c_SendByte((wdata&0x00FF)) ;    // LowByte 
        swi2c_SendByte((wdata>>8)) ;          // HiByte 
        break;
    } // while
    swi2c_Stop();
    return ((ucDummy)?(TRUE):(FALSE));
}

/////////////////// Eland use only /////////////////////////////////
// I2C write bytes to device.
//
// Arguments: ucSRID - SRID
//                   waddr   - write addr
//                   ucdata   - write data
/////////////////////////////////////////////////////////////////
Bool i2cElandWriteByte(BYTE ucSRID, WORD waddr, BYTE ucdata)
{
    BYTE ucDummy; // loop dummy

    ucDummy = SWI2C_ACCESS_DUMMY_TIME;
    while (--ucDummy)
    {
        if (swi2c_MasterStart(I2C_WRITE, ucSRID) == FALSE)
            continue;

        if (swi2c_SendByte(waddr>>8) == FALSE) // check non-acknowledge
            continue;

        swi2c_SendByte((waddr&0x00FF)) ;	
        swi2c_SendByte(ucdata) ;    // Byte 
        break;
    } // while
    swi2c_Stop();
    return ((ucDummy)?(TRUE):(FALSE));
}

/////////////////// Eland use only /////////////////////////////////
// I2C read word from device.
//
// Arguments: ucSRID - SRID
//                   waddr   - write addr
// Return value: RdBuf - read word
/////////////////////////////////////////////////////////////////
WORD i2cElandReadWord(BYTE ucSRID, WORD waddr)
{
    BYTE ucDummy; // loop dummy
    WORD RdBuf = 0;
    BYTE ucTemp = 0;
	
    ucDummy = SWI2C_ACCESS_DUMMY_TIME;
    while (ucDummy--)
    {
        if (swi2c_MasterStart(I2C_WRITE, ucSRID) == FALSE)
            continue;

        if (swi2c_SendByte(waddr>>8) == FALSE) // check non-acknowledge
            continue;

        if (swi2c_SendByte((waddr&0x00FF)) == FALSE) // check non-acknowledge
            continue;		
		
        if (swi2c_MasterStart(I2C_READ, ucSRID) == FALSE)
            continue;
		
	 ucTemp=swi2c_ReceiveByte(TRUE);  // LoByte	 
	 RdBuf=swi2c_ReceiveByte(FALSE);	 // HiByte
	 RdBuf=(RdBuf<<8)+ucTemp;
	 break;
    } // while

    swi2c_Stop();

    return RdBuf;	
}

/////////////////// Eland use only /////////////////////////////////
// I2C read word from device.
//
// Arguments: ucSRID - SRID
//                   waddr   - write addr
// Return value: RdBuf - read byte
/////////////////////////////////////////////////////////////////
BYTE i2cElandReadByte(BYTE ucSRID, WORD waddr)
{
    BYTE ucDummy; // loop dummy
    BYTE RdBuf = 0;
	
    ucDummy = SWI2C_ACCESS_DUMMY_TIME;
    while (ucDummy--)
    {
        if (swi2c_MasterStart(I2C_WRITE, ucSRID) == FALSE)
            continue;

        if (swi2c_SendByte(waddr>>8) == FALSE) // check non-acknowledge
            continue;

        if (swi2c_SendByte((waddr&0x00FF)) == FALSE) // check non-acknowledge
            continue;		
		
        if (swi2c_MasterStart(I2C_READ, ucSRID) == FALSE)
            continue;
		
	 RdBuf = swi2c_ReceiveByte(FALSE);	 // Byte
	 break;
    } // while

    swi2c_Stop();

    return RdBuf;	
}

#endif // #if ENABLE_MHL 

