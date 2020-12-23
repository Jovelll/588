#include <stdlib.h>
#include "types.h"
#include "ms_reg.h"
//#include "panel.h"
#include "Board.h"
#include "debug.h"
#include "Common.h"
#include "ms_rwreg.h"
#include "mstar.h"
#include "misc.h"
#include "global.h"
#include "menudef.h"
#include "font.h"
//#include "extlib.h"
#include "LoadCommonFont.h"
//#include "msid_v1.h"
#include "drvosd.h"
#include "msosd.h"
#include "colorpalette.h"
#include "loadpropfont1218.h"

#if CHIP_ID>=CHIP_TSUMV
BYTE xdata g_u8OsdWndNo=OSD_MAIN_WND;
BYTE xdata g_u8FontAddrHiBits=0;
BYTE xdata g_u8OsdFontDataHighByte=0;
#endif
extern void LoadCompressColorFont(BYTE *pu8FontTbl, WORD *pu16MapPtr, BYTE countu8Count);
#if EN_HotKeyXINGZHUN

extern WORD code tXZ_JSIconFont[];
extern WORD code tXZ1IconFont[];
extern WORD code tXZ2IconFont[];
extern WORD code tXZ3IconFont[];



extern void Osd_LoadCompressMonoFont(BYTE u8Addr, WORD *pu16FontPtr, WORD u16Count);
#endif
#define DoubleXSize 0
#define DoubleYSize 0
//==================================================================================
// Local function declaration
//
//void mStar_WriteOSDByte( BYTE address, BYTE value );
BYTE mStar_ReadOSDByte( BYTE address );
void Osd_SetWindowSize( BYTE width, BYTE height );
void Osd_SetPosition( BYTE xPos, BYTE yPos );
//void Osd_DrawChar(BYTE xPos, BYTE yPos, BYTE ch);
void msOSDuncall(void);

extern void mStar_WaitForDataBlanking();
//==================================================================================
// variable declaration
//
xdata BYTE OsdWindowWidth = 10;
xdata BYTE OsdWindowHeight = 10;
xdata BYTE OsdFontColor = 0x01;
extern xdata BYTE MenuPageIndex;
extern xdata BYTE MenuItemIndex;
//PropFontType * PropFont_TablePointer;
//WORD * PropFont_MapTablePointer;

#if 0//CHIP_ID == CHIP_TSUMU
extern void ColorPaletteUncall(void);
#endif
//==================================================================================


void Osd_InitOSD( void )
{
    BYTE u8Retry=10;

    if(!u8Retry)
    {
        drvOSD_uncall();
        msOSDuncall();
    }

#if CHIP_ID>=CHIP_TSUMV

    msWriteByte(OSD1_00, 0x00); //Set to OSD1 BK0

    while (u8Retry--)
    {
        msWriteByteMask(OSD1_65, BIT1, BIT1);       //clear cafsram
        ForceDelay1ms(3);
        if (!(msReadByte(OSD1_65)&BIT1))
            break;
    }
      msWriteByteMask(OSD1_03, BIT2, BIT2);       //OSD wr priority is over display for both cafsram/psram

    msWrite2Byte(OSD1_0A, 0x03FF); // 4 color start
    msWrite2Byte(OSD1_0C, 0x03FF); // 8 color start
    msWrite2Byte(OSD1_0E, 0x03FF); // 16 color start

    msWrite2Byte(OSD1_04, PANEL_WIDTH);         //set osd h range size
    msWrite2Byte(OSD1_06, PANEL_HEIGHT);        //set osd v range size
    //msWriteByteMask(OSD1_02,BIT1,BIT1);         //force blending
    //Init cafsram
    msWrite2Byte(OSD1_08, OSD_FONT_START_ENTRY);               //set font data entry base,
    msWriteByteMask(OSD1_0B, (OSD_FONT_UNIT_SIZE-1)<<4, BIT4|BIT5);  //Set unit size to 3 entries, [5:4] value+1 entries

    drvOSD_SetWndCABaseAddr(OSD_WIN0,0x0000);
    drvOSD_SetWndShiftPixel(OSD_WIN0, 0, 0);    //avoid compiler warning
    drvOSD_SetWndFixColor(OSD_WIN0, 0, 0, 0);   //avoid compiler warning

    //init OSD_MAIN_WND
    drvOSD_SetWndCtrl(OSD_MAIN_WND, OSD_WND_CTRL0,
                      (OWC0_FP_MODE|
                       OWC0_GROUP_SEL0|
                       OWC0_LV1_ALPHA_SRC(OSD_A_FIX)|
                       OWC0_LV2_ALPHA_SRC(OSD_A_FIX)|
                       OWC0_OUT_ALPHA_SRC(OSD_A_FIX)),
                       OWC0MASK_ALL);
    drvOSD_SetWndCtrl(OSD_MAIN_WND, OSD_WND_CTRL1,
                     (OWC1_1BP_44MODE_EN|
                      OWC1_44MODE_TRS_EN|
                      OWC1_FONT_HEIGHT(18)),
                      OWC1MASK_ALL);
    drvOSD_SetWndCtrl(OSD_MAIN_WND, OSD_WND_CTRL2,
                     (OWC2_GD_COLOR_R(OSD_COLOR_FIX)|
                      OWC2_GD_COLOR_G(OSD_COLOR_FIX)|
                      OWC2_GD_COLOR_B(OSD_COLOR_FIX)),
                      OWC2MASK_ALL);
    drvOSD_SetWndCtrl(OSD_MAIN_WND, OSD_WND_CTRL3,
                     (OWC3_LV1_KM_SWITCH|OWC3MASK_CKEY_INV),
                      OWC3MASK_ALL);

    drvOSD_SetWndFixAlpha(OSD_MAIN_WND,OSD_ALPHA_LV2,0x00); //PG   //(1-a)*low_lvl+a*top_lvl
    drvOSD_SetWndFixAlpha(OSD_MAIN_WND,OSD_ALPHA_LV1,0x3F); //FG
    drvOSD_SetWndFixAlpha(OSD_MAIN_WND,OSD_ALPHA_OUT,0x3F); //OSD
    drvOSD_SetWndCABaseAddr(OSD_MAIN_WND,OSD_MAIN_WND_CA_BASE);

    //init OSD_BUTTON_WND
    drvOSD_SetWndCtrl(OSD_BUTTON_WND, OSD_WND_CTRL0,
                     (OWC0_FP_MODE|
                      OWC0_GROUP_SEL0|
                      OWC0_LV1_ALPHA_SRC(OSD_A_FIX)|
                      OWC0_LV2_ALPHA_SRC(OSD_A_FIX)|
                      OWC0_OUT_ALPHA_SRC(OSD_A_FIX)),
                      OWC0MASK_ALL);
    drvOSD_SetWndCtrl(OSD_BUTTON_WND, OSD_WND_CTRL1,
                     (OWC1_1BP_44MODE_EN|
                      OWC1_44MODE_TRS_EN|
                      OWC1_FONT_HEIGHT(18)),
                      OWC1MASK_ALL);
    drvOSD_SetWndCtrl(OSD_BUTTON_WND, OSD_WND_CTRL2,
                     (OWC2_GD_COLOR_R(OSD_COLOR_FIX)|
                      OWC2_GD_COLOR_G(OSD_COLOR_FIX)|
                      OWC2_GD_COLOR_B(OSD_COLOR_FIX)),
                      OWC2MASK_ALL);
    drvOSD_SetWndCtrl(OSD_BUTTON_WND, OSD_WND_CTRL3,
                     (OWC3_LV1_KM_SWITCH|OWC3MASK_CKEY_INV),
                      OWC3MASK_ALL);
    drvOSD_SetWndFixAlpha(OSD_BUTTON_WND,OSD_ALPHA_LV2,0x00); //PG   //(1-a)*low_lvl+a*top_lvl
    drvOSD_SetWndFixAlpha(OSD_BUTTON_WND,OSD_ALPHA_LV1,0x3F); //FG
    drvOSD_SetWndFixAlpha(OSD_BUTTON_WND,OSD_ALPHA_OUT,0x3F); //OSD
    drvOSD_SetWndCABaseAddr(OSD_BUTTON_WND,OSD_BUTTON_WND_CA_BASE);

    //init OSD_CURSOR_WND
    drvOSD_SetWndCtrl(OSD_CURSOR_WND, OSD_WND_CTRL0,
                     (OWC0_FP_MODE|
                      OWC0_GROUP_SEL0|
                      OWC0_LV1_ALPHA_SRC(OSD_A_FIX)|
                      OWC0_LV2_ALPHA_SRC(OSD_A_FIX)|
                      OWC0_OUT_ALPHA_SRC(OSD_A_FIX)),
                      OWC0MASK_ALL);
    drvOSD_SetWndCtrl(OSD_CURSOR_WND, OSD_WND_CTRL1,
                     (OWC1_1BP_44MODE_EN|
                      OWC1_44MODE_TRS_EN|
                      OWC1_FONT_HEIGHT(18)),
                      OWC1MASK_ALL);
    drvOSD_SetWndCtrl(OSD_CURSOR_WND, OSD_WND_CTRL2,
                     (OWC2_GD_COLOR_R(OSD_GD_EG3)|
                      OWC2_GD_COLOR_G(OSD_GD_EG4)|
                      OWC2_GD_COLOR_B(OSD_GD_EG5)),
                      OWC2MASK_ALL);
    drvOSD_SetWndCtrl(OSD_CURSOR_WND, OSD_WND_CTRL3,
                      (OWC3_1BPP_ATTR_BIT1|OWC3MASK_CKEY_INV),
                      OWC3MASK_ALL);

    drvOSD_SetWndFixAlpha(OSD_CURSOR_WND,OSD_ALPHA_LV2,0x00); //PG   //(1-a)*low_lvl+a*top_lvl
    drvOSD_SetWndFixAlpha(OSD_CURSOR_WND,OSD_ALPHA_LV1,0x00); //PG
    drvOSD_SetWndFixAlpha(OSD_CURSOR_WND,OSD_ALPHA_OUT,0x3F); //OSD

    drvOSD_SetGDEngineEnableWnd(OSD_GD_EG3,OSD_CURSOR_WIN_BIT,OSD_CURSOR_WIN_BIT);
    drvOSD_SetGDEngineEnableWnd(OSD_GD_EG4,OSD_CURSOR_WIN_BIT,OSD_CURSOR_WIN_BIT);
    drvOSD_SetGDEngineEnableWnd(OSD_GD_EG5,OSD_CURSOR_WIN_BIT,OSD_CURSOR_WIN_BIT);

    OSD_SET_WND(OSD_MAIN_WND);
#else
    {
        BYTE retry = 10;
            while( retry-- )
            {
                msWriteByte( OSD2_A0, BIT6 );
                Delay1ms( 3 );
                if( !( msReadByte( OSD2_A0 )&BIT6 ) )
                    break;
            }
    }
    //msWriteByte( BLENDC, 0 ); //111102 Rick modified cal pipedelay condition - B39842
    //msWriteByte( BLENDL, 0 );
    msWriteByte( OSD1_10, 0x80 );
    msWriteByte( OSDDBC, 0x5 ); // enable double
    msWriteByte( OCBUFO, 0x00 ); // disable OSD code buffer Base/Offset address
    msWriteByte( IOSDC3, 0x00 ); //0x0F); // shadow function control
    msWriteByte( OSDHC, 0x11 ); // shadow width control
    Osd_SetWindowSize( 20, 10 ); // set osd window
    msWriteByte( IOSDC2, 0x00 ); // color bit setting
    msWriteByte( OSD1_70, BIT4 | BIT2 ); // [4]: set osd move step by 1 pixel; [2] add 9 pixels
#endif
    Osd_SetPosition(50, 50); // set osd position
    LoadCommonFont();

	#if ENABLE_OSD_MASK0_VALUE // initial Mask value for transprent
	{
	BYTE i = 0;
	for (i=0;i<0xFF;i++)
        drvOSDSetMask0(i,0);
	}
	#endif

}


void MonoColorSetting( void )
{
    #if CHIP_ID>=CHIP_TSUMV
    Osd_LoadColorPalette(); // load osd color
    #else
    //msWriteByteMask( OCFF, 0xC0, ~BIT5 );//mStar_WriteOSDByteMask( OSD_10, 0xC0, ~BIT5 );
    msWriteByte( OSDDBC, 0x5 );//mStar_WriteOSDByte( OSDDBC, 0x5 ); // enable double
    msWriteByte( OCBUFO, 0x00 );//mStar_WriteOSDByte( OCBUFO, 0x00 ); // disable OSD code buffer Base
    msWriteByte( IOSDC3, 0x00 );// mStar_WriteOSDByte( IOSDC3, 0x00 ); //0x0F); // shadow function control
    msWriteByte( OSDHC, 0x11 );// mStar_WriteOSDByte( OSDHC, 0x11 ); // shadow width control
    msWriteByte( IOSDC2, 0x00 );//mStar_WriteOSDByte( IOSDC2, 0x00 ); // color bit setting
    Osd_LoadColorPalette();
    #endif
}

// setup osd window size
void Osd_SetWindowSize( BYTE width, BYTE height )
{
    OsdWindowWidth = width;
    OsdWindowHeight = height;

#if CHIP_ID>=CHIP_TSUMV
  drvOSD_SetWndLineOffset(OSD_MAIN_WND,OsdWindowWidth); //for OSD_MAIN_WND only
  #if ENABLE_OSD_ROTATION //reset CABase addr after line offset was set
    if (UserPrefOsdRotateMode==OSDRotateMenuItems_90)
	{
        drvOSD_SetWndCABaseAddr(OSD_MAIN_WND, GET_CABASE_0_90(OSD_MAIN_WND_CA_BASE, OsdWindowWidth));
	}
	else
	#if defined(_OSD_ROTATION_180_)
	if (UserPrefOsdRotateMode==OSDRotateMenuItems_180)
	{
        drvOSD_SetWndCABaseAddr(OSD_MAIN_WND, GET_CABASE_0_180(OSD_MAIN_WND_CA_BASE,OsdWindowWidth,OsdWindowHeight));
	}
	else
	#endif
	#if defined(_OSD_ROTATION_270_)
	if (UserPrefOsdRotateMode==OSDRotateMenuItems_270)
	{
        drvOSD_SetWndCABaseAddr(OSD_MAIN_WND, GET_CABASE_0_270(OSD_MAIN_WND_CA_BASE,OsdWindowWidth,OsdWindowHeight));
	}
    else
	#endif
        drvOSD_SetWndCABaseAddr(OSD_MAIN_WND, OSD_MAIN_WND_CA_BASE);
  #endif
#else
     // 110819 wait for coding


    msWriteByte(OSD1_06, OsdWindowWidth-1);
    msWriteByte(OSD1_07, OsdWindowHeight-1);
    //if(bReset)
    {
        //msWriteByte(OSD2_A0, 0xDE); // clear osd content
        //ForceDelay1ms(3);
    }
    msWriteByte(OSD2_A0, 0xBE);//0x9E);

#endif

}

// setup
#define MainMenuOSD_Height  20
#define OSD_TRANSPARENT_SHIFT     7
#if CHIP_ID>=CHIP_TSUMV
#define OSD_WINDOW(Addr)    ((OSD_MAIN_WND<<5)+Addr)
void Osd_SetPosition( BYTE xPos, BYTE yPos )
{
#if ENABLE_MENULOAD
    WORD HposValue, VposValue;

    HposValue = (( DWORD )xPos * ( PanelWidth - ( WORD )OsdWindowWidth * 12 ) ) / 100;

    yPos = 100 - yPos;
    VposValue = (( DWORD )yPos * ( PanelHeight - ( WORD )OsdWindowHeight * 18 ) ) / 100;
    if( yPos == 100 && OsdWindowHeight == MainMenuOSD_Height )
        VposValue += OSD_TRANSPARENT_SHIFT;

    msML_WaitReady();
    msML_WriteByte(ML_MODE_NORMAL ,OSD_WINDOW(OSD2_04), HposValue);
    msML_WriteByte(ML_MODE_NORMAL ,OSD_WINDOW(OSD2_05), HposValue>>8);
    msML_WriteByte(ML_MODE_NORMAL ,OSD_WINDOW(OSD2_08), (HposValue+( WORD )OsdWindowWidth * 12));
    msML_WriteByte(ML_MODE_NORMAL ,OSD_WINDOW(OSD2_09), (HposValue+( WORD )OsdWindowWidth * 12)>>8);
    msML_WriteByte(ML_MODE_NORMAL ,OSD_WINDOW(OSD2_06), VposValue);
    msML_WriteByte(ML_MODE_NORMAL ,OSD_WINDOW(OSD2_07), VposValue>>8);
    msML_WriteByte(ML_MODE_NORMAL ,OSD_WINDOW(OSD2_0A), (VposValue+( WORD )OsdWindowHeight * 18));
    msML_WriteByte(ML_MODE_NORMAL ,OSD_WINDOW(OSD2_0B), (VposValue+( WORD )OsdWindowHeight * 18)>>8);
    msML_Trigger(ML_TRIG_OUT_VDE_END);// Vde-end-OP

#else
    BYTE u8xPos, u8yPos;
    WORD posValue;
    WORD u16Hight;

#if CHIP_ID>=CHIP_TSUMV
    Sint xdata  xOffset = 0, yOffset = 0;
    WORD preMainHStart, preMainVStart;
#endif
#if ENABLE_OSD_ROTATION
    BYTE bInvertPosition = FALSE;
#endif

#if ENABLE_SW_DOUBLE_BUFFER
    msSWDBWriteToRegister();
    msSWDBWaitForRdy();
#endif
#if CHIP_ID>=CHIP_TSUMV
    preMainHStart = drvOSD_GetWndInfo ( OSD_MAIN_WND, OSD_WND_INFO_H_START );
    preMainVStart = drvOSD_GetWndInfo ( OSD_MAIN_WND, OSD_WND_INFO_V_START );
#endif
#if ENABLE_OSD_ROTATION
    if ( UserPrefOsdRotateMode==OSDRotateMenuItems_90 )
    {
        bInvertPosition = TRUE;
        u8xPos = 100 - yPos;
        u8yPos = 100 - xPos;        
    }
    else
	#ifdef _OSD_ROTATION_180_
    if ( UserPrefOsdRotateMode==OSDRotateMenuItems_180 )
    {
        bInvertPosition = FALSE;
        u8xPos = 100 - xPos;
        u8yPos = yPos;        
    }
    else
	#endif
	#ifdef _OSD_ROTATION_270_
    if ( UserPrefOsdRotateMode==OSDRotateMenuItems_270 )
    {
        bInvertPosition = TRUE;
        u8xPos = yPos;
        u8yPos = xPos;        
    }
    else
	#endif
#endif
	{
#if ENABLE_OSD_ROTATION    
        bInvertPosition = FALSE;
#endif
        u8xPos = xPos;
        u8yPos = 100 - yPos;
    }
#if (ENABLE_OSD_ROTATION)
    if ( bInvertPosition )
    {
        posValue = ( ( DWORD ) u8xPos * ( PANEL_WIDTH - ( WORD ) ((OsdWindowHeight) * 18) ) ) / 100;
    }
    else
#endif
    {
        posValue = ( ( DWORD ) u8xPos * ( PANEL_WIDTH - ( WORD ) ((OsdWindowWidth) * 12) ) ) / 100;
    }
    xOffset = posValue - preMainHStart;
    DB_W2B ( ( OSD_WIN1 << 5 ) + OSD2_04, posValue );
#if ENABLE_OSD_ROTATION
    if ( bInvertPosition )
    {
        DB_W2B ( ( OSD_WIN1 << 5 ) + OSD2_08, posValue + ( WORD ) ((OsdWindowHeight) * 18) );
    }
    else
#endif
    {
        DB_W2B ( ( OSD_WIN1 << 5 ) + OSD2_08, posValue + ( WORD ) ((OsdWindowWidth) * 12) );
    }

#if (ENABLE_OSD_ROTATION)
    u16Hight = PANEL_HEIGHT ;
    if ( bInvertPosition )
	{
        posValue = ( ( DWORD ) u8yPos * ( u16Hight - ( WORD ) (((OsdWindowWidth)) * 12) ) / 100 );
	}
    else
#endif
    {
        posValue = ( ( DWORD ) u8yPos * ( u16Hight - ( WORD ) (((OsdWindowHeight)) * 18) ) ) / 100;
    }
#if CHIP_ID>=CHIP_TSUMV
    yOffset = posValue - preMainVStart;
    DB_W2B ( ( OSD_WIN1 << 5 ) + OSD2_06, posValue );
#if ENABLE_OSD_ROTATION
    if ( bInvertPosition )
    {
        DB_W2B ( ( OSD_WIN1 << 5 ) + OSD2_0A, posValue + ( WORD ) ((OsdWindowWidth) * 12) );
    }
    else
#endif
    {
        DB_W2B ( ( OSD_WIN1 << 5 ) + OSD2_0A, posValue + ( WORD ) ((OsdWindowHeight )* 18) );
    }
#endif



#if ENABLE_SW_DOUBLE_BUFFER
    #if CHIP_ID==CHIP_TSUMF // TSUMF no VDE end trig source, instead by OSD_VDE_End sw polling & trigger
    msSWDBWriteToRegisterByOsdEnd();
    #else
    msSWDBWriteToRegister();
    #endif
    msSWDBWaitForRdy();
	#endif
#endif
}
#else
#define ScalingDown_PIPESHIFT    10
WORD drvOsdGetPipeDelay( void )
{
    WORD  xdata OPBackPorch;
    BYTE  xdata PIPESHIFT;
    PIPESHIFT =msReadByte( OSD1_4B );
    OPBackPorch=msRead2Byte(SC0_12);
    //if(g_SetupPathInfo.bScalingUpV)
#if CHIP_ID == CHIP_TSUMY       //111005 Rick modified OSD Position formula - A041
    OPBackPorch=(PIPESHIFT-OPBackPorch-23);//*2;
#else
        OPBackPorch=(PIPESHIFT-OPBackPorch-38)*2;
#endif
    //else
        //OPBackPorch=(PIPESHIFT-OPBackPorch-38+ScalingDown_PIPESHIFT)*2;
    return OPBackPorch;
}
void Osd_SetPosition( BYTE xPos, BYTE yPos )
{
    WORD  xdata posValue;

#if DoubleXSize
    posValue = (( DWORD )xPos * ( PanelWidth - ( WORD )OsdWindowWidth * 12 * 2 ) ) / 100 + ( ( PanelHStart/PANEL_H_DIV ) - 48 );
#else
  posValue = (( DWORD )xPos * ( PanelWidth - ( WORD )OsdWindowWidth * 12 ) ) / 100 - drvOsdGetPipeDelay(); //);
#endif

    msWriteByte( OSD1_02, posValue & 0xFF );
    msWriteByte( OSD1_03, posValue >> 8 );

    posValue = ( PanelWidth - ( WORD )OsdWindowWidth * 12 ) - 4;
    msWriteByte( OSD2_0A, posValue & 0xFF );
    msWriteByte( OSD2_0B, (msReadByte(OSD2_0B)&0x80) |(posValue >> 8) );

    yPos = 100 - yPos;

#if DoubleYSize
    posValue = (( DWORD )yPos * ( PanelHeight - ( WORD )OsdWindowHeight * 18 * 2 ) ) / 100;
#else
    posValue = (( DWORD )yPos * ( PanelHeight - ( WORD )OsdWindowHeight * 18 ) ) / 100;
#endif

    // 100811 coding addition for main menu osd
    if( yPos == 100 && OsdWindowHeight == MainMenuOSD_Height )
        posValue += OSD_TRANSPARENT_SHIFT;

    msWriteByte( OSD1_04, posValue & 0xFF );
    msWriteByte( OSD1_05, posValue >> 8 );

}
#endif
void Osd_SetTextMonoColor( BYTE foreColor, BYTE backColor )
{
    OsdFontColor = ( foreColor & 0xF ) << 4 | ( backColor & 0xF );
}
void Osd_SetTextColor( BYTE foreColor, BYTE backColor )
{
    // OsdFontColor=(foreColor&0xF)<<4|(backColor&0xF);
    if( backColor == Color_2 )
        OsdFontColor = foreColor << 1;
    else if( backColor == Color_4 )
        OsdFontColor = foreColor << 2;
    else if( backColor == Color_8 )
        OsdFontColor = foreColor << 3;
    else if( backColor == Color_2T )
        OsdFontColor = foreColor << 1 | 0x01;
    else if( backColor == Color_4T )
        OsdFontColor = foreColor << 2 | 0x01;
    else// (backColor==Color_8T)
        OsdFontColor = foreColor << 3 | 0x01;
}

//Character Direct Write DisplayCode
void Osd_DrawCharDirect(BYTE u8XPos, BYTE u8YPos, WORD u8Char)
{
#if CHIP_ID>=CHIP_TSUMV
    if(( u8Char & 0xFF00 ) == 0x100 )
    {
        OSD_TEXT_HI_ADDR_SET_BIT8();
    }
    else if(( u8Char & 0xFF00 ) == 0x200 )
    {
        OSD_TEXT_HI_ADDR_SET_BIT9();
    }
    else
    {
        OSD_TEXT_HI_ADDR_CLR_TO_0();
    }
    
    drvOSD_DrawRealChar(g_u8OsdWndNo, u8XPos, u8YPos, u8Char);
    
	OSD_TEXT_HI_ADDR_CLR_TO_0();
#else
    WORD u16TempValue;
    BYTE u8DrawCode = (!(u8YPos & BIT7));

    u8YPos &= 0x7F;

    u16TempValue=(WORD)u8YPos*OsdWindowWidth+u8XPos; // get real address
    msWrite2Byte(OSD2_A8, u16TempValue); // display font attribute
    msWriteByte(OSD2_AA, OsdFontColor);
    if (u8DrawCode)
    {
        msWrite2Byte(OSD2_A5, u16TempValue); // dispaly font code

        if(( u8Char & 0xFF00 ) == 0x100 )
        {
            OSD_TEXT_HI_ADDR_SET_BIT8();
        }
        else if(( u8Char & 0xFF00 ) == 0x200 )
        {
            OSD_TEXT_HI_ADDR_SET_BIT9();
        }
        else
        {
            OSD_TEXT_HI_ADDR_CLR_TO_0();
        }
        
        msWriteByte(OSD2_A7, u8Char);
        
 	    OSD_TEXT_HI_ADDR_CLR_TO_0();
   }
#endif


}
void Osd_DrawContinuesChar(BYTE u8XPos, BYTE u8YPos, WORD u8Char, BYTE u8Num)
{

#if CHIP_ID>=CHIP_TSUMV
    if(( u8Char & 0xFF00 ) == 0x100 )
    {
        OSD_TEXT_HI_ADDR_SET_BIT8();
    }
    else if(( u8Char & 0xFF00 ) == 0x200 )
    {
        OSD_TEXT_HI_ADDR_SET_BIT9();
    }
    else
    {
        OSD_TEXT_HI_ADDR_CLR_TO_0();
    }
    
    drvOSD_DrawContinuousChar(g_u8OsdWndNo,u8XPos,u8YPos,u8Char,u8Num);
    
	OSD_TEXT_HI_ADDR_CLR_TO_0();
#else
    WORD u16TempValue;
    BYTE i;
    BYTE    u8DrawCode = (!(u8YPos & BIT7));

    u8YPos &= 0x7F;

    u16TempValue=(WORD)u8YPos*OsdWindowWidth+u8XPos; // get real address
    msWrite2Byte(OSD2_A8, u16TempValue);
    for(i=0;i<u8Num;i++)
    {
        msWriteByte(OSD2_AA, OsdFontColor);
    }
    if (u8DrawCode)
    {
        msWrite2Byte(OSD2_A5, u16TempValue);
        
        if(( u8Char & 0xFF00 ) == 0x100 )
        {
            OSD_TEXT_HI_ADDR_SET_BIT8();
        }
        else if(( u8Char & 0xFF00 ) == 0x200 )
        {
            OSD_TEXT_HI_ADDR_SET_BIT9();
        }
        else
        {
            OSD_TEXT_HI_ADDR_CLR_TO_0();
        }
        
        for(i=0;i<u8Num;i++)
        {
            msWriteByte(OSD2_A7, u8Char);
        }
        
 	    OSD_TEXT_HI_ADDR_CLR_TO_0();
    }
#endif

}

void Osd_DrawPropStr(BYTE u8XPos, BYTE u8YPos, BYTE *pu8Str)
{
#if CHIP_ID>=CHIP_TSUMV

    WORD  xdata  u16TempValue;
    Bool  xdata  IsDrawCode = TRUE;

    if (pu8Str==NULL || (*(pu8Str + 1))==0) //Jison
        return;
    if (u8YPos & BIT7)
        IsDrawCode = FALSE;

    if( u8YPos == 2 || u8XPos == 0 )
    {
        u8XPos=( OsdWindowWidth - (*(pu8Str + 1)) ) / 2 + 1;
    }

    u8YPos &= 0x7F; //~(BIT7|BIT6);
#define DISP_CHAR   u8XPos
#define DISP_PTR    u8YPos

    u16TempValue=GET_TXT_CASTART(g_u8OsdWndNo,u8XPos,u8YPos); // get real address
    DISP_PTR = 0;
    WRITE_CAFSRAM_ADDR();
    msWrite2Byte(PORT_ATTR_ADDR, u16TempValue);
    WRITE_ATTRIBUTE();
    while (DISP_PTR++ < *(pu8Str + 1))
        msWriteByte(PORT_ATTR_DATA, OsdFontColor);

    if (IsDrawCode == TRUE)
    {
        DISP_PTR = 0;
     WRITE_CAFSRAM_ADDR();
        msWrite2Byte(PORT_CODE_ADDR, u16TempValue);
     WRITE_CODE();
        while (DISP_PTR < *(pu8Str + 1))
        {
            msWriteByte(PORT_CODE_DATA, (*pu8Str) + DISP_PTR);
            DISP_PTR++;
        }
    }

#undef DISP_CHAR
#undef DISP_PTR

#else
    WORD tempValue;
    BYTE drawCode = TRUE, drawCode1 = FALSE, tempdispPtr;
    if( u8YPos & BIT7 )
    {
        drawCode = FALSE;
    }
    if( u8YPos & BIT6 )
        //set center
    {
        drawCode1 = TRUE;
    }
    u8YPos &= 0x3F; //~(BIT7|BIT6);
#define dispChar    u8XPos
#define dispPtr     tempdispPtr
    if( u8YPos == 2 || u8XPos == 0 )
        // just for title
    {
        //20070403
        tempValue = ( WORD )u8YPos * OsdWindowWidth + ( OsdWindowWidth - *( pu8Str + 1 ) ) / 2 + 1; //tempValue = ( WORD ) yPos * OsdWindowWidth + OsdWindowWidth/2;
        drawCode1 = 1;
    }
    else
        tempValue = ( WORD )u8YPos * OsdWindowWidth + u8XPos;
    // get real address
    dispPtr = 0;
    msWrite2Byte( OSD2_A8, tempValue );
    while( dispPtr++ < *( pu8Str + 1 ) )
    {
        msWriteByte( OSD2_AA, OsdFontColor );
    }
    if( drawCode1 )
    {
        BYTE tlen;
        if( u8YPos == 2 || u8XPos == 0 )
        {
            //tempValue =tempValue;     //tempValue =tempValue -dispPtr/2+1 ;       //20070403
        }
        else
        {
            tlen = ( OsdWindowWidth - u8XPos - dispPtr ) / 2; // get center position
            tempValue += tlen;
        }
    }
    if( drawCode == TRUE )
    {
        dispPtr = 0;
        msWrite2Byte( OSD2_A5, tempValue );
        while( dispPtr < *( pu8Str + 1 ) )
        {
            msWriteByte( OSD2_A7, ( *pu8Str ) + dispPtr );
            dispPtr++;
            //printMsg("Draw Porational");
            #if DEBUG_OSD
            ForceDelay1ms(20);
            #endif

        }
    }
#undef dispChar
#undef dispPtr
#endif
}

#define Num2ASCII(Num) (Num+0x30)

void DrawNum(BYTE xPos, BYTE yPos, char len, WORD value)
{ 
	char _minus=0;

	if (value<0)
	{ 
		value=0-value;
		_minus=1;
	}
	if (value==0)
		Osd_DrawCharDirect(xPos+(len--), yPos, (BYTE)Num2ASCII(0));
	else
	{ 
		while (value && len) // translate integer to string
		{
			Osd_DrawCharDirect(xPos+(len--), yPos, (BYTE)Num2ASCII(value%10));
			value/=10;
		}
	}
	if (_minus && len>=0)// add sign
		Osd_DrawCharDirect(xPos+(len--), yPos, (BYTE)'-');
	_minus=(_minus)?(0):(1);
	for (; _minus<=len; _minus++)
		Osd_DrawCharDirect(xPos+_minus, yPos, (BYTE)' ');
}


void Osd_DrawNum( BYTE xPos, BYTE yPos, int value )
{
    DrawNum( xPos, yPos, 3, value );
}

void Osd_Draw4Num( BYTE xPos, BYTE yPos, int value )
{
    DrawNum( xPos, yPos, 4, value );
}
#if BlacklitTimeOnShow 
void Osd_Draw5Num(BYTE xPos, BYTE yPos, WORD value)
{ 
	DrawNum(xPos, yPos, 5, value);
}  
void Osd_Draw2Num(BYTE xPos, BYTE yPos, int value)
{ 	
	DrawNum(xPos, yPos, 2, value);
}
#endif

// Draw Hex for Debug
BYTE Hex2ASCII(BYTE Num)
{
    return ( Num < 10 ) ? ( Num + 0x30 ) : ( Num - 10 + 0x41 );
}
void DrawHex(BYTE xPos, BYTE yPos, BYTE value)
{
    char len=1;
    while( value && len >= 0 )
    {
        Osd_DrawCharDirect( xPos + ( len-- ), yPos, ( BYTE )Hex2ASCII( value % 0x10 ) );
        value /= 0x10;
    }
    if( len > 0 )
    {
        for( value = 0; value <= len; value++ )
        {
            Osd_DrawCharDirect( xPos + value, yPos, ( BYTE )0x30 );
        }
    }
}
void Osd_DrawHex(BYTE xPos, BYTE yPos, BYTE value)
{
    DrawHex(xPos, yPos, value);
}

void DrawWordHex(BYTE xPos, BYTE yPos, WORD value)
{
    char len=3;
    while( value>=0 && len >= 0 )
    {
        Osd_DrawCharDirect( xPos + ( len-- ), yPos, ( BYTE )Hex2ASCII( value % 0x10 ) );
        value /= 0x10;
    }
    if( len > 0 )
    {
        for( value = 0; value <= len; value++ )
        {
            Osd_DrawCharDirect( xPos + value, yPos, ( BYTE )0x30 );
        }
    }
}
void Osd_DrawWordHex(BYTE xPos, BYTE yPos, WORD value)
{
    DrawWordHex(xPos, yPos, value);
}

//===================================================================================
void Osd_Show( void )
{
#if CHIP_ID>=CHIP_TSUMV
    mStar_WaitForDataBlanking();
    drvOSD_SetWndOn(OSD_MAIN_WND, TRUE);
#else
    BYTE u8RegByte;
    u8RegByte=(DoubleXSize<<4)|(DoubleYSize<<6)|MWIN_B;//|0x8
    msWriteByte(OSD1_0C, u8RegByte);
#endif

}


void Osd_Hide( void )
{
#if CHIP_ID>=CHIP_TSUMV

    drvOSD_SetWndOn(OSD_MAIN_WND, FALSE);
#else
    #if DEBUG_OSD
    msWriteByte( IOSDC1, ( DoubleXSize << 4 ) | ( DoubleYSize << 6 ) | MWIN_B );
    #else
    msWriteByte( IOSDC1, 0x00 );
    #endif
#endif
}
void Osd_SetTransparency(BYTE u8Value)
{
    u8Value=0x3F-(u8Value*8);
    drvOSD_SetWndFixAlpha(OSD_MAIN_WND, OSD_ALPHA_OUT, u8Value);
    drvOSD_SetWndFixAlpha(OSD_BUTTON_WND, OSD_ALPHA_OUT, u8Value);
    drvOSD_SetWndFixAlpha(OSD_CURSOR_WND, OSD_ALPHA_OUT, u8Value);
}
// ============================
void Osd_DynamicLoadFont( BYTE addr, BYTE *fontPtr, WORD num )
{

    #if PropFontUseCommonArea
    #if 0  //disable by smc.lmw 20160628
    if ( UserPrefLanguage == LANG_TChina )
    {
        SecondTblAddr = 0x80;
        LanguageIndex = 1;
    }
    else if ( UserPrefLanguage == LANG_Korea )
    {
        SecondTblAddr = 0x80;
        LanguageIndex = 2;
    }
    
    else
    #endif
    {
        SecondTblAddr = 0xFF;
        LanguageIndex = 0;
    }
    #else
    pstPropFontSet1218=tPropFontSet;
    #endif
    g_u8PropFontFlags=SPACE1PIXEL;

    LoadPropFonts1218(addr, fontPtr, num, NULL, 0, 0, 0);

}

BYTE Osd_DoubleBuffer(Bool u8Enable)
{
  #if CHIP_ID>=CHIP_TSUMV
    u8Enable=0;
    return u8Enable;
  #else
    BYTE u8Org=msReadByte(OSD1_01);
    msWriteByte(OSD1_01,(u8Enable?0x05:0));
    return (u8Org);
  #endif
}

void msOSDuncall(void)
{
#if !ENABLE_MENULOAD
       drvOSD_SetWndHPosition(0, 0, 0);
       drvOSD_SetWndVPosition(0, 0, 0);
#endif
}

#if ENABLE_OSD_MASK0_VALUE
void drvOSDSetMask0(BYTE item, BYTE v)
{
    _CTRL_OSD_CTRL_REGISTERS();
    SET_OSD_CTRL_REG_TRIG_MODE(_TRIG_MODE_P0_ADDR);
    SET_OSD_CTRL_REG_WR_PORT(item);
    SET_OSD_CTRL_REG_TRIG_MODE(_TRIG_MODE_P0_MASK);
    SET_OSD_CTRL_REG_WR_PORT(v);
}

#endif


//VS CCFLAG  ADD
void Osd_Set256TextColor(BYTE foreColor, BYTE backColor)
{
   if(backColor==Color_2)
      OsdFontColor=foreColor;
   else if(backColor==Color_2G)
      OsdFontColor=(foreColor&0xF0)|0x01;
   else if(backColor==Color_4)
      OsdFontColor=foreColor<<2;
   else if(backColor==Color_8)
      OsdFontColor=foreColor<<3;
   else if(backColor==Color_8G)
      OsdFontColor=(foreColor<<3)|BIT1;
   else if(backColor==Color_2T)
      OsdFontColor=foreColor|0x00;
   else if(backColor==Color_4T)
      OsdFontColor=foreColor<<2|0x01;
   else// (backColor==Color_8T)
      OsdFontColor=foreColor<<3|0x01;
}

#if EN_HotKeyXINGZHUN

void DynamicLoad_2CrossFont(BYTE menuPageIndex)
{

#if 1//EN_HotKeyXINGZHUN

	if(menuPageIndex==HotXINGZHUN)
	 {
		Osd_LoadCompressMonoFont(MonoFontXZStart, tXZ_JSIconFont, sizeof(tXZ_JSIconFont));
	 }
    else if(menuPageIndex==HotXINGZHUN1||menuPageIndex==HotXINGZHUN4)
	 {
		Osd_LoadCompressMonoFont(MonoFontXZStart, tXZ1IconFont, sizeof(tXZ1IconFont));
	}
	else if(menuPageIndex==HotXINGZHUN2||menuPageIndex==HotXINGZHUN5)
	   {
		  Osd_LoadCompressMonoFont(MonoFontXZStart, tXZ2IconFont, sizeof(tXZ2IconFont));
	  }
	else if(menuPageIndex==HotXINGZHUN3||menuPageIndex==HotXINGZHUN6)
	   {
		  Osd_LoadCompressMonoFont(MonoFontXZStart, tXZ3IconFont, sizeof(tXZ3IconFont));
	  }


 #endif
 

}
#endif


void DynamicLoad_4ColorFont(BYTE menuPageIndex)
{   
	OSD_FONT_HI_ADDR_SET_BIT8();

	//Osd_Write4ColorFontStartAddr( 0x100+FONT_4COLOR_ICON_START );

      	if (menuPageIndex==MainMenu)
      	{
      	printData("xxxxxxxxxxx\n", FONT_4COLOR_ICON_START);
		
		OSD_WRITE_FONT_ADDRESS(FONT_4COLOR_ICON_START);
		 LoadCompressColorFont(&Main_4ColorIcon, NULL, 18);
      	}
        
         #if EN_HotKeyDCRMenuWithIcon
         else if(menuPageIndex==HotKeyDCROnMenu)
         {
         OSD_WRITE_FONT_ADDRESS(FONT_4COLOR_ICON_START);
         LoadCompressColorFont(&tDCRONIconFont, NULL, 12);
         }
         else if(menuPageIndex==HotKeyDCROffMenu)
         {
         OSD_WRITE_FONT_ADDRESS(FONT_4COLOR_ICON_START);
          LoadCompressColorFont(&tDCROFFIconFont, NULL, 12);
         }
         #endif
#if EN_HotKeyECOMenuWithIcon
	else if(menuPageIndex==HotKeyECOMoveMenu)
	{
		OSD_WRITE_FONT_ADDRESS(FONT_4COLOR_ICON_START);
              	LoadCompressColorFont(&tECOMoveIconFont, NULL, 12);
	}
	else if(menuPageIndex==HotKeyECOWebMenu)
	{
	       	OSD_WRITE_FONT_ADDRESS(FONT_4COLOR_ICON_START);
              	LoadCompressColorFont(&tECOWebIconFont, NULL, 12);
	}
	else if(menuPageIndex==HotKeyECOGameMenu)
	{
	        OSD_WRITE_FONT_ADDRESS(FONT_4COLOR_ICON_START);
               	LoadCompressColorFont( &tECOGameIconFont, NULL, 12);
	}
	else if(menuPageIndex==HotKeyECOStandardMenu)
	{
	        OSD_WRITE_FONT_ADDRESS(FONT_4COLOR_ICON_START);
                LoadCompressColorFont( &tDCRStandardIconFont, NULL, 12);
	}
 #endif
#if 0//EN_HotKeyXINGZHUN
 else if(menuPageIndex==HotXINGZHUN)
 {
	 OSD_WRITE_FONT_ADDRESS(FONT_XZ4COLOR_ICON_START);
	 LoadCompressColorFont(&tXZ_JSIconFont, NULL, 31);
 }
 else if(menuPageIndex==HotXINGZHUN1||menuPageIndex==HotXINGZHUN4)
  {
	  OSD_WRITE_FONT_ADDRESS(FONT_XZ4COLOR_ICON_START);
	  LoadCompressColorFont(&tXZ1IconFont, NULL, 36);
  }
 else if(menuPageIndex==HotXINGZHUN2||menuPageIndex==HotXINGZHUN5)
  {
	  OSD_WRITE_FONT_ADDRESS(FONT_XZ4COLOR_ICON_START);
	  LoadCompressColorFont(&tXZ2IconFont, NULL, 36);
  }
 else if(menuPageIndex==HotXINGZHUN3||menuPageIndex==HotXINGZHUN6)
   {
	   OSD_WRITE_FONT_ADDRESS(FONT_XZ4COLOR_ICON_START);
	   LoadCompressColorFont(&tXZ3IconFont, NULL, 36);
   }



 #endif
	  	OSD_FONT_HI_ADDR_CLR_TO_0();
}

#define GuageLevel		4

#define GuageFontStart	0x01
#define GuageFont0_4 	0x03
#define GuageFont1_4	0x04
#define GuageFont2_4       0x05
#define GuageFont3_4	0x06
#define GuageFont4_4       0x07
#define GuageFontEnd   	0x02

void Osd_DrawGuage(BYTE xPos, BYTE yPos, BYTE length, BYTE value)
{

	BYTE pLead;
	BYTE ucLoop;

	if (value>100)
		value=100;

    
 	Osd_DrawCharDirect(xPos, yPos, GuageFontStart);
	Osd_DrawCharDirect(xPos+length-1, yPos, GuageFontEnd);

	length -= 2;

	// bar length should small than 42 ( ucLength < 43 )
	pLead = ( (WORD) value * length * GuageLevel ) / 100;   // resver 0.1 // fill bar ucLength 10 times
	if( value != 0 && pLead == 0 )
		pLead++;

	for(ucLoop=1; ucLoop<=(pLead/GuageLevel); ucLoop++)
	{
		Osd_DrawCharDirect(xPos+ucLoop, yPos, GuageFont4_4);
	}

	if(pLead < length*GuageLevel)
	{
		Osd_DrawCharDirect(xPos+ucLoop, yPos, GuageFont0_4+(pLead%GuageLevel));
	}

	for(ucLoop=(pLead/GuageLevel+2); ucLoop<=length; ucLoop++)
	{
	    Osd_DrawCharDirect(xPos + ucLoop, yPos, GuageFont0_4);
    }
	  
	
}
void Osd_DrawRealStr(BYTE xPos, BYTE yPos, BYTE *str)
{
#if CHIP_ID>=CHIP_TSUMV
	WORD u16TempValue;
	BYTE u8DrawCode = (!(yPos & BIT7));

	yPos &= 0x7F;

	//u8Wnd<<=5;
	//u16TempValue=msRead2Byte((u8Wnd)+OSD2_10)+(WORD)u8YPos*msReadByte((u8Wnd)+OSD2_12)+u8XPos; // get real address
	u16TempValue=GET_TXT_CASTART(g_u8OsdWndNo,xPos,yPos); // get real address
	
#define dispChar    xPos
#define dispPtr     yPos
	dispPtr = 0;
        WRITE_CAFSRAM_ADDR();
	msWrite2Byte(PORT_ATTR_ADDR, u16TempValue); // display font attribute
        WRITE_ATTRIBUTE();
	while( *( str + dispPtr ) ) 	   // display string font index
	{
		msWriteByte(PORT_ATTR_DATA, OsdFontColor);
		dispPtr++;
	}
	if( u8DrawCode )
	{
		dispPtr = 0;
              WRITE_CAFSRAM_ADDR();
		msWrite2Byte(PORT_CODE_ADDR, u16TempValue); // dispaly font code
              WRITE_CODE();
		while( dispChar = *( str + dispPtr ) )			  // display string font index
		{
			msWriteByte(PORT_CODE_DATA, dispChar);
			dispPtr++;
		}
	}
#undef dispChar
#undef dispPtr
#else
#if 1 // 110819 wait for coding
	WORD tempValue;
	BYTE drawCode = TRUE;
	BYTE y;
	if( yPos & BIT7 )
	{
		drawCode = FALSE;
	}
	yPos &= 0x3F; //~(BIT7|BIT6);
	y = yPos;
#define dispChar    xPos
#define dispPtr     yPos
	tempValue = ( WORD )yPos * OsdWindowWidth + xPos; // get real address
	dispPtr = 0;
	msWrite2Byte( OSD2_A8, tempValue );
	while( *( str + dispPtr ) ) 	   // display string font index
	{
		msWriteByte( OSD2_AA, OsdFontColor );
		dispPtr++;
	}
	if( drawCode == TRUE )
	{
		dispPtr = 0;
		msWrite2Byte( OSD2_A5, tempValue );
		while( dispChar = *( str + dispPtr ) )			  // display string font index
		{
			msWriteByte( OSD2_A7, dispChar );
			dispPtr++;
        #if DEBUG_OSD
			ForceDelay1ms(20);
        #endif			
		}
	}
#undef dispChar
#undef dispPtr
#endif
#endif

}


