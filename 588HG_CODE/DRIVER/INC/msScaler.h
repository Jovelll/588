#ifndef _MS_SCALER_H_
#define _MS_SCALER_H_

#if CHIP_ID == CHIP_TSUMU
#include "msScaler_U.h"
#elif CHIP_ID == CHIP_TSUM2
#include "msScaler_2.h"
#elif CHIP_ID == CHIP_TSUMC || CHIP_ID == CHIP_TSUMK
#include "msScaler_C.h"
#elif CHIP_ID == CHIP_TSUMD
#include "msScaler_D.h"
#elif CHIP_ID == CHIP_TSUMJ
#include "msScaler_J.h"
#elif CHIP_ID == CHIP_TSUM9
#include "msScaler_9.h"
#elif CHIP_ID == CHIP_TSUMF
#include "msScaler_F.h"
#else
//#message "please implement msScaler for new chip"
#endif

#endif

