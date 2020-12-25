
#include "global.h"

#if CHIP_ID == CHIP_TSUMU
#include "msFB_U.c"
#elif CHIP_ID == CHIP_TSUMB
#include "msFB_B.c"
#elif CHIP_ID == CHIP_TSUM2
#include "msFB_2.c"
#elif CHIP_ID == CHIP_TSUMC || CHIP_ID == CHIP_TSUMK
#include "msFB_C.c"
#elif CHIP_ID == CHIP_TSUMD
#include "msFB_D.c"
#elif (CHIP_ID == CHIP_TSUMV || CHIP_ID == CHIP_TSUMY || CHIP_ID == CHIP_TSUMJ || CHIP_ID == CHIP_TSUM9 ||CHIP_ID == CHIP_TSUMF) 
#else
#message "please implement msFB for new chip"
#endif

BYTE code msFBNullData[] = {0};

void msFBDummy(void)
{
    BYTE i = msFBNullData[0];
}

