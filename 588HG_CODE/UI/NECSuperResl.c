
#include "board.h"
#ifdef ENABLE_NECSUPERRESOLUTION

#define NEC_SUPER_RESOLUTION_C
#include "types.h"
#include "necsuperresl.h"
#include <intrins.h>
#include "misc.h"
#include "i2cdef.h"
void NEC_i2c_Delay( void )
{
    BYTE xdata i;
    for( i = 0; i < 8; i++ )
    {
        _nop_();
        _nop_();
        _nop_();
        _nop_();
    }


}
//====================================
// Setup i2c Start condition
Bool NEC_i2c_Start( void )
{
    Set_NEC_i2c_SDA();
    Set_NEC_i2c_SCL();
    NEC_i2c_Delay();
    if( NEC_i2c_SDALo() || NEC_i2c_SCLLo() )
        return FALSE;
    Clr_NEC_i2c_SDA();
    NEC_i2c_Delay();
    Clr_NEC_i2c_SCL();
    return TRUE;
}
//=============================================
// Setup i2c Stop condition
void NEC_i2c_Stop( void )
{
    // SCL=L, SDA=L, Stop condition.
    Clr_NEC_i2c_SCL();
    Clr_NEC_i2c_SDA();
    NEC_i2c_Delay();
    Set_NEC_i2c_SCL();
    NEC_i2c_Delay();
    Set_NEC_i2c_SDA();
    NEC_i2c_Delay();
}

Bool WaitNECAck( void )
{
    BYTE xdata i;
    for( i = 0; i < 5; i++ )
    {
        if( NEC_i2c_SDALo() )
            return TRUE;
        NEC_i2c_Delay();
    }

    if( NEC_i2c_SDALo() )
        return TRUE;
    else
        return FALSE;
}
//============================================
Bool NEC_i2c_SendByte( BYTE value )
{
    BYTE xdata i;
    Bool result;

    for( i = 0; i < 8; i++ ) // Send data via i2c pin
    {
        if( value & BIT7 )
            Set_NEC_i2c_SDA();
        else
            Clr_NEC_i2c_SDA();
        NEC_i2c_Delay();
        Set_NEC_i2c_SCL();
        NEC_i2c_Delay();
        value <<= 1;
        Clr_NEC_i2c_SCL();
    }
    Set_NEC_i2c_SDA();
    result = WaitNECAck();
    Set_NEC_i2c_SCL();
    NEC_i2c_Delay();
    Clr_NEC_i2c_SCL();
    NEC_i2c_Delay();
    Clr_NEC_i2c_SDA();

    return result;
}

Bool NEC_i2c_BurstWrite( BYTE *buffer, BYTE count )
{
    while( count-- )
    {
        if( NEC_i2c_SendByte( *buffer++ ) == FALSE )
            return FALSE;
    }
    return TRUE;
}

//============================================

BYTE NEC_i2c_ReceiveByte( Bool ack )
{
    BYTE xdata i;
    BYTE xdata value = 0;

    for( i = 0; i < 8; i++ )
    {
        value <<= 1;
        Set_NEC_i2c_SDA();
        NEC_i2c_Delay();
        Set_NEC_i2c_SCL();
        NEC_i2c_Delay();
        if( NEC_i2c_SDAHi() )
            value |= BIT0;
        Clr_NEC_i2c_SCL();
    }
    if( ack )
        Clr_NEC_i2c_SDA();
    else
        Set_NEC_i2c_SDA();
    NEC_i2c_Delay();
    Set_NEC_i2c_SCL();
    NEC_i2c_Delay();
    Clr_NEC_i2c_SCL();

    return value;
}

Bool NEC_i2c_BurstRead( BYTE *buffer, BYTE count )
{
    BYTE xdata i;

    for( i = 0; i < count - 1; i++ )
        *( buffer + i ) = NEC_i2c_ReceiveByte( 1 );
    *( buffer + i ) = NEC_i2c_ReceiveByte( 0 );
    NEC_i2c_Stop();
    return TRUE;
}

Bool NEC_i2c_MasterStart( I2C_Direction direct, BYTE addr )
{
    BYTE xdata retry = 2;

    if( direct == I2C_READ ) // Set I2C direction bit.
        addr |= BIT0;
    else
        addr &= ~BIT0;

    while( retry-- )
    {
        if( NEC_i2c_Start() == FALSE )
        {
            NEC_i2c_Stop();
            continue;
        }

        if( NEC_i2c_SendByte( addr ) == TRUE ) // send address success
            return TRUE;
        NEC_i2c_Stop();
        ForceDelay1ms( 10 );
    }
    return FALSE;
}

Bool D9281_Set_Reg( BYTE regaddr, WORD val )
{
    BYTE xdata tempbuf[3];

    if( NEC_i2c_MasterStart( I2C_WRITE, NEC_ADDR ) == FALSE )
        return FALSE;

    tempbuf[0] = regaddr;
    tempbuf[1] = val >> 8;
    tempbuf[2] = val & 0x00ff;

    if( NEC_i2c_BurstWrite( tempbuf, 3 ) == FALSE )
    {
        NEC_i2c_Stop();
        return FALSE;
    }

    NEC_i2c_Stop();
    return TRUE;

}

#if 1
WORD D9281_Get_Reg( BYTE regaddr )
{
    BYTE xdata tempbuf[1];
    WORD xdata value;

    if( NEC_i2c_MasterStart( I2C_WRITE, NEC_ADDR ) == FALSE )
        return FALSE;

    tempbuf[0] = regaddr;

    if( NEC_i2c_BurstWrite( tempbuf, 1 ) == FALSE )
    {
        NEC_i2c_Stop();
        return FALSE;
    }

    NEC_i2c_Stop();

    if( NEC_i2c_MasterStart( I2C_READ, NEC_ADDR ) == FALSE )
        return FALSE;

    NEC_i2c_BurstRead( tempbuf, 2 );
    value = ( WORD )( tempbuf[0] << 8 ) | tempbuf[1];

    NEC_i2c_Stop();
    return value;
}

#else
Bool D9281_Get_Reg( WORD regaddr, WORD *val )
{
    BYTE tempbuf[2];

    if( NEC_i2c_MasterStart( I2C_WRITE, NEC_ADDR ) == FALSE )
        return FALSE;

    tempbuf[0] = regaddr >> 8;
    tempbuf[1] = regaddr & 0x00ff;

    if( NEC_i2c_BurstWrite( tempbuf, 2 ) == FALSE )
    {
        NEC_i2c_Stop();
        return FALSE;
    }

    NEC_i2c_Stop();

    if( NEC_i2c_MasterStart( I2C_READ, NEC_ADDR ) == FALSE )
        return FALSE;

    NEC_i2c_BurstRead( tempbuf, 2 );
    *val = ( WORD )( tempbuf[0] << 8 ) | tempbuf[1];

    NEC_i2c_Stop();
    return TRUE;
}
#endif



#endif

