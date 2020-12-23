
#if CHIP_ID == CHIP_TSUM2
#include "swI2c.h"
#include "appMHLDefine.h"
#include "mapi_mhl_2.h"
#include "mdrv_mhl_2.h"
#include "mhal_mhl_2.h"
#elif CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK
#include "appMHLDefine.h"
#include "mdrv_mhl_C.h"
#include "mhal_mhl_C.h"
#include "mapi_mhl_C.h"
#elif CHIP_ID == CHIP_TSUMD
#include "appMHLDefine.h"
#include "mdrv_mhl_D.h"
#include "mhal_mhl_D.h"
#include "mapi_mhl_D.h"
#elif CHIP_ID == CHIP_TSUM9 
#include "appMHLDefine.h"
#include "mdrv_mhl_9.h"
#include "mhal_mhl_9.h"
#include "mapi_mhl_9.h"
#elif CHIP_ID == CHIP_TSUMF
#include "appMHLDefine.h"
#include "mdrv_mhl_F.h"
#include "mhal_mhl_F.h"
#include "mapi_mhl_F.h"
#endif

extern void msMHLDummy(void);

