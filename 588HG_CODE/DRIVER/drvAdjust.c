
#include "global.h"

#if CHIP_ID == CHIP_TSUMU
#include "drvAdjust_U.c"
#elif CHIP_ID == CHIP_TSUMV
#include "drvAdjust_V.c"
#elif CHIP_ID == CHIP_TSUMY
#include "drvAdjust_Y.c"
#elif CHIP_ID == CHIP_TSUMB
#include "drvAdjust_B.c"
#elif CHIP_ID == CHIP_TSUM2
#include "drvAdjust_2.c"
#elif CHIP_ID == CHIP_TSUMC || CHIP_ID == CHIP_TSUMK
#include "drvAdjust_C.c"
#elif CHIP_ID == CHIP_TSUMD
#include "drvAdjust_D.c"
#elif CHIP_ID == CHIP_TSUMJ
#include "drvAdjust_J.c"
#elif CHIP_ID == CHIP_TSUM9
#include "drvAdjust_9.c"
#elif CHIP_ID == CHIP_TSUMF
#include "drvAdjust_F.c"
#else
#message "please implement drvAdjust for new chip"
#endif

