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
#define MHL_DRIVER_U05                  4
#define MHL_DRIVER_U06                  5
#define MHL_DRIVER_U07                  6

#define MHL_DRIVER_VERSION              MHL_DRIVER_U01

#define MHL_CBUS_SW_POLLING_MODE        0   // Use SW polling to reply MSC command
#define MHL_CBUS_SW_ISR_MODE            1   // Use SW interrupt to reply MSC command, No function
#define MHL_CBUS_HW_ISR_MODE            2   // Use SW interrupt to clear MSC receiver flag to trigger HW auto reply, No function 
#define MHL_CBUS_HW_REPLY_MODE          3   // Use xdata to saving MSC command to trigger HW auto reply

#define MHL_CBUS_OPERATION_MODE         MHL_CBUS_HW_REPLY_MODE

#define DMHL_CTS                        1   // MHL setting for CTS
#define MHL_CBUS_USE_INTERRUPT          1   

#define MHL_RX_XRAM_SIZE_HALF           128
#define MHL_RX_XRAM_SIZE_TOTAL          (MHL_RX_XRAM_SIZE_HALF *2)
#define MHL_RX_XRAM_ADDR_FIRST          MHL_CBUS_SRAM_ADDRESS
#define MHL_RX_XRAM_ADDR_SECOND         (MHL_RX_XRAM_ADDR_FIRST +MHL_RX_XRAM_SIZE_HALF)

#define CBUS_COMMAND_TIMEOUT_SECS       2 // No response for 1 second means we hold off and move
#define CBUS_ABORT_PADING_SECS          2 // Allow bus to quieten down when ABORTs are received.

#define MHL_MSC_SCRATCHPAD_SIZE         16
#define MHL_MSC_SCRATCHPAD_OFFSET       0x40
#define MHL_CBUS_DATA_SIZE              (MHL_MSC_SCRATCHPAD_SIZE +2) // offset + maxi 16 packets
#define MHL_CBUS_TX_FIFO                (MHL_MSC_SCRATCHPAD_SIZE +3) // Write_Burst(0x6C) + offset(0x40 ~ 0x7F) + maxi 16 packets + EOF(0x32)
#define MHL_CBUS_TX_QUEUE_NUM           2
#define MHL_CBUS_TOTAL_QUEUE_NUM        ((MHL_CBUS_TX_QUEUE_NUM > 0)?(MHL_CBUS_TX_QUEUE_NUM +1):2)
#define MHL_CBUS_RX_QUEUE_INDEX         (MHL_CBUS_TOTAL_QUEUE_NUM -1)

#define MHL_AOUT_SCAN_PORT_INTERVAL     250 // 10ms one unit
#define MHL_PM_WAKEUP_CHECK_INTERVAL    250//100 // 10ms one unit
#define MHL_PM_CBUS_NONE_INTERVAL       2 // 10ms one unit
#define MHL_VBUS_TOGGLE_INTERVAL        50 // 10ms one unit
#define MHL_HPD_CONTROL_INTERVAL        30 // 10ms one unit

#define DMHL_CTS_HDCP_PATCH             0 // for CTS HDCP 2C-04
#define DMHL_LG_PRADA_PATCH             1 // for LG PRADA patch

#define MHL_EDID_BLOCK_LENGTH           128
#define MHL_EDID_DTD_LENGTH             18
#define MHL_EDID_FIRST_DTD_START        0x36
#define MHL_EDID_SECOND_DTD_START       0x48
#define MHL_EDID_EXTENSION_FLAG         0x7E  
#define MHL_EDID_RESERVED_DATA_BLOCK    0x82

#define MHL_3D_VIC_ID                   0x0010
#define MHL_3D_DTD_ID                   0x0011

#define MHL_3D_SEQUENCE_LENGTH          5
#define MHL_3D_DTD_INFORMATION_LENGTH   8
#define MHL_3D_VIC_INFORMATION_LENGTH   16

#define MHL_ADOPTER_ID_ASUS             0x052C
#define MHL_DEVICE_ID_PADFONE2          0x8240

#define MHL_ADOPTER_ID_HTC              0x016F
#define MHL_DEVICE_ID_BUTTERFLY         0x0000

#define MHL_ADOPTER_ID_SONY             0x03A7
#define MHL_DEVICE_ID_XPERIA            0x0401

#define MHL_ADOPTER_ID_HUAWEI           0x0142
#define MHL_DEVICE_ID_U9200             0x9244

#define MHL_ADOPTER_ID_LG               0x0000
#define MHL_DEVICE_ID_LG_PRADA          0x0000

#define MHL_ADOPTER_ID_SAMSUNG          0x0141

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

enum MHL_CBUS_HEADER_TYPE
{
    DDC_HEADER = 0,
    VENDER_HEADER,
    MSC_HEADER,
};

enum MHL_CBUS_CONTROL_TYPE
{
    CBUS_DATA = 0,
    CBUS_CONTROL,
};

enum MHL_CBUS_RECEIVE_STATE_TYPE
{
    CBUS_RECEIVE_NORMAL = 0,
    CBUS_RECEIVE_TIMEOUT,
};

enum MHL_CBUS_CLOCK_MODE_TYPE
{
    CBUS_CLOCK_RESERVED = 0,
    CBUS_CLOCK_PACKET_PIXEL = 2,
    CBUS_CLOCK_NORMAL_24BIT,
};

enum MHL_CBUS_SEND_COMMAND_TYPE
{
    CBUS_COMMAND_CTS = 0,
    CBUS_COMMAND_RCP,
    CBUS_COMMAND_RAP,
    CBUS_COMMAND_UCP,
    CBUS_COMMAND_WRITE_BURST,
};

enum MHL_CBUS_WRITE_BURST_STATE_TYPE
{
    CBUS_WRITE_BURST_NONE = 0,
    CBUS_WRITE_BURST_START,
    CBUS_WRITE_BURST_SEND_REQUEST,
    CBUS_WRITE_BURST_REVEIVE_GRANT,
    CBUS_WRITE_BURST_SEND_DATA,
    CBUS_WRITE_BURST_END,
};

enum MHL_3D_SUPPORT_TYPE
{
    MHL_3D_FS_SUPPORT = BIT0,
    MHL_3D_TB_SUPPORT = BIT1,
    MHL_3D_LR_SUPPORT = BIT2,
};

enum MHL_3D_DTD_NUMBER_TYPE
{
    MHL_3D_FIRST_DTD = 0,
    MHL_3D_SECOND_DTD,
    MHL_3D_THIRD_DTD,
};

enum MHL_3D_DTD_STEREO_TYPE
{
    MHL_3D_DTD_STEREO_NORMAL_DISPLAY = 0,
    MHL_3D_DTD_STEREO_FIELD_SEQUENTIAL_RIGHT = 2,
    MHL_3D_DTD_STEREO_FIELD_SEQUENTIAL_LEFT = 4,
    MHL_3D_DTD_STEREO_2WAY_INTERLEAVED_RIGHT = 3,
    MHL_3D_DTD_STEREO_2WAY_INTERLEAVED_LEFT = 5,
    MHL_3D_DTD_STEREO_4WAY_INTERLEAVED = 6,
    MHL_3D_DTD_STEREO_SIDE_BY_SIDE_INTERLEAVED = 7,
};

enum MHL_EDID_DATA_BLOCK_CODE_TYPE
{
    MHL_EDID_AUDIO_DATA_BLOCK_CODE = 1,
    MHL_EDID_VIDEO_DATA_BLOCK_CODE = 2,
    MHL_EDID_VENDER_SPECIFIC_DATA_BLOCK_CODE = 3,
    MHL_EDID_SPEAKER_ALLOCATION_DATA_BLOCK_CODE = 4,
    MHL_EDID_VESA_DTC_DATA_BLOCK_CODE = 5,
    MHL_EDID_USE_EXTENDED_TAG_BLOCK_CODE = 7,
};

enum MHL_VIC_PARSING_STATE_TYPE
{
    MHL_VIC_PARSING_START = 0,
    MHL_VIC_PARSING_CHECK_HDMI_VIDEO_PRESENT,
    MHL_VIC_PARSING_CHECK_3D_PRESENT,
    MHL_VIC_PARSING_CHECK_HDMI_VIC,
    MHL_VIC_PARSING_3D_MULTI_PRESENT_00,
    MHL_VIC_PARSING_3D_MULTI_PRESENT_01,
    MHL_VIC_PARSING_3D_MULTI_PRESENT_10,
    MHL_VIC_PARSING_CHECK_3D_STRUCTURE,
    MHL_VIC_PARSING_CHECK_3D_PRESENT_TIMING,
    MHL_VIC_PARSING_FINISH,
};

enum MHL_3D_PRESENT_TIMING_VIC_TYPE
{
    MHL_3D_PRESENT_VIC_1920x1080p24Hz = 32,
    MHL_3D_PRESENT_VIC_1280x720p60Hz = 4,
    MHL_3D_PRESENT_VIC_1920x1080i60Hz = 5,
    MHL_3D_PRESENT_VIC_1280x720p50Hz = 19,
    MHL_3D_PRESENT_VIC_1920x1080i50Hz = 20,
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
    MSC_MSG_UCP = 0x30,     // UCP sub-command
    MSC_MSG_UCPK = 0x31,    // UCP Acknowledge sub-command
    MSC_MSG_UCPE = 0x32,    // UCP Error sub-command
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

enum CBUS_DEVCAP_TYPE
{
    MHL_CBUS_DEV_STATE = 0,
    MHL_CBUS_VERSION,
    MHL_CBUS_DEV_CAT,
    MHL_CBUS_ADOPTER_ID_H,
    MHL_CBUS_ADOPTER_ID_L,
    MHL_CBUS_VID_LINK_MODE,
    MHL_CBUS_AUD_LINK_MODE,
    MHL_CBUS_VIDEO_TYPE,
    MHL_CBUS_LOG_DEV_MAP,
    MHL_CBUS_BANDWIDTH,
    MHL_CBUS_FEATURE_FLAG,
    MHL_CBUS_DEVICE_ID_H,
    MHL_CBUS_DEVICE_ID_L,
    MHL_CBUS_SCRATCHPAD_SIZE,
    MHL_CBUS_INT_STAT,
};

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
    //DWORD reqWrtTimer;    // Timer for RAP command response
    CbusState_T MhlCbusState;    // MHL Cbus state
    BYTE curIdx;    // current index.
    //Bool reqWrt;    // request to write peer's scratchpad regs
    Bool bIsCmdinDataField;    // Is command in data field
    //DevIntReg_S MhlDevIntReg;
    //DevStatusReg_S MhlDevStatusReg;
} CbusInfo_S;

typedef struct
{
    BYTE ucSystemIndex;
    BYTE ucCbusIndex; // MHL Cbus status
    BYTE ucCbusControl;
    BYTE ucErrorCode;
    BYTE ucReceiveIndex;
    BYTE ucSendIndex;
    BYTE ucExtendIndex;
    BYTE ucAskData;
    BYTE MhlPowerStatus; // MHL power status
    BYTE ucReceiveCounter;
    BYTE ucCellPhone;
    CbusInfo_S MhlCbusInfo;
} MHLInfo_S;

typedef struct  
{
    BYTE lens;
    WORD databuf[MHL_CBUS_TX_FIFO];
} mhalCbusFifo_S;

enum
{
    MHL_POWER_ON = 1,
    MHL_POWER_STANDBY,
    MHL_POWER_DOWN,
    MHL_POWER_NORMAL,
};

enum MHL_SYSTEM_FLAG_TYPE
{
    MHL_SYSTEM_CABLE_DETECT_FLAG = BIT0,
    MHL_SYSTEM_CBUS_CONNECT_FLAG = BIT1,
    MHL_SYSTEM_PLL_POWER_FLAG = BIT2,
    MHL_SYSTEM_PHY_SETTING_FLAG = BIT3,
    MHL_SYSTEM_GET_VENDER_ID_FLAG = BIT4,
    MHL_SYSTEM_POWER_SAVING_FLAG = BIT5,
    MHL_SYSTEM_CD_SENSE_ENABLE_FLAG = BIT6,
};

enum MHL_CBUS_FLAG_TYPE
{
    CBUS_HPD_SET_FLAG = BIT0,
    CBUS_DCAP_RDY_FLAG = BIT3,
    CBUS_DCAP_CHG_FLAG = BIT2,
    CBUS_PATH_EN_FLAG = BIT1,
    CBUS_TIME_OUT_FLAG = BIT4,
    CBUS_PORT_ON_FLAG = BIT5,
    CBUS_CONNECT_FLAG = BIT6,
    CBUS_CABLE_PLUG_FLAG = BIT7,

    CBUS_RX_SRAM_FLAG = BIT4,
    CBUS_PATH_SWITCH_FLAG = BIT7,
    CBUS_POWER_ON_FLAG = BIT3,
};

enum MHL_VBUS_FLAG_TYPE
{
    VBUS_CHARGE_PORT0_FLAG = BIT5,
    VBUS_CHARGE_PORT1_FLAG = BIT6,
    VBUS_CHARGE_PORT2_FLAG = BIT7,
};

enum MHL_RECEIVE_FLAG_TYPE
{
    CBUS_RECEIVE_MSC_FLAG = BIT0,
    CBUS_RECEIVE_ACK_FLAG = BIT1,
    CBUS_RECEIVE_ABORT_FLAG = BIT2,
    CBUS_RECEIVE_RCP_FLAG = BIT3,
    CBUS_RECEIVE_RAP_FLAG = BIT4,
    CBUS_RECEIVE_ERROR_FLAG = BIT5,
    CBUS_RECEIVE_DEVCAP_FLAG = BIT7,
};

enum MHL_SEND_FLAG_TYPE
{
    CBUS_SEND_RCP_ENABLE_FLAG = BIT0,
    CBUS_SEND_RAP_ENABLE_FLAG = BIT1,
    CBUS_SEND_MSG_FINISH_FLAG = BIT2,
    CBUS_SEND_HPD_CONTROL_FLAG = BIT3,
    CBUS_SEND_READ_FLAG = BIT6,
    CBUS_SEND_FINISH_FLAG = BIT7,
};

enum MHL_EXTEND_FLAG_TYPE
{
    CBUS_RECEIVE_3D_REQ_FLAG = BIT0,
    CBUS_SEND_3D_VIC_FLAG = BIT1,
    CBUS_SEND_3D_DTD_FLAG = BIT2,
    CBUS_HID_SEND_REQUEST_FLAG = BIT3,
    CBUS_HID_RECEIVE_REPLY_FLAG = BIT4,
};

enum MHL_CELL_PHONE_FLAG_TYPE
{
    CELL_PHONE_PAD_FONE2_FLAG = BIT0,
    CELL_PHONE_BUTTER_FLY_FLAG = BIT1,
    CELL_PHONE_SONY_XPERIA_FLAG = BIT2,
    CELL_PHONE_HUAWEI_PHONE_FLAG = BIT3,
    CELL_PHONE_SAMSUNG_PHONE_FLAG = BIT4,
};

enum MHL_CELL_PHONE_ID_TYPE
{
    CELL_PHONE_ID_NONE = 0,
    CELL_PHONE_ID_HTC_BUTTER_FLY,
    CELL_PHONE_ID_SONY_XPERIA,
    CELL_PHONE_ID_HUAWEI_PHONE,
    CELL_PHONE_ID_LG_PRADA,
    CELL_PHONE_ID_SAMSUNG_PHONE,
};

#define MHL_SYSTEM_INITIAL_MASK         (MHL_SYSTEM_PLL_POWER_FLAG)
#define MHL_CBUS_LINK_MASK              (CBUS_HPD_SET_FLAG |CBUS_PATH_EN_FLAG |CBUS_DCAP_RDY_FLAG)
#define MHL_VBUS_CHARGE_MASK            (VBUS_CHARGE_PORT0_FLAG |VBUS_CHARGE_PORT1_FLAG |VBUS_CHARGE_PORT2_FLAG)
#define MHL_3D_SUPPORT_MASK             (MHL_3D_FS_SUPPORT |MHL_3D_TB_SUPPORT |MHL_3D_LR_SUPPORT)

//-------------------------------------------------------------------------------------------------
//  Macro define
//-------------------------------------------------------------------------------------------------
#define GET_MHL_SYSTEM_CABLE_DETECT()   (Bool)((gMHLInfo.ucSystemIndex & MHL_SYSTEM_CABLE_DETECT_FLAG) ?TRUE :FALSE)
#define SET_MHL_SYSTEM_CABLE_DETECT()   (gMHLInfo.ucSystemIndex |= MHL_SYSTEM_CABLE_DETECT_FLAG)
#define CLR_MHL_SYSTEM_CABLE_DETECT()   (gMHLInfo.ucSystemIndex &= ~MHL_SYSTEM_CABLE_DETECT_FLAG)
#define SLT_MHL_SYSTEM_CABLE_DETECT(a)  (a ?SET_MHL_SYSTEM_CABLE_DETECT() :CLR_MHL_SYSTEM_CABLE_DETECT())

#define GET_MHL_SYSTEM_CBUS_CONNECT()   (Bool)((gMHLInfo.ucSystemIndex & MHL_SYSTEM_CBUS_CONNECT_FLAG) ?TRUE :FALSE)
#define SET_MHL_SYSTEM_CBUS_CONNECT()   (gMHLInfo.ucSystemIndex |= MHL_SYSTEM_CBUS_CONNECT_FLAG)
#define CLR_MHL_SYSTEM_CBUS_CONNECT()   (gMHLInfo.ucSystemIndex &= ~MHL_SYSTEM_CBUS_CONNECT_FLAG)
#define SLT_MHL_SYSTEM_CBUS_CONNECT(a)  (a ?SET_MHL_SYSTEM_CBUS_CONNECT() :CLR_MHL_SYSTEM_CBUS_CONNECT())

#define GET_MHL_SYSTEM_PLL_POWER()      (Bool)((gMHLInfo.ucSystemIndex & MHL_SYSTEM_PLL_POWER_FLAG) ?TRUE :FALSE)
#define SET_MHL_SYSTEM_PLL_POWER()      (gMHLInfo.ucSystemIndex |= MHL_SYSTEM_PLL_POWER_FLAG)
#define CLR_MHL_SYSTEM_PLL_POWER()      (gMHLInfo.ucSystemIndex &= ~MHL_SYSTEM_PLL_POWER_FLAG)

#define GET_MHL_SYSTEM_PHY_SETTING()    (Bool)((gMHLInfo.ucSystemIndex & MHL_SYSTEM_PHY_SETTING_FLAG) ?TRUE :FALSE)
#define SET_MHL_SYSTEM_PHY_SETTING()    (gMHLInfo.ucSystemIndex |= MHL_SYSTEM_PHY_SETTING_FLAG)
#define CLR_MHL_SYSTEM_PHY_SETTING()    (gMHLInfo.ucSystemIndex &= ~MHL_SYSTEM_PHY_SETTING_FLAG)

#define GET_MHL_SYSTEM_GET_VENDER_ID()  (Bool)((gMHLInfo.ucSystemIndex & MHL_SYSTEM_GET_VENDER_ID_FLAG) ?TRUE :FALSE)
#define SET_MHL_SYSTEM_GET_VENDER_ID()  (gMHLInfo.ucSystemIndex |= MHL_SYSTEM_GET_VENDER_ID_FLAG)
#define CLR_MHL_SYSTEM_GET_VENDER_ID()  (gMHLInfo.ucSystemIndex &= ~MHL_SYSTEM_GET_VENDER_ID_FLAG)

#define GET_MHL_SYSTEM_POWER_SAVING()   (Bool)((gMHLInfo.ucSystemIndex & MHL_SYSTEM_POWER_SAVING_FLAG) ?TRUE :FALSE)
#define SET_MHL_SYSTEM_POWER_SAVING()   (gMHLInfo.ucSystemIndex |= MHL_SYSTEM_POWER_SAVING_FLAG)
#define CLR_MHL_SYSTEM_POWER_SAVING()   (gMHLInfo.ucSystemIndex &= ~MHL_SYSTEM_POWER_SAVING_FLAG)

#define GET_MHL_SYSTEM_CD_ENABLE()      (Bool)((gMHLInfo.ucSystemIndex & MHL_SYSTEM_CD_SENSE_ENABLE_FLAG) ?TRUE :FALSE)
#define SET_MHL_SYSTEM_CD_ENABLE()      (gMHLInfo.ucSystemIndex |= MHL_SYSTEM_CD_SENSE_ENABLE_FLAG)
#define CLR_MHL_SYSTEM_CD_ENABLE()      (gMHLInfo.ucSystemIndex &= ~MHL_SYSTEM_CD_SENSE_ENABLE_FLAG)

#define SET_MHL_SYSTEM_FLAG(a)          (gMHLInfo.ucSystemIndex = a)

#define GET_MHL_CABLE_PLUGGED()         (Bool)((gMHLInfo.ucCbusIndex & CBUS_CABLE_PLUG_FLAG) ?TRUE :FALSE)
#define SET_MHL_CABLE_PLUGGED()         (gMHLInfo.ucCbusIndex |= CBUS_CABLE_PLUG_FLAG)
#define CLR_MHL_CABLE_PLUGGED()         (gMHLInfo.ucCbusIndex &= ~CBUS_CABLE_PLUG_FLAG)

#define GET_MHL_CBUS_CONNECT()          (Bool)((gMHLInfo.ucCbusIndex & CBUS_CONNECT_FLAG) ?TRUE :FALSE)
#define SET_MHL_CBUS_CONNECT()          (gMHLInfo.ucCbusIndex |= CBUS_CONNECT_FLAG)
#define CLR_MHL_CBUS_CONNECT()          (gMHLInfo.ucCbusIndex &= ~CBUS_CONNECT_FLAG)

#define GET_MHL_PORT_ON_FLAG()          (Bool)((gMHLInfo.ucCbusIndex & CBUS_PORT_ON_FLAG) ?TRUE :FALSE)
#define SET_MHL_PORT_ON_FLAG()          (gMHLInfo.ucCbusIndex |= CBUS_PORT_ON_FLAG)
#define CLR_MHL_PORT_ON_FLAG()          (gMHLInfo.ucCbusIndex &= ~CBUS_PORT_ON_FLAG)

#define GET_MHL_HPD_SET_FLAG()          (Bool)((gMHLInfo.ucCbusIndex & CBUS_HPD_SET_FLAG) ?TRUE :FALSE)
#define SET_MHL_HPD_SET_FLAG()          (gMHLInfo.ucCbusIndex |= CBUS_HPD_SET_FLAG)
#define CLR_MHL_HPD_SET_FLAG()          (gMHLInfo.ucCbusIndex &= ~CBUS_HPD_SET_FLAG)

#define GET_MHL_DCAP_RDY_FLAG()         (Bool)((gMHLInfo.ucCbusIndex & CBUS_DCAP_RDY_FLAG) ?TRUE :FALSE)
#define SET_MHL_DCAP_RDY_FLAG()         (gMHLInfo.ucCbusIndex |= CBUS_DCAP_RDY_FLAG)
#define CLR_MHL_DCAP_RDY_FLAG()         (gMHLInfo.ucCbusIndex &= ~CBUS_DCAP_RDY_FLAG)

#define GET_MHL_DCAP_CHG_FLAG()         (Bool)((gMHLInfo.ucCbusIndex & CBUS_DCAP_CHG_FLAG) ?TRUE :FALSE)
#define SET_MHL_DCAP_CHG_FLAG()         (gMHLInfo.ucCbusIndex |= CBUS_DCAP_CHG_FLAG)
#define CLR_MHL_DCAP_CHG_FLAG()         (gMHLInfo.ucCbusIndex &= ~CBUS_DCAP_CHG_FLAG)

#define GET_MHL_PATH_EN_FLAG()          (Bool)((gMHLInfo.ucCbusIndex & CBUS_PATH_EN_FLAG) ?TRUE :FALSE)
#define SET_MHL_PATH_EN_FLAG()          (gMHLInfo.ucCbusIndex |= CBUS_PATH_EN_FLAG)
#define CLR_MHL_PATH_EN_FLAG()          (gMHLInfo.ucCbusIndex &= ~CBUS_PATH_EN_FLAG)

#define GET_MHL_TIME_OUT_FLAG()         (Bool)((gMHLInfo.ucCbusIndex & CBUS_TIME_OUT_FLAG) ?TRUE :FALSE)
#define SET_MHL_TIME_OUT_FLAG()         (gMHLInfo.ucCbusIndex |= CBUS_TIME_OUT_FLAG)
#define CLR_MHL_TIME_OUT_FLAG()         (gMHLInfo.ucCbusIndex &= ~CBUS_TIME_OUT_FLAG)

#define GET_CBUS_LINK_FLAG()            (gMHLInfo.ucCbusIndex & MHL_CBUS_LINK_MASK)
#define CLR_CBUS_LINK_FLAG()            (gMHLInfo.ucCbusIndex &= ~MHL_CBUS_LINK_MASK)
#define CLR_CBUS_CABLE_FLAG()           (gMHLInfo.ucCbusIndex &= CBUS_PORT_ON_FLAG)

#define GET_CBUS_LINK_CHECK()           (gMHLInfo.ucCbusControl & MHL_CBUS_LINK_MASK)
#define SET_CBUS_LINK_CHECK(a)          (gMHLInfo.ucCbusControl = MASKSET(gMHLInfo.ucCbusControl, a, MHL_CBUS_LINK_MASK))
#define CLR_CBUS_LINK_CHECK()           (gMHLInfo.ucCbusControl &= ~MHL_CBUS_LINK_MASK)

#define GET_CBUS_LINK_FINISH()          (((GET_CBUS_LINK_CHECK() ^MHL_CBUS_LINK_MASK) > 0) ?FALSE :TRUE)

#define GET_MHL_RX_SRAM_FLAG()          (Bool)((gMHLInfo.ucCbusControl & CBUS_RX_SRAM_FLAG) ?TRUE :FALSE)
#define SET_MHL_RX_SRAM_FLAG()          (gMHLInfo.ucCbusControl |= CBUS_RX_SRAM_FLAG)
#define CLR_MHL_RX_SRAM_FLAG()          (gMHLInfo.ucCbusControl &= ~CBUS_RX_SRAM_FLAG)

#define GET_MHL_VBUS_PORT0_FLAG()       (Bool)((gMHLInfo.ucCbusControl & VBUS_CHARGE_PORT0_FLAG) ?TRUE :FALSE)
#define SET_MHL_VBUS_PORT0_FLAG()       (gMHLInfo.ucCbusControl |= VBUS_CHARGE_PORT0_FLAG)
#define CLR_MHL_VBUS_PORT0_FLAG()       (gMHLInfo.ucCbusControl &= ~VBUS_CHARGE_PORT0_FLAG)

#define GET_MHL_VBUS_PORT1_FLAG()       (Bool)((gMHLInfo.ucCbusControl & VBUS_CHARGE_PORT1_FLAG) ?TRUE :FALSE)
#define SET_MHL_VBUS_PORT1_FLAG()       (gMHLInfo.ucCbusControl |= VBUS_CHARGE_PORT1_FLAG)
#define CLR_MHL_VBUS_PORT1_FLAG()       (gMHLInfo.ucCbusControl &= ~VBUS_CHARGE_PORT1_FLAG)

#define GET_MHL_VBUS_PORT2_FLAG()       (Bool)((gMHLInfo.ucCbusControl & VBUS_CHARGE_PORT2_FLAG) ?TRUE :FALSE)
#define SET_MHL_VBUS_PORT2_FLAG()       (gMHLInfo.ucCbusControl |= VBUS_CHARGE_PORT2_FLAG)
#define CLR_MHL_VBUS_PORT2_FLAG()       (gMHLInfo.ucCbusControl &= ~VBUS_CHARGE_PORT2_FLAG)

#define GET_MHL_CABLE_DETECT_PORT(a)    (Bool)((gMHLInfo.ucCbusControl & (VBUS_CHARGE_PORT0_FLAG << a)) ?TRUE :FALSE)
#define GET_MHL_CABLE_DETECT_ALL()      (gMHLInfo.ucCbusControl &(VBUS_CHARGE_PORT0_FLAG |VBUS_CHARGE_PORT1_FLAG |VBUS_CHARGE_PORT2_FLAG))

#define CLR_MHL_VBUS_CHARGR()           (gMHLInfo.ucCbusControl &= ~MHL_VBUS_CHARGE_MASK)

#define GET_MHL_RECEIVE_MSC()           (Bool)((gMHLInfo.ucReceiveIndex & CBUS_RECEIVE_MSC_FLAG) ?TRUE :FALSE)
#define SET_MHL_RECEIVE_MSC()           (gMHLInfo.ucReceiveIndex |= CBUS_RECEIVE_MSC_FLAG)
#define CLR_MHL_RECEIVE_MSC()           (gMHLInfo.ucReceiveIndex &= ~CBUS_RECEIVE_MSC_FLAG)

#define GET_MHL_RECEIVE_ACK()           (Bool)((gMHLInfo.ucReceiveIndex & CBUS_RECEIVE_ACK_FLAG) ?TRUE :FALSE)
#define SET_MHL_RECEIVE_ACK()           (gMHLInfo.ucReceiveIndex |= CBUS_RECEIVE_ACK_FLAG)
#define CLR_MHL_RECEIVE_ACK()           (gMHLInfo.ucReceiveIndex &= ~CBUS_RECEIVE_ACK_FLAG)

#define GET_MHL_RECEIVE_ABORT()         (Bool)((gMHLInfo.ucReceiveIndex & CBUS_RECEIVE_ABORT_FLAG) ?TRUE :FALSE)
#define SET_MHL_RECEIVE_ABORT()         (gMHLInfo.ucReceiveIndex |= CBUS_RECEIVE_ABORT_FLAG)
#define CLR_MHL_RECEIVE_ABORT()         (gMHLInfo.ucReceiveIndex &= ~CBUS_RECEIVE_ABORT_FLAG)

#define GET_MHL_RECEIVE_RCP()           (Bool)((gMHLInfo.ucReceiveIndex & CBUS_RECEIVE_RCP_FLAG) ?TRUE :FALSE)
#define SET_MHL_RECEIVE_RCP()           (gMHLInfo.ucReceiveIndex |= CBUS_RECEIVE_RCP_FLAG)
#define CLR_MHL_RECEIVE_RCP()           (gMHLInfo.ucReceiveIndex &= ~CBUS_RECEIVE_RCP_FLAG)

#define GET_MHL_RECEIVE_RAP()           (Bool)((gMHLInfo.ucReceiveIndex & CBUS_RECEIVE_RAP_FLAG) ?TRUE :FALSE)
#define SET_MHL_RECEIVE_RAP()           (gMHLInfo.ucReceiveIndex |= CBUS_RECEIVE_RAP_FLAG)
#define CLR_MHL_RECEIVE_RAP()           (gMHLInfo.ucReceiveIndex &= ~CBUS_RECEIVE_RAP_FLAG)

#define GET_MHL_RECEIVE_ERROR()         (Bool)((gMHLInfo.ucReceiveIndex & CBUS_RECEIVE_ERROR_FLAG) ?TRUE :FALSE)
#define SET_MHL_RECEIVE_ERROR()         (gMHLInfo.ucReceiveIndex |= CBUS_RECEIVE_ERROR_FLAG)
#define CLR_MHL_RECEIVE_ERROR()         (gMHLInfo.ucReceiveIndex &= ~CBUS_RECEIVE_ERROR_FLAG)

#define GET_MHL_RECEIVE_DEVCAP()        (Bool)((gMHLInfo.ucReceiveIndex & CBUS_RECEIVE_DEVCAP_FLAG) ?TRUE :FALSE)
#define SET_MHL_RECEIVE_DEVCAP()        (gMHLInfo.ucReceiveIndex |= CBUS_RECEIVE_DEVCAP_FLAG)
#define CLR_MHL_RECEIVE_DEVCAP()        (gMHLInfo.ucReceiveIndex &= ~CBUS_RECEIVE_DEVCAP_FLAG)

#define CLR_MHL_RECEIVE_FLAG()          (gMHLInfo.ucReceiveIndex = 0)

#define GET_MHL_SEND_RCP_ENABLE()       (Bool)((gMHLInfo.ucSendIndex & CBUS_SEND_RCP_ENABLE_FLAG) ?TRUE :FALSE)
#define SET_MHL_SEND_RCP_ENABLE()       (gMHLInfo.ucSendIndex |= CBUS_SEND_RCP_ENABLE_FLAG)
#define CLR_MHL_SEND_RCP_ENABLE()       (gMHLInfo.ucSendIndex &= ~CBUS_SEND_RCP_ENABLE_FLAG)

#define GET_MHL_SEND_RAP_ENABLE()       (Bool)((gMHLInfo.ucSendIndex & CBUS_SEND_RAP_ENABLE_FLAG) ?TRUE :FALSE)
#define SET_MHL_SEND_RAP_ENABLE()       (gMHLInfo.ucSendIndex |= CBUS_SEND_RAP_ENABLE_FLAG)
#define CLR_MHL_SEND_RAP_ENABLE()       (gMHLInfo.ucSendIndex &= ~CBUS_SEND_RAP_ENABLE_FLAG)

#define GET_MHL_SEND_MSG_FINISH()       (Bool)((gMHLInfo.ucSendIndex & CBUS_SEND_MSG_FINISH_FLAG) ?TRUE :FALSE)
#define SET_MHL_SEND_MSG_FINISH()       (gMHLInfo.ucSendIndex |= CBUS_SEND_MSG_FINISH_FLAG)
#define CLR_MHL_SEND_MSG_FINISH()       (gMHLInfo.ucSendIndex &= ~CBUS_SEND_MSG_FINISH_FLAG)

#define GET_MHL_SEND_HPD_CONTROL()      (Bool)((gMHLInfo.ucSendIndex & CBUS_SEND_HPD_CONTROL_FLAG) ?TRUE :FALSE)
#define SET_MHL_SEND_HPD_CONTROL()      (gMHLInfo.ucSendIndex |= CBUS_SEND_HPD_CONTROL_FLAG)
#define CLR_MHL_SEND_HPD_CONTROL()      (gMHLInfo.ucSendIndex &= ~CBUS_SEND_HPD_CONTROL_FLAG)

#define GET_MHL_SEND_READ()             (Bool)((gMHLInfo.ucSendIndex & CBUS_SEND_READ_FLAG) ?TRUE :FALSE)
#define SET_MHL_SEND_READ()             (gMHLInfo.ucSendIndex |= CBUS_SEND_READ_FLAG)
#define CLR_MHL_SEND_READ()             (gMHLInfo.ucSendIndex &= ~CBUS_SEND_READ_FLAG)

#define GET_MHL_SEND_FINISH()           (Bool)((gMHLInfo.ucSendIndex & CBUS_SEND_FINISH_FLAG) ?TRUE :FALSE)
#define SET_MHL_SEND_FINISH()           (gMHLInfo.ucSendIndex |= CBUS_SEND_FINISH_FLAG)
#define CLR_MHL_SEND_FINISH()           (gMHLInfo.ucSendIndex &= ~CBUS_SEND_FINISH_FLAG)

#define CLR_MHL_SEND_FLAG()             (gMHLInfo.ucSendIndex = 0)

#define GET_MHL_RECEIVE_3D_REQ()        (Bool)((gMHLInfo.ucExtendIndex & CBUS_RECEIVE_3D_REQ_FLAG) ?TRUE :FALSE)
#define SET_MHL_RECEIVE_3D_REQ()        (gMHLInfo.ucExtendIndex |= CBUS_RECEIVE_3D_REQ_FLAG)
#define CLR_MHL_RECEIVE_3D_REQ()        (gMHLInfo.ucExtendIndex &= ~CBUS_RECEIVE_3D_REQ_FLAG)

#define GET_MHL_SEND_3D_VIC()           (Bool)((gMHLInfo.ucExtendIndex & CBUS_SEND_3D_VIC_FLAG) ?TRUE :FALSE)
#define SET_MHL_SEND_3D_VIC()           (gMHLInfo.ucExtendIndex |= CBUS_SEND_3D_VIC_FLAG)
#define CLR_MHL_SEND_3D_VIC()           (gMHLInfo.ucExtendIndex &= ~CBUS_SEND_3D_VIC_FLAG)

#define GET_MHL_SEND_3D_DTD()           (Bool)((gMHLInfo.ucExtendIndex & CBUS_SEND_3D_DTD_FLAG) ?TRUE :FALSE)
#define SET_MHL_SEND_3D_DTD()           (gMHLInfo.ucExtendIndex |= CBUS_SEND_3D_DTD_FLAG)
#define CLR_MHL_SEND_3D_DTD()           (gMHLInfo.ucExtendIndex &= ~CBUS_SEND_3D_DTD_FLAG)

#define GET_MHL_HID_SEND_REQUEST()      (Bool)((gMHLInfo.ucExtendIndex & CBUS_HID_SEND_REQUEST_FLAG) ?TRUE :FALSE)
#define SET_MHL_HID_SEND_REQUEST()      (gMHLInfo.ucExtendIndex |= CBUS_HID_SEND_REQUEST_FLAG)
#define CLR_MHL_HID_SEND_REQUEST()      (gMHLInfo.ucExtendIndex &= ~CBUS_HID_SEND_REQUEST_FLAG)

#define GET_MHL_HID_RECEIVE_REPLY()     (Bool)((gMHLInfo.ucExtendIndex & CBUS_HID_RECEIVE_REPLY_FLAG) ?TRUE :FALSE)
#define SET_MHL_HID_RECEIVE_REPLY()     (gMHLInfo.ucExtendIndex |= CBUS_HID_RECEIVE_REPLY_FLAG)
#define CLR_MHL_HID_RECEIVE_REPLY()     (gMHLInfo.ucExtendIndex &= ~CBUS_HID_RECEIVE_REPLY_FLAG)

#define GET_MHL_WRITE_BURST_STATE()     ((gMHLInfo.ucExtendIndex &MASKBIT(7:5)) >>5)
#define SET_MHL_WRITE_BURST_STATE(a)    (gMHLInfo.ucExtendIndex = MASKSET(gMHLInfo.ucExtendIndex, (a <<5), MASKBIT(7:5)))

#define CLR_MHL_EXTEND_FLAG()           (gMHLInfo.ucExtendIndex = 0)

#define GET_MHL_PAD_FONE2_FLAG()        (Bool)((gMHLInfo.ucCellPhone & CELL_PHONE_PAD_FONE2_FLAG) ?TRUE :FALSE)
#define SET_MHL_PAD_FONE2_FLAG()        (gMHLInfo.ucCellPhone |= CELL_PHONE_PAD_FONE2_FLAG)
#define CLR_MHL_PAD_FONE2_FLAG()        (gMHLInfo.ucCellPhone &= ~CELL_PHONE_PAD_FONE2_FLAG)

#define GET_MHL_BUTTER_FLY_FLAG()       (Bool)((gMHLInfo.ucCellPhone & CELL_PHONE_BUTTER_FLY_FLAG) ?TRUE :FALSE)
#define SET_MHL_BUTTER_FLY_FLAG()       (gMHLInfo.ucCellPhone |= CELL_PHONE_BUTTER_FLY_FLAG)
#define CLR_MHL_BUTTER_FLY_FLAG()       (gMHLInfo.ucCellPhone &= ~CELL_PHONE_BUTTER_FLY_FLAG)

#define GET_MHL_SONY_XPERIA_FLAG()      (Bool)((gMHLInfo.ucCellPhone & CELL_PHONE_SONY_XPERIA_FLAG) ?TRUE :FALSE)
#define SET_MHL_SONY_XPERIA_FLAG()      (gMHLInfo.ucCellPhone |= CELL_PHONE_SONY_XPERIA_FLAG)
#define CLR_MHL_SONY_XPERIA_FLAG()      (gMHLInfo.ucCellPhone &= ~CELL_PHONE_SONY_XPERIA_FLAG)

#define GET_MHL_HUAWEI_PHONE_FLAG()     (Bool)((gMHLInfo.ucCellPhone & CELL_PHONE_HUAWEI_PHONE_FLAG) ?TRUE :FALSE)
#define SET_MHL_HUAWEI_PHONE_FLAG()     (gMHLInfo.ucCellPhone |= CELL_PHONE_HUAWEI_PHONE_FLAG)
#define CLR_MHL_HUAWEI_PHONE_FLAG()     (gMHLInfo.ucCellPhone &= ~CELL_PHONE_HUAWEI_PHONE_FLAG)

#define GET_MHL_SAMSUNG_PHONE_FLAG()    (Bool)((gMHLInfo.ucCellPhone & CELL_PHONE_SAMSUNG_PHONE_FLAG) ?TRUE :FALSE)
#define SET_MHL_SAMSUNG_PHONE_FLAG()    (gMHLInfo.ucCellPhone |= CELL_PHONE_SAMSUNG_PHONE_FLAG)
#define CLR_MHL_SAMSUNG_PHONE_FLAG()    (gMHLInfo.ucCellPhone &= ~CELL_PHONE_SAMSUNG_PHONE_FLAG)

#define CLR_MHL_CELL_PHONE_FLAG()       (gMHLInfo.ucCellPhone = 0)

#define GET_MHL_CBUS_STATE()            (gMHLInfo.MhlCbusInfo.MhlCbusState)
#define SET_MHL_CBUS_STATE(a)           (gMHLInfo.MhlCbusInfo.MhlCbusState = a)

#define GET_MHL_CBUS_QUEUE(a)           (gMHLInfo.MhlCbusInfo.request[a])
#define GET_MHL_QUEUE_STATE(a)          (GET_MHL_CBUS_QUEUE(a).reqStatus)
#define SET_MHL_QUEUE_STATE(a ,b)       (GET_MHL_CBUS_QUEUE(a).reqStatus = b)

#define GET_MHL_RX_MSG_COMMAND()        (GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[10])
#define SET_MHL_RX_MSG_COMMAND(a)       (GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[10] = a)

#define GET_MHL_RX_MSG_DATA()           (GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[11])
#define SET_MHL_RX_MSG_DATA(a)          (GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[11] = a)

#define GET_MHL_TX_QUEUE_INDEX()        (gMHLInfo.MhlCbusInfo.curIdx)
#define SET_MHL_TX_QUEUE_INDEX(a)       (gMHLInfo.MhlCbusInfo.curIdx = a)

#define GET_MHL_CBUS_ERROE_CODE()       (gMHLInfo.ucErrorCode)
#define SET_MHL_CBUS_ERROE_CODE(a)      (gMHLInfo.ucErrorCode = a)

#define GET_MHL_POWER_STSTUS()          (gMHLInfo.MhlPowerStatus &MASKBIT(2:0))
#define SET_MHL_POWER_STSTUS(a)         (gMHLInfo.MhlPowerStatus = MASKSET(gMHLInfo.MhlPowerStatus, a, MASKBIT(2:0)))

#define GET_MHL_POWER_ON_FLAG()         (Bool)((gMHLInfo.MhlPowerStatus & CBUS_POWER_ON_FLAG) ?TRUE :FALSE)
#define SET_MHL_POWER_ON_FLAG()         (gMHLInfo.MhlPowerStatus |= CBUS_POWER_ON_FLAG)
#define CLR_MHL_POWER_ON_FLAG()         (gMHLInfo.MhlPowerStatus &= ~CBUS_POWER_ON_FLAG)

#define GET_MHL_PORT_SELECT()           ((gMHLInfo.MhlPowerStatus &MASKBIT(7:4)) >>4)
#define SET_MHL_PORT_SELECT(a)          (gMHLInfo.MhlPowerStatus = MASKSET(gMHLInfo.MhlPowerStatus, (a <<4), MASKBIT(7:4)))

#define GET_MHL_CBUS_ASK_DATA()         (gMHLInfo.ucAskData)
#define SET_MHL_CBUS_ASK_DATA(a)        (gMHLInfo.ucAskData = a)

#define GET_MHL_DEBOUNCE_TIMER()        (bCbusTimerFlag)
#define SET_MHL_DEBOUNCE_TIMER(a)       (bCbusTimerFlag = FALSE, usCbusTimerCnt = a, bCbusTimerFlag = TRUE)

#define GET_MHL_TIMEOUT_TIMER()         (bCbusTimerFlag)
#define SET_MHL_TIMEOUT_TIMER(a)        (bCbusTimerFlag = FALSE, usCbusTimerCnt = a *1010ul, bCbusTimerFlag = TRUE)  // Add 1% more time

#define GET_MHL_ABORT_TIMER()           (bCbusTimerFlag)
#define SET_MHL_ABORT_TIMER(a)          (bCbusTimerFlag = FALSE, usCbusTimerCnt = a *1010ul, bCbusTimerFlag = TRUE)  // Add 1% more time

#define GET_MHL_POLLING_TIMER()         (ucMHLPollingTimerCnt)
#define SET_MHL_POLLING_TIMER(a)        (ucMHLPollingTimerCnt = a)
#define DEC_MHL_POLLING_TIMER()         (ucMHLPollingTimerCnt--)

#define GET_MHL_CD_ENABLE_TIMER()       (ucCableDetectTimerCnt)
#define SET_MHL_CD_ENABLE_TIMER(a)      (ucCableDetectTimerCnt = a)
#define DEC_MHL_CD_ENABLE_TIMER()       (ucCableDetectTimerCnt--)

#define GET_MHL_HPD_CONTROL_TIMER()     (ucHPDControlTimerCnt)
#define SET_MHL_HPD_CONTROL_TIMER(a)    (ucHPDControlTimerCnt = a)
#define DEC_MHL_HPD_CONTROL_TIMER()     (ucHPDControlTimerCnt--)

#define GET_MHL_RX_SRAM_QUEUE(a)        (ucMHLRxSRAM[a]) 

#define GET_MHL_CBUS_SELECT_PATH()      (ucMHLCbusSelect &MASKBIT(3:0))
#define SET_MHL_CBUS_SELECT_PATH(a)     (ucMHLCbusSelect = MASKSET(ucMHLCbusSelect, a, MASKBIT(3:0)))

#define GET_MHL_INPUT_CLOCK_MODE()      ((ucMHLCbusSelect &MASKBIT(6:4)) >>4)
#define SET_MHL_INPUT_CLOCK_MODE(a)     (ucMHLCbusSelect = MASKSET(ucMHLCbusSelect, (a <<4), MASKBIT(6:4)))

#define GET_MHL_CBUS_PATH_SWITCH()      (Bool)((ucMHLCbusSelect & CBUS_PATH_SWITCH_FLAG) ?TRUE :FALSE)
#define SET_MHL_CBUS_PATH_SWITCH()      (ucMHLCbusSelect |= CBUS_PATH_SWITCH_FLAG)
#define CLR_MHL_CBUS_PATH_SWITCH()      (ucMHLCbusSelect &= ~CBUS_PATH_SWITCH_FLAG)

#define GET_MHL_RX_RECEIVE_COUNTER()    (gMHLInfo.ucReceiveCounter)
#define CLR_MHL_RX_RECEIVE_COUNTER()    (gMHLInfo.ucReceiveCounter = 0)
#define INC_MHL_RX_RECEIVE_COUNTER()    (gMHLInfo.ucReceiveCounter++)
#define DEC_MHL_RX_RECEIVE_COUNTER()    (gMHLInfo.ucReceiveCounter--)

#define CLR_MHL_PLUGGED_FLAG()          (CLR_CBUS_CABLE_FLAG(), CLR_CBUS_LINK_CHECK(), CLR_MHL_RECEIVE_FLAG(), CLR_MHL_SEND_FLAG(), CLR_MHL_EXTEND_FLAG(), CLR_MHL_RX_RECEIVE_COUNTER(), CLR_MHL_RX_SRAM_FLAG())
#define CLR_MHL_CONNECT_FLAG()          (CLR_CBUS_LINK_FLAG(), CLR_CBUS_LINK_CHECK(), CLR_MHL_RECEIVE_FLAG(), CLR_MHL_SEND_FLAG(), CLR_MHL_EXTEND_FLAG(), CLR_MHL_RX_RECEIVE_COUNTER(), CLR_MHL_RX_SRAM_FLAG())

#define GET_MHL_3D_TOT_ENT_DTD()        (ucMHL3DTotalEntryDTD)
#define SET_MHL_3D_TOT_ENT_DTD(a)       (ucMHL3DTotalEntryDTD = a)

#define GET_MHL_3D_TOT_ENT_VIC()        (ucMHL3DTotalEntryVIC)
#define SET_MHL_3D_TOT_ENT_VIC(a)       (ucMHL3DTotalEntryVIC = a)

#define GET_MHL_3D_DTD_INFORMATION(a)   (ucMHL3DInformationDTD[a])
#define SET_MHL_3D_DTD_INFORMATION(a,b) (ucMHL3DInformationDTD[a] = b)

#define GET_MHL_3D_VIC_INFORMATION(a)   (ucMHL3DInformationVIC[a])
#define SET_MHL_3D_VIC_INFORMATION(a,b) (ucMHL3DInformationVIC[a] = b)

#define GET_MHL_WRITE_BURST_LENGTH()    (ucMHLWriteBurstInformation[MHL_MSC_SCRATCHPAD_SIZE])
#define SET_MHL_WRITE_BURST_LENGTH(a)   (ucMHLWriteBurstInformation[MHL_MSC_SCRATCHPAD_SIZE] = a)
#define GET_MHL_WRITE_BURST_ADDRESS()   (ucMHLWriteBurstInformation[MHL_MSC_SCRATCHPAD_SIZE +1])
#define SET_MHL_WRITE_BURST_ADDRESS(a)  (ucMHLWriteBurstInformation[MHL_MSC_SCRATCHPAD_SIZE +1] = a)
#define GET_MHL_WRITE_BURST_DATA(a)     (ucMHLWriteBurstInformation[a])
#define SET_MHL_WRITE_BURST_DATA(a,b)   (ucMHLWriteBurstInformation[a] = b)

#define GET_MHL_CELL_PHONE_ID_PORT0()   (ucMHLCellPhoneIDPort0)
#define SET_MHL_CELL_PHONE_ID_PORT0(a)  (ucMHLCellPhoneIDPort0 = a)
#define CLR_MHL_CELL_PHONE_ID_PORT0()   (ucMHLCellPhoneIDPort0 = CELL_PHONE_ID_NONE)

#define GET_MHL_CELL_PHONE_ID_PORT1()   (ucMHLCellPhoneIDPort1)
#define SET_MHL_CELL_PHONE_ID_PORT1(a)  (ucMHLCellPhoneIDPort1 = a)
#define CLR_MHL_CELL_PHONE_ID_PORT1()   (ucMHLCellPhoneIDPort1 = CELL_PHONE_ID_NONE)

#define GET_MHL_CELL_PHONE_ID_PORT2()   (ucMHLCellPhoneIDPort2)
#define SET_MHL_CELL_PHONE_ID_PORT2(a)  (ucMHLCellPhoneIDPort2 = a)
#define CLR_MHL_CELL_PHONE_ID_PORT2()   (ucMHLCellPhoneIDPort2 = CELL_PHONE_ID_NONE)

//-------------------------------------------------------------------------------------------------
//  Function Prototype
//-------------------------------------------------------------------------------------------------
extern xdata MHLInfo_S gMHLInfo;
extern bit idata bCbusTimerFlag;
extern BYTE xdata ucMHLPollingTimerCnt;
extern BYTE xdata ucCableDetectTimerCnt;
extern BYTE xdata ucHPDControlTimerCnt;
extern WORD idata usCbusTimerCnt;
extern BYTE xdata ucMHLCbusSelect;
extern BYTE xdata ucMHLRxSRAM[];
extern BYTE xdata ucMHL3DTotalEntryDTD;
extern BYTE xdata ucMHL3DTotalEntryVIC;
extern BYTE xdata ucMHL3DInformationDTD[];
extern BYTE xdata ucMHL3DInformationVIC[];
extern BYTE xdata ucMHLWriteBurstInformation[];
extern BYTE xdata ucMHLCellPhoneIDPort0;
extern BYTE xdata ucMHLCellPhoneIDPort1;
extern BYTE xdata ucMHLCellPhoneIDPort2;

// Signal detect
void mdrv_mhl_Initial(BYTE *pEdid);
void mdrv_mhl_SourceChange(BYTE ucPort);
void mdrv_mhl_SetCableDetectTime(BYTE ucCbusSelect, BYTE ucTime);
void mdrv_mhl_SetHPDControlTime(BYTE ucTime);
Bool mdrv_mhl_CableDetect(BYTE ucPort);
Bool mdrv_mhl_CbusStatus(void);
void mdrv_mhl_CbusStablePolling(void);
// Rx function
Bool mdrv_mhl_CbusRcpReply(BYTE ucStatusCode, BYTE ucKeyCode);
Bool mdrv_mhl_CbusRapReply(BYTE ucStatusCode);
Bool mdrv_mhl_CbusUcpReply(BYTE ucStatusCode, BYTE ucKeyCode);
// Tx function
Bool mdrv_mhl_CbusMscMsgSubCmdSend(BYTE ucSubCmd, BYTE ucData);
Bool mdrv_mhl_CbusWriteBurst(BYTE ucAddr, BYTE ucLength, BYTE *pBuffer);
void mdrv_mhl_CbusSendCommand(BYTE ucSelect);
Bool mdrv_mhl_GetDeviceCapacibility(WORD usDevcapMask, BYTE *ucDevcap);
void mdrv_mhl_Send3DInformation(void);
// Handler
void mdrv_mhl_CbusIntCB(void);
void mdrv_mhl_CbusIntHandler(void);
Bool mdrv_mhl_CbusRxHandler(BYTE *ucCmd, BYTE *ucData);
void mdrv_mhl_CbusTxHandler(void);
void mdrv_mhl_CbusTimerHandler(void);
void mdrv_mhl_CbusConnectionCheck(void);
void mdrv_mhl_IsrHandler(void);
// System
void mdrv_mhl_PowerControl(BYTE ucState);
void mdrv_mhl_ChargePortDetect(Bool bReset);
void mdrv_mhl_InsertCellPhoneID(BYTE ucCellPhoneID);

#if MS_PM
Bool mdrv_mhl_WakeupDetect(Bool bAutoSearch);
#endif

#endif //_MDRV_MHL_H_

