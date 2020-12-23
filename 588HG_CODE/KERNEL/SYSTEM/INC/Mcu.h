#ifndef _MCU_H_
#define _MCU_H_

#ifdef _MCU_C_
#define _MCUDEC_
#else
#define _MCUDEC_    extern
#endif


_MCUDEC_ void Init_MCU( void );
_MCUDEC_ void Init_ExternalInterrupt( void );
_MCUDEC_ void Init_Timer( void );
_MCUDEC_ void Init_SerialPort( void );
_MCUDEC_ void Init_IOPorts( void );
_MCUDEC_ void Init_ExtTimerCount( void );

//_MCUDEC_ void MCU_EnableCache(Bool u8Enable);


#endif
