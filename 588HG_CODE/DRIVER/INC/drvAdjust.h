
#ifndef _DRVADJUST_H_
#define _DRVADJUST_H_

#if CHIP_ID == CHIP_TSUMV
#include "drvAdjust_V.h"
#elif CHIP_ID == CHIP_TSUMU
#include "drvAdjust_U.h"
#elif CHIP_ID == CHIP_TSUMY
#include "drvAdjust_Y.h"
#elif CHIP_ID == CHIP_TSUMB
#include "drvAdjust_B.h"
#elif CHIP_ID == CHIP_TSUM2
#include "drvAdjust_2.h"
#elif CHIP_ID == CHIP_TSUMC || CHIP_ID == CHIP_TSUMK
#include "drvAdjust_C.h"
#elif CHIP_ID == CHIP_TSUMD
#include "drvAdjust_D.h"
#elif CHIP_ID == CHIP_TSUMJ
#include "drvAdjust_J.h"
#elif CHIP_ID == CHIP_TSUM9
#include "drvAdjust_9.h"
#elif CHIP_ID == CHIP_TSUMF
#include "drvAdjust_F.h"
#else
#message "please implement drvAdjust for new chip"
#endif

#endif
