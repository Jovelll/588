
#ifndef _DRVPOWER_H_
#define _DRVPOWER_H_

#if CHIP_ID == CHIP_TSUMU
#include "drvPower_U.h"
#elif CHIP_ID == CHIP_TSUMV
#include "drvPower_V.h"
#elif CHIP_ID == CHIP_TSUMY
#include "drvPower_Y.h"
#else
#message "implement drvPower for new chip"
#endif

#endif


