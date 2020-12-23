
///////////////////////////////////////
// DRVADJUST_V VERSION: V02
////////////////////////////////////////
#include "global.h"

void mStar_AdjustUserPrefBlacklevel( BYTE Redblacklevel, BYTE Greenblacklevel, BYTE Blueblacklevel )
{
    WORD XDATA Redblacklevel_Real;
    WORD XDATA Greenblacklevel_Real;
    WORD XDATA Blueblacklevel_Real;
    
    Redblacklevel = (( WORD )Redblacklevel * ( MaxBlackLevelValue - MinBlackLevelValue ) ) / 100 + MinBlackLevelValue;
    Greenblacklevel = (( WORD )Greenblacklevel * ( MaxBlackLevelValue - MinBlackLevelValue ) ) / 100 + MinBlackLevelValue;
    Blueblacklevel = (( WORD )Blueblacklevel * ( MaxBlackLevelValue - MinBlackLevelValue ) ) / 100 + MinBlackLevelValue;

    Redblacklevel_Real=Redblacklevel<<3;
    Greenblacklevel_Real=Greenblacklevel<<3;
    Blueblacklevel_Real=Blueblacklevel<<3;
    
    msWriteByteMask(SC8_61, BIT7, BIT7);//Enable main bri function
    msWrite2Byte(SC8_62, Redblacklevel_Real-0x400);
    msWrite2Byte(SC8_64, Greenblacklevel_Real-0x400);
    msWrite2Byte(SC8_66, Blueblacklevel_Real-0x400);
}
