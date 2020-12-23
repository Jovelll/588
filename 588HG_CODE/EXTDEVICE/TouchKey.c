
#include "global.h"

#if ENABLE_TOUCH_KEY

#if TOUCH_KEY_SOURCE == TOUCH_KEY_SOURCE_ITE
#include "IT7230_TouchKey.c"
#include "IT7230_I2C.c"
#elif TOUCH_KEY_SOURCE == TOUCH_KEY_SOURCE_SMSC

#elif TOUCH_KEY_SOURCE == TOUCH_KEY_SOURCE_CYPRESS

#else
#message "please implement function for new touch key source"
#endif

#else
BYTE code TouchKeyNullData[] = {0};

void TouchKeyDummy(void)
{
    BYTE xdata i = TouchKeyNullData[0];
}
#endif
