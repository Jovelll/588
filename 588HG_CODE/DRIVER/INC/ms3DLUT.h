
#if (CHIP_ID == CHIP_TSUMC || CHIP_ID==CHIP_TSUMK || CHIP_ID == CHIP_TSUMD || CHIP_ID == CHIP_TSUMJ || CHIP_ID == CHIP_TSUM9 || CHIP_ID == CHIP_TSUMF)
#include "drv3DLUT_C.h"
#else
//#message "please implement 3D_LUT for new chip"
#endif

#if ENABLE_3DLUT
extern void drv3DLutInit();
extern void mStar_Setup3DLUT( void );
extern void ms3DLutEnable(BYTE window, BYTE enable);
extern void ms3DLutSetHardwareGain(BYTE ColorMode);
extern void ms3DLutLoadTbl(BYTE ColorMode);
extern void ms3DLutGrayGuard(BYTE enable);
#else
extern void ms3DLUT_Dummy(void);
#endif

