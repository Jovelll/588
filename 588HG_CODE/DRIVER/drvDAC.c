
#include "global.h"

#if CHIP_ID == CHIP_TSUMU
#include "drvDAC_U.c"
#elif CHIP_ID == CHIP_TSUMB
#include "drvDAC_B.c"
#elif CHIP_ID == CHIP_TSUM2
#include "drvDAC_2.c"
#elif (CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID == CHIP_TSUMD )
#include "drvDAC_C.c"
#elif (CHIP_ID == CHIP_TSUM9)||(CHIP_ID == CHIP_TSUMF)
#include "drvDAC_9.c"

#elif (CHIP_ID == CHIP_TSUMV || CHIP_ID == CHIP_TSUMY || CHIP_ID == CHIP_TSUMJ)
#else
#message "please implement drvDAC for new chip"
#endif

BYTE code msDACNullData[] = {0};
void msDACDummy(void)
{
    BYTE xdata i = msDACNullData[0];
}
