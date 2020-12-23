
///////////////////////////////////////
// DRVMSTAR_V VERSION: V02
////////////////////////////////////////
#include "global.h"
#include "Detect.h"

#ifndef _DRVMSTAR_2_C_
#define _DRVMSTAR_2_C_

#define drvmStar_DEBUG    1
#if ENABLE_DEBUG&&drvmStar_DEBUG
    #define drvmStar_printData(str, value)   printData(str, value)
    #define drvmStar_printMsg(str)           printMsg(str)
#else
    #define drvmStar_printData(str, value)
    #define drvmStar_printMsg(str)
#endif

code RegTblType tPowerOnOffTbl[]=
{
    {REG_1ED1, 0x00, BIT0},
    {REG_1EDC, 0x00, BIT5|BIT4},
    {REG_36F0, 0x00, BIT0},
    {REG_36FA, BIT7|BIT6, BIT7|BIT6},
    {REG_36EE, (BIT1&PanelDualPort)|BIT0, BIT1|BIT0},
    {REG_1100, 0x00, BIT5|BIT4|BIT3},
    {REG_1101, 0x00, BIT7},
    {REG_1133, 0x00, BIT7},
    {REG_3881, 0x00, BIT0},
    {REG_3A60, 0x00, BIT6},
    {REG_05F0, 0x00, BIT3},// [3]: AULL power down
#if 1    //  clock gating
    {REG_1E29, 0x00, BIT0},
    {REG_1E35, 0x00, BIT0},
    {REG_1E37, 0x00, BIT0},
    {REG_1E3E, 0x00, BIT0},
    {REG_1E3F, 0x00, BIT0},
#if ENABLE_USB_INPUT
    {REG_1E2E, 0x00, BIT6|BIT4|BIT2|BIT0},
#else
    {REG_1E2E, BIT6|BIT4|BIT2|BIT0, BIT6|BIT4|BIT2|BIT0},
#endif
    {REG_1E30, 0x00, BIT4|BIT0},
    {REG_1E25, 0x00, BIT2},
    {REG_1E28, 0x00, BIT0},
    {REG_1E24, 0x00, BIT4|BIT0},
    {REG_1E2C, 0x00, BIT4|BIT0},
#if ENABLE_R2_2DTO3D
    {REG_1E2A, 0x00, BIT0},
#else
    {REG_1E2A, BIT0, BIT0},
#endif
    {REG_1E47, 0x00, BIT0},
#endif
    {REG_29FE, 0x00, 0x3F},

};

void msWritePowerOnTble(void)
{
    BYTE i;
    for(i=0;i<sizeof(tPowerOnOffTbl)/sizeof(RegTblType);i++)
        msWriteByteMask(tPowerOnOffTbl[i].wReg,tPowerOnOffTbl[i].ucValue,tPowerOnOffTbl[i].ucMask );
}



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

    {REG_3680, (PanelSwapMLSB&BIT6)|(BIT5&PANEL_SWAP_PN)|(BIT2&LVDS_TIMode)|BIT3},
    {REG_368A, 0x3F},
    {REG_3693, (PanelSwapOddRB&BIT3)|(PanelSwapEvenRB&BIT5)|(PanelSwapOddML&BIT4)|(PanelSwapEvenML&BIT6)},
    {REG_3681, (PanelDClkDelay<<4)},
    {REG_3694, (PanelInvDClk&BIT4)|(PanelInvDE&BIT2)|(PanelInvVSync&BIT3)
               |(PanelDualPort&BIT1)|(PanelSwapPort&BIT0)},

    {REG_3689, PanelDualPort&BIT4},
    {REG_3695, (PanelInvHSync&BIT4)},
    {REG_3696, LVDS_TI_BitModeSel},
    {REG_3692, 0x00},
    //{REG_36DA, 0x55},
    //{REG_36DB, 0x55},
    //{REG_36DC, 0x05},
    {REG_36EE, BIT0|(BIT1&PanelDualPort)},
    {REG_36F0, BIT6|BIT0},
    {REG_380C, 0x00},
    {REG_380D, 0x00},
    {REG_380E, 0x03},
    {REG_3814, 0x00},
    {REG_3815, 0xD0},
    {REG_3816, 0x65},
    {REG_3817, 0x65},

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

    // Spectrum control
#if !defined(_SSC_BY_OUTPUT_DCLK_)  // the LPLL related settings are located in mStar_SetPanelTiming().
    {REG_382E, LOBYTE(SSC_STEP)},
    {REG_382F, HIBYTE(SSC_STEP)}, //bit8~bit4 have data, TSUMT should use _SSC_BY_OUTPUT_DCLK_
    {REG_3830, LOBYTE(SSC_SPAN)},
    {REG_3831, HIBYTE(SSC_SPAN)},
    {REG_381B, LPLL_SSC|0x03},
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

    // brightness 0x2A~0x2D
    {SC0_2A, BIT0}, // disable birghtness control
    // contrast 0x2E~0x31
    {SC0_2E, 0x60}, // enable contrast function and set 0 as center

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

#if 0//USE_HDMI_DVI_INT
    {REG_29E1, 0xFF}, //Disable HDMI INT_MASK
    {REG_05C0, 0xFF}, //Disable HDMI INT_MASK
    {REG_05C1, 0xFF}, //Disable HDMI INT_MASK
#endif

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

    msWriteByteMask(REG_1E29, 0x04, 0x3F);      // enable 432Mhz as AU_sync_clk   //[2]: AUPLL Synth. select 216MHz
    msWriteByteMask(REG_1E24, 0x00, _BIT0);                 // osd clk
    msWriteByteMask(REG_1E30, 0x00, BIT4|_BIT0);
    msWriteByteMask(REG_1E35, 0x00, _BIT0);                 // fclk (LB clk)
    msWriteByteMask(REG_1E37, 0x00, _BIT0);                 // odclk1
    msWriteByteMask(REG_1E3E, 0x00, _BIT0);                 // IDCLK
    msWriteByteMask(REG_1E3F, 0x00, _BIT0);                 // HSPCLK
    msWriteByteMask(REG_1E47, _BIT2, 0x0F);    // sysclk_ip
#if ENABLE_R2_2DTO3D
    msWriteByteMask(REG_1E2A, 0x00, BIT0);
#else
    msWriteByteMask(REG_1E2A, BIT0, BIT0);
#endif
#if ENABLE_USB_INPUT
    msWriteByteMask(REG_1E2E, 0x00, BIT6|BIT2|BIT4|BIT0);
#else
    msWriteByteMask(REG_1E2E, BIT6|BIT2|BIT4|BIT0, BIT6|BIT2|BIT4|BIT0);
#endif
    msWriteByteMask(REG_1E2A,BIT4,0x1F);
    msWriteByteMask( REG_3881, 0x00, 0x01);


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
    // DVI power enable
    //msWriteByteMask(REG_PM_B2, 0, BIT2);         // power on DVI CKIN. (port 0 and port 1) (reg_b2[2])
    // DVI enable
    msWriteByteMask(REG_2900, BIT2, BIT2);      // [2]: DVI enable
    // 1200301 coding, set EQ to 1
    msWriteByte(REG_2842, 0x01); // EQ strength level 5'b 00011
    msWriteByte(REG_2844, 0x01); // EQ strength level 5'b 00011
    msWriteByte(REG_2846, 0x01); // EQ strength level 5'b 00011
    msWriteByte(REG_29FE,0x00);                 // DVI phase dac power on for CHIP_TSUML2

    msWrite2Byte(REG_281E, 0x8165); // Set compvalue 40
    msWrite2Byte(REG_2820, 0x02CB); // Set compvalue 80
    msWrite2Byte(REG_2822, 0x0596); // Set compvalue 160
    msWriteByteMask(REG_29C9,0xC0,0xC0);

    // Power up Xtal clocks and DVI detection clock for DVI clock detection
    //msWriteWordMask(REG_290D, 0, 0x601);      // HDCP related power
    // DVI PLL power control
    //msWriteWordMask(REG_29C1, 0, 0xFFFF);     // enable DVI0 PLL power
    // DVI slowly update
    msWriteByteMask(REG_2853, 0xE3, 0xFF);      // [15]:update Bch slowly; [14:8]: 0x63(100 lines)
    msWrite2ByteMask(REG_2854, 0xE3E3, 0xFFFF); // [15:8]: update Rch slowly, [7:0]:update Gch slowly
    // Enable DVI phase accumulator extension and clock stable gate
    msWriteByteMask(REG_2801, BIT6|BIT5, BIT6|BIT5); // [6]:Enable of acc8lsb extension ; [5]:Enable of acc8lsb extension select
    msWriteByteMask(REG_2841, BIT2, BIT2);      // [2]:Enable the function that clock gen gate signal controlled by TMDS clock stable
    // enable Vsync glitch filter
    msWriteByteMask(REG_283C, BIT4, BIT4);      // enable DVI port0 Vsync glitch filter
    // Enable DVI clock tolerance: 4*0.7 = 2.8 MHz
    msWriteByteMask(REG_2827, 0x20, 0x70);      //the comp value is reg_comp_tol * 4
    msWriteByteMask(REG_2825, 0xF0, 0xF0);      //0x110A25=0xB0, DVI clock detection counter tolerance set to 7
    msWriteByte(REG_2852,0x10);                 // [4]: enable auto DVIPLL adjust
}

void drvmStar_Init( void )
{
    BYTE i;

#if ENABLE_SW_DOUBLE_BUFFER
    msSWDBInit();
#endif

#if CHIP_ID == CHIP_TSUM2 //U02 need to Set this bit to let MCU could restart after reset.
    msWriteByteMask(REG_0180,BIT0,BIT0); //when external 1.2V is lower than 0.8V, Scalar will enter into reset status. let 1.2V back to normal level , MCU should restart again.
#endif

    msInitClockGating();

    msTMDSInit();

    msWriteByteMask(SC0_02, BIT7, BIT7);

    for( i = 0; i < sizeof( tblInit ) / sizeof( RegUnitType ); i++ )
        msWriteByte( tblInit[i].u16Reg, tblInit[i].u8Value );

#if (FRAME_BFF_SEL == FRAME_BUFFER) || ENABLE_RTE
    msInitMemory();
#endif

    mcuInitXdataMapToDRAM();
    mcuXdataMapToDRAM(HK_XDATA_ADDR);

  #if (ENABLE_HK_CODE_ON_DRAM)
    mcuDMADownloadCode(DMA_TO_DRAM_SOURCE_ADDR, DMA_TO_DRAM_DESTIN_ADDR, DMA_TO_DRAM_BYTE_COUNT);
    mcuArrangeCodeAddr(_DRAM, MCU_ON_DRAM_START_ADDR , MCU_ON_DRAM_END_ADDR);
    mcuArrangeCodeAddr(_SPI, MCU_ON_SPI_START_ADDR, MCU_ON_SPI_END_ADDR);
  #endif

#if DISABLE_SHORT_LINE_TUNE
    msWriteByteMask(SC0_63, BIT0,  BIT1|BIT0);
    msWriteByteMask(SC0_28,    0,  BIT6);
#endif

#if DISABLE_SHORT_FRAME
    msWriteByteMask(SC0_63, BIT2, BIT2);
    msWriteByteMask(SC0_63, 0, BIT3);
#endif

    //msWriteByteMask(SC0_0E, BIT7, BIT7); // H coring disable, for dot pattern certical block issue, Jison 100610
    mStar_SetupPathInit(); //Jison need put here because mStar_SetupFreeRunMode() will use g_SetupPathInfo variable
#if (CHIP_ID==CHIP_TSUM2)
#ifndef FPGA
    if (PowerOnFlag)
        msDVIEQCalibration();
#endif
#endif // #if ChipID==CHIP_TSUMA

#if ENABLE_R2_2DTO3D
    if(UserPref2Dto3D)
        Sc2DTo3DInit();
#elif ENABLE_USB_INPUT
    mcuDMADownloadCode(DMA_TO_DRAM_SOURCE_ADDR, DMA_TO_DRAM_DESTIN_ADDR, DMA_TO_DRAM_BYTE_COUNT);
    mcuArrangeCodeAddr(_DRAM, MCU_ON_DRAM_START_ADDR , MCU_ON_DRAM_END_ADDR);
    mcuArrangeCodeAddr(_SPI, MCU_ON_SPI_START_ADDR, MCU_ON_SPI_END_ADDR);

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
        msWrite2ByteMask(REG_382E, 0, 0x3FF);
        msWrite2ByteMask(REG_3830, 0, 0x3FFF);
        return;
    }

    // freqMod: 0~40 means 0~40K
    if(freqMod > PANEL_SSC_MODULATION_MAX)
        freqMod = PANEL_SSC_MODULATION_MAX;

    // range: 0~30 means 0~3%
    if(range > PANEL_SSC_PERCENTAGE_MAX)
        range = PANEL_SSC_PERCENTAGE_MAX;

    dwFactor = msReadByte(REG_3820);
    dwFactor <<= 16;
    dwFactor |= msRead2Byte(REG_381E);

    wSpan = ((u32MPLL_MHZ*1024ul/freqMod) * 131072ul) / dwFactor;
    wStep = (float)(dwFactor*range/10)/wSpan/100; // 120320 coding modified

    //drvmStar_printData("PanelSSC SPAN[%x]",wSpan);
    //drvmStar_printData("PanelSSC STEP[%x]",wStep);

    if (wStep > 0x3FF)
        wStep = 0x3FF;

    if(wSpan > 0x3FFF)
        wSpan = 0x3FFF;

    msWrite2ByteMask(REG_382E, wStep, 0x3FF);
    msWrite2ByteMask(REG_3830, wSpan, 0x3FFF);
    msWriteBit(REG_381B, _ENABLE, BIT3); // ssc enable
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
#if ENABLE_MHL // 120914 coding addition
    if(CURRENT_INPUT_IS_HYBRID_MHL_HDMI())
    {
        msWriteByte(REG_2842, 0x00); // EQ strength
        msWriteByte(REG_2844, 0x00); // EQ strength
        msWriteByte(REG_2846, 0x00); // EQ strength
    }
    else
#endif
    {
        msWriteByte(REG_2842, 0x01); // EQ strength
        msWriteByte(REG_2844, 0x01); // EQ strength
        msWriteByte(REG_2846, 0x01); // EQ strength
    }

    if(INPUT_IS_DVI(inport))//( inport == Input_Digital )
    {
        //PORT A/B
        msWriteByteMask(REG_PM_B2, 0, BIT2|BIT3); // power down DVI CKIN/PLL band-gap.
        // Power up Xtal clocks and DVI detection clock for DVI clock detection
        msWrite2ByteMask( REG_290C, 0, 0xFFFF );

        // DVI PLL power control
#if 1
        msWrite2ByteMask( REG_29C0, 0x0000, 0xFFFF ); // enable DVI0 PLL power, [12:11]: portB/A Data R-term
    #if DVI_PORT==TMDS_PORT_B
        msWriteByteMask( REG_PM_B3, BIT5, BIT5 | BIT4 ); //DVI clock mux: [5]: port 1(portB), [4]: port 0(portA)
        msWriteByteMask( REG_29C2, BIT1, BIT1|BIT0); //  [1:0] port A/B data mux: 10: port B; 01: port A
    #else //TMDS_PORT_A
        msWriteByteMask( REG_PM_B3, BIT4, BIT5 | BIT4 ); //DVI clock mux: [5]: port 1(portB), [4]: port 0(portA)
        msWriteByteMask( REG_29C2, BIT0, BIT1|BIT0); //  [1:0] port A/B data mux: 10: port B; 01: port A
    #endif
#else
      #if CHIP_ID == CHIP_TSUM2
        msWrite2ByteMask( REG_29C0, 0x0000, 0xFFFF ); // enable DVI0 PLL power, [12:11]: portB/A Data R-term
        msWriteByteMask( REG_PM_B3, BIT4, BIT5 | BIT4 ); //DVI clock mux: [5]: port 1(portB), [4]: port 0(portA)
        msWriteByteMask( REG_29C2, BIT0, BIT1|BIT0); //  [1:0] port A/B data mux: 10: port B; 01: port A
      #else
      #if DVI_PORT==TMDS_PORT_B
        msWrite2ByteMask( REG_29C0, 0x0800, 0xFFFF ); // enable DVI0 PLL power, [12:11]: portB/A Data R-term
        msWriteByteMask( REG_PM_B3, BIT5, BIT5 | BIT4 ); //DVI clock mux: [5]: port 1(portB), [4]: port 0(portA)
        msWriteByteMask( REG_29C2, BIT1, BIT1|BIT0); // [1:0] port A/B data mux: 10: port B; 01: port A
      #else //TMDS_PORT_A
        msWrite2ByteMask( REG_29C0, 0x1000, 0xFFFF ); // enable DVI0 PLL power, [12:11]: portB/A Data R-term
        msWriteByteMask( REG_PM_B3, BIT4, BIT5 | BIT4 ); //DVI clock mux: [5]: port 1(portB), [4]: port 0(portA)
        msWriteByteMask( REG_29C2, BIT0, BIT1|BIT0); //  [1:0] port A/B data mux: 10: port B; 01: port A
      #endif
      #endif
#endif

    }
  #if ENABLE_HDMI
    else if( INPUT_IS_HDMI(inport) ) // HDMI - Port B
    {
        //PORT A/B
        msWriteByteMask(REG_PM_B2, 0x00, 0x0F); // power down DVI CKIN. (port 0 and port 1) (reg_b2[2])

        // Power up Xtal clocks and DVI detection clock for DVI clock detection
        msWrite2ByteMask( REG_290C, 0, 0xFFFF );
        // DVI PLL power control
#if 1
        msWrite2ByteMask( REG_29C0, 0x0000, 0xFFFF ); // enable DVI0 PLL power, [12:11]: portB/A Data R-term
    #if HDMI_PORT==TMDS_PORT_B
        msWriteByteMask( REG_PM_B3, BIT5, BIT5 | BIT4 ); //DVI clock mux: [5]: port 1(portB), [4]: port 0(portA)
        msWriteByteMask( REG_29C2, BIT1, BIT1|BIT0); //  [1:0] port A/B data mux: 10: port B; 01: port A
    #else //TMDS_PORT_A
        msWriteByteMask( REG_PM_B3, BIT4, BIT5 | BIT4 ); //DVI clock mux: [5]: port 1(portB), [4]: port 0(portA)
        msWriteByteMask( REG_29C2, BIT0, BIT1|BIT0); //  [1:0] port A/B data mux: 10: port B; 01: port A
    #endif
#else
        #if CHIP_ID == CHIP_TSUM2
        msWrite2ByteMask( REG_29C0, 0x0000, 0xFFFF ); // enable DVI0 PLL power, [12:11]: portB/A Data R-term
        msWriteByteMask( REG_PM_B3, BIT5, BIT5 | BIT4 ); //DVI clock mux: [5]: port 1(portB), [4]: port 0(portA)
        msWriteByteMask( REG_29C2, BIT1, BIT1|BIT0); //  [1:0] port A/B data mux: 10: port B; 01: port A
        #else
        #if HDMI_PORT==TMDS_PORT_B //Checked
        msWrite2ByteMask( REG_29C0, 0x0800, 0xFFFF ); // enable DVI0 PLL power, [12:11]: portB/A Data R-term
        msWriteByteMask( REG_PM_B3, BIT5, BIT5 | BIT4 ); //DVI clock mux: [5]: port 1(portB), [4]: port 0(portA)
        msWriteByteMask( REG_29C2, BIT1, BIT1|BIT0); //  [1:0] port A/B data mux: 10: port B; 01: port A
        #else //TMDS_PORT_A
        msWrite2ByteMask( REG_29C0, 0x1000, 0xFFFF ); // enable DVI0 PLL power, [12:11]: portB/A Data R-term
        msWriteByteMask( REG_PM_B3, BIT4, BIT5 | BIT4 ); //DVI clock mux: [5]: port 1(portB), [4]: port 0(portA)
        msWriteByteMask( REG_29C2, BIT0, BIT1|BIT0); //  [1:0] port A/B data mux: 10: port B; 01: port A
        #endif
        #endif
#endif
    }
  #endif // #if ENABLE_HDMI_INPUT
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
    u16SCLK = u16SCLK;
    u16DCLK = u16DCLK;
    msWriteByteMask(REG_1E35, _BIT3, _BIT3|_BIT2); // fix maximum fclk
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

    // 0.backup REG_3008/3009/30AA/30AB
    BYTE reg_36E2   = msReadByte(REG_36E2);
    BYTE reg_3646   = msReadByte(REG_3646);
    BYTE reg_3680   = msReadByte(REG_3680);
    BYTE reg_368B   = msReadByte(REG_368B);
    msWriteByteMask(REG_36FA,    BIT6, BIT7|BIT6);        /* Enable MOD self current bias.*/
        //MST_printData("--@@@@@@ REG_36FA=0x%x", msReadByte( REG_36FA ) );
    // 1.Disable double differential output swing
    msWriteByteMask(REG_36F0,   0x00,  BIT6);
    // 2.Select calibration source.
    //Note:
    //if BK36_FBh[1]=0, set BK36_46[1:0] = 2'b00, and BK_36_E2[2] = 1'b0 to make PADA_OUT[PN]_CH[2] as LVDS output.
    //if BK36_FBh[1]=1, set BK36_46[1:0] = 2'b00, and BK_36_E2[7] = 1'b0 to make PADA_OUT[PN]_CH[7] as LVDS output.
    #if 1
    msWriteByteMask(REG_36FB,   0x00,  BIT1);
    msWriteByteMask(REG_3646,   0x00,  BIT1|BIT0);
    msWriteByteMask(REG_36E2,   0x00,  BIT2);
    #else
    msWriteByteMask(REG_36FB,   BIT1,  BIT1);
    msWriteByteMask(REG_3646,   0x00,  BIT1|BIT0);
    msWriteByteMask(REG_36E2,   0x00,  BIT7);
    #endif
    // 3.Enable LVDS Test Mode, and make output data=High for LVDS output.
    //Note:
    //Please disable MOD Differential output polarity swap. (BK36_E6[7:0]=8'h00, BK36_E7[1:0]=2'h0)
    msWriteByteMask(REG_3680,   BIT1,  BIT1);
    msWriteByteMask(REG_368B,   0x00,  BIT1|BIT0);
    // 4.Set LVDS voltage target. Default value is 247mV if we didn't set those two bits.
    //Note:
    //BK36_FBh[4:3]=0x00, 247mV    BK36_FBh[4:3]=0x01, 296mV
    //BK36_FBh[4:3]=0x10, 197mV    BK36_FBh[4:3]=0x11, 345mV
    msWriteByteMask(REG_36FB,   ((LVDS_VOLTAGE_TARGET)<<3),  BIT4|BIT3);
    // 5.Enable Hardware Calibration
    msWriteByteMask(REG_36FD,   BIT0,  BIT0);
    // 6.Wait until BK36_FCh[7] = 1. Hardware Calibration ready.
    SetTimOutConter(50);
    //u16TimeOutCounter = 50;
    //bTimeOutCounterFlag=1;
    while (! ( msReadByte( REG_36FC ) & BIT7 ) && u16TimeOutCounter );

    // 7.If BK36_FCh[6] = 0, Set BK36_FAh[5:0] = BK36_FCh[5:0]. If BK36_FCh[6] = 1, calibration failed.
    if (msReadByte( REG_36FC )&BIT6)
    {
        i = LVDS_FAIL_TARGET;//SWING_MASK/2;
        msWriteByteMask(REG_36FD,	0x00,  BIT0);
        msWriteByteMask(REG_36FA, i,	SWING_MASK);
        drvmStar_printMsg(" @@@@@@@@@                       HW MOD Cali Fail !!");
    }
    else
    {
            i = msReadByte(REG_36FA)&SWING_MASK;
#ifdef ENABLE_UP_LOW_BOUNDARY_PROTECTION
		if (i >ICON_Upper_Boundary || i<ICON_Lower_Boundary)
		{
	        	i = LVDS_FAIL_TARGET;
	        	msWriteByteMask(REG_36FD,   0x00,  BIT0);
	        	msWriteByteMask(REG_36FA, i,	SWING_MASK);
	        	drvmStar_printMsg("HW MOD Cali Fail due to not located in normal range !!");
		}
		else
#endif
		{
	        	drvmStar_printMsg("HW MOD Cali Pass !!");
	        	drvmStar_printData(" @@@@@@@@@                        Cali result=0x%x", msReadByte(REG_36FA)&SWING_MASK); // REG_36FA[5:0] is calibration result
		}
    }

    // 8.Disable Hardware Calibration.
    //msWriteByteMask(REG_36FD,   0x00,  BIT0);
    //9.Set register "BK36_E2[2], BK36_E2[7], BK36_46[1:0], BK36_80[1], BK36_8B[1:0]" back to the original normal function.
    msWriteByte(REG_36E2, reg_36E2);
    msWriteByte(REG_3646, reg_3646);
    msWriteByte(REG_3680, reg_3680);
    msWriteByte(REG_368B, reg_368B);
    //MST_printMsg("HW I_Gen_Tuning_End");
    iGenTuningFinished=1;
}

#else //SW
// @brief MOD Current Calibration
// @pre   Current bank is bank-0.
// @pre   LPLL is on, i.e., ASSERT( BK0_D1[4]==0 )
void I_Gen_Tuning( void )
{
    BYTE i=0;
    #define SWING_MASK	0x3F

    // 0.backup REG_3008/3009/30AA/30AB
    volatile BYTE reg_36DA   = msReadByte(REG_36DA);
    volatile BYTE reg_36DB   = msReadByte(REG_36DB);
    volatile BYTE reg_36E2   = msReadByte(REG_36E2);
    volatile BYTE reg_3646   = msReadByte(REG_3646);
    volatile BYTE reg_3647   = msReadByte(REG_3647);
    volatile BYTE reg_3680   = msReadByte(REG_3680);
    volatile BYTE reg_368A   = msReadByte(REG_368A);
    volatile BYTE reg_368B   = msReadByte(REG_368B);
    #define SWING_MASK  0x3F
    drvmStar_printMsg("FW I_Gen_Tuning_Start");
    // 1.Disable power down mod
    msWriteByteMask(REG_36F0,   0x00, BIT0);
    // 2.Enable MOD self current bias.
    msWriteByteMask(REG_36FA,   BIT6, BIT6);
    // 3.Set output bias current.
    msWriteByteMask(REG_36FA,   0x00, 0x3F);
    // 4.Disable double differential output swing
    msWriteByteMask(REG_36F0,   0x00, BIT6);
    // 5.select calibration source.
    //Note:
    //if BK36_FBh[1]=0, set BK36_DAh[5:4]=2'b01, BK36_46[1:0] = 2'b00, BK_36_E2[2] = 1'b0, and BK36_EEh[3:0]=4'hF to make PADA_OUT[PN]_CH[2] as LVDS output.
    //if BK36_FBh[1]=1, set BK36_DBh[7:6]=2'b01, BK36_46[1:0] = 2'b00, BK_36_E2[7] = 1'b0, and BK36_EEh[3:0]=4'hF to make PADA_OUT[PN]_CH[7] as LVDS output.
    #if 1
    msWriteByteMask(REG_36FB,   0x00, BIT1);
    msWriteByteMask(REG_36DA,   BIT4, BIT5|BIT4); //PADA_OUT[PN]_CH[2] as LVDS output
    msWriteByteMask(REG_3646,   0x00, BIT1|BIT0);
    msWriteByteMask(REG_36E2,   0x00, BIT2);
    msWriteByteMask(REG_36EE,   0x0F, 0x0F);
    #else
    msWriteByteMask(REG_36FB,   BIT1, BIT1);
    msWriteByteMask(REG_36DB,   BIT6, BIT7|BIT6); //PADA_OUT[PN]_CH[7] as LVDS output
    msWriteByteMask(REG_3646,   0, BIT1|BIT0);
    msWriteByteMask(REG_36E2,   0, BIT7);
    msWriteByteMask(REG_36EE,   0x0F, 0x0F);
    #endif
    // 6.Enable LVDS Test Mode, and make output data=High for LVDS output.
    msWriteByteMask(REG_3680,   BIT1, BIT1);
    msWriteByteMask(REG_368A,   BIT6, BIT6);
    msWriteByteMask(REG_368B,   0x00, BIT1|BIT0);
    msWriteByteMask(REG_3682,   0xFF, 0xFF);
    // 7.Enable MOD Calibration.
    msWriteByteMask(REG_36FA,   BIT7, BIT7);
    // 8.Used to set LVDS voltage target. Default value is 247mV if we didn't set those two bits.
    //Note:
    //BK36_FBh[4:3]=0x00, 247mV    BK36_FBh[4:3]=0x01, 296mV
    //BK36_FBh[4:3]=0x10, 197mV    BK36_FBh[4:3]=0x11, 345mV
    msWriteByteMask(REG_36FB,   ((LVDS_VOLTAGE_TARGET)<<3), BIT4|BIT3);
    //9.Adjust the value of BK36_FAh[5:0] step by step to control output swing and check whether BK36_FBh[0] is equal to "1" or "0".
    for (i=0; i<=SWING_MASK; i++)
    {
        msWriteByteMask(REG_36FA, i,SWING_MASK);
        ForceDelay1ms(10);
        //10.If BK36_FBh[0] is toggling between "1" and "0", terminal the fine-tune procedure.
        if (msReadByte(REG_36FB)&BIT0)
        {
		if ( i==0 ||  i==SWING_MASK)
		{
			i = LVDS_FAIL_TARGET;
			msWriteByteMask(REG_36FA, i,SWING_MASK);
			drvmStar_printMsg("1111 FW MOD Cali Fail !!");
			break;
		}
		else
		{

	            i = msReadByte(REG_36FA)&SWING_MASK;
		#ifdef ENABLE_UP_LOW_BOUNDARY_PROTECTION
			if (i >ICON_Upper_Boundary || i<ICON_Lower_Boundary)
			{
		        	i = LVDS_FAIL_TARGET;
		        	msWriteByteMask(REG_36FA, i,	SWING_MASK);
		        	drvmStar_printMsg("HW MOD Cali Fail due to not located in normal range !!");
			}
			else
		#endif
			{
		        	drvmStar_printMsg("FW MOD Cali Pass !!");
		        	drvmStar_printData(" @@@@@@@@@                        Cali result=0x%x", msReadByte(REG_36FA)&SWING_MASK); // REG_36FA[5:0] is calibration result
			}
		}
            break;
        }
    }

    // 11.Disable MOD Calibration.
    msWriteByteMask(REG_36FA,   0x00, BIT7);
    // 12.Set register "BK36_DAh[5:4], BK36_DBh[15:14], BK36_E2[2], BK36_E2[7], BK36_46[5:4], BK36_47[7:6], BK36_80[1], BK36_8A[6], BK36_8B[1:0]" back to the original normal function.
    msWriteByte(REG_36DA, reg_36DA);   /* restore original value of BK0_08 */
    msWriteByte(REG_36DB, reg_36DB);   /* restore original value of BK0_09 */
    msWriteByte(REG_36E2, reg_36E2);   /* restore original value of BK0_AA */
    msWriteByte(REG_3646, reg_3646);   /* restore original value of BK0_AB */
    msWriteByte(REG_3647, reg_3647);   /* restore original value of BK0_08 */
    msWriteByte(REG_3680, reg_3680);   /* restore original value of BK0_09 */
    msWriteByte(REG_368A, reg_368A);   /* restore original value of BK0_AA */
    msWriteByte(REG_368B, reg_368B);   /* restore original value of BK0_AB */
    drvmStar_printMsg("FW I_Gen_Tuning_End");

    iGenTuningFinished=1;
}

#endif  // end of #if( USE_MOD_HW_CAL )

void Power_ModCtrl(BYTE ucSwitch)
{
    if (ucSwitch)
    {
        #if PanelLVDS
            msWriteByte(REG_36DA, 0x55);
            msWriteByte(REG_36DB, 0x55);
            msWriteByte(REG_36DC, 0x05);
            msWriteByteMask(REG_36F0,0,BIT0);
        #elif PanelMiniLVDS3
            msWriteByte(REG_36DA, 0x40);
            msWriteByte(REG_36DB, 0x65);
            msWriteByte(REG_36DC, 0x05);
            msWriteByteMask(REG_36F0,0,BIT0);
        #endif
    }
    else
    {
        msWriteByteMask(REG_368A, 0x3F, 0x3F);
        msWriteByteMask(REG_36F0,BIT0,BIT0);
        msWriteByte(REG_36DA, 0x00);
        msWriteByte(REG_36DB, 0x00);
        msWriteByte(REG_36DC, 0x00);
        msWriteByte(REG_36DD, 0x00);
        msWriteByte(REG_36DD, 0x00);
    }
}

BYTE mStar_ScalerDoubleBuffer(Bool u8Enable)
{
    u8Enable=0;
    return u8Enable;
}

void mStar_SetDClkPLL(DWORD u32ODCLK_KHZ)
{
    enum
    {
        LPLL_DIV_DIV1 = 0,
        LPLL_DIV_DIV2 = 1,
        LPLL_DIV_DIV4 = 2,
        LPLL_DIV_DIV8 = 3,
        LPLL_DIV_HIGH_DIV3 = 0,
        LPLL_DIV_HIGH_DIV4 = 1,
        LPLL_DIV_HIGH_DIV6 = 2,
        LPLL_DIV_HIGH_DIV8 = 3,
    };
    code BYTE LPLL_DIV_HIGH_tbl[4]={3,4,6,8};
    XDATA DWORD u32MPLL_KHZ;
    XDATA BYTE  u8LPLL_INPUT_DIV,u8LPLL_SKEW_DIV;
    XDATA BYTE  u8LPLL_LOOP_DIV_1ST,u8LPLL_LOOP_DIV_HIGH,u8LPLL_LOOP_DIV_LOW;
    XDATA BYTE  u8LPLL_ICTL,u8LPLL_MODE;
    XDATA DWORD u32Factor;

#if PanelLVDS
    u8LPLL_INPUT_DIV = LPLL_DIV_DIV1;
    u8LPLL_LOOP_DIV_1ST = LPLL_DIV_DIV1;
    u8LPLL_LOOP_DIV_LOW = LPLL_DIV_DIV1;
    u8LPLL_LOOP_DIV_HIGH = LPLL_DIV_HIGH_DIV4;

    u8LPLL_ICTL = 0x00;
    #if( PanelDualPort)
        u8LPLL_MODE = 1;
        if( u32ODCLK_KHZ <= 25*1000UL )    // 13MHz~25MHz
            u8LPLL_SKEW_DIV = LPLL_DIV_DIV8;
        else if( u32ODCLK_KHZ <= 50*1000UL )    // 25MHz~50MHz
            u8LPLL_SKEW_DIV = LPLL_DIV_DIV4;
        else if( u32ODCLK_KHZ <= 100*1000UL )    // 50MHz~100MHz
            u8LPLL_SKEW_DIV = LPLL_DIV_DIV2;
        else    // 100MHz~200MHz
            u8LPLL_SKEW_DIV = LPLL_DIV_DIV1;
    #else
    u8LPLL_MODE = 0;
        if( u32ODCLK_KHZ <= 25*1000UL )    // 13MHz~25MHz
            u8LPLL_SKEW_DIV = LPLL_DIV_DIV4;
        else if( u32ODCLK_KHZ <= 50*1000UL )    // 25MHz~50MHz
            u8LPLL_SKEW_DIV = LPLL_DIV_DIV2;
        else // 50MHZ~100MHZ
            u8LPLL_SKEW_DIV = LPLL_DIV_DIV1;
    #endif

#elif PanelMiniLVDS3
    #if (PanelMiniLVDS_CHANNEL_NUM==0x01 && PanelMiniLVDS_PAIR_NUM==0x03 && PanelDataBits==0x08)
        u8LPLL_ICTL = 0x00;
        u8LPLL_LOOP_DIV_1ST = 0x00;
        u8LPLL_LOOP_DIV_HIGH = 0x11;
        u8LPLL_LOOP_DIV_LOW = 0x00;
        u8LPLL_INPUT_DIV = 0x00;
        if( u32ODCLK_KHZ > 50*1000UL && u32ODCLK_KHZ <= 80*1000UL )    // 50MHz~80MHz
            u8LPLL_SKEW_DIV = LPLL_DIV_DIV1;
    #elif (PanelMiniLVDS_CHANNEL_NUM==0x02 && PanelMiniLVDS_PAIR_NUM==0x03 && PanelDataBits==0x08)
        u8LPLL_ICTL = 0x00;
        u8LPLL_LOOP_DIV_1ST = 0x00;
        u8LPLL_LOOP_DIV_LOW = 0x00;
        u8LPLL_INPUT_DIV = 0x00;
        if( u32ODCLK_KHZ > 50*1000UL && u32ODCLK_KHZ <= 850*1000UL )    // 13MHz~25MHz
        {
            u8LPLL_LOOP_DIV_HIGH = 0x01;
            u8LPLL_SKEW_DIV = LPLL_DIV_DIV1;
        }
        else // 85MHz~150MHz
        {
            u8LPLL_LOOP_DIV_HIGH = 0x11;
            u8LPLL_SKEW_DIV = LPLL_DIV_DIV2;
        }
    #elif (PanelMiniLVDS_CHANNEL_NUM==0x01 && PanelMiniLVDS_PAIR_NUM==0x03 && PanelDataBits==0x06)


    #elif (PanelMiniLVDS_CHANNEL_NUM==0x01 && PanelMiniLVDS_PAIR_NUM==0x06 && PanelDataBits==0x06)
        printMsg("ch=1,pair=6,data=6");
        u8LPLL_MODE = 1;
        u8LPLL_LOOP_DIV_LOW = LPLL_DIV_DIV1;
        u8LPLL_INPUT_DIV = LPLL_DIV_DIV1;
        if( u32ODCLK_KHZ > 50*1000UL && u32ODCLK_KHZ <= 90*1000UL )    // 50MHz~90MHz
        {
            u8LPLL_ICTL = 0x00;
            u8LPLL_LOOP_DIV_1ST = 0x00;
            u8LPLL_LOOP_DIV_HIGH = 0x02;
            u8LPLL_SKEW_DIV = 0x01;
        }
        else // 90MHz~200MHz
        {
            u8LPLL_ICTL = LPLL_DIV_DIV2;
            u8LPLL_LOOP_DIV_1ST = LPLL_DIV_DIV2;
            u8LPLL_LOOP_DIV_HIGH = LPLL_DIV_DIV1;
            u8LPLL_SKEW_DIV = LPLL_DIV_DIV1;
        }
    #endif


#endif
#if PanelLVDS
    msWriteByteMask( REG_38C8, (u8LPLL_ICTL<<2)|u8LPLL_LOOP_DIV_1ST, BIT3|BIT2|BIT1|BIT0);
    msWriteByteMask( REG_38C9, (u8LPLL_INPUT_DIV<<6)|u8LPLL_SKEW_DIV, BIT7|BIT6|BIT1|BIT0);
    msWriteByteMask( REG_38CA, (u8LPLL_LOOP_DIV_HIGH<<2)|u8LPLL_LOOP_DIV_LOW, BIT3|BIT2|BIT1|BIT0);
    msWriteByteMask( REG_38CA, u8LPLL_MODE<<5 ,BIT5|BIT4);
#elif PanelMiniLVDS3
    msWriteByteMask( REG_38C8, (u8LPLL_ICTL<<2)|u8LPLL_LOOP_DIV_1ST, BIT3|BIT2|BIT1|BIT0);
    msWriteByteMask( REG_38C9, (u8LPLL_INPUT_DIV<<6)|u8LPLL_SKEW_DIV, BIT7|BIT6|BIT1|BIT0);
    msWriteByteMask( REG_38CA, (u8LPLL_LOOP_DIV_HIGH<<2)|u8LPLL_LOOP_DIV_LOW, BIT3|BIT2|BIT1|BIT0);
    msWriteByteMask( REG_38CA, u8LPLL_MODE<<4 ,BIT5|BIT4);
#endif
    u8LPLL_INPUT_DIV =(BIT0<<u8LPLL_INPUT_DIV);
    u8LPLL_LOOP_DIV_1ST =(BIT0<<u8LPLL_LOOP_DIV_1ST);
    u8LPLL_LOOP_DIV_LOW =(BIT0<<u8LPLL_LOOP_DIV_LOW);
    u8LPLL_LOOP_DIV_HIGH = LPLL_DIV_HIGH_tbl[u8LPLL_LOOP_DIV_HIGH];
    u8LPLL_SKEW_DIV = (BIT0<<u8LPLL_SKEW_DIV);
    u32MPLL_KHZ = 429540UL / u8LPLL_INPUT_DIV;  // 429.54MHz

#if ENABLE_DEBUG && 0
    printData("SynthH_KHZh       = 0x%x", u32MPLL_KHZ>>16    );
    printData("SynthL_KHZh       = 0x%x", u32MPLL_KHZ&0xFFFF );
    printData("u8LPLL_INPUT_DIV  = %d",   u8LPLL_INPUT_DIV );
    printData("u8LPLL_LOOP_DIV_1ST = %d",   u8LPLL_LOOP_DIV_1ST);
    printData("u8LPLL_LOOP_DIV_HIGH   = %d",   u8LPLL_LOOP_DIV_HIGH  );
    printData("u8LPLL_LOOP_DIV_LOW   = %d",   u8LPLL_LOOP_DIV_LOW  );
    printData("u8LPLL_SKEW_DIV   = %d",   u8LPLL_SKEW_DIV  );
    printData("ODCLK_KHZh      = 0x%x", u32ODCLK_KHZ>>16);
    printData("ODCLK_KHZl      = 0x%x", u32ODCLK_KHZ&0xFFFF);
#endif
    u32Factor = u8LPLL_LOOP_DIV_HIGH * u8LPLL_LOOP_DIV_LOW * u8LPLL_LOOP_DIV_1ST;

#ifdef FPGA
    u32Factor = (float)(16.0/7)*(214.0)*( (float)524288UL)/u32ODCLK_KHZ*1000;
#else
    #if PanelMiniLVDS3
        u32Factor = (float)(429540UL)*(float)(524288UL*u8LPLL_LOOP_DIV_1ST)/(float)(u32ODCLK_KHZ);
    #else
        u32Factor = u8LPLL_LOOP_DIV_HIGH * u8LPLL_LOOP_DIV_LOW * u8LPLL_LOOP_DIV_1ST;
        u32Factor = (float)(u32MPLL_KHZ * 2 / 7) * ((float)524288UL * (float)(u32Factor/u8LPLL_SKEW_DIV)/ u32ODCLK_KHZ);
    #endif
    #if (!PanelRSDS && !PanelDualPort && !PanelMiniLVDS3)
    u32Factor/=2;
    #endif
#endif

#if FRAME_BFF_SEL==FRAME_BUFFER
    mStar_SetFrameDivider(V_FREQ_IN);
    mStar_SetIVS();
#endif

    //for faster FPLL lock
    if((V_FREQ_IN<48))
    {
        if(V_FREQ_IN<28) // 24Hz
            msWrite2Byte(REG_3816, 0x6565);
        else // 30Hz
            msWrite2Byte(REG_3816, 0x7676);
    }
    else if(_bit3_(msReadByte(SC0_E1))) // Interlace
    {
        msWrite2Byte(REG_3816, 0x7676);
    }
    else // Default
        msWrite2Byte(REG_3816, 0x8787);


#if 1 // U02 set 1
    //if(g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_0)
    {
        msWriteBit(REG_3818, _DISABLE, _BIT3);
        msWrite2Byte(SC0_D5, u32Factor);
        //msWriteByte(SC0_D7, u32Factor>>16);
        //msWriteByte(SC0_D7, 0x15);
        msWrite2Byte(REG_381E, u32Factor);
        msWriteByte(REG_3820, u32Factor>>16);
    }
    //else
#else // U01
    {
        msWriteBit(REG_3818, _DISABLE, _BIT3);
        msWrite2Byte(REG_381E, u32Factor);
        msWriteByte(REG_3820, u32Factor>>16);
    }
#endif

}

void drvmStar_SetupFreeRunMode( void )
{
    mStar_ScalerDoubleBuffer(FALSE);
    msWriteByteMask( SC0_02, BIT7, BIT7 ); // disable output Lock mode to enable free run.

    if(g_SetupPathInfo.bFBMode)
    {
        mStar_SetDClkPLL((DWORD)PanelHTotal*PanelVTotal*GetVfreq()/1000);
        msSetFrameSyncMode(FLM_FB_FPLL_NOLOCK);
    }
    else
    {
        msWriteByteMask(SC0_27,BIT6,BIT6|BIT1);
        msWrite2ByteMask(SC0_1E, PanelVTotal-1,SC_MASK_V); // set output panel vtotal
        msWrite2Byte(SC0_24, (PanelMinHTotal+8)-1);
        msWriteByteMask(SC0_63,BIT0,BIT1|BIT0);
        msWriteByteMask(SC0_28,BIT6,BIT6);
        msWriteByteMask(SC0_28,BIT3,BIT3);
        msWriteByteMask(SC0_9F,0,BIT0);
        mStar_SetDClkPLL(PanelDCLK*1000UL); //White101208 must use 1000UL to provent overflow truncate

    }
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

void msSetFrameSyncMode(FSyncMode ucFrameLockMode)
{
    switch(ucFrameLockMode)
    {
        case FLM_FBL:
            msWrite2ByteMask(SC0_1E, PanelMaxVTotal-1,SC_MASK_V);
            msWriteByteMask(SC0_63,BIT0,BIT1|BIT0);
            msWriteByteMask(SC0_28,BIT6,BIT6);
            msWriteByteMask(SC0_28,0,BIT3);
            msWriteByteMask(SC0_27,BIT6|BIT1,BIT6|BIT1);
            msWriteByteMask(SC0_9F,0,BIT0);
            break;

        case FLM_FB_FPLL_NOLOCK:
            msWrite2ByteMask(SC0_1E, PanelVTotal-1,SC_MASK_V);
            msWriteByteMask(SC0_63,BIT0,BIT1|BIT0);
            msWriteByteMask(SC0_28,0,BIT6);
            msWriteByteMask(SC0_28,BIT3,BIT3);
            msWriteByteMask(SC0_27,BIT6,BIT6|BIT1);
            msWrite2Byte(SC0_24,PanelHTotal-1);
            msWriteByteMask(SC0_9F,BIT0,BIT0);
            msWriteByteMask(REG_3818,0,BIT3);
            break;

        case FLM_FB_FPLL:
            msWrite2ByteMask(SC0_1E, PanelVTotal-1,SC_MASK_V);
            msWriteByteMask(SC0_63,BIT0,BIT1|BIT0);
            msWriteByteMask(SC0_28,0,BIT6);
            msWriteByteMask(SC0_28,BIT3,BIT3);
            msWriteByteMask(SC0_27,BIT6,BIT6|BIT1);
            msWrite2Byte(SC0_24,PanelHTotal-1);
            msWriteByteMask(SC0_9F,BIT0,BIT0);
            msWriteByteMask(REG_3818,BIT3,BIT3);
            break;
    }

}
#if FRAME_BFF_SEL==FRAME_BUFFER
void mStar_SetFrameDivider( BYTE vfreq_N )
{
    ////////////////////////////////
    //  FRC ratio N/M ==> (N-1), (M-1)//
    ////////////////////////////////
    XDATA BYTE u8Index_N;
    XDATA BYTE u8Index_M;
    XDATA BYTE u8Div_N; // input divider
    XDATA BYTE u8Div_M; // output divider
    XDATA BYTE u8VFreq_M; // output clock
    XDATA DWORD u32MinDiff; // minimum differencec
    XDATA BYTE u8Index_N_step;

    //printData( "input VFreq_N = %d", vfreq_N);

    u8VFreq_M = GetVfreq();

    //printData( "output VFreq_M = %d", u8VFreq_M);
    u8Index_N_step = (SrcFlags&bInterlaceMode)?2:1;
    for(u32MinDiff=0xFFFFFFFF,u8Index_N=u8Index_N_step;u8Index_N<=32;u8Index_N+=u8Index_N_step)
    {
        for(u8Index_M=1;u8Index_M<=32;u8Index_M++)
        {
            if((u8Index_N*u8VFreq_M) == (u8Index_M*vfreq_N))
            {
                u8Div_N   = u8Index_N;
                u8Div_M   = u8Index_M;
                u8Index_N = 33; //break 1st loop
                break;
            }else if( labs(((((DWORD)u8Index_N)<<16)/u8Index_M) - ((((DWORD)vfreq_N)<<16)/u8VFreq_M)) < u32MinDiff)
            {
                u8Div_N    = u8Index_N;
                u8Div_M    = u8Index_M;
                u32MinDiff = labs(((((DWORD)u8Index_N)<<16)/u8Index_M) - ((((DWORD)vfreq_N)<<16)/u8VFreq_M));
            }
        }
    }

    u8Div_N = u8Div_N-1;
    u8Div_M = u8Div_M-1;
    //printData( "Set input V_divier_N = %d", u8Div_N);
    //printData( "Set output V_divier_M = %d", u8Div_M);

    msWriteByte(REG_3819, (u8Div_M<<4)|(u8Div_N&0x0F)); //[3:0] input frame div for frame sync, [7:4] output frame div for frame sync
    msWriteByteMask(REG_381C, ((u8Div_M&BIT4)>>3)|(u8Div_N>>4),BIT1|BIT0); //[0] input frame div[4], [1] output frame div [4]
}
#endif
BYTE GetVfreq( void )
{
    if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_PF ) // Vout = Vin/2
        return (V_FREQ_IN + 1)/2;
    else if(g_SetupPathInfo.bFBMode) // FB, Output 60/50Hz // 120426 coding addition
    {
        if(VFreqIn==25)
            return (BYTE)50;
        else
            return (BYTE)60;
    }
    else if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FPI )// Vout = Vin*2
        return V_FREQ_IN*2;
    else // Vout = Vin
        return V_FREQ_IN;
}

#if FRAME_BFF_SEL==FRAME_BUFFER
WORD mStar_SetIVS( void )
{
    XDATA WORD ivs, Sample_vst, vFreqIn;
    WORD Input_Vtt = msRead2Byte(SC0_E2);
    BYTE VfreqRatio = V_FREQ_IN / GetVfreq();

    Sample_vst = ( msRead2Byte( SC0_05 ) & SC_MASK_V )/((SrcFlags&bInterlaceMode)?2:1);
    vFreqIn = V_FREQ_IN;

    if(g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FPI)
        vFreqIn *= 2;

    if(g_SetupPathInfo.bFBMode)
    {
        if((vFreqIn<48)) // FB mode, FRC case
        {
            if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FP) || (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_TB) ) // FP, TB
                ivs = g_SetupPathInfo.wImgSizeInV * 1.3 + Sample_vst;
#if ENABLE_DISPLAY_UNDERSCAN
            else if(UnderScanSetting.Enable)
                 ivs = (g_SetupPathInfo.wImgSizeInV+Sample_vst)-
            (WORD)((((DWORD)Input_Vtt * g_SetupPathInfo.wImgSizeOutV) /PanelVTotal*V_FREQ_IN)/GetVfreq())+10;
#endif
            else // SBS, 2D
                ivs = g_SetupPathInfo.wImgSizeInV * 0.65 + Sample_vst;
        }
        else
        {
            //ivs = IVS_DEFAULT + 13 + Sample_vst;
            ivs = (g_SetupPathInfo.wImgSizeInV+Sample_vst)-
            (WORD)((((DWORD)Input_Vtt * g_SetupPathInfo.wImgSizeOutV) /PanelVTotal*V_FREQ_IN)/GetVfreq())+10;
        }
    }
    else
        ivs = 4 + Sample_vst;

    msWrite2ByteMask(REG_38C0, 0x0000,0x0FFF);
    msWrite2ByteMask(REG_38C2, ivs,0x0FFF);
    msWrite2ByteMask(REG_38C4, 0x0000,0x0FFF);
    msWrite2ByteMask(REG_38C6, 0x0000,0x0FFF);

    return ivs;
}
#endif
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
    msWriteByte(SC0_02, NIS_B|ISEL_HDMI_444|SCALER_STYPE_HV);
}



#if ENABLE_HDMI
void drvmStar_SetupInputPort_HDMI(void)
{

#if (HDMI_RB_SWAP)
    msWriteByteMask(REG_281C,_BIT5,_BIT5);
#endif
#if (HDMI_PN_SWAP)
    msWriteByteMask(REG_2818,_BIT7,_BIT7);
#endif

    msTMDSSetMux( Input_HDMI );

#if (_TMDS_DE_MODE_EN_)
    msWriteByte( SC0_04, BIT3  | BIT7 |BIT6);
    msWriteByteMask( REG_1E22, BIT3, BIT3 ); // enable DE-only mode for SC_TOP
#else
    msWriteByte( SC0_04, BIT3  | BIT7);
    msWriteByteMask( REG_1E22, 0, BIT3 ); // enable DE-only mode for SC_TOP
#endif

    msWriteByte(SC0_F3, 0xF0);// enable DE glitch removal function & more tolerance for DE
    msWriteByteMask( REG_2E04, IP_MUX_DVI_A+1, 0x0F );       // IPmux 3 for Dual DVI and DP
    msWriteByteMask( REG_2E02, IP_MUX_DVI_A << 4, 0xF0 );    // IPmux 2
    msWriteByteMask( REG_1E3E, (CLK_DVI_BUFF_DIV2<<6)|(CLK_DVI_BUFF_DIV2<<4), 0xF0 );

    msWriteByte(SC0_02, NIS_B|ISEL_HDMI_444|SCALER_STYPE_HV);

}
#endif



Bool msDVIEQCalibration(void)
{
    BYTE ucCLKINVal, ucDMVal, ucDRtermVal, ucDVIMuxVal, ucADCPD1, ucADCPD2;
    Bool bFlag = TRUE;

    ucCLKINVal = msReadByte(REG_PM_B2);
    ucDMVal = msReadByte(REG_29C0);
    ucDRtermVal = msReadByte(REG_29C1);
    ucDVIMuxVal = msReadByte(REG_29C2);
    ucADCPD1 = msReadByte(REG_2508);
    ucADCPD2 = msReadByte(REG_2509);

    msWriteByte(REG_29C5, 0x04);
    msWriteByte(REG_29C2, 0xB0); // Isolate HDMI from outside to IC, 0x29C2[1:0] = 2'b 00
    msWriteByte(REG_29C8, 0x92);
    msWriteByteMask(REG_29C9, 0x02, 0x02);
    msWriteByte(REG_29CE, 0x08);
    msWriteByte(REG_29CA, 0x04);
    msWriteByte(REG_29C0, 0x00); // power on DM(EQ), 0x29C0[2:0]
    //#if CHIP_ID==CHIP_TSUM2
    msWriteByte(REG_29C1, 0x0C); // power down data r-term, 0x29C1[3:2]
    //#else
    //msWriteByte(REG_29C1, 0x18); // power down data r-term, 0x29C1[4:3]
    //#endif
    msWriteByte(REG_2508, 0x78);
    msWriteByte(REG_2509, 0x03);

    // power on CLKIN, EQ and power down data R-term to block input signal
    msWriteByte(REG_PM_B2, ucCLKINVal & (~BIT2)); // power on CLKIN, 0x01B2[2]

    // start EQ calibration
    msWriteByteMask(REG_29B8, BIT2, BIT2); // short input differential signal, 0x29B8[2] = 1
    msWriteByteMask(REG_29BE, BIT1, BIT1); // start calibration (EN_CAL), please keep 1 after cal, 0x29BE[1] = 1
    msWriteByteMask(REG_29B8, 0, BIT2); // short input differential signal, 0x29B8[2] = 1

    // wait calibration done
    while(!(msReadByte(REG_29B9) & BIT5) || !(msReadByte(REG_29BA) & BIT5) || !(msReadByte(REG_29BB) & BIT5));

    // restore the previous settings
    msWriteByte(REG_PM_B2, ucCLKINVal);
    msWriteByte(REG_29C0, ucDMVal);
    msWriteByte(REG_29C1, ucDRtermVal);
    msWriteByte(REG_29C2, ucDVIMuxVal);
    msWriteByte(REG_2508, ucADCPD1);
    msWriteByte(REG_2509, ucADCPD2);
    msWriteByte(REG_29C5, 0x00);
    msWriteByte(REG_29C8, 0x00);
    msWriteByteMask(REG_29C9, 0x00, 0x02);
    msWriteByte(REG_29CA, 0x00);
    msWriteByte(REG_29CE, 0x00);

    //PRINT_DATA("@GCal= 0x%x", msReadByte(REG_29B9) & 0x1F);
    //PRINT_DATA("@BCal= 0x%x", msReadByte(REG_29BA) & 0x1F);
    //PRINT_DATA("@RCal= 0x%x", msReadByte(REG_29BB) & 0x1F);

    return bFlag;
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
