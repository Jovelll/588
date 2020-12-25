
#ifndef _DRVMSOVD_H_
#define _DRVMSOVD_H_

#if CHIP_ID == CHIP_TSUMU
#include "drvmsOVD_U.h"
#elif CHIP_ID == CHIP_TSUMB
#include "drvmsOVD_B.h"
#elif CHIP_ID == CHIP_TSUM2
#include "drvmsOVD_2.h"
#elif CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK
#include "drvmsOVD_C.h"
#elif CHIP_ID == CHIP_TSUMD
#include "drvmsOVD_D.h"
#elif CHIP_ID == CHIP_TSUMF
#include "drvmsOVD_F.h"

#endif


#endif

