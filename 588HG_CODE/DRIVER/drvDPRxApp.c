
#include "global.h"

#if CHIP_ID == CHIP_TSUMU
#include "drvDPRxApp_U.c"
#elif (CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID == CHIP_TSUMD)
#include "drvDPRxApp_C.c"
#elif (CHIP_ID == CHIP_TSUM9 ||CHIP_ID == CHIP_TSUMF)
#include "drvDPRxApp_9.c"
#elif (CHIP_ID == CHIP_TSUML || CHIP_ID == CHIP_TSUMB || CHIP_ID == CHIP_TSUMY || CHIP_ID == CHIP_TSUMV || CHIP_ID == CHIP_TSUM2 || CHIP_ID == CHIP_TSUMJ)
#else
#message "please implement drvDPRxApp for new chip"
#endif

