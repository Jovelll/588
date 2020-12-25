#define _MSADC_C_
#include <math.h>
#include "types.h"
#include "board.h"     

#if defined(TSUMXXTX)
#include "msADC_TSUMXXTX.c"
#else
#include "msADC_TSUMXX.c"
#endif

