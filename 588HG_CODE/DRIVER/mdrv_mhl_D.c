///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    mdrv_mhl.c
/// @author MStar Semiconductor Inc.
/// @brief  MHL driver Function
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_MHL_C_
#define _MDRV_MHL_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "global.h"
#include <string.h>
/*
#include <math.h>
#include <string.h>
#include "board.h"
#include "types.h"
#include "ms_reg.h"
//#include "DebugMsg.h"
#include "global.h"
#include "misc.h"
#include "mapi_mhl.h"
#include "mdrv_mhl.h"
*/

#if ENABLE_MHL


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define MHL_DRV_DEBUG_MESSAGE       1

#if(ENABLE_DEBUG && MHL_DEBUG_MESSAGE && MHL_DRV_DEBUG_MESSAGE)
#define MHL_DRV_DPUTSTR(str)        printMsg(str)
#define MHL_DRV_DPRINTF(str, x)     printData(str, x)
#else
#define MHL_DRV_DPUTSTR(str)
#define MHL_DRV_DPRINTF(str, x)
#endif


//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
MHLInfo_S xdata gMHLInfo;
bit idata bCbusTimerFlag = FALSE;
WORD idata usCbusTimerCnt = 0;
BYTE xdata ucMHLPollingTimerCnt = 0;
BYTE xdata ucCableDetectTimerCnt = 0;
BYTE xdata ucHPDControlTimerCnt = 0;
BYTE xdata ucMHLCbusSelect = 0;
BYTE xdata ucMHLRxSRAM[MHL_RX_XRAM_SIZE_TOTAL] _at_ MHL_CBUS_SRAM_ADDRESS;
#if(MHL_PHASE_CHECK_PATCH)
bit idata bCDRTimerFlag = FALSE;
BYTE idata ucCDRTimerCnt = 0;
#endif
BYTE xdata ucMHL3DTotalEntryDTD = 0;
BYTE xdata ucMHL3DTotalEntryVIC = 0;
BYTE xdata ucMHL3DInformationDTD[MHL_3D_DTD_INFORMATION_LENGTH];
BYTE xdata ucMHL3DInformationVIC[MHL_3D_VIC_INFORMATION_LENGTH];
BYTE xdata ucMHLWriteBurstInformation[MHL_MSC_SCRATCHPAD_SIZE +2];
BYTE xdata ucMHLCellPhoneIDPort0 = CELL_PHONE_ID_NONE;
BYTE xdata ucMHLCellPhoneIDPort1 = CELL_PHONE_ID_NONE;
BYTE xdata ucMHLCellPhoneIDPort2 = CELL_PHONE_ID_NONE;

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
code BYTE tMHL_DEVCAP_TABLE[] =
{
    MHL_DEV_STATE_0x00,    //DEV_STATE
    MHL_VERSION_0x01,    //MHL_VERSION
    MHL_DEV_CAT_0x02,    //DEV_CAT, [4]:POW, [3:0]:DEV_TYPE
    MHL_ADOPTER_ID_H_0x03,    // ADOPTER_ID_H, Mstar adopter ID 715(0x2CB)
    MHL_ADOPTER_ID_L_0x04,    // ADOPTER_ID_L
    MHL_VID_LINK_MODE_0x05,    // VID_LINK_MODE, [5]:SUPP_VGA, [4]:ISLANDS, [3]:PPIXEL, [2]:YUV422, [1]:YUV444, [0]:RGB444
    MHL_AUD_LINK_MODE_0x06,    // AUD_LINK_MODE, [1]:AUD_8CH, [0]:AUD_2CH
    MHL_VIDEO_TYPE_0x07,    // VIDEO_TYPE, [7]: SUPP_VT, [3]:GAME, [2]:CINEMA, [1]: PHOTO, [0]:GRAPHICS
    MHL_LOG_DEV_MAP_0x08,    // LOG_DEV_MAP, [0]:LD_DISPLAY
    MHL_BANDWIDTH_0x09,    // BANDWIDTH, 15*5MHz = 75MHz
    MHL_FEATURE_FLAG_0x0A,    // FEATURE_FLAG, [2]:Scratchpad, [1]:RAP, [0]:RCP
    MHL_DEVICE_ID_H_0x0B,    // DEVICE_ID_H, ? CHIP_ID
    MHL_DEVICE_ID_L_0x0C,    // DEVICE_ID_L, ? CHIP_ID
    MHL_SCRATCHPAD_SIZE_0x0D,    // SCRATCHPAD_SIZE, 16 bytes
    MHL_INT_STAT_0x0E,    // INT_STAT_SIZE, [7:4]:Status 4 bytes, [3:0]:Interrupt 4 bytes
    0x00,    // Reserved
};

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_SetControlTimer()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _mdrv_mhl_SetControlTimer(BYTE ucTime)
{
    if(ucTime == 0)
    {
        CLR_MHL_TIME_OUT_FLAG();
    }
    else
    {
        SET_MHL_TIME_OUT_FLAG();
    }

    SET_MHL_TIMEOUT_TIMER(ucTime);
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_GetCellPhoneID
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE _mdrv_mhl_GetCellPhoneID(void)
{
    BYTE xdata ucCellPhoneID = CELL_PHONE_ID_NONE;

    switch(GET_MHL_CBUS_SELECT_PATH())
    {
#if(MHL_SUPPORT_PORT0)
        case MHL_CBUS_SELECT_PORT0:
            ucCellPhoneID = GET_MHL_CELL_PHONE_ID_PORT0();
            break;
#endif

#if(MHL_SUPPORT_PORT1)
        case MHL_CBUS_SELECT_PORT1:
            ucCellPhoneID = GET_MHL_CELL_PHONE_ID_PORT1();
            break;
#endif

#if(MHL_SUPPORT_PORT2)
        case MHL_CBUS_SELECT_PORT2:
            ucCellPhoneID = GET_MHL_CELL_PHONE_ID_PORT2();
            break;
#endif

        default:

            break;
    };

    return ucCellPhoneID;
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_CbusGetLeastFlag()
//  [Description]:
//                  MHL Cbus get the least True flag in index
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE _mdrv_mhl_CbusGetLeastFlag(BYTE unindex)
{
    BYTE xdata uctemp = BIT0;

    if(unindex == 0)
    {
        return unindex;
    }

    while(!(unindex &uctemp))
    {
        uctemp = uctemp <<1;
    }

    return uctemp;
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_GetLeastBitNum()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE _mdrv_mhl_GetLeastBitNum(WORD usData)
{
    BYTE xdata uctemp = 0;

    while(!(usData &BIT0))
    {
        usData = usData >>1;
        uctemp++;
    }

    return uctemp;
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_CbusLinkCheck()
//  [Description]
//                  MHL Cbus handle send MSC message
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _mdrv_mhl_CbusLinkCheck(void)
{
    BYTE xdata ucindex = GET_CBUS_LINK_CHECK() ^MHL_CBUS_LINK_MASK;

    if(ucindex > 0)
    {
        SET_CBUS_LINK_CHECK(GET_CBUS_LINK_CHECK() |_mdrv_mhl_CbusGetLeastFlag(ucindex));

#if(!MHL_READ_DEVICE_CAPABILITY)
        MHL_DRV_DPUTSTR("**MHL Cbus Receive ACK for Link");
#endif
    }
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_CbusQueueMscRxReply(BYTE subCmd, BYTE databuf)
//  [Description]
//                  MHL Cbus Reply MSC RCP or RAP into Queue
//  [Arguments]:
//                  subCmd: MSC subcommands
//                  databuf: command code
//  [Return]:
//                  TRUE: success
//                  FALSE: fail
//**************************************************************************
Bool _mdrv_mhl_CbusQueueMscRxReply(BYTE subCmd, BYTE databuf)
{
    CbusReq_S xdata req = {0};

    req.cmd = MSC_MSC_MSG;
    req.msgData[0] = subCmd;
    req.msgData[1] = databuf;
    req.len = 2;
    req.reqStatus = CBUS_REQ_PENDING;

    memcpy(&(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX)), &req, sizeof(CbusReq_S));

    return TRUE;
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_CbusQueueMscTxRequest()
//  [Description]
//                  MHL Cbus Request a MSC Cmd into Queue
//  [Arguments]:
//                  ucIndex: Current queue index
//                  pReq: Command buffer
//  [Return]:
//                  TRUE: success
//                  FALSE: fail
//**************************************************************************
Bool _mdrv_mhl_CbusQueueMscTxRequest(BYTE ucIndex, CbusReq_S *pReq)
{
    BYTE xdata uctemp = ucIndex;

    // Search free queue
    for(uctemp = 0; uctemp <(MHL_CBUS_TOTAL_QUEUE_NUM -1); uctemp++)
    {
        if(GET_MHL_QUEUE_STATE((ucIndex +uctemp +1) %(MHL_CBUS_TOTAL_QUEUE_NUM -1)) == CBUS_REQ_IDLE)
        {
            uctemp = (ucIndex +uctemp +1) %(MHL_CBUS_TOTAL_QUEUE_NUM -1);

            break;
        }
    }

    if(uctemp == (MHL_CBUS_TOTAL_QUEUE_NUM -1)) // No free queue
    {
        return FALSE;
    }

    if(GET_MHL_QUEUE_STATE((uctemp +MHL_CBUS_TOTAL_QUEUE_NUM -2) %(MHL_CBUS_TOTAL_QUEUE_NUM -1)) == CBUS_REQ_IDLE)
    {
        uctemp = (uctemp +MHL_CBUS_TOTAL_QUEUE_NUM -2) %(MHL_CBUS_TOTAL_QUEUE_NUM -1);
    }

    memcpy(&(GET_MHL_CBUS_QUEUE(uctemp)), pReq, sizeof(CbusReq_S));

    return TRUE;
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_CbusWriteStatusOrSetInt()
//  [Description]:
//                  MHL Cbus write status or set interrupt
//  [Arguments]:
//
//  [Return]:
//                  TRUE: Cbus message send successfully
//                  FALSE: Cbus message send fail
//**************************************************************************
Bool _mdrv_mhl_CbusWriteStatusOrSetInt(BYTE ucAddr, BYTE ucValue)
{
    CbusReq_S xdata stQueue;

    stQueue.cmd = MSC_WRITE_STAT_OR_SET_INT;
    stQueue.msgData[0] = ucAddr;
    stQueue.msgData[1] = ucValue;
    stQueue.len = 2;
    stQueue.reqStatus = CBUS_REQ_PENDING;

    if(!_mdrv_mhl_CbusQueueMscTxRequest(GET_MHL_TX_QUEUE_INDEX(), &stQueue))
    {
        return FALSE;
    }

    return TRUE;
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_CbusSetPacket()
//  [Description]
//                  MHL Cbus Packet setting
//  [Arguments]:
//                  ucHeader: Packet header type
//                  bControl: Packet control bit type
//                  ucContent: Packet payload
//  [Return]:
//                  Packet word for register write
//**************************************************************************
WORD _mdrv_mhl_CbusSetPacket(BYTE ucHeader, Bool bControl, BYTE ucContent)
{
    Bool bParity = 0;
    WORD xdata usPacket = (ucHeader <<9) |(bControl <<8) |ucContent;

    do
    {
        bParity ^= (usPacket & BIT0);
        usPacket = usPacket >>1;
    }
    while(usPacket);

    usPacket = (bParity <<11) |(ucHeader <<9) |(bControl <<8) |ucContent;

    return (usPacket |BIT15);
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_CbusRxReply()
//  [Description]
//                  MHL Cbus Response
//  [Arguments]:
//                  ctrlpky: ACK / NACK / ABORT
//                  bflag: TRUE means ACK with Tx required data. Use in "MSC_READ_DEVCAP", "MSC_GET_STATE" and "MSC_GET_VENDOR_ID"
//                  databuf: Tx required data
//  [Return]:
//                  TRUE: success
//                  FALSE: fail
//**************************************************************************
Bool _mdrv_mhl_CbusRxReply(BYTE ucCommand, Bool bFlag, BYTE ucData)
{
    Bool bindex = TRUE;
    mhalCbusFifo_S xdata pCbusFifo;

    switch(ucCommand)
    {
        case MSC_ACK:
            pCbusFifo.lens = bFlag ? 2 : 1;
            pCbusFifo.databuf[0] = _mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_CONTROL, MSC_ACK);

            if(bFlag)
            {
                pCbusFifo.databuf[1] = _mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_DATA, ucData);
            }

            break;

        case MSC_NACK:
            pCbusFifo.lens = 1;
            pCbusFifo.databuf[0] = _mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_CONTROL, MSC_NACK);

            break;

        case MSC_ABORT:
            pCbusFifo.lens = 1;
            pCbusFifo.databuf[0] = _mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_CONTROL, MSC_ABORT);
            SET_MHL_RECEIVE_ERROR();

            break;

        case MSC_GET_STATE:
        case MSC_GET_VENDOR_ID:
        case MSC_GET_DDC_ERRORCODE:
        case MSC_GET_MSC_ERRORCODE:
            pCbusFifo.lens = 1;
            pCbusFifo.databuf[0] = _mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_DATA, ucData);

            break;

        default:
            bindex = FALSE;
            break;
    }

    if(!mhal_mhl_CBusWrite(&pCbusFifo))
    {
        MHL_DRV_DPRINTF("@@Cbus Tx is busy = 0x%x", ucCommand);
        return FALSE;
    }

    return bindex;
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_CBusTxWriteCmd()
//  [Description]:
//                  MHL Cbus send command
//  [Arguments]:
//
//  [Return]:
//                  TRUE: Cbus command send successfully
//                  FALSE: Cbus command send fail
//**************************************************************************
Bool _mdrv_mhl_CBusTxWriteCmd(CbusReq_S *pReq)
{
    Bool xdata bindex = TRUE;
    BYTE xdata uctemp = 0;
    mhalCbusFifo_S xdata pCbusFifo;

    // payload 1
    pCbusFifo.databuf[0] = _mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_CONTROL, pReq->cmd);

    switch(pReq->cmd)
    {
        case MSC_ACK:
            pCbusFifo.lens = pReq->len;

            if(pReq->len == 2)
            {
                // payload 2
                pCbusFifo.databuf[1] = _mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_DATA, pReq->msgData[0]);
            }
            break;

        case MSC_NACK:
            pCbusFifo.lens = 1;
            break;

        case MSC_ABORT:
            pCbusFifo.lens = 1;
            break;

        case MSC_WRITE_STAT_OR_SET_INT:   // 0x60
            // payload 2
            pCbusFifo.databuf[1] = _mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_DATA, pReq->msgData[0]);
            // payload 3
            pCbusFifo.databuf[2] = _mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_DATA, pReq->msgData[1]);
            // message length
            pCbusFifo.lens = 3;
            break;

        case MSC_READ_DEVCAP:   // 0x61
            // payload 2
            pCbusFifo.databuf[1] = _mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_DATA, pReq->msgData[0]);
            // message length
            pCbusFifo.lens = 2;
            break;

        case MSC_GET_STATE:          // 0x62
        case MSC_GET_VENDOR_ID:      // 0x63
        case MSC_SET_HPD:            // 0x64
        case MSC_CLR_HPD:            // 0x65
        case MSC_GET_SC1_ERRORCODE:      // 0x69 - Get channel 1 command error code
        case MSC_GET_DDC_ERRORCODE:      // 0x6A - Get DDC channel command error code.
        case MSC_GET_MSC_ERRORCODE:      // 0x6B - Get MSC command error code.
        case MSC_GET_SC3_ERRORCODE:      // 0x6D - Get channel 3 command error code.
            // message length
            pCbusFifo.lens = 1;
            break;

        case MSC_MSC_MSG: // 0x68
            // payload 2
            pCbusFifo.databuf[1] = _mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_DATA, pReq->msgData[0]);
            // payload 3
            pCbusFifo.databuf[2] = _mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_DATA, pReq->msgData[1]);
            // message length
            pCbusFifo.lens = 3;
            break;

        case MSC_WRITE_BURST: // 0x6C
            // payload 2 ~ 17
            for(uctemp =0; uctemp <pReq->len; uctemp++) // offset packet + the maxi 16 packets
            {
                pCbusFifo.databuf[uctemp +1] = _mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_DATA, pReq->msgData[uctemp]);
            }
            // EOF
            pCbusFifo.databuf[pReq->len +1] = _mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_CONTROL, MSC_EOF);
            // message length
            pCbusFifo.lens = pReq->len +2;
            break;

        default:
            bindex = FALSE;
            break;
    }

    if(bindex)
    {
        if(!mhal_mhl_CBusWrite(&pCbusFifo))
        {
            MHL_DRV_DPRINTF("@@Cbus Tx is busy = 0x%x", pReq->cmd);
            return FALSE;
        }
    }

    return bindex;
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_Cbus_SetDCapRdy()
//  [Description]:
//                  MHL Cbus set capbility ready
//  [Arguments]:
//                  TRUE: Enable
//                  FALSE: Disable
//  [Return]:
//
//**************************************************************************
void _mdrv_mhl_Cbus_SetDCapRdy(void)
{
    Bool bindex = _mdrv_mhl_CbusWriteStatusOrSetInt(0x30, BIT0);

    if(bindex)
    {
        SET_MHL_DCAP_RDY_FLAG();
    }
    else
    {
        CLR_MHL_DCAP_RDY_FLAG();
    }
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_Cbus_SetDCapChg()
//  [Description]:
//                  MHL Cbus set path enable
//  [Arguments]:
//                  TRUE: Enable
//                  FALSE: Disable
//  [Return]:
//
//**************************************************************************
void _mdrv_mhl_Cbus_SetDCapChg(void)
{
    Bool bindex = _mdrv_mhl_CbusWriteStatusOrSetInt(0x20, BIT0);

    if(bindex)
    {
        SET_MHL_DCAP_CHG_FLAG();
    }
    else
    {
        CLR_MHL_DCAP_CHG_FLAG();
    }
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_Cbus_SetPathEn()
//  [Description]:
//                  MHL Cbus set path enable
//  [Arguments]:
//                  TRUE: Enable
//                  FALSE: Disable
//  [Return]:
//
//**************************************************************************
void _mdrv_mhl_Cbus_SetPathEn(Bool bFlag)
{
    Bool bindex = _mdrv_mhl_CbusWriteStatusOrSetInt(0x31, bFlag? BIT3: 0);

    if(bFlag && bindex)
    {
        SET_MHL_PATH_EN_FLAG();
    }
    else
    {
        CLR_MHL_PATH_EN_FLAG();
    }

    mhal_mhl_Cbus_SetPathEn(GET_MHL_PATH_EN_FLAG());
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_CBusProcessMSCSubCmd()
//  [Description]:
//                  MHL Cbus process MSC sub command
//  [Arguments]:
//                  cmd: command
//                  databuf: data
//  [Return]:
//
//**************************************************************************
Bool _mdrv_mhl_CBusProcessMSCSubCmd(BYTE ucCmd, BYTE ucData)
{
    Bool bindex = TRUE;

    switch(ucCmd)
    {
        case MSC_MSG_RCP:
        case MSC_MSG_RAP:
        case MSC_MSG_UCP:
            //SET_MHL_QUEUE_STATE(MHL_CBUS_RX_QUEUE_INDEX, CBUS_REQ_RECEIVED);
            SET_MHL_CBUS_STATE(CBUS_STATE_RECEIVED);
            SET_MHL_RX_MSG_COMMAND(ucCmd);
            SET_MHL_RX_MSG_DATA(ucData);
            SET_MHL_RECEIVE_RCP();
            SET_MHL_RECEIVE_RAP();

            break;

        case MSC_MSG_RCPK:
        case MSC_MSG_RCPE:
        case MSC_MSG_RAPK:
            SET_MHL_SEND_MSG_FINISH();
            break;

        default:
            bindex = FALSE;
            break;
    };

    return bindex;
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_ClockModeSwitch()
//  [Description]:
//                  MHL Cbus clock mode select
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _mdrv_mhl_ClockModeSwitch(BYTE ucSelect)
{
    Bool bPPmode = FALSE;

    if(GET_MHL_INPUT_CLOCK_MODE() != ucSelect)
    {
        if(ucSelect == CBUS_CLOCK_PACKET_PIXEL)
        {
            bPPmode = TRUE;

            MHL_DRV_DPUTSTR("** MHL switch to PPmode!!!!\n");
        }
        else if(ucSelect == CBUS_CLOCK_NORMAL_24BIT)
        {
            MHL_DRV_DPUTSTR("** MHL switch back to 24 BIT mode!!!!\n");
        }

        mhal_mhl_ClockModeSwitchProc(GET_MHL_PORT_SELECT(), bPPmode);

        SET_MHL_INPUT_CLOCK_MODE(ucSelect);
    }
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_GetReadData
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool _mdrv_mhl_GetReadData(BYTE *ucData)
{
    Bool bindex = FALSE;

    if(GET_MHL_SEND_FINISH())
    {
        CLR_MHL_SEND_FINISH();

        *ucData = GET_MHL_CBUS_ASK_DATA();

        bindex = TRUE;
    }

    return bindex;
}

#if(MHL_READ_DEVICE_CAPABILITY)
//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_SendReadDevcap
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool _mdrv_mhl_SendReadDevcap(BYTE ucAddr)
{
    CbusReq_S xdata stQueue;

    if(GET_MHL_SEND_READ() ||GET_MHL_SEND_FINISH())
    {
        return FALSE;
    }

    stQueue.cmd = MSC_READ_DEVCAP;
    stQueue.msgData[0] = ucAddr;
    stQueue.len = 1;
    stQueue.reqStatus = CBUS_REQ_PENDING;

    return (_mdrv_mhl_CbusQueueMscTxRequest(GET_MHL_TX_QUEUE_INDEX(), &stQueue));
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_ReadDeviceCapacibility
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool _mdrv_mhl_ReadDeviceCapacibility(BYTE ucAddr, BYTE *ucData)
{
    Bool bindex = FALSE;

    if(GET_MHL_RECEIVE_DEVCAP())
    {
        if(_mdrv_mhl_GetReadData(ucData))
        {
            bindex = TRUE;
        }
        else if(_mdrv_mhl_SendReadDevcap(ucAddr))
        {
            SET_MHL_SEND_READ();
        }
    }

    return bindex;
}

#endif

#if(MHL_READ_DEVICE_VENDER_ID)
//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_SendReadVenderID
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool _mdrv_mhl_SendReadVenderID(void)
{
    CbusReq_S xdata stQueue;

    if(GET_MHL_SEND_READ() ||GET_MHL_SEND_FINISH())
    {
        return FALSE;
    }

    stQueue.cmd = MSC_GET_VENDOR_ID;
    stQueue.len = 0;
    stQueue.reqStatus = CBUS_REQ_PENDING;

    return (_mdrv_mhl_CbusQueueMscTxRequest(GET_MHL_TX_QUEUE_INDEX(), &stQueue));
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_ReadVenderID
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool _mdrv_mhl_ReadVenderID(BYTE *ucData)
{
    Bool bindex = FALSE;

    if(GET_MHL_RECEIVE_DEVCAP())
    {
        if(_mdrv_mhl_GetReadData(ucData))
        {
            bindex = TRUE;
        }
        else if(_mdrv_mhl_SendReadVenderID())
        {
            SET_MHL_SEND_READ();
        }
    }

    return bindex;
}

#endif

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_GetCableDetectPort
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool _mdrv_mhl_GetCableDetectPort(BYTE ucPort)
{
    return GET_MHL_CABLE_DETECT_PORT(mhal_mhl_PortToCbusMapping(ucPort));
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_SearchNextCbusSelect
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE _mdrv_mhl_SearchNextCbusSelect(BYTE ucCbusSelect)
{
    BYTE xdata uctemp = (ucCbusSelect +1) %MHL_CBUS_SELECT_MASK;

    for(; uctemp <(ucCbusSelect +MHL_CBUS_SELECT_MASK); uctemp++)
    {
        if(GET_MHL_CABLE_DETECT_PORT(uctemp %MHL_CBUS_SELECT_MASK))
        {
            ucCbusSelect = uctemp %MHL_CBUS_SELECT_MASK;

            break;
        }
    }

    return ucCbusSelect;
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_SetNextCbusSelect()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool _mdrv_mhl_SetNextCbusSelect(BYTE ucCbusSelect)
{
    Bool bindex = FALSE;
    static BYTE xdata ucNextCbusSelect = MHL_CBUS_SELECT_NONE;

    if(GET_MHL_CBUS_SELECT_PATH() == ucCbusSelect)
    {
        if(GET_MHL_POWER_ON_FLAG())
        {
            if(!GET_MHL_POLLING_TIMER())
            {
                if(ucNextCbusSelect != MHL_CBUS_SELECT_NONE)
                {
                    SET_MHL_CBUS_SELECT_PATH(ucNextCbusSelect);

                    mhal_mhl_CbusSwitchMux(GET_MHL_CBUS_SELECT_PATH());
                }

                SET_MHL_POLLING_TIMER(MHL_AOUT_SCAN_PORT_INTERVAL);

                CLR_MHL_POWER_ON_FLAG();
            }
            else
            {
                bindex = TRUE;
            }
        }
    }
    else
    {
        ucNextCbusSelect = ucCbusSelect;

        SET_MHL_POWER_ON_FLAG();
        SET_MHL_POLLING_TIMER(2);
    }

    return bindex;
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_CheckInputCbusSelect
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool _mdrv_mhl_CheckInputCbusSelect(BYTE ucPort)
{
    Bool bFlag = FALSE;
    BYTE xdata ucCbusSelect = GET_MHL_CBUS_SELECT_PATH();

#if(MHL_CBUS_LINK_MODE == MHL_CBUS_AUTO_SEARCH_LINK)
    if((ucPort > MHL_INPUT_NONE) &&_mdrv_mhl_GetCableDetectPort(ucPort)) // Current port cable detect high
    {
        ucCbusSelect = mhal_mhl_PortToCbusMapping(ucPort); // Cbus select of current port
    }
    else if(GET_MHL_CABLE_DETECT_ALL() > 0) // Current port cable detect low
    {
        if(GET_MHL_CBUS_SELECT_PATH() == MHL_CBUS_SELECT_NONE) // Cbus mux to none in previous
        {
            ucCbusSelect = _mdrv_mhl_SearchNextCbusSelect(mhal_mhl_PortToCbusMapping(ucPort));
        }
        else if(!GET_MHL_CABLE_DETECT_PORT(GET_MHL_CBUS_SELECT_PATH())) // Current cable detect not high, search next cable detect high port
        {
            ucCbusSelect = _mdrv_mhl_SearchNextCbusSelect(GET_MHL_CBUS_SELECT_PATH());
        }
        else if((!GET_MHL_CBUS_CONNECT()) && (mhal_mhl_CbusStatus() != 0x03))
        {
            if(!GET_MHL_POLLING_TIMER())
            {
                ucCbusSelect = _mdrv_mhl_SearchNextCbusSelect(GET_MHL_CBUS_SELECT_PATH());

                if(GET_MHL_CBUS_SELECT_PATH() == ucCbusSelect)
                {
                    ucCbusSelect = MHL_CBUS_SELECT_NONE;
                }
            }
        }
    }
    else // No cable detect
    {
        ucCbusSelect = MHL_CBUS_SELECT_NONE;
    }

#elif(MHL_CBUS_LINK_MODE == MHL_CBUS_FIXED_PORT_LINK)
    if((ucPort > MHL_INPUT_NONE) &&_mdrv_mhl_GetCableDetectPort(ucPort)) // Current port cable detect high
    {
        ucCbusSelect = mhal_mhl_PortToCbusMapping(ucPort); // Cbus select of current port
    }
    else
    {
        ucCbusSelect = MHL_CBUS_SELECT_NONE;
    }

#endif

    if(GET_MHL_CBUS_SELECT_PATH() != ucCbusSelect) // Current Cbus select not in on line port
    {
        bFlag = TRUE;

        if(GET_MHL_CBUS_CONNECT())
        {
            mhal_mhl_RxRtermControl(RX_RTERM_OFF);
        }

        _mdrv_mhl_SetNextCbusSelect(ucCbusSelect);

        if(GET_MHL_CBUS_SELECT_PATH() != MHL_CBUS_SELECT_NONE)
        {
            SET_MHL_CBUS_SELECT_PATH(MHL_CBUS_SELECT_NONE);

            mhal_mhl_CbusSwitchMux(GET_MHL_CBUS_SELECT_PATH());

            mhal_mhl_CableDetect();
        }
    }

    return bFlag;
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_WriteBurstCheckSum()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE _mdrv_mhl_WriteBurstCheckSum(BYTE *ucData)
{
    BYTE xdata uctemp = 0;
    BYTE xdata ucCheckSum = 0;

    for(uctemp = 0; uctemp <MHL_MSC_SCRATCHPAD_SIZE; uctemp++)
    {
        ucCheckSum = ucCheckSum +ucData[uctemp];
    }

    ucCheckSum = (ucCheckSum ^0xFF) +1;

    return ucCheckSum;
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_WriteBurstHandler()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _mdrv_mhl_WriteBurstHandler(void)
{
    switch(GET_MHL_WRITE_BURST_STATE())
    {
        case CBUS_WRITE_BURST_START:
            if(_mdrv_mhl_CbusWriteStatusOrSetInt(0x20, BIT2))
            {
                SET_MHL_WRITE_BURST_STATE(CBUS_WRITE_BURST_SEND_REQUEST);
            }
            break;

        case CBUS_WRITE_BURST_SEND_DATA:
            if(_mdrv_mhl_CbusWriteStatusOrSetInt(0x20, BIT1))
            {
                SET_MHL_WRITE_BURST_STATE(CBUS_WRITE_BURST_END);
            }
            break;

        case CBUS_WRITE_BURST_END:
            SET_MHL_WRITE_BURST_STATE(CBUS_WRITE_BURST_NONE);
            break;

        default:

            break;
    };
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_ForceClearHPD()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _mdrv_mhl_ForceClearHPD(void)
{
    CbusReq_S xdata stQueue;

    stQueue.cmd = MSC_CLR_HPD;
    stQueue.len = 0;

    if(_mdrv_mhl_CBusTxWriteCmd(&stQueue))
    {
        CLR_MHL_HPD_SET_FLAG();

        mdrv_mhl_SetHPDControlTime(MHL_HPD_CONTROL_INTERVAL);
    }
}

#if(MHL_3D_DISPLAY_SUPPORT)
//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_DTDPresentinEDID()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool _mdrv_mhl_DTDPresentinEDID(BYTE *pData)
{
    Bool bPresent = FALSE;
    BYTE xdata uctemp = 0;

    for(uctemp = 0; uctemp < 4; uctemp++)
    {
        if(pData[uctemp] > 0)
        {
            bPresent = TRUE;

            break;
        }
    }

    return bPresent;
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_GetDTD3DTimingInformation()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE _mdrv_mhl_GetDTD3DTimingInformation(BYTE *pData)
{
    Bool bInterlace = ((pData[17] &BIT7) ?TRUE :FALSE);
    BYTE xdata ucIndex = 0;
    WORD xdata usHActive = ((pData[4] &MASKBIT(7:4)) <<4) |pData[2];
    WORD xdata usHBlank = ((pData[4] &MASKBIT(3:0)) <<8) |pData[3];
    WORD xdata usVActive = ((pData[7] &MASKBIT(7:4)) <<4) |pData[5];
    WORD xdata usVBlank = ((pData[7] &MASKBIT(3:0)) <<8) |pData[6];
    WORD xdata usHTotal = usHActive +usHBlank;
    WORD xdata usVTotal = usVActive +usVBlank;
    WORD xdata usPixelClk = (pData[1] <<8) |pData[0];
    DWORD xdata ulFrequenct = (usPixelClk *100 /usHTotal) *1000 /usVTotal;

    usPixelClk = ulFrequenct;

    //msg_mhl(printf("** MHL get DTD timing %d x %d @ %d\r\n", usHActive, usVActive, usPixelClk));

    if((usHActive == 1920) && (usVActive == 1080)) // 1920x1080p
    {
        if((ulFrequenct > 230) && (ulFrequenct < 245)) // 24Hz
        {
            ucIndex = MHL_3D_PRESENT_VIC_1920x1080p24Hz;
        }
    }
    else if((usHActive == 1920) && (usVActive == 540) && bInterlace) // 1920x1080i
    {
        if((ulFrequenct > 590) && (ulFrequenct < 610)) // 60Hz
        {
            ucIndex = MHL_3D_PRESENT_VIC_1920x1080i60Hz;
        }
        else if((ulFrequenct > 490) && (ulFrequenct < 510)) // 50Hz
        {
            ucIndex = MHL_3D_PRESENT_VIC_1920x1080i50Hz;
        }
    }
    else if((usHActive == 1280) && (usVActive == 720)) // 1280x720p
    {
        if((ulFrequenct > 590) && (ulFrequenct < 610)) // 60Hz
        {
            ucIndex = MHL_3D_PRESENT_VIC_1280x720p60Hz;
        }
        else if((ulFrequenct > 490) && (ulFrequenct < 510)) // 50Hz
        {
            ucIndex = MHL_3D_PRESENT_VIC_1280x720p50Hz;
        }
    }

    return ucIndex;
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_InsertDTD3DInformation()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE _mdrv_mhl_InsertDTD3DInformation(BYTE ucIndex, BYTE ucData, BYTE ucNumber)
{
    BYTE xdata uctemp = 0;

    if(ucIndex > 0) // Just for compiler warning, access first input data will happen
    {
        // Do nothing
    }

    ucData = ((ucData &MASKBIT(6:5)) >> 4) |(ucData &BIT0);

    switch(ucData)
    {
        case MHL_3D_DTD_STEREO_FIELD_SEQUENTIAL_RIGHT:
            uctemp = MASKSET(uctemp, MHL_3D_FS_SUPPORT, MHL_3D_SUPPORT_MASK);
            break;

        case MHL_3D_DTD_STEREO_FIELD_SEQUENTIAL_LEFT:
            uctemp = MASKSET(uctemp, MHL_3D_FS_SUPPORT, MHL_3D_SUPPORT_MASK);
            break;

        case MHL_3D_DTD_STEREO_2WAY_INTERLEAVED_RIGHT:
            // No setting
            break;

        case MHL_3D_DTD_STEREO_2WAY_INTERLEAVED_LEFT:
            // No setting
            break;

        case MHL_3D_DTD_STEREO_4WAY_INTERLEAVED:
            // No setting
            break;

        case MHL_3D_DTD_STEREO_SIDE_BY_SIDE_INTERLEAVED:
            uctemp = MASKSET(uctemp, MHL_3D_LR_SUPPORT, MHL_3D_SUPPORT_MASK);
            break;

        default: // MHL_3D_DTD_STEREO_NORMAL_DISPLAY

            break;
    };

    SET_MHL_3D_DTD_INFORMATION(ucNumber, uctemp);

    for(uctemp = 0, ucData = 0; uctemp < (ucNumber +1); uctemp++)
    {
        if(GET_MHL_3D_DTD_INFORMATION(uctemp) > 0)
        {
            ucData = uctemp +1;
        }
    }

    return ucData;
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_CheckVIC3DStructureAll()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE _mdrv_mhl_CheckVIC3DStructureAll(WORD usData)
{
    BYTE xdata uctemp = 0;

    if((usData &BIT0) == BIT0) // Support frame packing 3D formats
    {
        uctemp = MASKSET(uctemp, MHL_3D_FS_SUPPORT, MHL_3D_FS_SUPPORT);
    }

    if((usData &BIT6) == BIT6) // Support top and bottom 3D formats
    {
        uctemp = MASKSET(uctemp, MHL_3D_TB_SUPPORT, MHL_3D_TB_SUPPORT);
    }

    if((usData &BIT8) == BIT8) // Support side by side 3D formats
    {
        uctemp = MASKSET(uctemp, MHL_3D_LR_SUPPORT, MHL_3D_LR_SUPPORT);
    }

    return uctemp;
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_CheckVIC3DStructure()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE _mdrv_mhl_CheckVIC3DStructure(BYTE ucData)
{
    BYTE xdata uctemp = 0;

    if(ucData == 0) // Support frame packing
    {
        uctemp = MASKSET(uctemp, MHL_3D_FS_SUPPORT, MHL_3D_FS_SUPPORT);
    }
    else if(ucData == 6) // Support top and bottom
    {
        uctemp = MASKSET(uctemp, MHL_3D_TB_SUPPORT, MHL_3D_TB_SUPPORT);
    }
    else if(ucData == 8) // Support side by side
    {
        uctemp = MASKSET(uctemp, MHL_3D_LR_SUPPORT, MHL_3D_LR_SUPPORT);
    }

    return uctemp;
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_ParsingVIC3DInformation()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool _mdrv_mhl_ParsingVIC3DInformation(BYTE ucExtendLength, BYTE *pData)
{
    Bool bVICParseFinish = TRUE;
    Bool b3DPresent = FALSE;
    BYTE xdata uctemp = 0;
    BYTE xdata ucIndex = 0;
    BYTE xdata ucValue = 0;
    BYTE xdata ucLength = 0;
    BYTE xdata ucVSDBAddr = 0;
    BYTE xdata ucVSDBLength = 0;
    BYTE xdata uc3DMultiPresent = 0;
    BYTE xdata ucVICTimingNum = 0;
    BYTE xdata ucVICTiming[MHL_3D_VIC_INFORMATION_LENGTH];
    WORD xdata usMaskValue = 0;

    for(uctemp = 0; uctemp < MHL_3D_VIC_INFORMATION_LENGTH; uctemp++)
    {
        ucVICTiming[uctemp] = 0;

        SET_MHL_3D_VIC_INFORMATION(uctemp, 0);
    }

    for(uctemp = 0; uctemp < ucExtendLength; uctemp++)
    {
        ucValue = (pData[uctemp] &MASKBIT(7:5)) >>5;
        ucLength = pData[uctemp] &MASKBIT(4:0);

        switch(ucValue)
        {
            case MHL_EDID_AUDIO_DATA_BLOCK_CODE:
                MHL_DRV_DPRINTF("** MHL get audio data block length %d in port %c\r\n", ucLength);
                break;

            case MHL_EDID_VIDEO_DATA_BLOCK_CODE:
                MHL_DRV_DPRINTF("** MHL get video data block length %d in port %c\r\n", ucLength);

                if(ucLength > MHL_3D_VIC_INFORMATION_LENGTH)
                {
                    ucVICTimingNum = MHL_3D_VIC_INFORMATION_LENGTH;
                }
                else
                {
                    ucVICTimingNum = ucLength;
                }

                for(ucIndex = 0; ucIndex <ucVICTimingNum; ucIndex++)
                {
                    ucVICTiming[ucIndex] = pData[uctemp +ucIndex +1];
                }

                break;

            case MHL_EDID_VENDER_SPECIFIC_DATA_BLOCK_CODE:
                MHL_DRV_DPRINTF("** MHL get vender specific data block length %d in port %c\r\n", ucLength);

                ucVSDBAddr = uctemp;

                bVICParseFinish = FALSE;

                break;

            case MHL_EDID_SPEAKER_ALLOCATION_DATA_BLOCK_CODE:
                MHL_DRV_DPRINTF("** MHL get speaker allocation data block length %d in port %c\r\n", ucLength);
                break;

            case MHL_EDID_VESA_DTC_DATA_BLOCK_CODE:
                MHL_DRV_DPRINTF("** MHL get VESA DTC data block length %d in port %c\r\n", ucLength);
                break;

            case MHL_EDID_USE_EXTENDED_TAG_BLOCK_CODE:
                MHL_DRV_DPRINTF("** MHL get use extended tag length %d in port %c\r\n", ucLength);
                break;

            default:

                break;
        };

        uctemp = uctemp +ucLength;
    }

    ucIndex = MHL_VIC_PARSING_START;
    ucVSDBLength = ucVSDBAddr +1;

    while(!bVICParseFinish)
    {
        ucValue = pData[ucVSDBAddr];

        if(ucVSDBAddr > ucVSDBLength) // Check total length
        {
            ucIndex = MHL_VIC_PARSING_FINISH;
        }

        switch(ucIndex)
        {
            case MHL_VIC_PARSING_START:
                ucVSDBLength = pData[ucVSDBAddr] &MASKBIT(4:0);

                if(ucVSDBLength < 8) // HDMI_Video_Present absent, no 3D information
                {
                    ucIndex = MHL_VIC_PARSING_FINISH;
                }
                else
                {
                    ucVSDBAddr = ucVSDBAddr +8;

                    ucIndex = MHL_VIC_PARSING_CHECK_HDMI_VIDEO_PRESENT;
                }

                ucVSDBLength = ucVSDBAddr +ucVSDBLength;

                break;

            case MHL_VIC_PARSING_CHECK_HDMI_VIDEO_PRESENT:
                if((ucValue &BIT5) != BIT5) // HDMI_Video_Present absent, no 3D information
                {
                    ucIndex = MHL_VIC_PARSING_FINISH;
                }
                else
                {
                    ucIndex = MHL_VIC_PARSING_CHECK_3D_PRESENT;

                    if((ucValue &BIT7) != BIT7) // Latency_Fields_Present absent, no video and audio latency and no interlaced video and audio latency
                    {
                        ucVSDBAddr++;
                    }
                    else
                    {
                        if((ucValue &BIT6) != BIT6) // I_Latency_Fields_Present absent, no video and audio latency
                        {
                            ucVSDBAddr = ucVSDBAddr +3;
                        }
                        else
                        {
                            ucVSDBAddr = ucVSDBAddr +5;
                        }
                    }
                }

                break;

            case MHL_VIC_PARSING_CHECK_3D_PRESENT:
                if((ucValue &BIT7) != BIT7) // 3D_Present absent, no 3D information
                {
                    ucIndex = MHL_VIC_PARSING_FINISH;
                }
                else
                {
                    b3DPresent = TRUE;
                    uc3DMultiPresent = (ucValue &MASKBIT(6:5)) >> 5;

                    if(uc3DMultiPresent == 0)
                    {
                        ucIndex = MHL_VIC_PARSING_3D_MULTI_PRESENT_00;
                    }
                    else if(uc3DMultiPresent == 3) // Reserved for future use
                    {
                        ucIndex = MHL_VIC_PARSING_FINISH;
                    }
                    else
                    {
                        ucIndex = MHL_VIC_PARSING_CHECK_HDMI_VIC;

                        ucVSDBAddr++;
                    }
                }

                break;

            case MHL_VIC_PARSING_CHECK_HDMI_VIC:
                ucLength = (ucValue &MASKBIT(7:5)) >> 5;

                ucVSDBAddr = ucVSDBAddr +ucLength +1;

                if(uc3DMultiPresent == 1)
                {
                    ucIndex = MHL_VIC_PARSING_3D_MULTI_PRESENT_01;
                }
                else // uc3DMultiPresent = 2
                {
                    ucIndex = MHL_VIC_PARSING_3D_MULTI_PRESENT_10;
                }

                ucLength = ucVSDBAddr +(ucValue &MASKBIT(4:0)) -1; // HDMI_3D_LEN

                break;

            case MHL_VIC_PARSING_3D_MULTI_PRESENT_00:
                ucIndex = MHL_VIC_PARSING_CHECK_3D_PRESENT_TIMING;

                break;

            case MHL_VIC_PARSING_3D_MULTI_PRESENT_01:
                ucValue = _mdrv_mhl_CheckVIC3DStructureAll((pData[ucVSDBAddr +1] |(pData[ucVSDBAddr] << 8)));

                for(uctemp = 0; uctemp <ucVICTimingNum; uctemp++)
                {
                    SET_MHL_3D_VIC_INFORMATION(uctemp, ucValue);
                }

                ucIndex = MHL_VIC_PARSING_CHECK_3D_PRESENT_TIMING;

                break;

            case MHL_VIC_PARSING_3D_MULTI_PRESENT_10:
                ucValue = _mdrv_mhl_CheckVIC3DStructureAll((pData[ucVSDBAddr +1] |(pData[ucVSDBAddr] << 8)));

                usMaskValue = pData[ucVSDBAddr +3] |(pData[ucVSDBAddr +2] << 8);

                for(uctemp = 0; uctemp <ucVICTimingNum; uctemp++)
                {
                    if((usMaskValue &_BIT(uctemp)) == _BIT(uctemp))
                    {
                        SET_MHL_3D_VIC_INFORMATION(uctemp, ucValue);
                    }
                }

                ucVSDBAddr = ucVSDBAddr +4;

                ucIndex = MHL_VIC_PARSING_CHECK_3D_STRUCTURE;

                break;

            case MHL_VIC_PARSING_CHECK_3D_STRUCTURE:
                if(ucVSDBAddr > ucLength) // No 3D sturcture
                {
                    ucIndex = MHL_VIC_PARSING_CHECK_3D_PRESENT_TIMING;
                }
                else
                {
                    uctemp = (ucValue &MASKBIT(7:4)) >> 4;
                    ucValue = ucValue &MASKBIT(3:0);

                    SET_MHL_3D_VIC_INFORMATION(uctemp, _mdrv_mhl_CheckVIC3DStructure(ucValue));

                    if(ucValue < 8) // Only 2D_VIC_Order and 3D_Structure, 1 byte
                    {
                        ucVSDBAddr++;
                    }
                    else // Add 3D_Detail, 2 byte
                    {
                        ucVSDBAddr = ucVSDBAddr +2;
                    }
                }

                break;

            case MHL_VIC_PARSING_CHECK_3D_PRESENT_TIMING:
                for(uctemp = 0; uctemp <ucVICTimingNum; uctemp++)
                {
                    ucValue = GET_MHL_3D_VIC_INFORMATION(uctemp);

                    switch(ucVICTiming[uctemp])
                    {
                        case MHL_3D_PRESENT_VIC_1920x1080p24Hz: // Support frame packing with top and bottom
                        case MHL_3D_PRESENT_VIC_1280x720p60Hz: // Support frame packing with top and bottom
                        case MHL_3D_PRESENT_VIC_1280x720p50Hz: // Support frame packing with top and bottom
                            ucValue = MASKSET(ucValue, (MHL_3D_FS_SUPPORT |MHL_3D_TB_SUPPORT), (MHL_3D_FS_SUPPORT |MHL_3D_TB_SUPPORT));
                            SET_MHL_3D_VIC_INFORMATION(uctemp, ucValue);

                            break;

                        case MHL_3D_PRESENT_VIC_1920x1080i60Hz: // Support side by side
                        case MHL_3D_PRESENT_VIC_1920x1080i50Hz: // Support side by side
                            ucValue = MASKSET(ucValue, MHL_3D_LR_SUPPORT, MHL_3D_LR_SUPPORT);
                            SET_MHL_3D_VIC_INFORMATION(uctemp, ucValue);

                            break;

                        default:

                            break;
                    };
                }

                ucIndex = MHL_VIC_PARSING_FINISH;

                break;

            case MHL_VIC_PARSING_FINISH:
                bVICParseFinish = TRUE;

                break;

            default:

                break;
        };
    }

    for(uctemp = 0, ucValue = 0; uctemp < MHL_3D_VIC_INFORMATION_LENGTH; uctemp++)
    {
        if(GET_MHL_3D_VIC_INFORMATION(uctemp) > 0)
        {
            ucValue = uctemp +1;
        }
    }

    SET_MHL_3D_TOT_ENT_VIC(ucValue);

    return b3DPresent;
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_ParsingEDIDfor3D()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _mdrv_mhl_ParsingEDIDfor3D(BYTE *pEdid)
{
    Bool bDTDParseFinish = FALSE;
    BYTE xdata uctemp = 0;
    BYTE xdata ucValue = 0;
    BYTE xdata ucIndex = 0;
    BYTE xdata ucDTDTimingNum = 0;
    BYTE xdata ucThirdDTDStart = 0;
    BYTE xdata ucDTDTiming[MHL_3D_DTD_INFORMATION_LENGTH];

    // Parsing 3D DTD information Start
    if(_mdrv_mhl_DTDPresentinEDID(&pEdid[MHL_EDID_FIRST_DTD_START])) // Check first DTD
    {
        ucValue = _mdrv_mhl_InsertDTD3DInformation(MHL_3D_FIRST_DTD, pEdid[MHL_EDID_FIRST_DTD_START +MHL_EDID_DTD_LENGTH -1], MHL_3D_FIRST_DTD);

        ucDTDTiming[MHL_3D_FIRST_DTD] = _mdrv_mhl_GetDTD3DTimingInformation(&pEdid[MHL_EDID_FIRST_DTD_START]);
    }
    else // First DTD absent
    {
        bDTDParseFinish = TRUE;

        ucDTDTimingNum = 0;

        MHL_DRV_DPRINTF("** MHL first DTD absent in port %c\r\n", GET_MHL_CBUS_SELECT_PATH());
    }

    if(!bDTDParseFinish)
    {
        if(_mdrv_mhl_DTDPresentinEDID(&pEdid[MHL_EDID_SECOND_DTD_START])) // Check second DTD
        {
            ucValue = _mdrv_mhl_InsertDTD3DInformation(MHL_3D_SECOND_DTD, pEdid[MHL_EDID_SECOND_DTD_START +MHL_EDID_DTD_LENGTH -1], MHL_3D_SECOND_DTD);

            ucDTDTiming[MHL_3D_SECOND_DTD] = _mdrv_mhl_GetDTD3DTimingInformation(&pEdid[MHL_EDID_SECOND_DTD_START]);
        }
        else    // Second DTD absent
        {
            bDTDParseFinish = TRUE;

            ucDTDTimingNum = 1;

           MHL_DRV_DPRINTF("** MHL second DTD absent in port %c\r\n", GET_MHL_CBUS_SELECT_PATH());
        }
    }

    if(pEdid[MHL_EDID_EXTENSION_FLAG] > 0) // Have extend EDID
    {
        ucThirdDTDStart = MHL_EDID_BLOCK_LENGTH +pEdid[MHL_EDID_RESERVED_DATA_BLOCK];

        MHL_DRV_DPRINTF("** MHL third DTD start at %x\r\n", ucThirdDTDStart);

        if(!bDTDParseFinish)
        {
            for(uctemp = 0; uctemp <(MHL_3D_DTD_INFORMATION_LENGTH -2); uctemp++)
            {
                if(_mdrv_mhl_DTDPresentinEDID(&pEdid[ucThirdDTDStart +MHL_EDID_DTD_LENGTH *uctemp])) // Check others DTD
                {
                    ucValue = _mdrv_mhl_InsertDTD3DInformation(MHL_3D_THIRD_DTD +uctemp, pEdid[ucThirdDTDStart +MHL_EDID_DTD_LENGTH *(uctemp +1) -1], MHL_3D_THIRD_DTD +uctemp);

                    ucDTDTiming[MHL_3D_THIRD_DTD +uctemp] = _mdrv_mhl_GetDTD3DTimingInformation(&pEdid[ucThirdDTDStart +MHL_EDID_DTD_LENGTH *uctemp]);
                }
                else    // Last DTD find
                {
                    bDTDParseFinish = TRUE;

                    ucDTDTimingNum = uctemp +2;

                    MHL_DRV_DPRINTF("** MHL total have %d DTD\r\n", ucDTDTimingNum);

                    break;
                }
            }
        }

        // Parsing 3D VIC information Start
        if(ucThirdDTDStart > (MHL_EDID_RESERVED_DATA_BLOCK +2))
        {
            if(_mdrv_mhl_ParsingVIC3DInformation(pEdid[MHL_EDID_RESERVED_DATA_BLOCK] -4, &pEdid[MHL_EDID_RESERVED_DATA_BLOCK +2]))
            {
                // 3D Present
                for(uctemp = 0; uctemp <ucDTDTimingNum; uctemp++)
                {
                    ucIndex = GET_MHL_3D_DTD_INFORMATION(uctemp);

                    switch(ucDTDTiming[uctemp])
                    {
                        case MHL_3D_PRESENT_VIC_1920x1080p24Hz: // Support frame packing with top and bottom
                        case MHL_3D_PRESENT_VIC_1280x720p60Hz: // Support frame packing with top and bottom
                        case MHL_3D_PRESENT_VIC_1280x720p50Hz: // Support frame packing with top and bottom
                            ucIndex = MASKSET(ucIndex, (MHL_3D_FS_SUPPORT |MHL_3D_TB_SUPPORT), (MHL_3D_FS_SUPPORT |MHL_3D_TB_SUPPORT));
                            SET_MHL_3D_DTD_INFORMATION(uctemp, ucIndex);

                            break;

                        case MHL_3D_PRESENT_VIC_1920x1080i60Hz: // Support side by side
                        case MHL_3D_PRESENT_VIC_1920x1080i50Hz: // Support side by side
                            ucIndex = MASKSET(ucIndex, MHL_3D_LR_SUPPORT, MHL_3D_LR_SUPPORT);
                            SET_MHL_3D_DTD_INFORMATION(uctemp, ucIndex);

                            break;

                        default:

                            break;
                    };

                    if(ucIndex > 0)
                    {
                        ucValue = uctemp +1;
                    }
                }
            }
        }
    }

    // Parsing 3D DTD information End
    SET_MHL_3D_TOT_ENT_DTD(ucValue);
}

#endif

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_GetDTDBurstfor3D()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool _mdrv_mhl_GetDTDBurstfor3D(BYTE *pData)
{
    Bool bFinish = FALSE;
    BYTE xdata uctemp = 0;
    BYTE xdata ucDTDCount = 0;
    static xdata BYTE ucSequence = 0;

    ucDTDCount = ucSequence *MHL_3D_SEQUENCE_LENGTH;

    pData[0] = (MHL_3D_DTD_ID >> 8);
    pData[1] = (MHL_3D_DTD_ID &MASKBIT(7:0));
    pData[3] = GET_MHL_3D_TOT_ENT_DTD();

    if(GET_MHL_3D_TOT_ENT_DTD() == 0)
    {
        bFinish = TRUE;
    }

    for(uctemp = 0; uctemp < MHL_3D_SEQUENCE_LENGTH; uctemp++)
    {
        if((!bFinish) && ((ucDTDCount +uctemp) == (GET_MHL_3D_TOT_ENT_DTD() -1)))
        {
            bFinish = TRUE;
        }

        if((ucDTDCount +uctemp) < GET_MHL_3D_TOT_ENT_DTD())
        {
            pData[6 +uctemp *2] = 0;
            pData[6 +uctemp *2 +1] = GET_MHL_3D_DTD_INFORMATION(ucDTDCount +uctemp) &MASKBIT(3:0);
        }
        else
        {
            pData[6 +uctemp *2] = 0;
            pData[6 +uctemp *2 +1] = 0;
        }
    }

    ucSequence++;

    pData[4] = ucSequence;

    if(bFinish)
    {
        pData[5] = GET_MHL_3D_TOT_ENT_DTD() -ucDTDCount;
        ucSequence = 0;
    }
    else
    {
        pData[5] = MHL_3D_SEQUENCE_LENGTH;
    }

    pData[2] = _mdrv_mhl_WriteBurstCheckSum(pData);

    return bFinish;
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_GetVICBurstfor3D()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool _mdrv_mhl_GetVICBurstfor3D(BYTE *pData)
{
    Bool bFinish = FALSE;
    BYTE xdata uctemp = 0;
    BYTE xdata ucVIDCount = 0;
    static xdata BYTE ucSequence = 0;

    ucVIDCount = ucSequence *MHL_3D_SEQUENCE_LENGTH;

    pData[0] = (MHL_3D_VIC_ID >> 8);
    pData[1] = (MHL_3D_VIC_ID &MASKBIT(7:0));
    pData[3] = GET_MHL_3D_TOT_ENT_VIC();

    if(GET_MHL_3D_TOT_ENT_VIC() == 0)
    {
        bFinish = TRUE;
    }

    for(uctemp = 0; uctemp < MHL_3D_SEQUENCE_LENGTH; uctemp++)
    {
        if((!bFinish) && ((ucVIDCount +uctemp) == (GET_MHL_3D_TOT_ENT_VIC() -1)))
        {
            bFinish = TRUE;
        }

        if((ucVIDCount +uctemp) < GET_MHL_3D_TOT_ENT_VIC())
        {
            pData[6 +uctemp *2] = 0;
            pData[6 +uctemp *2 +1] = GET_MHL_3D_VIC_INFORMATION(ucVIDCount +uctemp) &MASKBIT(3:0);
        }
        else
        {
            pData[6 +uctemp *2] = 0;
            pData[6 +uctemp *2 +1] = 0;
        }
    }

    ucSequence++;

    pData[4] = ucSequence;

    if(bFinish)
    {
        pData[5] = GET_MHL_3D_TOT_ENT_VIC() -ucVIDCount;
        ucSequence = 0;
    }
    else
    {
        pData[5] = MHL_3D_SEQUENCE_LENGTH;
    }

    pData[2] = _mdrv_mhl_WriteBurstCheckSum(pData);

    return bFinish;
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_CellPhonePatchIntoPowerDown()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _mdrv_mhl_CellPhonePatchIntoPowerDown(void)
{
    BYTE xdata uctemp = MHL_CBUS_SELECT_PORT0;
    BYTE xdata ucCbusSelect = GET_MHL_CBUS_SELECT_PATH();

    for(; uctemp < MHL_CBUS_SELECT_MASK; uctemp++)
    {
        if(GET_MHL_CABLE_DETECT_PORT(uctemp))
        {
            SET_MHL_CBUS_SELECT_PATH(uctemp);

            switch(_mdrv_mhl_GetCellPhoneID())
            {
                case CELL_PHONE_ID_NONE:
                    //mdrv_mhl_SetCableDetectTime(GET_MHL_CBUS_SELECT_PATH(), MHL_VBUS_TOGGLE_INTERVAL);
                    break;

#if(MHL_HTC_CHARGE_PATCH)
                case CELL_PHONE_ID_HTC_BUTTER_FLY:
                    mdrv_mhl_SetCableDetectTime(GET_MHL_CBUS_SELECT_PATH(), MHL_VBUS_TOGGLE_INTERVAL);
                    break;
#endif

#if(MHL_SONY_CHARGE_PATCH)
                case CELL_PHONE_ID_SONY_XPERIA:
                    mdrv_mhl_SetCableDetectTime(GET_MHL_CBUS_SELECT_PATH(), MHL_VBUS_TOGGLE_INTERVAL);
                    break;
#endif

                default:

                    break;
            };
        }
    }

    SET_MHL_CBUS_SELECT_PATH(MHL_CBUS_SELECT_NONE);

    if(ucCbusSelect != GET_MHL_CBUS_SELECT_PATH())
    {
        mhal_mhl_CbusSwitchMux(GET_MHL_CBUS_SELECT_PATH());
    }
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_CellPhonePatchPowerDownPolling()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _mdrv_mhl_CellPhonePatchPowerDownPolling(void)
{
    BYTE xdata uctemp = MHL_CBUS_SELECT_PORT0;
    BYTE xdata ucCbusSelect = GET_MHL_CBUS_SELECT_PATH();

    for(; uctemp < MHL_CBUS_SELECT_MASK; uctemp++)
    {
        SET_MHL_CBUS_SELECT_PATH(uctemp);

        switch(_mdrv_mhl_GetCellPhoneID())
        {
            case CELL_PHONE_ID_NONE:

                break;

#if(MHL_SONY_CHARGE_PATCH)
            case CELL_PHONE_ID_SONY_XPERIA:
                if(GET_MHL_CABLE_DETECT_PORT(uctemp))
                {
                    mhal_mhl_RxRtermControl(RX_MHL_RTERM);
                }
                else
                {
                    mhal_mhl_RxRtermControl(RX_RTERM_OFF);
                }

                break;
#endif

            default:

                break;
        };
    }

    SET_MHL_CBUS_SELECT_PATH(ucCbusSelect);
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_CellPhonePatchPowerOn()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _mdrv_mhl_CellPhonePatchPowerOn(void)
{
    BYTE xdata uctemp = MHL_CBUS_SELECT_PORT0;
    BYTE xdata ucCbusSelect = GET_MHL_CBUS_SELECT_PATH();

    for(; uctemp < MHL_CBUS_SELECT_MASK; uctemp++)
    {
        if(GET_MHL_CABLE_DETECT_PORT(uctemp))
        {
            SET_MHL_CBUS_SELECT_PATH(uctemp);

            switch(_mdrv_mhl_GetCellPhoneID())
            {
#if(MHL_LG_PRADA_DC_PATCH)
                case CELL_PHONE_ID_LG_PRADA:
                    mdrv_mhl_SetCableDetectTime(GET_MHL_CBUS_SELECT_PATH(), MHL_VBUS_TOGGLE_INTERVAL);
                    break;
#endif

                default:
#if(MHL_LG_PRADA_DC_PATCH)
                    mdrv_mhl_SetCableDetectTime(GET_MHL_CBUS_SELECT_PATH(), MHL_VBUS_TOGGLE_INTERVAL);
#endif
                    break;
            };
        }
    }

    SET_MHL_CBUS_SELECT_PATH(ucCbusSelect);
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_Initial()
//  [Description]
//                  MHL init
//  [Arguments]:
//                  *edid: MHL EDID data
//                  *devcap: MHL device capability
//  [Return]:
//
//**************************************************************************
void mdrv_mhl_Initial(BYTE *pEdid)
{
    memset(&gMHLInfo, 0, sizeof(gMHLInfo));

    SET_MHL_SYSTEM_FLAG(MHL_SYSTEM_INITIAL_MASK);

    mhal_mhl_Initial(pEdid, &tMHL_DEVCAP_TABLE, MHL_VENDOR_ID);

#if(MHL_3D_DISPLAY_SUPPORT)
    _mdrv_mhl_ParsingEDIDfor3D(pEdid);
#endif

    SET_MHL_CBUS_SELECT_PATH(MHL_CBUS_SELECT_NONE);

    mhal_mhl_CbusSwitchMux(GET_MHL_CBUS_SELECT_PATH());

    mhal_mhl_CableDetectControl(TRUE, MHL_CBUS_SELECT_PSEUDO);
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_SetCableDetectTime
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_mhl_SetCableDetectTime(BYTE ucCbusSelect, BYTE ucTime)
{
    if(ucTime == 0)
    {
        if(!GET_MHL_SYSTEM_CD_ENABLE())
        {
            if(GET_MHL_CD_ENABLE_TIMER() > 0)
            {
                DEC_MHL_CD_ENABLE_TIMER();
            }
            else
            {
                SET_MHL_SYSTEM_CD_ENABLE();
                mhal_mhl_CableDetectControl(TRUE, ucCbusSelect);
            }
        }
    }
    else
    {
        SET_MHL_CD_ENABLE_TIMER(ucTime);
        CLR_MHL_SYSTEM_CD_ENABLE();

        mhal_mhl_CableDetectControl(FALSE, ucCbusSelect);
    }
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_SetHPDControlTime
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_mhl_SetHPDControlTime(BYTE ucTime)
{
    if(ucTime == 0)
    {
        if(GET_MHL_SEND_HPD_CONTROL())
        {
            if(GET_MHL_HPD_CONTROL_TIMER() > 0)
            {
                DEC_MHL_HPD_CONTROL_TIMER();
            }
            else
            {
                CLR_MHL_SEND_HPD_CONTROL();
            }
        }
    }
    else
    {
        SET_MHL_HPD_CONTROL_TIMER(ucTime);
        SET_MHL_SEND_HPD_CONTROL();
    }
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_SourceChange
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_mhl_SourceChange(BYTE ucPort)
{
    if(!mhal_mhl_CheckInputPort(ucPort))
    {
#if(MHL_CBUS_LINK_MODE == MHL_CBUS_AUTO_SEARCH_LINK)
        if(_mdrv_mhl_GetCableDetectPort(ucPort)) // New online port plug MHL cable
        {
            mhal_mhl_DriverControl(MHL_CBUS_SOURCE_CHANGE);

#if(MHL_HTC_CHARGE_PATCH)
            if(GET_MHL_BUTTER_FLY_FLAG())
            {
                mdrv_mhl_SetCableDetectTime(GET_MHL_CBUS_SELECT_PATH(), MHL_VBUS_TOGGLE_INTERVAL);
            }
#endif

#if(MHL_SONY_CHARGE_PATCH)
            if(GET_MHL_SONY_XPERIA_FLAG())
            {
                mdrv_mhl_SetCableDetectTime(GET_MHL_CBUS_SELECT_PATH(), MHL_VBUS_TOGGLE_INTERVAL);
                mhal_mhl_RxRtermControl(RX_MHL_RTERM);
            }
#endif
        }
        else if(GET_MHL_HPD_SET_FLAG())
        {
            if(GET_MHL_PAD_FONE2_FLAG())
            {
                // Do nothing
            }
            else
            {
                _mdrv_mhl_ForceClearHPD();
            }
        }

#elif(MHL_CBUS_LINK_MODE == MHL_CBUS_FIXED_PORT_LINK)
#if(MHL_HTC_CHARGE_PATCH)
        if(GET_MHL_BUTTER_FLY_FLAG())
        {
            mdrv_mhl_SetCableDetectTime(GET_MHL_CBUS_SELECT_PATH(), MHL_VBUS_TOGGLE_INTERVAL);
        }
#endif

#endif
    }
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_CableDetect()
//  [Description]
//                  MHL cable detection
//  [Arguments]:
//
//  [Return]:
//                  TRUE: MHL cable plugged
//                  FALSE: MHL cable unplugged
//**************************************************************************
Bool mdrv_mhl_CableDetect(BYTE ucPort)
{
    Bool bCableDetect = mhal_mhl_CableDetect(); // Cable detect of current Cbus select path
    BYTE xdata ucCbusSelect = MHL_CBUS_SELECT_NONE;

    if(_mdrv_mhl_SetNextCbusSelect(GET_MHL_CBUS_SELECT_PATH()))
    {
        return bCableDetect;
    }

    if(bCableDetect)
    {
        if(!GET_MHL_CABLE_PLUGGED())
        {
            mhal_mhl_CbusPathLinkProc(ucPort);

            if(GET_MHL_INPUT_CLOCK_MODE() == CBUS_CLOCK_PACKET_PIXEL)
            {
                mhal_mhl_ClockModeSwitchProc(GET_MHL_PORT_SELECT(), TRUE);
            }

            SET_MHL_CABLE_PLUGGED();

            MHL_DRV_DPUTSTR("** MHL Cbus path linked^^\n");
        }

        if(GET_MHL_PORT_SELECT() != ucPort)
        {
            mhal_mhl_PortSwitchProc(ucPort);

            SET_MHL_PORT_SELECT(ucPort);
        }
    }
    else
    {
        if(GET_MHL_CABLE_PLUGGED() ||(!GET_MHL_PORT_ON_FLAG()))
        {
            SET_MHL_TX_QUEUE_INDEX(0);
            SET_MHL_CBUS_STATE(CBUS_STATE_IDLE);
            SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_NO_ERROR);
            CLR_MHL_PLUGGED_FLAG();

            mhal_mhl_CbusPathRemoveProc(ucPort);

            SET_MHL_PORT_ON_FLAG();
            MHL_DRV_DPUTSTR("** MHL Cbus path remove!!!\n");
        }
    }

    if(_mdrv_mhl_CheckInputCbusSelect(ucPort))
    {
        bCableDetect = FALSE;
    }

    return GET_MHL_CABLE_PLUGGED();
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_CbusStatus()
//  [Description]:
//                  MHL Cbus status
//  [Arguments]:
//
//  [Return]:
//                  TRUE: MHL Cbus connected
//                  FALSE: MHL Cbus disconnected
//**************************************************************************
Bool mdrv_mhl_CbusStatus(void)
{
    BYTE xdata usStatus = mhal_mhl_CbusStatus();

    if(GET_MHL_CBUS_CONNECT() && ((usStatus & 0x03) != 0x03))
    {
        SET_MHL_CBUS_STATE(CBUS_STATE_IDLE);
    }

    if((usStatus & 0x03) == 0x03)
    {
        SET_MHL_CBUS_CONNECT();
    }
    else
    {
        CLR_MHL_CBUS_CONNECT();
    }

    return GET_MHL_CBUS_CONNECT();
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_CbusStablePolling()
//  [Description]
//                  MHL CDR mode setting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_mhl_CbusStablePolling(void)
{
    if(mhal_mhl_CheckInputPort(GET_MHL_PORT_SELECT()))
    {
        mhal_mhl_AutoEQProcess(mhal_mhl_CDRModeMonitor(GET_CBUS_LINK_FINISH()));
    }
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_CbusRcpReply
//  [Description]
//                  MHL Cbus RCP response
//  [Arguments]:
//                  statusCode: RCP status code
//                  keyCode: RCP key code
//  [Return]:
//                  TRUE: success
//                  FALSE: fail
//**************************************************************************
Bool mdrv_mhl_CbusRcpReply(BYTE ucStatusCode, BYTE ucKeyCode)
{
    if(ucStatusCode != MSC_RCP_NO_ERROR)
    {
        return _mdrv_mhl_CbusQueueMscRxReply(MSC_MSG_RCPE, ucStatusCode);
    }
    else
    {
        return _mdrv_mhl_CbusQueueMscRxReply(MSC_MSG_RCPK, ucKeyCode);
    }
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_CbusRapReply
//  [Description]
//                  MHL Cbus RAP response
//  [Arguments]:
//                  statusCode: RAP status code
//
//  [Return]:
//                  TRUE: success
//                  FALSE: fail
//**************************************************************************
Bool mdrv_mhl_CbusRapReply(BYTE ucStatusCode)
{
    return _mdrv_mhl_CbusQueueMscRxReply(MSC_MSG_RAPK, ucStatusCode);
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_CbusUcpReply
//  [Description]
//                  MHL Cbus UCP response
//  [Arguments]:
//                  statusCode: UCP status code
//                  keyCode: UCP key code
//  [Return]:
//                  TRUE: success
//                  FALSE: fail
//**************************************************************************
Bool mdrv_mhl_CbusUcpReply(BYTE ucStatusCode, BYTE ucKeyCode)
{
    if(ucStatusCode != MSC_RCP_NO_ERROR)
    {
        return _mdrv_mhl_CbusQueueMscRxReply(MSC_MSG_UCPE, ucStatusCode);
    }
    else
    {
        return _mdrv_mhl_CbusQueueMscRxReply(MSC_MSG_UCPK, ucKeyCode);
    }
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_CbusSetHPDState()
//  [Description]:
//                  MHL Cbus set HPD state
//  [Arguments]:
//                  TRUE: SET_HPD
//                  FALSE: CLR_HPD
//  [Return]:
//
//**************************************************************************
Bool mdrv_mhl_CbusSetHPDState(Bool bFlag)
{
    Bool bindex = FALSE;
    CbusReq_S xdata stQueue;

    if(!GET_MHL_SEND_HPD_CONTROL())
    {
        if(GET_MHL_HPD_SET_FLAG() == bFlag)
        {
            return TRUE;
        }

        stQueue.cmd = (bFlag == TRUE ? MSC_SET_HPD : MSC_CLR_HPD);
        stQueue.len = 0;
        stQueue.reqStatus = CBUS_REQ_PENDING;

        bindex = _mdrv_mhl_CbusQueueMscTxRequest(GET_MHL_TX_QUEUE_INDEX(), &stQueue);

        if(bFlag && bindex)
        {
            SET_MHL_HPD_SET_FLAG();
        }
        else
        {
            CLR_MHL_HPD_SET_FLAG();
        }

        if(bindex)
        {
            mdrv_mhl_SetHPDControlTime(MHL_HPD_CONTROL_INTERVAL);
        }
    }

    return GET_MHL_HPD_SET_FLAG();
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_CbusMscMsgSubCmdSend ()
//  [Description]
//                  MHL Cbus MSC Send RCP or RAP
//  [Arguments]:
//                  subCmd: MSC subcommands
//                  databuf: command code
//  [Return]:
//                  TRUE: success
//                  FALSE: fail
//**************************************************************************
Bool mdrv_mhl_CbusMscMsgSubCmdSend(BYTE ucSubCmd, BYTE ucData)
{
    CbusReq_S xdata stQueue;

    stQueue.cmd = MSC_MSC_MSG;
    stQueue.msgData[0] = ucSubCmd;
    stQueue.msgData[1] = ucData;
    stQueue.len = 2;
    stQueue.reqStatus = CBUS_REQ_PENDING;

    return _mdrv_mhl_CbusQueueMscTxRequest(GET_MHL_TX_QUEUE_INDEX(), &stQueue);
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_CbusWriteBurst
//  [Description]
//                  MHL Cbus write burst
//  [Arguments]:
//                  addr: offset
//                  lens: data length, offset + maxi 16 packets
//                  *buf: burst data
//  [Return]:
//                  TRUE: success
//                  FALSE: fail
//**************************************************************************
Bool mdrv_mhl_CbusWriteBurst(BYTE ucAddr, BYTE ucLength, BYTE *pBuffer)
{
    BYTE ucindex = 0;
    CbusReq_S xdata stQueue;

    if(ucLength > MHL_MSC_SCRATCHPAD_SIZE)
    {
        ucLength = MHL_MSC_SCRATCHPAD_SIZE;
    }

    stQueue.cmd = MSC_WRITE_BURST;
    stQueue.len = ucLength +1; // offset + data packets
    stQueue.msgData[0] = ucAddr; // offset packet
    stQueue.reqStatus = CBUS_REQ_PENDING;

    for(ucindex =0; ucindex <ucLength; ucindex++)
    {
        stQueue.msgData[ucindex +1] = pBuffer[ucindex];
    }

    return _mdrv_mhl_CbusQueueMscTxRequest(GET_MHL_TX_QUEUE_INDEX(), &stQueue);
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_CbusSendCommand(BYTE ucIndex)
//  [Description]:
//                  MHL Cbus get transmitting queue index
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_mhl_CbusSendCommand(BYTE ucSelect)
{
    switch(ucSelect)
    {
        case CBUS_HPD_SET_FLAG:
            mdrv_mhl_CbusSetHPDState(TRUE);
            break;

        case CBUS_DCAP_RDY_FLAG:
            _mdrv_mhl_Cbus_SetDCapRdy();
            break;

        case CBUS_PATH_EN_FLAG:
            _mdrv_mhl_Cbus_SetPathEn(TRUE);
            break;

        case CBUS_DCAP_CHG_FLAG:
            _mdrv_mhl_Cbus_SetDCapChg();
            break;

        case ~MHL_CBUS_LINK_MASK:    // Uncall function
            mdrv_mhl_CbusMscMsgSubCmdSend(1, 1);
            mdrv_mhl_CbusWriteBurst(1, 1, 1);
            mhal_mhl_CbusFloating(TRUE);
            _mdrv_mhl_GetReadData(1);
            _mdrv_mhl_GetLeastBitNum(1);
            mhal_mhl_CbusWakeupIntFlag();
            mhal_mhl_CbusToPortMapping(1);
            mhal_mhl_SetHPD(1);
            _mdrv_mhl_CbusRxReply(MSC_ABORT, FALSE, 0);
            break;

        default:

            break;
    };
}

#if(MHL_READ_DEVICE_CAPABILITY)
//**************************************************************************
//  [Function Name]:
//                  mapi_mhl_GetDeviceCapacibility
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mdrv_mhl_GetDeviceCapacibility(WORD usDevcapMask, BYTE *ucDevcap)
{
    static WORD xdata usDevCap = 0;
    Bool bindex = FALSE;
    Bool bFinish = FALSE;
    BYTE xdata uctemp = 0;
    BYTE xdata ucData = 0;

    if(GET_MHL_RECEIVE_DEVCAP())
    {
        do
        {
            if(usDevCap &usDevcapMask)
            {
                uctemp = _mdrv_mhl_GetLeastBitNum(usDevCap);

                bindex = _mdrv_mhl_ReadDeviceCapacibility(uctemp, &ucData);

                if(bindex)
                {
                    usDevCap = MASKSET(usDevCap, 0, _BIT(uctemp));

                    *(ucDevcap +uctemp) = ucData;

                    if(usDevCap == 0)
                    {
                        bFinish = TRUE;

                        CLR_MHL_RECEIVE_DEVCAP();
                    }

                    if(uctemp == MHL_CBUS_FEATURE_FLAG)
                    {
                        if(ucData &BIT0)
                        {
                            SET_MHL_SEND_RCP_ENABLE();
                        }

                        if(ucData &BIT1)
                        {
                            SET_MHL_SEND_RAP_ENABLE();
                        }
                    }
                }
            }
            else
            {
                bindex = FALSE;
            }
        }while(bindex);
    }
    else
    {
        usDevCap = usDevcapMask;
    }

    return bFinish;
}

#endif

#if(MHL_READ_DEVICE_VENDER_ID)
//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_GetDeviceVenderID
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mdrv_mhl_GetDeviceVenderID(BYTE *ucVenderID)
{
    Bool bindex = FALSE;
    Bool bFinish = FALSE;
    BYTE xdata ucData = 0;

    if(GET_MHL_RECEIVE_DEVCAP())
    {
        if(GET_MHL_SYSTEM_GET_VENDER_ID())
        {
            bindex = _mdrv_mhl_ReadVenderID(&ucData);

            if(bindex)
            {
                *ucVenderID = ucData;

                CLR_MHL_SYSTEM_GET_VENDER_ID();

                bFinish = TRUE;
            }
        }
    }
    else
    {
        SET_MHL_SYSTEM_GET_VENDER_ID();
    }

    return bFinish;
}

#endif

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_Send3DInformation()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_mhl_Send3DInformation(void)
{
    Bool bFinish = FALSE;
    BYTE xdata ucData[MHL_MSC_SCRATCHPAD_SIZE] = {0};

    if(GET_MHL_RECEIVE_3D_REQ())
    {
        if(GET_MHL_WRITE_BURST_STATE() == CBUS_WRITE_BURST_NONE)
        {
            SET_MHL_WRITE_BURST_STATE(CBUS_WRITE_BURST_START);
        }
        else if(GET_MHL_WRITE_BURST_STATE() == CBUS_WRITE_BURST_REVEIVE_GRANT)
        {
            if(!GET_MHL_SEND_3D_DTD())
            {
                if(_mdrv_mhl_GetDTDBurstfor3D(ucData))
                {
                    bFinish= TRUE;
                }

                if(mdrv_mhl_CbusWriteBurst(0x40, MHL_MSC_SCRATCHPAD_SIZE, ucData))
                {
                    if(bFinish)
                    {
                        SET_MHL_SEND_3D_DTD();
                    }

                    SET_MHL_WRITE_BURST_STATE(CBUS_WRITE_BURST_SEND_DATA);

                    MHL_DRV_DPRINTF("** MHL sent 3D DTD in sequence %d\r\n", ucData[4]);
                }
            }
            else if(!GET_MHL_SEND_3D_VIC())
            {
                if(_mdrv_mhl_GetVICBurstfor3D(ucData))
                {
                    bFinish= TRUE;
                }

                if(mdrv_mhl_CbusWriteBurst(0x40, MHL_MSC_SCRATCHPAD_SIZE, ucData))
                {
                    if(bFinish)
                    {
                        SET_MHL_SEND_3D_VIC();
                    }

                    SET_MHL_WRITE_BURST_STATE(CBUS_WRITE_BURST_SEND_DATA);

                    MHL_DRV_DPRINTF("** MHL sent 3D VIC in sequence %d\r\n", ucData[4]);
                }
            }
        }

        if(GET_MHL_SEND_3D_VIC() && GET_MHL_SEND_3D_DTD())
        {
            CLR_MHL_RECEIVE_3D_REQ();
        }
    }
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_CbusConnectionCheck()
//  [Description]:
//                  MHL Cbus connection check
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_mhl_CbusConnectionCheck(void)
{
    BYTE xdata ucindex = 0;

    if(GET_MHL_SYSTEM_CBUS_CONNECT() != GET_MHL_CBUS_CONNECT())
    {
        SLT_MHL_SYSTEM_CBUS_CONNECT(GET_MHL_CBUS_CONNECT());

        if(!GET_MHL_SYSTEM_CBUS_CONNECT())
        {
            SET_MHL_TX_QUEUE_INDEX(0);
            SET_MHL_CBUS_STATE(CBUS_STATE_IDLE);
            SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_NO_ERROR);
            CLR_MHL_CONNECT_FLAG();

            _mdrv_mhl_SetControlTimer(0);    // timeout reset.

            mhal_mhl_CbusStucktoLowProc(GET_MHL_PORT_SELECT());

            if(GET_MHL_INPUT_CLOCK_MODE() == CBUS_CLOCK_PACKET_PIXEL)
            {
                mhal_mhl_ClockModeSwitchProc(GET_MHL_PORT_SELECT(), FALSE);

                SET_MHL_INPUT_CLOCK_MODE(CBUS_CLOCK_NORMAL_24BIT);
            }

            memset(&gMHLInfo.MhlCbusInfo.request, 0, sizeof(gMHLInfo.MhlCbusInfo.request));

            mdrv_mhl_CableDetect(GET_MHL_PORT_SELECT());

            MHL_DRV_DPUTSTR("** MHL Cbus stuck to low @@");
        }
        else
        {
            SET_MHL_DEBOUNCE_TIMER(150); // reset 150msec
            //SET_MHL_TIME_OUT_FLAG();

            SET_MHL_POLLING_TIMER(0);

            mhal_mhl_CbusConnectProc(GET_MHL_PORT_SELECT());

            SET_MHL_INPUT_CLOCK_MODE(CBUS_CLOCK_NORMAL_24BIT);

            CLR_MHL_PORT_ON_FLAG();

            MHL_DRV_DPUTSTR("** MHL Cbus connected~~ ^Q^");
        }
    }

    if(GET_MHL_SYSTEM_CBUS_CONNECT())
    {
        if(mhal_mhl_CheckInputPort(GET_MHL_PORT_SELECT()))
        {
            ucindex = GET_CBUS_LINK_FLAG() ^MHL_CBUS_LINK_MASK;

            if(ucindex > 0)
            {
                mdrv_mhl_CbusSendCommand(_mdrv_mhl_CbusGetLeastFlag(ucindex));
            }

            _mdrv_mhl_WriteBurstHandler();
        }
        else
        {
            if(GET_MHL_HPD_SET_FLAG())
            {
                if(GET_MHL_PAD_FONE2_FLAG())
                {
                    // Do nothing
                }
                else
                {
                    mdrv_mhl_CbusSetHPDState(FALSE);
                }
            }

            if(GET_MHL_SAMSUNG_PHONE_FLAG() && GET_MHL_PATH_EN_FLAG())
            {
                _mdrv_mhl_Cbus_SetPathEn(FALSE);
            }

#if(MHL_HTC_CHARGE_PATCH)
            if(GET_MHL_BUTTER_FLY_FLAG() || GET_MHL_HUAWEI_PHONE_FLAG())
            {
                if(!GET_MHL_DCAP_RDY_FLAG())
                {
                    _mdrv_mhl_Cbus_SetDCapRdy();
                }
            }
#endif
        }
    }

    mdrv_mhl_CbusStablePolling();
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_CBusGetQueueIndex(BYTE ucIndex)
//  [Description]:
//                  MHL Cbus get transmitting queue index
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mdrv_mhl_CBusGetQueueIndex(BYTE ucIndex)
{
    BYTE xdata uctemp = 0;

    // Check Rx queue have packet transmit success or not
    if(GET_MHL_QUEUE_STATE(MHL_CBUS_RX_QUEUE_INDEX) != CBUS_REQ_IDLE)
    {
        return MHL_CBUS_RX_QUEUE_INDEX;
    }

    // Check Tx queue have packet transmit success or not, search start  from current index
    for(uctemp =0; uctemp <(MHL_CBUS_TOTAL_QUEUE_NUM -1); uctemp++)
    {
        if(GET_MHL_QUEUE_STATE((ucIndex +uctemp) %(MHL_CBUS_TOTAL_QUEUE_NUM -1)) != CBUS_REQ_IDLE)
        {
            return ((ucIndex +uctemp) %(MHL_CBUS_TOTAL_QUEUE_NUM -1));
        }
    }

    return MHL_CBUS_TOTAL_QUEUE_NUM;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_CbusTxHandler()
//  [Description]
//                  MHL Cbus handle send MSC message
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_mhl_CbusTxHandler(void)
{
    BYTE xdata ucindex = 0;

    switch(GET_MHL_CBUS_STATE())
    {
        case CBUS_STATE_IDLE:
            ucindex = mdrv_mhl_CBusGetQueueIndex(GET_MHL_TX_QUEUE_INDEX());

            if((ucindex < MHL_CBUS_TOTAL_QUEUE_NUM) && !GET_MHL_DEBOUNCE_TIMER())
            {
                if(_mdrv_mhl_CBusTxWriteCmd(&(GET_MHL_CBUS_QUEUE(ucindex))))
                {
                    // Retry send packet twice
                    if(GET_MHL_QUEUE_STATE(ucindex) == CBUS_REQ_SENT)
                    {
                        SET_MHL_QUEUE_STATE(ucindex, CBUS_REQ_IDLE);
                        MHL_DRV_DPUTSTR("** MHL Tx Packet Retry");
                    }
                    else if(GET_MHL_QUEUE_STATE(ucindex) > CBUS_REQ_SENT)
                    {
                        SET_MHL_QUEUE_STATE(ucindex, CBUS_REQ_SENT);
                    }

                    // Update current index to search index, but not Rx queue index
                    if(ucindex < MHL_CBUS_RX_QUEUE_INDEX)
                    {
                        SET_MHL_TX_QUEUE_INDEX(ucindex);
                    }

                    SET_MHL_CBUS_STATE(CBUS_STATE_SENT);
                    _mdrv_mhl_SetControlTimer(CBUS_COMMAND_TIMEOUT_SECS);
                }
            }

            break;

        case CBUS_STATE_SENT:
        case CBUS_STATE_WAITING_ACK:
        case CBUS_STATE_RECEIVED:

            break;

        case CBUS_STATE_ACK:

            break;

        default:
            SET_MHL_CBUS_STATE(CBUS_STATE_IDLE);
            break;
    }
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_CbusRxHandler()
//  [Description]
//                  MHL Cbus reply the received MSC message
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mdrv_mhl_CbusRxHandler(BYTE *ucCmd, BYTE *ucData)
{
    Bool bindex = FALSE;
    BYTE xdata uctemp = 0;

    if(GET_MHL_RECEIVE_MSC())
    {
        CLR_MHL_RECEIVE_MSC();

        if(GET_MHL_TIME_OUT_FLAG())
        {
            _mdrv_mhl_SetControlTimer(0);    // timeout reset.
        }
    }

    if(GET_MHL_RECEIVE_ACK())
    {
        CLR_MHL_RECEIVE_ACK();

        if(GET_MHL_QUEUE_STATE(MHL_CBUS_RX_QUEUE_INDEX) != CBUS_REQ_IDLE)
        {
            SET_MHL_QUEUE_STATE(MHL_CBUS_RX_QUEUE_INDEX, CBUS_REQ_IDLE);
        }
        else if(GET_MHL_QUEUE_STATE(GET_MHL_TX_QUEUE_INDEX()) != CBUS_REQ_PENDING)
        {
            SET_MHL_QUEUE_STATE(GET_MHL_TX_QUEUE_INDEX(), CBUS_REQ_IDLE);
        }

        uctemp = mdrv_mhl_CBusGetQueueIndex(GET_MHL_TX_QUEUE_INDEX());

        // Update current index to search index, but not Rx queue index
        if(uctemp < MHL_CBUS_RX_QUEUE_INDEX)
        {
            SET_MHL_TX_QUEUE_INDEX(uctemp);
        }

        _mdrv_mhl_CbusLinkCheck();

        SET_MHL_CBUS_STATE(CBUS_STATE_IDLE);
    }

    if(GET_MHL_RECEIVE_ABORT())
    {
        CLR_MHL_RECEIVE_ABORT();

        _mdrv_mhl_SetControlTimer(CBUS_ABORT_PADING_SECS);
    }

    if(GET_MHL_RECEIVE_RCP() ||GET_MHL_RECEIVE_RAP())
    {
        CLR_MHL_RECEIVE_RCP();
        CLR_MHL_RECEIVE_RAP();

        *ucCmd = GET_MHL_RX_MSG_COMMAND();
        *ucData = GET_MHL_RX_MSG_DATA();
        SET_MHL_CBUS_STATE(CBUS_STATE_IDLE);

        bindex = TRUE;
    }

    if(GET_MHL_RECEIVE_ERROR())
    {
        CLR_MHL_RECEIVE_ERROR();

        _mdrv_mhl_SetControlTimer(CBUS_ABORT_PADING_SECS);
    }

    return bindex;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_CbusIntHandler()
//  [Description]
//                  MHL Cbus Interrupt Handler
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_mhl_CbusIntHandler(void)
{
    Bool bretAck = TRUE;
    BYTE xdata ucCount = 0;

#if(MHL_CBUS_OPERATION_MODE == MHL_CBUS_HW_REPLY_MODE)
    switch(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).cmd)
    {
        case MSC_ACK:
            SET_MHL_RECEIVE_ACK();

            if((GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).len > 1) && GET_MHL_SEND_READ())
            {
                SET_MHL_CBUS_ASK_DATA(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[0]);

                CLR_MHL_SEND_READ();
                SET_MHL_SEND_FINISH();
            }

            break;

        case MSC_ABORT: // wait 2 sec before starting a new Cbus transaction after receiving an ABORT, Chapter 13.10.3
            SET_MHL_RECEIVE_ABORT();
            break;

        case MSC_GET_DDC_ERRORCODE:
            mhal_mhl_GetDDCErrorCode();
            break;

        case MSC_MSC_MSG:
            bretAck = _mdrv_mhl_CBusProcessMSCSubCmd(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[0], GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[1]);
            break;

        case MSC_WRITE_STAT_OR_SET_INT:
            if(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[0] == 0x31)
            {
                _mdrv_mhl_ClockModeSwitch(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[1] &(BIT2 |BIT1 |BIT0));
            }

            if((GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[0] == 0x30) && (GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[1] == 0x01)) // Devcap ready
            {
                SET_MHL_RECEIVE_DEVCAP();
            }

            if(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[0] == 0x20)
            {
                if((GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[1] &BIT1) == BIT1) // Devcap change
                {
                    SET_MHL_RECEIVE_DEVCAP();
                }

                if((GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[1] &BIT3) == BIT3) // Burst write grant
                {
                    if(GET_MHL_WRITE_BURST_STATE() == CBUS_WRITE_BURST_SEND_REQUEST)
                    {
                        SET_MHL_WRITE_BURST_STATE(CBUS_WRITE_BURST_REVEIVE_GRANT);
                    }
                }

                if((GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[1] &BIT4) == BIT4) // 3D request
                {
                    SET_MHL_RECEIVE_3D_REQ();

                    CLR_MHL_SEND_3D_DTD();
                    CLR_MHL_SEND_3D_VIC();
                }
            }

            break;

        case MSC_WRITE_BURST:
            SET_MHL_WRITE_BURST_ADDRESS(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[0]);
            SET_MHL_WRITE_BURST_LENGTH(MHL_MSC_SCRATCHPAD_SIZE +1);

            for(ucCount = 0; ucCount < (MHL_MSC_SCRATCHPAD_SIZE +1); ucCount++)
            {
                if(ucCount < GET_MHL_WRITE_BURST_LENGTH())
                {
                    if(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[ucCount +1] == MSC_EOF)
                    {
                        SET_MHL_WRITE_BURST_DATA(ucCount, 0);

                        SET_MHL_WRITE_BURST_LENGTH(ucCount);
                    }
                    else
                    {
                        SET_MHL_WRITE_BURST_DATA(ucCount, GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[ucCount +1]);
                    }
                }
                else if(ucCount < MHL_MSC_SCRATCHPAD_SIZE)
                {
                    SET_MHL_WRITE_BURST_DATA(ucCount, 0);
                }
            }

            if(GET_MHL_HID_SEND_REQUEST())
            {
                CLR_MHL_HID_SEND_REQUEST();
                SET_MHL_HID_RECEIVE_REPLY();
            }

            break;

        case MSC_RESERVED:
            //_mdrv_mhl_CbusRxReply(MSC_ABORT, FALSE, 0);
            break;

        default:

            break;
    };

#else
    if(GET_MHL_CBUS_STATE() == CBUS_STATE_RECEIVED) // received valid message
    {
        // Begin processing received packet, set Cbus state idle and free queue
        SET_MHL_QUEUE_STATE(MHL_CBUS_RX_QUEUE_INDEX, CBUS_REQ_IDLE);
        SET_MHL_CBUS_STATE(CBUS_STATE_IDLE);

        if(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).rcState == CBUS_RECEIVE_NORMAL) // received normally
        {
            switch(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).cmd)
            {
                case MSC_ACK:
                    SET_MHL_RECEIVE_ACK();
                    break;

                case MSC_NACK:
                    break;

                case MSC_ABORT: // wait 2 sec before starting a new Cbus transaction after receiving an ABORT, Chapter 13.10.3
                    SET_MHL_RECEIVE_ABORT();
                    break;

                case MSC_WRITE_STAT_OR_SET_INT:
                    // CTS 6.3.11.8 && 6.3.11.11
                    if(gMHLInfo.MhlCbusInfo.bIsCmdinDataField)
                    {
                        SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_PROTOCOL_ERROR);
                        bretAck = FALSE;
                    }
                    else
                    {
                        if(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[0] == 0x31)
                        {
                            _mdrv_mhl_ClockModeSwitch(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[1] &(BIT2 |BIT1 |BIT0));
                        }

                        if((GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[0] == 0x30) && (GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[1] == 0x01)) // Devcap ready
                        {
                            SET_MHL_RECEIVE_DEVCAP();
                        }

                        if(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[0] == 0x20)
                        {
                            if((GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[1] &BIT1) == BIT1) // Devcap change
                            {
                                SET_MHL_RECEIVE_DEVCAP();
                            }

                            if((GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[1] &BIT3) == BIT3) // Burst write grant
                            {
                                if(GET_MHL_WRITE_BURST_STATE() == CBUS_WRITE_BURST_SEND_REQUEST)
                                {
                                    SET_MHL_WRITE_BURST_STATE(CBUS_WRITE_BURST_REVEIVE_GRANT);
                                }
                            }

                            if((GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[1] &BIT4) == BIT4) // 3D request
                            {
                                SET_MHL_RECEIVE_3D_REQ();

                                CLR_MHL_SEND_3D_DTD();
                                CLR_MHL_SEND_3D_VIC();
                            }
                        }
                    }

                    _mdrv_mhl_CbusRxReply(bretAck ? MSC_ACK : MSC_ABORT, FALSE, 0);

                    break;

                case MSC_READ_DEVCAP:
                    // HW auto reply, REG_MHL_CBUS_23[10] = 1

                    // CTS 6.3.11.8
                    if(gMHLInfo.MhlCbusInfo.bIsCmdinDataField)
                    {
                        _mdrv_mhl_CbusRxReply(MSC_ABORT, FALSE, 0);
                        SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_PROTOCOL_ERROR);
                    }
                    else
                    {
                        // CTS 6.3.11.9
                        if(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[0] <= 0x0F)
                        {
                            _mdrv_mhl_CbusRxReply(MSC_ACK, TRUE, tMHL_DEVCAP_TABLE[GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[0]]);
                            SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_NO_ERROR);
                        }
                        else
                        {
                            _mdrv_mhl_CbusRxReply(MSC_ABORT, FALSE, 0);
                            SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_BAD_OFFSET);
                        }
                    }

                    break;

                case MSC_GET_STATE:
                    _mdrv_mhl_CbusRxReply(MSC_GET_STATE, TRUE, tMHL_DEVCAP_TABLE[0]); // DevCap register 0x00
                    break;

                case MSC_GET_VENDOR_ID:
                    _mdrv_mhl_CbusRxReply(MSC_GET_VENDOR_ID, TRUE, MHL_VENDOR_ID);
                    break;

                case MSC_MSC_MSG:
                    // CTS 6.3.11.22 & 6.3.11.24
                    if(gMHLInfo.MhlCbusInfo.bIsCmdinDataField)
                    {
                        SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_PROTOCOL_ERROR);
                        bretAck = FALSE;
                    }
                    else
                    {
                        bretAck = _mdrv_mhl_CBusProcessMSCSubCmd(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[0], GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[1]);
                    }

                    _mdrv_mhl_CbusRxReply(bretAck ? MSC_ACK : MSC_ABORT, FALSE, 0);

                    break;

                case MSC_WRITE_BURST:
                    // CTS 6.3.11.20
                    if(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[0] + (GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).len -2) > (MHL_MSC_SCRATCHPAD_OFFSET +MHL_MSC_SCRATCHPAD_SIZE))
                    {
                        SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_BAD_OFFSET);
                        bretAck = FALSE;
                    }

                    // CTS 6.3.11.14
                    if(gMHLInfo.MhlCbusInfo.bIsCmdinDataField)
                    {
                        SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_PROTOCOL_ERROR);
                        bretAck = FALSE;
                    }

                    _mdrv_mhl_CbusRxReply(bretAck ? MSC_ACK : MSC_ABORT, FALSE, 0);
                    break;

                case MSC_GET_MSC_ERRORCODE:
                    _mdrv_mhl_CbusRxReply(MSC_GET_MSC_ERRORCODE, TRUE, GET_MHL_CBUS_ERROE_CODE());
                    break;

                case MSC_GET_DDC_ERRORCODE:
                    _mdrv_mhl_CbusRxReply(MSC_GET_DDC_ERRORCODE, TRUE, mhal_mhl_GetDDCErrorCode());
                    break;

                case MSC_GET_SC1_ERRORCODE:
                case MSC_GET_SC3_ERRORCODE:
                    // CTS 6.3.11.1
                    SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_BAD_OFFSET);
                    //mdrv_mhl_CbusReply(MSC_ACK, TRUE, MSC_ERRORCODE_NO_ERROR);
                    break;

                case MSC_EOF:
                    break;

                // CTS 6.3.16.1 ~ 6.3.16.2
                case MSC_SET_HPD:
                case MSC_CLR_HPD:
                    _mdrv_mhl_CbusRxReply(MSC_ABORT, FALSE, 0);
                    SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_INVALID_OPCODE);
                    break;

                default:
                    // CTS 6.3.6.2 & 6.3.11.2
                    _mdrv_mhl_CbusRxReply(MSC_ABORT, FALSE, 0);
                    SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_INVALID_OPCODE);
                    break;
            }
        }
        else // received message is not valid
        {
            MHL_DRV_DPRINTF("**Unknow: 0x%x \n", GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).cmd);
        }
    }

#endif
}

#if(MHL_CBUS_OPERATION_MODE == MHL_CBUS_HW_REPLY_MODE)
//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_RxSRAMHandler()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_mhl_RxSRAMHandler(void)
{
    BYTE xdata uctemp = 0;
    BYTE xdata ucAddress = 0;
    BYTE xdata ucLength = 0;

    if(GET_MHL_RX_SRAM_FLAG())
    {
        ucAddress = MHL_RX_XRAM_SIZE_HALF;
    }

    //MHL_DRV_DPRINTF("**MHL ============================= %d \n", GET_MHL_RX_RECEIVE_COUNTER());

    mhal_mhl_RxSRAMSwitch();

    while((GET_MHL_RX_SRAM_QUEUE(ucAddress) > 0) && GET_MHL_CBUS_CONNECT())
    {
        ucLength = GET_MHL_RX_SRAM_QUEUE(ucAddress);

        if((ucLength &BIT7) == BIT7) // Time out flag
        {
            ucLength = ucLength &MASKBIT(6:0);
        }
        else
        {
            GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).cmd = GET_MHL_RX_SRAM_QUEUE(ucAddress +2);
            //MHL_DRV_DPRINTF("**MHL Rx command %x \n", GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).cmd);

            for(uctemp = 4; uctemp < ucLength; uctemp += 2)
            {
                if((GET_MHL_RX_SRAM_QUEUE(ucAddress +uctemp -1) &BIT0) == BIT0) // Command in data error happen
                {
                    // For CTS 6.3.6.5 entry 4
                    if(GET_MHL_RX_SRAM_QUEUE(ucAddress +uctemp) == MSC_ABORT)
                    {
                        GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).cmd = GET_MHL_RX_SRAM_QUEUE(ucAddress +uctemp);
                    }
                    else if(GET_MHL_RX_SRAM_QUEUE(ucAddress +uctemp) == MSC_EOF)
                    {
                        // EOF is command but it is not error
                        GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[(uctemp -4) /2] = GET_MHL_RX_SRAM_QUEUE(ucAddress +uctemp);
                    }
                }
                else
                {
                    GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[(uctemp -4) /2] = GET_MHL_RX_SRAM_QUEUE(ucAddress +uctemp);
                    //MHL_DRV_DPRINTF("**MHL Rx data %x \n", GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[(uctemp -4) /2]);
                }
            }

            GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).len = (ucLength -1) /2;

            if(!(GET_MHL_RX_SRAM_QUEUE(ucAddress +1) &BIT0))
            {
                if(GET_MHL_SEND_READ())
                {
                    SET_MHL_CBUS_ASK_DATA(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).cmd);

                    CLR_MHL_SEND_READ();
                    SET_MHL_SEND_FINISH();
                }

                GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).cmd = MSC_ACK;
            }

            mdrv_mhl_CbusIntHandler();
        }

        if(GET_MHL_RX_RECEIVE_COUNTER() > 0)
        {
            DEC_MHL_RX_RECEIVE_COUNTER();
        }

        ucAddress += ucLength;
    }
}

#endif

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_CbusIntCB()
//  [Description]
//                  MHL Cbus Interrupt Call Back function
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_mhl_CbusIntCB(void)
{
    Bool bflag;
    CbusReq_S xdata preq;

#if(MHL_CBUS_OPERATION_MODE == MHL_CBUS_HW_REPLY_MODE)
    if((GET_MHL_RX_RECEIVE_COUNTER() > 0) || mhal_mhl_CbusIsMscMsgReceived()) // received valid message
    {
        if(!mhal_mhl_IsCbusBusy())
        {
            // Read received Cbus packet from register
            mhal_mhl_CbusIntCB(&(preq.rcState), &(preq.cmd), &(preq.msgData), &(preq.len), &bflag);

            SET_MHL_CBUS_STATE(CBUS_STATE_IDLE);
            //SET_MHL_QUEUE_STATE(MHL_CBUS_RX_QUEUE_INDEX, CBUS_REQ_IDLE);

            mdrv_mhl_RxSRAMHandler();
        }
    }

#else // Cbus SW Mode
    if(GET_MHL_RECEIVE_MSC() || mhal_mhl_CbusIsMscMsgReceived()) // received valid message
    {
        SET_MHL_RECEIVE_MSC();

        // Read received Cbus packet from register
        if(mhal_mhl_CbusIntCB(&(preq.rcState), &(preq.cmd), &(preq.msgData), &(preq.len), &bflag))
        {
            if(GET_MHL_SEND_READ())
            {
                SET_MHL_CBUS_ASK_DATA(preq.cmd);

                CLR_MHL_SEND_READ();
                SET_MHL_SEND_FINISH();
            }

            preq.cmd = MSC_ACK;
        }

        // CTS 6.3.10.5
        // CTS 6.3.12.1 ~ 6.3.12.9
        if(preq.rcState == CBUS_RECEIVE_TIMEOUT)
        {
            SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_PEER_TIMEOUT);
        }

        // CTS 6.3.11.4 ~ 6.3.11.7
        if((GET_MHL_CBUS_STATE() != CBUS_STATE_SENT) &&
			((preq.cmd == MSC_ACK) || (preq.cmd == MSC_NACK) || (preq.cmd == MSC_ABORT) ||(preq.cmd == MSC_EOF)))
        {
            SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_INVALID_OPCODE);

            if(preq.cmd != MSC_ABORT)
            {
                _mdrv_mhl_CbusRxReply(MSC_ABORT, FALSE, 0);
            }
        }

        if((preq.cmd != MSC_ACK) && (preq.cmd != MSC_NACK) && (preq.cmd != MSC_ABORT))
        {
            // Save Cbus packet into queue
            memcpy(&(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX)), &preq, sizeof(CbusReq_S));
            gMHLInfo.MhlCbusInfo.bIsCmdinDataField = bflag;

            switch(GET_MHL_CBUS_STATE())
            {
                case CBUS_STATE_IDLE:
                case CBUS_STATE_SENT:
                    SET_MHL_QUEUE_STATE(MHL_CBUS_RX_QUEUE_INDEX, CBUS_REQ_RECEIVED);
                    SET_MHL_CBUS_STATE(CBUS_STATE_RECEIVED);
                    break;

                default:
                    SET_MHL_CBUS_STATE(CBUS_STATE_IDLE);
                    break;
            }
        }
        else
        {
            // CTS 6.3.6.3
            if((GET_MHL_CBUS_STATE() == CBUS_STATE_SENT) && ((preq.cmd == MSC_ACK) && bflag))
            {
                _mdrv_mhl_CbusRxReply(MSC_ABORT, FALSE, 0);
                SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_PROTOCOL_ERROR);
            }

            // CTS 6.3.6.5
            if((GET_MHL_CBUS_STATE() == CBUS_STATE_SENT) && (preq.cmd == MSC_ABORT))
            {
                SET_MHL_RECEIVE_ABORT();
            }

            // CTS 6.3.6.4 & 6.3.6.5
            if((preq.rcState != CBUS_RECEIVE_TIMEOUT) && (preq.cmd != MSC_ABORT))
            {
                if(preq.cmd == MSC_ACK)
                {
                    SET_MHL_RECEIVE_ACK();

                    if((preq.len > 0) && GET_MHL_SEND_READ())
                    {
                        SET_MHL_CBUS_ASK_DATA(preq.msgData[0]);

                        CLR_MHL_SEND_READ();
                        SET_MHL_SEND_FINISH();
                    }
                }

                SET_MHL_CBUS_STATE(CBUS_STATE_IDLE);
            }
        }
    }

#endif
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_CbusTimerHandler()
//  [Description]
//                  MHL Cbus handle send MSC message
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_mhl_CbusTimerHandler(void)
{
    Bool bFlag = TRUE;
    BYTE xdata uctemp = 0;

    // CTS 6.3.6.4 verify DUT receives result timeout => couldn's send any MSC packet within 2sec
    // CTS 6.3.6.5 verify no next command until hold-off after Abort seen => couldn's send any MSC packet within 2sec
    if(GET_MHL_TIME_OUT_FLAG() &&(!GET_MHL_TIMEOUT_TIMER()))
    {
        MHL_DRV_DPUTSTR("**MHL TOUT");
        SET_MHL_CBUS_STATE(CBUS_STATE_IDLE);
        //SET_MHL_QUEUE_STATE(GET_MHL_TX_QUEUE_INDEX(), CBUS_REQ_IDLE);
        CLR_MHL_SEND_READ();

        _mdrv_mhl_SetControlTimer(0);

#if MHL_HUAWEI_PHONE_PATCH
        if(GET_MHL_HUAWEI_PHONE_FLAG())  //start --abnormal display after the HUAWEI cell phone to connect and dc off/on.
        {
            mhal_mhl_RxRtermControl(RX_RTERM_OFF);

            for(uctemp = 0; uctemp < 20; uctemp++)
            {
                if(mhal_mhl_CbusStatus() != 0x03)
                {
                    bFlag = FALSE;

                    break;
                }
            }

            if(bFlag)
            {
                mhal_mhl_CbusForceToStandby();
            }
        }
#endif
    }
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_IsrHandler()
//  [Description]
//                  MHL isr handler
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_mhl_IsrHandler(void)
{
#if(MHL_CBUS_USE_INTERRUPT)
    mhal_mhl_IsrMaskControl(TRUE);

    if(mhal_mhl_IsrMscReceiveFlag())
    {
        SET_MHL_RECEIVE_MSC();

        INC_MHL_RX_RECEIVE_COUNTER();
    }

    if(mhal_mhl_IsrStucktoLowFlag())
    {
        mhal_mhl_IsrRtermOff();

        CLR_MHL_CBUS_CONNECT();
    }

    mhal_mhl_IsrMaskControl(FALSE);

#endif
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_RtermControlHWMode()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_mhl_RtermControlHWMode(Bool bFlag)
{
    if(bFlag) // HW control rterm
    {
        mhal_mhl_RtermControlHWMode(TRUE);
    }
    else
    {
        if(mhal_mhl_CbusStatus() == 0x03)
        {
            mhal_mhl_RxRtermControl(RX_MHL_RTERM);
        }
        else
        {
            mhal_mhl_RxRtermControl(RX_RTERM_OFF);
        }

        mhal_mhl_RtermControlHWMode(FALSE);
    }
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_PowerCtrl(BYTE pctrl)
//  [Description]
//                  MHL power control
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_mhl_PowerControl(BYTE ucState)
{
    BYTE xdata uctemp = 0;
    BYTE xdata ucCbusPath = GET_MHL_CBUS_SELECT_PATH();

    if((ucState == MHL_POWER_STANDBY) || (ucState == MHL_POWER_DOWN))
    {
        memset(&(gMHLInfo.MhlCbusInfo), 0, sizeof(gMHLInfo.MhlCbusInfo));
        CLR_MHL_PLUGGED_FLAG();
    }

    if(ucState == MHL_POWER_STANDBY)
    {
        //mhal_mhl_DriverControl(MHL_CBUS_WAKE_UP_DETECT);

        CLR_MHL_SYSTEM_POWER_SAVING();
    }
    else if(ucState == MHL_POWER_DOWN)
    {
        _mdrv_mhl_CellPhonePatchIntoPowerDown();

        mdrv_mhl_ChargePortDetect(TRUE);
    }
    else
    {
        if(!mdrv_mhl_CbusStatus())
        {
            SET_MHL_CBUS_SELECT_PATH(MHL_CBUS_SELECT_NONE);
            mhal_mhl_CbusSwitchMux(GET_MHL_CBUS_SELECT_PATH());

            _mdrv_mhl_SetNextCbusSelect(ucCbusPath);

            mhal_mhl_RxRtermControl(RX_RTERM_OFF);

            _mdrv_mhl_CellPhonePatchPowerOn();
        }

        mhal_mhl_RtermControlHWMode(FALSE);
    }

    SET_MHL_POWER_STSTUS(ucState);
    mhal_mhl_PowerControl(ucState);

    mhal_mhl_CableDetect();
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_InsertCellPhoneID
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_mhl_InsertCellPhoneID(BYTE ucCellPhoneID)
{
    switch(GET_MHL_CBUS_SELECT_PATH())
    {
#if(MHL_SUPPORT_PORT0)
        case MHL_CBUS_SELECT_PORT0:
            SET_MHL_CELL_PHONE_ID_PORT0(ucCellPhoneID);
            break;
#endif

#if(MHL_SUPPORT_PORT1)
        case MHL_CBUS_SELECT_PORT1:
            SET_MHL_CELL_PHONE_ID_PORT1(ucCellPhoneID);
            break;
#endif

#if(MHL_SUPPORT_PORT2)
        case MHL_CBUS_SELECT_PORT2:
            SET_MHL_CELL_PHONE_ID_PORT2(ucCellPhoneID);
            break;
#endif

        default:

            break;
    };
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_ChargePortDetect
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_mhl_ChargePortDetect(Bool bReset)
{
    static BYTE xdata ucPollingCount = 0;
    static BYTE xdata ucCableDetect = 0;
    BYTE xdata ucCbusSelect = GET_MHL_CBUS_SELECT_PATH();

    if(bReset)
    {
        ucPollingCount = 1;
        ucCableDetect = 0;
    }
    else
    {
#if(MHL_POWER_DOWN_CBUS_LINK)
        if(GET_MHL_CABLE_DETECT_ALL() > 0)
        {
            if(GET_MHL_CBUS_SELECT_PATH() == MHL_CBUS_SELECT_NONE) // Cbus mux to none in previous
            {
                ucCbusSelect = _mdrv_mhl_SearchNextCbusSelect(GET_MHL_CBUS_SELECT_PATH());
            }
        }
        else
        {
            ucCbusSelect = MHL_CBUS_SELECT_NONE;
        }

        if(GET_MHL_CBUS_SELECT_PATH() != ucCbusSelect)
        {
            mhal_mhl_CbusSwitchMux(ucCbusSelect);

            SET_MHL_CBUS_SELECT_PATH(ucCbusSelect);
        }

#endif

        mhal_mhl_CableDetect();

        if(ucPollingCount < 4)
        {
            ucPollingCount++;
        }
        else
        {
            ucPollingCount = 0;

            if(ucCableDetect != GET_MHL_CABLE_DETECT_ALL())
            {
                _mdrv_mhl_CellPhonePatchPowerDownPolling();

                ucCableDetect = GET_MHL_CABLE_DETECT_ALL();
            }
        }
    }
}

#if MS_PM
//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_WakeupDetect()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mdrv_mhl_WakeupDetect(Bool bAutoSearch)
{
    Bool bWakeup = FALSE;
    BYTE xdata ucPort = GET_MHL_PORT_SELECT();
    BYTE xdata ucCbusSelect = MHL_CBUS_SELECT_NONE;
    static BYTE xdata ucNextCbusSelectPM = MHL_CBUS_SELECT_NONE;

    if(!GET_MHL_SYSTEM_POWER_SAVING())
    {
        // Initial PM process first into Cbus path switch to path none case
        CLR_MHL_CBUS_PATH_SWITCH();

        SET_MHL_SYSTEM_POWER_SAVING();

        mdrv_mhl_RtermControlHWMode(FALSE);

#if(MHL_PM_USE_RTERM_HW_MODE)
        mdrv_mhl_RtermControlHWMode(TRUE);
#endif

        SET_MHL_POLLING_TIMER(0);
    }
    else if(mdrv_mhl_CbusStatus())
    {
        ucPort = mhal_mhl_CbusToPortMapping(GET_MHL_CBUS_SELECT_PATH());
        SET_MHL_PORT_SELECT(ucPort);

        mhal_mhl_CbusConnectProc(GET_MHL_PORT_SELECT());

        mhal_mhl_RtermControlHWMode(TRUE);

        bWakeup = TRUE;

        MHL_DRV_DPRINTF("## MHL PM wake up in port = %d ...\n", ucPort);
    }

    if(!bWakeup)
    {
        if(!GET_MHL_POLLING_TIMER())
        {
            if(GET_MHL_CBUS_PATH_SWITCH())
            {
                SET_MHL_POLLING_TIMER(MHL_PM_WAKEUP_CHECK_INTERVAL);

                SET_MHL_CBUS_SELECT_PATH(ucNextCbusSelectPM);

                mhal_mhl_CbusSwitchMux(GET_MHL_CBUS_SELECT_PATH());

#if(MHL_PM_USE_RTERM_HW_MODE)
                mhal_mhl_RtermControlHWMode(TRUE);
#endif

                MHL_DRV_DPRINTF("## MHL PM scan to port = %d ...\r\n", ucPort);

                CLR_MHL_CBUS_PATH_SWITCH();
            }
            else // Cbus path switch to path none case
            {
                if(bAutoSearch) // Auto search all cable detect high port
                {
                    if(GET_MHL_CABLE_DETECT_ALL() > 0)
                    {
                        if(GET_MHL_CBUS_SELECT_PATH() == MHL_CBUS_SELECT_NONE) // Cbus mux to none in previous
                        {
                            ucCbusSelect = _mdrv_mhl_SearchNextCbusSelect(mhal_mhl_PortToCbusMapping(ucPort));
                        }
                        else
                        {
                            ucCbusSelect = _mdrv_mhl_SearchNextCbusSelect(GET_MHL_CBUS_SELECT_PATH());
                        }
                    }
                    else
                    {
                        ucCbusSelect = MHL_CBUS_SELECT_NONE;
                    }
                }
                else // Only search PM sleep port
                {
                    if(_mdrv_mhl_GetCableDetectPort(ucPort))
                    {
                        ucCbusSelect = mhal_mhl_PortToCbusMapping(ucPort);
                    }
                    else
                    {
                        ucCbusSelect = MHL_CBUS_SELECT_NONE;
                    }
                }

                if(GET_MHL_CBUS_SELECT_PATH() != ucCbusSelect)
                {
                    SET_MHL_POLLING_TIMER(MHL_PM_CBUS_NONE_INTERVAL);

                    ucNextCbusSelectPM = ucCbusSelect;

#if(MHL_PM_USE_RTERM_HW_MODE)
                    mhal_mhl_RtermControlHWMode(FALSE);
#endif

                    SET_MHL_CBUS_SELECT_PATH(MHL_CBUS_SELECT_NONE);

                    mhal_mhl_CbusSwitchMux(GET_MHL_CBUS_SELECT_PATH());

                    SET_MHL_CBUS_PATH_SWITCH();
                }
            }
        }

        if(!mhal_mhl_CableDetectPM())
        {
            SET_MHL_POLLING_TIMER(0);
            mhal_mhl_RtermControlHWMode(FALSE);
        }
    }

    return bWakeup;
}

#endif
/*
#else
BYTE code msMdrvMHLNullData[] = {0};

void msMdrvMHLDummy(void)
{
    BYTE xdata i = msMdrvMHLNullData[0];
}
*/
#endif //#if ENABLE_MHL

#endif // _MDRV_MHL_C_

