///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   Mdrv_mhl.h
/// @author MStar Semiconductor Inc.
/// @brief  MHL driver Function
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_MHL_H_
#define _MDRV_MHL_H_

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

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
} CbusState_T;

typedef enum
{
    CBUS_REQ_IDLE = 0,
    CBUS_REQ_SENT,              // Request has been sent
    CBUS_REQ_PENDING,           // Request is waiting to be sent
    CBUS_REQ_RECEIVED,          // Request data has been received
} CbusReqState_T;

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

enum
{
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
    BYTE msgData[MHL_CBUS_DATA_SIZE+1];    // Cbus message data
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
    DevIntReg_S MhlDevIntReg;
    DevStatusReg_S MhlDevStatusReg;
} CbusInfo_S;

typedef struct
{
    BYTE ucCbusIndex; // MHL Cbus status
    BYTE ucCbusControl;
    BYTE MhlPowerStatus; // MHL power status
    CbusInfo_S MhlCbusInfo;
} MHLInfo_S;


//-------------------------------------------------------------------------------------------------
//  Function Prototype
//-------------------------------------------------------------------------------------------------
extern xdata MHLInfo_S gMHLInfo;
extern WORD xdata g_u16S2PatchCnt;
extern bit bS2PatchCntFlag;

// System
Bool mdrv_mhl_init(BYTE *edid);
void mdrv_mhl_CbusIsolate(Bool bFlag);
//void mdrv_mhl_SetHPD(Bool bflag);
void mdrv_mhl_PowerCtrl(BYTE pctrl);
void mdrv_mhl_SetTMDSTxOnOff(Bool bflag);
// Signal detect
Bool mdrv_mhl_CableDetect(void);
Bool mdrv_mhl_CbusStatus(void);
Bool mdrv_mhl_CheckDEStable(void);
Bool mdrv_mhl_CbusConnectionCheck(void);
void mdrv_mhl_CDRModeMonitor(void);
// Packet process
#if 0//DMHL_CTS
Bool mdrv_mhl_CbusMscMsgSubCmdSend(BYTE subCmd, BYTE databuf);
#endif
Bool mdrv_mhl_CbusRcpReply(BYTE statusCode, BYTE keyCode);
Bool mdrv_mhl_CbusRapReply(BYTE statusCode);
// Handler
void mdrv_mhl_CbusIntCB(void);
void mdrv_mhl_CbusIntHandler(void);
Bool mdrv_mhl_CbusRxHandler(BYTE *ucCmd, BYTE *ucData);
BYTE mdrv_mhl_CbusTxHandler(void);

#if 1
Bool mdrv_mhl_WakeupDetect(void);
#endif

#endif //_MDRV_MHL_H_

