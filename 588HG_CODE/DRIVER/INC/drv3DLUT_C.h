
#ifndef _MS_3DLUT_H_
#define _MS_3DLUT_H_
#define ENABLE_3DLUT_Test          0

typedef enum
{
    e3dLutDelta1X = 0,
    e3dLutDelta2X,
    e3dLutDelta4X,
    e3dLutDelta8X
} _3dLutDeltaEnum;

#if ENABLE_3DLUT_Test

extern void drv3DLutWriteReadTest(void);
#endif
//extern void ms3DLutEnable(BYTE window, BYTE enable);
extern void drv3DLutInit();

extern void mStar_Setup3DLUT( void );

#endif

