///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    mapi_mhl.c
/// @author MStar Semiconductor Inc.
/// @brief  MHL driver Function
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MAPI_MHL_C_
#define _MAPI_MHL_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "global.h"
#if ENABLE_MHL
#include "appMHLDefine.h"

#if ENABLE_MHL_CTS_TEST
#include "keypad.h"
#include "keypaddef.h"
#endif

#if ENABLE_MHL_EDID_IN_FLASH
#include "msflash.h"
#endif
//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define MHL_API_DEBUG_MESSAGE       1

#if(ENABLE_DEBUG && MHL_DEBUG_MESSAGE && MHL_API_DEBUG_MESSAGE)
#define MHL_API_DPUTSTR(str)        printMsg(str)
#define MHL_API_DPRINTF(str, x)     printData(str, x)
#else
#define MHL_API_DPUTSTR(str)
#define MHL_API_DPRINTF(str, x)
#endif

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

//**************************************************************************
//  [Function Name]:
//                  _mapi_mhl_InputToPortMapping
//  [Description]
//                  
//  [Arguments]:
//                  
//  [Return]:
//                  
//**************************************************************************
BYTE _mapi_mhl_InputToPortMapping(BYTE ucPort)
{
    if(ucPort == MHL_SELECT_PORT0)
    {
        ucPort = MHL_INPUT_PORT0;
    }
    else if(ucPort == MHL_SELECT_PORT1)
    {
        ucPort = MHL_INPUT_PORT1;
    }
    else if(ucPort == MHL_SELECT_PORT2)
    {
        ucPort = MHL_INPUT_PORT2;
    }
    else // VGA and DP
    {
        ucPort = MHL_INPUT_NONE;
    }

    return ucPort;
}

//**************************************************************************
//  [Function Name]:
//                  _mapi_mhl_CbusRcpProcess()
//  [Description]
//                  MHL Cbus process MSC RCP sub-command
//  [Arguments]:
//                  ucRcpCode: RCP sub-command code
//  [Return]:
//
//**************************************************************************
BYTE _mapi_mhl_CbusRcpProcess(BYTE ucRcpCode)
{
    BYTE xdata statusCode = MSC_RCP_NO_ERROR;

    switch(ucRcpCode)
    {
#if ENABLE_MHL_CTS_TEST
        case RCP_KEYID_UP:
            MHL_TX_SEND_RCP_KEY_UP();
            break;
            
        case RCP_KEYID_DOWN:
            MHL_TX_SEND_RCP_KEY_DOWN();
            break;
            
        case RCP_KEYID_ROOT_MENU:
            MHL_TX_SEND_RCP_KEY_ROOT_MENU();
            break;
            
        case RCP_KEYID_EXIT:
            MHL_TX_SEND_RCP_KEY_EXIT();
            break;
#endif
        default:
            statusCode = MSC_RCP_INEFFECTIVE_KEY_CODE;
            break;
    }
    
    if(statusCode == MSC_RCP_INEFFECTIVE_KEY_CODE)
    {
        //MHL_DPUTSTR("KeyCode not supported" );
    }

    return (statusCode);
}

//**************************************************************************
//  [Function Name]:
//                  _mapi_mhl_CbusRapProcess (BYTE rapCode)
//  [Description]
//                  MHL Cbus process MSC RAP sub-command
//  [Arguments]:
//                  rapCode: RAP sub-command code
//  [Return]:
//
//**************************************************************************
BYTE _mapi_mhl_CbusRapProcess(BYTE ucRapCode)
{
    BYTE xdata statusCode  = MSC_RAP_NO_ERROR;

    switch(ucRapCode)
    {
        case MSC_RAP_POLL:
            
            break;
            
        case MSC_RAP_CONTENT_ON:
            msWriteByteMask(SC0_02,0,BIT7);
            hw_SetBlacklit();//alan.lee

            break; 
            
        case MSC_RAP_CONTENT_OFF:
            hw_ClrBlacklit();//alan.lee
            msWriteByteMask(SC0_02,BIT7,BIT7);

            break;
            
        default:
            statusCode = MSC_RAP_UNRECOGNIZED_ACTION_CODE;
            break;
    }
	
    return(statusCode);
}

//**************************************************************************
//  [Function Name]:
//                  _mapi_mhl_CbusUcpProcess (BYTE rapCode)
//  [Description]
//                  MHL Cbus process MSC UCP sub-command
//  [Arguments]:
//                  rapCode: UCP sub-command code
//  [Return]:
//
//**************************************************************************
BYTE _mapi_mhl_CbusUcpProcess(BYTE ucRapCode)
{
    BYTE xdata statusCode  = MSC_RAP_NO_ERROR;

    switch(ucRapCode)
    {
        default:
            statusCode = MSC_RAP_UNRECOGNIZED_ACTION_CODE;
            break;
    };
	
    return(statusCode);
}

//**************************************************************************
//  [Function Name]:
//                  _mapi_mhl_CbusProcessMSCSubCmd()
//  [Description]
//                  MHL Cbus process MSC sub-command
//  [Arguments]:
//                  subcmd: MSC sub-command
//                  subdata: MSC sub-data
//  [Return]:
//
//**************************************************************************
void _mapi_mhl_CbusProcessMSCSubCmd (BYTE subcmd, BYTE subdata)
{
    BYTE xdata status = 0;

    switch (subcmd)
    {
        case MSC_MSG_RCP:
            status = _mapi_mhl_CbusRcpProcess(subdata);
            mdrv_mhl_CbusRcpReply(status, subdata);
            break;

        case MSC_MSG_RCPK:
        case MSC_MSG_RCPE:
            
            break;
            
        case MSC_MSG_RAP:
            status = _mapi_mhl_CbusRapProcess(subdata);
            mdrv_mhl_CbusRapReply(status);
            
            break;
            
        case MSC_MSG_RAPK: 
            
            break;

        case MSC_MSG_UCP:
            status = _mapi_mhl_CbusUcpProcess(subdata);
            mdrv_mhl_CbusUcpReply(status, subdata);
            break;

        case MSC_MSG_UCPK:
        case MSC_MSG_UCPE:
            
            break;
                    
        default: // MSGE
            //MHL_DPUTSTR("Respond with MSC_MSGE");
            break;
    }
}

//**************************************************************************
//  [Function Name]:
//                  _mapi_mhl_RxHandler()
//  [Description]
//                  MHL handler
//  [Arguments]:
//                  
//  [Return]:
//
//**************************************************************************
void _mapi_mhl_RxHandler(void)
{
    BYTE xdata ucCmd = 0;
    BYTE xdata ucData = 0;
    
    if(mdrv_mhl_CbusRxHandler(&ucCmd, &ucData))
    {
        _mapi_mhl_CbusProcessMSCSubCmd(ucCmd, ucData);
    }
}

//**************************************************************************
//  [Function Name]:
//                  _mapi_mhl_TxHandler()
//  [Description]
//                  MHL handler
//  [Arguments]:
//                  
//  [Return]:
//
//**************************************************************************
void _mapi_mhl_TxHandler(void)
{
    if(mdrv_mhl_CbusStatus())
    {
        mdrv_mhl_CbusTxHandler();
    }

    mdrv_mhl_CbusConnectionCheck();
}

#if(MHL_READ_DEVICE_CAPABILITY)
//**************************************************************************
//  [Function Name]:
//                  _mapi_mhl_CheckDeviceCapacibility()
//  [Description]
//                  MHL check device capacibility
//  [Arguments]:
//                  
//  [Return]:
//
//**************************************************************************
void _mapi_mhl_CheckDeviceCapacibility(BYTE *ucDevcap)
{
    BYTE xdata uctemp = 0;
    BYTE ucCellPhoneID = CELL_PHONE_ID_NONE;
    WORD xdata usAdopterID = 0;
    WORD xdata usDeviceID = 0;
    
    for(uctemp = 0; uctemp <MHL_DEVICE_CAPABILITY_SIZE ;uctemp++)
    {
        MHL_API_DPRINTF("** MHL read devcap %x \n", *(ucDevcap +uctemp));
    }

    usAdopterID = *(ucDevcap +4) |((*(ucDevcap +3)) << 8);
    usDeviceID = *(ucDevcap +12) |((*(ucDevcap +11)) << 8);

    CLR_MHL_CELL_PHONE_FLAG();

    switch(usAdopterID)
    {
#if(MHL_ASUS_PADFONE2_PATCH)
        case MHL_ADOPTER_ID_ASUS:
            SET_MHL_PAD_FONE2_FLAG();

            MHL_API_DPRINTF("** MHL Asus pad phone2 attach %x \n", 1);
        
            break;
#endif

#if(MHL_HTC_CHARGE_PATCH)
        case MHL_ADOPTER_ID_HTC:
            SET_MHL_BUTTER_FLY_FLAG();
            ucCellPhoneID = CELL_PHONE_ID_HTC_BUTTER_FLY;
            
            MHL_API_DPRINTF("** MHL HTC butter fly attach %x \n", 1);
            
            break;
#endif

#if(MHL_SONY_CHARGE_PATCH)
        case MHL_ADOPTER_ID_SONY:
            SET_MHL_SONY_XPERIA_FLAG();
            ucCellPhoneID = CELL_PHONE_ID_SONY_XPERIA;
            
            MHL_API_DPRINTF("** MHL Sony phone attach %x \n", 1);
            
            break;
#endif

#if(MHL_HUAWEI_PHONE_PATCH)
        case MHL_ADOPTER_ID_HUAWEI:
            SET_MHL_HUAWEI_PHONE_FLAG();
            ucCellPhoneID = CELL_PHONE_ID_HUAWEI_PHONE;
            
            MHL_API_DPRINTF("** MHL HAUWEI phone attach %x \n", 1);
            
            break;
#endif

#if(MHL_LG_PRADA_DC_PATCH)
        case MHL_ADOPTER_ID_LG:
            ucCellPhoneID = CELL_PHONE_ID_LG_PRADA;
            
            MHL_API_DPRINTF("** MHL LG prada phone attach %x \n", 1);
            
            break;
#endif

        case MHL_ADOPTER_ID_SAMSUNG:
            SET_MHL_SAMSUNG_PHONE_FLAG();
            ucCellPhoneID = CELL_PHONE_ID_SAMSUNG_PHONE;
            
            MHL_API_DPRINTF("** MHL Samsung phone phone attach %x \n", 1);
            
            break;
            
        default:
            
            break;
    };

    mdrv_mhl_InsertCellPhoneID(ucCellPhoneID);
}

//**************************************************************************
//  [Function Name]:
//                  _mapi_mhl_ReadDeviceCapacibility()
//  [Description]
//                  MHL read device capacibility
//  [Arguments]:
//                  
//  [Return]:
//
//**************************************************************************
void _mapi_mhl_ReadDeviceCapacibility(void)
{
    static BYTE xdata ucDevcap[MHL_DEVICE_CAPABILITY_SIZE] = {0};
    
    if(mdrv_mhl_GetDeviceCapacibility(MHL_READ_DEVCAP_MASK, ucDevcap))
    {
        _mapi_mhl_CheckDeviceCapacibility(ucDevcap);
    }
}

#endif

#if(MHL_READ_DEVICE_VENDER_ID)
//**************************************************************************
//  [Function Name]:
//                  _mapi_mhl_ReadDeviceVenderID()
//  [Description]
//                  MHL read vender ID
//  [Arguments]:
//                  
//  [Return]:
//
//**************************************************************************
void _mapi_mhl_ReadDeviceVenderID(void)
{
    static BYTE xdata ucVenderID = 0;
    
    if(mdrv_mhl_GetDeviceVenderID(&ucVenderID))
    {
        MHL_API_DPRINTF("** MHL get vender ID %x \n", ucVenderID);
    }
}

#endif

//**************************************************************************
//  [Function Name]:
//                  _mapi_mhl_ReadDeviceInformation()
//  [Description]
//                  MHL read device information
//  [Arguments]:
//                  
//  [Return]:
//
//**************************************************************************
void _mapi_mhl_ReadDeviceInformation(void)
{
    mdrv_mhl_Send3DInformation();
        
#if(MHL_READ_DEVICE_VENDER_ID)
    _mapi_mhl_ReadDeviceVenderID();
#endif

#if(MHL_READ_DEVICE_CAPABILITY)
    _mapi_mhl_ReadDeviceCapacibility();
#endif
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

//**************************************************************************
//  [Function Name]:
//                  mapi_mhl_init()
//  [Description]
//                  MHL init
//  [Arguments]:
//                  
//  [Return]:
//
//**************************************************************************
void mapi_mhl_init(void)
{
#if ENABLE_MHL_EDID_IN_FLASH
    BYTE xdata Mstar_MHL_EDID[256];

    Flash_ReadTbl(MHL_EDID_START, Mstar_MHL_EDID, 256);
#endif

    MHL_API_DPUTSTR("** MHL initial setting");
    mdrv_mhl_Initial(Mstar_MHL_EDID);

    _mapi_mhl_ReadDeviceInformation();
    mdrv_mhl_InsertCellPhoneID(CELL_PHONE_ID_NONE);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_mhl_CbusTimerInterrupt()
//  [Description]
//                  MHL Cbus timer control in isr
//  [Arguments]:
//                  
//  [Return]:
//
//**************************************************************************
void mapi_mhl_CbusTimerInterrupt(void)
{
    if(bCbusTimerFlag)
    {
        if(usCbusTimerCnt)
        {
            usCbusTimerCnt--;
        }
        else
        {
            bCbusTimerFlag = FALSE;
        }
    }
}

//**************************************************************************
//  [Function Name]:
//                  mapi_mhl_PollingTimerhandler
//  [Description]
//                  
//  [Arguments]:
//                  
//  [Return]:
//                  
//**************************************************************************
void mapi_mhl_PollingTimerhandler(void)
{
    if(GET_MHL_POLLING_TIMER())
    {
        DEC_MHL_POLLING_TIMER();
    }

    mdrv_mhl_SetCableDetectTime(GET_MHL_CBUS_SELECT_PATH(), 0);
    mdrv_mhl_SetHPDControlTime(0);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_mhl_IsrHandler()
//  [Description]
//                  MHL isr handler
//  [Arguments]:
//                  
//  [Return]:
//
//**************************************************************************
void mapi_mhl_IsrHandler(void)
{
    mdrv_mhl_IsrHandler();
}

//**************************************************************************
//  [Function Name]:
//                  mapi_mhl_handler()
//  [Description]
//                  MHL handler
//  [Arguments]:
//                  
//  [Return]:
//
//**************************************************************************
void mapi_mhl_handler(void)
{
    BYTE xdata ucPort = _mapi_mhl_InputToPortMapping(MHL_CURRENT_PORT);
        
    if(mdrv_mhl_CableDetect(ucPort))
    {
        if(GET_MHL_CBUS_CONNECT())
        {
#if(MHL_CBUS_OPERATION_MODE == MHL_CBUS_SW_POLLING_MODE)
            mdrv_mhl_CbusIntCB();
            mdrv_mhl_CbusIntHandler();
#elif(MHL_CBUS_OPERATION_MODE == MHL_CBUS_HW_REPLY_MODE)
            mdrv_mhl_CbusIntCB();
#endif
            _mapi_mhl_RxHandler();

            _mapi_mhl_ReadDeviceInformation();
        }

        mdrv_mhl_CbusTimerHandler();
        
        _mapi_mhl_TxHandler();
    }
}

//**************************************************************************
//  [Function Name]:
//                  mapi_mhl_SourceChange
//  [Description]
//                  
//  [Arguments]:
//                  
//  [Return]:
//                  
//**************************************************************************
void mapi_mhl_SourceChange(void)
{
    BYTE xdata ucPort = _mapi_mhl_InputToPortMapping(MHL_CURRENT_PORT);
    
    if(ucPort != GET_MHL_PORT_SELECT())
    {
        mdrv_mhl_SourceChange(ucPort);
    }
}

//**************************************************************************
//  [Function Name]:
//                  mapi_mhl_PowerCtrl
//  [Description]
//                  MHL power control
//  [Arguments]:
//                  
//  [Return]:
//                  
//**************************************************************************
void mapi_mhl_PowerCtrl(BYTE ucState)
{
    if(ucState != GET_MHL_POWER_STSTUS())
    {
        mdrv_mhl_PowerControl(ucState);
    }
}

#if(MHL_CTS_SEND_MSC)
//**************************************************************************
//  [Function Name]:
//                  mapi_mhl_CbusSendCommand()
//  [Description]
//                  For CTS send a MSC command
//  [Arguments]:
//                  
//  [Return]:
//                  
//**************************************************************************
void mapi_mhl_CbusSendCommand(BYTE ucSelect)
{
    BYTE xdata ucData[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

    #if ENABLE_MHL_CTS_TEST
    static BYTE RCP_RELEASE_CMD=0,RCP_RELEASE_CNT=0;;
    #endif
    
    switch(ucSelect)
    {
        case CBUS_COMMAND_CTS:
            mdrv_mhl_CbusSendCommand(CBUS_DCAP_CHG_FLAG);
            break;

        case CBUS_COMMAND_RCP:
            if(GET_MHL_PATH_EN_FLAG())
            {
                ucData[1] = 1; // send command
#if ENABLE_MHL_CTS_TEST
                if(IS_MENU_KEY_PRESSED() || IS_EXIT_KEY_PRESSED() || IS_DOWN_KEY_PRESSED() || IS_PLUS_KEY_PRESSED())
                 {
                    if(IS_MENU_KEY_PRESSED())
                        ucData[0] = RCP_KEYID_ROOT_MENU;
                    else if(IS_EXIT_KEY_PRESSED())
                        ucData[0] = RCP_KEYID_EXIT;
                    else if(IS_DOWN_KEY_PRESSED())
                        ucData[0] = RCP_KEYID_DOWN;
                    else if(IS_PLUS_KEY_PRESSED())
                        ucData[0] = RCP_KEYID_UP;

                    RCP_RELEASE_CMD = ucData[0]|BIT7;
                    RCP_RELEASE_CNT = 5;
                 }
                else if(RCP_RELEASE_CNT)
                {
                    RCP_RELEASE_CNT--;
                    if(IS_REPEAT_KEY_PRESSED())
                    {
                        ucData[1] = 0;// don't send command
                        if(RCP_RELEASE_CNT == 0)
                            RCP_RELEASE_CNT =1; //wait nothing key to send release RCP key
                    }
                    else if(IS_NOTHING_KEY_PRESSED())
                    {
                        if(RCP_RELEASE_CNT == 0)
                            ucData[0] = RCP_RELEASE_CMD;
                        else
                            ucData[1] = 0;// don't send command
                     }
                    else
                    {
                        ucData[1] = 0;// don't send command
                        RCP_RELEASE_CMD = 0;
                    }
                }
                else
                 {
                         ucData[1] = 0; // don't send command
                  }

#endif
                if(ucData[1])
                mdrv_mhl_CbusMscMsgSubCmdSend(MSC_MSG_RCP, ucData[0]);
            }
            break;

        case CBUS_COMMAND_RAP:
            if(GET_MHL_PATH_EN_FLAG())
            {
                mdrv_mhl_CbusMscMsgSubCmdSend(MSC_MSG_RAP, MSC_RAP_POLL);
            }
            break;

        case CBUS_COMMAND_UCP:
            if(GET_MHL_PATH_EN_FLAG())
            {
                mdrv_mhl_CbusMscMsgSubCmdSend(MSC_MSG_UCP, 0);
            }
            break;
            
        case CBUS_COMMAND_WRITE_BURST:
            if(GET_MHL_PATH_EN_FLAG())
            {
                mdrv_mhl_CbusWriteBurst(0x40, 16, &ucData);
            }
            break;
            
        default:
            
            break;
    };
}

#endif

#if MS_PM
//**************************************************************************
//  [Function Name]:
//                  _mapi_mhl_PortToInputMapping
//  [Description]
//                  
//  [Arguments]:
//                  
//  [Return]:
//                  
//**************************************************************************
BYTE _mapi_mhl_PortToInputMapping(BYTE ucPort)
{
    if(ucPort == MHL_INPUT_PORT0)
    {
        ucPort = MHL_SELECT_PORT0;
    }
    else if(ucPort == MHL_INPUT_PORT1)
    {
        ucPort = MHL_SELECT_PORT1;
    }
    else if(ucPort == MHL_INPUT_PORT2)
    {
        ucPort = MHL_SELECT_PORT2;
    }
    else
    {
        ucPort = MHL_INPUT_NONE;
    }

    return ucPort;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_mhl_WakeupDetect
//  [Description]
//                  
//  [Arguments]:
//                  
//  [Return]:
//                  
//**************************************************************************
Bool mapi_mhl_WakeupDetect(Bool bAutoSearch, BYTE *ucPort)
{
    Bool bFlag = FALSE;

    if(mdrv_mhl_WakeupDetect(bAutoSearch))
    {
        *ucPort = _mapi_mhl_PortToInputMapping(GET_MHL_PORT_SELECT());

        bFlag = TRUE;
    }
    
    return bFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_mhl_ChargePortDetect
//  [Description]
//                  
//  [Arguments]:
//                  
//  [Return]:
//                  
//**************************************************************************
void mapi_mhl_ChargePortDetect(void)
{
    mdrv_mhl_ChargePortDetect(FALSE);
}

#endif

#if(MHL_HW_RTERM_SUPPORT)
//**************************************************************************
//  [Function Name]:
//                  mapi_mhl_RtermControlHWMode()
//  [Description]
//                  
//  [Arguments]:
//
//  [Return]:
//                  
//**************************************************************************
void mapi_mhl_RtermControlHWMode(Bool bFlag)
{
    mdrv_mhl_RtermControlHWMode(bFlag);
}

#endif

#if(MHL_RCP_FUNCTION_SUPPORT)
//**************************************************************************
//  [Function Name]:
//                  mapi_mhl_SendRCPCommand
//  [Description]
//                  MHL Cbus MSC Send RCP Command
//  [Arguments]:
//
//  [Return]:
//                  TRUE: success
//                  FALSE: fail
//**************************************************************************
Bool mapi_mhl_SendRCPCommand(BYTE ucKeyCode)
{
    return mdrv_mhl_CbusMscMsgSubCmdSend(MSC_MSG_RCP, ucKeyCode);
}

#endif

#if(MHL_RAP_FUNCTION_SUPPORT)
//**************************************************************************
//  [Function Name]:
//                  mapi_mhl_SendRAPCommand
//  [Description]
//                  MHL Cbus MSC Send RAP Command
//  [Arguments]:
//
//  [Return]:
//                  TRUE: success
//                  FALSE: fail
//**************************************************************************
Bool mapi_mhl_SendRAPCommand(BYTE ucKeyCode)
{
    return mdrv_mhl_CbusMscMsgSubCmdSend(MSC_MSG_RAP, ucKeyCode);
}

#endif

#if(MHL_UCP_FUNCTION_SUPPORT)
//**************************************************************************
//  [Function Name]:
//                  mapi_mhl_SendUCPCommand
//  [Description]
//                  MHL Cbus MSC Send UCP Command
//  [Arguments]:
//
//  [Return]:
//                  TRUE: success
//                  FALSE: fail
//**************************************************************************
Bool mapi_mhl_SendUCPCommand(BYTE ucKeyCode)
{
    return mdrv_mhl_CbusMscMsgSubCmdSend(MSC_MSG_UCP, ucKeyCode);
}

#endif

#if(MHL_WRITE_BURST_SUPPORT)
//**************************************************************************
//  [Function Name]:
//                  mapi_mhl_SendWriteBurst
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//                  TRUE: success
//                  FALSE: fail
//**************************************************************************
Bool mapi_mhl_SendWriteBurst(BYTE ucLength, BYTE *ucData)
{
    return mdrv_mhl_CbusWriteBurst(0x40, *ucLength, ucData);
}

#endif

#if(MHL_HID_FUNCTION_SUPPORT)
//**************************************************************************
//  [Function Name]:
//                  mapi_mhl_HIDSentWriteBurst
//  [Description]
//                  
//  [Arguments]:
//                  
//  [Return]:
//                  
//**************************************************************************
Bool mapi_mhl_HIDSentWriteBurst(Bool bReset, Bool bReply, BYTE *ucLength, BYTE *ucData)
{
    Bool bFinish = FALSE;
    BYTE xdata uctemp = 0;

    if(bReset)
    {
        CLR_MHL_HID_SEND_REQUEST();
        CLR_MHL_HID_RECEIVE_REPLY();
    }
    else
    {
        if(bReply)
        {
            if(GET_MHL_HID_RECEIVE_REPLY())
            {
                bFinish = TRUE;

                CLR_MHL_HID_RECEIVE_REPLY();

                *ucLength = GET_MHL_WRITE_BURST_LENGTH();

                for(uctemp = 0; uctemp < GET_MHL_WRITE_BURST_LENGTH(); uctemp++)
                {
                    ucData[uctemp] = GET_MHL_WRITE_BURST_DATA(uctemp);
                }

                SET_MHL_WRITE_BURST_LENGTH(0);
            }
            else if(!GET_MHL_HID_SEND_REQUEST())
            {
                if(mdrv_mhl_CbusWriteBurst(0x40, *ucLength, ucData))
                {
                    SET_MHL_HID_SEND_REQUEST();
                }
            }
        }
        else
        {
            bFinish = mdrv_mhl_CbusWriteBurst(0x40, *ucLength, ucData);
        }
    }

    return bFinish;
}

#endif

/*
#else
BYTE code msMapiMHLNullData[] = {0};

void msMapiMHLDummy(void)
{
    BYTE xdata i = msMapiMHLNullData[0];
}
*/
#endif // #if ENABLE_MHL

#endif // _MAPI_MHL_C_

