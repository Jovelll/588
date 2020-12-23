///////////////////////////////////////////////////////////////////////////////
/// @file LoadPropFont1218.h
/// @brief
/// @author MStarSemi Inc.
///
/// The Prop. font generating and loading function.
///
/// Features
///  -Left/Center/Right Align
///  -Prop Font shift up/down
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _LOADPROPFONT1218_H
#define _LOADPROPFONT1218_H
#include "Types.h"

#ifdef _LOADPROPFONT1218_C
#define INTERFACE
#else
#define INTERFACE   extern
#endif

#define FONT_WIDTH      12
#define FONT_HEIGHT     18
#define SPACE1PIXEL     0x01
#define SPACE2PIXEL     0x02
#define SPACE3PIXEL     0x03
#define LEFT_ALIGN      0x00
//#define RIGHT_ALIGN     0x04
#define CENTER_ALIGN    0x0C
#define SP_BETWEEN_FONT (g_u8PropFontFlags&0x03)
#define IS_LEFT_ALIGN   ((g_u8PropFontFlags&0x0C)==0x00)
#define IS_RIGHT_ALIGN  ((g_u8PropFontFlags&0x0C)==0x04)
#define IS_CENTER_ALIGN ((g_u8PropFontFlags&0x0C)==0x0C)

//#define PORT_FONT_ADDR  OSD1_66
//#define PORT_FONT_DATA  OSD1_6C

typedef struct
{ BYTE u8SpaceWidth;
  WORD tLineData[18];
} PropFontNonCompressType;

#ifndef _LOADPROPFONT1218_C
extern xdata PropFontNonCompressType *pstPropFontSet1218;
extern BYTE xdata g_u8PropFontFlags;
extern BYTE xdata g_u8AlignResetIndex;
extern BYTE xdata g_u8ByPassLength;
#endif

INTERFACE BYTE LoadPropFonts1218(BYTE u8Addr, BYTE *pu8Strings, WORD u16FontCount, BYTE *pu8Strings1, WORD u16FontCount1, BYTE u8UDShift ,BYTE u8UDShift1);
INTERFACE void OSDLoadOneFont(WORD* pu16SN);
INTERFACE void ClearFontBuf(WORD* pu16SN);
INTERFACE BYTE LoadShiftBmpFont(BYTE u8Addr, BYTE *u8FontTbl, WORD u16Size, BYTE u8UDShift);

#undef INTERFACE
#endif
