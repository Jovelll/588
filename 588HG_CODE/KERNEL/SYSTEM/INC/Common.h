#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef _COMMON_C_
  #define _COMMONDEC_
#else
  #define _COMMONDEC_ extern
#endif


_COMMONDEC_ void printData( char *str, WORD value );
_COMMONDEC_ void printMsg( char * str );
_COMMONDEC_ void putSIOChar( unsigned char sendData );

#endif