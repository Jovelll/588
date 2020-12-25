
///////////////////////////////////////
// DRVMSTAR_V VERSION: V02
////////////////////////////////////////
#include "global.h"
#include "msEread.h"
#include "menufunc.h"
#include "Detect.h"


#ifndef _DRVMSTAR_J_C_
#define _DRVMSTAR_J_C_

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
    {REG_3041, 0x00, 0xFF},
    {REG_30F0, 0x00, BIT0},
    {REG_30FA, 0x00, BIT7},
    {REG_30EE, 0x07, 0x07},
    {REG_3806, 0x00, BIT5},
    {REG_3A60, 0x00, BIT6},
    //{REG_05F0, 0x00, BIT3},// [3]: AULL power down
#if 1    //  clock gating
    {REG_1E29, 0x00, BIT0},
    {REG_1E35, 0x00, BIT0},
    {REG_1E37, 0x00, BIT0},
    {REG_1E3E, 0x00, BIT0},
    {REG_1E3F, 0x00, BIT0},
    {REG_1E2E, BIT6|BIT4|BIT2|BIT0, BIT6|BIT4|BIT2|BIT0},
    {REG_1E30, 0x00, BIT4|BIT0},
    {REG_1E25, BIT2, BIT2},
    {REG_1E28, BIT0, BIT0},
    {REG_1E24, 0x00, BIT4|BIT0},          //
    {REG_1E2C, 0x00, BIT4|BIT0},
    {REG_1E2A, BIT0, BIT0},
    {REG_1E47, 0x00, BIT0},
#endif
    {REG_01C7, 0x00, 0xFF},  //PHDAC on

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
    //{REG_30AA, 0x00}, // BK0_D0[0] Rose <--> REG_30AA[0] CHIP_TSUML                   //???????????????????????????
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

    msWriteByteMask(REG_1E25, BIT2, BIT2);
    msWriteByteMask(REG_1E28, BIT0, BIT0);
    msWriteByteMask(REG_1133, BIT7, BIT7);

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
    msWriteByteMask(REG_1700, BIT2, BIT2);      // [2]: DVI enable
    // 1200301 coding, set EQ to 1
    msWriteByte(REG_1642, 0x01); // EQ strength level
    msWriteByte(REG_1644, 0x01); // EQ strength level
    msWriteByte(REG_1646, 0x01); // EQ strength level
    msWriteByte(REG_17FE,0x00);  // DVI phase dac power on

    msWrite2Byte(REG_161E, 0x810C); // Set compvalue 30
    msWrite2Byte(REG_1620, 0x0218); // Set compvalue 60
    msWrite2Byte(REG_1622, 0x0431); // Set compvalue 120

    msWriteByteMask(REG_1569, 0x00, BIT0);

    msWrite2Byte(REG_1604, 0x00C0);
    msWrite2Byte(REG_1438, 0x0010);
    msWrite2Byte(REG_142A, 0x0017);

    msWriteByteMask(REG_1627, 0x20, 0x70);      //the comp value is reg_comp_tol * 4
    msWriteByteMask(REG_1625, 0xF0, 0xF0);      //0x110A25=0xB0, DVI clock detection counter tolerance set to 7
    msWriteByte(REG_1652,0x10);                 // [4]: enable auto DVIPLL adjust
}


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


void drvmStar_Init( void )
{
    BYTE i;

#if ENABLE_SW_DOUBLE_BUFFER
    msSWDBInit();
#endif

#if CHIP_ID == CHIP_TSUM2 //U02 need to Set this bit to let MCU could restart after reset.
    msWriteByteMask(REG_0180,BIT0,BIT0); //when external 1.2V is lower than 0.8V, Scalar will enter into reset status. let 1.2V back to normal level , MCU should restart again.
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

    //msWriteByteMask(SC0_0E, BIT7, BIT7); // H coring disable, for dot pattern certical block issue, Jison 100610
    mStar_SetupPathInit(); //Jison need put here because mStar_SetupFreeRunMode() will use g_SetupPathInfo variable


#if DISABLE_SHORT_LINE_TUNE
    msWriteByteMask(SC0_63, BIT0,  BIT1|BIT0);
    msWriteByteMask(SC0_28,    0,  BIT6);
#endif

#if DISABLE_SHORT_FRAME
    msWriteByteMask(SC0_63, BIT2, BIT2);
    msWriteByteMask(SC0_63, 0, BIT3);
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

static void msSetupDelayLine(void)
{
    BYTE xdata u8VSyncTime=GetVSyncTime();

	WORD xdata Hi_Act=GetImageWidth();	 //  OK
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
                 if(CURRENT_INPUT_IS_DISPLAYPORT())      // For DP interlace 20121025
                 msWriteByteMask(SC0_E9,0,BIT0);
                 else
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
        msWriteByteMask(REG_01C3, BIT4, BIT7|BIT5|BIT4); // [7]: pd_pll, [5]:pd_clkin_offl, [4]: pd_clkin
        //msWriteByteMask(REG_01CC, 0, 0x1F);       // setup in msPM_EnableDVIDetect
        msWriteByteMask(REG_01CD, BIT0, BIT0);  // [0]: pm_dvi_5v_det_en_0
        msWriteByteMask(REG_01CE, BIT3, 0x19);  // [4]: pdn_dvi_bg, [3]: pd_avddl, [0]: pdn_dvi_rclk

        msWriteByteMask(REG_17BE, BIT0, BIT0);  //pd_envdet
        msWriteByteMask(REG_17C0, 0xA7, 0xA7);  //[7]: pdn_dvipll, [5]: pdn_dvipllreg, [2:0]: pdn_dm
        msWriteByteMask(REG_17C1, 0x3F, 0x3F);  // [5:3]: pdn_dvird, [2:0]: pdn_dplmxr
    }
    else if(ctrl == DVI_POWER_DOWN)
    {
        msWriteByteMask(REG_01C3, 0xB0, 0xB0);  // [7]: pd_pll, [5]:pd_clkin_offl, [4]: pd_clkin
        //msWriteByteMask(REG_01CC, 0x18, 0x1F); // setup in msPM_EnableDVIDetect
        msWriteByteMask(REG_01CD, 0, BIT0);     // [0]: pm_dvi_5v_det_en_0
        msWriteByteMask(REG_01CE, 0x19, 0x19);  // [4]: pdn_dvi_bg, [3]: pd_avddl, [0]: pdn_dvi_rclk

        msWriteByteMask(REG_17BE, BIT0, BIT0);  //pd_envdet
        msWriteByteMask(REG_17C0, 0xA7, 0xA7);  //[7]: pdn_dvipll, [5]: pdn_dvipllreg, [2:0]: pdn_dm
        msWriteByteMask(REG_17C1, 0x3F, 0x3F);  // [5:3]: pdn_dvird, [2:0]: pdn_dplmxr
    }
    else    // power on
    {
        msWriteByteMask(REG_01C2, BIT5, BIT7|BIT6|BIT5); //[7:5]: gcr_swck
        msWriteByteMask(REG_01C3, 0, 0xB0); // [7]: pd_pll, [5]:pd_clkin_offl, [4]: pd_clkin
        msWriteByteMask(REG_01CC, 0, 0x1F); // [2:0]: rclk
        msWriteByteMask(REG_01CD, 0, BIT0);
        msWriteByteMask(REG_01CE, 0, 0x19); // [4]: pdn_dvi_bg, [3]: pd_avddl, [0]: pdn_dvi_rclk

        msWriteByteMask(REG_17BE, 0, BIT0); //pd_envdet
        msWriteByteMask(REG_17C0, 0, 0xA7); //[7]: pdn_dvipll, [5]: pdn_dvipllreg, [2:0]: pdn_dm
        msWriteByteMask(REG_17C1, 0, 0x3F); // [5:3]: pdn_dvird, [2:0]: pdn_dplmxr
    }
}
#if MS_PM
void drvDVI_PortMuxControl(BYTE ucPort)
{
    switch(ucPort)
    {
        case DVI_INPUT_PORT0:
            msWriteByteMask(REG_01C2, BIT5, BIT7 |BIT6 |BIT5);
            break;
        case DVI_INPUT_NONE:
            msWriteByteMask(REG_01C2, 0, BIT7 |BIT6 |BIT5);
            break;
        case DVI_INPUT_ALL:
            msWriteByteMask(REG_01C2, BIT7 |BIT6 |BIT5, BIT7 |BIT6 |BIT5);
            break;
        default:
            break;
    }
}
#endif
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
    if(INPUT_IS_DVI(inport))//( inport == Input_Digital )
    {
        //msWriteByteMask(REG_01C3, 0, BIT4); // power on DVI CKIN.
        msWriteByteMask(REG_01CE, 0, BIT4); // power on PLL band-gap.
        // Power up Xtal clocks and DVI detection clock for DVI clock detection
        msWrite2ByteMask(REG_170C, 0x0000, 0xFFFF);
        msWrite2ByteMask(REG_01CE, 0x0000, 0x0019); //Clock R-term
        msWrite2ByteMask(REG_17C0, 0x0000, 0xFFFF); //Data R-term
        msWrite2ByteMask(REG_17C2, 0x0001, 0x0003); //Clock path
        #if (MainBoardType == MainBoard_TSUML58CN3 ||MainBoardType == MainBoard_TSUML88CMC2)
        msWriteByteMask(REG_0265, BIT6, BIT6); // DDCD0_Group GPIO32/33.
        #endif
    }
    else // non-DVI/HDMI ports
    {
        //msWriteByteMask(REG_01C3, BIT4, BIT4); // power down DVI CKIN.
        msWriteByteMask(REG_01CE, BIT4, BIT4); // power down PLL band-gap.
        // Power up Xtal clocks and DVI detection clock for DVI clock detection
        msWrite2ByteMask(REG_170C, 0xFFFF, 0xFFFF);
        msWrite2ByteMask(REG_01CE, 0x0019, 0x0019);
        msWrite2ByteMask(REG_17C0, 0xFFFF, 0xFFFF); //Data R-term
        msWrite2ByteMask(REG_17C2, 0x0000, 0x0003); //Clock path
        //msWrite2ByteMask(REG_01C2, 0xCE08, 0xFF08);
        //msWrite2ByteMask(REG_01C4, 0xFEEE, 0xFFFF);
        //msWrite2ByteMask(REG_01C6, 0xFFFF, 0xFFFF);

        //msWriteByteMask( REG_01C2, 0, BIT7|BIT6|BIT5 );
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
	BYTE XDATA	i;
	BYTE XDATA	reg_30E2   = msReadByte(REG_30E2);
	WORD XDATA	reg_3046   = msRead2Byte(REG_3046);
	WORD XDATA	reg_3048   = msRead2Byte(REG_3048);
	BYTE XDATA	reg_3080   = msReadByte(REG_3080);
	BYTE XDATA	reg_308B   = msReadByte(REG_308B);
	WORD XDATA	reg_306A   = msRead2Byte(REG_306A); // ch double swing
	BYTE XDATA	reg_30DA   = msReadByte(REG_30DA);
	BYTE XDATA	reg_30DB   = msReadByte(REG_30DB);
	BYTE XDATA	reg_30DC   = msReadByte(REG_30DC);
        BYTE XDATA    clockswing;
	drvmStar_printMsg("HW I_Gen_Tuning_Start");

#if 1
	msWrite2ByteMask(REG_3076,0x003C		,0xFFFF);  //5us
	msWriteByte(REG_30DA, 0xFF);
	msWriteByte(REG_30DB, 0xFF);
	msWriteByte(REG_30DC, 0xFF);
#endif

	// 1.Disable double differential output swing
	msWrite2Byte(REG_306A,	 0x00);
	// 2.Select calibration source.
#if(I_GEN_CH==CH2)
	msWriteByteMask(REG_30FA,	0x00,  BIT3|BIT2); // Ch2
	msWrite2ByteMask(REG_3046,	0x00,  BIT8| BIT7|BIT6);
	msWriteByteMask(REG_30E2,	0x00,  BIT2);
#else
	msWriteByteMask(REG_30FA,	BIT2,  BIT3|BIT2); // Ch6
	msWrite2ByteMask(REG_3048,	0x00,  BIT5| BIT4|BIT3);
	msWriteByteMask(REG_30E2,	0x00,  BIT6);
#endif

	// 3.Enable LVDS Test Mode, and make output data=High for LVDS output.
	//Note:
	//Please disable MOD Differential output polarity swap. (BK36_E6[7:0]=8'h00, BK36_E7[1:0]=2'h0)
	msWriteByteMask(REG_3080,	BIT1,  BIT1);
	msWriteByteMask(REG_308B,	0x00,  BIT1|BIT0);
	// 4.Set LVDS voltage target.
	msWriteByteMask(REG_30FA,	LVDS_VOLTAGE_TARGET,  BIT1|BIT0);
	// 5.Enable Hardware Calibration
	msWriteByteMask(REG_30FA,	BIT7,  BIT7);
	msWriteByteMask(REG_307B,	BIT7,  BIT7);
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
	msWriteByteMask(REG_30FA,	0x00,  BIT7);
	msWriteByteMask(REG_307B,	0x00,  BIT7);
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
	drvmStar_printMsg(" @@@@@@@@@						HW I_Gen_Tuning_End");
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
    BYTE XDATA	 clockswing;

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
	BYTE XDATA	reg_306E   = msReadByte(REG_306E);
	WORD XDATA	reg_3046   = msRead2Byte(REG_3046);
	WORD XDATA	reg_3048   = msRead2Byte(REG_3048);
	WORD XDATA	reg_306C   = msRead2Byte(REG_306C);
	WORD XDATA	reg_30EA   = msRead2Byte(REG_30EA);
	WORD XDATA	reg_30F2   = msRead2Byte(REG_30F2);

#if 1
	msWriteByteMask(REG_306E, 0x00	,BIT6);
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
    //XDATA float     u32i_gain ,u32p_gain;

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
    //u32i_gain = ( float )(( float )15*524288/8/(PanelHTotal/PANEL_H_DIV)/(WORD)PanelVTotal*u8LPLL_LOOP_GAIN )/(float)(( ((msReadByte(REG_3819)>>4) + ((msReadByte(REG_381C)&BIT1)<<1)) +1)*7);
#else
    u32Factor = (float)((float)u32MPLL_KHZ/u8LPLL_LOOP_DIV)*((float)524288UL*(float)u8LPLL_LOOP_GAIN/u32ODCLK_KHZ);
    //u32i_gain = ( float )(( float )30*524288/8/(PanelHTotal/PANEL_H_DIV)/(WORD)PanelVTotal*u8LPLL_LOOP_GAIN )/(float)(( ((msReadByte(REG_3819)>>4) + ((msReadByte(REG_381C)&BIT1)<<1)) +1)*7);
#endif
    //u32i_gain = 5 + GetLog(u32i_gain);  // follow formula to set and add a offset value 2 to speed up locking time 20130220
    //u32p_gain = u32i_gain + 1;

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

    //msWriteByteMask(REG_3816,(BYTE)u32i_gain       ,BIT0|BIT1|BIT2|BIT3);//i_gain
    //msWriteByteMask(REG_3816,((BYTE)u32p_gain)<<4  ,BIT4|BIT5|BIT6|BIT7);//p_gain
    //msWriteByteMask(REG_3817,(BYTE)u32i_gain       ,BIT0|BIT1|BIT2|BIT3);//i_gain_phase
    //msWriteByteMask(REG_3817,((BYTE)u32p_gain)<<4  ,BIT4|BIT5|BIT6|BIT7);//p_gain_phase

}

void drvmStar_SetupFreeRunMode( void )
{
    mStar_ScalerDoubleBuffer(FALSE);
    msWriteByteMask( SC0_02, BIT7, BIT7 ); // disable output Lock mode to enable free run.
    msWriteByteMask(SC0_27,BIT6,BIT6|BIT1);
    msWrite2ByteMask(SC0_1E, PanelVTotal-1,SC_MASK_V); // set output panel vtotal
    msWrite2Byte(SC0_24, (PanelMinHTotal+8)-1);
    msWriteByteMask(SC0_63,BIT0,BIT1|BIT0);
    msWriteByteMask(SC0_28,BIT6,BIT6);
    msWriteByteMask(SC0_28,BIT3,BIT3);
    msWriteByteMask(SC0_9F,0,BIT0);
    mStar_SetDClkPLL(PanelDCLK*1000UL); //White101208 must use 1000UL to provent overflow truncate
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
    }
}
void drvmStar_SetupInputPort_VGA(void)
{
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

    msWriteByteMask(REG_2E02, IP_MUX_ADC<<4, 0xF0);
    msWriteByteMask(REG_1E3E, IP_MUX_ADC<<4, 0x30);
    msWriteByte(SC0_02, NIS_B|ISEL_ANALOG1);
}

void drvmStar_SetupInputPort_DVI(void)
{
    msWriteByteMask(SC0_A8, 0, _BIT1|BIT0);
#if (DVI_RB_SWAP)
    msWriteByteMask(REG_161C,_BIT5,_BIT5);
#endif
#if (DVI_PN_SWAP)
    msWriteByteMask(REG_1618,_BIT7,_BIT7);
#endif

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
    msWriteByteMask(SC0_A8, 0, _BIT1|BIT0);
#if (HDMI_RB_SWAP)
    msWriteByteMask(REG_1600,_BIT5,_BIT5);
#endif
#if (HDMI_PN_SWAP)
    msWriteByteMask(REG_1600,0x0F,0x0F);
#endif

    msTMDSSetMux( SrcInputType  );

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

#endif
