
//#include "board.h"

#if Enable_Gamma

#ifdef _GAMMA_C_
#define _GAMMA_DEC_
#else
#define _GAMMA_DEC_   extern
#endif

#define GAMMA_CH_R  0
#define GAMMA_CH_G  1
#define GAMMA_CH_B  2
#define GAMMA_CH_ALL  3

extern void mStar_SetupGamma( BYTE GamaMode );
#endif


