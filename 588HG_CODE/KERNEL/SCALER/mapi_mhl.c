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
#include "mapi_mhl.h"

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define MHL_DBG     1

#if MHL_DBG && ENABLE_DEBUG
#define MHL_DPUTSTR(str)        printMsg(str)
#define MHL_DPRINTF(str, x)     printData(str, x)
#else
#define MHL_DPUTSTR(str)
#define MHL_DPRINTF(str, x)
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

code BYTE Mstar_MHL_EDID[256] =
{
#if 0 // 120321 coding test, tpv htc edid test
    0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x3E,0x93,0x29,0x00,0x55,0x31,0x30,0x30,
    0x1B,0x14,0x01,0x03,0x80,0x3C,0x22,0x78,0xEE,0x2C,0xC5,0xA5,0x55,0x54,0xA1,0x27,
    0x0C,0x50,0x54,0xA5,0x4B,0x00,0x71,0x4F,0x81,0x80,0xD1,0xC0,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x02,0x3A,0x80,0x18,0x71,0x38,0x2D,0x40,0x58,0x2C,
    0x45,0x00,0x56,0x50,0x21,0x00,0x00,0x1E,0x00,0x00,0x00,0x0C,0x00,0x01,0xFE,0x44,
    0x73,0x10,0x00,0x1F,0x00,0x03,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0xFC,0x00,0x4F,
    0x54,0x53,0x20,0x32,0x37,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xFD,
    0x00,0x38,0x4C,0x1E,0x53,0x11,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x01,0xF5,
    0x02,0x03,0x20,0xF0,0x4D,0x90,0x05,0x04,0x03,0x02,0x07,0x16,0x01,0x14,0x1F,0x12,
    0x13,0x22,0x23,0x09,0x07,0x07,0x83,0x01,0x00,0x00,0x65,0x03,0x0C,0x00,0x10,0x00,
    0x01,0x1D,0x80,0x18,0x71,0x38,0x2D,0x40,0x58,0x2C,0x45,0x00,0x56,0x50,0x21,0x00,
    0x00,0x1E,0x01,0x1D,0x80,0x18,0x71,0x1C,0x16,0x20,0x58,0x2C,0x25,0x00,0x56,0x50,
    0x21,0x00,0x00,0x9E,0x01,0x1D,0x00,0x72,0x51,0xD0,0x1E,0x20,0x6E,0x28,0x55,0x00,
    0x56,0x50,0x21,0x00,0x00,0x1E,0x8C,0x0A,0xD0,0x8A,0x20,0xE0,0x2D,0x10,0x10,0x3E,
    0x96,0x00,0x56,0x50,0x21,0x00,0x00,0x18,0x02,0x3A,0x80,0x18,0x71,0x38,0x2D,0x40,
    0x58,0x2C,0x45,0x00,0x56,0x50,0x21,0x00,0x00,0x1E,0x00,0x00,0x00,0x00,0x00,0xDE,
#else
    0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x36,0x74,0x30,0x00,0x01,0x00,0x00,0x00,
    0x18,0x0F,0x01,0x03,0x80,0x73,0x41,0x78,0x0A,0xCF,0x74,0xA3,0x57,0x4C,0xB0,0x23,
    0x09,0x48,0x4C,0x00,0x00,0x00,0x01,0x01,0x01,0xFF,0x01,0xFF,0xFF,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x20,0x01,0x1D,0x00,0x72,0x51,0xD0,0x1E,0x20,0x6E,0x28,
    0x55,0x00,0xC4,0x8E,0x21,0x00,0x00,0x1E,0x01,0x1D,0x80,0x18,0x71,0x1C,0x16,0x20,
    0x58,0x2C,0x25,0x00,0xC4,0x8E,0x21,0x00,0x00,0x9E,0x00,0x00,0x00,0xFC,0x00,0x4D,
    0x53,0x74,0x61,0x72,0x20,0x44,0x65,0x6D,0x6F,0x0A,0x20,0x20,0x00,0x00,0x00,0xFD,
    0x00,0x3B,0x3C,0x1F,0x2D,0x08,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x01,0x25,
    0x02,0x03,0x1D,0xF2,0x4A,0x05,0x84,0x03,0x02,0x01,0x12,0x93,0x14,0x16,0x07,0x23,
    0x09,0x07,0x07,0x83,0x01,0x00,0x00,0x65,0x03,0x0C,0x00,0x10,0x00,0x8C,0x0A,0xD0,
    0x8A,0x20,0xE0,0x2D,0x10,0x10,0x3E,0x96,0x00,0xC4,0x8E,0x21,0x00,0x00,0x18,0x8C,
    0x0A,0xA0,0x14,0x51,0xF0,0x16,0x00,0x26,0x7C,0x43,0x00,0xC4,0x8E,0x21,0x00,0x00,
    0x99,0x01,0x1D,0x00,0xBC,0x52,0xD0,0x1E,0x20,0xB8,0x28,0x55,0x40,0xC4,0x8E,0x21,
    0x00,0x00,0x1F,0x01,0x1D,0x80,0xD0,0x72,0x1C,0x16,0x20,0x10,0x2C,0x25,0x80,0xC4,
    0x8E,0x21,0x00,0x00,0x9E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x68,
#endif    
};

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

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
    BYTE statusCode = MSC_RCP_NO_ERROR;

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
    BYTE statusCode  = MSC_RAP_NO_ERROR;

    switch(ucRapCode)
    {
        case MSC_RAP_POLL:
            break;
            
        case MSC_RAP_CONTENT_ON:
            SC0_BLACK_WHITE_SCREEN_DISABLE(); // CTS 4.2.7.1
            break; 
            
        case MSC_RAP_CONTENT_OFF:
            SC0_BLACK_SCREEN_ENABLE(); // CTS 4.2.7.1
            break;
            
        default:
            statusCode = MSC_RAP_UNRECOGNIZED_ACTION_CODE;
            break;
    }
	
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
    BYTE status;

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
            
        default: // MSGE
            //MHL_DPUTSTR("Respond with MSC_MSGE");
            break;
    }
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

//**************************************************************************
//  [Function Name]:
//                  mapi_mhl_Initial()
//  [Description]
//                  MHL init
//  [Arguments]:
//                  
//  [Return]:
//
//**************************************************************************
void mapi_mhl_Initial(void)
{
    MHL_DPUTSTR("** MHL initial setting");
    mdrv_mhl_Initial(&Mstar_MHL_EDID);
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
//                  mapi_mhl_RxHandler()
//  [Description]
//                  MHL handler
//  [Arguments]:
//                  
//  [Return]:
//
//**************************************************************************
void mapi_mhl_RxHandler(void)
{
    BYTE ucCmd = 0;
    BYTE ucData = 0;
    
    if(mdrv_mhl_CbusRxHandler(&ucCmd, &ucData))
    {
        _mapi_mhl_CbusProcessMSCSubCmd(ucCmd, ucData);
    }
}

//**************************************************************************
//  [Function Name]:
//                  mapi_mhl_TxHandler()
//  [Description]
//                  MHL handler
//  [Arguments]:
//                  
//  [Return]:
//
//**************************************************************************
void mapi_mhl_TxHandler(void)
{
    if(mdrv_mhl_CbusStatus())
    {
        mdrv_mhl_CbusTxHandler();
    }

    mdrv_mhl_CbusConnectionCheck();
}

//**************************************************************************
//  [Function Name]:
//                  mapi_mhl_Handler()
//  [Description]
//                  MHL handler
//  [Arguments]:
//                  
//  [Return]:
//
//**************************************************************************
void mapi_mhl_Handler(BYTE ucPort)
{
    mdrv_mhl_InputSelect(ucPort);
    
    if(mdrv_mhl_CableDetect())
    {
        if(GET_MHL_CBUS_CONNECT())
        {
            mdrv_mhl_CbusIntCB();
            mdrv_mhl_CbusIntHandler();
            mapi_mhl_RxHandler();
        }

        mdrv_mhl_CbusTimerHandler();
        
        mapi_mhl_TxHandler();
    }
}

//**************************************************************************
//  [Function Name]:
//                  mapi_mhl_CbusLinkCheck
//  [Description]
//                  MHL Cbus Check Connect
//  [Arguments]:
//                  
//  [Return]:
//                  
//**************************************************************************
Bool mapi_mhl_CbusLinkCheck(void)
{
    Bool bindex = FALSE;

    if(GET_MHL_CBUS_CONNECT())
    {
        bindex = TRUE;

        if(GET_MHL_PORT_ON_FLAG())
        {
            bindex = FALSE;
        }
        else
        {
            SET_MHL_PORT_ON_FLAG();
            MHL_DPUTSTR("**MHL Reset Port Stable Counter");
        }
    }

    return bindex;
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
void mapi_mhl_PowerControl(BYTE ucState)
{
    if(ucState != GET_MHL_POWER_STSTUS())
    {
        mdrv_mhl_PowerControl(ucState);
    }
}

#if(DMHL_CTS_SEND_MSC)
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
    
    switch(ucSelect)
    {
        case CBUS_COMMAND_CTS:
            mdrv_mhl_CbusSendCommand(CBUS_DCAP_CHG_FLAG);
            break;

        case CBUS_COMMAND_RCP:
            if(GET_MHL_PATH_EN_FLAG())
            {
                mdrv_mhl_CbusMscMsgSubCmdSend(MSC_MSG_RCP, RCP_KEYID_UP);
            }
            break;

        case CBUS_COMMAND_RAP:
            if(GET_MHL_PATH_EN_FLAG())
            {
                mdrv_mhl_CbusMscMsgSubCmdSend(MSC_MSG_RAP, MSC_RAP_POLL);
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

#ifdef _MS_PM_
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
Bool mapi_mhl_WakeupDetect(void)
{
    return mdrv_mhl_WakeupDetect();
}
#endif

#else
BYTE code msMapiMHLNullData[] = {0};

void msMapiMHLDummy(void)
{
    BYTE xdata i = msMapiMHLNullData[0];
}

#endif // #if ENABLE_MHL

#endif // _MAPI_MHL_C_

