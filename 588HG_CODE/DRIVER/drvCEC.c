#include "types.h"
#include "board.h"
#if ENABLE_CEC

#include "global.h"

#if CHIP_ID==CHIP_TSUMC || CHIP_ID==CHIP_TSUMK  
#include "drvCEC_C.c"
#elif CHIP_ID==CHIP_TSUMD
#include "drvCEC_D.c"
#elif CHIP_ID==CHIP_TSUM9
#include "drvCEC_9.c"
#elif CHIP_ID==CHIP_TSUMF
#include "drvCEC_F.c"
#else
#include "drvCEC_.c"
#endif
BYTE code drvCECNullData[] = {0};
void drv_CEC_DummyFunction(void)
{
    BYTE xdata i = drvCECNullData[0];
}

#endif
