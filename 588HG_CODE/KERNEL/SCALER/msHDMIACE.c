/******************************************************************************
 Copyright (c) 2007 MStar Semiconductor, Inc.
 All rights reserved.

 [Module Name]: HDMIACE.c
 [Date]:
 [Comment]:
   Color Adjust subroutines.
 [Reversion History]:
*******************************************************************************/
#define _HDMIACE_C_
#include <math.h>
// System

#include "board.h"
#include "types.h"
#include "ms_reg.h"
#include "Ms_rwreg.h"

#if HDMICOLORTEST
extern void ArrayMultiply( void );
extern void msWriteColorMatrix( short* ppsMatrix );

extern LOCAL_TYPE short( * sFirst )[3];
extern LOCAL_TYPE short( * sSecond )[3];
extern LOCAL_TYPE short( * sResult )[3];
extern code short tSDTVYuv2rgb[3][3];
extern code short tNormalColorCorrectionMatrix[3][3];

XDATA short g_sPCContrastMatrix[3][3];

code short tNormalSatHueMatrix[3][3] =
{
    { 1024,      0,         0},
    {      0,   1024,       0},
    {      0,      0,  1024}
};

#define msDoubleBufferStart()
#define msDoubleBufferEnd()

void InitHDMIACEVar( void )
{
    g_sPCContrastMatrix[0][0] = g_sPCContrastMatrix[1][1] = g_sPCContrastMatrix[2][2] = 1024;
    g_sPCContrastMatrix[0][1] = g_sPCContrastMatrix[1][0] = g_sPCContrastMatrix[2][0] =
                                    g_sPCContrastMatrix[0][2] = g_sPCContrastMatrix[1][2] = g_sPCContrastMatrix[2][1] = 0;
}


void msSetPCColorMatrix()
{
    XDATA short sResultTmp1[3][3];
    XDATA short sResultTmp2[3][3];
    typedef XDATA short TypArr[3][3];
    LOCAL_TYPE BYTE ucBank;
    LOCAL_TYPE BYTE ucColorBank;
    LOCAL_TYPE short* psOutTab;
    //XDATA BYTE ucTmpBri;

    // Adjust hue&saturation, and then YUV to RGB
    sFirst = tSDTVYuv2rgb;
    sSecond = tNormalSatHueMatrix;
    sResult = sResultTmp1;
    ArrayMultiply();
    //ArrayMultiply( tSDTVYuv2rgb, g_sVideoSatHueMatrix, sResultTmp1 );

    // Adjust contrast-RGB
    sFirst = g_sPCContrastMatrix;
    sSecond = sResultTmp1;
    sResult = sResultTmp2;
    ArrayMultiply();
    //ArrayMultiply( g_sVideoContrastMatrix, sResultTmp1, sResultTmp2 );

    psOutTab = ( short* )sResultTmp2;

    // Do color correction
    //ArrayMultiply( tVedioColorCorrectionMatrix, (TypArr)psOutTab, sResultTmp1 );

    sFirst = tNormalColorCorrectionMatrix;  //sFirst=(TypArr)tPCColorCorrectionMatrix;
    sSecond = ( TypArr )psOutTab;
    sResult = sResultTmp1;
    ArrayMultiply();
    //ArrayMultiply( (TypArr)tColorCorrectionMatrix, (TypArr)psOutTab, sResultTmp1 );
    psOutTab = ( short* )sResultTmp1;

    // g_ucBriteComp = sResultTmp1[1][1] >> 6;

    ucBank = msReadByte( BK0_00 ); // Backup bank

    //if( bScalerWin == MAIN_WINDOW )
    //{
    //    ucColorBank = REGBANK0; // Use main window bank
    //    //ucTmpBri = g_ucMainDigiBrightness - g_ucBriteComp;
    //}
    //else // Sub window
    {
#if 0//ChipID==Chip9259
        ucColorBank = REGBANKSUBWIN;
#else
        ucColorBank = REGBANK3;
#endif
        //ucTmpBri = g_ucSubDigiBrightness - g_ucBriteComp;
    }

    msDoubleBufferStart();

    msWriteByte( BK0_00, ucColorBank );
    ucColorBank = msReadByte( BK3_71 );
    msWriteByte( BK3_71, ucColorBank | BIT2 );
    msWriteColorMatrix( psOutTab );
    //msWriteByte( BK0_70, 0x15 ); // Enable color matrix&Change R/B range
    msWriteByte( BK3_71, ucColorBank );

    //msWriteByte( BK0_72, ucTmpBri );
    //msWriteByte( BK0_73, ucTmpBri );
    //msWriteByte( BK0_74, ucTmpBri );

    msDoubleBufferEnd();

    msWriteByte( BK0_00, ucBank ); // Restore bank
}

void msAdjustMainWinRedColor( BYTE ucColor )
{
    g_sPCContrastMatrix[0][0] = ( WORD )ucColor * 8;
    msSetPCColorMatrix();
}
void msAdjustMainWinGreenColor( BYTE ucColor )
{
    g_sPCContrastMatrix[1][1] = ( WORD )ucColor * 8;
    msSetPCColorMatrix();
}
void msAdjustMainWinBlueColor( BYTE ucColor )
{
    g_sPCContrastMatrix[2][2] = ( WORD )ucColor * 8;
    msSetPCColorMatrix();
}
void msAdjustMainWinContrast( BYTE ucRColor, BYTE ucGColor, BYTE ucBColor )
{
    g_sPCContrastMatrix[0][0] = ( WORD )ucRColor * 8;
    g_sPCContrastMatrix[1][1] = ( WORD )ucGColor * 8;
    g_sPCContrastMatrix[2][2] = ( WORD )ucBColor * 8;
    msSetPCColorMatrix();
}
#endif

