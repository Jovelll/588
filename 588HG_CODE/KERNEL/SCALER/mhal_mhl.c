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
#include "board.h"
#include "types.h"
#include "ms_reg.h"
//#include "DebugMsg.h"
#include "global.h"
#include "misc.h"
#include "mapi_mhl.h"
#include "mdrv_mhl.h"
#include "mhal_mhl.h"

#if ENABLE_MHL

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define MHL_DBG    1

#if MHL_DBG && ENABLE_DEBUG
#define MHL_DPUTSTR(str)        printMsg(str)
#define MHL_DPRINTF(str, x)     printData(str, x)
#else
#define MHL_DPUTSTR(str)
#define MHL_DPRINTF(str, x)
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

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------

typedef struct
{
    WORD addr;
    WORD mask;
    WORD databuf;
} msLoadTbl_S;

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
Bool bMHLFirstPowerOn = FALSE;
BYTE xdata ucMHLDriver = MHL_DRIVER_VERSION;

//-------------------------------------------------------------------------------------------------
//  MHL initial table
//-------------------------------------------------------------------------------------------------
code msLoadTbl_S tMHL_INITIAL_TABLE[] = 
{
    // Set accepted discover pulse high pulse width to ignore USB pulse
    {REG_PM_MHL_CBUS_0C, MASKBIT(15:0), 0x0046}, // reject cbus discovery pulse below this limit
    {REG_PM_MHL_CBUS_0D, MASKBIT(15:0), 0x0082}, // reject cbus discovery pulse above this limit
    // CTS 4.3.7.1
    {REG_MHL_CBUS_55, MASKBIT(7:4), BIT4}, // [7:4]: cbus requester transmit opportunity after arbitration
    {REG_MHL_CBUS_70, BIT14, BIT14}, // [14]: let edid current read initial address add 1

#if(DMHL_CBUS_HW_MODE)
    {REG_MHL_CBUS_23, MASKBIT(10:8) |BIT4, BIT10 |BIT4}, // [10]: enable HW auto response read_devcap request
    {REG_MHL_CBUS_6D, MASKBIT(11:0), MASKBIT(11:0)}, // Enable MHL HW mode
    {REG_MHL_CBUS_71, MASKBIT(15:14), MASKBIT(15:14)},
    {REG_MHL_CBUS_50, BIT13, BIT13}, // [13]: MSC send command keep mode
    {REG_MHL_CBUS_6C, BIT14, BIT14}, // [14]: Register xdata enable
#endif

    {REG_PM_GPIO_32, MASKBIT(13:11), 0},
    
    // Set Pixel mode temp
    {REG_COMBO_ATOP_56, BIT1, BIT1},
    {REG_COMBO_ATOP_53, MASKBIT(7:6), BIT7},
};

//-------------------------------------------------------------------------------------------------
//  MHL HDMI bypass table
//-------------------------------------------------------------------------------------------------
code msLoadTbl_S tMHL_HDMI_BYPASS_TABLE[] = 
{
#if(MHL_ADDRESS_TYPE == MHL_ADDERSS_8BIT)
    {REG_01C2, MASKBIT(15:14), 0}, // Power On PLL
    //{REG_01C4, MASKBIT(15:0), 0x0000},
    {REG_01C8, MASKBIT(15:0), BIT1 |BIT0},
    {REG_01CC, BIT7, 0}, // [7]: mhl sw mode pm
    {REG_1606, MASKBIT(15:0), 0x0AD4}, // threshold for vco div = 2, [11] : enable
    {REG_1608, MASKBIT(15:0), 0x0964}, // threshold for vco div = 4, [11] : enable
    {REG_160A, MASKBIT(15:0), 0x08B5}, // threshold for vco div = 8, [11] : enable
    {REG_17AA, MASKBIT(7:4), 0}, // [7:4]: Set Icontrol override value
    {REG_17AC, 0, BIT0}, // Not override MHL icontrol
    {REG_17BC, MASKBIT(15:0), 0x8000},

#else   // 16 bit Address
    {REG_PM_BANK_61, MASKBIT(15:14), 0}, // Power On PLL
    //{REG_PM_BANK_62, MASKBIT(15:0), 0x0000},
    {REG_PM_BANK_64, MASKBIT(15:0), BIT1 |BIT0},
    {REG_PM_BANK_66, BIT7, 0}, // [7]: mhl sw mode pm
    {REG_COMBO_DECODER_03, MASKBIT(15:0), 0x0AD4}, // threshold for vco div = 2, [11] : enable
    {REG_COMBO_DECODER_04, MASKBIT(15:0), 0x0964}, // threshold for vco div = 4, [11] : enable
    {REG_COMBO_DECODER_05, MASKBIT(15:0), 0x08B5}, // threshold for vco div = 8, [11] : enable
    {REG_COMBO_ATOP_55, MASKBIT(7:4), 0}, // [7:4]: Set Icontrol override value
    {REG_COMBO_ATOP_56, 0, BIT0}, // Not override MHL icontrol
    {REG_COMBO_ATOP_5E, MASKBIT(15:0), 0x8000},
    
#endif
};

//-------------------------------------------------------------------------------------------------
//  MHL 24 bit mode table
//-------------------------------------------------------------------------------------------------
code msLoadTbl_S tMHL_24BIT_MODE_TABLE[] = 
{
#if(MHL_ADDRESS_TYPE == MHL_ADDERSS_8BIT)
    {REG_01C8, MASKBIT(15:0), BIT10 |BIT5 |BIT1 |BIT0},
    {REG_01C8, MASKBIT(14:12), 0},   // HW rterm control
    {REG_01CC, BIT7, BIT7}, // [7]: mhl sw mode pm
    {REG_16C4, MASKBIT(15:0), 0x08F1}, // threshold for mhl vco div = 2, [11] : enable
    {REG_16C6, MASKBIT(15:0), 0x0879}, // threshold for mhl vco div = 4, [11] : enable
    {REG_17AA, MASKBIT(7:4), BIT5}, // [7:4]: Set Icontrol override value
    {REG_17AC, BIT0, BIT0}, // Override MHL icontrol
    {REG_17BC, MASKBIT(15:0), 0x800C},
    
#else   // 16 bit Address
    //{REG_PM_BANK_63, MASKBIT(15:0), 0x1111},
    {REG_PM_BANK_64, MASKBIT(15:0), BIT10 |BIT5 |BIT1 |BIT0},
    //{REG_PM_BANK_64, MASKBIT(14:12), MASKBIT(14:12)},   // HW rterm control
    {REG_PM_BANK_64, MASKBIT(14:12), 0},   // HW rterm control
    //{REG_PM_BANK_65, BIT12, BIT12}, // [12]: enable dcdr lbw
    {REG_PM_BANK_66, BIT7, BIT7}, // [7]: mhl sw mode pm
    //{REG_COMBO_LINK_15, MASKBIT(15:0), 0x0015},
    //{REG_COMBO_LINK_1C, MASKBIT(15:0), 0xC010},
    //{REG_COMBO_HDCP_21, MASKBIT(15:0), 0x100F},
    //{REG_COMBO_DECODER_00, MASKBIT(15:0), 0x000F},
    //{REG_COMBO_DECODER_01, MASKBIT(15:0), 0x024A},
    //{REG_COMBO_DECODER_02, MASKBIT(7:6), MASKBIT(7:6)},
    //{REG_COMBO_DECODER_23, MASKBIT(15:0), 0x081F},
    {REG_COMBO_DECODER_62, MASKBIT(15:0), 0x08F1}, // threshold for mhl vco div = 2, [11] : enable
    {REG_COMBO_DECODER_63, MASKBIT(15:0), 0x0879}, // threshold for mhl vco div = 4, [11] : enable
    //{REG_COMBO_ATOP_20, BIT0, BIT0}, // [0]: Main link function select
    //{REG_COMBO_ATOP_26, MASKBIT(15:0), MASKBIT(15:0)},
    //{REG_COMBO_ATOP_27, MASKBIT(15:0), MASKBIT(13:0)},
    //{REG_COMBO_ATOP_51, MASKBIT(15:0), 0x002E},
    //{REG_COMBO_ATOP_57, BIT7, BIT7}, // [7]: tmds 2x clk enable
    {REG_COMBO_ATOP_55, MASKBIT(7:4), BIT5}, // [7:4]: Set Icontrol override value
    {REG_COMBO_ATOP_56, BIT0, BIT0}, // Override MHL icontrol
    {REG_COMBO_ATOP_5E, MASKBIT(15:0), 0x800C},
    //{REG_COMBO_ATOP_5F, MASKBIT(15:0), 0x0000},
    
    {REG_COMBO_DECODER_0E, MASKBIT(15:8), 0xFF00},
    
#endif
};

//-------------------------------------------------------------------------------------------------
//  MHL packet pixel mode table
//-------------------------------------------------------------------------------------------------
code msLoadTbl_S tMHL_PACKET_PIXEL_MODE_TABLE[] = 
{
    {REG_PM_MHL_CBUS_16, BIT13|BIT9, BIT13|BIT9},
};

//-------------------------------------------------------------------------------------------------
//  MHL Auto EQ table
//-------------------------------------------------------------------------------------------------
code msLoadTbl_S tMHL_AUTO_EQ_TABLE[] = 
{
    {REG_COMBO_DECODER_25, MASKBIT(15:0), 0x0AF9},
    {REG_COMBO_DECODER_26, MASKBIT(14:8) |MASKBIT(6:0), BIT11 |BIT9 |BIT3 |BIT1},
    {REG_COMBO_DECODER_27, MASKBIT(14:8) |MASKBIT(6:0), BIT9 |BIT8 |BIT1 |BIT0},
    {REG_COMBO_DECODER_28, MASKBIT(10:0), BIT10 |BIT8 |BIT7 |BIT1 |BIT0},
    {REG_COMBO_DECODER_29, MASKBIT(15:0), 0x0003},
    {REG_COMBO_DECODER_2A, MASKBIT(15:0), 0x0400},
    {REG_COMBO_DECODER_2B, MASKBIT(15:0), 0x0040},
    {REG_COMBO_DECODER_2C, MASKBIT(15:0), 0x0002},
    {REG_COMBO_DECODER_2D, MASKBIT(15:0), 0x0028},
    // Not overwrite EQ
    {REG_COMBO_ATOP_56, MASKBIT(7:5), 0},
};

//-------------------------------------------------------------------------------------------------
//  MHL power on table
//-------------------------------------------------------------------------------------------------
code msLoadTbl_S tMHL_POWER_ON_TABLE[] = 
{
    {REG_PM_MHL_CBUS_16, BIT13|BIT9, BIT13|BIT9},
};

//-------------------------------------------------------------------------------------------------
//  MHL power down table
//-------------------------------------------------------------------------------------------------
code msLoadTbl_S tMHL_POWER_DOWN_TABLE[] = 
{
    {REG_PM_MHL_CBUS_16, BIT13|BIT9, BIT13|BIT9},
};

//-------------------------------------------------------------------------------------------------
//  MHL power saving table
//-------------------------------------------------------------------------------------------------
code msLoadTbl_S tMHL_POWER_SAVING_TABLE[] = 
{
    {REG_PM_MHL_CBUS_16, BIT13|BIT9, BIT13|BIT9},
};

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
    BYTE uctemp = 0;
	
    for(uctemp = 0; uctemp <(sizeof(tMHL_HDMI_BYPASS_TABLE) /sizeof(msLoadTbl_S)); uctemp++)
    {
        msWrite2ByteMask(tMHL_HDMI_BYPASS_TABLE[uctemp].addr, tMHL_HDMI_BYPASS_TABLE[uctemp].databuf, tMHL_HDMI_BYPASS_TABLE[uctemp].mask);
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
    BYTE uctemp = 0;
	
    for(uctemp = 0; uctemp <(sizeof(tMHL_24BIT_MODE_TABLE) /sizeof(msLoadTbl_S)); uctemp++)
    {
        msWrite2ByteMask(tMHL_24BIT_MODE_TABLE[uctemp].addr, tMHL_24BIT_MODE_TABLE[uctemp].databuf, tMHL_24BIT_MODE_TABLE[uctemp].mask);
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
    BYTE uctemp = 0;
	
    for(uctemp = 0; uctemp <(sizeof(tMHL_PACKET_PIXEL_MODE_TABLE) /sizeof(msLoadTbl_S)); uctemp++)
    {
        msWrite2ByteMask(tMHL_PACKET_PIXEL_MODE_TABLE[uctemp].addr, tMHL_PACKET_PIXEL_MODE_TABLE[uctemp].databuf, tMHL_PACKET_PIXEL_MODE_TABLE[uctemp].mask);
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
    BYTE uctemp = 0;
	
    for(uctemp = 0; uctemp <(sizeof(tMHL_AUTO_EQ_TABLE) /sizeof(msLoadTbl_S)); uctemp++)
    {
        msWrite2ByteMask(tMHL_AUTO_EQ_TABLE[uctemp].addr, tMHL_AUTO_EQ_TABLE[uctemp].databuf, tMHL_AUTO_EQ_TABLE[uctemp].mask);
    }
}

//**************************************************************************
//  [Function Name]:
//                  _mhal_mhl_IsCbusBusy()
//  [Description]
//                  
//  [Arguments]:
//
//  [Return]:
//                  
//**************************************************************************
Bool _mhal_mhl_IsCbusBusy(void)
{
    return ((msRead2Byte(REG_MHL_CBUS_5D) & 0x00F0) != 0 ? TRUE : FALSE);
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
void _mhal_mhl_AdjustCommonModeResistor(Bool bFlag) 
{
    bFlag = bFlag;
    //ElandWriteWordMask(REG_DVI_ATOP_62, bflag ? BIT3 : 0, BIT3); // [3]: adjust common-mode resistor
    //msWrite2ByteMask(REG_DVI_ATOP_61, bflag ? 0 : BIT1, BIT1);    // Common-mode resistor open
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
Bool _mhal_mhl_CheckCable(BYTE ucPort)
{
    Bool bindex = FALSE;
    
    switch(ucPort)
    {
#if(MHL_SUPPORT_PORT0)
        case MHL_INPUT_PORT0:
            if(MHL_CABLE_DETECT_PORT0 >= CBUS_CABLE_DETECT_LEVEL)
            {
                bindex = TRUE;
            }

            if(GET_MHL_VBUS_PORT0_FLAG() != bindex)
            {
                if(bindex)
                {
                    SET_MHL_VBUS_PORT0_FLAG();
                    SET_VBUS_CHARGE_PORT0();

                    MHL_DPUTSTR("**MHL Vbus charge port1\n");
                }
                else
                {
                    CLR_MHL_VBUS_PORT0_FLAG();
                    CLR_VBUS_CHARGE_PORT0();

                    MHL_DPUTSTR("**MHL Vbus uncharge port1\n");
                }
            }
            
            break;
            
#endif

#if(MHL_SUPPORT_PORT1)
        case MHL_INPUT_PORT1:
            if(MHL_CABLE_DETECT_PORT1 >= CBUS_CABLE_DETECT_LEVEL)
            {
                bindex = TRUE;
            }

            if(GET_MHL_VBUS_PORT1_FLAG() != bindex)
            {
                if(bindex)
                {
                    SET_MHL_VBUS_PORT1_FLAG();
                    SET_VBUS_CHARGE_PORT1();

                    MHL_DPUTSTR("**MHL Vbus charge port2\n");
                }
                else
                {
                    CLR_MHL_VBUS_PORT1_FLAG();
                    CLR_VBUS_CHARGE_PORT1();

                    MHL_DPUTSTR("**MHL Vbus uncharge port2\n");
                }
            }
            
            break;
            
#endif

#if(MHL_SUPPORT_PORT2)
        case MHL_INPUT_PORT2:
            if(MHL_CABLE_DETECT_PORT2 >= CBUS_CABLE_DETECT_LEVEL)
            {
                bindex = TRUE;
            }

            if(GET_MHL_VBUS_PORT2_FLAG() != bindex)
            {
                if(bindex)
                {
                    SET_MHL_VBUS_PORT2_FLAG();
                    SET_VBUS_CHARGE_PORT2();

                    MHL_DPUTSTR("**MHL Vbus charge port3\n");
                }
                else
                {
                    CLR_MHL_VBUS_PORT2_FLAG();
                    CLR_VBUS_CHARGE_PORT2();

                    MHL_DPUTSTR("**MHL Vbus uncharge port3\n");
                }
            }
            
            break;
            
#endif

        default:
            
            break;
    };

    return bindex;
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_CbusIsolate()
//  [Description]
//                  MHL cable isolate
//  [Arguments]:
//
//  [Return]:
//                  
//**************************************************************************
void mhal_mhl_CbusIsolate(Bool bFlag)
{
    if(bFlag)
    {
        msWrite2ByteMask(REG_PM_MHL_CBUS_00, BIT9, MASKBIT(9:8));
    }
    else
    {
        msWrite2ByteMask(REG_PM_MHL_CBUS_00, 0, MASKBIT(9:8));
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
void mhal_mhl_Initial(BYTE *pEdid, BYTE *pDevcap)
{
    BYTE ucindex = 0;
    WORD uctemp = 0; 
	
    // Initial setting
    for(uctemp = 0; uctemp <(sizeof(tMHL_INITIAL_TABLE) /sizeof(msLoadTbl_S)); uctemp++)
    {
        msWrite2ByteMask(tMHL_INITIAL_TABLE[uctemp].addr, tMHL_INITIAL_TABLE[uctemp].databuf, tMHL_INITIAL_TABLE[uctemp].mask);
    }
	
    // load EDID
    MHL_DPUTSTR("**Load EDID...\r\n");

    msWrite2ByteMask(REG_MHL_CBUS_52, BIT1, BIT1); // CPU write enable
    
    for(uctemp = 0; uctemp <256; uctemp++)
    {        
        msWrite2ByteMask(REG_MHL_CBUS_52, uctemp <<8, 0xFF00); // address        
        msWrite2ByteMask(REG_MHL_CBUS_53, pEdid[uctemp], 0x00FF); // data
        msWrite2ByteMask(REG_MHL_CBUS_52, BIT0, BIT0);
        msWrite2ByteMask(REG_MHL_CBUS_52, 0, BIT0);
        while(msRead2Byte(REG_MHL_CBUS_52) & BIT5);        
    }    

    msWrite2ByteMask(REG_MHL_CBUS_52, 0, BIT1); // CPU write disable
    
    // load MHL device capability
    MHL_DPUTSTR("**Load DevCap...\r\n"); 
    
    for(uctemp = 0; uctemp <8; uctemp++)
    {
        if(ucindex == 2)
        {
            ucindex = 1;
        }
        
        msWrite2Byte(REG_MHL_CBUS_01 +(uctemp *2), (pDevcap[(uctemp *2) +1 -ucindex] <<8) | pDevcap[uctemp *2 -ucindex]);
    }

    msWrite2Byte(REG_MHL_CBUS_03, DMHL_ADOPTER_ID);
    msWrite2Byte(REG_MHL_CBUS_07, DMHL_DEVICE_ID);

    // clear Cbus received interrupt status
    msWrite2ByteMask(REG_MHL_CBUS_3A, BIT4 |BIT0, BIT4 |BIT0);
    msWrite2ByteMask(REG_MHL_CBUS_3A, 0, BIT1); // [1]: receive packet valid mask
    
    //mhal_mhl_CbusIsolate(TRUE);
    
    bMHLFirstPowerOn = TRUE;
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
void mhal_mhl_PortSwitchMux(BYTE ucPort)
{
    switch(ucPort)
    {
#if(MHL_SUPPORT_PORT0)
        case MHL_INPUT_PORT0: // Port B
            msWrite2ByteMask(REG_PM_GPIO_30, BIT0, MASKBIT(1:0)); // CBUS port selsct [01] : portB
            msWrite2ByteMask(REG_PM_GPIO_31, BIT2 |BIT5, BIT2 |BIT3 |BIT5);

            if(GET_MHL_VBUS_PORT0_FLAG())
            {
                CLR_MHL_VBUS_PORT0_FLAG();
                CLR_VBUS_CHARGE_PORT0();

                MHL_DPUTSTR("**MHL Vbus uncharge port1~~~~\n");
            }
            
            break;
            
#endif

#if(MHL_SUPPORT_PORT1)
        case MHL_INPUT_PORT1: // Port A
            msWrite2ByteMask(REG_PM_GPIO_30, 0, MASKBIT(1:0)); // CBUS port selsct [00] : portA
            msWrite2ByteMask(REG_PM_GPIO_31, BIT3 |BIT5, BIT2 |BIT3 |BIT5);

            if(GET_MHL_VBUS_PORT1_FLAG())
            {
                CLR_MHL_VBUS_PORT1_FLAG();
                CLR_VBUS_CHARGE_PORT1();

                MHL_DPUTSTR("**MHL Vbus uncharge port2~~~~\n");
            }
            
            break;

#endif

#if(MHL_SUPPORT_PORT2)
        case MHL_INPUT_PORT2: // Port C
            msWrite2ByteMask(REG_PM_GPIO_30, BIT1, MASKBIT(1:0)); // CBUS port selsct [10] : portC
            msWrite2ByteMask(REG_PM_GPIO_31, BIT2 |BIT3, BIT2 |BIT3 |BIT5);

            if(GET_MHL_VBUS_PORT2_FLAG())
            {
                CLR_MHL_VBUS_PORT2_FLAG();
                CLR_VBUS_CHARGE_PORT2();

                MHL_DPUTSTR("**MHL Vbus uncharge port3~~~~\n");
            }
            
            break;

#endif

        default:
            msWrite2ByteMask(REG_PM_GPIO_30, BIT1 |BIT0, MASKBIT(1:0)); // CBUS port selsct [11] : Reserved
            msWrite2ByteMask(REG_PM_GPIO_31, BIT2 |BIT3 |BIT5, BIT2 |BIT3 |BIT5);
            break;
    };
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
    return ((msRead2Byte(REG_MHL_CBUS_3A) & BIT3) ? TRUE : FALSE);
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
    BYTE uctemp = 0;
    
    for(uctemp = 0; uctemp <pdatabuf->lens; uctemp++)
    {
        msWrite2Byte(REG_MHL_CBUS_26 +uctemp *2, pdatabuf->databuf[uctemp]);
    }
    
    // clear the unsed parts
    msWrite2Byte(REG_MHL_CBUS_26 +((pdatabuf->lens) *2), 0);

    if(_mhal_mhl_IsCbusBusy() && mhal_mhl_CbusIsMscMsgReceived())
    {
        return FALSE;
    }

    msWrite2ByteMask(REG_MHL_CBUS_25, BIT12, BIT12); // trigger to send

    return TRUE;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_MHLForceToStandby()
//  [Description]
//                  
//  [Arguments]:
//
//  [Return]:
//                  
//**************************************************************************
void mhal_mhl_MHLForceToStandby(void)
{
    msWrite2ByteMask(REG_MHL_CBUS_3A, BIT4, BIT4); // clear received FIFO
    
//#if(!DMHL_IGNORE_USB_PULSE_PATCH)
    msWrite2ByteMask(REG_PM_MHL_CBUS_17, BIT15, BIT15); // Force CBUS Standby
//#endif
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
    BYTE uctemp = 0;

    for(uctemp = 0; uctemp <MHL_INPUT_PORT_MASK; uctemp++)
    {
        bindex = _mhal_mhl_CheckCable((uctemp +GET_MHL_PORT_SELECT() +1) %MHL_INPUT_PORT_MASK);
    }
    
    if(bindex)
    {
        msWrite2ByteMask(REG_PM_MHL_CBUS_00, MASKBIT(9:8), MASKBIT(9:8));
    }
    else
    {
        msWrite2ByteMask(REG_PM_MHL_CBUS_00, BIT9, MASKBIT(9:8));
    }

    return bindex;
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
        msWrite2ByteMask(REG_PM_MHL_CBUS_17, BIT12, BIT12);
    }
    else // clear state to not PATH_EN
    {
        msWrite2ByteMask(REG_PM_MHL_CBUS_17, BIT13, BIT13);
    }
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_SetTMDSTxOnOff()
//  [Description]:
//                  HDMITx Set On/OFF
//  [Arguments]:
//                  TRUE: Enable
//                  FALSE: Disable
//  [Return]:
//   
//**************************************************************************
void mhal_mhl_SetTMDSTxOnOff(Bool bFlag)
{
    if(bFlag) // power on TMDS channel and clock
    {
        //msWrite2ByteMask(REG_HDMITX_MISC_2A, 0x000F, 0); // [3:0]: TDMS power on
        //msWrite2ByteMask(REG_HDMITX_MISC_2E, 0x000F, 0x000F); // enable TMDS DINEN async
    }
    else // power down TMDS channel and clock
    {
        //msWrite2ByteMask(REG_HDMITX_MISC_2E, 0x000F, 0x0000); // disable TMDS DINEN async
        //msWrite2ByteMask(REG_HDMITX_MISC_2A, 0x000F, 0x000F); // [3:0]: TDMS power down
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
void mhal_mhl_CbusIntCB(BYTE *rcstate, BYTE *rccmd, BYTE *rcdata, BYTE *rclen, Bool *bIsCmdInData)
{
    BYTE uctemp = 0;
    WORD reg_val;
    
    *rcstate = msRead2Byte(REG_MHL_CBUS_3B) & 0x000F; // received state, 0: normal, 1: timeout
    
    msWrite2ByteMask(REG_MHL_CBUS_3A, BIT0, BIT0); // clear INT
    
    *rccmd = msRead2Byte(REG_MHL_CBUS_3C) & 0x00FF;
    *bIsCmdInData = FALSE;

    for(uctemp = 0; uctemp <=(MHL_CBUS_DATA_SIZE +1); uctemp++) // offset+16bytes+EOF
    {
        reg_val = msRead2Byte(REG_MHL_CBUS_3D +(uctemp *2));
        
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

    msWrite2ByteMask(REG_MHL_CBUS_3A, BIT4, BIT4); // clear received FIFO
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
WORD mhal_mhl_CbusStatus(void)
{
    return (msRead2Byte(REG_PM_MHL_CBUS_17));
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
    switch(GET_MHL_PORT_SELECT())
    {
#if(MHL_SUPPORT_PORT0)
        case MHL_INPUT_PORT0: // Port B
            if(rctrl == RX_HDMI_RTERM)
            {
                msWrite2ByteMask(REG_PM_BANK_62, 0, MASKBIT(3:0));
            }
            else if(rctrl == RX_MHL_RTERM)
            {
                msWrite2ByteMask(REG_PM_BANK_62, BIT3 |BIT2 |BIT0, MASKBIT(3:0));
            }
            else if(rctrl == RX_RTERM_OFF)
            {
                msWrite2ByteMask(REG_PM_BANK_62, MASKBIT(3:0), MASKBIT(3:0));
            }
            break;
            
#endif

#if(MHL_SUPPORT_PORT1)
        case MHL_INPUT_PORT1: // Port A
            if(rctrl == RX_HDMI_RTERM)
            {
                msWrite2ByteMask(REG_PM_BANK_62, 0, MASKBIT(7:4));
            }
            else if(rctrl == RX_MHL_RTERM)
            {
                msWrite2ByteMask(REG_PM_BANK_62, BIT7 |BIT6 |BIT4, MASKBIT(7:4));
            }
            else if(rctrl == RX_RTERM_OFF)
            {
                msWrite2ByteMask(REG_PM_BANK_62, MASKBIT(7:4), MASKBIT(7:4));
            }
            break;
            
#endif

#if(MHL_SUPPORT_PORT2)
        case MHL_INPUT_PORT2: // Port C
            if(rctrl == RX_HDMI_RTERM)
            {
                msWrite2ByteMask(REG_PM_BANK_62, 0, MASKBIT(11:8));
            }
            else if(rctrl == RX_MHL_RTERM)
            {
                msWrite2ByteMask(REG_PM_BANK_62, BIT11 |BIT10 |BIT8, MASKBIT(11:8));
            }
            else if(rctrl == RX_RTERM_OFF)
            {
                msWrite2ByteMask(REG_PM_BANK_62, MASKBIT(11:8), MASKBIT(11:8));
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
        msWrite2ByteMask(REG_MHL_CBUS_6B, MHL_RX_XRAM_ADDR_FIRST, MASKBIT(15:0));
        
        CLR_MHL_RX_SRAM_FLAG();
    }
    else
    {
        msWrite2ByteMask(REG_MHL_CBUS_6B, MHL_RX_XRAM_ADDR_SECOND, MASKBIT(15:0));
        
        SET_MHL_RX_SRAM_FLAG();
    }

    msWrite2ByteMask(REG_MHL_CBUS_6C, BIT15, BIT15);
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
        case MHL_CABLE_PLUGGED:
            _mhal_mhl_LoadMhl24bitsModeTbl();
            mhal_mhl_RxRtermControl(RX_RTERM_OFF);

#if(DMHL_LG_PRADA_PATCH)
            _mhal_mhl_AdjustCommonModeResistor(TRUE);
#endif

            break;

        case MHL_CABLE_UNPLUGGED:
            _mhal_mhl_LoadHdmiBypassTbl();
            mhal_mhl_RxRtermControl(RX_HDMI_RTERM);
            break;

        case MHL_CBUS_UNCONNECTION:
            mhal_mhl_RxRtermControl(RX_RTERM_OFF);

#if(DMHL_LG_PRADA_PATCH)
            _mhal_mhl_AdjustCommonModeResistor(TRUE);
#endif
            break;

        case MHL_CBUS_CONNECTION:
            mhal_mhl_RxRtermControl(RX_MHL_RTERM);
            _mhal_mhl_LoadMhl24bitsModeTbl();

#if(DMHL_LG_PRADA_PATCH)
            _mhal_mhl_AdjustCommonModeResistor(FALSE);
#endif

            mhal_mhl_RxSRAMSwitch();

            break;

        case MHL_CBUS_WAKE_UP_DETECT:
#if DMHL_LG_PRADA_PATCH
            _mhal_mhl_AdjustCommonModeResistor(FALSE);
#endif
            mhal_mhl_RxRtermControl(RX_MHL_RTERM);

            break;

        case MHL_CBUS_NORMAL_24BIT_MODE:
            _mhal_mhl_LoadMhl24bitsModeTbl();
            break;
            
        case MHL_CBUS_PACKET_PIXEL_MODE:
            _mhal_mhl_LoadMhlPackedPixelModeTbl();
            break;

        case MHL_CBUS_AUTO_EQ_SETTING:
            _mhal_mhl_LoadMhlAutoEQTbl();
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
void mhal_mhl_CDRModeMonitor(Bool bPathEnable)
{
    static WORD wpre_val = 0;
    WORD wcur_val;
    static BYTE u8count = 1;
    BYTE u8Threshold = 100;

    if(bPathEnable)
    {
        // pixel rate must large than 25MHz then power on DVI PLL
        wcur_val = msRead2Byte(REG_COMBO_DECODER_5B) &0x0FFF;
        
        if(wcur_val > CBUS_CLOCK_DETECT_LEVEL)
        {
            if((wcur_val >= (wpre_val -CBUS_CLOCK_DETECT_OFFSET)) && (wcur_val <= (wpre_val +CBUS_CLOCK_DETECT_OFFSET)))
            {
                if(u8count<u8Threshold)
                {
                    u8count++;
                }
                
                if(u8count == (u8Threshold -1))
                {
                    msWrite2ByteMask(REG_PM_BANK_61, 0, MASKBIT(15:14));
                    MHL_DPRINTF("**MHL CDR PLL Power On: %x\n", wcur_val);
                }
#if(DMHL_CBUS_AUTO_EQ)
                else if(u8count == (u8Threshold /2))
                {
                    mhal_mhl_DriverControl(MHL_CBUS_AUTO_EQ_SETTING);
                    
                    msWrite2ByteMask(REG_COMBO_DECODER_25, BIT2, BIT2);
                }
#endif
            }
        }
        else
        {
            bPathEnable = FALSE;
        }
        
        wpre_val = wcur_val;
    }

    if(!bPathEnable)
    {
        if(u8count > 0)
        {
            u8count = 0;

            msWrite2ByteMask(REG_PM_BANK_61, MASKBIT(15:14), MASKBIT(15:14));
            MHL_DPRINTF("**MHL CDR PLL Power Off: %x\n", wcur_val);
        }
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
    BYTE uctemp = 0;

    switch(ucState)
    {
        case MHL_POWER_ON:
            for(uctemp = 0; uctemp <(sizeof(tMHL_POWER_ON_TABLE) /sizeof(msLoadTbl_S)); uctemp++)
            {
                msWrite2ByteMask(tMHL_POWER_ON_TABLE[uctemp].addr, tMHL_POWER_ON_TABLE[uctemp].databuf, tMHL_POWER_ON_TABLE[uctemp].mask);
            }
            
            break;
            
        case MHL_POWER_STANDBY:
            for(uctemp = 0; uctemp <(sizeof(tMHL_POWER_SAVING_TABLE) /sizeof(msLoadTbl_S)); uctemp++)
            {
                msWrite2ByteMask(tMHL_POWER_SAVING_TABLE[uctemp].addr, tMHL_POWER_SAVING_TABLE[uctemp].databuf, tMHL_POWER_SAVING_TABLE[uctemp].mask);
            }
            
            break;
            
        case MHL_POWER_DOWN:
            for(uctemp = 0; uctemp <(sizeof(tMHL_POWER_DOWN_TABLE) /sizeof(msLoadTbl_S)); uctemp++)
            {
                msWrite2ByteMask(tMHL_POWER_DOWN_TABLE[uctemp].addr, tMHL_POWER_DOWN_TABLE[uctemp].databuf, tMHL_POWER_DOWN_TABLE[uctemp].mask);
            }
            
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
        msWrite2ByteMask(REG_PM_MHL_CBUS_20, BIT3, BIT4|BIT3); // [4]: output val, [3]: oen
        //MHL_DPUTSTR("**MHL_HPD is High");
    }
    else // hpd is low
    {
        msWrite2ByteMask(REG_PM_MHL_CBUS_20, 0, BIT4|BIT3); // [4]: output val, [3]: oen
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
    return msRead2Byte(HB(REG_MHL_CBUS_21));
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_CheckDEStable()
//  [Description]
//                  
//  [Arguments]:
//
//  [Return]:
//                  
//**************************************************************************
Bool mhal_mhl_CheckDEStable(void)
{
    return 0;//(msRead2Byte(REG_DVI_DTOP_31) & BIT6);
}

#else
BYTE code msMhalMHLNullData[] = {0};

void msMhalMHLDummy(void)
{
    BYTE xdata i = msMhalMHLNullData[0];
}

#endif //#if ENABLE_MHL

#endif // _MHAL_MHL_C_

