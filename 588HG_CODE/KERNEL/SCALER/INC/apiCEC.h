//***********************************************************************************
// @file: ms_apiCEC.h
// @author: MStar Semiconductor Inc.
// @brief: CEC api layer interface
//***********************************************************************************

#ifndef _API_CEC_H_
#define _API_CEC_H_

#if ENABLE_CEC

#ifdef _API_CEC_C_
    #define _API_CEC_DEF_
#else
   #define _API_CEC_DEF_ extern
#endif


//***********************************************************************************
//  Macro and Define
//***********************************************************************************

#define CEC_VER_11      0   //CEC1.1
#define CEC_VER_12      1   //CEC1.2
#define CEC_VER_12a     2   //CEC1.2a
#define CEC_VER_13      3   //CEC1.3
#define CEC_VER_13a     4   //CEC1.3a

#define HDMI_CEC_VER    CEC_VER_13a

//***********************************************************************************
//  Type and Structure
//***********************************************************************************

//CEC [Abort Reason]
typedef enum _enMsCEC_MSG_ABORT_REASON_
{
    CEC_AR_UNKNOWN_OPCODE           = 0,
    CEC_AR_INCORRECT_MODE           = 1,
    CEC_AR_CANT_PROVIDE_SRC         = 2,
    CEC_AR_INVALID_OPERANG          = 3,
    CEC_AR_REFUSED                  = 4,
    CEC_AR_UNABLE_DETERMINE         = 5,
} enMsCEC_MSG_ABORT_REASON;

//CEC [Analog Broadcast Type]
typedef enum _enMsCEC_ANALOG_BROADCAST_TYPE_
{
    CEC_BDTYPE_CABLE            = 0,
    CEC_BDTYPE_SATELLITE        = 1,
    CEC_BDTYPE_TERRESTRIAL      = 2,
} enMsCEC_ANALOG_BROADCAST_TYPE;

//CEC [Device Type]
typedef enum _enMsCEC_DEVICE_TYPE_
{
    CEC_DEV_TV                  = 0,
    CEC_DEV_RECORDER            = 1,
    CEC_DEV_RESERVED            = 2,
    CEC_DEV_TUNER               = 3,
    CEC_DEV_PLAYBACK            = 4,
    CEC_DEV_AUD_SYSTEM          = 5,
    CEC_DEV_SWITCH              = 6,
    CEC_DEV_VID_PROCSSOR        = 7,
} enMsCEC_DEVICE_TYPE;

//CEC [Power Status]
typedef enum _enMsCEC_POWER_STATUS_
{
    CEC_PWRSTS_ON           = 0,
    CEC_PWRSTS_STANDBY      = 1,
    CEC_PWRSTS_STANDBY2ON   = 2,
    CEC_PWRSTS_ON2STANDBY   = 3,
} enMsCEC_POWER_STATUS;

//CEC [OpCode]
//the opcode is referenced from CEC1.3a table 7 ~ 27
typedef enum _enMsCEC_OPCODE_LIST_
{
    //----- One Touch Play ----------------------------
    CEC_OPCODE_ACTIVE_SOURCE                = 0x82,
    CEC_OPCODE_IMG_VIEW_ON                  = 0x04,
    CEC_OPCODE_TXT_VIEW_ON                  = 0x0D,

    //----- Routing Control ---------------------------
    CEC_OPCODE_INACTIVE_SOURCE              = 0x9D,
    CEC_OPCODE_REQ_ACTIVE_SOURCE            = 0x85,
    CEC_OPCODE_ROUTING_CHANGE               = 0x81,
    CEC_OPCODE_ROUTING_INFO                 = 0x80,
    CEC_OPCODE_SET_STREAM_PATH              = 0x86,

    //----- Standby Command ---------------------------
    CEC_OPCODE_STANDBY                      = 0x36,

    //----- One Touch Record---------------------------
    CEC_OPCODE_RECORD_ON                    = 0x09,
    CEC_OPCODE_RECORD_OFF                   = 0x0B,
    CEC_OPCODE_RECORD_STATUS                = 0x0A,
    CEC_OPCODE_RECORD_TV_SCREEN             = 0x0F,

    //----- Timer programmer -------------------------- CEC1.3a
    CEC_OPCODE_CLEAR_ANALOG_TIMER           = 0x33,
    CEC_OPCODE_CLEAR_DIGITAL_TIMER          = 0x99,
    CEC_OPCODE_CLEAR_EXT_TIMER              = 0xA1,
    CEC_OPCODE_SET_ANALOG_TIMER             = 0x34,
    CEC_OPCODE_SET_DIGITAL_TIMER            = 0x97,
    CEC_OPCODE_SET_EXT_TIMER                = 0xA2,
    CEC_OPCODE_SET_TIMER_PROGRAM_TITLE      = 0x67,
    CEC_OPCODE_TIMER_CLEARD_STATUS          = 0x43,
    CEC_OPCODE_TIMER_STATUS                 = 0x35,

    //----- System Information ------------------------
    CEC_OPCODE_CEC_VER                      = 0x9E,       //1.3a
    CEC_OPCODE_GET_CEC_VER                  = 0x9F,       //1.3a
    CEC_OPCODE_GIVE_PHY_ADDR                = 0x83,    
    CEC_OPCODE_GET_MENU_LANGUAGE            = 0x91,
    CEC_OPCODE_REPORT_PHY_ADDR              = 0x84,
    CEC_OPCODE_SET_MENU_LANGUAGE            = 0x32,
    CEC_OPCODE_POLLING_MESSAGE              = 0x00,

    //E_MSG_SI_REC_TYPE_PRESET                   = 0x00,  //parameter   ?
    //E_MSG_SI_REC_TYPE_OWNSRC                 =  0x01,  //parameter   ?

    //----- Deck Control Feature-----------------------
    CEC_OPCODE_DECK_CTRL                    = 0x42,
    CEC_OPCODE_DECK_STATUS                  = 0x1B,
    CEC_OPCODE_GIVE_DECK_STATUS             = 0x1A,
    CEC_OPCODE_PLAY                         = 0x41,

    //----- Tuner Control ------------------------------
    CEC_OPCODE_GIVE_TUNER_STATUS            = 0x08,
    CEC_OPCODE_SEL_ANALOG_SERVICE           = 0x92,
    CEC_OPCODE_SEL_DIGITAL_SERVICE          = 0x93,
    CEC_OPCODE_TUNER_DEVICE_STATUS          = 0x07,
    CEC_OPCODE_TUNER_STEP_DECREASE          = 0x06,
    CEC_OPCODE_TUNER_STEP_INCREASE          = 0x05,

    //---------Vendor Specific -------------------------
    //E_MSG_VS_CEC_VERSION                        = 0x9E,       //1.3a
    //E_MSG_VS_GET_CEC_VERSION                 = 0x9F,       //1.3a
    CEC_OPCODE_DEVICE_VENDOR_ID             = 0x87,
    CEC_OPCODE_GIVE_VENDOR_ID               = 0x8C,
    CEC_OPCODE_VENDOR_COMMAND               = 0x89,
    CEC_OPCODE_VENDOR_COMMAND_WITH_ID       = 0xA0,      //1.3a
    CEC_OPCODE_VENDOR_RC_BUT_DOWN           = 0x8A,
    CEC_OPCODE_VENDOR_RC_BUT_UP             = 0x8B,

    //----- OSD Display --------------------------------
    CEC_OPCODE_SET_OSD_STRING               = 0x64,

    //----- Device OSD Name Transfer  -------------------------
    CEC_OPCODE_GIVE_OSD_NAME                = 0x46,
    CEC_OPCODE_SET_OSD_NAME                 = 0x47,

    //----- Device Menu Control ------------------------
    CEC_OPCODE_MENU_REQUEST                 = 0x8D,
    CEC_OPCODE_MENU_STATUS                  = 0x8E,
    //E_MSG_DMC_MENU_ACTIVATED                 = 0x00,   //parameter
    //E_MSG_DMC_MENU_DEACTIVATED             = 0x01,   //parameter
    CEC_OPCODE_UI_PRESS                     = 0x44,
    CEC_OPCODE_UI_RELEASE                   = 0x45,

    //----- Remote Control Passthrough ----------------
    //----- UI Message --------------------------------
    //#define UI_PRESS             0x44
    //#define UI_RELEASE           0x45

    //----- Power Status  ------------------------------
    CEC_OPCODE_GIVE_POWER_STATUS            = 0x8F,
    CEC_OPCODE_REPORT_POWER_STATUS          = 0x90,

    //----- General Protocal Message ------------------
    //----- Abort Message -----------------------------
    CEC_OPCODE_ABORT_MESSAGE                = 0xFF,
    //----- Feature Abort -----------------------------
    CEC_OPCODE_FEATURE_ABORT                = 0x00,

    //----- System Audio Control ------------------
    CEC_OPCODE_GIVE_AUDIO_STATUS            = 0x71,
    CEC_OPCODE_GIVE_SYSTEM_AUDIO_MODE_STATUS    = 0x7D,
    CEC_OPCODE_REPORT_AUDIO_STATUS          = 0x7A,
    CEC_OPCODE_SET_SYSTEM_AUDIO_MODE        = 0x72,
    CEC_OPCODE_SYSTEM_AUDIO_MODE_REQUEST    = 0x70,
    CEC_OPCODE_SYSTEM_AUDIO_MODE_STATUS     = 0x7E,

    //----- System Audio Control ------------------
    CEC_OPCODE_SET_AUDIO_RATE               = 0x9A,

    //----- Audio Channel Return Control ------------
    CEC_OPCODE_INIT_ARC                     = 0xC0,
    CEC_OPCODE_REPORT_ARC_INIT              = 0xC1,
    CEC_OPCODE_REPORT_ARC_TERMINATE         = 0xC2,
    CEC_OPCODE_REQ_ARC_INIT                 = 0xC3,
    CEC_OPCODE_REQ_ARC_TERMINATE            = 0xC4,
    CEC_OPCODE_TERMINATE_ARC                = 0xC5,
    CEC_OPCODE_CDC_MESSAGE                  = 0xF8,
    
} enMsCEC_OPCODE_LIST;

typedef enum _enMsCEC_MSGTRANS_TYPE_
{
    CEC_MSGTRANS_BROADCAST = 0,
    CEC_MSGTRANS_DIRECT = 1,
    CEC_MSGTRANS_BOTH = 2,
    
} enMsCEC_MSGTRANS_TYPE;


//CEC error code
typedef enum _enMsCEC_INT_EVENT_
{
    CEC_EVENT_FEATURE_ABORT     = 0,
    CEC_EVENT_RX_SUCCESS        = _BIT0,
    CEC_EVENT_TX_SUCCESS        = _BIT1,
    CEC_EVENT_RETRY_FAIL        = _BIT2,
    CEC_EVENT_LOST_ARBITRATION  = _BIT3,
    CEC_EVENT_FOLLOWER_NACK     = _BIT4,
    CEC_EVENT_SYSTEM_BUSY       = _BIT5,
} enMsCEC_INT_EVENT;

//Logical Address
typedef enum _enMsCEC_DEV_LOGIC_ADDR_
{
    CEC_LA_TV               = 0,
    CEC_LA_RECORDER1        = 1,
    CEC_LA_RECORDER2        = 2,
    CEC_LA_TUNER1           = 3,
    CEC_LA_PLAYBACK_DEV1    = 4,
    CEC_LA_AUD_SYSTEM       = 5,
    CEC_LA_TUNER2           = 6,
    CEC_LA_TUNER3           = 7,
    CEC_LA_PLAYBACK_DEV2    = 8,
    CEC_LA_RECORDER3        = 9,
#if (HDMI_CEC_VER == CEC_VER_13a)
    CEC_LA_TUNER4           = 10,
    CEC_LA_PLAYBACK_DEV3    = 11,
#endif
    CEC_LA_FREE_USE         = 14,
    CEC_LA_UNREGISTERED     = 15,
    CEC_LA_BOARDCAST        = 15,
    CEC_LA_MAX              = 15,
} enMsCEC_DEV_LOGIC_ADDR;


//***********************************************************************************
//  Function Prototype
//***********************************************************************************
_API_CEC_DEF_ enMsCEC_INT_EVENT api_CECMSG_ReportPowerStatus(enMsCEC_DEV_LOGIC_ADDR dst_addr);
_API_CEC_DEF_ void api_CEC_init(void);
_API_CEC_DEF_ enMsCEC_INT_EVENT api_CEC_SendMessage(enMsCEC_DEV_LOGIC_ADDR dst_addr, enMsCEC_OPCODE_LIST msg, BYTE* operandPtr, BYTE length);
_API_CEC_DEF_ enMsCEC_INT_EVENT api_CECMSG_FeatureAbort(enMsCEC_DEV_LOGIC_ADDR dst_addr, enMsCEC_OPCODE_LIST msg, enMsCEC_MSG_ABORT_REASON cmd);
_API_CEC_DEF_ void api_CEC_ProcessCommand(void);
_API_CEC_DEF_ void api_CEC_CheckRxBuffer(void);
_API_CEC_DEF_ void api_CEC_Handler(void);


#endif //#if ENABLE_CEC

#endif //#ifndef _API_CEC_H_

