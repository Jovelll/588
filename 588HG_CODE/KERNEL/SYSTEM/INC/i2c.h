
/****add by smc.lmw 20160526******/
#define EEPROM_24C02    0 
#define EEPROM_24C04    1
#define EEPROM_24C08    2
#define EEPROM_24C16    3
#define EEPROM_24C32    4
#define EEPROM_24C64    5
#define EEPROM_24C128   6
#define EEPROM_24C256   7



#define EEPROM_TYPE  EEPROM_24C02//EEPROM_24C16   


extern void i2c_WriteTBL( BYTE deviceID, WORD addr, BYTE *buffer, BYTE count );
extern void i2c_ReadTBL( BYTE deviceID, WORD addr, BYTE *buffer, BYTE count );

extern void NVRAM_i2c_WriteTBL(BYTE deviceID, WORD addr, BYTE *buffer, BYTE count);
extern void NVRAM_i2c_ReadTBL(BYTE deviceID, WORD addr, BYTE *buffer, BYTE count);

