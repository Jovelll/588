
#include "global.h"
#if Enable_Gamma
#if CHIP_ID == CHIP_TSUMU
#include "drvGamma_U.c"
#elif CHIP_ID == CHIP_TSUMV
#include "drvGamma_V.c"
#elif CHIP_ID == CHIP_TSUMY
#include "drvGamma_Y.c"
#elif CHIP_ID == CHIP_TSUMB
#include "drvGamma_B.c"
#elif CHIP_ID == CHIP_TSUM2
#include "drvGamma_2.c"
#elif (CHIP_ID == CHIP_TSUMC || CHIP_ID == CHIP_TSUMJ || CHIP_ID == CHIP_TSUM9|| CHIP_ID == CHIP_TSUMF)
#include "drvGamma_C.c"
#elif CHIP_ID == CHIP_TSUMD
#include "drvGamma_D.c"
#elif CHIP_ID == CHIP_TSUMK
#include "drvGamma_K.c"
#else
#message "please implement DrvGamma for new chip"
#endif
#else
BYTE code msGammaNullData[] = {0};
void msGammaDummy(void)
{
    BYTE xdata i = msGammaNullData[0];
}
#endif

