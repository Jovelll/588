#define _COMMON_C_
#include "board.h"
#include "types.h"
#include "debugDef.h"
#include "global.h"
#include "ms_reg.h"
#include "ms_rwreg.h"
#include "misc.h"
#include "Debug.h"
#include "Common.h"

#if DEBUG_PRINT_ENABLE
void printMsg( char *str )
{
    char sendData;

    if(!UART_READ_ES())//(!ES)
        return;

    sendData = *( str++ );

    while( sendData )        //!='\0')
    {
        putSIOChar(( unsigned char )sendData );
        sendData = *( str++ );
    }
    putSIOChar( 0xd );
    putSIOChar(( char )'\n' );
}

void printData( char *str, WORD value )
{
    char sendData;

    if(!UART_READ_ES())//(!ES)
        return;

    sendData = Send_MsgValue;

    while( sendData = *( str++ ) )
    {
        if( sendData == ( char )'%' )            // %
        {
            sendData = *( str++ );
            if( sendData == ( char )'d' || sendData == ( char )'x' )                // d
            {
                if( value )
                {
                    Bool noneZero = FALSE;
                    WORD divider = 10000;
                    char dispValue;
                    if( sendData == ( char )'x' )
                    {
                        divider = 0x1000;
                    }
                    while( divider )
                    {
                        dispValue = value / divider;
                        value -= dispValue * divider;
                        if( dispValue )
                        {
                            noneZero = TRUE;
                        }
                        if( noneZero )
                        {
                            if( dispValue > 9 )
                            {
                                dispValue += 55;
                            }
                            else
                            {
                                dispValue += 0x30;
                            }
                            putSIOChar( dispValue );
                        }
                        if( sendData == ( char )'d' )
                        {
                            divider /= 10;
                        }
                        else
                        {
                            divider /= 0x10;
                        }
                    }
                }
                else
                {
                    putSIOChar( '0' );
                }
            }
            else if( sendData == ( char )'s' )
            {
                BYTE xdata *sPtr = (BYTE xdata *) value;
                while( *sPtr )
                    putSIOChar(*sPtr++);
                break;
            }
        }
        else
        {
            putSIOChar( sendData );
        }
    }
    putSIOChar( 0xd );
    putSIOChar(( char )'\n' );
}
#endif
