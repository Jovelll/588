
#include "global.h"

#if CHIP_ID == CHIP_TSUMU
#include "drvMcu_U.c"
#elif CHIP_ID == CHIP_TSUMV
#include "drvMcu_V.c"
#elif CHIP_ID == CHIP_TSUMY
#include "drvMcu_Y.c"
#elif CHIP_ID == CHIP_TSUMB
#include "drvMcu_B.c"
#elif CHIP_ID == CHIP_TSUM2
#include "drvMcu_2.c"
#elif CHIP_ID == CHIP_TSUMC || CHIP_ID == CHIP_TSUMK
#include "drvMcu_C.c"
#elif CHIP_ID == CHIP_TSUMD
#include "drvMcu_D.c"
#elif CHIP_ID == CHIP_TSUMJ
#include "drvMcu_J.c"
#elif CHIP_ID == CHIP_TSUM9
#include "drvMcu_9.c"
#elif CHIP_ID == CHIP_TSUMF
#include "drvMcu_F.c"
#else
#message "please implement drvMcu for new chip"
#endif

