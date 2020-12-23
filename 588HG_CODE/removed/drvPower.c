
#include "global.h"

#if CHIP_ID == CHIP_TSUMU
#include "drvPower_U.c"
#elif CHIP_ID == CHIP_TSUMV
#include "drvPower_V.c"
#elif CHIP_ID == CHIP_TSUMY
#include "drvPower_Y.c"
#else
#message "please implement drvPower for new chip"
#endif

