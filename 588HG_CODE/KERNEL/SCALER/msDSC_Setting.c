
#include <math.h>
#include "types.h"
#include "board.h"
#include "panel.h"
#include "ms_rwreg.h"
#include "NVRAM.h"
#include "Mode.h"
#include "UserPrefDef.h"
#include "global.h"
#include "misc.h"
#include "menudef.h"
#include "menufunc.h"
#include "debug.h"
#include "userpref.h"
#include "msOSD.h"
#include "msDSC_Handler.h"
#include "msDSC_Setting.h"
#include "msDSC_RegSetting.h"
#include "MsACE.h"
#include "Adjust.h"
#include "MsDlc.h"
#include "Common.h"

//===================================================================================================================================================================:
//#include "msDPS.h"
//===================================================================================================================================================================:
#if ENABLE_DSC

#define  DSC_DEBUG  1
#if (DSC_DEBUG&&ENABLE_DEBUG)
#define DSC_printData(str, value)   printData(str, value)
#define DSC_printMsg(str)               printMsg(str)
#else
#define DSC_printData(str, value)
#define DSC_printMsg(str)
#endif
//#if ENABLE_DPS
//extern xdata BOOL  REG_DPS_RUN_Flag =0;
//#endif
#if !SHARE_DLC_XDATA
xdata WORD  ISR_SatHistogram32H[32]; // >>6
#endif
xdata BOOL  REG_DSC_RUN_Flag =0;

xdata BYTE  ISR_preDSCLoopCount = 0;
xdata BYTE  ISR_curDSCLoopCount = 0;
xdata BYTE  u8Win = 0;
xdata BYTE  Flag_DSC_Latch_Busy = 0;
xdata WORD  CurxvYgain = 0x400;

//-------------------------------------------------------------------------------------------------
void msAdjustPCXvyccContrast( BYTE bScalerWin, WORD uwContrast );
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//  Local Constant
//-------------------------------------------------------------------------------------------------
#if 1
// matrix used to convert RGB color space to YUV color space for HDTV
code short t_SDTVRgb2yuv[3][3] =
{
//<<11
    {  0x0264,    0x04B2,   0x00E9 },   //  0.299     0.587  0.114
    { -0x0160, -0x02B6,   0x0416 },     // -0.172   -0.339   0.511
    {  0x0416,  -0x036C, -0x00A9 }      //  0.511   -0.428  -0.083

// <<10
/*
    {  0x00DA,    0x02DC,   0x004A },   //  0.299     0.587  0.114
    { -0x0078, -0x0193,   0x020B },     // -0.172   -0.339   0.511
    {  0x020B,  -0x01DB, -0x0030 }      //  0.511   -0.428  -0.083
  */
};
code short t_SDTVYuv2rgb[3][3] =
{
//<<11
    {    0x0800,  -0x0000,  0x0AF9 },     // 1  -0.0003  1.3717
    {    0x0800, -0x02B1 ,-0x0596},     //  1   -0.3365 -0.6984
    {    0x0800,   0x0DDE ,-0x0003}      //  1   1.7336 -0.0016

   /* <<10
    {    0x0400,  -0x0000,  0x057C },     // 1  -0.0003  1.3717
    {    0x0400, -0x0158 ,-0x02CB},     //  1   -0.3365 -0.6984
    {    0x0400,   0x06EF ,-0x0001}      //  1   1.7336 -0.0016
   */
};
#else
// matrix used to convert RGB color space to YUV color space for HDTV
code short t_SDTVRgb2yuv[3][3] =
{
    {  0x00DA,  0x02DC,  0x004A }, // 0.213,  0.715,  0.072
    { -0x0078, -0x0193,  0x020B }, // -0.117, -0.394, 0.511
    {  0x020B, -0x01DB, -0x0030 }  // 0.511,  -0.464, -0.047
};
#if 0
// matrix used to convert YUV color space to RGB color space, used for video input for HDTV
code short t_SDTVYuv2rgb[3][3] =
{
    {   0x072C, 0x04A8,  0x0000 },     // 1.793,  1.164, 0
    {  -0x0223, 0x04A8, -0x00DA },     // -0.534, 1.164, -0.213
    {   0x0000, 0x04A8,  0x0876 }      // 0,      1.164, 2.115
};
#else
code short t_SDTVYuv2rgb[3][3] =
{
    {    0x0400,  0x0000,0x072C },     // 1.793,  1.164, 0
    {    0x0400, -0x00DA ,-0x0223},     // -0.534, 1.164, -0.213
    {    0x0400,  0x0876 ,0x0000}      // 0,      1.164, 2.115
};
#endif
#endif
//===================================================================================================================================================================:
//===================================================================================================================================================================:
void ArrayMultiply(short sFirst[3][3], short sSecond[3][3], short sResult[3][3], int Res)
{
    BYTE LOCAL_TYPE ucRow, ucCol;
    for(ucRow=0; ucRow!=3; ucRow++)
    {
        for(ucCol=0; ucCol!=3; ucCol++)
        {
            sResult[ucRow][ucCol] = (((long)sFirst[ucRow][0] * sSecond[0][ucCol]) +
                                     ((long)sFirst[ucRow][1] * sSecond[1][ucCol]) +
                                     ((long)sFirst[ucRow][2] * sSecond[2][ucCol])) >> Res;
       }
    }
}
//===================================================================================================================================================================:
//===================================================================================================================================================================:
#if DSC_UsexvYccColorMatrix_EN
void msWritexvYCCColorMatrix(BYTE bScalerWin, short sResult[3][3])
{
    BYTE  LOCAL_TYPE i,j;
    WORD  LOCAL_TYPE ucAddr;
    short LOCAL_TYPE sTmp;

    DB_Mode(eDB_NORMAL_MODE);

    if( bScalerWin == MAIN_WINDOW )
        ucAddr = SCE_04;
    else
        ucAddr = SCE_24;

    for(i=0; i!=3; i++)
    {
        for(j=0; j!=3; j++)
        {
        sTmp = sResult[i][j] ;
        #if DEBUG_ColorMaxrix_Value
        DSC_printData("sTmp===%x",sTmp);
        #endif

        #if ENABLE_SW_DOUBLE_BUFFER
        DB_W2B( ucAddr, sTmp );
        #else
        msWrite2Byte( ucAddr, sTmp );
        #endif
        ucAddr += 2;
        }
    }

#if ENABLE_SW_DOUBLE_BUFFER
    msSWDBWriteToRegister();
    msSWDBWaitForRdy();
#endif

}
#else
void msWritexvYCCColorMatrix(BYTE bScalerWin, short sResult[3][3])
{

     BYTE  LOCAL_TYPE i,j;
    WORD  LOCAL_TYPE ucAddr;
    short LOCAL_TYPE sTmp;

    if( bScalerWin == MAIN_WINDOW )
        ucAddr = SC8_82;
    else
        ucAddr = SC8_6E;

    for(i=0; i!=3; i++)
    {
        for(j=0; j!=3; j++)
        {
           // sTmp = psMatrix[i*3+j];
            sTmp = sResult[i][j] ;//&pstColor[i*3+j];

            if( sTmp >= 0 )
            {
                if( sTmp > 0xfff )
                {
                    sTmp = 0xfff;
                }

        //printData("\r\n 111 ucAddr==>%x", ucAddr);
        //printData("\r\n 111 sTmp==>%x", sTmp);
                msWrite2Byte( ucAddr, sTmp );
            }
            else
            {
                sTmp = sTmp * -1;
                if( sTmp > 0xfff )
                {
                    sTmp = 0xfff;
                }
                sTmp |= 0x1000;
                //printData("\r\n 222  ucAddr==>%x", ucAddr);
                //printData("\r\n 222  Data==>%x", sTmp);

                msWrite2Byte( ucAddr, sTmp );
            }
            ucAddr += 2;
        }
    }
}
#endif
//===================================================================================================================================================================:
//===================================================================================================================================================================:
#if DSC_UsexvYccColorMatrix_EN
void msAdjustPCXvyccContrast( BYTE bScalerWin, WORD uwContrast )
{
    BYTE LOCAL_TYPE i , j ;
    BYTE LOCAL_TYPE ucBank;
    XDATA short sResultTmp1[3][3];
    XDATA short sResultTmp2[3][3];
    XDATA short SatGainTmp1[3][3]={0};
    ucBank = msReadByte(SC0_00);

    SatGainTmp1[0][0]=(0x400<<1);
    SatGainTmp1[1][1]=(uwContrast<<1);
    SatGainTmp1[2][2]=(uwContrast<<1);
    ArrayMultiply( SatGainTmp1,t_SDTVRgb2yuv, sResultTmp1,11);
    for(i=0; i<3; i++)
        for(j=0; j<3; j++)
        sResultTmp2[i][j] = sResultTmp1[i][j];
    ArrayMultiply( t_SDTVYuv2rgb,sResultTmp2, sResultTmp1,12);

    //if (i==100)
    msWritexvYCCColorMatrix(bScalerWin,sResultTmp1);

    if( bScalerWin == MAIN_WINDOW )
        msWriteByteMask(SCE_02, BIT1,BIT1); // Enable color matrix&Change R/B range
    else
        msWriteByteMask(SCE_22, BIT1,BIT1); // Enable color matrix&Change R/B range
    msWriteByte(SC0_00, ucBank); // Restore bank
}
#else
void msAdjustPCXvyccContrast( BYTE bScalerWin, WORD uwContrast )
{
    XDATA short sResultTmp1[3][3];
    XDATA short sResultTmp2[3][3];
    XDATA short SatGainTmp1[3][3]={0};
    BYTE LOCAL_TYPE i , j ;
    BYTE LOCAL_TYPE ucBank;
    ucBank = msReadByte(SC0_00);
    SatGainTmp1[0][0]=0x400;
    SatGainTmp1[1][1]=uwContrast;
    SatGainTmp1[2][2]=uwContrast;
    ArrayMultiply( SatGainTmp1,t_SDTVRgb2yuv, sResultTmp1,10);
    for(i=0; i<3; i++)
        for(j=0; j<3; j++)
        sResultTmp2[i][j] = sResultTmp1[i][j];
    ArrayMultiply( t_SDTVYuv2rgb,sResultTmp2, sResultTmp1,10);
    //if (i==100)
    msWritexvYCCColorMatrix(bScalerWin,sResultTmp1);

    if( bScalerWin == MAIN_WINDOW )
        msWriteByte(SC8_81, 0x80); // Enable color matrix&Change R/B range
    else
        msWriteByte(SC8_80, 0x80); // Enable color matrix&Change R/B range

    msWriteByte(SC0_00, ucBank); // Restore bank
}
#endif

//===================================================================================================================================================================:
void msDSC_AdjustYgain ( WORD YgainValue ) // 2.10 format
{
#if DEBUG_SAT_HISTOGRAM
    BYTE xdata i;
    printMsg("\r\n ===== Sat Histogram Start ======");
    for ( i = 0; i < 32; i++ )
        DSC_printData("\r\n  %d", ISR_SatHistogram32H[i]);
    DSC_printMsg("\r\n ===== Sat Histogram End ======");
#endif
if (!REG_DSC_RUN_Flag /*|| REG_DPS_RUN_Flag*/)
    YgainValue = 0x400;
#if DEBUG_DSCSatgain
    if (CurxvYgain != YgainValue)
        DSC_printData("\r\n Sgain %x", YgainValue);
#endif

if (CurxvYgain != YgainValue)
    msAdjustPCXvyccContrast ( u8Win, ( YgainValue & 0x0FFF ) ); // ratio: 0~3.99x

    CurxvYgain = YgainValue;

#if DSC_EnablexvYCC_ForDebug//temp enable xvYCC
    msWriteByteMask ( REG_ADDR_xvYCC_MAIN_COLOR_MATRIX_ENABLE, BIT1, BIT1 ); // 3x3 disable
    msWriteByteMask ( REG_ADDR_xvYCC_SUB_COLOR_MATRIX_ENABLE , BIT1, BIT1 ); // 3x3 disable
#endif

}
//===================================================================================================================================================================:
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
///// Function Declaration //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void msDSC_Interrupt ( void )
{
    BYTE xdata ucTmp, ucBank;
    ucBank = scReadByte ( SC0_00 );
    scWriteByte ( REG_ADDR_INTENB, scReadByte ( REG_ADDR_INTENB ) & ( ~BIT3 ) );
    scWriteByte ( REG_ADDR_INTSTA, scReadByte ( REG_ADDR_INTSTA ) & ( ~BIT3 ) );

#if UseHistrogramToTest
    #if CHIP_ID == CHIP_TSUMY ||CHIP_ID == CHIP_TSUMB
        scWriteByte(SC0_00, 0x03);
    #elif CHIP_ID == CHIP_TSUMV ||CHIP_ID == CHIP_TSUMU||CHIP_ID == CHIP_TSUM2||CHIP_ID == CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID == CHIP_TSUMD||CHIP_ID == CHIP_TSUMJ||CHIP_ID == CHIP_TSUM9||CHIP_ID == CHIP_TSUMF
        scWriteByte(SC0_00, 0x07);
    #endif
    if ( ( scReadByte ( SC7_B8 ) &BIT3 ) && ( !Flag_DSC_Latch_Busy )) //if( (scRegs[0xB8]&BIT3) && (!Flag_DPS_Latch_Busy) )
    {
        //u8Win=SUB_WINDOW;
        scWriteByte(SC0_00, 0x08);
        for ( ucTmp = 0; ucTmp < 32; ucTmp++ )
        {
            ISR_SatHistogram32H[ucTmp] = scReadByte ( SC8_C0 + ( 2 * ucTmp ) + 1 );
            ISR_SatHistogram32H[ucTmp] <<= 8;
            ISR_SatHistogram32H[ucTmp] |= scReadByte ( SC8_C0 + ( 2 * ucTmp ) );
        }

    #if CHIP_ID == CHIP_TSUMY || CHIP_ID == CHIP_TSUMB
        scWriteByte(SC0_00, 0x03);
    #elif CHIP_ID == CHIP_TSUMV ||CHIP_ID == CHIP_TSUMU||CHIP_ID == CHIP_TSUM2||CHIP_ID == CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID == CHIP_TSUMD||CHIP_ID == CHIP_TSUMJ||CHIP_ID == CHIP_TSUM9||CHIP_ID == CHIP_TSUMF
        scWriteByte(SC0_00, 0x07);
    #endif
        //Request Hist
        scWriteByte ( SC7_B8, scReadByte ( SC7_B8 ) & 0xF0 ); //scRegs[0xB8] = scRegs[0xB8]&0xF0;
        if ( MAIN_WINDOW == u8Win )
        {
            scWriteByte ( SC7_B8, scReadByte ( SC7_B8 ) | BIT2 | BIT1 );    //scRegs[0xB8] = scRegs[0xB8]|BIT2|BIT1;
        }
        else
        {
            scWriteByte ( SC7_B8, scReadByte ( SC7_B8 ) | BIT2 | BIT0 );    //scRegs[0xB8] = scRegs[0xB8]|BIT2|BIT0;
        }
    }
    scWriteByte(SC0_00, 0x00);
    ISR_curDSCLoopCount++;
    scWriteByte ( REG_ADDR_INTENB, scReadByte ( REG_ADDR_INTENB ) | BIT3 ); //scRegs[0xCE] = scRegs[0xCE]|BIT3;
#else

    #if CHIP_ID == CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID == CHIP_TSUMD||CHIP_ID == CHIP_TSUMJ||CHIP_ID == CHIP_TSUM9||CHIP_ID == CHIP_TSUMF
        scWriteByte(SC0_00, 0x0F);
    #endif
    if ( ( scReadByte ( REG_ADDR_SATHIS_HANDSHAKE ) &BIT3 ) && ( !Flag_DSC_Latch_Busy )) //if( (scRegs[0xB8]&BIT3) && (!Flag_DPS_Latch_Busy) )
    {
        //u8Win=SUB_WINDOW;
        for ( ucTmp = 0; ucTmp < 32; ucTmp++ )
        {
            ISR_SatHistogram32H[ucTmp] = scReadByte ( REG_ADDR_SATHISTOGRAM_DATA_32 + ( 2 * ucTmp ) + 1 );
            ISR_SatHistogram32H[ucTmp] <<= 8;
            ISR_SatHistogram32H[ucTmp] |= scReadByte ( REG_ADDR_SATHISTOGRAM_DATA_32 + ( 2 * ucTmp ) );
        }
        //Request Hist
        scWriteByte ( REG_ADDR_SATHIS_HANDSHAKE, scReadByte ( REG_ADDR_SATHIS_HANDSHAKE ) & 0xF0 ); //scRegs[0xB8] = scRegs[0xB8]&0xF0;
        if ( MAIN_WINDOW == u8Win )
        {
            scWriteByte ( REG_ADDR_SATHIS_HANDSHAKE, scReadByte ( REG_ADDR_SATHIS_HANDSHAKE ) | BIT2 | BIT1 );    //scRegs[0xB8] = scRegs[0xB8]|BIT2|BIT1;
        }
        else
        {
            scWriteByte ( REG_ADDR_SATHIS_HANDSHAKE, scReadByte ( REG_ADDR_SATHIS_HANDSHAKE ) | BIT2 | BIT0 );    //scRegs[0xB8] = scRegs[0xB8]|BIT2|BIT0;
        }
    }
    ISR_curDSCLoopCount++;
#if CHIP_ID == CHIP_TSUMC||CHIP_ID==CHIP_TSUMK||CHIP_ID == CHIP_TSUMD||CHIP_ID == CHIP_TSUMJ||CHIP_ID == CHIP_TSUM9||CHIP_ID == CHIP_TSUMF
    scWriteByte(SC0_00, 0x00);
#endif

    scWriteByte ( REG_ADDR_INTENB, scReadByte ( REG_ADDR_INTENB ) | BIT3 ); //scRegs[0xCE] = scRegs[0xCE]|BIT3;
#endif
    scWriteByte ( SC0_00, ucBank );
}

//===================================================================================================================================================================:
static void SetSatHistogramRange ( WORD wVStart, WORD wVEnd, WORD wHStart, WORD wHEnd, BYTE Enable)
{
    BYTE xdata ucBank;
    ucBank = scReadByte ( SC0_00 );
    if ( REG_ADDR_SATHISTOGRAM_RANGE_M_HST !=0xFF)
    {
        // Set histogram range - H
        // Main Window
        msWriteByte ( REG_ADDR_SATHISTOGRAM_RANGE_M_HST, wHStart );
        msWriteByte ( REG_ADDR_SATHISTOGRAM_RANGE_M_HST + 1, ( msReadByte( REG_ADDR_SATHISTOGRAM_RANGE_M_HST + 1 ) & 0xF0 ) | ( wHStart >> 8 ) );
        msWriteByte ( REG_ADDR_SATHISTOGRAM_RANGE_M_HEN, wHEnd );
        msWriteByte ( REG_ADDR_SATHISTOGRAM_RANGE_M_HEN + 1, ( msReadByte( REG_ADDR_SATHISTOGRAM_RANGE_M_HEN + 1 ) & 0xF0 ) | ( wHEnd >> 8 ) );
    }
    // Set histogram range - V
    // Main Window
    msWriteByte ( REG_ADDR_SATHISTOGRAM_RANGE_M_VST, wVStart / 8 );
    msWriteByte ( REG_ADDR_SATHISTOGRAM_RANGE_M_VEN, wVEnd / 8 );

    // Enable range for Histogram
    if(Enable)
    {
        msWriteByteMask ( REG_ADDR_SATHISTOGRAM_RANGE_H_ENABLE, BIT6 | BIT7, BIT6 | BIT7 );
        msWriteByteMask ( REG_ADDR_SATHISTOGRAM_RANGE_V_ENABLE, BIT4, BIT4 );
    }
    else
    {
        msWriteByteMask ( REG_ADDR_SATHISTOGRAM_RANGE_H_ENABLE, 0, BIT6 | BIT7 );
        msWriteByteMask ( REG_ADDR_SATHISTOGRAM_RANGE_V_ENABLE, 0, BIT4 );
    }
    scWriteByte ( SC0_00, ucBank );
}
//===================================================================================================================================================================:
//////////////////////////////////////////////////////////////////////////////////
///// DSC initail ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void msDSC_Init( void )
{
    //Default brightness 100
    CurxvYgain = 0x400;
    REG_DSC_RUN_Flag = 1;
    msDSC_FadingArea_Setting (FadingArea_0, Fading_area0_Cr_coordination,Fading_area0_Cb_coordination,Fading_Range);
    msDSC_FadingArea_Setting (FadingArea_1, Fading_area1_Cr_coordination,Fading_area1_Cb_coordination,Fading_Range);
}
//===================================================================================================================================================================:
//////////////////////////////////////////////////////////////////////////////////
///// DPS On <--> Off Settings ///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void msDSC_On_Setting( void )
{
    BYTE xdata ucBank = msReadByte ( SC0_00 );

    msDSC_Init();
    SetSatHistogramRange ( 0, ( PANEL_HEIGHT - 1 ), 0, ( PANEL_WIDTH - 1 ), 0 );

#if UseHistrogramToTest
    msWriteByteMask ( SC7_B5, BIT1, BIT1 ); //  histogram accelerate mode on
    msWriteByteMask ( SC7_B8, 0, 0x0F );
    if ( MAIN_WINDOW == u8Win ) // Main window
        msWriteByteMask ( SC7_B8, BIT2 | BIT1, BIT2 | BIT1 | BIT0 );
    else // Sub window
        msWriteByteMask ( SC7_B8, BIT2 | BIT0, BIT2 | BIT1 | BIT0 );
    //printData("... SC7_B5[]==>%x ...",msReadByte ( SC7_B5 ));
    //printData("... SC7_B8[]==>%x ...",msReadByte ( SC7_B8 ));
#else
    msWriteByteMask ( REG_ADDR_SATHISTOGRAM_Acceler_ENABLE, BIT1, BIT1 ); //  histogram accelerate mode on
    msWriteByteMask ( REG_ADDR_SATHIS_HANDSHAKE, 0, 0x0F );
    if ( MAIN_WINDOW == u8Win ) // Main window
        msWriteByteMask ( REG_ADDR_SATHIS_HANDSHAKE, BIT2 | BIT1, BIT2 | BIT1 | BIT0 );
    else // Sub window
        msWriteByteMask ( REG_ADDR_SATHIS_HANDSHAKE, BIT2 | BIT0, BIT2 | BIT1 | BIT0 );
#endif
    msWriteByteMask ( REG_ADDR_INTENB, BIT3, BIT3 ); // Vsync int enable

#if DSC_UsexvYccColorMatrix_EN
    msAdjustPCXvyccContrast ( MAIN_WINDOW, CurxvYgain );
#else
    //UserPrefContrast=128; // as range is 0~255, if range is 0~100, the value is 50
    if(IsColorspaceRGB())
    {
            msWriteByteMask ( REG_ADDR_xvYCC_MAIN_COLOR_MATRIX_ENABLE, BIT1, BIT1 ); // 3x3 disable
            mStar_AdjustContrast(UserPrefContrast);
    }
    else
    {
            msWriteByteMask ( REG_ADDR_xvYCC_MAIN_COLOR_MATRIX_ENABLE, BIT7, BIT7 ); // 3x3 disable
            msAdjustVideoContrast(MAIN_WINDOW,UserPrefContrast);
    }
#endif

    msWriteByteMask ( REG_ADDR_XVYCC_CM_SAT_HIST, BIT0, BIT0 );
    msWriteByte ( SC0_00, ucBank );
}
//===================================================================================================================================================================:
void msDSC_Off_Setting ( void )
{
    BYTE xdata ucBank = msReadByte ( SC0_00 );
    REG_DSC_RUN_Flag = 0;
    msWriteByteMask ( REG_ADDR_INTENB, 0, BIT3 ); // Vsync int disable
    ISR_curDSCLoopCount = 0;
    ISR_preDSCLoopCount = 0;
#if 1
#if DSC_UsexvYccColorMatrix_EN
        CurxvYgain=0x400;
        msAdjustPCXvyccContrast ( MAIN_WINDOW, CurxvYgain );
#else
        //UserPrefContrast=128; // as range is 0~255, if range is 0~100, the value is 50
        if(IsColorspaceRGB())
        {
                msWriteByteMask ( REG_ADDR_xvYCC_MAIN_COLOR_MATRIX_ENABLE, BIT1, BIT1 ); // 3x3 disable
                mStar_AdjustContrast(UserPrefContrast);
        }
        else
        {
                msWriteByteMask ( REG_ADDR_xvYCC_MAIN_COLOR_MATRIX_ENABLE, BIT7, BIT7 ); // 3x3 disable
                msAdjustVideoContrast(MAIN_WINDOW,UserPrefContrast);
        }
#endif
#endif

    msWriteByte ( SC0_00, ucBank );
}

//===================================================================================================================================================================:
//===================================================================================================================================================================:
void msDSC_FadingArea_Setting (BYTE u8AresSelect, WORD u16Cr,WORD u16Cb, BYTE u8Range)
{
    if (u8AresSelect==FadingArea_0)  //enable Fading area
    {
        msWriteByteMask ( SCF_C0, BIT0, BIT0 );
        msWriteByteMask ( SCF_C2, Fading_slop<<4|Fading_alpha, 0xFF );
        //Set CR max/min  for Area 0
        msWrite2ByteMask ( SCF_C4, (u16Cr-u8Range), 0x3FF );
        msWrite2ByteMask ( SCF_C6, (u16Cr+u8Range), 0x3FF );
        //Set CB max/min
        msWrite2ByteMask ( SCF_CC, (u16Cb-u8Range), 0x3FF );
        msWrite2ByteMask ( SCF_CE, (u16Cb+u8Range), 0x3FF );
        //printData("111 (u168Cr+u8Range) ==%x",(u168Cr+u8Range));
        //printData("222 (u168Cr-u8Range) ==%x",(u168Cr-u8Range));
        //printData("333 (u16Cb+u8Range) ==%x",(u16Cb+u8Range));
        //printData("444 (u16Cb-u8Range) ==%x",(u16Cb-u8Range));
    }
    else if (u8AresSelect==FadingArea_1)
    {
        msWriteByteMask ( SCF_C0, BIT1, BIT1 );

        msWriteByteMask ( SCF_D2, Fading_slop<<4|Fading_alpha, 0xFF );
        //Set CR max/min for Area 1
        msWrite2ByteMask ( SCF_D4, (u16Cr-u8Range), 0x3FF );
        msWrite2ByteMask ( SCF_D6, (u16Cr+u8Range), 0x3FF );
        //Set CB max/min
        msWrite2ByteMask ( SCF_DC, (u16Cb-u8Range), 0x3FF );
        msWrite2ByteMask ( SCF_DE, (u16Cb+u8Range), 0x3FF );
        //printData("555 (u168Cr+u8Range) ==%x",(u168Cr+u8Range));
        //printData("666 (u168Cr-u8Range) ==%x",(u168Cr-u8Range));
        //printData("777 (u16Cb+u8Range) ==%x",(u16Cb+u8Range));
        //printData("888 (u16Cb-u8Range) ==%x",(u16Cb-u8Range));
    }
}

//===================================================================================================================================================================:
#else
BYTE code msDSCSettingNullData[] = {0};
void msDSCSettingDummy(void)
{
    BYTE xdata i = msDSCSettingNullData[0];
}
#endif // #if EnableDPS
//===================================================================================================================================================================:
