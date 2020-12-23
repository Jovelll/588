
#include "global.h"

#if ENABLE_MHL
#if CHIP_ID == CHIP_TSUM2
#include "swI2c.c"
#include "mhal_mhl_2.c"
#include "mdrv_mhl_2.c"
#include "mapi_mhl_2.c"
#elif CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK
#include "mhal_mhl_C.c"
#include "mdrv_mhl_C.c"
#include "mapi_mhl_C.c"
#elif CHIP_ID == CHIP_TSUMD
#include "mhal_mhl_D.c"
#include "mdrv_mhl_D.c"
#include "mapi_mhl_D.c"
#elif CHIP_ID == CHIP_TSUM9
#include "mhal_mhl_9.c"
#include "mdrv_mhl_9.c"
#include "mapi_mhl_9.c"
#elif CHIP_ID == CHIP_TSUMF
#include "mhal_mhl_F.c"
#include "mdrv_mhl_F.c"
#include "mapi_mhl_F.c"
#elif (CHIP_ID == CHIP_TSUML || CHIP_ID == CHIP_TSUMU || CHIP_ID == CHIP_TSUMY || CHIP_ID == CHIP_TSUMV || CHIP_ID == CHIP_TSUMJ)

#else  //dont have mhl code for TSUMB
#message "please implement MHL function for new chip"
#endif
#endif
BYTE code drvMHLDummy[] = {0};
void msMHLDummy(void)
{
    BYTE x = drvMHLDummy[0];
}


