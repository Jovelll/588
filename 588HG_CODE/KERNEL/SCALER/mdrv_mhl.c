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


//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
xdata MHLInfo_S gMHLInfo;
idata bCbusTimerFlag = FALSE;
WORD idata usCbusTimerCnt = 0;
BYTE xdata ucMHLRxSRAM[MHL_RX_XRAM_SIZE_TOTAL] _at_ MHL_CBUS_SRAM_ADDRESS;

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------

code BYTE tMHL_DEVCAP_TABLE[] = 
{
    0x00,    //DEV_STATE
    0x12,    //MHL_VERSION, 1.2
    0x11,    //DEV_CAT, [4]:POW, [3:0]:DEV_TYPE
    (DMHL_ADOPTER_ID >>8),    // ADOPTER_ID_H, Mstar adopter ID 715(0x2CB)
    (DMHL_ADOPTER_ID &0xFF),    // ADOPTER_ID_L 
#if DMHL_CTS // not support packed-pixel mode
    0x37,    // VID_LINK_MODE, [5]:SUPP_VGA, [4]:ISLANDS, [3]:PPIXEL, [2]:YUV422, [1]:YUV444, [0]:RGB444
#else
    0x3F,    // VID_LINK_MODE, [5]:SUPP_VGA, [4]:ISLANDS, [3]:PPIXEL, [2]:YUV422, [1]:YUV444, [0]:RGB444
#endif
    0x01,    // AUD_LINK_MODE, [1]:AUD_8CH, [0]:AUD_2CH
    0x00,    // VIDEO_TYPE, [7]: SUPP_VT, [3]:GAME, [2]:CINEMA, [1]: PHOTO, [0]:GRAPHICS
    0x01,    // LOG_DEV_MAP, [0]:LD_DISPLAY
    0x0F,    // BANDWIDTH, 15*5MHz = 75MHz
    0x07,    // FEATURE_FLAG, [2]:Scratchpad, [1]:RAP, [0]:RCP
    (DMHL_DEVICE_ID >>8),    // DEVICE_ID_H, ? CHIP_ID
    (DMHL_DEVICE_ID &0x00FF),    // DEVICE_ID_L, ? CHIP_ID
    MHL_MSC_SCRATCHPAD_SIZE,    // SCRATCHPAD_SIZE, 16 bytes
    0x33,    // INT_STAT_SIZE, [7:4]:Status 4 bytes, [3:0]:Interrupt 4 bytes
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
    BYTE uctemp = BIT0;

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
    BYTE ucindex = GET_CBUS_LINK_CHECK() ^MHL_CBUS_LINK_MASK;
    
    if(ucindex > 0)
    {
        SET_CBUS_LINK_CHECK(GET_CBUS_LINK_CHECK() |_mdrv_mhl_CbusGetLeastFlag(ucindex));

        MHL_DPUTSTR("**MHL Cbus Receive ACK for Link");
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
    CbusReq_S req = {0};

    req.cmd = MSC_MSC_MSG;
    req.msgData[0] = subCmd; 
    req.msgData[1] = databuf;
    req.len = 2;
    req.reqStatus = CBUS_REQ_RECEIVED;
    
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
    BYTE uctemp = ucIndex;

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
    WORD usPacket = (ucHeader <<9) |(bControl <<8) |ucContent;

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
    mhalCbusFifo_S pCbusFifo;

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
        MHL_DPRINTF("@@Cbus Tx is busy = 0x%x", ucCommand);
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
    Bool bindex = TRUE;
    BYTE uctemp = 0;
    mhalCbusFifo_S pCbusFifo;

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
            MHL_DPRINTF("@@Cbus Tx is busy = 0x%x", pReq->cmd);
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
void _mdrv_mhl_Cbus_SetPathEn(void)
{
    Bool bindex = _mdrv_mhl_CbusWriteStatusOrSetInt(0x31, BIT3);

    if(bindex)
    {
        SET_MHL_PATH_EN_FLAG();
        mhal_mhl_Cbus_SetPathEn(bindex);
    }
    else
    {
        CLR_MHL_PATH_EN_FLAG();
    }
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
    ucData = ucData;

    switch(ucCmd)
    {
        case MSC_MSG_RCP:
        case MSC_MSG_RAP:
            SET_MHL_QUEUE_STATE(MHL_CBUS_RX_QUEUE_INDEX, CBUS_REQ_RECEIVED);
            SET_MHL_CBUS_STATE(CBUS_STATE_RECEIVED);
            
            break;

        case MSC_MSG_RCPK:
            
            break;

        case MSC_MSG_RCPE:
            //MHL_DPUTSTR("**MHL_MSC_MSG_RCPE\n");
            break;
            
        case MSC_MSG_RAPK:
            //MHL_DPUTSTR("**MHDL_MSC_MSG_RAPK\n");
            break;
			
        default:
            bindex = FALSE;
            break;          
    };

    return bindex;
}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_CBusClockModeSwitch()
//  [Description]:
//                  MHL Cbus clock mode select
//  [Arguments]:
//                  
//  [Return]:
//
//**************************************************************************
void _mdrv_mhl_CBusClockModeSwitch(BYTE ucSelect)
{
    switch(ucSelect)
    {
        case CBUS_CLOCK_PACKET_PIXEL:
            mhal_mhl_DriverControl(MHL_CBUS_PACKET_PIXEL_MODE);
            break;

        case CBUS_CLOCK_NORMAL_24BIT:
            mhal_mhl_DriverControl(MHL_CBUS_NORMAL_24BIT_MODE);
            break;
			
        default:
            
            break;          
    };
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
    
    mhal_mhl_Initial(pEdid, &tMHL_DEVCAP_TABLE);
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_CbusIsolate()
//  [Description]
//                  MHL cable isolate
//  [Arguments]:
//
//  [Return]:
//                  
//**************************************************************************
void mdrv_mhl_CbusIsolate(Bool bFlag)
{
    mhal_mhl_CbusIsolate(bFlag);

    if(bFlag)
    {
        _mdrv_mhl_SetControlTimer(0);
    }
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_InputSelect()
//  [Description]
//                  MHL input port select
//  [Arguments]:
//
//  [Return]:
//                  
//**************************************************************************
void mdrv_mhl_InputSelect(BYTE ucPort)
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
    else
    {
        ucPort = MHL_INPUT_NONE;
    }
    
    if(GET_MHL_PORT_SELECT() != ucPort)
    {
        SET_MHL_PORT_SELECT(ucPort);
        
        mhal_mhl_PortSwitchMux(ucPort);

        mdrv_mhl_CbusIsolate(TRUE);
        
        CLR_CBUS_CABLE_FLAG();
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
Bool mdrv_mhl_CableDetect(void)
{
    if(mhal_mhl_CableDetect())
    {  
        if(!GET_MHL_CABLE_PLUGGED())
        {
            mhal_mhl_DriverControl(MHL_CABLE_PLUGGED);
            
            SET_MHL_CABLE_PLUGGED();

            MHL_DPUTSTR("** MHL Cable Plugged!\n");
        }
    }
    else
    {  
        if(GET_MHL_CABLE_PLUGGED() ||(!GET_MHL_PORT_ON_FLAG()))
        {
            SET_MHL_TX_QUEUE_INDEX(0);
            SET_MHL_CBUS_STATE(CBUS_STATE_IDLE);
            SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_NO_ERROR);
            CLR_CBUS_CABLE_FLAG();
            CLR_CBUS_LINK_CHECK();

            mhal_mhl_DriverControl(MHL_CABLE_UNPLUGGED);

            SET_MHL_PORT_ON_FLAG();
            MHL_DPUTSTR("** MHL Cable not Plugged!\n");
        }
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
    WORD usStatus;

    usStatus = mhal_mhl_CbusStatus();

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
//                  mdrv_mhl_CDRModeMonitor()
//  [Description]
//                  MHL CDR mode setting
//  [Arguments]:
//                  
//  [Return]:
//                  
//**************************************************************************
void mdrv_mhl_CDRModeMonitor(void)
{
    mhal_mhl_CDRModeMonitor(GET_CBUS_LINK_FINISH());
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_CheckDEStable()
//  [Description]
//                  MHL check DE stable
//  [Arguments]:
//                  
//  [Return]:
//                  
//**************************************************************************
Bool mdrv_mhl_CheckDEStable(void)
{
    return mhal_mhl_CheckDEStable();
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

    stQueue.cmd = MSC_WRITE_BURST;
    stQueue.len = ucLength +1; // offset + data packets
    stQueue.msgData[0] = ucAddr; // offset packet
    stQueue.reqStatus = CBUS_REQ_PENDING;
    
    for(ucindex =0; ucindex <ucLength; ucindex++)
    {
        stQueue.msgData[ucindex +1] = pBuffer[ucindex];
    }

    if(!_mdrv_mhl_CbusQueueMscTxRequest(GET_MHL_TX_QUEUE_INDEX(), &stQueue))
    {
        MHL_DPUTSTR("Couldn't write burst\n");
        
        return FALSE;
    }
    
    return TRUE;
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
            _mdrv_mhl_Cbus_SetPathEn();
            break;

        case CBUS_DCAP_CHG_FLAG:
            _mdrv_mhl_Cbus_SetDCapChg();
            break;

        case MHL_CBUS_LINK_MASK:    // Uncall function
            mdrv_mhl_CbusMscMsgSubCmdSend(1, 1);
            mdrv_mhl_CbusWriteBurst(1, 1, 1);
            mdrv_mhl_CheckDEStable();
            break;

        default:
            
            break;
    };
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
    static Bool bCbusConnected = FALSE;
    BYTE ucindex = 0;

    if(bCbusConnected != GET_MHL_CBUS_CONNECT())
    {
        bCbusConnected = GET_MHL_CBUS_CONNECT();

        if(!bCbusConnected)
        {
            SET_MHL_TX_QUEUE_INDEX(0);
            SET_MHL_CBUS_STATE(CBUS_STATE_IDLE);
            SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_NO_ERROR);
            CLR_CBUS_LINK_FLAG();
            CLR_CBUS_LINK_CHECK();

            _mdrv_mhl_SetControlTimer(0);    // timeout reset.

            mhal_mhl_DriverControl(MHL_CBUS_UNCONNECTION);

            memset(&gMHLInfo.MhlCbusInfo.request, 0, sizeof(gMHLInfo.MhlCbusInfo.request));	
			
            if(!mdrv_mhl_CableDetect())
            {
                CLR_CBUS_CABLE_FLAG();
                mhal_mhl_DriverControl(MHL_CABLE_UNPLUGGED);
            }
            
            MHL_DPUTSTR("** MHL Cbus not connection");
        }
        else
        {	
            if((GET_MHL_POWER_STSTUS() != MHL_POWER_ON) && PowerOnFlag)
            {
                mdrv_mhl_PowerControl(MHL_POWER_ON);
            }
            
            SET_MHL_DEBOUNCE_TIMER(150); // reset 150msec

            mhal_mhl_DriverControl(MHL_CBUS_CONNECTION);

            CLR_MHL_PORT_ON_FLAG();
            MHL_DPUTSTR("** MHL Cbus connected");
        }
    }
	
    if(bCbusConnected)
    {
        ucindex = GET_CBUS_LINK_FLAG() ^MHL_CBUS_LINK_MASK;

        if(ucindex > 0)
        {
            mdrv_mhl_CbusSendCommand(_mdrv_mhl_CbusGetLeastFlag(ucindex));
        }
    }

    mdrv_mhl_CDRModeMonitor();
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
    BYTE uctemp = 0;

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
    BYTE ucindex = 0;
    
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
                        MHL_DPUTSTR("** MHL Tx Packet Retry");
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
            if(GET_MHL_QUEUE_STATE(MHL_CBUS_RX_QUEUE_INDEX) != CBUS_REQ_IDLE)
            {
                SET_MHL_QUEUE_STATE(MHL_CBUS_RX_QUEUE_INDEX, CBUS_REQ_IDLE);
            }
            else
            {
                SET_MHL_QUEUE_STATE(GET_MHL_TX_QUEUE_INDEX(), CBUS_REQ_IDLE);
            }
            
            ucindex = mdrv_mhl_CBusGetQueueIndex(GET_MHL_TX_QUEUE_INDEX());

            // Update current index to search index, but not Rx queue index
            if(ucindex < MHL_CBUS_RX_QUEUE_INDEX)
            {
                SET_MHL_TX_QUEUE_INDEX(ucindex);
            }

            _mdrv_mhl_CbusLinkCheck();
            
            SET_MHL_CBUS_STATE(CBUS_STATE_IDLE);
            
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
    
    switch(GET_MHL_QUEUE_STATE(MHL_CBUS_RX_QUEUE_INDEX))
    {
        case CBUS_REQ_IDLE: 
        case CBUS_REQ_PENDING:
        case CBUS_REQ_SENT:
            
            break;
			
        case CBUS_REQ_RECEIVED:
            *ucCmd = GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[0];
            *ucData = GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[1];
            SET_MHL_CBUS_STATE(CBUS_STATE_IDLE);
            bindex = TRUE;
            
            break;
            
        default:
            
            break;
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
    BYTE ucCount = 0;

#if(DMHL_CBUS_HW_MODE)
    switch(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).cmd)
    {
        case MSC_ACK:
            SET_MHL_CBUS_STATE(CBUS_STATE_ACK);
            mdrv_mhl_CbusTxHandler();
            break;

        case MSC_ABORT: // wait 2 sec before starting a new Cbus transaction after receiving an ABORT, Chapter 13.10.3 
            _mdrv_mhl_SetControlTimer(CBUS_ABORT_PADING_SECS);
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
                _mdrv_mhl_CBusClockModeSwitch(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[1] &(BIT2 |BIT1 |BIT0));
            }
            break;

        case MSC_RESERVED:
            _mdrv_mhl_CbusRxReply(MSC_ABORT, FALSE, 0);
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
                    SET_MHL_CBUS_STATE(CBUS_STATE_ACK);
                    break;
        
                case MSC_NACK:
                    break;
        			
                case MSC_ABORT: // wait 2 sec before starting a new Cbus transaction after receiving an ABORT, Chapter 13.10.3 
                    _mdrv_mhl_SetControlTimer(CBUS_ABORT_PADING_SECS);
                    break;	
        
                case MSC_WRITE_STAT_OR_SET_INT:
                    // CTS 6.3.11.8 && 6.3.11.11
                    if(gMHLInfo.MhlCbusInfo.bIsCmdinDataField)
                    {
                        SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_PROTOCOL_ERROR);
                        bretAck = FALSE;
                    }

                    if(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[0] == 0x31)
                    {
                        _mdrv_mhl_CBusClockModeSwitch(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[1] &(BIT2 |BIT1 |BIT0));
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
                    _mdrv_mhl_CbusRxReply(MSC_GET_VENDOR_ID, TRUE, DMHL_VENDOR_ID);
                    break;
        
                case MSC_MSC_MSG:
                    bretAck = _mdrv_mhl_CBusProcessMSCSubCmd(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[0], GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[1]);
                    // HW auto reply, REG_MHL_CBUS_23[9:8] = 2'b11

                    // CTS 6.3.11.22 & 6.3.11.24
                    if(gMHLInfo.MhlCbusInfo.bIsCmdinDataField)
                    {
                        SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_PROTOCOL_ERROR);
                        bretAck = FALSE;
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
            MHL_DPRINTF("**Unknow: 0x%x \n", GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).cmd);
        }        
    }

#endif
}

#if(DMHL_CBUS_HW_MODE)
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
    BYTE uctemp = 0;
    BYTE ucAddress = 0;

    if(GET_MHL_RX_SRAM_FLAG())
    {
        ucAddress = MHL_RX_XRAM_SIZE_HALF;
    }

    mhal_mhl_RxSRAMSwitch();
    
    while(GET_MHL_RX_SRAM_QUEUE(ucAddress) > 0)
    {
        GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).cmd = GET_MHL_RX_SRAM_QUEUE(ucAddress +2);
        //MHL_DPRINTF("**MHL Rx command %x \n", GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).cmd);
        
        for(uctemp = 4; uctemp < GET_MHL_RX_SRAM_QUEUE(ucAddress); uctemp += 2)
        {
            GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[(uctemp -4) /2] = GET_MHL_RX_SRAM_QUEUE(ucAddress +uctemp);
            //MHL_DPRINTF("**MHL Rx data %x \n", GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[(uctemp -4) /2]);
        }

        mdrv_mhl_CbusIntHandler();
        
        ucAddress += GET_MHL_RX_SRAM_QUEUE(ucAddress);
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
    xdata CbusReq_S preq;
    
#if(DMHL_CBUS_HW_MODE)
    if(mhal_mhl_CbusIsMscMsgReceived()) // received valid message 
    {
        if(GET_MHL_TIME_OUT_FLAG())
        {
            _mdrv_mhl_SetControlTimer(0);    // timeout reset.
        }

        // Read received Cbus packet from register
        mhal_mhl_CbusIntCB(&(preq.rcState), &(preq.cmd), &(preq.msgData), &(preq.len), &bflag);

        SET_MHL_CBUS_STATE(CBUS_STATE_IDLE);
        
        mdrv_mhl_RxSRAMHandler();

        memcpy(&(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX)), &preq, sizeof(CbusReq_S));
    }
    
#else // Cbus SW Mode
    if(mhal_mhl_CbusIsMscMsgReceived()) // received valid message 
    {
        if(GET_MHL_TIME_OUT_FLAG())
        {
            _mdrv_mhl_SetControlTimer(0);    // timeout reset.
        }

        // Read received Cbus packet from register
        mhal_mhl_CbusIntCB(&(preq.rcState), &(preq.cmd), &(preq.msgData), &(preq.len), &bflag);

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
                _mdrv_mhl_SetControlTimer(CBUS_ABORT_PADING_SECS);
            }
            
            // CTS 6.3.6.4 & 6.3.6.5
            if((preq.rcState != CBUS_RECEIVE_TIMEOUT) && (preq.cmd != MSC_ABORT))
            {
                if(preq.cmd == MSC_ACK)
                {
                    SET_MHL_CBUS_STATE(CBUS_STATE_ACK);
                }
                else
                {
                    SET_MHL_CBUS_STATE(CBUS_STATE_IDLE);
                }
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
    // CTS 6.3.6.4 verify DUT receives result timeout => couldn's send any MSC packet within 2sec  
    // CTS 6.3.6.5 verify no next command until hold-off after Abort seen => couldn's send any MSC packet within 2sec
    if(GET_MHL_TIME_OUT_FLAG() &&(!GET_MHL_TIMEOUT_TIMER()))
    {
        MHL_DPUTSTR("**MHL TOUT");
        SET_MHL_CBUS_STATE(CBUS_STATE_IDLE);
        SET_MHL_QUEUE_STATE(GET_MHL_TX_QUEUE_INDEX(), CBUS_REQ_IDLE);

        _mdrv_mhl_SetControlTimer(0);
    }
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_SetTMDSTxOnOff()
//  [Description]:
//                  HDMITx Set On/OFF
//  [Arguments]:
//                  TRUE: Enable
//                  FALSE: Disable
//  [Return]:
//   
//**************************************************************************
void mdrv_mhl_SetTMDSTxOnOff(Bool bFlag)
{
    mhal_mhl_SetTMDSTxOnOff(bFlag);
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
    if((ucState == MHL_POWER_STANDBY) || (ucState == MHL_POWER_DOWN))
    {
        memset(&(gMHLInfo.MhlCbusInfo), 0, sizeof(gMHLInfo.MhlCbusInfo));
        mhal_mhl_MHLForceToStandby();
    }
    
    if(ucState == MHL_POWER_STANDBY)
    {
        mhal_mhl_RxRtermControl(GET_MHL_CABLE_PLUGGED() ? RX_RTERM_OFF: RX_MHL_RTERM);
    }
    else if(ucState == MHL_POWER_DOWN)
    {
        mhal_mhl_RxRtermControl(RX_RTERM_OFF);
    }
    else
    {
        mhal_mhl_RxRtermControl(RX_HDMI_RTERM);
    }
    
    SET_MHL_POWER_STSTUS(ucState);
    mhal_mhl_PowerControl(ucState);
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_SetHPD()
//  [Description]
//                  config HPD in combo(MHL/HDMI) port A
//  [Arguments]:
//                  
//  [Return]:
//
//**************************************************************************
#if 0
void mdrv_mhl_SetHPD(Bool bFlag)
{
    if(!GET_MHL_CABLE_PLUGGED())
    {
        mhal_mhl_SetHPD(bFlag);
    }
}
#endif
#ifdef _MS_PM_
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
Bool mdrv_mhl_WakeupDetect(void)
{
    if(mdrv_mhl_CableDetect()) // MHL
    {
        if(mdrv_mhl_CbusStatus())
        {
            mhal_mhl_DriverControl(MHL_CBUS_WAKE_UP_DETECT);
            
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
}

#endif

#else
BYTE code msMdrvMHLNullData[] = {0};

void msMdrvMHLDummy(void)
{
    BYTE xdata i = msMdrvMHLNullData[0];
}

#endif //#if ENABLE_MHL

#endif // _MDRV_MHL_C_

