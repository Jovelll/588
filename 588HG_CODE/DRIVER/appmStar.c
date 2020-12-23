
#include "global.h"

#if CHIP_ID == CHIP_TSUMU
#include "appmStar_U.c"
#elif CHIP_ID == CHIP_TSUMV
#include "appmStar_V.c"
#elif CHIP_ID == CHIP_TSUMY
#include "appmStar_Y.c"
#elif CHIP_ID == CHIP_TSUMB
#include "appmStar_B.c"
#elif CHIP_ID==CHIP_TSUM2
#include "appmstar_2.c"
#elif CHIP_ID==CHIP_TSUMC ||  CHIP_ID==CHIP_TSUMK
#include "appmstar_C.c"
#elif CHIP_ID==CHIP_TSUMD
#include "appmstar_D.c"
#elif CHIP_ID==CHIP_TSUMJ
#include "appmstar_J.c"
#elif CHIP_ID==CHIP_TSUM9
#include "appmstar_9.c"
#elif CHIP_ID==CHIP_TSUMF
#include "appmstar_F.c"

#else
#message "please implement appmStar for new chip"
#endif

BYTE code appmSstarNullData[] = {0};
void msapmStarDummy(void)
{
    BYTE xdata i = appmSstarNullData[0];
}

