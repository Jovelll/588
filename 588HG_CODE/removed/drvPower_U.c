
///////////////////////////////////////
// DRVPOWER_U VERSION: V02
////////////////////////////////////////
#include "global.h"

void Power_ModCtrl(BYTE ucSwitch)
{
    BYTE ucOnOff;
    BYTE u8Bank=msReadByte(REG_3200);

    ucOnOff = (ucSwitch)? 0xFF:0x00;
    msWriteByte(REG_3200,0x00);
    msWriteByteMask(REG_3250,(~ucOnOff)&(((LVDS_CHANNEL)<<1)|BIT0),(((LVDS_CHANNEL)<<1)|BIT0));
    msWriteByteMask(REG_3264,ucOnOff&(((LVDS_CHANNEL)|BIT4)),(((LVDS_CHANNEL)|BIT4)) );
    msWriteByteMask(REG_3265,(~ucOnOff)&(BIT6|BIT2),BIT6|BIT2);
    msWriteByte(REG_3200,u8Bank);
}
