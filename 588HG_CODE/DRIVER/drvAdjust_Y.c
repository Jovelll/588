
///////////////////////////////////////
// DRVADJUST_Y VERSION: V02
////////////////////////////////////////
#include "global.h"

void mStar_AdjustUserPrefBlacklevel( BYTE Redblacklevel, BYTE Greenblacklevel, BYTE Blueblacklevel )
{
    Redblacklevel = (( WORD )Redblacklevel * ( MaxBlackLevelValue - MinBlackLevelValue ) ) / 100 + MinBlackLevelValue;
    Greenblacklevel = (( WORD )Greenblacklevel * ( MaxBlackLevelValue - MinBlackLevelValue ) ) / 100 + MinBlackLevelValue;
    Blueblacklevel = (( WORD )Blueblacklevel * ( MaxBlackLevelValue - MinBlackLevelValue ) ) / 100 + MinBlackLevelValue;

    msWriteByte(SC0_2B, Redblacklevel);
    msWriteByte(SC0_2C, Greenblacklevel);
    msWriteByte(SC0_2D, Blueblacklevel);
}
