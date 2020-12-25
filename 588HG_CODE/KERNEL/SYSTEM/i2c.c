#include "types.h"
#include "reg52.h"
#include "i2cdef.h"
#include "board.h"
#include "debug.h"
#include "Common.h"
#include "misc.h"
#include "global.h"
#include "gpio_def.h"
#include "ms_reg.h"
#include "ms_rwreg.h"

#include "i2c.h"//modify by smc.lmw 20160526

#define i2cSlaveAddr(deviceID, addr)    deviceID|((addr>>8)<<1)
#define i2cWordAddr(addr)       addr&0xFF
#if (EEPROM_TYPE >= EEPROM_24C32) //modify by smc.lmw 20160526
BYTE ucADDR_HI_BYTE;
#endif
#if !USEFLASH || (defined(UseVGACableReadWriteAllPortsEDID)&&!defined(UseInternalDDCRam))
//====================================
// Setup i2c Start condition
Bool i2c_Start( void )
{
    Set_i2c_SDA();
    Set_i2c_SCL();
    Delay4us();
    if( i2c_SDALo() || i2c_SCLLo() )
        return FALSE;
    Clr_i2c_SDA();
    Delay4us();
    Clr_i2c_SCL();
    Delay4us();
    return TRUE;
}
//=============================================
// Setup i2c Stop condition
void i2c_Stop( void )
{
    // SCL=L, SDA=L, Stop condition.
    Clr_i2c_SCL();
    Clr_i2c_SDA();
    Delay4us();
    Set_i2c_SCL();
    Delay4us();
    Set_i2c_SDA();
    Delay4us();
}
//============================================
Bool Wait_i2C_Ack( void )
{
    BYTE i;
    for( i = 0; i < 5; i++ )
    {    	
        Delay4us();
        if( i2c_SDALo() )
            return TRUE;
    }

    if( i2c_SDALo() )
        return TRUE;
    else
        return FALSE;

}
//============================================
Bool i2c_SendByte( BYTE value )
{
    BYTE i;
    Bool result;

    for( i = 0; i < 8; i++ ) // Send data via i2c pin
    {
        if( value & BIT7 )
            Set_i2c_SDA();
        else
            Clr_i2c_SDA();
        Delay4us();
        Set_i2c_SCL();
        Delay4us();
        value <<= 1;
        Clr_i2c_SCL();
    }
    Set_i2c_SDA();
    result = Wait_i2C_Ack();
    Set_i2c_SCL();
    Delay4us();
    Clr_i2c_SCL();
    Delay4us();
    Clr_i2c_SDA();

    return result;
}

//============================================
BYTE i2c_ReceiveByte( const Bool ack )
{
    BYTE i;
    BYTE value = 0;

    for( i = 0; i < 8; i++ )
    {
        value <<= 1;
        Set_i2c_SDA();
        Delay4us();
        Set_i2c_SCL();
        Delay4us();
        if( i2c_SDAHi() )
            value |= BIT0;
        Clr_i2c_SCL();
    }
    if( ack )
    {
        Clr_i2c_SDA();
    }
    else
    {
        Set_i2c_SDA();
    }
    Delay4us();
    Set_i2c_SCL();
    Delay4us();
    Clr_i2c_SCL();

    return value;
}

Bool i2c_BurstWrite( BYTE count, BYTE *buffer )
{
    while( count-- )
    {
        if( i2c_SendByte( *( buffer++ ) ) == FALSE )
            return FALSE;
    }
    return TRUE;
}
Bool i2c_BurstRead( BYTE count, BYTE * buffer )
{
    BYTE i;

    for( i = 0; i < count - 1; i++ )
        *( buffer + i ) = i2c_ReceiveByte( 1 );
    *( buffer + i ) = i2c_ReceiveByte( 0 );
    i2c_Stop();
    return TRUE;
}

#if (EEPROM_TYPE >= EEPROM_24C32)//modify by smc.lmw 20160526

Bool i2c_MasterStart( I2C_Direction direct, BYTE addr )
{
    #define NVRAM_DEVICE    0xA0

    BYTE u8Retry=5;
    BYTE addr1 = addr;
    //BYTE u8NvRamID=NVRAM_DEVICE;
    if (direct==I2C_READ) // Set I2C direction bit.
    {
        addr1=NVRAM_DEVICE;// get 0xA0
        addr1|=BIT0;
    }
    else
        addr1&=~BIT0;

    while (u8Retry--)
    {
        if (i2c_Start()==FALSE)
        {
           ForceDelay1ms(1);
            continue;
        } 
        if(direct==I2C_READ)
        {
            if (i2c_SendByte(addr1)==TRUE) // send address success
                return TRUE;
        }
        else
        {
            if (i2c_SendByte(NVRAM_DEVICE)==TRUE) // send address success
            {

                if (i2c_SendByte(ucADDR_HI_BYTE)==TRUE) // send address success
                    return TRUE;
            }
        }
        i2c_Stop();
        ForceDelay1ms(2);
    }
    return FALSE;
}

#define PageWriteLength     16
#define PageWriteDelayTime  10
void i2c_WriteTBL( BYTE deviceID, WORD addr, BYTE *buffer, BYTE count )
{
    BYTE slaveAddr;
    BYTE wordAddr;
    BYTE offset = 0;
    BYTE tempSize;
    Bool succ = TRUE;

    while( count )
    {
        slaveAddr = i2cSlaveAddr( deviceID, addr + offset );
        wordAddr = i2cWordAddr( addr + offset );
        ucADDR_HI_BYTE = ((addr + offset)>>8); //modify by smc.lmw 20160526
        if( i2c_MasterStart( I2C_WRITE, slaveAddr ) == FALSE )
            break;
        if( i2c_BurstWrite( 1, &wordAddr ) == FALSE )
            break;
        tempSize = PageWriteLength - ( wordAddr & ( PageWriteLength - 1 ) );
        if( count > tempSize )
        {
            if( i2c_BurstWrite( tempSize, ( BYTE* )( buffer + offset ) ) == FALSE )
                break;
            i2c_Stop();
            count -= tempSize;
            offset += tempSize;
        }
        else if( count > PageWriteLength )
        {
            if( i2c_BurstWrite( 0x10, ( BYTE* )( buffer + offset ) ) == FALSE )
                break;
            i2c_Stop();
            count -= PageWriteLength;
            offset += PageWriteLength;
        }
        else
        {
            if( i2c_BurstWrite( count, ( BYTE* )( buffer + offset ) ) == FALSE )
                break;
            i2c_Stop();
            count = 0;
        }
        ForceDelay1ms( PageWriteDelayTime );
    }
    i2c_Stop();
}

void i2c_ReadTBL( BYTE deviceID, WORD addr, BYTE *buffer, BYTE count )
{
    WORD retry = 5;
    BYTE slaveAddr;
    BYTE wordAddr;

    slaveAddr = i2cSlaveAddr( deviceID, addr );
    wordAddr = i2cWordAddr( addr );
     ucADDR_HI_BYTE = (addr>>8); //modify by smc.lmw 20160526
    while( retry-- )
    {
        if( i2c_MasterStart( I2C_WRITE, slaveAddr ) == FALSE )
            continue;
        if( i2c_BurstWrite( 1, &wordAddr ) == FALSE )
            continue;
        if( i2c_MasterStart( I2C_READ, slaveAddr ) == FALSE )
            continue;
        if( i2c_BurstRead( count, buffer ) == FALSE )
            continue;
        return;
    }
}

#else
Bool i2c_MasterStart( I2C_Direction direct, BYTE addr )
{
    BYTE retry = 3;

    if( direct == I2C_READ ) // Set I2C direction bit.
        addr |= BIT0;
    else
        addr &= ~BIT0;

    while( retry-- )
    {
        if( i2c_Start() == FALSE )
        {
            i2c_Stop();
            continue;
        }

        if( i2c_SendByte( addr ) == TRUE ) // send address success
            return TRUE;
        i2c_Stop();
        ForceDelay1ms( 2 );
    }
    return FALSE;
}

#define PageWriteLength     16
#define PageWriteDelayTime  10
void i2c_WriteTBL( BYTE deviceID, WORD addr, BYTE *buffer, BYTE count )
{
    BYTE slaveAddr;
    BYTE wordAddr;
    BYTE offset = 0;
    BYTE tempSize;
    Bool succ = TRUE;

    while( count )
    {
        slaveAddr = i2cSlaveAddr( deviceID, addr + offset );
        wordAddr = i2cWordAddr( addr + offset );
        if( i2c_MasterStart( I2C_WRITE, slaveAddr ) == FALSE )
            break;
        if( i2c_BurstWrite( 1, &wordAddr ) == FALSE )
            break;
        tempSize = PageWriteLength - ( wordAddr & ( PageWriteLength - 1 ) );
        if( count > tempSize )
        {
            if( i2c_BurstWrite( tempSize, ( BYTE* )( buffer + offset ) ) == FALSE )
                break;
            i2c_Stop();
            count -= tempSize;
            offset += tempSize;
        }
        else if( count > PageWriteLength )
        {
            if( i2c_BurstWrite( 0x10, ( BYTE* )( buffer + offset ) ) == FALSE )
                break;
            i2c_Stop();
            count -= PageWriteLength;
            offset += PageWriteLength;
        }
        else
        {
            if( i2c_BurstWrite( count, ( BYTE* )( buffer + offset ) ) == FALSE )
                break;
            i2c_Stop();
            count = 0;
        }
        ForceDelay1ms( PageWriteDelayTime );
    }
    i2c_Stop();
}

void i2c_ReadTBL( BYTE deviceID, WORD addr, BYTE *buffer, BYTE count )
{
    WORD retry = 5;
    BYTE slaveAddr;
    BYTE wordAddr;

    slaveAddr = i2cSlaveAddr( deviceID, addr );
    wordAddr = i2cWordAddr( addr );
    while( retry-- )
    {
        if( i2c_MasterStart( I2C_WRITE, slaveAddr ) == FALSE )
            continue;
        if( i2c_BurstWrite( 1, &wordAddr ) == FALSE )
            continue;
        if( i2c_MasterStart( I2C_READ, slaveAddr ) == FALSE )
            continue;
        if( i2c_BurstRead( count, buffer ) == FALSE )
            continue;
        return;
    }
}


#endif
#if !USEFLASH
void i2c_Start1(void)
{
    Set_i2c_SDA();
    Delay4us();
    Set_i2c_SCL();
    Delay4us();
    Clr_i2c_SDA();
    Delay4us();
    Clr_i2c_SCL();
}
void i2C_Intial(void)
{
    BYTE i, j;

    for(i=0 ;i < 20; i++)	// Generate SCL signals to reset EEPROM.
    {
        Set_i2c_SCL();
        Delay4us();
        Clr_i2c_SCL();
        Delay4us();
    }

    j = 20;

    while( j-- )	// Male EEPROM to Release I2C bus.
    {
        for(i=0 ;i < 9; i++)	// 9 STARTs
        {
            i2c_Start1();
        }
        i2c_Stop();
        ForceDelay1ms(10);

        if(i2c_SDAHi())
            break;
    }

}
#endif
#endif

#ifdef Internal_EDID_Write_To_24C02
#define NVRAM_RETRY     2

void NVRAM_i2c_Delay(void)
{
    BYTE i;
    for(i=0; i<100; i++) // 100: 59K
        _nop_();
}

#define NVRAM_i2cSCL_Chk()

Bool NVRAM_CheckIICAck(void)
{
    BYTE i;
    for(i=0; i<100; i++)
    {
        if(NVRAM_i2c_SDA_IS_LO())
            return TRUE;
        _nop_();
        _nop_();
        _nop_();
        _nop_();
    }

    if(NVRAM_i2c_SDA_IS_LO())
        return TRUE;
    else
        return FALSE;
    
}

//====================================
// Setup i2c Start condition
Bool NVRAM_i2c_Start(void)
{
  Set_NVRAM_i2c_SDA();
  NVRAM_i2c_Delay();
  Set_NVRAM_i2c_SCL();
  NVRAM_i2c_Delay();
  NVRAM_i2cSCL_Chk();
  if (NVRAM_i2c_SDA_IS_LO() || NVRAM_i2c_SCL_IS_LO())
    return FALSE;
  Clr_NVRAM_i2c_SDA();
  NVRAM_i2c_Delay();
  Clr_NVRAM_i2c_SCL();
  return TRUE;
}
//=============================================
// Setup i2c Stop condition
void NVRAM_i2c_Stop(void)
{ // SCL=L, SDA=L, Stop condition.
  Clr_NVRAM_i2c_SCL();
  NVRAM_i2c_Delay();
  Clr_NVRAM_i2c_SDA();  
  NVRAM_i2c_Delay();
  Set_NVRAM_i2c_SCL();
  NVRAM_i2c_Delay();
  NVRAM_i2cSCL_Chk();
  Set_NVRAM_i2c_SDA();

}

//============================================
Bool NVRAM_i2c_SendByte(BYTE value)
{ BYTE i;
  Bool result;

  for (i=0; i<8; i++) // Send data via i2c pin
    { if (value&BIT7)
        Set_NVRAM_i2c_SDA();
      else
      	Clr_NVRAM_i2c_SDA();      
       NVRAM_i2c_Delay();
      Set_NVRAM_i2c_SCL();    
      NVRAM_i2c_Delay();
      NVRAM_i2cSCL_Chk();
      value<<=1;
      Clr_NVRAM_i2c_SCL(); 
      
    }
  Set_NVRAM_i2c_SDA();
  NVRAM_i2c_Delay();
  result = NVRAM_CheckIICAck();
  Set_NVRAM_i2c_SCL();
  NVRAM_i2c_Delay();
  NVRAM_i2cSCL_Chk();
  Clr_NVRAM_i2c_SCL();
  NVRAM_i2c_Delay();
  Clr_NVRAM_i2c_SDA();

  return result;
}

//============================================
BYTE NVRAM_i2c_ReceiveByte(const Bool ack)
{ BYTE i;
  BYTE value = 0;

  for (i=0; i<8; i++)
    { value<<=1;
      Set_NVRAM_i2c_SDA();
#if Enable_Cache// 090417
      NVRAM_i2c_Delay();
#endif
      Set_NVRAM_i2c_SCL();
      NVRAM_i2c_Delay();
      NVRAM_i2cSCL_Chk();
      if (NVRAM_i2c_SDA_IS_HI())
      	value|=BIT0;
      Clr_NVRAM_i2c_SCL();
    }
  if (ack)
    Clr_NVRAM_i2c_SDA();
  else
    Set_NVRAM_i2c_SDA();
  NVRAM_i2c_Delay();
  Set_NVRAM_i2c_SCL();
  NVRAM_i2c_Delay();
  NVRAM_i2cSCL_Chk();
  Clr_NVRAM_i2c_SCL();
  
  return value;  	
}

Bool NVRAM_i2c_MasterStart(I2C_Direction direct, BYTE addr)
{
    BYTE retry=NVRAM_RETRY;

  if (direct==I2C_READ) // Set I2C direction bit.
    addr|=BIT0;
  else
    addr&=~BIT0;
  
  while (retry--)
    { if (NVRAM_i2c_Start()==FALSE)
        continue;
      
      if (NVRAM_i2c_SendByte(addr)==TRUE) // send address success
        {
        //printMsg("SendByte TRUE");
        return TRUE;
        }
      NVRAM_i2c_Stop();
      
      //printMsg("SendByte Fail");
      ForceDelay1ms(2);
    }
  return FALSE;
}

#if EEPROM_TYPE >= EEPROM_24C32
#define	NVRAMPageWriteLength		32
#else
#define	NVRAMPageWriteLength		16
#endif
#define	NVRAMPageWriteDelayTime	12 //edit by lizzie,100429.for the problem: use HDMI/VGA Cable write all port EDID ,the time to write 32byte is too long

#if EEPROM_TYPE >= EEPROM_24C32
void rmSWPageWrite(WORD wIndex, BYTE *pBuf, BYTE ucBufLen)
{
	BYTE ucDummy; // loop dummy
	//CHECK_PROTECT;
	ucDummy = NVRAM_RETRY;
	while(ucDummy--)
	{
		if (NVRAM_i2c_MasterStart(I2C_WRITE, 0xA0) == FALSE)
			continue;

		if (NVRAM_i2c_SendByte(HIBYTE(wIndex)) == FALSE) // check non-acknowledge
			continue;

		if (NVRAM_i2c_SendByte(LOBYTE(wIndex)) == FALSE) // check non-acknowledge
			continue;

		while(ucBufLen--) // loop of writting data
		{
			NVRAM_i2c_SendByte(*pBuf); // send byte

			pBuf++; // next byte pointer
		} // while

		break;
	} // while

	NVRAM_i2c_Stop();
}

void rmSWBurstReadBytes(WORD wIndex, BYTE *pBuf, BYTE ucBufLen)
{
    BYTE ucDummy; // loop dummy

    ucDummy = NVRAM_RETRY;
    while(ucDummy--)
    {
        if (NVRAM_i2c_MasterStart(I2C_WRITE, 0xA0) == FALSE)
            continue;

        if (NVRAM_i2c_SendByte(HIBYTE(wIndex)) == FALSE) // check non-acknowledge
            continue;

        if (NVRAM_i2c_SendByte(LOBYTE(wIndex)) == FALSE) // check non-acknowledge
            continue;

        if (NVRAM_i2c_MasterStart(I2C_READ, 0xA0) == FALSE)
            continue;

        while(ucBufLen) // loop to burst read
        {
            ucBufLen--;
            *pBuf = NVRAM_i2c_ReceiveByte(ucBufLen); // receive byte
            pBuf++; // next byte pointer
        } // while

        break;
    } // while

    NVRAM_i2c_Stop();

    while(ucBufLen--) // if read error, fill 0xff
    {
        *pBuf = 0xff; // clear
        pBuf++; // next byte pointer
    } // while
}

void rmBurstWriteBytes(WORD wIndex, BYTE *pBuf, WORD ucBufLen)
{
    BYTE ucWriteCount; // write counter
    BYTE ucRemainLen; // i2c word address
	//CHECK_PROTECT;
    while(1)
    {
        // check page write roll over
        if (ucBufLen > NVRAMPageWriteLength)
            ucWriteCount = NVRAMPageWriteLength;
        else
            ucWriteCount = ucBufLen;

        // check word address higher than page write bits
        ucRemainLen = LOBYTE(wIndex) % NVRAMPageWriteLength;
        if (ucRemainLen) // check ramain
        {
            ucRemainLen = NVRAMPageWriteLength - ucRemainLen;
            if (ucWriteCount > ucRemainLen) // check not roll over
                ucWriteCount = ucRemainLen;
        }

        // EEPROM page write
        rmSWPageWrite(wIndex, pBuf, ucWriteCount);
		
        // check end of buffer
        ucBufLen -= ucWriteCount;
        if (ucBufLen == 0)
            break;

        wIndex += ucWriteCount; // next word address
        pBuf += ucWriteCount; // next buffer pointer
        
        ForceDelay1ms(NVRAMPageWriteDelayTime);//edit by lizzie,100429.for the problem: use HDMI/VGA Cable write all port EDID ,the time to write 32byte is too long

    } // while

}
#else
Bool NVRAM_i2c_BurstWrite(BYTE count, BYTE *buffer)
{ while (count--)
    { if (NVRAM_i2c_SendByte(*(buffer++))==FALSE)
        return FALSE;
    }
 return TRUE;
}

Bool NVRAM_i2c_BurstRead(BYTE count, BYTE * buffer)
{ BYTE i;

  for (i=0; i<count-1; i++)
    *(buffer+i)=NVRAM_i2c_ReceiveByte(1);
  *(buffer+i)=NVRAM_i2c_ReceiveByte(0);
  NVRAM_i2c_Stop();
  return TRUE;
}

 void NVRAM_i2c_WriteTBL(BYTE deviceID, WORD addr, BYTE *buffer, BYTE count)
{ BYTE slaveAddr;
  BYTE wordAddr;
  BYTE offset=0;
  BYTE tempSize;
  Bool succ=TRUE;
  
  while (count)
    { slaveAddr=i2cSlaveAddr(deviceID, addr+offset);
      wordAddr=i2cWordAddr(addr+offset);
      if (NVRAM_i2c_MasterStart(I2C_WRITE, slaveAddr)==FALSE)
      	break;
      if (NVRAM_i2c_BurstWrite(1, &wordAddr)==FALSE)
      	break;
      tempSize=NVRAMPageWriteLength-(wordAddr&(NVRAMPageWriteLength-1));//0x10-(wordAddr&0xF);
      if (count>tempSize)
      	{ if (NVRAM_i2c_BurstWrite(tempSize, (BYTE*)(buffer+offset))==FALSE)
      	    break;
      	  NVRAM_i2c_Stop();
      	  count-=tempSize;
      	  offset+=tempSize;
      	}
      else if (count>NVRAMPageWriteLength)//0x10)
      	{ if (NVRAM_i2c_BurstWrite(0x10, (BYTE*)(buffer+offset))==FALSE)
            break;
      	  NVRAM_i2c_Stop();
      	  count-=NVRAMPageWriteLength;//0x10;
      	  offset+=NVRAMPageWriteLength;//0x10;
      	}
      else
      	{ if (NVRAM_i2c_BurstWrite(count, (BYTE*)(buffer+offset))==FALSE)
      	    break;
      	    NVRAM_i2c_Stop();
          count=0;
      	}
      ForceDelay1ms(NVRAMPageWriteDelayTime);//edit by lizzie,100429.for the problem: use HDMI/VGA Cable write all port EDID ,the time to write 32byte is too long
    }
  NVRAM_i2c_Stop();
}

void NVRAM_i2c_ReadTBL(BYTE deviceID, WORD addr, BYTE *buffer, BYTE count)
{  
   WORD retry=NVRAM_RETRY;
  BYTE slaveAddr;
  BYTE wordAddr;

  slaveAddr=i2cSlaveAddr(deviceID, addr);
  wordAddr=i2cWordAddr(addr);
  while (retry--)
    { if (NVRAM_i2c_MasterStart(I2C_WRITE, slaveAddr)==FALSE)
        continue;
      if (NVRAM_i2c_BurstWrite(1, &wordAddr)==FALSE)
      	continue;
      if (NVRAM_i2c_MasterStart(I2C_READ, slaveAddr)==FALSE)
        continue;
      if (NVRAM_i2c_BurstRead(count, buffer)==FALSE)
      	continue;      
      return;
    }  
}

#endif


#endif

