


//OSD_WIN0 for PG
#define OSD_MAIN_WND        OSD_WIN1
#define OSD_BUTTON_WND      OSD_WIN2
#define OSD_CURSOR_WND      OSD_WIN3
#define OSD_CURSOR_WIN_BIT  OSD_GD_WIN3_EN_BIT
#define OSD_WIN_USED_MAX    OSD_WIN3

#define OSD_MAIN_WND_CA_BASE    0x0001
#define OSD_MAIN_WND_SIZE       1728//(20+20+24)*27
#define OSD_BUTTON_WND_CA_BASE  (OSD_MAIN_WND_CA_BASE+OSD_MAIN_WND_SIZE)
#define OSD_BUTTON_WND_SIZE     60//

#if CHIP_ID>=CHIP_TSUMV

extern BYTE xdata g_u8OsdWndNo;
extern BYTE xdata g_u8FontAddrHiBits;
//extern WORD xdata g_u16OsdWndCABase[];
extern BYTE xdata g_u8OsdFontDataHighByte;

#define OSD_SET_WND(u8Wnd)  (g_u8OsdWndNo=u8Wnd)


#define OSD_ENABLE_GRADIENT_COLOR()     //msWriteByteMask(OSD1_15, BIT3, BIT3)
#define OSD_DISABLE_GRADIENT_COLOR()    //msWriteByteMask(OSD1_15,    0, BIT3)

//Font RAM bit8/bit9
#define OSD_FONT_HI_ADDR_SET_BIT8()     (g_u8FontAddrHiBits=BIT0) //msWriteByteMask(OSD2_AE, BIT7, BIT7|BIT6); //enable bit 8
#define OSD_FONT_HI_ADDR_SET_BIT9()     (g_u8FontAddrHiBits=BIT1) //msWriteByteMask(OSD2_AE, BIT6, BIT7|BIT6); //enable bit 9
#define OSD_FONT_HI_ADDR_CLR_TO_0()     (g_u8FontAddrHiBits=0) //msWriteByteMask(OSD2_AE,    0, BIT7|BIT6);

#define OSD_FONT_START_ENTRY        0x1D4   //464(0x1D0) + 2160 entries= 2088 c/a + 720 fonts
#define OSD_FONT_UNIT_SIZE          3       //12x18 font, 1.5x18 = 27 Bytes = 3 x 72 bits = 3 entries
#define GET_FONT_RAM_ADDR(u8Addr)   ((OSD_FONT_START_ENTRY+((((WORD)g_u8FontAddrHiBits)<<8)+(u8Addr))*OSD_FONT_UNIT_SIZE)<<2)


typedef struct
{
    BYTE bButtonOsd:1;
    BYTE bSync2Main:1;
}WrapLayerFlagType;
#else
#define GET_FONT_RAM_ADDR(u8Addr)		(u8Addr)
#define OSD_FONT_HI_ADDR_SET_BIT8()     msWriteByteMask(OSD2_AE, BIT7, BIT7|BIT6); //enable bit 8
#define OSD_FONT_HI_ADDR_SET_BIT9()     msWriteByteMask(OSD2_AE, BIT6, BIT7|BIT6); //enable bit 9
#define OSD_FONT_HI_ADDR_CLR_TO_0()     msWriteByteMask(OSD2_AE,    0, BIT7|BIT6);
#endif

#define ENABLE_OSD_MASK0_VALUE (CHIP_ID>=CHIP_TSUMV)
#if ENABLE_OSD_MASK0_VALUE
#define _CTRL_OSD_CTRL_REGISTERS() (msWriteByte( 0x1C00, 0x0 ))
#define GET_OSD_CTRL_REG_WR_PORT()  ( msReadByte( 0x1C66 ) | ( (msReadByte( 0x1C67 ) & 0x3F) <<  8 ) )
#define SET_OSD_CTRL_REG_WR_PORT( V ) ((msWriteByte( 0x1C66, (V) )), (msWriteByteMask( 0x1C67, ((V) >>  8), 0x3F )))
#define GET_OSD_CTRL_REG_TRIG_MODE() ( msReadByte( 0x1C68 ) & 0x1F )
#define SET_OSD_CTRL_REG_TRIG_MODE( V ) (msWriteByteMask( 0x1C68, (V), 0x1F ))
#define GET_OSD_GD_REG_GD3_WIN7_CUR_SEL() ( ( msReadByte( 0x1C39 ) & 0xF0 ) >>  4 )
#define SET_OSD_GD_REG_GD3_WIN7_CUR_SEL( V ) (msWriteByteMask( 0x1C39, ((V) <<  4), 0xF0 ))
 
#define _TRIG_MODE_P0_ADDR      10
#define _TRIG_MODE_P0_MASK      11
#define _TRIG_MODE_P0_CKEY      12
#define _TRIG_MODE_P0_COLOR     13
#define _TRIG_MODE_P0_ERASE     14
#define _TRIG_MODE_P0_READ      15
#endif


extern xdata BYTE OsdWindowWidth;
extern xdata BYTE OsdWindowHeight;
//extern BYTE mStar_ReadOSDByte( BYTE address );
//extern void mStar_WriteOSDByte( BYTE address, BYTE value );
extern void Osd_DrawNum( BYTE xPos, BYTE yPos, int value);//WORD value );
extern void Osd_Draw4Num( BYTE xPos, BYTE yPos, int value);//WORD value );
extern void Osd_DrawHex( BYTE xPos, BYTE yPos, BYTE value );
extern void Osd_SetPosition( BYTE xPos, BYTE yPos );
extern void Osd_SetWindowSize( BYTE width, BYTE height );
extern void Osd_SetTextColor( BYTE foreColor, BYTE backColor );
//extern void Osd_DrawChar(BYTE xPos, BYTE yPos, BYTE ch);
extern void Osd_DrawGuage( BYTE ucX, BYTE ucY, BYTE ucLength, BYTE ucValue );
extern void Osd_Show( void );
extern void Osd_Hide( void );
extern void Osd_SetTransparency(BYTE u8Value);
extern void Osd_InitOSD( void );
extern void Osd_DynamicLoadFont( BYTE addr, BYTE *fontPtr, WORD num );
extern void Osd_DrawPropStr( BYTE xPos, BYTE yPos, BYTE *str );
extern void DisableOsdWindow( BYTE winIndex );
extern void MoveOsdWindow( BYTE winIndex, BYTE YPos, BYTE xPosStar, BYTE Xlength, BYTE Ylength, BYTE FgColor, BYTE BgColor );
extern void Osd_DrawCharDirect(BYTE u8XPos, BYTE u8YPos, WORD u8Char);
//extern void Osd_SetText4Color(color);
//extern void Osd_DynamicLoadFont(BYTE addr, BYTE *fontPtr, WORD num);
extern void Osd_DrawPropStr( BYTE xPos, BYTE yPos, BYTE *str );
extern void MonoColorSetting( void );
extern void Osd_SetTextMonoColor( BYTE foreColor, BYTE backColor );
extern void Osd_DrawContinuesChar(BYTE u8XPos, BYTE u8YPos, WORD u8Char, BYTE u8Num);
//extern void mStar_WriteOSDByteMask( BYTE ucReg, BYTE ucValue, BYTE ucMask );

extern BYTE Osd_DoubleBuffer(Bool u8Enable);
extern void Osd_SetWindowEnable(BYTE u8Win_Num,Bool u8Enable);
#if ENABLE_OSD_MASK0_VALUE
extern void drvOSDSetMask0(BYTE item, BYTE v);
#endif

//VS CCFLAG  ADD
extern void Osd_Set256TextColor(BYTE foreColor, BYTE backColor);
extern void DynamicLoad_4ColorFont(BYTE menuPageIndex);
extern void Osd_DrawRealStr(BYTE xPos, BYTE yPos, BYTE *str);
extern void DrawNum(BYTE xPos,BYTE yPos,char len,WORD value);	
extern void Osd_DrawWordHex(BYTE xPos, BYTE yPos, WORD value);
#if BlacklitTimeOnShow 
void Osd_Draw5Num(BYTE xPos, BYTE yPos, WORD value);
void Osd_Draw2Num(BYTE xPos, BYTE yPos, int value);
#endif

#if EN_HotKeyXINGZHUN
extern void DynamicLoad_2CrossFont(BYTE menuPageIndex);
#endif




