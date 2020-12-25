
///////////////////////////////////////
// DRVPOWER_Y VERSION: V02
////////////////////////////////////////
#include "global.h"

void Power_ModCtrl(BYTE ucSwitch)
{
    if (ucSwitch)
    {
        msWriteByte(REG_3008, BIT3 | BIT2 | BIT0);  // enable LVDS mode
        msWriteByte(REG_3009, BIT3 | BIT0);  // enable LVDS mode
    }
    else
    {
        msWriteByte(REG_3008, BIT0);    // tri-state all output panel signal
        msWriteByte(REG_3009, BIT0);    // tri-state all output panel signal
    }
}
