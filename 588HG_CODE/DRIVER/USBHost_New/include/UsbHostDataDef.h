
#ifndef __USBHOSTDATADEF_H_
#define __USBHOSTDATADEF_H_

#include "types.h"

#ifndef U8
#define U8    BYTE
#endif

#ifndef U16
#define U16    WORD
#endif

#ifndef U32
#define U32    DWORD
#endif

#ifndef S8
#define S8    Schar
#endif

#ifndef S16
#define S16    Sint
#endif

#ifndef UINT8
#define UINT8     BYTE
#endif

#ifndef UINT16
#define UINT16    WORD
#endif

#ifndef UINT32
#define UINT32    DWORD
#endif


#ifndef BIT0
#define BIT0 0x01
#endif

#ifndef BIT1
#define BIT1 0x02
#endif

#ifndef BIT2
#define BIT2 0x04
#endif

#ifndef BIT3
#define BIT3 0x08
#endif

#ifndef BIT4
#define BIT4 0x10
#endif

#ifndef BIT5
#define BIT5 0x20
#endif

#ifndef BIT6
#define BIT6 0x40
#endif

#ifndef BIT7
#define BIT7 0x80
#endif

//#define ErrorRetryCount  6
//#define ErrorRetryCount2 3
//#define ErrorRetryCount3 0x1000

//#define Enable_UHCI_Recovery
//#define Enable_UHCI_Recovery2
//#define Enable_UHCI_Recovery3
//#define Eanble_UHCI_Recovery4
//#define Enable_UHCI_Recovery5
//#define USBHostLibVersion 0x1B
//#define Enable_Close_INT
#endif /*__USBHOSTDATADEF_H_ */

