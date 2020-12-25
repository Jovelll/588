#ifndef _MSSXAFirmware_H
#define _MSSXAFirmware_H
#define MSSXA_ON_FIRMWARE

//#define _MSSXA_
#define _MSSXA_NUM_C	4
#include "MSSXA_CONFIG.h"


#if !_MSSXA_ENABLE

#define MSSXA_DEF		BYTE xdata _MSSXA_REF; \
						BYTE xdata _MSSXA_REFX[8]; \
					    WORD	   _kkk_q = 0;
#define _MSSXA_BL_ST	1

#else

#define _MSSXA_E_ARR		((BYTE*)&_MSSXA_REFX[_MSSXA_E_OFFSET])
#define _MSSXA_C_ARR		((WORD*)&_MSSXA_REFX[_MSSXA_C_OFFSET])
#define _MSSXA_COMMAND_ARR	((BYTE*)&_MSSXA_REFX[_MSSXA_COMMAND_OFFSET])
#define _MSSXA_DELAY_ARR	((WORD*)&_MSSXA_REFX[_MSSXA_DELAY_OFFSET])

#define _MSSXA_REF 		_MSSXA_REFX[_MSSXA_REF_OFFSET]
#define _MSSXA_OPT		_MSSXA_REFX[_MSSXA_OPT_OFFSET]
#define _MSSXA_TOKEN_I	_MSSXA_REFX[_MSSXA_TOKEN_I_OFFSET]
#define _MSSXA_DELAY1ms	_MSSXA_REFX[_MSSXA_DELAY1ms_OFFSET]
#define _MSSXA_DELAY	(*_MSSXA_DELAY_P)

extern BYTE xdata 	_MSSXA_REFX[ _MSSXA_META_SZ ];
extern BYTE* xdata	_MSSXA_E; 
extern WORD* xdata	_MSSXA_C;
extern WORD* xdata	_MSSXA_DELAY_P;
extern BYTE* xdata	_MSSXA_COMMAND; 

#endif



#if !_MSSXA_ENABLE

#define MSSXA_INIT()				/* if _MSSXA_ENABLE == 0 */
#define MSSXA_START()				/* if _MSSXA_ENABLE == 0 */
#define MSSXA_BP( index )			/* if _MSSXA_ENABLE == 0 */
#define MSSXA_BP_P( index )			/* if _MSSXA_ENABLE == 0 */
#define MSSXA_BPEP()				/* if _MSSXA_ENABLE == 0 */
#define MSSXA_BP_B( index )			/* if _MSSXA_ENABLE == 0 */
#define MSSXA_CBP( index, c )		/* if _MSSXA_ENABLE == 0 */
#define MSSXA_CBP_B( index, c )		/* if _MSSXA_ENABLE == 0 */
#define MSSXA_BP_E()				/* if _MSSXA_ENABLE == 0 */
#define MSSXA_ENTER_LOOP()			/* if _MSSXA_ENABLE == 0 */
#define MSSXA_SW_DELAY()			/* if _MSSXA_ENABLE == 0 */
#define MSSXA_ISR_HANDLE()			/* if _MSSXA_ENABLE == 0 */

#define MSSXA_LOOP_START()			/* if _MSSXA_ENABLE == 0 */ do { \
	if( 0 == (_kkk_q) % 500) \
		printData("\n[...%d]", _kkk_q); \
	if( 0 == (_kkk_q) % 18000) \
		printMsg("@L@ Hi~ Mother, I am here #"); \
	if( 0 == (_kkk_q) % 8000) \
	{ \
		int i; \
		for( i = 0; i < 8; i++ ) \
		{ \
			printData( "\r\n_MSSXA_REFX[%d]", i ); \
			printData( " = %x", _MSSXA_REFX[i] ); \
		} \
	} \
	if( 0 != (0x80 & _MSSXA_REF) && 0 < SIORxIndex )  \
	{ \
		int i; \
		for( i = 0; i < SIORxIndex; i++ ) \
		{ \
			printData( "\r\nSIORxBuffer[%d]", i ); \
			printData( " = %x", SIORxBuffer[i] ); \
		} SIORxIndex = 0; \
	} _kkk_q++; } while( 0 )

#else

#define MSSXA_ENABLE()				MSSXA_ENABLE_(_MSSXA_REF)
#define MSSXA_DISABLE()				MSSXA_DISABLE_(_MSSXA_REF)
#define MSSXA_BLOCK()				MSSXA_BLOCK_(_MSSXA_REF)
#define MSSXA_RELEASE()				MSSXA_RELEASE_(_MSSXA_REF)
#define MSSXA_SET_BP_I( i )			MSSXA_SET_BP_I_(_MSSXA_REF, i)
#define MSSXA_IS_ENABLE				MSSXA_IS_ENABLE_(_MSSXA_REF)
#define MSSXA_IS_DISABLE			MSSXA_IS_DISABLE_(_MSSXA_REF)
#define MSSXA_IS_BLOKED				MSSXA_IS_BLOKED_(_MSSXA_REF)

#define MSSXA_SET_TO_LOOP_START()	MSSXA_SET_TO_LOOP_START_(_MSSXA_OPT)
#define MSSXA_SET_H_RESET()			MSSXA_SET_H_RESET_(_MSSXA_OPT)
#define MSSXA_CLR_H_RESET()			MSSXA_CLR_H_RESET_(_MSSXA_OPT)
#define MSSXA_SET_RESET()			MSSXA_SET_RESET_(_MSSXA_OPT)
#define MSSXA_CLR_RESET()			MSSXA_CLR_RESET_(_MSSXA_OPT)
#define MSSXA_SET_ISR_READY_GO()	MSSXA_SET_ISR_READY_GO_(_MSSXA_OPT)
#define MSSXA_SET_ISR_READY()		MSSXA_SET_ISR_READY_(_MSSXA_OPT)
#define MSSXA_CLR_ISR_READY_GO()	MSSXA_CLR_ISR_READY_GO_(_MSSXA_OPT)
#define MSSXA_CLR_ISR_READY()		MSSXA_CLR_ISR_READY_(_MSSXA_OPT)
#define MSSXA_ENTER_PBP_WAIT()		MSSXA_ENTER_PBP_WAIT_(_MSSXA_OPT)
#define MSSXA_EXIT_PBP_WAIT()		MSSXA_EXIT_PBP_WAIT_(_MSSXA_OPT)
#define MSSXA_ENTER_BP_WAIT()		MSSXA_ENTER_BP_WAIT_(_MSSXA_OPT)
#define MSSXA_EXIT_BP_WAIT()		MSSXA_EXIT_BP_WAIT_(_MSSXA_OPT)
#define MSSXA_IS_DISABLE_COMM		MSSXA_IS_DISABLE_COMM_(_MSSXA_OPT)
#define MSSXA_IS_OUT_BP_WAIT		MSSXA_IS_OUT_BP_WAIT_(_MSSXA_OPT)
#define MSSXA_IS_OUT_PBP_WAIT		MSSXA_IS_OUT_PBP_WAIT_(_MSSXA_OPT)
#define MSSXA_IS_ISR_READY			MSSXA_IS_ISR_READY_(_MSSXA_OPT)
#define MSSXA_IS_ISR_READY_GO		MSSXA_IS_ISR_READY_GO_(_MSSXA_OPT)
#define MSSXA_IS_RESET_ON			MSSXA_IS_RESET_ON_(_MSSXA_OPT)
#define MSSXA_IS_H_RESET_ON			MSSXA_IS_H_RESET_ON_(_MSSXA_OPT)

#define MSSXA_TO_RESET()		do { /*printData("MSSXA_REEST!!%x", _MSSXA_OPT);*/ \
	if( !MSSXA_IS_RESET_ON ) { if( MSSXA_IS_H_RESET_ON ) { MSSXA_CLR_H_RESET(); goto _MSSXA_START; } } \
	else { MSSXA_CLR_RESET(); if( !MSSXA_IS_H_RESET_ON ) goto _MSSXA_RESET; else { MSSXA_CLR_H_RESET(); goto _MSSXA_H_RESET; } } \
	} while( 0 )
#define MSSXA_NOBLOCKED()		do { MSSXA_RELEASE(); MSSXA_DISABLE(); } while( 0 )
#define MSSXA_INIT()			/* if _MSSXA_ENABLE == 1 */ WORD _test_c = 0; _MSSXA_RESET: mssxaInit();
#define MSSXA_START()			/* if _MSSXA_ENABLE == 1 */ do { _MSSXA_H_RESET: mssxaStart(); MSSXA_TO_RESET(); } while( 0 )
#define MSSXA_BP( index )		/* if _MSSXA_ENABLE == 1 */ do { if( mssxaBreakPointB( index, 0xff ) ) mssxaBreakPointE(); } while( 0 )
#define MSSXA_BP_B( index )		/* if _MSSXA_ENABLE == 1 */ if( mssxaBreakPointB( index, 0xff ) ) {
#define MSSXA_CBP( index, c )	/* if _MSSXA_ENABLE == 1 */ do { if( mssxaBreakPointB( index, c ) ) mssxaBreakPointE(); } while( 0 )
#define MSSXA_CBP_B( index, c )	/* if _MSSXA_ENABLE == 1 */ if( mssxaBreakPointB( index, c ) ) {
#define MSSXA_BP_E()			/* if _MSSXA_ENABLE == 1 */ mssxaBreakPointE(); }
#define MSSXA_ENTER_LOOP()		/* if _MSSXA_ENABLE == 1 */ _MSSXA_START: printMsg("\r\nMSSXA_START:"); do { mssxaEnterLoop(); MSSXA_TO_RESET(); } while( 0 )
#define MSSXA_LOOP_START()		/* if _MSSXA_ENABLE == 1 */ do { mssxaLoopStart(); MSSXA_TO_RESET(); } while( 0 )
#define MSSXA_ISR_HANDLE( buff, _sio_rx_index )		/* if _MSSXA_ENABLE == 1 */ do { \
		BYTE  code _mssxa_token_str[] = _MSSXA_TOKEN_STRING; \
		BYTE r_data; r_data = buff; \
		if( _MSSXA_TOKEN_STRING_LEN > _MSSXA_TOKEN_I ) \
			if( _mssxa_token_str[ _MSSXA_TOKEN_I ] != r_data ) _MSSXA_TOKEN_I = 0; else _MSSXA_TOKEN_I++; \
		else if( 4 + _MSSXA_TOKEN_STRING_LEN > _MSSXA_TOKEN_I ) { _sio_rx_index = 0; \
			_MSSXA_COMMAND[_MSSXA_TOKEN_I - _MSSXA_TOKEN_STRING_LEN] = r_data; _MSSXA_TOKEN_I++; } \
		else MSSXA_COMPORT_ISR( r_data ); } while( 0 )	

#endif

extern BYTE mssxaBreakPointB( BYTE index, BYTE c );
extern void mssxaBreakPointE( void );

#endif

