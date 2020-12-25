
#ifndef _DRVMSTAR_H_
#define _DRVMSTAR_H_

#if CHIP_ID == CHIP_TSUMU
#include "drvmStar_U.h"
#elif CHIP_ID == CHIP_TSUMV
#include "drvmStar_V.h"
#elif CHIP_ID == CHIP_TSUMY
#include "drvmStar_Y.h"
#elif CHIP_ID == CHIP_TSUMB
#include "drvmStar_B.h"
#elif CHIP_ID==CHIP_TSUM2
#include "drvmStar_2.h"
#elif CHIP_ID==CHIP_TSUMC || CHIP_ID==CHIP_TSUMK
#include "drvmStar_C.h"
#elif CHIP_ID==CHIP_TSUMD
#include "drvmStar_D.h"
#elif CHIP_ID==CHIP_TSUMJ
#include "drvmStar_J.h"
#elif CHIP_ID==CHIP_TSUM9
#include "drvmStar_9.h"
#elif CHIP_ID==CHIP_TSUMF
#include "drvmStar_F.h"
#else
#message "please implement drvmStar for new chip"
#endif

#endif
