
#ifndef _DRVDDC2BI_H_
#define _DRVDDC2BI_H_

#include "DDC2Bi.H"

#if (CHIP_ID == CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMK) || (CHIP_ID == CHIP_TSUMD) || (CHIP_ID == CHIP_TSUMJ)
#include "drvDDC2Bi_C.h"
#elif (CHIP_ID == CHIP_TSUM9 ||CHIP_ID == CHIP_TSUMF)
#include "drvDDC2Bi_9.h"
#else // CHIP_TSUMU, CHIP_TSUMV, CHIP_TSUMY, CHIP_TSUMB, CHIP_TSUM2
#include "drvDDC2Bi_.h"
#endif

// Extern Common functions in DDC2Bi.c
extern void DDC2Bi_InitRx( void );
extern void DDC2Bi_GetTxBuffer( void );
extern BYTE DDC2Bi_ComputeChecksum( BYTE count );
extern void DDC2Bi_MessageReady( void );
#if DDCCI_FILTER_FUNCTION // 111228 coding not ready
extern Bool DDCCICommandExecutable(void);
#endif
extern BYTE AlignControl( void );
//
#endif
