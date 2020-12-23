
#ifdef NEC_SUPER_RESOLUTION_C
#define NEC_SUPER_RESOLUTION_DEC
#else
#define NEC_SUPER_RESOLUTION_DEC extern
#endif

#ifndef NEC_SUPER_RESOLUTION_H
#define NEC_SUPER_RESOLUTION_H
#include "types.h"

#define NEC_ADDR  0x40

#define NEC_IIC_SCL   P1_0   // Rex 100706
#define NEC_IIC_SDA   P1_1

#define Set_NEC_i2c_SCL()   (NEC_IIC_SCL=1)
#define Clr_NEC_i2c_SCL()   (NEC_IIC_SCL=0)
#define Set_NEC_i2c_SDA()   (NEC_IIC_SDA=1)
#define Clr_NEC_i2c_SDA()   (NEC_IIC_SDA=0)

#define NEC_i2c_SCLLo() (!NEC_IIC_SCL)
#define NEC_i2c_SDALo() (!NEC_IIC_SDA)

#define NEC_i2c_SDAHi()   (NEC_IIC_SDA)

//NEC_SUPER_RESOLUTION_DEC BYTE xdata NECStatus;
//#define InitNECBit    BIT0
//#define bInitNECFlag            (NECStatus&InitNECBit)
//#define Set_bInitNECFlag()  (NECStatus|=InitNECBit)
//#define Clr_bInitNECFlag()  (NECStatus&=~InitNECBit)

NEC_SUPER_RESOLUTION_DEC BYTE xdata Regaddr;
NEC_SUPER_RESOLUTION_DEC WORD xdata wRegvalue;
NEC_SUPER_RESOLUTION_DEC Bool D9281_Set_Reg( BYTE regaddr, WORD val );
NEC_SUPER_RESOLUTION_DEC WORD D9281_Get_Reg( BYTE regaddr );

//NEC_SUPER_RESOLUTION_DEC Bool D9281_Get_Reg(WORD regaddr, WORD *val);
//NEC_SUPER_RESOLUTION_DEC Bool NEC_i2c_WriteTable(BYTE *buf, BYTE count);
//NEC_SUPER_RESOLUTION_DEC Bool NEC_i2c_ReadTable(BYTE *buf, BYTE count);



#endif
