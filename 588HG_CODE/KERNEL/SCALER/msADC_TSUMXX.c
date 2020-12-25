#define _MSADC_TSUMXX_C_
#include <math.h>
#include "types.h"
#include "board.h"     
#include "ms_reg.h"
#include "Ms_rwreg.h"   
#include "Global.h" 
#include "misc.h"    
#include "AutoFunc.h"  
#include "MsADC.h"

#define OverFlow    		BIT7
#define NoneOverFlow    	BIT6
#define UnderFlow   		BIT7
#define NoneUnderFlow   	BIT6

//**************************************************************************
//  [Function Name]:
//                  msADC_AdjustHTotal()
//  [Description]
//                  Adjust PLL divider
//  [Arguments]:
//                  htotal: Htotal -3
//  [Return]:
//                  
//**************************************************************************
void msADC_AdjustHTotal( WORD htotal )
{
    msWriteByte( BK0_00, REGBANK1 );
    htotal -= 3;
#ifdef TSUMXXNX
    msWriteByte( BK1_02, ( htotal & 0xFF ) );
    msWriteByte( BK1_03, ( htotal >> 8 ) & 0x1F );
#else
    msWriteByte( BK1_02, htotal >> 4 );
    msWriteByte( BK1_03, ( htotal & 0xF ) << 4 );
#endif
    msWriteByte( BK0_00, REGBANK0 );
}
//**************************************************************************
//  [Function Name]:
//                  msADC_AdjustPhase()
//  [Description]
//                  Adjust ADC phase code
//  [Arguments]:
//                  code: code, 0~63
//  [Return]:
//
//**************************************************************************
void msADC_AdjustPhase( BYTE phase )
{
    msWriteByte( BK0_00, REGBANK1 );
    msWriteByte( BK1_10, phase + 8 );
    msWriteByte( BK1_11, phase );
    msWriteByte( BK0_00, REGBANK0 );
}
//**************************************************************************
//  [Function Name]:
//                  msADC_AdjustAdcGain()
//  [Description]
//                  Adjust ADC R/G/B gain code
//  [Arguments]:
//                  rcode/gcode/bcode: R/G/B gain value, 0~2047
//  [Return]:
//
//**************************************************************************
void msADC_AdjustAdcGain( BYTE rGain, BYTE gGain, BYTE bGain )
{
    msWriteByte( BK0_00, REGBANK1 );
    msWriteByte( BK1_04, 0xFF - rGain );
    msWriteByte( BK1_05, 0xFF - gGain );
    msWriteByte( BK1_06, 0xFF - bGain );
    msWriteByte( BK0_00, REGBANK0 );
}
//**************************************************************************
//  [Function Name]:
//                  msADC_AdjustAdcOffset()
//  [Description]
//                  Adjust ADC R/G/B offset code
//  [Arguments]:
//                  
//  [Return]:
//                  
//**************************************************************************
void msADC_AdjustAdcOffset( BYTE rOffset, BYTE gOffset, BYTE bOffset )
{
    msWriteByte( BK0_00, REGBANK1 );
    msWriteByte( BK1_07, 0xFF - rOffset );
    msWriteByte( BK1_08, 0xFF - gOffset );
    msWriteByte( BK1_09, 0xFF - bOffset );
    msWriteByte( BK0_00, REGBANK0 );
}
//**************************************************************************
//  [Function Name]:
//                  msADC_AdjustAdcRedGain()
//  [Description]
//                  Set ADC R/G/B offset code
//  [Arguments]:
//                  rcode
//  [Return]:
//                  
//**************************************************************************
void msADC_AdjustAdcRedGain( BYTE gain )
{
    msWriteByte( BK0_00, REGBANK1 );
    msWriteByte( BK1_04, 0xFF - gain );
    msWriteByte( BK0_00, REGBANK0 );
}
//**************************************************************************
//  [Function Name]:
//                  msADC_AdjustAdcGain()
//  [Description]
//                  Adjust ADC R/G/B gain code
//  [Arguments]:
//                  gcode
//  [Return]:
//
//**************************************************************************
void msADC_AdjustAdcGreenGain( BYTE gain )
{
    msWriteByte( BK0_00, REGBANK1 );
    msWriteByte( BK1_05, 0xFF - gain );
    msWriteByte( BK0_00, REGBANK0 );
}
//**************************************************************************
//  [Function Name]:
//                  msADC_AdjustAdcGain()
//  [Description]
//                  Adjust ADC R/G/B gain code
//  [Arguments]:
//                  bcode
//  [Return]:
//
//**************************************************************************
void msADC_AdjustAdcBlueGain( BYTE gain )
{
    msWriteByte( BK0_00, REGBANK1 );
    msWriteByte( BK1_06, 0xFF - gain );
    msWriteByte( BK0_00, REGBANK0 );
}
//**************************************************************************
//  [Function Name]:
//                  msADC_AdjustAdcOffset()
//  [Description]
//                  Adjust ADC R offset code
//  [Arguments]:
//                  rcode
//  [Return]:
//                  
//**************************************************************************
void msADC_AdjustAdcRedOffset( BYTE offset )
{
    msWriteByte( BK0_00, REGBANK1 );
    msWriteByte( BK1_07, 0xFF - offset );
    msWriteByte( BK0_00, REGBANK0 );
}
//**************************************************************************
//  [Function Name]:
//                  msADC_AdjustAdcOffset()
//  [Description]
//                  Adjust ADC G offset code
//  [Arguments]:
//                  gcode
//  [Return]:
//                  
//**************************************************************************
void msADC_AdjustAdcGreenOffset( BYTE offset )
{
    msWriteByte( BK0_00, REGBANK1 );
    msWriteByte( BK1_08, 0xFF - offset );
    msWriteByte( BK0_00, REGBANK0 );
}
//**************************************************************************
//  [Function Name]:
//                  msADC_AdjustAdcOffset()
//  [Description]
//                  Adjust ADC B offset code
//  [Arguments]:
//                  bcode
//  [Return]:
//                  
//**************************************************************************
void msADC_AdjustAdcBlueOffset( BYTE offset )
{
    msWriteByte( BK0_00, REGBANK1 );
    msWriteByte( BK1_09, 0xFF - offset );
    msWriteByte( BK0_00, REGBANK0 );
}

// Auto Color Setting
Bool msADC_AutoAdjustAdcGain( BYTE vsyncTime )
{
    BYTE rGain, gGain, bGain;
    BYTE maxRGain, maxGGain, maxBGain;
    BYTE minRGain, minGGain, minBGain;
    BYTE retry = 8;
    BYTE adcStatus;
    BYTE atgFlags = 0;
    maxRGain = maxGGain = maxBGain = 0xFF;
    minRGain = minGGain = minBGain = 0;
    while( retry-- )
    {
        rGain = (( WORD )maxRGain + minRGain ) / 2;
        gGain = (( WORD )maxGGain + minGGain ) / 2;
        bGain = (( WORD )maxBGain + minBGain ) / 2;
        msADC_AdjustAdcGain( rGain, gGain, bGain );
        Delay1ms( vsyncTime );
        WaitAutoStatusReady( BK0_78, BIT1 );
        adcStatus = msReadByte( BK0_78 );
        if( adcStatus & MAXR_B )
        {
            rGain--;
            atgFlags |= OverFlow;
        }
        else
        {
            rGain++;
            atgFlags |= NoneOverFlow;
        }
        if( adcStatus & MAXG_B )
        {
            gGain--;
            atgFlags |= OverFlow;
        }
        else
        {
            gGain++;
            atgFlags |= NoneOverFlow;
        }
        if( adcStatus & MAXB_B )
        {
            bGain--;
            atgFlags |= OverFlow;
        }
        else
        {
            bGain++;
            atgFlags |= NoneOverFlow;
        }
        msADC_AdjustAdcGain( rGain, gGain, bGain );
        Delay1ms( vsyncTime );
        WaitAutoStatusReady( BK0_78, BIT1 );
        adcStatus = msReadByte( BK0_78 );
        if( adcStatus & MAXR_B )
        {
            maxRGain = rGain + 1;
            atgFlags |= OverFlow;
        }
        else
        {
            minRGain = rGain - 1;
            atgFlags |= NoneOverFlow;
        }
        if( adcStatus & MAXG_B )
        {
            maxGGain = gGain + 1;
            atgFlags |= OverFlow;
        }
        else
        {
            minGGain = gGain - 1;
            atgFlags |= NoneOverFlow;
        }
        if( adcStatus & MAXB_B )
        {
            maxBGain = bGain + 1;
            atgFlags |= OverFlow;
        }
        else
        {
            minBGain = bGain - 1;
            atgFlags |= NoneOverFlow;
        }
        if( CheckSyncLoss() )
        {
            return FALSE;
        }
    }
    if(( atgFlags & 0xC0 ) != 0xC0 )
    {
        return FALSE;
    }
    UserPrefAdcRedGain = rGain - 1;
    UserPrefAdcGreenGain = gGain - 1;
    UserPrefAdcBlueGain = bGain - 1;
    msADC_AdjustAdcGain( UserPrefAdcRedGain, UserPrefAdcGreenGain, UserPrefAdcBlueGain );
    return TRUE;
}

#define OffsetValue     0x10

#if ENABLE_HWAUTO_ADCOFFSET

#if defined (TSUMXXNX) || defined (TSUMXXQX) ||defined(TSUMXXKX)
#define HWREPORT_TIME           	80
#if defined (TSUMXXQX)
#define REGBANK_ADC_DTOP    	0x09
#elif defined (TSUMXXNX)
#define REGBANK_ADC_DTOP    	0x07
#elif defined (TSUMXXKX)
#define REGBANK_ADC_DTOP    	0x0E
#endif

Bool msADC_ADCOffset_AutoCalibraton( void )
{
    BYTE BKE_0C_Value, BKE_0B_Value;
    BYTE R_OFFSET, G_OFFSET, B_OFFSET;
    BYTE u8ReTry = 0;

    if( SrcInputType != Input_Analog1 && SrcInputType != Input_Analog2)
        return FALSE;

    while( u8ReTry++ < 100 )
    {
        msADC_AdjustAdcOffset( 0x7D, 0x7D, 0x7D );
        msWriteByte( BK0_00, REGBANK1 );
        msWriteByte( BK1_3F, 0x18 );
        mStar_WriteByteMask( BK1_40, (BIT5), (BIT5) );
        msWriteByte( BK1_3F, 0x1C );

        msWriteByte( BK0_00, REGBANK_ADC_DTOP );
        BKE_0C_Value = msReadByte( 0x0C );
        BKE_0B_Value = msReadByte( 0x0B );

        msWriteByte( 0x0C, BKE_0C_Value | BIT3 );
        msWriteByte( 0x0B, BKE_0B_Value | 0x01 );
        Delay1ms( HWREPORT_TIME );
        msWriteByte( BK0_00, REGBANK1 );
        R_OFFSET = msReadByte( BK1_3C );
        
        msWriteByte( BK0_00, REGBANK_ADC_DTOP );
        msWriteByte( 0x0B, BKE_0B_Value | 0x02 );
        msWriteByte( BK0_00, REGBANK1 );
        G_OFFSET = msReadByte( BK1_3C );

        msWriteByte( BK0_00, REGBANK_ADC_DTOP );
        msWriteByte( 0x0B, BKE_0B_Value | 0x03 );
        msWriteByte( BK0_00, REGBANK1 );
        B_OFFSET = msReadByte( BK1_3C );

        msWriteByte( BK0_00, REGBANK_ADC_DTOP );
        msWriteByte( 0x0C, BKE_0C_Value );
        msWriteByte( 0x0B, BKE_0B_Value );

        msWriteByte( BK0_00, REGBANK0 );

        if( !( R_OFFSET == 0x7F && G_OFFSET == 0x7F && B_OFFSET == 0x7F ) )
            break;
    }
    msWriteByte( BK0_00, REGBANK1 );
    mStar_WriteByteMask( BK1_40, 0x00, (BIT5) );
    msWriteByte( BK1_3F, 0x00 );
    msWriteByte( BK0_00, REGBANK0 );

    if( u8ReTry < 100 )
    {
        UserPrefAdcRedOffset  = R_OFFSET - OffsetValue;
        UserPrefAdcGreenOffset = G_OFFSET - OffsetValue;
        UserPrefAdcBlueOffset = B_OFFSET - OffsetValue;

        msADC_AdjustAdcOffset( UserPrefAdcRedOffset , UserPrefAdcGreenOffset, UserPrefAdcBlueOffset );
        return TRUE;
    }
    return FALSE;
}
#else
Bool msADC_ADCOffset_AutoCalibraton( void )
{
    msWriteByte( BK0_00, REGBANK1 );
    msWriteByte( BK1_3F, 0x1C );
    Delay1ms( 50 );
    mStar_WriteByteMask( BK1_40, (BIT5), (BIT6 | BIT5) );
    Delay1ms( 50 );
    UserPrefAdcRedOffset = msReadByte( BK1_3C );
    
    mStar_WriteByteMask( BK1_40, (BIT6), (BIT6 | BIT5) );
    Delay1ms( 50 );
    UserPrefAdcGreenOffset = msReadByte( 0x3C );
    
    Delay1ms( 50 );
    mStar_WriteByteMask( BK1_40, (BIT6 | BIT5), (BIT6 | BIT5) );
    UserPrefAdcBlueOffset = msReadByte( BK1_3C );

    msWriteByte( BK1_3F, 0x00 );

    msWriteByte( BK0_00, REGBANK0 );
    UserPrefAdcRedOffset = UserPrefAdcRedOffset - OffsetValue;
    UserPrefAdcGreenOffset = UserPrefAdcGreenOffset - OffsetValue;
    UserPrefAdcBlueOffset = UserPrefAdcBlueOffset - OffsetValue;
    msADC_AdjustAdcOffset( UserPrefAdcRedOffset, UserPrefAdcGreenOffset, UserPrefAdcBlueOffset );

    return TRUE;
}
#endif

#else

Bool msADC_AutoAdjustAdcOffset( BYTE vsyncTime )
{
    BYTE rOffset, gOffset, bOffset;
    BYTE maxROffset, maxGOffset, maxBOffset;
    BYTE minROffset, minGOffset, minBOffset;
    BYTE retry = 8;
    BYTE adcStatus;
    BYTE atgFlags = 0;
    maxROffset = maxGOffset = maxBOffset = 0xFF;
    minROffset = minGOffset = minBOffset = 0;
    while( retry-- )
    {
        rOffset = (( WORD )maxROffset + minROffset ) / 2;
        gOffset = (( WORD )maxGOffset + minGOffset ) / 2;
        bOffset = (( WORD )maxBOffset + minBOffset ) / 2;
        msADC_AdjustAdcOffset( rOffset, gOffset, bOffset );
        Delay1ms( vsyncTime );
        WaitAutoStatusReady( BK0_78, BIT1 );
        adcStatus = msReadByte( BK0_79 );
        if( adcStatus & BIT2 )
        {
            rOffset++;
            atgFlags |= UnderFlow;
        }
        else
        {
            rOffset--;
            atgFlags |= NoneUnderFlow;
        }
        if( adcStatus & BIT1 )
        {
            gOffset++;
            atgFlags |= UnderFlow;
        }
        else
        {
            gOffset--;
            atgFlags |= NoneUnderFlow;
        }
        if( adcStatus & BIT0 )
        {
            bOffset++;
            atgFlags |= UnderFlow;
        }
        else
        {
            bOffset--;
            atgFlags |= NoneUnderFlow;
        }
        msADC_AdjustAdcOffset( rOffset, gOffset, bOffset );
        Delay1ms( vsyncTime );
        WaitAutoStatusReady( BK0_78, BIT1 );
        adcStatus = msReadByte( BK0_79 );
        if( adcStatus & BIT2 )
        {
            minROffset = rOffset - 1;
            atgFlags |= UnderFlow;
        }
        else
        {
            maxROffset = rOffset + 1;
            atgFlags |= NoneUnderFlow;
        }
        if( adcStatus & BIT1 )
        {
            minGOffset = gOffset - 1;
            atgFlags |= UnderFlow;
        }
        else
        {
            maxGOffset = gOffset + 1;
            atgFlags |= NoneUnderFlow;
        }
        if( adcStatus & BIT0 )
        {
            minBOffset = bOffset - 1;
            atgFlags |= UnderFlow;
        }
        else
        {
            maxBOffset = bOffset + 1;
            atgFlags |= NoneUnderFlow;
        }
        if( CheckSyncLoss() )
        {
            return FALSE;
        }
    }
    if(( atgFlags & 0xC0 ) != 0xC0 )
    {
        return FALSE;
    }

    UserPrefAdcRedOffset = rOffset - OffsetValue;
    UserPrefAdcGreenOffset = gOffset - OffsetValue;
    UserPrefAdcBlueOffset = bOffset - OffsetValue;
    msADC_AdjustAdcOffset( UserPrefAdcRedOffset, UserPrefAdcGreenOffset, UserPrefAdcBlueOffset );
    return TRUE;
}
#endif
//==================================================================
Bool msADC_AutoAdcColor( BYTE vsyncTime )
{
    BYTE result;
    BYTE u8AdcPgaGain;
    
    if( SrcInputType == Input_Digital || SrcInputType == Input_Digital2 || SrcInputType == Input_Digital3 )
        return FALSE;

    drvADC_SetPGAGainCode(0);
    //if(USER_PREF_INPUT_COLOR_FORMAT==INPUT_COLOR_YPBPR) //YPbPr or YUV_H+V
    //    drvADC_SetRGBOffsetCode(0x200, 0x40, 0x200);
    else
        drvADC_SetRGBOffsetCode(0, 0, 0);
    drvADC_SetRGBGainCode(0x400, 0x400, 0x400);

    msWriteByte(SC0_78, 0x01); // enable auto Gain
    u8AdcPgaGain=drvADC_CheckPGAGain(vsyncTime);

    result = drvADC_AutoAdjustAdcGain(vsyncTime);
    if (!result)
    {
        //ADC_DPUTSTR( "Auto color failed !!");
        drvADC_SetPGAGainCode(UserPrefAdcPgaGain);
        drvADC_SetRGBGainCode( UserPrefAdcRedGain, UserPrefAdcGreenGain, UserPrefAdcBlueGain );
    }
    else
        UserPrefAdcPgaGain=u8AdcPgaGain;
    msWriteByte(SC0_78, 0); // disable auto Gain




/*    
    msWriteByte( BK0_78, 0x11 ); // enable auto Gain
    msWriteByte( BK0_00, REGBANK1 );
    msWriteByte( BK1_01, 0x00 );
    msWriteByte( BK1_41, 0x10 ); // Rex 100709 for HW auto offset
    msWriteByte( BK1_42, 0x10 );
    msWriteByte( BK1_43, 0x00 );
    msWriteByte( BK0_00, REGBANK0 );

    #if ENABLE_HWAUTO_ADCOFFSET
    result = msADC_ADCOffset_AutoCalibraton();
    #else
    result = msADC_AutoAdjustAdcOffset( vsyncTime );
    #endif

    if( !result )
    {
        msADC_AdjustAdcOffset( UserPrefAdcRedOffset, UserPrefAdcGreenOffset, UserPrefAdcBlueOffset );
    }
    result = msADC_AutoAdjustAdcGain( vsyncTime );
    if( !result )
    {
        msADC_AdjustAdcGain( UserPrefAdcRedGain, UserPrefAdcGreenGain, UserPrefAdcBlueGain );
    }
    
    msWriteByte( BK0_00, REGBANK1 );
    msWriteByte( BK1_01, 0x01 );
    msWriteByte( BK0_00, REGBANK0 );
    msWriteByte( BK0_78, 0x00 ); // enable auto Gain
*/    
    return result;
}
