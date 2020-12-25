
#ifndef _DRVMCU_H_
#define _DRVMCU_H_

#if CHIP_ID == CHIP_TSUMU
#include "drvMcu_U.h"
#elif CHIP_ID == CHIP_TSUMV
#include "drvMcu_V.h"
#elif CHIP_ID == CHIP_TSUMY
#include "drvMcu_Y.h"
#elif CHIP_ID == CHIP_TSUMB
#include "drvMcu_B.h"
#elif CHIP_ID == CHIP_TSUM2
#include "drvMcu_2.h"
#elif CHIP_ID == CHIP_TSUMC || CHIP_ID == CHIP_TSUMK
#include "drvMcu_C.h"
#elif CHIP_ID == CHIP_TSUMD
#include "drvMcu_D.h"
#elif CHIP_ID == CHIP_TSUMJ
#include "drvMcu_J.h"
#elif CHIP_ID == CHIP_TSUM9
#include "drvMcu_9.h"
#elif CHIP_ID == CHIP_TSUMF
#include "drvMcu_F.h"
#else
#message "please implement drvMcu for new chip"
#endif

#endif
