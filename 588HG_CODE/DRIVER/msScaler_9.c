#define _MS_SCALER_C_
#include "global.h"

xdata SetupPathInfo g_SetupPathInfo;

#define MSSCALER_DEBUG    1
#if ENABLE_DEBUG && MSSCALER_DEBUG
#define MSSCALER_printData(str, value)   printData(str, value)
#define MSSCALER_printMsg(str)           printMsg(str)
#else
#define MSSCALER_printData(str, value)
#define MSSCALER_printMsg(str)
#endif

#if EN_V4TAP_SCALING
#define V_COEF_NUM (20)
#define V4TAP_COEF_SEL  3 //Fc80
BYTE code tVScalingCoeff[ 6 ][ V_COEF_NUM ] =
{
    { 0x02, 0x03, 0x04, 0x05, 0x06, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x14, 0x15, 0x16, 0x17, 0x18 }, /* Address */
    { 0x18, 0x11, 0x0B, 0x07, 0x04, 0x50, 0x4E, 0x4A, 0x44, 0x3C, 0x18, 0x21, 0x2A, 0x33, 0x3C, 0x00, 0x00, 0x01, 0x02, 0x04 }, /* Fc60 */ /* Coef */
    { 0x13, 0x0B, 0x05, 0x02, 0x00, 0x5A, 0x5A, 0x54, 0x4B, 0x40, 0x13, 0x1C, 0x28, 0x34, 0x40, 0x00, 0x81, 0x81, 0x81, 0x00 }, /* Fc70 */ /* Coef */
    { 0x0D, 0x05, 0x00, 0x82, 0x83, 0x66, 0x64, 0x5D, 0x51, 0x43, 0x0D, 0x18, 0x25, 0x34, 0x43, 0x00, 0x81, 0x82, 0x83, 0x83 }, /* Fc80 */ /* Coef */
    { 0x07, 0x81, 0x85, 0x86, 0x86, 0x72, 0x70, 0x67, 0x57, 0x46, 0x07, 0x12, 0x21, 0x33, 0x46, 0x00, 0x81, 0x83, 0x84, 0x86 }, /* Fc90 */ /* Coef */
    { 0x00, 0x87, 0x8A, 0x8A, 0x88, 0x80, 0x7C, 0x70, 0x5E, 0x48, 0x00, 0x0C, 0x1D, 0x31, 0x48, 0x00, 0x81, 0x83, 0x85, 0x88 }, /* Fc100 *//* Coef */
};
#else
#define V_COEF_NUM (5)
BYTE code tVScalingCoeff[ 3 ][ V_COEF_NUM ] =
{
    /*0x5A*/   { 0x00, 0x04, 0x08, 0x0C, 0x10 }, /* VDSUSG */
    /*0x57*/   { 0x00, 0x02, 0x0A, 0x27, 0x51 }, /* CB(0) */   /*VSPC*/    /*Scaling up */
    /*0x57*/   { 0x1F, 0x2B, 0x37, 0x4F, 0x67 }, /* BS(0.75) */ /*VSPC*/   /*Scaling down*/
};
#endif

BYTE code tHFilterTbl[] =
{
    0x00,   // no-scaling
    0x02,   // upscaling
    0x04    // downscaling
};

static void mStar_SetVScalingFilter( BYTE u8Filter )
{
    //v filter
    if(( u8Filter & 0x0F ) == 0 )
    {
        DB_WBMask( SC0_5A,  0, ( BIT1 | BIT0 ) ); /* Vertical scaling filter is Bilinear mode */
    }
    else
    {
        XDATA BYTE i;
#if EN_V4TAP_SCALING
        msWriteByteMask( SC0_5A,  2, (BIT1|BIT0) ); /* Vertical scaling filter is  4-tap scaling mode */
        for ( i = 0; i < V_COEF_NUM; i++ )
            DB_WB( (BIT15|0x0700|tVScalingCoeff[ 0 ][ i ]), tVScalingCoeff[ V4TAP_COEF_SEL ][ i ] );

        DB_WB( SC0_00 ,  0x00 ); // SC0
#else
        msWriteByteMask( SC0_5A,  1, ( BIT1 | BIT0 ) ); /* Vertical scaling filter is  Register define mode */
        for( i = 0; i < V_COEF_NUM; i++ )
        {
            DB_WBMask( SC0_5A, ( tVScalingCoeff[ 0 ][ i ] ), ( BIT4 | BIT3 | BIT2 ) ); /*Bit4~2*/
            DB_WB( SC0_57 ,  tVScalingCoeff[ u8Filter&0x0F ][ i ] );
        }
#endif
    }
}

static void mStar_SetHScalingFilter( BYTE u8Filter )
{
    DB_WB( SC0_56, tHFilterTbl[u8Filter] );
}

void mStar_SetScalingFilter( void )
{
    DB_Mode(eDB_NORMAL_MODE);
    // horizontal
    if( g_SetupPathInfo.wImgSizeInH == g_SetupPathInfo.wImgSizeOutH )
    {
        mStar_SetHScalingFilter( 0 );   // no-scaling
    }
    else if( g_SetupPathInfo.wImgSizeInH < g_SetupPathInfo.wImgSizeOutH )
    {
        mStar_SetHScalingFilter( 1 );   // scaling-up
    }
    // scaling-down
    else
    {
        mStar_SetHScalingFilter( 2 );   // scaling-down
    }

    // vertical
    if( g_SetupPathInfo.wImgSizeInV == g_SetupPathInfo.wImgSizeOutV )
    {
        mStar_SetVScalingFilter( 0 );   // no-scaling
    }
    else if( g_SetupPathInfo.wImgSizeInV < g_SetupPathInfo.wImgSizeOutV )
    {
        mStar_SetVScalingFilter( 1 );   // scaling-up
    }
    else
    {
        mStar_SetVScalingFilter( 2 );   // scaling-down
    }

#if ENABLE_SW_DOUBLE_BUFFER
    msSWDBWriteToRegister();
    msSWDBWaitForRdy();
#endif
}

void mStar_SetScalingFactor( void )
{
    XDATA DWORD factor;
    XDATA WORD height;
    XDATA BYTE ratioUp =0, ratioDown =0;
    XDATA BYTE bLimitDown19 = 0, bLimitDown2 = 0, bLimitDown29 = 0;
    XDATA BYTE bLimitUp5 = 0;
	
    DB_Mode(eDB_SUCCESSIVE_MODE);
    height = g_SetupPathInfo.wImgSizeInV;
	
    // Setup horizontal expansion
    if( g_SetupPathInfo.wImgSizeInH != g_SetupPathInfo.wImgSizeOutH ) // set expansion factor
    {
#if SC_H_SC_NON_MINUS1MODE
        factor = g_SetupPathInfo.wImgSizeInH;
        factor = (( DWORD )( factor << 20 ) ) / ( g_SetupPathInfo.wImgSizeOutH  ) ;

        if(g_SetupPathInfo.wImgSizeInH < g_SetupPathInfo.wImgSizeOutH)
        {
            DB_W3B( SC0_50, factor | 0xC00000 );//SC0_52[6]:H initial factor -1(L)
            DB_WBMask( SC0_58, BIT0, BIT0);//H initial factor -1(R)
            factor = ((DWORD)g_SetupPathInfo.wImgSizeOutH + (DWORD)g_SetupPathInfo.wImgSizeInH)*256 / 2 / g_SetupPathInfo.wImgSizeOutH;
            DB_WB(SC0_B3,factor);  // hfac initial value for L
            DB_WB(SC0_B4,factor);    // hfac initial value for R
        }
        else
        {
            DB_W3B( SC0_50, factor | 0x800000 );
            factor = ((DWORD)g_SetupPathInfo.wImgSizeInH - (DWORD)g_SetupPathInfo.wImgSizeOutH) *256 / 2 / g_SetupPathInfo.wImgSizeOutH;
            DB_WB(SC0_B3,factor);  // hfac initial value for L
            DB_WB(SC0_B4,factor);    // hfac initial value for R
        }
#else
        factor = g_SetupPathInfo.wImgSizeInH - HSC_offset; // factor=((input width-1)*2^20)/(output width-1) + 1
        factor = (( DWORD )( factor << 20 ) ) / ( g_SetupPathInfo.wImgSizeOutH - HSC_offset ) + 1;
        DB_W3B( SC0_50, factor | 0x800000 );
        DB_WB(SC0_B3,0x00);  // hfac initial value for L
        DB_WB(SC0_B4,0x00);    // hfac initial value for R

#endif
    }
    else
        DB_WB( SC0_52, 0 ); // input width=output width, then disable expansion

    // Setup vertical expansion
    if( height != g_SetupPathInfo.wImgSizeOutV )
    {
#if SC_V_SC_NON_MINUS1MODE
        factor = height ;
        factor = (( DWORD )( factor << 20 ) ) / ( g_SetupPathInfo.wImgSizeOutV );
        DB_W3B( SC0_53, factor |0x800000);

        if(height < g_SetupPathInfo.wImgSizeOutV)
        {
            DB_WBMask(SC0_C2,0x0F,0x0F);//V initial -1 [0]:L+odd,[1]:R+odd,[2]:L+even,[3]:R+even
            factor = ((DWORD)g_SetupPathInfo.wImgSizeOutV + (DWORD)height) *256 / 2 / g_SetupPathInfo.wImgSizeOutV;

        }
        else
        {
            DB_WBMask(SC0_C2,0x00,0x0F);
            factor = ((DWORD)height - (DWORD)g_SetupPathInfo.wImgSizeOutV) *256 / 2 / g_SetupPathInfo.wImgSizeOutV;
        }
        DB_WB(SC0_D8,factor);
        DB_WB(SC0_D9,factor);
        DB_WB(SC0_DA,factor);
        DB_WB(SC0_DB,factor);
#else
        DB_WB(SC0_D8,0x00);
        DB_WB(SC0_DA,0x80);
        factor = height - 1; // factor=((input height-1)*2^20)/(output height-1) + 1
        factor = (( DWORD )( factor << 20 ) ) / ( g_SetupPathInfo.wImgSizeOutV - 1 ) + 1;
        DB_W3B( SC0_53, factor|0x800000);
#endif
    }
    else
        DB_WB( SC0_55, 0 ); // input height=output height, then disable expansion

#if ENABLE_SW_DOUBLE_BUFFER
    msSWDBWriteToRegister();
    msSWDBWaitForRdy();
#endif
	#if Enable_Expansion
	IsSupportedAspRatioMode(OverScanSetting.FinalCapWinWidth,OverScanSetting.FinalCapWinHeight);
	#endif

}

static void msSetupPathParam()
{
    //setup output image size
#if Enable_Expansion
    if(OverScanSetting.Enable && (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL))
    {
        g_SetupPathInfo.wImgSizeOutH = OverScanSetting.OverScanH;
        g_SetupPathInfo.wImgSizeOutV = OverScanSetting.OverScanV;
    }
    else
    {
        g_SetupPathInfo.wImgSizeOutH = PANEL_WIDTH;
        g_SetupPathInfo.wImgSizeOutV = PANEL_HEIGHT;
    }
#else
    {
    g_SetupPathInfo.wImgSizeOutV = PANEL_HEIGHT;
    g_SetupPathInfo.wImgSizeOutH = PANEL_WIDTH;
    }
#endif
    MSSCALER_printData("StandardModeWidth(%d)", StandardModeWidth);
    MSSCALER_printData("StandardModeHeight(%d)", StandardModeHeight);

    g_SetupPathInfo.wImgSizeInH = msRead2Byte(SC0_0B) & SC_MASK_H;
    g_SetupPathInfo.wImgSizeInV = msRead2Byte(SC0_09) & SC_MASK_V;

    if(CURRENT_SOURCE_IS_INTERLACE_MODE())
        g_SetupPathInfo.wImgSizeInV /= 2;

    if( g_SetupPathInfo.wImgSizeInV < g_SetupPathInfo.wImgSizeOutV )
        g_SetupPathInfo.bScalingUpV = _ENABLE;
    else
        g_SetupPathInfo.bScalingUpV = _DISABLE;

    if( g_SetupPathInfo.wImgSizeInH < g_SetupPathInfo.wImgSizeOutH )
        g_SetupPathInfo.bScalingUpH = _ENABLE;
    else
        g_SetupPathInfo.bScalingUpH = _DISABLE;

    g_SetupPathInfo.ucSCPathMode = SC_PATH_MODE_0;
	
#if 0//Enable_Expansion
	IsSupportedAspRatioMode(OverScanSetting.FinalCapWinWidth,OverScanSetting.FinalCapWinHeight);
#endif
	
}

static void msSetupPathConfig()
{

#if AUTO_DATA_PATH
    msWriteByteMask(SCD_60, 0, BIT2);
    if(g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_0)
        msWriteByteMask(SCD_60, BIT0, BIT0);//Only H/V SP
    else
        msWriteByteMask(SCD_60, 0, BIT0);//HSP, VSP, MRW
#else
    switch(g_SetupPathInfo.ucSCPathMode)
    {
        case SC_PATH_MODE_0:
            if(g_SetupPathInfo.bScalingUpH)//VSP->HSP
            {
                msWrite2ByteMask(SCD_60, FIFO_VSP_HSP_FIFO,0xFFF0);
                msWriteByteMask(SCD_62, 0x00, BIT0|BIT1);
            }
            else //HSP->VSP
            {
                msWrite2ByteMask(SCD_60, FIFO_HSP_VSP_FIFO,0xFFF0);
                msWriteByteMask(SCD_62, 0x00, BIT0|BIT1);
            }
            break;
     }
#endif

    msWriteByteMask(SCD_62, _BIT4, _BIT4);
}

static void msSetupPathIP()
{
    XDATA BYTE ucDlyLine;

#if EN_V4TAP_SCALING
    if(CURRENT_SOURCE_IS_INTERLACE_MODE() )
        ucDlyLine = 4;
    else
        ucDlyLine = 3;
#else
    if(CURRENT_SOURCE_IS_INTERLACE_MODE() )
        ucDlyLine = 2;
    else
        ucDlyLine = 2;
#endif

    DB_Mode(eDB_NORMAL_MODE);

    if( !g_SetupPathInfo.bScalingUpV || !g_SetupPathInfo.bScalingUpH )
        DB_WBMask(SC0_39, 0, _BIT6);
    else
        DB_WBMask(SC0_39, _BIT6, _BIT6);
	
//20201210
	//DB_WBMask(SC0_39, _BIT0, _BIT0);

    //DB_WBMask(SC0_90, (ucDlyLine)<<4, _BIT6|_BIT5|_BIT4); //delay line
    DB_WBMask(SC0_0F, (SC_LOCK_LINE), 0x0F); //Ylock line

    DB_WB(SCD_68, 0x06);    // follow Junior setting

#if ENABLE_SW_DOUBLE_BUFFER
    msSWDBWriteToRegister();
#endif
}

static void msSetupPathHSP()
{
        msWrite2ByteMask(SC0_C8,g_SetupPathInfo.wImgSizeOutH,0x0FFF);
        MSSCALER_printData("wImgSizeOutH==>%d",g_SetupPathInfo.wImgSizeOutH);
}

static void msSetupPathVSP()
{
    msWrite2ByteMask(SC0_C0,g_SetupPathInfo.wImgSizeInV,0x0FFF);
    msWrite2ByteMask(SC0_C4,g_SetupPathInfo.wImgSizeOutV,0x0FFF);
    MSSCALER_printData("wImgSizeInV==>%d",g_SetupPathInfo.wImgSizeInV);
    MSSCALER_printData("wImgSizeOutV==>%d",g_SetupPathInfo.wImgSizeOutV);

#if EN_V4TAP_SCALING
    msWriteByteMask(SC0_5B, 0, BIT7|BIT6);
#else
    msWriteByteMask(SC0_5B, BIT7, BIT7|BIT6);//Line buffer 2 line mode
#endif

    msWriteByteMask(SC0_C1, BIT7, BIT7);//Invert source field.

    //V 0.5 line shifting mode
    if(CURRENT_SOURCE_IS_INTERLACE_MODE())
        msWriteByteMask(SC7_1B, 0, BIT5|BIT6);
    else
        msWriteByteMask(SC7_1B, BIT5|BIT6, BIT5|BIT6);//[6]:Force Feild [5]:Even/Odd Field Selection
}

//ENABLE_DEBUG
static void msSetupPathDbgDump()
{
#if DEBUG_PRINT_ENABLE
    MSSCALER_printData("wImgSizeInH = %d", g_SetupPathInfo.wImgSizeInH);
    MSSCALER_printData("wImgSizeInV = %d", g_SetupPathInfo.wImgSizeInV);
    MSSCALER_printData("wImgSizeOutH = %d", g_SetupPathInfo.wImgSizeOutH);
    MSSCALER_printData("wImgSizeOutV = %d", g_SetupPathInfo.wImgSizeOutV);
    MSSCALER_printData("ucSCFmtIn = %d", g_SetupPathInfo.ucSCFmtIn);
    MSSCALER_printData("bScalingUpH = %d", g_SetupPathInfo.bScalingUpH);
    MSSCALER_printData("bScalingUpV = %d", g_SetupPathInfo.bScalingUpV);
    MSSCALER_printData("ucSCPathMode = %d", g_SetupPathInfo.ucSCPathMode);
#endif
}

void mStar_SetupPath()
{
    XDATA BYTE u8Org=mStar_ScalerDoubleBuffer(FALSE);

    msSetupPathParam();
    msSetupPathConfig();
    msSetupPathIP();
    msSetupPathHSP();
    msSetupPathVSP();
    msSetupPathDbgDump();
    mStar_SetScalingFactor();
    mStar_SetScalingFilter();
    mStar_ScalerDoubleBuffer(u8Org&BIT0);
}

void mStar_SetupPathInit()
{
    g_SetupPathInfo.ucSCFmtIn = SC_FMT_IN_NORMAL;
    g_SetupPathInfo.wImgSizeOutH = PANEL_WIDTH;
    g_SetupPathInfo.wImgSizeOutV = PANEL_HEIGHT;
}


