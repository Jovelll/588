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

    {REG_1740, BIT0, BIT0},//REG_combo_atop_20[0], Main link function select. 0:DP 1:HDMI/MHL
};

//-------------------------------------------------------------------------------------------------
//  MHL HDMI bypass table
//-------------------------------------------------------------------------------------------------
code msLoadTbl_S tMHL_HDMI_BYPASS_TABLE[] = 
{
    {REG_1BDF, 0, BIT6}, // REG_COMBO_PHY0_6F[14], Override MHL icontrol
    {REG_17B0, (MHL_HDMI_B_CHANNEL_EQ <<8), MASKBIT(13:8)}, // REG_COMBO_ATOP_58[15:0], [13:0]: Ch1 EQ, [5:0]:
    {REG_1604, 0, MASKBIT(7:6)|BIT3|BIT1}, //REG_combo_atop_02[7:6][3][1], [7]:seq chk fail enalbe for mhl decode. [6]:dynamaic sequential check for mhl check. [3]:swap for mhl v12. [1]:mhl packed pixel mode enable for SW mode
    {REG_153C, 0, BIT2}, // REG_HDCP_1E[2]: PP mode + HDCP enable

    {REG_1BDC, 0, BIT6}, // REG_COMBO_PHY0_60[6], atop_gc_eq_l1_ove disable
    
    #if ENABLE_XTAL_LESS
    {REG_1B76, 0x1C80, MASKBIT(12:0)},// FRO set 12MHz, Upper bound of valid clk freq
    {REG_1B74, 0x90A, MASKBIT(11:0)},// FRO set 12MHz, Lower Bound of valid clk freq
    #endif
};

//-------------------------------------------------------------------------------------------------
//  MHL 24 bit mode table
//-------------------------------------------------------------------------------------------------
code msLoadTbl_S tMHL_24BIT_MODE_TABLE[] = 
{
    //{REG_17AA, (MHL_ICONTROL_VALUE <<4), MASKBIT(6:4)}, // REG_COMBO_ATOP_55[7:4]: Set Icontrol override value
    //{REG_17AC, BIT0, BIT0}, // REG_COMBO_ATOP_56[0], Override MHL icontrol
    //{REG_1BDF, 0, BIT6}, // REG_COMBO_PHY0_6F[14], Override MHL icontrol
    //{REG_17B0, (MHL_B_CHANNEL_EQ <<8), MASKBIT(13:8)}, // REG_COMBO_ATOP_58[15:0], [13:0]: Ch1 EQ, [5:0]: Ch0 EQ(Clock)

    {REG_1604, MASKBIT(7:6), MASKBIT(7:6)|BIT3|BIT1}, //REG_combo_atop_02[7:6][3][1], [7]:seq chk fail enalbe for mhl decode. [6]:dynamaic sequential check for mhl check. [3]:swap for mhl v12. [1]:mhl packed pixel mode enable for SW mode
    {REG_153C, 0, BIT2}, // REG_HDCP_1E[2]: PP mode + HDCP disable

    {REG_1BD9, 0x0E, MASKBIT(5:0)}, // REG_COMBO_PHY0_6C[12:8]: Set Icontrol override value
    {REG_1BDF, BIT6, BIT6}, // REG_COMBO_PHY0_6F[14], Override MHL icontrol
    {REG_1BDC, 0, BIT6}, // REG_COMBO_PHY0_60[6], atop_gc_eq_l1_ove 
    #if ENABLE_XTAL_LESS
    {REG_1B44, 0x02A3, MASKBIT(15:0)},//FRO set 12MHz, mhl24_clk_thr2
    {REG_1B9E, 0x0215, MASKBIT(15:0)},//FRO set 12MHz, mhl24_clk_thr1
    {REG_1B76, 0x042A, MASKBIT(12:0)},// FRO set 12MHz, Upper bound of valid clk freq
    {REG_1B74, 0x58, MASKBIT(11:0)},// FRO set 12MHz, Lower Bound of valid clk freq
    #endif
};

//-------------------------------------------------------------------------------------------------
//  MHL packet pixel mode table
//-------------------------------------------------------------------------------------------------
code msLoadTbl_S tMHL_PACKET_PIXEL_MODE_TABLE[] = 
{
    {REG_1604, MASKBIT(7:6)|BIT3|BIT1, MASKBIT(7:6)|BIT3|BIT1}, //REG_combo_atop_02[7:6][3][1], [7]:seq chk fail enalbe for mhl decode. [6]:dynamaic sequential check for mhl check. [3]:swap for mhl v12. [1]:mhl packed pixel mode enable for SW mode
    {REG_153C, BIT2, BIT2}, // REG_HDCP_1E[2]: PP mode + HDCP enable
    {REG_1BD9, 0x09, MASKBIT(5:0)}, // REG_COMBO_PHY0_6C[12:8]: Set Icontrol override value

    {REG_1BDC, BIT6, BIT6}, // REG_COMBO_PHY0_60[6], atop_gc_eq_l1_ove 
    {REG_1BC0, 0x0A, MASKBIT(4:0)}, // REG_COMBO_PHY0_60[4:0]:EQ value setting of Lane 1

     #if ENABLE_XTAL_LESS
    {REG_1B9A, 0x01FA, MASKBIT(15:0)},//FRO set 12MHz, mhlpp_clk_thr2
    {REG_1B9C, 0x0190, MASKBIT(15:0)},//FRO set 12MHz, mhlpp_clk_thr1
    {REG_1B76, 0x0320, MASKBIT(12:0)},// FRO set 12MHz, Upper bound of valid clk freq
    {REG_1B74, 0x42, MASKBIT(11:0)},// FRO set 12MHz, Lower Bound of valid clk freq
    #endif

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
	
    msEread_SetHDMIInitialValue();
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

    msEread_SetMHLInitialValue();
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
                msWrite2ByteMask(REG_0906, BIT8, BIT8); // REG_combo_phy_pm_03[8], mhl software enable bit
                msWrite2ByteMask(REG_0906, BIT0, BIT0); // REG_combo_phy_pm_03[0], mhl overwrite bit
            }
            else
            {
                msWrite2ByteMask(REG_0906, 0, BIT8); // REG_combo_phy_pm_03[8], mhl software enable bit
                msWrite2ByteMask(REG_0906, 0, BIT0); // REG_combo_phy_pm_03[0], mhl overwrite bit
            }
            break;
            
#endif

#if(MHL_SUPPORT_PORT1)
        case MHL_CBUS_SELECT_PORT1: // Port A
            if(bFlag)
            {

            }
            else
            {

            }
            break;
            
#endif

#if(MHL_SUPPORT_PORT2)
        case MHL_CBUS_SELECT_PORT2: // Port C
            if(bFlag)
            {

            }
            else
            {

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
        msWriteByteMask(REG_0C40, 0, BIT5); // REG_PM_MHL_CBUS_20[5], cbus hdmi pu1k
    }
    else
    {
        msWriteByteMask(REG_0C01, BIT1, MASKBIT(1:0)); // REG_PM_MHL_CBUS_00[9:8], SW control cable detect
        msWriteByteMask(REG_0C40, BIT5, BIT5); // REG_PM_MHL_CBUS_20[5], cbus hdmi pu1k
    }
}

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
    BYTE xdata uctemp = 0;//msEread_GetDataFromEfuse(0x0002); alan.lee

    if((uctemp &BIT7) == BIT7)  // Trimming flag
    {
        if((uctemp &BIT1) == BIT1)  // PD 1K low
        {
            msWriteByteMask(REG_0266, BIT4, BIT4); //pad_top_33[4], Enable GPIO31 pd1k_low
            msWriteByteMask(REG_0266, BIT5, BIT5); //pad_top_33[4], Enable GPIO36 pd1k_low
        }

        if((uctemp &BIT0) == BIT0)  // PD 1K high
        {
            msWriteByteMask(REG_0266, BIT0, BIT0); //pad_top_33[0], Enable GPIO31 pd1k_high
            msWriteByteMask(REG_0266, BIT1, BIT1); //pad_top_33[1], Enable GPIO36 pd1k_high
        }
    }
}

//**************************************************************************
//  [Function Name]:
//                  _mhal_mhl_PullDown100KSetting()
//  [Description]
//                  
//  [Arguments]:
//                  
//  [Return]:
//
//**************************************************************************
void _mhal_mhl_PullDown100KSetting(void)
{
    BYTE xdata uctemp = 0;//msEread_GetDataFromEfuse(0x0002); 

    if((uctemp &BIT7) == BIT7)  // Trimming flag
    {
        if((uctemp &BIT3) == BIT3)  // PD 100K low
        {
            msWriteByteMask(REG_0267, BIT4, BIT4); //pad_top_33[12], Enable GPIO31 pd100k_low
            msWriteByteMask(REG_0267, BIT5, BIT5); //pad_top_33[13], Enable GPIO36 pd100k_low
        }

        if((uctemp &BIT2) == BIT2)  // PD 100K high
        {
            msWriteByteMask(REG_0267, BIT0, BIT0); //pad_top_33[8], Enable GPIO31 pd100k_high
            msWriteByteMask(REG_0267, BIT1, BIT1); //pad_top_33[9], Enable GPIO36 pd100k_high
        }
    }
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_SetPullDownResistor()
//  [Description]
//                  
//  [Arguments]:
//                  
//  [Return]:
//                  
//**************************************************************************
void _mhal_mhl_SetPullDownResistor(void)
{
     _mhal_mhl_PullDown1KSetting();
     _mhal_mhl_PullDown100KSetting();
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
                MEM_MSWRITE_BYTE_MASK(REG_0940, BIT1, BIT1); // REG_combo_phy_pm_20[3:1], Power Down R-term of each Lane
            break;
            
#endif

#if(MHL_SUPPORT_PORT1)
        case MHL_CBUS_SELECT_PORT1: // Port A

            break;
            
#endif

#if(MHL_SUPPORT_PORT2)
        case MHL_CBUS_SELECT_PORT2: // Port C

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

    msWriteByteMask(REG_0BA4, BIT1, BIT1); // REG_MHL_CBUS_52[1], cpu write EDID SRAM enable
    
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
    msWriteByteMask(REG_0B74, BIT4 |BIT0, BIT4 |BIT0); // REG_MHL_CBUS_3A[4]:set clr, HW will clear received data. [0]:after SW read the receive packets, set clr
    msWriteByteMask(REG_0B74, 0, BIT1); // REG_MHL_CBUS_3A[1]: receive packet valid mask

    msComboRx_Init(&uctemp, &ucMHLDriver);
    _mhal_mhl_SetPullDownResistor();

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
            
            msWriteByteMask(REG_0260, 0, MASKBIT(1:0)); // REG_PM_GPIO_30[1:0], CBUS port selsct [1:0] : 2'b 01 -> portB
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
            
            msWriteByteMask(REG_0260, BIT0, MASKBIT(1:0)); // REG_PM_GPIO_30[1:0], CBUS port selsct [1:0] : 2'b 00 -> portA
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
                msWriteByteMask(REG_0940, 0, MASKBIT(3:1)); // REG_combo_phy_pm_20[3:1], Power Down R-term of each Lane
                //msWriteByteMask(REG_0924, 0, BIT7); // REG_combo_phy_pm_12[7], mhl_atop p0 power down clock R-term
            }
            else if(rctrl == RX_MHL_RTERM)
            {
                msWriteByteMask(REG_0940, 0, BIT1); // REG_combo_phy_pm_20[3:1], Power Down R-term of each Lane
                //msWriteByteMask(REG_0924, 0, BIT7); // REG_combo_phy_pm_12[7], mhl_atop p0 power down clock R-term
            }
            else if(rctrl == RX_RTERM_OFF)
            {
                msWriteByteMask(REG_0940, MASKBIT(3:1), MASKBIT(3:1)); // REG_combo_phy_pm_20[3:1], Power Down R-term of each Lane
                //msWriteByteMask(REG_0924, 0, BIT7); // REG_combo_phy_pm_12[7], mhl_atop p0 power down clock R-term
            }
            break;
            
#endif

#if(MHL_SUPPORT_PORT1)
        case MHL_CBUS_SELECT_PORT1: // Port A
            if(rctrl == RX_HDMI_RTERM)
            {

            }
            else if(rctrl == RX_MHL_RTERM)
            {

            }
            else if(rctrl == RX_RTERM_OFF)
            {

            }
            break;
            
#endif

#if(MHL_SUPPORT_PORT2)
        case MHL_CBUS_SELECT_PORT2: // Port C
            if(rctrl == RX_HDMI_RTERM)
            {

            }
            else if(rctrl == RX_MHL_RTERM)
            {

            }
            else if(rctrl == RX_RTERM_OFF)
            {

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
                msWriteByteMask(REG_0907, BIT4, BIT4); // REG_combo_phy_pm_03[12], HW rterm control port0.
                break;
                
#endif

#if(MHL_SUPPORT_PORT1)
            case MHL_CBUS_SELECT_PORT1: // Port A
                msWriteByteMask(REG_0907, BIT5, BIT5); // REG_combo_phy_pm_03[12], HW rterm control port1.
                break;
                
#endif

#if(MHL_SUPPORT_PORT2)
            case MHL_CBUS_SELECT_PORT2: // Port C
                msWriteByteMask(REG_0907, BIT6, BIT6); // REG_combo_phy_pm_03[12], HW rterm control port2.
                break;

#endif

            default:
                
                break;
        };
    }
    else
    {
        msWriteByteMask(REG_0907, 0, MASKBIT(6:4)); // REG_PM_BANK_64[14:12], HW rterm control.
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
                //msWriteByteMask(REG_01C3, 0, BIT6); // REG_PM_BANK_61[14], power on PLL

                SET_MHL_SYSTEM_PLL_POWER();

                //MHL_HAL_DPRINTF("** MHL CDR PLL Power On: 0x%x\n", usCDRClk);
            }
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
            //msWriteByteMask(REG_01C3, BIT6, BIT6); // REG_PM_BANK_61[14], power down PLL

            CLR_MHL_SYSTEM_PLL_POWER();
            ucCounter = 0;

            MHL_HAL_DPRINTF("** MHL CDR PLL Power Off: 0x%x\n", usCDRClk);

            //msWriteByteMask(REG_17BD, 0, BIT7); // REG_COMBO_ATOP_5E[7]
            //msWriteByteMask(REG_17A8, 0, BIT7); // REG_COMBO_ATOP_54[7], PLL PD/PFD override value

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

