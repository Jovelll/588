#ifndef _AUTOFUNC_H_
#define _AUTOFUNC_H_       (1)

#ifdef _AUTOFUNC_C_
#define _AUTOFUNCDEC_
#else
#define _AUTOFUNCDEC_ extern
#endif

_AUTOFUNCDEC_ Bool CheckSyncLoss( void );
_AUTOFUNCDEC_ void WaitAutoStatusReady( WORD u16RegAddr, BYTE regFlag );
_AUTOFUNCDEC_ WORD GetAutoValue(WORD u16RegLoAddr);
_AUTOFUNCDEC_ BYTE GetTranstionPosition( BYTE vsyncTime, WORD u16RegLoAddr );
_AUTOFUNCDEC_ BYTE AutoSetDataThreshold( BYTE vsyncTime );
_AUTOFUNCDEC_ WORD SearchMaxWidth( BYTE vsyncTime );
_AUTOFUNCDEC_ Bool mStar_AutoHTotal( BYTE vsyncTime );
_AUTOFUNCDEC_ Bool mStar_AutoPhase( BYTE vsyncTime );

#if PANEL_WIDTH==1366&&PANEL_HEIGHT==768
_AUTOFUNCDEC_ void CheckHtotal( void );
#endif


_AUTOFUNCDEC_ Bool mStar_AutoPosition( void );
_AUTOFUNCDEC_ Bool mStar_AutoGeomtry( void );
_AUTOFUNCDEC_ Bool mStar_AutoColor( void );

#endif
