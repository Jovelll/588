
#include "global.h"

#if CHIP_ID == CHIP_TSUMU
#include "drvmStar_U.c"
#elif CHIP_ID == CHIP_TSUMV
#include "drvmStar_V.c"
#elif CHIP_ID == CHIP_TSUMY
#include "drvmStar_Y.c"
#elif CHIP_ID == CHIP_TSUMB
#include "drvmStar_B.c"
#elif CHIP_ID==CHIP_TSUM2
#include "drvmStar_2.c"
#elif CHIP_ID==CHIP_TSUMC || CHIP_ID==CHIP_TSUMK
#include "drvmStar_C.c"
#elif CHIP_ID==CHIP_TSUMD
#include "drvmStar_D.c"
#elif CHIP_ID==CHIP_TSUMJ
#include "drvmStar_J.c"
#elif CHIP_ID==CHIP_TSUM9
#include "drvmStar_9.c"
#elif CHIP_ID==CHIP_TSUMF
#include "drvmStar_F.c"
#else
#message "please implement drvmStar for new chip"
#endif