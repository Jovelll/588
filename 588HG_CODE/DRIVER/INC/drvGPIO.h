

//#include "global.h"
//#include "Model.h"

#ifndef _DRVGPIO_H_
#define _DRVGPIO_H_
#if CHIP_ID == CHIP_TSUMK
	#include "drvGPIO_K.h"
#elif CHIP_ID == CHIP_TSUMU || CHIP_ID == CHIP_TSUMV \
	|| CHIP_ID == CHIP_TSUMY|| CHIP_ID == CHIP_TSUMB \
	||CHIP_ID == CHIP_TSUM2 ||CHIP_ID == CHIP_TSUMC \
	|| CHIP_ID == CHIP_TSUMK||CHIP_ID == CHIP_TSUMD \
	||CHIP_ID == CHIP_TSUMJ ||CHIP_ID == CHIP_TSUM9 \
	|| CHIP_ID == CHIP_TSUMF
	#include "drvGPIO_other.h"
#else
	#message "please implement drvGPIO for new chip"
#endif
#endif

