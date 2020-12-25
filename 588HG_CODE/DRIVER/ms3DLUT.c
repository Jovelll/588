
#include "global.h"

#if (CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID == CHIP_TSUMD || CHIP_ID == CHIP_TSUMJ || CHIP_ID == CHIP_TSUM9|| CHIP_ID == CHIP_TSUMF)
#include "drv3DLUT_C.c"
#elif (CHIP_ID == CHIP_TSUML || CHIP_ID == CHIP_TSUMU || CHIP_ID == CHIP_TSUMB || CHIP_ID == CHIP_TSUMY || CHIP_ID == CHIP_TSUMV || CHIP_ID == CHIP_TSUM2)

#else
#message "please implement 3D_LUT for new chip"
#endif

#if !ENABLE_3DLUT
BYTE code ms3DLUTDummy[] = {0};
void ms3DLUT_Dummy()
{
    BYTE xdata x = ms3DLUTDummy;
}
#endif

