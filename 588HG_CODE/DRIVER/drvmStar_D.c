
///////////////////////////////////////
// DRVMSTAR_V VERSION: V02
////////////////////////////////////////
#include "global.h"
#include "drvDPRxApp.h"
#include "msEread.h"
#include "menufunc.h"
#include "Detect.h"


#ifndef _DRVMSTAR_D_C_
#define _DRVMSTAR_D_C_

#define drvmStar_DEBUG    1
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
    {REG_1133, 0x00, BIT7},
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
    {REG_17BC, 0x0040 |HDMI_PLL_LDO_VALUE, 0xFFFF},   // [6]: for all mode,   VCO switch cap always on, add PLL LDO 5% setting
    {REG_1782, 0x8101, 0xFFFF},
    {REG_17A8, 0x001B, 0xFFFF},
    {REG_17AA, 0x0000, 0x6000},   // Disable overwrite [D]: Phase DAC DIV select, [E]: PLL DIV select
    //{REG_17AC, 0x8FE0, 0xFFFF},
    {REG_17AC, 0x87E0, 0xFFFF}, // 20130201 - follow RD new IP driver table
    {REG_1740, 0x0001, 0xFFFF},
    {REG_17B4, 0x0010, 0xFFFF},   // [4]: for all mode,  phase dac vring LDO +5%
    {REG_174C, 0xFFFF, 0xFFFF},
    {REG_174E, 0x3FFF, 0xFFFF},
    {REG_17BE, 0x0000, 0xFFFF},

    {REG_01C2, 0x0104, 0xFF1F},    // port mux couldn't be set in the initial table
    {REG_01C6, 0x111F, 0xFFFF},
    {REG_01CA, 0x10F8, 0xFFFF},
    {REG_01CC, 0x0000, 0xFFFF},
    {REG_01CE, 0x0020, 0xFFF8},
    {REG_0C01, 0x0000, 0x0300},
    {REG_1506, 0x0000, 0x0004},
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

    {REG_3041, 0x00},
    {REG_3080, (PanelSwapMLSB&BIT6)|(BIT5&PANEL_SWAP_PN)|(BIT2&LVDS_TIMode)|BIT3},
    {REG_308A, 0x3F},
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

    {REG_3081, (PanelDClkDelay<<4)}, // DE delay, clock delay for TTL output
    {REG_3094, (PanelInvDClk&BIT4)|(PanelInvDE&BIT2)|(PanelInvVSync&BIT3)
               |(PanelDualPort&BIT1)|(PanelSwapPort&BIT0)},

    //{REG_3089, PanelDualPort&BIT4},
    {REG_3095, (PanelInvHSync&BIT4)},
    {REG_3096, LVDS_TI_BitModeSel},
    //{REG_3092, 0x00},
    //{REG_30DA, 0x55},
    //{REG_30DB, 0x55},
    //{REG_30DC, 0x05},
    {REG_30EE, 0x07},
    {REG_30F0, BIT0},

    {REG_3820, 0x2A},
    {REG_380A, 0xFF},
    {REG_380B, 0xFF},
    {REG_380E, 0x02},
    {REG_380D, 0x00},
    {REG_3812, 0x02},
    {REG_3813, 0x00},
    {REG_3814, 0x00},
    {REG_3815, 0xF0},
    {REG_3816, 0x43},
    {REG_3817, 0x43},

    {SC0_6C, 0x90}, // short line tune coef. // 120928 coding, modified default value 0x70 to 0x90

    // Clock generator control 0xD0~0xDB
    {SC0_D0, 0x00},
    //{REG_30AA, 0x00}, // BK0_D0[0] Rose <--> REG_30AA[0] CHIP_TSUML
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
    {SC6_01, 0x00}, //can be deleted
    {SC6_02, 0x14}, //[2]IPM CLK [4]DB_EN
    {SC6_03, 0x00},

    {SC6_1A, 0x02}, //OPM CLK
    {SC6_1B, 0x00}, //can be deleted


    {SC6_06 ,0x40},    //[7:0]write  MIU unit of N-data per time ,initial only.
    {SC6_07 ,0x40},    //[7:0]IPM Write high priority thrd
    {SC6_08 ,0x40},    //[7:0]Read MIU unit of N-data per time ,initial only.
    {SC6_09 ,0x40},    //[7:0]IPM Read high priority thrd
    {SC6_0A ,0x40},
    {SC6_0B ,0x40},    //reg_ipr_rreq_hpri

    {SC6_0C ,0x28},    //reg_ipm_arb_lvl_thrd
    {SC6_0D ,0x28},    //reg_ipm_arb_req_length
    {SC6_0E ,0xC8},    //reg_opm_arb_lvl_thrd
    {SC6_0F ,0x28},    //reg_opm_arb_req_length

    {SC6_10 ,0x28},    //reg_ipr_arb_lvl_thrd
    {SC6_11 ,0x28},    //reg_ipr_arb_req_length

    {SC6_14 ,0x00},    //[2:0] memory config:3'000: YG, CB, CR 3'b001: Y, C 3'b010: YG only 3'b100: YG, CB, CR, M 'b101: Y, C, M 3'b110: Y, M
    {SC6_15 ,0x00},    //IPM channel
    {SC6_16 ,0x00},    //OPM channel
    {SC6_17 ,0x00},    //IPR channel

    {SC6_C0 ,0x60},    //REG_MAX_ADDR
    {SC6_C1 ,0x3d},    //REG_MAX_ADDR
    {SC6_C2 ,0x08},    //REG_MAX_ADDR
    {SC6_C3 ,0x00},    //REG_MAX_ADDR
    {SC6_C4 ,0x00},    //REG_MIN_ADDR
    {SC6_C5 ,0x00},    //REG_MIN_ADDR
    {SC6_C6 ,0x00},    //REG_MIN_ADDR
    {SC6_C7 ,0x00},    //REG_MIN_ADDR
    {SC6_C8 ,0x80},    //REG_MAX_ADDR_M
    {SC6_C9 ,0xfc},    //REG_MAX_ADDR_M
    {SC6_CA ,0x0a},    //REG_MAX_ADDR_M
    {SC6_CB ,0x00},    //REG_MAX_ADDR_M
    {SC6_CC ,0x60},    //REG_MIN_ADDR_M
    {SC6_CD ,0x3d},    //REG_MIN_ADDR_M
    {SC6_CE ,0x08},    //REG_MIN_ADDR_M
    {SC6_CF,0x00},     //REG_MIN_ADDR_M

#if 0
    // Video Compression Settings
    {SC1_00,0x01},
    {SC1_01,0x00},
    {SC1_02,0x00},
    {SC1_03,0x15},
    {SC1_04,0x40},
    {SC1_05,0x06},
    {SC1_06,0x12},
    {SC1_07,0x00},
    {SC1_08,0x11},
    {SC1_09,0x02},
    {SC1_0A,0x11},
    {SC1_0B,0x02},
    {SC1_0C,0x09},
    {SC1_0D,0x07},
    {SC1_0E,0x13},
    {SC1_0F,0x05},
    {SC1_10,0x03},
    {SC1_11,0x00},
    {SC1_12,0x1E},
    {SC1_13,0x44},
    {SC1_20,0x0F},
    {SC1_21,0x00},
    {SC1_22,0x00},
    {SC1_23,0x15},
    {SC1_24,0x40},
    {SC1_25,0x06},
    {SC1_26,0x12},
    {SC1_27,0x00},
    {SC1_28,0x11},
    {SC1_29,0x02},
    {SC1_2A,0x11},
    {SC1_2B,0x02},
    {SC1_2C,0x09},
    {SC1_2D,0x07},
    {SC1_2E,0x13},
    {SC1_2F,0x05},
    {SC1_30,0x03},
    {SC1_31,0x00},
    {SC1_32,0x1E},
    {SC1_33,0x44},
    {SC1_A0,0x07},
    {SC1_A1,0x07},
    {SC1_A4,0x07},
    {SC1_A5,0x00},
    {SC1_A6,0x57},
    {SC1_A7,0x55},
    {SC1_A8,0x07},
    {SC1_A9,0x07},
    {SC1_AA,0x07},
    {SC1_AB,0x07},
    {SC1_AC,0xE5},
    {SC1_AD,0xE5},
    {SC1_AE,0xE5},
    {SC1_AF,0x00},
    {SC1_44,0xA0},
    {SC1_C6,0x51},
    {SC1_C8,0x01},
    {SC1_C9,0x07},
#endif
    {SCC_42,0x00},  //disable DNR before IPR setting is ready
#if 0
    {SC1_CC,0xE5},
#endif
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
    msWriteByteMask(REG_1E37, 0x00, BIT2|_BIT0);                 // odclk1
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

    msWriteByteMask(REG_1E46,0x00, 0x0F);   // enable MRW clock
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
    msWriteByte(REG_01C7,0x00);                 // DVI phase dac power on for CHIP_TSUML2

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

#if 0//If support I2S, need these settings
    #if I2S_Earout_Simultaneously
        msWriteByteMask(REG_0207, BIT0, BIT0);  //Enable I2S output pad
        msWriteByteMask(REG_1426, 0x05, 0x05); //[2]: I2S out mute OEN, [0]: I2S out OEN
        msWriteByteMask(REG_174B, 0x04, 0x04); //[2]: I2S main clock 192fs out
        msWriteByteMask(REG_1751, BIT7, BIT7);  //[7]: Inverse I2S BCK
        msWriteByteMask(REG_025C, BIT7|BIT6, BIT7|BIT6);  //[7]: Inverse I2S BCK
    #else
        msWriteByteMask(REG_0207, 0x03, 0x03); //Enable I2S output pad and AUMCK output pad control
        msWriteByteMask(REG_1426, 0x05, 0x05); //[2]: I2S out mute OEN, [0]: I2S out OEN
        msWriteByteMask(REG_174B, 0x04, 0x04); //[2]: I2S main clock 192fs out
        msWriteByteMask(REG_1751, BIT7, BIT7); //[2]: I2S main clock 192fs out
    #endif
    #if I2S_gothrough_DPGA
        #if I2S_Earout_Simultaneously
        msWriteByteMask(REG_0207, BIT6|BIT5     , BIT6|BIT5|BIT4|BIT1|BIT0);    //Enable I2S output pad
        #else
        msWriteByteMask(REG_0207, BIT6|BIT5|BIT4    , BIT6|BIT5|BIT4|BIT1|BIT0);    //Enable I2S output pad
        #endif
        msWriteByteMask(REG_2C04, BIT6|0x08, BIT6|0x0F);    ////[6]enable test clk to MCK [3:0]=1000 //sel test clk = 256FS   =48*256=12.288M
    #endif

    #if CASE1_LineIn_To_I2S
        msWriteByteMask(REG_0207, 0x00, BIT6|BIT5|BIT1|BIT0);
        msWriteByteMask(REG_0214, 0x70, BIT6|BIT5|BIT4|BIT2|BIT1|BIT0);
    #endif
#endif

    msWriteByteMask(REG_1569, 0x00, BIT0);

    msWrite2Byte(REG_1604, 0x00C0);
    msWrite2Byte(REG_1438, 0x0010);
    msWrite2Byte(REG_142A, 0x0017);

    //CTS
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

void drvmStar_Init( void )
{
    BYTE i;

#if ENABLE_SW_DOUBLE_BUFFER
    msSWDBInit();
#endif

#if CHIP_ID == CHIP_TSUM2 //U02 need to Set this bit to let MCU could restart after reset.
    msWriteByteMask(REG_0180,BIT0,BIT0); //when external 1.2V is lower than 0.8V, Scalar will enter into reset status. let 1.2V back to normal level , MCU should restart again.
#endif
#ifndef FPGA
    if (!msPM_StartRCOSCCal()) // calibration here for other IP to use FRO_12M
        drvmStar_printData("CALIBARYION RCOSC_12M FAIL!",0);
    else
        drvmStar_printData("CALIBARYION RCOSC_12M Success!",0);
#endif

    msInitClockGating();

    msTMDSInit();

    msWriteByteMask(SC0_02, BIT7, BIT7);

    for( i = 0; i < sizeof( tblInit ) / sizeof( RegUnitType ); i++ )
        msWriteByte( tblInit[i].u16Reg, tblInit[i].u8Value );
    for( i = 0; i < sizeof( tb_3D_DITHER_TABLE ) / sizeof( RegUnitType ); i++ )
        msWriteByte( tb_3D_DITHER_TABLE[i].u16Reg, tb_3D_DITHER_TABLE[i].u8Value );

// due to f/w do FRC automatically, therefore always executes msInitMemory.
    msInitMemory();

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
    {
        g_SetupPathInfo.b2DTo3DEnable = TRUE;
        Sc2DTo3DInit();
    }
#endif
#if 0// ENABLE_USB_INPUT
    mcuDMADownloadCode(DMA_TO_DRAM_SOURCE_ADDR, DMA_TO_DRAM_DESTIN_ADDR, DMA_TO_DRAM_BYTE_COUNT);
    mcuArrangeCodeAddr(_DRAM, MCU_ON_DRAM_START_ADDR , MCU_ON_DRAM_END_ADDR);
    mcuArrangeCodeAddr(_SPI, MCU_ON_SPI_START_ADDR, MCU_ON_SPI_END_ADDR);

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
        msWriteByteMask(REG_01C2, BIT5 | BIT6 | BIT7, BIT5 | BIT6 | BIT7); // (Turn on Port Mux 0~2) BIT 5~7 = Port 0~2
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
    BYTE XDATA clockswing;

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
    XDATA BYTE u8Div_M;
    XDATA float  u32i_gain ,u32p_gain;

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

    u8Div_M = mStar_SetFrameDivider(V_FREQ_IN);
    mStar_SetIVS();

    // SET/i_gain/p_gain calculation

#ifdef FPGA
#if FRAME_BFF_SEL==FRAME_BUFFER
    u32Factor = (float)(0x2C0000);
    u32i_gain = ( float )(( float )15*524288/8/(PanelHTotal/PANEL_H_DIV)/(WORD)PanelVTotal*u8LPLL_LOOP_GAIN )/(float)(( ((msReadByte(REG_3819)>>4) + ((msReadByte(REG_381C)&BIT1)<<1)) +1)*7);
#else
    u32Factor = (float)(16.0/7)*(214.0)*( (float)524288UL)/u32ODCLK_KHZ*1000;
#endif
#else
    u32Factor = (float)((float)u32MPLL_KHZ/u8LPLL_LOOP_DIV)*((float)524288UL*(float)u8LPLL_LOOP_GAIN/u32ODCLK_KHZ);
    u32i_gain = ( float )((( float ) u8LPLL_LOOP_GAIN*30*524288)/(( DWORD )PanelHTotal*PanelVTotal*(u8Div_M+1) *8 *7));
#endif

    u32i_gain=GetLog(u32i_gain*1024) -5;
    u32p_gain = u32i_gain + 1;
    if(INPUT_TIMING_EXECUTE_FRAME_PLL_LOCK())
    {
        u32i_gain += 2;
        u32p_gain += 2;
    }

#if 0//ENABLE_DEBUG && 0
        drvmStar_printData("SynthH_KHZh      = 0x%x", u32MPLL_KHZ>>16    );
        drvmStar_printData("SynthL_KHZh      = 0x%x", u32MPLL_KHZ&0xFFFF );
        drvmStar_printData("u8LPLL_LOOP_DIV_1ST = %d",   u8LPLL_LOOP_DIV_1ST );
        drvmStar_printData("u8LPLL_OUTPUT_DIV_1ST   = %d",  u8LPLL_OUTPUT_DIV_1ST  );
        drvmStar_printData("u8LPLL_TEST  = %d",   u8LPLL_TEST  );
        drvmStar_printData("u8LPLL_LOOP_DIV  = %d",   u8LPLL_LOOP_DIV  );
        drvmStar_printData("u8LPLL_LOOP_GAIN   = %d",   u8LPLL_LOOP_GAIN  );
        drvmStar_printData("ODCLK_KHZh     = 0x%x", u32ODCLK_KHZ>>16);
        drvmStar_printData("ODCLK_KHZl     = 0x%x", u32ODCLK_KHZ&0xFFFF);
//#else
        drvmStar_printData("$$$$ u32i_gain    = 0x%x", u32i_gain);
        drvmStar_printData("$$$$ u32p_gain    = 0x%x", u32p_gain);
#endif

    drvmStar_printData("$$$$ u32i_gain    = 0x%x", u32i_gain);
    drvmStar_printData("$$$$ u32p_gain    = 0x%x", u32p_gain);

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

    msWriteByteMask(REG_3816,(BYTE)u32i_gain       ,BIT0|BIT1|BIT2|BIT3);//i_gain
    msWriteByteMask(REG_3816,((BYTE)u32p_gain)<<4  ,BIT4|BIT5|BIT6|BIT7);//p_gain
    msWriteByteMask(REG_3817,(BYTE)u32i_gain       ,BIT0|BIT1|BIT2|BIT3);//i_gain_phase
    msWriteByteMask(REG_3817,((BYTE)u32p_gain)<<4  ,BIT4|BIT5|BIT6|BIT7);//p_gain_phase
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
    WORD uwV_total_OP22; // output vtotal OP2-2
    WORD uwV_total_OP21; // output vtotal OP2-1

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
        if( OverScanSetting.Enable )
        {
            WORD offset = (PanelHeight - OverScanSetting.OverScanV);
            msWrite2ByteMask(SC0_18, ( ( PanelHStart + ( PanelWidth-OverScanSetting.OverScanH )/2 )/1/*Hdivider*/ ), 0xFFF);
            msWrite2ByteMask(SC0_1C, ( ( ( PanelHStart + ( PanelWidth-OverScanSetting.OverScanH )/2 + OverScanSetting.OverScanH )/1/*Hdivider*/ ) - 1 ), 0xFFF);

            if( OverScanSetting.OverScanV < PanelHeight )
            {
                msWrite2Byte(SC0_10, (uwV_total_OP22 - (offset/2) - 1 ));
                msWrite2Byte(SC0_14, (PanelHeight - (offset/2) - 1));
                msWrite2Byte(SC0_1A, (PanelHeight - offset - 1));
                msWrite2ByteMask(SC0_20, ( uwV_total_OP22 - (offset/2) - 1 - 3 ), 0x7FF); // vsync start
                msWrite2ByteMask(SC0_22, ( uwV_total_OP22 - (offset/2) - 1 - 2 ), 0xFFF); // vsync end
            }
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
            //TIME_MEASURE_BEGIN();
            break;
        default:
            break;
    }

}

#define INPUT_VFREQ_TOLERANCE   2 // unit: Hz
BYTE mStar_ClassifyVfreqBefCalFRCDivider( BYTE u8Vinfreq )
{
    BYTE u8VinModified =0;

    if(!IS_INPUT_VFREQ_IN_PANEL_RANGE())
    {
        drvmStar_printData("input VFreq over panel spec:%d", u8Vinfreq);

    if (abs(u8Vinfreq-50)<=INPUT_VFREQ_TOLERANCE)
            u8VinModified=50;
    else if (abs(u8Vinfreq-56)<=INPUT_VFREQ_TOLERANCE)
            u8VinModified=56;
    else if (abs(u8Vinfreq-60)<=INPUT_VFREQ_TOLERANCE)
            u8VinModified=60;
    else if (abs(u8Vinfreq-66)<=INPUT_VFREQ_TOLERANCE)
            u8VinModified=66;
    else if (abs(u8Vinfreq-70)<=INPUT_VFREQ_TOLERANCE)
            u8VinModified=70;
    else if (abs(u8Vinfreq-72)<=INPUT_VFREQ_TOLERANCE)
            u8VinModified=72;
    else if (abs(u8Vinfreq-75)<=INPUT_VFREQ_TOLERANCE)
            u8VinModified=75;
    else
        {
            drvmStar_printData("input VFreq cannot classify:%d",u8VinModified);
            u8VinModified = u8Vinfreq;
        }
    }
    else
    {
        u8VinModified = u8Vinfreq;
        drvmStar_printData("input VFreq in panel spec:%d",u8VinModified);
    }

    return u8VinModified;
}

BYTE mStar_SetFrameDivider( BYTE vfreq_N )
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

    vfreq_N = mStar_ClassifyVfreqBefCalFRCDivider(vfreq_N);

    u8VFreq_M = GetVfreq();

    drvmStar_printData( "input VFreq_N = %d", vfreq_N);
    drvmStar_printData( "output VFreq_M = %d", u8VFreq_M);

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
            }
            else if( labs(((((DWORD)u8Index_N)<<16)/u8Index_M) - ((((DWORD)vfreq_N)<<16)/u8VFreq_M)) < u32MinDiff)
            {
                u8Div_N    = u8Index_N;
                u8Div_M    = u8Index_M;
                u32MinDiff = labs(((((DWORD)u8Index_N)<<16)/u8Index_M) - ((((DWORD)vfreq_N)<<16)/u8VFreq_M));
            }
        }
    }

    u8Div_N = u8Div_N-1;
    u8Div_M = u8Div_M-1;
    drvmStar_printData( "Set input V_divier_N = %d", u8Div_N);
    drvmStar_printData( "Set output V_divier_M = %d", u8Div_M);
    msWriteByte(REG_3819, (u8Div_M<<4)|(u8Div_N&0x0F)); //[3:0] input frame div for frame sync, [7:4] output frame div for frame sync
    msWriteByteMask(REG_381C, ((u8Div_M&BIT4)>>3)|(u8Div_N>>4),BIT1|BIT0); //[0] input frame div[4], [1] output frame div [4]
    return u8Div_M;
}

BYTE GetVfreq( void )
{
    if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_PF ) // Vout = Vin/2
        return (V_FREQ_IN + 1)/2;
    else if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FPI )// Vout = Vin*2
        return V_FREQ_IN*2;
    else if (g_SetupPathInfo.bFBMode) // FB, Output 60/Vfreq-in double/Vfreq-in triple
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


void mStar_SetIVS( void )
{
    XDATA WORD ivs, Sample_vst;
    WORD Input_Vtt = msRead2Byte(SC0_E2);

    Sample_vst = ( msRead2Byte( SC0_05 ) & SC_MASK_V )/((SrcFlags&bInterlaceMode)?2:1);

    if(g_SetupPathInfo.bFBMode)
    {
#if 0
        if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_TB)
                 && (g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_PSV) )
        {
            ivs = IVS_DEFAULT + g_SetupPathInfo.wImgSizeInV + Sample_vst;
        }
        else
        {
            ivs = IVS_DEFAULT + Sample_vst;
        }
#endif
        ivs = Sample_vst + 10*GetVfreq()/V_FREQ_IN;
    }
    else
        ivs = 4 + Sample_vst;

    msWrite2ByteMask(SC0_B8, ivs<<4,0xFFF0);//IVS:{SC0_B9[7:0],SC0_B8[7:4]}
}

void drvmStar_SetupInputPort_VGA(void)
{
    msWriteByteMask(SC0_03, 0, _BIT7);                                       // YUV422 issue
    msWriteByteMask( REG_06C2, BIT2|BIT3, BIT2|BIT3 );                   //DP Port B/C Off Line
    msWriteByteMask(SC0_A8, 0x00, _BIT1|BIT0);
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
    msWriteByteMask(SC0_A8, 0x00, _BIT1|BIT0);
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
    msWriteByteMask(SC0_A8, 0x00, _BIT1|BIT0);
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


void msSetFRC_LockBase_VCNT(void)
{
    BYTE u8outputVfreq=GetVfreq();
    BYTE u8InputVfreq=V_FREQ_IN;
    BYTE u8GreatestCommonDivisor=0;
    BYTE u8InputRatio=0,u8OutputRatio=0;
    BYTE u8RatioNum=0;
    while( u8outputVfreq> 0 && u8InputVfreq > 0 )
    {
        if( u8outputVfreq > u8InputVfreq )
            u8outputVfreq = u8outputVfreq % u8InputVfreq;
        else
            u8InputVfreq = u8InputVfreq % u8outputVfreq;
    }
    if( u8outputVfreq == 0 )
        u8GreatestCommonDivisor=u8InputVfreq;
    else
        u8GreatestCommonDivisor=u8outputVfreq;
    u8outputVfreq=GetVfreq();
    u8InputVfreq=V_FREQ_IN;
    u8OutputRatio=u8outputVfreq/u8GreatestCommonDivisor;
    u8InputRatio=u8InputVfreq/u8GreatestCommonDivisor;
    //printData(">>>output V %d\n",u8outputVfreq);
    //printData(">>>input V %d\n",u8InputVfreq);
    //printData(">>> maximum CommonDivisor %d\n",u8GreatestCommonDivisor);
    //printData(">>> u8InputRatio %d\n",u8InputRatio);
    //printData(">>> u8OutputRatio %d\n",u8OutputRatio);
    if(u8InputRatio==2 && u8OutputRatio==5)
        u8RatioNum=RatioOfInputVAndOutputVIs0205;
    else if(u8InputRatio==5 && u8OutputRatio==12)
        u8RatioNum=RatioOfInputVAndOutputVIs0512;
    else if(u8InputRatio==1 && u8OutputRatio==2)
        u8RatioNum=RatioOfInputVAndOutputVIs0102;
    else if(u8InputRatio==5 && u8OutputRatio==6)
        u8RatioNum=RatioOfInputVAndOutputVIs0506;
    else if(u8InputRatio==14 && u8OutputRatio==15)
        u8RatioNum=RatioOfInputVAndOutputVIs1415;
    else if(u8InputRatio==65 && u8OutputRatio==66)
        u8RatioNum=RatioOfInputVAndOutputVIs6566;
    else if(u8InputRatio==1 && u8OutputRatio==1)
        u8RatioNum=RatioOfInputVAndOutputVIs0101;
    else if(u8InputRatio==67 && u8OutputRatio==66)
        u8RatioNum=RatioOfInputVAndOutputVIs6766;
    else if(u8InputRatio==13 && u8OutputRatio==12)
        u8RatioNum=RatioOfInputVAndOutputVIs1312;
    else if(u8InputRatio==11 && u8OutputRatio==10)
        u8RatioNum=RatioOfInputVAndOutputVIs1110;
    else if(u8InputRatio==67 && u8OutputRatio==60)
        {
        u8RatioNum=RatioOfInputVAndOutputVIs6760;
    //printData("@@@@@@@@@@@@@@ u8RatioNum %d\n",u8RatioNum);
        }
    else if(u8InputRatio==7 && u8OutputRatio==6)
        u8RatioNum=RatioOfInputVAndOutputVIs0706;
    else if((u8InputRatio==6 && u8OutputRatio==5)||(u8InputRatio==71 && u8OutputRatio==60))
        u8RatioNum=RatioOfInputVAndOutputVIs0605;
    else if(u8InputRatio==5 && u8OutputRatio==4)
        u8RatioNum=RatioOfInputVAndOutputVIs0504;
    else
        {
        u8RatioNum=RatioOfInputVAndOutputVIsNumEnd;
        //printData("######## u8RatioNum %d\n",u8RatioNum);
        }
    //printData(">>> u8RatioNum %d\n",u8RatioNum);
    //printData(">>> g_SetupPathInfo.wImgSizeInV %d\n",g_SetupPathInfo.wImgSizeInV);
    switch(u8RatioNum)
    {
            WORD u16FRCLockBaseVCNT;
            case RatioOfInputVAndOutputVIs0205:  //Ex 24-> 60
                u16FRCLockBaseVCNT=(DWORD)g_SetupPathInfo.wImgSizeInV/2;
                //printData(">>> u16FRCLockBaseVCNT %d\n",u16FRCLockBaseVCNT);
                msWrite2Byte(SC6_18,u16FRCLockBaseVCNT|BIT15);
                break;
            case RatioOfInputVAndOutputVIs0512:  //Ex 25-> 60
                u16FRCLockBaseVCNT=(DWORD)g_SetupPathInfo.wImgSizeInV*19/24;
                //printData(">>> u16FRCLockBaseVCNT %d\n",u16FRCLockBaseVCNT);
                msWrite2Byte(SC6_18,u16FRCLockBaseVCNT|BIT15);
                break;
            case RatioOfInputVAndOutputVIs0102:  //Ex 30-> 60
                u16FRCLockBaseVCNT=(DWORD)g_SetupPathInfo.wImgSizeInV/4;
                //printData(">>> u16FRCLockBaseVCNT %d\n",u16FRCLockBaseVCNT);
                msWrite2Byte(SC6_18,u16FRCLockBaseVCNT|BIT15);
                break;
        case RatioOfInputVAndOutputVIs0506:  //Ex 50-> 60
            u16FRCLockBaseVCNT=(DWORD)g_SetupPathInfo.wImgSizeInV*5/12;
            //printData(">>> u16FRCLockBaseVCNT %d\n",u16FRCLockBaseVCNT);
            msWrite2Byte(SC6_18,u16FRCLockBaseVCNT|BIT15);
            break;
        case RatioOfInputVAndOutputVIs1415:  //Ex 56-> 60
            u16FRCLockBaseVCNT=(DWORD)g_SetupPathInfo.wImgSizeInV-10;
            //printData(">>> u16FRCLockBaseVCNT %d\n",u16FRCLockBaseVCNT);
            msWrite2Byte(SC6_18,u16FRCLockBaseVCNT|BIT15);
            break;
        case RatioOfInputVAndOutputVIs6566:  //Ex 65-> 60
            u16FRCLockBaseVCNT=(DWORD)g_SetupPathInfo.wImgSizeInV-10;
            //printData(">>> u16FRCLockBaseVCNT %d\n",u16FRCLockBaseVCNT);
            msWrite2Byte(SC6_18,u16FRCLockBaseVCNT|BIT15);
            break;
        case RatioOfInputVAndOutputVIs0101:  //Ex 60-> 60
            u16FRCLockBaseVCNT=10;
            //printData(">>> u16FRCLockBaseVCNT %d\n",u16FRCLockBaseVCNT);
            msWrite2Byte(SC6_18,u16FRCLockBaseVCNT|BIT15);
            break;
        case RatioOfInputVAndOutputVIs6766:  //Ex 67-> 60
            u16FRCLockBaseVCNT=10;
            //printData(">>> u16FRCLockBaseVCNT %d\n",u16FRCLockBaseVCNT);
            msWrite2Byte(SC6_18,u16FRCLockBaseVCNT|BIT15);
            break;
        case RatioOfInputVAndOutputVIs1312:  //Ex 65-> 60
            u16FRCLockBaseVCNT=(DWORD)g_SetupPathInfo.wImgSizeInV/24;
            //printData(">>> u16FRCLockBaseVCNT %d\n",u16FRCLockBaseVCNT);
            msWrite2Byte(SC6_18,u16FRCLockBaseVCNT|BIT15);
            break;
        case RatioOfInputVAndOutputVIs1110:  //Ex 66-> 60
            u16FRCLockBaseVCNT=(DWORD)g_SetupPathInfo.wImgSizeInV/20;
            //printData(">>> u16FRCLockBaseVCNT %d\n",u16FRCLockBaseVCNT);
            msWrite2Byte(SC6_18,u16FRCLockBaseVCNT|BIT15);
            break;
        case RatioOfInputVAndOutputVIs6760:  //Ex 67-> 60
            u16FRCLockBaseVCNT=(DWORD)g_SetupPathInfo.wImgSizeInV/20;
            //printData(">>> u16FRCLockBaseVCNT %d\n",u16FRCLockBaseVCNT);
            msWrite2Byte(SC6_18,u16FRCLockBaseVCNT|BIT15);
            break;
        case RatioOfInputVAndOutputVIs0706:  //Ex 70-> 60
            u16FRCLockBaseVCNT=(DWORD)g_SetupPathInfo.wImgSizeInV/12;
            //printData(">>> u16FRCLockBaseVCNT %d\n",u16FRCLockBaseVCNT);
            msWrite2Byte(SC6_18,u16FRCLockBaseVCNT|BIT15);
            break;
        case RatioOfInputVAndOutputVIs0605:  //Ex 72-> 60
            u16FRCLockBaseVCNT=(DWORD)g_SetupPathInfo.wImgSizeInV/10;
            //printData(">>> u16FRCLockBaseVCNT %d\n",u16FRCLockBaseVCNT);
            msWrite2Byte(SC6_18,u16FRCLockBaseVCNT|BIT15);
            break;
        case RatioOfInputVAndOutputVIs0504:  //Ex 75-> 60
            u16FRCLockBaseVCNT=(DWORD)g_SetupPathInfo.wImgSizeInV/8;
            //printData(">>> u16FRCLockBaseVCNT %d\n",u16FRCLockBaseVCNT);
            msWrite2Byte(SC6_18,u16FRCLockBaseVCNT|BIT15);
            break;
            default:
            //printData(">>> default LockBaseVCNT %d\n",(g_SetupPathInfo.wImgSizeInV/2+10));
            msWrite2Byte(SC6_18,(g_SetupPathInfo.wImgSizeInV/2+10)|BIT15);
            break;
    }
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
