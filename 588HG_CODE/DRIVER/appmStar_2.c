
#include "global.h"

WORD mSTar_GetInputHTotal(void)
{
    WORD u16HTotal;
    msWriteByteMask(SC0_ED,BIT6,BIT6);
    u16HTotal=(msRead2Byte(SC0_E4)&0x1FFF)+1; //get input HTotal
    msWriteByteMask(SC0_ED,0,BIT6);
    return u16HTotal;
}

Bool appmStar_SetPanelTiming( void )
{
    DWORD factor;
    WORD dclk;
    WORD sclk;
    WORD height;
    WORD dstHTotal;
    WORD hPeriod;
    DWORD xdata oVtotal; //Estimated output vtotal in frame lock mode.

    // for getting more accurate output htotal //0812 for AC speed switch
    if( SrcHPeriod > 511 )        // prevent counter overflow when input frequency is very low
    {
        hPeriod = SrcHPeriod * 16;
    }
    else
    {
        #if UseINT && Enable_Expansion
        mStar_EnableModeChangeINT(FALSE);
        #endif
        sclk = GetVSyncTime() * 8; ///3
        SC0_HPEROID_DETECT_MODE(TRUE);//msWriteByte(SC0_E5, BIT7); // enable 16 line line to calculate hsync period.
        Delay1ms( sclk );
        hPeriod = SC0_READ_HPEROID();//msRead2Byte(SC0_E4)&MASK_13BIT;
        SC0_HPEROID_DETECT_MODE(FALSE);//msWriteByte(SC0_E5, 0);
        Delay1ms( sclk );
        if( abs( hPeriod - SrcHPeriod * 16 ) > 80 )            // prevent from getting the wrong hperiod to generate unknown panel clock
        {
            hPeriod = SrcHPeriod * 16;
        }
        #if UseINT && Enable_Expansion
        mStar_EnableModeChangeINT(TRUE);//restore
        #endif
    }

    // calculate input timing dot clock for comparing with output dot clock
    if(CURRENT_INPUT_IS_VGA())
        sclk = (( DWORD )UserPrefHTotal * MST_HPeriod_UINT ) / (( DWORD )hPeriod * 1000 );
    else
    {
        dclk = mSTar_GetInputHTotal();
        sclk = (( DWORD )dclk * MST_HPeriod_UINT ) / (( DWORD )hPeriod * 1000 );
    }

        #if 0//DEBUG_PRINT_ENABLE
        printData( "  sclk=%d", sclk );
        #endif

    height = SC0_READ_IMAGE_HEIGHT();//msRead2Byte(SC0_09)&SC_MASK_V;
    oVtotal = (DWORD)SrcVTotal*PanelHeight/height;

    if( CURRENT_SOURCE_IS_INTERLACE_MODE() )
    {
        height /= 2;
    }

    dstHTotal = PanelMinHTotal + 10;

    //***************************************************************
    if( sclk )
    {
        // in expansion mode, chip will assume output clock is faster than input clock,
        // if input clock is faster than output clock, must make output clock faster than input clock
        factor = (( float )dstHTotal * ( PanelHeight - 1 ) * MST_HPeriod_UINT ) / (( DWORD )hPeriod * ( height - 1 ) ); // output dclk
        dclk = ( factor + 500 ) / 1000;
        #if DEBUG_PRINT_ENABLE
        printData( "  dclk=%d", dclk );
        printData( "  PanelMaxDCLK=%d", PanelMaxDCLK );
        printData( "  oVotal=%d", oVtotal );
        printData( "  PanelMaxVTotal=%d", PanelMaxVTotal );
        #endif

        if(((dclk > PanelMaxDCLK) || (oVtotal>PanelMaxVTotal)) && (!g_SetupPathInfo.bFBMode))// && !OverScanFlag)
        {
            WORD wHeight;
            BYTE xdata VBlanking = (SC_LOCK_LINE+3);

            if(CURRENT_SOURCE_IS_INTERLACE_MODE())
            {
                height *= 2;
                VBlanking *= 2;
            }

            if(SrcVTotal-height > VBlanking)
            {
                DecVScaleValue = SrcVTotal-height-VBlanking;
                if(DecVScaleValue%2)
                    DecVScaleValue +=1;
                //modified some timing display unsymmetrically
                DecVScaleValue = ((VSTART_OFFSET>=SC0_READ_AUTO_START_V())?(SC0_READ_AUTO_START_V()*2):(DecVScaleValue));
            }
            else
            {
                SrcFlags|=bUnsupportMode;
                return FALSE;
            }

            wHeight = height+DecVScaleValue;
            oVtotal = (DWORD)SrcVTotal*PanelHeight/wHeight;

            if(CURRENT_SOURCE_IS_INTERLACE_MODE())
                wHeight /= 2;
            factor=((float)dstHTotal*(PanelHeight-1)*MST_HPeriod_UINT)/((DWORD)hPeriod*(wHeight-1)); // output dclk
            dclk=(factor+500)/1000;


            if((dclk > PanelMaxDCLK) || (oVtotal>PanelMaxVTotal))
            {
                SrcFlags|=bUnsupportMode;
                return FALSE;
            }

            mStar_SetCaptureWindow();
         	mStar_SetupPath();
            //mStar_SetScalingFactor();
            height += DecVScaleValue;
            if(CURRENT_SOURCE_IS_INTERLACE_MODE())
                height /= 2;

        }
        else
        {
            DecVScaleValue = 0;
        }

    }

#if 1
        SC0_SAMPLE_CLOCK_INVERT(sclk);
#else
    if (sclk > 165)
        msWriteByteMask(SC0_F3, BIT0, BIT0);
    else
        msWriteByteMask(SC0_F3, 0, BIT0);
#endif

    //SC0_SET_OUTPUT_VTOTAL(0xFFF);//set in the following msSetFrameSyncMode //msWrite2ByteMask(SC0_1E, 0xFFF, SC_MASK_V); // output vtotal //Jison 110527 lost to add.

    factor = (float)XTAL_CLOCK_KHZ * 16UL / hPeriod * dstHTotal * (PANEL_HEIGHT-1) / (height-1); // output dot clock in KHz

    //mStar_SetDClkPLL(factor);

    //mStar_ResetDClkPLL(); // reset output PLL to prevent PLL works abnormally

//****************Setup line buffer clock*************************************
  #if 0//DEBUG_EN
    PRINT_DATA(" current idclk=%d",(((DWORD)USER_PREF_H_TOTAL*MST_HPeriod_UINT)/((DWORD)u16HPeriod*1000)));
  #endif

    if (SyncLossState() || UnsupportedModeFlag)
    {
        mStar_SetDClkPLL((DWORD)PanelHTotal*PanelVTotal*GetVfreq()/1000);
        msSetFrameSyncMode(FLM_FB_FPLL_NOLOCK);
    }
    else // valid timing
    {
        if(g_SetupPathInfo.bFBMode)
        {
#ifdef FPGA
#if 1
            mStar_SetDClkPLL((DWORD)PanelHTotal*PanelVTotal*GetVfreq()/1000);
            msSetFrameSyncMode(FLM_FB_FPLL);
#else
            mStar_SetDClkPLL(u32Factor);
            msSetFrameSyncMode(FLM_FBL);
#endif
#else
            mStar_SetDClkPLL((DWORD)PanelHTotal*PanelVTotal*GetVfreq()/1000);
            msSetFrameSyncMode(FLM_FB_FPLL);
#endif
        }
        else
        {
            mStar_SetDClkPLL(factor);
            msSetFrameSyncMode(FLM_FBL);
        }
    }

    mStar_SetTimingGen(PANEL_WIDTH, PANEL_HEIGHT, FALSE);
    mStar_CheckFastClock(hPeriod, hPeriod);

    mStar_SetPanelSSC( 0, 0 );
    Delay1ms(2);
    SC0_OUTPUT_LOCK_MODE(TRUE);//msWriteByteMask(SC0_02, 0 , NIS_B);// enable lock input mode
    Delay1ms(100);
#if ENABLE_FACTORY_SSCADJ // 2011-09-16 19:46 CC
    mStar_SetPanelSSC(FactorySetting.SSCModulation, FactorySetting.SSCPercentage);
#else
    mStar_SetPanelSSC(PANEL_SSC_MODULATION_DEF, PANEL_SSC_PERCENTAGE_DEF);
#endif

    if(g_SetupPathInfo.bFBMode)
        WaitOutputFpllStable();

    return TRUE;
}

