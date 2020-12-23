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
#if ENABLE_R2_2DTO3D && LGE_DEMO
#define T3D_V4TAP_COEF_SEL  6 //LGE_DEMO
#endif
#define V4TAP_COEF_SEL  3 //Fc80
BYTE code tVScalingCoeff[ 7 ][ V_COEF_NUM ] =
{
    { 0x02, 0x03, 0x04, 0x05, 0x06, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x14, 0x15, 0x16, 0x17, 0x18 }, /* Address */
    { 0x18, 0x11, 0x0B, 0x07, 0x04, 0x50, 0x4E, 0x4A, 0x44, 0x3C, 0x18, 0x21, 0x2A, 0x33, 0x3C, 0x00, 0x00, 0x01, 0x02, 0x04 }, /* Fc60 */ /* Coef */
    { 0x13, 0x0B, 0x05, 0x02, 0x00, 0x5A, 0x5A, 0x54, 0x4B, 0x40, 0x13, 0x1C, 0x28, 0x34, 0x40, 0x00, 0x81, 0x81, 0x81, 0x00 }, /* Fc70 */ /* Coef */
    { 0x0D, 0x05, 0x00, 0x82, 0x83, 0x66, 0x64, 0x5D, 0x51, 0x43, 0x0D, 0x18, 0x25, 0x34, 0x43, 0x00, 0x81, 0x82, 0x83, 0x83 }, /* Fc80 */ /* Coef */
    { 0x07, 0x81, 0x85, 0x86, 0x86, 0x72, 0x70, 0x67, 0x57, 0x46, 0x07, 0x12, 0x21, 0x33, 0x46, 0x00, 0x81, 0x83, 0x84, 0x86 }, /* Fc90 */ /* Coef */
    { 0x00, 0x87, 0x8A, 0x8A, 0x88, 0x80, 0x7C, 0x70, 0x5E, 0x48, 0x00, 0x0C, 0x1D, 0x31, 0x48, 0x00, 0x81, 0x83, 0x85, 0x88 }, /* Fc100 *//* Coef */
    { 0x1D, 0x16, 0x10, 0x0B, 0x08, 0x46, 0x45, 0x42, 0x3E, 0x38, 0x1D, 0x24, 0x2B, 0x32, 0x38, 0x00, 0x01, 0x03, 0x05, 0x08 }, /* T3D - LGE_DEMO */
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

        #if ENABLE_R2_2DTO3D && LGE_DEMO
        if (UserPref2Dto3D)
        {
            for ( i = 0; i < V_COEF_NUM; i++ )
                DB_WB( (BIT15|0x0700|tVScalingCoeff[ 0 ][ i ]), tVScalingCoeff[ T3D_V4TAP_COEF_SEL ][ i ] );
        }
        else
        {
            for ( i = 0; i < V_COEF_NUM; i++ )
            DB_WB( (BIT15|0x0700|tVScalingCoeff[ 0 ][ i ]), tVScalingCoeff[ V4TAP_COEF_SEL ][ i ] );
        }
        #else
        for ( i = 0; i < V_COEF_NUM; i++ )
            DB_WB( (BIT15|0x0700|tVScalingCoeff[ 0 ][ i ]), tVScalingCoeff[ V4TAP_COEF_SEL ][ i ] );
        #endif

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
            DB_WBMask( SC0_58, 0x00, BIT0);//H initial factor -1(R)
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
        DB_W3B( SC0_50, 0x100000 );// input width=output width, then disable expansion

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
            DB_WBMask(SC0_C2,0x00,0x0F);//V initial -1 [0]:L+odd,[1]:R+odd,[2]:L+even,[3]:R+even
            factor = ((DWORD)height - (DWORD)g_SetupPathInfo.wImgSizeOutV) *256 / 2 / g_SetupPathInfo.wImgSizeOutV;
        }
        DB_WB(SC0_D8,factor);
        DB_WB(SC0_D9,factor);
        DB_WB(SC0_DA,factor);
        DB_WB(SC0_DB,factor);
#else
        if(g_SetupPathInfo.ucSCFmtIn != SC_FMT_IN_NORMAL)
        {
            if( (g_SetupPathInfo.bPsvPnlOutLRInvert)
             && !(g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_1)
             && !(g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_2) )
            {
                DB_WB(SC0_D8,0x80);//L,odd
                DB_WB(SC0_D9,0x00);//R,odd
                DB_WB(SC0_DA,0xFF);//L,even
                DB_WB(SC0_DB,0x80);//R,even
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

        #if ENABLE_R2_2DTO3D && LGE_DEMO
        if (UserPref2Dto3D)
        {
            DB_WB(SC0_D8,0x80);
            DB_WB(SC0_D9,0x80);
        }
        else
        #endif
        {
            DB_WB(SC0_D8,0x00);
            DB_WB(SC0_DA,0x80);
        }
        }

        // In line repeat mode, the factor should be ((Vin -1)/(Vout -1)) * 2.
        // The original equation is (Vin-1) / ((Vout /2) -1) which is not equal to ((Vin -1)/(Vout -1)) * 2
        if (msReadByte(SC0_C3) & BIT7)
            g_SetupPathInfo.wImgSizeOutV *= 2;

        factor = height - 1; // factor=((input height-1)*2^20)/(output height-1) + 1
        factor = (( DWORD )( factor << 20 ) ) / ( g_SetupPathInfo.wImgSizeOutV - 1 ) + 1;

        // Please refer to the former comment about line repeat mode.
        if (msReadByte(SC0_C3) & BIT7)
            factor *= 2;

        DB_W3B( SC0_53, factor|0x800000);
#endif
    }
    else
        DB_W3B( SC0_53, 0x100000 ); // input height=output height, then disable expansion

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
    //output line alternative
    if((IS_FMT_OUT_PSV()||(g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_TB))
        && (g_SetupPathInfo.ucSCFmtIn != SC_FMT_IN_NORMAL) && (!g_SetupPathInfo.b3DTo2DEnable) )
        g_SetupPathInfo.wImgSizeOutV = PANEL_HEIGHT / 2;
    else//normal or frame sequential
        g_SetupPathInfo.wImgSizeOutV = PANEL_HEIGHT;

    if(((g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_SBS)||(g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_CB))
        &&(g_SetupPathInfo.ucSCFmtIn != SC_FMT_IN_NORMAL) && (!g_SetupPathInfo.b3DTo2DEnable))
        g_SetupPathInfo.wImgSizeOutH = PANEL_WIDTH / 2;
    else
        g_SetupPathInfo.wImgSizeOutH = PANEL_WIDTH;

#if Enable_Expansion && !PANEL_OUTPUT_FMT_3D_CB // Not support //JisonWideMode
    if(OverScanSetting.Enable && (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL))
    {
        if(((g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_SBS)||(g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_CB))
        &&(g_SetupPathInfo.ucSCFmtIn != SC_FMT_IN_NORMAL) && (!g_SetupPathInfo.b3DTo2DEnable))
        	g_SetupPathInfo.wImgSizeOutH = OverScanSetting.OverScanH/2;
        else
        	g_SetupPathInfo.wImgSizeOutH = OverScanSetting.OverScanH;
		
        if(((IS_FMT_OUT_PSV()||(g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_TB))
        && (g_SetupPathInfo.ucSCFmtIn != SC_FMT_IN_NORMAL) && (!g_SetupPathInfo.b3DTo2DEnable) )
        || g_SetupPathInfo.b2DTo3DEnable )
        	g_SetupPathInfo.wImgSizeOutV = OverScanSetting.OverScanV/2;
        else
        	g_SetupPathInfo.wImgSizeOutV = OverScanSetting.OverScanV;
    }
    else
    {
        if(((g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_SBS)||(g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_CB))
            &&(g_SetupPathInfo.ucSCFmtIn != SC_FMT_IN_NORMAL)&& (!g_SetupPathInfo.b3DTo2DEnable))
            g_SetupPathInfo.wImgSizeOutH = PANEL_WIDTH / 2;
        else
            g_SetupPathInfo.wImgSizeOutH = PANEL_WIDTH;

        g_SetupPathInfo.wImgSizeOutV = PANEL_HEIGHT;
        if( ((IS_FMT_OUT_PSV()||(g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_TB))
            && (g_SetupPathInfo.ucSCFmtIn != SC_FMT_IN_NORMAL) && (!g_SetupPathInfo.b3DTo2DEnable) )
            || g_SetupPathInfo.b2DTo3DEnable)
            g_SetupPathInfo.wImgSizeOutV /= 2;
    }
#endif
//setup wImgSizeOutH & wImgSizeOutV ---end

    MSSCALER_printData("g_SetupPathInfo.ucSCFmtIn(%d)", g_SetupPathInfo.ucSCFmtIn);
    MSSCALER_printData("StandardModeWidth(%d)", StandardModeWidth);
    MSSCALER_printData("StandardModeHeight(%d)", StandardModeHeight);

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
            else if(g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FPI)
            {
                g_SetupPathInfo.wImgSizeInH = 800;
                g_SetupPathInfo.wImgSizeInV = (600-60) / 4;
            }
#endif

            break;
        case SC_FMT_IN_3D_SBSH:
        case SC_FMT_IN_3D_SBSF:
        case SC_FMT_IN_3D_CB:
        case SC_FMT_IN_3D_PI:
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

    if( g_SetupPathInfo.wImgSizeInH < g_SetupPathInfo.wImgSizeOutH )
        g_SetupPathInfo.bScalingUpH = _ENABLE;
    else
        g_SetupPathInfo.bScalingUpH = _DISABLE;


    if(!g_SetupPathInfo.bOverrideSCPathMode)
    {
        if ((( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL )&& IS_INPUT_VFREQ_IN_PANEL_RANGE())
#if ENABLE_FREESYNC
            ||(msAPI_combo_IPGetDDRFlag())
#endif
            )
            g_SetupPathInfo.ucSCPathMode = SC_PATH_MODE_0;
        else

        if(g_SetupPathInfo.bScalingUpH && g_SetupPathInfo.bScalingUpV )
            g_SetupPathInfo.ucSCPathMode = SC_PATH_MODE_1;
        else if (g_SetupPathInfo.bScalingUpH && !g_SetupPathInfo.bScalingUpV )
            g_SetupPathInfo.ucSCPathMode = SC_PATH_MODE_3;
        else if (!g_SetupPathInfo.bScalingUpH && g_SetupPathInfo.bScalingUpV )
            g_SetupPathInfo.ucSCPathMode = SC_PATH_MODE_2;
        else if (!g_SetupPathInfo.bScalingUpH && !g_SetupPathInfo.bScalingUpV )
            g_SetupPathInfo.ucSCPathMode = SC_PATH_MODE_4;




#if ENABLE_TNR
        if( g_SetupPathInfo.bScalingUpH )
            g_SetupPathInfo.ucSCPathMode = SC_PATH_MODE_1;
        else
            g_SetupPathInfo.ucSCPathMode = SC_PATH_MODE_2;
#endif
    }

    if(g_SetupPathInfo.ucSCPathMode != SC_PATH_MODE_0)
        g_SetupPathInfo.bFBMode =TRUE;
    else
        g_SetupPathInfo.bFBMode =FALSE;

#if 0//Enable_Expansion
	IsSupportedAspRatioMode(OverScanSetting.FinalCapWinWidth,OverScanSetting.FinalCapWinHeight);
#endif
	

#if ENABLE_TNR
    if( TNROnFlag )
    {
        if( IsColorspaceRGBInput() )
            g_SetupPathInfo.bMRWR2Y = 1;
        else
            g_SetupPathInfo.bMRWR2Y = 0;

        g_SetupPathInfo.bMRW422 = 1;
    }
    else
#endif
    {
        g_SetupPathInfo.bMRWR2Y = 0;
        g_SetupPathInfo.bMRW422 = 0;
    }

    //setup MRW in/out size
    ucMRWOut3DFmt = ((!(g_SetupPathInfo.b2DTo3DEnable&&((g_SetupPathInfo.ucSCPathMode==SC_PATH_MODE_3)\
                    ||(g_SetupPathInfo.ucSCPathMode==SC_PATH_MODE_4))))
                    &&(g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL||(g_SetupPathInfo.b3DTo2DEnable)))? 1:2;

    switch(g_SetupPathInfo.ucSCPathMode)
    {
        case SC_PATH_MODE_0://frame buffer less
            g_SetupPathInfo.wMRWSizeInH = 0;
            g_SetupPathInfo.wMRWSizeInV = 0;
            g_SetupPathInfo.wMRWSizeOutH = 0;
            g_SetupPathInfo.wMRWSizeOutV = 0;
            break;
        case SC_PATH_MODE_1://MRW->VSP->HSP
            g_SetupPathInfo.wMRWSizeInH = g_SetupPathInfo.wImgSizeInH;
            g_SetupPathInfo.wMRWSizeInV = g_SetupPathInfo.wImgSizeInV;
            break;
        case SC_PATH_MODE_2://HSP->MRW->VSP
            g_SetupPathInfo.wMRWSizeInH = g_SetupPathInfo.wImgSizeOutH;
            g_SetupPathInfo.wMRWSizeInV = g_SetupPathInfo.wImgSizeInV;
            break;
        case SC_PATH_MODE_3://VSP->MRW->HSP
            g_SetupPathInfo.wMRWSizeInH = g_SetupPathInfo.wImgSizeInH;
            g_SetupPathInfo.wMRWSizeInV = g_SetupPathInfo.wImgSizeOutV;
            break;
        case SC_PATH_MODE_4://HSP->VSP->MRW
            g_SetupPathInfo.wMRWSizeInH = g_SetupPathInfo.wImgSizeOutH;
            g_SetupPathInfo.wMRWSizeInV = g_SetupPathInfo.wImgSizeOutV;
            break;
        case SC_PATH_MODE_5://MRW->HSP->VSP
            g_SetupPathInfo.wMRWSizeInH = g_SetupPathInfo.wImgSizeInH;
            g_SetupPathInfo.wMRWSizeInV = g_SetupPathInfo.wImgSizeInV;
            break;
        case SC_PATH_MODE_6://VSP->HSP->MRW
            g_SetupPathInfo.wMRWSizeInH = g_SetupPathInfo.wImgSizeOutH;
            g_SetupPathInfo.wMRWSizeInV = g_SetupPathInfo.wImgSizeOutV;
            break;
    }
    g_SetupPathInfo.wMRWSizeInV = ucMRWOut3DFmt*g_SetupPathInfo.wMRWSizeInV;;
    g_SetupPathInfo.wMRWSizeOutH = g_SetupPathInfo.wMRWSizeInH;
    g_SetupPathInfo.wMRWSizeOutV = g_SetupPathInfo.wMRWSizeInV;
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
                msWrite2Byte(SCD_60, FIFO_VSP_HSP_FIFO);
                msWriteByteMask(SCD_62, 0x00, BIT0|BIT1|BIT2|BIT3|BIT4);
            }
            else //HSP->VSP
            {
                msWrite2Byte(SCD_60, FIFO_HSP_VSP_FIFO);
                msWriteByteMask(SCD_62, 0x00, BIT0|BIT1|BIT2|BIT3|BIT4);
            }
            msWriteByteMask(SC6_02,BIT1,BIT2|BIT1);
            break;
        case SC_PATH_MODE_1://MRW->VSP->HSP
            msWrite2Byte(SCD_60, FIFO_MRW_VSP_HSP_FIFO);
            if(g_SetupPathInfo.bFBMode)
                msWriteByteMask(SCD_62, BIT0|BIT1, BIT0|BIT1|BIT2|BIT3|BIT4);
            else
                msWriteByteMask(SCD_62, 0x00, BIT0|BIT1|BIT2|BIT3|BIT4);
            msWriteByteMask(SC6_F2, IP2MRW2VSP,0x0F);
            msWriteByteMask(SC6_02,BIT2,BIT2|BIT1);
            break;
        case SC_PATH_MODE_2://HSP->MRW->VSP
            msWrite2Byte(SCD_60, FIFO_HSP_MRW_VSP_FIFO);
            if(g_SetupPathInfo.bFBMode)
                msWriteByteMask(SCD_62, BIT1, BIT0|BIT1|BIT2|BIT3|BIT4);
            else
                msWriteByteMask(SCD_62, 0x00, BIT0|BIT1|BIT2|BIT3|BIT4);
            msWriteByteMask(SC6_F2, HSP2MRW2VSP, 0x0F);
            msWriteByteMask(SC6_02,BIT2,BIT2|BIT1);
            break;
        case SC_PATH_MODE_3://VSP->MRW->HSP
            msWrite2Byte(SCD_60, FIFO_VSP_MRW_HSP_FIFO);
            if(g_SetupPathInfo.bFBMode)
                msWriteByteMask(SCD_62, BIT0|BIT3, BIT0|BIT1|BIT2|BIT3|BIT4);
            else
                msWriteByteMask(SCD_62, 0x00, BIT0|BIT1|BIT2|BIT3|BIT4);
            msWriteByteMask(SC6_F2, VSP2MRW2HSP, 0x0F);
            msWriteByteMask(SC6_02,BIT2,BIT2|BIT1);
            break;
        case SC_PATH_MODE_4://HSP->VSP->MRW
            msWrite2Byte(SCD_60, FIFO_HSP_VSP_MRW_FIFO);
            if(g_SetupPathInfo.bFBMode)
                msWriteByteMask(SCD_62, BIT3, BIT0|BIT1|BIT2|BIT3|BIT4);
            else
                msWriteByteMask(SCD_62, 0x00, BIT0|BIT1|BIT2|BIT3|BIT4);
            msWriteByteMask(SC6_F2, VSP2MRW2OP1, 0x0F);
            msWriteByteMask(SC6_02,BIT2,BIT2|BIT1);
            break;
        case SC_PATH_MODE_5://MRW->HSP->VSP
            msWrite2Byte(SCD_60, FIFO_MRW_HSP_VSP_FIFO);
            if(g_SetupPathInfo.bFBMode)
                msWriteByteMask(SCD_62, BIT0|BIT1, BIT0|BIT1|BIT2|BIT3|BIT4);
            else
                msWriteByteMask(SCD_62, 0x00, BIT0|BIT1|BIT2|BIT3|BIT4);
            msWriteByteMask(SC6_F2, IP2MRW2HSP, 0x0F);
            msWriteByteMask(SC6_02,BIT2,BIT2|BIT1);
        break;
        case SC_PATH_MODE_6://VSP->HSP->MRW
            msWrite2Byte(SCD_60, FIFO_VSP_HSP_MRW_FIFO);
            if(g_SetupPathInfo.bFBMode)
                msWriteByteMask(SCD_62, BIT3, BIT0|BIT1|BIT2|BIT3|BIT4);
            else
                msWriteByteMask(SCD_62, 0x00, BIT0|BIT1|BIT2|BIT3|BIT4);
            msWriteByteMask(SC6_F2, HSP2MRW2OP1, 0x0F);
            msWriteByteMask(SC6_02,BIT2,BIT2|BIT1);
            break;
    }
#endif
}

static void msSetupPathIP()
{
    XDATA BYTE ucBlanking, ucFraction, ucRstOffset, ucDlyLine;
    XDATA WORD uwVDE, uwOffset;
    XDATA WORD uwIHDE, uwOHDE, uwIHtt, uwOHtt, uwIVtt, uwOVtt;
    XDATA WORD uwTita, uwTota;
    BYTE u83DTo2D_EN = 0;

    if(g_SetupPathInfo.b3DTo2DEnable)
        u83DTo2D_EN = BIT5;
    else
        u83DTo2D_EN = 0;

    uwIHDE = msRead2Byte(SC0_0B) & SC_MASK_H;
    uwOHDE = PANEL_WIDTH;
    uwIHtt = mSTar_GetInputHTotal();
    uwOHtt = PanelHTotal;
    uwIVtt = SC0_READ_VTOTAL();
    uwOVtt = PanelVTotal;
    uwTita = ((DWORD)uwIHDE * 1000) / uwIHtt;
    uwTota = ((DWORD)uwOHDE * 1000) / uwOHtt * uwIVtt / uwOVtt;
    if(uwTita > uwTota)
        ucDlyLine = 3;
    else
        ucDlyLine = 2;
#if 0
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

#if ENABLE_TNR
    if( TNROnFlag
     && (   (g_SetupPathInfo.ucSCPathMode==SC_PATH_MODE_3)
         || (g_SetupPathInfo.ucSCPathMode==SC_PATH_MODE_4)
         || (g_SetupPathInfo.ucSCPathMode==SC_PATH_MODE_6) )
    )
        ucDlyLine++;
#endif
#endif
    DB_Mode(eDB_NORMAL_MODE);

    if( !g_SetupPathInfo.bScalingUpV || !g_SetupPathInfo.bScalingUpH )
        DB_WBMask(SC0_39, 0, _BIT6);
    else
        DB_WBMask(SC0_39, _BIT6, _BIT6);

    DB_WBMask(SCD_2E, BIT0, BIT0);
    DB_WB(SCD_68, 0x06);

#if 0
#if ENABLE_R2_2DTO3D && LGE_DEMO
    if (UserPref2Dto3D)
        DB_WBMask(SC0_90, (ucDlyLine+1)<<4, _BIT6|_BIT5|_BIT4); //delay line
    else
#endif
        DB_WBMask(SC0_90, (ucDlyLine)<<4, _BIT6|_BIT5|_BIT4); //delay line
#endif
    DB_WBMask(SC0_0F, (SC_LOCK_LINE), 0x0F); //Ylock line

    ucRstOffset = SC_LOCK_LINE - ucDlyLine;
    uwVDE = g_SetupPathInfo.wImgSizeInV;
    switch(g_SetupPathInfo.ucSCFmtIn)
    {
        case SC_FMT_IN_3D_FP:
        case SC_FMT_IN_3D_FA:
#ifdef FPGA
            ucBlanking = 31;
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
#ifdef FPGA
            ucBlanking = 20;
            ucFraction = 0;
#else
            ucBlanking = (StandardModeVTotal - StandardModeHeight)/4;
            ucFraction = ((StandardModeVTotal - StandardModeHeight)/2) & _BIT0;
#endif
            DB_W2BMask(REG_2E60, uwVDE*3 + 3*ucBlanking + 2*ucFraction + 1 + BIT12, 0x1FFF); //start
            uwOffset = uwVDE*3 + 2*ucBlanking + ucFraction + 1;
            DB_W2BMask(REG_2E62, uwOffset, 0x0FFF); //end
            DB_W2BMask(REG_2E6C, uwOffset + ucRstOffset + BIT12, 0x1FFF); // frame reset setting

            DB_W2BMask(REG_2E64, uwVDE*2 + 2*ucBlanking + ucFraction + 1 + BIT12, 0x1FFF); //start
            uwOffset = uwVDE*2 + ucBlanking + ucFraction + 1;
            DB_W2BMask(REG_2E66, uwOffset, 0x0FFF); //end
            DB_W2BMask(REG_2E6E, uwOffset + ucRstOffset + BIT12, 0x1FFF); // frame reset setting

            DB_W2BMask(REG_2E68, uwVDE + ucBlanking + ucFraction + 1 + BIT12, 0x1FFF); //start
            uwOffset = uwVDE + 1;
            DB_W2BMask(REG_2E6A, uwOffset, 0x0FFF); //end
            DB_W2BMask(REG_2E70, uwOffset + ucRstOffset + BIT12, 0x1FFF); // frame reset setting
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

    switch(g_SetupPathInfo.ucSCFmtIn)//IP LR gen
    {
        case SC_FMT_IN_3D_PI:
            DB_WBMask(SC0_A6,BIT6|BIT7,BIT2|BIT4|BIT5|BIT6|BIT7);
            DB_WBMask(SC0_A8,u83DTo2D_EN,BIT4|BIT5|BIT6|BIT7);
            DB_W2BMask(REG_2E78, 0x02,0x1FFF);
            DB_W2B(REG_2E74, 0x00);
        break;
        case SC_FMT_IN_3D_CB:
            DB_WBMask(SC0_A6,BIT2|BIT5|BIT7,BIT2|BIT4|BIT5|BIT6|BIT7);
            DB_WBMask(SC0_A8,u83DTo2D_EN,BIT4|BIT5|BIT6|BIT7);
            DB_W2BMask(REG_2E78, 0x02,0x1FFF);
            DB_W2B(REG_2E74, 0x00);
        break;
        case SC_FMT_IN_3D_LA:
            DB_WBMask(SC0_A6,BIT4|BIT5,BIT2|BIT4|BIT5|BIT6|BIT7);
            DB_WBMask(SC0_A8,u83DTo2D_EN,BIT4|BIT5|BIT6|BIT7);
            DB_W2B(REG_2E74, 0x00);
            DB_W2B(REG_2E78, 0x1FFF);
        break;
        case SC_FMT_IN_3D_TB:
            DB_WBMask(SC0_A6,0x00,BIT2|BIT4|BIT5|BIT6|BIT7);
            DB_WBMask(SC0_A8,BIT6|BIT7|u83DTo2D_EN,BIT4|BIT5|BIT6|BIT7);
            DB_W2B(REG_2E74, g_SetupPathInfo.wImgSizeInV+1);
            DB_W2B(REG_2E78, 0x1FFF);
        break;
        case SC_FMT_IN_3D_SBSH:
        case SC_FMT_IN_3D_SBSF:
            DB_WBMask(SC0_A6,BIT7,BIT2|BIT4|BIT5|BIT6|BIT7);
            DB_WBMask(SC0_A8,BIT4|u83DTo2D_EN,BIT4|BIT5|BIT6|BIT7);
            DB_W2B(REG_2E74, 0x00);
            DB_W2BMask(REG_2E78, g_SetupPathInfo.wImgSizeInH+1,0x1FFF);
        break;
        case SC_FMT_IN_3D_PF:
            DB_WBMask(SC0_A6,0x00,BIT2|BIT4|BIT5|BIT6|BIT7);
            DB_WBMask(SC0_A8,u83DTo2D_EN,BIT4|BIT5|BIT6|BIT7);
            DB_W2B(REG_2E74, 0x00);
            DB_W2B(REG_2E78, 0x1FFF);

        break;
        default://2D
            DB_WBMask(SC0_A6,0x00,BIT2|BIT4|BIT5|BIT6|BIT7);
            DB_WBMask(SC0_A8,0x00,BIT4|BIT5|BIT6|BIT7);
            DB_W2B(REG_2E74, 0x00);
            DB_W2B(REG_2E78, 0x1FFF);
        break;
    }

#if ENABLE_SW_DOUBLE_BUFFER
    msSWDBWriteToRegister();
#endif

}
static void msHSPDataFormat(BYTE ucSCFmtIn)
{
    switch(ucSCFmtIn)//HSP 3D data format
    {
        case SC_FMT_IN_NORMAL:
            msWriteByteMask(SC0_B5, 0, BIT4|BIT5|BIT6);
        break;
        case SC_FMT_IN_3D_SBSH:
        case SC_FMT_IN_3D_SBSF:
             msWriteByteMask(SC0_B5, BIT5, BIT4|BIT5|BIT6);
        break;
        case SC_FMT_IN_3D_CB:
            msWriteByteMask(SC0_B5, BIT4|BIT5, BIT4|BIT5|BIT6);//CB
        break;
        case SC_FMT_IN_3D_PI:
                msWriteByteMask(SC0_B5, BIT4, BIT4|BIT5|BIT6);//PI
        break;
        case SC_FMT_IN_3D_PF:
        case SC_FMT_IN_3D_FP:
        case SC_FMT_IN_3D_FPI:
        case SC_FMT_IN_3D_TB:
        case SC_FMT_IN_3D_LA:
        case SC_FMT_IN_3D_FA:
            msWriteByteMask(SC0_B5,BIT6, BIT4|BIT5|BIT6);//Other 3D
        break;
    }
}
static void msSetupPathHSP()
{
    switch(g_SetupPathInfo.ucSCPathMode)
    {
        //behind MRW
        case SC_PATH_MODE_1:
        case SC_PATH_MODE_3:
        case SC_PATH_MODE_5:
            if(g_SetupPathInfo.b3DTo2DEnable)
                msWrite2ByteMask(SC0_C8,g_SetupPathInfo.wImgSizeOutH,0x1FFF);
            else if((g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_SBS)||(g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_CB))
                msWrite2ByteMask(SC0_C8,g_SetupPathInfo.wImgSizeOutH*2,0x1FFF);
            else
                msWrite2ByteMask(SC0_C8,g_SetupPathInfo.wImgSizeOutH,0x1FFF);
            if(g_SetupPathInfo.ucSCFmtIn != SC_FMT_IN_NORMAL)
            {
                if(g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_FS)
                    msHSPDataFormat(SC_FMT_IN_3D_PF);
                else if(g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_TB)
                    msHSPDataFormat(SC_FMT_IN_3D_TB);
                else if(g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_SBS)
                    msHSPDataFormat(SC_FMT_IN_3D_SBSH);
                else if(g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_CB)
                {
                    if(g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_1)
                        msHSPDataFormat(SC_FMT_IN_3D_CB);
                    else
                        msHSPDataFormat(SC_FMT_IN_3D_SBSH);
                }
                else if(g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_PSV)
                msHSPDataFormat(SC_FMT_IN_3D_LA);
            }
            else
                msHSPDataFormat(g_SetupPathInfo.ucSCFmtIn);//2D
            break;
        //in front of MRW
        case SC_PATH_MODE_0:
        case SC_PATH_MODE_2:
        case SC_PATH_MODE_4:
        case SC_PATH_MODE_6:

            if(g_SetupPathInfo.b3DTo2DEnable)
                msWrite2ByteMask(SC0_C8,g_SetupPathInfo.wImgSizeOutH,0x1FFF);
            else if((g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_SBSH)||(g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_SBSF)
                ||(g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_CB)||(g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_PI))
                msWrite2ByteMask(SC0_C8,2*g_SetupPathInfo.wImgSizeOutH,0x1FFF);
            else
                msWrite2ByteMask(SC0_C8,g_SetupPathInfo.wImgSizeOutH,0x1FFF);
            if(IS_FMT_IN_CB_PI()
            &&((g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_2)||(g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_6)))
                msHSPDataFormat(SC_FMT_IN_3D_SBSH);
            else
            msHSPDataFormat(g_SetupPathInfo.ucSCFmtIn);
            break;
    }
    if(g_SetupPathInfo.b3DTo2DEnable)
        msHSPDataFormat(SC_FMT_IN_NORMAL);
}

static void msSetupPathHMIR()
{
    BYTE H_MIR_EN=0;

    if(g_SetupPathInfo.bHMirrorMode)
        H_MIR_EN = BIT4;
    else
        H_MIR_EN = 0;

    switch(g_SetupPathInfo.ucSCPathMode)
    {
        case SC_PATH_MODE_0:
        case SC_PATH_MODE_1:
        case SC_PATH_MODE_2:
        case SC_PATH_MODE_5:
        {
            if( g_SetupPathInfo.wImgSizeInH > 1920 )
                msWriteByteMask(SCD_66,BIT4,BIT4|BIT5);//hmir position, [0]:pre-hvsp, [1]:post-hvsp,[5]:hmir bypass mode
            else
                msWriteByteMask(SCD_66,0x00,BIT4|BIT5);//hmir position, [0]:pre-hvsp, [1]:post-hvsp,[5]:hmir bypass mode
            msWriteByteMask(SCD_62,0x00,BIT3);//hmir select source,0:from IP/OP,1:from MRW

            switch(g_SetupPathInfo.ucSCFmtIn)
            {
                case SC_FMT_IN_3D_CB:
                case SC_FMT_IN_3D_PI:
                case SC_FMT_IN_3D_SBSH:
                case SC_FMT_IN_3D_SBSF:
                    msWriteByteMask(SCD_80,BIT6|H_MIR_EN,BIT4|BIT5|BIT6|BIT7);//Output data format
                    //transfer CB and PI to SBS because MRW can't deal with these two 3d format input
                    break;
                case SC_FMT_IN_3D_PF:
                case SC_FMT_IN_3D_FP:
                case SC_FMT_IN_3D_FPI:
                case SC_FMT_IN_3D_TB:
                case SC_FMT_IN_3D_LA:
                case SC_FMT_IN_3D_FA:
                    msWriteByteMask(SCD_80,BIT7|H_MIR_EN,BIT4|BIT5|BIT6|BIT7);//Output data format
                    break;
                break;
                default://normal
                    msWriteByteMask(SCD_80,H_MIR_EN,BIT4|BIT5|BIT6|BIT7);//Output data format
                    break;
            }
        }
        break;
        case SC_PATH_MODE_3:
        case SC_PATH_MODE_4:
        case SC_PATH_MODE_6:
        {
            msWriteByteMask(SCD_66,BIT4,BIT4|BIT5);//hmir position, [0]:pre-hvsp, [1]:post-hvsp,[5]:hmir bypass mode
            msWriteByteMask(SCD_62,BIT3,BIT3);//hmir select source,0:from IP/OP,1:from MRW
            if((g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_CB)
                &&(g_SetupPathInfo.ucSCFmtIn!=SC_FMT_IN_NORMAL)&&(!g_SetupPathInfo.b3DTo2DEnable))
                msWriteByteMask(SCD_80,BIT5|BIT6|H_MIR_EN,BIT4|BIT5|BIT6|BIT7);//Output data format
            else
                msWriteByteMask(SCD_80,H_MIR_EN,BIT4|BIT5|BIT6|BIT7);//Output data format
        }
        break;
    }
    if(g_SetupPathInfo.b3DTo2DEnable)
        msWriteByteMask(SCD_80,H_MIR_EN,BIT4|BIT5|BIT6|BIT7);//Output data format
}

static void msSetupPathRSP()
{
    msWriteByteMask(SCD_60,0x00,BIT3);//RSP position, [0]:pre-hvsp, [1]:post-hvsp
    msWriteByteMask(SCD_62,0x00,BIT2);//RSP select source,0:from IP/OP,1:from MRW
    msWriteByteMask(SCD_67,BIT0,BIT0);//RSP bypass mode
    switch(g_SetupPathInfo.ucSCPathMode)
    {
        case SC_FMT_IN_3D_CB:
        case SC_FMT_IN_3D_PI:
            msWriteByteMask(SCD_72,BIT7,BIT7);//Horizontal Resolution Down-conversion Enable in hsp resampling.
            msWriteByteMask(SCD_73,BIT0,BIT0|BIT1);//[0]:bilinear,[1]:y only mode, if both bit0 and bit1 are 0, 4 tap mode.
            msWriteByte(SCD_70,0x80);//Initial H scaling factor of L frame
            msWriteByte(SCD_71,0x00);//Initial H scaling factor of R frame
            if(g_SetupPathInfo.ucSCPathMode==SC_FMT_IN_3D_CB)
                msWriteByteMask(SCD_72,BIT0|BIT1,BIT0|BIT1|BIT2);//Data format in RSP
            else
                msWriteByteMask(SCD_72,BIT0,BIT0|BIT1|BIT2);
            msWriteByteMask(SCD_72,0x00,BIT4|BIT5);//H scaling factor start position of L,R frame
            msWriteByteMask(SCD_72,0x00,BIT7);//Horizontal Resolution Down-conversion Enable in hsp resampling.
            msWriteByteMask(SCD_73,BIT0,BIT0|BIT1);//[8]Bilinear mode,[9]Only filter Y
            msWrite2ByteMask(SCD_74,g_SetupPathInfo.wImgSizeInH,0x3FFF);//HSP resampling Output horizontal resolution
        default:
            msWriteByteMask(SCD_67,BIT0,BIT0);//RSP bypass mode
            break;
    }

}

static void msVSPDataFormat(BYTE ucSCFmtIn)
{
    WORD V4TAP_SCALING = 0;

#if EN_V4TAP_SCALING
    V4TAP_SCALING = 0;
#else
    V4TAP_SCALING = BIT15;
#endif
    switch(ucSCFmtIn)//VSP 3D data format
    {
        case SC_FMT_IN_NORMAL:
            msWrite2ByteMask(SC0_5A, V4TAP_SCALING, BIT6|BIT7|BIT8|BIT14|BIT15);
        break;
        case SC_FMT_IN_3D_TB:
        case SC_FMT_IN_3D_FP:
             msWrite2ByteMask(SC0_5A, BIT6|V4TAP_SCALING, BIT6|BIT7|BIT8|BIT14|BIT15);
        break;
        case SC_FMT_IN_3D_LA:
            msWrite2ByteMask(SC0_5A, BIT7|V4TAP_SCALING, BIT6|BIT7|BIT8|BIT14|BIT15);
        break;
        case SC_FMT_IN_3D_SBSH:
        case SC_FMT_IN_3D_SBSF:
             msWrite2ByteMask(SC0_5A, BIT8|V4TAP_SCALING, BIT6|BIT7|BIT8|BIT14|BIT15);
        break;
        case SC_FMT_IN_3D_PI:
        case SC_FMT_IN_3D_CB:
        {
            if(((g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_CB)||(g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_SBS))
            &&((g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_3)||(g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_4)))
            {
                msWriteByteMask(SC0_C1,BIT6|BIT7,BIT5|BIT6|BIT7);//transfer CB to SBS format,[7]:Invert source field.
                if(g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_CB)
                    msWrite2ByteMask(SC0_5A, BIT6|BIT8|V4TAP_SCALING, BIT6|BIT7|BIT8|BIT14|BIT15);
                else
                    msWrite2ByteMask(SC0_5A, BIT6|BIT7|V4TAP_SCALING, BIT6|BIT7|BIT8|BIT14|BIT15);
            }
            else
                msWrite2ByteMask(SC0_5A, BIT8|V4TAP_SCALING, BIT6|BIT7|BIT8|BIT14|BIT15);
        }
        break;
        case SC_FMT_IN_3D_PF:
                msWrite2ByteMask(SC0_5A, BIT7|BIT8|V4TAP_SCALING, BIT6|BIT7|BIT8|BIT14|BIT15);
        break;
        case SC_FMT_IN_3D_FPI:
            msWrite2ByteMask(SC0_5A, BIT6|BIT7|BIT8|V4TAP_SCALING, BIT6|BIT7|BIT8|BIT14|BIT15);
        break;
    }
}
/*
[0]:1:2D mode,0:3D mode
[1]:Input line interleave
[2]:Input top and bottom
[3]:Output line interleave
[4]:Output top and bottom
[7]:Input frame packing interlace
*/
static void msMRWDataFormat(BYTE ucSCFmtIn, BYTE ucSCFmtOut)
{
    BYTE _3D_OUTPUT_FORMAT = 0;

    msWriteByteMask(SC6_27,0x00,BIT4|BIT3|BIT2|BIT1|BIT0);
    if(ucSCFmtOut == SC_FMT_OUT_3D_PSV)
        _3D_OUTPUT_FORMAT = BIT3;
    else if(ucSCFmtOut == SC_FMT_OUT_3D_TB)
        _3D_OUTPUT_FORMAT = BIT4;
    else if(ucSCFmtOut == SC_FMT_OUT_3D_SBS)
        _3D_OUTPUT_FORMAT = BIT3;
    else if(ucSCFmtOut == SC_FMT_OUT_3D_CB)
        _3D_OUTPUT_FORMAT = BIT3;
    else//normal or frame sequential
        _3D_OUTPUT_FORMAT = 0;

    switch(ucSCFmtIn)//MRW 3D data format
    {
        case SC_FMT_IN_NORMAL:
            msWriteByteMask(SC6_28, BIT0, BIT0|BIT1|BIT2|BIT3|BIT4|BIT7);
        break;
        case SC_FMT_IN_3D_TB:
             msWriteByteMask(SC6_28, BIT2|_3D_OUTPUT_FORMAT, BIT0|BIT1|BIT2|BIT3|BIT4|BIT7);
        break;
        case SC_FMT_IN_3D_LA:
            msWriteByteMask(SC6_28, BIT1|_3D_OUTPUT_FORMAT, BIT0|BIT1|BIT2|BIT3|BIT4|BIT7);
        break;
        case SC_FMT_IN_3D_PI:
            msWriteByteMask(SC6_28, BIT1|_3D_OUTPUT_FORMAT, BIT0|BIT1|BIT2|BIT3|BIT4|BIT7);
        break;
        case SC_FMT_IN_3D_SBSH:
        case SC_FMT_IN_3D_SBSF:
             msWriteByteMask(SC6_28, BIT1|_3D_OUTPUT_FORMAT, BIT0|BIT1|BIT2|BIT3|BIT4|BIT7);
        break;
        case SC_FMT_IN_3D_CB:
            msWriteByteMask(SC6_28, BIT1|_3D_OUTPUT_FORMAT, BIT0|BIT1|BIT2|BIT3|BIT4|BIT7);
        break;
        case SC_FMT_IN_3D_PF:
            if(g_SetupPathInfo.ucSCFmtOut != SC_FMT_OUT_3D_FS)
            {
                msWriteByteMask(SC6_27,BIT4|BIT3|BIT1,BIT4|BIT3|BIT2|BIT1|BIT0);
                msWriteByteMask(SC6_28,BIT2|_3D_OUTPUT_FORMAT, BIT0|BIT1|BIT2|BIT3|BIT4|BIT7);
            }
            else
            {
                msWriteByteMask(SC6_27,0x00,BIT4|BIT3|BIT2|BIT1|BIT0);
                 msWriteByteMask(SC6_28,_3D_OUTPUT_FORMAT, BIT0|BIT1|BIT2|BIT3|BIT4|BIT7);
            }
        break;
        case SC_FMT_IN_3D_FP:
            msWriteByteMask(SC6_28, BIT2|_3D_OUTPUT_FORMAT, BIT0|BIT1|BIT2|BIT3|BIT4|BIT7);
        break;
        case SC_FMT_IN_3D_FPI:
            msWriteByteMask(SC6_1A,BIT3,BIT3);
            msWriteByteMask(SC6_27,BIT4|BIT3|BIT1,BIT4|BIT3|BIT2|BIT1|BIT0);
            msWriteByteMask(SC6_28, BIT2|_3D_OUTPUT_FORMAT, BIT0|BIT1|BIT2|BIT3|BIT4|BIT7);
        break;
    }
}
static void msSetupPathVSP()
{

    switch(g_SetupPathInfo.ucSCPathMode)
    {
        case SC_PATH_MODE_0:
            msWrite2ByteMask(SC0_C0,g_SetupPathInfo.wImgSizeInV,0x0FFF);
            if(g_SetupPathInfo.b2DTo3DEnable)
                msWrite2ByteMask(SC0_C4,g_SetupPathInfo.wImgSizeOutV*2,0x0FFF);
            else
            msWrite2ByteMask(SC0_C4,g_SetupPathInfo.wImgSizeOutV,0x0FFF);
            msWriteByteMask(SC0_C1,BIT7,BIT5|BIT6|BIT7);
            msVSPDataFormat(g_SetupPathInfo.ucSCFmtIn);
            break;
        case SC_PATH_MODE_3://before MRW
        case SC_PATH_MODE_4:
        case SC_PATH_MODE_6:
            if(g_SetupPathInfo.b3DTo2DEnable)
            {
                msWrite2ByteMask(SC0_C0,g_SetupPathInfo.wImgSizeInV,0x0FFF);
                msWrite2ByteMask(SC0_C4,g_SetupPathInfo.wImgSizeOutV,0x0FFF);
                msVSPDataFormat(SC_FMT_IN_NORMAL);
            }
            else if(g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_TB || g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_LA)
            {
                msWrite2ByteMask(SC0_C0,2*g_SetupPathInfo.wImgSizeInV,0x0FFF);
                msWrite2ByteMask(SC0_C4,2*g_SetupPathInfo.wImgSizeOutV,0x0FFF);
                msVSPDataFormat(g_SetupPathInfo.ucSCFmtIn);
            }
            else if(g_SetupPathInfo.b2DTo3DEnable)
            {
                msWrite2ByteMask(SC0_C0,g_SetupPathInfo.wImgSizeInV,0x0FFF);
                msWrite2ByteMask(SC0_C4,g_SetupPathInfo.wImgSizeOutV*2,0x0FFF);
                msVSPDataFormat(g_SetupPathInfo.ucSCFmtIn);
            }
            else
            {
                msWrite2ByteMask(SC0_C0,g_SetupPathInfo.wImgSizeInV,0x0FFF);
                msWrite2ByteMask(SC0_C4,g_SetupPathInfo.wImgSizeOutV,0x0FFF);
                msVSPDataFormat(g_SetupPathInfo.ucSCFmtIn);
            }

            if(IS_FMT_IN_CB_PI()&&(!g_SetupPathInfo.b3DTo2DEnable))
                msWriteByteMask(SC0_C1,BIT6|BIT7,BIT5|BIT6|BIT7);//[7]:Invert source field.
            else
                msWriteByteMask(SC0_C1,BIT7,BIT5|BIT6|BIT7);//disable 3D format convert,[7]:Invert source field.
            break;
        case SC_PATH_MODE_1://after MRW
        case SC_PATH_MODE_2:
        case SC_PATH_MODE_5:
            if(g_SetupPathInfo.b3DTo2DEnable)
            {
                msWrite2ByteMask(SC0_C0,g_SetupPathInfo.wImgSizeInV,0x0FFF);
                msWrite2ByteMask(SC0_C4,g_SetupPathInfo.wImgSizeOutV,0x0FFF);
                msVSPDataFormat(SC_FMT_IN_NORMAL);
            }
            else if(g_SetupPathInfo.ucSCFmtIn != SC_FMT_IN_NORMAL)
            {
                if(g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_TB || g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_PSV)
                {
                    msWrite2ByteMask(SC0_C0,g_SetupPathInfo.wImgSizeInV*2,0x0FFF);
                    msWrite2ByteMask(SC0_C4,g_SetupPathInfo.wImgSizeOutV*2,0x0FFF);
                }
                else
                {
                    msWrite2ByteMask(SC0_C0,g_SetupPathInfo.wImgSizeInV,0x0FFF);
                    msWrite2ByteMask(SC0_C4,g_SetupPathInfo.wImgSizeOutV,0x0FFF);
                }
                //set up 3D format
                if(g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_FS)
                    msVSPDataFormat(SC_FMT_IN_3D_PF);
                else if(g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_TB)
                    msVSPDataFormat(SC_FMT_IN_3D_TB);
                else if(g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_SBS)
                    msVSPDataFormat(SC_FMT_IN_3D_SBSH);
                else if(g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_CB)
                    msVSPDataFormat(SC_FMT_IN_3D_SBSH);
                else if(g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_PSV)
                    msVSPDataFormat(SC_FMT_IN_3D_LA);
            }
            else if(g_SetupPathInfo.b2DTo3DEnable)
            {
                msWrite2ByteMask(SC0_C0,g_SetupPathInfo.wImgSizeInV,0x0FFF);
                msWrite2ByteMask(SC0_C4,g_SetupPathInfo.wImgSizeOutV*2,0x0FFF);
                msVSPDataFormat(g_SetupPathInfo.ucSCFmtIn);
            }
            else
            {
                msWrite2ByteMask(SC0_C0,g_SetupPathInfo.wImgSizeInV,0x0FFF);
                msWrite2ByteMask(SC0_C4,g_SetupPathInfo.wImgSizeOutV,0x0FFF);
                msVSPDataFormat(g_SetupPathInfo.ucSCFmtIn);
            }

            if((g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_CB)&&!(g_SetupPathInfo.b3DTo2DEnable))
                msWriteByteMask(SC0_C1,BIT5|BIT6|BIT7,BIT5|BIT6|BIT7);//[7]:Invert source field.
            else
                msWriteByteMask(SC0_C1,BIT7,BIT5|BIT6|BIT7);//disable 3D format convert
            break;
    }

    if(CURRENT_SOURCE_IS_INTERLACE_MODE()||(g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FPI))
        msWriteByteMask(SC7_1B, 0x00, BIT5|BIT6);
    else
        msWriteByteMask(SC7_1B, BIT5|BIT6, BIT5|BIT6);//[6]:Force Feild [5]:Even/Odd Field Selection

    if(g_SetupPathInfo.b2DTo3DEnable)
        msWriteByteMask(SC0_C3,BIT7,BIT7);//repeat 2 lines for t3d use
    else
        msWriteByteMask(SC0_C3,0x00,BIT7);
    #if ENABLE_TNR
        msWriteByteMask(SC2_E0, 0x00 ,BIT3);//disable SPF bypass mode
    #else
        msWriteByteMask(SC2_E0, BIT3 ,BIT3);//enable SPF bypass mode
    #endif

    #if 0   //Junior uses force field to replace this patch
    if( g_SetupPathInfo.bFBMode && g_SetupPathInfo.bScalingUpV )
        msWriteByteMask(SC0_5B, _BIT5, _BIT5);
    else
        msWriteByteMask(SC0_5B, 0, _BIT5);
    #endif

}

static void msSetupPathMRWBaseRD()
{
    msWrite2ByteMask(SC6_D4,g_SetupPathInfo.wMRWSizeOutH,0x1FFF);  //MRW read H size
    msWrite2ByteMask(SC6_D6,g_SetupPathInfo.wMRWSizeOutV,0x0FFF);      //MRW read V size

//[0]: the starting address would be pass at vsync
//[1]: the starting address would be pass when vcnt(SC6_18[11:0]) is reached
    msWrite2Byte(SC6_18,(g_SetupPathInfo.wImgSizeInV/2+10)|BIT15);
#if 0
    if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_TB)
     && !g_SetupPathInfo.bPsvPnlOutLRInvert )
        msWrite2Byte(SC6_18,(g_SetupPathInfo.wMRWSizeInV/2 + 10)|BIT15);//when reach vcnt, base address would latch from ipm to opm
    else if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_TB)
              && g_SetupPathInfo.bPsvPnlOutLRInvert )
        msWrite2Byte(SC6_18,(g_SetupPathInfo.wMRWSizeInV/2 - 10)|BIT15);
    else if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FP || g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FPI)
             && g_SetupPathInfo.bPsvPnlOutLRInvert )
        msWrite2Byte(SC6_18,(g_SetupPathInfo.wMRWSizeInV - 10)|BIT15);
    else
        msWrite2Byte(SC6_18,10|BIT15);
#endif

}

static void msSetupPathMRWBaseWR()
{
    // set Frame buffer
    msWrite3Byte(SC6_C4,(DWORD)MIU_FB_ADDR_START>>2);
    msWrite3Byte(SC6_C0,(DWORD)MIU_FB_ADDR_END>>2);

    //set motion buffer
#if ENABLE_TNR
    msWrite3Byte(SC6_CC,(DWORD)MIU_MOTION_ADDR_START>>2);
    msWrite3Byte(SC6_C8,(DWORD)MIU_MOTION_ADDR_END>>2);
#endif

    msWrite2ByteMask(SC6_D0,g_SetupPathInfo.wMRWSizeInH,0x1FFF);
    msWrite2ByteMask(SC6_D2,g_SetupPathInfo.wMRWSizeInV,0x0FFF);

}
#if ENABLE_TNR
void msTNROnOff(Bool bEnable)
{
    BYTE vsyncTime;

    if(bEnable)
    {
        MSSCALER_printMsg("  >>>>>>msDNROn");
        //msWriteByteMask(SCC_42, BIT0|BIT1, BIT0|BIT1);//DNR core enable
        msWriteByteMask(SC6_29, BIT2, BIT2);//ipr enable
        msWriteByteMask(SC6_1A, BIT2, BIT2);//ipr mclk enable

        vsyncTime = GetVSyncTime();
        ForceDelay1ms(2*vsyncTime); //need to delay 1~2 frame till IPR data is ready , then enable DNR
         mStar_WaitForDataBlanking();
        msWriteByteMask(SCC_42, BIT0|BIT1, BIT0|BIT1);//DNR core enable
        msWriteByteMask(SCC_44, BIT0|BIT1, BIT0|BIT1);//[0]:pre-SNR enable, [1]:pre-SNR motion adaptive enable
        msWriteByteMask(SCC_34, BIT1|BIT2, BIT0|BIT1|BIT2);
        msWriteByteMask(SC6_10, BIT1, BIT1);//F2 delta Y only do for Y channel.
        msWriteByteMask(SC6_14, BIT2, BIT2);
        if( IsColorspaceRGBInput() )
        {
            g_SetupPathInfo.bMRWR2Y = 1;
        }
        else
        {
            g_SetupPathInfo.bMRWR2Y = 0;
        }
    }
    else
    {
        MSSCALER_printMsg("  >>>>>>msDNROff");
        mStar_WaitForDataBlanking();
        msWriteByteMask(SCC_42, 0x00, BIT0|BIT1);//DNR core enable
        msWriteByteMask(SCC_44, 0x00, BIT0|BIT1);//pre-SNR enable
        msWriteByteMask(SC6_14, 0x00, BIT2);
        g_SetupPathInfo.bMRWR2Y = 0;
    }
    msWriteBit(SCD_66,g_SetupPathInfo.bMRWR2Y,BIT6);//R to Y enable
    mStar_AdjustContrast( UserPrefContrast );
    //msWrite2Byte(SCC_66,0xFFFF);//TNR strength, need to check with RD
}
#endif

static void msSetupPathVideoCompress()
{
    WORD u16TargetBitRate, u16MRWLineOffset, u16MRWLineFetch;
    DWORD u16MRWFrameOffset;
//BK01 VC
    msWrite2Byte(SC1_00 ,0X0001);
    msWrite2Byte(SC1_02 ,0X1000);

    msWrite2Byte(SC1_06 ,0X001C);
    msWrite2Byte(SC1_08 ,0X0211);
    msWrite2Byte(SC1_0A ,0X0211);
    msWrite2Byte(SC1_0C ,0X0707);
    msWrite2Byte(SC1_0E ,0X0310);
    msWrite2Byte(SC1_10 ,0X0001);
    msWrite2Byte(SC1_12 ,0X5030);

//VEN ENABLE
    msWrite2Byte(SC1_20 ,0X0007);
    msWrite2Byte(SC1_22 , 0X1000);
    msWrite2Byte(SC1_26 ,0X001C);
    msWrite2Byte(SC1_28 ,0X0211);
    msWrite2Byte(SC1_2A ,0X0211);

    if( IsColorspaceRGBInput() )
    {
        msWrite2Byte(SC1_2C ,0X0707);
        msWrite2Byte(SC1_AA ,0X0707);
    }

    else
    {
        msWrite2Byte(SC1_2C ,0X0F0F);
        msWrite2Byte(SC1_AA ,0X0F0F);
    }

    msWrite2Byte(SC1_2E ,0X0310);
    msWrite2Byte(SC1_30 ,0X0001);
    msWrite2Byte(SC1_32 ,0X5030);

    //msWrite2Byte(SC1_44 ,0X00A0);

    msWrite2Byte(SC1_A8 ,0X0707);
    //msWrite2Byte(SC1_AA ,0X0707);
    msWrite2Byte(SC1_AC ,0XE5E5);
    msWrite2Byte(SC1_AE ,0X00E5);

//VDE
    msWrite2Byte(SC1_A0 ,0X0F07);
    //msWrite2Byte(SC1_A2,0X0780);
    msWrite2Byte(SC1_A6 ,0X0007);
    msWrite2Byte(SC1_C0 ,0X0F07);
    //msWrite2Byte(SC1_C2, 0X0780);
    msWrite2Byte(SC1_C6 ,0X0007);

    msWrite2Byte(SC1_C8 ,0X0701);
    msWrite2Byte(SC1_CC ,0X00E5);

 //20131017 for bottom side display abnormal while 832x624,800x600 and 1024x768 timing
#if ENABLE_VC_8_5
    msWriteByte(SC1_44,0xA0);//Compression target bit:Format (xxx.xxxxx)
    u16TargetBitRate = (WORD)(PANEL_WIDTH * 5);
    u16MRWLineFetch = ((WORD)PANEL_WIDTH * 5 ) / 32 + 1;
#else
    msWriteByte(SC1_44,0xC0);//Compression target bit:Format (xxx.xxxxx)
    u16TargetBitRate = (WORD)(PANEL_WIDTH * 6);
    u16MRWLineFetch = ((WORD)PANEL_WIDTH * 6 ) / 32+ 1;
#endif
    u16MRWLineOffset = (g_SetupPathInfo.bMRW422 ? u16MRWLineFetch*2 : u16MRWLineFetch*3);
    //u16MRWFrameOffset = (DWORD)u16MRWLineOffset * PANEL_HEIGHT;
    u16MRWFrameOffset = (DWORD)u16MRWLineOffset * g_SetupPathInfo.wMRWSizeOutV;
    msWrite2Byte(SC1_46, u16TargetBitRate|BIT15);
    msWrite2Byte(SC1_48, u16TargetBitRate|BIT15);
    msWrite2ByteMask(SC6_D8, u16MRWLineOffset|BIT15,0x8FFF);
    msWrite2ByteMask(SC6_DC, u16MRWLineFetch|BIT15,0x8FFF);
    msWrite2ByteMask(SC6_E0, u16MRWLineFetch|BIT15,0x8FFF);
    msWrite3Byte(SC6_E4, u16MRWFrameOffset);
    msWriteByteMask(SC6_E7, BIT7, BIT7);

    msWriteByteMask(SC1_20,BIT0|BIT1|BIT2|BIT3,BIT0|BIT1|BIT2|BIT3);//ven enable,[0]:R,[1]:G,[2]:B,[3]:motion
    msWriteByteMask(SC1_A0,BIT7|BIT2,BIT7|BIT3|BIT2);//vde ctrl select, [0]:from OPM reg,[1]:from self reg  [2]:OPM_FRAME_Auto_CLR [7]:OPM_Line_Auto_CLR
    msWriteByteMask(SC1_A1,BIT0|BIT1|BIT2|BIT3,BIT0|BIT1|BIT2|BIT3);//vde enable,[0]:R,[1]:G,[2]:B,[3]:motion
    msWriteBit(SC1_20,g_SetupPathInfo.bMRW422,BIT6);//ven 422 enable
    msWriteBit(SC1_A0,g_SetupPathInfo.bMRW422,BIT1);//vde 422 enable
    msWriteBit(SC1_C0,g_SetupPathInfo.bMRW422,BIT1);//vde 422 enable
    //msWriteBit(SC1_A5,g_SetupPathInfo.bMRW422,BIT1|BIT2);//vde 422 enable
    msWrite2ByteMask(SC1_A2, g_SetupPathInfo.wMRWSizeOutH, 0x0FFF);//H size of compress image
    msWriteByteMask(SC1_C0,BIT7|BIT2,BIT7|BIT3|BIT2);//vde ctrl select[0]:from IPR reg,[1]:from self reg [2]:IPR_FRAME_Auto_CLR [7]:IPR_Line_Auto_CLR
    msWriteByteMask(SC1_C1,BIT0 ,BIT0);
    msWrite2ByteMask(SC1_C2,g_SetupPathInfo.wMRWSizeInH,0x0FFF);//H size of compress image(IPR)
}


static void msSetupPathMRW()
{
    DWORD dwVtotalOut;
    BYTE ucDelay;



    //HSD
    msWriteByteMask(SC11_0B, 0x00, BIT6|BIT7);//[7]:HSD enable, [6]:HSD mode
    msWrite2Byte(SC11_08,0x00);
    msWriteByteMask(SC11_0A, 0x00, 0x7F);

    // memory config
    if (g_SetupPathInfo.bFBMode == TRUE)
    {
        msWriteByteMask(SC6_29,BIT0|BIT1,BIT0|BIT1);//[0]:ipm enable, [1]:opm enable
        msWriteByteMask(SC6_02,BIT2,BIT2);//ipm mclk enable
        msWriteByteMask(SC6_1A,BIT1,BIT0|BIT1);//[0]:DNR FBL mode,[1]:opm mclk enable

        if(g_SetupPathInfo.bMRW422)//422
            msWriteByteMask(SC6_14, BIT0, BIT0|BIT1|BIT2);
        else//444
            msWriteByteMask(SC6_14, 0x00, BIT0|BIT1|BIT2);

    }
    else
    {
        msWriteByteMask(SC6_29,BIT0,BIT0|BIT1);//[0]:ipm enable, [1]:opm enable
        msWriteByteMask(SC6_02,BIT2,BIT2);//ipm mclk enable
        msWriteByteMask(SC6_1A,BIT0,BIT0|BIT1);//[0]:DNR FBL mode,[1]:opm mclk enable
    #if ENABLE_TNR
        if(TNROnFlag)
        {
            msWriteByteMask(SC6_14, BIT2|BIT1, BIT0|BIT1|BIT2);//[2] motion, [1] Y only
        }
    #endif
        msWriteByteMask(SC6_1A, BIT0, BIT0);//DNR FBL mode
    }

    msSetupPathMRWBaseRD();
    msSetupPathMRWBaseWR();
    msSetupPathVideoCompress();
    if(g_SetupPathInfo.b3DTo2DEnable)
        msMRWDataFormat(SC_FMT_IN_NORMAL, SC_FMT_OUT_3D_FS);
    else
        msMRWDataFormat(g_SetupPathInfo.ucSCFmtIn, g_SetupPathInfo.ucSCFmtOut);
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

    // setup MIU video format
    msWriteBit(SC6_03,g_SetupPathInfo.bMRW422,BIT0);//16 bits mode
    msWriteBit(SC6_A3,g_SetupPathInfo.bMRW422,BIT6);
    msWriteBit(SC6_A3,g_SetupPathInfo.bMRW422,BIT0);
    msWriteBit(SC6_A3,g_SetupPathInfo.bMRW422,BIT3);//444 to 422 enable
    msWriteBit(SC6_A3,g_SetupPathInfo.bMRW422,BIT4);//422 to 444 by keeping even one
    msWriteByteMask(SC6_A3,BIT1,BIT1);
    msWriteBit(SCD_66,g_SetupPathInfo.bMRWR2Y,BIT6);//R to Y enable

    // set trig
    dwVtotalOut = SC0_READ_VTOTAL();
    if(CURRENT_SOURCE_IS_INTERLACE_MODE())
        dwVtotalOut /= 2;
    dwVtotalOut = (dwVtotalOut * g_SetupPathInfo.wImgSizeOutV) / g_SetupPathInfo.wImgSizeInV ;
    msWrite2Byte(SC0_AC,00);
    switch(g_SetupPathInfo.ucSCPathMode)
    {
        // VSP -> MRW
        case SC_PATH_MODE_3:
        case SC_PATH_MODE_4:
        case SC_PATH_MODE_6:
            msWrite2Byte(SC0_AE , dwVtotalOut-1);
            break;

        default:
            ucDelay = (g_SetupPathInfo.wImgSizeOutV * 2 / g_SetupPathInfo.wImgSizeInV) + 2;
            msWrite2Byte(SC0_AE ,(dwVtotalOut - ucDelay));
            break;
    }

    // read/write byte enable
    msWriteByte(SC6_BE, 0xFF);
#if CHIP_ID==CHIP_TSUMD
    if(g_SetupPathInfo.bFBMode)
    {
        msSetFRC_LockBase_VCNT();
    }
#endif
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
#endif
//ENABLE_DEBUG
static void msSetupPathDbgDump()
{
#if DEBUG_PRINT_ENABLE
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
    MSSCALER_printData("bFBMode = %d", g_SetupPathInfo.bFBMode);
    MSSCALER_printData("bMRWR2Y = %d", g_SetupPathInfo.bMRWR2Y );
    MSSCALER_printData("bMRW422 = %d", g_SetupPathInfo.bMRW422 );
    MSSCALER_printData("b3DTo2DEnable = %d", g_SetupPathInfo.b3DTo2DEnable );
    MSSCALER_printData("b2DTo3DEnable = %d", g_SetupPathInfo.b2DTo3DEnable );
#endif
}

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
    msSetupPathHMIR();
    msSetupPathRSP();
    mStar_SetScalingFactor();
    mStar_SetScalingFilter();
    msSetupPathDbgDump();
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
    g_SetupPathInfo.bOverrideSCPathMode = FALSE;
    g_SetupPathInfo.ucSCFmtIn = SC_FMT_IN_NORMAL;
    g_SetupPathInfo.bMRWDual = 0;
    g_SetupPathInfo.bMRW422 = 0;
    g_SetupPathInfo.bMRWR2Y = 0;
#if ENABLE_R2_2DTO3D
    g_SetupPathInfo.b2DTo3DEnable = UserPref2Dto3D;
#else
    g_SetupPathInfo.b2DTo3DEnable = FALSE;
#endif
    g_SetupPathInfo.b3DTo2DEnable = FALSE;

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
#elif PANEL_OUTPUT_FMT_3D_TB
    g_SetupPathInfo.ucSCFmtOut = SC_FMT_OUT_3D_TB;
    g_SetupPathInfo.bPsvPnlOutLRInvert = 0;
#elif PANEL_OUTPUT_FMT_3D_SBS
    g_SetupPathInfo.ucSCFmtOut = SC_FMT_OUT_3D_SBS;
    g_SetupPathInfo.bPsvPnlOutLRInvert = 0;
#elif PANEL_OUTPUT_FMT_3D_CB
    g_SetupPathInfo.ucSCFmtOut = SC_FMT_OUT_3D_CB;
    g_SetupPathInfo.bPsvPnlOutLRInvert = 0;
#else
    g_SetupPathInfo.ucSCFmtOut = SC_FMT_OUT_3D_FS;
    g_SetupPathInfo.bPsvPnlOutLRInvert = 0;
#endif
    g_SetupPathInfo.bHMirrorMode = 0;
    g_SetupPathInfo.wImgSizeOutH = PANEL_WIDTH;
    g_SetupPathInfo.wImgSizeOutV = PANEL_HEIGHT;
}
