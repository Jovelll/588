#include "board.h"
#include "types.h"
#include "CommonFont.h"
#include "msOSD.h"
#include "global.h"
#include "ms_reg.h"
#include "ms_rwreg.h"
#include "common.h"

#include "Menudef.h"
//#include "extlib.h"
#include "colorpalette.h"
#if CHIP_ID == CHIP_TSUMU
#include "colorpalettetbl.h"
#endif
#include "loadpropfont1218.h"

#include "drvOSD.h"
#include "LoadCommonFont.h"

extern void LoadCompressColorFont(BYTE *pu8FontTbl, WORD *pu16MapPtr, BYTE countu8Count);
extern void mStar_LoadCompressedFont(BYTE u8Addr, WORD *pu16FontPtr, WORD u16Count);

extern xdata BYTE MenuPageIndex;
extern xdata BYTE MenuItemIndex;



void LoadCommonFontUncall(void);

//8 color font raw data
void Osd_Load8ColorFont(BYTE u8Addr, BYTE *pu8Font, WORD u16Num)
{
    #if CHIP_ID>=CHIP_TSUMV
    DWORD xdata u32Data;
    BYTE xdata u8FontData0,u8FontData1,u8FontData2;
    BYTE xdata u8PixelBit;
    BYTE xdata i;
    #if CHIP_ID==CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID==CHIP_TSUMD||CHIP_ID==CHIP_TSUMJ || CHIP_ID == CHIP_TSUM9 || CHIP_ID == CHIP_TSUMF
    msWriteByteMask(OSD1_6D, BIT6,BIT7|BIT6);   //00: for 1/2Bpp; 01: for 3Bpp; 10:for 4Bpp;
    #elif  CHIP_ID==CHIP_TSUM2||CHIP_ID==CHIP_TSUMV
    msWriteByteMask(OSD1_6D, 0x00,BIT7);  //0: for 1/2/3Bpp; 1: for 4Bpp;
    #endif

    WRITE_CAFSRAM_ADDR();
    msWrite2Byte(PORT_FONT_ADDR, GET_FONT_RAM_ADDR(u8Addr));
    WRITE_FONT();
    // handle 2 lines, 8 pixels each time, 2 lines x 12 pixels = 8 pixels x 3 times
    while(u16Num)
    {
        for (i=0;i<3;i++)
        {
        //    0        1        2        3        4        5        6        7        8
        //11110000 00000000 11111111 00002222 22222222 00000000 22221111 11111111 22222222 //line 0/1
            if (i==0)
            {
                u8FontData0=((*(pu8Font))<<4)+((*(pu8Font+1))>>4);
                u8FontData1=((*(pu8Font))&0xF0)+((*(pu8Font+2))>>4);
                u8FontData2=((*(pu8Font+3))<<4)+((*(pu8Font+4))>>4);
            }
            else if (i==1)
            {
                u8FontData0=((*(pu8Font+1))<<4)+((*(pu8Font+3))>>4);
                u8FontData1=((*(pu8Font+2))<<4)+((*(pu8Font+6))&0x0F);
                u8FontData2=((*(pu8Font+4))<<4)+((*(pu8Font+6))>>4);
            }
            else
            {
                u8FontData0=(*(pu8Font+5));
                u8FontData1=(*(pu8Font+7));
                u8FontData2=(*(pu8Font+8));
            }
            u8PixelBit=BIT7;
            u32Data=0;
            while(u8PixelBit)
            {
                u32Data<<=3;
                if (u8FontData0&u8PixelBit) //color bit0
                    u32Data|=BIT0;
                if (u8FontData1&u8PixelBit) //color bit1
                    u32Data|=BIT1;
                if (u8FontData2&u8PixelBit) //color bit2
                    u32Data|=BIT2;
                u8PixelBit>>=1;
            }
            msWriteByte(PORT_FONT_DATA, u32Data>>16);
            msWriteByte(PORT_FONT_DATA, u32Data>>8);
            msWriteByte(PORT_FONT_DATA, u32Data);
        }
        if (u16Num>=9)
        {
            u16Num-=9;
            pu8Font+=9;
        }
        else
            u16Num=0;
    }
    #if CHIP_ID==CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID==CHIP_TSUMD||CHIP_ID==CHIP_TSUMJ || CHIP_ID==CHIP_TSUM9 || CHIP_ID==CHIP_TSUMF
    	msWriteByteMask(OSD1_6D, 0x00,BIT7|BIT6);    //00: for 1/2Bpp; 01: for 3Bpp; 10:for 4Bpp;
    #elif  CHIP_ID==CHIP_TSUM2||CHIP_ID==CHIP_TSUMV
   	msWriteByteMask(OSD1_6D, 0x00,BIT7);  		  //0: for 1/2/3Bpp; 1: for 4Bpp;
    #endif
    #else
    msWriteByte(OSD2_A3, u8Addr);
    while (u16Num--)
        msWriteByte(OSD2_A4, *(pu8Font++));
    #endif
}
void Osd_LoadFont( BYTE addr, BYTE *fontPtr, WORD num )
{
	msWriteByte( OSD2_A3, addr );
	while( num-- )
		{
			msWriteByte( OSD2_A4, *( fontPtr++ ) );
		}

}
void Osd_LoadCompressMonoFont(BYTE u8Addr, WORD *pu16FontPtr, WORD u16Count)
{
	BYTE addr;
	#if CHIP_ID > CHIP_TSUMV 
	addr = GET_FONT_RAM_ADDR(u8Addr);
	#else// for osd edge font error, need add chip use CompFontTSUMV.LIB
	addr = u8Addr;
	#endif
	mStar_LoadCompressedFont( addr, pu16FontPtr, u16Count);
}

void Osd_LoadCompressColorFont( WORD addr, BYTE *fontPtr, WORD *mapPtr, BYTE count )
{
    if(( addr & 0xFF00 ) == 0x100 )
    {
        OSD_FONT_HI_ADDR_SET_BIT8();
    }
    else if(( addr & 0xFF00 ) == 0x200 )
    {
        OSD_FONT_HI_ADDR_SET_BIT9();
    }
    else
    {
        OSD_FONT_HI_ADDR_CLR_TO_0();
    }
	OSD_WRITE_FONT_ADDRESS(LOBYTE(addr));
	LoadCompressColorFont( fontPtr, mapPtr, count );
	OSD_FONT_HI_ADDR_CLR_TO_0();
}

void Osd_Write4ColorFontStartAddr( WORD wAddr )
{
	#if CHIP_ID>=CHIP_TSUMV
	msWrite2ByteMask(OSD1_0A,wAddr,0x03FF); // 4 color start
	#else
	if(( wAddr & 0xFF00 ) == 0x100 )
	msWriteByte( OSD1_41, ( msReadByte( OSD1_41 )&( ~( BIT3 | BIT2 ) ) ) | BIT3 );
	else if(( wAddr & 0xFF00 ) == 0x200 )
	msWriteByte( OSD1_41, ( msReadByte( OSD1_41 )&( ~( BIT3 | BIT2 ) ) ) | BIT2 );
	else if(( wAddr & 0xFF00 ) == 0x300 )
	msWriteByte( OSD1_41, ( msReadByte( OSD1_41 )&( ~( BIT3 | BIT2 ) ) ) | BIT3 | BIT2 );
	else
	msWriteByte( OSD1_41, ( msReadByte( OSD1_41 )&( ~( BIT3 | BIT2 ) ) ) );
	msWriteByte( OSD1_11, ( BYTE )wAddr ); // color font start address
	#endif
}

void Osd_Write8ColorFontStartAddr( WORD wAddr )
{
	#if CHIP_ID>=CHIP_TSUMV
	msWrite2ByteMask(OSD1_0C,wAddr,0x03FF); // 8 color start
	#else
	if(( wAddr & 0xFF00 ) == 0x100 )
	msWriteByte( OSD1_41, ( msReadByte( OSD1_41 )&( ~( BIT7 | BIT6 ) ) ) | BIT7 );
	else if(( wAddr & 0xFF00 ) == 0x200 )
	msWriteByte( OSD1_41, ( msReadByte( OSD1_41 )&( ~( BIT7 | BIT6 ) ) ) | BIT6 );
	else if(( wAddr & 0xFF00 ) == 0x300 )
	msWriteByte( OSD1_41, ( msReadByte( OSD1_41 )&( ~( BIT7 | BIT6 ) ) ) | BIT7 | BIT6 );
	else
	msWriteByte( OSD1_41, ( msReadByte( OSD1_41 )&( ~( BIT7 | BIT6 ) ) ) );
	msWriteByte( OSD1_3B, ( BYTE )wAddr );
	#endif
}

#if DisplayLogo!=NoBrand
void Osd_LoadLogoFontCP( void )
{

#if DisplayLogo == TERRA
MonoColorSetting();
Osd_LoadCompressMonoFont(1, &tOSDLogoFont, sizeof(tOSDLogoFont));

#else 
	Osd_Write4ColorFontStartAddr( Logo_4C );
	Osd_Write8ColorFontStartAddr( 0x3FF );

	Osd_LoadCompressColorFont( Logo_4C, &tLogo_4C, NULL, Logo_Size);
  
#endif
}
#endif

#if 1
//vs add ccflag
void Osd_LoadNormalFontTable_LG2005_WordByLanguage(void)
{
 #if CHIP_ID==CHIP_TSUMV
      		if (UserPrefLanguage==LANG_SimChinese)
      		  mStar_LoadCompressedFont((FONT_DYNIC_START),    &tOSDFont_SimChiness, sizeof(tOSDFont_SimChiness));      
      #if Language_TraChinese	
      		else if (UserPrefLanguage==LANG_TraChinese)
      		       mStar_LoadCompressedFont((FONT_DYNIC_START),    &tOSDFont_TraChiness, sizeof(tOSDFont_TraChiness));       
      #endif	
      #if Language_Russian
      		else if (UserPrefLanguage==LANG_Russian)
      		       mStar_LoadCompressedFont((FONT_DYNIC_START),    &tOSDFont_Russia, sizeof(tOSDFont_Russia));       
      #endif	
      #if Language_Korean
      		else if (UserPrefLanguage==LANG_Korean)
      		       mStar_LoadCompressedFont((FONT_DYNIC_START),    &tOSDFont_Korean, sizeof(tOSDFont_Korean));     
      #endif	
      		else
      		       mStar_LoadCompressedFont((FONT_DYNIC_START),    &tOSDFont_English, sizeof(tOSDFont_English));       
  #else
    	if (UserPrefLanguage==LANG_SimChinese)
    	  mStar_LoadCompressedFont(GET_FONT_RAM_ADDR(FONT_DYNIC_START),    &tOSDFont_SimChiness, sizeof(tOSDFont_SimChiness));      
    #if Language_TraChinese	
    	else if (UserPrefLanguage==LANG_TraChinese)
    	       mStar_LoadCompressedFont(GET_FONT_RAM_ADDR(FONT_DYNIC_START),    &tOSDFont_TraChiness, sizeof(tOSDFont_TraChiness));       
    #endif	
    #if Language_Russian
    	else if (UserPrefLanguage==LANG_Russian)
    	       mStar_LoadCompressedFont(GET_FONT_RAM_ADDR(FONT_DYNIC_START),    &tOSDFont_Russia, sizeof(tOSDFont_Russia));       
    #endif	
    #if Language_Korean
    	else if (UserPrefLanguage==LANG_Korean)
    	       mStar_LoadCompressedFont(GET_FONT_RAM_ADDR(FONT_DYNIC_START),    &tOSDFont_Korean, sizeof(tOSDFont_Korean));     
    #endif	
    	else
    	       mStar_LoadCompressedFont(GET_FONT_RAM_ADDR(FONT_DYNIC_START),    &tOSDFont_English, sizeof(tOSDFont_English));       
  #endif
}
#endif


void LoadCommonFont( void )
{
		BYTE i=0;
		if(i)
		{
			LoadCommonFontUncall();
		}
#if 0 
//Load 4 COLOR Font
Osd_LoadCompressColorFont( SmallLogo_4C, &tSmallLogo_4C, NULL, SmallLogo_Size);
#else
	Osd_Write4ColorFontStartAddr( 0x100+FONT_4COLOR_ICON_START );
	Osd_Write8ColorFontStartAddr( 0x100+FONT_8COLOR_ICON_START );
	msWrite2ByteMask(OSD1_0E,0x03FF,0x03FF);	// 16 color start
	
	Osd_LoadCompressMonoFont( MonoFontStart, tOSDFont_Base, sizeof(tOSDFont_Base));

#if EN_HotKeyXINGZHUN 
	//Osd_LoadCompressMonoFont(MonoFontXZStart, tXZ_JSIconFont, sizeof(tXZ_JSIconFont) );
#else
  #if (DisplayLogo==TERRA)
  	Osd_LoadCompressMonoFont(MonoFontSmallLogoStart, tOSDFont_SmallLogoTerra, sizeof(tOSDFont_SmallLogoTerra) );	
  #else  //planar
  	Osd_LoadCompressMonoFont(MonoFontSmallLogoStart, tOSDFont_SmallLogoPLANAR, sizeof(tOSDFont_SmallLogoPLANAR) );
  #endif
#endif
	
		 Osd_LoadNormalFontTable_LG2005_WordByLanguage();
	
		
		OSD_FONT_HI_ADDR_SET_BIT8();//0x100~0x1FF
		//OSD_WRITE_FONT_ADDRESS(FONT_4COLOR_ICON_START);
		//LoadCompressColorFont(&Main_4ColorIcon,  NULL, 18);
		//Load 4 COLOR Font
		//OSD_FONT_HI_ADDR_SET_BIT8();//0x100~0x1FF
		//Load 8 COLOR Font
		OSD_WRITE_FONT_ADDRESS(FONT_8COLOR_ICON_START);
		Osd_Load8ColorFont( FONT_8COLOR_ICON_START, &tMainMenuIcon8ColorFont, sizeof( tMainMenuIcon8ColorFont ) ); 
		
		 OSD_FONT_HI_ADDR_CLR_TO_0();
#endif	
    Osd_LoadColorPalette256(); // load osd color

}

void LoadCommonFontUncall(void)
{
    //OSDLoadFontData(0, 0);
   // Osd_Load291Font(0, 0, 0);
    #if DisplayLogo != TERRA
	MonoColorSetting();
	#endif
    Osd_LoadFont(0,0,0);
  
    Osd_Load8ColorFont(0,0,0);
   
   //Load 4 COLOR Font
   Osd_LoadCompressColorFont( 0, NULL, NULL, 0);
}














