///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   Mapi_mhl.h
/// @author MStar Semiconductor Inc.
/// @brief  MHL driver Function
///////////////////////////////////////////////////////////////////////////////////////////////////

/*
    1. Check whether MHL cable is plugged or not in Rx side
    2. Disable R-term and output Vbus
    3. Check whether Cbus is connected(discovery pulse completed)
    4. Sink -> Source: "SET_HPD"
    5. Sink -> Source: Write_Stat - "DCAP_RDY" and Write_INT - "DCAP_CHG"
    6. Sink -> Source: Write_Stat - "PATH_EN"
*/


#ifndef _MAPI_MHL_H_
#define _MAPI_MHL_H_

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define MHL_DRIVER_U01                   0
#define MHL_DRIVER_U02                   1
#define MHL_DRIVER_U03                   2
#define MHL_DRIVER_U04                   3

#define MHL_DRIVER_VERSION               MHL_DRIVER_U04

#define DMHL_INT_ENABLE    1 // MHL interrupt enable
#define DMHL_DEVCAP_HWACK    0 // enable HW auto response read_devcap request
#define DMHL_RCP_RAP_HWACK    0 // enable HW auto response ack pkt while receive msc rcp and rap sub-command
#define DMHL_WRT_GRT_HWACK    0 // while scrachpad ok, and need HW responses wrt_grt_int
#define DMHL_WRITE_STATE_INT_HWACK    0 // enable HW auto response write state and interupt request
#define DMHL_CTS    1 // MHL setting for CTS
#define DMHL_EDID_BYPASS    0 // MHL EDID is used HDMI EDID

#define DEM_POLLING_DELAY       4     // Main Loop Polling interval (ms)
#define CBUS_COMMAND_TIMEOUT_SECS    2       // No response for 1 second means we hold off and move
#define CBUS_ABORT_PADING_SECS       2       // Allow bus to quieten down when ABORTs are received.

#define DMHL_ADOPTER_ID             0x02CB // Mstar: 715
#define DMHL_DEVICE_ID              0x0000 // Chip ID?
#define DMHL_VENDOR_ID              0x00 // ?

#define MHL_MSC_SCRATCHPAD_SIZE     16
#define MHL_MSC_SCRATCHPAD_OFFSET     0x40
#define MHL_CBUS_DATA_SIZE          (MHL_MSC_SCRATCHPAD_SIZE +2) // offset + maxi 16 packets
#define DMHL_CBUS_TX_FIFO           (MHL_MSC_SCRATCHPAD_SIZE +3) // Write_Burst(0x6C) + offset(0x40 ~ 0x7F) + maxi 16 packets + EOF(0x32)
#define MHL_CBUS_TX_QUEUE_NUM       3
#define MHL_CBUS_TOTAL_QUEUE_NUM    ((MHL_CBUS_TX_QUEUE_NUM > 0)?(MHL_CBUS_TX_QUEUE_NUM +1):2)
#define MHL_CBUS_RX_QUEUE_INDEX     (MHL_CBUS_TOTAL_QUEUE_NUM -1)

#define DMHL_HTC_FLYER_PATCH    1 // for HTC Flyer patch
#define DMHL_FPGA_TEST    0 // for MHL Tx FPGA testing
#define DMHL_CTS_HDCP_PATCH    0 // for CTS HDCP 2C-04
#define DMHL_LG_PRADA_PATCH    1 // for LG PRADA patch
#define DMHL_IGNORE_USB_PULSE_PATCH    1 // for ignore USB pulse patch

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
enum
{
    MHL_POWER_ON,
    MHL_POWER_STANDBY,
    MHL_POWER_DOWN
};

enum CBUS_FLAG_TYPE
{
    CBUS_HPD_SET_FLAG = BIT0,
    CBUS_DCAP_RDY_FLAG = BIT1,
    CBUS_DCAP_CHG_FLAG = BIT2,
    CBUS_PATH_EN_FLAG = BIT3,
    CBUS_TIME_OUT_FLAG = BIT4,
    CBUS_PORT_ON_FLAG = BIT5,
    CBUS_CONNECT_FLAG = BIT6,
    CBUS_CABLE_PLUG_FLAG = BIT7,
};

#define MHL_CBUS_LINK_MASK          (CBUS_HPD_SET_FLAG |CBUS_PATH_EN_FLAG |CBUS_DCAP_RDY_FLAG)

//-------------------------------------------------------------------------------------------------
//  Macro define
//-------------------------------------------------------------------------------------------------
#define GET_MHL_CABLE_PLUGGED()     (Bool)(gMHLInfo.ucCbusIndex & CBUS_CABLE_PLUG_FLAG)
#define SET_MHL_CABLE_PLUGGED()     (gMHLInfo.ucCbusIndex |= CBUS_CABLE_PLUG_FLAG)
#define CLR_MHL_CABLE_PLUGGED()     (gMHLInfo.ucCbusIndex &= ~CBUS_CABLE_PLUG_FLAG)

#define GET_MHL_CBUS_CONNECT()      (Bool)(gMHLInfo.ucCbusIndex & CBUS_CONNECT_FLAG)
#define SET_MHL_CBUS_CONNECT()      (gMHLInfo.ucCbusIndex |= CBUS_CONNECT_FLAG)
#define CLR_MHL_CBUS_CONNECT()      (gMHLInfo.ucCbusIndex &= ~CBUS_CONNECT_FLAG)

#define GET_MHL_PORT_ON_FLAG()      (Bool)(gMHLInfo.ucCbusIndex & CBUS_PORT_ON_FLAG)
#define SET_MHL_PORT_ON_FLAG()      (gMHLInfo.ucCbusIndex |= CBUS_PORT_ON_FLAG)
#define CLR_MHL_PORT_ON_FLAG()      (gMHLInfo.ucCbusIndex &= ~CBUS_PORT_ON_FLAG)

#define GET_MHL_HPD_SET_FLAG()      (Bool)(gMHLInfo.ucCbusIndex & CBUS_HPD_SET_FLAG)
#define SET_MHL_HPD_SET_FLAG()      (gMHLInfo.ucCbusIndex |= CBUS_HPD_SET_FLAG)
#define CLR_MHL_HPD_SET_FLAG()      (gMHLInfo.ucCbusIndex &= ~CBUS_HPD_SET_FLAG)

#define GET_MHL_DCAP_RDY_FLAG()     (Bool)(gMHLInfo.ucCbusIndex & CBUS_DCAP_RDY_FLAG)
#define SET_MHL_DCAP_RDY_FLAG()     (gMHLInfo.ucCbusIndex |= CBUS_DCAP_RDY_FLAG)
#define CLR_MHL_DCAP_RDY_FLAG()     (gMHLInfo.ucCbusIndex &= ~CBUS_DCAP_RDY_FLAG)

#define GET_MHL_DCAP_CHG_FLAG()     (Bool)(gMHLInfo.ucCbusIndex & CBUS_DCAP_CHG_FLAG)
#define SET_MHL_DCAP_CHG_FLAG()     (gMHLInfo.ucCbusIndex |= CBUS_DCAP_CHG_FLAG)
#define CLR_MHL_DCAP_CHG_FLAG()     (gMHLInfo.ucCbusIndex &= ~CBUS_DCAP_CHG_FLAG)

#define GET_MHL_PATH_EN_FLAG()      (Bool)(gMHLInfo.ucCbusIndex & CBUS_PATH_EN_FLAG)
#define SET_MHL_PATH_EN_FLAG()      (gMHLInfo.ucCbusIndex |= CBUS_PATH_EN_FLAG)
#define CLR_MHL_PATH_EN_FLAG()      (gMHLInfo.ucCbusIndex &= ~CBUS_PATH_EN_FLAG)

#define GET_MHL_TIME_OUT_FLAG()     (Bool)(gMHLInfo.ucCbusIndex & CBUS_TIME_OUT_FLAG)
#define SET_MHL_TIME_OUT_FLAG()     (gMHLInfo.ucCbusIndex |= CBUS_TIME_OUT_FLAG)
#define CLR_MHL_TIME_OUT_FLAG()     (gMHLInfo.ucCbusIndex &= ~CBUS_TIME_OUT_FLAG)

#define GET_CBUS_LINK_FLAG()        (gMHLInfo.ucCbusIndex & MHL_CBUS_LINK_MASK)
#define CLR_CBUS_LINK_FLAG()        (gMHLInfo.ucCbusIndex &= ~MHL_CBUS_LINK_MASK)
#define CLR_CBUS_CABLE_FLAG()       (gMHLInfo.ucCbusIndex &= CBUS_PORT_ON_FLAG)

#define GET_CBUS_LINK_CHECK()       (gMHLInfo.ucCbusControl & MHL_CBUS_LINK_MASK)
#define SET_CBUS_LINK_CHECK(a)      (gMHLInfo.ucCbusControl = a)
#define CLR_CBUS_LINK_CHECK()       (gMHLInfo.ucCbusControl &= ~MHL_CBUS_LINK_MASK)

#define GET_CBUS_LINK_FINISH()      (((GET_CBUS_LINK_CHECK() ^MHL_CBUS_LINK_MASK) > 0) ?FALSE :TRUE)

#define GET_MHL_CBUS_STATE()        (gMHLInfo.MhlCbusInfo.MhlCbusState)
#define SET_MHL_CBUS_STATE(a)       (gMHLInfo.MhlCbusInfo.MhlCbusState = a)

#define GET_MHL_CBUS_QUEUE(a)       (gMHLInfo.MhlCbusInfo.request[a])
#define GET_MHL_QUEUE_STATE(a)      (GET_MHL_CBUS_QUEUE(a).reqStatus)
#define SET_MHL_QUEUE_STATE(a ,b)   (GET_MHL_CBUS_QUEUE(a).reqStatus = b)

#define GET_MHL_DEBOUNCE_TIMER()    (bS2PatchCntFlag)//(g_u16S2PatchCnt)
#define SET_MHL_DEBOUNCE_TIMER(a)   ((a)?(bS2PatchCntFlag=0, g_u16S2PatchCnt = a, bS2PatchCntFlag=1):(bS2PatchCntFlag=0))//(g_u16S2PatchCnt = a)

#define GET_MHL_TIMEOUT_TIMER()     (bS2PatchCntFlag)//(g_u16S2PatchCnt)
#define SET_MHL_TIMEOUT_TIMER(a)    ((a)?(bS2PatchCntFlag=0, g_u16S2PatchCnt = a *1010ul, bS2PatchCntFlag=1):(bS2PatchCntFlag=0))//(g_u16S2PatchCnt = a *1010)  // Add 1% more time
//#define DEC_MHL_TIMEOUT_TIMER()     (g_u16S2PatchCnt)

#define GET_MHL_ABORT_TIMER()       (bS2PatchCntFlag)//(g_u16S2PatchCnt)
#define SET_MHL_ABORT_TIMER(a)      ((a)?(bS2PatchCntFlag=0, g_u16S2PatchCnt = a *1010ul, bS2PatchCntFlag=1):(bS2PatchCntFlag=0))//(g_u16S2PatchCnt = a *1010)  // Add 1% more time
//#define DEC_MHL_ABORT_TIMER()       (g_u16S2PatchCnt)

#define GET_MHL_HANDLER_TIMER()     (bMHLHandlerCntFlag)//(g_u16MHLHandlerCnt)
#define SET_MHL_HANDLER_TIMER(a)    ((a)?(bMHLHandlerCntFlag=0, g_u16MHLHandlerCnt=a, bMHLHandlerCntFlag=1):(bMHLHandlerCntFlag=0))//(g_u16MHLHandlerCnt = a)
#define INC_MHL_HANDLER_TIMER()     (bMHLHandlerCntFlag=0, g_u16MHLHandlerCnt++, bMHLHandlerCntFlag=1)//(g_u16MHLHandlerCnt++)
#define DEC_MHL_HANDLER_TIMER()     (bMHLHandlerCntFlag=0, g_u16MHLHandlerCnt--, bMHLHandlerCntFlag=1)//(g_u16MHLHandlerCnt--)

#define GET_MHL_CBUS_ERROE_CODE()   (g_u8ErrorCode)
#define SET_MHL_CBUS_ERROE_CODE(a)  (g_u8ErrorCode = a)

//-------------------------------------------------------------------------------------------------
//  Function Prototype
//-------------------------------------------------------------------------------------------------

extern xdata WORD g_u16MHLHandlerCnt;
extern bit bMHLHandlerCntFlag;

Bool mapi_mhl_init(void);
void mapi_mhl_handler(void);
void mapi_mhl_CbusIsolate(void);
void mapi_mhl_CbusProcessMSCSubCmd (BYTE subcmd, BYTE subdata);
void mapi_mhl_PowerCtrl(BYTE pctrl);
#if 0//DMHL_CTS
void mapi_mhl_CbusSendRcp (BYTE kcode);
#endif

Bool mapi_mhl_WakeupDetect(void);
#endif //_MAPI_MHL_H_


