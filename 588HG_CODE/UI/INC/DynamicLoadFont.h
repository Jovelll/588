//extern void Osd_DynamicLoadFont( BYTE addr, BYTE *fontPtr, WORD num );
//#include "types.h"
#include "board.h"
//#include "global.h"
//#include "Debug.h"
//#include "Common.h"

#if LANGUAGE_TYPE == ASIA
#include "PropFontAsia.h"
#elif LANGUAGE_TYPE == SIXTEEN                     //090330 xiandi.yu
#include "PropFontSixteen.h"
#elif LANGUAGE_TYPE == AllLanguage                   //100325 xiandi.yu
#include "PropFontAllLanguage.h"
#else
#include "PropFontEurope.h"
#endif

