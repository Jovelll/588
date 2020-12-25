#ifndef _ADJSUT_H_
#define _ADJSUT_H_

#ifdef _ADJSUT_C_
  #define _ADJSUTDEC_
#else
  #define _ADJSUTDEC_ extern
#endif

typedef struct
{
  BYTE u8Red;
  BYTE u8Green;
  BYTE u8Blue;
} ColorType;

_ADJSUTDEC_ void mStar_AdjustBrightness( BYTE brightness );
_ADJSUTDEC_ void mStar_AdjustDcrBrightness( void );
_ADJSUTDEC_ void mStar_AdjustBlackLevel( BYTE BlackLevel );
_ADJSUTDEC_ void mStar_AdjustUserPrefBlacklevel( BYTE Redblacklevel, BYTE Greenblacklevel, BYTE Blueblacklevel );
_ADJSUTDEC_ void mStar_AdjustContrast( BYTE contrast );

_ADJSUTDEC_ void mStar_AdjustHPosition( WORD position );
_ADJSUTDEC_ void mStar_AdjustVPosition( WORD position );
#if UsesRGB
_ADJSUTDEC_ void mStar_AdjustRedColor( BYTE rColor, BYTE gColor, BYTE bColor );
_ADJSUTDEC_ void mStar_AdjustGreenColor( BYTE rColor, BYTE gColor, BYTE bColor );
_ADJSUTDEC_ void mStar_AdjustBlueColor( BYTE rColor, BYTE gColor, BYTE bColor );
#else
_ADJSUTDEC_ void mStar_AdjustRedColor( BYTE color, BYTE contrast );
_ADJSUTDEC_ void mStar_AdjustGreenColor( BYTE color, BYTE contrast );
_ADJSUTDEC_ void mStar_AdjustBlueColor( BYTE color, BYTE contrast );
#endif
_ADJSUTDEC_ void mStar_AdjustVolume( BYTE volume );

_ADJSUTDEC_ void mStar_WaitForDataBlanking( void );
_ADJSUTDEC_ void mStar_AdjustBackgoundColor( BYTE pattern );
#endif
