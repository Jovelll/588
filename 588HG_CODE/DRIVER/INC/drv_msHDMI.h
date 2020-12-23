
#ifndef _DRV_MSHDMI_H_
#define _DRV_MSMDMI_H_

#if CHIP_ID==CHIP_TSUMC || CHIP_ID==CHIP_TSUMK
#include "drv_msHDMI_C.h"
#elif CHIP_ID==CHIP_TSUMD
#include "drv_msHDMI_D.h"
#elif CHIP_ID==CHIP_TSUM9
#include "drv_msHDMI_9.h"
#elif CHIP_ID==CHIP_TSUMF
#include "drv_msHDMI_F.h"
#else
#include "drv_msHDMI_.h"
#endif

#endif
