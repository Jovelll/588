
///////////////////////////////////////
// DRVMSTAR_U VERSION: V02
////////////////////////////////////////
#include "global.h"
#include "Detect.h"

#ifndef _DRVMSTAR_U_C_
#define _DRVMSTAR_U_C_
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
#if (FRAME_BFF_SEL == FRAME_BUFFER)
    {SC0_27, BIT7 |BIT6 | BIT5 | BIT1}, // enable auto-htotal
#else
    {SC0_27, BIT6 | BIT5 | BIT1}, // enable auto-htotal
#endif
    //{SC0_28, BIT5}, // setup clock mode
    {SC0_49, 0x6F},

    // display timing OP2-2
    {REG_3002, ( PanelVTotal - 1 ) & 0xFF}, //Vtotal
    {REG_3003, ( PanelVTotal - 1 ) >> 8},
    {REG_3004, ( PanelHTotal/PANEL_H_DIV - 1 ) & 0xFF}, //Htotal
    {REG_3005, ( PanelHTotal/PANEL_H_DIV - 1 ) >> 8},
    {REG_3006, 0x00}, //FPLL trig line number <Default 0>
    {REG_3007, 0x00},
    {REG_3008, 0x00}, //FPLL trig pixel number <Default 0>
    {REG_3009, 0x00},
    {REG_300A, ( PanelVTotal - 1 - 3 ) & 0xFF}, //VS line start <-3>
    {REG_300B, ( PanelVTotal - 1 - 3 ) >> 8},
    {REG_300C, ( PanelVTotal - 1 - 2 ) & 0xFF}, //VS line end <-2>
    {REG_300D, ( PanelVTotal - 1 - 2 ) >> 8},
    {REG_3010, 0x00}, //HS pixel start
    {REG_3011, 0x00},
    {REG_3012, ( PanelHSyncWidth/PANEL_H_DIV - 2 ) & 0xFF}, //HS pixel end
    {REG_3013, ( PanelHSyncWidth/PANEL_H_DIV - 2 ) >> 8},

    {REG_3014, 0x00}, //VFDE line start
    {REG_3015, 0x00},
    {REG_3016, ( PANEL_HEIGHT - 1 ) & 0xFF}, //VFDE line end
    {REG_3017, ( PANEL_HEIGHT - 1 ) >> 8},
    {REG_3018, ( PanelHStart/PANEL_H_DIV ) & 0xFF}, //HFDE pixel Start
    {REG_3019, ( PanelHStart/PANEL_H_DIV ) >> 8},
    {REG_301A, ( ( PanelHStart/PANEL_H_DIV ) + (PANEL_WIDTH/PANEL_H_DIV) - 1 ) & 0xFF}, //HFDE pixel End
    {REG_301B, ( ( PanelHStart/PANEL_H_DIV ) + (PANEL_WIDTH/PANEL_H_DIV) - 1 ) >> 8},

    {REG_301C, 0x00}, //VMDE line start
    {REG_301D, 0x00},
    {REG_301E, ( PANEL_HEIGHT - 1 ) & 0xFF}, //VMDE line end
    {REG_301F, ( PANEL_HEIGHT - 1 ) >> 8},
    {REG_3020, ( PanelHStart/PANEL_H_DIV ) & 0xFF}, //HMDE pixel start
    {REG_3021, ( PanelHStart/PANEL_H_DIV ) >> 8},
    {REG_3022, ( ( PanelHStart/PANEL_H_DIV ) + (PANEL_WIDTH/PANEL_H_DIV) - 1 ) & 0xFF}, //HMDE pixel end
    {REG_3023, ( ( PanelHStart/PANEL_H_DIV ) + (PANEL_WIDTH/PANEL_H_DIV) - 1 ) >> 8},

    {REG_302E, 0x00}, //MRW trig x <Default 0>
    {REG_302F, 0x00},
    {REG_3030, ( PanelVTotal - 1 - 3 ) & 0xFF}, //MRW trig y (vtotal -3)
    {REG_3031, ( PanelVTotal - 1 - 3 ) >> 8},
  #if PANEL_OUTPUT_FMT_3D_LR || PANEL_OUTPUT_FMT_3D_CB
    {REG_304D, _BIT7|(HIBYTE( (PANEL_WIDTH/2) - 1 ))},
    {REG_304C, LOBYTE( (PANEL_WIDTH/2) - 1 ) },
    {REG_3050, LOBYTE( (PANEL_WIDTH/4) + 0x0A - 1) },
    {REG_3051, HIBYTE( (PANEL_WIDTH/4) + 0x0A - 1) },
  #endif

  #if PANEL_OUTPUT_FMT_3D_CB
    {REG_30A1, 0x40},
  #endif

    //MRW Init Settings
    {SC6_02, 0x80}, // double buffer enable
  #if FRAME_BFF_SEL == FRAME_BFFLESS// frame buffer less
    {SC6_02, 0xE0}, //[3] FB mode, [5]default 1, [6] MRW access mode:RGB444/RGB565
    {SC6_03, 0x00},
  #else
    {SC6_02, 0xE8}, //[3] FB mode, [5]default 1, [6] MRW access mode:RGB444/RGB565
    {SC6_03, 0x40},
  #endif
    {SC6_06, 0x40}, //MSB write burst length to MIU
    {SC6_07, 0x20}, //LSB write burst length to MIU
    {SC6_A2, 0x10}, // Write Syn FIFO delay 1t
    {SC6_D4, 0x40}, //MSB read burst length to MIU
    {SC6_D5, 0x20}, //LSB read burst length to MIU

    //---Output Control for MOD By Owen---
    {REG_3200, 0x00 }, // sub bank 0, MST5378 need
    {REG_320A,(LVDS_TI_BitModeSel<<2)|(PanelInvDClk&_BIT1)|(PanelTTL&_BIT0 )},
  #if 0 //Jison 110421 disabled, use Power_ModCtrl(_DISABLE) instead
    {REG_3250,0x00}, //enable port a/b/c/d  power on
    {REG_3264,0x1F},//enable clock toggle port a/b/c/d  power on
    {REG_3265, 0x00},
  #endif
    #if(PanelLVDS /*&& PanelDualPort*/)
    {REG_3220, 0x55 }, // output configure for 26 pair output 00: TTL, 01: LVDS/RSDS/mini-LVDS data differential pair, 10: mini-LVDS clock output, 11: RSDS clock output
    {REG_3221, 0x55 }, // output configure for 26 pair output 00: TTL, 01: LVDS/RSDS/mini-LVDS data differential pair, 10: mini-LVDS clock output, 11: RSDS clock output
    {REG_3222, 0x55 }, // output configure for 26 pair output 00: TTL, 01: LVDS/RSDS/mini-LVDS data differential pair, 10: mini-LVDS clock output, 11: RSDS clock output
    {REG_3223, 0x55 }, // output configure for 26 pair output 00: TTL, 01: LVDS/RSDS/mini-LVDS data differential pair, 10: mini-LVDS clock output, 11: RSDS clock output
    {REG_3224, 0x55 },
    {REG_3225, 0x55 },
     //---------------
    {REG_3228, 0xFF }, //enable 0~ 23 paire for LVDS 0x3228 ~ 0x3233
    {REG_3229, 0xFF }, //enable 0~ 23 paire for LVDS
    {REG_322A, 0xFF }, //enable 0~ 23 paire for LVDS
    {REG_322B, 0xFF },
    {REG_322C, 0xFF },
    {REG_322D, 0xFF },
    //----------------
    {REG_3236, 0x00 }, //enable 0~ 23 paire for LVDS 0x3236 ~ 0x3241
    {REG_3237, 0x00 },
    {REG_3238, 0x00 },
    {REG_3239, 0x00 },
    {REG_323A, 0x00 },
    {REG_323B, 0x00 },

    #elif(PanelTTL)
    {REG_3220, 0x00 }, // output configure for 26 pair output 00: TTL, 01: LVDS/RSDS/mini-LVDS data differential pair, 10: mini-LVDS clock output, 11: RSDS clock output
    {REG_3221, 0x00 }, // output configure for 26 pair output 00: TTL, 01: LVDS/RSDS/mini-LVDS data differential pair, 10: mini-LVDS clock output, 11: RSDS clock output
    {REG_3222, 0x00 }, // output configure for 26 pair output 00: TTL, 01: LVDS/RSDS/mini-LVDS data differential pair, 10: mini-LVDS clock output, 11: RSDS clock output
    {REG_3223, 0x00 }, // output configure for 26 pair output 00: TTL, 01: LVDS/RSDS/mini-LVDS data differential pair, 10: mini-LVDS clock output, 11: RSDS clock output
    {REG_3224, 0x00 },
    {REG_3225, 0x00 },
    //----------------
    {REG_3228, 0x00 }, //enable 0~ 23 paire for LVDS 0x3228 ~ 0x3233
    {REG_3229, 0x00 }, //enable 0~ 23 paire for LVDS
    {REG_322A, 0x00 }, //enable 0~ 23 paire for LVDS
    {REG_322B, 0x00 },
    {REG_322C, 0x00 },
    {REG_322D, 0x00 },
    //----------------
    {REG_3236, 0xFF }, //enable 0~ 23 paire for LVDS 0x3236 ~ 0x3241
    {REG_3237, 0xFF },
    {REG_3238, 0xFF },
    {REG_3239, 0xFF },
    {REG_323A, 0xFF },
    {REG_323B, 0xFF },

    {REG_3242, 0xFF },  //pre-emphasis enable
    {REG_3243, 0xFF },  //pre-emphasis enable
    {REG_3244, 0x0F },  //pre-emphasis enable
    #endif

    {REG_3266, 0x00}, //pre - emphasis a/b/c/d port default off

    // FFC
    {REG_320B,(LVDS_TIMode&BIT6)|(BIT5)|(PANEL_SWAP_PN&BIT3)|(PAIR_SWAP_B&BIT2)}, // setup

    {REG_3226, 0x00},//  Cha R/B , R/G, B/R SWAP
    {REG_3227, 0x00}, //chb/c/d R/B , R/G , B/G swap

    // output signal control 0xF2
    {
      REG_3202,(PanelInvDE&BIT7) |( PanelInvVSync&BIT6 ) | ( PanelInvHSync&BIT5) | BIT0
    }, // output signal polarity control, [0]:default 1
    {
      REG_3203, (PANEL_QUAD_PORT<<1) | (PANEL_SWAP_AB_PORT << 2) |(PANEL_SWAP_CD_PORT<<3)
    }, // outpu
    {REG_321F, (LVDS_CH_D_SWAP<<6)|(LVDS_CH_C_SWAP<<4)|(LVDS_CH_B_SWAP<<2)|(LVDS_CH_A_SWAP)},
  #if ( !PanelDualPort && !PANEL_QUAD_PORT) // Single Port
    {REG_3274,0x80},
  #else
    {REG_3274,0x00},
  #endif

    {SC0_6C, 0x90}, // short line tune coef. // 120831 coding, modified default value 0x70 to 0x90

    // Clock generator control 0xD0~0xDB
    {SC0_D0, 0x00},
    {REG_PM_E9, 0x00},
    //{REG_1EDC, 0x00}, //Jison110311 move to mcuSetSystemSpeed()
    //{REG_1ED1, BIT7}, // setup output pll & master pll  //// 20071219 RD Anson suggest for B1/B2/Goya enable Bit7 fixed line clock in 172Mhz
    //{REG_1ED1, BIT2}, //Jison110311 move to mcuSetSystemSpeed()
    // Master PLL
    //{MPLL_M, 0x6F},//0x72:12MHz // set master pll as 215 MHz & drive current
    //{REG_1ED2, 0x0F}, //Jison110311 move to mcuSetSystemSpeed()

    // output dclk setting
    //{LPLL_M, 0x48},//0x02},//0x42}, // output pll spread spectrum control
    {REG_1E26, 0x00},

    //FPLL Fsync Init Settings
    {REG_380B, 0x0A}, //phase lock thresh
    {REG_380C, 0x00}, //limit for clock freq correction modification(d5d6d7)
    {REG_380D, 0x00},
    {REG_380E, 0x01},
    {REG_380F, 0x00},
    {REG_3810, 0x00}, //limit for phase correction modification(d5d6d7)
    {REG_3811, 0x00},
    {REG_3812, 0x01},
    {REG_3813, 0x00},
    {REG_3814, 0x00}, //limit for lpll phase offset
    {REG_3815, 0xC0},
    {REG_381A, 0x01}, //[0] S.W. force prd stable check ok
                      //[1] S.W. force prd lock done
                      //[2] S.W. force phase reduce done
                      //[3] S.W. force phase close done
                      //[4] enable 2 limit
  #ifdef _HW_AUTO_NO_SIGNAL_
    {REG_381B, 0x09},
  #else
    {REG_381B, 0x08}, //[0] frame pll disable when no_signal flag high          //111012 Rick modified for enable SSC function
                      //[1] frame pll disable when non_signal flag high
                      //[2] select ori ovs as lock  prd reference
                      //[3] SSC mode enable
  #endif
    {REG_3832, 0xFF}, //input dclock divider for prd sync
    {REG_3833, 0x03},
    {REG_3834, 0x00},
    {REG_3835, 0x00},
    {REG_3836, 0xFF}, //output dclock divider for prd sync
    {REG_3837, 0x01},
    {REG_3838, 0x00},
    {REG_3839, 0x00},
  #if (OD_MODE == OD_MODE_333)
    {REG_387B, 0x40},
  #endif
    {REG_38E0, 0x80}, //fsync phase offset threshold
    {REG_38E1, 0x00}, //[0] fsync reset multi-times flag
                      //[1] clear fsync reset multi-times flag
                      //[3] enable fsync reset signal generate
                      //[7] enable lock freq mode
  #if FRAME_BFF_SEL == FRAME_BFFLESS // frame buffer less
    {REG_3818, 0x00}, //[1:0] FPLL mode, 00:lock phase mode
                      //[3] frame lpll enable
                      //[4] disable i_gain for lock prd
                      //[5] disable p_gain for lock prd
                      //[6] disable i_gain for lock phase
                      //[7] disable p_gain for lock phase
    {SC0_28  , 0x40}, //[6] short line tune enable
    {SC0_9F  , 0x00}, //[0] OP2-1 output selection
  #else
    {REG_3818, 0x00}, //[3] frame lpll enable
    {SC0_63  , BIT2}, //[2] short frame force write, [3] 0:disable
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
    //{SC0_2A, BIT0}, // disable birghtness control
    // contrast 0x2E~0x31
  #if FRAME_BFF_SEL == FRAME_BFFLESS
    {SC0_2E, 0x60 }, // enable contrast function and set 0 as center
  #else
    {SC0_2E, 0x60 | BIT7}, // enable contrast function and set 0 as center, [7]: avoid Htotal zero
  #endif

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

    ///???
    {SC3_17,0x06   },//[1]: mwe_start_is_even (MWE window H start is even)
                    //[2]:mwe_end_is_even (MWE window H end is even)
  #if 1  //????
    {SCE_02,0x20   },//BIT5: Main win POST contrast enable
    #if 0 //Jison 110323 move ACE InitialPeaking()
    {SC7_20,0x01   },//BIT0: Main win Peaking enable
    {SC7_3B,0x01   },//BIT0: Sub win Peaking enable
    #endif
  #endif
  #if ENABLE_DP_INPUT
    {REG_356C, 0x00},
    {REG_376D, 0x00},
    {REG_3580, 0x00},
   // {REG_37B3, 0x00}, //It can't set 0. the register is different with Songlu. it must be default value.
    {REG_35D1, 0x00},
    {REG_37E8, 0x00},
    {REG_37E9, 0x00},
    {REG_37EA, 0x00},
    {REG_37EB, 0x00},
  #endif

    {SC0_00, 0x00},
};

code RegUnitType tb_3D_DITHER_TABLE[] =
{
    {SC3_D0, 0x19},
    {SC3_D1, 0x00},
    {SC3_D2, 0x99},
    {SC3_D3, 0x27},
    {SC3_D4, 0x8D},
    {SC3_D5, 0x63},
    {SC3_D6, 0x9C},
    {SC3_D7, 0x4E},
    {SC3_D8, 0x4B},
    {SC3_D9, 0x93},
    {SC3_DA, 0x39},
    {SC3_DB, 0x0C},

    {SC0_00, 0x00},
};

void msInitClockGating( void )
{
    msWriteByteMask( REG_3881, 0x00, 0x01);
  #if FRAME_BFF_SEL == FRAME_BFFLESS
    msWriteByteMask( REG_1133, 0x00, 0xE0); // OD rst circuit need miu clock
  #endif
    msWriteByteMask( REG_1E24, 0x00, 0x0F );    // osd clk
    msWriteByteMask( REG_1E27, 0x00, 0xF0 );
#if (ENABLE_RTE && (OD_MODE == OD_MODE_333) && OD_HSCAL_EN)
    msWriteByteMask( REG_1E28, 0x10, 0x3F );    // od clk
    msWriteByteMask( REG_1E29, 0x14, 0x3F );    // od hsd clk
#elif (ENABLE_ROTATE_CLK_SET) || (!PanelDualPort && !PANEL_QUAD_PORT) || (PanelTTL)
    msWriteByteMask( REG_1E28, 0x14, 0x3F );    // od clk
    msWriteByteMask( REG_1E29, 0x14, 0x3F );    // od hsd clk
#else
    msWriteByteMask( REG_1E28, 0x10, 0x3F );    // od clk
    msWriteByteMask( REG_1E29, 0x10, 0x3F);
#endif
    msWriteByteMask( REG_1E2A, 0x10, 0x3F );    // od sram0 clk
    msWriteByteMask( REG_1E2B, 0x10, 0x3F );    // od sram1 clk
    msWriteByteMask( REG_1E2C, 0x10, 0x3F );    // od sram2 clk
    msWriteByteMask( REG_1E35, 0x28, 0x3F );    // fclk (LB clk)
    msWriteByteMask( REG_1E36, 0x20, 0x3F );    // odclk2
    msWriteByteMask( REG_1E37, 0x20, 0x3F );    // odclk1
    msWriteByteMask( REG_1E3E, 0x08, 0x0F );    // idclk
    msWriteByteMask( REG_1E3F, 0x04, 0x0F );    // hsp clk
    msWriteByteMask( REG_1E40, 0x00, 0x0F );    // idclk 1to2
    msWriteByteMask( REG_1E40, 0x00, 0xF0 );    // lbw
    msWriteByteMask( REG_1E41, 0x10, 0x3F);
    msWriteByteMask( REG_1E44, 0x00, 0x0F );    // lbr
    msWriteByteMask( REG_1E46, 0x04, 0xFF );    // mrw fclk:iclk
    msWriteByteMask( REG_1E47, 0x04, 0x0F );    // sysclk_ip
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
    //msWriteByteMask( REG_PM_B2, 0, BIT2 ); // power on DVI CKIN. (port 0 and port 1) (reg_b2[2])
    // DVI enable
    #if (DVI_PORT && DVI_PORT==TMDS_PORT_C) || (HDMI_PORT && HDMI_PORT==TMDS_PORT_C)
    #define PORT_C_EN   BIT1
    #else
    #define PORT_C_EN   0
    #endif
    #if ((DVI_PORT && DVI_PORT!=TMDS_PORT_C) || (HDMI_PORT && HDMI_PORT!=TMDS_PORT_C) )
    #define PORT_AB_EN   BIT0
    #else
    #define PORT_AB_EN   0
    #endif

    //patch for HDCP issue, reset HDCP FSM when no input clock
    msWriteByteMask( REG_28D2, BIT2, BIT2 );
    msWriteByteMask( REG_3BD2, BIT2, BIT2 );

    msWriteByteMask( REG_2900, PORT_C_EN|PORT_AB_EN, 0x03 ); // [1]: DVI C enable, [0]: DVI A/B enable
    // Power up Xtal clocks and DVI detection clock for DVI clock detection
    //msWrite2ByteMask( REG_290C, 0, 0xFFFF );
    // DVI PLL power control
    //msWrite2ByteMask( REG_29C0, 0x1000, 0xFFFF ); // enable DVI0 PLL power, [12:11]: portB/A Data R-term
    //msWrite2ByteMask( REG_29D2, 0, 0x8FF7 ); // enable DVI1 PLL power, [11]: port C Data R-term

    // Enable DVI phase accumulator extension and clock stable gate
    //msWriteByteMask( REG_2800, BIT2|BIT1, BIT2|BIT1 ); // [2]:Enable of acc8lsb extension ; [1]:Enable of acc8lsb extension select
    //msWriteByteMask( REG_3B00, BIT2|BIT1, BIT2|BIT1 ); // [2]:Enable of acc8lsb extension ; [1]:Enable of acc8lsb extension select
    //msWriteByteMask( REG_2841, BIT2, BIT2); // [2]:Enable the function that clock gen gate signal controlled by TMDS clock stable
    //msWriteByteMask( REG_3B41, BIT2, BIT2); // [2]:Enable the function that clock gen gate signal controlled by TMDS clock stable
    // DVI slowly update
    //msWriteByteMask( REG_2853, 0xE3, 0xFF ); // [15]:update Bch slowly; [14:8]: 0x63(100 lines)
    //msWrite2ByteMask( REG_2854, 0xE3E3, 0xFFFF ); // [15:8]: update Rch slowly, [7:0]:update Gch slowly
    //msWriteByteMask( REG_3B53, 0xE3, 0xFF ); // [15]:update Bch slowly; [14:8]: 0x63(100 lines)
    //msWrite2ByteMask( REG_3B54, 0xE3E3, 0xFFFF ); // [15:8]: update Rch slowly, [7:0]:update Gch slowly
    // enable Vsync glitch filter
    msWriteByteMask( REG_283C, BIT4, BIT4 ); // enable DVI port0 Vsync glitch filter
    msWriteByteMask( REG_3B3C, BIT4, BIT4 ); // enable DVI port0 Vsync glitch filter
    // Enable DVI clock tolerance: 4*0.7 = 2.8 MHz
    msWriteByteMask( REG_2827, 0x20, 0x70 ); //the comp value is reg_comp_tol * 4
    msWriteByteMask( REG_2825, 0xF0, 0xF0 ); //0x110A25=0xB0, DVI clock detection counter tolerance set to 7
    msWriteByteMask( REG_3B27, 0x20, 0x70 ); //the comp value is reg_comp_tol * 4
    msWriteByteMask( REG_3B25, 0xF0, 0xF0 ); //0x110A25=0xB0, DVI clock detection counter tolerance set to 7
    msWriteByteMask( REG_2852, BIT4, BIT4); // enable auto DVIPLL adjust
    msWriteByteMask( REG_3B52, BIT4, BIT4); // enable auto DVIPLL adjust

    // HDMI setting
#if ENABLE_HDMI //ENABLE_HDMI_INPUT //coding
    msWriteByte( REG_050D, 0xCA ); // [7]: Output black color in video blanking, [6]: GCP CD val, [3]:Video mute when AVMUTE, [1]: HDMI/DVI auto detect
    msWriteByte( REG_050E, 0x03 ); // [1:0]: deep color mode enable
    //msWriteByteMask(REG_0510, 0, BIT4|BIT3); // [4]:0: select pixel clock, [3]:0: manuel pixel clock mode
    msWriteByteMask( REG_0510, BIT3, BIT4 | BIT3 ); // [3]:1: auto pixel clock mode - Montior series
  #if 1//EN_HDMI_1_3
    //#if (CHIP_ID == CHIP_TSUML || CHIP_ID==CHIP_TSUMU) // only for Seurat
    msWriteByteMask( REG_05B9, 0, BIT0 ); // [0]: HPLL power on
    //#endif
    msWriteByteMask( REG_05F4, BIT5, BIT5 ); // [5]: Enable video clock 2x for HDMI deep color 12 bits
  #endif // #if EN_HDMI_1_3
    msWriteByte( REG_05C8, 0x8A ); // [7]: Frame repetition manual mode; [3]: auto DSD detection; [1]: auto PCM detection
    msWriteByte( REG_05CC, 0xD1 ); // [7]: mute pin to normal(H->L), (set active low for this DAC); [6]:Disable audio mute output; [4]: Global audio mute; [1:0]: 1/4 audio FIFO
    msWriteByte( REG_05CD, 0x0A ); // [7:6]: audio ch 12/34/56/78, [4]: DP/HDMI audio, [3]: auto DSD detection; [1]: auto PCM detection
    msWriteByte( REG_05CF, 0xC0 ); // Fading gain = 0dB
  #if 0 // ENABLE_AUDIO_AUTO_MUTE // auto mute setting //coding
    msWriteByte( REG_05D3, 0x7F ); // Fading events: [6]:HDMI video clock change, [5]: no input clock, [4]: HDMI CTS/N over range, [3]: HDMI AVMUTE, [2]: non-PCM, [1:0]:ASP error
    msWriteByte( REG_05D4, 0x02 ); // Fading events: [9]:flat bit
  #endif
  #if 0 //ENABLE_AUDIO_AUTO_FADING // auto fading setting //coding
    msWriteByte( REG_05CE, 0x40 ); // [6:5]: Automatic fading mode
    msWriteByte( REG_05D0, 0xFF ); // Zero crossing threshold = 0xFF
    msWriteByte( REG_05D1, 0xFF ); // Zero crossing threshold = 0xFF
    msWriteByte( REG_05D5, 0x7F ); // Fading events: [6]:HDMI video clock change, [5]: no input clock, [4]: HDMI CTS/N over range, [3]: HDMI AVMUTE, [2]: non-PCM, [1:0]:ASP error
    msWriteByte( REG_05D6, 0x02 ); // Fading events: [9]:flat bit
  #endif
    // I2S and S/PDIF setting
    msWriteByte( REG_05E4, 0x13 ); // [4]: New mode, [1]: Enable CTSN synthesizer clock, [0]: Enable audio FIFO read clock.
    msWriteByte( REG_05E5, 0x6A ); // [7:6]:128Fs, [5]:MCLK output, [3]:I2S encoder clock, [1]:S/PDIF encoder clock
    msWriteByte( REG_05E6, 0x39 ); // [6:5]:24bits to DAC, [4]:enable DAC 256Fs clock, [3]: I2S left-justified, [1:0]:24 bits to I2S
    msWriteByte( REG_05E7, 0x07 ); // [2]:AUMUTE pin, [1]:S/PDIF, [0]:I2S enable
    msWriteByteMask( REG_05F0, 0, BIT3 ); // [3]: AULL power down
    msWriteByte( REG_05F1, 0x0A ); // [3]:FBCLK enable, [1]:DCLK enable
    msWriteByte( REG_05F7, 0x26 ); // [7:4]:DDIV = 2, [3:0]:FBDIV = 6
    msWriteByte( REG_05F8, 0x81 ); // [7]: Enable CTS-N synthesizer, [0]: CTS[19:0]
  #if 0//EN_HDMI_NEW_PIXEL_REPETITION
    msWriteByteMask( SC0_7A, BIT7, BIT7 ); // [7]: HDMI 444 pixel repetition mode enable
  #endif
    mstar_HDMIAudioMute(HDMI_AUDIO_OFF);
#endif // ENABLE_HDMI_INPUT

#if 0 //ENABLE_DVI_AUTO_EQ //coding
    msWriteByteMask( REG_2800, BIT4, BIT4 ); // enable EQ new mode
    msWriteByte( REG_28A0, 0x80 ); // [7]: angle overwirte mode, [1]:disable manual EQ mode
    msWriteByte( REG_28A1, 0xA0 ); // [7]: check good phase and error rate, [5:4]: 32 phases
    msWriteByteMask( REG_28A5, 0x30, 0xF0 ); // [7:4]: Continuous good phases threshold = 3
    msWriteByte( REG_28AE, 0x8F ); // [7]: Time out enable, [6:5]: 64ms timeout,  [3]: waive enable, [2]: 2nd eye, [1]: unused DE, [0]: DE de-glitch
    msWriteByte( REG_28AF, 0x3F ); // de-glitch length = 0x3F(pixels)
    msWriteByte( REG_285E, 0x89 ); // enable auto EQ trigger
#endif
    //msTMDSInitialVariable(); //coding

}

void drvmStar_Init( void )
{
    BYTE i;

    msInitClockGating();

    msTMDSInit();

    msWriteByteMask(SC0_02, BIT7, BIT7);

    for( i = 0; i < sizeof( tblInit ) / sizeof( RegUnitType ); i++ )
        msWriteByte( tblInit[i].u16Reg, tblInit[i].u8Value );

    for( i = 0; i < sizeof( tb_3D_DITHER_TABLE ) / sizeof( RegUnitType ); i++ )
        msWriteByte( tb_3D_DITHER_TABLE[i].u16Reg, tb_3D_DITHER_TABLE[i].u8Value );

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

    msWriteByteMask( SC0_48, 0, BIT4 );                     //Turn off New mode, (Show line tune fast)

#if DISABLE_SHORT_LINE_TUNE
    msWriteByteMask(SC0_63, BIT0,  BIT1|BIT0);
    msWriteByteMask(SC0_28,    0,  BIT6);
#endif

#if DISABLE_SHORT_FRAME
    msWriteByteMask(SC0_63, BIT2, BIT2);
    msWriteByteMask(SC0_63, 0, BIT3);
#endif

    msWriteByteMask(SC0_0E, BIT7, BIT7); // H coring disable, for dot pattern certical block issue, Jison 100610
    mStar_SetupPathInit(); //Jison need put here because mStar_SetupFreeRunMode() will use g_SetupPathInfo variable

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
#if Enable_Expansion && (FRAME_BFF_SEL==FRAME_BUFFER)//ENABLE_WIDE_MODE && (FRAME_BFF_SEL==FRAME_BUFFER && ASPECT_RATIO_CASE34)
        if (!EXPANSION_MODE_IS_FULL())//(UserprefExpansionMode!=Expansion_Full)//(USER_PREF_WIDE_MODE!=WM_WIDE) //Jison 110516
            msWriteByteMask(SC0_0F,0,BIT7); //shift line mode disable
        else
#endif
        {
            msWriteByteMask(SC0_0F,BIT7,BIT7); //shift line mode enable
        }
        if(CURRENT_INPUT_IS_VGA())//(SrcInputType == Input_Analog1)
        {
            if(msReadByte(SC0_ED)&BIT5)
               msWriteByteMask(SC0_E9,0,BIT0);
            else
               msWriteByteMask(SC0_E9,BIT0,BIT0);
        }
        else
        {
            if(msReadByte(SC0_04)&BIT6)
                {
            msWriteByteMask(SC0_E9,0,BIT0);//0x01); //Jison 080925 for DVI interlace //0x00 090512
                     msWriteByteMask(SC0_05,0,BIT0);
                }
            else
                {
                     msWriteByteMask(SC0_E9,BIT0,BIT0);
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
    DWORD xdata u32MPLL_MHZ;

    if( (freqMod == 0) || (range == 0) )
    {
        msWrite2Byte( REG_382E, 0 );
        msWrite2Byte( REG_3830, 0 );
        return;
    }

    // freqMod: 0~40 means 0~40K
    if(freqMod > PANEL_SSC_MODULATION_MAX)
        freqMod = PANEL_SSC_MODULATION_MAX;

    // range: 0~30 means 0~3%
    if(range > PANEL_SSC_PERCENTAGE_MAX)
        range = PANEL_SSC_PERCENTAGE_MAX;

    u32MPLL_MHZ = ( msReadByte(REG_1E37) & BIT7 )? 214ul : 429ul;  // 214.77MHz or 429.54MHz

    dwFactor = msReadByte( REG_3852 );
    dwFactor <<= 16;
    dwFactor |= msRead2Byte( REG_3850 );

    //wSpan = ((14318ul*30/freqMod) * 131072ul) / dwFactor;
    //wStep = (float)(((dwFactor*freqMod*range/10)/14318ul)*(dwFactor/1500))/131072; // 120320 coding modified
    wSpan = ((u32MPLL_MHZ*1024ul/freqMod) * 131072ul) / dwFactor;
    wStep = (float)(dwFactor*range/10)/wSpan/100;

    if (wStep > 0x3FF)
        wStep = 0x3FF;

    if(wSpan > 0x3FFF)
        wSpan = 0x3FFF;

    //MST_printData("PanelSSC STEP[%x]",wStep);
    //MST_printData("PanelSSC SPAN[%x]",wSpan);

    msWrite2Byte( REG_382E, wStep );
    msWrite2Byte( REG_3830, wSpan );
    msWriteBit(REG_381B, _ENABLE, BIT3);

}

// 111220 coding test
void drvDVI_PowerCtrl(DVI_PowerCtrlType ctrl)
{
    if(ctrl == DVI_POWER_STANDBY)
    {
        // DVI power on
        msWriteByteMask( REG_PM_B3, 0, BIT2 ); // power on DVI CKIN. (port C) (reg_b3[2])
        msWriteByteMask( REG_PM_B2, 0, BIT2 ); // power on DVI CKIN. (port 0 and port 1) (reg_b2[2])
        msWrite2ByteMask( REG_290C, 0, 0xFFFF ); // powe on DVI
        // DVI PLL power control
        msWrite2ByteMask( REG_29C0, 0xFFFF, 0xFFFF ); // enable DVI0 PLL power, [12:11]: portB/A Data R-term
        msWrite2ByteMask( REG_29D2, 0x8FF7, 0x8FF7 ); // enable DVI1 PLL power, [11]: port C Data R-term
    }
    else if(ctrl == DVI_POWER_DOWN)
    {
        // DVI power down:
        msWriteByteMask( REG_PM_B3, BIT2, BIT2 ); // power down DVI CKIN. (port C) (reg_b3[2])
        msWriteByteMask( REG_PM_B2, BIT2, BIT2 ); // power down DVI CKIN. (port 0 and port 1) (reg_b2[2])
        // Power up Xtal clocks and DVI detection clock for DVI clock detection
        msWrite2ByteMask( REG_290C, 0x7FFE, 0xFFFF );
        // DVI PLL power control
        msWrite2ByteMask( REG_29C0, 0xFFFF, 0xFFFF ); // enable DVI0 PLL power, [12:11]: portB/A Data R-term
        msWrite2ByteMask( REG_29D2, 0x8FF7, 0x8FF7 ); // enable DVI1 PLL power, [11]: port C Data R-term
    }
    else
    {
        // DVI power on
        msWriteByteMask( REG_PM_B3, 0, BIT2 ); // power on DVI CKIN. (port C) (reg_b3[2])
        msWriteByteMask( REG_PM_B2, 0, BIT2 ); // power on DVI CKIN. (port 0 and port 1) (reg_b2[2])
        msWrite2ByteMask( REG_290C, 0, 0xFFFF ); // powe on DVI
        // DVI PLL power control
        msWrite2ByteMask( REG_29C0, 0, 0xFFFF ); // enable DVI0 PLL power, [12:11]: portB/A Data R-term
        msWrite2ByteMask( REG_29D2, 0, 0x8FF7 ); // enable DVI1 PLL power, [11]: port C Data R-term
    }
}
void mStar_IPPowerControl(void)
{
    if (CURRENT_INPUT_IS_VGA())//( (SrcInputType == Input_Analog1) ) // ADC
    {
        // ADC power on:
        drvADC_PowerCtrl(ADC_POWER_ON);

  #ifdef _IP_POWER_CTRL_
        drvDVI_PowerCtrl(DVI_POWER_DOWN);
  #endif
    }
    else if( CURRENT_INPUT_IS_DVI()//(SrcInputType == Input_Digital)
        #if ENABLE_HDMI
        || CURRENT_INPUT_IS_HDMI()//(SrcInputType == Input_HDMI)
        #endif // #if Enable_DVI2
    ) // DVI / HDMI
    {
  #ifdef _IP_POWER_CTRL_
        drvDVI_PowerCtrl(DVI_POWER_ON);
  #endif
        // ADC power down
        drvADC_PowerCtrl(ADC_POWER_DOWN);
    }
#if ENABLE_DP_INPUT
    else if(CURRENT_INPUT_IS_DISPLAYPORT())//(SrcInputType == Input_Displayport)
    {
        // ADC power down
        drvADC_PowerCtrl(ADC_POWER_DOWN);

  #ifdef _IP_POWER_CTRL_
        drvDVI_PowerCtrl(DVI_POWER_DOWN);
  #endif
    }
#endif // #if INPUT_DISPLAYPORT

}

void msTMDSSetMux( InputPortType inport )
{
    if(INPUT_IS_DVI(inport))//( inport == Input_Digital )
    {
      #if ENABLE_DVI_DUAL_LINK
        //power down port a CKIN to avoid couple
        msWriteByteMask( REG_PM_B2,    BIT2, BIT2 ); // power down DVI CKIN. (port A/B) (reg_b2[2])
        msWriteByteMask( REG_PM_B3,    0, BIT2 ); // power on DVI CKIN. (port C) (reg_b3[2])
      #elif DVI_PORT==TMDS_PORT_C
        msWriteByteMask( REG_PM_B2, BIT2, BIT2 ); // power on DVI CKIN. (port A/B) (reg_b2[2])
        msWriteByteMask( REG_PM_B3,    0, BIT2 ); // power on DVI CKIN. (port C) (reg_b3[2])
      #else //PORT A/B
        msWriteByteMask( REG_PM_B2,    0, BIT2 ); // power on DVI CKIN. (port A/B) (reg_b2[2])
        msWriteByteMask( REG_PM_B3, BIT2, BIT2 ); // power on DVI CKIN. (port C) (reg_b3[2])
      #endif
        // Power up Xtal clocks and DVI detection clock for DVI clock detection
        msWrite2ByteMask( REG_290C, 0, 0xFFFF );
  #if ENABLE_DVI_DUAL_LINK
    #if DVI_MASTER_PORT==TMDS_PORT_C && DVI_SLAVE_PORT==TMDS_PORT_A // DVI Dual-link - PortC+A
        // DVI PLL power control
        msWrite2ByteMask( REG_29C0, 0x1000, 0xFFFF ); // enable DVI0 PLL power, [12:11]: portB/A Data R-term
        msWrite2ByteMask( REG_29D2, 0, 0x8FF7 ); // enable DVI1 PLL power, [11]: port C Data R-term
        msWriteByteMask( REG_29D4, BIT0, BIT0); // [0]: enable DVI port 2
        msWriteByteMask( REG_PM_B3, BIT4, BIT5 | BIT4 ); //DVI clock mux: [5]: port 1(portB), [4]: port 0(portA)
        msWriteByteMask( REG_29C2, BIT2|BIT0, 0x0F); // Dual link primary/secondary:[3]: 0: port C clock from itself [2]: 1: port A/B clock is from port C, [1:0] port A/B data mux: 10: port B; 01: port A
        //msWriteByteMask( REG_2800, 0, BIT7|BIT6 ); // [7]: ABSWAP, [6]: ignore_dual_en
        //msWriteByteMask( REG_2801, 0, BIT6|BIT5 ); // [6]: DVI_linkb_en, [5]: DVI_dual_manual_en
        msWriteByteMask( REG_2800, BIT6, BIT7|BIT6 ); // [7]: ABSWAP, [6]: ignore_dual_en
        msWriteByteMask( REG_2801, BIT6|BIT5, BIT6|BIT5 ); // [6]: DVI_linkb_en, [5]: DVI_dual_manual_en
        msWriteByteMask( REG_2860, BIT5|BIT4, BIT6|BIT5|BIT4 ); // [6]: DVI_dual_b_ov_en, [5]: DVI_dual_c_ov_en, [4]: DVI_dual_ov_en
      #if 0 // DDC link is from port A for ISP
        msWriteByteMask( REG_28F8, 0x01, 0x0F);  // HDCP0: [3]: response 0x74 only, [2]:DDC linkA - for ISP, [1]: primary, [0]: dual-link
        msWriteByteMask( REG_3BF8, 0x03, 0x0F);  // HDCP1: [3]: response 0x74 or 0x76, [2]:DDC linkA - f0r ISP, [1]: secondary, [0]: dual-link
      #else // DDC link is from port C
        msWriteByteMask( REG_28F8, 0x05, 0x0F);  // HDCP0: [3]: response 0x74 only, [2]:DDC linkC, [1]: primary, [0]: dual-link
        #if 1//Jison 110505 for DVI dual link HDCP
        msWriteByteMask( REG_3BF8, 0x37, 0x3F);  // HDCP1: [3]: response 0x74 or 0x76, [2]:DDC linkC, [1]: secondary, [0]: dual-link
        #else
        msWriteByteMask( REG_3BF8, 0x07, 0x0F);  // HDCP1: [3]: response 0x74 or 0x76, [2]:DDC linkC, [1]: secondary, [0]: dual-link
        #endif
      #endif
    #else
    #endif
  #else
        // DVI PLL power control
      #if DVI_PORT==TMDS_PORT_C
        msWrite2ByteMask( REG_29D2, 0, 0x8FF7 ); // enable DVI1 PLL power, [11]: port C Data R-term
        msWriteByteMask( REG_29D4, BIT0, BIT0); // [0]: enable DVI port 2
        msWriteByteMask( REG_PM_B3, BIT4, BIT5 | BIT4 ); //DVI clock mux: [5]: port 1(portB), [4]: port 0(portA)
        msWriteByteMask( REG_29C2, 0, BIT3); // Dual link primary/secondary:[3]: 0: port C clock from itself [2]: 1: port A/B clock is from port C, [1:0] port A/B data mux: 10: port B; 01: port A
        msWrite2ByteMask( REG_29C0, 0x9FF7, 0x9FF7 ); // enable DVI0 PLL power, [12:11]: portB/A Data R-term
        msWriteByteMask( REG_2800, BIT6, BIT7|BIT6 ); // [7]: ABSWAP, [6]: ignore_dual_en
      #elif DVI_PORT==TMDS_PORT_B
        msWrite2ByteMask( REG_29C0, 0x0800, 0xFFFF ); // enable DVI0 PLL power, [12:11]: portB/A Data R-term
        msWriteByteMask( REG_PM_B3, BIT5, BIT5 | BIT4 ); //DVI clock mux: [5]: port 1(portB), [4]: port 0(portA)
        msWriteByteMask( REG_29C2, BIT1, 0x0F); // Dual link primary/secondary:[3]: 0: port C clock from itself [2]: 0: port A/B clock is from itself, [1:0] port A/B data mux: 10: port B; 01: port A
        msWriteByteMask( REG_29D4, 0, BIT0); // [0]: enable DVI port 2
        msWrite2ByteMask( REG_29D2, 0x8FF7, 0x8FF7 ); // enable DVI1 PLL power, [11]: port C Data R-term
        msWriteByteMask( REG_2800, BIT7, BIT7|BIT6 ); // [7]: ABSWAP, [6]: ignore_dual_en //????
      #else //TMDS_PORT_A
        msWrite2ByteMask( REG_29C0, 0x1000, 0xFFFF ); // enable DVI0 PLL power, [12:11]: portB/A Data R-term
        msWriteByteMask( REG_PM_B3, BIT4, BIT5 | BIT4 ); //DVI clock mux: [5]: port 1(portB), [4]: port 0(portA)
        msWriteByteMask( REG_29C2, BIT0, 0x0F); // Dual link primary/secondary:[3]: 0: port C clock from itself [2]: 0: port A/B clock is from itself, [1:0] port A/B data mux: 10: port B; 01: port A
        msWriteByteMask( REG_29D4, 0, BIT0); // [0]: enable DVI port 2
        msWrite2ByteMask( REG_29D2, 0x8FF7, 0x8FF7 ); // enable DVI1 PLL power, [11]: port C Data R-term
        msWriteByteMask( REG_2800, BIT7, BIT7|BIT6 ); // [7]: ABSWAP, [6]: ignore_dual_en //????
      #endif
        #if defined(_BD_MST_MST8556T_H) //clr to 0 if not use demo board????
        msWriteByteMask( REG_2801, BIT6|BIT5, BIT6|BIT5 ); // [6]: DVI_linkb_en, [5]: DVI_dual_manual_en
        msWriteByteMask( REG_2860, BIT5|BIT4, BIT6|BIT5|BIT4 ); // [6]: DVI_dual_b_ov_en, [5]: DVI_dual_c_ov_en, [4]: DVI_dual_ov_en
        #else
        msWriteByteMask( REG_2801, 0, BIT6|BIT5 ); // [6]: DVI_linkb_en, [5]: DVI_dual_manual_en
        msWriteByteMask( REG_2860, 0, BIT6|BIT5|BIT4 ); // [6]: DVI_dual_b_ov_en, [5]: DVI_dual_c_ov_en, [4]: DVI_dual_ov_en
        #endif
        msWriteByteMask( REG_28F8, 0x08, 0x0F);  // HDCP0: [3]: response 0x74 only, [2]:DDC linkC, [1]: primary, [0]: single-link
        msWriteByteMask( REG_3BF8, 0x08, 0x0F);  // HDCP1: [3]: response 0x74 only, [2]:DDC linkC, [1]: primary, [0]: single-link
        //msWriteByteMask( REG_3BF8, 0x00, 0x0F);  // HDCP1: [3]: response 0x74 or 0x76, [2]:DDC linkA, [1]: secondary, [0]: signle-link
  #endif // #if ENABLE_DVI_DUAL_LINK
    }
  #if ENABLE_HDMI
    else if(INPUT_IS_HDMI(inport))//( inport == Input_HDMI ) // HDMI - Port B
    {
        #if HDMI_PORT==TMDS_PORT_C
        msWriteByteMask( REG_PM_B3, 0, BIT2 ); // power down DVI CKIN. (port C) (reg_b3[2])
        msWriteByteMask( REG_PM_B2, BIT2, BIT2 ); // power on DVI CKIN. (port A/B) (reg_b2[2])
        #else //PORT A/B
        msWriteByteMask( REG_PM_B3, BIT2, BIT2 ); // power down DVI CKIN. (port C) (reg_b3[2])
        msWriteByteMask( REG_PM_B2, 0, BIT2 ); // power on DVI CKIN. (port 0 and port 1) (reg_b2[2])
        #endif

        // Power up Xtal clocks and DVI detection clock for DVI clock detection
        msWrite2ByteMask( REG_290C, 0, 0xFFFF );
        // DVI PLL power control
        #if HDMI_PORT==TMDS_PORT_C
        msWrite2ByteMask( REG_29D2, 0x0000, 0x8FF7 ); // enable DVI1 PLL power, [11]: port C Data R-term
        msWriteByteMask( REG_PM_B3, BIT4, BIT5 | BIT4 ); //DVI clock mux: [5]: port 1(portB), [4]: port 0(portA)
        msWriteByteMask( REG_29C2, 0, BIT3); // Dual link primary/secondary:[3]: 0: port C clock from itself [2]: 1: port A/B clock is from port C, [1:0] port A/B data mux: 10: port B; 01: port A
        msWriteByteMask( REG_29D4, BIT0, BIT0); // [0]: enable DVI port 2
        msWrite2ByteMask( REG_29C0, 0x9FF7, 0x9FF7 ); // enable DVI0 PLL power, [12:11]: portB/A Data R-term
        msWriteByteMask( REG_2800, BIT6, BIT7|BIT6 ); // [7]: ABSWAP, [6]: ignore_dual_en //????
        #elif HDMI_PORT==TMDS_PORT_B //Checked
        msWrite2ByteMask( REG_29C0, 0x0800, 0xFFFF ); // enable DVI0 PLL power, [12:11]: portB/A Data R-term
        msWriteByteMask( REG_PM_B3, BIT5, BIT5 | BIT4 ); //DVI clock mux: [5]: port 1(portB), [4]: port 0(portA)
        msWriteByteMask( REG_29C2, BIT1, 0x0F); // Dual link primary/secondary:[3]: 0: port C clock from itself [2]: 0: port A/B clock is from itself, [1:0] port A/B data mux: 10: port B; 01: port A
        msWriteByteMask( REG_29D4, 0, BIT0); // [0]: enable DVI port 2
        msWrite2ByteMask( REG_29D2, 0x8FF7, 0x8FF7 ); // enable DVI1 PLL power, [11]: port C Data R-term
        msWriteByteMask( REG_2800, BIT7, BIT7|BIT6 ); // [7]: ABSWAP, [6]: ignore_dual_en //????
        #else //TMDS_PORT_A
        msWrite2ByteMask( REG_29C0, 0x1000, 0xFFFF ); // enable DVI0 PLL power, [12:11]: portB/A Data R-term
        msWriteByteMask( REG_PM_B3, BIT4, BIT5 | BIT4 ); //DVI clock mux: [5]: port 1(portB), [4]: port 0(portA)
        msWriteByteMask( REG_29C2, BIT0, 0x0F); // Dual link primary/secondary:[3]: 0: port C clock from itself [2]: 0: port A/B clock is from itself, [1:0] port A/B data mux: 10: port B; 01: port A
        msWriteByteMask( REG_29D4, 0, BIT0); // [0]: enable DVI port 2
        msWrite2ByteMask( REG_29D2, 0x8FF7, 0x8FF7 ); // enable DVI1 PLL power, [11]: port C Data R-term
        msWriteByteMask( REG_2800, BIT7, BIT7|BIT6 ); // [7]: ABSWAP, [6]: ignore_dual_en //????
        #endif

        msWriteByteMask( REG_2801, 0, BIT6|BIT5 ); // [6]: DVI_linkb_en, [5]: DVI_dual_manual_en
        msWriteByteMask( REG_2860, 0, BIT6|BIT5|BIT4 ); // [6]: DVI_dual_b_ov_en, [5]: DVI_dual_c_ov_en, [4]: DVI_dual_ov_en
        msWriteByteMask( REG_28F8, 0x08, 0x0F);  // HDCP0: [3]: response 0x74 only, [2]:DDC linkA, [1]: primary, [0]: dual-link
        msWriteByteMask( REG_3BF8, 0x08, 0x0F);  // HDCP1: [3]: response 0x74 only, [2]:DDC linkC, [1]: primary, [0]: single-link
    }
  #endif // #if ENABLE_HDMI_INPUT
    else // non-DVI/HDMI ports
    {
        msWriteByteMask( REG_PM_B3, BIT2, BIT2 ); // power down DVI CKIN. (port C) (reg_b3[2])
        msWriteByteMask( REG_PM_B2, BIT2, BIT2 ); // power down DVI CKIN. (port 0 and port 1) (reg_b2[2])
        msWriteByteMask( REG_2800, 0, BIT7|BIT6 ); // [7]: ABSWAP, [6]: ignore_dual_en
        msWriteByteMask( REG_2801, 0, BIT6|BIT5 ); // [6]: DVI_linkb_en, [5]: DVI_dual_manual_en
        msWriteByteMask( REG_2860, BIT5|BIT4, BIT6|BIT5|BIT4 ); // [6]: DVI_dual_b_ov_en, [5]: DVI_dual_c_ov_en, [4]: DVI_dual_ov_en
        // Power up Xtal clocks and DVI detection clock for DVI clock detection
        msWrite2ByteMask( REG_290C, 0xFFFF, 0xFFFF );
        // DVI PLL power control
        msWrite2ByteMask( REG_29C0, 0xFFFF, 0xFFFF ); // enable DVI0 PLL power, [12:11]: portB/A Data R-term
        msWrite2ByteMask( REG_29D2, 0x8FF7, 0x8FF7 ); // enable DVI1 PLL power, [11]: port C Data R-term
        msWriteByteMask( REG_29D4, 0, BIT0); // [0]: enable DVI port 2
        //msWriteByteMask( REG_01B3, BIT4, BIT5 | BIT4 ); //DVI clock mux: [5]: port 1(portB), [4]: port 0(portA)
        //msWriteByteMask( REG_29C2, BIT0, 0x0F); //DVI clock mux: data mux
    }

}

//Jison 091023 Do I_Gen_Tuning at turn on panel 1st time. 100305 alway enable
#ifndef USE_MOD_HW_CAL
#define USE_MOD_HW_CAL        (1)   // coding test// choose hardware(1) or software(0) MOD calibration
#endif

#ifndef I_GEN_CH //use a defined port
#if (LVDS_CHANNEL&MOD_PORTA)
#define I_GEN_CH  CH3_CLK
#elif (LVDS_CHANNEL&MOD_PORTB)
#define I_GEN_CH  CH9_CLK
#elif (LVDS_CHANNEL&MOD_PORTC)
#define I_GEN_CH  CH15_CLK
#elif (LVDS_CHANNEL&MOD_PORTD)
#define I_GEN_CH  CH21_CLK
#endif
#endif

#if ENABLE_EDP_OUTPUT
//eDP don't need this
#else
#if( USE_MOD_HW_CAL )
// MOD hardware current calibration
// @pre MOD is not in power-down (BK1_26[7]==0)
// @pre LPLL is not in power-down
void I_Gen_Tuning( void )
{
    BYTE i;
    BYTE u8Bank = msReadByte( REG_3200 );
    #define SWING_MASK  0x1F
    drvmStar_printMsg( "I_Gen_Tuning_Start" );

    msWriteByte(REG_3200,0x00);//Select
    msWriteBit( REG_3268,0, BIT0 );//Disable H/W Calbration
    msWriteByteMask( REG_3262, (I_GEN_CH<<2)|LVDS_VOLTAGE_TARGET, 0x0F );

    Power_ModCtrl(_ENABLE);

    //--------------------------
    msWriteBit(REG_3268,BIT0, BIT0 );     /* Enable MOD HW Calibration */
    SetTimOutConter(50);
    //u16TimeOutCounter = 50;
    //bTimeOutCounterFlag=1;
    while(!( msReadByte( REG_3269 ) & BIT7 ) && u16TimeOutCounter);  // wait for MOD HW calibration done

    if (msReadByte( REG_3269 )&BIT6)
    {
        i = LVDS_FAIL_TARGET;//SWING_MASK/2;
        msWriteByteMask(REG_3268,   0x00,  BIT0); /* Disable MOD HW Calibration */
        msWriteByteMask( REG_3260, i, SWING_MASK); //Disable S/W Cal. and swing write to register
        drvmStar_printMsg(" @@@@@@@@@                       HW MOD Cali Fail !!");
    }
    else
    {
        drvmStar_printMsg(" @@@@@@@@@                       HW MOD Cali Pass !!");
        drvmStar_printData("-----REG_3260=0x%x\n\r", msReadByte(REG_3260)&SWING_MASK);  // REG_3260[5:0] is calibration result
    }
    msWriteByte( REG_3200, u8Bank );

    iGenTuningFinished=1;
}

#else //SW
// @brief MOD Current Calibration
// @pre   Current bank is bank-0.
// @pre   LPLL is on, i.e., ASSERT( BK0_D1[4]==0 )
void I_Gen_Tuning( void )
{
    BYTE i;
    #define SWING_MASK  0x3F
    BYTE u8Bank = msReadByte( REG_3200 );

   //MST_printMsg( "I_Gen_Tuning_Start" );
    msWriteByte(REG_3200,0x00);//Select bank
    msWriteByte( REG_3260,0x00);// bit 7=Disable Cal /bit 6 don't use / clean swing data bit[5:0]=0
    msWriteByteMask( REG_3262, (I_GEN_CH<<2)|LVDS_VOLTAGE_TARGET, 0x0F );

    #if(I_GEN_CH==CH3_CLK)
    msWriteByte(REG_3220,0xC0);
    #elif(I_GEN_CH==CH9_CLK)
    msWriteByte(REG_3222,0x0C);
    #elif(I_GEN_CH==CH15_CLK)
    msWriteByte(REG_3223,0xC0);
    #elif(I_GEN_CH==CH21_CLK)
    msWriteByte(REG_3225,0x0C);
    #endif
    //---------------------------

    for( i = 0; i <= SWING_MASK; i++ ) /*? if LVDS swing > 233mV , then REG_306C[7](C_DDA_OUT)=1*/
    {
        msWriteByte(REG_3260,(BIT7|i));

        if(msReadByte(REG_3261 )&BIT0)// Read S/W Calbration ready ?
        {
        if ( i==0 ||  i==SWING_MASK)
        {
            msWriteByteMask(REG_3260, LVDS_FAIL_TARGET,SWING_MASK);                     /* Set MOD output swing value for clock channel*/
            drvmStar_printMsg(" @@@@@@@@@                       FW MOD Cali Fail !!");
        }
        else
        {
            msWriteByteMask(REG_3260, i,SWING_MASK);                        /* Set MOD output swing value for clock channel*/
            drvmStar_printData("-----REG_3260=0x%x", msReadByte(REG_3260));
            drvmStar_printMsg(" @@@@@@@@@                       FW MOD Cali Pass !!");
        }
        break;
        }
    }

    if(i==0x00||i>SWING_MASK)//if(!(msReadByte( REG_3260 ) & 0x3F ))
    {
        msWriteByteMask( REG_3260, SWING_MASK/2,SWING_MASK );
    }
    //------Restore LVDS-----
    #if(PanelLVDS)
    msWriteByte(REG_3220,0x55);
    msWriteByte(REG_3221,0x55);
    msWriteByte(REG_3222,0x55);
    msWriteByte(REG_3223,0x55);
    msWriteByte(REG_3224,0x55);
    msWriteByte(REG_3225,0x55);
    #elif(PanelTTL)
    msWriteByte(REG_3220,0x00);
    msWriteByte(REG_3221,0x00);
    msWriteByte(REG_3222,0x00);
    msWriteByte(REG_3223,0x00);
    msWriteByte(REG_3224,0x00);
    msWriteByte(REG_3225,0x00);
    #endif

    #if PANEL_QUAD_PORT
    msWriteByteMask(REG_3250,0x00,0x1F);
    msWriteByteMask(REG_3264,0x1F,0x1F);
    #elif PanelDualPort
    msWriteByteMask(REG_3250,0x12,0x1F);
    msWriteByteMask(REG_3264,0x16,0x1F);
    #else
    if( I_GEN_CH== CH3_CLK)
    {
        msWriteByteMask(REG_3250,0x1C,0x1F);
        msWriteByteMask(REG_3264,0x11,0x1F);
    }
    else if( I_GEN_CH== CH9_CLK)
    {
        msWriteByteMask(REG_3250,0x1A,0x1F);
        msWriteByteMask(REG_3264,0x12,0x1F);
    }
    else if( I_GEN_CH== CH15_CLK)
    {
        msWriteByteMask(REG_3250,0x16,0x1F);
        msWriteByteMask(REG_3264,0x14,0x1F);
    }
    else
    {
        msWriteByteMask(REG_3250,0x0E,0x1F);
        msWriteByteMask(REG_3264,0x18,0x1F);
    }
    #endif
    msWriteByteMask(REG_3265,0x00,BIT6|BIT2);

    msWriteByte( REG_3200, u8Bank );

    iGenTuningFinished=1;
}

#endif  // end of #if( USE_MOD_HW_CAL )
#endif //#if ENABLE_EDP_OUTPUT
void Power_ModCtrl(BYTE ucSwitch)
{
    BYTE ucOnOff;
    BYTE u8Bank=msReadByte(REG_3200);

    ucOnOff = (ucSwitch)? 0xFF:0x00;
    msWriteByte(REG_3200,0x00);
    msWriteByteMask(REG_3250,(~ucOnOff)&(((LVDS_CHANNEL)<<1)|BIT0),(((LVDS_CHANNEL)<<1)|BIT0));
    msWriteByteMask(REG_3264,ucOnOff&(((LVDS_CHANNEL)|BIT4)),(((LVDS_CHANNEL)|BIT4)) );
    msWriteByteMask(REG_3265,(~ucOnOff)&(BIT6|BIT2),BIT6|BIT2);
    msWriteByte(REG_3200,u8Bank);
}

BYTE mStar_ScalerDoubleBuffer(Bool u8Enable)
{
    BYTE u8OrgValue=msReadByte(SC0_01);
    msWriteByte(SC0_01,(u8Enable?0x05:0x00));
    return u8OrgValue;
}

#if DVI_PORT||HDMI_PORT
WORD mSTar_GetInputHTotal(void)
{
    WORD u16HTotal;
#if ENABLE_DP_INPUT
    if (CURRENT_INPUT_IS_DISPLAYPORT())//(SrcInputType==Input_Displayport)
    {
        u16HTotal=msRead2Byte( REG_1F76 ) & 0x7FFF;
    }
    else
#endif
    {
        msWriteByteMask(SC0_ED,BIT6,BIT6);
        u16HTotal=(msRead2Byte(SC0_E4)&0x1FFF)+1; //get input HTotal
        msWriteByteMask(SC0_ED,0,BIT6);
    }
    return u16HTotal;
}
#endif
#define MPLL_KHZ        429540ul
#define LPLL_ICP_ICTRL  0x03
#define LPLL2_PD        0ul


BYTE GetVfreq( void )
{
#if ( FRAME_BFF_SEL == FRAME_BFFLESS ) // Frame buffer less

    return V_FREQ_IN;

#elif PANEL_3D_PASSIVE_4M // 4M

    if( g_SetupPathInfo.b4MFRC ) // 4M FRC case
        return 60;
    else if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_PF ) // Vout = Vin/2
        return (V_FREQ_IN + 1)/2;
    else if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FPI )// Vout = Vin*2
        return V_FREQ_IN*2;
    else // Vout = Vin
        return V_FREQ_IN;

#else

  #if GLASSES_TYPE==GLASSES_NVIDIA
    if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL || g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_PSV || g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_PSV_4M )
    {
        if(V_FREQ_IN < 48)
            return 60;
        else
            return V_FREQ_IN; // Normal In Freq
    }
    else
        return Get3DVFreq();
  #else
    if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL || g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_PSV || g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_PSV_4M )
        return (BYTE)60; // Normal In Freq
    else
    #if GLASSES_TYPE==GLASSES_INTEL
        return (BYTE)96; // 3D In Freq
    #else
        return (BYTE)120; // 3D In Freq
    #endif
  #endif

#endif
}
WORD GetVtotal( void )
{
    DWORD   xdata dwVtotal_4M;
    WORD    xdata wVDE_in, wVtotal_in;

    if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL) || (g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_PSV) )
    {
        return (WORD)PanelVTotal; // Normal Vtotal
    }
    else if( g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_PSV_4M ) // 4M case
    {
        wVDE_in = (msRead2Byte(SC0_09)&SC_MASK_V)/((CURRENT_SOURCE_IS_INTERLACE_MODE())?2:1);
        wVtotal_in = SrcVTotal/((CURRENT_SOURCE_IS_INTERLACE_MODE())?2:1);

        if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_PF )
            dwVtotal_4M = (DWORD)PANEL_HEIGHT*(wVtotal_in*2)/(wVtotal_in+wVDE_in-IVS_DEFAULT)+10;
        else if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FP) && (!g_SetupPathInfo.b4MFRC) )
            dwVtotal_4M = (DWORD)PANEL_HEIGHT*wVtotal_in/(wVDE_in-IVS_DEFAULT)+10;
        else
            dwVtotal_4M = 0;

        if( PanelVTotal > dwVtotal_4M )
            dwVtotal_4M = PanelVTotal;

        return (WORD)dwVtotal_4M;
    }
    else // 3D output frame sequential case
    {
#if ENABLE_3D_FUNCTION && GLASSES_TYPE==GLASSES_NVIDIA
        return Get3DVtotal();
#else
        return (WORD)PANEL_VTT_3D; // 3D Vtotal: Panel_VTT_3D_100, Panel_VTT_3D_110, Panel_VTT_3D_120
#endif
    }
}


//*******************************************************************
// Function Name: mStar_LPLL1_Setting
//
// Decscription: LPLL1 settings according to Display MISC output clock
//*******************************************************************
void mStar_LPLL1_Setting( const DWORD kHzODCLK )
{
    //XDATA BYTE lpll_icp_ictrl;
    XDATA BYTE u8lpll_loop_div;
    XDATA BYTE u8lpll_out_div_scalar;
    XDATA BYTE u8lpll_out_div_fifo;
    XDATA BYTE u8loop_gain; // For calculating SET value

    XDATA DWORD     u32factor;
    XDATA float     u32i_gain;
    XDATA float     u32p_gain;

    //MST_printData( " LPLL1 kHzODCLK = %d", kHzODCLK);

#if PANEL_QUAD_PORT
    // LPLL Parameters
    if( kHzODCLK <= 25 * 1000UL ) //  <= 25MHz
    {
        u8lpll_loop_div       = 0x1;
        u8lpll_out_div_scalar = 0x1;
        u8lpll_out_div_fifo   = 0x7;
        u8loop_gain           = 2;
    }
    else if( kHzODCLK <= 50 * 1000UL ) // 25MHz~50MHz
    {
        u8lpll_loop_div       = 0x2;
        u8lpll_out_div_scalar = 0x1;
        u8lpll_out_div_fifo   = 0x6;
        u8loop_gain           = 4;
    }
    else if( kHzODCLK <= 100 * 1000UL )  // 50MHz~100MHz
    {
        u8lpll_loop_div       = 0x2;
        u8lpll_out_div_scalar = 0x2;
        u8lpll_out_div_fifo   = 0x6;
        u8loop_gain           = 8;
    }
#if 1  // 120406 coding modified for New Setting for SSC 40k 4%
    else if( kHzODCLK <= 200 * 1000UL )  // 100MHz~200MHz
    {
        u8lpll_loop_div       = 0x2;
        u8lpll_out_div_scalar = 0x2;
        u8lpll_out_div_fifo   = 0x5;
        u8loop_gain           = 8;
    }
#else
    else if( kHzODCLK <= 200 * 1000UL )  // 100MHz~200MHz
    {
        u8lpll_loop_div       = 0x2;
        u8lpll_out_div_scalar = 0x4;
        u8lpll_out_div_fifo   = 0x5;
        u8loop_gain           = 16;
    }
#endif
    else if( kHzODCLK <= 300 * 1000UL )  // 200MHz~300MHz
    {
        u8lpll_loop_div       = 0x3;
        u8lpll_out_div_scalar = 0x4;
        u8lpll_out_div_fifo   = 0x4;
        u8loop_gain           = 32;
    }
    else    // Not Defined
    {
        //MST_printMsg("Not Defined Output CLK Freq in LVDS 4ch");
    }
#else // PanelDualPort & Single
    // LPLL Parameters
    if( kHzODCLK <= 100 * 1000UL ) //  <= 10MHz
    {
        u8lpll_loop_div       = 0x3;
        u8lpll_out_div_scalar = 0x1;
        u8lpll_out_div_fifo   = 0x1;
        u8loop_gain           = 8;
    }
    else // 200MHz
    {
        u8lpll_loop_div       = 0x3;
        u8lpll_out_div_scalar = 0x4;
        u8lpll_out_div_fifo   = 0x0;
        u8loop_gain           = 32;
    }
#endif

    // SET/i_gain/p_gain calculation
    #if PANEL_QUAD_PORT // loop_div = 7/2=3.5, MM=15 for 216MHz selected
    {
        u32factor = ( float )( MPLL_KHZ / 7 ) * (( float )524288UL * ( float )u8loop_gain* 2 / kHzODCLK );
        u32i_gain = ( float )( ( float )30*524288/8/(PanelHTotal/PANEL_H_DIV)/GetVtotal()*u8loop_gain*2 )/(float)(( ((msReadByte(REG_3819)>>4) + ((msReadByte(REG_381C)&BIT1)<<3)) +1)*7);
    }
    #else
    {
        u32factor = ( float )( MPLL_KHZ / 7 ) * (( float )524288UL * ( float )u8loop_gain / kHzODCLK );
        u32i_gain = ( float )( ( float )30*524288/8/(PanelHTotal/PANEL_H_DIV)/GetVtotal()*u8loop_gain )/(float)(( ((msReadByte(REG_3819)>>4) + ((msReadByte(REG_381C)&BIT1)<<3)) +1)*7);
    }
    #endif

    u32i_gain = 5 + GetLog(u32i_gain) + 2;  // follow formula to set and add a offset value 2 to speed up locking time 20130220
    u32p_gain = u32i_gain + 1;

    //Setting LPLL
    msWriteByteMask(REG_3881,LPLL_ICP_ICTRL<<4  ,BIT4|BIT5|BIT6);//lpll_icp_ictrl
    msWriteByteMask(REG_3882,u8lpll_loop_div<<4   ,BIT4|BIT5);//lpll_loop_div
    msWriteByteMask(REG_3882,u8lpll_out_div_scalar,BIT0|BIT1|BIT2|BIT3);//lpll_out_div_scalar
    msWriteByteMask(REG_3883,u8lpll_out_div_fifo  ,BIT0|BIT1|BIT2);//lpll_out_div_fifo

   // MST_printData( " u32factor = %x", (WORD)(u32factor>>8));

#if (PanelDualPort) &&( ENABLE_ROTATE_CLK_SET || (ENABLE_RTE&& (OD_MODE == OD_MODE_333)&& OD_HSCAL_EN))
    msWriteByteMask(REG_3883, _BIT2, _BIT2);
#endif
#if (ENABLE_ROTATE_CLK_SET) || (!PanelDualPort && !PANEL_QUAD_PORT) || (PanelTTL) || (ENABLE_RTE && (OD_MODE == OD_MODE_333) && OD_HSCAL_EN)
        msWriteByteMask(REG_387B, _BIT6, _BIT6);
#endif


    msWriteByte(REG_381E,u32factor);//SET[7:0]
    msWriteByte(REG_381F,u32factor>>8);//SET[15:8]
    msWriteByte(REG_3820,u32factor>>16);//SET[23:16]
#if FRAME_BFF_SEL == FRAME_BFFLESS
    msWriteByte(SC0_D5,u32factor);//SET[7:0]
    msWriteByte(SC0_D6,u32factor>>8);//SET[15:8]
    msWriteByte(SC0_D7,u32factor>>16);//SET[23:16]
#endif
    msWriteByteMask(REG_3816,(BYTE)u32i_gain       ,BIT0|BIT1|BIT2|BIT3);//i_gain
    msWriteByteMask(REG_3816,((BYTE)u32p_gain)<<4  ,BIT4|BIT5|BIT6|BIT7);//p_gain
    msWriteByteMask(REG_3817,(BYTE)u32i_gain       ,BIT0|BIT1|BIT2|BIT3);//i_gain_phase
    msWriteByteMask(REG_3817,((BYTE)u32p_gain)<<4  ,BIT4|BIT5|BIT6|BIT7);//p_gain_phase
}

//*******************************************************************
// Function Name: mStar_LPLL2_Setting
//
// Decscription: LPLL2 settings according to Scalar output clock
//*******************************************************************
void mStar_LPLL2_Setting( const DWORD kHzODCLK )
{
    XDATA BYTE u8lpll2_loop_div;
    XDATA BYTE u8lpll2_scalar_div;
    XDATA BYTE u8lpll2_output_div;
    XDATA BYTE u8loop_gain; // For calculating SET value

    XDATA DWORD     u32Factor;

   // MST_printData( " LPLL2 kHzODCLK = %d", kHzODCLK);

    // LPLL Parameters
    if( kHzODCLK <= 100 * 1000UL ) //  <= 100MHz
    {
        u8lpll2_loop_div      = 0x31;
        u8lpll2_scalar_div    = 0x0;
        u8lpll2_output_div    = 0x1;
        u8loop_gain           = 8;
    }
    else  // 200MHz
    {
        u8lpll2_loop_div      = 0x34;
        u8lpll2_scalar_div    = 0x0;
        u8lpll2_output_div    = 0x0;
        u8loop_gain           = 32;
    }

    // SET calculation
    u32Factor = ( float )( MPLL_KHZ / 7 ) * (( float )524288UL * ( float )u8loop_gain / kHzODCLK );

    //MST_printData( " u32Factor = %x", (WORD)(u32Factor>>8));

    //Setting LPLL
    msWriteByteMask(REG_38A3,LPLL2_PD<<7          ,BIT7);//lpll2_pd
    msWriteByteMask(REG_38A3,LPLL_ICP_ICTRL       ,BIT0|BIT1|BIT2);//lpll2_icp_ictrl
    msWriteByteMask(REG_38A5,u8lpll2_loop_div     ,BIT0|BIT1|BIT2|BIT3|BIT4|BIT5);//lpll2_loop_div
    msWriteByteMask(REG_38A4,u8lpll2_scalar_div   ,BIT0);//lpll2_scalar_div
    msWriteByteMask(REG_38A7,u8lpll2_output_div   ,BIT0|BIT1);//lpll2_output_div

    msWriteByte(REG_38B0,u32Factor);//SET[7:0]
    msWriteByte(REG_38B1,u32Factor>>8);//SET[15:8]
    msWriteByte(REG_38B2,u32Factor>>16);//SET[23:16]
}

void mStar_SetTimingGen( WORD u16HDE_OP21, WORD u16VDE_OP21, BOOL OP21_FreeRun)
{
    WORD uwV_total_OP22; // output vtotal OP2-2
    WORD uwV_total_OP21; // output vtotal OP2-1
    #define Hdivider                    2 // H relative parameters(display timing) divided by 1 or 2

    uwV_total_OP22 = GetVtotal(); // output V total
    uwV_total_OP21 = (PanelVTotal-PANEL_HEIGHT+u16VDE_OP21);

    if ( ((g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_1) && (!OP21_FreeRun)) || (!g_SetupPathInfo.bFBMode)) //case 12, FBL
    {
        //OP2-1
        msWriteByteMask(SC0_9F ,     0, BIT1); //[1]frame lock mode
        if( OP21_FreeRun ) // frame buffless mode, free run
        {
            //Jison 110511 fill back Htotal to avoid auto htotal enabled and h total was changed before SC_02[7] is set.(FBL)
            msWrite2ByteMask(SC0_24, ( PanelHTotal/PANEL_H_DIV - 1 ), SC_MASK_H); // Htt_OP2-1
            msWrite2ByteMask(SC0_1E, PanelVTotal, SC_MASK_V); // output vtotal
        }
        else
            msWrite2ByteMask(SC0_1E, PanelMaxVTotal-1, SC_MASK_V); // output vtotal
        msWriteByteMask(SC0_27 ,  BIT6, BIT6); // frame lock mode
        msWriteByteMask(SC0_28 ,     0, BIT3); // output free run
        msWriteByteMask(SC0_27 ,  BIT1, BIT1); // Enable auto update Htotal
#if DOWN_COUNTER_V_SYNC
        msWriteByteMask(SC0_1F, BIT7, BIT7);
        msWrite2ByteMask(SC0_20, 0x0002, 0x7FF); // vsync start
        msWrite2ByteMask(SC0_22, 0x0001, 0xFFF); // vsync end
#else
        msWrite2ByteMask(SC0_20, ( uwV_total_OP21 - PanelVStart ), 0x7FF); // vsync start
        msWrite2ByteMask(SC0_22, ( uwV_total_OP21 - PanelVSyncBackPorch ), 0xFFF); //vsync end
#endif

        msWrite2Byte(SC0_10, 0x00); //panel V. start
        msWrite2ByteMask(SC0_14, ( (u16VDE_OP21) - 1 ), 0x7FF); //panel Vert. DE end
        msWrite2ByteMask(SC0_16, ( ( PanelHStart/PANEL_H_DIV ) + ((u16HDE_OP21)/PANEL_H_DIV) - 1 ), 0xFFF); //panel Hor. DE end

        msWrite2ByteMask(SC0_1A, ( (u16VDE_OP21) - 1 ), 0x7FF); //image V. end


#if Enable_Expansion
        if (!EXPANSION_MODE_IS_FULL() && (!OP21_FreeRun))//(UserprefExpansionMode!=DefExpansion&& (!OP21_FreeRun))//(USER_PREF_WIDE_MODE!=WM_DEFAULT && (!OP21_FreeRun))
        {
            msWrite2ByteMask(SC0_18, ( ( PanelHStart + ( PanelWidth-OverScanSetting.OverScanH )/2 )/PANEL_H_DIV ), 0xFFF);
            msWrite2ByteMask(SC0_1C, ( ( ( PanelHStart + ( PanelWidth-OverScanSetting.OverScanH )/2 + OverScanSetting.OverScanH )/PANEL_H_DIV ) - 1 ), 0xFFF);
            {
                WORD offset = (PanelHeight - OverScanSetting.OverScanV);
                if( OverScanSetting.OverScanV < PanelHeight )
                {
                    msWrite2Byte(SC0_10, (uwV_total_OP22 - (offset/2) - 1 ));
                    msWrite2Byte(SC0_14, (PanelHeight - (offset/2) - 1));
                    msWrite2Byte(SC0_1A, (PanelHeight - offset - 1));
                    msWrite2ByteMask(SC0_20, ( uwV_total_OP22 - (offset/2) - 1 - 3 ), 0x7FF); // vsync start
                    msWrite2ByteMask(SC0_22, ( uwV_total_OP22 - (offset/2) - 1 - 2 ), 0xFFF); // vsync end

                    msWrite2ByteMask(SC0_18, ( PanelHStart/Hdivider ), 0xFFF);
                    msWrite2ByteMask(SC0_1A, ( (u16VDE_OP21) - 1 ), 0x7FF);
                    msWrite2ByteMask(SC0_1C, ( ( PanelHStart/Hdivider ) + ((u16HDE_OP21)/Hdivider) - 1 ), 0xFFF);
                }
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
        //OP2-1
        msWrite2Byte(SC0_10, 0x00);
        msWrite2ByteMask(SC0_14, ( PanelHeight - 1 ), 0x7FF); // Vert. DE end
        msWriteByteMask(SC0_9F, BIT1, BIT1); // [1]frame pll mode
        msWriteByteMask(SC0_27,    0, BIT6); // frame pll mode
        msWriteByteMask(SC0_28, BIT3, BIT3); // output free run
        msWriteByteMask(SC0_27,    0, BIT1); // Disable auto update Htotal
        msWrite2ByteMask(SC0_24, ( PanelHTotal/Hdivider - 1 ), SC_MASK_H); // Htt_OP2-1
        msWrite2ByteMask(SC0_1E, ( uwV_total_OP22 - 1 ), SC_MASK_V ); // Vtt_OP2-1
        msWrite2ByteMask(SC0_16, ( ( PanelHStart/Hdivider ) + (PanelWidth/Hdivider) - 1 ), 0xFFF); // Hor. DE end
#if DOWN_COUNTER_V_SYNC
        msWriteByteMask(SC0_1F, 0, BIT7);
        msWrite2ByteMask(SC0_20, ( uwV_total_OP22 - 1 - 3 ), 0x7FF); // vsync start
        msWrite2ByteMask(SC0_22, ( uwV_total_OP22 - 1 - 2 ), 0xFFF); // vsync end
#else
        msWrite2ByteMask(SC0_20, ( uwV_total_OP22 - PanelVStart ), 0x7FF); // vsync start
        msWrite2ByteMask(SC0_22, ( uwV_total_OP22 - PanelVSyncBackPorch ), 0xFFF); //vsync end
#endif
        msWrite2ByteMask(SC0_18, ( PanelHStart/Hdivider ), 0xFFF);
        msWrite2ByteMask(SC0_1C, ( ( PanelHStart/Hdivider ) + (PanelWidth/Hdivider) - 1 ), 0xFFF);
        msWrite2ByteMask(SC0_1A, ( PanelHeight - 1 ), 0x7FF);

        if( OverScanSetting.Enable )
        {
            WORD offset = (PanelHeight - OverScanSetting.OverScanV);

            msWrite2ByteMask(SC0_18, ( ( PanelHStart + ( PanelWidth-OverScanSetting.OverScanH )/2 )/Hdivider ), 0xFFF);
            msWrite2ByteMask(SC0_1C, ( ( ( PanelHStart + ( PanelWidth-OverScanSetting.OverScanH )/2 + OverScanSetting.OverScanH )/Hdivider ) - 1 ), 0xFFF);

            if( OverScanSetting.OverScanV < PanelHeight )
            {
                //_WAIT_FOR_OVSYNC_();
                msWrite2Byte(SC0_10, (uwV_total_OP22 - (offset/2) - 1 ));
                msWrite2Byte(SC0_14, (PanelHeight - (offset/2) - 1));
                msWrite2Byte(SC0_1A, (PanelHeight - offset - 1));
                msWrite2ByteMask(SC0_20, ( uwV_total_OP22 - (offset/2) - 1 - 3 ), 0x7FF); // vsync start
                msWrite2ByteMask(SC0_22, ( uwV_total_OP22 - (offset/2) - 1 - 2 ), 0xFFF); // vsync end
            }
        }
    }

#if ENABLE_3D_FUNCTION && GLASSES_TYPE==GLASSES_NVIDIA
    //OP2-2
    msWrite2Byte(REG_300A, ( PANEL_HEIGHT  + 1 ));
    msWrite2Byte(REG_300C, ( PANEL_HEIGHT  + 3));
#else
    msWrite2Byte(REG_300A, ( uwV_total_OP22 - 1 - 3 )); //VS line start <-3>
    msWrite2Byte(REG_300C, ( uwV_total_OP22 - 1 - 2 )); //VS line end <-2>
#endif
    msWrite2Byte(REG_3002, ( uwV_total_OP22 - 1 )); //Vtotal
    msWrite2Byte(REG_3030, ( uwV_total_OP22 - 1 - 3 )); //MRW trig y (vtotal -3)

}
void mStar_SetFrameDivider( BYTE vfreq_N )
{
    ////////////////////////////////
    //  FRC ratio N/M ==> (N-1), (M-1)//
    ////////////////////////////////
    BYTE u8Index_N;
    BYTE u8Index_M;
    BYTE u8Div_N; // input divider
    BYTE u8Div_M; // output divider
    BYTE u8VFreq_M; // output clock
    DWORD u32MinDiff; // minimum differencec
    BYTE u8Index_N_step;

    //MST_printData( "mStar_SetFrameDivider vfreq_N = %d", vfreq_N);

    u8VFreq_M = GetVfreq();

    //MST_printData( "mStar_SetFrameDivider u8VFreq_M = %d", u8VFreq_M);
    u8Index_N_step = (CURRENT_SOURCE_IS_INTERLACE_MODE())?2:1;

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

   // MST_printData( "u8Div_N = %d", u8Div_N);
    //MST_printData( "u8Div_M = %d", u8Div_M);

    msWriteByte(REG_3819, (u8Div_M<<4)|(u8Div_N&0x0F)); //[3:0] input frame div for frame sync, [7:4] output frame div for frame sync
    msWriteByteMask(REG_381C, ((u8Div_M&BIT4)>>3)|(u8Div_N>>4),BIT1|BIT0); //[0] input frame div[4], [1] output frame div [4]
}

void mStar_SetIVS( void )
{
    WORD u16IVS, u16VDE_in, u16Vtotal_in, u16Sample_vst;

    u16Sample_vst = ( msRead2Byte( SC0_05 ) & SC_MASK_V )/((CURRENT_SOURCE_IS_INTERLACE_MODE())?2:1);

    if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_PF) // PF 120Hz in, FS 120Hz out with VBI
     && (g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_FS)
     && ((VFreq( HFreq( SrcHPeriod ), SrcVTotal ))>=990) )
    {
        u16VDE_in = msRead2Byte( SC0_09 ) & SC_MASK_V;
        u16Vtotal_in = msRead2Byte( SC0_E2 ) & SC_MASK_V;
#if 0
        u16IVS = ((DWORD)u16VDE_in * GetVtotal()+(u16Vtotal_in/2))/u16Vtotal_in - PANEL_HEIGHT;
#else
        u16IVS = GetVtotal();
        u16IVS = u16VDE_in - (((DWORD)PANEL_HEIGHT * u16Vtotal_in+(u16IVS/2))/u16IVS);
#endif
        u16IVS += 10;

        if ( u16IVS <= IVS_DEFAULT )
            u16IVS = IVS_DEFAULT; // default: IVS_default

        u16IVS += u16Sample_vst; // Sample V start
    }
    else if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_TB)
          && (g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_PSV_4M) )
    {
        u16IVS = IVS_DEFAULT + g_SetupPathInfo.wImgSizeInV + u16Sample_vst;
    }
    else
    {
        u16IVS = IVS_DEFAULT + u16Sample_vst;
    }
    msWrite2ByteMask(SC0_B8, u16IVS<<4, 0xFFF0);
}
void mStar_CheckFastClock(WORD u16hPeriod)
{
#if ENABLE_LBCLK_FIXED_216M

    u16hPeriod = u16hPeriod;
    msWriteByteMask(REG_1E35, _BIT3, _BIT3|_BIT2);

#else

    DWORD u32Dclk;
    //WORD  u16Sclk;
    WORD  u16Height;

    u16Height = g_SetupPathInfo.wImgSizeInV; // case 1,2 only

    //sclk
    //u16Sclk = (( DWORD ) USER_PREF_H_TOTAL * MST_HPeriod_UINT ) / (( DWORD )u16hPeriod * 1000 ) / PANEL_H_DIV;

    // Case 1, 2, FBL mode
    if ( (g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_1) || (!g_SetupPathInfo.bFBMode) )
    {
        // Case 1,2
        if ( g_SetupPathInfo.bFBMode
            #if Enable_Expansion //JisonWideMode
            && !(!EXPANSION_MODE_IS_FULL() && (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL))//(UserprefExpansionMode!=Expansion_Full && (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL))
            #endif
            )
        {
            if( IS_FMT_IN_SBS())
                u32Dclk = ( float )XTAL_CLOCK_KHZ * 16UL / u16hPeriod * (PanelHTotal-PANEL_WIDTH+g_SetupPathInfo.wMRWSizeInH*2) * ( (g_SetupPathInfo.wMRWSizeInV/2) - 1 ) / ( u16Height - 1 ); // output dot clock in KHz
            else
                u32Dclk = ( float )XTAL_CLOCK_KHZ * 16UL / u16hPeriod * (PanelHTotal-PANEL_WIDTH+g_SetupPathInfo.wMRWSizeInH) * ( (g_SetupPathInfo.wMRWSizeInV) - 1 ) / ( u16Height - 1 ); // output dot clock in KHz
            u32Dclk = ( u32Dclk + 500 ) / 1000 / PANEL_H_DIV; // output dot clock in MHz

            if( u32Dclk >= 172 )
                msWriteByteMask( REG_1E35, _BIT2, _BIT3 | _BIT2 ); // dclk
            else
                msWriteByteMask( REG_1E35, 0, _BIT3 | _BIT2 ); // 172 MHz
        }
        else // FBL mode
            msWriteByteMask( REG_1E35, 0, _BIT3 | _BIT2 ); // 172 MHz
    }
    // Case 3, 4, 5, 6
    else
    {
        msWriteByteMask( REG_1E35, _BIT2, _BIT3 | _BIT2 ); // LPLL2 ( (idclk or odclk) * 1.05 )
    }
#endif
}

//*******************************************************************
// Function Name: mStar_SetupFreeRunMode
//
// Decscription: setup registers for free run mode without any input timing,
//
// caller: mStar_ResetDClkPLL() in mstar.c
//         mSar_WriteByte(), msReadByte(), mStar_WriteWord() in ms_rwreg.c
// callee: mStar_ModeHandler() in detect.c
//*******************************************************************
void drvmStar_SetupFreeRunMode( void )
{
    mStar_ScalerDoubleBuffer(FALSE);
    msWriteByteMask( SC0_02, BIT7, BIT7 ); // disable output Lock mode to enable free run.
    msWriteByte( REG_1E3F, 0x04 ); //Jison 110512 restore HSP clk ,patch for scaling down timing remove cable abnormal fly window OSD
    //msAccOnOff(_DISABLE);//Jison 110322 garbage image issue if enabled
  #if FRAME_BFF_SEL == FRAME_BUFFER
    msWriteByteMask(REG_3818,   0, BIT3 ); //[3] frame lpll enable
    msWriteByteMask(SC0_9F,     0, _BIT0); // Using OP2-1
    msWriteByteMask(REG_1E37,0x28,0x3F); // ODCLK1 = ODCLK2
  #endif
    //mStar_SetFrameDivider(60); //Jison 110426 for mStar_LPLL1_Setting() read back???
    mStar_LPLL1_Setting( ((DWORD)PANEL_V_SYNC_FREQ * PanelVTotal* PanelHTotal + 500)/1000/PANEL_H_DIV ); // output dot clock in KHz
    mStar_SetTimingGen(PANEL_WIDTH, PANEL_HEIGHT, TRUE); // Set OP2-1 timing gen
}
void drvmStar_SetupInputPort_VGA(void)
{
    msWriteByteMask(SC0_A6, 0, _BIT1); //DP/DVI dual
    msWriteByteMask(SC0_03, 0, _BIT7);                   //DP YUV422 Issue
    msWriteByteMask(SC0_E8, 0, _BIT5|_BIT4|_BIT3|_BIT2); //user define HV polarity(bit5,bit3)
    msWriteByteMask(SC0_A8, 0, _BIT1|BIT0);

    drvADC_SetInputMux(Input_VGA);
    mStar_SetAnalogInputPort( FALSE );    // switch to RGB port by default

    msWriteByte(SC0_04, VSE_B);
    msWriteByteMask(REG_1E22, 0,BIT3); //DE only mode for SC_TOP //recover

    msWriteByte(SC0_6F, 0x00);   // Interlace mode line shift
    msWriteByte(SC0_ED, 0x01);   // enable ADC coast
    msWriteByte(SC0_EE, 0x03);   // enable coast window start
    msWriteByte(SC0_EF, 0x01);   // enable coast window end
    msWriteByte(SC0_F3, 0x00);// disable DE glitch removal function & more tolerance for DE
    msWriteByteMask( REG_05CD, 0, BIT4 ); // Set HDMI Audio Source // 120418 coding addition

    msWriteByteMask( REG_2E04, IP_MUX_ADC+1, 0x0F );       // IPmux 3 for Dual DVI and DP
    msWriteByteMask( REG_2E02, IP_MUX_ADC << 4, 0xF0 );    // IPmux 2
    msWriteByteMask( REG_1E3E, (CLK_ADC_BUFF_DIV2<<6)|(CLK_ADC_BUFF_DIV2<<4), 0xF0 );

    msWriteByte(SC0_02, NIS_B|ISEL_ANALOG1);

}

void drvmStar_SetupInputPort_DVI(void)
{
    msWriteByteMask(SC0_A6, 0, _BIT1); //DP/DVI dual
    msWriteByteMask(SC0_03, 0, _BIT7);                   //DP YUV422 Issue
    msWriteByteMask(SC0_E8, 0, _BIT5|_BIT4|_BIT3|_BIT2); //user define HV polarity(bit5,bit3)
    msWriteByteMask(SC0_A8, 0, _BIT1|BIT0);

#if (DVI_RB_SWAP)
    msWriteByteMask(REG_281C,_BIT5,_BIT5);
#endif
#if (DVI_PN_SWAP)
    msWriteByteMask(REG_2818,_BIT7,_BIT7);
#endif

      msTMDSSetMux( SrcInputType );

#if (_TMDS_DE_MODE_EN_)
    msWriteByte( SC0_04, BIT3  | BIT7 |BIT6);
    msWriteByteMask( REG_1E22, BIT3, BIT3 ); // enable DE-only mode for SC_TOP
#else
    msWriteByte( SC0_04, BIT3  | BIT7);
    msWriteByteMask( REG_1E22, 0, BIT3 ); // enable DE-only mode for SC_TOP
#endif

    msWriteByteMask( REG_05CD, 0, BIT4 ); // Set HDMI Audio Source // 120418 coding addition
    msWriteByte(SC0_F3, 0xF0);// enable DE glitch removal function & more tolerance for DE
    msWriteByteMask( REG_2E04, IP_MUX_DVI_A+1, 0x0F );       // IPmux 3 for Dual DVI and DP
    msWriteByteMask( REG_2E02, IP_MUX_DVI_A << 4, 0xF0 );    // IPmux 2
    msWriteByteMask( REG_1E3E, (CLK_DVI_BUFF_DIV2<<6)|(CLK_DVI_BUFF_DIV2<<4), 0xF0 );

    msWriteByte(SC0_02, NIS_B|ISEL_DVI|SCALER_STYPE_HV);

}

#if ENABLE_HDMI
void drvmStar_SetupInputPort_HDMI(void)
{
    msWriteByteMask(SC0_A6, 0, _BIT1); //DP/DVI dual
    msWriteByteMask(SC0_03, 0, _BIT7);                   //DP YUV422 Issue
    msWriteByteMask(SC0_E8, 0, _BIT5|_BIT4|_BIT3|_BIT2); //user define HV polarity(bit5,bit3)
    msWriteByteMask(SC0_A8, 0, _BIT1|BIT0);

#if (HDMI_RB_SWAP)
    msWriteByteMask(REG_281C,_BIT5,_BIT5);
#endif
#if (HDMI_PN_SWAP)
    msWriteByteMask(REG_2818,_BIT7,_BIT7);
#endif

    msTMDSSetMux( SrcInputType );

#if (_TMDS_DE_MODE_EN_)&&(!ENABLE_FREESYNC)
    msWriteByte( SC0_04, BIT3  | BIT7 |BIT6);
    msWriteByteMask( REG_1E22, BIT3, BIT3 ); // enable DE-only mode for SC_TOP
#else
    msWriteByte( SC0_04, BIT3  | BIT7);
    msWriteByteMask( REG_1E22, 0, BIT3 ); // enable DE-only mode for SC_TOP
#endif

    msWriteByteMask( REG_05CD, 0, BIT4 ); // Set HDMI Audio Source // 120418 coding addition
    msWriteByte(SC0_F3, 0xF0);// enable DE glitch removal function & more tolerance for DE
    msWriteByteMask( REG_2E04, IP_MUX_DVI_A+1, 0x0F );       // IPmux 3 for Dual DVI and DP
    msWriteByteMask( REG_2E02, IP_MUX_DVI_A << 4, 0xF0 );    // IPmux 2
    msWriteByteMask( REG_1E3E, (CLK_DVI_BUFF_DIV2<<6)|(CLK_DVI_BUFF_DIV2<<4), 0xF0 );

    msWriteByte(SC0_02, NIS_B|ISEL_HDMI_444|SCALER_STYPE_HV);

}
#endif

#if ENABLE_DP_INPUT
void drvmStar_SetupInputPort_DisplayPort(void)
{
    msWriteByteMask(SC0_03, _BIT7, _BIT7);                                       // YUV422 issue
    msWriteByteMask( REG_05E6, BIT1, BIT1|BIT0 );                          // Set I2S sample width
    msWriteByte( SC0_04, BIT3 |BIT4| BIT7 );
    msWriteByteMask( REG_1E22, 0, BIT3 );                                    // enable DE-only mode for SC_TOP
    msWriteByte( SC0_F3, 0xF0 );                                                        // enable DE glitch removal function & more tolerance for DE
    msWriteByteMask( REG_05CD, BIT4, BIT4 );                                // Set DP Audio Source

    msWriteByteMask(SC0_A6, _BIT1, _BIT1);
    msWriteByteMask(SC0_A8, _BIT1|BIT0, _BIT1|BIT0);
    msWriteByteMask(SC0_E7, _BIT5, _BIT5);
    msWriteByteMask(SC0_E8, _BIT5|_BIT4|_BIT3|_BIT2, _BIT5|_BIT4|_BIT3|_BIT2);
    g_SetupPathInfo.ucIPLRSel = IP_3D_LR_DP;

    msWriteByteMask( REG_2E04, IP_MUX_DP_A+1, 0x0F );       // IPmux 3 for Dual DVI and DP
    msWriteByteMask( REG_2E02, IP_MUX_DP_A << 4, 0xF0 );    // IPmux 2
    msWriteByteMask( REG_1E3E, (DP_STRM_CLK<<6)|(DP_STRM_CLK<<4), 0xF0 );

    msWriteByte(SC0_02, NIS_B|SCALER_STYPE_HV|SCALER_ISEL_DP);

}
#endif

#if ENABLE_FREESYNC
void msSetupDelayLine(void)
{
		if(IS_DP_FREESYNC())//freesync
		{
			BYTE xdata u8VSyncTime=GetVSyncTime();
		
			WORD xdata Hi_Act=GetImageWidth();	 //  OK
			WORD xdata Vi_Act=GetImageHeight();  //  OK
			WORD xdata Hi_Total=mSTar_GetInputHTotal();   //  OK
			WORD xdata Vi_Total=SC0_READ_VTOTAL();			 //  OK
			WORD xdata Hi_Blanking=Hi_Total-Hi_Act;   //  OK
		
			WORD xdata Ho_Act=PanelWidth;
			WORD xdata Vo_Act=PanelHeight;
			WORD xdata Ho_Total=PanelMinHTotal;
			WORD xdata Vo_Total=( (DWORD)Vo_Act*Vi_Total/Vi_Act);
		
#if 0//SC_V_SC_NON_MINUS1MODE
			WORD xdata u16FV=(DWORD)Vi_Act*1000/Vo_Act;
#else
			WORD xdata u16FV=((((DWORD)Vi_Act-1)*1000)/(Vo_Act-1));
#endif
		
			WORD xdata u16Ldly_mid = (6200-(DWORD)u16FV*Ho_Act/Ho_Total-((DWORD)Hi_Blanking*1000)/Ho_Total)/2;

			BYTE u8DlyLine = u16Ldly_mid/1000;
			BYTE u8SampleRangeVStartOffset = ((msRead2Byte(SC0_05)) + ((u8DlyLine < 4)?(4 - u8DlyLine):0));
	
			msWriteByteMask(SC0_0F, 0x02, 0x0F); //Ylock line
			msWrite2Byte(SC0_05, u8SampleRangeVStartOffset+1); // set capture window vstart
	
		}
		else
	    	msWriteByteMask(SC0_0F, (SC_LOCK_LINE), 0x0F); //Ylock line
}

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
