#if _MSSXA_ENABLE
//#define _MSSXA_ICE_DEBUG

#ifdef _MSSXA_ICE_DEBUG
WORD xdata	_ccc666 = 0;
WORD xdata	_ccc777 = 0;
#endif


BYTE xdata 	_MSSXA_REFX[ _MSSXA_META_SZ ];
BYTE* xdata	_MSSXA_E; 
WORD* xdata	_MSSXA_C;
WORD* xdata	_MSSXA_DELAY_P;
BYTE* xdata	_MSSXA_COMMAND; 


BYTE
mssxaBlockSection( BYTE e_f )
{
#define MASK_VALUE( var, value, mask )  do { var &= ~(mask); var |= (mask) & (value); } while( 0 )
	while(1) 
	{
		if( 4 + _MSSXA_TOKEN_STRING_LEN == _MSSXA_TOKEN_I )
		{
#ifdef _MSSXA_ICE_DEBUG
			BYTE xdata	i;
			
			printData( "\r\nBefore...%x ", _MSSXA_COMMAND[0] );
			for( i = 1; i < 4; i++ )
				printData( "%x ", _MSSXA_COMMAND[i] );
			printData( "\r\n_MSSXA_REF=%x ", _MSSXA_REF );
			printData( "\r\n_MSSXA_OPT=%x ", _MSSXA_OPT );
#endif			
			MASK_VALUE( _MSSXA_REF, _MSSXA_COMMAND[0], _MSSXA_COMMAND[1] ); 
			MASK_VALUE( _MSSXA_OPT, _MSSXA_COMMAND[2], _MSSXA_COMMAND[3] ); 
			_MSSXA_TOKEN_I = 0;
#ifdef _MSSXA_ICE_DEBUG
			printData( "\r\nAfter...%x ", _MSSXA_COMMAND[0] );
			for( i = 1; i < 4; i++ )
				printData( "%x ", _MSSXA_COMMAND[i] );
			printData( "\r\n_MSSXA_REF=%x ", _MSSXA_REF );
			printData( "\r\n_MSSXA_OPT=%x ", _MSSXA_OPT );
#endif			
		}
		if( !MSSXA_IS_BLOKED )
			break;
		if( MSSXA_IS_RESET_ON || MSSXA_IS_H_RESET_ON )
			break;
		if( e_f ) if( MSSXA_IS_DISABLE )
			break;
	}
	if( MSSXA_IS_DISABLE_COMM )
		SIORxIndex = 0;
	if( MSSXA_IS_RESET_ON || MSSXA_IS_H_RESET_ON )
		return 0;
	return (BYTE)MSSXA_IS_ENABLE;
}


BYTE 
mssxaBreakPointB( BYTE index, BYTE c )
{
	MSSXA_SET_BP_I( index );
	MSSXA_ENTER_PBP_WAIT();
	if( !mssxaBlockSection(0) )
	{
		MSSXA_EXIT_PBP_WAIT();
		return 0;
	}
	MSSXA_EXIT_PBP_WAIT();
	if( 0xFF != c ) if( 0 < _MSSXA_C[ c ] )
	{
		_MSSXA_C[ c ]--;
		return 0;
	}
	if( !( _MSSXA_E[ index >> 3 ] & ( 1 << (index & 0x7) ) ) )
		return 0;
#ifdef _MSSXA_ICE_DEBUG
	printData( "\r\n_MSSXA_REF=%X", _MSSXA_REF );
	printData( "::%d", _ccc666);
	_ccc666++;
#endif	
	printMsg("\r\n"); 
	printMsg( _MSSXA_UART_TRI_STR ); 
	return 1;
}


void 
mssxaBreakPointE( void )
{
	MSSXA_BLOCK();
	printMsg( _MSSXA_UART_END_STR ); 
	MSSXA_ENTER_BP_WAIT();
	mssxaBlockSection(1);
	MSSXA_EXIT_BP_WAIT();
}


void 
mssxaInit( void )
{
	BYTE data i;

	for( i = 0; i < _MSSXA_META_SZ; i++ )
		_MSSXA_REFX[i] = 0;
	MSSXA_NOBLOCKED();
	_MSSXA_E = _MSSXA_E_ARR;
	_MSSXA_C = _MSSXA_C_ARR;
	_MSSXA_COMMAND = _MSSXA_COMMAND_ARR;
	_MSSXA_DELAY_P = _MSSXA_DELAY_ARR;
}


void 
mssxaStart( void )
{
	MSSXA_SET_ISR_READY();
	MSSXA_SET_ISR_READY_GO();
	printMsg( _MSSXA_UART_ADD_STR );
	printData( "%x ", (int)(&_MSSXA_REF) );
	printData( "%d",  _MSSXA_NUM_C );
	printMsg( _MSSXA_UART_END_STR );
}


void 
mssxaEnterLoop( void )
{
	printMsg( _MSSXA_UART_ADD_STR );
	printData( "%x ", (int)(&_MSSXA_REF) );
	printData( "%d",  _MSSXA_NUM_C );
	printMsg( _MSSXA_UART_END_STR );
	MSSXA_BP_B( _MSSXA_BP_ENTER_LOOP );		
	MSSXA_USR_INFO();	
	MSSXA_BP_E()
}


void 
mssxaLoopStart( void )
{
	BYTE xdata i;

#ifdef _MSSXA_ICE_DEBUG
	_ccc777++;
	if( 0 == _ccc777 % 100 )
		printData( "\r\n[[[_________%d ", _ccc777 ); 
#endif	
	MSSXA_CBP( _MSSXA_BP_LOOP_START, 0 );
	for( i = _MSSXA_DELAY; i > 0; i-- ) 
		Delay4us();
	Delay1ms( _MSSXA_DELAY1ms );
}


#endif

