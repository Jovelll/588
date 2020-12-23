
#ifndef _DRVDAC_H_
#define _DRVDAC_H_

#if CHIP_ID == CHIP_TSUMU
#include "drvDAC_U.h"
#elif CHIP_ID == CHIP_TSUMV
#include "drvDAC_V.h"
#elif CHIP_ID == CHIP_TSUMY
#include "drvDAC_Y.h"
#elif CHIP_ID == CHIP_TSUMB
#include "drvDAC_B.h"
#elif CHIP_ID == CHIP_TSUM2
#include "drvDAC_2.h"
#elif (CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID == CHIP_TSUMD )
#include "drvDAC_C.h"
#elif (CHIP_ID == CHIP_TSUM9)||(CHIP_ID == CHIP_TSUMF)
#include "drvDAC_9.h"

#elif (CHIP_ID == CHIP_TSUML|| CHIP_ID == CHIP_TSUMJ)

#else
#message "please implement DrvDAC.h for new chip"
#endif

#endif
