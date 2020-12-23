
#ifndef _APPMSTAR_H_
#define _APPMSTAR_H_

#if CHIP_ID == CHIP_TSUMU
#include "appmStar_U.h"
#elif CHIP_ID == CHIP_TSUMV
#include "appmStar_V.h"
#elif CHIP_ID == CHIP_TSUMY
#include "appmStar_Y.h"
#elif CHIP_ID == CHIP_TSUMB
#include "appmStar_B.h"
#elif CHIP_ID==CHIP_TSUM2
#include "appmstar_2.h"
#elif CHIP_ID==CHIP_TSUMC || CHIP_ID==CHIP_TSUMK
#include "appmstar_C.h"
#elif CHIP_ID==CHIP_TSUMD
#include "appmstar_D.h"
#elif CHIP_ID==CHIP_TSUMJ
#include "appmstar_J.h"
#elif CHIP_ID==CHIP_TSUM9
#include "appmstar_9.h"
#elif CHIP_ID==CHIP_TSUMF
#include "appmstar_F.h"
#else
#message "please implement appmStar for new chip"
#endif

#endif
