
#ifndef _DRV_CEC_H_
#define _DRV_CEC_H_

#if CHIP_ID==CHIP_TSUMC || CHIP_ID==CHIP_TSUMK
#include "drvCEC_C.h"
#elif CHIP_ID==CHIP_TSUMD
#include "drvCEC_D.h"
#elif CHIP_ID==CHIP_TSUM9
#include "drvCEC_9.h"
#elif CHIP_ID==CHIP_TSUMF
#include "drvCEC_F.h"
#else
#include "drvCEC_.h"
#endif

#endif

