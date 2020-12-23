///////////////////////////////////////////////////////////////////////////////
/// @file ColorPalette.c
/// @brief
/// @author MStarSemi Inc.
///
/// Functions to load color palette.
///
/// Features
///  -
///////////////////////////////////////////////////////////////////////////////
#define _COLORPALETTE_C

#include "types.h"
#include "board.h"
#include "ms_reg.h"
#include "Global.h"
#include "menudef.h"
#include "ColorPalette.h"
#include "ColorPaletteTbl.h"
//#include "debugMsg.h"
#include "ms_rwreg.h"
//#include "mstar.h"
#include "drvOSD.h"
#include "msOSD.h"

#include "Common.h"

ColorPaletteType code tOSDColorPalette0[] =   //2bit logo
{
	{0xFF, 0xF8, 0xFF,	  CP_COLORKEY_F},  //  {0xFF, 0xFF, 0xFF,	 CP_COLORKEY_F},  // Palette 0 // White
	{0xFF, 0x00, 0x00,	  CP_COLORKEY_F},  // Palette 1 // Red
	{0x00, 0x80, 0xff,	  CP_COLORKEY_F},  // Palette 2 //lightblue Green
	{0x00, 0x00, 0xFF,	  CP_COLORKEY_F},  // Palette 3 // Blue

	{0xF6, 0xB8, 0x00,	  CP_COLORKEY_F},  // Palette 4 // Envision PowerOnLogo "O"Color
	{0xc0, 0xc0, 0xc0,	  CP_COLORKEY_F},  // Palette 5 //gray
	{0x49, 0x56, 0x77,	  CP_COLORKEY_F},  // Palette 6 //Main Logo Color +AOC power On
	{0x00, 0x00, 0x00,	  CP_COLORKEY_F},  // Palette 7

	{0xFF, 0xF8, 0xFF,	  CP_COLORKEY_F},  // {0xFF, 0xFF, 0xFF,	CP_COLORKEY_F},  // Palette 8 // White

};

//--color

ColorPaletteType code tOSDColorPalette256[] =
{
    // 0x00
  {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F}, // {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F},  // 0
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 1
    {0x30, 0x90, 0xE0,  CP_COLORKEY_F},  // 2
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 3

    {0x80, 0xC0, 0xF0,  CP_COLORKEY_F},  // 4
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 5
    {0x30, 0x90, 0xE0,  CP_COLORKEY_F},  // 6
   {0xFF, 0xF8, 0xF8,  CP_COLORKEY_F},// {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F},  // 7

    // 0x08
    {0x00, 0x00, 0x00,  CP_COLORKEY_F},  // 8   // {0x30, 0x90, 0xE0,  CP_COLORKEY_F},  // 8
    {0xE0, 0x10, 0xA0,  CP_COLORKEY_F},  // 9
   {0xFF, 0xFF, 0x00,  CP_COLORKEY_F},// {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F},  // A
    {0x30, 0x90, 0xE0,  CP_COLORKEY_F},  // B

    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // C
    {0xB0, 0xB0, 0xB0,  CP_COLORKEY_F},  // D
    {0xFF, 0xF8, 0xF8,  CP_COLORKEY_F},//  {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F},  // E
    {0xF0, 0x00, 0x00,  CP_COLORKEY_F},  // F
    
//=========4 COLOR =========0x10//------------------
    {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F},  // 0
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 0
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 0
    {0xF0, 0x00, 0x00,  CP_COLORKEY_F},  // 0
  // {0x00, 0x00, 0x00,    CP_COLORKEY_F},           // 0
  // {0xFF, 0xFF, 0xFF,    CP_COLORKEY_F},           // 1
  //  {0xFF, 0x00, 0x00,    CP_COLORKEY_F},           // 2
  // {NULL, NULL, NULL,    CP_COLORKEY_F},           // 3」
    
    {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F},  // 0
    {0x80, 0xC0, 0xF0,  CP_COLORKEY_F},  // 0
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 0
    {0xF0, 0x00, 0x00,  CP_COLORKEY_F},  // 0
// 0X18
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 0
    {0x50, 0xA0, 0xF0,  CP_COLORKEY_F},  // 0
    {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F},  // 0
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 0
    
    {0x80, 0xC0, 0xF0,  CP_COLORKEY_F},  // 0
    {0x50, 0xA0, 0xF0,  CP_COLORKEY_F},  // 0
    {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F},  // 0
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 0
// 0X20
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 0
    {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F},  // 0
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 0
    {0x00, 0x00, 0x00,  CP_COLORKEY_F},  // 0
    
    {0x80, 0xC0, 0xF0,  CP_COLORKEY_F},  // 0
    {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F},  // 0
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 0
    {0x00, 0x00, 0x00,  CP_COLORKEY_F},  // 0
 //==========4 COLOR END======0x28
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 0
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 0
    {0x00, 0x00, 0xF0,  CP_COLORKEY_F},  // 0
    {0x00, 0x00, 0xF0,  CP_COLORKEY_F},  // 0
    
    {0x60, 0x60, 0x60,  CP_COLORKEY_F},  // 0
    {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F},  // 0
    {0xF0, 0x00, 0x00,  CP_COLORKEY_F},  // 0
    {0x00, 0x80, 0x00,  CP_COLORKEY_F},  // 0
  //========8 COLOR ============0x30
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 0
    {0xF0, 0xF0, 0x00,  CP_COLORKEY_F},  // 0
    {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F},  // 0
    {0x00, 0x00, 0x00,  CP_COLORKEY_F},  // 0
    
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 0
    {0xD0, 0xD0, 0x00,  CP_COLORKEY_F},  // 0
    {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F},  // 0
    {0x00, 0x00, 0x00,  CP_COLORKEY_F},  // 0
// 0X38
    {0x80, 0xC0, 0xF0,  CP_COLORKEY_F},  // 0
    {0xD0, 0xD0, 0x00,  CP_COLORKEY_F},  // 0
    {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F},  // 0
    {0x00, 0x00, 0x00,  CP_COLORKEY_F},  // 0
    
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 0
    {0x00, 0xF0, 0x00,  CP_COLORKEY_F},  // 0
    {0xF0, 0x00, 0xF0,  CP_COLORKEY_F},  // 0
    {0x10, 0x10, 0x10,  CP_COLORKEY_F},  // 0
// 0X40
   {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 0
    {0xF0, 0x00, 0x00,  CP_COLORKEY_F},  // 0
    {0x00, 0x00, 0xE0,  CP_COLORKEY_F},  // 0
    {0x00, 0xF0, 0x00,  CP_COLORKEY_F},  // 0
  
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 0
    {0xF0, 0x00, 0x00,  CP_COLORKEY_F},  // 0
    {0x00, 0x00, 0xE0,  CP_COLORKEY_F},  // 0
    {0x00, 0xF0, 0x00,  CP_COLORKEY_F},  // 0
// 0X48
    {0x80, 0xC0, 0xF0,  CP_COLORKEY_F},  // 0
    {0xF0, 0x00, 0x00,  CP_COLORKEY_F},  // 0
    {0x00, 0x00, 0xE0,  CP_COLORKEY_F},  // 0
    {0x00, 0xF0, 0x00,  CP_COLORKEY_F},  // 0
    
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 0
    {0x00, 0x00, 0x00,  CP_COLORKEY_F},  // 0
    {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F},  // 0
    {0x80, 0x90, 0xC0,  CP_COLORKEY_F},  // 0
// 0X50
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 0
    {0x00, 0x00, 0x00,  CP_COLORKEY_F},  // 0
    {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F},  // 0
    {0x80, 0x90, 0xC0,  CP_COLORKEY_F},  // 0
    
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 0
    {0x20, 0x80, 0xD0,  CP_COLORKEY_F},  // 0
    {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F},  // 0
    {0x80, 0x90, 0xC0,  CP_COLORKEY_F},  // 0
// 0X58
    {0x80, 0xC0, 0xF0,  CP_COLORKEY_F},  // 0
    {0x00, 0x00, 0x00,  CP_COLORKEY_F},  // 0
    {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F},  // 0
    {0x80, 0x90, 0xC0,  CP_COLORKEY_F},  // 0
    
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 0
    {0x20, 0x80, 0xD0,  CP_COLORKEY_F},  // 0
    {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F},  // 0
    {0x80, 0x90, 0xC0,  CP_COLORKEY_F},  // 0
//===========hotkeyMenu color Palette=//
//HotKeyDCROnMenu		//0x60
    {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F},  // 0
    {0x00, 0x00, 0x00,  CP_COLORKEY_F},  // 0
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 0
    {0x80, 0x90, 0xC0,  CP_COLORKEY_F},  // 0
//HotKeyDCROffMenu		//0x64
    {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F},  // 0
    {0x00, 0x00, 0x00,  CP_COLORKEY_F},  // 0
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 0
    {0xF0, 0x00, 0x00,  CP_COLORKEY_F},  // 0
//HotKeyECOStandardMenu	//0x68
    {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F},  // 0
    {0x80, 0xC0, 0xF0,  CP_COLORKEY_F},  // 0
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 0
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 0
//HotKeyECOWebMenu		//0x6C
    {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F},  // 0
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 0
    {0x80, 0xC0, 0xF0,  CP_COLORKEY_F},  // 0
    {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F},  // 0
//HotKeyECOGameMenu		//0x70
    {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F},  // 0
    {0x80, 0xC0, 0xF0,  CP_COLORKEY_F},  // 0
    {0xF0, 0xF0, 0x00,  CP_COLORKEY_F},  // 0
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 0
//HotKeyECOMoveMenu		//0x74
    {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F},  // 0
    {0x00, 0x50, 0xA0,  CP_COLORKEY_F},  // 0
    {0xF0, 0x00, 0x00,  CP_COLORKEY_F},  // 0
    {0xF0, 0xF0, 0x00,  CP_COLORKEY_F},  // 0
							//--0X78
    {0x30, 0x90, 0xE0,  CP_COLORKEY_F},  // 0
    {0xF0, 0x00, 0x00,  CP_COLORKEY_F},  // 0
    {0xE0, 0x00, 0x70,  CP_COLORKEY_F},  // 0
    {0x00, 0x00, 0x00,  CP_COLORKEY_F},  // 0
   							 //--0X7C
    {0xF0, 0xF0, 0xF0,  CP_COLORKEY_F},  // 0
    {0xB0, 0xB0, 0xB0,  CP_COLORKEY_F},  // 0
    {0x00, 0x00, 0x00,  CP_COLORKEY_F},  // 0
    {0x00, 0x00, 0x00,  CP_COLORKEY_F},  // 0
							//--0X80
    {0x30, 0x90, 0xE0,  CP_COLORKEY_F},  // --OK
    {0x00, 0x00, 0x00,  CP_COLORKEY_F},  // 0
    {NULL, NULL, NULL,  CP_COLORKEY_F},  // no use,
    {NULL, NULL, NULL,  CP_COLORKEY_F},  // no use

    {NULL, NULL, NULL,  CP_COLORKEY_F},  // no use,
    {NULL, NULL, NULL,  CP_COLORKEY_F},  // no use,
    {NULL, NULL, NULL,  CP_COLORKEY_F},  // no use,
    {NULL, NULL, NULL,  CP_COLORKEY_F},  // no use

    {NULL, NULL, NULL,  CP_COLORKEY_F},  // no use,
    {NULL, NULL, NULL,  CP_COLORKEY_F},  // no use,
    {NULL, NULL, NULL,  CP_COLORKEY_F},  // no use,
    {NULL, NULL, NULL,  CP_COLORKEY_F},  // no use

    {NULL, NULL, NULL,  CP_COLORKEY_F},  // no use,
    {NULL, NULL, NULL,  CP_COLORKEY_F},  // no use,
    {NULL, NULL, NULL,  CP_COLORKEY_F},  // no use,
    {NULL, NULL, NULL,  CP_COLORKEY_F},  // no use

#if DisplayLogo!=NoBrand
#if DisplayLogo ==NANCHANG
//NANCAHNG					//0x90
    {0xFF, 0xFF, 0xFF,  CP_COLORKEY_F},  // 0
    {0x21, 0x8C, 0xFF,  CP_COLORKEY_F},  // 0
    {0xEF, 0x8C, 0x21,  CP_COLORKEY_F},  // 0
    {0x00, 0x00, 0x00,  CP_COLORKEY_F},  // 0
#elif DisplayLogo ==EPAI
//EPAI						
    {0xFF, 0xFF, 0xFF,  CP_COLORKEY_F},  // 0
    {0x21, 0x18, 0x18,  CP_COLORKEY_F},  // 0
    {0x29, 0x18, 0x18,  CP_COLORKEY_F},  // 0
    {NULL, NULL, NULL, CP_COLORKEY_F},  // 0
#elif DisplayLogo ==MAYA
//MAYA						
    {0xFF, 0xFF, 0xFF,  CP_COLORKEY_F},  // 0
    {0x00, 0x4A, 0x18,  CP_COLORKEY_F},  // 0
    {0x31, 0x42, 0x29,  CP_COLORKEY_F},  // 0
    {NULL, NULL, NULL, CP_COLORKEY_F},  // 0
#elif DisplayLogo ==IMAGIC
 //IMAGIC					
    {0xFF, 0xFF, 0xFF,  CP_COLORKEY_F},  // 0
    {0xE7, 0x5A, 0x31,  CP_COLORKEY_F},  // 0
    {0x8C, 0x8C, 0x8C,  CP_COLORKEY_F},  // 0
    {0xEF, 0xEF, 0xEF, CP_COLORKEY_F},  // 0
#elif DisplayLogo ==FANGZHENG
//FANGZHENG                         
    {0xFF, 0xFF, 0xFF,  CP_COLORKEY_F},  // 0
    {0xFF, 0x00, 0x00,  CP_COLORKEY_F},  // 0
    {0xFF, 0x7B, 0x7B,  CP_COLORKEY_F},  // 0
    {NULL, NULL, NULL, CP_COLORKEY_F},  // 0
#elif DisplayLogo ==GRC
//GRC                         		
    {0xFF, 0xFF, 0xFF,  CP_COLORKEY_F},  // 0
    {0xFF, 0x00, 0x00,  CP_COLORKEY_F},  // 0
    {0xFF, 0x7B, 0x7B,  CP_COLORKEY_F},  // 0
    {NULL, NULL, NULL, CP_COLORKEY_F},  // 0
#elif DisplayLogo ==BUBALUS
//BUBALUS                        		
    {0x00, 0x00, 0x00,  CP_COLORKEY_F},  // 0
    {0xEF, 0xEF, 0xEF,  CP_COLORKEY_F},  // 0
    {NULL, NULL, NULL,  CP_COLORKEY_F},  // 0
    {NULL, NULL, NULL, CP_COLORKEY_F},  // 0
#elif DisplayLogo ==BUBALUSWHITE
//BUBALUSWHITE                       
    {0xFF, 0xFF, 0xFF,  CP_COLORKEY_F},  // 0
    {0xC6, 0xCE, 0xCE,  CP_COLORKEY_F},  // 0
    {NULL, NULL, NULL,  CP_COLORKEY_F},  // 0
    {NULL, NULL, NULL, CP_COLORKEY_F},  // 0
#elif DisplayLogo ==SAMPIN
//SAMPIN                               
    {0xFF, 0xFF, 0xFF,  CP_COLORKEY_F},  // 0
    {0xA5, 0xA5, 0xA5,  CP_COLORKEY_F},  // 0
    {0x29, 0x29 ,0x29,  CP_COLORKEY_F},  // 0
    {0x63, 0x63 ,0x6B, CP_COLORKEY_F},  // 0
#elif DisplayLogo ==PCTOP
//PCTOP								
	{0x00, 0x00, 0xFF,  CP_COLORKEY_F},  // 0
    {0x80, 0x8A, 0x87,  CP_COLORKEY_F},  // 0
    {NULL, NULL, NULL,  CP_COLORKEY_F},  // 0
    {NULL, NULL, NULL, CP_COLORKEY_F},  // 0
#elif DisplayLogo ==FOXIN
//foxin						
	{0x00, 0x00, 0x00,  CP_COLORKEY_F},  // 0
    {0xFF, 0xFF, 0xFF,  CP_COLORKEY_F},  // 0
    {NULL, NULL, NULL,  CP_COLORKEY_F},  // 0
    {NULL, NULL, NULL, CP_COLORKEY_F},  // 
#elif (DisplayLogo ==ZEBION || DisplayLogo == INFORCE)							
	{0x00, 0x00, 0x00,  CP_COLORKEY_F},  // 0
    {0xFF, 0xFF, 0xFF,  CP_COLORKEY_F},  // 0
    {NULL, NULL, NULL,  CP_COLORKEY_F},  // 0
    {NULL, NULL, NULL, CP_COLORKEY_F},  // 0
#elif DisplayLogo ==SUGON
	{0xFF, 0xFF, 0xFF,  CP_COLORKEY_F},  // 0
    {0xBD, 0x10, 0x18,  CP_COLORKEY_F},  // 0
    {NULL, NULL, NULL,  CP_COLORKEY_F},  // 0
    {NULL, NULL, NULL, CP_COLORKEY_F},  // 0
#elif DisplayLogo ==SONGUAN								
	{0xFF, 0xFF, 0xFF,  CP_COLORKEY_F},  // 0
    {0xCE, 0x21, 0x63,  CP_COLORKEY_F},  // 0
    {NULL, NULL, NULL,  CP_COLORKEY_F},  // 0
    {NULL, NULL, NULL, CP_COLORKEY_F},  // 0
#elif DisplayLogo ==MAGIX
{0x00, 0x00, 0x00,	CP_COLORKEY_F},  // 0
{0xDE, 0x29, 0x21,	CP_COLORKEY_F},  // 0
{0xFF, 0xFF, 0xFF,	CP_COLORKEY_F},  // 0
{NULL, NULL, NULL, CP_COLORKEY_F},	// 0
#elif DisplayLogo ==DRAZILPC
{0x18, 0x18, 0xFA,	CP_COLORKEY_F},  // 0
{0xFF, 0xFF, 0xFF,	CP_COLORKEY_F},  // 0
{0x9C, 0xCE, 0x42,	CP_COLORKEY_F},  // 0
{0XF7, 0XAD, 0X18, CP_COLORKEY_F},	// 0
#elif DisplayLogo ==MEGAONIC
{0x03, 0x03, 0xF0,	CP_COLORKEY_F},  // 0
{0xFF, 0xFF, 0xFF,	CP_COLORKEY_F},  // 0
{NULL, NULL, NULL,	CP_COLORKEY_F},  // 0
{NULL, NULL, NULL, CP_COLORKEY_F},	// 0


#endif
#endif
};


#if CHIP_ID>=CHIP_TSUMV
void Load256ColorPalette(BYTE u8Group, BYTE u8Addr, ColorPaletteType *colorPtr, WORD u16Num)
{

	#if CHIP_ID==CHIP_TSUM2||CHIP_ID==CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID==CHIP_TSUMD||CHIP_ID==CHIP_TSUMJ || CHIP_ID == CHIP_TSUM9 || CHIP_ID == CHIP_TSUMF
	 WORD i;
    if (u8Group==0)
        WRITE_PSRAM0_ADDR();
    else
        WRITE_PSRAM1_ADDR();
    //write R/G/B
    msWriteByte(OSD1_66, u8Addr);
    if (u8Group==0)
        WRITE_PSRAM0_COLOR();
    else
        WRITE_PSRAM1_COLOR();

    for (i=0;i<u16Num;i++)
    {
        msWriteByte(OSD1_66, ((colorPtr+i)->u8Blue));
        msWriteByte(OSD1_66, ((colorPtr+i)->u8Green));
        msWriteByte(OSD1_66, ((colorPtr+i)->u8Red));
    }

    //write colorkey
    if (u8Group==0)
        WRITE_PSRAM0_ADDR();
    else
        WRITE_PSRAM1_ADDR();
    msWriteByte(OSD1_66, u8Addr);
    if (u8Group==0)
        WRITE_PSRAM0_COLOR_KEY();
    else
        WRITE_PSRAM0_COLOR_KEY();
    while (u16Num--)
    {
        if(colorPtr->u8ColorKey)
            msWriteByteMask(OSD1_66, BIT0,BIT0);
        else
            msWriteByteMask(OSD1_66, 0,BIT0);
        colorPtr++;
    }


	#else


    WORD xdata i,u16PSRAM_AddrPort;
    BYTE u8Bank=msReadByte(OSD1_00);
    msWriteByte(OSD1_00,0x00);//subbank 0
    if (u8Group==0)
        u16PSRAM_AddrPort=OSD1_76;
    else
        u16PSRAM_AddrPort=OSD1_86;
//u8Addr=0;
    //write R/G/B
    msWriteByte(u16PSRAM_AddrPort, u8Addr);
    for (i=0;i<u16Num;i++)
    {
        msWriteByte(u16PSRAM_AddrPort+2, ((colorPtr+i)->u8Blue));
        msWriteByte(u16PSRAM_AddrPort+2, ((colorPtr+i)->u8Green));
        msWriteByte(u16PSRAM_AddrPort+2, ((colorPtr+i)->u8Red));
    }

    //write colorkey
    msWriteByte(u16PSRAM_AddrPort, u8Addr);
    while (u16Num--)
    {
        if(colorPtr->u8ColorKey)
            msWriteByteMask(u16PSRAM_AddrPort+3, BIT0,BIT0);
        else
            msWriteByteMask(u16PSRAM_AddrPort+3, 0,BIT0);
        colorPtr++;
    }
    msWriteByte(OSD1_00,u8Bank);//subbank 0
    #endif
}
#else
void Load256ColorPalette(BYTE u8Group, BYTE u8Addr, ColorPaletteType *colorPtr, WORD u16Num)
{
    u8Group=u8Group;
    msWriteByte(OSD2_AC, u8Addr);
    while (u16Num--)
    {
        msWriteByte(OSD2_AD, (colorPtr->u8Red));
        msWriteByte(OSD2_AD, (colorPtr->u8Green));
        msWriteByte(OSD2_AD, (colorPtr->u8Blue));
        colorPtr++;
    }
}
#endif

void Osd_LoadColorPalette( void )
{

    Load256ColorPalette(0, 0, &tOSDColorPalette0, sizeof( tOSDColorPalette0 )/sizeof(ColorPaletteType) );
}

void Osd_LoadColorPalette256( void )
{

    Load256ColorPalette(0, 0, &tOSDColorPalette256, sizeof( tOSDColorPalette256 )/sizeof(ColorPaletteType) );
}


