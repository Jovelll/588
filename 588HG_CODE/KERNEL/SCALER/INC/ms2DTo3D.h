#ifndef _MS_2DTo3D_H_
#define _MS_2DTo3D_H_

#ifdef _MS_2DTo3D_C_
#define MS_2DTo3DDec
#else
#define MS_2DTo3DDec   extern
#endif

#if ENABLE_R2_2DTO3D
//typedef struct
//{
//    BYTE ucOffset;
//    BYTE ucDepth;
//}Sc2DTo3DInfo;

//MS_2DTo3DDec xdata Sc2DTo3DInfo g_Sc2DTo3DInfo;

MS_2DTo3DDec void Sc2DTo3DInit(void);
MS_2DTo3DDec void Sc2DTo3DOff(void);
#if CHIP_ID == CHIP_TSUMD
MS_2DTo3DDec void Sc2DTo3DSetPIXGain(WORD pixGain);
MS_2DTo3DDec void Sc2DTo3DSetPIXOffset(BYTE pixOffset);
MS_2DTo3DDec void Sc2DTo3DSetBLKGain(BYTE blkGain);
MS_2DTo3DDec void Sc2DTo3DSetBLKOffset(BYTE blkOffset);
#elif CHIP_ID == CHIP_TSUM2
MS_2DTo3DDec void Sc2DTo3DSetDepth(BYTE ucDepth);
MS_2DTo3DDec void Sc2DTo3DSetOffset(BYTE offset);
#endif
#endif

#undef MS_2DTo3DDec
#endif
