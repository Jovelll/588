
///////////////////////////////////////
// DRVMSTAR_V VERSION: V02
////////////////////////////////////////
#include "global.h"
#include "drvDPRxApp.h"
#include "msEread.h"
#include "menufunc.h"
#include "msfb.h"
#include "Detect.h"

#ifndef _DRVMSTAR_C_C_
#define _DRVMSTAR_C_C_

#define drvmStar_DEBUG    0
#if ENABLE_DEBUG&&drvmStar_DEBUG
    #define drvmStar_printData(str, value)   printData(str, value)
    #define drvmStar_printMsg(str)           printMsg(str)
#else
    #define drvmStar_printData(str, value)
    #define drvmStar_printMsg(str)
#endif

#if ENABLE_AUTOEQ
    Bool xdata AUTOEQ_FLAG = 0;
#endif
code RegTblType tPowerOnOffTbl[]=
{
    {REG_1ED1, 0x00, BIT0},
    {REG_1EDC, 0x00, BIT5|BIT4},
    {REG_3041, 0x00, 0xFF},
    {REG_30F0, 0x00, BIT0},
    {REG_30FA, 0x00, BIT7},
    {REG_30EE, 0x07, 0x07},
    {REG_1100, 0x00, BIT5|BIT4|BIT3},
    {REG_1101, 0x00, BIT7},
    {REG_3806, 0x00, BIT5},
    {REG_3A60, 0x00, BIT6},
    //{REG_05F0, 0x00, BIT3},// [3]: AULL power down
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
#if ( (ENABLE_TNR) || (ENABLE_RTE) || (SPI_SSC_EN))
    {REG_1E25, 0x00, BIT2},
    {REG_1E28, 0x00, BIT0},
    {REG_1133, 0x00, BIT7},
#else
    {REG_1E25, BIT2, BIT2},
    {REG_1E28, BIT0, BIT0},
    {REG_1133, BIT7, BIT7},
#endif
    {REG_1E24, 0x00, BIT4|BIT0},
    {REG_1E2C, 0x00, BIT4|BIT0},
#if ENABLE_R2_2DTO3D
    {REG_1E2A, 0x00, BIT0},
#else
    {REG_1E2A, BIT0, BIT0},
#endif
    {REG_1E47, 0x00, BIT0},
#endif
    {REG_01C7, 0x00, 0xFF},  //PHDAC on

};

code RegTbl2Type tComboTMDSMuxTbl[]=
{
#if ENABLE_AUDIO_AUTO_MUTE
    {REG_1428, 0xAF00, 0xFF00},
#endif
    {REG_1600, 0x000F, 0xFFFF},
    {REG_1602, 0x020A, 0xFFFF},
    {REG_1606, BIT11 |(WORD)(HDMI_VCO_CODE_CALCULATE(HDMI_VCO_LOOP_DIV2)), 0xFFFF},
    {REG_1608, BIT11 |(WORD)(HDMI_VCO_CODE_CALCULATE(HDMI_VCO_LOOP_DIV4)), 0xFFFF},
    {REG_160A, BIT11 |(WORD)(HDMI_VCO_CODE_CALCULATE(HDMI_VCO_LOOP_DIV8)), 0xFFFF},
    {REG_1612, 0x0D11, 0xFFFF},
    {REG_161C, 0xFF00, 0xFFFF},
    {REG_1620, 0xFF00, 0xFFFF},
    {REG_1624, 0xFF00, 0xFFFF},
    {REG_1628, 0xFF00, 0xFFFF},
    {REG_1646, 0x081F, 0xFFFF},
    {REG_165C, 0x000D, 0xFFFF},

    {REG_1700, 0x7045, 0xFFFF},
    {REG_1706, 0x1001, 0xFFFF},
    {REG_170F, 0x0000, 0x0008},
    {REG_1710, 0x8000, 0xFFFF},
    {REG_1714, 0x3130, 0xFFFF},
    {REG_1730, 0x0081, 0xFFFF},
    {REG_1752, 0x0000, 0xFFFF},
    {REG_175A, 0x0000, 0xFFFF},
    {REG_175C, 0xDFFF, 0xFFFF},
    {REG_17A2, 0x800E, 0xFFFF},
    //{REG_17AE, 0x0080, 0xFFFF},
    {REG_17AE, 0x0F80, 0xFFFF}, // 20130201 - follow RD new IP driver table
    {REG_17B0, ((HDMI_B_CHANNEL_EQ <<8) |HDMI_CLK_CHANNEL_EQ), 0xFFFF},
    {REG_17B2, ((HDMI_R_CHANNEL_EQ <<8) |HDMI_G_CHANNEL_EQ), 0xFFFF},
    {REG_17BC, 0x0040 |HDMI_PLL_LDO_VALUE, 0xFFFF},   // [6]: for all mode,   VCO switch cap always on, add PLL LDO 10% setting
    {REG_1782, 0x8101, 0xFFFF},
    {REG_17A8, 0x001B, 0xFFFF},
    //{REG_17AC, 0x8FE0, 0xFFFF},
    {REG_17AC, 0x87E0, 0xFFFE}, // 20130201 - follow RD new IP driver table
    {REG_1740, 0x0001, 0xFFFF},
    {REG_17B4, 0x0010, 0xFFFF},   // [4]: for all mode,  phase dac vring LDO +5%
    {REG_174C, 0xFFFF, 0xFFFF},
    {REG_174E, 0x3FFF, 0xFFFF},
    {REG_17BE, 0x0000, 0xFFFF},

    {REG_01C2, 0x0104, 0xFF1F}, // port mux couldn't be set in the initial table
    {REG_01C6, 0x111F, 0xFFFF},
    {REG_01CA, 0x10F8, 0xFFFF},
    {REG_01CC, 0x0000, 0xFFFF},
    {REG_01CE, 0x0020, 0xFFF8},
    {REG_0C01, 0x0000, 0x0300},
    {REG_1506, 0x0000, 0x0004},
    {REG_01BB, 0x0000, 0x0030}, // disable DP differential Rterm
#if DHDMI_SIMPLAYHD_PATCH // SimplayHD CTS 8-18
    {REG_1508, 0x000C, 0x840C}, //km INT
    {REG_153C, 0x0002, 0x0002}, //km INT
#endif // #if DHDMI_SIMPLAYHD_PATCH // SimplayHD CTS 8-18
#if ENABLE_DP_INPUT
    {REG_18E0, 0x0000, 0x0006}, //disable FT
#endif

    {REG_1542, 0, 0x1FFF}, // REG_COMBO_HDCP_21[12]: phase overwrite bit, [11:0]: TMDS ch0 phase code, TMDS ch0 phase release
    {REG_1544, 0, 0x1FFF}, // REG_COMBO_HDCP_22[12]: phase overwrite bit, [11:0]: TMDS ch0 phase code, TMDS ch1 phase release
    {REG_1546, 0, 0x1FFF}, // REG_COMBO_HDCP_23[12]: phase overwrite bit, [11:0]: TMDS ch0 phase code, TMDS ch2 phase release
    {REG_1604, 0, BIT3| BIT2| BIT1}, // REG_COMBO_DECODER_02[3:1]: swap for MHL v1.2, [2]: 0:SW mode, [1]: packed-pixel mode enable
    {REG_17AA, 0, BIT15| BIT1| BIT0}, // REG_COMBO_ATOP_55[15]: MHL mode DIV selection overwrite, [1:0]: MHL mode DIV select for override control.
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
    {SC0_1E, ( PanelVTotal - 1 ) & 0xFF}, // output vtotal
    {SC0_1F, ( PanelVTotal - 1 ) >> 8}, // output vtotal
    {SC0_20, ( PanelVTotal - PanelVStart ) & 0xFF}, // vsync start
    {SC0_21, ( PanelVTotal - PanelVStart)>>8}, // vsync start
    {SC0_22, ( PanelVTotal - PanelVStart ) >> 8}, // vsync start
    {SC0_22, ( PanelVTotal - PanelVSyncBackPorch ) & 0xFF}, //vsync end
    {SC0_23, ( PanelVTotal - PanelVSyncBackPorch ) >> 8}, //vsync end
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

    {REG_3041, 0x00},       //???????????????????????????
    {REG_3080, (PanelSwapMLSB&BIT6)|(BIT5&PANEL_SWAP_PN)|(BIT2&LVDS_TIMode)|BIT3},
    {REG_308A, 0x3F},       //???????????????????????????
    {REG_3093, (PanelSwapOddRB&BIT3)|(PanelSwapEvenRB&BIT5)|(PanelSwapOddML&BIT4)|(PanelSwapEvenML&BIT6)},
#if (PanelDither==6 && (PanelSwapEvenML||PanelSwapOddML))
    {REG_3092, 0x80},       //[7:6]=2'b10 for 6bit panel   need set this for PanelSwapEvenML(REG_3093[6][4])
#elif (PanelDither==8 && (PanelSwapEvenML||PanelSwapOddML))
    {REG_3092, 0x40},       //[7:6]=2'b01 for 8bit panel
#elif (PanelDither==10 && (PanelSwapEvenML||PanelSwapOddML))
    {REG_3092, 0xC0},       //[7:6]=others for 10bit panel
#else
    {REG_3092, 0x00},       //[7:6]=others for 10bit panel
#endif

    {REG_3081, (PanelDClkDelay<<4)}, // DE delay, clock delay for TTL output    //???????????????????????????
    {REG_3094, (PanelInvDClk&BIT4)|(PanelInvDE&BIT2)|(PanelInvVSync&BIT3)   //???????????????????????????
               |(PanelDualPort&BIT1)|(PanelSwapPort&BIT0)},

    //{REG_3089, PanelDualPort&BIT4},
    {REG_3095, (PanelInvHSync&BIT4)},   //???????????????????????????
    {REG_3096, LVDS_TI_BitModeSel},     //???????????????????????????
    //{REG_3092, 0x00},                                //???????????????????????????
    //{REG_30DA, 0x55},
    //{REG_30DB, 0x55},
    //{REG_30DC, 0x05},
    {REG_30EE, 0x07},               //???????????????????????????
    {REG_30F0, BIT0},               //???????????????????????????
    {REG_380C, 0x00},               //???????????????????????????
    {REG_380D, 0x00},               //???????????????????????????
    {REG_380E, 0x03},               //???????????????????????????
    {REG_3814, 0x00},               //???????????????????????????
    {REG_3815, 0xD0},               //???????????????????????????
    {REG_3816, 0x65},               //???????????????????????????
    {REG_3817, 0x65},               //???????????????????????????

    {SC0_6C, 0x90}, // short line tune coef. // 120928 coding, modified default value 0x70 to 0x90

    // Clock generator control 0xD0~0xDB
    {SC0_D0, 0x00},
    //{REG_30AA, 0x00}, // BK0_D0[0] Rose <--> REG_30AA[0] CHIP_TSUML                     //???????????????????????????
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

     //MRW Init Settings
    {SC6_02, 0x80}, // double buffer enable
    {SC6_02, 0xA0},
    {SC6_03, 0x80},
    {SC6_06, 0x20}, //write burst length to MIU
    {SC6_08, 0x20}, //read burst length to MIU
    {SC6_A2, 0x10}, // Write Syn FIFO delay 1t
    {SC6_D4, 0x40}, //MSB read burst length to MIU
    {SC6_D5, 0x20}, //LSB read burst length to MIU

    // R2Y Sel
    {SC7_41, 0x11}, //Main & Sub window RGB to YCbCr equation selection 0: coefficient(RGB range 16~235), BT601. 1 : coefficient(RGB range 0~255), BT601

    {SC8_30, 0xFF}, //reg_gamma_r_max_base0
    {SC8_31, 0x03}, //
    {SC8_34, 0xFF}, //reg_gamma_g_max_base0
    {SC8_35, 0x03}, //
    {SC8_38, 0xFF}, //reg_gamma_b_max_base0
    {SC8_39, 0x03}, //

    // Sub window init setting
    {SCB_28, 0x02}, // SCB_28[1], sub window init setting, better to set/clr according to sub win enable/disable (SC0_5C[3])


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
    //{REG_29E1, 0xFF}, //Disable HDMI INT_MASK
    //{REG_05C0, 0xFF}, //Disable HDMI INT_MASK
    //{REG_05C1, 0xFF}, //Disable HDMI INT_MASK
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

    // combo
    {SC0_FA, BIT2}, // Need set for combo hv mode ([2] always 1)

    // SAR
    {REG_3A60, 0x20},
    {REG_3A61, 0x20},
    {REG_3A62, SARKEY_EN|CABLE_DET_SAR_EN}, // SAR0-3
    {REG_3A66, CABLE_DET_SAR2_EN}, // SAR4-7 - HDMI cable det

    {SC0_00, 0x00},
};
code RegUnitType tb_3D_DITHER_TABLE[] =
{
    {SC9_D0, 0x19},
    {SC9_D1, 0x00},
    {SC9_D2, 0x99},
    {SC9_D3, 0x27},
    {SC9_F4, 0x8D},
    {SC9_F5, 0x63},
    {SC9_F6, 0x9C},
    {SC9_F7, 0x4E},
    {SC9_F8, 0x4B},
    {SC9_F9, 0x93},
    {SC9_FA, 0x39},
    {SC9_FB, 0x0C},

    {SC0_00, 0x00},
};

void msInitClockGating( void )
{

    msWriteByteMask(REG_1E29, 0x04, 0x3F);      // enable 432Mhz as AU_sync_clk   //[2]: AUPLL Synth. select 216MHz
    msWriteByteMask(REG_1E24, 0x00, _BIT0);                 // osd clk
    msWriteByteMask(REG_1E30, 0x00, BIT4|_BIT0);
    msWriteByteMask(REG_1E35, 0x00, _BIT0);                 // fclk (LB clk)
    msWriteByteMask(REG_1E37, 0x00, BIT2|_BIT0);                 // odclk1 & [2] must be set.
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
    msWriteByteMask( REG_3806, 0x00, BIT5);

#if ( (ENABLE_TNR) || (ENABLE_RTE) || (SPI_SSC_EN) )
    msWriteByteMask(REG_1E25, 0x00, BIT2);
    msWriteByteMask(REG_1E28, 0x00, BIT0);
    msWriteByteMask(REG_1133, 0x00, BIT7);
#else
    msWriteByteMask(REG_1E25, BIT2, BIT2);
    msWriteByteMask(REG_1E28, BIT0, BIT0);
    msWriteByteMask(REG_1133, BIT7, BIT7);
#endif

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
    //patch for HDCP issue, reset HDCP FSM when no input clock
    msWriteByteMask(REG_150D, BIT5, BIT5);

    // DVI power enable
    //msWriteByteMask(REG_PM_B2, 0, BIT2);         // power on DVI CKIN. (port 0 and port 1) (reg_b2[2])
    // 1200301 coding, set EQ to 1
    msWriteByte(REG_01C7,0x00);  // DVI phase dac power on

    // Audio
    msWriteByteMask(REG_1712, 0x22, 0xFF); //[3:0]:KP, [7:4]:KM
    msWriteByteMask(REG_1713, 0x00, 0x60); //[6:5]:KN
    msWriteByteMask(REG_1715, 0x31, 0xFF); //[B:8]:DDIV, [F:C]:FBDIV

    // Word Boundary setting
    msWriteByteMask(REG_161D, 0xFF, 0xFF);
    msWriteByteMask(REG_1621, 0xFF, 0xFF);
    msWriteByteMask(REG_1625, 0xFF, 0xFF);
    msWriteByteMask(REG_1629, 0xFF, 0xFF);

    // SPDIF Out Enable
    msWriteByteMask(REG_1426, 0x02, 0x02);

#if 0 //If support I2S, need these settings
    msWriteByteMask(REG_0207, 0x03, 0x03); //Enable I2S output pad and AUMCK output pad control
    msWriteByteMask(REG_1426, 0x05, 0x05); //[2]: I2S out mute OEN, [0]: I2S out OEN
    msWriteByteMask(REG_174B, 0x04, 0x04); //[2]: I2S main clock 192fs out
    msWriteByteMask(REG_1751, BIT7, BIT7); //[2]: I2S main clock 192fs out
    #if I2S_gothrough_DPGA
        msWriteByteMask(REG_0207, BIT6|BIT5|BIT4    , BIT6|BIT5|BIT4|BIT1|BIT0);    //Enable I2S output pad
        msWriteByteMask(REG_2C04, BIT6|0x08, BIT6|0x0F);    ////[6]enable test clk to MCK [3:0]=1000 //sel test clk = 256FS   =48*256=12.288M
    #endif
#endif

    msWrite2Byte(REG_1604, 0x00C0);
    msWrite2Byte(REG_1438, 0x0010);
    msWrite2Byte(REG_142A, 0x0017);

    //CTS
    msWriteByteMask(REG_1730, 0x81, 0x81);  //Enable HDMI CTS-N synthesizer, select CTS[19:0]
    msWriteByteMask(REG_1740, 0x00, 0x06);
    msWriteByteMask(REG_1711, BIT7, BIT7);
    msWriteByteMask(REG_1707, BIT4, BIT4);

    //msWriteByteMask(REG_0200, 0x02, 0x02);
    msWriteByteMask(REG_1439, 0xC0, 0xFF);  //Audio volume attenuation control
    msWriteByteMask(REG_1427, 0x10, 0x10);  //Reset Audio FIFO
    msWriteByteMask(REG_1427, 0x00, 0x10);

    // Lane 0 delay control
    msWriteByteMask(REG_1613, 0x0C, 0x0E);

    msWrite2Byte(REG_16B0, 0x0859); //clock stable IRQ boumdary
    msWrite2Byte(REG_16B2, 0x145D);  //clock stable IRQ boumdary

    // PLL + Digital CDR (For U01 Demo)
    //msWriteByteMask(REG_17AD, 0x03, 0x03);
    //msWriteByteMask(REG_17A8, 0x1B, 0x2F);
    msWriteByteMask(REG_17CF, 0x80, 0x80);

    msWriteByteMask(REG_163D, 0x1F, 0x1F);
    msWrite2Byte(REG_175C, 0xFFFF);  //IRQ Mask
    msWrite2Byte(REG_175A, 0xFFFF);  //IRQ CLR
    msWrite2Byte(REG_175A, 0x0000);  //IRQ CLR
    msWrite2Byte(REG_1418, 0xFFFF);  //IRQ Mask Error Status
    msWrite2Byte(REG_1508, 0xFF00);  //IRQ Mask HDCP
    msWrite2Byte(REG_153C, 0x0001);  //Clr IRQ
    msWriteByteMask(REG_2B19, 0x00, 0x80);  //IRQ bank Mask off

    // Power up Xtal clocks and DVI detection clock for DVI clock detection
    //msWriteWordMask(REG_290D, 0, 0x601);      // HDCP related power
    // DVI PLL power control
    //msWriteWordMask(REG_29C1, 0, 0xFFFF);     // enable DVI0 PLL power
    // DVI slowly update
    //msWriteByteMask(REG_2853, 0xE3, 0xFF);      // [15]:update Bch slowly; [14:8]: 0x63(100 lines)
    //msWrite2ByteMask(REG_2854, 0xE3E3, 0xFFFF); // [15:8]: update Rch slowly, [7:0]:update Gch slowly
    // Enable DVI phase accumulator extension and clock stable gate
    //msWriteByteMask(REG_2801, BIT6|BIT5, BIT6|BIT5); // [6]:Enable of acc8lsb extension ; [5]:Enable of acc8lsb extension select
    //msWriteByteMask(REG_2841, BIT2, BIT2);      // [2]:Enable the function that clock gen gate signal controlled by TMDS clock stable
    // enable Vsync glitch filter
    //msWriteByteMask(REG_283C, BIT4, BIT4);      // enable DVI port0 Vsync glitch filter
    // Enable DVI clock tolerance: 4*0.7 = 2.8 MHz
    //msWriteByteMask(REG_2827, 0x20, 0x70);      //the comp value is reg_comp_tol * 4
    //msWriteByteMask(REG_2825, 0xF0, 0xF0);      //0x110A25=0xB0, DVI clock detection counter tolerance set to 7
    //msWriteByte(REG_2852,0x10);                 // [4]: enable auto DVIPLL adjust
}

#if ENABLE_AUTOEQ
void drv_TMDS_AUTOEQ_initial(void)
{
    msWriteByteMask(REG_17AB, 0x04, 0x04); //ECO bit
    msWriteByteMask(REG_1650, 0x80, 0x80); //Enable speed up EQ tune
    msWriteByteMask(REG_164A, 0x00, 0x13); //under over parse mode
    msWrite2Byte(REG_1658, 0x0002); //fine tune time value
    msWriteByteMask(REG_164A, 0x08, 0x68);
    msWrite2Byte(REG_1652, 0x0042); //symbol detect time

    msWriteByteMask(REG_164A, 0x00, 0x80);
    msWriteByteMask(REG_164C, 0x0A, 0x3F); //phase check times
    msWriteByteMask(REG_164E, 0x03, 0x3F); //phase stable threshold value
    msWrite2Byte(REG_1654, 0x0080); //phase check times

    msWriteByteMask(REG_1650, 0x03, 0x7F); //accumulator threshold
    msWrite2Byte(REG_1656, 0x0040); //under over calcu time

    msWriteByteMask(REG_164D, 0x0A, 0x3F);
    msWriteByteMask(REG_164F, 0x03, 0x3F);

    msWriteByteMask(REG_1651, 0x01, 0x03);
    msWriteByteMask(REG_1651, 0x04, 0x04);
    msWrite2Byte(REG_165A, 0x0028);
}

void drv_TMDS_AUTOEQ_TRIG(void)
{
    AUTOEQ_FLAG = 0;
    msWriteByteMask(REG_164A, 0x00, 0x04); //TRIG  off
    msWriteByteMask(REG_17AC, 0x00, 0xE0); //disable EQ overwrite
    msWrite2Byte(REG_1752, 0x0008); //RESET
    msWrite2Byte(REG_1752, 0x0000);
    msWriteByteMask(REG_164A, 0x04, 0x04); //TRIG
}
#endif

#if DHDMI_SIMPLAYHD_PATCH // SimplayHD CTS 8-18

void Delay1us(void)
{
    BYTE xdata i; // Add by Jonson 20100917

    if(g_bMcuPMClock)
        return;
    i=5; // 315K // 20: 135K
    while(i--)
    {
        _nop_();
    }
}
#endif // #if DHDMI_SIMPLAYHD_PATCH // SimplayHD CTS 8-18

void TMDS_Config_For_PM(void)
{
    if(INPUT_IS_TMDS())
    {
        WORD clk_cnts =0;
        if(DVI_CLK_STABLE())
            clk_cnts = ((((WORD)msReadByte((REG_16B6)+1))<<8)|msReadByte((REG_16B6)));
        else
            return;


        if( clk_cnts > HDMI_VCO_CODE_CALCULATE(HDMI_VCO_LOOP_DIV2) ) //div 1
        {
            msWriteByteMask(REG_17AB, 0x60, 0x60);
            msWriteByteMask(REG_17AA, 0x00, 0x0C);
            msWriteByteMask(REG_17A5, 0x00, 0x0C);
            msWriteByteMask(REG_17AC, 0x01, 0x01); //ICtrl overwrite
            msWriteByteMask(REG_17AA, 0x10, 0xF0); //ICtrl value
        }
        else if( clk_cnts > HDMI_VCO_CODE_CALCULATE(HDMI_VCO_LOOP_DIV4) ) //div 2
        {
            msWriteByteMask(REG_17AB, 0x60, 0x60);
            msWriteByteMask(REG_17AA, 0x04, 0x0C);
            msWriteByteMask(REG_17A5, 0x04, 0x0C);
            msWriteByteMask(REG_17AC, 0x01, 0x01); //ICtrl overwrite
            msWriteByteMask(REG_17AA, 0x20, 0xF0); //ICtrl value
        }
        else if( clk_cnts > HDMI_VCO_CODE_CALCULATE(HDMI_VCO_LOOP_DIV8) ) //div 4
        {
            msWriteByteMask(REG_17AB, 0x60, 0x60);
            msWriteByteMask(REG_17AA, 0x08, 0x0C);
            msWriteByteMask(REG_17A5, 0x08, 0x0C);
            msWriteByteMask(REG_17AC, 0x01, 0x01); //ICtrl overwrite
            msWriteByteMask(REG_17AA, 0x40, 0xF0); //ICtrl value
        }
        else //div 8
        {
            msWriteByteMask(REG_17AB, 0x60, 0x60);
            msWriteByteMask(REG_17AA, 0x0C, 0x0C);
            msWriteByteMask(REG_17A5, 0x0C, 0x0C);
            msWriteByteMask(REG_17AC, 0x01, 0x01); //ICtrl overwrite
            msWriteByteMask(REG_17AA, 0x40, 0xF0); //ICtrl value
        }
    }

}

void TMDSISR(void)
{
#if DHDMI_SIMPLAYHD_PATCH // SimplayHD CTS 8-18
    BYTE xdata u8AksvVal = 0;
    BYTE xdata u8HDCPFSM = 0;
    static Bool xdata bflag = FALSE;
    BYTE xdata i_loop;
#endif

    if(!INPUT_IS_TMDS())
    {
        MEM_MSWRITE_BYTE_MASK(REG_175D, 0x20, 0x20);
        MEM_MSWRITE_BYTE_MASK(REG_175B, 0x20, 0x20);
        MEM_MSWRITE_BYTE_MASK(REG_175B, 0x00, 0x20);
        return;
    }

    if(_bit5_(msRegs[REG_1757])) // CLK back to stable IRQ
    {
        WORD clk_cnts =0;
        MEM_MSWRITE_BYTE_MASK(REG_175D, 0x20, 0x20);//Mask
        MEM_MSWRITE_BYTE_MASK(REG_1543, 0x00, 0x10);//disable overwrite
        MEM_MSWRITE_BYTE_MASK(REG_1569, 0x01, 0x01);//auto_CLR
        MEM_MSWRITE_BYTE_MASK(REG_1569, 0x00, 0x01);
        clk_cnts = ((((WORD)MEM_MSREAD_BYTE((REG_16B6)+1))<<8)|MEM_MSREAD_BYTE((REG_16B6)));
        //if (!(_bit2_(msRegs[REG_0C01]))) // not MHL
        {
            if( clk_cnts > HDMI_VCO_CODE_CALCULATE(HDMI_VCO_LOOP_DIV2) ) //div 1
            {
                MEM_MSWRITE_BYTE_MASK(REG_17AB, 0x60, 0x60);
                MEM_MSWRITE_BYTE_MASK(REG_17AA, 0x00, 0x0C);
                MEM_MSWRITE_BYTE_MASK(REG_17A5, 0x00, 0x0C);
                MEM_MSWRITE_BYTE_MASK(REG_17AC, 0x01, 0x01); //ICtrl overwrite
                MEM_MSWRITE_BYTE_MASK(REG_17AA, 0x10, 0xF0); //ICtrl value
            }
            else if( clk_cnts > HDMI_VCO_CODE_CALCULATE(HDMI_VCO_LOOP_DIV4) ) //div 2
            {
                MEM_MSWRITE_BYTE_MASK(REG_17AB, 0x60, 0x60);
                MEM_MSWRITE_BYTE_MASK(REG_17AA, 0x04, 0x0C);
                MEM_MSWRITE_BYTE_MASK(REG_17A5, 0x04, 0x0C);
                MEM_MSWRITE_BYTE_MASK(REG_17AC, 0x01, 0x01); //ICtrl overwrite
                MEM_MSWRITE_BYTE_MASK(REG_17AA, 0x20, 0xF0); //ICtrl value

            }
            else if( clk_cnts > HDMI_VCO_CODE_CALCULATE(HDMI_VCO_LOOP_DIV8) ) //div 4
            {
                MEM_MSWRITE_BYTE_MASK(REG_17AB, 0x60, 0x60);
                MEM_MSWRITE_BYTE_MASK(REG_17AA, 0x08, 0x0C);
                MEM_MSWRITE_BYTE_MASK(REG_17A5, 0x08, 0x0C);
                MEM_MSWRITE_BYTE_MASK(REG_17AC, 0x01, 0x01); //ICtrl overwrite
                MEM_MSWRITE_BYTE_MASK(REG_17AA, 0x40, 0xF0); //ICtrl value
            }
            else //div 8
            {
                MEM_MSWRITE_BYTE_MASK(REG_17AB, 0x60, 0x60);
                MEM_MSWRITE_BYTE_MASK(REG_17AA, 0x0C, 0x0C);
                MEM_MSWRITE_BYTE_MASK(REG_17A5, 0x0C, 0x0C);
                MEM_MSWRITE_BYTE_MASK(REG_17AC, 0x01, 0x01); //ICtrl overwrite
                MEM_MSWRITE_BYTE_MASK(REG_17AA, 0x40, 0xF0); //ICtrl value
            }
#if ENABLE_AUTOEQ
            if( clk_cnts > HDMI_VCO_CODE_CALCULATE(HDMI_VCO_LOOP_DIV2) )
            {
                AUTOEQ_FLAG = 1;
                MEM_MSWRITE_BYTE_MASK(REG_1752, 0x00, 0x80);
            }
            else
            {
                AUTOEQ_FLAG = 0;
                MEM_MSWRITE_BYTE_MASK(REG_164A,0x00,0x04);
                MEM_MSWRITE_BYTE_MASK(REG_17AC,0xE0,0xE0);
                MEM_MSWRITE_BYTE(REG_17B0, HDMI_CLK_CHANNEL_EQ);
                MEM_MSWRITE_BYTE(REG_17B1, HDMI_B_CHANNEL_EQ);
                MEM_MSWRITE_BYTE(REG_17B2, HDMI_G_CHANNEL_EQ);
                MEM_MSWRITE_BYTE(REG_17B3, HDMI_R_CHANNEL_EQ);
            }
#endif
        }

        MEM_MSWRITE_BYTE_MASK(REG_175B, 0x20, 0x20);
        MEM_MSWRITE_BYTE_MASK(REG_175B, 0x00, 0x20);
        MEM_MSWRITE_BYTE_MASK(REG_175D, 0x00, 0x20);
    }
#if DHDMI_SIMPLAYHD_PATCH // SimplayHD CTS 8-18
    if(_bit2_(msRegs[REG_1539]))
    {

        MEM_MSWRITE_BYTE_MASK(REG_1509, 0x84, 0x84);
        MEM_MSWRITE_BYTE(REG_1539, 0xFF);

        u8HDCPFSM = MEM_MSREAD_BYTE(REG_1531) & 0x1F;


        if(((u8HDCPFSM !=  0x02) && !((u8HDCPFSM ==  0x03) && (MEM_MSREAD_BYTE(REG_1518) & BIT7))) && !bflag)
        //if((MEM_MSREAD_BYTE(REG_1518) & BIT7) && !bflag)
        {
            MEM_MSWRITE_BYTE_MASK(REG_150F, 0x00, 0xC0);
            MEM_MSWRITE_BYTE_MASK(REG_150C, 0x14, 0xFF);
            MEM_MSWRITE_BYTE_MASK(REG_150D, 0, 0x03);
            MEM_MSWRITE_BYTE(REG_153A, 0x10);
            for(i_loop=0; i_loop<2;i_loop++) {Delay1us();}
            MEM_MSWRITE_BYTE(REG_153A, 0x04);
            for(i_loop=0; i_loop<2;i_loop++) {Delay1us();}
            u8AksvVal = MEM_MSREAD_BYTE(REG_1525);
            MEM_MSWRITE_BYTE_MASK(REG_150F, 0x80, 0xC0);
            MEM_MSWRITE_BYTE(REG_153A, 0x10);
            for(i_loop=0; i_loop<2;i_loop++) {Delay1us();}
            MEM_MSWRITE_BYTE(REG_150E, u8AksvVal);
            MEM_MSWRITE_BYTE(REG_153A, 0x08);
            for(i_loop=0; i_loop<2;i_loop++) {Delay1us();}
            MEM_MSWRITE_BYTE_MASK(REG_150F, 0x00, 0xC0);
            bflag = TRUE;
        }
        else
        {
            bflag = FALSE;
        }
        MEM_MSWRITE_BYTE_MASK(REG_1509, 0x00, 0x84);
    }
#endif // #if DHDMI_SIMPLAYHD_PATCH // SimplayHD CTS 8-18
}

#if ENABLE_CABLE_5V_EDID
#define EDID_BLOCK_SIZE 128
#define VGA_EDID_SIZE   128
#define VGA_EDID_SRAM_BASE_ADDR    0// 0
/*
code BYTE tEDID_TABLE_VGA[VGA_EDID_SIZE] =
{
#if 1//JIARUNYUAN  1920 1080
0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x2A,0x59, 
0x80,0x23,0x00,0x00,0x00,0x00,0x14,0x1D,0x01,0x03, 
0x68,0x34,0x1F,0x78,0xEA,0x00,0x10,0xA0,0x57,0x49, 
0x9B,0x26,0x10,0x48,0x4C,0xAF,0xCF,0x00,0x81,0x00, 
0x81,0x40,0x95,0x00,0x71,0x40,0xB3,0x00,0x81,0xC0, 
0x81,0x80,0xA9,0xC0,0x02,0x3A,0x80,0x18,0x71,0x38, 
0x2D,0x40,0x58,0x2C,0x45,0x00,0xB4,0xF5,0x10,0x00, 
0x00,0x1E,0x80,0x48,0x80,0x18,0x71,0x38,0x2D,0x40, 
0x30,0x28,0x55,0x00,0xB4,0xFE,0x10,0x00,0x00,0x18, 
0x66,0x21,0x50,0xB0,0x51,0x00,0x1B,0x30,0x40,0x70, 
0x34,0x00,0xC4,0xFE,0x10,0x00,0x00,0x18,0x00,0x00, 
0x00,0xFC,0x00,0x00,0x0A,0x20,0x20,0x20,0x20,0x20, 
0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x97            
#else
    0x00,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0x00,   0x1E,   0x6D,   0x8F,   0x5A,   0x01,   0x01,   0x01,   0x01,
    0x01,   0x18,   0x01,   0x03,   0x6B,   0x35,   0x1E,   0x78,   0xEA,   0xA4,   0x35,   0xA5,   0x54,   0x4F,   0x9E,   0x27,
    0x12,   0x50,   0x54,   0xA7,   0x6B,   0x80,   0xD1,   0xC0,   0x45,   0x7C,   0x61,   0x7C,   0x81,   0x80,   0x81,   0xBC,
    0x95,   0x3C,   0x31,   0x7C,   0x01,   0x01,   0x02,   0x3A,   0x80,   0x18,   0x71,   0x38,   0x2D,   0x40,   0x58,   0x2C,
    0x45,   0x00,   0x13,   0x2A,   0x21,   0x00,   0x00,   0x1E,   0x00,   0x00,   0x00,   0xFD,   0x00,   0x38,   0x78,   0x1E,
    0x87,   0x15,   0x00,   0x0A,   0x20,   0x20,   0x20,   0x20,   0x20,   0x20,   0x00,   0x00,   0x00,   0xFC,   0x00,   0x32,
    0x34,   0x47,   0x4D,   0x37,   0x37,   0x0A,   0x20,   0x20,   0x20,   0x20,   0x20,   0x20,   0x00,   0x00,   0x00,   0xFF,
    0x00,   0x0A,   0x20,   0x20,   0x20,   0x20,   0x20,   0x20,   0x20,   0x20,   0x20,   0x20,   0x20,   0x20,   0x00,   0x1F
    #endif
};
*/
#if( (PANEL_WIDTH == 640) &&(PANEL_HEIGHT == 480) )
code BYTE tEDID_TABLE_VGA[VGA_EDID_SIZE] =
{
	//Todo:
	#if 1		//CJT add 640X480
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x63,0x18,0x80,0x01,0x00,0x00,0x00,0x00,
	0x2C,0x11,0x01,0x03,0x08,0x59,0x32,0x8C,0xE8,0xE2,0xB2,0xA1,0x5B,0x4E,0x98,0x24,
	0x15,0x4F,0x4A,0xA1,0x0C,0x00,0x61,0x40,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x66,0x21,0x50,0xB0,0x51,0x00,0x1B,0x30,0x40,0x70,
	0x36,0x00,0x75,0xF2,0x31,0x00,0x00,0x1E,0x64,0x19,0x00,0x40,0x41,0x00,0x26,0x30,
	0x18,0x88,0x36,0x00,0x75,0xF2,0x31,0x00,0x00,0x18,0x00,0x00,0x00,0xFD,0x00,0x38,
	0x46,0x20,0x50,0x09,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xFC,
	0x00,0x48,0x44,0x54,0x56,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x8B,
	#endif
};
#elif( (PANEL_WIDTH == 800) &&(PANEL_HEIGHT == 600) )
code BYTE tEDID_TABLE_VGA[VGA_EDID_SIZE]=
{
	//Todo:
	#if 1		//CJT add 800X600
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x63,0x18,0x80,0x01,0x00,0x00,0x00,0x00,
	0x2C,0x11,0x01,0x03,0x08,0x59,0x32,0x8C,0xE8,0xE2,0xB2,0xA1,0x5B,0x4E,0x98,0x24,
	0x15,0x4F,0x4A,0xA5,0x4C,0x00,0x61,0x40,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0xA0,0x0F,0x20,0x00,0x31,0x58,0x1C,0x20,0x28,0x80,
	0x14,0x00,0x20,0x56,0x00,0x00,0x00,0x1E,0x64,0x19,0x00,0x40,0x41,0x00,0x26,0x30,
	0x18,0x88,0x36,0x00,0x75,0xF2,0x31,0x00,0x00,0x18,0x00,0x00,0x00,0xFD,0x00,0x38,
	0x46,0x20,0x50,0x09,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x66,0x21,0x50,0xB0,
	0x51,0x00,0x1B,0x30,0x40,0x70,0x36,0x00,0x75,0xF2,0x31,0x00,0x00,0x1E,0x00,0x9F,

	#endif
};
#elif( (PANEL_WIDTH == 800) &&(PANEL_HEIGHT == 480) )
code BYTE tEDID_TABLE_VGA[VGA_EDID_SIZE] =
{
	//Todo:
	#if 1		//CJT add 800X600
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x63,0x18,0x80,0x01,0x00,0x00,0x00,0x00,
	0x2C,0x11,0x01,0x03,0x08,0x59,0x32,0x8C,0xE8,0xE2,0xB2,0xA1,0x5B,0x4E,0x98,0x24,
	0x15,0x4F,0x4A,0xA5,0x4C,0x00,0x61,0x40,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0xA0,0x0F,0x20,0x00,0x31,0x58,0x1C,0x20,0x28,0x80,
	0x14,0x00,0x20,0x56,0x00,0x00,0x00,0x1E,0x64,0x19,0x00,0x40,0x41,0x00,0x26,0x30,
	0x18,0x88,0x36,0x00,0x75,0xF2,0x31,0x00,0x00,0x18,0x00,0x00,0x00,0xFD,0x00,0x38,
	0x46,0x20,0x50,0x09,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x66,0x21,0x50,0xB0,
	0x51,0x00,0x1B,0x30,0x40,0x70,0x36,0x00,0x75,0xF2,0x31,0x00,0x00,0x1E,0x00,0x9F,
	#endif
};
#elif( (PANEL_WIDTH == 1024) &&(PANEL_HEIGHT == 502) )
code BYTE tEDID_TABLE_VGA[VGA_EDID_SIZE]=
{
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x63,0x18,0x80,0x01,0x00,0x00,0x00,0x00,
	0x2C,0x11,0x01,0x03,0x08,0x59,0x32,0x8C,0xE8,0xE2,0xB2,0xA1,0x5B,0x4E,0x98,0x24,
	0x15,0x4F,0x4A,0xA1,0x0C,0x00,0x61,0x40,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x66,0x21,0x50,0xB0,0x51,0x00,0x1B,0x30,0x40,0x70,
	0x36,0x00,0x75,0xF2,0x31,0x00,0x00,0x1E,0x64,0x19,0x00,0x40,0x41,0x00,0x26,0x30,
	0x18,0x88,0x36,0x00,0x75,0xF2,0x31,0x00,0x00,0x18,0x00,0x00,0x00,0xFD,0x00,0x38,
	0x46,0x20,0x50,0x09,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xFC,
	0x00,0x48,0x44,0x54,0x56,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x8B,
};
#elif 0//( (PANEL_WIDTH == 1024) &&(PANEL_HEIGHT == 576) )
code BYTE tEDID_TABLE_VGA[VGA_EDID_SIZE]=
{
	//Todo:
};
#elif( (PANEL_WIDTH == 1024) &&(PANEL_HEIGHT == 600) )
code BYTE tEDID_TABLE_VGA[VGA_EDID_SIZE]=
{
0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x31,0x8C,0x01,0x00,0x01,0x00,0x00,0x00,
0x1B,0x14,0x01,0x03,0x68,0x00,0x00,0xFF,0xE8,0xBB,0xB9,0xA3,0x52,0x46,0x98,0x24,
0x0F,0x48,0x4C,0x2D,0xCE,0x00,0x31,0x4F,0x45,0x4F,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x24,0x13,0x00,0x20,0x41,0x58,0x18,0x20,0x28,0x80,
0x63,0x00,0xF8,0x15,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x10,0x00,0x58,0x18,0x20,
0x28,0x80,0xA3,0x00,0xF8,0x15,0x00,0x00,0x00,0x1E,0x00,0x00,0x00,0x10,0x00,0x4C,
0x43,0x44,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xFC,
0x00,0x4C,0x43,0x44,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x00,0xEE,
};
#elif( (PANEL_WIDTH == 1024) &&(PANEL_HEIGHT == 768) )
code BYTE tEDID_TABLE_VGA[VGA_EDID_SIZE] =
{
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x63,0x18,0x80,0x01,0x00,0x00,0x00,0x00,
	0x2C,0x11,0x01,0x03,0x08,0x59,0x32,0x8C,0xE8,0xE2,0xB2,0xA1,0x5B,0x4E,0x98,0x24,
	0x15,0x4F,0x4A,0xA1,0x0C,0x00,0x61,0x40,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x66,0x21,0x50,0xB0,0x51,0x00,0x1B,0x30,0x40,0x70,
	0x36,0x00,0x75,0xF2,0x31,0x00,0x00,0x1E,0x64,0x19,0x00,0x40,0x41,0x00,0x26,0x30,
	0x18,0x88,0x36,0x00,0x75,0xF2,0x31,0x00,0x00,0x18,0x00,0x00,0x00,0xFD,0x00,0x38,
	0x46,0x20,0x50,0x09,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xFC,
	0x00,0x48,0x44,0x54,0x56,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x8B,

	//Todo:
};
#elif( (PANEL_WIDTH == 1280) &&(PANEL_HEIGHT == 768) )
code BYTE tEDID_TABLE_VGA[VGA_EDID_SIZE]=
{
0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x31,0x8C,0x01,0x00,0x01,0x00,0x00,0x00,
0x1B,0x14,0x01,0x03,0x08,0x00,0x00,0x00,0x8E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xAF,0xCE,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x0E,0x1F,0x00,0x80,0x51,0x00,0x1E,0x30,0x40,0x80,
0x37,0x00,0x20,0x56,0x00,0x00,0x00,0x1A,0x3C,0x28,0x00,0xA0,0x51,0x00,0x25,0x30,
0x50,0x80,0x37,0x00,0x20,0x56,0x00,0x00,0x00,0x1A,0x64,0x19,0x00,0x40,0x41,0x00,
0x26,0x30,0x18,0x88,0x36,0x00,0x20,0x56,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0xFC,
0x00,0x4C,0x43,0x44,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x00,0xF2,

	//Todo:
};
#elif( (PANEL_WIDTH == 1280) &&(PANEL_HEIGHT == 800) )
code BYTE tEDID_TABLE_VGA[VGA_EDID_SIZE] =
{
0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x31,0x8C,0x01,0x00,0x01,0x00,0x00,0x00,
0x1B,0x14,0x01,0x03,0x68,0x00,0x00,0xFF,0xEA,0xBB,0xB9,0xA3,0x52,0x46,0x98,0x24,
0x0F,0x48,0x4C,0xAF,0xCE,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0xD0,0x20,0x00,0x98,0x51,0x20,0x1E,0x30,0xD0,0x80,
0x47,0x04,0x68,0x22,0x11,0x00,0x00,0x1C,0x48,0x26,0x00,0x98,0x51,0x20,0x1E,0x30,
0xD0,0x80,0x47,0x04,0x68,0x22,0x11,0x00,0x00,0x1C,0x36,0x29,0x00,0x98,0x51,0x20,
0x1E,0x30,0xD0,0x80,0x47,0x04,0x68,0x22,0x11,0x00,0x00,0x1C,0x00,0x00,0x00,0xFC,
0x00,0x4C,0x43,0x44,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x60,
	//Todo:
};
#elif( (PANEL_WIDTH == 1280) &&(PANEL_HEIGHT == 960) )
code BYTE tEDID_TABLE_VGA[VGA_EDID_SIZE]=
{
0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x31,0x8C,0x01,0x00,0x01,0x00,0x00,0x00,
0x1B,0x14,0x01,0x03,0x08,0x00,0x00,0x00,0x8E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xAF,0xCE,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x0E,0x1F,0x00,0x80,0x51,0x00,0x1E,0x30,0x40,0x80,
0x37,0x00,0x20,0x56,0x00,0x00,0x00,0x1A,0x3C,0x28,0x00,0xA0,0x51,0x00,0x25,0x30,
0x50,0x80,0x37,0x00,0x20,0x56,0x00,0x00,0x00,0x1A,0x64,0x19,0x00,0x40,0x41,0x00,
0x26,0x30,0x18,0x88,0x36,0x00,0x20,0x56,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0xFC,
0x00,0x4C,0x43,0x44,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x00,0xF2,

	//Todo:
};
#elif( (PANEL_WIDTH == 1280) &&(PANEL_HEIGHT == 854) )
code BYTE tEDID_TABLE_VGA[VGA_EDID_SIZE]=
{
0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x31,0x8C,0x01,0x00,0x01,0x00,0x00,0x00,
0x1B,0x14,0x01,0x03,0x08,0x00,0x00,0x00,0x8E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xAF,0xCE,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x0E,0x1F,0x00,0x80,0x51,0x00,0x1E,0x30,0x40,0x80,
0x37,0x00,0x20,0x56,0x00,0x00,0x00,0x1A,0x3C,0x28,0x00,0xA0,0x51,0x00,0x25,0x30,
0x50,0x80,0x37,0x00,0x20,0x56,0x00,0x00,0x00,0x1A,0x64,0x19,0x00,0x40,0x41,0x00,
0x26,0x30,0x18,0x88,0x36,0x00,0x20,0x56,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0xFC,
0x00,0x4C,0x43,0x44,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x00,0xF2,

	//Todo:
};
#elif( (PANEL_WIDTH == 1366) &&(PANEL_HEIGHT == 768) )
code BYTE tEDID_TABLE_VGA[VGA_EDID_SIZE]=
{
	#if 0//ENABLE_EDID_1366_1920
//yym no 1680x1680
#if 0//hfl 20120616 add 1280x768
0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x49,0xF7,0x00,0x00,0x00,0x00,0x00,0x00,
0x06,0x16,0x01,0x03,0x08,0x00,0x00,0x78,0x0A,0xD7,0xA5,0xA2,0x59,0x4A,0x96,0x24,
0x14,0x50,0x54,0xA3,0x08,0x00,0xD1,0xC0,0x01,0x01,0x01,0x01,0x81,0x80,0x81,0x40,
0x81,0xC0,0x01,0x01,0x01,0x01,0x02,0x3A,0x80,0x18,0x71,0x38,0x2D,0x40,0x58,0x2C,
0x45,0x00,0x3F,0x43,0x21,0x00,0x00,0x1A,0x66,0x21,0x50,0xB0,0x51,0x00,0x1B,0x30,
0x40,0x70,0x36,0x00,0x3F,0x43,0x21,0x00,0x00,0x1E,0x0E,0x1F,0x00,0x80,0x51,0x00,
0x1E,0x30,0x40,0x80,0x37,0x00,0x20,0x56,0x00,0x00,0x00,0x1E,0x00,0x00,0x00,0xFC,
0x00,0x48,0x55,0x49,0x44,0x49,0x20,0x54,0x56,0x0A,0x0A,0x0A,0x0A,0x0A,0x00,0x26,
#else
0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x49,0xF7,0x00,0x00,0x00,0x00,0x00,0x00,
0x06,0x16,0x01,0x03,0x08,0x00,0x00,0x78,0x0A,0xD7,0xA5,0xA2,0x59,0x4A,0x96,0x24,
0x14,0x50,0x54,0xA3,0x08,0x00,0xD1,0xC0,0x01,0x01,0x01,0x01,0x81,0x80,0x81,0x40,
0x81,0xC0,0x01,0x01,0x01,0x01,0x02,0x3A,0x80,0x18,0x71,0x38,0x2D,0x40,0x58,0x2C,
0x45,0x00,0x3F,0x43,0x21,0x00,0x00,0x1A,0x66,0x21,0x50,0xB0,0x51,0x00,0x1B,0x30,
0x40,0x70,0x36,0x00,0x3F,0x43,0x21,0x00,0x00,0x1E,0x00,0x00,0x00,0xFD,0x00,0x32,
0x4C,0x1E,0x50,0x10,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xFC,
0x00,0x48,0x55,0x49,0x44,0x49,0x20,0x54,0x56,0x0A,0x0A,0x0A,0x0A,0x0A,0x00,0x3A,
#endif
/*
0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x49,0xF7,0x00,0x00,0x00,0x00,0x00,0x00,
0x06,0x16,0x01,0x03,0x08,0x00,0x00,0x78,0x0A,0xD7,0xA5,0xA2,0x59,0x4A,0x96,0x24,
0x14,0x50,0x54,0xA3,0x08,0x00,0xD1,0xC0,0xB3,0x00,0x01,0x01,0x81,0x80,0x81,0x40,
0x81,0xC0,0x01,0x01,0x01,0x01,0x02,0x3A,0x80,0x18,0x71,0x38,0x2D,0x40,0x58,0x2C,
0x45,0x00,0x3F,0x43,0x21,0x00,0x00,0x1A,0x66,0x21,0x50,0xB0,0x51,0x00,0x1B,0x30,
0x40,0x70,0x36,0x00,0x3F,0x43,0x21,0x00,0x00,0x1E,0x00,0x00,0x00,0xFD,0x00,0x32,
0x4C,0x1E,0x50,0x10,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xFC,
0x00,0x43,0x56,0x54,0x45,0x20,0x54,0x56,0x0A,0x20,0x20,0x20,0x20,0x0A,0x00,0x68,          

*/
	#elif 0//ENABLE_MODE_1366X768
//xkk 120507
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x31,0x8C,0x01,0x00,0x01,0x00,0x00,0x00,
	0x1B,0x14,0x01,0x03,0x08,0x00,0x00,0x00,0x8E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0xAD,0xCE,0x00,0x45,0x40,0x81,0xCF,0x01,0x01,0x01,0x01,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x34,0x21,0x50,0xB0,0x51,0x00,0x1B,0x30,0x40,0x70,
	0x36,0x00,0x20,0x56,0x00,0x00,0x00,0x1E,0x99,0x21,0x56,0xAA,0x51,0x00,0x20,0x30,
	0x46,0x8F,0x33,0x00,0x20,0x56,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x10,0x00,0x00,
	0x26,0x30,0x18,0x88,0x36,0x00,0x20,0x56,0x00,0x00,0x00,0x18,0x0E,0x1F,0x00,0x80,
	0x51,0x00,0x20,0x30,0x40,0x80,0x37,0x00,0x20,0x56,0x00,0x00,0x00,0x1A,0x00,0x2F,

	#else
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x63,0x18,0x80,0x01,0x00,0x00,0x00,0x00,
	0x2C,0x11,0x01,0x03,0x08,0x59,0x32,0x8C,0xE8,0xE2,0xB2,0xA1,0x5B,0x4E,0x98,0x24,
	0x15,0x4F,0x4A,0xA1,0x0C,0x00,0x61,0x40,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x66,0x21,0x50,0xB0,0x51,0x00,0x1B,0x30,0x40,0x70,
	0x36,0x00,0x75,0xF2,0x31,0x00,0x00,0x1E,0x64,0x19,0x00,0x40,0x41,0x00,0x26,0x30,
	0x18,0x88,0x36,0x00,0x75,0xF2,0x31,0x00,0x00,0x18,0x00,0x00,0x00,0xFD,0x00,0x38,
	0x46,0x20,0x50,0x09,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xFC,
	0x00,0x48,0x44,0x54,0x56,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x8B,
	#endif
};
#elif( (PANEL_WIDTH == 1280) &&(PANEL_HEIGHT == 1024) )
code BYTE tEDID_TABLE_VGA[VGA_EDID_SIZE]=
{
0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x0E,0xD4,
0x01,0x00,0x00,0x00,0x00,0x00,0x0C,0x15,0x01,0x03,
0x08,0x58,0x31,0x78,0x0A,0xEE,0x9D,0xA3,0x54,0x4C,
0x99,0x26,0x0F,0x4F,0x54,0xA1,0x08,0x00,0x81,0x80,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x02,0x3A,0x80,0x18,0x71,0x38,
0x2D,0x40,0x58,0x2C,0x45,0x00,0x94,0x24,0x00,0x00,
0x00,0x1E,0x9A,0x29,0xA0,0xD0,0x51,0x84,0x22,0x30,
0x50,0x98,0x36,0x00,0xE8,0x19,0x00,0x00,0x00,0x1C,
0x21,0x39,0x90,0x30,0x62,0x1A,0x27,0x40,0x68,0xB0,
0x36,0x00,0x00,0x00,0x00,0x00,0x00,0x1E,0x00,0x00,
0x00,0xFC,0x00,0x4C,0x43,0x44,0x20,0x54,0x56,0x0A,
0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x00,0x8E,
	//Todo:
};
#elif( (PANEL_WIDTH == 1280) &&(PANEL_HEIGHT == 720) )
code BYTE tEDID_TABLE_VGA[VGA_EDID_SIZE] =
{
0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x31,0x8C,0x01,0x00,0x01,0x00,0x00,0x00,
0x1B,0x14,0x01,0x03,0x04,0x58,0x46,0x00,0x84,0x00,0x13,0xA0,0x57,0x49,0x9B,0x26,
0x10,0x48,0x4E,0xAD,0xCE,0x00,0x31,0x4A,0x45,0x4A,0x81,0xCA,0x81,0xCF,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0xF4,0x1C,0x00,0x78,0x51,0xD0,0x1A,0x20,0x60,0xA0,
0x37,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x10,0x00,0x3C,0x4B,0x47,
0x41,0x0A,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0x10,0x00,0x4C,
0x43,0x44,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xFC,
0x00,0x4C,0x43,0x44,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x00,0x91,
};
#elif( (PANEL_WIDTH == 1440) &&(PANEL_HEIGHT == 900) )
code BYTE tEDID_TABLE_VGA[VGA_EDID_SIZE]=
{
#if 1//cai 20120219 in fact 1920x1080 edid
0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x0E,0xD4,
0x01,0x00,0x00,0x00,0x00,0x00,0x0C,0x15,0x01,0x03,
0x08,0x58,0x31,0x78,0x0A,0xEE,0x9D,0xA3,0x54,0x4C,
0x99,0x26,0x0F,0x4F,0x54,0xA1,0x08,0x00,0x81,0x80,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x02,0x3A,0x80,0x18,0x71,0x38,
0x2D,0x40,0x58,0x2C,0x45,0x00,0x94,0x24,0x00,0x00,
0x00,0x1E,0x9A,0x29,0xA0,0xD0,0x51,0x84,0x22,0x30,
0x50,0x98,0x36,0x00,0xE8,0x19,0x00,0x00,0x00,0x1C,
0x21,0x39,0x90,0x30,0x62,0x1A,0x27,0x40,0x68,0xB0,
0x36,0x00,0x00,0x00,0x00,0x00,0x00,0x1E,0x00,0x00,
0x00,0xFC,0x00,0x4C,0x43,0x44,0x20,0x54,0x56,0x0A,
0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x00,0x8E,
#endif
	//Todo:
};
#elif( (PANEL_WIDTH == 1600) &&(PANEL_HEIGHT == 900) )
code BYTE tEDID_TABLE_VGA[VGA_EDID_SIZE]=
{
#if 0//ENABLE_MODE_1600X900
0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x31,0x8C,0x01,0x00,0x01,0x00,0x00,0x00,
0x1B,0x14,0x01,0x03,0x08,0x20,0x12,0x78,0x2E,0xD7,0xA5,0xA2,0x59,0x4A,0x96,0x24,
0x14,0x50,0x54,0xAF,0xCF,0x00,0x01,0x01,0x81,0x80,0x01,0x01,0x81,0x4A,0x81,0x4F,
0x01,0x01,0x01,0x01,0x01,0x01,0x2F,0x26,0x40,0xA0,0x60,0x84,0x1A,0x30,0x30,0x20,
0x35,0x00,0x00,0x00,0x00,0x00,0x00,0x1A,0x9A,0x29,0xA0,0xD0,0x51,0x84,0x22,0x30,
0x50,0x98,0x36,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x00,0x00,0x00,0x10,0x00,0x4C,
0x43,0x44,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xFC,
0x00,0x4C,0x43,0x44,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x00,0xF1,
#else
0x00,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x63,0x18,0x80,0x01,0x00,0x00,0x00,0x00,
0x2c,0x11,0x01,0x03,0x08,0x59,0x32,0x78,0xe8,0xe2,0xbd,0xa1,0x5b,0x4a,0x98,0x24,
0x15,0x47,0x4a,0xa5,0xce,0x00,0x61,0x40,0x81,0x80,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x64,0x19,0x00,0x40,0x41,0x00,0x26,0x30,0x18,0x88,
0x36,0x00,0x74,0x97,0x32,0x00,0x00,0x18,0x7c,0x2e,0x40,0x10,0x62,0x84,0x20,0x30,
0x50,0x98,0x36,0x00,0x97,0x75,0x21,0x00,0x00,0x1c,0x00,0x00,0x00,0xfd,0x00,0x38,
0x4b,0x20,0x50,0x0c,0x00,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xfc,
0x00,0x4c,0x43,0x44,0x54,0x56,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x2f,
#endif
};
#elif( (PANEL_WIDTH == 1400) &&(PANEL_HEIGHT == 1050) )
code BYTE tEDID_TABLE_VGA[VGA_EDID_SIZE] =
{

0x00,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x31,0x8c,0x01,0x00,0x01,0x00,0x00,0x00,
0x1a,0x10,0x01,0x03,0x08,0x00,0x00,0x00,0x8e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xa1,0x0a,0x00,0x81,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x8f,0x2f,0x78,0xd0,0x51,0x1a,0x27,0x40,0x58,0x90,
0x34,0x00,0x20,0x56,0x00,0x00,0x00,0x1c,0xf0,0x3c,0x78,0xf0,0x51,0x1a,0x31,0x40,
0x68,0x90,0x34,0x00,0x20,0x56,0x00,0x00,0x00,0x1c,0x00,0x00,0x00,0xfc,0x00,0x4c,
0x43,0x44,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0a,0x00,0x00,0x00,0xfc,
0x00,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0xa4,

};

#elif( (PANEL_WIDTH == 1680) &&(PANEL_HEIGHT == 1050) )
code BYTE tEDID_TABLE_VGA[VGA_EDID_SIZE] =
{
0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x31,0x8C,0x01,0x00,0x01,0x00,0x00,0x00,
0x1B,0x14,0x01,0x03,0x08,0x00,0x00,0x00,0x01,0xEE,0x95,0xA3,0x54,0x4C,0x99,0x26,
0x0F,0x50,0x54,0xAD,0xCF,0x00,0x81,0x80,0x81,0x8A,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x6C,0x39,0x90,0x30,0x62,0x1A,0x27,0x40,0x68,0xB0,
0x36,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x74,0x27,0x78,0xA0,0x50,0x1A,0x1E,0x40,
0x30,0x20,0x34,0x00,0x00,0x00,0x00,0x00,0x00,0x1A,0x00,0x00,0x00,0x10,0x00,0x4C,
0x43,0x44,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x00,0x00,0x00,0xFC,
0x00,0x4C,0x43,0x44,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x00,0xFA,

	//Todo:
};
#elif( (PANEL_WIDTH == 1920) &&(PANEL_HEIGHT == 1080) )
code BYTE tEDID_TABLE_VGA[VGA_EDID_SIZE]=
{
#if 1//cai 20120217 copy from CVT V29
0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x0E,0xD4,
0x01,0x00,0x00,0x00,0x00,0x00,0x0C,0x15,0x01,0x03,
0x08,0x58,0x31,0x78,0x0A,0xEE,0x9D,0xA3,0x54,0x4C,
0x99,0x26,0x0F,0x4F,0x54,0xA1,0x08,0x00,0x81,0x80,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x02,0x3A,0x80,0x18,0x71,0x38,
0x2D,0x40,0x58,0x2C,0x45,0x00,0x94,0x24,0x00,0x00,
0x00,0x1E,0x9A,0x29,0xA0,0xD0,0x51,0x84,0x22,0x30,
0x50,0x98,0x36,0x00,0xE8,0x19,0x00,0x00,0x00,0x1C,
0x21,0x39,0x90,0x30,0x62,0x1A,0x27,0x40,0x68,0xB0,
0x36,0x00,0x00,0x00,0x00,0x00,0x00,0x1E,0x00,0x00,
0x00,0xFC,0x00,0x4C,0x43,0x44,0x20,0x54,0x56,0x0A,
0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x00,0x8E,
#else
	0x00,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x63,0x18,0x80,0x01,0x02,0x00,0x00,0x00,
	0x31,0x11,0x01,0x03,0x08,0x5d,0x35,0x78,0xe8,0xd9,0xb0,0xa3,0x57,0x49,0x9c,0x25,
	0x11,0x49,0x4b,0xa1,0x08,0x00,0xd1,0xc0,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x94,0x43,0x80,0x90,0x72,0x38,0x28,0x40,0x80,0xc8,
	0x35,0x00,0x52,0xbe,0x10,0x00,0x00,0x1c,0x00,0x00,0x00,0xff,0x00,0x30,0x30,0x30,
	0x31,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xfc,0x00,0x48,
	0x44,0x54,0x56,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xfd,
	0x00,0x38,0x78,0x1f,0x5a,0x12,0x00,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x94,
#endif	
};
#elif( (PANEL_WIDTH == 1920) &&(PANEL_HEIGHT == 1200) )
code BYTE tEDID_TABLE_VGA[VGA_EDID_SIZE]=
{
//MOVE FROM 1920X1080
0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x0E,0xD4,
0x01,0x00,0x00,0x00,0x00,0x00,0x0C,0x15,0x01,0x03,
0x08,0x58,0x31,0x78,0x0A,0xEE,0x9D,0xA3,0x54,0x4C,
0x99,0x26,0x0F,0x4F,0x54,0xA1,0x08,0x00,0x81,0x80,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x02,0x3A,0x80,0x18,0x71,0x38,
0x2D,0x40,0x58,0x2C,0x45,0x00,0x94,0x24,0x00,0x00,
0x00,0x1E,0x9A,0x29,0xA0,0xD0,0x51,0x84,0x22,0x30,
0x50,0x98,0x36,0x00,0xE8,0x19,0x00,0x00,0x00,0x1C,
0x21,0x39,0x90,0x30,0x62,0x1A,0x27,0x40,0x68,0xB0,
0x36,0x00,0x00,0x00,0x00,0x00,0x00,0x1E,0x00,0x00,
0x00,0xFC,0x00,0x4C,0x43,0x44,0x20,0x54,0x56,0x0A,
0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x00,0x8E,
	//Todo:
};
#else//default:1366x768
code BYTE tEDID_TABLE_VGA[VGA_EDID_SIZE] =
{
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x63,0x18,0x80,0x01,0x00,0x00,0x00,0x00,
	0x2C,0x11,0x01,0x03,0x08,0x59,0x32,0x8C,0xE8,0xE2,0xB2,0xA1,0x5B,0x4E,0x98,0x24,
	0x15,0x4F,0x4A,0xA1,0x0C,0x00,0x61,0x40,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x66,0x21,0x50,0xB0,0x51,0x00,0x1B,0x30,0x40,0x70,
	0x36,0x00,0x75,0xF2,0x31,0x00,0x00,0x1E,0x64,0x19,0x00,0x40,0x41,0x00,0x26,0x30,
	0x18,0x88,0x36,0x00,0x75,0xF2,0x31,0x00,0x00,0x18,0x00,0x00,0x00,0xFD,0x00,0x38,
	0x46,0x20,0x50,0x09,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xFC,
	0x00,0x48,0x44,0x54,0x56,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x8B,
};
#endif

#if 0//ENABLE_HDMI
#define HDMI_EDID_SIZE  256
#define HDMI_EDID_SRAM_BASE_ADDR    1
code BYTE tEDID_TABLE_HDMI[HDMI_EDID_SIZE] =
{
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x36, 0x74, 0x30, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x0A, 0x14, 0x01, 0x03, 0x80, 0x46, 0x28, 0x78, 0x0A, 0x0D, 0xC9, 0xA0, 0x57, 0x47, 0x98, 0x27,
    0x12, 0x48, 0x4C, 0x20, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
    0x45, 0x00, 0xDF, 0xA4, 0x21, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x80, 0x18, 0x71, 0x1C, 0x16, 0x20,
    0x58, 0x2C, 0x25, 0x00, 0xDF, 0xA4, 0x21, 0x00, 0x00, 0x9E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x4D,
    0x53, 0x74, 0x61, 0x72, 0x20, 0x44, 0x65, 0x6D, 0x6F, 0x0A, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFD,
    0x00, 0x3A, 0x3E, 0x0F, 0x46, 0x0F, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0xA1,
    0x02, 0x03, 0x24, 0x71, 0x4F, 0x94, 0x13, 0x05, 0x03, 0x04, 0x02, 0x01, 0x16, 0x15, 0x07, 0x06,
    0x11, 0x10, 0x12, 0x1F, 0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00, 0x67, 0x03, 0x0C, 0x00,
    0x10, 0x00, 0xA8, 0x2D, 0x01, 0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20, 0x6E, 0x28, 0x55, 0x00,
    0xDF, 0xA4, 0x21, 0x00, 0x00, 0x1E, 0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E,
    0x96, 0x00, 0xDF, 0xA4, 0x21, 0x00, 0x00, 0x18, 0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10,
    0x10, 0x3E, 0x96, 0x00, 0x30, 0xA4, 0x21, 0x00, 0x00, 0x18, 0x8C, 0x0A, 0xA0, 0x14, 0x51, 0xF0,
    0x16, 0x00, 0x26, 0x7C, 0x43, 0x00, 0x30, 0xA4, 0x21, 0x00, 0x00, 0x98, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24
 };
#if ENABLE_FREESYNC//freesync EDID FHD 40~70
code BYTE tEDID_TABLE_HDMIFreeSync[HDMI_EDID_SIZE] =
{
	0x00  ,0xFF  ,0xFF  ,0xFF  ,0xFF  ,0xFF  ,0xFF  ,0x00  ,0x05  ,0xA4  ,0x30  ,0x00  ,0x02  ,0x00  ,0x00  ,0x00,
	0x01  ,0x18  ,0x01  ,0x04  ,0x80  ,0x73  ,0x41  ,0x78  ,0x0B  ,0xCF  ,0x74  ,0xA7  ,0x55  ,0x46  ,0x98  ,0x24,
	0x10  ,0x49  ,0x4B  ,0x21  ,0x08  ,0x00  ,0x81  ,0x80  ,0x95  ,0x00  ,0x90  ,0x40  ,0xA9  ,0xC0  ,0xA9  ,0x40,
	0xB3  ,0x00  ,0x61  ,0x40  ,0x71  ,0x40  ,0x37  ,0x45  ,0x80  ,0x4A  ,0x71  ,0x38  ,0x2D  ,0x40  ,0x30  ,0x20,
	0x35  ,0x00  ,0x35  ,0xAD  ,0x10  ,0x00  ,0x00  ,0x1E  ,0x00  ,0x00  ,0x00  ,0x10  ,0x00  ,0x00  ,0x00  ,0x00,
	0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0xFC  ,0x00  ,0x46,
	0x52  ,0x45  ,0x45  ,0x53  ,0x59  ,0x4E  ,0x43  ,0x37  ,0x30  ,0x2D  ,0x34  ,0x30  ,0x00  ,0x00  ,0x00  ,0xFD,
	0x00  ,0x28  ,0x46  ,0x6B  ,0x6B  ,0x1E  ,0x01  ,0x0A  ,0x20  ,0x20  ,0x20  ,0x20  ,0x20  ,0x20  ,0x01  ,0x0C,
	0x02  ,0x03  ,0x31  ,0xF3  ,0x4D  ,0x01  ,0x03  ,0x84  ,0x05  ,0x07  ,0x10  ,0x12  ,0x93  ,0x14  ,0x16  ,0x1F,
	0x20  ,0x22  ,0x29  ,0x09  ,0x07  ,0x07  ,0x01  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x83  ,0x01  ,0x00  ,0x00,
	0x67  ,0x03  ,0x0C  ,0x00  ,0x10  ,0x00  ,0x00  ,0x44  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00,
	0x00  ,0x8C  ,0x0A  ,0xD0  ,0x8A  ,0x20  ,0xE0  ,0x2D  ,0x10  ,0x10  ,0x3E  ,0x96  ,0x00  ,0xC4  ,0x8E  ,0x21,
	0x00  ,0x00  ,0x18  ,0x02  ,0x3A  ,0x80  ,0x18  ,0x71  ,0x38  ,0x2D  ,0x40  ,0x58  ,0x2C  ,0x45  ,0x00  ,0xC4,
	0x8E  ,0x21  ,0x00  ,0x00  ,0x1E  ,0x01  ,0x1D  ,0x00  ,0xBC  ,0x52  ,0xD0  ,0x1E  ,0x20  ,0xB8  ,0x28  ,0x55,
	0x40  ,0xC4  ,0x8E  ,0x21  ,0x00  ,0x00  ,0x1E  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00,
	0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x07,
 };
#endif
#endif
static void InsertEDIDData(BYTE ucAddress, BYTE ucData)
{
    msWriteByte(REG_3E4B, ucAddress);       // REG_DDC_BANK_25[14:8]: DDC address port for CPU read/write
    msWriteByte(REG_3E4C, ucData);          // REG_DDC_BANK_26[7:0]: DDC Data Port for cpu write
    msWriteByteMask(REG_3E43, BIT5, BIT5);  // REG_DDC_BANK_21[13]: ADC sram write data pulse gen when cpu write
    while(msReadByte(REG_3E7B) & BIT5);     // REG_DDC_BANK_3D[13]: ADC_0 cpu wr busy status (wr for ADC sram)
}
void drvmStar_LoadInternalEDID_VGA( void )
{
    BYTE uctemp;
    msWriteByteMask(REG_3EEB, VGA_EDID_SRAM_BASE_ADDR, MASKBIT(4:0));   // REG_DDC_BANK_75[12:8]: Select EDID sram base address for cpu read/write
    for(uctemp = 0; uctemp < EDID_BLOCK_SIZE; uctemp++)
    {
        InsertEDIDData(uctemp, tEDID_TABLE_VGA[uctemp]);
    }
    msWriteByteMask(REG_3EFA, VGA_EDID_SRAM_BASE_ADDR, MASKBIT(4:0));   // REG_DDC_BANK_7D[4:0]: Select A0 EDID sram base address
    msWriteByteMask(REG_3E49, BIT7, BIT7);                              // DDC function Enable for ADC_0.
    msWriteByteMask(REG_3E60, BIT7, BIT7);                              //reply ACK while source accesses A0_EDID with address is over 128
    msWriteByteMask(REG_3E49, BIT5, BIT5);                              // write protect
}
#if 0//ENABLE_HDMI
void drvmStar_LoadInternalEDID_HDMI(BYTE *pEdid)
{
    BYTE uctemp;
    BYTE ucSizeCount;
    for(ucSizeCount = 0; ucSizeCount < (HDMI_EDID_SIZE/EDID_BLOCK_SIZE); ucSizeCount++)
    {
       msWriteByteMask(REG_3EEB, HDMI_EDID_SRAM_BASE_ADDR+ucSizeCount, MASKBIT(4:0));
       for(uctemp = 0; uctemp < EDID_BLOCK_SIZE; uctemp++)
       {
           InsertEDIDData(uctemp, pEdid[uctemp+(ucSizeCount*EDID_BLOCK_SIZE)]);
       }
    }
    msWriteByteMask(REG_3EEC, HDMI_EDID_SRAM_BASE_ADDR, MASKBIT(4:0));  // REG_DDC_BANK_76[4:0]: Select D0 EDID sram base address
    msWriteByteMask(REG_3E45, BIT7, BIT7);                              // DDC function Enable for DVI_0.
    msWriteByteMask(REG_3E45, BIT5, BIT5);                              // write protect
    DDC_Port_Sel();
}
#if ENABLE_HDMI && ENABLE_FREESYNC
 void drvmStar_EnableHDMIFreeSyncEDID(Bool Enable)
{
    BYTE ucFlag=0;
    ucFlag = Enable;
	if (ucFlag)
    {	
		drvmStar_LoadInternalEDID_HDMI(tEDID_TABLE_HDMIFreeSync);
    }
	else
    {
		drvmStar_LoadInternalEDID_HDMI(tEDID_TABLE_HDMI);
    }
}
#endif
#endif
#endif
void drvmStar_Init( void )
{
    BYTE i;

#if ENABLE_SW_DOUBLE_BUFFER
    msSWDBInit();
#endif

#if CHIP_ID == CHIP_TSUM2 //U02 need to Set this bit to let MCU could restart after reset.
    msWriteByteMask(REG_0180,BIT0,BIT0); //when external 1.2V is lower than 0.8V, Scalar will enter into reset status. let 1.2V back to normal level , MCU should restart again.
#endif
#if ENABLE_CABLE_5V_EDID                    // support cable 5V read EDID
		drvmStar_LoadInternalEDID_VGA();
#if 0//ENABLE_HDMI
		drvmStar_LoadInternalEDID_HDMI(tEDID_TABLE_HDMI);
#endif
#endif

    if (!msPM_StartRCOSCCal()) // calibration here for other IP to use FRO_12M
        drvmStar_printData("CALIBARYION RCOSC_12M FAIL!",0);
    else
        drvmStar_printData("CALIBARYION RCOSC_12M Success!",0);

    msInitClockGating();

    msTMDSInit();

    msWriteByteMask(SC0_02, BIT7, BIT7);

    for( i = 0; i < sizeof( tblInit ) / sizeof( RegUnitType ); i++ )
        msWriteByte( tblInit[i].u16Reg, tblInit[i].u8Value );
    for( i = 0; i < sizeof( tb_3D_DITHER_TABLE ) / sizeof( RegUnitType ); i++ )
        msWriteByte( tb_3D_DITHER_TABLE[i].u16Reg, tb_3D_DITHER_TABLE[i].u8Value );

#if  ENABLE_TNR || ENABLE_RTE
    msInitMemory();
#endif

    mcuInitXdataMapToDRAM();
    mcuXdataMapToDRAM(HK_XDATA_ADDR);

  #if (ENABLE_HK_CODE_ON_DRAM)
    mcuDMADownloadCode(DMA_TO_DRAM_SOURCE_ADDR, DMA_TO_DRAM_DESTIN_ADDR, DMA_TO_DRAM_BYTE_COUNT);
    mcuArrangeCodeAddr(_DRAM, MCU_ON_DRAM_START_ADDR , MCU_ON_DRAM_END_ADDR);
    mcuArrangeCodeAddr(_SPI, MCU_ON_SPI_START_ADDR, MCU_ON_SPI_END_ADDR);
  #endif


    //msWriteByteMask(SC0_0E, BIT7, BIT7); // H coring disable, for dot pattern certical block issue, Jison 100610
    mStar_SetupPathInit(); //Jison need put here because mStar_SetupFreeRunMode() will use g_SetupPathInfo variable

#if ENABLE_R2_2DTO3D
    if(UserPref2Dto3D)
        Sc2DTo3DInit();
#elif ENABLE_USB_INPUT
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

    msWriteByteMask(REG_06AE,0x00,0x07);
    msWriteByteMask(REG_07AE,0x00,0x07);
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

static void msSetupDelayLine(void)
{
    BYTE xdata u8VSyncTime=GetVSyncTime();

    WORD xdata Hi_Act=GetImageWidth();   //  OK
    WORD xdata Vi_Act=GetImageHeight();  //  OK
    WORD xdata Hi_Total=mSTar_GetInputHTotal();   //  OK
    WORD xdata Vi_Total=SC0_READ_VTOTAL();           //  OK
    WORD xdata Hi_Blanking=Hi_Total-Hi_Act;   //  OK

    WORD xdata Ho_Act=PanelWidth;
    WORD xdata Vo_Act=PanelHeight;
    WORD xdata Ho_Total=PanelMinHTotal;
    WORD xdata Vo_Total=( (DWORD)Vo_Act*Vi_Total/Vi_Act);

#if SC_V_SC_NON_MINUS1MODE
    WORD xdata u16FV=(DWORD)Vi_Act*1000/Vo_Act;
#else
    WORD xdata u16FV=((((DWORD)Vi_Act-1)*1000)/(Vo_Act-1));
#endif

    WORD xdata u16Ldly_mid = (6200-(DWORD)u16FV*Ho_Act/Ho_Total-((DWORD)Hi_Blanking*1000)/Ho_Total)/2;
    drvmStar_printData(" 11111111 u16Ldly_mid ==%d",u16Ldly_mid);
    msWriteByteMask(SC0_90, (u16Ldly_mid/1000) <<4 ,BIT6|BIT5|BIT4);
#if ENABLE_FREESYNC
    if(IS_DP_FREESYNC())//freesync
    {
        BYTE u8DlyLine = u16Ldly_mid/1000;
        BYTE u8SampleRangeVStartOffset = ((msRead2Byte(SC0_05)) + ((u8DlyLine < 4)?(4 - u8DlyLine):0));

        msWriteByteMask(SC0_0F, 0x02, 0x0F); //Ylock line
        msWrite2Byte(SC0_05, u8SampleRangeVStartOffset); // set capture window vstart

    }
    else
    {
        msWriteByteMask(SC0_0F, (SC_LOCK_LINE), 0x0F); //Ylock line
    }
#endif
    u16Ldly_mid%=1000;
    msWriteByteMask(SC0_0C,  (u16Ldly_mid/=250)<<5 ,BIT6|BIT5);
    drvmStar_printData("222222222 u16Ldly_mid=%d",u16Ldly_mid);

#if 1
    drvmStar_printData(" == Hi_Act ==%d",Hi_Act);
    drvmStar_printData(" == Vi_Act ==%d",Vi_Act);
    drvmStar_printData(" == Hi_Total ==%d",Hi_Total);
    drvmStar_printData(" == Vi_Total ==%d",Vi_Total);
    drvmStar_printData(" == Hi_Blanking ==%d",Hi_Blanking);
    drvmStar_printData(" == Ho_Act ==%d",Ho_Act);
    drvmStar_printData(" == Vo_Act ==%d",Vo_Act);
    drvmStar_printData(" == Ho_Total ==%d",Ho_Total);
    drvmStar_printData(" == Vo_Total ==%d",Vo_Total);
    drvmStar_printData(" == u16Ldly_mid ==%d",u16Ldly_mid);
    drvmStar_printData(" == u16FV ==%d",u16FV);
#endif
    //-------------------------------------------------------------
    //SCD_64[1]:"flag 1 once afifo can't receive pixels when IP sends.0: don't happen1: occur"
    //SCD_65[1]:"flag 1 once afifo can't give pixels when OP needs.0: don't happen1: occur"
    //-------------------------------------------------------------
    msWriteByteMask(SCD_64, BIT2 ,BIT2);
    msWriteByteMask(SCD_65, BIT2 ,BIT2);
    Delay1ms(1);
    msWriteByteMask(SCD_64, 0x00 ,BIT2);
    msWriteByteMask(SCD_65, 0x00 ,BIT2);
    Delay1ms(u8VSyncTime);
    if ((msReadByte(SCD_64)&BIT1) || (msReadByte(SCD_65)&BIT1))
        drvmStar_printMsg("DelayLine setting checked: FAIL");
    else
        drvmStar_printMsg("DelayLine setting checked: PASS");
}



void mStar_InterlaceModeSetting( void )
{
    msSetupDelayLine();

    if( CURRENT_SOURCE_IS_INTERLACE_MODE() )
    {
        msWriteByte(SC0_6F,0x00);
        //msWriteByte(SC0_90, (ucDelayLine)<<4);
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
        #if ENABLE_DP_INPUT
                 if(CURRENT_INPUT_IS_DISPLAYPORT())      // For DP interlace 20121025
                 {
                         if (DPRxGetVB_ID()&BIT2) //Jison 090526
                        {
                            msWriteByteMask(SC0_E9,0,BIT0);
                        }
                        else
                        {
                                #if DP_ASTRO_INTERLACE_PATCH
                                if (msReadByte(SC0_E8)&(_BIT1|_BIT0)) //Mike //Chroma 2233 Patch
                                msWriteByteMask(SC0_E9,0,BIT0);
                                else
                                #endif
                                msWriteByteMask(SC0_05,BIT0,BIT0);
                        }
                }
         #endif
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
        //msWriteByte(SC0_90, (ucDelayLine<<4));
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

void ComboClockRtermControl(BYTE ucStatus)
{
    if( ucStatus == COMBO_INPUT_OFF )
    {
        msWriteByteMask(REG_01CE, BIT2 | BIT1 |BIT0, BIT2 | BIT1 | BIT0); // Turn off Clock R-term
    }
    else //if( ucStatus == COMBO_INPUT_DIGITAL || ucStatus == COMBO_INPUT_POWERSAVING || ucStatus == COMBO_INPUT_ANALOG)
    {
        msWriteByteMask(REG_01CE, 0, BIT2 | BIT1 | BIT0); // Turn on Clock R-term
    }
}
void ComboDataRtermControl(BYTE  ucStatus)
{

#if(ENABLE_MHL)
    if( ucStatus == COMBO_INPUT_OFF || ucStatus == COMBO_INPUT_ANALOG || ucStatus == COMBO_INPUT_POWERSAVING)
    {
        if(!MHL_CABLE_DETECT_PORT0())
            msWriteByteMask(REG_01C4, 0x0F, 0x0F); // Turn off Port 0 Date R-term
        if(!MHL_CABLE_DETECT_PORT1())
            msWriteByteMask(REG_01C4, 0xF0, 0xF0); // Turn off Port 1 Date R-term
        if(!MHL_CABLE_DETECT_PORT2())
            msWriteByteMask(REG_01C5, 0x0F, 0x0F); // Turn off Port 2 Date R-term
    }
    else //if( ucStatus == COMBO_INPUT_DIGITAL || ucStatus == COMBO_INPUT_POWERSAVING )
    {
        if(!MHL_CABLE_DETECT_PORT0())
            msWriteByteMask(REG_01C4, 0x00, 0x0F); // Turn on Port 0 Date R-term
        if(!MHL_CABLE_DETECT_PORT1())
            msWriteByteMask(REG_01C4, 0x00, 0xF0); // Turn on Port 1 Date R-term
        if(!MHL_CABLE_DETECT_PORT2())
            msWriteByteMask(REG_01C5, 0x00, 0x0F); // Turn on Port 2 Date R-term
    }
#else
    if( ucStatus == COMBO_INPUT_OFF || ucStatus == COMBO_INPUT_ANALOG || ucStatus == COMBO_INPUT_POWERSAVING)
    {
        msWriteByteMask(REG_01C4, 0x0F, 0x0F); // Turn off Port 0 Date R-term
        msWriteByteMask(REG_01C4, 0xF0, 0xF0); // Turn off Port 1 Date R-term
        msWriteByteMask(REG_01C5, 0x0F, 0x0F); // Turn off Port 2 Date R-term
    }
    else //if( ucStatus == COMBO_INPUT_DIGITAL || ucStatus == COMBO_INPUT_POWERSAVING)
    {
        msWriteByteMask(REG_01C4, 0x00, 0x0F); // Turn on Port 0 Date R-term
        msWriteByteMask(REG_01C4, 0x00, 0xF0); // Turn on Port 1 Date R-term
        msWriteByteMask(REG_01C5, 0x00, 0x0F); // Turn on Port 2 Date R-term
    }
#endif

}
void ComboPortMuxControl(BYTE  ucStatus)
{
#if(ENABLE_MHL)
    if( ucStatus == COMBO_INPUT_OFF || ucStatus == COMBO_INPUT_ANALOG )
    {
        if(!MHL_CABLE_DETECT_PORT0())
            msWriteByteMask(REG_01C2, 0x00, BIT5); // Turn off Port 0 Port Mux
        if(!MHL_CABLE_DETECT_PORT1())
            msWriteByteMask(REG_01C2, 0x00, BIT6); // Turn off Port 1 Port Mux
        if(!MHL_CABLE_DETECT_PORT2())
            msWriteByteMask(REG_01C2, 0x00, BIT7); // Turn off Port 2 Port Mux
    }
    else if( ucStatus == COMBO_INPUT_DIGITAL)
    {
        msWriteByteMask(REG_01C2, 0x00, BIT5 | BIT6 | BIT7);    // Turn off (Port 0 Port 1 Port 2) Port Mux

        if(SrcInputType==Input_HDMI || SrcInputType==Input_DVI)
        {
            if(!MHL_CABLE_DETECT_PORT0())
                msWriteByteMask(REG_01C2, BIT5, BIT5);  // Turn on Port 0 Port Mux
        }
        else if(SrcInputType==Input_HDMI2 || SrcInputType==Input_DVI2 || SrcInputType==Input_Displayport)
        {
            if(!MHL_CABLE_DETECT_PORT1())
                msWriteByteMask(REG_01C2, BIT6, BIT6);  // Turn on Port 1 Port Mux
        }
        else //if(SrcInputType==Input_HDMI3 || SrcInputType==Input_DVI3 || SrcInputType==Input_Displayport3)
        {
            if(!MHL_CABLE_DETECT_PORT2())
                msWriteByteMask(REG_01C2, BIT7, BIT7);  // Turn on Port 2 Port Mux
        }
    }
    else if(ucStatus == COMBO_INPUT_DIGITAL_A)
    {
        if(!MHL_CABLE_DETECT_PORT0())
            msWriteByteMask(REG_01C2, BIT5, BIT5 | BIT6 | BIT7);  // Turn on Port 0 Port Mux
    }
    else if(ucStatus == COMBO_INPUT_DIGITAL_B)
    {
        if(!MHL_CABLE_DETECT_PORT1())
            msWriteByteMask(REG_01C2, BIT6, BIT5 | BIT6 | BIT7);  // Turn on Port 1 Port Mux
    }
    else if(ucStatus == COMBO_INPUT_DIGITAL_C)
    {
        if(!MHL_CABLE_DETECT_PORT2())
            msWriteByteMask(REG_01C2, BIT7, BIT5 | BIT6 | BIT7);  // Turn on Port 2 Port Mux
    }
    else // if(ucStatus == COMBO_INPUT_POWERSAVING)
    {
        if(!MHL_CABLE_DETECT_PORT0())
            msWriteByteMask(REG_01C2, BIT5, BIT5); // Turn on Port 0 Port Mux
        if(!MHL_CABLE_DETECT_PORT1())
            msWriteByteMask(REG_01C2, BIT6, BIT6); // Turn on Port 1 Port Mux
        if(!MHL_CABLE_DETECT_PORT2())
            msWriteByteMask(REG_01C2, BIT7, BIT7); // Turn on Port 2 Port Mux
    }
#else
    if( ucStatus == COMBO_INPUT_OFF)
    {
        msWriteByteMask(REG_01C2, 0x00, BIT5 | BIT6 | BIT7); // (Turn off Port Mux 0~2) BIT 5~7 = Port 0~2
    }
    else if(ucStatus == COMBO_INPUT_ANALOG)
    {
        msWriteByteMask(REG_01C2, 0x00, BIT5 | BIT6 | BIT7); // (Turn off Port Mux 0~2) BIT 5~7 = Port 0~2
    }
    else if( ucStatus == COMBO_INPUT_DIGITAL)
    {
        msWriteByteMask(REG_01C2, 0x00, BIT5 | BIT6 | BIT7); // (Turn off Port Mux 0~2) BIT 5~7 = Port 0~2

        if(SrcInputType==Input_HDMI || SrcInputType==Input_DVI)
        {
            msWriteByteMask(REG_01C2, BIT5, BIT5);  // Turn on Port 0 Port Mux
        }
        else if(SrcInputType==Input_HDMI2 || SrcInputType==Input_DVI2 || SrcInputType==Input_Displayport)
        {
            msWriteByteMask(REG_01C2, BIT6, BIT6);   // Turn on Port 1 Port Mux
        }
        else //if(SrcInputType==Input_HDMI3 || SrcInputType==Input_DVI3 || SrcInputType==Input_Displayport3)
        {
            msWriteByteMask(REG_01C2, BIT7, BIT7);   // Turn on Port 2 Port Mux
        }
    }
    else if(ucStatus == COMBO_INPUT_DIGITAL_A)
    {
        msWriteByteMask(REG_01C2, BIT5, BIT5 | BIT6 | BIT7);  // Turn on Port 0 Port Mux
    }
    else if(ucStatus == COMBO_INPUT_DIGITAL_B)
    {
        msWriteByteMask(REG_01C2, BIT6, BIT5 | BIT6 | BIT7);  // Turn on Port 1 Port Mux
    }
    else if(ucStatus == COMBO_INPUT_DIGITAL_C)
    {
        msWriteByteMask(REG_01C2, BIT7, BIT5 | BIT6 | BIT7);  // Turn on Port 2 Port Mux
    }
    else //if(ucStatus == COMBO_INPUT_POWERSAVING)
    {
       // msWriteByteMask(REG_01C2, BIT5 | BIT6 | BIT7, BIT5 | BIT6 | BIT7); // (Turn on Port Mux 0~2) BIT 5~7 = Port 0~2
    }
#endif

}
void ComboDeMuxControl(BYTE  ucStatus)
{
    if( ucStatus == COMBO_INPUT_OFF)
    {
#if(ENABLE_MHL)
        if(!(MHL_CABLE_DETECT_PORT0() || MHL_CABLE_DETECT_PORT1() || MHL_CABLE_DETECT_PORT2()))
            msWriteByteMask(REG_01C5, 0xF0, 0xF0); // Turn off DeMux
#else
        msWriteByteMask(REG_01C5, 0xF0, 0xF0); // Turn off DeMux
#endif
    }
    else
    {
        msWriteByteMask(REG_01C5, 0x00, 0xF0); // Turn on DeMux
    }
}

void ComboInputControl(ComboInputType ctrl)
{
    ComboClockRtermControl(ctrl);
    ComboDataRtermControl(ctrl);
    ComboPortMuxControl(ctrl);
    ComboDeMuxControl(ctrl);

/*
    if(ucStatus == COMBO_INPUT_OFF)
        printMsg("ucStatus == COMBO_INPUT_OFF");
    else if(ucStatus == COMBO_INPUT_POWERSAVING)
        printMsg("ucStatus == COMBO_INPUT_POWERSAVING");
    else if(ucStatus == COMBO_INPUT_ANALOG)
        printMsg("ucStatus == COMBO_INPUT_ANALOG");
    else //if(ucStatus == COMBO_INPUT_DIGITAL)
        printMsg("ucStatus == COMBO_INPUT_DIGITAL");
*/
}


// 111220 coding test
void drvDVI_PowerCtrl(DVI_PowerCtrlType ctrl)
{
    if(ctrl == DVI_POWER_STANDBY)
    {
        // DVI power on
        // msWriteByteMask(REG_01C3, 0, BIT4); // power on DVI CKIN.
        msWriteByteMask(REG_01CE, 0, BIT4); // power on PLL band-gap.
        // Power up Xtal clocks and DVI detection clock for DVI clock detection
        msWrite2ByteMask(REG_174C, 0x0001, 0xFFFF); // enable main link clock
        msWrite2ByteMask(REG_174E, 0x1000, 0x3FFF); // enable Xtal

        msWrite2ByteMask(REG_01C2, 0xCE08, 0xFF08);
        msWrite2ByteMask(REG_01C6, 0xFFFF, 0xFFFF);
    }
    else if(ctrl == DVI_POWER_DOWN)
    {
        // DVI power down
        // msWriteByteMask(REG_01C3, BIT4, BIT4); // power down DVI CKIN.
        msWriteByteMask(REG_01CE, BIT4, BIT4); // power down PLL band-gap.
        // Power up Xtal clocks and DVI detection clock for DVI clock detection
        msWrite2ByteMask(REG_174C, 0x0000, 0xFFFF);
        msWrite2ByteMask(REG_174E, 0x0000, 0x3FFF);

        msWrite2ByteMask(REG_01C2, 0xFF08, 0xFF08);
        msWrite2ByteMask(REG_01C6, 0xFFFF, 0xFFFF);

    }
    else
    {
        // DVI power on
        //msWriteByteMask(REG_01C3, 0, BIT4); // power on DVI CKIN.
        msWriteByteMask(REG_01CE, 0, BIT4); // power on PLL band-gap
        msWrite2ByteMask(REG_174C, 0xFFFF, 0xFFFF);
        msWrite2ByteMask(REG_174E, 0x3FFF, 0x3FFF);

        msWrite2ByteMask(REG_01C2, 0x0000, 0xFF08);
        msWrite2ByteMask(REG_01C6, 0x0000, 0xFFFF);
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
     else if(CURRENT_INPUT_IS_DISPLAYPORT())
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
#if ENABLE_AUTOEQ
    AUTOEQ_FLAG = 0;
#endif

    if(INPUT_IS_DVI(inport))//( inport == Input_Digital )
    {
        BYTE i;
        //msWriteByteMask(REG_01C3, 0, BIT4); // power on DVI CKIN.
        msWriteByteMask(REG_01CE, 0, BIT4); // power on PLL band-gap.
        // Power up Xtal clocks and DVI detection clock for DVI clock detection
        msWrite2ByteMask(REG_174C, 0xFFFF, 0xFFFF);
        msWrite2ByteMask(REG_174E, 0x3FFF, 0x3FFF);
        // DVI PLL power control
        msWrite2ByteMask(REG_01C2, 0x4000, 0x4000); // power off DVI PLL power
        msWriteByteMask( REG_1740, BIT0, BIT0 ); //HDMI select.
        msWriteByteMask( REG_17A5, BIT0, BIT1|BIT0 ); //DCDR demux ratio select.
        //msWriteByteMask( REG_17A5, BIT4, BIT5|BIT4 ); //EQ mode setting.
        msWriteByteMask( REG_17A5, BIT5|BIT4, BIT5|BIT4 ); //EQ mode setting.
        msWriteByteMask( REG_17AC, 0x01, 0x01); //ICtrl overwrite
        msWriteByteMask( REG_17AA, 0x40, 0xF0); //ICtrl value

        for(i=0;i<sizeof(tComboTMDSMuxTbl)/sizeof(RegTbl2Type);i++)
            msWrite2ByteMask(tComboTMDSMuxTbl[i].wReg,tComboTMDSMuxTbl[i].wValue,tComboTMDSMuxTbl[i].wMask );
#if ENABLE_AUTOEQ
        drv_TMDS_AUTOEQ_initial();
#endif
        //Port Select
        if(SrcInputType == Input_DVI)
        {
            msWriteByteMask( REG_1501, BIT4, BIT5|BIT4 );
        }
        else if(SrcInputType == Input_DVI2)
        {
            msWriteByteMask( REG_1501, 0, BIT5|BIT4 );
        }
        else if(SrcInputType == Input_DVI3)
        {
            msWriteByteMask( REG_1501, BIT5, BIT5|BIT4 );
        }
        msEread_SetHDMIInitialValue();
    }
  #if ENABLE_HDMI
    else if( INPUT_IS_HDMI(inport) ) // HDMI - Port B
    {
        BYTE i;
        //msWriteByteMask(REG_01C3, 0, BIT4); // power on DVI CKIN.
        msWriteByteMask(REG_01CE, 0, BIT4); // power on PLL band-gap.
        // Power up Xtal clocks and DVI detection clock for DVI clock detection
        msWrite2ByteMask(REG_174C, 0xFFFF, 0xFFFF);
        msWrite2ByteMask(REG_174E, 0x3FFF, 0x3FFF);
        // DVI PLL power control
        msWrite2ByteMask(REG_01C2, 0x4000, 0x4000); // power off DVI PLL power
        msWriteByteMask( REG_1740, BIT0, BIT0 ); //HDMI select.
        msWriteByteMask( REG_17A5, BIT0, BIT1|BIT0 ); //DCDR demux ratio select.
        //msWriteByteMask( REG_17A5, BIT4, BIT5|BIT4 ); //EQ mode setting.
        msWriteByteMask( REG_17A5, BIT5|BIT4, BIT5|BIT4 ); //EQ mode setting.
        msWriteByteMask( REG_17AC, 0x01, 0x01); //ICtrl overwrite
        msWriteByteMask( REG_17AA, 0x40, 0xF0); //ICtrl value
        msWriteByteMask( REG_1427, 0, BIT0 ); //audio source selection

        for(i=0;i<sizeof(tComboTMDSMuxTbl)/sizeof(RegTbl2Type);i++)
            msWrite2ByteMask(tComboTMDSMuxTbl[i].wReg,tComboTMDSMuxTbl[i].wValue,tComboTMDSMuxTbl[i].wMask );
#if ENABLE_AUTOEQ
        drv_TMDS_AUTOEQ_initial();
#endif
        //Port Select
        if(SrcInputType == Input_HDMI)
        {
            msWriteByteMask( REG_1501, BIT4, BIT5|BIT4 );
        }
        else if(SrcInputType == Input_HDMI2)
        {
            msWriteByteMask( REG_1501, 0, BIT5|BIT4 );
        }
        else if(SrcInputType == Input_HDMI3)
        {
            msWriteByteMask( REG_1501, BIT5, BIT5|BIT4 );
        }
        msEread_SetHDMIInitialValue();
    }
  #endif // #if ENABLE_HDMI_INPUT
    else // non-DVI/HDMI ports
    {
        //msWriteByteMask(REG_01C3, BIT4, BIT4); // power down DVI CKIN.
        msWriteByteMask(REG_01CE, BIT4, BIT4); // power down PLL band-gap.
        // Power up Xtal clocks and DVI detection clock for DVI clock detection
        msWrite2ByteMask(REG_174C, 0x0001, 0xFFFF); // enable main link clock
        msWrite2ByteMask(REG_174E, 0x2000, 0x3FFF); // enable Xtal
        msWriteByteMask( REG_17AC, 0x00, 0x01); //ICtrl overwrite
        msWriteByteMask( REG_17AA, 0x00, 0xF0); //ICtrl value
        // DVI PLL power control
        msWrite2ByteMask(REG_01C2, 0xCE08, 0xFF08);
        msWrite2ByteMask(REG_01C6, 0xFFFF, 0xFFFF);

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
    msWriteByteMask(REG_1E35, 0, _BIT3|_BIT2); // fix maximum fclk
}


//Jison 091023 Do I_Gen_Tuning at turn on panel 1st time. 100305 alway enable

#if CHIP_ID==CHIP_TSUMK
void I_Gen_Tuning( void )
{
	WORD XDATA u16Target=LVDS_SWING_TARGET;
	BYTE XDATA u8icon=LVDS_FAIL_TARGET; //default value = 300mV
	BYTE XDATA  clockswing;
	
	if (u16Target<=50)
		u16Target=LVDS_SWING_TARGET;

	u8icon=(u16Target-50)/10;
	
#ifdef ENABLE_UP_LOW_BOUNDARY_PROTECTION
        if (u8icon >ICON_Upper_Boundary || u8icon<ICON_Lower_Boundary)
        {
                u8icon = LVDS_FAIL_TARGET;
                drvmStar_printMsg("use Default Icon setting because setting value is not in reasonable range  !!");
        }
#endif

#if DOUBLE_LVDS_CLK_SWING
        clockswing = u8icon*2+LVDS_CLK_1TO2_OFFSET;
        if(clockswing > 0x3F)
            clockswing = 0x3F;
#else
        clockswing = u8icon;
#endif

    // write calibration value for each ch
    msWrite2Byte(REG_3052, ((WORD)u8icon<<10)|(u8icon<<4)); // ch0~ch1
    msWrite2Byte(REG_3054, ((WORD)u8icon<<12)|(clockswing<<6)|(u8icon)); // ch2~ch4
    msWrite2Byte(REG_3056, ((WORD)u8icon<<14)|(u8icon<<8)|(u8icon<<2)|(u8icon>>4)); // ch4~ch7
    msWrite2Byte(REG_3058, ((WORD)u8icon<<10)|(clockswing<<4)|(u8icon>>2));// ch7~ch9
    iGenTuningFinished=1;
}

#else
#if( USE_MOD_HW_CAL )
// MOD hardware current calibration
// @pre MOD is not in power-down (BK1_26[7]==0)
// @pre LPLL is not in power-down
void I_Gen_Tuning( void )
{
#ifndef FPGA
    // 0.backup reg
    BYTE XDATA  i;
    BYTE XDATA  reg_30E2   = msReadByte(REG_30E2);
    WORD XDATA  reg_3046   = msRead2Byte(REG_3046);
    WORD XDATA  reg_3048   = msRead2Byte(REG_3048);
    BYTE XDATA  reg_3080   = msReadByte(REG_3080);
    BYTE XDATA  reg_308B   = msReadByte(REG_308B);
    WORD XDATA  reg_306A   = msRead2Byte(REG_306A); // ch double swing
    BYTE XDATA  reg_30DA   = msReadByte(REG_30DA);
    BYTE XDATA  reg_30DB   = msReadByte(REG_30DB);
    BYTE XDATA  reg_30DC   = msReadByte(REG_30DC);
    BYTE XDATA  clockswing;

    drvmStar_printMsg("HW I_Gen_Tuning_Start");

#if 1
    msWrite2ByteMask(REG_3076,0x003C        ,0xFFFF);  //5us
    msWriteByte(REG_30DA, 0xFF);
    msWriteByte(REG_30DB, 0xFF);
    msWriteByte(REG_30DC, 0xFF);
#endif

    // 1.Disable double differential output swing
    msWrite2Byte(REG_306A,   0x00);
    // 2.Select calibration source.
#if(I_GEN_CH==CH2)
    msWriteByteMask(REG_30FA,   0x00,  BIT3|BIT2); // Ch2
    msWrite2ByteMask(REG_3046,  0x00,  BIT8| BIT7|BIT6);
    msWriteByteMask(REG_30E2,   0x00,  BIT2);
#else
    msWriteByteMask(REG_30FA,   BIT2,  BIT3|BIT2); // Ch6
    msWrite2ByteMask(REG_3048,  0x00,  BIT5| BIT4|BIT3);
    msWriteByteMask(REG_30E2,   0x00,  BIT6);
#endif

    // 3.Enable LVDS Test Mode, and make output data=High for LVDS output.
    //Note:
    //Please disable MOD Differential output polarity swap. (BK36_E6[7:0]=8'h00, BK36_E7[1:0]=2'h0)
    msWriteByteMask(REG_3080,   BIT1,  BIT1);
    msWriteByteMask(REG_308B,   0x00,  BIT1|BIT0);
    // 4.Set LVDS voltage target.
    msWriteByteMask(REG_30FA,   LVDS_VOLTAGE_TARGET,  BIT1|BIT0);
    // 5.Enable Hardware Calibration
    msWriteByteMask(REG_30FA,   BIT7,  BIT7);
    msWriteByteMask(REG_307B,   BIT7,  BIT7);
    // 6.Wait until BK36_FCh[7] = 1. Hardware Calibration ready.
    SetTimOutConter(50);
    //u16TimeOutCounter = 50;
    //bTimeOutCounterFlag=1;
    while (! ( msReadByte( REG_307B ) & BIT6 ) && bTimeOutCounterFlag );

    // 7.If REG_307B[5] = 1, calibration failed.
    if(msReadByte( REG_307B ) & BIT5 )
        {
            i = LVDS_FAIL_TARGET;
            drvmStar_printMsg(" @@@@@@@@@                       HW MOD Cali Fail !!");
        }
    else
    {
            i = msReadByte(REG_307A)&0x3F;
#ifdef ENABLE_UP_LOW_BOUNDARY_PROTECTION
        if (i >ICON_Upper_Boundary || i<ICON_Lower_Boundary)
        {
                i = LVDS_FAIL_TARGET;
                drvmStar_printMsg("HW MOD Cali Fail due to not located in normal range !!");
        }
        else
#endif
        {
                drvmStar_printMsg("HW MOD Cali Pass !!");
                drvmStar_printData(" @@@@@@@@@                        Cali result=0x%x", msReadByte(REG_307A));
        }
    }

#if DOUBLE_LVDS_CLK_SWING
        clockswing = i*2+LVDS_CLK_1TO2_OFFSET;
        if(clockswing > 0x3F)
            clockswing = 0x3F;
#else
        clockswing = i;
#endif

    // write calibration value for each ch
    msWrite2Byte(REG_3052, ((WORD)i<<10)|(i<<4)); // ch0~ch1
    msWrite2Byte(REG_3054, ((WORD)i<<12)|(clockswing<<6)|(i)); // ch2~ch4
    msWrite2Byte(REG_3056, ((WORD)i<<14)|(i<<8)|(i<<2)|(i>>4)); // ch4~ch7
    msWrite2Byte(REG_3058, ((WORD)i<<10)|(clockswing<<4)|(i>>2));// ch7~ch9

    // 8.Disable Hardware Calibration.
    msWriteByteMask(REG_30FA,   0x00,  BIT7);
    msWriteByteMask(REG_307B,   0x00,  BIT7);
    //9.Set register "BK36_E2[2], BK36_E2[7], BK36_46[1:0], BK36_80[1], BK36_8B[1:0]" back to the original normal function.
    msWriteByte(REG_30E2, reg_30E2);
    msWrite2Byte(REG_3046, reg_3046);
    msWrite2Byte(REG_3048, reg_3048);
    msWriteByte(REG_3080, reg_3080);
    msWriteByte(REG_308B, reg_308B);
    msWrite2Byte(REG_306A, reg_306A);

#if 1
    msWriteByte(REG_30DA, reg_30DA);
    msWriteByte(REG_30DB, reg_30DB);
    msWriteByte(REG_30DC, reg_30DC);
#endif
    drvmStar_printMsg(" @@@@@@@@@                       HW I_Gen_Tuning_End");
#endif
    iGenTuningFinished=1;
}


#else //SW
// @brief MOD Current Calibration
// @pre   Current bank is bank-0.
// @pre   LPLL is on, i.e., ASSERT( BK0_D1[4]==0 )
void I_Gen_Tuning( void )
{
    BYTE i=0;
    // 0.backup reg
    volatile BYTE reg_30E2   = msReadByte(REG_30E2);
    volatile WORD reg_3046   = msRead2Byte(REG_3046);
    volatile WORD reg_3048   = msRead2Byte(REG_3048);
    volatile BYTE reg_3080   = msReadByte(REG_3080);
    volatile BYTE reg_308A   = msReadByte(REG_308A);
    volatile BYTE reg_308B   = msReadByte(REG_308B);
    volatile WORD reg_306A   = msRead2Byte(REG_306A); // ch double swing
    volatile BYTE reg_3065   = msReadByte(REG_3065);
    BYTE XDATA   clockswing;

    #define SWING_MASK  0x3F
    drvmStar_printMsg("FW I_Gen_Tuning_Start");

    msWriteByteMask(REG_3079, 0, BIT7); // Auto update Cal result to control swing reg in HW mode, SW mode needs set to 0 (for FT use)

    // 1.Disable power down mod
    msWriteByteMask(REG_30F0,   0x00, BIT0);
    // ((// 2.Enable MOD self current bias.))
    //msWriteByteMask(REG_30FA,   BIT6, BIT6);
    // 3.Set output bias current.
    //msWriteByteMask(REG_30FA,   0x00, 0x3F);
    // 4.Disable double differential output swing
    msWrite2Byte(REG_306A,   0x00);
    // 5.select calibration source.
    //Note:
    //if BK36_FBh[1]=0, set BK36_DAh[5:4]=2'b01, BK36_46[1:0] = 2'b00, BK_36_E2[2] = 1'b0, and BK36_EEh[3:0]=4'hF to make PADA_OUT[PN]_CH[2] as LVDS output.
    //if BK36_FBh[1]=1, set BK36_DBh[7:6]=2'b01, BK36_46[1:0] = 2'b00, BK_36_E2[7] = 1'b0, and BK36_EEh[3:0]=4'hF to make PADA_OUT[PN]_CH[7] as LVDS output.
    #if(I_GEN_CH==CH2)
    msWriteByteMask(REG_3054,   0x00, 0x3F); // ch2 control swing
    msWriteByteMask(REG_30FA,   0x00, BIT3|BIT2);
    msWrite2ByteMask(REG_3046,  0x00, BIT8| BIT7|BIT6);
    msWriteByteMask(REG_30E2,   0x00, BIT2);
    msWriteByteMask(REG_30EE,   0x07, 0x07);
    #else
    msWriteByteMask(REG_3057,   0x00, 0x3F); // ch6 control swing
    msWriteByteMask(REG_30FA,   BIT2, BIT3|BIT2);
    msWrite2ByteMask(REG_3048,  0, BIT5| BIT4|BIT3);
    msWriteByteMask(REG_30E2,   0, BIT6);
    msWriteByteMask(REG_30EE,   0x07, 0x07);
    #endif
    // 6.Enable LVDS Test Mode, and make output data=High for LVDS output.
    msWriteByteMask(REG_3065,   BIT7, BIT7);
    msWriteByteMask(REG_3080,   BIT1, BIT1);
    msWriteByteMask(REG_308A,   BIT6, BIT6);
    msWriteByteMask(REG_308B,   0x00, BIT1|BIT0);
    msWriteByteMask(REG_3082,   0xFF, 0xFF);
    // 7.Enable MOD Calibration.
    msWriteByteMask(REG_30FA,   BIT7, BIT7);
    // 8.Used to set LVDS voltage target.
    msWriteByteMask(REG_30FA,   LVDS_VOLTAGE_TARGET,  BIT1|BIT0);
    //9.Adjust the value of BK36_FAh[5:0] step by step to control output swing and check whether BK36_FBh[0] is equal to "1" or "0".
    for (i=0; i<=SWING_MASK; i++)
    {
        #if(I_GEN_CH==CH2)
        msWriteByteMask(REG_3054, i, 0x3F); // ch2 control swing
        #else
        msWriteByteMask(REG_3057, i, 0x3F); // ch6 control swing
        #endif

        ForceDelay1ms(10);
        //10.If BK36_FBh[0] is toggling between "1" and "0", terminal the fine-tune procedure.
        if (msReadByte(REG_30FB)&BIT0 )
        {

            if ( i==0 ||  i==SWING_MASK)
            {
                i = LVDS_FAIL_TARGET;
#if DOUBLE_LVDS_CLK_SWING
                clockswing = i*2+LVDS_CLK_1TO2_OFFSET;
                if(clockswing > 0x3F)
                    clockswing = 0x3F;
#else
                clockswing = i;
#endif
                msWrite2Byte(REG_3052, ((WORD)i<<10)|(i<<4)); // ch0~ch1
                msWrite2Byte(REG_3054, ((WORD)i<<12)|(clockswing<<6)|(i)); // ch2~ch4
                msWrite2Byte(REG_3056, ((WORD)i<<14)|(i<<8)|(i<<2)|(i>>4)); // ch4~ch7
                msWrite2Byte(REG_3058, ((WORD)i<<10)|(clockswing<<4)|(i>>2));// ch7~ch9
                drvmStar_printMsg("1111 FW MOD Cali Fail !!");
            }
            else
            {
#ifdef ENABLE_UP_LOW_BOUNDARY_PROTECTION
        if (i >ICON_Upper_Boundary || i<ICON_Lower_Boundary)
        {
                i = LVDS_FAIL_TARGET;
                drvmStar_printMsg("HW MOD Cali Fail due to not located in normal range !!");
        }
 #endif
#if DOUBLE_LVDS_CLK_SWING
                clockswing = i*2+LVDS_CLK_1TO2_OFFSET;
                if(clockswing > 0x3F)
                    clockswing = 0x3F;
#else
                clockswing = i;
#endif
                msWrite2Byte(REG_3052, ((WORD)i<<10)|(i<<4)); // ch0~ch1
                msWrite2Byte(REG_3054, ((WORD)i<<12)|(clockswing<<6)|(i)); // ch2~ch4
                msWrite2Byte(REG_3056, ((WORD)i<<14)|(i<<8)|(i<<2)|(i>>4)); // ch4~ch7
                msWrite2Byte(REG_3058, ((WORD)i<<10)|(clockswing<<4)|(i>>2));// ch7~ch9
                drvmStar_printData("msReadByte(REG_30FB)%x", msReadByte(REG_30FB));
                drvmStar_printMsg("222 FW MOD Cali Pass !!");
                drvmStar_printData("Cali result=0x%x", i);
            }
            break;
        }
    }

    if(!(msReadByte(REG_30FB)&BIT0))
    {
        i = LVDS_FAIL_TARGET;
#if DOUBLE_LVDS_CLK_SWING
        clockswing = i*2+LVDS_CLK_1TO2_OFFSET;
        if(clockswing > 0x3F)
            clockswing = 0x3F;
#else
        clockswing = i;
#endif
        msWrite2Byte(REG_3052, ((WORD)i<<10)|(i<<4)); // ch0~ch1
        msWrite2Byte(REG_3054, ((WORD)i<<12)|(clockswing<<6)|(i)); // ch2~ch4
        msWrite2Byte(REG_3056, ((WORD)i<<14)|(i<<8)|(i<<2)|(i>>4)); // ch4~ch7
        msWrite2Byte(REG_3058, ((WORD)i<<10)|(clockswing<<4)|(i>>2));// ch7~ch9
        drvmStar_printMsg("3333 FW MOD Cali Fail !!");
    }

    // 11.Disable MOD Calibration.
    msWriteByteMask(REG_30FA,   0x00, BIT7);
    // 12.Set register "BK36_DAh[5:4], BK36_DBh[15:14], BK36_E2[2], BK36_E2[7], BK36_46[5:4], BK36_47[7:6], BK36_80[1], BK36_8A[6], BK36_8B[1:0]" back to the original normal function.
    msWriteByte(REG_30E2, reg_30E2);   /* restore original value of BK0_AA */
    msWrite2Byte(REG_3046, reg_3046);   /* restore original value of BK0_AB */
    msWrite2Byte(REG_3048, reg_3048);   /* restore original value of BK0_08 */
    msWriteByte(REG_3080, reg_3080);   /* restore original value of BK0_09 */
    msWriteByte(REG_308A, reg_308A);   /* restore original value of BK0_AA */
    msWriteByte(REG_308B, reg_308B);   /* restore original value of BK0_AB */
    msWrite2Byte(REG_306A, reg_306A);
    msWriteByte(REG_3065, reg_3065);
    drvmStar_printMsg("FW I_Gen_Tuning_End");

    iGenTuningFinished=1;
}
#endif  // end of #if( USE_MOD_HW_CAL )
#endif
void Power_ModCtrl(BYTE ucSwitch)
{
    BYTE XDATA  reg_306E   = msReadByte(REG_306E);
    WORD XDATA  reg_3046   = msRead2Byte(REG_3046);
    WORD XDATA  reg_3048   = msRead2Byte(REG_3048);
    WORD XDATA  reg_306C   = msRead2Byte(REG_306C);
    WORD XDATA  reg_30EA   = msRead2Byte(REG_30EA);
    WORD XDATA  reg_30F2   = msRead2Byte(REG_30F2);

#if 1
    msWriteByteMask(REG_306E, 0x00  ,BIT6);
    msWrite2Byte(REG_3046,  0xFFFF);
    msWrite2Byte(REG_3048,  0xFFFF);
    msWrite2Byte(REG_306C,  0x3FF);
    msWrite2Byte(REG_30EA,  0x3FF);
    msWrite2Byte(REG_30F2,  0x3FFF);
    Delay1ms(1);
#endif

    if (ucSwitch)
    {
#if 0
#if PanelLVDS
  #if PanelDualPort!=0 // port A + B
        msWriteByte(REG_30DA, 0x55);
        msWriteByte(REG_30DB, 0x55);
        msWriteByte(REG_30DC, 0x05);
  #elif PanelSwapPort!=0 // port B // need check
        msWriteByte(REG_30DB, 0x54);
        msWriteByte(REG_30DC, 0x05);
  #else // port A // need check
        msWriteByte(REG_30DA, 0x55);
        msWriteByte(REG_30DB, 0x01);
  #endif
        msWriteByteMask(REG_30EE,0x07,0x07);
        msWriteByteMask(REG_30F0,0,BIT0);
#else
    #error "Only LVDS Supported!"
#endif
#else
    msWriteByteMask(REG_30EE,0x07,0x07);
    msWriteByteMask(REG_30F0,0,BIT0);
    #if PanelLVDS
      #if PanelDualPort!=0 // port A + B
            msWriteByte(REG_30DA, 0x55);
            msWriteByte(REG_30DB, 0x55);
            msWriteByte(REG_30DC, 0x05);
      #elif PanelSwapPort!=0 // port B // need check
            msWriteByte(REG_30DB, 0x54);
            msWriteByte(REG_30DC, 0x05);
      #else // port A // need check
            msWriteByte(REG_30DA, 0x55);
            msWriteByte(REG_30DB, 0x01);
      #endif
            //msWriteByteMask(REG_30EE,0x07,0x07);
            //msWriteByteMask(REG_30F0,0,BIT0);
    #else
        #error "Only LVDS Supported!"
    #endif
#endif
        msWrite2Byte(REG_306A, LVDS_XSWING_CH); // ch double swing
    }
    else
    {
#if 0
        msWriteByteMask(REG_30F0,BIT0,BIT0);
        msWriteByteMask(REG_30EE,0x00,0x07);
        msWriteByte(REG_30DA, 0x00);
        msWriteByte(REG_30DB, 0x00);
        msWriteByte(REG_30DC, 0x00);
        msWriteByte(REG_30DD, 0x00);
#else
            msWriteByte(REG_30DA, 0x00);
            msWriteByte(REG_30DB, 0x00);
            msWriteByte(REG_30DC, 0x00);
            msWriteByte(REG_30DD, 0x00);
            msWriteByteMask(REG_30F0,BIT0,BIT0);
            msWriteByteMask(REG_30EE,0x00,0x07);
#endif
    }
#if 1
    msWriteByte(REG_306E, reg_306E);
    msWrite2Byte(REG_3046,  reg_3046);
    msWrite2Byte(REG_3048,  reg_3048);
    msWrite2Byte(REG_306C,  reg_306C);
    msWrite2Byte(REG_30EA,  reg_30EA);
    msWrite2Byte(REG_30F2,  reg_30F2);
#endif
}


BYTE mStar_ScalerDoubleBuffer(Bool u8Enable)
{
    u8Enable=0;
    return u8Enable;
}

void mStar_SetDClkPLL(DWORD u32ODCLK_KHZ)
{
    XDATA BYTE  u8LPLL_ICP_ICTL,u8LPLL_IBIAS_ICTL;
    XDATA BYTE  u8LPLL_LOOP_DIV_1ST,u8LPLL_LOOP_DIV_2ND;
    XDATA BYTE  u8LPLL_OUTPUT_DIV_1ST, u8LPLL_SCALER_DIV_SEL;
    XDATA BYTE  u8LPLL_TEST;
    XDATA BYTE  u8LPLL_LOOP_GAIN, u8LPLL_LOOP_DIV;
    XDATA DWORD u32Factor, u32MPLL_KHZ;

    u8LPLL_ICP_ICTL         = 2;
    u8LPLL_IBIAS_ICTL       = 3;
    u8LPLL_LOOP_DIV_2ND     = 1;
    u8LPLL_SCALER_DIV_SEL   = 4;
    u8LPLL_LOOP_DIV         = 7;
    u32MPLL_KHZ = 429540UL;  // 429.54MHz

#if PanelDualPort
    u8LPLL_LOOP_DIV_1ST     = 1;
    u8LPLL_OUTPUT_DIV_1ST   = 2;
    if( u32ODCLK_KHZ <= 50*1000UL )    // 25MHz~50MHz
    {
        u8LPLL_TEST         = 3;
        u8LPLL_LOOP_GAIN    = 2;
    }
    else if( u32ODCLK_KHZ <= 100*1000UL )    // 50MHz~100MHz
    {
        u8LPLL_TEST         = 1;
        u8LPLL_LOOP_GAIN    = 4;
    }
    else    // 100MHz~200MHz
    {
        u8LPLL_TEST         = 0;
        u8LPLL_LOOP_GAIN    = 8;
    }
#else
    u8LPLL_LOOP_DIV_1ST     = 2;
    u8LPLL_OUTPUT_DIV_1ST   = 1;
    if( u32ODCLK_KHZ <= 50*1000UL )    // 25MHz~50MHz
    {
        u8LPLL_TEST         = 3;
        u8LPLL_LOOP_GAIN    = 2;
    }
    else // 50MHZ~80MHZ
    {
        u8LPLL_TEST         = 1;
        u8LPLL_LOOP_GAIN    = 4;
    }
#endif

    // SET/i_gain/p_gain calculation

#ifdef FPGA
    u32Factor = (float)(16.0/7)*(214.0)*( (float)524288UL)/u32ODCLK_KHZ*1000;
#else
    u32Factor = (float)((float)u32MPLL_KHZ/u8LPLL_LOOP_DIV)*((float)524288UL*(float)u8LPLL_LOOP_GAIN/u32ODCLK_KHZ);
#endif

    msWriteByteMask( REG_3802, u8LPLL_LOOP_DIV_1ST, BIT1|BIT0);
    msWriteByte(     REG_3803, u8LPLL_LOOP_DIV_2ND);
    msWriteByteMask( REG_3804, u8LPLL_OUTPUT_DIV_1ST, BIT1|BIT0);
    msWriteByteMask( REG_3806, (u8LPLL_IBIAS_ICTL<<2)|(u8LPLL_ICP_ICTL), BIT3|BIT2|BIT1|BIT0);
    msWriteByteMask( REG_3807, u8LPLL_SCALER_DIV_SEL<<2, BIT4|BIT3|BIT2);
    msWriteByteMask( REG_386F, u8LPLL_TEST<<1, BIT2|BIT1);

    msWriteBit(REG_3818, _DISABLE, _BIT3);
    msWrite2Byte(SC0_D5, u32Factor);
    msWriteByte(SC0_D7, u32Factor>>16);
    msWrite2Byte(REG_381E, u32Factor);
    msWriteByte(REG_3820, u32Factor>>16);
}

void drvmStar_SetupFreeRunMode( void )
{
    mStar_ScalerDoubleBuffer(FALSE);
    msWriteByteMask( SC0_02, BIT7, BIT3|BIT7 ); // disable output Lock mode to enable free run.

    msWriteByteMask(SC0_27,BIT6,BIT6|BIT1);
    msWrite2ByteMask(SC0_1E, PanelVTotal-1,SC_MASK_V); // set output panel vtotal
    msWrite2Byte(SC0_24, PanelHTotal-1);
    msWriteByteMask(SC0_63,BIT0,BIT1|BIT0);
    msWriteByteMask(SC0_28,BIT6,BIT6);
    msWriteByteMask(SC0_28,BIT3,BIT3);
    msWriteByteMask(SC0_9F,0,BIT0);
    mStar_SetDClkPLL(PanelDCLK*1000UL); //White101208 must use 1000UL to provent overflow truncate
}

void mStar_SetTimingGen( WORD u16HDE_OP21, WORD u16VDE_OP21, BOOL OP21_FreeRun)
{
    WORD uwV_total_OP22;
    WORD uwV_total_OP21;

    uwV_total_OP22 = PanelVTotal;//GetVtotal(); // output V total
    uwV_total_OP21 = (PanelVTotal-PANEL_HEIGHT+u16VDE_OP21);
    if ( ((g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_1) && (!OP21_FreeRun)) || (!g_SetupPathInfo.bFBMode)) //case 12, FBL
    {
        msWrite2Byte(SC0_10, 0x00); //panel V. start
        msWrite2ByteMask(SC0_14, ( (u16VDE_OP21) - 1 ), 0x7FF); //panel Vert. DE end
        msWrite2ByteMask(SC0_16, ( ( PanelHStart/PANEL_H_DIV ) + ((u16HDE_OP21)/PANEL_H_DIV) - 1 ), 0xFFF); //panel Hor. DE end
        msWrite2ByteMask(SC0_1A, ( (u16VDE_OP21) - 1 ), 0x7FF); //image V. end
        msWrite2ByteMask(SC0_20, ( uwV_total_OP21 - PanelVStart ), 0x7FF); // vsync start
        msWrite2ByteMask(SC0_22, ( uwV_total_OP21 - PanelVSyncBackPorch ), 0xFFF); //vsync end
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
    else // Case 3456, FB free run
    {
        msWrite2Byte(SC0_10, 0x00);
        msWrite2ByteMask(SC0_14, ( PanelHeight - 1 ), 0x7FF); // Vert. DE end
        msWrite2ByteMask(SC0_1A, ( PanelHeight - 1 ), 0x7FF);

        msWrite2ByteMask(SC0_16, ( ( PanelHStart/PANEL_H_DIV ) + (PanelWidth/PANEL_H_DIV) - 1 ), 0xFFF); // Hor. DE end
        msWrite2ByteMask(SC0_20, ( uwV_total_OP22 - PanelVStart ), 0x7FF); // vsync start
        msWrite2ByteMask(SC0_22, ( uwV_total_OP22 - PanelVSyncBackPorch ), 0xFFF); //vsync end

        msWrite2ByteMask(SC0_18, ( PanelHStart/PANEL_H_DIV ), 0xFFF);
        msWrite2ByteMask(SC0_1C, ( ( PanelHStart/PANEL_H_DIV ) + (PanelWidth/PANEL_H_DIV) - 1 ), 0xFFF);

        if(OverScanSetting.Enable && (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL))
        {
            WORD offset = (PanelHeight - OverScanSetting.OverScanV);
            msWrite2ByteMask(SC0_18, ( ( PanelHStart + ( PanelWidth-OverScanSetting.OverScanH )/2 )/PANEL_H_DIV ), 0xFFF);
            msWrite2ByteMask(SC0_1C, ( ( ( PanelHStart + ( PanelWidth-OverScanSetting.OverScanH )/2 + OverScanSetting.OverScanH )/PANEL_H_DIV ) - 1 ), 0xFFF);
            if( OverScanSetting.OverScanV < PanelHeight )
            {
                msWrite2Byte(SC0_10, (uwV_total_OP22 - (offset/2) - 1 ));
                msWrite2Byte(SC0_14, (PanelHeight - (offset/2) - 1));
                msWrite2Byte(SC0_1A, (PanelHeight - offset - 1));
                msWrite2ByteMask(SC0_20, ( uwV_total_OP22 - (offset/2) - 1 - 3 ), 0x7FF); // vsync start
                msWrite2ByteMask(SC0_22, ( uwV_total_OP22 - (offset/2) - 1 - 2 ), 0xFFF); // vsync end
            }
        }
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
#if CHIP_ID==CHIP_TSUMK
            msWriteByteMask(SC0_28,BIT5,BIT5);  //prevent output vtt counter overflow
            msWriteByteMask(SC0_A3,BIT7,BIT7);  //prevent input vtt counter overflow
#endif
            break;
        default:
            break;
    }

}

void drvmStar_SetupInputPort_VGA(void)
{
    msWriteByteMask(SC0_03, 0, _BIT7);                                       // YUV422 issue
    msWriteByteMask( REG_06C2, BIT2|BIT3, BIT2|BIT3 );                   //DP Port B/C Off Line
    msWriteByteMask(SC0_A8, 0, _BIT1|BIT0);
    drvADC_SetInputMux(Input_VGA);
    mStar_SetAnalogInputPort( FALSE );    // switch to RGB port by default
    msWriteByteMask(SC0_E8, 0, _BIT5|_BIT4|_BIT3|_BIT2); //user define HV polarity(bit5,bit3)
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
    msWriteByteMask(SC0_03, 0, _BIT7);                                       // YUV422 issue
  msWriteByteMask( REG_06C2, BIT2|BIT3, BIT2|BIT3 );                     //DP Port B/C Off Line
    msWriteByteMask(SC0_A8, 0, _BIT1|BIT0);
#if (DVI_RB_SWAP)
    msWriteByteMask(REG_1600,_BIT5,_BIT5);
#endif
#if (DVI_PN_SWAP)
    msWriteByteMask(REG_1600,0x0F,0x0F);
#endif
    msWriteByteMask(SC0_E8, 0, _BIT5|_BIT4|_BIT3|_BIT2); //user define HV polarity(bit5,bit3)
    msTMDSSetMux( SrcInputType  );

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



#if ENABLE_HDMI
void drvmStar_SetupInputPort_HDMI(void)
{
    msWriteByteMask(SC0_03, 0, _BIT7);                                       // YUV422 issue
    msWriteByteMask( REG_06C2, BIT2|BIT3, BIT2|BIT3 );                   //DP Port B/C Off Line
    msWriteByteMask(SC0_A8, 0, _BIT1|BIT0);
#if (HDMI_RB_SWAP)
    msWriteByteMask(REG_1600,_BIT5,_BIT5);
#endif
#if (HDMI_PN_SWAP)
    msWriteByteMask(REG_1600,0x0F,0x0F);
#endif
    msWriteByteMask(SC0_E8, 0, _BIT5|_BIT4|_BIT3|_BIT2); //user define HV polarity(bit5,bit3)
    msTMDSSetMux( SrcInputType  );

#if (_TMDS_DE_MODE_EN_)&&(!ENABLE_FREESYNC)
    msWriteByte( SC0_04, BIT3  | BIT7 |BIT6);
    msWriteByteMask( REG_1E22, BIT3, BIT3 ); // enable DE-only mode for SC_TOP
#else
    msWriteByte( SC0_04, BIT3  | BIT7);
    msWriteByteMask( REG_1E22, 0, BIT3 ); // enable DE-only mode for SC_TOP
#endif

    msWriteByte(SC0_F3, 0xF0);// enable DE glitch removal function & more tolerance for DE
    msWriteByteMask( REG_2E04, IP_MUX_DVI_A+1, 0x0F );       // IPmux 3 for Dual DVI and DP
    msWriteByteMask( REG_2E02, IP_MUX_DVI_A << 4, 0xF0 );    // IPmux 2
    msWriteByteMask( REG_1E3E, (CLK_DVI_BUFF_DIV2<<4), 0xF0 );

    msWriteByte(SC0_02, NIS_B|ISEL_HDMI_444|SCALER_STYPE_HV);
}
#endif

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

#if CHIP_ID==CHIP_TSUMK
BYTE GetVfreq( void )
{
    if (g_SetupPathInfo.bFBMode) // FB, Output 60/Vfreq-in double/Vfreq-in triple
    {
        if(IS_INPUT_VFREQ_DOUBLE_IN_PANEL_RANGE())
                return (V_FREQ_IN*2);
            else if(IS_INPUT_VFREQ_TRIPLE_IN_PANEL_RANGE())
                return (V_FREQ_IN*3);
            else
                return 60;
    }
    else // Vout = Vin
    {
        return V_FREQ_IN;
    }
}
#endif
#endif
