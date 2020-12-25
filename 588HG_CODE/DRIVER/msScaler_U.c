

#define _MS_SCALER_C_
/*
#include "types.h"
#include "board.h"
#include "ms_reg.h"
#include "mode.h"
#include "UserPrefDef.h"
#include "global.h"
#include "panel.h"
#include "adjust.h"
#include "ms_rwreg.h"
#include "debug.h"
#include "misc.h"
#include "power.h"
#include "detect.h"
#include "msOsd.h"
#include "menu.h"
#include "mStar.h"
#include "Mode.h"
#include "MsFlash.h"
#include "AutoFunc.h"
#include "msScaler.h"
#include "msfb.h"
#include "MenuDef.h"
#include "Mshdmi.h"
////////////////////////////////////////////////////
#include "drvmStar.h"
///////////////////////////////////////////////////
*/
//#include "DebugMsg.h"
#include "global.h"
#include "detect.h"
#if  CHIP_ID==CHIP_TSUMU
xdata SetupPathInfo g_SetupPathInfo;
#define MSSCALER_DEBUG    0
#if ENABLE_DEBUG && MSSCALER_DEBUG
#define MSSCALER_printData(str, value)   printData(str, value)
#define MSSCALER_printMsg(str)           printMsg(str)
#else
#define MSSCALER_printData(str, value)
#define MSSCALER_printMsg(str)
#endif
#define V_COEF_NUM (5)
BYTE code tVScalingCoeff[ 3 ][ V_COEF_NUM ] =
{
    /*0x5A*/   { 0x00, 0x04, 0x08, 0x0C, 0x10 }, /* VDSUSG */
    /*0x57*/   { 0x00, 0x02, 0x0A, 0x27, 0x51 }, /* CB(0) */   /*VSPC*/    /*Scaling up */
    /*0x57*/   { 0x1F, 0x2B, 0x37, 0x4F, 0x67 }, /* BS(0.75) */ /*VSPC*/   /*Scaling down*/
};
BYTE code tHFilterTbl[] =
{
    0x00,   // no-scaling
    0x02,   // upscaling
    0x04    // downscaling
};

static void mStar_SetVScalingFilter( BYTE u8Filter )
{
    BYTE u8Bank=msReadByte(SC0_00);
    //v filter
    if(( u8Filter & 0x0F ) == 0 )
    {
        msWriteByteMask( SC0_5A,  0, ( BIT1 | BIT0 ) ); /* Vertical scaling filter is Bilinear mode */
    }
    else
    {
        BYTE i;
        msWriteByteMask( SC0_5A,  1, ( BIT1 | BIT0 ) ); /* Vertical scaling filter is  Register define mode */
        for( i = 0; i < V_COEF_NUM; i++ )
        {
            msWriteByteMask( SC0_5A, ( tVScalingCoeff[ 0 ][ i ] ), ( BIT4 | BIT3 | BIT2 ) ); /*Bit4~2*/
            msWriteByte( SC0_57 ,  tVScalingCoeff[ u8Filter&0x0F ][ i ] );
        }
    }
    msWriteByte(SC0_00,u8Bank);
}

static void mStar_SetHScalingFilter( BYTE u8Filter )
{
    msWriteByte( SC0_56, tHFilterTbl[u8Filter] );
}

void mStar_SetScalingFilter( void )
{
    // horizontal
    if( g_SetupPathInfo.wImgSizeInH == g_SetupPathInfo.wImgSizeOutH )
    {
        mStar_SetHScalingFilter( 0 );   // no-scaling
        msWriteByte( REG_1E3F, 0x04 );
    }
    else if( g_SetupPathInfo.wImgSizeInH < g_SetupPathInfo.wImgSizeOutH )
    {
        mStar_SetHScalingFilter( 1 );   // scaling-up
        msWriteByte( REG_1E3F, 0x04 );
    }
    // scaling-down
    else
    {
        mStar_SetHScalingFilter( 2 );   // scaling-down
        msWriteByte( REG_1E3F, 0x00 );
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
}

void mStar_SetScalingFactor( void )
{
    DWORD factor;
    WORD height;
    BYTE ratioUp, ratioDown;
    BYTE bLimitDown19 = 0, bLimitDown2 = 0, bLimitDown29 = 0;
    BYTE bLimitUp5 = 0;
    BYTE u8Bank=msReadByte(SC0_00);
    msWriteByte(SC0_00,0x00);

    height = g_SetupPathInfo.wImgSizeInV;

    // Setup horizontal expansion
    if( g_SetupPathInfo.wImgSizeInH != g_SetupPathInfo.wImgSizeOutH ) // set expansion factor
    {
        factor = (g_SetupPathInfo.wImgSizeInH / 2) - HSC_offset; // factor=((input width/2-1)*2^20)/(output width/2-1) + 1, 2 pixel engine
        factor = (( DWORD )( factor << 20 ) ) / ( (g_SetupPathInfo.wImgSizeOutH / 2) - HSC_offset ) + 1;
        msWrite2Byte( SC0_50, factor & 0xFFFF );
        msWriteByte( SC0_52, (( factor >> 16 ) & 0xFF ) | BIT7 );
    }
    else
        msWriteByte( SC0_52, 0 ); // input width=output width, then disable expansion

    // Setup vertical expansion
    if( height != g_SetupPathInfo.wImgSizeOutV )
    {
        factor = height - 1; // factor=((input height-1)*2^20)/(output height-1) + 1
        factor = (( DWORD )( factor << 20 ) ) / ( g_SetupPathInfo.wImgSizeOutV - 1 ) + 1;
        msWrite2Byte( SC0_53, factor & 0xFFFF );
        msWriteByte( SC0_55, (( factor >> 16 ) & 0xFF ) | BIT7 );

        ratioDown = height / g_SetupPathInfo.wImgSizeOutV;
        ratioUp = g_SetupPathInfo.wImgSizeOutV / height;
        if( msReadByte(SC0_5B) & _BIT0 )    /* Top Bottom */
        {
            switch(g_SetupPathInfo.ucSCPathMode)
            {
                case SC_PATH_MODE_1:
                    if( ratioUp >= 5 )
                        bLimitUp5 = 1;
                    else if( ratioDown >= 2 )
                        bLimitDown2 = 1;
                    break;

                case SC_PATH_MODE_3:
                case SC_PATH_MODE_4:
                    if( ratioDown >= 2 )
                        bLimitDown19 = 1;
                    break;

                case SC_PATH_MODE_5:
                case SC_PATH_MODE_6:
                    if( ratioDown >= 3 )
                        bLimitDown29 = 1;
                    break;

                default:
                    break;
            }
        }
        else if( msReadByte(SC0_5B) & _BIT1 )   /* Line Interleave */
        {
            if( ratioDown >= 2 )
                bLimitDown19 = 1;
        }
        else
        {
            if( ratioDown >= 3 )
                bLimitDown29 = 1;
        }

        if( bLimitDown19 )
        {
            msWrite2Byte(SC0_53, 0xFFFF);
            msWriteByte(SC0_55, 0x1F | BIT7);
            MSSCALER_printMsg("VSP Scaling Ratio Over 19");
        }
        else if( bLimitDown2 )
        {
            msWrite2Byte(SC0_53, 0x0000);
            msWriteByte(SC0_55, 0x20 | BIT7);
            MSSCALER_printMsg("VSP Scaling Ratio Over 2");
        }
        else if( bLimitDown29 )
        {
            msWrite2Byte(SC0_53, 0xFFFF);
            msWriteByte(SC0_55, 0x2F | BIT7);
            MSSCALER_printMsg("VSP Scaling Ratio Over 29");
        }
        else if( bLimitUp5 )
        {
            msWrite2Byte(SC0_53, 0x3333);
            msWriteByte(SC0_55, 0x00 | BIT7);
            MSSCALER_printMsg("VSP Scaling Ratio Over 5");
        }
    }
    else
        msWriteByte( SC0_55, 0 ); // input height=output height, then disable expansion
    msWriteByte(SC0_00,u8Bank);
    #if 0//Enable_Expansion
    IsSupportedAspRatioMode(OverScanSetting.FinalCapWinWidth,OverScanSetting.FinalCapWinHeight);
    #endif

}

static void msSetupPathParam()
{
//setup wImgSizeOutH & wImgSizeOutV ---begin
    if( IS_FMT_OUT_PSV() )
    {
        if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL )
        {
            g_SetupPathInfo.wImgSizeOutH = PANEL_WIDTH;
            g_SetupPathInfo.wImgSizeOutV = PANEL_HEIGHT;
        }
        else
#if PANEL_OUTPUT_FMT_3D_CB
            g_SetupPathInfo.wImgSizeOutH = PANEL_WIDTH / 2;
#else
            g_SetupPathInfo.wImgSizeOutV = PANEL_HEIGHT / 2;
#endif
    }

#if Enable_Expansion && !PANEL_OUTPUT_FMT_3D_CB // Not support //JisonWideMode
    if(OverScanSetting.Enable && (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL))
    {
        g_SetupPathInfo.wImgSizeOutH = OverScanSetting.OverScanH;
        g_SetupPathInfo.wImgSizeOutV = OverScanSetting.OverScanV;
        g_SetupPathInfo.bNormalCase34 = TRUE;
    }
    else
    {
        g_SetupPathInfo.wImgSizeOutH = PANEL_WIDTH;
        g_SetupPathInfo.wImgSizeOutV = PANEL_HEIGHT;
        if( IS_FMT_OUT_PSV() && (g_SetupPathInfo.ucSCFmtIn != SC_FMT_IN_NORMAL) )
            g_SetupPathInfo.wImgSizeOutV /= 2;
        g_SetupPathInfo.bNormalCase34 = FALSE;
    }

#endif
//setup wImgSizeOutH & wImgSizeOutV ---end

#if ENABLE_HDMI
#if 0//PANEL_3D_PASSIVE_4M
    if( gScInfo.InputColor == INPUT_RGB &&IS_3D_MODE)
#else
   // if( gScInfo.InputColor == INPUT_RGB )
   if(CURRENT_INPUT_IS_DVI())//( SrcInputType== Input_Digital)
#endif
        g_SetupPathInfo.bMRWR2Y = 1;
    else
        g_SetupPathInfo.bMRWR2Y = 0;
#endif

    if( g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_PSV_4M )
        g_SetupPathInfo.bMRW422 = 1;
    else
        g_SetupPathInfo.bMRW422 = 0;
    MSSCALER_printData("g_SetupPathInfo.ucSCFmtIn(%d)", g_SetupPathInfo.ucSCFmtIn);
    MSSCALER_printData("STD_MODE_WIDTH(%d)", STD_MODE_WIDTH);
    MSSCALER_printData("STD_MODE_HEIGHT(%d)", STD_MODE_HEIGHT);

    switch(g_SetupPathInfo.ucSCFmtIn)
    {
        case SC_FMT_IN_NORMAL:
            g_SetupPathInfo.wImgSizeInH = msRead2Byte(SC0_0B) & SC_MASK_H;
           // PRINT_DATA("\r\n g_SetupPathInfo.wImgSizeInH 1=%x", g_SetupPathInfo.wImgSizeInH);

            g_SetupPathInfo.wImgSizeInV = msRead2Byte(SC0_09) & SC_MASK_V;
            if( msReadByte(SC0_A6) & _BIT1)
            {
               // PRINT_DATA("\r\n g_SetupPathInfo.wImgSizeInH 2=%x", g_SetupPathInfo.wImgSizeInH);
                g_SetupPathInfo.wImgSizeInH *= 2;
            }
            if(CURRENT_SOURCE_IS_INTERLACE_MODE())//(SRC_FLAGS & INTERLACE_MODE_BIT)
                g_SetupPathInfo.wImgSizeInV /= 2;
            break;

        case SC_FMT_IN_3D_PF:
            g_SetupPathInfo.wImgSizeInH =StandardModeWidth; //STD_MODE_WIDTH;
            g_SetupPathInfo.wImgSizeInV = StandardModeHeight;//STD_MODE_HEIGHT;
            break;

        case SC_FMT_IN_3D_FP:
        case SC_FMT_IN_3D_FPI:
        case SC_FMT_IN_3D_FA:
        case SC_FMT_IN_3D_TB:
        case SC_FMT_IN_3D_LA:
            g_SetupPathInfo.wImgSizeInH = StandardModeWidth;//STD_MODE_WIDTH;
            g_SetupPathInfo.wImgSizeInV = StandardModeHeight/2;//STD_MODE_HEIGHT / 2;
            if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FPI )
                g_SetupPathInfo.wImgSizeInV /= 2;
            break;

        case SC_FMT_IN_3D_SBSH:
        case SC_FMT_IN_3D_SBSF:
        case SC_FMT_IN_3D_CB:
            g_SetupPathInfo.wImgSizeInH = StandardModeWidth/2;//STD_MODE_WIDTH / 2;
            g_SetupPathInfo.wImgSizeInV = StandardModeHeight;//STD_MODE_HEIGHT;
            break;

        default:
            MSSCALER_printData("Unknown SC FMT IN line(%d)", __LINE__);
    }

    if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL )
    {
        /* May Over Scan */
        g_SetupPathInfo.wOriSizeH = g_SetupPathInfo.wImgSizeInH;
        g_SetupPathInfo.wOriSizeV = g_SetupPathInfo.wImgSizeInV;
    }
    else
    {
        g_SetupPathInfo.wOriSizeH = StandardModeWidth;//STD_MODE_WIDTH;
        g_SetupPathInfo.wOriSizeV = StandardModeHeight;//STD_MODE_HEIGHT;
    }

    if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FP )
        g_SetupPathInfo.wOriSizeV /=2;
    else if(g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FPI
            || g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FA)
        g_SetupPathInfo.wOriSizeV /=4;

    // Interlaced mode for TB, SBS, PF, LA
    if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_TB)
     || (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_PF)
     || (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_LA)
     ||  IS_FMT_IN_SBS() )
    {
        if( CURRENT_SOURCE_IS_INTERLACE_MODE() )//(SRC_FLAGS & INTERLACE_MODE_BIT)
        {
            g_SetupPathInfo.wImgSizeInV /= 2;
            g_SetupPathInfo.wOriSizeV /=2;
        }
    }

    if( g_SetupPathInfo.wImgSizeInV > g_SetupPathInfo.wImgSizeOutV )
        g_SetupPathInfo.bScalingUpV = 0;
    else
        g_SetupPathInfo.bScalingUpV = 1;

    if( g_SetupPathInfo.wImgSizeInH > g_SetupPathInfo.wImgSizeOutH )
        g_SetupPathInfo.bScalingUpH = 0;
    else
        g_SetupPathInfo.bScalingUpH = 1;

#if (FRAME_BFF_SEL==FRAME_BUFFER)


    if( IS_FMT_OUT_PSV()|| (g_SetupPathInfo.bNormalCase34) )
    {
        if( g_SetupPathInfo.bScalingUpV
         || (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_CB) )
        {
            if( g_SetupPathInfo.bScalingUpH )
            {
                g_SetupPathInfo.ucSCPathMode = SC_PATH_MODE_3;
                g_SetupPathInfo.wMRWSizeInH = g_SetupPathInfo.wOriSizeH;
                if( IS_FMT_IN_SBS() )
                    g_SetupPathInfo.wMRWSizeInH /= 2;
            }
            else
            {
                g_SetupPathInfo.ucSCPathMode = SC_PATH_MODE_4;
                g_SetupPathInfo.wMRWSizeInH = g_SetupPathInfo.wImgSizeOutH;
                if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_CB) )
                    g_SetupPathInfo.wMRWSizeInH *= 2;
            }
            g_SetupPathInfo.wMRWSizeOutH = g_SetupPathInfo.wMRWSizeInH;
            if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_CB) )
                g_SetupPathInfo.wMRWSizeOutH /= 2;

            g_SetupPathInfo.wMRWSizeInV = g_SetupPathInfo.wOriSizeV;
            if( IS_FMT_IN_SBS())
                g_SetupPathInfo.wMRWSizeInV *= 2;
            g_SetupPathInfo.wMRWSizeOutV = g_SetupPathInfo.wMRWSizeInV;
            if( (g_SetupPathInfo.ucSCFmtIn != SC_FMT_IN_3D_TB)
             && (g_SetupPathInfo.ucSCFmtIn != SC_FMT_IN_NORMAL)
             && (!IS_MRW_FMT_LA()) )
                g_SetupPathInfo.wMRWSizeOutV *= 2;
        }
        else
        {
            if( g_SetupPathInfo.bScalingUpH )
            {
                g_SetupPathInfo.ucSCPathMode = SC_PATH_MODE_5;
                g_SetupPathInfo.wMRWSizeInH = g_SetupPathInfo.wOriSizeH;
                if( IS_FMT_IN_SBS() )
                    g_SetupPathInfo.wMRWSizeInH /= 2;
            }
            else
            {
                g_SetupPathInfo.ucSCPathMode = SC_PATH_MODE_6;
                g_SetupPathInfo.wMRWSizeInH = g_SetupPathInfo.wImgSizeOutH;
            }
            g_SetupPathInfo.wMRWSizeOutH = g_SetupPathInfo.wMRWSizeInH;

            g_SetupPathInfo.wMRWSizeInV = g_SetupPathInfo.wImgSizeOutV;
            if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_TB)
             || (IS_MRW_FMT_LA()) )
                g_SetupPathInfo.wMRWSizeInV *= 2;

            if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL )
                g_SetupPathInfo.wMRWSizeOutV = g_SetupPathInfo.wImgSizeOutV;
            else
                g_SetupPathInfo.wMRWSizeOutV = g_SetupPathInfo.wImgSizeOutV * 2;
        }

        if ( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_CB || IS_MRW_FMT_LA() )
            g_SetupPathInfo.bMRWDual = 1;
        else
            g_SetupPathInfo.bMRWDual = 0;

    }
    else
    {
        switch(g_SetupPathInfo.ucSCFmtIn)
        {
            case SC_FMT_IN_NORMAL:
                g_SetupPathInfo.ucSCPathMode = SC_PATH_MODE_1;

                g_SetupPathInfo.wMRWSizeInH = g_SetupPathInfo.wImgSizeOutH;
                g_SetupPathInfo.wMRWSizeInV = g_SetupPathInfo.wImgSizeOutV;
                g_SetupPathInfo.wMRWSizeOutH = g_SetupPathInfo.wImgSizeOutH;
                g_SetupPathInfo.wMRWSizeOutV = g_SetupPathInfo.wImgSizeOutV;
                g_SetupPathInfo.bMRWDual = 0;
                break;

            case SC_FMT_IN_3D_PF:
                g_SetupPathInfo.ucSCPathMode = SC_PATH_MODE_1;

                g_SetupPathInfo.wMRWSizeInH = g_SetupPathInfo.wImgSizeOutH;
                g_SetupPathInfo.wMRWSizeInV = g_SetupPathInfo.wImgSizeOutV;
                g_SetupPathInfo.wMRWSizeOutH = g_SetupPathInfo.wImgSizeOutH;
                g_SetupPathInfo.wMRWSizeOutV = g_SetupPathInfo.wImgSizeOutV;
                g_SetupPathInfo.bMRWDual = 0;
                if( g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_PSV )
                    g_SetupPathInfo.wMRWSizeOutV *= 2;
                break;

            case SC_FMT_IN_3D_FP:
            case SC_FMT_IN_3D_FPI:
            case SC_FMT_IN_3D_FA:
                g_SetupPathInfo.ucSCPathMode = SC_PATH_MODE_1;

                g_SetupPathInfo.wMRWSizeInH = g_SetupPathInfo.wImgSizeOutH;
                g_SetupPathInfo.wMRWSizeInV = g_SetupPathInfo.wImgSizeOutV;
                g_SetupPathInfo.wMRWSizeOutH = g_SetupPathInfo.wImgSizeOutH;
                g_SetupPathInfo.wMRWSizeOutV = g_SetupPathInfo.wImgSizeOutV;
                g_SetupPathInfo.bMRWDual = 0;
                if( g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_PSV )
                    g_SetupPathInfo.wMRWSizeOutV *= 2;
                break;

            case SC_FMT_IN_3D_TB:
                g_SetupPathInfo.ucSCPathMode = SC_PATH_MODE_3;
                g_SetupPathInfo.wMRWSizeInH = StandardModeWidth;//STD_MODE_WIDTH;
                g_SetupPathInfo.wMRWSizeInV = g_SetupPathInfo.wImgSizeInV * 2;
                g_SetupPathInfo.wMRWSizeOutH = g_SetupPathInfo.wMRWSizeInH;
                g_SetupPathInfo.wMRWSizeOutV = g_SetupPathInfo.wMRWSizeInV / 2;
                g_SetupPathInfo.bMRWDual = 0;
                if( g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_PSV )
                    g_SetupPathInfo.wMRWSizeOutV *= 2;
                break;

            case SC_FMT_IN_3D_SBSH:
            case SC_FMT_IN_3D_SBSF:
                g_SetupPathInfo.ucSCPathMode = SC_PATH_MODE_1;

                g_SetupPathInfo.wMRWSizeInH = g_SetupPathInfo.wImgSizeOutH;
                g_SetupPathInfo.wMRWSizeInV = g_SetupPathInfo.wImgSizeOutV*2;
                g_SetupPathInfo.wMRWSizeOutH = g_SetupPathInfo.wImgSizeOutH;
                g_SetupPathInfo.wMRWSizeOutV = g_SetupPathInfo.wImgSizeOutV;
                if( g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_PSV )
                {
                    g_SetupPathInfo.wMRWSizeOutV *= 2;
                    g_SetupPathInfo.bMRWDual = 0;
                }
                else
                {
                    g_SetupPathInfo.bMRWDual = 1;
                }
                break;

            case SC_FMT_IN_3D_LA:
                g_SetupPathInfo.ucSCPathMode = SC_PATH_MODE_3;
                g_SetupPathInfo.wMRWSizeInH = StandardModeWidth;//STD_MODE_WIDTH;
                g_SetupPathInfo.wMRWSizeInV = StandardModeHeight;//STD_MODE_HEIGHT;
                if( g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_PSV )
                {
                    g_SetupPathInfo.wMRWSizeOutH = g_SetupPathInfo.wMRWSizeInH;
                    g_SetupPathInfo.wMRWSizeOutV = g_SetupPathInfo.wMRWSizeInV;
                    g_SetupPathInfo.bMRWDual = 0;
                }
                else
                {
                    g_SetupPathInfo.wMRWSizeOutH = g_SetupPathInfo.wImgSizeInH;
                    g_SetupPathInfo.wMRWSizeOutV = g_SetupPathInfo.wImgSizeInV;
                    g_SetupPathInfo.bMRWDual = 1;
                }
                break;

            case SC_FMT_IN_3D_CB:
                g_SetupPathInfo.ucSCPathMode = SC_PATH_MODE_3;
                g_SetupPathInfo.wMRWSizeInH = StandardModeWidth;//STD_MODE_WIDTH;
                g_SetupPathInfo.wMRWSizeInV = StandardModeHeight;//STD_MODE_HEIGHT;
                g_SetupPathInfo.wMRWSizeOutH = g_SetupPathInfo.wImgSizeInH;
                g_SetupPathInfo.wMRWSizeOutV = g_SetupPathInfo.wImgSizeInV;
                g_SetupPathInfo.bMRWDual = 1;
                if( g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_PSV )
                    g_SetupPathInfo.wMRWSizeOutV *= 2;
                break;

            default:
                MSSCALER_printData("Unknown SC FMT IN line(%d)", __LINE__);
        }

        if( !g_SetupPathInfo.bScalingUpH )
        {
            if( (g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_3)
             || (g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_5) )
            {
                g_SetupPathInfo.ucSCPathMode++;

                g_SetupPathInfo.wMRWSizeInH = g_SetupPathInfo.wImgSizeOutH;
                g_SetupPathInfo.wMRWSizeOutH = g_SetupPathInfo.wImgSizeOutH;
            }
        }
    }
#else //frame buffer less
    g_SetupPathInfo.wMRWSizeInH = g_SetupPathInfo.wImgSizeOutH;
    g_SetupPathInfo.wMRWSizeInV = g_SetupPathInfo.wImgSizeOutV;
    g_SetupPathInfo.wMRWSizeOutH = g_SetupPathInfo.wImgSizeOutH;
    g_SetupPathInfo.wMRWSizeOutV = g_SetupPathInfo.wImgSizeOutV;
    g_SetupPathInfo.bMRWDual = 0;
#endif

    if(!g_SetupPathInfo.bFBMode)
        g_SetupPathInfo.ucSCPathMode = SC_PATH_MODE_0;




    // 4M FRC
    if( (g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_PSV_4M)
     && (V_FREQ_IN <= VFREQ_4MFRC) )
    {
        DWORD InputImgSize = (DWORD)g_SetupPathInfo.wMRWSizeOutH * g_SetupPathInfo.wMRWSizeOutV * 2;

        g_SetupPathInfo.b4MFRC = 1;

        if( InputImgSize >= 0x200000 )
            g_SetupPathInfo.bHSDEnable = TRUE;
        else
            g_SetupPathInfo.bHSDEnable = FALSE;

        if( (g_SetupPathInfo.ucSCPathMode != SC_PATH_MODE_5) && (InputImgSize >= 0x200000) )
        {
            g_SetupPathInfo.b4MFRC = 0;
            g_SetupPathInfo.bHSDEnable = FALSE;
        }
    }
    else
    {
        g_SetupPathInfo.b4MFRC = 0;
        g_SetupPathInfo.bHSDEnable = FALSE;
    }

    if( g_SetupPathInfo.bHSDEnable )
    {
        g_SetupPathInfo.wImgSizeInH /= 2;
        g_SetupPathInfo.wOriSizeH /=2;

        if( (g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_3)
         || (g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_5) )
        {
            g_SetupPathInfo.wMRWSizeInH /= 2;
            g_SetupPathInfo.wMRWSizeOutH /= 2;
        }
    }

    if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL)
     || (g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_PSV)
     || (g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_PSV_4M) )
        g_SetupPathInfo.bVBIExtend = 0;
    else
        g_SetupPathInfo.bVBIExtend = 1;

    if( g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_PSV_4M )
    {
        if( g_SetupPathInfo.b4MFRC )
        {
            if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL)
            || (IS_MRW_FMT_LA()) )
                g_SetupPathInfo.ucSCFrameMode = SC_FRAME_MODE_2;
            else
                g_SetupPathInfo.ucSCFrameMode = SC_FRAME_MODE_4;
        }
        else
        {
            if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL)
             || (IS_MRW_FMT_LA()) )
                g_SetupPathInfo.ucSCFrameMode = SC_FRAME_MODE_1;
            else
                g_SetupPathInfo.ucSCFrameMode = SC_FRAME_MODE_2;
        }
    }
    else
    {
        if( g_SetupPathInfo.bMRWDual == 1
         || g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FPI
         || g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FA )
            g_SetupPathInfo.ucSCFrameMode = SC_FRAME_MODE_4;
        else
        g_SetupPathInfo.ucSCFrameMode = SC_FRAME_MODE_8;
    }

    if( g_SetupPathInfo.bMRWLLRR )
        g_SetupPathInfo.ucSCFrameMode = SC_FRAME_MODE_4;

#if Enable_Expansion
    IsSupportedAspRatioMode(OverScanSetting.FinalCapWinWidth,OverScanSetting.FinalCapWinHeight);
#endif

}

static void msSetupPathConfig()
{
#if ENABLE_RTE
    msOverDriveOnOff( FALSE );
#endif

    switch(g_SetupPathInfo.ucSCPathMode)
    {
         case SC_PATH_MODE_0:
            /* VSP */
            msWriteByteMask(SC0_5B, 0, _BIT6);
            msWriteByteMask(SC0_5B, 0, _BIT7);
            /* MRW */
            /* OP2 Select */
            msWriteByteMask(SC0_9F, 0, _BIT0);
            /* Clock */
            msWriteByteMask(REG_1E36, 0x20, 0x3F);
            #if (!PanelDualPort && !PANEL_QUAD_PORT) || (PanelTTL) || (ENABLE_RTE && (OD_MODE == OD_MODE_333) && OD_HSCAL_EN)
            msWriteByteMask( REG_1E37, 0x2C, 0x3F );
            #else
            msWriteByteMask( REG_1E37, 0x28, 0x3F );
            #endif
            msWriteByteMask(REG_1E40, 0x00, 0xF0);
            msWriteByteMask(REG_1E44, 0x00, 0x0F);
            msWriteByteMask(REG_1E46, 0x11, 0xFF);
            break;

        case SC_PATH_MODE_1:
            /* VSP */
            msWriteByteMask(SC0_5B, 0, _BIT6);
            msWriteByteMask(SC0_5B, 0, _BIT7);
            /* MRW */
            msWriteByteMask(SC6_AC, _BIT1|_BIT0, _BIT1|_BIT0);
            msWriteByteMask(SC6_AC,           0, _BIT3|_BIT2);
            /* OP2 Select */
            msWriteByteMask(SC0_9F, _BIT0, _BIT0);
            /* Clock */
            msWriteByteMask(REG_1E36, 0x20, 0x3F);
            #if (!PanelDualPort && !PANEL_QUAD_PORT) || (PanelTTL) || (ENABLE_RTE && (OD_MODE == OD_MODE_333) && OD_HSCAL_EN)
            msWriteByteMask( REG_1E37, 0x24, 0x3F );
            #else
            msWriteByteMask( REG_1E37, 0x20, 0x3F );
            #endif
            msWriteByteMask(REG_1E40, 0x00, 0xF0);
            msWriteByteMask(REG_1E44, 0x00, 0x0F);
            msWriteByteMask(REG_1E46, 0x84, 0xFF);
            break;

        case SC_PATH_MODE_3:
            /* VSP */
            msWriteByteMask(SC0_5B, _BIT6, _BIT6);
            msWriteByteMask(SC0_5B,     0, _BIT7);
            /* MRW */
            msWriteByteMask(SC6_AC,     0, _BIT1|_BIT0);
            msWriteByteMask(SC6_AC, _BIT2, _BIT3|_BIT2);
            /* OP2 Select */
            msWriteByteMask(SC0_9F,     0, _BIT0);
            /* Clock */
            msWriteByteMask(REG_1E36, 0x20, 0x3F);
            #if (!PanelDualPort && !PANEL_QUAD_PORT) || (PanelTTL) || (ENABLE_RTE && (OD_MODE == OD_MODE_333) && OD_HSCAL_EN)
            msWriteByteMask( REG_1E37, 0x2C, 0x3F );
            #else
            msWriteByteMask( REG_1E37, 0x28, 0x3F );
            #endif
            msWriteByteMask(REG_1E40, 0x80, 0xF0);
            msWriteByteMask(REG_1E44, 0x00, 0x0F);
            msWriteByteMask(REG_1E46, 0x40, 0xFF);
            break;

        case SC_PATH_MODE_4:
            /* VSP */
            msWriteByteMask(SC0_5B, _BIT6, _BIT6);
            msWriteByteMask(SC0_5B,     0, _BIT7);
            /* MRW */
            msWriteByteMask(SC6_AC, _BIT1, _BIT1|_BIT0);
            msWriteByteMask(SC6_AC, _BIT2, _BIT3|_BIT2);
            /* OP2 Select */
            msWriteByteMask(SC0_9F,     0, _BIT0);
            /* Clock */
            msWriteByteMask(REG_1E36, 0x20, 0x3F);
            #if (!PanelDualPort && !PANEL_QUAD_PORT) || (PanelTTL) || (ENABLE_RTE && (OD_MODE == OD_MODE_333) && OD_HSCAL_EN)
            msWriteByteMask( REG_1E37, 0x2C, 0x3F );
            #else
            msWriteByteMask( REG_1E37, 0x28, 0x3F );
            #endif
            msWriteByteMask(REG_1E40, 0x80, 0xF0);
            msWriteByteMask(REG_1E44, 0x00, 0x0F);
            msWriteByteMask(REG_1E46, 0x40, 0xFF);
            break;

        case SC_PATH_MODE_5:
            /* VSP */
            msWriteByteMask(SC0_5B,     0, _BIT6);
            msWriteByteMask(SC0_5B, _BIT7, _BIT7);
            /* MRW */
            msWriteByteMask(SC6_AC, _BIT0, _BIT1|_BIT0);
            msWriteByteMask(SC6_AC, _BIT3, _BIT3|_BIT2);
            /* OP2 Select */
            msWriteByteMask(SC0_9F,     0, _BIT0);
            /* Clock */
            msWriteByteMask(REG_1E36, 0x20, 0x3F);
            #if (!PanelDualPort && !PANEL_QUAD_PORT) || (PanelTTL) || (ENABLE_RTE && (OD_MODE == OD_MODE_333) && OD_HSCAL_EN)
            msWriteByteMask( REG_1E37, 0x2C, 0x3F );
            #else
            msWriteByteMask( REG_1E37, 0x28, 0x3F );
            #endif
            msWriteByteMask(REG_1E40, 0x00, 0xF0);
            msWriteByteMask(REG_1E44, 0x04, 0x0F);
            msWriteByteMask(REG_1E46, 0x44, 0xFF);
            break;

        case SC_PATH_MODE_6:
            /* VSP */
            msWriteByteMask(SC0_5B,     0, _BIT6);
            msWriteByteMask(SC0_5B, _BIT7, _BIT7);
            /* MRW */
            msWriteByteMask(SC6_AC,       _BIT0, _BIT1|_BIT0);
            msWriteByteMask(SC6_AC, _BIT3|_BIT2, _BIT3|_BIT2);
            /* OP2 Select */
            msWriteByteMask(SC0_9F,     0, _BIT0);
            /* Clock */
            msWriteByteMask(REG_1E36, 0x20, 0x3F);
            #if (!PanelDualPort && !PANEL_QUAD_PORT) || (PanelTTL) || (ENABLE_RTE && (OD_MODE == OD_MODE_333) && OD_HSCAL_EN)
            msWriteByteMask( REG_1E37, 0x2C, 0x3F );
            #else
            msWriteByteMask( REG_1E37, 0x28, 0x3F );
            #endif
            msWriteByteMask(REG_1E40, 0x00, 0xF0);
            msWriteByteMask(REG_1E44, 0x04, 0x0F);
            msWriteByteMask(REG_1E46, 0x44, 0xFF);
            break;

        default:
            MSSCALER_printMsg("Unimplemented SC PATH MODE");
            break;
    }

#if (!PanelDualPort && !PANEL_QUAD_PORT) || (PanelTTL) || (ENABLE_RTE && (OD_MODE == OD_MODE_333) && OD_HSCAL_EN)
    msWriteByteMask(REG_1E36, 0x24, 0x3F);
    #if 0//(ENABLE_MS_OSDROTATE) // MST8556T_ROTATION
    if( g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_1 )
    {
        msWriteByteMask(REG_1E3F, 0x40, 0x70);
    }
    else
    {
        #if (ENABLE_OSD_ROTATE_CONTROL)
        if( UserPrefOsdRotate)
            msWriteByteMask(REG_1E3F, 0x60, 0x70);
        else
        #endif
            msWriteByteMask(REG_1E3F, 0x70, 0x70);
    }
    #endif
    msWriteByteMask(REG_1E41, 0x10, 0x3F);
#endif
}

static void msSetupPathIP()
{
    BOOL bOddPxl;
    BYTE ucBlanking, ucFraction, ucRstOffset, ucDlyLine;
    WORD uwVDE, uwOffset;

    if( ( CURRENT_SOURCE_IS_INTERLACE_MODE() )
     || (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FP)
     || ((g_SetupPathInfo.wImgSizeOutV / g_SetupPathInfo.wImgSizeInV) >=3) )
    {
#if (PanelWidth == 2560)&&(PanelHeight==1440)
        if(!g_SetupPathInfo.bFBMode)
        {
            ucDlyLine = 1;
            msWriteByteMask(SC0_0C, _BIT6|_BIT5, _BIT6|_BIT5|_BIT4); //delay line
        }
        else
#endif
            ucDlyLine = 2;
    }
    else
    {
#if (PanelWidth == 2560)&&(PanelHeight==1440)
        if(!g_SetupPathInfo.bFBMode)
        {
            msWriteByteMask(SC0_0C, _BIT5, _BIT6|_BIT5|_BIT4); //delay line
        }
#endif
        ucDlyLine = 1;
    }

    ucRstOffset = SC_LOCK_LINE - ucDlyLine;

    // 3D LR
    if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_TB )
    {
        msWriteByteMask(SC0_A1, _BIT4|_BIT3, _BIT5|_BIT4|_BIT3|_BIT2);
        msWriteByteMask(SC0_A5, IP_3D_LR_FRAME_DET, _BIT2|_BIT1|_BIT0);
    }
    else if( g_SetupPathInfo.ucIPLRSel == IP_3D_LR_FRAME_DET )
        msWriteByteMask(SC0_A1, _BIT5|_BIT4|_BIT2, _BIT5|_BIT4|_BIT3|_BIT2);

    // chess board
    msWriteBit(SC0_A6, (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_CB), _BIT2);

    // dummy
    msWriteBit(SC0_A6, FALSE, _BIT3);
    msWriteBit(REG_2E6D, FALSE, _BIT5);
    bOddPxl = g_SetupPathInfo.wImgSizeInH & _BIT0;
    if( bOddPxl )
    {
        if( IS_FMT_IN_SBS() )
        {
            msWriteBit(SC0_A6, TRUE, _BIT3);
            msWriteBit(REG_2E6D, TRUE, _BIT5);
            msWrite2ByteMask(REG_2E6C, g_SetupPathInfo.wImgSizeInH + 1, 0x1FFF);
        }
        else if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_CB )
        {
            msWriteBit(SC0_A6, TRUE, _BIT3);
        }
    }

    //  frame packing
    msWrite2Byte(REG_2E40, 0x0000);
    msWrite2Byte(REG_2E42, 0x0000);
    msWrite2Byte(REG_2E44, 0x0000);
    msWrite2Byte(REG_2E46, 0x0000);
    msWrite2Byte(REG_2E48, 0x0000);
    msWrite2Byte(REG_2E62, 0x0000);
    msWrite2Byte(REG_2E64, 0x0000);
    msWrite2Byte(REG_2E66, 0x0000);
    msWrite2Byte(REG_2E68, 0x0000);

    msWriteByteMask(SC0_90, (ucDlyLine)<<4, _BIT5|_BIT4); //delay line
#if ENABLE_FREESYNC
	msSetupDelayLine();
#else
	msWriteByteMask(SC0_0F, (SC_LOCK_LINE), 0x0F); //Ylock line
#endif
    msWriteByteMask(SC0_9F, 0, _BIT4);
    uwVDE = g_SetupPathInfo.wImgSizeInV;
    if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FP)
     || (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FA) )

    {
        ucBlanking = (StandardModeVTotal - StandardModeHeight)/2;//(STD_MODE_V_TOTAL - STD_MODE_HEIGHT)/2;

        msWrite2ByteMask(REG_2E40, uwVDE + ucBlanking + 1, 0x0FFF); //start
        uwOffset = uwVDE + 1;
        msWrite2ByteMask(REG_2E42, uwOffset, 0x0FFF); //end
        msWriteBit(REG_2E41, TRUE, _BIT4);
        msWrite2ByteMask(REG_2E64, uwOffset + ucRstOffset, 0x0FFF); // frame reset setting
        msWriteBit(REG_2E65, TRUE, _BIT4);

        msWriteByteMask(SC0_9F, _BIT4, _BIT4);
    }
    else if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FPI)
    {
        ucBlanking = (StandardModeVTotal - StandardModeHeight)/4;
        ucFraction = ((StandardModeVTotal - StandardModeHeight)/2) & _BIT0;

        msWrite2ByteMask(REG_2E40, uwVDE*3 + 3*ucBlanking + ucFraction + 1, 0x0FFF); //start
        uwOffset = uwVDE*3 + 2*ucBlanking + ucFraction + 1;
        msWrite2ByteMask(REG_2E42, uwOffset, 0x0FFF); //end
        msWriteBit(REG_2E41, TRUE, _BIT4);
        msWrite2ByteMask(REG_2E64, uwOffset + ucRstOffset, 0x0FFF); // frame reset setting
        msWriteBit(REG_2E65, TRUE, _BIT4);

        msWrite2ByteMask(REG_2E44, uwVDE*2 + 2*ucBlanking + ucFraction + 1, 0x0FFF); //start
        uwOffset = uwVDE*2 + ucBlanking + 1;
        msWrite2ByteMask(REG_2E46, uwOffset, 0x0FFF); //end
        msWriteBit(REG_2E45, TRUE, _BIT4);
        msWrite2ByteMask(REG_2E66, uwOffset + ucRstOffset, 0x0FFF); // frame reset setting
        msWriteBit(REG_2E67, TRUE, _BIT4);

        msWrite2ByteMask(REG_2E48, uwVDE + ucBlanking + 1, 0x0FFF); //start
        uwOffset = uwVDE + 1;
        msWrite2ByteMask(REG_2E62, uwOffset, 0x0FFF); //end
        msWriteBit(REG_2E49, TRUE, _BIT4);
        msWrite2ByteMask(REG_2E68, uwOffset + ucRstOffset, 0x0FFF); // frame reset setting
        msWriteBit(REG_2E69, TRUE, _BIT4);

        msWriteByteMask(SC0_9F, _BIT4, _BIT4);
    }
}

static void msSetupPathHSP()
{
    msWrite2ByteMask(SC0_B3, g_SetupPathInfo.wImgSizeInH, 0xFFF);
    msWrite2ByteMask(SC0_B5, g_SetupPathInfo.wImgSizeOutH, 0xFFF);

    if( IS_FMT_IN_SBS()
     && (g_SetupPathInfo.ucSCPathMode != SC_PATH_MODE_3)
     && (g_SetupPathInfo.ucSCPathMode != SC_PATH_MODE_5) )
        msWriteByteMask(SC0_B4, _BIT7, _BIT7);
    else
        msWriteByteMask(SC0_B4, 0, _BIT7);

    if( g_SetupPathInfo.bHSDEnable )
        msWriteByteMask(SC0_B6, _BIT7, _BIT7);
    else
        msWriteByteMask(SC0_B6, 0, _BIT7);

    if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_CB )
    {
        if( g_SetupPathInfo.bHSDEnable )
            msWriteByteMask(SC0_B6, _BIT6, _BIT6);
        else
            msWriteByteMask(SC0_B6, 0, _BIT6);
        if( g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_4 )
            msWriteByteMask(SC0_B4, _BIT6, _BIT6);
    }
    else
    {
        msWriteByteMask(SC0_B4, 0, _BIT6);
        msWriteByteMask(SC0_B6, 0, _BIT6);
    }
}

static void msSetupPathVSP()
{
    msWrite2ByteMask(SC0_70, g_SetupPathInfo.wMRWSizeOutV, 0xFFF);
    if( IS_FMT_IN_SBS() )
    {
        msWrite2ByteMask(SC0_72, g_SetupPathInfo.wMRWSizeInH, 0xFFF);
        msWrite2ByteMask(SC0_74, g_SetupPathInfo.wMRWSizeInV / 2, 0xFFF);
    }
    else
    {
        msWrite2ByteMask(SC0_72, g_SetupPathInfo.wMRWSizeInH / 2, 0xFFF);
        msWrite2ByteMask(SC0_74, g_SetupPathInfo.wMRWSizeInV, 0xFFF);
    }

    msWriteByteMask(SC0_5B, 0, _BIT1|_BIT0);
    switch(g_SetupPathInfo.ucSCPathMode)
    {
        case SC_PATH_MODE_3:
        case SC_PATH_MODE_4:
            if( IS_FMT_OUT_PSV() && (g_SetupPathInfo.ucSCFmtIn != SC_FMT_IN_NORMAL) )
                msWriteByteMask(SC0_5B, _BIT1, _BIT1|_BIT0);
            break;

        case SC_PATH_MODE_5:
        case SC_PATH_MODE_6:
            if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_TB )
            {
                msWrite2ByteMask(SC0_74, g_SetupPathInfo.wMRWSizeInV, 0xFFF);
                msWriteByteMask(SC0_5B, _BIT0, _BIT1|_BIT0);
            }
            else if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_LA )
            {
                msWriteByteMask(SC0_5B, _BIT1, _BIT1|_BIT0);
            }
            break;

        default: break;
    }
}

static void msSetupPathMRWBaseRD()
{
    // MRW Read Base Address
    BYTE u8Bank=msReadByte(SC0_00);
    msWriteByte(SC0_00, 0x06);

    if( IS_FMT_OUT_PSV()
     && (!IS_MRW_FMT_LA())
     && (g_SetupPathInfo.ucSCFmtIn != SC_FMT_IN_3D_CB)
     && (g_SetupPathInfo.ucSCFmtIn != SC_FMT_IN_NORMAL) )
    {
        msWrite3Byte(MRW_L_RD_LSB_0, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_0));
        msWrite3Byte(MRW_L_RD_LSB_2, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_1));
        msWrite3Byte(MRW_L_RD_LSB_4, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_2));
        msWrite3Byte(MRW_L_RD_LSB_6, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_3));
        msWrite3Byte(MRW_L_RD_MSB_0, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_0));
        msWrite3Byte(MRW_L_RD_MSB_2, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_1));
        msWrite3Byte(MRW_L_RD_MSB_4, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_2));
        msWrite3Byte(MRW_L_RD_MSB_6, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_3));
    }
    else
    {
        if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL)
         || (g_SetupPathInfo.ucOut3DLRSel == OUT_3D_FIXED_NORMAL) )
        {
            msWrite3Byte(MRW_L_RD_LSB_0, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_0));
            msWrite3Byte(MRW_L_RD_LSB_1, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_1));
            msWrite3Byte(MRW_L_RD_LSB_2, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_2));
            msWrite3Byte(MRW_L_RD_LSB_3, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_3));
            msWrite3Byte(MRW_L_RD_LSB_4, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_4));
            msWrite3Byte(MRW_L_RD_LSB_5, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_5));
            msWrite3Byte(MRW_L_RD_LSB_6, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_6));
            msWrite3Byte(MRW_L_RD_LSB_7, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_7));
            msWrite3Byte(MRW_L_RD_MSB_0, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_0));
            msWrite3Byte(MRW_L_RD_MSB_1, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_1));
            msWrite3Byte(MRW_L_RD_MSB_2, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_2));
            msWrite3Byte(MRW_L_RD_MSB_3, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_3));
            msWrite3Byte(MRW_L_RD_MSB_4, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_4));
            msWrite3Byte(MRW_L_RD_MSB_5, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_5));
            msWrite3Byte(MRW_L_RD_MSB_6, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_6));
            msWrite3Byte(MRW_L_RD_MSB_7, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_7));
        }
        else if( g_SetupPathInfo.ucOut3DLRSel == OUT_3D_FIXED_L )
        {
            msWrite3Byte(MRW_L_RD_LSB_0, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_0));
            msWrite3Byte(MRW_L_RD_LSB_1, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_0));
            msWrite3Byte(MRW_L_RD_LSB_2, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_2));
            msWrite3Byte(MRW_L_RD_LSB_3, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_2));
            msWrite3Byte(MRW_L_RD_LSB_4, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_4));
            msWrite3Byte(MRW_L_RD_LSB_5, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_4));
            msWrite3Byte(MRW_L_RD_LSB_6, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_6));
            msWrite3Byte(MRW_L_RD_LSB_7, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_6));
            msWrite3Byte(MRW_L_RD_MSB_0, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_0));
            msWrite3Byte(MRW_L_RD_MSB_1, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_0));
            msWrite3Byte(MRW_L_RD_MSB_2, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_2));
            msWrite3Byte(MRW_L_RD_MSB_3, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_2));
            msWrite3Byte(MRW_L_RD_MSB_4, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_4));
            msWrite3Byte(MRW_L_RD_MSB_5, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_4));
            msWrite3Byte(MRW_L_RD_MSB_6, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_6));
            msWrite3Byte(MRW_L_RD_MSB_7, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_6));
        }
        else if( g_SetupPathInfo.ucOut3DLRSel == OUT_3D_FIXED_R )
        {
            msWrite3Byte(MRW_L_RD_LSB_0, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_1));
            msWrite3Byte(MRW_L_RD_LSB_1, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_1));
            msWrite3Byte(MRW_L_RD_LSB_2, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_3));
            msWrite3Byte(MRW_L_RD_LSB_3, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_3));
            msWrite3Byte(MRW_L_RD_LSB_4, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_5));
            msWrite3Byte(MRW_L_RD_LSB_5, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_5));
            msWrite3Byte(MRW_L_RD_LSB_6, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_7));
            msWrite3Byte(MRW_L_RD_LSB_7, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_7));
            msWrite3Byte(MRW_L_RD_MSB_0, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_1));
            msWrite3Byte(MRW_L_RD_MSB_1, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_1));
            msWrite3Byte(MRW_L_RD_MSB_2, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_3));
            msWrite3Byte(MRW_L_RD_MSB_3, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_3));
            msWrite3Byte(MRW_L_RD_MSB_4, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_5));
            msWrite3Byte(MRW_L_RD_MSB_5, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_5));
            msWrite3Byte(MRW_L_RD_MSB_6, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_7));
            msWrite3Byte(MRW_L_RD_MSB_7, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_7));
        }
    }
    msWriteByte(SC0_00,u8Bank);
}

static void msSetupPathMRWBaseWR()
{
    DWORD wOffsetMSB, wOffsetLSB;
    BYTE u8Bank=msReadByte(SC0_00);
    msWriteByte(SC0_00,0x06);

    if( (!IS_MRW_FMT_LA())
     && (g_SetupPathInfo.ucSCFmtIn != SC_FMT_IN_NORMAL)
     && (IS_FMT_OUT_PSV()) )
    {
        /* MRW Format Output Line Interleave */
        if( (g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_3)
         || (g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_5) )
        {
            wOffsetLSB = MIU_ADDR_OFFSET(g_SetupPathInfo.wImgSizeInH);
            wOffsetMSB = MIU_ADDR_OFFSET(g_SetupPathInfo.wImgSizeInH * 2);
        }
        else
        {
            wOffsetLSB = MIU_ADDR_OFFSET(g_SetupPathInfo.wImgSizeOutH);
            wOffsetMSB = MIU_ADDR_OFFSET(g_SetupPathInfo.wImgSizeOutH * 2);
        }


        if( g_SetupPathInfo.bMRWDual )
        {
                /* LEFT */
                msWrite3Byte(MRW_L_WR_MSB_0, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_0));
                msWrite3Byte(MRW_L_WR_MSB_1, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_1));
                msWrite3Byte(MRW_L_WR_MSB_2, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_2));
                msWrite3Byte(MRW_L_WR_MSB_3, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_3));
                msWrite3Byte(MRW_L_WR_LSB_0, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_0));
                msWrite3Byte(MRW_L_WR_LSB_1, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_1));
                msWrite3Byte(MRW_L_WR_LSB_2, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_2));
                msWrite3Byte(MRW_L_WR_LSB_3, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_3));

                /* RIGHT */
                msWrite3Byte(MRW_R_WR_MSB_0, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_0));
                msWrite3Byte(MRW_R_WR_MSB_1, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_1));
                msWrite3Byte(MRW_R_WR_MSB_2, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_2));
                msWrite3Byte(MRW_R_WR_MSB_3, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_3));
                msWrite3Byte(MRW_R_WR_LSB_0, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_0));
                msWrite3Byte(MRW_R_WR_LSB_1, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_1));
                msWrite3Byte(MRW_R_WR_LSB_2, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_2));
                msWrite3Byte(MRW_R_WR_LSB_3, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_3));
        }
        else
        {
            if(g_SetupPathInfo.bPsvPnlOutLRInvert)
            {
                /* LEFT */
                msWrite3Byte(MRW_L_WR_MSB_1, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_0));
                msWrite3Byte(MRW_L_WR_MSB_0, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_0) + wOffsetMSB);
                msWrite3Byte(MRW_L_WR_MSB_3, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_1));
                msWrite3Byte(MRW_L_WR_MSB_2, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_1) + wOffsetMSB);
                msWrite3Byte(MRW_L_WR_MSB_5, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_2));
                msWrite3Byte(MRW_L_WR_MSB_4, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_2) + wOffsetMSB);
                msWrite3Byte(MRW_L_WR_MSB_7, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_3));
                msWrite3Byte(MRW_L_WR_MSB_6, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_3) + wOffsetMSB);
                msWrite3Byte(MRW_L_WR_LSB_1, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_0));
                msWrite3Byte(MRW_L_WR_LSB_0, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_0) + wOffsetLSB);
                msWrite3Byte(MRW_L_WR_LSB_3, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_1));
                msWrite3Byte(MRW_L_WR_LSB_2, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_1) + wOffsetLSB);
                msWrite3Byte(MRW_L_WR_LSB_5, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_2));
                msWrite3Byte(MRW_L_WR_LSB_4, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_2) + wOffsetLSB);
                msWrite3Byte(MRW_L_WR_LSB_7, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_3));
                msWrite3Byte(MRW_L_WR_LSB_6, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_3) + wOffsetLSB);
            }
            else
            {
                /* LEFT */
                msWrite3Byte(MRW_L_WR_MSB_0, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_0));
                msWrite3Byte(MRW_L_WR_MSB_1, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_0) + wOffsetMSB);
                msWrite3Byte(MRW_L_WR_MSB_2, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_1));
                msWrite3Byte(MRW_L_WR_MSB_3, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_1) + wOffsetMSB);
                msWrite3Byte(MRW_L_WR_MSB_4, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_2));
                msWrite3Byte(MRW_L_WR_MSB_5, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_2) + wOffsetMSB);
                msWrite3Byte(MRW_L_WR_MSB_6, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_3));
                msWrite3Byte(MRW_L_WR_MSB_7, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_3) + wOffsetMSB);
                msWrite3Byte(MRW_L_WR_LSB_0, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_0));
                msWrite3Byte(MRW_L_WR_LSB_1, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_0) + wOffsetLSB);
                msWrite3Byte(MRW_L_WR_LSB_2, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_1));
                msWrite3Byte(MRW_L_WR_LSB_3, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_1) + wOffsetLSB);
                msWrite3Byte(MRW_L_WR_LSB_4, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_2));
                msWrite3Byte(MRW_L_WR_LSB_5, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_2) + wOffsetLSB);
                msWrite3Byte(MRW_L_WR_LSB_6, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_3));
                msWrite3Byte(MRW_L_WR_LSB_7, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_3) + wOffsetLSB);
            }
        }

        if(g_SetupPathInfo.bPsvPnlOutLRInvert && g_SetupPathInfo.bMRWDual)
        {

            msWrite2Byte(MRW_L_MSB_OFFSET_LINE, 0);
            msWrite2Byte(MRW_L_LSB_OFFSET_LINE, 0);
            msWrite2Byte(MRW_L_MSB_OFFSET_ACT, wOffsetMSB);
            msWrite2Byte(MRW_L_LSB_OFFSET_ACT, wOffsetLSB);

            msWrite2Byte(MRW_R_MSB_OFFSET_LINE, wOffsetMSB);
            msWrite2Byte(MRW_R_LSB_OFFSET_LINE, wOffsetLSB);
            msWrite2Byte(MRW_R_MSB_OFFSET_ACT, 0);
            msWrite2Byte(MRW_R_LSB_OFFSET_ACT, 0);

        }
        else
        {
            msWrite2Byte(MRW_L_MSB_OFFSET_LINE, wOffsetMSB);
            msWrite2Byte(MRW_L_LSB_OFFSET_LINE, wOffsetLSB);
            msWrite2Byte(MRW_L_MSB_OFFSET_ACT, 0);
            msWrite2Byte(MRW_L_LSB_OFFSET_ACT, 0);

            msWrite2Byte(MRW_R_MSB_OFFSET_LINE, 0);
            msWrite2Byte(MRW_R_LSB_OFFSET_LINE, 0);
            msWrite2Byte(MRW_R_MSB_OFFSET_ACT, wOffsetMSB);
            msWrite2Byte(MRW_R_LSB_OFFSET_ACT, wOffsetLSB);
        }
    }
    else
    {
        if(IS_FMT_OUT_PSV()
           && (g_SetupPathInfo.ucSCFmtIn != SC_FMT_IN_NORMAL) )
        {
            wOffsetLSB = MIU_ADDR_OFFSET(g_SetupPathInfo.wImgSizeInH);
            wOffsetMSB = MIU_ADDR_OFFSET(g_SetupPathInfo.wImgSizeInH * 2);
        }
        else
        {
            wOffsetLSB = 0;
            wOffsetMSB = 0;
        }

        if( g_SetupPathInfo.bMRWDual )
        {
            if(IS_FMT_OUT_PSV()) //LA image in and Passive out
            {
                /* LEFT */
                msWrite3Byte(MRW_L_WR_MSB_0, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_0));
                msWrite3Byte(MRW_L_WR_MSB_1, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_1));
                msWrite3Byte(MRW_L_WR_MSB_2, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_2));
                msWrite3Byte(MRW_L_WR_MSB_3, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_3));
                msWrite3Byte(MRW_L_WR_LSB_0, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_0));
                msWrite3Byte(MRW_L_WR_LSB_1, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_1));
                msWrite3Byte(MRW_L_WR_LSB_2, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_2));
                msWrite3Byte(MRW_L_WR_LSB_3, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_3));

                /* RIGHT */
                msWrite3Byte(MRW_R_WR_MSB_0, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_0));
                msWrite3Byte(MRW_R_WR_MSB_1, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_1));
                msWrite3Byte(MRW_R_WR_MSB_2, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_2));
                msWrite3Byte(MRW_R_WR_MSB_3, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_3));
                msWrite3Byte(MRW_R_WR_LSB_0, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_0));
                msWrite3Byte(MRW_R_WR_LSB_1, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_1));
                msWrite3Byte(MRW_R_WR_LSB_2, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_2));
                msWrite3Byte(MRW_R_WR_LSB_3, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_3));
            }
            else
            {
                /* LEFT */
                msWrite3Byte(MRW_L_WR_MSB_0, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_0));
                msWrite3Byte(MRW_L_WR_MSB_1, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_2));
                msWrite3Byte(MRW_L_WR_MSB_2, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_4));
                msWrite3Byte(MRW_L_WR_MSB_3, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_6));
                msWrite3Byte(MRW_L_WR_LSB_0, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_0));
                msWrite3Byte(MRW_L_WR_LSB_1, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_2));
                msWrite3Byte(MRW_L_WR_LSB_2, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_4));
                msWrite3Byte(MRW_L_WR_LSB_3, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_6));

                /* RIGHT */
                msWrite3Byte(MRW_R_WR_MSB_0, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_1));
                msWrite3Byte(MRW_R_WR_MSB_1, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_3));
                msWrite3Byte(MRW_R_WR_MSB_2, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_5));
                msWrite3Byte(MRW_R_WR_MSB_3, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_7));
                msWrite3Byte(MRW_R_WR_LSB_0, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_1));
                msWrite3Byte(MRW_R_WR_LSB_1, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_3));
                msWrite3Byte(MRW_R_WR_LSB_2, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_5));
                msWrite3Byte(MRW_R_WR_LSB_3, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_7));
            }
        }
        else
        {
            /* LEFT */
            msWrite3Byte(MRW_L_WR_MSB_0, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_0));
            msWrite3Byte(MRW_L_WR_MSB_1, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_1));
            msWrite3Byte(MRW_L_WR_MSB_2, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_2));
            msWrite3Byte(MRW_L_WR_MSB_3, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_3));
            msWrite3Byte(MRW_L_WR_MSB_4, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_4));
            msWrite3Byte(MRW_L_WR_MSB_5, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_5));
            msWrite3Byte(MRW_L_WR_MSB_6, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_6));
            msWrite3Byte(MRW_L_WR_MSB_7, MIU_ADDR_OFFSET(MIU_FB_ADDR_MSB_7));
            msWrite3Byte(MRW_L_WR_LSB_0, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_0));
            msWrite3Byte(MRW_L_WR_LSB_1, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_1));
            msWrite3Byte(MRW_L_WR_LSB_2, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_2));
            msWrite3Byte(MRW_L_WR_LSB_3, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_3));
            msWrite3Byte(MRW_L_WR_LSB_4, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_4));
            msWrite3Byte(MRW_L_WR_LSB_5, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_5));
            msWrite3Byte(MRW_L_WR_LSB_6, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_6));
            msWrite3Byte(MRW_L_WR_LSB_7, MIU_ADDR_OFFSET(MIU_FB_ADDR_LSB_7));
        }

        if(IS_FMT_OUT_PSV() && g_SetupPathInfo.bPsvPnlOutLRInvert )
        {
            msWrite2Byte(MRW_L_MSB_OFFSET_LINE, 0);
            msWrite2Byte(MRW_L_LSB_OFFSET_LINE, 0);
            msWrite2Byte(MRW_L_MSB_OFFSET_ACT, wOffsetMSB);
            msWrite2Byte(MRW_L_LSB_OFFSET_ACT, wOffsetLSB);

            msWrite2Byte(MRW_R_MSB_OFFSET_LINE, wOffsetMSB);
            msWrite2Byte(MRW_R_LSB_OFFSET_LINE, wOffsetMSB);
            msWrite2Byte(MRW_R_MSB_OFFSET_ACT, 0);
            msWrite2Byte(MRW_R_LSB_OFFSET_ACT, 0);
        }
        else
        {
            msWrite2Byte(MRW_L_MSB_OFFSET_LINE, wOffsetMSB);
            msWrite2Byte(MRW_L_LSB_OFFSET_LINE, wOffsetLSB);
            msWrite2Byte(MRW_L_MSB_OFFSET_ACT, 0);
            msWrite2Byte(MRW_L_LSB_OFFSET_ACT, 0);

            msWrite2Byte(MRW_R_MSB_OFFSET_LINE, 0);
            msWrite2Byte(MRW_R_LSB_OFFSET_LINE, 0);
            msWrite2Byte(MRW_R_MSB_OFFSET_ACT, wOffsetMSB);
            msWrite2Byte(MRW_R_LSB_OFFSET_ACT, wOffsetMSB);
        }
    }

    msWrite2Byte(SC6_D2, (MIU_FB_ADDR_END-1)>>12);
    msWriteByte(SC0_00,u8Bank);
}

static void msSetupPathMRW()
{
    BYTE ucReg;
    WORD wReg;
    DWORD wOffset;
    BYTE u8Bank=msReadByte(SC0_00);

    msWriteByte(SC0_00, 0x06);

    // double buffer lock
    msWriteByteMask(SC6_03, _BIT0, _BIT0);

    msSetupPathMRWBaseRD();
    msSetupPathMRWBaseWR();

    ucReg = 0;
    msWriteByte(SC6_A0, 0x00);
    if( g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_FS )
    {
        if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FP)           /* 2Img -> 2Img *//* 2Img -> Line */
            ucReg |= (_BIT0|_BIT2);
#if !(ENABLE_3D_FUNCTION && GLASSES_TYPE==GLASSES_NVIDIA)
        else if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_PF )     /* 2Img -> 2Img */
            ucReg |= _BIT0;
#endif
        else if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FPI )
            ucReg |= _BIT6;
        else if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FA )     /* field alternative -> 2img */
            ucReg |= _BIT5;
        else if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_TB )     /* using bit0 instead of bit4 for correct LR signal, TB -> 2ImG */
            ucReg |= _BIT0;
        else if( IS_MRW_FMT_LA()                                    /* Line -> 2Img */
              || (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_CB) )
            ucReg |= _BIT1;
    }
    else if( IS_FMT_OUT_PSV() )
    {
        if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FP)          /* 2Img -> Line */
         || (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_PF) )
            ucReg |= _BIT2;
        else if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FA)     /* 2Img -> Line *//* field alternative -> 2img */
              || (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FPI))
            ucReg |= (_BIT5|_BIT2);

        if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_TB )
            ucReg |= (_BIT4|_BIT3);
    }

    if( g_SetupPathInfo.bMRWLLRR )
        ucReg = 0;

    msWriteByte(SC6_A0, ucReg);

    msWriteBit(SC6_03, g_SetupPathInfo.bMRWLLRR, _BIT2);

    // write field invert
    if ( IS_FMT_OUT_PSV() && (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FPI) )
        msWriteByteMask(SC6_F5, _BIT4, _BIT4);
    else
        msWriteByteMask(SC6_F5,     0, _BIT4);

    ucReg = 0;
    msWriteByte(SC6_F4, 0x00);
    if( g_SetupPathInfo.bMRWDual )
    {
        if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_CB )
            ucReg |= _BIT0;

        if( IS_MRW_FMT_LA() )
            ucReg |= _BIT1;

        ucReg |= _BIT7;
    }
    ucReg |= ((g_SetupPathInfo.ucSCFrameMode<<4) & 0x70);
    msWriteByte(SC6_F4, ucReg);

    msWrite2Byte(SC6_DC, g_SetupPathInfo.wMRWSizeInH);
    msWrite2Byte(SC6_DE, g_SetupPathInfo.wMRWSizeInV);
    msWrite2Byte(SC6_E0, g_SetupPathInfo.wMRWSizeOutH);
    msWrite2Byte(SC6_E2, g_SetupPathInfo.wMRWSizeOutV);

    msWrite2Byte(SC6_AE, g_SetupPathInfo.wMRWSizeOutH);
    msWrite2Byte(SC6_B0, g_SetupPathInfo.wMRWSizeOutH);

    if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_TB )
    {
        wOffset = MIU_ADDR_OFFSET(((DWORD)g_SetupPathInfo.wMRWSizeInH * g_SetupPathInfo.wMRWSizeInV / 2));
        if( g_SetupPathInfo.bMRW422 )
            msWrite3Byte(SC6_A4, 0);
        else
            msWrite3Byte(SC6_A4, wOffset);
        msWrite3Byte(SC6_A8, wOffset*2);
        msWriteByteMask(SC6_A6, _BIT7, _BIT7);
    }
    else
    {
        msWriteByte(SC6_A6, 0);
    }

    if( g_SetupPathInfo.bFBMode )
        msWriteByteMask(SC6_02, _BIT3, _BIT3);
    else
        msWriteByteMask(SC6_02, 0, _BIT3);

    // 422 format
    wReg = 0;
    if( g_SetupPathInfo.bMRW422 )
    {
        if( g_SetupPathInfo.bMRWR2Y )
            wReg |= _BIT7;

        wReg |= (_BIT14|_BIT8);

        // TBD. enable Y2R 3x3 setting
    }
    msWrite2ByteMask(SC6_A2, wReg, _BIT14|_BIT13|_BIT8|_BIT7);

    // near end
    if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_PF) // PF 120Hz in, FS 120Hz out with VBI
     && (g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_FS)
     && ((VFreq( HFreq( SrcHPeriod ), SrcVTotal ))>=990) )
        msWrite2Byte(SC6_D0, 0x0A);
    else if( (g_SetupPathInfo.bMRWDual)
     && (g_SetupPathInfo.ucSCFmtIn != SC_FMT_IN_3D_CB) )
        msWrite2Byte(SC6_D0, (g_SetupPathInfo.wMRWSizeInV/2 - 1));
    else
        msWrite2Byte(SC6_D0, (g_SetupPathInfo.wMRWSizeInV - 1));

    // lock disable
    msWriteByteMask(SC6_03, 0, _BIT0);

    // reset
    msWriteByteMask(SC6_02, _BIT1, _BIT1);
    Delay1ms(50);
    msWriteByteMask(SC6_02, 0, _BIT1);
    msWriteByte(SC0_00,u8Bank);
}

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
#if 0//ENABLE_DEBUG
static void msSetupPathDbgDump(BYTE ucVaule)
{
ucVaule=ucVaule;
    MSSCALER_printData("===========\r\nmsSetupPath(%d)" , ucVaule);
    MSSCALER_printData("wOriSizeH = %d", g_SetupPathInfo.wOriSizeH);
    MSSCALER_printData("wOriSizeV = %d", g_SetupPathInfo.wOriSizeV);
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
    MSSCALER_printData("ucSCFrameMode = %d", g_SetupPathInfo.ucSCFrameMode);
    MSSCALER_printData("bMRWDual = %d", g_SetupPathInfo.bMRWDual);
    MSSCALER_printData("bVBIExtend = %d", g_SetupPathInfo.bVBIExtend);
    MSSCALER_printData("bHSDEnable = %d", g_SetupPathInfo.bHSDEnable);
    MSSCALER_printData("bFBMode = %d", g_SetupPathInfo.bFBMode);
}
#endif
static void msSetupPathOD()
{
#if (ENABLE_RTE && (OD_MODE != OD_MODE_333))
    BYTE ucBank = msReadByte(REG_3100);
    msWriteByte(REG_3100, 0x00);

    if( (g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_1)
     && (OD_MODE == OD_MODE_565) )
    {
        msWriteByte(REG_3191,0x00); // Base Address limit for Odd
        msWriteByte(REG_3192,0x00);
        msWriteByte(REG_3193,0x00);

        msWriteByte(REG_312E,0x00); // Base Address limit for Even
        msWriteByte(REG_312F,0x00);
        msWriteByte(REG_3130,0xFE);

        msWriteByte(REG_31B0,0xFF);

        msWriteByteMask(REG_31E6, _BIT4, _BIT4|_BIT3|_BIT1|_BIT0);
        msWriteByteMask(REG_31B7, _BIT4|_BIT2, _BIT4|_BIT2);

        msWrite2Byte(REG_1248, 0xFE3F);
    }
    else
    {
        msWriteByte(REG_3191,0xFF);
        msWriteByte(REG_3192,0xFF);
        msWriteByte(REG_3193,0xFB);

        msWriteByte(REG_312E,0x00);
        msWriteByte(REG_312F,0x00);
        msWriteByte(REG_3130,0xFE);

        msWriteByte(REG_31B0,0x60);

        msWriteByteMask(REG_31B7, 0, _BIT4|_BIT2);
        msWriteByteMask(REG_31E6, 0, _BIT4|_BIT3|_BIT1|_BIT0);

        msWrite2Byte(REG_1248, 0xF8FF);
    }

    msWriteByte(REG_3100, ucBank);
#endif

#if ENABLE_RTE
    if( UserprefOverDriveSwitch)
        msOverDriveOnOff( TRUE );
    else
        msOverDriveOnOff( FALSE );
#endif
}

void mStar_SetupPath()
{
    BYTE u8Org=mStar_ScalerDoubleBuffer(FALSE);
    msSetupPathParam();
    msSetupPathConfig();
    msSetupPathIP();
    msSetupPathHSP();
    msSetupPathVSP();
    msSetupPathMRW();
    msSetupPathOD();
#if 0//ENABLE_DEBUG
    msSetupPathDbgDump(2);
#endif
    mStar_SetScalingFactor();
    mStar_SetScalingFilter();
    mStar_ScalerDoubleBuffer(u8Org&BIT0);
}

void mStar_SetupPathInit()
{
    g_SetupPathInfo.bHSDEnable = 0;
    g_SetupPathInfo.ucOut3DLRSel = OUT_3D_FIXED_NORMAL;

#if (FRAME_BFF_SEL == FRAME_BFFLESS)
    g_SetupPathInfo.bFBMode = 0;
    g_SetupPathInfo.ucSCPathMode = SC_PATH_MODE_0;
    msSetupPathConfig();
#else
    g_SetupPathInfo.bFBMode = 1;
#endif

    g_SetupPathInfo.bOverrideSCFmtIn = 0;
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
    g_SetupPathInfo.bPsvPnlOutLRInvert = 1;
#elif PANEL_3D_PASSIVE_4M
    g_SetupPathInfo.ucSCFmtOut = SC_FMT_OUT_3D_PSV_4M;
    g_SetupPathInfo.bPsvPnlOutLRInvert = 1;
#else
    g_SetupPathInfo.ucSCFmtOut = SC_FMT_OUT_3D_FS;
    g_SetupPathInfo.bPsvPnlOutLRInvert = 0;
#endif

    g_SetupPathInfo.wImgSizeOutH = PANEL_WIDTH;
    g_SetupPathInfo.wImgSizeOutV = PANEL_HEIGHT;
    g_SetupPathInfo.b4MFRC = 0;
    msSetupPathMRWInit();

}
#endif
