///////////////////////////////////////////////////////////////////////////////
/// @file ColorPalette.h
/// @brief
/// @author MStarSemi Inc.
///
/// Functions to load Color Palette
///
/// Features
///  -
///////////////////////////////////////////////////////////////////////////////

#ifndef _COLORPALETTE_H
#define _COLORPALETTE_H

#ifdef _COLORPALETTE_C
#define INTERFACE
#else
#define INTERFACE   extern
#endif

typedef enum
{ CP_LOGO_BLACK,
#ifndef _AW_OSD_STYLE_
  CP_LOGO_BLUE,
#else
  CP_LOGO_GRAY,
#endif
  CP_LOGO_WHITE,
  CP_LOGO_RBLUE,
  CP_LOGO_WBLUE,   //ENABLE_ENERGY_STAR_LOGO, Steven100206

  CP_LOGO_MAX
} CP_LogoColor;



typedef enum
{ 
  CP_WhiteColor=0x08,
  CP_RedColor=0x01,
  CP_LightBlue,
  CP_BlueColor,
  CP_WhiteColor1,
  CP_GrayColor,
  CP_LightColor,
  CP_BlackColor,
  CP_YellowColor=0x09,
  CP_MaxColor
} CP_ColorType;





//For S1
#define CP1_4_HM_SAPERATION 45
//Color Select
//-----------------------
#define CS_NORMAL           0
#define CS_HIGHLIGHT        1
#define CS_SELECTED         2
//-----------------------
#define MONO_COLOR(FColor,BkColor)  ((FColor)<<4|(BkColor))
#define FOUR_COLOR(ColorGroup)      ((ColorGroup)<<2)
#define EIGHT_COLOR(ColorGroup)     ((ColorGroup)<<3)

//INTERFACE void LoadLogoCP(void);
//INTERFACE void LoadNormalCP(void);

INTERFACE void Osd_LoadColorPalette( void );
INTERFACE void Osd_LoadColorPalette256( void );


#define CURSOR_TOP_R            0x00
#define CURSOR_TOP_G            0x00
#define CURSOR_TOP_B            0x00
#define CURSOR_COLOR_DELTA_R    0x00
#define CURSOR_COLOR_DELTA_G    0x00
#define CURSOR_COLOR_DELTA_B    0x00

#define CURSOR_DOWN_R           (CURSOR_TOP_R-CURSOR_COLOR_DELTA_R)
#define CURSOR_DOWN_G           (CURSOR_TOP_G-CURSOR_COLOR_DELTA_G)
#define CURSOR_DOWN_B           (CURSOR_TOP_B-CURSOR_COLOR_DELTA_B)

typedef struct
{
    BYTE u8Red;
    BYTE u8Green;
    BYTE u8Blue;
  #if CHIP_ID>=CHIP_TSUMV
    BYTE u8ColorKey;
  #endif
} ColorPaletteType;

#undef INTERFACE
#endif ///


