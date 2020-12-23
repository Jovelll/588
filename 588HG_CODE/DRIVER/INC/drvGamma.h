
//#include "board.h"

#ifndef _DRVGAMMA_H_
#define _DRVGAMMA_H_

//#define USE_NORMAL_GAMMA_TBL  1
//#define MAIN_WINDOW 0
//#define SUB_WINDOW  1

#if CHIP_ID == CHIP_TSUMU
#include "drvGamma_U.h"
#elif CHIP_ID == CHIP_TSUMV
#include "drvGamma_V.h"
#elif CHIP_ID == CHIP_TSUMY
#include "drvGamma_Y.h"
#elif CHIP_ID == CHIP_TSUMB
#include "drvGamma_B.h"
#elif CHIP_ID == CHIP_TSUM2
#include "drvGamma_2.h"
#elif (CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID == CHIP_TSUMJ || CHIP_ID == CHIP_TSUM9 || CHIP_ID == CHIP_TSUMF)
#include "drvGamma_C.h"
#elif CHIP_ID == CHIP_TSUMD
#include "drvGamma_D.h"
#else
#message "please implement DrvGamma for new chip"
#endif

#endif
