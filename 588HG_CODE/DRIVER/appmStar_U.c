
#include "global.h"

Bool appmStar_SetPanelTiming(void)
{
    DWORD u32Sclk;
    WORD u16Height;
    WORD u16dstHTotal;
    WORD u16hPeriod;
    DWORD u32ODCLK_KHZ;
    WORD u16HDE_OP21;
    WORD u16VDE_OP21;
#if DECREASE_V_SCALING
    DWORD factor;
#endif
    DWORD xdata oVtotal; //Estimated output vtotal in frame lock mode.

    // for getting more accurate output htotal
    if( SrcHPeriod > 511 ) // avoid overflow
        u16hPeriod = SrcHPeriod * 16;
    else
    {
        #if UseINT && Enable_Expansion//Jison110331 for wideMode switch set SC0_E5[7] will cause mode change
        mStar_EnableModeChangeINT(FALSE);
        #endif
        u32Sclk = GetVSyncTime() * 8;
        SC0_HPEROID_DETECT_MODE(TRUE);//msWriteByte( SC0_E5, BIT7 ); // enable 16 line line to calculate hsync period.
        Delay1ms( u32Sclk );
        u16hPeriod = SC0_READ_HPEROID();//msRead2Byte( SC0_E4 ) & 0x1FFF;
        if( abs( u16hPeriod / 16 - SrcHPeriod ) > 5 )
            u16hPeriod = SrcHPeriod * 16;
        SC0_HPEROID_DETECT_MODE(FALSE);//msWriteByte( SC0_E5, 0 );
        Delay1ms( u32Sclk );
        #if UseINT && Enable_Expansion
        mStar_EnableModeChangeINT(TRUE);//restore
        #endif
    }

    u16Height = g_SetupPathInfo.wImgSizeInV; // case 1,2 only
    oVtotal = (DWORD)SrcVTotal*PanelHeight/u16Height;
    if(CURRENT_SOURCE_IS_INTERLACE_MODE())
    {
        oVtotal /= 2;
    }

    u16dstHTotal = PanelMinHTotal+10;
#if Enable_Expansion //JisonWideMode
    if( !EXPANSION_MODE_IS_FULL() && (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL) )//( UserprefExpansionMode!=Expansion_Full && (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL) )
    {
        u16HDE_OP21 = PANEL_WIDTH;
        u16VDE_OP21 = PANEL_HEIGHT;
    }
    else
#endif
    if( IS_FMT_IN_SBS() )
    {
        u16HDE_OP21 = g_SetupPathInfo.wMRWSizeInH*2;
        u16VDE_OP21 = g_SetupPathInfo.wMRWSizeInV/2;
    }
    else
    {
        u16HDE_OP21 = g_SetupPathInfo.wMRWSizeInH;
        u16VDE_OP21 = g_SetupPathInfo.wMRWSizeInV;
    }

    //input dot clock in KHz
#if DVI_PORT||HDMI_PORT//ENABLE_DIGITAL_INPUT
    if (!(CURRENT_INPUT_IS_VGA()))//(SrcInputType!=Input_Analog1)
    {
        u32Sclk=mSTar_GetInputHTotal(); //get input HTotal
        //PRINT_DATA("SrcHTotal=%d",u32Sclk);
        //use input htotal
        u32Sclk=((DWORD)u32Sclk*MST_HPeriod_UINT)/((DWORD)u16hPeriod);
    }
    else
#endif

    if(CURRENT_INPUT_IS_VGA())
        u32Sclk = (( DWORD ) UserPrefHTotal * MST_HPeriod_UINT ) / (( DWORD )u16hPeriod );
    else
        u32Sclk = (( DWORD ) StandardModeHTotal * MST_HPeriod_UINT ) / (( DWORD )u16hPeriod );

#if DECREASE_V_SCALING          //111013 Rick add for display unsupport mode - A045
#if FRAME_BFF_SEL == FRAME_BFFLESS
	#if ENABLE_FREESYNC
	if(IS_DP_FREESYNC())
	{
		factor = (float)msAPI_combo_IPGetPixelClk() *10 * u16dstHTotal / msAPI_combo_IPGetGetHTotal() * (PANEL_HEIGHT-1)  / ((u16Height-1)); // output dot clock in KHz
	}
	else
	#endif
    factor = ((float)XTAL_CLOCK_KHZ * 16UL / u16hPeriod * u16dstHTotal * (PANEL_HEIGHT-1) / (u16Height-1)); // output dot clock in KHz
#else
    factor = (( float )GetVfreq() * GetVtotal() * u16dstHTotal + 500)/1000; // output dot clock in KHz
#endif
    u32ODCLK_KHZ = (factor+500) / 1000;
//printData("OOR oclk(%d)>PANEL_MAX_DCLK",u32ODCLK_KHZ/1000);
#if FRAME_BFF_SEL==FRAME_BFFLESS  // Need check .....648x480@72_185
    //if (u32ODCLK_KHZ>PanelMaxDCLK) // if ouput clock is out of panel spec, then enable unsupported flag
#if ENABLE_FACTORY_SSCADJ // 2011-09-16 19:46 CC

        // 120321 coding, modified for ssc percentage, original 0~3 means 0~3%, modified 0~30 means 0~30%
        //if ((u32ODCLK_KHZ + (u32ODCLK_KHZ * FactorySetting.SSCPercentage) / 100 + 1)>PanelMaxDCLK-2)
        if (((u32ODCLK_KHZ + (u32ODCLK_KHZ * FactorySetting.SSCPercentage/10) / 100 + 1)>PanelMaxDCLK-2) || (oVtotal>PanelMaxVTotal))
#else
        // 120321 coding, modified for ssc percentage, original 0~3 means 0~3%, modified 0~30 means 0~30%
        //if ((u32ODCLK_KHZ + (u32ODCLK_KHZ * PANEL_SSC_PERCENTAGE_DEF) / 100 + 1)>PanelMaxDCLK-2)
        if ( ((u32ODCLK_KHZ + (u32ODCLK_KHZ * PANEL_SSC_PERCENTAGE_DEF/10) / 100 + 1)>PanelMaxDCLK-2) || (oVtotal>PanelMaxVTotal))
#endif
        {
            WORD wHeight;
            BYTE xdata VBlanking = (SC_LOCK_LINE+3);

            if(CURRENT_SOURCE_IS_INTERLACE_MODE())
            {
                u16Height *= 2;
                VBlanking *= 2;
            }

            if(SrcVTotal-u16Height > VBlanking)        //111019 Rick modified  - A066
            {
                DecVScaleValue = SrcVTotal-u16Height-VBlanking;
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

            wHeight = u16Height+DecVScaleValue;
            oVtotal = (DWORD)SrcVTotal*PanelHeight/wHeight;

            if(CURRENT_SOURCE_IS_INTERLACE_MODE())
                wHeight /= 2;
		#if ENABLE_FREESYNC
			if(IS_DP_FREESYNC())
			{
			factor = (float)msAPI_combo_IPGetPixelClk() *10 * u16dstHTotal / msAPI_combo_IPGetGetHTotal() * (PANEL_HEIGHT-1)  / ((u16Height-1)); // output dot clock in KHz
			}
			else
		#endif
            factor=((float)u16dstHTotal*(PanelHeight-1)*MST_HPeriod_UINT)/((DWORD)u16hPeriod*(wHeight-1)); // output dclk
            u32ODCLK_KHZ=(factor+500)/1000;
#if ENABLE_FREESYNC
			if(!IS_HDMI_FREESYNC() &&	!IS_DP_FREESYNC())
#endif
			{

	            if((u32ODCLK_KHZ > PanelMaxDCLK) || (oVtotal>PanelMaxVTotal))
	            {
	                SrcFlags|=bUnsupportMode;
	                return FALSE;
	            }
			
            mStar_SetCaptureWindow();
            mStar_SetupPath();
            //mStar_SetScalingFactor();
            u16Height += DecVScaleValue;
            if(CURRENT_SOURCE_IS_INTERLACE_MODE())
                u16Height /= 2;
			}
        }
        else
        {
            DecVScaleValue = 0;
        }
#endif
    u32ODCLK_KHZ = u32ODCLK_KHZ * 1000;
#else
#if FRAME_BFF_SEL == FRAME_BFFLESS
#if ENABLE_FREESYNC
	if(IS_DP_FREESYNC())
	{
		u32ODCLK_KHZ = (float)msAPI_combo_IPGetPixelClk() *10 * u16dstHTotal / msAPI_combo_IPGetGetHTotal() * (PANEL_HEIGHT-1)  / ((u16Height-1)); // output dot clock in KHz
	}
	else
#endif
    u32ODCLK_KHZ = (float)XTAL_CLOCK_KHZ * 16UL / u16hPeriod * u16dstHTotal * (PANEL_HEIGHT-1) / (u16Height-1); // output dot clock in KHz
#else
    u32ODCLK_KHZ = (( float )GetVfreq() * GetVtotal() * u16dstHTotal + 500)/1000; // output dot clock in KHz
#endif
//printData("OOR oclk(%d)>PANEL_MAX_DCLK",u32ODCLK_KHZ/1000);
#if FRAME_BFF_SEL==FRAME_BFFLESS  // Need check .....648x480@72_185
    if ( (u32ODCLK_KHZ/1000>PanelMaxDCLK) || (oVtotal>PanelMaxVTotal) ) // if ouput clock is out of panel spec, then enable unsupported flag
    {  SrcFlags |= bUnsupportMode;
        //PRINT_DATA("OOR oclk(%d)>PANEL_MAX_DCLK",u32ODCLK_KHZ/1000);
        return FALSE;
    }
#endif
#endif

    // input/output frame div for frame sync
    	#if 0//ENABLE_FREESYNC
    if(IS_HDMI_FREESYNC())
	mStar_SetFrameDivider(SrcVFreq);//mStar_SetFrameDivider( (DWORD)(((XTAL_CLOCK_KHZ * 16UL * 1000 + u16hPeriod/2)/u16hPeriod +msRead2Byte(SC0_E2)/2)/(msRead2Byte(SC0_E2))) * ((SrcFlags&bInterlaceMode)?2:1) ); // param: input Vfreq
    else
		#endif
    mStar_SetFrameDivider( (DWORD)(((XTAL_CLOCK_KHZ * 16UL * 1000 + u16hPeriod/2)/u16hPeriod +SC0_READ_VTOTAL()/2)/(SC0_READ_VTOTAL())) * ((CURRENT_SOURCE_IS_INTERLACE_MODE())?2:1) ); // param: input Vfreq

    // ivs setting for nvidia VBI and PSV_4M TB
    mStar_SetIVS();

#if ( !PanelDualPort && !PANEL_QUAD_PORT) || (PanelTTL)// Single Port
    u32ODCLK_KHZ = u32ODCLK_KHZ * 2;
#endif // end of ##if LVDS_1ch
    mStar_LPLL1_Setting( u32ODCLK_KHZ/PANEL_H_DIV); // scalar dual engine: /2
    //MST_printData( "output dclk_OP22 %d", (u32ODCLK_KHZ+500)/1000 );
//printData("\r\n _____[%d]",g_SetupPathInfo.ucSCPathMode);
    // Case 1, 2: Set LPLL2
    if ( (g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_1) )
    {
        // clock changed according to 3D formats
        u32ODCLK_KHZ = ( float )XTAL_CLOCK_KHZ * 16UL / u16hPeriod * (PanelHTotal-PANEL_WIDTH+u16HDE_OP21) * ( (u16VDE_OP21) - 1 ) / ( u16Height - 1 ); // output dot clock in KHz
        mStar_LPLL2_Setting( u32ODCLK_KHZ/PANEL_H_DIV); // scalar dual engine: /2
        //printData( "output dclk_OP21 %d", (u32ODCLK_KHZ+500)/1000 );
    }
    else //Setting LPLL2 for LB clk
    {
        if( (g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_5)
         || (g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_6) )
            u32ODCLK_KHZ = u32Sclk;
#if (ENABLE_ROTATE_CLK_SET) && ((!PanelDualPort && !PANEL_QUAD_PORT) || (PanelTTL))
        else
            u32ODCLK_KHZ = u32ODCLK_KHZ / 2;
#endif
        mStar_LPLL2_Setting( u32ODCLK_KHZ/PANEL_H_DIV*21/20); // +5% for SSC
        //MST_printData( "LPLL2 clk (LB) %d", (u32ODCLK_KHZ*21/20+500)/1000 );
    }
    //  Timing Gen Setting
    mStar_SetTimingGen(u16HDE_OP21, u16VDE_OP21, FALSE);
	
	SC0_SET_OUTPUT_VTOTAL(0xFFF);//msWrite2ByteMask(SC0_1E, 0xFFF, SC_MASK_V); // output vtotal //Jison 110527 lost to add.

    mStar_CheckFastClock(u16hPeriod); //Jison 110413
    mStar_SetPanelSSC( 0, 0 );
    Delay1ms( 2 );
    msWriteByteMask( SC0_02, 0, BIT7); // enable lock input mode
    Delay1ms(100);

#if FRAME_BFF_SEL == FRAME_BUFFER
    msWriteByteMask( REG_3818, BIT3, BIT3 ); //[3] frame lpll enable
#endif
#if ENABLE_FACTORY_SSCADJ // 2011-09-16 19:46 CC
    mStar_SetPanelSSC(FactorySetting.SSCModulation, FactorySetting.SSCPercentage);
#else
    mStar_SetPanelSSC(PANEL_SSC_MODULATION_DEF, PANEL_SSC_PERCENTAGE_DEF);
#endif
    msWriteByteMask(REG_3876 ,    0, BIT0); // resync
    msWriteByteMask(REG_3876 , BIT0, BIT0);
    msWriteByteMask(REG_3876 ,    0, BIT0);

	


    return TRUE;
}

