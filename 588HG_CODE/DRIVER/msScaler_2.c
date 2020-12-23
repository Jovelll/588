

#define _MS_SCALER_C_
#include "global.h"

#if  CHIP_ID==CHIP_TSUM2
xdata SetupPathInfo g_SetupPathInfo;
#if ENABLE_DISPLAY_UNDERSCAN
xdata UnderScanInfo UnderScanSetting;
#endif

#define MSSCALER_DEBUG    0
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
        DB_WB( REG_1E3F, 0x04 );
    }
    else if( g_SetupPathInfo.wImgSizeInH < g_SetupPathInfo.wImgSizeOutH )
    {
        mStar_SetHScalingFilter( 1 );   // scaling-up
        DB_WB( REG_1E3F, 0x04 );
    }
    // scaling-down
    else
    {
        mStar_SetHScalingFilter( 2 );   // scaling-down
        DB_WB( REG_1E3F, 0x00 );
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
        
        DB_WBit(SC0_B5,g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_CB,BIT6);
        if(g_SetupPathInfo.wImgSizeInH < g_SetupPathInfo.wImgSizeOutH)
        {
            DB_W3B( SC0_50, factor | 0xC00000 );
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
        DB_WBit(SC0_B5,g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_CB,BIT6);
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
            DB_WBMask(SC0_5B,BIT5,BIT5);
            factor = ((DWORD)g_SetupPathInfo.wImgSizeOutV + (DWORD)height) *256 / 2 / g_SetupPathInfo.wImgSizeOutV;
            
        }
        else
        {
            if(g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_TB)
                DB_WBMask(SC0_5B,BIT5,BIT5);
            else
                DB_WBMask(SC0_5B,0,BIT5);

                factor = ((DWORD)height - (DWORD)g_SetupPathInfo.wImgSizeOutV) *256 / 2 / g_SetupPathInfo.wImgSizeOutV;    
        }
        
        DB_WBit(SC0_5B,IS_VSP_FMT_2IMG(),BIT4);
    
            DB_WB(SC0_D8,factor);
            DB_WB(SC0_D9,factor); 
        DB_WB(SC0_DA,factor);
        DB_WB(SC0_DB,factor);             
#else
        DB_WBit(SC0_5B,IS_VSP_FMT_2IMG(),BIT4);
        if(g_SetupPathInfo.ucSCFmtIn != SC_FMT_IN_NORMAL)
        {
            if( (g_SetupPathInfo.bPsvPnlOutLRInvert)
             && !(g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_1)
             && !(g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_2) )
            {
                DB_WB(SC0_D8,0x80); 
                DB_WB(SC0_D9,0x00); 
                DB_WB(SC0_DA,0xFF); 
                DB_WB(SC0_DB,0x80); 
            }
            else
            {
                DB_WB(SC0_D8,0x00); 
                DB_WB(SC0_D9,0x80); 
                DB_WB(SC0_DA,0x80); 
                DB_WB(SC0_DB,0xFF); 
            }
            
        }
        else
        {   // for 2D shift line mode enable
            DB_WB(SC0_D8,0x00); 
            DB_WB(SC0_DA,0x80); 
        }
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
    BYTE ucMRWOut3DFmt;
    //setup output image size
    if(IS_FMT_OUT_PSV() && (g_SetupPathInfo.ucSCFmtIn != SC_FMT_IN_NORMAL) )
        g_SetupPathInfo.wImgSizeOutV = PANEL_HEIGHT / 2;
    else
        g_SetupPathInfo.wImgSizeOutV = PANEL_HEIGHT;
        
    g_SetupPathInfo.wImgSizeOutH = PANEL_WIDTH;

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
        if( IS_FMT_OUT_PSV() && (g_SetupPathInfo.ucSCFmtIn != SC_FMT_IN_NORMAL) )
            g_SetupPathInfo.wImgSizeOutV /= 2;
    }
#else
    g_SetupPathInfo.wImgSizeOutH = PANEL_WIDTH;
    g_SetupPathInfo.wImgSizeOutV = PANEL_HEIGHT;
#endif

    if(g_SetupPathInfo.b2DTo3DEnable)
        g_SetupPathInfo.wImgSizeOutV /= 2;

    MSSCALER_printData("g_SetupPathInfo.ucSCFmtIn(%d)", g_SetupPathInfo.ucSCFmtIn);
    MSSCALER_printData("StandardModeWidth(%d)", StandardModeWidth);
    MSSCALER_printData("StandardModeHeight(%d)", StandardModeHeight);

#if ENABLE_DISPLAY_UNDERSCAN
    if(UnderScanSetting.Enable)
    {
        g_SetupPathInfo.wImgSizeOutH -= (UnderScanSetting.HSize*2);
        g_SetupPathInfo.wImgSizeOutV -= (UnderScanSetting.VSize*2);
    }
#endif

#if 0//Enable_Expansion
	IsSupportedAspRatioMode(OverScanSetting.FinalCapWinWidth,OverScanSetting.FinalCapWinHeight);
#endif

    //setup input image size
    switch(g_SetupPathInfo.ucSCFmtIn)
    {
        case SC_FMT_IN_NORMAL:
            g_SetupPathInfo.wImgSizeInH = msRead2Byte(SC0_0B) & SC_MASK_H;
            g_SetupPathInfo.wImgSizeInV = msRead2Byte(SC0_09) & SC_MASK_V;
            if(CURRENT_SOURCE_IS_INTERLACE_MODE())
                g_SetupPathInfo.wImgSizeInV /= 2;
            break;
        case SC_FMT_IN_3D_PF:
            g_SetupPathInfo.wImgSizeInH = StandardModeWidth;
            g_SetupPathInfo.wImgSizeInV = StandardModeHeight;
            break;
        case SC_FMT_IN_3D_FP:
        case SC_FMT_IN_3D_FPI:
        case SC_FMT_IN_3D_FA:
        case SC_FMT_IN_3D_TB:
        case SC_FMT_IN_3D_LA:
            g_SetupPathInfo.wImgSizeInH = StandardModeWidth;
            g_SetupPathInfo.wImgSizeInV = StandardModeHeight / 2;
            if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FPI )
                g_SetupPathInfo.wImgSizeInV /= 2;
#ifdef FPGA
            if(g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FP)
            {
                g_SetupPathInfo.wImgSizeInH = 800;
                g_SetupPathInfo.wImgSizeInV = (600-30) / 2;
            }
#endif
           
            break;
        case SC_FMT_IN_3D_SBSH:
        case SC_FMT_IN_3D_SBSF:
        case SC_FMT_IN_3D_CB:
            g_SetupPathInfo.wImgSizeInH = StandardModeWidth / 2;
            g_SetupPathInfo.wImgSizeInV = StandardModeHeight;
            break;
        default:
            MSSCALER_printData("Unknown SC FMT IN line(%d)", __LINE__);
            break;
    }
    
    if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_TB)
            || (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_PF)
            || (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_LA)
            ||  IS_FMT_IN_SBS() )
    {
        if(CURRENT_SOURCE_IS_INTERLACE_MODE())
            g_SetupPathInfo.wImgSizeInV /= 2;
    }
    
    if( g_SetupPathInfo.wImgSizeInV < g_SetupPathInfo.wImgSizeOutV )
        g_SetupPathInfo.bScalingUpV = _ENABLE;
    else
        g_SetupPathInfo.bScalingUpV = _DISABLE;
        
    if( g_SetupPathInfo.wImgSizeInH <= g_SetupPathInfo.wImgSizeOutH )
        g_SetupPathInfo.bScalingUpH = _ENABLE;
    else
        g_SetupPathInfo.bScalingUpH = _DISABLE;
#ifdef FPGA
    g_SetupPathInfo.ucSCPathMode = SC_PATH_MODE_0;
#else
    if ((FRAME_BFF_SEL == FRAME_BFFLESS || (!IS_FMT_OUT_PSV()) 
        || (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL))
#if ENABLE_DISPLAY_UNDERSCAN
        && (!UnderScanSetting.Enable)
#endif
        )
        g_SetupPathInfo.ucSCPathMode = SC_PATH_MODE_0; //FBL mode
    else if(g_SetupPathInfo.bScalingUpH && g_SetupPathInfo.bScalingUpV )
        g_SetupPathInfo.ucSCPathMode = SC_PATH_MODE_1;
    else if (g_SetupPathInfo.bScalingUpH && !g_SetupPathInfo.bScalingUpV )
        g_SetupPathInfo.ucSCPathMode = SC_PATH_MODE_3;
    else if (!g_SetupPathInfo.bScalingUpH && g_SetupPathInfo.bScalingUpV )
        g_SetupPathInfo.ucSCPathMode = SC_PATH_MODE_2;
    else if (!g_SetupPathInfo.bScalingUpH && !g_SetupPathInfo.bScalingUpV )
        g_SetupPathInfo.ucSCPathMode = SC_PATH_MODE_4;
    
#endif
    g_SetupPathInfo.bFBMode = !(g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_0);
    g_SetupPathInfo.bLoadOdTbl = (g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_0);
    
    ucMRWOut3DFmt = (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL)? 1:2;
    switch(g_SetupPathInfo.ucSCPathMode)
    {
        case SC_PATH_MODE_0:
            g_SetupPathInfo.wMRWSizeInH = 0;
            g_SetupPathInfo.wMRWSizeInV = 0;
            g_SetupPathInfo.wMRWSizeOutH = 0;
            g_SetupPathInfo.wMRWSizeOutV = 0;
            break;
        case SC_PATH_MODE_1:
            g_SetupPathInfo.wMRWSizeInH = g_SetupPathInfo.wImgSizeInH;
            g_SetupPathInfo.wMRWSizeInV = g_SetupPathInfo.wImgSizeInV;
            if((g_SetupPathInfo.ucSCFmtIn >= SC_FMT_IN_3D_TB &&  g_SetupPathInfo.ucSCFmtIn <= SC_FMT_IN_3D_SBSF))
            {
                g_SetupPathInfo.wMRWSizeInV *=2;                
            }
            else if(g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_CB)
            {
                g_SetupPathInfo.wMRWSizeInH = 2*g_SetupPathInfo.wImgSizeInH;
                g_SetupPathInfo.wMRWSizeInV = g_SetupPathInfo.wImgSizeInV;
            }
            g_SetupPathInfo.wMRWSizeOutH = g_SetupPathInfo.wImgSizeInH;
            g_SetupPathInfo.wMRWSizeOutV = ucMRWOut3DFmt*g_SetupPathInfo.wImgSizeInV;
            break;
        case SC_PATH_MODE_2:
            g_SetupPathInfo.wMRWSizeInH = g_SetupPathInfo.wImgSizeOutH;
            g_SetupPathInfo.wMRWSizeInV = g_SetupPathInfo.wImgSizeInV;
            if((g_SetupPathInfo.ucSCFmtIn >= SC_FMT_IN_3D_TB &&  g_SetupPathInfo.ucSCFmtIn <= SC_FMT_IN_3D_SBSF)){
                g_SetupPathInfo.wMRWSizeInV *=2;
                
            }
            else if(g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_CB)
            {
                g_SetupPathInfo.wMRWSizeInH = 2*g_SetupPathInfo.wImgSizeOutH;
                g_SetupPathInfo.wMRWSizeInV = g_SetupPathInfo.wImgSizeInV;
            }
            g_SetupPathInfo.wMRWSizeOutH = g_SetupPathInfo.wImgSizeOutH;
            g_SetupPathInfo.wMRWSizeOutV = ucMRWOut3DFmt*g_SetupPathInfo.wImgSizeInV;
            break;
        case SC_PATH_MODE_3:
            g_SetupPathInfo.wMRWSizeInH = g_SetupPathInfo.wImgSizeInH;
            g_SetupPathInfo.wMRWSizeInV = g_SetupPathInfo.wImgSizeOutV;
            if((g_SetupPathInfo.ucSCFmtIn >= SC_FMT_IN_3D_TB &&  g_SetupPathInfo.ucSCFmtIn <= SC_FMT_IN_3D_SBSF))
            {
                g_SetupPathInfo.wMRWSizeInV *=2;
                
            }
            else if(g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_CB)
            {
                g_SetupPathInfo.wMRWSizeInH = 2*g_SetupPathInfo.wImgSizeInH;
                g_SetupPathInfo.wMRWSizeInV = g_SetupPathInfo.wImgSizeOutV;
            }
            g_SetupPathInfo.wMRWSizeOutH = g_SetupPathInfo.wImgSizeInH;
            g_SetupPathInfo.wMRWSizeOutV = ucMRWOut3DFmt*g_SetupPathInfo.wImgSizeOutV;
            break;
        case SC_PATH_MODE_4:
            g_SetupPathInfo.wMRWSizeInH = g_SetupPathInfo.wImgSizeOutH;
            g_SetupPathInfo.wMRWSizeInV = g_SetupPathInfo.wImgSizeOutV;
            if((g_SetupPathInfo.ucSCFmtIn >= SC_FMT_IN_3D_TB &&  g_SetupPathInfo.ucSCFmtIn <= SC_FMT_IN_3D_SBSF))
            {
                g_SetupPathInfo.wMRWSizeInV *=2;
                
            }
            else if(g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_CB)
            {
                g_SetupPathInfo.wMRWSizeInH = 2*g_SetupPathInfo.wImgSizeOutH;
                g_SetupPathInfo.wMRWSizeInV = g_SetupPathInfo.wImgSizeOutV;
            }
            g_SetupPathInfo.wMRWSizeOutH = g_SetupPathInfo.wImgSizeOutH;
            g_SetupPathInfo.wMRWSizeOutV = ucMRWOut3DFmt*g_SetupPathInfo.wImgSizeOutV;
            break;
    }

    if( InputColorFormat == INPUTCOLOR_RGB
        &&  g_SetupPathInfo.ucSCPathMode != SC_PATH_MODE_0)
        g_SetupPathInfo.bMRWR2Y = 1;
    else
        g_SetupPathInfo.bMRWR2Y = 0;
        
    if( g_SetupPathInfo.bFBMode && (DRAM_SIZE <= 0x800000UL))
        g_SetupPathInfo.bMRW422 = 1;
    else
        g_SetupPathInfo.bMRW422 = 0;

}

static void msSetupPathConfig()
{
    DB_Mode(eDB_NORMAL_MODE);
    
    switch(g_SetupPathInfo.ucSCPathMode)
    {
        case SC_PATH_MODE_0:
            msWriteByteMask(SC6_02,0x00,BIT3|BIT2);
            DB_WBMask(SC0_5B,IP_HSP2VSP2OP,BIT7|BIT6); // IP->VSP->OP
            DB_WBMask(REG_1E2C,BIT4|BIT0,0xFF);
            DB_WBMask(REG_1E30,0x00,0xFF);
            break;
        case SC_PATH_MODE_1:
            msWriteByteMask(SC6_02,BIT3|BIT2,BIT3|BIT2);
            msWriteByteMask(SC6_AC,IP2MRW2VSP,0x0F); // ip->MRW->VSP
            DB_WBMask(SC0_5B,MRW2VSP2HSP_OP,BIT7|BIT6); // IP->VSP->HSP
            DB_WBMask(REG_1E2C,BIT6,0xFF);
            DB_WBMask(REG_1E30,BIT7,0xFF);
            break;
        case SC_PATH_MODE_2:
            msWriteByteMask(SC6_02,BIT3|BIT2,BIT3|BIT2);
            msWriteByteMask(SC6_AC,HSP2MRW2VSP,0x0F); // HSP->MRW->VSP
            DB_WBMask(SC0_5B,MRW2VSP2HSP_OP,BIT7|BIT6); // MRW->VSP->OP
            DB_WBMask(REG_1E2C,BIT6,0xFF);
            DB_WBMask(REG_1E30,BIT7,0xFF);
            break;
        case SC_PATH_MODE_3:
            msWriteByteMask(SC6_02,BIT3|BIT2,BIT3|BIT2);
            msWriteByteMask(SC6_AC,VSP2MRW2HSP,0x0F); // VSP->MRW->HSP
            msWriteByteMask(SC0_5B,IP_HSP2VSP2MRW,BIT7|BIT6); // IP->VSP->MRW
            DB_WBMask(REG_1E2C,BIT6|BIT2,0xFF);
            DB_WBMask(REG_1E30,BIT2,0xFF);
            break;
        case SC_PATH_MODE_4:
            msWriteByteMask(SC6_02,BIT3|BIT2,BIT3|BIT2);
            msWriteByteMask(SC6_AC,VSP2MRW2OP1,0x0F); //VSP->MRW->OP1
            DB_WBMask(SC0_5B,IP_HSP2VSP2MRW,BIT7|BIT6); // HSP->VSP->MRW
            DB_WBMask(REG_1E2C,BIT6|BIT2,0xFF);
            DB_WBMask(REG_1E30,BIT2,0xFF);
            break;
    }
    
#if ENABLE_SW_DOUBLE_BUFFER 
    msSWDBWriteToRegister();
    msSWDBWaitForRdy();
#endif      

}

static void msSetupPathIP()
{
    XDATA BYTE ucBlanking, ucFraction, ucRstOffset, ucDlyLine;
    XDATA WORD uwVDE, uwOffset;

    if(CURRENT_SOURCE_IS_INTERLACE_MODE() )
    	 ucDlyLine = 5;  
    else
        ucDlyLine = 4; 

    ucRstOffset = SC_LOCK_LINE - ucDlyLine;
 
    // chess board
    msWriteBit(SC0_A6, (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_CB), _BIT2);

    DB_Mode(eDB_NORMAL_MODE);

    DB_WBMask(SC0_90, (ucDlyLine)<<4, _BIT6|_BIT5|_BIT4); //delay line
    DB_WBMask(SC0_0F, (SC_LOCK_LINE), 0x0F); //Ylock line

    uwVDE = g_SetupPathInfo.wImgSizeInV;
    switch(g_SetupPathInfo.ucSCFmtIn)
    {
        case SC_FMT_IN_3D_FP:
        case SC_FMT_IN_3D_FA:
#ifdef FPGA
            ucBlanking = 30;
#else
            ucBlanking = (StandardModeVTotal - StandardModeHeight)/2;
#endif
            DB_W2BMask(REG_2E60, uwVDE + ucBlanking + 1 + BIT12, 0x1FFF); //start
            DB_W2BMask(REG_2E62, uwVDE + 1, 0x0FFF); //end
            uwOffset = uwVDE + 1;
            DB_W2BMask(REG_2E6C, uwOffset + ucRstOffset + BIT12, 0x1FFF); // frame reset setting
            DB_W2B(REG_2E64, 0x0000);
            DB_W2B(REG_2E66, 0x0000);
            DB_W2B(REG_2E68, 0x0000);
            DB_W2B(REG_2E6A, 0x0000);
            DB_W2B(REG_2E6E, 0x0000);
            DB_W2B(REG_2E70, 0x0000);
            break;
        case SC_FMT_IN_3D_FPI:
            ucBlanking = (StandardModeVTotal - StandardModeHeight)/4;
            ucFraction = ((StandardModeVTotal - StandardModeHeight)/2) & _BIT0;

            DB_W2BMask(REG_2E60, uwVDE*3 + 3*ucBlanking + 2*ucFraction + 1, 0x0FFF); //start
            uwOffset = uwVDE*3 + 2*ucBlanking + ucFraction + 1;
            DB_W2BMask(REG_2E62, uwOffset, 0x0FFF); //end
            DB_WBit(REG_2E61, TRUE, _BIT4);
            DB_W2BMask(REG_2E6C, uwOffset + ucRstOffset, 0x0FFF); // frame reset setting
            DB_WBit(REG_2E6D, TRUE, _BIT4);

            DB_W2BMask(REG_2E64, uwVDE*2 + 2*ucBlanking + ucFraction + 1, 0x0FFF); //start
            uwOffset = uwVDE*2 + ucBlanking + ucFraction + 1;
            DB_W2BMask(REG_2E66, uwOffset, 0x0FFF); //end
            DB_WBit(REG_2E65, TRUE, _BIT4);
            DB_W2BMask(REG_2E6E, uwOffset + ucRstOffset, 0x0FFF); // frame reset setting
            DB_WBit(REG_2E6F, TRUE, _BIT4);

            DB_W2BMask(REG_2E68, uwVDE + ucBlanking + ucFraction + 1, 0x0FFF); //start
            uwOffset = uwVDE + 1;
            DB_W2BMask(REG_2E6A, uwOffset, 0x0FFF); //end
            DB_WBit(REG_2E69, TRUE, _BIT4);
            DB_W2BMask(REG_2E70, uwOffset + ucRstOffset, 0x0FFF); // frame reset setting
            DB_WBit(REG_2E71, TRUE, _BIT4);
            break;      
        default:
            DB_W2B(REG_2E60, 0x0000);
            DB_W2B(REG_2E62, 0x0000);
            DB_W2B(REG_2E64, 0x0000);
            DB_W2B(REG_2E66, 0x0000);
            DB_W2B(REG_2E68, 0x0000);
            DB_W2B(REG_2E6A, 0x0000);
            DB_W2B(REG_2E6C, 0x0000);
            DB_W2B(REG_2E6E, 0x0000);
            DB_W2B(REG_2E70, 0x0000);
            break;
    }
    
#if ENABLE_SW_DOUBLE_BUFFER 
    msSWDBWriteToRegister();
#endif

}

static void msSetupPathHSP()
{ 
    switch(g_SetupPathInfo.ucSCPathMode)
    {
        case SC_PATH_MODE_0:
        case SC_PATH_MODE_1:
        case SC_PATH_MODE_3:
            msWrite2ByteMask(SC0_C6,g_SetupPathInfo.wImgSizeInH,0x0FFF);
            msWrite2ByteMask(SC0_C8,g_SetupPathInfo.wImgSizeOutH,0x0FFF);
            msWriteByteMask(SC0_B5,0,BIT5|BIT4);
            break;
        case SC_PATH_MODE_2:
        case SC_PATH_MODE_4:
            if(IS_FMT_IN_SBS() || g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_CB)
            {
                msWrite2ByteMask(SC0_C6,2*g_SetupPathInfo.wImgSizeInH,0x0FFF);
                msWrite2ByteMask(SC0_C8,2*g_SetupPathInfo.wImgSizeOutH,0x0FFF);
            }
            else
            {   
                msWrite2ByteMask(SC0_C6,g_SetupPathInfo.wImgSizeInH,0x0FFF);
                msWrite2ByteMask(SC0_C8,g_SetupPathInfo.wImgSizeOutH,0x0FFF);
            }
            msWriteBit(SC0_B5,IS_FMT_IN_SBS(),BIT5);
            msWriteBit(SC0_B5,g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_CB,BIT4);
            break;
    }
}

static void msSetupPathVSP()
{
    switch(g_SetupPathInfo.ucSCPathMode)
    {
        case SC_PATH_MODE_0:
            msWrite2ByteMask(SC0_C0,g_SetupPathInfo.wImgSizeInV,0x0FFF);
            msWrite2ByteMask(SC0_C4,g_SetupPathInfo.wImgSizeOutV,0x0FFF);
            msWrite2ByteMask(SC0_C2,g_SetupPathInfo.wImgSizeOutH,0x0FFF);
            break;
        case SC_PATH_MODE_4:
            if(g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_TB || g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_LA)
            {
                msWrite2ByteMask(SC0_C0,2*g_SetupPathInfo.wImgSizeInV,0x0FFF);
                msWrite2ByteMask(SC0_C4,2*g_SetupPathInfo.wImgSizeOutV,0x0FFF);
            }
            else
            {
                msWrite2ByteMask(SC0_C0,g_SetupPathInfo.wImgSizeInV,0x0FFF);
                msWrite2ByteMask(SC0_C4,g_SetupPathInfo.wImgSizeOutV,0x0FFF);
            }
            if(IS_FMT_IN_SBS() || g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_CB)
                msWrite2ByteMask(SC0_C2,2*g_SetupPathInfo.wImgSizeOutH,0x0FFF);
            else
                msWrite2ByteMask(SC0_C2,g_SetupPathInfo.wImgSizeOutH,0x0FFF);
            break;
        case SC_PATH_MODE_3:
            if(g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_TB || g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_LA)
            {
                msWrite2ByteMask(SC0_C0,2*g_SetupPathInfo.wImgSizeInV,0x0FFF);
                msWrite2ByteMask(SC0_C4,2*g_SetupPathInfo.wImgSizeOutV,0x0FFF);
            }
            else
            {
                msWrite2ByteMask(SC0_C0,g_SetupPathInfo.wImgSizeInV,0x0FFF);
                msWrite2ByteMask(SC0_C4,g_SetupPathInfo.wImgSizeOutV,0x0FFF);
            }
            if(IS_FMT_IN_SBS() || g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_CB)
                msWrite2ByteMask(SC0_C2,2*g_SetupPathInfo.wImgSizeInH,0x0FFF);
            else
                msWrite2ByteMask(SC0_C2,g_SetupPathInfo.wImgSizeInH,0x0FFF);
            break;
        case SC_PATH_MODE_1:
        case SC_PATH_MODE_2:
            if(g_SetupPathInfo.ucSCFmtIn != SC_FMT_IN_NORMAL)
            {
                msWrite2ByteMask(SC0_C0,g_SetupPathInfo.wImgSizeInV*2,0x0FFF);
                msWrite2ByteMask(SC0_C2,g_SetupPathInfo.wMRWSizeOutH,0x0FFF);
                msWrite2ByteMask(SC0_C4,g_SetupPathInfo.wImgSizeOutV*2,0x0FFF);
            }
            else
            {
                msWrite2ByteMask(SC0_C0,g_SetupPathInfo.wImgSizeInV,0x0FFF);
                msWrite2ByteMask(SC0_C2,g_SetupPathInfo.wMRWSizeOutH,0x0FFF);
                msWrite2ByteMask(SC0_C4,g_SetupPathInfo.wImgSizeOutV,0x0FFF);
            }
            break;
    }

    if(g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_3 || g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_4)
    {
        if(g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_TB)
            msWriteByteMask(SC0_5B,BIT0,BIT1|BIT0);
        else if(g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_LA)
            msWriteByteMask(SC0_5B,BIT1,BIT1|BIT0);
        else
            msWriteByteMask(SC0_5B,0x00,BIT1|BIT0);
        
        msWriteBit(SC0_C1,IS_FMT_IN_SBS(),BIT6);       
        
        if(IS_FMT_IN_SBS() && (StandardModeWidth > 1920))
        {
            msWriteBit(SC0_C1,1,BIT5);  
            msWriteBit(SC0_C1,0,BIT6);   
            msWriteByteMask(SC0_5B,BIT1,BIT1|BIT0);
            msWrite2ByteMask(SC0_C2,(msRead2Byte(SC0_C2)&0xFFF)>>1,0x0FFF);             
            msWrite2ByteMask(SC0_C4,(msRead2Byte(SC0_C4)&0xFFF)<<1,0x0FFF);           
        }
        else
            msWriteBit(SC0_C1,0,BIT5);  
    }
    else if(g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_1 || g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_2)
    {
        msWriteByteMask(SC0_C1, 0x00, _BIT6|_BIT5);
        if(g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL)
            msWriteByteMask(SC0_5B, 0x00, _BIT1|_BIT0);
        else
            msWriteByteMask(SC0_5B,BIT1,BIT1|BIT0);
    }
    else
    {
        msWriteByteMask(SC0_5B, 0x00, _BIT1|_BIT0);
        msWriteByteMask(SC0_C1, 0x00, _BIT6|_BIT5);
    }
}

static void msSetupPathMRWBaseRD()
{
    msWrite2Byte(SC6_E0,g_SetupPathInfo.wMRWSizeOutH);
    msWrite2Byte(SC6_E2,g_SetupPathInfo.wMRWSizeOutV);
    msWrite2Byte(SC6_AE,g_SetupPathInfo.wMRWSizeOutH);
   
}

static void msSetupPathMRWBaseWR()
{
    XDATA DWORD FB_SIZE;
    XDATA WORD u16offset;
    
    FB_SIZE = ((DWORD)g_SetupPathInfo.wMRWSizeInH) * (DWORD)g_SetupPathInfo.wMRWSizeInV;
   
    // set Frame buffer 
    msWrite3Byte(SC6_C4,(DWORD)MIU_FB_ADDR_START>>2);
    msWrite3Byte(SC6_C0,(DWORD)MIU_FB_ADDR_END>>2);
    
    msWrite2Byte(SC6_DC,g_SetupPathInfo.wMRWSizeInH);
    msWrite2Byte(SC6_DE,g_SetupPathInfo.wMRWSizeInV);
    
    msWriteByteMask(SC6_A6,0,BIT7);
    
    if(g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL)
        msWriteByteMask(SC6_03,BIT3,BIT3);
    else
        msWriteByteMask(SC6_03,0,BIT3); 

    if(IS_MRW_FMT_LA() && IS_FMT_OUT_PSV())
    {
        msWriteByteMask(SC6_03,BIT2,BIT2|BIT1);   
#ifdef FPGA
        if(g_SetupPathInfo.bPsvPnlOutLRInvert)
        {
            u16offset = g_SetupPathInfo.wMRWSizeInH *16/32;
            msWrite2Byte(SC6_E4,u16offset);
            msWrite2Byte(SC6_E6,u16offset); 
        }
        else
        {
            msWrite2Byte(SC6_E4,0x00);
            msWrite2Byte(SC6_E6,0x00); 
        }
#else
        if(g_SetupPathInfo.bPsvPnlOutLRInvert)
        {
            if(g_SetupPathInfo.bMRW422)
                u16offset = g_SetupPathInfo.wMRWSizeInH *16/32;
            else
                u16offset = g_SetupPathInfo.wMRWSizeInH *24/32;
            msWrite2Byte(SC6_E4,u16offset);
            msWrite2Byte(SC6_E6,u16offset); 
        }
        else
        {
            msWrite2Byte(SC6_E4,0x00);
            msWrite2Byte(SC6_E6,0x00); 
        }
#endif        
        
    }
    else  if( (!IS_MRW_FMT_LA()) && (g_SetupPathInfo.ucSCFmtIn != SC_FMT_IN_NORMAL)
              && (IS_FMT_OUT_PSV()) )
    {
        if(g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FP || g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FPI)
            msWriteByteMask(SC6_03,0,BIT2|BIT1);
        else if(g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_CB)
            msWriteByteMask(SC6_03,BIT1,BIT2|BIT1);
        else
            msWriteByteMask(SC6_03,0,BIT2|BIT1);
#ifdef FPGA
        u16offset = g_SetupPathInfo.wMRWSizeInH *16/32;
#else
        if(g_SetupPathInfo.bMRW422)
            u16offset = g_SetupPathInfo.wMRWSizeInH *16/32;
        else
            u16offset = g_SetupPathInfo.wMRWSizeInH *24/32;
#endif        
        if(g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_CB)
            u16offset = 0;
        
        msWrite2Byte(SC6_E4,u16offset);
        msWrite2Byte(SC6_E6,u16offset);  
        
        if(g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_TB)
        {
#ifdef FPGA
            msWrite3Byte(SC6_A4,FB_SIZE/2*16/32);
#else
            if(g_SetupPathInfo.bMRW422)
                msWrite3Byte(SC6_A4,FB_SIZE/2*16/32);
            else
                msWrite3Byte(SC6_A4,FB_SIZE/2*24/32);
#endif
            msWriteByteMask(SC6_A6,BIT7,BIT7);
        }
    }
    else
    {
        msWrite2Byte(SC6_E4,0x00);
        msWrite2Byte(SC6_E6,0x00);  
    }

    if(g_SetupPathInfo.bFBMode && (V_FREQ_IN<48)) // FB mode, FRC case
    {
        if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FP) || (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FPI) ) // FP
            msWrite2Byte(SC6_D0,11);
#if ENABLE_DISPLAY_UNDERSCAN
        else if(UnderScanSetting.Enable)
        {
            WORD Sample_vst;
            Sample_vst = ( msRead2Byte( SC0_05 ) & SC_MASK_V )/((SrcFlags&bInterlaceMode)?2:1);
            msWrite2Byte(SC6_D0,(WORD)((DWORD)(mStar_SetIVS()- Sample_vst-10)*g_SetupPathInfo.wMRWSizeInV/g_SetupPathInfo.wImgSizeInV));
        }
#endif        
        else // TB, SBS, 2D
            msWrite2Byte(SC6_D0,g_SetupPathInfo.wMRWSizeInV/2 + 10);
    }
    else if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_TB)
     && !g_SetupPathInfo.bPsvPnlOutLRInvert )
        msWrite2Byte(SC6_D0,g_SetupPathInfo.wMRWSizeInV/2 + 10);
    else if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_TB)
              && g_SetupPathInfo.bPsvPnlOutLRInvert )
        msWrite2Byte(SC6_D0,g_SetupPathInfo.wMRWSizeInV/2 - 10);
    else if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FP || g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FPI)
             && g_SetupPathInfo.bPsvPnlOutLRInvert )
        msWrite2Byte(SC6_D0,g_SetupPathInfo.wMRWSizeInV - 10);
    else
        msWrite2Byte(SC6_D0,10);
    
    msWriteByteMask(SC6_BA,0,BIT1|BIT2);
    msWriteByteMask(SC0_A6,0,BIT5);
    msWriteByteMask(SC6_03,0,BIT5);
    msWriteByteMask(SC6_D4,0,BIT1|BIT0);
    
    if(g_SetupPathInfo.bPsvPnlOutLRInvert)
    {
        if( IS_MRW_FMT_LA())
            msWriteByteMask(SC6_BA,BIT2,BIT1|BIT2);
        else if (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_CB)
            msWriteByteMask(SC0_A6,BIT5,BIT5);
        else
        {
            msWriteByteMask(SC6_03,BIT5,BIT5);
            msWriteByteMask(SC6_D4,BIT1|BIT0,BIT1|BIT0); // U2, MRW LR invert
        }
    }
    
}

static void msSetupPathMRW()
{
    msSetupPathMRWBaseRD();
    msSetupPathMRWBaseWR();
    // set MRW R/W burst
    if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_TB)
     && ( (g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_3)
        ||(g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_4) ) )
    {
        XDATA BYTE i;
        XDATA DWORD FB_SIZE;

        FB_SIZE = ((DWORD)g_SetupPathInfo.wMRWSizeInH) * (DWORD)g_SetupPathInfo.wMRWSizeInV * ((g_SetupPathInfo.bMRW422)?16:24);
        
        for(i=0;i<=16;i++)
        {
            if( (FB_SIZE % (((DWORD)0x20+i)<<6)) == 0 )
            {
                i = 0x20 + i;
                break;
            }
            if( (FB_SIZE % (((DWORD)0x20-i)<<6)) == 0 )
            {
                i = 0x20 - i;
                break;
            }
            if (i==16)
            {
                i = 0x20;
                MSSCALER_printMsg("*****MRW Burst Length NOT Aligned!! Need to Increase Threshold!!*****");            
            }
        }
        msWriteByte(SC6_06,i);
    }
    else
    {
        msWriteByte(SC6_06,0x20);
    }
    
    msWriteByte(SC6_08,0x20);
    // set MRW R/W FIFO depth
    msWrite2Byte(SC6_C8,360-1); // max write fifo depth = 360 x 64 bit
    msWrite2Byte(SC6_CA,360-1); // max read fifo depth = 360 x 64 bit
    // setup MIU video format     
#ifdef FPGA
    msWriteBit(SC6_03,1,BIT0);
#else
    msWriteBit(SC6_03,g_SetupPathInfo.bMRW422,BIT0);
#endif
    msWriteBit(SC6_A3,g_SetupPathInfo.bMRW422,BIT6);
    msWriteBit(SC6_A3,g_SetupPathInfo.bMRW422,BIT0);
    msWriteByteMask(SC6_A3,BIT1,BIT1);
    msWriteBit(SC6_A2,g_SetupPathInfo.bMRWR2Y,BIT7);
    
    // set trig
    msWrite2Byte(SC0_AC,00);
    msWrite2Byte(SC0_AE,PanelVTotal - 4);     
}
#if 0
static void msSetupPathMRWInit()
{
    BYTE u8Bank=msReadByte(SC0_00);
    msWriteByte(SC0_00, 0x06);

    // double buffer lock
    msWriteByteMask(SC6_03, _BIT0, _BIT0);

    msSetupPathMRWBaseWR();

    // lock disable
    msWriteByteMask(SC6_03, 0, _BIT0);
    msWriteByte(SC0_00,u8Bank);
}
//ENABLE_DEBUG
static void msSetupPathDbgDump(BYTE ucVaule)
{
ucVaule=ucVaule;
    MSSCALER_printData("===========\r\nmsSetupPath(%d)" , ucVaule);
    MSSCALER_printData("wImgSizeInH = %d", g_SetupPathInfo.wImgSizeInH);
    MSSCALER_printData("wImgSizeInV = %d", g_SetupPathInfo.wImgSizeInV);
    MSSCALER_printData("wImgSizeOutH = %d", g_SetupPathInfo.wImgSizeOutH);
    MSSCALER_printData("wImgSizeOutV = %d", g_SetupPathInfo.wImgSizeOutV);
    MSSCALER_printData("wMRWSizeInH = %d", g_SetupPathInfo.wMRWSizeInH);
    MSSCALER_printData("wMRWSizeInV = %d", g_SetupPathInfo.wMRWSizeInV);
    MSSCALER_printData("wMRWSizeOutH = %d", g_SetupPathInfo.wMRWSizeOutH);
    MSSCALER_printData("wMRWSizeOutV = %d", g_SetupPathInfo.wMRWSizeOutV);
    MSSCALER_printData("ucSCFmtIn = %d", g_SetupPathInfo.ucSCFmtIn);
    MSSCALER_printData("ucSCFmtOut = %d", g_SetupPathInfo.ucSCFmtOut);
    MSSCALER_printData("bScalingUpH = %d", g_SetupPathInfo.bScalingUpH);
    MSSCALER_printData("bScalingUpV = %d", g_SetupPathInfo.bScalingUpV);
    MSSCALER_printData("ucSCPathMode = %d", g_SetupPathInfo.ucSCPathMode);
    MSSCALER_printData("bMRWDual = %d", g_SetupPathInfo.bMRWDual);
    MSSCALER_printData("bVBIExtend = %d", g_SetupPathInfo.bVBIExtend);
    MSSCALER_printData("bHSDEnable = %d", g_SetupPathInfo.bHSDEnable);
    MSSCALER_printData("bFBMode = %d", g_SetupPathInfo.bFBMode);
}
#endif
static void msSetupPathOD()
{
#if ENABLE_RTE
    if(g_SetupPathInfo.bLoadOdTbl)
    {
        msOverDriverInit();
        msRTE_LoadLUT();
        g_SetupPathInfo.bLoadOdTbl = FALSE;
    }
#endif
}

#if ENABLE_DISPLAY_UNDERSCAN    
static void msSetupPathTimingGen(void)
{
    if(UnderScanSetting.Enable)
    {
        msWrite2Byte(SC0_10,PanelVTotal-1-UnderScanSetting.VSize);//v de start
        msWrite2Byte(SC0_14,PANEL_HEIGHT - 1 -UnderScanSetting.VSize);
        msWrite2Byte(SC0_1A,PANEL_HEIGHT-1-UnderScanSetting.VSize*2);
        if(EXPANSION_MODE_IS_FULL())
        {
            msWrite2Byte(SC0_18,( PanelHStart/PANEL_H_DIV )+UnderScanSetting.HSize);
            msWrite2Byte(SC0_1C,( ( PanelHStart/PANEL_H_DIV ) + (PANEL_WIDTH/PANEL_H_DIV) - 1 )-UnderScanSetting.HSize);
        }
    }
    else
    {
        msWrite2Byte(SC0_10,0x00);//v de start
        msWrite2Byte(SC0_14,PANEL_HEIGHT - 1);
        msWrite2Byte(SC0_1A,PANEL_HEIGHT-1);
        if(EXPANSION_MODE_IS_FULL())
        {
            msWrite2Byte(SC0_18,( PanelHStart/PANEL_H_DIV ));
            msWrite2Byte(SC0_1C,( ( PanelHStart/PANEL_H_DIV ) + (PANEL_WIDTH/PANEL_H_DIV) - 1 ));
        }
    }
}
/*
void mStar_SetUnderScan(BOOL Enable, BYTE HSize, BYTE VSize)
{
    UnderScanSetting.Enable = Enable;
    UnderScanSetting.HSize = HSize;
    UnderScanSetting.VSize = VSize;
#if 1
    mStar_SetupPath();
    if(Enable)
    {
        msSetFrameSyncMode(FLM_FB_FPLL);
        msWriteByteMask(REG_3818,BIT3,BIT3);//FramePLL Enable
    }
    else
    {
        msSetFrameSyncMode(FLM_FBL);
        msWriteByteMask(REG_3818,0,BIT3);//FramePLL Enable
    }
#else
    mStar_SetIVS();
    mStar_SetupPath();
    msSetFrameSyncMode(FLM_FB_FPLL);
    msWriteByteMask(REG_3818,BIT3,BIT3);//FramePLL Enable
#endif    
    //Menu_AdjustColorSetting();
}
*/
#endif

void mStar_SetupPath()
{
    XDATA BYTE u8Org=mStar_ScalerDoubleBuffer(FALSE);
    msWriteByteMask(SC6_02, _BIT5, _BIT5);

    msSetupPathParam();  
    msSetupPathConfig();
    msSetupPathIP();
    msSetupPathHSP();
    msSetupPathVSP();
    msSetupPathMRW(); 
    msSetupPathOD();
    //msSetupPathDbgDump();
    mStar_SetScalingFactor();
    mStar_SetScalingFilter();
#if ENABLE_DISPLAY_UNDERSCAN
    msSetupPathTimingGen();
#endif
    msWriteByteMask(SC6_02, 0, _BIT5);

    mStar_ScalerDoubleBuffer(u8Org&BIT0);
}

void mStar_SetupPathInit()
{
    msWriteByteMask(SC6_02,BIT4,BIT5|BIT4);
    
    g_SetupPathInfo.bHSDEnable = 0;
    g_SetupPathInfo.ucOut3DLRSel = OUT_3D_FIXED_NORMAL;

    g_SetupPathInfo.bFBMode = 0;

    g_SetupPathInfo.bOverrideSCFmtIn = FALSE;
    g_SetupPathInfo.ucSCFmtIn = SC_FMT_IN_NORMAL;
    g_SetupPathInfo.bMRWDual = 0;
    g_SetupPathInfo.bMRW422 = 0;
    g_SetupPathInfo.bMRWR2Y = 0;
#if ENABLE_3D_FUNCTION && GLASSES_TYPE==GLASSES_INTEL
    g_SetupPathInfo.bMRWLLRR = 1;
#else
    g_SetupPathInfo.bMRWLLRR = 0;
#endif

#if PANEL_3D_PASSIVE
    g_SetupPathInfo.ucSCFmtOut = SC_FMT_OUT_3D_PSV;
    g_SetupPathInfo.bPsvPnlOutLRInvert = 0;
#elif PANEL_3D_PASSIVE_4M
    g_SetupPathInfo.ucSCFmtOut = SC_FMT_OUT_3D_PSV_4M;
    g_SetupPathInfo.bPsvPnlOutLRInvert = 1;
#else
    g_SetupPathInfo.ucSCFmtOut = SC_FMT_OUT_3D_FS;
    g_SetupPathInfo.bPsvPnlOutLRInvert = 0;
#endif

    g_SetupPathInfo.wImgSizeOutH = PANEL_WIDTH;
    g_SetupPathInfo.wImgSizeOutV = PANEL_HEIGHT;

#if CHIP_ID == CHIP_TSUM2
#if ENABLE_R2_2DTO3D
    g_SetupPathInfo.b2DTo3DEnable = UserPref2Dto3D;
#else
    g_SetupPathInfo.b2DTo3DEnable = FALSE;
#endif
    g_SetupPathInfo.bLoadOdTbl = FALSE;
#endif

#if ENABLE_DISPLAY_UNDERSCAN
    UnderScanSetting.Enable = FALSE;
    UnderScanSetting.HSize = 0;
    UnderScanSetting.VSize = 0;
#endif

}



#endif
