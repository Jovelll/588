
#include "global.h"

#if CHIP_ID == CHIP_TSUMU
#include "drvmsOVD_U.c"
#elif CHIP_ID == CHIP_TSUMB
#include "drvmsOVD_B.c"
#elif CHIP_ID == CHIP_TSUM2
#include "drvmsOVD_2.c"
#elif CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK
#include "drvmsOVD_C.c"
#elif CHIP_ID == CHIP_TSUMD
#include "drvmsOVD_D.c"
#elif CHIP_ID == CHIP_TSUMF
#include "drvmsOVD_F.c"

#elif (CHIP_ID == CHIP_TSUMV || CHIP_ID == CHIP_TSUMY || CHIP_ID == CHIP_TSUMJ || CHIP_ID == CHIP_TSUM9)

#else
#message "please check if need OD function for your project"
#endif

BYTE code drvmsOVDNullData[] = {0};
void drvmsOVDDummy(void)
{
    BYTE xdata i = drvmsOVDNullData[0];
}
