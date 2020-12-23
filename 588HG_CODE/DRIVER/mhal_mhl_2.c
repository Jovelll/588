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
/*
#include "board.h"
#include "types.h"
#include "ms_reg.h"
//#include "DebugMsg.h"
#include "global.h"
#include "misc.h"
#include "swI2c.h"
#include "mapi_mhl.h"
#include "mhal_mhl.h"
*/
#if ENABLE_MHL

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

#define MHL_DBG     0

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
#define WMASK    0xFFFF    // word mask

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
BYTE xdata ucMHLDriver = MHL_DRIVER_VERSION;
Bool bMHLFirstPowerOn = FALSE;

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------

code msLoadTbl_S MhlDevInit_JunoE_tbl[] = 
{
#if DMHLInit
    {REG_CHIPTOP_06, 0xFFFF, 0x1080}, // [12:8]: ckg_mhl_out, MHL 24 bits mode, Eland internal 12M RC clock **
#else
    {REG_CHIPTOP_06, 0xFFEF, 0x0000}, // [12:8]: ckg_mhl_out, MHL packed-pixel mode
    //{REG_CHIPTOP_06, 0xFFEF, 0x0080}, // [12:8]: ckg_mhl_out, MHL packed-pixel mode
    //{REG_CHIPTOP_06, 0xFFEF, 0x0081}, // [12:8]: ckg_mhl_out, MHL packed-pixel mode
#endif
    //{REG_CHIPTOP_06, 0xFFFF, 0x1081}, // [12:8]: ckg_mhl_out, MHL 24 bits mode
    //{REG_CHIPTOP_2D, BIT13|BIT10, BIT13}, // [13]: reg_mhlrx_cable_det_en, [10]: reg_all_pad_in
    //{REG_CHIPTOP_2E, BIT5, BIT5}, // [5]: reg_qfn40_mode(SI mode)    
    //{REG_DVI_DTOP_27, 0xFFFF, 0x2C6C}, // [1]: MHL pack-pixel mode, [0]: MHL enable
    {REG_CHIPTOP_2C, 0xFFFF, 0x0480}, // JunoE MHL Rx and HDMITx pad mux setting **
    {REG_CHIPTOP_2D, 0xFFFF, 0x2103}, // JunoE MHL Rx and HDMITx pad mux setting **
    {REG_CHIPTOP_2E, 0xFFFF, 0x0006}, // JunoE MHL Rx and HDMITx pad mux setting **
#if DMHLInit
    {REG_DVI_DTOP_27, 0xFFFF, 0x016D/*0x2C6D*/}, // [7]: MHL HW mode, [1]: MHL pack-pixel mode, [0]: MHL enable
#else // HDMIBypass
    {REG_DVI_DTOP_27, 0xFFFF, 0x2C6C}, // [7]: MHL HW mode, [1]: MHL pack-pixel mode, [0]: MHL enable
#endif
    {REG_DVI_DTOP_28, 0xFFFF, 0x0000}, // [14]: MHL pack to HDMI, [13]: MHL pack video swap
    // Enable DVI clock tolerance: 4*0.7 = 2.8 MHz
    {REG_DVI_DTOP_13, 0x7FFF, 0x2C80}, // [14:12]: the comp value is reg_comp_tol * 4, [11:0]: 300MHz
    {REG_DVI_DTOP_12, 0xF000, 0xF000}, // [15]: reg_comp_tol_en, [14:12]: reg_comp_tol = 0x7
    {REG_DVI_DTOP_1F, 0x0700, 0x0700}, // [10:8] : clock unstable count threshold = 0x07
    {REG_DVI_ATOP_00, 0xFFFF, 0x0010}, // [4]: enable DVI
    {REG_DVI_ATOP_03, BIT13|BIT8|BIT0, 0x0000}, // clock powe on, [13]: PD_ICLK, [8]: PD_CLKD_DVI
    {REG_DVI_ATOP_60, 0xFFFF, 0x0000}, // DVI port A PLL power on
    {REG_DVI_ATOP_61, 0x000F, 0x000A}, // [3:2]: [1:0] RX swap for JunoE **        
    {REG_DVI_ATOP_69, 0xC7FF, 0x0000}, // DVI port C PLL power on
    //{REG_DVI_ATOP_69, 0xFFFF, 0x3800}, // DVI port C PLL power on
    {REG_DVI_ATOP_6C, BIT1, BIT1}, // reg_en_cal2=1・b1
    {REG_DVI_ATOP_6D, 0xFBFF, 0x0000}, // DVI PM port C power on
    {REG_DVI_ATOP_7F, 0xFFFF, 0x0000}, // DVI DPL power on
    {REG_HDMITX_MISC_5A, 0xFFFF, 0x001F}, // [13:12]: Tx div mode
    //{REG_HDMITX_MISC_5A, 0xFFFF, 0x201F}, // [13:12]: Tx div mode
    {REG_HDMITX_MISC_1E, 0xFFFF, 0xFFFF}, // enable clock gen
    {REG_HDMITX_MISC_2A, 0xFFFF, 0x0000}, // [3:0]: TMDS Tx power down
    {REG_HDMITX_MISC_2B, 0xFFFF, 0x0000}, // analog test register
    {REG_HDMITX_MISC_2C, 0xFFFF, 0x0001}, // analog test register
    {REG_HDMITX_MISC_2D, 0xFFFF, 0x0000}, // analog test register
    {REG_HDMITX_MISC_58, 0xFFFF, 0x0002}, // [1]: enable new FIFO
    {REG_HDMITX_MISC_5B, 0x1800, 0x1800}, // [12:11] = 2'b 11 HDMI Tx swap for JunoE **        
    {REG_HDMITX_MISC_2E, 0xFFFF, 0x000F}, // [3:0]: enable TMDS Tx
    {REG_MHL_CBUS_18, BIT7, BIT7}, // [7]: reg_hdcp_mhlcbus2iic_en, HDCP bypass mode
    {REG_PM_MHL_CBUS_21, BIT1, BIT1}, // [1]: output mhl_zxsense_tmds inv
#if 1
    {REG_CBUS2I2C_08, 0xFFFF, 0x000A}, // reg_stop_cnt = 'ha
    {REG_CBUS2I2C_09, 0xFFFF, 0x0014}, // reg_hcnt = ．h14
    {REG_CBUS2I2C_0A, 0xFFFF, 0x000A}, // reg_lcnt =・ha
    {REG_CBUS2I2C_0B, 0xFFFF, 0x0002}, // reg_sda_cnt = ．h2
    {REG_CBUS2I2C_0C, 0xFFFF, 0x000A}, // reg_start_cnt = ．ha
    {REG_CBUS2I2C_0D, 0xFFFF, 0x0004}, // reg_data_lat_cnt = ．h4
#else
    {REG_CBUS2I2C_08, 0xFFFF, 0x001E}, // reg_stop_cnt = 'ha
    {REG_CBUS2I2C_09, 0xFFFF, 0x003C}, // reg_hcnt = ．h14
    {REG_CBUS2I2C_0A, 0xFFFF, 0x001E}, // reg_lcnt =・ha
    {REG_CBUS2I2C_0B, 0xFFFF, 0x0006}, // reg_sda_cnt = ．h2
    {REG_CBUS2I2C_0C, 0xFFFF, 0x001E}, // reg_start_cnt = ．ha
    {REG_CBUS2I2C_0D, 0xFFFF, 0x000C}, // reg_data_lat_cnt = ．h4
#endif
    {REG_CBUS2I2C_00, BIT0, BIT0}, // [0]: soft reset
    {REG_CBUS2I2C_00, BIT0, 0}, // [0]: soft reset

#if DMHL_INT_ENABLE
    // Mask unused interrupt events
    {REG_PM_MHL_CBUS_16, BIT13|BIT9, BIT13|BIT9}, // [13]: lnk_lay_en_chg_int mask, [9]: conn_state_chg_int mask
    {REG_PM_MHL_CBUS_18, BIT9|BIT5|BIT1, BIT9|BIT5|BIT1}, // [9]: cbus_discover_pul_confirm_int mask, [5]: wakeup_pul_confirm_int mask, [1]: cbus_stuck_to_low_int mask.
    {REG_PM_MHL_CBUS_30, BIT1, BIT1}, // [1]: cbus conflict_int mask 
    {REG_PM_MHL_CBUS_38, BIT13|BIT9|BIT5|BIT1, BIT13|BIT9|BIT5|BIT1}, // [13]: unused mask, [9]: unused mask, [5]: unused mask, [1]: unused mask.
    {REG_MHL_CBUS_14, BIT13, BIT13}, // [13]: int mask for monitor_sram_full  
    {REG_MHL_CBUS_18, BIT13, BIT13}, // [13]: send rcv_pkt_ddc_sw_overwrite_err_in mask
    {REG_MHL_CBUS_19, BIT5|BIT1, BIT5|BIT1}, // [5]: send ddc send complete interrupt mask, [1]: send ddc error interrupt mask
    {REG_MHL_CBUS_1B, BIT1, BIT1}, // [1]: receive ddc packet valid mask
    {REG_MHL_CBUS_1F, BIT5|BIT1, BIT5|BIT1}, // [5]: ddc access edid timeout int mask, [1]: client_wrt_ddc_ram interrupt mask 
    {REG_MHL_CBUS_21, BIT5, BIT5}, // [5]: ddc error interrupt mask
    {REG_MHL_CBUS_22, BIT9|BIT5|BIT1, BIT9|BIT5|BIT1}, // [9]: ddc access hdcp timeout int mask, [5]: receive nack pkt int_mask, [1]: receive abort pkt int mask
    {REG_MHL_CBUS_23, BIT13, BIT13}, // [13]: send rcv_pkt_msc_sw_overwrite_err_in mask
    {REG_MHL_CBUS_24, BIT1, BIT1}, // [1]: send error interrupt mask
    {REG_MHL_CBUS_25, BIT9|BIT5|BIT1, BIT9|BIT5|BIT1}, // [9]: response_pkt_msc_hw_int mask, [5]: snd_pkt_msc_hw_int mask, [1]: msc sw send complete interrupt mask    
    {REG_MHL_CBUS_63, BIT9, BIT9}, // [9]: dytycycle_bad_int mask
    {REG_MHL_CBUS_65, BIT9|BIT5|BIT1, BIT9|BIT5|BIT1}, // [9]: rcv_parity_err_int mask, [5]: rcv_data_err_int mask, [1]: rcv_sync_err_int mask
    {REG_MHL_CBUS_78, BIT13|BIT9|BIT5|BIT1, BIT13|BIT9|BIT5|BIT1}, // [13]: unused mask, [9]: unused mask, [5]: unused mask, [1]: unused mask.
#endif

    {REG_DVI_ATOP_65, BIT1, BIT1},  //0465[1]=1
    {REG_DVI_ATOP_5B, MASKBIT(7:4), BIT5},  // 045B[7:4]=4'b0010
};

code msLoadTbl_S Mhl24bitsMode_JunoE_tbl[] = 
{
    {REG_CHIPTOP_06, 0xFFEF, 0x1000}, // [12:8]: ckg_mhl_out, MHL 24 bits mode
    //{REG_CHIPTOP_06, 0xFFEF, 0x1080}, // [12:8]: ckg_mhl_out, MHL 24 bits mode
    //{REG_CHIPTOP_06, 0xFFFF, 0x1081}, // [12:8]: ckg_mhl_out, MHL 24 bits mode
    //{REG_CHIPTOP_2E, BIT5, BIT5}, // [5]: reg_qfn40_mode(SI mode)    
    // switch to CDR mode
    {REG_DVI_DTOP_0E, BIT4, 0}, // auto clear phase accumulator 
    {REG_DVI_DTOP_3A, 0xFFFF, 0x18}, // phase code = 0x18
    {REG_DVI_DTOP_3B, BIT4, BIT4}, // overwirte enable
    {REG_DVI_DTOP_3C, 0xFFFF, 0x18}, // phase code = 0x18
    {REG_DVI_DTOP_3D, BIT4, BIT4}, // overwirte enable
    {REG_DVI_DTOP_3E, 0xFFFF, 0x18}, // phase code = 0x18
    {REG_DVI_DTOP_3F, BIT4, BIT4}, // overwirte enable
    {REG_DVI_ATOP_64, BIT9|BIT7, BIT9|BIT7}, // DVI DPL test register [39][9][7] = 1
    {REG_DVI_ATOP_66, BIT7, BIT7},
    {REG_DVI_ATOP_69, 0x0007, 0x06}, // [2:0]: power down TMDS ch1/2 EQ
    {REG_DVI_ATOP_7F, 0x3F00, 0x3600}, // power down TMDS ch1&2 phase dac, [13:11]: PDN_DPLPH_Q2, [10:8]: PDN_DPLPH_I2
    {REG_DVI_DTOP_27, 0xFFFF, 0x016D/*0x2C6D*/}, // [7]: MHL HW mode, [1]: MHL pack-pixel mode, [0]: MHL enable
    {REG_DVI_DTOP_28, 0xFFFF, 0x0000}, // [14]: MHL pack to HDMI, [13]: MHL pack video swap    
    {REG_HDMITX_MISC_5A, 0xFFFF, 0x201F}, // [13:12]: Tx div mode
};

code msLoadTbl_S MhlPackedPixelMode_JunoE_tbl[] = 
{
    {REG_CHIPTOP_06, 0xFFEF, 0x0800}, // [12:8]: ckg_mhl_out, MHL packed-pixel mode
    //{REG_CHIPTOP_06, 0xFFEF, 0x0880}, // [12:8]: ckg_mhl_out, MHL packed-pixel mode
    //{REG_CHIPTOP_06, 0xFFEF, 0x0881}, // [12:8]: ckg_mhl_out, MHL packed-pixel mode
    //{REG_CHIPTOP_2E, BIT5, BIT5}, // [5]: reg_qfn40_mode(SI mode) 
    {REG_DVI_DTOP_0C, BIT9|BIT1|BIT0, BIT9|BIT0}, // R-term overwrite to MHL R-term
    {REG_DVI_DTOP_27, 0xFFFF, 0x2C6F}, // [7]: MHL HW mode, [1]: MHL pack-pixel mode, [0]: MHL enable
    {REG_DVI_DTOP_28, 0xFFFF, 0x4000}, // [14]: MHL pack to HDMI, [13]: MHL pack video swap    
    {REG_HDMITX_MISC_5A, 0xFFFF, 0x101F}, // [13:12]: Tx div mode
};

code msLoadTbl_S MhlHdmiBypass_JunoE_tbl[] = 
{
    {REG_CHIPTOP_06, 0xFFEF, 0x0000}, // [12:8]: ckg_mhl_out, MHL packed-pixel mode
    //{REG_CHIPTOP_06, 0xFFEF, 0x0080}, // [12:8]: ckg_mhl_out, MHL packed-pixel mode
    //{REG_CHIPTOP_06, 0xFFEF, 0x0081}, // [12:8]: ckg_mhl_out, MHL packed-pixel mode
    // switch to CDR mode
    {REG_DVI_DTOP_0E, BIT4, BIT4}, // auto clear phase accumulator 
    {REG_DVI_DTOP_3B, BIT4, 0}, // overwirte enable
    {REG_DVI_DTOP_3D, BIT4, 0}, // overwirte enable
    {REG_DVI_DTOP_3F, BIT4, 0}, // overwirte enable
    {REG_DVI_ATOP_64, BIT9|BIT7, 0}, // DVI DPL test register [39][9][7] = 1
    {REG_DVI_ATOP_66, BIT7, 0},
    {REG_DVI_ATOP_69, BIT7, 0}, // [7]: power on DVI PLL
    {REG_DVI_ATOP_61, 0x000F, 0x000A}, // [3:2]: [1:0] RX swap for JunoE **  
    {REG_DVI_ATOP_69, 0x0007, 0}, // [2:0]: power on TMDS ch1/2 EQ
    {REG_DVI_ATOP_7F, 0x3F00, 0x0000}, // power on TMDS ch1&2 phase dac, [13:11]: PDN_DPLPH_Q2, [10:8]: PDN_DPLPH_I2
    {REG_DVI_DTOP_0C, BIT9|BIT1|BIT0, 0x0000}, // R-term overwrite disable
    {REG_DVI_DTOP_27, 0xFFFF, 0x2C6C}, // [7]: MHL HW mode, [1]: MHL pack-pixel mode, [0]: MHL enable
    {REG_DVI_DTOP_28, 0xFFFF, 0x0000}, // [14]: MHL pack to HDMI, [13]: MHL pack video swap    
    {REG_HDMITX_MISC_5A, 0xFFFF, 0x001F}, // [13:12]: Tx div mode
};

code msLoadTbl_S MhlPowerDown_JunoE_tbl[] = 
{
    {REG_DVI_ATOP_03, BIT13|BIT8|BIT0, BIT13|BIT8}, // clock powe on
    //{REG_DVI_ATOP_69, 0x38A7, 0x38A7}, // DVI port C PLL power on
    {REG_DVI_ATOP_69, 0x00A7, 0x00A7}, // DVI port C PLL power on
    {REG_DVI_ATOP_7F, 0x3F00, 0x3F00}, // DVI DPL power on
    //{REG_DVI_ATOP_6D, 0x0D00, 0x0D00}, // DVI PM port C power on
    {REG_DVI_ATOP_6D, 0x0900, 0x0900}, // DVI PM port C power on
    //{REG_DVI_DTOP_27, BIT0, BIT0}, // [0]: MHL enable
    {REG_HDMITX_MISC_1E, 0x0006, 0}, // [2]: PD_TX_XTAL, [1]: PD_TX_ICLK
    {REG_HDMITX_MISC_2A, 0x000F, 0x000F}, // [3:0]: TMDS Tx power down
    {REG_HDMITX_MISC_2B, 0x0007, 0x0007}, // analog test register
    {REG_CHIPTOP_06, 0x0178, 0x0178}, // [8]: reg_ckg_mhl_out, [6]: reg_ckg_cbus, [5]: reg_ckg_ddc, [4]: reg_ckg_cec, [3]: reg_ckg_efuse
};

code msLoadTbl_S MhlPowerSave_JunoE_tbl[] = 
{
    //{REG_DVI_ATOP_03, BIT13|BIT8|BIT0, 0}, // clock powe on
    //{REG_DVI_ATOP_69, 0x38A7, 0}, // DVI port C PLL power on
    {REG_DVI_ATOP_7F, 0x3F00, 0x3F00}, // DVI DPL power on
    //{REG_DVI_ATOP_69, 0x38A7, 0x30A6}, // [2:0]: power down TMDS ch1/2 EQ
    {REG_DVI_ATOP_69, 0x00A7, 0x00A6}, // [2:0]: power down TMDS ch1/2 EQ
    //{REG_DVI_ATOP_6D, 0x0D00, 0x0C00}, // [11]: PD_BG, [10]: PD_RCK, [8]: PD_CLKIN
    //{REG_DVI_DTOP_27, BIT0, BIT0}, // [0]: MHL enable
    {REG_HDMITX_MISC_1E, 0x0006, 0}, // [2]: PD_TX_XTAL, [1]: PD_TX_ICLK
    {REG_HDMITX_MISC_2A, 0x000F, 0x000F}, // [3:0]: TMDS Tx power down
    {REG_HDMITX_MISC_2B, 0x0007, 0x0007}, // analog test register
    {REG_CHIPTOP_06, 0x0178, 0x0010}, // [8]: reg_ckg_mhl_out, [6]: reg_ckg_cbus, [5]: reg_ckg_ddc, [4]: reg_ckg_cec, [3]: reg_ckg_efuse
};

code msLoadTbl_S MhlPowerOn_JunoE_tbl[] = 
{
    {REG_DVI_ATOP_03, BIT13|BIT8|BIT0, 0}, // clock powe on
    //{REG_DVI_ATOP_69, 0x38A7, 0}, // DVI port C PLL power on
    {REG_DVI_ATOP_69, 0x00A7, 0}, // DVI port C PLL power on
    {REG_DVI_ATOP_7F, 0x3F00, 0}, // DVI DPL power on
    //{REG_DVI_ATOP_6D, 0x0D00, 0}, // DVI PM port C power on
    {REG_DVI_ATOP_6D, 0x0900, 0}, // DVI PM port C power on
    //{REG_DVI_DTOP_27, BIT0, 0}, // [0]: MHL enable
    {REG_HDMITX_MISC_1E, 0x0006, 0x0006}, // [2]: PD_TX_XTAL, [1]: PD_TX_ICLK
    {REG_HDMITX_MISC_2A, 0x000F, 0}, // [3:0]: TMDS Tx power down
    {REG_HDMITX_MISC_2B, 0x0007, 0x0000}, // analog test register
    {REG_CHIPTOP_06, 0x0178, 0x0010}, // [8]: reg_ckg_mhl_out, [6]: reg_ckg_cbus, [5]: reg_ckg_ddc, [4]: reg_ckg_cec, [3]: reg_ckg_efuse
};

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

Bool ElandWriteWord (WORD waddr, WORD wdata)
{
      if(i2cElandWriteWord (Eland_SRID_ID, waddr, wdata) == FALSE)
        return FALSE;
      return TRUE;
}

WORD ElandReadWord (WORD waddr)
{
      return  i2cElandReadWord (Eland_SRID_ID, waddr); 
}

Bool ElandWriteHalfWord (WORD ucaddr, BYTE ucdata)
{
      if(i2cElandWriteByte (Eland_SRID_ID, ucaddr, ucdata) == FALSE)
        return FALSE;
      return TRUE;
}

BYTE ElandReadHalfWord(WORD ucaddr)
{
      return  i2cElandReadByte (Eland_SRID_ID, ucaddr); 
}

Bool ElandSetI2C16BMode (void)
{
     if(swi2cElandConfigBytes(Eland_CONFIG_ID,0x00,Eland_SRID_ID) == FALSE) // Set Slave ID
        return FALSE;
     if(swi2cElandConfigBytes(Eland_CONFIG_ID,0x01,0x01) == FALSE) // Set 16Bits Mode
        return FALSE;

     return TRUE;
}


Bool ElandWriteWordMask(WORD reg, WORD val, WORD mask)
{
    WORD wtemp;
    BYTE uctemp;
    
    if(mask !=WMASK)
    {
        if((mask & HBMASK) && (mask & LBMASK))
        {
            wtemp=ElandReadWord(reg) & (~mask);
            if(ElandWriteWord(reg,( wtemp | (val & mask))) == FALSE)
                return FALSE;
        }
        else if(mask & HBMASK)
        {
            uctemp=ElandReadHalfWord(HB(reg)) & (BYTE)((~mask) >> 8);
            if(ElandWriteHalfWord(HB(reg),( uctemp | (BYTE)((val & mask) >> 8))) == FALSE)
                return FALSE;
        }
        else if(mask & LBMASK)
        {
            uctemp=ElandReadHalfWord(LB(reg)) & (BYTE)(~mask);
            if(ElandWriteHalfWord(LB(reg),( uctemp | (BYTE)(val & mask))) == FALSE)
                return FALSE;
        }
    }
    else
    {
        if(ElandWriteWord(reg, val) == FALSE)
            return FALSE;
    }
    
    return TRUE;
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
    if(IS_MHL_CABLE_HI())
    {
        if(bFlag)
        {
            ElandWriteWordMask(REG_PM_MHL_CBUS_00, BIT9, BIT9 |BIT8);
        }
        else
        {
            ElandWriteWordMask(REG_PM_MHL_CBUS_00, BIT9 |BIT8, BIT9 |BIT8);
        }
    }
}

//**************************************************************************
//  [Function Name]:
//                  mhal_mhl_init()
//  [Description]
//                  MHL init
//  [Arguments]:
//                  *edid: MHL EDID data
//                  *devcap: MHL device capability
//  [Return]:
//
//**************************************************************************
Bool mhal_mhl_init(BYTE *edid, BYTE *devcap)
{
    WORD i, wval=0;

    // Configure Eland I2C 
    //SET_SWI2C_SDA_OUT();
    //SET_SWI2C_SCL_OUT();
    // Configure MHL cable detect and Vbus
    //SET_MHL_CABLE_DETECT();
    //SET_MHL_PS_CTRL_HI(); // Vbus output low
    //Clr_MHL_PS_CTRL_CHARGE();
    //SET_MHL_PS_CTRL_OUT();


    // set Eland I2C 16 bits access mode	
    //ElandSetI2C16BMode(); 
    if(ElandSetI2C16BMode() == FALSE)
        return FALSE;
    ElandWriteWordMask(REG_CHIPTOP_55, (BIT15 |BIT14 |BIT13 |BIT8), MASKBIT(15:8));
    ucMHLDriver = ElandReadHalfWord(HB(REG_CHIPTOP_04));
    ElandWriteWordMask(REG_CHIPTOP_55, 0, MASKBIT(15:8));
#if ENABLE_DEBUG
    printData("**MHL driver version = %d", ucMHLDriver);
#endif	
    // Initial setting
    for(i=0; i<(sizeof(MhlDevInit_JunoE_tbl) /sizeof(msLoadTbl_S)); i++)
    {
        if(ElandWriteWordMask(MhlDevInit_JunoE_tbl[i].addr, MhlDevInit_JunoE_tbl[i].databuf, MhlDevInit_JunoE_tbl[i].mask) == FALSE)
            return FALSE;
    }
	
    // load EDID
#if DMHL_EDID_BYPASS 
{
    BYTE tmp_val = edid[0];
    ElandWriteWordMask(REG_MHL_CBUS_18, BIT6, BIT6); // Enable BYPASS EDID DDC
}
#else
    MHL_DPUTSTR("**Load EDID...\r\n");
    //ElandWriteWordMask(REG_CHIPTOP_06, 0, BIT5); // [5]: reg_ckg_ddc     
    ElandWriteWordMask(REG_DDC_22, BIT15, BIT15); // [15]: DDC function enable
    ElandWriteWordMask(REG_DDC_21, 0, BIT9); // [9]: DDC write enable   
    for(i=0; i<256; i++)
    {        
        //ElandWriteWordMask(REG_DDC_23, i<<8, HBMASK); // address        
        //ElandWriteWordMask(REG_DDC_24, edid[i], LBMASK); // data
        if(ElandWriteWord(HB(REG_DDC_23), (edid[i]<<8)|i) == FALSE) // [15:8]:edid, [7:0]:address          
            return FALSE;
        if(ElandWriteWordMask(REG_DDC_21, BIT15, BIT15) == FALSE) // write trigger
            return FALSE;
        if(ElandWriteWordMask(REG_DDC_21, 0, BIT15) == FALSE)
            return FALSE;
        //while(ElandReadByte(HB(REG_DDC_22)) & BIT2);        
    }    
    ElandWriteWordMask(REG_DDC_21, BIT9, BIT9); // CPU write disable
#endif // #if DMHL_EDID_BYPASS

    // load Devcap
    MHL_DPUTSTR("**Load DevCap...\r\n");    
    // load MHL device capability
    for(i=0; i<8; i++)
    {
        if(ElandWriteWord(REG_MHL_CBUS_01+(i*2), (devcap[(i*2)+1]<<8) | devcap[i*2]) == FALSE)
            return FALSE;
    }

    // clear Cbus received interrupt status
#if 1
    ElandWriteWordMask(REG_MHL_CBUS_3A, BIT4|BIT0, BIT4|BIT0);
    //ElandWriteWordMask(REG_MHL_CBUS_3A, 0, BIT4|BIT0);
#if DMHL_INT_ENABLE
    ElandWriteWordMask(REG_MHL_CBUS_3A, 0, BIT1); // [1]: receive packet valid mask
#else
    ElandWriteWordMask(REG_MHL_CBUS_3A, BIT1, BIT1); // [1]: receive packet valid mask
#endif
#endif

#if 0
    // switch to CDR mode
    ElandWriteWord(REG_DVI_DTOP_3E, 0x18); // phase code = 0x18
    ElandWriteWordMask(REG_DVI_DTOP_3F, BIT4, BIT4); // overwirte enable
    ElandWriteWordMask(REG_DVI_ATOP_64, BIT9|BIT7, BIT9|BIT7);	 // DVI DPL test register [39][9][7] = 1
    ElandWriteWordMask(REG_DVI_ATOP_66, BIT7, BIT7);
#endif

    // modified Cbus related setting for Eland 12M RC clock - 10MHz 
    if(!(ElandReadWord(REG_CHIPTOP_02) & BIT15)) // [15:8]: efuse output data
    {
    #if DMHL_CTS
        ElandWriteWord(REG_CHIPTOP_01, 0x8C); // 9.8MHz	
    #else
    ElandWriteWord(REG_CHIPTOP_01, 0x91); // 9.8MHz
    #endif
    }
    // Enable INT
#if DMHL_INT_ENABLE
    ElandWriteWordMask(REG_CHIPTOP_2C, BIT12, BIT12|BIT11|BIT10); // INT pad mux
    ElandWriteWordMask(REG_CHIPTOP_0B, 0, BIT0); // Chiptop INT mask  
#endif
    //ElandWriteWord(REG_MHL_CBUS_58, 0x000A);
    //ElandWriteWord(REG_MHL_CBUS_57, 0x0205);
    // HW auto reply setting in Cbus
#if DMHL_DEVCAP_HWACK
    wval |=BIT10;
#endif
#if DMHL_RCP_RAP_HWACK
    wval |=BIT9|BIT8;
#endif
#if DMHL_WRT_GRT_HWACK
    wval |=BIT4;
#endif
    ElandWriteWordMask(REG_MHL_CBUS_23, wval, BIT10|BIT9|BIT8|BIT4);
    // MHL cable detect overwrite to HDMI
    ElandWriteWordMask(REG_PM_MHL_CBUS_00, BIT9, BIT9|BIT8);

#if DMHL_HTC_FLYER_PATCH
    ElandWriteWordMask(REG_MHL_CBUS_70, BIT14, BIT14); // [14]: let edid current read initial address add 1
#endif
    // U02
#if DMHL_WRITE_STATE_INT_HWACK
    ElandWriteWordMask(REG_MHL_CBUS_71, BIT15, BIT15);
#endif
    ElandWriteWordMask(REG_DVI_DTOP_37, BIT12, BIT12); // [12]: Support DVI mode

#if DMHL_FPGA_TEST
    mhal_mhl_LoadMhl24bitsModeTbl();
    ElandWriteWordMask(REG_PM_MHL_CBUS_00, BIT9|BIT8, BIT9|BIT8); // MHL cable detect overwrite
    SET_MHL_PS_CTRL_CHARGE0();
    mhal_mhl_RxRtermControl(RX_MHL_RTERM);
#endif

#if DMHL_CTS
    // CTS 4.3.7.1
    ElandWriteWordMask(REG_MHL_CBUS_55, 0x0010, 0x00F0); // [7:4]: cbus requester transmit opportunity after arbitration
    // CTS 4.3.8.1
    //ElandWriteWord(REG_PM_MHL_CBUS_0C, 0x0028); // reject cbus discovery pulse below this limit (40us)
    //ElandWriteWord(REG_PM_MHL_CBUS_0D, 0x00C8); // reject cbus discovery pulse above this limit (200us) 
    //ElandWriteWordMask(REG_PM_MHL_CBUS_15, 0x0005, 0x000F); // discovery pulse count to assume connected state
#endif

#if(DMHL_IGNORE_USB_PULSE_PATCH)
    // Set accepted discover pulse high pulse width to ignore USB pulse
    ElandWriteWord(REG_PM_MHL_CBUS_0C, 0x46);
    ElandWriteWord(REG_PM_MHL_CBUS_0D, 0x82);

#endif

    if(ucMHLDriver >= MHL_DRIVER_U03)
    {
        // Enable DVI mode support
        ElandWriteWordMask(REG_DVI_DTOP_37, BIT14, BIT14);
        
        // Detect discover pulse low width for CTS 4.3.15.1/2
        ElandWriteWordMask(REG_PM_MHL_CBUS_31, BIT4, BIT4);

        // Modify burst write time out timer for CTS 6.3.10.6[1]
        // For CTS 6.3.21.1/2[5][3] and CTS 6.3.22.2[4]
        ElandWriteWordMask(REG_MHL_CBUS_71, (BIT1 |BIT3 |BIT4 |BIT5), MASKBIT(5:3) |BIT1);

        // power down dvi pll till clock stable
        //ElandWriteWordMask(REG_DVI_ATOP_6A, BIT2, BIT2);
    }

    if(ucMHLDriver >= MHL_DRIVER_U04)
    {
        // 0x0f5b[15] : DC Balance enable
        // 0x0f5b[14] : DC Balance mode, 0 : div2 mode; 1 : lfsr mode
        ElandWriteWordMask(REG_HDMITX_MISC_5B, MASKBIT(15:14), MASKBIT(15:14));

        // fix send abort issue
        ElandWriteWordMask(REG_MHL_CBUS_71, BIT7, BIT7);
    }

    // DVI clock detection in 40M count value
    ElandWriteWordMask(REG_DVI_DTOP_0F, 0x01AA, MASKBIT(11:0));
    
    // DVI clock detection in 80M count value
    ElandWriteWordMask(REG_DVI_DTOP_10, 0x0355, MASKBIT(11:0));
    
    // DVI clock detection in 160M count value
    ElandWriteWordMask(REG_DVI_DTOP_11, 0x06AA, MASKBIT(11:0));

    ElandWriteWordMask(REG_DVI_ATOP_64, MASKBIT(15:14), MASKBIT(15:14));
    
    mhal_mhl_CbusIsolate(TRUE);
    bMHLFirstPowerOn = TRUE;

    return TRUE;
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
    BYTE i;

    for(i=0;i<pdatabuf->lens;i++)
    {
        if(mhal_mhl_CbusIsMscMsgReceived())
        {
            return FALSE;
        }
        
        ElandWriteWord(REG_MHL_CBUS_26+i*2, pdatabuf->databuf[i]);
    }
    // clear the unsed parts
    ElandWriteWord(REG_MHL_CBUS_26+((pdatabuf->lens)*2), 0);
#if 0
    for(i=pdatabuf->lens; i<DMHL_CBUS_TX_FIFO; i++)
    {
        ElandWriteWord(REG_MHL_CBUS_26+i*2, 0);
    }		
#endif
    if(mhal_mhl_IsCbusBusy() || mhal_mhl_CbusIsMscMsgReceived())
    {
        return FALSE;
    }

    //ElandWriteWordMask(REG_MHL_CBUS_3A, BIT4|BIT0, BIT4|BIT0); // clear Rx FIFO
    ElandWriteWordMask(REG_MHL_CBUS_25, BIT12, BIT12); // trigger to send

    return TRUE;
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
    static Bool bMhlCablePlug = FALSE;
    Bool res = TRUE;
#if 1
    BYTE i, u8unstablecnt = 0;

    if(GET_MHL_PORT_ON_FLAG())
    {
        // JunoE need use GPIO for MHL cable detection 
        //return ((ElandReadWord(REG_PM_MHL_CBUS_00) & BIT10) ? TRUE : FALSE);
        if ((ElandReadWord(REG_PM_MHL_CBUS_17) & 0x03) != 0x03)
        {
            for(i=0, u8unstablecnt=0;i<2;i++)
            {
                Delay1ms(1);
                if (!IS_MHL_CABLE_HI())
                    u8unstablecnt++;
            }
        }
        if(u8unstablecnt > 1) // unplugged
        {
            //MHL_DPRINTF("**cableunstable = %d", u8unstablecnt);
            if(bMhlCablePlug)
            {
                ElandWriteWordMask(REG_PM_MHL_CBUS_00, BIT9, BIT9|BIT8); // MHL cable detect overwrite 
                bMhlCablePlug = FALSE;
            }
            res = FALSE;
        }
        else // plugged
        {
            if(!bMhlCablePlug)
            {
#if DMHL_LG_PRADA_PATCH
                mhal_mhl_AdjustCommonModeResistor(TRUE);
#endif
                mhal_mhl_RxRtermControl(RX_RTERM_OFF);//mhal_mhl_RxRtermControl(RX_MHL_RTERM);//mhal_mhl_RxRtermControl(RX_RTERM_OFF);
                //SET_MHL_PS_CTRL_CHARGE0();
                mhal_mhl_MHLForceToStandby();
                ElandWriteWordMask(REG_PM_MHL_CBUS_00, BIT9|BIT8, BIT9|BIT8); // MHL cable detect overwrite
#if 1
                if(bMHLFirstPowerOn)
                {
                    bMHLFirstPowerOn = FALSE;
                    ForceDelay1ms(100);
                    ElandWriteWordMask(REG_PM_MHL_CBUS_01, 0x0028, 0x003C); // Cbus pull down resistor overwrite
                    ForceDelay1ms(100);
                    ElandWriteWordMask(REG_PM_MHL_CBUS_01, 0, 0x003C); // Cbus pull down resistor overwrite
                }
#endif
                bMhlCablePlug = TRUE;

            }
            res = TRUE;
        }
#else
        if(!bMhlCablePlug)
        {
            ElandWriteWordMask(REG_PM_MHL_CBUS_00, BIT9|BIT8, BIT9|BIT8); // MHL cable detect overwrite
            bMhlCablePlug = TRUE;		
        }
#endif	
    }
    else // Force HDMI mode
    {
        // Cable detect reset
        bMhlCablePlug = FALSE;
        res = FALSE;
        ElandWriteWordMask(REG_PM_MHL_CBUS_00, BIT9, BIT9|BIT8); // MHL cable detect overwrite 
    }
    
    return (res);		
}


void mhal_mhl_MHLForceToStandby(void)
{
    ElandWriteWordMask(REG_MHL_CBUS_3A, BIT4, BIT4); // clear received FIFO
    
//#if(!DMHL_IGNORE_USB_PULSE_PATCH)
        ElandWriteWordMask(REG_PM_MHL_CBUS_17, BIT15, BIT15); // Force CBUS Standby
//#endif
}

#if 0
void mhal_mhl_MHLForceToFloating(Bool flag)
{
    ElandWriteWordMask(REG_PM_MHL_CBUS_17, flag ? BIT5 : 0, BIT5); // Cbus floating
}
#endif

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
void mhal_mhl_Cbus_SetPathEn(Bool bflag)
{
    if(bflag) // set state to PATH_EN
    {
        ElandWriteWordMask(REG_PM_MHL_CBUS_17, BIT12, BIT12);
        //ElandWriteWordMask(REG_PM_MHL_CBUS_17, 0, BIT12);
    }
    else // clear state to not PATH_EN
    {
        ElandWriteWordMask(REG_PM_MHL_CBUS_17, BIT13, BIT13);
        //ElandWriteWordMask(REG_PM_MHL_CBUS_17, 0, BIT13);
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
void mhal_mhl_SetTMDSTxOnOff(Bool bflag)
{
    if(bflag) // power on TMDS channel and clock
    {
        ElandWriteWordMask(REG_HDMITX_MISC_2A, 0x000F, 0); // [3:0]: TDMS power on
        ElandWriteWordMask(REG_HDMITX_MISC_2E, 0x000F, 0x000F); // enable TMDS DINEN async
    }
    else // power down TMDS channel and clock
    {
        ElandWriteWordMask(REG_HDMITX_MISC_2E, 0x000F, 0x0000); // disable TMDS DINEN async
        ElandWriteWordMask(REG_HDMITX_MISC_2A, 0x000F, 0x000F); // [3:0]: TDMS power down
    }
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
    return((ElandReadWord(REG_MHL_CBUS_3A) & BIT3) ? TRUE : FALSE);
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
    BYTE i = 0;
    WORD reg_val;
    
    *rcstate = ElandReadWord(REG_MHL_CBUS_3B) & 0x000F; // received state, 0: normal, 1: timeout
    
    ElandWriteWordMask(REG_MHL_CBUS_3A, BIT0, BIT0); // clear INT
    //ElandWriteWordMask(REG_MHL_CBUS_3A, 0, BIT0);
    
    *rccmd = ElandReadWord(REG_MHL_CBUS_3C) & 0x00FF;
    *bIsCmdInData = FALSE;
#if 0    
    while(ElandReadWord(REG_MHL_CBUS_3D+(i*2)) & BIT15)
    {
        rcdata[i] = ElandReadWord(REG_MHL_CBUS_3D+(i*2)) & 0x00FF;
        i++;
    }
#else
    for(i=0;i<=(MHL_CBUS_DATA_SIZE+1); i++) // offset+16bytes+EOF
    {
        reg_val = ElandReadWord(REG_MHL_CBUS_3D+(i*2));
        if(reg_val & BIT15)
        {
            rcdata[i] = reg_val & 0x00FF;
            if( (((i>=0) && (i<=2)) ||(i==(MHL_CBUS_DATA_SIZE+1))) && !(*bIsCmdInData) )
                *bIsCmdInData = (reg_val & BIT8) ? TRUE : FALSE;
        }
        else
        {
            *rclen = i;
            break;   
        }
    }
    // CTS 6.3.11.19
    if(i>(MHL_CBUS_DATA_SIZE+1))
        *rclen = MHL_CBUS_DATA_SIZE+2;
#endif

    ElandWriteWordMask(REG_MHL_CBUS_3A, BIT4, BIT4); // clear received FIFO
    //ElandWriteWordMask(REG_MHL_CBUS_3A, 0, BIT4);	
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
    return(ElandReadWord(REG_PM_MHL_CBUS_17));
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
#if DMHL_CTS_HDCP_PATCH
    static BYTE u8DEStableCnt = 0;
#endif
    BYTE u8Threshold = 100;

    if(ucMHLDriver <= MHL_DRIVER_U02)
    {
        if(bPathEnable)
        {
            // pixel rate must large than 25MHz then power on DVI PLL
            wcur_val = mhal_mhl_GetTMDSClk();//ElandReadWord(REG_DVI_DTOP_17) & 0x0FFF;
            
            if(wcur_val > 0xD0)
            {
                if((wcur_val >= (wpre_val-2)) && (wcur_val <= (wpre_val+2)))
                {
                    if(u8count<u8Threshold)
                        u8count++;
                    if(u8count == (u8Threshold-1))
                    {
                        ElandWriteWordMask(REG_DVI_ATOP_69, 0, BIT7); // power on DVI PLL
                        ElandWriteWordMask(REG_DVI_ATOP_66, BIT7, BIT7); // [7]: enable CDR mode            
            #if DMHL_CTS_HDCP_PATCH
                        ElandWriteWordMask(REG_DVI_DTOP_27, 0, BIT6|BIT5); // [5]: sequential check disable
            #endif
                    }
            #if 0//DMHL_CTS_HDCP_PATCH
                        ElandWriteWordMask(REG_DVI_DTOP_27, 0, BIT6|BIT5); // [5]: sequential check disable
            #endif
            #if DMHL_CTS_HDCP_PATCH
                    if((u8count == u8Threshold) && (u8DEStableCnt<u8Threshold))
                    {
                        if((ElandReadWord(REG_DVI_DTOP_31) & BIT6))
                            u8DEStableCnt++;
                        else
                            u8DEStableCnt =0;

                        if(u8DEStableCnt == u8Threshold)
                            ElandWriteWordMask(REG_DVI_DTOP_27, 0, BIT6|BIT5); // [5]: sequential check disable
                    }
            #endif
                }
            }
            else
            {
                if(u8count > 0)
                {
                    u8count = 0;
                    ElandWriteWordMask(REG_DVI_ATOP_66, 0, BIT7); // [7]: disable CDR mode
                    ElandWriteWordMask(REG_DVI_ATOP_69, BIT7, BIT7); // [7]: power down DVI PLL
            #if DMHL_CTS_HDCP_PATCH
                    u8DEStableCnt =0;
                    ElandWriteWordMask(REG_DVI_DTOP_27, BIT6|BIT5, BIT6|BIT5); // [5]: sequential check enable
            #endif
                }
            }
            wpre_val = wcur_val;
        }
        else
        {
            if(u8count > 0)
            {
                u8count = 0;
                ElandWriteWordMask(REG_DVI_ATOP_66, 0, BIT7); // [7]: disable CDR mode
                ElandWriteWordMask(REG_DVI_ATOP_69, BIT7, BIT7); // [7]: power down DVI PLL
            #if DMHL_CTS_HDCP_PATCH
                u8DEStableCnt =0;
                ElandWriteWordMask(REG_DVI_DTOP_27, BIT6|BIT5, BIT6|BIT5); // [5]: sequential check enable
            #endif
            }
        }
    }
}

Bool mhal_mhl_IsCbusBusy(void)
{
    return ( (ElandReadWord(REG_MHL_CBUS_5D) & 0x00F0) != 0 ? TRUE : FALSE);
}


// MHL TMDS termination resistor control
void mhal_mhl_RxRtermControl(RXRtermControl_T rctrl) 
{
    if (rctrl == RX_HDMI_RTERM)
    {
        ElandWriteWordMask(REG_DVI_ATOP_69, 0, BIT13|BIT12|BIT11);// data R-term
        ElandWriteWordMask(REG_DVI_ATOP_6D, 0, BIT10);// clock R-term
    }
    else if (rctrl == RX_MHL_RTERM)
    {
        ElandWriteWordMask(REG_DVI_ATOP_69, BIT13|BIT12, BIT13|BIT12|BIT11);// data R-term
        ElandWriteWordMask(REG_DVI_ATOP_6D, BIT10, BIT10);// clock R-term
    }
    else if (rctrl == RX_RTERM_OFF)
    {
        ElandWriteWordMask(REG_DVI_ATOP_69, BIT13|BIT12|BIT11, BIT13|BIT12|BIT11);// data R-term
        ElandWriteWordMask(REG_DVI_ATOP_6D, BIT10, BIT10);// clock R-term
    }
}

#if DMHL_LG_PRADA_PATCH
void mhal_mhl_AdjustCommonModeResistor(Bool bflag) 
{
    //ElandWriteWordMask(REG_DVI_ATOP_62, bflag ? BIT3 : 0, BIT3); // [3]: adjust common-mode resistor
    ElandWriteWordMask(REG_DVI_ATOP_61, bflag ? 0 : BIT1, BIT1);    // Common-mode resistor open
}
#endif


// MHL HDMI bypass setting
void mhal_mhl_LoadHdmiBypassTbl(void) 
{
    BYTE i;
	
    for(i=0; i<(sizeof(MhlHdmiBypass_JunoE_tbl) /sizeof(msLoadTbl_S)); i++)
    {
        if(ElandWriteWordMask(MhlHdmiBypass_JunoE_tbl[i].addr, MhlHdmiBypass_JunoE_tbl[i].databuf, MhlHdmiBypass_JunoE_tbl[i].mask) == FALSE)
        {
            MHL_DPRINTF("mhal_mhl_LoadHdmiBypassTbl FAIL:%X", MhlHdmiBypass_JunoE_tbl[i].addr);
        }
    }

    if(ucMHLDriver >= MHL_DRIVER_U03)
    {
        // power down dvi pll till clock stable
        ElandWriteWordMask(REG_DVI_ATOP_6A, 0, BIT2);
    }
}

// MHL 24 bits mode setting
void mhal_mhl_LoadMhl24bitsModeTbl(void) 
{
    BYTE i;
	
    for(i=0; i<(sizeof(Mhl24bitsMode_JunoE_tbl) /sizeof(msLoadTbl_S)); i++)
    {
        if(ElandWriteWordMask(Mhl24bitsMode_JunoE_tbl[i].addr, Mhl24bitsMode_JunoE_tbl[i].databuf, Mhl24bitsMode_JunoE_tbl[i].mask) == FALSE)
        {
            MHL_DPRINTF("mhal_mhl_LoadMhl24bitsModeTbl FAIL:%X", Mhl24bitsMode_JunoE_tbl[i].addr);
        }
    }

    if(ucMHLDriver <= MHL_DRIVER_U02)
    {
        ElandWriteWordMask(REG_DVI_DTOP_0C, BIT9|BIT0, BIT9|BIT1|BIT0);
    }
    else
    {
        // power down dvi pll till clock stable
        ElandWriteWordMask(REG_DVI_ATOP_6A, BIT2, BIT2);
    }
}

#if 0
// MHL packed pixel mode setting
void mhal_mhl_LoadMhlPackedPixelModeTbl(void) 
{
    BYTE i;
	
    for(i=0; i<(sizeof(MhlPackedPixelMode_JunoE_tbl) /sizeof(msLoadTbl_S)); i++)
    {
        ElandWriteWordMask(MhlPackedPixelMode_JunoE_tbl[i].addr, MhlPackedPixelMode_JunoE_tbl[i].databuf, MhlPackedPixelMode_JunoE_tbl[i].mask);
    }
}
#endif

// MHL power control
void mhal_mhl_PowerCtrl(BYTE pctrl) 
{
    BYTE i;

    switch(pctrl)
    {
        case MHL_POWER_ON:
            for(i=0; i<(sizeof(MhlPowerOn_JunoE_tbl) /sizeof(msLoadTbl_S)); i++)
            {
                ElandWriteWordMask(MhlPowerOn_JunoE_tbl[i].addr, MhlPowerOn_JunoE_tbl[i].databuf, MhlPowerOn_JunoE_tbl[i].mask);
            }
            break;
        case MHL_POWER_STANDBY:
            for(i=0; i<(sizeof(MhlPowerSave_JunoE_tbl) /sizeof(msLoadTbl_S)); i++)
            {
                ElandWriteWordMask(MhlPowerSave_JunoE_tbl[i].addr, MhlPowerSave_JunoE_tbl[i].databuf, MhlPowerSave_JunoE_tbl[i].mask);
            }

            if(ucMHLDriver <= MHL_DRIVER_U03)
            {
                ElandWriteWordMask(REG_DVI_ATOP_6D, 0x0800, 0x0900);
            }
            break;
        case MHL_POWER_DOWN:
            for(i=0; i<(sizeof(MhlPowerDown_JunoE_tbl) /sizeof(msLoadTbl_S)); i++)
            {
                ElandWriteWordMask(MhlPowerDown_JunoE_tbl[i].addr, MhlPowerDown_JunoE_tbl[i].databuf, MhlPowerDown_JunoE_tbl[i].mask);
            }
            break;
        default:
            break;
    }
}

// config HPD in combo(MHL/HDMI) port A
void mhal_mhl_SetHPD(Bool bflag)
{
    if(bflag) // HPD is high
    {
        ElandWriteWordMask(REG_PM_MHL_CBUS_20, BIT3, BIT4|BIT3); // [4]: output val, [3]: oen
        //MHL_DPUTSTR("**MHL_HPD is High");
    }
    else // hpd is low
    {
        ElandWriteWordMask(REG_PM_MHL_CBUS_20, 0, BIT4|BIT3); // [4]: output val, [3]: oen
        //MHL_DPUTSTR("**MHL_HPD is Low");
    }
}

// Get DDC error code
BYTE mhal_mhl_GetDDCErrorCode(void)
{
    return ElandReadHalfWord(HB(REG_MHL_CBUS_21));
}

WORD mhal_mhl_GetTMDSClk(void)
{
    return (ElandReadWord(REG_DVI_DTOP_17) & 0x0FFF);
}

Bool mhal_mhl_CheckDEStable(void)
{
    return (ElandReadWord(REG_DVI_DTOP_31) & BIT6);
}


#if ENABLE_DEBUG
extern Bool mdrv_mhl_CbusWriteBurst(BYTE addr, BYTE lens, BYTE *buf);
void MhlTestfunction(BYTE tb1, BYTE tb2, BYTE tb3, BYTE tb4)
{
    BYTE i;
    mhalCbusFifo_S xdata pbuf;
	
    switch(tb1)
    {
        case 0: // read
            MHL_DPRINTF("Eland= %x\r\n", ElandReadWord(tb2<<8|tb3));
            break;
        case 1: // write
            ElandWriteHalfWord(tb2<<8|tb3, tb4);
            MHL_DPRINTF("Eland= %x\r\n", ElandReadWord(tb2<<8|tb3));			
            break;
        case 2: // write
            ElandWriteWord(REG_MHL_CBUS_26, 0x8D64);
            ElandWriteWordMask(REG_MHL_CBUS_25, BIT12, BIT12); // trigger to send
            MHL_DPRINTF("ACK= %x\r\n", ElandReadWord(0x0B74));			
            //ElandWriteWordMask(REG_MHL_CBUS_25, 0, BIT12); // trigger to send   
            break;
	case 3:
	    
	    ElandWriteWordMask(0x0B30, 0, BIT5);
	    // start bit
		MHL_DPUTSTR("I2C start ...\n");
		for (i=0;i<100; i++)
		{
            ElandWriteWordMask(0x0D02, BIT0, BIT0);
	        //MHL_DPRINTF("I2C= %x\r\n", ElandReadWord(0x0D08));
            ElandWriteWordMask(0x0D08, 0, BIT0);
	        //MHL_DPRINTF("I2C= %x\r\n", ElandReadWord(0x0D0A));
			// I2c slave address
			//MHL_DPUTSTR("slave addr ...\n");
            ElandWriteWordMask(0x0D04, 0x74, 0x00FF);
			//Delay1ms(1);
	        //MHL_DPRINTF("I2C= %x\r\n", ElandReadWord(0x0D08));
	        //MHL_DPRINTF("I2C= %x\r\n", ElandReadWord(0x0D04));
            ElandWriteWordMask(0x0D08, 0, BIT0);
	        //MHL_DPRINTF("I2C= %x\r\n", ElandReadWord(0x0D0A));
			// offset
		    //MHL_DPUTSTR("offset addr ...\n");
            ElandWriteWordMask(0x0D04, 0x40, 0x00FF);
			//Delay1ms(1);
	        //MHL_DPRINTF("I2C= %x\r\n", ElandReadWord(0x0D08));
	        //MHL_DPRINTF("I2C= %x\r\n", ElandReadWord(0x0D04));
            ElandWriteWordMask(0x0D08, 0, BIT0);
	        //MHL_DPRINTF("I2C= %x\r\n", ElandReadWord(0x0D0A));
			// stop
			//MHL_DPUTSTR("stop ...\n");
            ElandWriteWordMask(0x0D02, BIT8, BIT8);
	        //MHL_DPRINTF("I2C= %x\r\n", ElandReadWord(0x0D08));
            ElandWriteWordMask(0x0D08, 0, BIT0);
	        //MHL_DPRINTF("I2C= %x\r\n", ElandReadWord(0x0D0A));
			}
            break;
        case 4: // set HPD
            ElandWriteWordMask(REG_MHL_CBUS_3A, BIT4, BIT4); // clear received FIFO
            pbuf.lens = 1;
            pbuf.databuf[0] = 0x8D64;
            pbuf.databuf[1] = 0x0;
            pbuf.databuf[2] = 0x0;
            pbuf.databuf[3] = 0x0;
            mhal_mhl_CBusWrite(&pbuf);			
            MHL_DPUTSTR("** Test modeL: Set HPD...\n");
            break;
        case 5: // Set CDR mode
            ElandWriteWordMask(REG_DVI_DTOP_0E, 0, BIT4); // disable DVI phase clear
            ElandWriteWord(REG_DVI_DTOP_3A, 0x18); // phase code = 0x18
            ElandWriteWordMask(REG_DVI_DTOP_3B, BIT4, BIT4); // overwirte enable
            ElandWriteWord(REG_DVI_DTOP_3C, 0x18); // phase code = 0x18
            ElandWriteWordMask(REG_DVI_DTOP_3D, BIT4, BIT4); // overwirte enable
            ElandWriteWord(REG_DVI_DTOP_3E, 0x18); // phase code = 0x18
            ElandWriteWordMask(REG_DVI_DTOP_3F, BIT4, BIT4); // overwirte enable
            ElandWriteWordMask(REG_DVI_ATOP_64, BIT9|BIT7, BIT9|BIT7);	 // DVI DPL test register [39][9][7] = 1
            ElandWriteWordMask(REG_DVI_ATOP_66, BIT7, BIT7);
	     break;
        case 6: // Set PLL mode
            ElandWriteWordMask(REG_DVI_DTOP_0E, BIT4, BIT4); // enable DVI phase clear   
            ElandWriteWordMask(REG_DVI_DTOP_3B, 0, BIT4); // overwirte enable
            ElandWriteWordMask(REG_DVI_DTOP_3D, 0, BIT4); // overwirte enable
            ElandWriteWordMask(REG_DVI_DTOP_3F, 0, BIT4); // overwirte enable
            ElandWriteWordMask(REG_DVI_ATOP_64, 0, BIT9|BIT7);	 // DVI DPL test register [39][9][7] = 1
            ElandWriteWordMask(REG_DVI_ATOP_66, 0, BIT7);
	     break;
        case 7:
            ElandWriteWordMask(REG_MHL_CBUS_3A, BIT4, BIT4); // clear received FIFO
            pbuf.lens = 2;
            pbuf.databuf[0] = 0x8D61;
            pbuf.databuf[1] = 0x840D;
            mhal_mhl_CBusWrite(&pbuf);			
            MHL_DPUTSTR("** Test: Read Scrachpad size...\n");			
            break;
        case 8: // Write INT - REQ_WRT
            ElandWriteWordMask(REG_MHL_CBUS_3A, BIT4, BIT4); // clear received FIFO
            pbuf.lens = 3;
            pbuf.databuf[0] = 0x8560;
            pbuf.databuf[1] = 0x8420;
            pbuf.databuf[2] = 0x8404;
            mhal_mhl_CBusWrite(&pbuf);			
            MHL_DPUTSTR("** Test: REQ_WRT...\n");
            break;
        case 9:
            {
            ElandWriteWordMask(REG_MHL_CBUS_3A, BIT4, BIT4); // clear received FIFO
            pbuf.lens = 18;
            pbuf.databuf[0] = 0x856C;
            pbuf.databuf[1] = 0x8440;
            pbuf.databuf[2] = 0x8401;
            pbuf.databuf[3] = 0x8402;
            pbuf.databuf[4] = 0x8C03;
            pbuf.databuf[5] = 0x8404;
            pbuf.databuf[6] = 0x8C05;
            pbuf.databuf[7] = 0x8C06;
            pbuf.databuf[8] = 0x8407;
            pbuf.databuf[9] = 0x8408;
            pbuf.databuf[10] = 0x8C09;
            pbuf.databuf[11] = 0x8C0A;
            pbuf.databuf[12] = 0x840B;
            pbuf.databuf[13] = 0x8C0C;
            pbuf.databuf[14] = 0x840D;
            pbuf.databuf[15] = 0x840E;
            pbuf.databuf[16] = 0x8C0F;
            pbuf.databuf[17] = 0x8410;
            mhal_mhl_CBusWrite(&pbuf);			
            MHL_DPUTSTR("** Test mode: Scrtchpad...\n");
            break;
            }
        case 10:
            {
            BYTE buf[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
            ElandWriteWordMask(REG_MHL_CBUS_3A, BIT4, BIT4); // clear received FIFO
            mdrv_mhl_CbusWriteBurst(0x40, 16, &buf[0]);
            MHL_DPUTSTR("** Test mode: WriteBurst...\n");
            }
            break;
        default:
            break;
    }
}

void MhlTestfunction1(BYTE tb1, BYTE tb2, BYTE tb3, BYTE tb4)
{
    BYTE tmp = tb4;

    ElandWriteHalfWord(tb1<<8|tb2, tb3);
    MHL_DPRINTF("Eland= %x\r\n", ElandReadWord(tb1<<8|tb2));


}
#endif
#endif //#if ENABLE_MHL

#endif // _MHAL_MHL_C_

