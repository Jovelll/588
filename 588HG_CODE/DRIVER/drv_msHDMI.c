
#include "global.h"

#if CHIP_ID==CHIP_TSUMC || CHIP_ID==CHIP_TSUMK
#include "drv_msHDMI_C.c"
#elif CHIP_ID==CHIP_TSUMD
#include "drv_msHDMI_D.c"
#elif CHIP_ID==CHIP_TSUM9
#include "drv_msHDMI_9.c"
#elif CHIP_ID==CHIP_TSUMF
#include "drv_msHDMI_F.c"

#else
#include "drv_msHDMI_.c"
#endif
BYTE code drvHDMINullData[] = {0};
void msdrvHDMIDummy(void)
{
    BYTE xdata i = drvHDMINullData[0];
}
