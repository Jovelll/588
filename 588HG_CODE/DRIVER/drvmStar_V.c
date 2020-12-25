
///////////////////////////////////////
// DRVMSTAR_V VERSION: V02
////////////////////////////////////////
#include "global.h"

#ifndef _DRVMSTAR_V_C_
#define _DRVMSTAR_V_C_
#define drvmStar_DEBUG    1
#if ENABLE_DEBUG&&drvmStar_DEBUG
    #define drvmStar_printData(str, value)   printData(str, value)
    #define drvmStar_printMsg(str)           printMsg(str)
#else
    #define drvmStar_printData(str, value)
    #define drvmStar_printMsg(str)
#endif


code RegUnitType tblInit[]=
{
    //=========================================================================
#if 1
    // display timing OP2-1
    // output DE size // 0x10~0x1D
    {SC0_10, 0}, // Vert. DE start
    {SC0_11, 0}, // Vert. DE start
    {SC0_14, ( PANEL_HEIGHT - 1 ) & 0xFF}, // Vert. DE end
    {SC0_15, ( PANEL_HEIGHT - 1 ) >> 8}, // Vert. DE end
    {SC0_12, ( PanelHStart/PANEL_H_DIV ) & 0xFF}, // Hor. DE start
    {SC0_13, ( PanelHStart/PANEL_H_DIV ) >> 8}, // Hor. DE start
    {SC0_16, ( ( PanelHStart/PANEL_H_DIV ) + (PANEL_WIDTH/PANEL_H_DIV) - 1 ) & 0xFF}, // Hor. DE end
    {SC0_17, ( ( PanelHStart/PANEL_H_DIV ) + (PANEL_WIDTH/PANEL_H_DIV) - 1 ) >> 8}, // Hor. DE end


    // Output Sync Timing //0x1E~0x26
#if DOWN_COUNTER_V_SYNC //Vsync using down counter
    {SC0_1E, ( PanelVTotal - 1 ) & 0xFF}, // output vtotal
    {SC0_1F, ( ( PanelVTotal - 1 ) >> 8 ) | BIT7}, // output vtotal
    {SC0_20, 0x02}, // vsync start
    {SC0_22, 0x01}, //vsync end
#else //Vsync using v counter
    {SC0_1E, ( PanelVTotal - 1 ) & 0xFF}, // output vtotal
    {SC0_1F, ( PanelVTotal - 1 ) >> 8}, // output vtotal
    {SC0_20, ( PanelVTotal - PanelVStart ) & 0xFF}, // vsync start
    {SC0_21, ( PanelVTotal - PanelVStart)>>8}, // vsync start
    {SC0_22, ( PanelVTotal - PanelVStart ) >> 8}, // vsync start
    {SC0_22, ( PanelVTotal - PanelVSyncBackPorch ) & 0xFF}, //vsync end
    {SC0_23, ( PanelVTotal - PanelVSyncBackPorch ) >> 8}, //vsync end
#endif
    //{SC0_02, 0x80},
    {SC0_24, ( PanelHTotal/PANEL_H_DIV - 1 ) & 0xFF}, // output htotal
    {SC0_25, ( PanelHTotal/PANEL_H_DIV - 1 ) >> 8}, // output htotal
    {SC0_26, ( PanelHSyncWidth/PANEL_H_DIV ) - 2}, //-1}, //

    // Scaling Image window size
    {SC0_18, ( PanelHStart/PANEL_H_DIV ) & 0xFF},
    {SC0_19, ( PanelHStart/PANEL_H_DIV ) >> 8},
    {SC0_1C, ( ( PanelHStart/PANEL_H_DIV ) + (PANEL_WIDTH/PANEL_H_DIV) - 1 ) & 0xFF},
    {SC0_1D, ( ( PanelHStart/PANEL_H_DIV ) + (PANEL_WIDTH/PANEL_H_DIV) - 1 ) >> 8},

    {SC0_1A, ( PANEL_HEIGHT - 1 ) & 0xFF},
    {SC0_1B, ( PANEL_HEIGHT - 1 ) >> 8},
    {SC0_27, BIT6 | BIT5 | BIT1}, // enable auto-htotal

    //{SC0_28, BIT5}, // setup clock mode
    {SC0_49, 0xAF},
#endif
// FFC
    {REG_3042, (BIT7&PANEL_SWAP_PN)|(BIT4&LVDS_TIMode)|(PanelSwapOddRB&BIT1)|(PanelSwapEvenRB&BIT0)| // setup output port
        (PanelSwapMLSB&BIT5)|(PanelSwapOddML&BIT3)|(PanelSwapEvenML&BIT2)}, // setup
    // output signal control 0xF2
    {REG_30F2, (PanelDClkDelay<<4)|(BIT3&PanelInvDClk)|(BIT2&PanelInvDE)|
        (BIT1&PanelInvVSync)|(BIT0&PanelInvHSync)}, // output signal polarity control
  #if (CHIP_ID == CHIP_TSUMV)
    {REG_3043,  (PanelDualPort&BIT0)|(PanelSwapPort&BIT1)},
  #else
    {SC0_43,  (PanelDualPort&BIT0)|(PanelSwapPort&BIT1)},
  #endif

    {SC0_6C, 0x90}, // short line tune coef. // 120831 coding, modified default value 0x70 to 0x90

    // Clock generator control 0xD0~0xDB
    {SC0_D0, 0x00},
    {REG_30AA, 0x00}, // BK0_D0[0] Rose <--> REG_30AA[0] CHIP_TSUML
    //{REG_1ED1, BIT7}, // setup output pll & master pll  //// 20071219 RD Anson suggest for B1/B2/Goya enable Bit7 fixed line clock in 172Mhz
    //{REG_1ED1, BIT2}, //Jison110311 move to mcuSetSystemSpeed()
    // Master PLL
    //{MPLL_M, 0x6F},//0x72:12MHz // set master pll as 215 MHz & drive current
    //{REG_1ED2, 0x0F}, //Jison110311 move to mcuSetSystemSpeed()
    {REG_1ED3, 0x40},
    #if PanelLVDS
    {REG_1ED4, 0x03},
    #else
    {REG_1ED4, 0x03},
    #endif



    // Dither control //0x36~0x38

#if PanelDither==6
    #if defined(_6BIT_EN_)  //Steven110126, for Output pixel bits number
    {SC0_36, 0x03},
    #else
    {SC0_36, 0x01},
    #endif
#else
    {SC0_36, 0x00},
#endif
    {SC0_37, DITHER_COEFF}, //coefficient
    {SC0_38,     DITHER_METHOD}, // dither method

    {SC0_BC, BIT2 | BIT5 | BIT7}, // 071026 RD suggest, New added for auto position error
    // display port

    {SC0_2B, 0x00}, //set SC0_2B[7]=0, Antony

    // border color 0x32~0x35
    {SC0_32, 0},// panel background color

    // Auto Adjustment 0x78~
    {SC0_7B, 0x1}, // enable auto position
    {SC0_7C, 0x40},//Albert 070118
    {SC0_8B, 0x19}, // enable auto phase
    {SC0_78, 0},//x11}, // enable auto Gain
    {SC0_8A, 0x1C}, // enable auto phase

    // VSync Status
    {SC0_9B, 0x30},
    {SC0_9E, 0x20},

    // interrupt control
    {SC0_CA, 0x0F}, // BIT0 for level trigger, BIT1 for priority
    {SC0_CE, 0},
    {SC0_CF, 0},

    {SC0_E6, 3},//1},//10}, // RD suggest 20081008
    //{SC0_E7, 1}, //default=1
    {SC0_E5, 0},
    {SC0_EB, 0x1E},
    // interlace judgement
    //{SC0_E8, 0}, //0x01} //1:non-interlace default=0
    {SC0_E9, BIT7}, // 20071219 RD Anson suggest for B1/B2/Goya, bk0_80~81 don't need to -0x10
   // {0x5E, 0x20},// Coring threshold
    //{SC0_FA, BIT2}, //20091215 Disable Auto switch clock for SC0_5B[7..6]

    // SAR
    {REG_3A60, 0x20},
    {REG_3A61, 0x20},
    {REG_3A62, SARKEY_EN|CABLE_DET_SAR_EN}, // SARADC_CTRL //coding

    {SC0_00, 0x00},
};

void msInitClockGating( void )
{
    msWriteByteMask(REG_1E24, 0x00, _BIT0);                 // osd clk
    msWriteByteMask(REG_1E35, 0x00, _BIT0);                 // fclk (LB clk)
    msWriteByteMask(REG_1E37, 0x00, _BIT0);                 // odclk1
    msWriteByteMask(REG_1E3E, 0x00, _BIT0);                 // IDCLK
    msWriteByteMask(REG_1E3F, 0x00, _BIT0);                 // HSPCLK
    msWriteByteMask(REG_1E47, _BIT2, _BIT3|_BIT2|_BIT0);    // sysclk_ip
}

///////////////////////////////////////////////////////////////////////////////
// <Function>: msTMDSInit
//
// <Description>: HDMI and HDMI audio initial function
//
// <Parameter>: None
//
///////////////////////////////////////////////////////////////////////////////
void msTMDSInit(void)
{
    // DVI setting
    // DVI power enable
    //msWriteByteMask(REG_PM_B2, 0, BIT2);         // power on DVI CKIN. (port 0 and port 1) (reg_b2[2])
    // DVI enable
    msWriteByteMask(REG_2900, BIT2, BIT2);      // [2]: DVI enable
    // 1200301 coding, set EQ to 1
    msWriteByte(REG_2842, 0x01); // EQ strength level 5'b 00011
    msWriteByte(REG_2844, 0x01); // EQ strength level 5'b 00011
    msWriteByte(REG_2846, 0x01); // EQ strength level 5'b 00011
    msWriteByte(REG_29FE,0x00);                 // DVI phase dac power on for CHIP_TSUML2
    // Power up Xtal clocks and DVI detection clock for DVI clock detection
    //msWriteWordMask(REG_290D, 0, 0x601);      // HDCP related power
    // DVI PLL power control
    //msWriteWordMask(REG_29C1, 0, 0xFFFF);     // enable DVI0 PLL power

    // DVI slowly update
    #if 0
    msWriteByteMask(REG_2853, 0x0E, 0x0F);      // [15]:update Bch slowly; [14:8]: 0x63(100 lines)
    #endif

    // Enable DVI clock tolerance: 4*0.7 = 2.8 MHz
    msWriteByteMask(REG_2827, 0x20, 0x70);      //the comp value is reg_comp_tol * 4
    msWriteByteMask(REG_2825, 0xF0, 0xF0);      //0x110A25=0xB0, DVI clock detection counter tolerance set to 7
    msWriteByte(REG_2852,0x10);                 // [4]: enable auto DVIPLL adjust
}

void drvmStar_Init( void )
{
    BYTE i;

    msInitClockGating();

    msTMDSInit();

    msWriteByteMask(SC0_02, BIT7, BIT7);

    for( i = 0; i < sizeof( tblInit ) / sizeof( RegUnitType ); i++ )
        msWriteByte( tblInit[i].u16Reg, tblInit[i].u8Value );

    msWriteByteMask( SC0_48, 0, BIT4 );                     //Turn off New mode, (Show line tune fast)

#if DISABLE_SHORT_LINE_TUNE
    msWriteByteMask(SC0_63, BIT0,  BIT1|BIT0);
    msWriteByteMask(SC0_28,    0,  BIT6);
#endif

#if DISABLE_SHORT_FRAME
    msWriteByteMask(SC0_63, BIT2, BIT2);
    msWriteByteMask(SC0_63, 0, BIT3);
#endif

#if DOUBLE_LVDS_CLK_SWING // 111227 coding addition, enable LVDS clock swing double.
    msWriteByteMask( REG_306B, BIT0, BIT0 );
#endif

}

//Average short line tuning
void mStar_STuneAverageMode(Bool bEnable)
{
    if(bEnable)
    {
        msWriteByteMask(SC0_FA, BIT3,BIT3);
    }
    else
    {
        msWriteByteMask(SC0_FA, 0,BIT3);
    }
}

void mStar_InterlaceModeSetting( void )
{
#define DELAY_LINES  1 //  default: 1

    if( CURRENT_SOURCE_IS_INTERLACE_MODE() )
    {
        msWriteByte(SC0_6F,0x00);
        msWriteByte(SC0_90, (DELAY_LINES)<<4);
        msWriteByteMask(SC0_0F,BIT7,BIT7); //shift line mode enable
        if(CURRENT_INPUT_IS_VGA())//(SrcInputType == Input_Analog1)
        {
            if(msReadByte(SC0_ED)&BIT5)
               msWriteByteMask(SC0_E9,0,BIT0);
            else
               msWriteByteMask(SC0_E9,BIT0,BIT0);
        }
        else            //110921 Rick modified - B011
        {
            if(msReadByte(SC0_04)&BIT6)
            {
            msWriteByteMask(SC0_E9,0,BIT0);//0x01); //Jison 080925 for DVI interlace //0x00 090512
                msWriteByteMask(SC0_05,0,BIT0);
            }
            else
            {
                msWriteByteMask(SC0_E9,BIT0,BIT0);//0x01); //Jison 080925 for DVI interlace //0x00 090512
                msWriteByteMask(SC0_05,BIT0,BIT0);
            }
        }

        // mStar_WriteByte(BK0_74, 0x10); //enable Vertical 2-tap and CB0
        // mStar_WriteByte(BK0_90, 0x10); //set delay line=1 for 2-tap scaling
         //delay line number needs to calculate from formula
         mStar_STuneAverageMode(TRUE);
    }
    else
    {
        msWriteByte(SC0_6F,0x00);
        msWriteByte(SC0_90, (DELAY_LINES<<4));
        msWriteByteMask(SC0_0F,0,BIT7); //shift line mode disable
        msWriteByteMask(SC0_E9,0,BIT0);
       // mStar_WriteByte(BK0_74, 0x80); //enable Vertical 3-tap and WinSinc
       // mStar_WriteByte(BK0_90, 0x20); //set delay line=2 for 3-tap scaling
       mStar_STuneAverageMode(FALSE);
    }

}

void mStar_SetPanelSSC( BYTE freqMod, BYTE range )
{
    DWORD xdata dwFactor;
    WORD xdata wStep;
    WORD xdata wSpan;
    DWORD xdata u32MPLL_MHZ = 429ul;

    if( (freqMod == 0) || (range == 0) )
    {
        msWriteByte(REG_1ED8, 0);
        msWriteByteMask(REG_1ED9, 0, (BIT2|BIT1|BIT0));
        msWrite2Byte(REG_1EDA, 0);
        return;
    }

    // freqMod: 0~40 means 0~40K
    if(freqMod > PANEL_SSC_MODULATION_MAX)
        freqMod = PANEL_SSC_MODULATION_MAX;

    // range: 0~30 means 0~3%
    if(range > PANEL_SSC_PERCENTAGE_MAX)
        range = PANEL_SSC_PERCENTAGE_MAX;

    dwFactor = msReadByte(REG_1ED7);
    dwFactor <<= 16;
    dwFactor |= msRead2Byte(REG_1ED5);

    //wSpan = ((14318ul*30/freqMod) * 131072ul) / dwFactor;
    //wStep = (float)(((dwFactor*freqMod*range/10)/14318ul)*(dwFactor/1500))/131072; // 120320 coding modified
    wSpan = ((u32MPLL_MHZ*1024ul/freqMod) * 131072ul) / dwFactor;
    wStep = (float)(dwFactor*range/10)/wSpan/100;

    //MST_printData("PanelSSC STEP[%x]",wStep);
    //MST_printData("PanelSSC SPAN[%x]",wSpan);

    if (wStep > 0x7FF)
        wStep = 0x7FF;

    if(wSpan > 0x7FFF)
        wSpan = 0x7FFF;

    msWriteByte(REG_1ED8, (wStep & 0x00FF));
    msWriteByteMask(REG_1ED9, (wStep>>8), (BIT2|BIT1|BIT0));
    msWrite2Byte(REG_1EDA, wSpan);
    msWriteBit(REG_1ED3, _ENABLE, BIT6); // ssc enable
}

// 111220 coding test
void drvDVI_PowerCtrl(DVI_PowerCtrlType ctrl)
{
    if(ctrl == DVI_POWER_STANDBY)
    {
        // DVI power on
        msWriteByteMask(REG_PM_B2, 0, BIT2|BIT3); // power on DVI CKIN/PLL band-gap.
        // Power up Xtal clocks and DVI detection clock for DVI clock detection
        msWrite2ByteMask(REG_290C, 0x7FFE, 0xFFFF);
        // DVI PLL power control
        msWrite2ByteMask(REG_29C0, 0xFFFF, 0xFFFF); // power down DVI0 PLL power
    }
    else if(ctrl == DVI_POWER_DOWN)
    {
        // DVI power down
        msWriteByteMask(REG_PM_B2, BIT2|BIT3, BIT2|BIT3); // power down DVI CKIN/PLL band-gap.
        // Power up Xtal clocks and DVI detection clock for DVI clock detection
        msWrite2ByteMask(REG_290C, 0x7FFE, 0xFFFF);
        // DVI PLL power control
        msWrite2ByteMask(REG_29C0, 0xFFFF, 0xFFFF); // power down DVI0 PLL power
    }
    else
    {
        // DVI power on
        msWriteByteMask(REG_PM_B2, 0, BIT2|BIT3); // power on DVI CKIN/PLL band-gap.
        msWrite2ByteMask(REG_290C, 0, 0xFFFF); // power on DVI power
        msWrite2ByteMask(REG_29C0, 0, 0xFFFF); // power on DVI PLL power
    }
}
void mStar_IPPowerControl(void)
{
    if (CURRENT_INPUT_IS_VGA())//( (SrcInputType == Input_Analog1) ) // ADC
    {
        // ADC power on
        drvADC_PowerCtrl(ADC_POWER_ON);

  #ifdef _IP_POWER_CTRL_
        drvDVI_PowerCtrl(DVI_POWER_DOWN);
  #endif
    }
    else if( (CURRENT_INPUT_IS_DVI())//(SrcInputType == Input_Digital)
        #if ENABLE_HDMI
        || (CURRENT_INPUT_IS_HDMI())//(SrcInputType == Input_HDMI)
        #endif // #if Enable_DVI2
    ) // DVI / HDMI
    {
  #ifdef _IP_POWER_CTRL_
        drvDVI_PowerCtrl(DVI_POWER_ON);
  #endif
        // ADC power down
        drvADC_PowerCtrl(ADC_POWER_DOWN); //CHIP_TSUMV need power down ADC for DVI good phase
    }
}

void msTMDSSetMux( InputPortType inport )
{
    if(INPUT_IS_DVI(inport))//( inport == Input_Digital )
    {
        //PORT A/B
        msWriteByteMask(REG_PM_B2, 0, BIT2|BIT3); // power down DVI CKIN/PLL band-gap.
        // Power up Xtal clocks and DVI detection clock for DVI clock detection
        msWrite2ByteMask( REG_290C, 0, 0xFFFF );

        // DVI PLL power control
      #if DVI_PORT==TMDS_PORT_B
        msWrite2ByteMask( REG_29C0, 0x0800, 0xFFFF ); // enable DVI0 PLL power, [12:11]: portB/A Data R-term
        msWriteByteMask( REG_PM_B3, BIT5, BIT5 | BIT4 ); //DVI clock mux: [5]: port 1(portB), [4]: port 0(portA)
        msWriteByteMask( REG_29C2, BIT1, BIT1|BIT0); // [1:0] port A/B data mux: 10: port B; 01: port A
      #else //TMDS_PORT_A
        msWrite2ByteMask( REG_29C0, 0x1000, 0xFFFF ); // enable DVI0 PLL power, [12:11]: portB/A Data R-term
        msWriteByteMask( REG_PM_B3, BIT4, BIT5 | BIT4 ); //DVI clock mux: [5]: port 1(portB), [4]: port 0(portA)
        msWriteByteMask( REG_29C2, BIT0, BIT1|BIT0); //  [1:0] port A/B data mux: 10: port B; 01: port A
      #endif
    }
    else // non-DVI/HDMI ports
    {
        msWriteByteMask(REG_PM_B2, BIT2|BIT3, BIT2|BIT3); // power down DVI CKIN/PLL band-gap.
        // Power up Xtal clocks and DVI detection clock for DVI clock detection
        msWrite2ByteMask( REG_290C, 0x7FFF, 0xFFFF );
        // DVI PLL power control
        msWrite2ByteMask( REG_29C0, 0xFFFF, 0xFFFF ); // enable DVI0 PLL power, [12:11]: portB/A Data R-term

        msWriteByteMask(REG_01B3, 0, BIT5|BIT4); //DVI clock mux: [5]: port 1, [4]: port 0
        msWriteByteMask(REG_29C2, 0, BIT1|BIT0); //DVI data mux: [1]: port 1, [0]: port 0
    }
}

//*******************************************************************
// Function Name: mStar_SetScalingFactor
//
// Decscription: setup scaler hor/ver scaling factor to expand image to full screen
//
// caller: mSar_WriteByte(), msReadByte() in ms_rwreg.c
///
// callee: mStar_SetupMode() in mstar.c
//*******************************************************************
#define HSC_offset  1
void mStar_SetScalingFactor( void )
{
    DWORD factor;
    WORD width, height;
    WORD Expansionwidth;

    width = OverScanSetting.FinalCapWinWidth;//msRead2Byte(SC0_0B)&SC_MASK_H;
    height = OverScanSetting.FinalCapWinHeight; //msRead2Byte(SC0_09)&SC_MASK_V;
    Expansionwidth = OverScanSetting.OverScanH;

    if( CURRENT_SOURCE_IS_INTERLACE_MODE() )// if input timing is interlace, must use 1/2 height to get scaler factor
        height /= 2;

    if( width != OverScanSetting.OverScanH )        // set expansion factor
    {
        factor = width - HSC_offset; // factor=((input width-1)*2^20)/(output width-1) + 1
        factor = (( DWORD )( factor << 20 ) ) / ( Expansionwidth - HSC_offset ) + 1;
        msWrite2Byte(SC0_50, factor & 0xFFFF );
        msWriteByte(SC0_52, (( factor >> 16 ) & 0xFF ) | BIT7);
    }
    else
        msWriteByte(SC0_52, 0 );    // input width=output width, then disable expansion

    // Setup vertical expansion
    if( height != OverScanSetting.OverScanV )
    {
        factor = height - 1; // factor=((input height-1)*2^20)/(output height-1) + 1
        factor = (( DWORD )( factor << 20 ) ) / ( PanelHeight - 1 ) + 1;
        msWrite2Byte(SC0_53, factor & 0xFFFF );
        msWriteByte(SC0_55, (( factor >> 16 ) & 0xFF ) | BIT7);
    }
    else
        msWriteByte(SC0_55, 0x00 );    // input height=output height, then disable expansion

	#if Enable_Expansion
	IsSupportedAspRatioMode(OverScanSetting.FinalCapWinWidth,OverScanSetting.FinalCapWinHeight);
	#endif
}


#if defined(_ENABLE_LPLL_FINETUNE_)
void mStar_LPLLFineTune(WORD u16DST_HTotal)
{
   DWORD   u32Factor;
   WORD u16HttFraction;
   WORD u16LockHtt, u16OHtt;

   msWriteByteMask(REG_1ED3, 0,BIT6);// disable SSC
   Delay1ms(40);// at least 2 frame delay after LPLL update for fraction value stable
   //PRINT_DATA("\r\n u16DST_HTotal=%x",u16DST_HTotal);

   msWriteByteMask(SC0_96, BIT7,BIT7);
   u16HttFraction = msRead2Byte(SC0_91)&0x0FFF;// Read Fraction
   //PRINT_DATA(" u16HttFraction :%x",u16HttFraction);

   u32Factor = msReadByte(REG_1ED7); // Read output dot clock
   u32Factor=(u32Factor<<16);
   u32Factor+= msRead2Byte(REG_1ED5);
   //PRINT_DATA(" REG_1ED7=%x",msReadByte(REG_1ED7));
   //PRINT_DATA(" REG_1ED4_5=%x",msRead2Byte(REG_1ED5));
   //((float)u16OHtt*(PANEL_HEIGHT-1)+3), +3 to avoid too close to cause fraction value floating
   #if 1 //Jison 110727 use theoretical value u16DST_HTotal to make sure get the normal dclk divider
   u32Factor = (((float)u16DST_HTotal*(PANEL_HEIGHT-1)+u16HttFraction)/((float)u16DST_HTotal*(PANEL_HEIGHT-1)+3))*u32Factor;
   #else
   u16OHtt = msRead2Byte(SC0_24)&0x0FFF;
   u16LockHtt = msRead2Byte(SC0_97)&0x0FFF;
   PRINT_DATA("\r\n u16LockHtt :%x",u16LockHtt);
   PRINT_DATA(" u16OHtt :%x",u16OHtt);
   u32Factor = (((float)u16LockHtt*(PANEL_HEIGHT-1)+u16HttFraction)/((float)u16OHtt*(PANEL_HEIGHT-1)+3))*u32Factor;
   #endif
   // program LPLL parameters to generate expected output dclk
   msWrite2Byte(REG_1ED5, u32Factor&0xFFFF);
   msWriteByte(REG_1ED7, u32Factor>>16); // setup output dot clock
   #if 0//DEBUG_EN
   Delay1ms(40); //at least 2 frame
   PRINT_DATA(" NEW_Fration :%x",msRead2Byte(SC0_91)&0x0FFF);
   //PRINT_DATA(" new REG_1ED7=%x",msReadByte(REG_1ED7));
   //PRINT_DATA(" new REG_1ED4_5=%x\r\n",msRead2Byte(REG_1ED5));
   #endif
   msWriteByteMask(SC0_96, 0,BIT7);
   msWriteByteMask(REG_1ED3, BIT6,BIT6);// Enable SSC
}
#endif

void mStar_CheckFastClock(WORD u16SCLK, WORD u16DCLK)
{
#if ENABLE_LBCLK_FIXED_216M

    u16SCLK = u16SCLK;
    u16DCLK = u16DCLK;
    msWriteByteMask(REG_1E35, _BIT3, _BIT3|_BIT2);

#else
#define FIXCLK (172)
#define CLK_TOLERANCE       (3)//(SAFE_TOLERANCE+SRC_CLK_TOLERANCE)

#if ENABLE_DEBUG
printData(" CheckFastClock_sclk:%d", u16SCLK);
printData(" CheckFastClock_dclk0:%d", u16DCLK);
#endif

    // dclk add SSC setting
    // 120321 coding, modified for ssc percentage, original 0~3 means 0~3%, modified 0~30 means 0~30%
    //u16DCLK = u16DCLK + (u16DCLK * PANEL_SSC_PERCENTAGE_DEF) / 100 + 1;
    // 120322 coding modified
    u16DCLK = u16DCLK - (u16DCLK * PANEL_SSC_PERCENTAGE_DEF/10) / 100;

#if ENABLE_DEBUG
//printData(" PANEL_SSC_PERCENTAGE_DEF:%d", PANEL_SSC_PERCENTAGE_DEF);
printData(" CheckFastClock_dclk1:%d", u16DCLK);
#endif

#if ( !PanelDualPort && !PanelRSDS)
    u16DCLK /= 2;
#endif

    if(u16DCLK >= u16SCLK+CLK_TOLERANCE) // dclk
        msWriteByteMask(REG_1E35, _BIT2, _BIT3|_BIT2);
    else if((FIXCLK-CLK_TOLERANCE)>u16SCLK && (FIXCLK-CLK_TOLERANCE)>u16DCLK) // 172MHz
        msWriteByteMask(REG_1E35, 0, _BIT3|_BIT2);
    else // 216MHz
        msWriteByteMask(REG_1E35, _BIT3, _BIT3|_BIT2);
#endif    
}


//*******************************************************************
// Function Name: mStar_SetScalingFilter
//
// Decscription: Setup scaling filter for sharpness function
//
// callee: mStar_WriteByte() in ms_rwreg.c
//
// caller: mStar_SetupMode() in mStar.c
//*******************************************************************
//@brief  Set vertical scaling filter for sharpness.
//@param  u8Filter ranges from 0(no-scaling), 1(scaling up), 2(scaling down)
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
BYTE code tHFilterTbl[]=
{
    0x00,   // no-scaling
    0x02,   // upscaling
    0x04    // downscaling
};

static void mStar_SetVScalingFilter(BYTE u8Filter)
{
    //v filter
    if ( (u8Filter&0x0F)==0 )
    {
        msWriteByteMask( SC0_5A,  0, (BIT1|BIT0) ); /* Vertical scaling filter is Bilinear mode */
    }
    else
    {
        BYTE i;
#if EN_V4TAP_SCALING
        msWriteByteMask( SC0_5A,  2, (BIT1|BIT0) ); /* Vertical scaling filter is  4-tap scaling mode */
        msWriteByte( SC0_00 ,  0x07 ); // SC7
        for ( i = 0; i < V_COEF_NUM; i++ )
            msWriteByte( tVScalingCoeff[ 0 ][ i ], tVScalingCoeff[ V4TAP_COEF_SEL ][ i ] );

        msWriteByte( SC0_00 ,  0x00 ); // SC0
#else
        msWriteByteMask( SC0_5A,  1, (BIT1|BIT0) ); /* Vertical scaling filter is  Register define mode */
        for ( i = 0; i < V_COEF_NUM; i++ )
        {
            msWriteByteMask( SC0_5A,  (tVScalingCoeff[ 0 ][ i ]), (BIT4|BIT3|BIT2) );     /*Bit4~2*/
            msWriteByte( SC0_57 ,  tVScalingCoeff[ u8Filter&0x0F ][ i ] );
        }
#endif
    }
}

//@brief  Set horizontal scaling filter for sharpness.
//@param  u8Filter ranges from 0(no-scaling), 1(scaling up), 2(scaling down)
static void mStar_SetHScalingFilter(BYTE u8Filter)
{
    //printData("  u8Filter=%x",u8Filter);
    msWriteByte( SC0_56, tHFilterTbl[u8Filter] );
}

//*******************************************************************
// Function Name: mStar_SetScalingFilter
//
// Decscription: Setup scaling filter for sharpness function
//
// callee: mStar_WriteByte() in ms_rwreg.c
//
// caller: mStar_SetupMode() in mStar.c
//*******************************************************************
void mStar_SetScalingFilter(void)
{
    WORD u16PxlImgHDE = OverScanSetting.FinalCapWinWidth;
    WORD u16PxlImgVDE = OverScanSetting.FinalCapWinHeight;
    WORD u16ImageOutH = OverScanSetting.OverScanH;
    WORD u16ImageOutV = OverScanSetting.OverScanV;

    // horizontal
    if ( u16PxlImgHDE == u16ImageOutH )
    {
        mStar_SetHScalingFilter( 0 );  // no-scaling
    }
    else if ( u16PxlImgHDE < u16ImageOutH )
    {
        mStar_SetHScalingFilter( 1 );  // scaling-up
    }
    else    // scaling-down
    {
        mStar_SetHScalingFilter( 2 );  // scaling-down
    }

    // vertical
    if ( u16PxlImgVDE == u16ImageOutV )
    {
        mStar_SetVScalingFilter( 0 );   // no-scaling
    }
    else if ( u16PxlImgVDE < u16ImageOutV )
    {
        mStar_SetVScalingFilter( 1 );   // scaling-up
    }
    else
    {
        mStar_SetVScalingFilter( 2 );   // scaling-down
    }
}

//Jison 091023 Do I_Gen_Tuning at turn on panel 1st time. 100305 alway enable


#if( USE_MOD_HW_CAL )
// MOD hardware current calibration
// @pre MOD is not in power-down (BK1_26[7]==0)
// @pre LPLL is not in power-down
void I_Gen_Tuning( void )
{
    BYTE i;	
    #define SWING_MASK	0x3F

    drvmStar_printMsg( "I_Gen_Tuning_Start" );

    msWriteByteMask(REG_306B,    0, BIT2);       /* Enable MOD self current bias.*/
    msWriteByteMask(REG_306D,    0, BIT4);       /* disable MOD HW Calibration */
    msWriteByteMask(REG_306B,    0, BIT6);       /* Select calibration source: (0)PADA_OUT[PN]_CH[4](PAD_LHSYNC(LVB3P)/PAD_LVSYNC(LVB3M))    (1)PADA_OUT[PN]_CH[5](PAD_R_ODD_6(LVA0P)/PAD_R_ODD_7(LVA0M)) */
    msWriteByteMask(REG_306B, ((LVDS_VOLTAGE_TARGET)<<4), BIT5|BIT4);  /* Select calibration target: (00)250mV (01)300mV (10)350mV (11)200mV */
                                                                       //REG_306B[6] can select cal src
    msWriteByteMask(REG_306D, BIT4, BIT4);   /*MOD Calibration enable. */
    SetTimOutConter(50);
    //u16TimeOutCounter = 50;
    //bTimeOutCounterFlag=1;	
    while (! ( msReadByte( REG_306E ) & BIT7 ) && u16TimeOutCounter);             // wait for MOD HW calibration done

    if (msReadByte( REG_306E )&BIT6)
    {
        i = LVDS_FAIL_TARGET;//SWING_MASK/2;
        msWriteByteMask(REG_306D,	0x00,  BIT4); /* Disable MOD HW Calibration */
        msWriteByteMask( REG_306C, i, SWING_MASK); //Disable S/W Cal. and swing write to register

        drvmStar_printMsg(" @@@@@@@@@                       HW MOD Cali Fail !!");
    }
    else
    {
        drvmStar_printMsg(" @@@@@@@@@                       HW MOD Cali Pass !!");
        drvmStar_printData("-----REG_306C=0x%x\n\r", msReadByte(REG_306C)&SWING_MASK);  // REG_306C[5:0] is calibration result		
    }

    iGenTuningFinished=1;
}

#else //SW
// @brief MOD Current Calibration
// @pre   Current bank is bank-0.
// @pre   LPLL is on, i.e., ASSERT( BK0_D1[4]==0 )
void I_Gen_Tuning( void )
{
    BYTE i;
    volatile BYTE bLpllOff = msReadByte(REG_1ED1) & BIT4;
    volatile BYTE reg_30ab = msReadByte(REG_30AB);
    #ifdef _ENABLE_LVDS_OUT_CURRENT_  //Steven100506
    BYTE u8Temp1;
    #endif
    #define SWING_MASK  0x3F
    //MST_printMsg("I_Gen_Tuning_Start");

    //msWriteByteMask(REG_306C, BIT4, (BIT4|BIT3|BIT2|BIT1|BIT0) ); /*Tuning output current of self-igen=5b'10000.*/
    msWriteByteMask(REG_306C, (SWING_MASK+1)/2, SWING_MASK ); /*Tuning output current of self-igen=5b'10000.*/
    msWriteByteMask(REG_306B,    0, BIT2); /*Enable MOD self current bias.*/
    msWriteByteMask(REG_30AA, BIT0, BIT0); /*Enable LVDS/RSDS Test Mode enable.*/

    msWriteByte(REG_30AB, 0xFF);   /*LVDS/RSDS Test mode Data=0xFF.*/
    msWriteByteMask(REG_1ED1, BIT4, BIT4);   /*Power Down LPLL (LPLL will latch TSTDATA[7:1])*/

    msWriteByteMask(REG_306B, BIT1, BIT1);    /*MOD Calibration enable.*/
    msWriteByteMask(REG_306B,    0, BIT6);       /* Select calibration source: (0)PADA_OUT[PN]_CH[4](PAD_LHSYNC(LVB3P)/PAD_LVSYNC(LVB3M))    (1)PADA_OUT[PN]_CH[5](PAD_R_ODD_6(LVA0P)/PAD_R_ODD_7(LVA0M)) */
    msWriteByteMask(REG_306B,    ((LVDS_VOLTAGE_TARGET)<<4), BIT5|BIT4);  /* Select calibration target: (00)250mV (01)300mV (10)350mV (11)200mV */

    for (i=0; i<=SWING_MASK; i++)   /*? if LVDS swing > 233mV , then REG_306C[7](C_DDA_OUT)=1*/
    {
        msWriteByteMask(REG_306C, (BIT6|i),SWING_MASK|BIT6);
        ForceDelay1ms(10);

        if (msReadByte(REG_306C)&BIT7)
        {
		if ( i==0 ||  i==SWING_MASK)
		{
			msWriteByteMask(REG_306C, LVDS_FAIL_TARGET|BIT6,SWING_MASK|BIT6);						/* Set MOD output swing value for clock channel*/
			drvmStar_printMsg(" @@@@@@@@@						FW MOD Cali Fail !!");
		}
		else
		{
            		msWriteByteMask(REG_306C, i&SWING_MASK|BIT6,SWING_MASK|BIT6);			//Sky110712 //CHIP_TSUMV similar to TSUMT must set [6] to Enable MOD self current bias
			drvmStar_printData("-----REG_306C=0x%x", msReadByte(REG_306C)&SWING_MASK);
			drvmStar_printMsg(" @@@@@@@@@						FW MOD Cali Pass !!");
		}
            break;
        }
    }

    if ( ! (msReadByte( REG_306C )&SWING_MASK) )
    {
        msWriteByteMask(REG_306C, (SWING_MASK+1)/2, SWING_MASK);
    }

    #ifdef _ENABLE_LVDS_OUT_CURRENT_  //Steven100510, LVDS output current fine tuning
    else
    {
        u8Temp1=msReadByte(REG_306C)&SWING_MASK;
        if((u8Temp1+IGEN_LVDS_OUT_CURRENT)>=SWING_MASK)
            msWriteByteMask(REG_306C, u8Temp1,SWING_MASK);
        else
            msWriteByte(REG_306C, (u8Temp1+IGEN_LVDS_OUT_CURRENT),SWING_MASK);
        //PRINT_DATA("LVDS_OUT_CURRENT=0x%x", msReadByte(REG_306C)&SWING_MASK);
    }
    #endif
    msWriteByteMask(REG_306B, 0, BIT1);    /*MOD Calibration disable.*/

    msWriteByte(REG_30AB, reg_30ab);   /* restore original value of BK0_AB */
    msWriteByteMask(REG_30AA,    0, BIT0); /*Enable LVDS/RSDS Test Mode enable.*/
    if ( ! bLpllOff )
    {
        msWriteByteMask( REG_1ED1, 0, BIT4 );  // power on LPLL
    }

    iGenTuningFinished=1;
}

#endif  // end of #if( USE_MOD_HW_CAL )

void Power_ModCtrl(BYTE ucSwitch)
{
    if (ucSwitch)
    {
        msWriteByte(REG_3008, BIT3 | BIT2 | BIT0);  // enable LVDS mode
        msWriteByte(REG_3009, BIT3 | BIT2 | BIT0);  // enable LVDS mode
        msWriteByteMask(REG_306C, BIT6, BIT6);
    }
    else
    {
        msWriteByteMask(REG_306C, 0, BIT6);
        msWriteByte(REG_3008, BIT0);    // tri-state all output panel signal
        msWriteByte(REG_3009, BIT0);    // tri-state all output panel signal
    }
}

BYTE mStar_ScalerDoubleBuffer(Bool u8Enable)
{
    u8Enable=0;
    return u8Enable;
}

void mStar_SetDClkPLL(DWORD u32ODCLK_KHZ)
{
    typedef enum LPLL_INPUT_DIV_t
    {
      LPLL_INPUT_DIV_1 = 0,
      LPLL_INPUT_DIV_2 = 1,
      LPLL_INPUT_DIV_4 = 2,
      LPLL_INPUT_DIV_8 = 3,
    } LPLL_INPUT_DIV_t;

    typedef enum LPLL_OUTPUT_DIV_t
    {
      LPLL_OUTPUT_DIV_1 = 0,
      LPLL_OUTPUT_DIV_2 = 1,
      LPLL_OUTPUT_DIV_4 = 2
    } LPLL_OUTPUT_DIV_t;

    volatile DWORD             u32MPLL_KHZ;
    const    LPLL_INPUT_DIV_t  enLPLL_INPUT_DIV = LPLL_INPUT_DIV_1;
             LPLL_OUTPUT_DIV_t enLPLL_OUTPUT_DIV;
             BYTE              u8LPLL_LOOP_DIV;
    const    BYTE              u8LPLL_ICTL = 3;
             DWORD             u32Factor;

    #if( PanelDualPort )
    if( u32ODCLK_KHZ <= 50*1000UL )  // 40MHz~50MHz
    {
        enLPLL_OUTPUT_DIV = LPLL_OUTPUT_DIV_4;
        u8LPLL_LOOP_DIV   = 1;    // divide by 1
    }
    else if( u32ODCLK_KHZ <= 100*1000UL )    // 50MHz~100MHz
    {
        enLPLL_OUTPUT_DIV = LPLL_OUTPUT_DIV_2;
        u8LPLL_LOOP_DIV   = 2;    // divide by 2
    }
    else    // 100MHz~200MHz
    {
        enLPLL_OUTPUT_DIV = LPLL_OUTPUT_DIV_1;
        u8LPLL_LOOP_DIV   = 4;    // divide by 4
    }
    #else
    if( u32ODCLK_KHZ <= 50*1000UL )  // 40MHz~50MHz
    {
        enLPLL_OUTPUT_DIV = LPLL_OUTPUT_DIV_2;
        u8LPLL_LOOP_DIV   = 2;    // divide by 2
    }
    else    // 50MHz~100MHz
    {
        enLPLL_OUTPUT_DIV = LPLL_OUTPUT_DIV_1;
        u8LPLL_LOOP_DIV   = 4;    // divide by 4
    }
    #endif


    //@todo Change the following computation to compile-time, not run-time
    u32MPLL_KHZ = ( msReadByte(REG_1ED9) & BIT3 )? 214770UL : 429540UL;  // 214.77MHz or 429.54MHz

//        factor = ((float)u16hPeriod*524288ul/16*(float)u8LPLL_LOOP_DIV*(height-1))/((float)u16dstHTotal*7*(PANEL_HEIGHT-1))*((float)u32MPLL_KHZ/XTAL_CLOCK_KHZ);
    u32Factor = (float)(u32MPLL_KHZ / 7) * ((float)524288UL * (float)u8LPLL_LOOP_DIV * 2 / u32ODCLK_KHZ);

    #if (!PanelRSDS && !PanelDualPort)
    u32Factor/=2;
    #endif

    msWriteByteMask( REG_1ED3, (enLPLL_OUTPUT_DIV<<2)|(enLPLL_INPUT_DIV), (BIT3|BIT2)|(BIT1|BIT0) ); // REG_1ED3[3:2]=LPLL_OUTPUT_DIV;  REG_1ED3[1:0]=LPLL_INPUT_DIV
    msWriteByteMask( REG_1ED4, (u8LPLL_ICTL), (BIT2|BIT1|BIT0)); // REG_1ED4[2:0] = LPLL_ICTL
    msWriteByteMask( REG_1ED9, (u8LPLL_LOOP_DIV  <<4), (BIT7|BIT6|BIT5|BIT4) ); // REG_1ED9[7:4] = LPLL_LOOP_DIV

    // program LPLL parameters to generate expected output dclk
    msWrite2Byte(REG_1ED5, u32Factor&0xFFFF);
    msWriteByte(REG_1ED7, u32Factor>>16); // setup output dot clock

}

void drvmStar_SetupFreeRunMode( void )
{
    mStar_ScalerDoubleBuffer(FALSE);
    msWriteByteMask( SC0_02, BIT7, BIT7 ); // disable output Lock mode to enable free run.

  // disable output Lock mode to enable free run.
  mStar_SetDClkPLL(PanelDCLK*1000UL); //White101208 must use 1000UL to provent overflow truncate
  //mStar_ResetDClkPLL(); // reset output PLL
//  mStar_WriteByte(LPLL_SET_H, 0x24);
  msWrite2Byte(SC0_1E, PanelVTotal-1); // set output panel vtotal
  #if 1 //Jison 100818 avoid OSD garbage while change from PANEL_MIN_H_TOTAL to PANEL_H_TOTAL found on some panels
  msWrite2Byte(SC0_24, (PanelMinHTotal+8)-1); // set output panel htotal
  #else
  msWrite2Byte(SC0_24, PanelHTotal-1); // set output panel htotal
  #endif
}
void drvmStar_SetupInputPort_VGA(void)
{
    drvADC_SetInputMux(Input_VGA);
    mStar_SetAnalogInputPort( FALSE );    // switch to RGB port by default

    msWriteByte(SC0_04, VSE_B);
    msWriteByteMask(REG_1E22, 0,BIT3); //DE only mode for SC_TOP //recover

    msWriteByte(SC0_6F, 0x00);   // Interlace mode line shift
    msWriteByte(SC0_ED, 0x01);   // enable ADC coast
    msWriteByte(SC0_EE, 0x03);   // enable coast window start
    msWriteByte(SC0_EF, 0x01);   // enable coast window end
    msWriteByte(SC0_F3, 0x00);// disable DE glitch removal function & more tolerance for DE

    msWriteByteMask(REG_2E02, IP_MUX_ADC<<4, 0xF0);
    msWriteByteMask(REG_1E3E, IP_MUX_ADC<<4, 0x30);
    msWriteByte(SC0_02, NIS_B|ISEL_ANALOG1);
}
void mStar_SetTimingGen( WORD u16HDE_OP21, WORD u16VDE_OP21, BOOL OP21_FreeRun)
{
	u16VDE_OP21=u16VDE_OP21;
	OP21_FreeRun=OP21_FreeRun;
#if Enable_Expansion
    if (!EXPANSION_MODE_IS_FULL())
    {
        WORD u16HStart=( PANEL_WIDTH - OverScanSetting.OverScanH ) / 2 + msRead2Byte( SC0_12 )*PANEL_H_DIV;
        msWrite2ByteMask( SC0_18, u16HStart/PANEL_H_DIV,0xFFF );
        msWrite2ByteMask( SC0_1C, (u16HStart + OverScanSetting.OverScanH)/PANEL_H_DIV - 1 , 0xFFF);
    }
	else //restore
#endif
    {
        msWrite2ByteMask(SC0_18, ( PanelHStart/PANEL_H_DIV ), 0xFFF); //image H. start
        msWrite2ByteMask(SC0_1C, ( ( PanelHStart/PANEL_H_DIV ) + ((u16HDE_OP21)/PANEL_H_DIV) - 1 ), 0xFFF); //image H. end
    }
}

void drvmStar_SetupInputPort_DVI(void)
{
#if (DVI_RB_SWAP)
    msWriteByteMask(REG_281C,_BIT5,_BIT5);
#endif
#if (DVI_PN_SWAP)
    msWriteByteMask(REG_2818,_BIT7,_BIT7);
#endif

    msTMDSSetMux( Input_DVI );

#if (_TMDS_DE_MODE_EN_)
    msWriteByte( SC0_04, BIT3  | BIT7 |BIT6);
    msWriteByteMask( REG_1E22, BIT3, BIT3 ); // enable DE-only mode for SC_TOP
#else
    msWriteByte( SC0_04, BIT3  | BIT7);
    msWriteByteMask( REG_1E22, 0, BIT3 ); // enable DE-only mode for SC_TOP
#endif

    msWriteByte(SC0_F3, 0xF0);// enable DE glitch removal function & more tolerance for DE
    msWriteByteMask(REG_2E02, IP_MUX_DVI_A<<4, 0xF0);
    msWriteByteMask(REG_1E3E, IP_MUX_DVI_A<<4, 0x30);
    msWriteByte(SC0_02, NIS_B|ISEL_DVI|SCALER_STYPE_HV);
}

#if ENABLE_FREESYNC
#define max(a, b) (((a) > (b)) ? (a) : (b))
void msDrv_forceIP1VsyncPolarityEnable ()
{
    BYTE u8CountTime = 255;
    BYTE u8Posit=0,u8Negat=0;
    BYTE u8Pol=0;
    msWriteBit ( SC0_E8, FALSE, BIT4);
    if( IS_HDMI_FREESYNC() )
    {
        do
        {
            if (SC0_READ_SYNC_STATUS() & BIT0)
            {
                u8Posit++;
            }
            else
            {
                u8Negat++;
            }
            u8CountTime--;
        } while (u8CountTime > 0);

        u8Pol=max(u8Posit,u8Negat);
        if (u8Pol==u8Posit)
            u8Pol=TRUE;
        else
            u8Pol=FALSE;

        msWriteBit(SC0_E8, u8Pol, BIT5);
        msWriteBit (SC0_E8, TRUE, BIT4);

    }
}
#endif

#endif
