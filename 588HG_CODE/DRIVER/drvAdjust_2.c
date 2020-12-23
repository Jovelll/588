
///////////////////////////////////////
// DRVADJUST_V VERSION: V02
////////////////////////////////////////
#include "global.h"

void mStar_AdjustUserPrefBlacklevel( BYTE Redblacklevel, BYTE Greenblacklevel, BYTE Blueblacklevel )
{
    Redblacklevel = (( WORD )Redblacklevel * ( MaxBlackLevelValue - MinBlackLevelValue ) ) / 100 + MinBlackLevelValue;
    Greenblacklevel = (( WORD )Greenblacklevel * ( MaxBlackLevelValue - MinBlackLevelValue ) ) / 100 + MinBlackLevelValue;
    Blueblacklevel = (( WORD )Blueblacklevel * ( MaxBlackLevelValue - MinBlackLevelValue ) ) / 100 + MinBlackLevelValue;

    msWriteByteMask(SC8_61, BIT7, BIT7);//Enable main bri function
    msWrite2Byte(SC8_62, Redblacklevel-0x80);
    msWrite2Byte(SC8_64, Greenblacklevel-0x80);
    msWrite2Byte(SC8_66, Blueblacklevel-0x80);
}
