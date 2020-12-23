
#include "global.h"

#if ENABLE_TOUCH_KEY
#if (MS_PM) // 120525 coding, modified to avoid iic clock too slow
#define CPK_IIC_DELAY_COUNT     ((g_bMcuPMClock)?(2):(30))
#else
#define CPK_IIC_DELAY_COUNT     (30)
#endif
void SK_i2c_Delay( void )
{
#message "please check touch key IIC clock speed"
    BYTE i;
    BYTE count = CPK_IIC_DELAY_COUNT;
    for( i = 0; i < count; i ++ ) // check iic clock freq
    {
        _nop_();
        _nop_();
        _nop_();
        _nop_();
    }
}

Bool SK_i2c_Wait_Ack( void )
{
    BYTE i;

    for( i = 0; i < 10; i++ )
    {
        if( SK_SDALo() )
            return TRUE;
        SK_i2c_Delay();
    }

    if( SK_SDALo() )
        return TRUE;
    else
        return FALSE;
}

Bool SK_i2c_Start( void )
{
    Set_SK_SDA();
    SK_i2c_Delay();
    Set_SK_SCL();
    SK_i2c_Delay();
    if( SK_SDALo() || SK_SCLLo() )
        return FALSE;
    Clr_SK_SDA();
    SK_i2c_Delay();
    Clr_SK_SCL();
    SK_i2c_Delay();
    return TRUE;
}
//=============================================
// Setup i2c Stop condition
void SK_i2c_Stop( void )
{
    // SCL=L, SDA=L, Stop condition.
    Clr_SK_SCL();
    SK_i2c_Delay();
    Clr_SK_SDA();
    SK_i2c_Delay();
    Set_SK_SCL();
    SK_i2c_Delay();
    Set_SK_SDA();
}

//============================================
Bool SK_i2c_SendByte( BYTE value )
{
    BYTE i;
    Bool result;

    for( i = 0; i < 8; i++ )
    {
        if( value & BIT7 )
            Set_SK_SDA();
        else
            Clr_SK_SDA();
        SK_i2c_Delay();
        Set_SK_SCL();
        value <<= 1;
        SK_i2c_Delay();
        Clr_SK_SCL();
    }
    Set_SK_SDA();
    SK_i2c_Delay();
    result = SK_i2c_Wait_Ack();
    Set_SK_SCL();
    SK_i2c_Delay();
    Clr_SK_SCL();
    SK_i2c_Delay();
    Clr_SK_SDA();
    return result;
}

//============================================
BYTE SK_i2c_ReceiveByte( const Bool ack )
{
    BYTE i;
    BYTE value = 0;

    for( i = 0; i < 8; i++ )
    {
        value <<= 1;
        Set_SK_SDA();
        SK_i2c_Delay();
        Set_SK_SCL();
        SK_i2c_Delay();
        if( SK_SDAHi() )
            value |= BIT0;
        Clr_SK_SCL();
        SK_i2c_Delay();
    }
    if( ack )
        Clr_SK_SDA();
    else
        Set_SK_SDA();
    SK_i2c_Delay();
    Set_SK_SCL();
    SK_i2c_Delay();
    Clr_SK_SCL();
    return value;
}

Bool SK_i2c_BurstWrite( BYTE count, BYTE *buffer )
{
    while( count-- )
    {
        if( SK_i2c_SendByte( *( buffer++ ) ) == FALSE )
            return FALSE;
    }
    return TRUE;
}
Bool SK_i2c_BurstRead( BYTE count, BYTE * buffer )
{
    BYTE i;

    for( i = 0; i < count - 1; i++ )
        *( buffer + i ) = SK_i2c_ReceiveByte( 1 );
    *( buffer + i ) = SK_i2c_ReceiveByte( 0 );
    SK_i2c_Stop();
    return TRUE;
}

Bool SK_i2c_MasterStart( I2C_Direction direct, BYTE addr )
{
    BYTE retry = 3;

    if( direct == I2C_READ ) // Set I2C direction bit.
        addr |= BIT0;
    else
        addr &= ~BIT0;

    while( retry-- )
    {
        if( SK_i2c_Start() == FALSE )
        {
            SK_i2c_Stop();
            ForceDelay1ms( 1 );
            continue;
        }
        if( SK_i2c_SendByte( addr ) == TRUE ) // send address success
            return TRUE;
        SK_i2c_Stop();
        ForceDelay1ms( 1 );
    }
    return FALSE;
}

static void I2CInit(void)
{
    Set_SK_SCL();
    Set_SK_SDA();
    isInit = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//------I2C bus hardware reset , Should be let SCL=0(low) over the 4ms.----//
/////////////////////////////////////////////////////////////////////////////
void IT7230_I2C_Reset(void) 
{	
    Set_SK_SCL();
    ForceDelay1ms(2);
    Clr_SK_SCL();
    ForceDelay1ms(6);
    Set_SK_SCL();
    ForceDelay1ms(2);
}

BOOL IT7230_I2C_WriteWord(const BYTE registerStartAddr, const WORD WriteData)
{
    BYTE WriteDataArray[3];

    WriteDataArray[0] = registerStartAddr;
    WriteDataArray[1] = (WriteData & 0x00FF);
    WriteDataArray[2] = (WriteData & 0xFF00) >>8;

    if (!isInit)
        I2CInit();

    if( SK_i2c_MasterStart( I2C_WRITE, I2C_DEVICE_ADDR ) == FALSE )
        return FALSE;

    if( SK_i2c_BurstWrite( 3, WriteDataArray ) == FALSE )
        return FALSE;

    SK_i2c_Stop();
    
    return TRUE;
}

BOOL IT7230_I2C_ReadWord(const BYTE registerStartAddr, WORD* recWord)
{
    BYTE WriteDataArray[2];
    WriteDataArray[0] = registerStartAddr;

    if (!isInit)
        I2CInit();
    
    if( SK_i2c_MasterStart( I2C_WRITE, I2C_DEVICE_ADDR ) == FALSE )
        return FALSE;

    if( SK_i2c_BurstWrite( 1, WriteDataArray ) == FALSE )
        return FALSE;

    if( SK_i2c_MasterStart( I2C_READ, I2C_DEVICE_ADDR ) == FALSE )
        return FALSE;

    SK_i2c_BurstRead(2, WriteDataArray);
    *recWord = (WORD)(WriteDataArray[1]<<8)|WriteDataArray[0];

    SK_i2c_Stop();
    
    return TRUE;
}

BOOL IT7230_I2C_Device_Addr_Check(void)
{
    if( SK_i2c_MasterStart( I2C_WRITE, I2C_DEVICE_ADDR ) == FALSE )
        return FALSE;
    
    SK_i2c_Stop();
    
    return TRUE;
}

#endif


