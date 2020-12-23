
#include "global.h"

#if CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID == CHIP_TSUMD || CHIP_ID == CHIP_TSUMJ
#include "drvDDC2Bi_C.c"
#elif CHIP_ID == CHIP_TSUM9 ||CHIP_ID == CHIP_TSUMF
#include "drvDDC2Bi_9.c"
#else // CHIP_TSUMU, CHIP_TSUMV, CHIP_TSUMY, CHIP_TSUMB, CHIP_TSUM2
#include "drvDDC2Bi_.c"
#endif
