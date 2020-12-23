#include <math.h>
#include "types.h"
#include "msDPS_Handler.h"
#include "msDPS_Setting.h"
#include "Debug.h"
//#include "Debugmsg.h"
//#include "debug.h"
#include "Common.h"

//===================================================================================================================================================================:
#define  DPS_DEBUG  0
#if (DPS_DEBUG&&ENABLE_DEBUG)
#define DPS_printData(str, value)   printData(str, value)
#define DPS_printMsg(str)               printMsg(str)
#else
#define DPS_printData(str, value)
#define DPS_printMsg(str)
#endif

//===================================================================================================================================================================:
#if  ENABLE_DPS
#if 0
#define msDpsVer        0x0005
#endif
//////msDPS_Handler/////////////////////////////////////////////////////////////
#if DPS_YminForWindowSave_EN
xdata BYTE  Flag_CurWindowSavePat = 0;
#endif
xdata WORD  curLumaHist[32];
xdata BYTE  HistDataMaxArray[5] = 0;
xdata BYTE  HistDataAvgArray[5] = 0;
xdata BYTE  HistImgYMax_tar = 0;
xdata BYTE  StepIncCount = 0;
xdata BYTE  StepDecCount = 0;
xdata BYTE  YgainCycLoopCount = 0;
xdata BYTE  PdutyCycLoopCount = 0;
xdata WORD  Ygain_diff = 0;
xdata BYTE  Pduty_diff = 0;
xdata WORD  Ygain_nxt = 0;
xdata BYTE  Pduty_nxt = 0;
xdata WORD  Ygain_ISR = 1024;
xdata BYTE  Pduty_ISR = 255;
xdata WORD  Ygain_tar_pre = 0;
xdata BYTE  Pduty_tar_pre = 0;
#if DPS_HistPseuMax_MOD1_EN 
xdata BYTE  HistPseuMax;
#endif
//===================================================================================================================================================================:
//////////////////////////////////////////////////////////////////////////////////
///// Main Code ///////////////////////////////////////////////////////////////////
///// Input Interface ///////////////////////////////////////////////////////////////
///// 1. BYTE   xdata ISR_Count;
///// 2. WORD xdata ISR_LumaHistogram32H[32]; // >>6
///// 3. BYTE   xdata ISR_HistogramMax;              // 0~255
///// 4. BYTE   xdata ISR_HistogramMin;               // 0~255
///// 5. WORD xdata ISR_HistogramTotalPix;        // >>6
///// 6. WORD xdata ISR_HistogramTotalLuma;    // >>14
///// 7. WORD xdata CurYgain;                           // 2.10 format
///// 8. BYTE   xdata CurBacklight;                      // 0~255 <--> 0%~100%
//////////////////////////////////////////////////////////////////////////////////
///// Output Interface///////////////////////////////////////////////////////////////
///// 1. Backlight ==> static void msDPS_AdjustBacklight(BYTE BLValue)  // 0~255 <--> 0%~100%
///// 2. Y-gain    ==>  static void msDPS_AdjustYgain(WORD YgainValue) // 2.10 format
//////////////////////////////////////////////////////////////////////////////////
void msDPS_ClearBuffer ( void )
{
    StepIncCount = 0;
    StepDecCount = 0;
    YgainCycLoopCount = 0;
    PdutyCycLoopCount = 0;
    Ygain_diff = 0;
    Pduty_diff = 0;
}
void msDPS_Handler ( void )
{
#if DPS_YminForWindowSave_EN
    xdata BYTE  Flag_PreWindowSavePat = 0;
    xdata BYTE  Flag_OutWindowSavePat = 0;
    xdata WORD  HistogramWeight_L0= 0;
    xdata WORD  HistogramWeight_L31= 0;
#endif
    xdata BYTE  Flag_PureColorPattern = 0;
    xdata BYTE  Flag_SceneChangeValid = 0;
    xdata BYTE  Flag_GradientPattern = 0;
    xdata BYTE  Flag_BlendSchemeInPWM = 0;
    xdata BYTE  Flag_StepIncValid = 0;
    xdata BYTE  Flag_StepDecValid = 0;
    xdata BYTE  curPixelMax = 0;
    xdata BYTE  curPixelMin = 0;
    xdata WORD  curPixelCount = 0;
    xdata WORD  curPixelWeight = 0;
    xdata WORD  preLumaHist[32] = 0;
    xdata BYTE  HistDiffWeight = 0;
    xdata BYTE  HistDiffCount = 0;
    xdata DWORD TempVariable1 = 0;
    xdata DWORD TempVariable2 = 0;
    xdata DWORD TempVariable3 = 0;
    xdata BYTE  HistDensity = 0;
    xdata BYTE  HistDataMax = 0;
    xdata BYTE  HistDataMin = 0;
    xdata BYTE  HistDataAvg = 0;
    xdata BYTE  HistDataMaxDiff = 0;
    xdata BYTE  HistDataAvgDiff = 0;
    xdata BYTE  RefSection = 0;
    xdata BYTE  RefDistance = 0;
    xdata BYTE  RowStart = 0;
    xdata BYTE  ColStart = 0;
    xdata BYTE  PowerLimit = 0;
    xdata BYTE  PdutyLimit = 0;
    xdata BYTE  LumiMap = 0;
    xdata BYTE  YMaxIndex = 0;
    xdata BYTE  LevelIndex = 0;
    xdata WORD  Ygain_tar = 0;
    xdata BYTE  Pduty_tar = 0;
    xdata BYTE  Ygain_sign = 0;
    xdata BYTE  Pduty_sign = 0;
    xdata BYTE  i, j, Flag_ES60WhitePattern;
    xdata BYTE  REG_DPS_ColStart;
    //----------------------------------------------------------------------
    //--check valid DPS loop
    //----------------------------------------------------------------------
    if ( ISR_curLoopCount == ISR_preLoopCount )
    {
        return;
    }
    ISR_preLoopCount = ISR_curLoopCount;
    //----------------------------------------------------------------------
    //--latch current-frame Histogram
    //----------------------------------------------------------------------
    Flag_DPS_Latch_Busy = 1;
    curPixelCount = ISR_HistogramTotalPix;
    curPixelWeight = ISR_HistogramTotalLuma; 
    curPixelMax = ISR_HistogramMax; 
    curPixelMin = ISR_HistogramMin; 
    for ( i = 0; i < 32; i++ )
    {
        preLumaHist[i] = curLumaHist[i];
        curLumaHist[i] = ISR_LumaHistogram32H[i]; 
        TempVariable1 = ABS2 ( preLumaHist[i], curLumaHist[i] );
        TempVariable2 += TempVariable1;
        TempVariable3 += (DWORD)curLumaHist[i]*(DWORD)(i*8+7);
        if ( TempVariable1 >= REG_DPS_HistDiffCountThd )
        {
            HistDiffCount++;
        }
        DPS_printData ( "i =%d", i*8+7 );
        DPS_printData ( "curLumaHist[i] =%d", curLumaHist[i] );
#if DPS_HistPseuMax_MOD1_EN 
        if (curLumaHist[i]>REG_MouseCount)
            HistPseuMax = i*8+4;
#endif
    }
    curPixelWeight = TempVariable3>>8; // Return for pure color with mouse which will effect "pure colro detection" for Nanao 
    DPS_printData ( "curPixelWeight=%x", curPixelWeight );
    TempVariable2 = TempVariable2 >> 7;
    TempVariable2 = MIN2 ( TempVariable2, 255 );
    HistDiffWeight = TempVariable2;
    Flag_DPS_Latch_Busy = 0;
#if DPS_HistPseuMax_MOD1_EN 
    curPixelMax = HistPseuMax;
#endif
#if DPS_HistPseuMax_MOD2_EN 
    curPixelMax = ( ( curPixelMax >> 3 ) << 3 ) + 7;
#endif
    //----------------------------------------------------------------------
    //--Histogram parser
    //----------------------------------------------------------------------
    TempVariable1 = ( 256UL * curPixelWeight ) / curPixelCount;
    TempVariable1 = ( ( TempVariable1 >> 3 ) << 3 ) + 7;
    TempVariable1 = MIN2 ( TempVariable1, 255 );
    HistDataAvg = TempVariable1;
    RefSection = HistDataAvg >> 3;
    TempVariable2 = 0;
    for ( i = 32; i > 0; i-- )
    {
        j = i - 1;
        if ( curLumaHist[j] != 0 )
        {
            if ( HistDataMax == 0 )
            {
                HistDataMax = j * 8 + 7;
            }
            HistDataMin = j * 8 + 7;
        }
        if ( j >= RefSection )
        {
            RefDistance = j - RefSection + 1;
        }
        else
        {
            RefDistance = RefSection - j + 1;
        }
        TempVariable2 += curLumaHist[j] / RefDistance;
    }
    TempVariable2 = ( 255UL * TempVariable2 ) / curPixelCount;
    TempVariable2 = MIN2 ( TempVariable2, 255 );
    HistDensity = TempVariable2;
    //----------------------------------------------------------------------
    //--check flags
    //----------------------------------------------------------------------

    //ToDo ES6.0
    if(REG_DPS_BW_MODE_EN)
    {
    if(REG_DPS_YOUT_ENABLE)
        TempVariable1 = curLumaHist[29]+curLumaHist[2];
    else    
        TempVariable1 = curLumaHist[31]+curLumaHist[0];
    TempVariable1 = TempVariable1*256/curPixelCount;
    Flag_ES60WhitePattern = TempVariable1>REG_DPS_BW_MODE_Gain;
    }
    
    TempVariable1 = ABS2 ( curPixelMax, HistDataAvg );
    Flag_PureColorPattern = ( HistDensity >= REG_DPS_PureColorDstyThd ) &&
                            ( TempVariable1 <= REG_DPS_PureColorDiffThd );
    DPS_printData ( "HistDensity=%d", HistDensity );
    DPS_printData ( "TempVariable1=%d", TempVariable1 );
#if DPS_YminForWindowSave_EN

    HistogramWeight_L0 = (DWORD)(curLumaHist[0]*32768) / curPixelCount; 
    HistogramWeight_L31 =  (DWORD)(curLumaHist[31]*32768) / curPixelCount; 
    TempVariable1 = 255; // max
    TempVariable2 = 0; //min
    
    Flag_PreWindowSavePat = Flag_CurWindowSavePat;
    Flag_CurWindowSavePat = ( !Flag_PureColorPattern ) &&
                            ( HistDensity >= REG_DPS_WindowSaveDensityTH ) &&
                            ( HistDataAvg <= REG_DPS_WindowSaveYavgLimit )/* &&
                            (curPixelMax == TempVariable1 )&& (curPixelMin == TempVariable2) &&
                            (HistogramWeight_L0 >= REG_DPS_WindowSaveHisWeight_0_Up) && 
                            (HistogramWeight_L0 <= REG_DPS_WindowSaveHisWeight_0_Down)&&
                            (HistogramWeight_L31 >= REG_DPS_WindowSaveHisWeight_31_Up)&&
                            (HistogramWeight_L31 <= REG_DPS_WindowSaveHisWeight_31_Down))||
                            (Flag_PureColorPattern &&(curPixelMax == TempVariable2 )&& (curPixelMin == TempVariable2) )*/;

    Flag_OutWindowSavePat = ( Flag_PreWindowSavePat == 1 && Flag_CurWindowSavePat == 0 ) ||
                            ( Flag_PreWindowSavePat == 0 && Flag_CurWindowSavePat == 1 );
#if 0    
    //DPS_printData ( "Flag_CurWindowSavePat=%d", Flag_CurWindowSavePat );
    //DPS_printData ( "HistDensity=%d", HistDensity );
    //DPS_printData ( "HistDataAvg=%d", HistDataAvg );
    //DPS_printData ( "HistogramWeight_L0=%d", HistogramWeight_L0 );
    //DPS_printData ( "HistogramWeight_L31=%d", HistogramWeight_L31 );
#endif   
#endif
    for ( i = 4; i > 0; i-- )
    {
        HistDataMaxArray[i] = HistDataMaxArray[i-1];
        HistDataAvgArray[i] = HistDataAvgArray[i-1];
    }
    HistDataMaxArray[0] = curPixelMax;
    HistDataAvgArray[0] = HistDataAvg;
    TempVariable1 = ( HistDataMaxArray[0] > HistDataMaxArray[1] ) &&
                    ( HistDataMaxArray[1] > HistDataMaxArray[2] ) &&
                    ( HistDataMaxArray[2] > HistDataMaxArray[3] ) &&
                    ( HistDataMaxArray[3] > HistDataMaxArray[4] );
    TempVariable2 = ( HistDataMaxArray[0] < HistDataMaxArray[1] ) &&
                    ( HistDataMaxArray[1] < HistDataMaxArray[2] ) &&
                    ( HistDataMaxArray[2] < HistDataMaxArray[3] ) &&
                    ( HistDataMaxArray[3] < HistDataMaxArray[4] );
    Flag_GradientPattern = ( TempVariable1 != 0 ) || ( TempVariable2 != 0 );
    TempVariable1 = 0;
    TempVariable2 = 255;
    for ( i = 0; i < 5; i++ )
    {
        if ( HistDataMaxArray[i] >= TempVariable1 )
        {
            TempVariable1 = HistDataMaxArray[i];
        }
        if ( HistDataMaxArray[i] <= TempVariable2 )
        {
            TempVariable2 = HistDataMaxArray[i];
        }
    }
    HistDataMaxDiff = ABS2 ( TempVariable1, TempVariable2 );
    TempVariable1 = 0;
    TempVariable2 = 255;
    for ( i = 0; i < 5; i++ )
    {
        if ( HistDataAvgArray[i] >= TempVariable1 )
        {
            TempVariable1 = HistDataAvgArray[i];
        }
        if ( HistDataAvgArray[i] <= TempVariable2 )
        {
            TempVariable2 = HistDataAvgArray[i];
        }
    }
    HistDataAvgDiff = ABS2 ( TempVariable1, TempVariable2 );
    Flag_SceneChangeValid = ( Flag_GradientPattern == 0 ) &&
                            ( HistDataMaxDiff >= REG_DPS_SceChgMaxDiffThd ) &&
                            ( HistDataAvgDiff >= REG_DPS_SceChgAvgDiffThd ) &&
                            ( HistDiffWeight >= REG_DPS_SceChgHistWgtThd ) &&
                            ( HistDiffCount >= REG_DPS_SceChgHistCntThd );
#if DPS_YminForWindowSave_EN
    Flag_SceneChangeValid = Flag_SceneChangeValid || Flag_OutWindowSavePat;
#endif
    DPS_printData ( "M = %d", HistDataMaxDiff );
    DPS_printData ( "A = %d", HistDataAvgDiff );
    DPS_printData ( "W = %d", HistDiffWeight );
    DPS_printData ( "C = %d", HistDiffCount );
    if ( Flag_GradientPattern  ) DPS_printData ( "======G", Flag_GradientPattern );
    if ( Flag_PureColorPattern ) DPS_printData ( "============P", Flag_PureColorPattern );
    if ( Flag_SceneChangeValid ) DPS_printData ( "==================S", Flag_SceneChangeValid );
    DPS_printData ( "curPixelMax=%d", curPixelMax );
    //----------------------------------------------------------------------
    //--get HistImgYMax_tar
    //----------------------------------------------------------------------
    TempVariable3 = curPixelMax;
    if(REG_DPS_YOUT_ENABLE)
    {
        TempVariable1 = (WORD)(((WORD)HistImgYMax_tar) * ((WORD)219)/((WORD)255))+16;
        TempVariable1 = MIN2 ( TempVariable1, 235 );
        Flag_StepIncValid = ( TempVariable3 > TempVariable1 ) ? 1 : 0;
        Flag_StepDecValid = ( TempVariable3 < TempVariable1 ) ? 1 : 0;
    }
    else
    {
        Flag_StepIncValid = ( TempVariable3 > HistImgYMax_tar ) ? 1 : 0;
        Flag_StepDecValid = ( TempVariable3 < HistImgYMax_tar ) ? 1 : 0;
    }
    
    if ( Flag_StepIncValid )
    {
        StepIncCount++;
    }
    else if ( Flag_StepDecValid && StepIncCount != 0 )
    {
        StepIncCount--;
    }
    if ( Flag_StepDecValid )
    {
        StepDecCount++;
    }
    else if ( Flag_StepIncValid && StepDecCount != 0 )
    {
        StepDecCount--;
    }
    if ((Flag_SceneChangeValid)||(StepIncCount >= REG_DPS_StepIncCountThd)||(StepDecCount >= REG_DPS_StepDecCountThd))
    {
        StepIncCount = 0;
        StepDecCount = 0;
        HistImgYMax_tar = curPixelMax;
        if(REG_DPS_YOUT_ENABLE)
        {
            TempVariable1 = (WORD)((WORD)HistImgYMax_tar - 16) * (255)/(219);
            TempVariable1 = MIN2 ( TempVariable1, 255 );
            HistImgYMax_tar = TempVariable1;
        }
    }
    else 
    {
        HistImgYMax_tar = HistImgYMax_tar;
    }

#if 0
    DPS_printData ( "HistImgYMax_tar=%d", HistImgYMax_tar );
#endif

    if (Flag_PureColorPattern)
    {
        HistImgYMax_tar = MIN2 ( HistImgYMax_tar, REG_DPS_ImgYmaxForPureColor );
        HistImgYMax_tar = MAX2 ( HistImgYMax_tar, REG_DPS_ImgYminForPureColor );
    }
    else
    {
        HistImgYMax_tar = MIN2 ( HistImgYMax_tar, REG_DPS_ImgYmaxForNatureImg );
        HistImgYMax_tar = MAX2 ( HistImgYMax_tar, REG_DPS_ImgYminForNatureImg );
    }
    if ( Flag_GradientPattern  ) DPS_printData ( "====G", Flag_GradientPattern );
    if ( Flag_PureColorPattern ) DPS_printData ( "=====P", Flag_PureColorPattern );
    if ( Flag_SceneChangeValid ) DPS_printData ( "=======S", Flag_SceneChangeValid );
    DPS_printData ( "curPixelMax =%d", curPixelMax );
    DPS_printData ( "HistImgYMax_cur =%d", TempVariable3   );
    DPS_printData ( "HistImgYMax_tar =%d", HistImgYMax_tar );
    ///----------------------------------------------------------------------
    //--get Ygain_tar & Pduty_tar
    //----------------------------------------------------------------------
    if((REG_DPS_UserBrightContlGain%10)> 0)
        REG_DPS_ColStart = (REG_DPS_UserBrightContlGain / 10)+1;
    else
        REG_DPS_ColStart = REG_DPS_UserBrightContlGain / 10;

    REG_DPS_ColStart = MIN2 ( REG_DPS_ColStart,10 );
    
    RowStart = REG_DPS_RowStart; //--REG setting (no used now)
    ColStart = 10 - REG_DPS_ColStart; //--PWM setting (0 for 100%; 1 for 90%; ...)
    PowerLimit = MAX2(REG_DPS_PowerLimitHW, REG_DPS_PowerLimitFW);
    PdutyLimit = ( 255UL * PowerLimit * 10 ) / 100;
    TempVariable1 = ( HistImgYMax_tar == 255 ) ? 16 : ( HistImgYMax_tar >> 4 );
    YMaxIndex = 16 - TempVariable1;
    if ( YMaxIndex == 0 )
    {
        LumiMap = DPS_Lumi_Table[0][ColStart];
    }
    else
    {
        TempVariable1 = ( ( WORD ) ( DPS_Lumi_Table[YMaxIndex - 1][ColStart] - DPS_Lumi_Table[YMaxIndex][ColStart] ) * ( HistImgYMax_tar - ( ( 16UL - YMaxIndex ) << 4 ) ) ) >> 4;
        TempVariable1 += DPS_Lumi_Table[YMaxIndex][ColStart];
        LumiMap = TempVariable1;
    }
    Flag_BlendSchemeInPWM = ( DPS_Lumi_Table[RowStart][10 - PowerLimit] < LumiMap ) ? 1 : 0;
    if ( LumiMap >= REG_DPS_LevelLimit )
    {
        Ygain_tar = 1024;
        Pduty_tar = 255;
    }
    else
    {
        if ( Flag_BlendSchemeInPWM )
        {
            if ( RowStart == 0 )
            {
                TempVariable1 = ( 1024UL * 255 ) / HistImgYMax_tar;
            }
            else
            {
                TempVariable1 = 1024UL * 16 * ( 16UL - RowStart ) / HistImgYMax_tar;
            }
            for ( i = 0; i < ( 10 - PowerLimit ); i++ )
            {
                if ( DPS_Lumi_Table[RowStart][i] >= LumiMap )
                {
                    LevelIndex = i;
                }
            }
            TempVariable2 = 10UL * ( LumiMap - DPS_Lumi_Table[RowStart][LevelIndex + 1] ) / ( DPS_Lumi_Table[RowStart][LevelIndex] - DPS_Lumi_Table[RowStart][LevelIndex + 1] );
            TempVariable2 += 100 - ( LevelIndex + 1 ) * 10;
            TempVariable2 = ( 255UL * TempVariable2 ) / 100;
        }
        else
        {
            for ( i = RowStart; i < 16; i++ )
            {
                if ( DPS_Lumi_Table[i][10 - PowerLimit] >= LumiMap )
                {
                    LevelIndex = i;
                }
            }
            TempVariable1 = 16UL * ( LumiMap - DPS_Lumi_Table[LevelIndex + 1][10 - PowerLimit] ) / ( DPS_Lumi_Table[LevelIndex][10 - PowerLimit] - DPS_Lumi_Table[LevelIndex + 1][10 - PowerLimit] );
            TempVariable1 += ( 16 - ( LevelIndex + 1 ) ) << 4;
            TempVariable1 = 1024UL * TempVariable1 / HistImgYMax_tar;
            TempVariable2 = PdutyLimit;
        }
        Ygain_tar = TempVariable1;
        Pduty_tar = TempVariable2;
    }
    /*
    TempVariable3 = ((DWORD)Pduty_tar * REG_DPS_UserBrightContlGain) >> 8;
    Pduty_tar = TempVariable3;
    */  
    TempVariable3 = (DWORD)Pduty_tar * REG_DPS_UserBrightContlGain / (REG_DPS_ColStart*10);
    Pduty_tar = TempVariable3;
    
    //#if DPS_AggrPowerSaveMode_EN
    if ((!REG_DPS_BW_MODE_EN)||(REG_DPS_BW_MODE_EN && !Flag_ES60WhitePattern))
    {    
        if (HistImgYMax_tar <= REG_DPS_AggressiveGainRange)
    {
    TempVariable3 = MIN2( REG_DPS_UserBrightLimitGain, REG_DPS_AggressivePdutyGain);
    TempVariable1 = ((DWORD)Ygain_tar * REG_DPS_AggressiveYgainGain) >> 8;
    TempVariable2 = ((DWORD)Pduty_tar * TempVariable3) >> 8;
    Ygain_tar = Flag_PureColorPattern ? Ygain_tar : TempVariable1;
    Pduty_tar = Flag_PureColorPattern ? Pduty_tar : TempVariable2;
    /*
    TempVariable1 = ((DWORD)Ygain_tar * REG_DPS_AggressiveYgainGain) >> 8;
    TempVariable2 = ((DWORD)Pduty_tar * REG_DPS_AggressivePdutyGain) >> 8;
    Ygain_tar = Flag_PureColorPattern ? Ygain_tar : TempVariable1;
    Pduty_tar = Flag_PureColorPattern ? Pduty_tar : TempVariable2;
    */
    }
    }
    #if 0
    DPS_printData ( "ES6.0_Flag  %d", Flag_ES60WhitePattern );
    #endif
    //#endif
    Ygain_tar = MIN2 ( Ygain_tar,       2047 ); //--Max. Ygain = 1.99x
    Ygain_tar = MAX2 ( Ygain_tar,       1024 ); //--min. Ygain = 1.00x
    Pduty_tar = MIN2 ( Pduty_tar,        255 ); //--HW Limitation
    Pduty_tar = MAX2 ( Pduty_tar, PdutyLimit ); //--HW Limitation
#if 0
    DPS_printData ( "REG_DPS_UserBrightContlGain =%d", REG_DPS_UserBrightContlGain );
    DPS_printData ( "Flag_BlendSchemeInPWM =%d", Flag_BlendSchemeInPWM );
    DPS_printData ( "LumiMap =%d", LumiMap );
    DPS_printData ( "YMaxIndex =%d", YMaxIndex );
    DPS_printData ( "LevelIndex =%d", LevelIndex );
    DPS_printData ( "Ygain_tar =%d", Ygain_tar );
    DPS_printData ( "Pduty_tar =%d", Pduty_tar );
#endif
#if 0
    DPS_printData ( "======", HistImgYMax_tar );
    DPS_printData ( "Y=%d", Ygain_tar );
    DPS_printData ( "P=%d", Pduty_tar );
#endif

    //----------------------------------------------------------------------
    //--get Ygain_nxt & Pduty_nxt
    //----------------------------------------------------------------------
    if ((Ygain_tar_pre != Ygain_tar) || (Pduty_tar_pre != Pduty_tar))
    {
        if (REG_DPS_UpdateFrameCyc == 0)
        {
                Ygain_diff = REG_DPS_YgainDiffMinTH;
                Pduty_diff = REG_DPS_PdutyDiffMinTH;
        }
        else
        {
                TempVariable3 = ABS2 ( Pduty_ISR, Pduty_tar );
                TempVariable1 = TempVariable3/REG_DPS_UpdateFrameCyc;
                Pduty_diff = MAX2 ( TempVariable1, REG_DPS_PdutyDiffMinTH );
                TempVariable3 = TempVariable3/Pduty_diff;
                TempVariable2 = ABS2 ( Ygain_ISR, Ygain_tar );
                TempVariable2 = (TempVariable3 !=0) ? TempVariable2/TempVariable3 : 0;
                Ygain_diff = MAX2 ( TempVariable2, REG_DPS_YgainDiffMinTH );
        }
    }
    if ( YgainCycLoopCount == REG_DPS_YgainUpdateCyc )
    {
        if (Ygain_ISR != Ygain_tar)
        {
                Ygain_sign = ( Ygain_tar > Ygain_ISR ) ? 1 : 0;
                if ( Ygain_sign )
            {
                    TempVariable1 = Ygain_ISR + Ygain_diff;
                    Ygain_nxt = MIN2(TempVariable1, Ygain_tar);
            }
            else
            {
                    TempVariable1 = Ygain_ISR >= Ygain_diff ? Ygain_ISR - Ygain_diff : 0;
                    Ygain_nxt = MAX2(TempVariable1, Ygain_tar);
            }
        }
    }
    if ( PdutyCycLoopCount == REG_DPS_PdutyUpdateCyc )
    {
        if (Pduty_ISR != Pduty_tar)
        {
            Pduty_sign = ( Pduty_tar > Pduty_ISR ) ? 1 : 0;
            if ( Pduty_sign )
    {
                TempVariable1 = Pduty_ISR + Pduty_diff;
                Pduty_nxt = MIN2(TempVariable1, Pduty_tar);
    }
            else
    {
                TempVariable1 = Pduty_ISR >= Pduty_diff ? Pduty_ISR - Pduty_diff : 0;
                Pduty_nxt = MAX2(TempVariable1, Pduty_tar);
    }
        }
    }
    if ( YgainCycLoopCount == REG_DPS_YgainUpdateCyc )
    {
        YgainCycLoopCount = 0;
    }
    else
    {
        YgainCycLoopCount++;
    }
    if ( PdutyCycLoopCount == REG_DPS_PdutyUpdateCyc )
    {
        PdutyCycLoopCount = 0;
    }
    else
    {
        PdutyCycLoopCount++;
    }
    Ygain_tar_pre = Ygain_tar;
    Pduty_tar_pre = Pduty_tar;
#if 0
    DPS_printData ( "M=%d", HistImgYMax_tar );
    DPS_printData ( "Y=%d", Ygain_tar );
    DPS_printData ( "P=%d", Pduty_tar );
    DPS_printData ( "       %d", Ygain_nxt );
    DPS_printData ( "            %d", Pduty_nxt );
#endif
    //----------------------------------------------------------------------
    //--special patch
    //----------------------------------------------------------------------
#if DPS_YminForWindowSave_EN
    if ( Flag_CurWindowSavePat )
    {
        Ygain_nxt = REG_DPS_WindowSaveYgain;
        Pduty_nxt = REG_DPS_PowerLevel; // REG_DPS_PowerLevel; //PdutyLimit
        msDPS_ClearBuffer();
    }
#endif
    if ( Flag_SceneChangeValid )
    {
        Ygain_nxt = Ygain_tar;
        Pduty_nxt = Pduty_tar;
        msDPS_ClearBuffer();
    }
    //----------------------------------------------------------------------
    //--update Ygain & Pduty
    //----------------------------------------------------------------------

    Ygain_nxt = MIN2 ( Ygain_nxt,       2047 ); //--Max. Ygain = 1.99x
    Ygain_nxt = MAX2 ( Ygain_nxt,       1024 ); //--min. Ygain = 1.00x
    Pduty_nxt = MIN2 ( Pduty_nxt,        255 ); //--HW Limitation
    Pduty_nxt = MAX2 ( Pduty_nxt, PdutyLimit ); //--HW Limitation

    Ygain_ISR = Ygain_nxt;
    Pduty_ISR = Pduty_nxt;
    
    msDPS_AdjustYgain ( Ygain_ISR );
    msDPS_AdjustBacklight ( Pduty_ISR );
}
#if 0
WORD msDPS_Ver(void)
{
    return msDpsVer;
}
#endif
//===================================================================================================================================================================:
#else
BYTE code msDPSHandlerNullData[] = {0};
void msDPSHandlerDummy(void)
{
    BYTE xdata i = msDPSHandlerNullData[0];
}
#endif // #if EnableDPS
//===================================================================================================================================================================:
