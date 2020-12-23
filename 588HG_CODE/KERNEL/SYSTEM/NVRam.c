#include "types.h"
#include "i2cdef.h"
#include "i2c.h"
#include "board.h"
#include "global.h"
#include "mstar.h"
#include "ms_reg.h"
#include "Ms_rwreg.h"
#include "gpio_def.h"
#if !USEFLASH
#define NVRAM_DEVICE    0xA0
void NVRam_WriteByte( WORD addr, BYTE value )
{
    Clr_EEPROM_WP(); //wumaozhong 20050331
    i2c_WriteTBL( NVRAM_DEVICE, addr, &value, 1 );
    Set_EEPROM_WP(); //wumaozhong 20050331
}

/*
void NVRam_WriteWord(WORD addr, WORD value)
{ i2c_WriteTBL(NVRAM_DEVICE, addr, (BYTE*)&value, 2);
}
 */
void NVRam_ReadByte( WORD addr, BYTE *value )
{
    i2c_ReadTBL( NVRAM_DEVICE, addr, value, 1 );
}

void NVRam_WriteTbl( WORD addr, BYTE *buffer, BYTE count )
{
    Clr_EEPROM_WP(); //wumaozhong 20050331
    i2c_WriteTBL( NVRAM_DEVICE, addr, buffer, count );
    Set_EEPROM_WP(); //wumaozhong 20050331
}

void NVRam_ReadTbl( WORD addr, BYTE *buffer, BYTE count )
{
    i2c_ReadTBL( NVRAM_DEVICE, addr, buffer, count );
}

#if 0
void Device_WriteByte( BYTE deviceAddr, WORD addr, BYTE value )
{
    i2c_WriteTBL( NVRAM_DEVICE, addr, &value, 1 );
}
void Device_ReadByte( BYTE deviceAddr, WORD addr, BYTE *value )
{
    i2c_ReadTBL( deviceAddr, addr, value, 1 );
}
#endif
#endif

#ifdef Internal_EDID_Write_To_24C02
#define NVRAM_DEVICE    0xA0
void NVRam_WriteByte( WORD addr, BYTE value )
{
    Clr_EEPROM_WP(); //wumaozhong 20050331
    NVRAM_i2c_WriteTBL(NVRAM_DEVICE, addr, &value, 1);
    Set_EEPROM_WP(); //wumaozhong 20050331
}

void NVRam_ReadByte( WORD addr, BYTE *value )
{
    NVRAM_i2c_ReadTBL(NVRAM_DEVICE, addr, value, 1);
}

#endif

