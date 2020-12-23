
#ifndef _HALRWREG_H_
#define _HALRWREG_H_

#if CHIP_ID == CHIP_TSUMU
#include "halRwreg_U.h"
#elif CHIP_ID == CHIP_TSUMV
#include "halRwreg_V.h"
#elif CHIP_ID == CHIP_TSUMY
#include "halRwreg_Y.h"
#elif CHIP_ID == CHIP_TSUMB
#include "halRwreg_B.h"
#elif CHIP_ID == CHIP_TSUM2
#include "halRwreg_2.h"
#elif CHIP_ID == CHIP_TSUMC || CHIP_ID == CHIP_TSUMK
#include "halRwreg_C.h"
#elif CHIP_ID == CHIP_TSUMD
#include "halRwreg_D.h"
#elif CHIP_ID == CHIP_TSUMJ
#include "halRwreg_J.h"
#elif CHIP_ID == CHIP_TSUM9
#include "halRwreg_9.h"
#elif CHIP_ID == CHIP_TSUMF
#include "halRwreg_F.h"
#else
#message "please implement halRwreg for new chip"
#endif

#endif

