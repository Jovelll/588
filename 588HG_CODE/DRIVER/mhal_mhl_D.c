///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    mhal_mhl.c
/// @author MStar Semiconductor Inc.
/// @brief  MHL driver Function
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MHAL_MHL_C_
#define _MHAL_MHL_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "global.h"
#include "msEread.h"
#include "msComboRx.h"

/*
#include "board.h"
#include "types.h"
#include "ms_reg.h"
//#include "DebugMsg.h"
#include "global.h"
#include "misc.h"
#include "mapi_mhl.h"
#include "mdrv_mhl.h"
#include "mhal_mhl.h"
*/
#if ENABLE_MHL

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define MHL_HAL_DEBUG_MESSAGE       1

#if(ENABLE_DEBUG && MHL_DEBUG_MESSAGE && MHL_HAL_DEBUG_MESSAGE)
#define MHL_HAL_DPUTSTR(str)        printMsg(str)
#define MHL_HAL_DPRINTF(str, x)     printData(str, x)
#else
#define MHL_HAL_DPUTSTR(str)
#define MHL_HAL_DPRINTF(str, x)
#endif

//######### Eland #################
#define Eland_CONFIG_ID    0xD0
#define Eland_SRID_ID    0xC8
//##############################

#define HB(x)    x+1   // High byte
#define LB(x)    x    // low byte
#define LBMASK    0x00FF   // low byte mask
#define HBMASK    0xFF00    // high byte mask
#define WMASK     0xFFFF    // word mask

#define DMHLInit    0

#define DELAY1US()                  _nop_()

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------
typedef struct
{
    WORD addr;
    WORD databuf;
    WORD mask;
} msLoadTbl_S;

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
BYTE xdata ucCableDetectEnable = 0;
BYTE xdata ucMHLDriver = MHL_DRIVER_VERSION;
#if(MHL_PHASE_CHECK_PATCH)
BYTE xdata ucPLLDivMode = 0;
#endif

//-------------------------------------------------------------------------------------------------
//  MHL initial table
//-------------------------------------------------------------------------------------------------
code msLoadTbl_S tMHL_INITIAL_TABLE[] =
{
    // Set accepted discover pulse high pulse width to ignore USB pulse
    {REG_0C18, 0x0046, MASKBIT(15:0)}, // REG_PM_MHL_CBUS_0C[15:0], reject cbus discovery pulse below this limit
    {REG_0C1A, 0x0082, MASKBIT(15:0)}, // REG_PM_MHL_CBUS_0D[15:0], reject cbus discovery pulse above this limit
    // CTS 4.3.7.1
    {REG_0BAA, BIT4, MASKBIT(7:4)}, // REG_MHL_CBUS_55[7:4]: cbus requester transmit opportunity after arbitration
    {REG_0BE0, BIT14, BIT14}, // REG_MHL_CBUS_70[14]: let edid current read initial address add 1

    // For CTS 6.3.10.7
    {REG_0BB2, 0xADB0, MASKBIT(15:0)}, // REG_MHL_CBUS_59[15:0], timeout for a device receiving a packet within a command
    {REG_0BB4, 0x0001, MASKBIT(15:0)}, // REG_MHL_CBUS_5A[15:0], modify to 110ms

#if(MHL_CBUS_OPERATION_MODE >= MHL_CBUS_HW_ISR_MODE)
    {REG_0B46, BIT10 |BIT4, MASKBIT(10:8) |BIT4}, // REG_MHL_CBUS_23[10]: enable HW auto response read_devcap request, [9:8]: old mode RCP/RAP HW auto response, [4]: enable HW responses wrt_grt_int.
    {REG_0BDA, MASKBIT(11:0), MASKBIT(11:0)}, // REG_MHL_CBUS_6D[11:0], Enable MHL HW mode
    {REG_0BE2, MASKBIT(15:14), MASKBIT(15:14)}, // REG_MHL_CBUS_71[15:14], [15]: Enable msc hw auto response., [14]:Auto response packet after SW clr fifo or Xdata.
    {REG_0BA0, BIT13, BIT13}, // REG_MHL_CBUS_50[13]: MSC sending command keep mode once auto response is onging
#else
    {REG_0B46, 0, MASKBIT(10:8) |BIT4}, // REG_MHL_CBUS_23[10]: enable HW auto response read_devcap request, [9:8]: old mode RCP/RAP HW auto response, [4]: enable HW responses wrt_grt_int.
    {REG_0BDA, 0, MASKBIT(11:0)}, // REG_MHL_CBUS_6D[11:0], Enable MHL HW mode
    {REG_0BE2, 0, MASKBIT(15:14)}, // REG_MHL_CBUS_71[15:14], [15]: Enable msc hw auto response., [14]:Auto response packet after SW clr fifo or Xdata.
    {REG_0BA0, 0, BIT13}, // REG_MHL_CBUS_50[13]: MSC sending command keep mode once auto response is onging
#endif

#if(MHL_CBUS_OPERATION_MODE == MHL_CBUS_HW_REPLY_MODE)
    {REG_0BD8, BIT14, BIT14}, // REG_MHL_CBUS_6C[14]: Register xdata enable
#else
    {REG_0BD8, 0, BIT14}, // REG_MHL_CBUS_6C[14]: Register xdata enable
#endif
    {REG_0C22, 0x88B8, MASKBIT(15:0)}, // extend the maxi wakeup pulse width to 35msec 
    //{REG_0264, MASKBIT(13:11), 0}, // REG_PM_GPIO_32[13:11], [13]: Enable GPIO51 pull-up 1k, [12]: Enable GPIO36 pull-up 1k, [11]: Enable GPIO31 pull-up 1k.
};

//-------------------------------------------------------------------------------------------------
//  MHL HDMI bypass table
//-------------------------------------------------------------------------------------------------
code msLoadTbl_S tMHL_HDMI_BYPASS_TABLE[] =
{
    {REG_01C2, 0, MASKBIT(15:14)}, // REG_PM_BANK_61[15:14], Power On PLL
    {REG_01C4, 0, MASKBIT(15:12)}, // REG_PM_BANK_62[15:12], power down demux
    {REG_01C6, 0x111F, MASKBIT(15:0)}, // REG_PM_BANK_63[15:0]: [5]: TMDS ch0 PHDAC LDO
    {REG_01C8, 0, BIT5}, // REG_PM_BANK_64[15:0]
    {REG_01CC, 0, BIT7}, // REG_PM_BANK_66[7]: mhl sw mode pm
    {REG_1604, 0, MASKBIT(3:1)}, // REG_COMBO_DECODER_02[3:1]: swap for MHL v1.2, [2]: 0:SW mode, [1]: packed-pixel mode enable
    {REG_1606, BIT11 |(WORD)(MHL_VCO_CODE_CALCULATE(MHL_HDMI_VCO_LOOP_DIV2)), MASKBIT(15:0)}, // REG_COMBO_DECODER_03[15:0], threshold for vco div = 2, [11] : enable
    {REG_1608, BIT11 |(WORD)(MHL_VCO_CODE_CALCULATE(MHL_HDMI_VCO_LOOP_DIV4)), MASKBIT(15:0)}, // REG_COMBO_DECODER_04[15:0], threshold for vco div = 4, [11] : enable
    {REG_160A, BIT11 |(WORD)(MHL_VCO_CODE_CALCULATE(MHL_HDMI_VCO_LOOP_DIV8)), MASKBIT(15:0)}, // REG_COMBO_DECODER_05[15:0], threshold for vco div = 8, [11] : enable
    {REG_163C, MASKBIT(9:8), MASKBIT(12:8)}, // REG_COMBO_DECODER_1E[12:8]: CR lock success threshold time.
    {REG_17B0, (MHL_HDMI_B_CHANNEL_EQ <<8), MASKBIT(13:8)}, // REG_COMBO_ATOP_58[15:0], [13:0]: Ch1 EQ, [5:0]: Ch0 EQ(Clock)
    {REG_17BC, 0x0040 |MHL_PLL_LDO_VALUE, MASKBIT(15:0)}, // REG_COMBO_ATOP_5E[15:0], ATOP_TEST_PLL, add PLL LDO 5% setting
    {REG_17A2, BIT15, BIT15}, // REG_COMBO_ATOP_51[15], word bondary detect
    {REG_17BE, 0, MASKBIT(12:10) |BIT1}, // REG_COMBO_ATOP_5F[1][12:10]
    {REG_175C, 0, BIT13}, // REG_COMBO_ATOP_2E[13]: Mask clk stable int
    {REG_153C, 0, BIT2}, // REG_COMBO_HDCP_1E[2]: PP mode HDCP
    {REG_1542, 0, MASKBIT(12:0)}, // REG_COMBO_HDCP_21[12]: phase overwrite bit, [11:0]: TMDS ch0 phase code, TMDS ch0 phase release
    {REG_1544, 0, MASKBIT(12:0)}, // REG_COMBO_HDCP_22[12]: phase overwrite bit, [11:0]: TMDS ch0 phase code, TMDS ch1 phase release
    {REG_1546, 0, MASKBIT(12:0)}, // REG_COMBO_HDCP_23[12]: phase overwrite bit, [11:0]: TMDS ch0 phase code, TMDS ch2 phase release
    {REG_17AA, 0, BIT15 |MASKBIT(1:0)}, // REG_COMBO_ATOP_55[15]: MHL mode DIV selection overwrite, [1:0]: MHL mode DIV select for override control.
};

//-------------------------------------------------------------------------------------------------
//  MHL 24 bit mode table
//-------------------------------------------------------------------------------------------------
code msLoadTbl_S tMHL_24BIT_MODE_TABLE[] =
{
    {REG_01C4, BIT15| BIT14 |BIT12, MASKBIT(15:12)}, // REG_PM_BANK_62[15:12], power down demux
    {REG_01C6, 0xFFDF, MASKBIT(15:0)}, // REG_PM_BANK_63[15:0]: [5]: TMDS ch0 PHDAC LDO
    {REG_01C8, BIT5, BIT5}, // REG_PM_BANK_64[15:0], [14:12]: disable Cbus control R-term
    {REG_01CC, BIT7, BIT7}, // REG_PM_BANK_66[7]: mhl mode enable, REG_PM_BANK_65[7]: MHL software mode overwrite
    {REG_1604, 0, MASKBIT(3:1)}, // REG_COMBO_DECODER_02[3:1]: swap for MHL v1.2, [2]: 0:SW mode, [1]: packed-pixel mode enable
    {REG_163C, MASKBIT(12:8), MASKBIT(12:8)}, // REG_COMBO_DECODER_1E[12:8]: CR lock success threshold time.
    {REG_16C4, BIT11 |(WORD)(MHL_VCO_CODE_CALCULATE(MHL_24BIT_VCO_LOOP_DIV2)), MASKBIT(15:0)}, // REG_COMBO_DECODER_62[15:0], threshold for mhl vco div = 2, [11] : enable
    {REG_16C6, (WORD)(MHL_VCO_CODE_CALCULATE(MHL_24BIT_VCO_LOOP_DIV4)), MASKBIT(15:0)}, // REG_COMBO_DECODER_63[15:0], threshold for mhl vco div = 4, [11] : enable
    {REG_17AA, (MHL_ICONTROL_VALUE <<4), MASKBIT(6:4)}, // REG_COMBO_ATOP_55[7:4]: Set Icontrol override value
    {REG_17AC, BIT0, BIT0}, // REG_COMBO_ATOP_56[0], Override MHL icontrol
    {REG_17BC, BIT6 |BIT3 |BIT2 |MHL_PLL_LDO_VALUE, MASKBIT(15:0)}, // REG_COMBO_ATOP_5E[15:0], ATOP_TEST_PLL[3:2] = 2'b 11, add PLL LDO 5% setting
    {REG_17B0, (MHL_B_CHANNEL_EQ <<8), MASKBIT(13:8)}, // REG_COMBO_ATOP_58[15:0], [13:0]: Ch1 EQ, [5:0]: Ch0 EQ(Clock)
    //{REG_17B2, MASKBIT(15:0), 0x0B0B}, // REG_COMBO_ATOP_59[15:0], [13:0]: Ch3 EQ, [5:0]: Ch2 EQ
    {REG_17B4, BIT4, MASKBIT(15:0)}, // REG_COMBO_ATOP_5A[15:0], [7:4]: Test register for PDAC
    {REG_17BE, MASKBIT(12:10) |BIT1, MASKBIT(12:10) |BIT4|BIT1}, // REG_COMBO_ATOP_5F[1][12:10]
    {REG_17A2, BIT15, BIT15}, // REG_COMBO_ATOP_51[15], word bondary detect
    {REG_175C, BIT13, BIT13}, // REG_COMBO_ATOP_2E[13]: Mask clk stable int
    {REG_153C, 0, BIT2}, // REG_COMBO_HDCP_1E[2]: PP mode HDCP
    {REG_1542, BIT12 |MHL_CDR_PHASE_CODE, MASKBIT(12:0)}, // REG_COMBO_HDCP_21[12]: phase overwrite bit, [11:0]: TMDS ch0 phase code, TMDS ch0 phase overwrite
    {REG_1544, BIT12 |MHL_CDR_PHASE_CODE, MASKBIT(12:0)}, // REG_COMBO_HDCP_22[12]: phase overwrite bit, [11:0]: TMDS ch0 phase code, TMDS ch1 phase overwrite
    {REG_1546, BIT12 |MHL_CDR_PHASE_CODE, MASKBIT(12:0)}, // REG_COMBO_HDCP_23[12]: phase overwrite bit, [11:0]: TMDS ch0 phase code, TMDS ch2 phase overwrite
    {REG_17AA, BIT15 |BIT1, BIT15 |MASKBIT(1:0)}, // REG_COMBO_ATOP_55[15]: MHL mode DIV selection overwrite, [1:0]: MHL mode DIV select for override control.
};

//-------------------------------------------------------------------------------------------------
//  MHL packet pixel mode table
//-------------------------------------------------------------------------------------------------
code msLoadTbl_S tMHL_PACKET_PIXEL_MODE_TABLE[] =
{
    {REG_153C, BIT2, BIT2}, // REG_COMBO_HDCP_1E[2]: PP mode HDCP
    {REG_160A, BIT11 |(WORD)(MHL_VCO_CODE_CALCULATE(MHL_PPMODE_VCO_LOOP_DIV2)), MASKBIT(15:0)}, // REG_COMBO_DECODER_05[15:0], threshold for vco div = 8, [11] : enable
    {REG_1604, BIT3 |BIT1, MASKBIT(3:1)}, // REG_COMBO_DECODER_02[3:1]: swap for MHL v1.2, [2]: 0:SW mode, [1]: packed-pixel mode enable
    {REG_17AA, BIT15 |MASKBIT(1:0), BIT15 |MASKBIT(1:0)}, // REG_COMBO_DECODER_55[15]: MHL mode DIV selection overwrite, [1:0]: MHL mode DIV select for override control.
    {REG_17AA, (MHL_PP_ICONTROL_VALUE <<4), MASKBIT(6:4)}, // REG_COMBO_ATOP_55[7:4]: Set Icontrol override value
    {REG_17BE, MASKBIT(12:10)|BIT4 |BIT1, MASKBIT(12:10) |BIT4|BIT1}, // REG_COMBO_ATOP_5F[1][12:10]
};

//-------------------------------------------------------------------------------------------------
//  MHL Auto EQ table
//-------------------------------------------------------------------------------------------------
code msLoadTbl_S tMHL_AUTO_EQ_TABLE[] =
{
    {REG_164A, 0x0AF9, MASKBIT(15:0)}, // REG_COMBO_DECODER_25[15:0]
    {REG_164C, BIT11 |BIT9 |BIT3 |BIT1, MASKBIT(14:8) |MASKBIT(6:0)}, // REG_COMBO_DECODER_26
    {REG_164E, BIT9 |BIT8 |BIT1 |BIT0, MASKBIT(14:8) |MASKBIT(6:0)}, // REG_COMBO_DECODER_27
    {REG_1650, BIT10 |BIT8 |BIT7 |BIT1 |BIT0, MASKBIT(10:0)}, // REG_COMBO_DECODER_28[10:0]
    {REG_1652, 0x0003, MASKBIT(15:0)}, // REG_COMBO_DECODER_29[15:0]
    {REG_1654, 0x0400, MASKBIT(15:0)}, // REG_COMBO_DECODER_2A[15:0]
    {REG_1656, 0x0040, MASKBIT(15:0)}, // REG_COMBO_DECODER_2B[15:0]
    {REG_1658, 0x0002, MASKBIT(15:0)}, // REG_COMBO_DECODER_2C[15:0]
    {REG_165A, 0x0028, MASKBIT(15:0)}, // REG_COMBO_DECODER_2D[15:0]
    // Not overwrite EQ
    {REG_17AC, 0, MASKBIT(7:5)}, // REG_COMBO_ATOP_56[7:5]
};

//-------------------------------------------------------------------------------------------------
//  MHL power on table
//-------------------------------------------------------------------------------------------------
code msLoadTbl_S tMHL_POWER_ON_TABLE[] =
{
    {REG_0BD8, BIT14, BIT14}, // REG_MHL_CBUS_6C[14]: Register xdata enable
    {REG_0C02, 0, MASKBIT(5:2)}, // REG_PM_MHL_CBUS_0C[5][3][2]
};

//-------------------------------------------------------------------------------------------------
//  MHL power down table
//-------------------------------------------------------------------------------------------------
code msLoadTbl_S tMHL_POWER_DOWN_TABLE[] =
{
    {REG_0260, MASKBIT(1:0), MASKBIT(1:0)}, // REG_PM_GPIO_30[1:0], CBUS port selsct [1:0]
    //{REG_0262, BIT2 |BIT3 |BIT5, BIT2 |BIT3 |BIT5}, // REG_PM_GPIO_31[5][3][2],  Clear all PE_CTRL
    {REG_0BD8, 0, BIT14}, // REG_MHL_CBUS_6C[14]: Register xdata enable
    {REG_0C02, BIT5 |BIT3 |BIT2, MASKBIT(5:2)}, // REG_PM_MHL_CBUS_0C[5][3][2]
    {REG_01C4, MASKBIT(15:0), MASKBIT(15:0)}, // power down r-term
};

//-------------------------------------------------------------------------------------------------
//  MHL power saving table
//-------------------------------------------------------------------------------------------------
code msLoadTbl_S tMHL_POWER_SAVING_TABLE[] =
{
    {REG_0BD8, BIT14, BIT14}, // REG_MHL_CBUS_6C[14]: Register xdata enable // 130220 coding test
    {REG_1542, 0, MASKBIT(12:0)}, // REG_COMBO_HDCP_21[12]: phase overwrite bit, [11:0]: TMDS ch0 phase code, TMDS ch0 phase release
    {REG_1544, 0, MASKBIT(12:0)}, // REG_COMBO_HDCP_22[12]: phase overwrite bit, [11:0]: TMDS ch0 phase code, TMDS ch1 phase release
    {REG_1546, 0, MASKBIT(12:0)}, // REG_COMBO_HDCP_23[12]: phase overwrite bit, [11:0]: TMDS ch0 phase code, TMDS ch2 phase release
    {REG_1604, 0, MASKBIT(3:1)}, // REG_COMBO_DECODER_02[3:1]: swap for MHL v1.2, [2]: 0:SW mode, [1]: packed-pixel mode enable
    {REG_17AA, 0, BIT15 |MASKBIT(1:0)}, // REG_COMBO_ATOP_55[15]: MHL mode DIV selection overwrite, [1:0]: MHL mode DIV select for override control.
};

#if(MHL_PHASE_CHECK_PATCH)
//-------------------------------------------------------------------------------------------------
//  MHL power saving table
//-------------------------------------------------------------------------------------------------
code WORD tMHL_PHASE_CODE_TABLE[] =
{
    0x1189, // Phase 0
    0x11C8, // Phase 1
    0x1158, // Phase 2
    0x1119, // Phase 3
    0x131B, // Phase 4
    0x135A, // Phase 5
    0x13DE, // Phase 6
    0x139F, // Phase 7
    0x129D, // Phase 8
    0x12DC, // Phase 9
    0x1254, // Phase 10
    0x1215, // Phase 11
    0x1617, // Phase 12
    0x1656, // Phase 13
    0x16D2, // Phase 14
    0x1693, // Phase 15
    0x1791, // Phase 16
    0x17D0, // Phase 17
    0x1770, // Phase 18
    0x1731, // Phase 19
    0x1533, // Phase 20
    0x1572, // Phase 21
    0x15F6, // Phase 22
    0x15B7, // Phase 23
    0x14B5, // Phase 24
    0x14F4, // Phase 25
    0x1464, // Phase 26
    0x1425, // Phase 27
    0x1C27, // Phase 28
    0x1C66, // Phase 29
    0x1CC6, // Phase 30
    0x1C87, // Phase 31
    0x1D85, // Phase 32
    0x1DC4, // Phase 33
    0x1D4C, // Phase 34
    0x1D0D, // Phase 35
    0x190F, // Phase 36
    0x194E, // Phase 37
    0x19CA, // Phase 38
    0x198B, // Phase 39
};

#endif

//**************************************************************************
//  [Function Name]:
//                  _mhal_mhl_CbusInterruptMask()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _mhal_mhl_CbusInterruptMask(Bool bFlag)
{
    if(bFlag)
    {
        msWriteByteMask(REG_2B1A, 0, BIT0); // Cbus int mask
    }
    else
    {
        msWriteByteMask(REG_2B1A, BIT0, BIT0); // Cbus int mask
    }
}

//**************************************************************************
//  [Function Name]:
//                  _mhal_mhl_LoadHdmiBypassTbl()
//  [Description]
//                  MHL HDMI bypass setting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _mhal_mhl_LoadHdmiBypassTbl(void)
{
    BYTE xdata uctemp = 0;

    for(uctemp = 0; uctemp <(sizeof(tMHL_HDMI_BYPASS_TABLE) /sizeof(msLoadTbl_S)); uctemp++)
    {
        msWrite2ByteMask(tMHL_HDMI_BYPASS_TABLE[uctemp].addr, tMHL_HDMI_BYPASS_TABLE[uctemp].databuf, tMHL_HDMI_BYPASS_TABLE[uctemp].mask);
    }

#if (MHL_PLL_DIV_OVERWRITE)
    msWriteByteMask(REG_17AB, 0, MASKBIT(6:5)); // REG_COMBO_ATOP_55[14:13], PLL DIV selection overwrite
#endif

    // Auto clear phase
    msWriteBit(REG_1569, FALSE, BIT0);
    msWriteBit(REG_1569, TRUE, BIT0);
    msWriteBit(REG_1569, FALSE, BIT0);

    msEread_SetHDMIInitialValue();

    if(ucMHLDriver >= MHL_DRIVER_U04)
    {
        msWriteByteMask(REG_17BE, 0, BIT4);
    }
}

//**************************************************************************
//  [Function Name]:
//                  _mhal_mhl_LoadMhl24bitsModeTbl()
//  [Description]
//                  MHL 24 bits mode setting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _mhal_mhl_LoadMhl24bitsModeTbl(void)
{
    BYTE xdata uctemp = 0;

    for(uctemp = 0; uctemp <(sizeof(tMHL_24BIT_MODE_TABLE) /sizeof(msLoadTbl_S)); uctemp++)
    {
        msWrite2ByteMask(tMHL_24BIT_MODE_TABLE[uctemp].addr, tMHL_24BIT_MODE_TABLE[uctemp].databuf, tMHL_24BIT_MODE_TABLE[uctemp].mask);
    }

#if(MHL_PLL_DIV_OVERWRITE)
    msWriteByteMask(REG_17AB, MASKBIT(6:5), MASKBIT(6:5)); // REG_COMBO_ATOP_55[14:13], PLL DIV selection overwrite
    msWriteByteMask(REG_17AA, 0, MASKBIT(3:2)); // REG_COMBO_ATOP_55[3:2], PLL DIV select for override control
    msWriteByteMask(REG_17A5, 0, MASKBIT(3:2)); // REG_COMBO_ATOP_52[11:10], Phase DAC DVI selection override value

#endif

    msEread_SetMHLInitialValue();

#if(MHL_FORCE_VCO_LOOP_DEV2)
    msWriteByteMask(REG_17AB, MASKBIT(6:5), MASKBIT(6:5)); // REG_COMBO_ATOP_55[14:13], PLL DIV selection overwrite
    msWriteByteMask(REG_17AA, BIT2, MASKBIT(3:2));  // REG_COMBO_ATOP_55[3:2], PLL DIV select for override control
    msWriteByteMask(REG_17A5, BIT2, MASKBIT(3:2)); // REG_COMBO_ATOP_52[11:10], Phase DAC DVI selection override value
    msWriteByteMask(REG_17AA, (MHL_ICONTROL_VALUE << 4), MASKBIT(6:4)); // REG_COMBO_ATOP_55[7:4]: Set Icontrol override value
    msWriteByteMask(REG_17BC, 0, BIT6); // REG_COMBO_ATOP_5E[6]

#endif

    if(ucMHLDriver >= MHL_DRIVER_U04)
    {
        msWriteByteMask(REG_17BE, 0, BIT4);
    }
}

//**************************************************************************
//  [Function Name]:
//                  _mhal_mhl_LoadMhlPackedPixelModeTbl()
//  [Description]
//                  MHL packed pixel mode setting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _mhal_mhl_LoadMhlPackedPixelModeTbl(void)
{
    BYTE xdata uctemp = 0;

    for(uctemp = 0; uctemp <(sizeof(tMHL_PACKET_PIXEL_MODE_TABLE) /sizeof(msLoadTbl_S)); uctemp++)
    {
        msWrite2ByteMask(tMHL_PACKET_PIXEL_MODE_TABLE[uctemp].addr, tMHL_PACKET_PIXEL_MODE_TABLE[uctemp].databuf, tMHL_PACKET_PIXEL_MODE_TABLE[uctemp].mask);
    }

    if(ucMHLDriver >= MHL_DRIVER_U04)
    {
        msWriteByteMask(REG_17BE, BIT4, BIT4);
    }
}

//**************************************************************************
//  [Function Name]:
//                  _mhal_mhl_LoadMhl24bitsModeTbl()
//  [Description]
//                  MHL 24 bits mode setting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _mhal_mhl_LoadMhlAutoEQTbl(void)
{
    BYTE xdata uctemp = 0;

    for(uctemp = 0; uctemp <(sizeof(tMHL_AUTO_EQ_TABLE) /sizeof(msLoadTbl_S)); uctemp++)
    {
        msWrite2ByteMask(tMHL_AUTO_EQ_TABLE[uctemp].addr, tMHL_AUTO_EQ_TABLE[uctemp].databuf, tMHL_AUTO_EQ_TABLE[uctemp].mask);
    }
}

#if(DMHL_LG_PRADA_PATCH)
//**************************************************************************
//  [Function Name]:
//                  _mhal_mhl_AdjustCommonModeResistor()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _mhal_mhl_AdjustCommonModeResistor(Bool bFlag, BYTE ucCbusPath) 
{
    switch(ucCbusPath)
    {
#if(MHL_SUPPORT_PORT0)
        case MHL_CBUS_SELECT_PORT0: // Port B
            if(bFlag)
            {
                msWriteByteMask(REG_01C2, 0, BIT5); // REG_PM_BANK_61[7:5], port siwtch
            }
            else
            {
                msWriteByteMask(REG_01C2, BIT5, BIT5); // REG_PM_BANK_61[7:5], port siwtch
            }
            break;

#endif

#if(MHL_SUPPORT_PORT1)
        case MHL_CBUS_SELECT_PORT1: // Port A
            if(bFlag)
            {
                msWriteByteMask(REG_01C2, 0, BIT6); // REG_PM_BANK_61[7:5], port siwtch
            }
            else
            {
                msWriteByteMask(REG_01C2, BIT6, BIT6); // REG_PM_BANK_61[7:5], port siwtch
            }
            break;

#endif

#if(MHL_SUPPORT_PORT2)
        case MHL_CBUS_SELECT_PORT2: // Port C
            if(bFlag)
            {
                msWriteByteMask(REG_01C2, 0, BIT7); // REG_PM_BANK_61[7:5], port siwtch
            }
            else
            {
                msWriteByteMask(REG_01C2, BIT7, BIT7); // REG_PM_BANK_61[7:5], port siwtch
            }
            break;

#endif

        default:
            bFlag = bFlag;
            break;
    };
}
#endif

//**************************************************************************
//  [Function Name]:
//                  _mhal_mhl_CheckCable()
//  [Description]
//                  MHL cable detection
//  [Arguments]:
//
//  [Return]:
//                  TRUE: MHL cable plugged
//                  FALSE: MHL cable unplugged
//**************************************************************************
Bool _mhal_mhl_CheckCable(Bool bCharge, BYTE ucPort)
{
    Bool bindex = FALSE;

    switch(ucPort)
    {
#if(MHL_SUPPORT_PORT0)
        case MHL_CBUS_SELECT_PORT0:
            if(GET_MHL_CABLE_DETECT_ENABLE0() && (MHL_CABLE_SAR_DETECT_PORT0 >= CBUS_CABLE_DETECT_LEVEL))
            {
                bindex = TRUE;
            }

            if(GET_MHL_VBUS_PORT0_FLAG() != bindex)
            {
                if(bindex)
                {
                    //CLR_HDMI_HPD_PORT0();
                    SET_HPD_TRISTSTE_PORT0();

                    if(bCharge)
                    {
                        SET_MHL_VBUS_PORT0_FLAG();
                        SET_VBUS_CHARGE_PORT0();

                        MHL_HAL_DPUTSTR("** MHL Vbus charge port0\n");
                    }
                }
                else
                {
                    CLR_MHL_VBUS_PORT0_FLAG();
                    CLR_VBUS_CHARGE_PORT0();
                    SET_HDMI_HPD_PORT0();
                    CLR_MHL_CELL_PHONE_ID_PORT0();

                    MHL_HAL_DPUTSTR("** MHL Vbus discharge port0\n");
                }
            }

            break;

#endif

#if(MHL_SUPPORT_PORT1)
        case MHL_CBUS_SELECT_PORT1:
            if(GET_MHL_CABLE_DETECT_ENABLE1() && (MHL_CABLE_SAR_DETECT_PORT1 >= CBUS_CABLE_DETECT_LEVEL))
            {
                bindex = TRUE;
            }

            if(GET_MHL_VBUS_PORT1_FLAG() != bindex)
            {
                if(bindex)
                {
                    //CLR_HDMI_HPD_PORT1();
                    SET_HPD_TRISTSTE_PORT1();

                    if(bCharge)
                    {
                        SET_MHL_VBUS_PORT1_FLAG();
                        SET_VBUS_CHARGE_PORT1();

                        MHL_HAL_DPUTSTR("** MHL Vbus charge port1\n");
                    }
                }
                else
                {
                    CLR_MHL_VBUS_PORT1_FLAG();
                    CLR_VBUS_CHARGE_PORT1();
                    SET_HDMI_HPD_PORT1();
                    CLR_MHL_CELL_PHONE_ID_PORT1();

                    MHL_HAL_DPUTSTR("** MHL Vbus discharge port1\n");
                }
            }

            break;

#endif

#if(MHL_SUPPORT_PORT2)
        case MHL_CBUS_SELECT_PORT2:
            if(GET_MHL_CABLE_DETECT_ENABLE2() && (MHL_CABLE_SAR_DETECT_PORT2 >= CBUS_CABLE_DETECT_LEVEL))
            {
                bindex = TRUE;
            }

            if(GET_MHL_VBUS_PORT2_FLAG() != bindex)
            {
                if(bindex)
                {
                    //CLR_HDMI_HPD_PORT2();
                    SET_HPD_TRISTSTE_PORT2();

                    if(bCharge)
                    {
                        SET_MHL_VBUS_PORT2_FLAG();
                        SET_VBUS_CHARGE_PORT2();

                        MHL_HAL_DPUTSTR("** MHL Vbus charge port2\n");
                    }
                }
                else
                {
                    CLR_MHL_VBUS_PORT2_FLAG();
                    CLR_VBUS_CHARGE_PORT2();
                    SET_HDMI_HPD_PORT2();
                    CLR_MHL_CELL_PHONE_ID_PORT2();

                    MHL_HAL_DPUTSTR("** MHL Vbus discharge port2\n");
                }
            }

            break;

#endif

        default:

            break;
    };

    return bindex;
}

//**************************************************************************
//  [Function Name]:
//                  _mhal_mhl_CbusStucktoLow()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _mhal_mhl_CbusStucktoLow(Bool bFlag)
{
    if(bFlag)
    {
        msWriteByteMask(REG_0C30, 0, BIT1); // REG_PM_MHL_CBUS_18[1]: cbus stuck to low int mask
    }
    else
    {
        msWriteByteMask(REG_0C30, BIT1, BIT1); // REG_PM_MHL_CBUS_18[1]: cbus stuck to low int mask
    }

    msWriteByteMask(REG_0C30, BIT0, BIT0); // REG_PM_MHL_CBUS_18[0]
}

//**************************************************************************
//  [Function Name]:
//                  _mhal_mhl_CbusWakeupInterrupt()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _mhal_mhl_CbusWakeupInterrupt(Bool bFlag)
{
    if(bFlag)
    {
        msWriteByteMask(REG_0C30, 0, BIT5); // REG_PM_MHL_CBUS_18[5]: wake up pulse int mask
    }
    else
    {
        msWriteByteMask(REG_0C30, BIT5, BIT5); // REG_PM_MHL_CBUS_18[5]: wake up pulse int mask
    }

    msWriteByteMask(REG_0C30, BIT4, BIT4); // REG_PM_MHL_CBUS_18[4]: Clear wake up pulse int flag
}

//**************************************************************************
//  [Function Name]:
//                  _mhal_mhl_SetCableDetect()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _mhal_mhl_SetCableDetect(Bool bFlag)
{
    if(bFlag)
    {
        msWriteByteMask(REG_0C01, MASKBIT(1:0), MASKBIT(1:0)); // REG_PM_MHL_CBUS_00[9:8], SW control cable detect
        msWriteByteMask(REG_0C40, 0, BIT5); // REG_PM_MHL_CBUS_20[5],
    }
    else
    {
        msWriteByteMask(REG_0C01, BIT1, MASKBIT(1:0)); // REG_PM_MHL_CBUS_00[9:8], SW control cable detect
        msWriteByteMask(REG_0C40, BIT5, BIT5); // REG_PM_MHL_CBUS_20[5],
    }
}

#if(MHL_PLL_DIV_OVERWRITE)
//**************************************************************************
//  [Function Name]:
//                  _mhal_mhl_CDRPhySetting()
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _mhal_mhl_CDRPhySetting(WORD usCDRClk)
{
    BYTE xdata uctemp = msReadByte(REG_1604) &BIT1; // REG_COMBO_DECODER_02[1]: packed-pixel mode enable
    WORD xdata usDivSelect = msReadByte(REG_17AA) &MASKBIT(3:2); // REG_COMBO_ATOP_55[3:2], PLL DIV select for override control

    usCDRClk = CBUS_CDR_CLOCK_CALCULATE(usCDRClk);

    if(uctemp > 0) // Packet pixel mode
    {
        uctemp = MHL_PPMODE_VCO_LOOP_DIV2;
    }
    else // 24 bit mode
    {
        uctemp = MHL_24BIT_VCO_LOOP_DIV2;
    }

    if(usCDRClk > uctemp)   // Div1
    {
        uctemp = 0;
#if(MHL_PHASE_CHECK_PATCH)
        ucPLLDivMode = 1;
#endif
    }
    else // Div2
    {
        uctemp = BIT0;
#if(MHL_PHASE_CHECK_PATCH)
        ucPLLDivMode = 2;
#endif
    }

    if(usDivSelect != uctemp)
    {
        usDivSelect = uctemp;
        usDivSelect = (usDivSelect <<10) |(uctemp <<2);

        msWriteByteMask(REG_17AB, MASKBIT(6:5), MASKBIT(6:5)); // REG_COMBO_ATOP_55[14:13], PLL DIV selection overwrite
        msWriteByteMask(REG_17AA, usDivSelect, MASKBIT(3:2));   // REG_COMBO_ATOP_55[3:2], PLL DIV select for override control
        msWrite2ByteMask(REG_17A4, usDivSelect, MASKBIT(11:10)); // REG_COMBO_ATOP_52[11:10], Phase DAC DVI selection override value
    }
}

#endif

#if(MHL_PHASE_CHECK_PATCH)
//**************************************************************************
//  [Function Name]:
//                  _mhal_mhl_PhaseDebounce()
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _mhal_mhl_PhaseDebounce(void)
{
    WORD xdata uctemp = 0;

    for(uctemp = 0; uctemp <2000; uctemp++)
    {
        DELAY1US();
    }
}

//**************************************************************************
//  [Function Name]:
//                  _mhal_mhl_CDRCheckPhase()
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool _mhal_mhl_CDRCheckPhase(Bool bFlag)
{
    static WORD xdata ucPhase = 0;
    static BYTE xdata ucState = MHL_PHASE_CHECK_START;
    Bool bError = FALSE;
    Bool bFinish = FALSE;
    WORD xdata uctemp = 0;
#if(MHL_PHASE_CHECK_PATCH == MHL_PHASE_PATH_II)
    static WORD xdata u16GoodPhaseCnt = 0;
#endif

    if(ucMHLDriver <= MHL_DRIVER_U02)
    {
        if(bFlag)
        {
#if(MHL_PHASE_CHECK_PATCH == MHL_PHASE_PATH_I)
            if(!GET_PHASE_CHECK_TIMER())
            {
                if((ucPhase > 0) || (ucState > MHL_PHASE_CHECK_START))
                {
                    if(msReadByte(REG_14EC) & BIT5) // check DE difference
                    {
                        bError = TRUE;

                        msWriteByte(REG_14EC, BIT5); // clear DE difference status
                    }

                    switch(ucState)
                    {
                        case MHL_PHASE_CHECK_START:
                            if(bError)
                            {
                                ucState = MHL_PHASE_ERROR_HAPPEN;
                            }
                            else
                            {
                                ucState = MHL_PHASE_WAIT_ERROR;
                            }

                            break;

                        case MHL_PHASE_WAIT_ERROR:
                            if(bError)
                            {
                                bFinish = TRUE;

                                ucPhase %= MHL_PHASE_CODE_NUM;

                                MHL_HAL_DPRINTF("** MHL phase wait error happen in phase %d\n", ucPhase);

                                for(uctemp = 1; uctemp < ((MHL_PHASE_CODE_MARGIN *ucPLLDivMode) +MHL_PHASE_CHECK_STEP +1); uctemp++)
                                {
                                    msWrite2ByteMask(REG_1542, tMHL_PHASE_CODE_TABLE[(ucPhase + (MHL_PHASE_CHECK_GAIN *MHL_PHASE_CODE_NUM *ucPLLDivMode) -uctemp -1) %MHL_PHASE_CODE_NUM], MASKBIT(12:0));
                                    _mhal_mhl_PhaseDebounce();
                                }

                                ucPhase = (ucPhase + (MHL_PHASE_CHECK_GAIN *MHL_PHASE_CODE_NUM *ucPLLDivMode) -(uctemp -1) -1) %MHL_PHASE_CODE_NUM;

                                //ucState = MHL_PHASE_CHECK_POLLING;
                            }
                            else if(ucPhase > ((MHL_PHASE_CODE_MARGIN *ucPLLDivMode) << 1))
                            {
                                bFinish = TRUE;

                                ucPhase %= MHL_PHASE_CODE_NUM;

                                MHL_HAL_DPRINTF("** MHL phase wait error finish in phase %d\n", ucPhase);

                                for(uctemp = 1; uctemp < ((MHL_PHASE_CODE_MARGIN *ucPLLDivMode) +1); uctemp++)
                                {
                                    msWrite2ByteMask(REG_1542, tMHL_PHASE_CODE_TABLE[(ucPhase +(MHL_PHASE_CHECK_GAIN *MHL_PHASE_CODE_NUM *ucPLLDivMode) -uctemp -1) %MHL_PHASE_CODE_NUM], MASKBIT(12:0));
                                    _mhal_mhl_PhaseDebounce();
                                }

                                ucPhase = (ucPhase +(MHL_PHASE_CHECK_GAIN *MHL_PHASE_CODE_NUM *ucPLLDivMode) -(uctemp -1) -1) %MHL_PHASE_CODE_NUM;

                                //ucState = MHL_PHASE_CHECK_POLLING;
                            }
                            if(bFinish)
                            {
                                msWriteByte(REG_14EC, BIT5); // clear DE difference status
                            }
                            break;

                        case MHL_PHASE_ERROR_HAPPEN:
                            if(!bError)
                            {
                                bFinish = TRUE;

                                ucPhase %= MHL_PHASE_CODE_NUM;

                                MHL_HAL_DPRINTF("** MHL phase error happen finish in phase %d\n", ucPhase);

                                for(uctemp = 0; uctemp <(MHL_PHASE_CODE_MARGIN *ucPLLDivMode); uctemp++)
                                {
                                    msWrite2ByteMask(REG_1542, tMHL_PHASE_CODE_TABLE[(ucPhase +uctemp) %MHL_PHASE_CODE_NUM], MASKBIT(12:0));
                                    _mhal_mhl_PhaseDebounce();
                                }

                                ucPhase = (ucPhase +(uctemp -1)) %MHL_PHASE_CODE_NUM;

                                //ucState = MHL_PHASE_CHECK_POLLING;
                                msWriteByte(REG_14EC, BIT5); // clear DE difference status
                            }

                            break;

                        case MHL_PHASE_CHECK_POLLING:
                            if(bError)
                            {
                                for(uctemp = 1; uctemp < (MHL_PHASE_JUMP_STEP *ucPLLDivMode +1); uctemp++)
                                {
                                    msWrite2ByteMask(REG_1542, tMHL_PHASE_CODE_TABLE[(ucPhase +uctemp) %MHL_PHASE_CODE_NUM], MASKBIT(12:0));
                                    _mhal_mhl_PhaseDebounce();
                                }

                                ucPhase = (ucPhase +(uctemp -1)) %MHL_PHASE_CODE_NUM;

                                MHL_HAL_DPRINTF("** MHL phase polling error jump to phase %d\n", ucPhase);

                                SET_PHASE_CHECK_TIMER(MHL_PHASE_POLLING_INTERVAL);

                                msWriteByte(REG_14EC, BIT5); // clear DE difference status
                            }
                            else
                            {
                    #if 0
                                msWriteByteMask(REG_1753, BIT5, BIT5); // 0x1729[13]: DC FIFO reset
                                msWriteByteMask(REG_1753, 0, BIT5); // 0x1729[13]: DC FIFO reset

                                if(ucMHLDriver <= MHL_DRIVER_U02)
                                {
                                    msWriteByteMask(REG_17A8, BIT7, BIT7); // REG_COMBO_ATOP_54[7], PLL PD/PFD override value
                                    msWriteByteMask(REG_17BD, BIT7, BIT7); // REG_COMBO_ATOP_5E[7]
                                }
                    #endif
                                bFinish = TRUE;

                                ucState = MHL_PHASE_CHECK_FINISH;
                            }

                            break;

                        case MHL_PHASE_CHECK_FINISH:
                        default:

                            break;
                    }
                }

                if(bFinish)
                {
                    if(ucState < MHL_PHASE_CHECK_POLLING)
                    {
                        MHL_HAL_DPRINTF("** MHL phase finally set in phase %d\n", ucPhase);

                        msWriteByte(REG_14EC, BIT5); // clear DE difference status

                        //SET_PHASE_CHECK_TIMER(MHL_PHASE_POLLING_INTERVAL);
                        ucState = MHL_PHASE_CHECK_FINISH;
                        ucPhase = 0;
                        //bFinish = FALSE;
                    }
                }
                else if(ucState < MHL_PHASE_CHECK_POLLING)
                {
                    if(ucPhase == 0)
                    {
                        msWrite2ByteMask(REG_1542, tMHL_PHASE_CODE_TABLE[ucPhase], MASKBIT(12:0));
                        _mhal_mhl_PhaseDebounce();
                        _mhal_mhl_PhaseDebounce();
                        _mhal_mhl_PhaseDebounce();
                        ucPhase++;
                    }
                    else
                    {
                        for(uctemp = 0; uctemp < MHL_PHASE_CHECK_STEP; uctemp++)
                        {
                            msWrite2ByteMask(REG_1542, tMHL_PHASE_CODE_TABLE[(ucPhase +uctemp) %MHL_PHASE_CODE_NUM], MASKBIT(12:0));
                            _mhal_mhl_PhaseDebounce();
                        }

                        ucPhase += MHL_PHASE_CHECK_STEP;
                    }

                    msWriteByte(REG_14EC, BIT5); // clear DE difference status
                    _mhal_mhl_PhaseDebounce();
                    msWriteByte(REG_14EC, BIT5); // clear DE difference status

                    if(ucPhase == 1)
                        SET_PHASE_CHECK_TIMER(MHL_PHASE_CHECK_INTERVAL*3);
                    else
                        SET_PHASE_CHECK_TIMER(MHL_PHASE_CHECK_INTERVAL);
                }
            }

#elif (MHL_PHASE_CHECK_PATCH == MHL_PHASE_PATH_II)
            if(!GET_PHASE_CHECK_TIMER() && (ucState == MHL_PHASE_CHECK_START))
            {
                if(ucPhase > 0)
                {
                #if 0
                     if(ucPhase != 0)
                     {
                     //MHL_HAL_DPRINTF("** ucState %d\n", ucState);
                     MHL_HAL_DPRINTF("** MHL phase %d\n", ucPhase);
                     MHL_HAL_DPRINTF("** Error status 0x%x\n", msReadByte(REG_14EC));
                     }
                #endif
                    if(msReadByte(REG_14EC) & BIT5) // check DE difference
                    {
                        bError = TRUE;
                    }
                    if(bError)
                    {
                        u16GoodPhaseCnt = 0;
                        MHL_HAL_DPRINTF("** Error phase %d\n", ucPhase);
                    }
                    else
                    {
                        if(u16GoodPhaseCnt == 0)
                            MHL_HAL_DPRINTF("** Good phase start %d\n", ucPhase);
                        u16GoodPhaseCnt++;
                        if(u16GoodPhaseCnt > (((MHL_PHASE_CODE_MARGIN - MHL_PHASE_TOLERANCE)*ucPLLDivMode*2)/MHL_PHASE_CHECK_STEP))
                        {
                            bFinish = TRUE;
                        }
                    }

                }

                if(bFinish)
                {
                    for(uctemp = 1; uctemp < ((MHL_PHASE_CODE_MARGIN *ucPLLDivMode-MHL_PHASE_TOLERANCE) +1); uctemp++)
                    {
                        msWrite2ByteMask(REG_1542, tMHL_PHASE_CODE_TABLE[(ucPhase -uctemp -1) %MHL_PHASE_CODE_NUM], MASKBIT(12:0));
                        _mhal_mhl_PhaseDebounce();
                    }
                    msWriteByte(REG_14EC, BIT5); // clear DE difference status
                    u16GoodPhaseCnt = 0;
                    ucState = MHL_PHASE_CHECK_FINISH;
                    MHL_HAL_DPRINTF("** MHL phase finally set in phase %d\n", ucPhase -uctemp);
                }
                else
                {
                    if(ucPhase == 0)
                    {
                        msWrite2ByteMask(REG_1542, tMHL_PHASE_CODE_TABLE[ucPhase], MASKBIT(12:0));
                        _mhal_mhl_PhaseDebounce();
                        ucPhase++;
                    }
                    else
                    {
                        for(uctemp = 0; uctemp < MHL_PHASE_CHECK_STEP; uctemp++)
                        {
                            msWrite2ByteMask(REG_1542, tMHL_PHASE_CODE_TABLE[(ucPhase +uctemp) %MHL_PHASE_CODE_NUM], MASKBIT(12:0));
                            _mhal_mhl_PhaseDebounce();
                        }
                        ucPhase += MHL_PHASE_CHECK_STEP;
                    }

                    msWriteByte(REG_14EC, BIT5); // clear DE difference status
                    _mhal_mhl_PhaseDebounce();
                    msWriteByte(REG_14EC, BIT5); // clear DE difference status

                    if(ucPhase == 1)
                        SET_PHASE_CHECK_TIMER(MHL_PHASE_CHECK_INTERVAL*3);
                    else
                    SET_PHASE_CHECK_TIMER(MHL_PHASE_CHECK_INTERVAL);
                }
            }

#endif
        }
        else
        {
            ucPhase = 0;
            ucState = MHL_PHASE_CHECK_START;

            msWriteByte(REG_14EC, BIT5); // clear DE difference status
            _mhal_mhl_PhaseDebounce();
            msWriteByte(REG_14EC, BIT5); // clear DE difference status
        }
    }
    else
    {
        if(bFlag)
        {
            if(ucPhase < MHL_PHASE_CHANGE_MASK)
            {
                ucPhase++;
            }

            if(ucPhase == MHL_FORCE_CDR_COUNTER)
            {
                bFinish = TRUE;
            }
        }
        else
        {
            ucPhase = 0;
        }
    }

    return bFinish;
}

#endif

//**************************************************************************
//  [Function Name]:
//                  _mhal_mhl_ClockBigChangeFlag()
//  [Description]:
//                  
//  [Arguments]:
//                  
//  [Return]:
//                 
//**************************************************************************
Bool _mhal_mhl_ClockBigChangeFlag(void)
{
    Bool bindex = ((msReadByte(REG_1756) & BIT4) ? TRUE : FALSE); // REG_COMBO_ATOP_2B[4]

    if(bindex)
    {
        msWriteByteMask(REG_175A, BIT4, BIT4); // REG_COMBO_ATOP_2D[4]
        msWriteByteMask(REG_175A, 0, BIT4); // REG_COMBO_ATOP_2D[4]
    }
    
    return bindex;
}

//**************************************************************************
//  [Function Name]:
//                  _mhal_mhl_CheckClockStatus()
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool _mhal_mhl_CheckClockStatus(WORD usCDRClk)
{
    Bool bStable = FALSE;
    BYTE xdata ucPPMode = msReadByte(REG_1604) &BIT1; // REG_COMBO_DECODER_02[1]: packed-pixel mode enable
    WORD xdata usClockDetectLevel = CBUS_CLOCK_24_DETECT_LEVEL;
    static WORD xdata usPreCDRClk = 0;

    if(ucPPMode > 0)
    {
        usClockDetectLevel = CBUS_CLOCK_PP_DETECT_LEVEL;
    }
    
    if((usCDRClk < usClockDetectLevel) || _mhal_mhl_ClockBigChangeFlag())
    {
        // Do nothing
    }
    else if(msReadByte(REG_165C) & BIT3) // REG_COMBO_DECODER_2E[3], Clock stable indication.
    {
        if(!GET_MHL_SYSTEM_PHY_SETTING())
        {
#if(MHL_PLL_DIV_OVERWRITE)
            _mhal_mhl_CDRPhySetting(usCDRClk);
#endif

            SET_MHL_SYSTEM_PHY_SETTING();
        }

        bStable = TRUE;
    }

    usPreCDRClk = usCDRClk;

    return bStable;
}

//**************************************************************************
//  [Function Name]:
//                  _mhal_mhl_PullDown1KSetting()
//  [Description]
//                  
//  [Arguments]:
//                  
//  [Return]:
//
//**************************************************************************
void _mhal_mhl_PullDown1KSetting(void)
{
    Bool bFlag = FALSE;
    BYTE xdata uctemp = msEread_GetDataFromEfuse(0x0138);

    if((uctemp &BIT3) == BIT3)  // Trimming flag
    {
        if((uctemp &BIT0) == BIT0)  // PD 1K low
        {
            bFlag = TRUE;
        }

        msWriteByteMask(REG_0C40, bFlag ?BIT2 :0, BIT2); 

        if((uctemp &BIT1) == BIT1)  // PD 1K high
        {
            msWriteByteMask(REG_07AA, MASKBIT(6:4), MASKBIT(6:4));
        }

        if(ucMHLDriver < MHL_DRIVER_U04) // Junior U04 have no reversion, use trimming bit to update
        {
            ucMHLDriver = MHL_DRIVER_U04;
        }
    }
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

#if(MHL_CBUS_USE_INTERRUPT)
//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_CbusFloating()
//  [Description]
//                  MHL cable floating
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_mhl_IsrMaskControl(Bool bFlag)
{
    if(bFlag)
    {
        MEM_MSWRITE_BYTE_MASK(REG_2B1A, BIT0, BIT0);
    }
    else
    {
        MEM_MSWRITE_BYTE_MASK(REG_2B1A, 0, BIT0);
    }
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_IsrRtermOff()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_mhl_IsrRtermOff(void)
{
    switch(GET_MHL_CBUS_SELECT_PATH())
    {
#if(MHL_SUPPORT_PORT0)
        case MHL_CBUS_SELECT_PORT0: // Port B
            MEM_MSWRITE_BYTE_MASK(REG_01C4, BIT1, BIT1); // REG_PM_BANK_62[3:0], Power down all rterm.
            break;

#endif

#if(MHL_SUPPORT_PORT1)
        case MHL_CBUS_SELECT_PORT1: // Port A
            MEM_MSWRITE_BYTE_MASK(REG_01C4, BIT5, BIT5); // REG_PM_BANK_62[7:4], Power down all rterm.
            break;

#endif

#if(MHL_SUPPORT_PORT2)
        case MHL_CBUS_SELECT_PORT2: // Port C
            MEM_MSWRITE_BYTE_MASK(REG_01C5, BIT1, BIT1); // REG_PM_BANK_62[3:0], Power down all rterm.
            break;

#endif

        default:

            break;
    };
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_IsrMscReceiveFlag()
//  [Description]
//                  MHL Cbus check whether msc message is received or not
//  [Arguments]:
//
//  [Return]:
//                  TRUE: recieved
//                  FALSE: not yet
//**************************************************************************
Bool mhal_mhl_IsrMscReceiveFlag(void)
{
    Bool bindex = ((MEM_MSREAD_BYTE(REG_0B74) & BIT3) ? TRUE : FALSE); // REG_MHL_CBUS_3A[3], Receive packet valid interrupt.

    if(bindex)
    {
        MEM_MSWRITE_BYTE_MASK(REG_0B74, BIT0, BIT0); // REG_MHL_CBUS_3A[0], Clear receive packet interrupt
    }

    return bindex;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_CbusStucktoLowFlag()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mhal_mhl_IsrStucktoLowFlag(void)
{
    Bool bindex = ((MEM_MSREAD_BYTE(REG_0C30) &BIT3) ?TRUE: FALSE); // REG_PM_MHL_CBUS_18[3], Cbus stuck to low interrupt.

    if(bindex)
    {
        MEM_MSWRITE_BYTE_MASK(REG_0C30, BIT0, BIT0); // REG_PM_MHL_CBUS_18[0], Clear Cbus stuck to low interrupt
    }

    return bindex;
}

#endif

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_PortToCbusMapping()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mhal_mhl_PortToCbusMapping(BYTE ucPort)
{
    switch(ucPort)
    {
        case MHL_INPUT_PORT0:
            ucPort = MHL_CBUS_SELECT_PORT0;
            break;

        case MHL_INPUT_PORT1:
            ucPort = MHL_CBUS_SELECT_PORT1;
            break;

        case MHL_INPUT_PORT2:
            ucPort = MHL_CBUS_SELECT_PORT2;
            break;
            
        default:
            ucPort = MHL_CBUS_SELECT_PSEUDO;
            break;
    };

    return ucPort;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_CbusToPortMapping()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mhal_mhl_CbusToPortMapping(BYTE ucCbusSelect)
{
    switch(ucCbusSelect)
    {
        case MHL_CBUS_SELECT_PORT0:
            ucCbusSelect = MHL_INPUT_PORT0;
            break;

        case MHL_CBUS_SELECT_PORT1:
            ucCbusSelect = MHL_INPUT_PORT1;
            break;

        case MHL_CBUS_SELECT_PORT2:
            ucCbusSelect = MHL_INPUT_PORT2;
            break;

        default:
            ucCbusSelect = MHL_INPUT_NONE;
            break;
    };

    return ucCbusSelect;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_CheckInputPort()
//  [Description]
//                  MHL check current input port
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mhal_mhl_CheckInputPort(BYTE ucPort)
{
    Bool bindex = FALSE;

    if(mhal_mhl_PortToCbusMapping(ucPort) == GET_MHL_CBUS_SELECT_PATH())
    {
        bindex = TRUE;
    }

    return bindex;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_CbusFloating()
//  [Description]
//                  MHL cable floating
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_mhl_CbusFloating(Bool bFlag)
{
    if(bFlag)
    {
        msWriteByteMask(REG_0C2E, BIT5, BIT5); // REG_PM_MHL_CBUS_17[5], SW cbus floating
    }
    else
    {
        msWriteByteMask(REG_0C2E, 0, BIT5); // REG_PM_MHL_CBUS_17[5], SW cbus floating
    }
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_Initial()
//  [Description]
//                  MHL init
//  [Arguments]:
//                  *edid: MHL EDID data
//                  *devcap: MHL device capability
//  [Return]:
//
//**************************************************************************
void mhal_mhl_Initial(BYTE *pEdid, BYTE *pDevcap, BYTE ucVenderID)
{
    BYTE xdata ucindex = 0;
    WORD xdata uctemp = 0;

    // Initial setting
    for(uctemp = 0; uctemp <(sizeof(tMHL_INITIAL_TABLE) /sizeof(msLoadTbl_S)); uctemp++)
    {
        msWrite2ByteMask(tMHL_INITIAL_TABLE[uctemp].addr, tMHL_INITIAL_TABLE[uctemp].databuf, tMHL_INITIAL_TABLE[uctemp].mask);
    }

    // Load EDID
    MHL_HAL_DPUTSTR("** MHL load EDID...\r\n");

    msWriteByteMask(REG_0BA4, BIT1, BIT1); // REG_MHL_CBUS_52[1], CPU write enable

    for(uctemp = 0; uctemp <256; uctemp++)
    {
        msWriteByte(REG_0BA5, uctemp); // REG_MHL_CBUS_52[15:8], address
        msWriteByte(REG_0BA6, pEdid[uctemp]); // REG_MHL_CBUS_53[7:0], data
        msWriteByteMask(REG_0BA4, BIT0, BIT0); // REG_MHL_CBUS_52[0], Cpu write EDID SRAM triger.
        msWriteByteMask(REG_0BA4, 0, BIT0);
        while(msReadByte(REG_0BA4) & BIT5); // REG_MHL_CBUS_52[5], is write data busy
    }

    msWriteByteMask(REG_0BA4, 0, BIT1); // REG_MHL_CBUS_52[1], CPU write disable

    // Load vendor ID
    msWriteByte(REG_0B01, ucVenderID); // REG_MHL_CBUS_00[15:8], MHL_VENDOR_ID

    // Load MHL device capability
    MHL_HAL_DPUTSTR("** MHL load DevCap...\r\n");

    for(uctemp = 0; uctemp <8; uctemp++)
    {
        if(uctemp == 2)
        {
            ucindex = 1;
        }

        msWrite2Byte(REG_0B02 +(uctemp *2), (pDevcap[(uctemp *2) +1 -ucindex] <<8) | pDevcap[uctemp *2 -ucindex]);
    }

    msWrite2Byte(REG_0B06, (pDevcap[3] <<8) | pDevcap[4]); // REG_MHL_CBUS_03[15:0], MHL_ADOPTER_ID
    msWrite2Byte(REG_0B0E, (pDevcap[11] <<8) | pDevcap[12]); // REG_MHL_CBUS_07[15:0], MHL_DEVICE_ID
    msWriteByte(REG_0B12, pDevcap[15]); // REG_MHL_CBUS_09[7:0], reg_devcap_0f
    
    // Clear Cbus received interrupt status
    msWriteByteMask(REG_0B74, BIT4 |BIT0, BIT4 |BIT0); // REG_MHL_CBUS_3A[4][0]
    msWriteByteMask(REG_0B74, 0, BIT1); // REG_MHL_CBUS_3A[1]: receive packet valid mask

    msComboRx_Init(&uctemp, &ucMHLDriver);

    _mhal_mhl_PullDown1KSetting();

#if(MHL_CBUS_USE_INTERRUPT)
    // Enable Cbus interrupt
    _mhal_mhl_CbusInterruptMask(TRUE);
#endif

    // Clear Cbus stuck to low interrupt status
    _mhal_mhl_CbusStucktoLow(FALSE);

    // Clear Cbus wake up pulse interrupt status
    _mhal_mhl_CbusWakeupInterrupt(FALSE);

    mhal_mhl_RxRtermControl(FALSE);

    //mhal_mhl_CbusFloating(TRUE);
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_PortSwitchMux()
//  [Description]
//                  MHL port switch setting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_mhl_CbusSwitchMux(BYTE ucPort)
{
    switch(ucPort)
    {
#if(MHL_SUPPORT_PORT0)
        case MHL_CBUS_SELECT_PORT0: // Port B
            if(GET_MHL_VBUS_PORT0_FLAG())
            {
                _mhal_mhl_SetCableDetect(TRUE);
            }

            msWriteByteMask(REG_0260, BIT0, MASKBIT(1:0)); // REG_PM_GPIO_30[1:0], CBUS port selsct [1:0] : 2'b 01 -> portB
            //msWriteByteMask(REG_0262, BIT2|BIT5, BIT2|BIT3|BIT5); // REG_PM_GPIO_31[5][3][2], [3]: GPIO36, PE_CTRL_B
            msWriteByteMask(REG_0262, 0, BIT3); // REG_PM_GPIO_31[5][3][2], [3]: GPIO36, PE_CTRL_B

            MHL_HAL_DPUTSTR("** MHL Cbus switch mux to port0\n");

            break;

#endif

#if(MHL_SUPPORT_PORT1)
        case MHL_CBUS_SELECT_PORT1: // Port A
            if(GET_MHL_VBUS_PORT1_FLAG())
            {
                _mhal_mhl_SetCableDetect(TRUE);
            }

            msWriteByteMask(REG_0260, 0, MASKBIT(1:0)); // REG_PM_GPIO_30[1:0], CBUS port selsct [1:0] : 2'b 00 -> portA
            //msWriteByteMask(REG_0262, BIT3|BIT5, BIT2|BIT3|BIT5); // REG_PM_GPIO_31[5][3][2], [2]: GPIO31, PE_CTRL_A
            msWriteByteMask(REG_0262, 0, BIT2); // REG_PM_GPIO_31[5][3][2], [2]: GPIO31, PE_CTRL_A

            MHL_HAL_DPUTSTR("** MHL Cbus switch mux to port1\n");

            break;

#endif

#if(MHL_SUPPORT_PORT2)
        case MHL_CBUS_SELECT_PORT2: // Port C
            if(GET_MHL_VBUS_PORT2_FLAG())
            {
                _mhal_mhl_SetCableDetect(TRUE);
            }

            msWriteByteMask(REG_0260, BIT1, MASKBIT(1:0)); // REG_PM_GPIO_30[1:0], CBUS port selsct [1:0] : 2'b 10 -> portC
            //msWriteByteMask(REG_0262, BIT2|BIT3, BIT2|BIT3|BIT5); // REG_PM_GPIO_31[5][3][2], [5]: GPIO52, PE_CTRL_C
            msWriteByteMask(REG_0262, 0, BIT5); // REG_PM_GPIO_31[5][3][2], [5]: GPIO52, PE_CTRL_C

            MHL_HAL_DPUTSTR("** MHL Cbus switch mux to port2\n");

            break;

#endif

        default:
            msWriteByteMask(REG_0260, BIT1 |BIT0, MASKBIT(1:0)); // REG_PM_GPIO_30[1:0], CBUS port selsct [1:0] : 2'b 11 -> reserved
            //msWriteByteMask(REG_0262, BIT2 |BIT3 |BIT5, BIT2 |BIT3 |BIT5); // REG_PM_GPIO_31[5][3][2], PE_CTRL
            //msWriteByteMask(REG_0262, 0, BIT2 |BIT3 |BIT5); // REG_PM_GPIO_31[5][3][2], PE_CTRL

            MHL_HAL_DPUTSTR("** MHL Cbus switch mux to port none\n");

            break;
    };
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_CableDetect()
//  [Description]
//                  MHL cable detection
//  [Arguments]:
//
//  [Return]:
//                  TRUE: MHL cable plugged
//                  FALSE: MHL cable unplugged
//**************************************************************************
Bool mhal_mhl_CableDetect(void)
{
    Bool bindex = FALSE;
    Bool bCharge = TRUE;
    BYTE xdata uctemp = 0;

    bindex = _mhal_mhl_CheckCable(FALSE, GET_MHL_CBUS_SELECT_PATH());

    if(GET_MHL_SYSTEM_CABLE_DETECT() == bindex)
    {
        for(uctemp = 0; uctemp <MHL_CBUS_SELECT_MASK; uctemp++)
        {
            bindex = _mhal_mhl_CheckCable(bCharge, (uctemp +GET_MHL_CBUS_SELECT_PATH() +1) %MHL_CBUS_SELECT_MASK);
        }
    }
    else
    {
        SLT_MHL_SYSTEM_CABLE_DETECT(bindex);

        _mhal_mhl_SetCableDetect(bindex);
    }

    return GET_MHL_SYSTEM_CABLE_DETECT();
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_IsCbusBusy()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mhal_mhl_IsCbusBusy(void)
{
    return ((msReadByte(REG_0BBA) & 0xF0) != 0 ? TRUE : FALSE); // REG_MHL_CBUS_5D[7:4]
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_RxRtermControl()
//  [Description]
//                  MHL TMDS termination resistor control
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_mhl_RxRtermControl(RXRtermControl_T rctrl)
{
    switch(GET_MHL_CBUS_SELECT_PATH())
    {
#if(MHL_SUPPORT_PORT0)
        case MHL_CBUS_SELECT_PORT0: // Port B
            if(rctrl == RX_HDMI_RTERM)
            {
                msWriteByteMask(REG_01C4, 0, MASKBIT(3:0)); // REG_PM_BANK_62[3:0], Power on port0 rterm.
            }
            else if(rctrl == RX_MHL_RTERM)
            {
                msWriteByteMask(REG_01C4, 0, BIT1); // REG_PM_BANK_62[3:0], Power on port0's ch0 rterm.
            }
            else if(rctrl == RX_RTERM_OFF)
            {
                msWriteByteMask(REG_01C4, BIT1, BIT1); // REG_PM_BANK_62[3:0], Power down port0 rterm.
            }
            break;

#endif

#if(MHL_SUPPORT_PORT1)
        case MHL_CBUS_SELECT_PORT1: // Port A
            if(rctrl == RX_HDMI_RTERM)
            {
                msWriteByteMask(REG_01C4, 0, MASKBIT(7:4)); // REG_PM_BANK_62[7:4], Power on port1 rterm.
            }
            else if(rctrl == RX_MHL_RTERM)
            {
                msWriteByteMask(REG_01C4, 0, BIT5); // REG_PM_BANK_62[7:4], Power on port1's ch0 rterm.
            }
            else if(rctrl == RX_RTERM_OFF)
            {
                msWriteByteMask(REG_01C4, BIT5, BIT5); // REG_PM_BANK_62[7:4], Power down port1 rterm.
            }
            break;

#endif

#if(MHL_SUPPORT_PORT2)
        case MHL_CBUS_SELECT_PORT2: // Port C
            if(rctrl == RX_HDMI_RTERM)
            {
                msWriteByteMask(REG_01C5, 0, MASKBIT(3:0)); // REG_PM_BANK_62[11:8], Power on port2 rterm.
            }
            else if(rctrl == RX_MHL_RTERM)
            {
                msWriteByteMask(REG_01C5, 0, BIT1); // REG_PM_BANK_62[11:8], Power on port2's ch0 rterm.
            }
            else if(rctrl == RX_RTERM_OFF)
            {
                msWriteByteMask(REG_01C5, BIT1, BIT1); // REG_PM_BANK_62[11:8], Power down port2 rterm.
            }
            break;

#endif

        default:
            rctrl = rctrl;
            break;
    };
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_RtermControlHWMode()
//  [Description]
//                  
//  [Arguments]:
//
//  [Return]:
//                  
//**************************************************************************
void mhal_mhl_RtermControlHWMode(Bool bFlag)
{
    if(bFlag)
    {
        switch(GET_MHL_CBUS_SELECT_PATH())
        {
#if(MHL_SUPPORT_PORT0)
            case MHL_CBUS_SELECT_PORT0: // Port B
                msWriteByteMask(REG_01C9, BIT4, BIT4); // REG_PM_BANK_64[12], HW rterm control port0.
                break;
                
#endif

#if(MHL_SUPPORT_PORT1)
            case MHL_CBUS_SELECT_PORT1: // Port A
                msWriteByteMask(REG_01C9, BIT5, BIT5); // REG_PM_BANK_64[12], HW rterm control port1.
                break;
                
#endif

#if(MHL_SUPPORT_PORT2)
            case MHL_CBUS_SELECT_PORT2: // Port C
                msWriteByteMask(REG_01C9, BIT6, BIT6); // REG_PM_BANK_64[12], HW rterm control port2.
                break;

#endif

            default:
                
                break;
        };
    }
    else
    {
        msWriteByteMask(REG_01C9, 0, MASKBIT(6:4)); // REG_PM_BANK_64[14:12], HW rterm control.
    }
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_CbusStatus()
//  [Description]:
//                  MHL Cbus status
//  [Arguments]:
//
//  [Return]:
//                  Cbus status value
//**************************************************************************
BYTE mhal_mhl_CbusStatus(void)
{
    return (msReadByte(REG_0C2E)); // REG_PM_MHL_CBUS_17[1:0], link layer status
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_CbusIsMscMsgReceived()
//  [Description]
//                  MHL Cbus check whether msc message is received or not
//  [Arguments]:
//
//  [Return]:
//                  TRUE: recieved
//                  FALSE: not yet
//**************************************************************************
Bool mhal_mhl_CbusIsMscMsgReceived(void)
{
    Bool bindex = ((msReadByte(REG_0B74) & BIT3) ? TRUE : FALSE); // REG_MHL_CBUS_3A[3], Receive packet valid interrupt.

    if(bindex)
    {
        msWriteByteMask(REG_0B74, BIT0, BIT0); // REG_MHL_CBUS_3A[0], Clear receive packet interrupt
    }

    return bindex;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_CbusWakeupIntFlag()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mhal_mhl_CbusWakeupIntFlag(void)
{
    Bool bindex = ((msReadByte(REG_0C30) &BIT7) ?TRUE: FALSE); // REG_PM_MHL_CBUS_18[7], Wake up valid interrupt

    if(bindex)
    {
        //msWriteByteMask(REG_0C30, BIT4, BIT4); // REG_PM_MHL_CBUS_18[4]: Clear wake up pulse int flag

        _mhal_mhl_CbusWakeupInterrupt(FALSE);

        msWriteByteMask(REG_0BD9, BIT6, BIT6); // REG_MHL_CBUS_6C[14]: Register xdata enable
    }

    return bindex;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_CBusWrite()
//  [Description]
//                  MHL Cbus write trigger
//  [Arguments]:
//                  *pdatabuf: Cbus tx data
//  [Return]:
//
//**************************************************************************
Bool mhal_mhl_CBusWrite(mhalCbusFifo_S *pdatabuf)
{
    BYTE xdata uctemp = 0;

    if(mhal_mhl_IsCbusBusy() || mhal_mhl_CbusIsMscMsgReceived())
    {
        return FALSE;
    }

    for(uctemp = 0; uctemp <pdatabuf->lens; uctemp++)
    {
        msWrite2Byte(REG_0B4C +uctemp *2, pdatabuf->databuf[uctemp]); // REG_MHL_CBUS_26
    }

    // clear the unsed parts
    msWrite2Byte(REG_0B4C +((pdatabuf->lens) *2), 0);

    msWriteByteMask(REG_0B4B, BIT4, BIT4); // REG_MHL_CBUS_25[12], trigger to send

    return TRUE;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_CbusForceToStandby()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_mhl_CbusForceToStandby(void)
{
    msWriteByteMask(REG_0B74, BIT4, BIT4); // REG_MHL_CBUS_3A[4], clear received FIFO

    msWriteByteMask(REG_0C2F, BIT7, BIT7); // REG_PM_MHL_CBUS_17[15], Force CBUS Standby
}

//**************************************************************************
//  [Function Name]:
//                  _mhal_mhl_Cbus_SetPathEn()
//  [Description]:
//                  MHL Cbus set path enable
//  [Arguments]:
//                  TRUE: Enable
//                  FALSE: Disable
//  [Return]:
//
//**************************************************************************
void mhal_mhl_Cbus_SetPathEn(Bool bFlag)
{
    if(bFlag) // set state to PATH_EN
    {
        msWriteByteMask(REG_0C2F, BIT4, BIT4); // REG_PM_MHL_CBUS_17[12], Sw set conn_state path.
    }
    else // clear state to not PATH_EN
    {
        msWriteByteMask(REG_0C2F, BIT5, BIT5); // REG_PM_MHL_CBUS_17[13], Sw clr conn_state path.
    }
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_CbusIntCB()
//  [Description]
//                  MHL Cbus Interrupt Call Back function
//  [Arguments]:
//                  *rcstate: recevied state, 0:normal / 1:timeout
//                  *rccmd: recevied command
//                  *rcdata: recevied data
//                  *rclen: received length
//                  *bIsCmdInData: Is command in data field
//  [Return]:
//
//**************************************************************************
Bool mhal_mhl_CbusIntCB(BYTE *rcstate, BYTE *rccmd, BYTE *rcdata, BYTE *rclen, Bool *bIsCmdInData)
{
    Bool bindex = FALSE;
    BYTE xdata uctemp = 0;
    WORD xdata reg_val = msRead2Byte(REG_0B78); // REG_MHL_CBUS_3C[15:0];

    if(!(reg_val &BIT8)) // Received data
    {
        bindex = TRUE;
    }

    *rcstate = msReadByte(REG_0B76) & 0x0F; // REG_MHL_CBUS_3B[3:0], received state, 0: normal, 1: timeout

    *rccmd = reg_val & 0x00FF; // REG_MHL_CBUS_3C[7:0]
    *bIsCmdInData = FALSE;

    for(uctemp = 0; uctemp <=(MHL_CBUS_DATA_SIZE +1); uctemp++) // offset+16bytes+EOF
    {
        reg_val = msRead2Byte(REG_0B7A +(uctemp *2));

        if(reg_val & BIT15)
        {
            rcdata[uctemp] = reg_val & 0x00FF;

            if(((uctemp <=2) ||(uctemp ==(MHL_CBUS_DATA_SIZE +1))) && !(*bIsCmdInData))
            {
                *bIsCmdInData = (reg_val & BIT8) ? TRUE : FALSE;
            }
        }
        else
        {
            *rclen = uctemp;

            break;
        }
    }

    // CTS 6.3.11.19
    if(uctemp >(MHL_CBUS_DATA_SIZE +1))
    {
        *rclen = MHL_CBUS_DATA_SIZE +2;
    }

    msWriteByteMask(REG_0B74, BIT4, BIT4); // REG_MHL_CBUS_3A[4], clear received FIFO

    return bindex;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_RxSRAMSwitch()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_mhl_RxSRAMSwitch(void)
{
    if(GET_MHL_RX_SRAM_FLAG())
    {
        msWrite2ByteMask(REG_0BD6, MHL_RX_XRAM_ADDR_FIRST, MASKBIT(15:0)); // REG_MHL_CBUS_6B[15:0], Xdata_write_address.

        CLR_MHL_RX_SRAM_FLAG();
    }
    else
    {
        msWrite2ByteMask(REG_0BD6, MHL_RX_XRAM_ADDR_SECOND, MASKBIT(15:0)); // REG_MHL_CBUS_6B[15:0], Xdata_write_address.

        SET_MHL_RX_SRAM_FLAG();
    }

    msWriteByteMask(REG_0BD9, BIT7, BIT7); // REG_MHL_CBUS_6C[15], reset xdata write address
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_DriverControl()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_mhl_DriverControl(BYTE ucState)
{
    switch(ucState)
    {
        case MHL_CBUS_WAKE_UP_DETECT:
            mhal_mhl_CbusForceToStandby();

            break;

        case MHL_CBUS_AUTO_EQ_SETTING:
            _mhal_mhl_LoadMhlAutoEQTbl();
            break;

        case MHL_CBUS_SOURCE_CHANGE:
            _mhal_mhl_CbusStucktoLow(FALSE);
            mhal_mhl_CbusIsMscMsgReceived();
            mhal_mhl_RxRtermControl(RX_RTERM_OFF);
            break;

        default:

            break;
    }
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_CDRModeMonitor()
//  [Description]:
//                  MHL CDR mode monitor
//  [Arguments]:
//                  isCbusConnected: Cbus is at connected state or not
//  [Return]:
//
//**************************************************************************
Bool mhal_mhl_CDRModeMonitor(Bool bPathEnable)
{
    static BYTE xdata ucCounter = 0;
    WORD xdata usCDRClk = msRead2Byte(REG_16B6) &0x0FFF; // REG_COMBO_DECODER_5B[3], Clock counter.

    if(bPathEnable)
    {
        if(_mhal_mhl_CheckClockStatus(usCDRClk))
        {
            if(!GET_MHL_SYSTEM_PLL_POWER())
            {
                msWriteByteMask(REG_01C3, 0, BIT6); // REG_PM_BANK_61[14], power on PLL

                SET_MHL_SYSTEM_PLL_POWER();

                MHL_HAL_DPRINTF("** MHL CDR PLL Power On: 0x%x\n", usCDRClk);
            }
#if(MHL_PHASE_CHECK_PATCH)
            else
            {
                if((msReadByte(REG_155D) & BIT6) &&(msReadByte(REG_17C1) & BIT5)) // REG_COMBO_HDCP_2E[14]: DE stable, REG_COMBO_ATOP_60[13], PLL lock flag
                {
                    if(_mhal_mhl_CDRCheckPhase(TRUE))
                    {
                        if(ucMHLDriver <= MHL_DRIVER_U02)
                        {
                            msWriteByteMask(REG_1753, BIT5, BIT5); // 0x1729[13]: DC FIFO reset
                            msWriteByteMask(REG_1753, 0, BIT5); // 0x1729[13]: DC FIFO reset
                        }
                        
                        msWriteByteMask(REG_17A8, BIT7, BIT7); // REG_COMBO_ATOP_54[7], PLL PD/PFD override value
                        msWriteByteMask(REG_17BD, BIT7, BIT7); // REG_COMBO_ATOP_5E[7]
                    }
                }
            }
#endif

            //ucCounter = 3;
        }
        else
        {
            if(ucCounter > 0)
            {
                ucCounter--;
            }
            else
            {
                bPathEnable = FALSE;
            }
        }
    }

    if(!bPathEnable)
    {
        if(GET_MHL_SYSTEM_PLL_POWER())
        {
#if(MHL_PHASE_CHECK_PATCH)
            _mhal_mhl_CDRCheckPhase(FALSE);
#endif

            msWriteByteMask(REG_01C3, BIT6, BIT6); // REG_PM_BANK_61[14], power down PLL

            CLR_MHL_SYSTEM_PLL_POWER();
            ucCounter = 0;

            MHL_HAL_DPRINTF("** MHL CDR PLL Power Off: 0x%x\n", usCDRClk);

            msWriteByteMask(REG_17BD, 0, BIT7); // REG_COMBO_ATOP_5E[7]
            msWriteByteMask(REG_17A8, 0, BIT7); // REG_COMBO_ATOP_54[7], PLL PD/PFD override value

            if(GET_MHL_SYSTEM_PHY_SETTING())
            {
                CLR_MHL_SYSTEM_PHY_SETTING();
            }
        }
    }

    return GET_MHL_SYSTEM_PLL_POWER();
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_AutoEQProcess()
//  [Description]
//                  MHL Auto EQ
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_mhl_AutoEQProcess(Bool bEQEnable)
{
    static BYTE xdata ucCount = 1;

#if(MHL_CBUS_AUTO_EQ)
    if(ucCount == MHL_AUTO_EQ_TRIGGER_INTERVAL)
    {
        mhal_mhl_DriverControl(MHL_CBUS_AUTO_EQ_SETTING);

        msWriteByteMask(REG_175A, 0, BIT4); // REG_COMBO_ATOP_2D[4], IRQ_CLR
        msWriteByteMask(REG_164A, BIT2, BIT2); // REG_COMBO_DECODER_25[2], Enable auto_eq_engine.
    }
    else if(ucCount == (MHL_AUTO_EQ_RESULT_INTERVAL -1))
    {
        BYTE uctemp = msReadByte(REG_1616); // REG_COMBO_DECODER_0B[5:1] = CH0 auto EQ result bits [4:0]

        MHL_HAL_DPRINTF("** MHL Auto EQ result: %x\n", uctemp);
    }

#endif

    if(!bEQEnable)
    {
        ucCount = 0;
    }
    else if(ucCount < MHL_AUTO_EQ_RESULT_INTERVAL)
    {
        ucCount++;
    }
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_PowerControl()
//  [Description]
//                  MHL power control
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_mhl_PowerControl(BYTE ucState)
{
    BYTE xdata uctemp = 0;

    switch(ucState)
    {
        case MHL_POWER_ON:
            for(uctemp = 0; uctemp <(sizeof(tMHL_POWER_ON_TABLE) /sizeof(msLoadTbl_S)); uctemp++)
            {
                msWrite2ByteMask(tMHL_POWER_ON_TABLE[uctemp].addr, tMHL_POWER_ON_TABLE[uctemp].databuf, tMHL_POWER_ON_TABLE[uctemp].mask);
            }
            msWriteByte(REG_0C2F, BIT6); // exit Cbus PM state

#if(MHL_CBUS_USE_INTERRUPT)
            // Enable Cbus interrupt
            _mhal_mhl_CbusInterruptMask(TRUE);
#endif
            break;
            
        case MHL_POWER_STANDBY:
            for(uctemp = 0; uctemp <(sizeof(tMHL_POWER_SAVING_TABLE) /sizeof(msLoadTbl_S)); uctemp++)
            {
                msWrite2ByteMask(tMHL_POWER_SAVING_TABLE[uctemp].addr, tMHL_POWER_SAVING_TABLE[uctemp].databuf, tMHL_POWER_SAVING_TABLE[uctemp].mask);
            }

#if(MHL_CBUS_USE_INTERRUPT)
            // Disable Cbus interrupt
            _mhal_mhl_CbusInterruptMask(FALSE);
#endif
            break;
            
        case MHL_POWER_DOWN:
            for(uctemp = 0; uctemp <(sizeof(tMHL_POWER_DOWN_TABLE) /sizeof(msLoadTbl_S)); uctemp++)
            {
                msWrite2ByteMask(tMHL_POWER_DOWN_TABLE[uctemp].addr, tMHL_POWER_DOWN_TABLE[uctemp].databuf, tMHL_POWER_DOWN_TABLE[uctemp].mask);
            }

            _mhal_mhl_CbusStucktoLow(FALSE);
            mhal_mhl_CableDetect();

#if(MHL_CBUS_USE_INTERRUPT)
            // Disable Cbus interrupt
            _mhal_mhl_CbusInterruptMask(FALSE);
#endif
            break;

        default:

            break;
    }
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_SetHPD()
//  [Description]
//                  config HPD in combo(MHL/HDMI) port A
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_mhl_SetHPD(Bool bFlag)
{
    if(bFlag) // HPD is high
    {
        msWriteByteMask(REG_0C40, BIT3, BIT4|BIT3); // REG_PM_MHL_CBUS_20[4:3], [4]: output val, [3]: oen
        //MHL_DPUTSTR("**MHL_HPD is High");
    }
    else // hpd is low
    {
        msWriteByteMask(REG_0C40, 0, BIT4|BIT3); // REG_PM_MHL_CBUS_20[4:3], [4]: output val, [3]: oen
        //MHL_DPUTSTR("**MHL_HPD is Low");
    }
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_GetDDCErrorCode()
//  [Description]
//                  Get DDC error code
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mhal_mhl_GetDDCErrorCode(void)
{
    return msReadByte(REG_0B43); // REG_MHL_CBUS_21[15:8], DDC error occurs and statue.
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_CbusPathLinkProc()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_mhl_CbusPathLinkProc(BYTE ucPort)
{
    Bool bFlag = FALSE;

    if(mhal_mhl_CbusStatus() != 0x03) // Cbus disconnect
    {
        bFlag = TRUE;
    }
    
    if(mhal_mhl_CheckInputPort(ucPort))
    {
        _mhal_mhl_LoadMhl24bitsModeTbl();

#if(DMHL_LG_PRADA_PATCH)
        _mhal_mhl_AdjustCommonModeResistor(bFlag, GET_MHL_CBUS_SELECT_PATH());
#endif
    }

    if(bFlag)
    {
        mhal_mhl_RxRtermControl(RX_RTERM_OFF);
    }
    
    _mhal_mhl_CbusStucktoLow(!bFlag);
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_CbusPathRemoveProc()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_mhl_CbusPathRemoveProc(BYTE ucPort)
{
    if(mhal_mhl_CheckInputPort(ucPort))
    {
        _mhal_mhl_LoadHdmiBypassTbl();

#if(DMHL_LG_PRADA_PATCH)
        _mhal_mhl_AdjustCommonModeResistor(FALSE, GET_MHL_CBUS_SELECT_PATH());
#endif
    }

    mhal_mhl_RxRtermControl(RX_HDMI_RTERM);
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_CbusConnectProc()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_mhl_CbusConnectProc(BYTE ucPort)
{
    if(mhal_mhl_CheckInputPort(ucPort))
    {
#if(DMHL_LG_PRADA_PATCH)
        _mhal_mhl_AdjustCommonModeResistor(FALSE, GET_MHL_CBUS_SELECT_PATH());
#endif
    }

    mhal_mhl_RxRtermControl(RX_MHL_RTERM);

    _mhal_mhl_CbusStucktoLow(TRUE);
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_CbusStucktoLowProc()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_mhl_CbusStucktoLowProc(BYTE ucPort)
{
    if(mhal_mhl_CheckInputPort(ucPort))
    {
#if(DMHL_LG_PRADA_PATCH)
        _mhal_mhl_AdjustCommonModeResistor(TRUE, GET_MHL_CBUS_SELECT_PATH());
#endif
    }

    mhal_mhl_RxRtermControl(RX_RTERM_OFF);
    _mhal_mhl_CbusStucktoLow(FALSE);
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_ClockModeSwitchProc()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_mhl_ClockModeSwitchProc(BYTE ucPort, Bool bPPmode)
{
    if(mhal_mhl_CheckInputPort(ucPort))
    {
        if(bPPmode)
        {
            _mhal_mhl_LoadMhlPackedPixelModeTbl();
        }
        else
        {
            _mhal_mhl_LoadMhl24bitsModeTbl();
        }

        mhal_mhl_CDRModeMonitor(FALSE);
    }
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_PortSwitchProc()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_mhl_PortSwitchProc(BYTE ucPort)
{
    if(mhal_mhl_CheckInputPort(ucPort))
    {
        _mhal_mhl_LoadMhl24bitsModeTbl();

        if(GET_MHL_INPUT_CLOCK_MODE() == CBUS_CLOCK_PACKET_PIXEL)
        {
            mhal_mhl_ClockModeSwitchProc(ucPort, TRUE);
        }

        if(mhal_mhl_CbusStatus() == 0x03) // Cbus connection
        {
#if(DMHL_LG_PRADA_PATCH)
        _mhal_mhl_AdjustCommonModeResistor(FALSE, GET_MHL_CBUS_SELECT_PATH());
#endif        
        }
    }
    else if(ucPort != MHL_INPUT_NONE)
    {
        _mhal_mhl_LoadHdmiBypassTbl();
    }
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_CableDetectControl()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_mhl_CableDetectControl(Bool bEnable, BYTE ucPort)
{
    if(!bEnable)
    {
        switch(ucPort)
        {
#if(MHL_SUPPORT_PORT0)
            case MHL_CBUS_SELECT_PORT0: // Port B
                CLR_MHL_CABLE_DETECT_ENABLE0();
                break;

#endif

#if(MHL_SUPPORT_PORT1)
            case MHL_CBUS_SELECT_PORT1: // Port A
                CLR_MHL_CABLE_DETECT_ENABLE1();
                break;

#endif

#if(MHL_SUPPORT_PORT2)
            case MHL_CBUS_SELECT_PORT2: // Port C
                CLR_MHL_CABLE_DETECT_ENABLE2();
                break;

#endif

            default:

                break;
        };
    }
    else
    {
        SET_MHL_CABLE_DETECT_ENABLE0();
        SET_MHL_CABLE_DETECT_ENABLE1();
        SET_MHL_CABLE_DETECT_ENABLE2();
    }
}

#if MS_PM
//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_CableDetectPM()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mhal_mhl_CableDetectPM(void)
{
    Bool bindex = FALSE;
    Bool bCharge = TRUE;
    BYTE xdata uctemp = 0;

    bindex = _mhal_mhl_CheckCable(FALSE, GET_MHL_CBUS_SELECT_PATH());

    if(GET_MHL_SYSTEM_CABLE_DETECT() == bindex)
    {
        for(uctemp = 0; uctemp <MHL_CBUS_SELECT_MASK; uctemp++)
        {
            bCharge = GET_MHL_CABLE_DETECT_PORT(uctemp);
            
            bindex = _mhal_mhl_CheckCable(TRUE, uctemp);

            if(bCharge != bindex)
            {
#if DMHL_LG_PRADA_PATCH
                _mhal_mhl_AdjustCommonModeResistor(bindex, uctemp);
#endif
            }
        }
    }
    else
    {
        SLT_MHL_SYSTEM_CABLE_DETECT(bindex);

        _mhal_mhl_SetCableDetect(bindex);

#if DMHL_LG_PRADA_PATCH
        _mhal_mhl_AdjustCommonModeResistor(bindex, GET_MHL_CBUS_SELECT_PATH());
#endif
    }

    return GET_MHL_SYSTEM_CABLE_DETECT();
}

#endif

/*
#else
BYTE code msMhalMHLNullData[] = {0};

void msMhalMHLDummy(void)
{
    BYTE xdata i = msMhalMHLNullData[0];
}
*/
#endif //#if ENABLE_MHL

#endif // _MHAL_MHL_C_

