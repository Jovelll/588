
#if CHIP_ID == CHIP_TSUMU
#include "msFB_U.h"
#elif CHIP_ID == CHIP_TSUMB
#include "msFB_B.h"
#elif CHIP_ID == CHIP_TSUM2
#include "msFB_2.h"
#elif CHIP_ID == CHIP_TSUMC || CHIP_ID == CHIP_TSUMK
#include "msFB_C.h"
#elif CHIP_ID == CHIP_TSUMD
#include "msFB_D.h"
#elif CHIP_ID == CHIP_TSUMJ
#include "msFB_J.h"
#elif CHIP_ID == CHIP_TSUM9
#include "msFB_9.h"
#elif CHIP_ID == CHIP_TSUMF
#include "msFB_F.h"
#else
//#message "please implement msFB for new chip"
#endif





