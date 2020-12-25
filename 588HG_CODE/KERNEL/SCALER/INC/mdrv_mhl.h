///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   Mdrv_mhl.h
/// @author MStar Semiconductor Inc.
/// @brief  MHL driver Function
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_MHL_H_
#define _MDRV_MHL_H_

//-------------------------------------------------------------------------------------------------
//  Define
//-------------------------------------------------------------------------------------------------
#define MHL_DRIVER_U01                  0
#define MHL_DRIVER_U02                  1
#define MHL_DRIVER_U03                  2
#define MHL_DRIVER_U04                  3

#define MHL_DRIVER_VERSION              MHL_DRIVER_U04

#define DMHL_CTS                        1 // MHL setting for CTS
#define DMHL_CBUS_HW_MODE               1
#define DMHL_CBUS_AUTO_EQ               0

#define MHL_RX_XRAM_SIZE_HALF           128
#define MHL_RX_XRAM_SIZE_TOTAL          (MHL_RX_XRAM_SIZE_HALF *2)
#define MHL_RX_XRAM_ADDR_FIRST          MHL_CBUS_SRAM_ADDRESS
#define MHL_RX_XRAM_ADDR_SECOND         (MHL_RX_XRAM_ADDR_FIRST +MHL_RX_XRAM_SIZE_HALF)

#define CBUS_COMMAND_TIMEOUT_SECS       2 // No response for 1 second means we hold off and move
#define CBUS_ABORT_PADING_SECS          2 // Allow bus to quieten down when ABORTs are received.

#define CBUS_SAR_VALTAGE_TARGET         20
#define CBUS_CABLE_DETECT_LEVEL         (CBUS_SAR_VALTAGE_TARGET *255 /33)

#define CBUS_CDR_CLOCK_TARGET           20
#define CBUS_CLOCK_DETECT_LEVEL         (CBUS_CDR_CLOCK_TARGET *64 /14.318)

#define CBUS_CDR_OFFSET_TARGET          0.5
#define CBUS_CLOCK_DETECT_OFFSET        (CBUS_CDR_OFFSET_TARGET *64 /14.318)

#define MHL_MSC_SCRATCHPAD_SIZE         16
#define MHL_MSC_SCRATCHPAD_OFFSET       0x40
#define MHL_CBUS_DATA_SIZE              (MHL_MSC_SCRATCHPAD_SIZE +2) // offset + maxi 16 packets
#define DMHL_CBUS_TX_FIFO               (MHL_MSC_SCRATCHPAD_SIZE +3) // Write_Burst(0x6C) + offset(0x40 ~ 0x7F) + maxi 16 packets + EOF(0x32)
#define MHL_CBUS_TX_QUEUE_NUM           2
#define MHL_CBUS_TOTAL_QUEUE_NUM        ((MHL_CBUS_TX_QUEUE_NUM > 0)?(MHL_CBUS_TX_QUEUE_NUM +1):2)
#define MHL_CBUS_RX_QUEUE_INDEX         (MHL_CBUS_TOTAL_QUEUE_NUM -1)

#define DMHL_CTS_HDCP_PATCH             0 // for CTS HDCP 2C-04
#define DMHL_LG_PRADA_PATCH             1 // for LG PRADA patch

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
typedef enum
{
    CBUS_STATE_IDLE = 0,    // Cbus idle
    CBUS_STATE_SENT,    // Cbus command sent
    CBUS_STATE_ACK,    // Cbus command has been acked
    CBUS_STATE_RECEIVED,    // Cbus message received
    CBUS_STATE_WAITING_ACK,    // Cbus waiting RCPK or PAPK
}CbusState_T;

typedef enum
{
    CBUS_REQ_IDLE = 0,
    CBUS_REQ_SENT,              // Request has been sent
    CBUS_REQ_PENDING,           // Request is waiting to be sent
    CBUS_REQ_RECEIVED,          // Request data has been received
}CbusReqState_T;

enum MHL_INPUT_PORT_TYPE
{
    MHL_INPUT_NONE = 0,
    MHL_INPUT_PORT0,
    MHL_INPUT_PORT1,
    MHL_INPUT_PORT2,
    MHL_INPUT_PORT_MASK,
};

enum CBUS_HEADER_TYPE
{
    DDC_HEADER = 0,
    VENDER_HEADER,
    MSC_HEADER,
};

enum CBUS_CONTROL_TYPE
{
    CBUS_DATA = 0,
    CBUS_CONTROL,
};

enum CBUS_RECEIVE_STATE_TYPE
{
    CBUS_RECEIVE_NORMAL = 0,
    CBUS_RECEIVE_TIMEOUT,
};

enum CBUS_CLOCK_MODE_TYPE
{
    CBUS_CLOCK_RESERVED = 0,
    CBUS_CLOCK_PACKET_PIXEL = 2,
    CBUS_CLOCK_NORMAL_24BIT,
};

enum CBUS_SEND_COMMAND_TYPE
{
    CBUS_COMMAND_CTS = 0,
    CBUS_COMMAND_RCP,
    CBUS_COMMAND_RAP,
    CBUS_COMMAND_WRITE_BURST,
};

enum
{
    MSC_NONE = 0x00,
    MSC_ACK = 0x33, // Command/Data byte acknowledged
    MSC_NACK = 0x34, // Command or Data byte not acknowledge
    MSC_ABORT = 0x35, // Transaction abort
    MSC_WRITE_STAT_OR_SET_INT = 0x60,  // Write one status or interrupt register
    MSC_READ_DEVCAP = 0x61, // Read one register
    MSC_GET_STATE = 0x62, // Read CBUS revision level from follower
    MSC_GET_VENDOR_ID = 0x63, // Read vendor ID value from follower.
    MSC_SET_HPD = 0x64, // Set Hot Plug Detect in follower
    MSC_CLR_HPD = 0x65, // Clear Hot Plug Detect in follower
    MSC_MSC_MSG = 0x68, // VS command to send RCP sub-commands
    MSC_GET_SC1_ERRORCODE = 0x69, // Get Vendor-Specific command error code.
    MSC_GET_DDC_ERRORCODE = 0x6A, // Get DDC channel command error code.
    MSC_GET_MSC_ERRORCODE = 0x6B, // Get MSC command error code.
    MSC_WRITE_BURST = 0x6C, // Write 1-16 bytes to responder’s scratchpad.
    MSC_GET_SC3_ERRORCODE = 0x6D, // Get channel 3 command error code.
    MSC_EOF = 0x32, // End of Frame
    MSC_RESERVED = 0xFF,
}; //MscCtrlPkt_T;

enum
{
    SI_HPD_ACTIVE               = 0x00,     // HPD HI, HDCP, EDID, RX Term enabled
    SI_HPD_INACTIVE,                        // HPD LOW, HDCP, RX Term disabled
    SI_HPD_ACTIVE_EX,                       // EDID, RX Term enabled
    SI_HPD_INACTIVE_EX,                     // HPD HI, HDCP, EDID, RX Term disabled
    SI_HPD_TOGGLE,                  // Special use for CBUS connect
};

enum
{
    MSC_MSG_MSGE = 0x02,    // MSG Error sub-command
    MSC_MSG_RCP = 0x10,    // RCP sub-command
    MSC_MSG_RCPK = 0x11,    // RCP Acknowledge sub-command
    MSC_MSG_RCPE = 0x12,    // RCP Error sub-command
    MSC_MSG_RAP = 0x20,    // Mode Change Warning sub-command
    MSC_MSG_RAPK = 0x21,     // MCW Acknowledge sub-command
}; //MscMsgSubCmd_T;

enum
{
    MSC_ERRORCODE_NO_ERROR = 0x00,    // Normal response, no error
    MSC_ERRORCODE_RETRY_EXCEEDED = 0x01,    // Re-try threshold exceeded
    MSC_ERRORCODE_PROTOCOL_ERROR = 0x02,    // Protocol error   
    MSC_ERRORCODE_PEER_TIMEOUT = 0x04,    // Peer timed out
    MSC_ERRORCODE_INVALID_OPCODE = 0x08,    // Invalid op code
    MSC_ERRORCODE_BAD_OFFSET = 0x10,    // Bad offset in command
    MSC_ERRORCODE_PEER_BUSY = 0x20,    // Peer is busy, can't process new request
};

enum
{
    MSC_MSGE_NO_ERROR = 0x00,     // No Error
    MSC_MSGE_INVALID_SUB_COMMAND_CODE = 0x01,     // The sub-command code is not valid
}; //MsceStatusCode_T;

enum
{
    MSC_RCP_NO_ERROR = 0x00,     // RCP No Error
    MSC_RCP_INEFFECTIVE_KEY_CODE = 0x01,     // The key code in the RCP sub-command is not recognized
    MSC_RCP_RESPONDER_BUSY = 0x02,     // RCP Response busy
}; //MscRcpStatusCode_T;

typedef enum
{
    MSC_RAP_POLL = 0x00,
    MSC_RAP_CONTENT_ON = 0x10,
    MSC_RAP_CONTENT_OFF = 0x11,
} MscRapActionCode_T;

enum
{
    MSC_RAP_NO_ERROR = 0x00,     // RAP No Error
    MSC_RAP_UNRECOGNIZED_ACTION_CODE = 0x01,     
    MSC_RAP_UNSUPPORTED_ACTION_CODE = 0x02,    
    MSC_RAP_RESPONDER_BUSY = 0x03,     
}; //MscRapStatusCode_T;

enum
{
    RCP_KEYID_SELECT = 0x00,
    RCP_KEYID_UP = 0x01,
    RCP_KEYID_DOWN = 0x02,
    RCP_KEYID_LEFT = 0x03,
    RCP_KEYID_RIGHT = 0x04,
    RCP_KEYID_RIGHT_UP = 0x05,
    RCP_KEYID_RIGHT_DOWN = 0x06,
    RCP_KEYID_LEFT_UP = 0x07,
    RCP_KEYID_LEFT_DOWN = 0x08,
    RCP_KEYID_ROOT_MENU = 0x09,
    RCP_KEYID_SETUP_MENU = 0x0A,
    RCP_KEYID_CONTENTS_MENU = 0x0B,
    RCP_KEYID_FAVORITE_MENU = 0x0C,
    RCP_KEYID_EXIT = 0x0D, 
    //0x0E - 0x1F are reserved
    RCP_KEYID_NUM_0 = 0x20,
    RCP_KEYID_NUM_1 = 0x21,
    RCP_KEYID_NUM_2 = 0x22,
    RCP_KEYID_NUM_3 = 0x23,
    RCP_KEYID_NUM_4 = 0x24,
    RCP_KEYID_NUM_5 = 0x25,
    RCP_KEYID_NUM_6 = 0x26,
    RCP_KEYID_NUM_7 = 0x27,
    RCP_KEYID_NUM_8 = 0x28,
    RCP_KEYID_NUM_9 = 0x29,
    RCP_KEYID_DOT = 0x2A,
    RCP_KEYID_ENTER = 0x2B,
    RCP_KEYID_CLEAR = 0x2C,
    //0x2D - 0x2F are reserved
    RCP_KEYID_CH_UP = 0x30,
    RCP_KEYID_CH_DOWN = 0x31,
    RCP_KEYID_PRE_CH = 0x32,
    RCP_KEYID_SOUND_SELECT = 0x33,
    RCP_KEYID_INPUT_SELECT = 0x34,
    RCP_KEYID_SHOW_INFO = 0x35,
    RCP_KEYID_HELP = 0x36,
    RCP_KEYID_PAGE_UP = 0x37,
    RCP_KEYID_PAGE_DOWN = 0x38,
    //0x39 - 0x40 are reserved
    RCP_KEYID_VOL_UP = 0x41,
    RCP_KEYID_VOL_DOWN = 0x42,
    RCP_KEYID_MUTE = 0x43,
    RCP_KEYID_PLAY = 0x44,
    RCP_KEYID_STOP = 0x45,
    RCP_KEYID_PAUSE = 0x46,
    RCP_KEYID_RECORD = 0x47,
    RCP_KEYID_REWIND = 0x48,
    RCP_KEYID_FAST_FWD = 0x49,
    RCP_KEYID_EJECT  = 0x4A,
    RCP_KEYID_FORWARD = 0x4B,
    RCP_KEYID_BACKWARD = 0x4C,
    //0x4D - 0x4F are reserved
    RCP_KEYID_ANGLE = 0x50,
    RCP_KEYID_SUBPICTURE = 0x51,
    //0x52 - 0x5F are reserved
    RCP_KEYID_PLAY_FUNC = 0x60,
    RCP_KEYID_PAUSE_PLAY_FUNC = 0x61,
    RCP_KEYID_RECORD_FUNC = 0x62,
    RCP_KEYID_PAUSE_REC_FUNC = 0x63,
    RCP_KEYID_STOP_FUNC = 0x64,
    RCP_KEYID_MUTE_FUNC = 0x65,
    RCP_KEYID_UN_MUTE_FUNC = 0x66,
    RCP_KEYID_TUNE_FUNC = 0x67,
    RCP_KEYID_MEDIA_FUNC = 0x68,
    //0x69 - 0x70 are reserved
    RCP_KEYID_F1 = 0x71,
    RCP_KEYID_F2 = 0x72,
    RCP_KEYID_F3 = 0x73,
    RCP_KEYID_F4 = 0x74,
    RCP_KEYID_F5 = 0x75,
    //0x76 - 0x7D are reserved
    RCP_KEYID_VENDOR_SPECIFIC = 0x7E,
    RCP_KEYID_RESERVED = 0x7F,
}; // MscRcpKcode_T;

typedef struct
{
    Bool bDCAP_CHG; // 0x20[0], device capability changed
    Bool bDSCR_CHG; // 0x20[1], device scratchpad changed
    Bool bREQ_WRT; // 0x20[2], Request-to-Write
    Bool bGRT_WRT; // 0x20[3], Grant-to-Write
    Bool bEDID_CHG; // 0x21[1], EDID changed  
} DevIntReg_S;

typedef struct
{
    BYTE CLK_MODE; // 0x31[2:0], MHL clock mode
} DevStatusReg_S;

typedef struct 
{
    BYTE msgData[MHL_CBUS_DATA_SIZE +1];    // Cbus message data
    BYTE cmd;    // MSC command
    BYTE len;    // message length
    BYTE rcState;    // received state
    CbusReqState_T reqStatus;    // Cbus request status
} CbusReq_S;

typedef struct
{
    CbusReq_S request[MHL_CBUS_TOTAL_QUEUE_NUM];
    DWORD reqWrtTimer;    // Timer for RAP command response
    CbusState_T MhlCbusState;    // MHL Cbus state
    BYTE curIdx;    // current index.
    Bool reqWrt;    // request to write peer's scratchpad regs
    Bool bIsCmdinDataField;    // Is command in data field
    //DevIntReg_S MhlDevIntReg;
    //DevStatusReg_S MhlDevStatusReg;
} CbusInfo_S;

typedef struct
{
    BYTE ucCbusIndex; // MHL Cbus status
    BYTE ucCbusControl;
    BYTE ucErrorCode;
    BYTE MhlPowerStatus; // MHL power status
    CbusInfo_S MhlCbusInfo;
} MHLInfo_S;

typedef struct  
{
    BYTE lens;
    WORD databuf[DMHL_CBUS_TX_FIFO];
} mhalCbusFifo_S;

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

    CBUS_RX_SRAM_FLAG = BIT4,
};

enum VBUS_FLAG_TYPE
{
    VBUS_CHARGE_PORT0_FLAG = BIT5,
    VBUS_CHARGE_PORT1_FLAG = BIT6,
    VBUS_CHARGE_PORT2_FLAG = BIT7,
};

#define MHL_CBUS_LINK_MASK          (CBUS_HPD_SET_FLAG |CBUS_PATH_EN_FLAG |CBUS_DCAP_RDY_FLAG)
#define MHL_VBUS_CHARGE_MASK        (VBUS_CHARGE_PORT0_FLAG |VBUS_CHARGE_PORT1_FLAG |VBUS_CHARGE_PORT2_FLAG)

//-------------------------------------------------------------------------------------------------
//  Macro define
//-------------------------------------------------------------------------------------------------
#define GET_MHL_CABLE_PLUGGED()     (Bool)((gMHLInfo.ucCbusIndex & CBUS_CABLE_PLUG_FLAG) ?TRUE :FALSE)
#define SET_MHL_CABLE_PLUGGED()     (gMHLInfo.ucCbusIndex |= CBUS_CABLE_PLUG_FLAG)
#define CLR_MHL_CABLE_PLUGGED()     (gMHLInfo.ucCbusIndex &= ~CBUS_CABLE_PLUG_FLAG)

#define GET_MHL_CBUS_CONNECT()      (Bool)((gMHLInfo.ucCbusIndex & CBUS_CONNECT_FLAG) ?TRUE :FALSE)
#define SET_MHL_CBUS_CONNECT()      (gMHLInfo.ucCbusIndex |= CBUS_CONNECT_FLAG)
#define CLR_MHL_CBUS_CONNECT()      (gMHLInfo.ucCbusIndex &= ~CBUS_CONNECT_FLAG)

#define GET_MHL_PORT_ON_FLAG()      (Bool)((gMHLInfo.ucCbusIndex & CBUS_PORT_ON_FLAG) ?TRUE :FALSE)
#define SET_MHL_PORT_ON_FLAG()      (gMHLInfo.ucCbusIndex |= CBUS_PORT_ON_FLAG)
#define CLR_MHL_PORT_ON_FLAG()      (gMHLInfo.ucCbusIndex &= ~CBUS_PORT_ON_FLAG)

#define GET_MHL_HPD_SET_FLAG()      (Bool)((gMHLInfo.ucCbusIndex & CBUS_HPD_SET_FLAG) ?TRUE :FALSE)
#define SET_MHL_HPD_SET_FLAG()      (gMHLInfo.ucCbusIndex |= CBUS_HPD_SET_FLAG)
#define CLR_MHL_HPD_SET_FLAG()      (gMHLInfo.ucCbusIndex &= ~CBUS_HPD_SET_FLAG)

#define GET_MHL_DCAP_RDY_FLAG()     (Bool)((gMHLInfo.ucCbusIndex & CBUS_DCAP_RDY_FLAG) ?TRUE :FALSE)
#define SET_MHL_DCAP_RDY_FLAG()     (gMHLInfo.ucCbusIndex |= CBUS_DCAP_RDY_FLAG)
#define CLR_MHL_DCAP_RDY_FLAG()     (gMHLInfo.ucCbusIndex &= ~CBUS_DCAP_RDY_FLAG)

#define GET_MHL_DCAP_CHG_FLAG()     (Bool)((gMHLInfo.ucCbusIndex & CBUS_DCAP_CHG_FLAG) ?TRUE :FALSE)
#define SET_MHL_DCAP_CHG_FLAG()     (gMHLInfo.ucCbusIndex |= CBUS_DCAP_CHG_FLAG)
#define CLR_MHL_DCAP_CHG_FLAG()     (gMHLInfo.ucCbusIndex &= ~CBUS_DCAP_CHG_FLAG)

#define GET_MHL_PATH_EN_FLAG()      (Bool)((gMHLInfo.ucCbusIndex & CBUS_PATH_EN_FLAG) ?TRUE :FALSE)
#define SET_MHL_PATH_EN_FLAG()      (gMHLInfo.ucCbusIndex |= CBUS_PATH_EN_FLAG)
#define CLR_MHL_PATH_EN_FLAG()      (gMHLInfo.ucCbusIndex &= ~CBUS_PATH_EN_FLAG)

#define GET_MHL_TIME_OUT_FLAG()     (Bool)((gMHLInfo.ucCbusIndex & CBUS_TIME_OUT_FLAG) ?TRUE :FALSE)
#define SET_MHL_TIME_OUT_FLAG()     (gMHLInfo.ucCbusIndex |= CBUS_TIME_OUT_FLAG)
#define CLR_MHL_TIME_OUT_FLAG()     (gMHLInfo.ucCbusIndex &= ~CBUS_TIME_OUT_FLAG)

#define GET_CBUS_LINK_FLAG()        (gMHLInfo.ucCbusIndex & MHL_CBUS_LINK_MASK)
#define CLR_CBUS_LINK_FLAG()        (gMHLInfo.ucCbusIndex &= ~MHL_CBUS_LINK_MASK)
#define CLR_CBUS_CABLE_FLAG()       (gMHLInfo.ucCbusIndex &= CBUS_PORT_ON_FLAG)

#define GET_CBUS_LINK_CHECK()       (gMHLInfo.ucCbusControl & MHL_CBUS_LINK_MASK)
#define SET_CBUS_LINK_CHECK(a)      (gMHLInfo.ucCbusControl = (gMHLInfo.ucCbusControl &~MHL_CBUS_LINK_MASK) |(a & MHL_CBUS_LINK_MASK))
#define CLR_CBUS_LINK_CHECK()       (gMHLInfo.ucCbusControl &= ~MHL_CBUS_LINK_MASK)

#define GET_CBUS_LINK_FINISH()      (((GET_CBUS_LINK_CHECK() ^MHL_CBUS_LINK_MASK) > 0) ?FALSE :TRUE)

#define GET_MHL_RX_SRAM_FLAG()      (Bool)((gMHLInfo.ucCbusControl & CBUS_RX_SRAM_FLAG) ?TRUE :FALSE)
#define SET_MHL_RX_SRAM_FLAG()      (gMHLInfo.ucCbusControl |= CBUS_RX_SRAM_FLAG)
#define CLR_MHL_RX_SRAM_FLAG()      (gMHLInfo.ucCbusControl &= ~CBUS_RX_SRAM_FLAG)

#define GET_MHL_VBUS_PORT0_FLAG()   (Bool)((gMHLInfo.ucCbusControl & VBUS_CHARGE_PORT0_FLAG) ?TRUE :FALSE)
#define SET_MHL_VBUS_PORT0_FLAG()   (gMHLInfo.ucCbusControl |= VBUS_CHARGE_PORT0_FLAG)
#define CLR_MHL_VBUS_PORT0_FLAG()   (gMHLInfo.ucCbusControl &= ~VBUS_CHARGE_PORT0_FLAG)

#define GET_MHL_VBUS_PORT1_FLAG()   (Bool)((gMHLInfo.ucCbusControl & VBUS_CHARGE_PORT1_FLAG) ?TRUE :FALSE)
#define SET_MHL_VBUS_PORT1_FLAG()   (gMHLInfo.ucCbusControl |= VBUS_CHARGE_PORT1_FLAG)
#define CLR_MHL_VBUS_PORT1_FLAG()   (gMHLInfo.ucCbusControl &= ~VBUS_CHARGE_PORT1_FLAG)

#define GET_MHL_VBUS_PORT2_FLAG()   (Bool)((gMHLInfo.ucCbusControl & VBUS_CHARGE_PORT2_FLAG) ?TRUE :FALSE)
#define SET_MHL_VBUS_PORT2_FLAG()   (gMHLInfo.ucCbusControl |= VBUS_CHARGE_PORT2_FLAG)
#define CLR_MHL_VBUS_PORT2_FLAG()   (gMHLInfo.ucCbusControl &= ~VBUS_CHARGE_PORT2_FLAG)

#define CLR_MHL_VBUS_CHARGR()       (gMHLInfo.ucCbusControl &= ~MHL_VBUS_CHARGE_MASK)

#define GET_MHL_CBUS_STATE()        (gMHLInfo.MhlCbusInfo.MhlCbusState)
#define SET_MHL_CBUS_STATE(a)       (gMHLInfo.MhlCbusInfo.MhlCbusState = a)

#define GET_MHL_CBUS_QUEUE(a)       (gMHLInfo.MhlCbusInfo.request[a])
#define GET_MHL_QUEUE_STATE(a)      (GET_MHL_CBUS_QUEUE(a).reqStatus)
#define SET_MHL_QUEUE_STATE(a ,b)   (GET_MHL_CBUS_QUEUE(a).reqStatus = b)

#define GET_MHL_TX_QUEUE_INDEX()    (gMHLInfo.MhlCbusInfo.curIdx)
#define SET_MHL_TX_QUEUE_INDEX(a)   (gMHLInfo.MhlCbusInfo.curIdx = a)

#define GET_MHL_CBUS_ERROE_CODE()   (gMHLInfo.ucErrorCode)
#define SET_MHL_CBUS_ERROE_CODE(a)  (gMHLInfo.ucErrorCode = a)

#define GET_MHL_POWER_STSTUS()      (gMHLInfo.MhlPowerStatus &0x0F)
#define SET_MHL_POWER_STSTUS(a)     (gMHLInfo.MhlPowerStatus = gMHLInfo.MhlPowerStatus &(~0x0F) |(a &0x0F))

#define GET_MHL_PORT_SELECT()       ((gMHLInfo.MhlPowerStatus &0xF0) >>4)
#define SET_MHL_PORT_SELECT(a)      (gMHLInfo.MhlPowerStatus = gMHLInfo.MhlPowerStatus &(~0xF0) |((a <<4) &0xF0))

#define GET_MHL_DEBOUNCE_TIMER()    (bCbusTimerFlag)
#define SET_MHL_DEBOUNCE_TIMER(a)   (bCbusTimerFlag = FALSE, usCbusTimerCnt = a, bCbusTimerFlag = TRUE)

#define GET_MHL_TIMEOUT_TIMER()     (bCbusTimerFlag)
#define SET_MHL_TIMEOUT_TIMER(a)    (bCbusTimerFlag = FALSE, usCbusTimerCnt = a *1010, bCbusTimerFlag = TRUE)  // Add 1% more time

#define GET_MHL_ABORT_TIMER()       (bCbusTimerFlag)
#define SET_MHL_ABORT_TIMER(a)      (bCbusTimerFlag = FALSE, usCbusTimerCnt = a *1010, bCbusTimerFlag = TRUE)  // Add 1% more time

#define GET_MHL_RX_SRAM_QUEUE(a)    (ucMHLRxSRAM[a]) 

//-------------------------------------------------------------------------------------------------
//  Function Prototype
//-------------------------------------------------------------------------------------------------
extern xdata MHLInfo_S gMHLInfo;
extern idata bCbusTimerFlag;
extern WORD idata usCbusTimerCnt;
extern BYTE xdata ucMHLRxSRAM[];

// Signal detect
void mdrv_mhl_Initial(BYTE *pEdid);
void mdrv_mhl_InputSelect(BYTE ucPort);
void mdrv_mhl_CbusIsolate(Bool bFlag);
Bool mdrv_mhl_CableDetect(void);
Bool mdrv_mhl_CbusStatus(void);
void mdrv_mhl_CDRModeMonitor(void);
Bool mdrv_mhl_CheckDEStable(void);
// Rx function
Bool mdrv_mhl_CbusRcpReply(BYTE ucStatusCode, BYTE ucKeyCode);
Bool mdrv_mhl_CbusRapReply(BYTE ucStatusCode);
// Tx function
Bool mdrv_mhl_CbusMscMsgSubCmdSend(BYTE ucSubCmd, BYTE ucData);
Bool mdrv_mhl_CbusWriteBurst(BYTE ucAddr, BYTE ucLength, BYTE *pBuffer);
void mdrv_mhl_CbusSendCommand(BYTE ucSelect);
// Handler
void mdrv_mhl_CbusIntCB(void);
void mdrv_mhl_CbusIntHandler(void);
Bool mdrv_mhl_CbusRxHandler(BYTE *ucCmd, BYTE *ucData);
void mdrv_mhl_CbusTxHandler(void);
void mdrv_mhl_CbusTimerHandler(void);
void mdrv_mhl_CbusConnectionCheck(void);
// System
//void mdrv_mhl_SetHPD(Bool bFlag);
void mdrv_mhl_PowerControl(BYTE ucState);
void mdrv_mhl_SetTMDSTxOnOff(Bool bFlag);

#ifdef _MS_PM_
Bool mdrv_mhl_WakeupDetect(void);
#endif

#endif //_MDRV_MHL_H_

