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
#include "mhal_mhl.h"
#include "mdrv_mhl.h"
*/

#if ENABLE_MHL


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

#define MHL_DBG     0

#if  MHL_DBG && ENABLE_DEBUG
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
WORD xdata g_u16S2PatchCnt = 0;
bit bS2PatchCntFlag = 0;
BYTE xdata g_u8ErrorCode = 0;

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------

code BYTE MhlDevCap_tbl[] = 
{
    0x00,    //DEV_STATE
    0x12,    //MHL_VERSION, 1.1
    0x11,    //DEV_CAT, [4]:POW, [3:0]:DEV_TYPE
#if DMHL_DEVCAP_HWACK
    0x00,    // dummy
    (DMHL_ADOPTER_ID &0xFF),    // ADOPTER_ID_L 
    (DMHL_ADOPTER_ID >>8),    // ADOPTER_ID_H, Mstar adopter ID 715(0x2CB)
#else
    (DMHL_ADOPTER_ID >>8),    // ADOPTER_ID_H, Mstar adopter ID 715(0x2CB)
    (DMHL_ADOPTER_ID &0xFF),    // ADOPTER_ID_L 
#endif // #if DMHL_DEVCAP_HWACK
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
#if DMHL_DEVCAP_HWACK
    (DMHL_DEVICE_ID &0x00FF),    // DEVICE_ID_L, ? CHIP_ID
    (DMHL_DEVICE_ID >>8),    // DEVICE_ID_H, ? CHIP_ID
#else
    (DMHL_DEVICE_ID >>8),    // DEVICE_ID_H, ? CHIP_ID
    (DMHL_DEVICE_ID &0x00FF),    // DEVICE_ID_L, ? CHIP_ID
#endif // #if DMHL_DEVCAP_HWACK
    //0x10,    // SCRATCHPAD_SIZE, 16 bytes
    MHL_MSC_SCRATCHPAD_SIZE,    // SCRATCHPAD_SIZE, 16 bytes
    0x33,    // INT_STAT_SIZE, [7:4]:Status 4 bytes, [3:0]:Interrupt 4 bytes
    0x00,    // Reserved
};

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_init()
//  [Description]
//                  MHL init
//  [Arguments]:
//                  *edid: MHL EDID data
//                  *devcap: MHL device capability
//  [Return]:
//
//**************************************************************************
Bool mdrv_mhl_init(BYTE *edid)
{
    memset(&gMHLInfo, 0, sizeof(gMHLInfo));
    //mhal_mhl_init(edid, &MhlDevCap_tbl);
    if(mhal_mhl_init(edid, &MhlDevCap_tbl) == FALSE)
        return FALSE;
    return TRUE;
}
//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_SetControlTimer()
//  [Description]
//                  
//  [Arguments]:
//                  
//  [Return]:
//
//**************************************************************************
void mdrv_mhl_SetControlTimer(BYTE ucTime)
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
        mdrv_mhl_SetControlTimer(0);
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
void mdrv_mhl_PowerCtrl(BYTE pctrl)
{
    if( (pctrl == MHL_POWER_STANDBY) || (pctrl == MHL_POWER_DOWN) )
    {
        memset(&(gMHLInfo.MhlCbusInfo), 0, sizeof(gMHLInfo.MhlCbusInfo));
        mhal_mhl_MHLForceToStandby();
    }
    
    if(pctrl == MHL_POWER_STANDBY)
    {
        mhal_mhl_RxRtermControl(GET_MHL_CABLE_PLUGGED() ? RX_RTERM_OFF: RX_MHL_RTERM);
    }
    else if(pctrl == MHL_POWER_DOWN)
    {
        mhal_mhl_RxRtermControl(RX_RTERM_OFF);
    }
    else
    {
        mhal_mhl_RxRtermControl(RX_HDMI_RTERM);
    }

    gMHLInfo.MhlPowerStatus = pctrl;
    mhal_mhl_PowerCtrl(pctrl);
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
    WORD status;

    status = mhal_mhl_CbusStatus();

    if(GET_MHL_CBUS_CONNECT() && ((status & 0x03) != 0x03))
    {
        SET_MHL_CBUS_STATE(CBUS_STATE_IDLE);
    }

    if((status & 0x03) == 0x03)
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
            mhal_mhl_LoadMhl24bitsModeTbl();
            SET_MHL_PS_CTRL_CHARGE0();
            //mhal_mhl_MHLForceToStandby();
            //if(!bIsFirstPowerOn)
#if 0
            {
                mhal_mhl_MHLForceToFloating(TRUE);
                mhal_mhl_MHLForceToFloating(FALSE);
                //bIsFirstPowerOn = TRUE;
            }
#endif
            //mhal_mhl_RxRtermControl(RX_RTERM_OFF);
            SET_MHL_CABLE_PLUGGED();
            MHL_DPUTSTR("** MHL Cable Connected!\n");
        }
    }
    else
    {  
        if(GET_MHL_CABLE_PLUGGED() ||(!GET_MHL_PORT_ON_FLAG()))
        {
            gMHLInfo.MhlCbusInfo.curIdx = 0;
            SET_MHL_CBUS_STATE(CBUS_STATE_IDLE);
            SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_NO_ERROR);
            CLR_CBUS_CABLE_FLAG();
            CLR_CBUS_LINK_CHECK();
            
            mhal_mhl_MHLForceToStandby();
            CLR_MHL_PS_CTRL_CHARGE0();
            mhal_mhl_LoadHdmiBypassTbl();
            mhal_mhl_RxRtermControl(RX_HDMI_RTERM);
            MHL_DPUTSTR("** MHL Cable Disconnected!\n");
        }
    }

    if(!GET_MHL_PORT_ON_FLAG())
    {
        mhal_mhl_CbusIsolate(FALSE);
        
        SET_MHL_PORT_ON_FLAG();
    }

    return GET_MHL_CABLE_PLUGGED();
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
//                  mdrv_mhl_CbusQueueMscRxReply(BYTE subCmd, BYTE databuf)
//  [Description]
//                  MHL Cbus Reply MSC RCP or RAP into Queue
//  [Arguments]:
//                  subCmd: MSC subcommands
//                  databuf: command code
//  [Return]:
//                  TRUE: success
//                  FALSE: fail
//**************************************************************************
Bool mdrv_mhl_CbusQueueMscRxReply(BYTE subCmd, BYTE databuf)
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
//                  mdrv_mhl_CbusQueueMscTxRequest(BYTE ucIndex, CbusReq_S *pReq)
//  [Description]
//                  MHL Cbus Request a MSC Cmd into Queue
//  [Arguments]:
//                  ucIndex: Current queue index
//                  pReq: Command buffer
//  [Return]:
//                  TRUE: success
//                  FALSE: fail
//**************************************************************************
Bool mdrv_mhl_CbusQueueMscTxRequest(BYTE ucIndex, CbusReq_S *pReq)
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

#if 0//DMHL_CTS
//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_CbusMscMsgSubCmdSend (BYTE subCmd, BYTE databuf)
//  [Description]
//                  MHL Cbus MSC Send RCP or RAP
//  [Arguments]:
//                  subCmd: MSC subcommands
//                  databuf: command code
//  [Return]:
//                  TRUE: success
//                  FALSE: fail
//**************************************************************************
Bool mdrv_mhl_CbusMscMsgSubCmdSend(BYTE subCmd, BYTE databuf)
{
    CbusReq_S req;

    req.cmd = MSC_MSC_MSG;
    req.msgData[0] = subCmd; 
    req.msgData[1] = databuf;
    req.len = 2;
    req.reqStatus = CBUS_REQ_PENDING;
    
    return mdrv_mhl_CbusQueueMscTxRequest(gMHLInfo.MhlCbusInfo.curIdx, &req);
}
#endif

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
Bool mdrv_mhl_CbusRcpReply(BYTE statusCode, BYTE keyCode)
{
    if(statusCode != MSC_RCP_NO_ERROR)
    {
        return mdrv_mhl_CbusQueueMscRxReply(MSC_MSG_RCPE, statusCode);
    }
    else
    {
        return mdrv_mhl_CbusQueueMscRxReply(MSC_MSG_RCPK, keyCode);
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
Bool mdrv_mhl_CbusRapReply(BYTE statusCode)
{
    return mdrv_mhl_CbusQueueMscRxReply(MSC_MSG_RAPK, statusCode);
}

#if ENABLE_DEBUG
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
Bool mdrv_mhl_CbusWriteBurst (BYTE addr, BYTE lens, BYTE *buf)
{
    BYTE ucindex = 0;
    CbusReq_S xdata req;

    req.cmd = MSC_WRITE_BURST;
    req.len = lens +1; // offset + data packets
    req.msgData[0] = addr; // offset packet
    req.reqStatus = CBUS_REQ_PENDING;
    
    for(ucindex =0; ucindex <lens; ucindex++)
    {
        req.msgData[ucindex +1] = buf[ucindex];
    }

    if(!mdrv_mhl_CbusQueueMscTxRequest(gMHLInfo.MhlCbusInfo.curIdx, &req))
    {
        MHL_DPUTSTR("Couldn't write burst\n");
        
        return FALSE;
    }
    
#if 0
    else
    {
        mdrv_mhl_CbusWriteStatusOrSetInt(0x20, BIT1); // DSCR_CHG
    }
#endif

    return TRUE;
}

#endif

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_CbusWriteStatusOrSetInt(BYTE addr, BYTE val)
//  [Description]:
//                  MHL Cbus write status or set interrupt
//  [Arguments]:
//
//  [Return]:
//                  TRUE: Cbus message send successfully
//                  FALSE: Cbus message send fail       
//**************************************************************************
Bool mdrv_mhl_CbusWriteStatusOrSetInt(BYTE addr, BYTE val)
{
#if 1

    CbusReq_S xdata req;
    
    req.cmd = MSC_WRITE_STAT_OR_SET_INT;
    req.msgData[0] = addr;
    req.msgData[1] = val;
    req.len = 2;
    req.reqStatus = CBUS_REQ_PENDING;

    if(!mdrv_mhl_CbusQueueMscTxRequest(gMHLInfo.MhlCbusInfo.curIdx, &req))
    {
        //MHL_DPUTSTR("**Couldn't write status or set INT\n");
        return FALSE;
    }
    
    return TRUE;

#else

    //mdrvCbusParityCal_S pCbusParity;
    mhalCbusFifo_S  pCbusFifo;

    gMHLInfo.MhlCbusInfo.MhlCbusState = CBUS_STATE_SENT;

    //pCbusParity.header = 0x02;
    // payload 1
    //pCbusFifo.databuf[0] = 0x8560;
    pCbusFifo.databuf[0] = mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_CONTROL, MSC_WRITE_STAT_OR_SET_INT);
    // payload 2			
    //pCbusParity.control = 0x00;
    //pCbusParity.databuf = addr;
    //pCbusFifo.databuf[1] = BIT15 | _mdrv_mhl_CbusParityCalculate(&pCbusParity);
    pCbusFifo.databuf[1] = mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_DATA, addr);
    // payload 3
    //pCbusParity.control = 0x00;
    //pCbusParity.databuf = val;
    //pCbusFifo.databuf[2] = BIT15 | _mdrv_mhl_CbusParityCalculate(&pCbusParity);
    pCbusFifo.databuf[2] = mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_DATA, val);
    // message length
    pCbusFifo.lens = 3;

    if(!mhal_mhl_CBusWrite(&pCbusFifo))
        return FALSE;

    return TRUE;

#endif

}

//**************************************************************************
//  [Function Name]:
//                  _mdrv_mhl_Cbus_SetHPDState()
//  [Description]:
//                  MHL Cbus set HPD state
//  [Arguments]:
//                  TRUE: SET_HPD
//                  FALSE: CLR_HPD
//  [Return]:
//    
//**************************************************************************
Bool mdrv_mhl_Cbus_SetHPDState(Bool bflag)
{
    Bool res;

#if 1

    CbusReq_S xdata req;

    if(GET_MHL_HPD_SET_FLAG() == bflag)
    {
        return TRUE;
    }

    req.cmd = (bflag == TRUE ? MSC_SET_HPD : MSC_CLR_HPD);
    req.len = 0;
    req.reqStatus = CBUS_REQ_PENDING;
    
    //res = _mdrv_mhl_CbusSendMsg(&req);
    res = mdrv_mhl_CbusQueueMscTxRequest(gMHLInfo.MhlCbusInfo.curIdx, &req);
#else

    mhalCbusFifo_S xdata pCbusFifo;

    if(gMHLInfo.MhlCbusInfo.bMhlHpdState == bflag)
        return TRUE;

    gMHLInfo.MhlCbusInfo.MhlCbusState = CBUS_STATE_SENT;
    if(bflag) // SET_HPD
        pCbusFifo.databuf[0] = 0x8D64;
    else // CLR_HPD
        pCbusFifo.databuf[0] = 0x8565;		
    // message length
    pCbusFifo.lens = 1;
    res = mhal_mhl_CBusWrite(&pCbusFifo);

#endif

    if(res)
    {
        if(bflag)
        {
            SET_MHL_HPD_SET_FLAG();
            //MHL_DPUTSTR("SetHPD"); // 120717
        }
        else
        {
            CLR_MHL_HPD_SET_FLAG();
        }
    }

    return(res);
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_Cbus_SetPathEn()
//  [Description]:
//                  MHL Cbus set path enable
//  [Arguments]:
//                  TRUE: Enable
//                  FALSE: Disable
//  [Return]:
//   
//**************************************************************************
void mdrv_mhl_Cbus_SetPathEn(Bool bflag)
{
    Bool res = FALSE;

    res = mdrv_mhl_CbusWriteStatusOrSetInt(0x31, bflag ? BIT3 : 0);

    if(res || (!bflag))
    {
        if(bflag)
        {
            SET_MHL_PATH_EN_FLAG();
            //MHL_DPUTSTR("SetPathEn"); // 120717
        }
        else
        {
            CLR_MHL_PATH_EN_FLAG();
        }
        
        mhal_mhl_Cbus_SetPathEn(bflag);
    }
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_Cbus_SetDCapChg(Bool bflag)
//  [Description]:
//                  MHL Cbus set path enable
//  [Arguments]:
//                  TRUE: Enable
//                  FALSE: Disable
//  [Return]:
//   
//**************************************************************************
void mdrv_mhl_Cbus_SetDCapChg(Bool bflag)
{
    Bool res = FALSE;

    if(bflag)
    {
        res = mdrv_mhl_CbusWriteStatusOrSetInt(0x20, BIT0);
    }

    if(res || (!bflag))
    {
        if(bflag)
        {
            SET_MHL_DCAP_CHG_FLAG();
            //MHL_DPUTSTR("SetDCapChg"); // 120717
        }
        else
        {
            CLR_MHL_DCAP_CHG_FLAG();
        }
    }
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
void mdrv_mhl_Cbus_SetDCapRdy(Bool bflag)
{
    Bool res = FALSE;

    if(bflag)
    {
        //set DCAP_RDY bit
        res = mdrv_mhl_CbusWriteStatusOrSetInt(0x30, BIT0);
#if 0
        //set DCAP_CHG bit
        if (res)
        {
            res = mdrv_mhl_CbusWriteStatusOrSetInt(0x20, BIT0);
        }
#endif
    }

    if(res || (!bflag))
    {
        if(bflag)
        {
            SET_MHL_DCAP_RDY_FLAG();
            //MHL_DPUTSTR("SetDCapRdy"); // 120717
        }
        else
        {
            CLR_MHL_DCAP_RDY_FLAG();
        }
    }
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_CbusSetPacket(BYTE ucHeader, Bool bControl, BYTE ucContent)
//  [Description]
//                  MHL Cbus Packet setting
//  [Arguments]:
//                  ucHeader: Packet header type
//                  bControl: Packet control bit type
//                  ucContent: Packet payload
//  [Return]:
//                  Packet word for register write
//**************************************************************************
WORD mdrv_mhl_CbusSetPacket(BYTE ucHeader, Bool bControl, BYTE ucContent)
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
//                  mdrv_mhl_CbusReply (BYTE ctrlpky, Bool bflag, BYTE databuf)
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
Bool mdrv_mhl_CbusReply (BYTE ctrlpky, Bool bflag, BYTE databuf)
{
    Bool  res = TRUE;
    mhalCbusFifo_S pCbusFifo;

    switch (ctrlpky)
    {
        case MSC_ACK:
            pCbusFifo.lens = bflag ? 2 : 1;
            pCbusFifo.databuf[0] = mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_CONTROL, MSC_ACK);
            
            if(bflag)
            {
                pCbusFifo.databuf[1] = mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_DATA, databuf);
            }
            
            break;	
            
        case MSC_NACK:
            pCbusFifo.lens = 1;
            pCbusFifo.databuf[0] = mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_CONTROL, MSC_NACK);
            
            break;	

        case MSC_ABORT:
            pCbusFifo.lens = 1;
            pCbusFifo.databuf[0] = mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_CONTROL, MSC_ABORT);
            
            break;

        case MSC_GET_STATE:
        case MSC_GET_VENDOR_ID:
        case MSC_GET_DDC_ERRORCODE:
        case MSC_GET_MSC_ERRORCODE:
            pCbusFifo.lens = 1;
            pCbusFifo.databuf[0] = mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_DATA, databuf);
            
            break;

        default:
            res = FALSE;
            
            break;
    }

    if(!mhal_mhl_CBusWrite(&pCbusFifo))
    {
        //MHL_DPRINTF("@@Cbus Tx is busy = 0x%x", ctrlpky);
        return FALSE;
    }

    return(res);
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_CBusWriteCmd()
//  [Description]:
//                  MHL Cbus send command
//  [Arguments]:
//
//  [Return]:
//                  TRUE: Cbus command send successfully
//                  FALSE: Cbus command send fail       
//**************************************************************************
Bool mdrv_mhl_CBusWriteCmd(CbusReq_S *pReq)
{
    BYTE ucindex = 0;
    Bool  res = TRUE;
    mhalCbusFifo_S pCbusFifo;

    pCbusFifo.databuf[0] = mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_CONTROL, pReq->cmd);
        
    switch(pReq->cmd)
    {
        case MSC_ACK:
            pCbusFifo.lens = pReq->len;
            //pCbusFifo.databuf[0] = mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_CONTROL, MSC_ACK);
            
            if(pReq->len == 2)
            {
                // payload 2
                pCbusFifo.databuf[1] = mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_DATA, pReq->msgData[0]);
            }
            break;
			
        case MSC_NACK:
            pCbusFifo.lens = 1;
            //pCbusFifo.databuf[0] = mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_CONTROL, MSC_NACK);
            break;	

        case MSC_ABORT:
            pCbusFifo.lens = 1;
            //pCbusFifo.databuf[0] = mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_CONTROL, MSC_ABORT);
            break;

        case MSC_WRITE_STAT_OR_SET_INT:   // 0x60
            // payload 1
        #if 0    
            pCbusParity.control = 0x01;
            pCbusParity.databuf = pReq->cmd;
            pCbusFifo.databuf[0] = BIT15 | _mdrv_mhl_CbusParityCalculate(&pCbusParity);
        #else
            //pCbusFifo.databuf[0] = mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_CONTROL, MSC_WRITE_STAT_OR_SET_INT);
        #endif
            // payload 2			
            pCbusFifo.databuf[1] = mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_DATA, pReq->msgData[0]);
            // payload 3
            pCbusFifo.databuf[2] = mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_DATA, pReq->msgData[1]);
            // message length
            pCbusFifo.lens = 3;
            break;

        case MSC_READ_DEVCAP:   // 0x61
            // payload 1
        #if 0
            pCbusParity.control = 0x01;
            pCbusParity.databuf = pReq->cmd;
            pCbusFifo.databuf[0] = BIT15 | _mdrv_mhl_CbusParityCalculate(&pCbusParity);
        #else
            //pCbusFifo.databuf[0] = mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_CONTROL, MSC_READ_DEVCAP);
        #endif
            // payload 2			
            pCbusFifo.databuf[1] = mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_DATA, pReq->msgData[0]);
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
            // payload 1
            //pCbusFifo.databuf[0] = mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_CONTROL, pReq->cmd);
            // message length
            pCbusFifo.lens = 1;
            break;

        case MSC_MSC_MSG: // 0x68
            // payload 1
        #if 0
            pCbusParity.control = 0x01;
            pCbusParity.databuf = pReq->cmd;
            pCbusFifo.databuf[0] = BIT15 | _mdrv_mhl_CbusParityCalculate(&pCbusParity);
        #else
            //pCbusFifo.databuf[0] = mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_CONTROL, MSC_MSC_MSG);
        #endif
            // payload 2			
            pCbusFifo.databuf[1] = mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_DATA, pReq->msgData[0]);
            // payload 3
            pCbusFifo.databuf[2] = mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_DATA, pReq->msgData[1]);
            // message length
            pCbusFifo.lens = 3;
            break;

        case MSC_WRITE_BURST: // 0x6C
            // payload 1
        #if 0
            pCbusParity.control = 0x01;
            pCbusParity.databuf = pReq->cmd;
            pCbusFifo.databuf[0] = BIT15 | _mdrv_mhl_CbusParityCalculate(&pCbusParity);
        #else
            //pCbusFifo.databuf[0] = mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_CONTROL, MSC_WRITE_BURST);
        #endif
            // payload 2 ~ 17			
            for(ucindex =0; ucindex <pReq->len; ucindex++) // offset packet + the maxi 16 packets
            {
                pCbusFifo.databuf[ucindex +1] = mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_DATA, pReq->msgData[ucindex]);
            }
            // EOF
            pCbusFifo.databuf[pReq->len +1] = mdrv_mhl_CbusSetPacket(MSC_HEADER, CBUS_CONTROL, MSC_EOF); 
            // message length
            pCbusFifo.lens = pReq->len +2;
            break;

        default:
            res = FALSE;
            break;
    }

    if (res)
    {
        if(!mhal_mhl_CBusWrite(&pCbusFifo))
        {
            //MHL_DPRINTF("@@Cbus Tx is busy = 0x%x", pReq->cmd);
            return FALSE;
        }
    }

    return(res);
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
void mdrv_mhl_SetTMDSTxOnOff(Bool bflag)
{
    mhal_mhl_SetTMDSTxOnOff(bflag);
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_CBusProcessMSCSubCmd(BYTE cmd, BYTE databuf)
//  [Description]:
//                  MHL Cbus process MSC sub command
//  [Arguments]:
//                  cmd: command               
//                  databuf: data
//  [Return]:
//
//**************************************************************************
Bool mdrv_mhl_CBusProcessMSCSubCmd (BYTE cmd, BYTE databuf)
{
    Bool res = TRUE;
    BYTE tmp = databuf;

    //MHL_DPUTSTR("mdrv_mhl_CBusProcessMSCSubCmd !!\n");

    switch (cmd)
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

            break;          
    }

    return res;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_mhl_CbusGetLeastFlag(BYTE unindex)
//  [Description]:
//                  MHL Cbus get the least True flag in index
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mdrv_mhl_CbusGetLeastFlag(BYTE unindex)
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
//                  mdrv_mhl_CbusConnectionCheck()
//  [Description]:
//                  MHL Cbus connection check
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mdrv_mhl_CbusConnectionCheck(void)
{
    static Bool bCbusConnected = FALSE;
    Bool bindex = FALSE;
    BYTE ucindex = 0;

    if(bCbusConnected != GET_MHL_CBUS_CONNECT())
    {
        bCbusConnected = GET_MHL_CBUS_CONNECT();

        if(!bCbusConnected)
        {
            //clear HPD, PathEnable & DcapRdy if cbus disconncted
        #if 0
            mdrv_mhl_Cbus_SetHPDState(FALSE);
            mdrv_mhl_Cbus_SetPathEn(FALSE);
            mdrv_mhl_Cbus_SetDCapRdy(FALSE);
        #endif
            gMHLInfo.MhlCbusInfo.curIdx = 0;
            SET_MHL_CBUS_STATE(CBUS_STATE_IDLE);
            SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_NO_ERROR);
            CLR_CBUS_LINK_FLAG();
            CLR_CBUS_LINK_CHECK();

            mdrv_mhl_SetControlTimer(0);    // timeout reset.
			
            mhal_mhl_MHLForceToStandby();
            //SET_MHL_PS_CTRL_HI(); // set VBUS to low
#if DMHL_LG_PRADA_PATCH
            mhal_mhl_AdjustCommonModeResistor(TRUE);
#endif
            mhal_mhl_RxRtermControl(RX_RTERM_OFF);//mhal_mhl_RxRtermControl(RX_MHL_RTERM);//mhal_mhl_RxRtermControl(RX_RTERM_OFF);
            memset(&gMHLInfo.MhlCbusInfo.request, 0, sizeof(gMHLInfo.MhlCbusInfo.request)); 
            
            if( !mdrv_mhl_CableDetect() )
            {
                mhal_mhl_RxRtermControl(RX_HDMI_RTERM);
                mhal_mhl_LoadHdmiBypassTbl();
            }
            MHL_DPUTSTR("** Cbus connection failed");
        }
        else
        {   
            if((gMHLInfo.MhlPowerStatus != MHL_POWER_ON) && PowerOnFlag)
            {
                mdrv_mhl_PowerCtrl(MHL_POWER_ON);
            }
            
            SET_MHL_DEBOUNCE_TIMER(150); // reset 100msec
            
            //SET_MHL_PS_CTRL_LO(); // set VBUS to high
#if DMHL_LG_PRADA_PATCH
            mhal_mhl_AdjustCommonModeResistor(FALSE);
#endif
            mhal_mhl_RxRtermControl(RX_MHL_RTERM);
            //mhal_mhl_LoadMhl24bitsModeTbl();
            MHL_DPUTSTR("**Cbus connected");
            // Cbus connected then check Rx packet soon
            bindex = TRUE;
        }
    }
    
    if(bCbusConnected)
    {
        ucindex = GET_CBUS_LINK_FLAG() ^MHL_CBUS_LINK_MASK;

        if(ucindex > 0)
        {
            switch(mdrv_mhl_CbusGetLeastFlag(ucindex))
            {
                case CBUS_HPD_SET_FLAG:
                    mdrv_mhl_Cbus_SetHPDState(TRUE);
                    break;

                case CBUS_DCAP_RDY_FLAG:
                    mdrv_mhl_Cbus_SetDCapRdy(TRUE);
                    break;

                case CBUS_PATH_EN_FLAG:
                    mdrv_mhl_Cbus_SetPathEn(TRUE);
                    break;

                case CBUS_DCAP_CHG_FLAG:
                    mdrv_mhl_Cbus_SetDCapChg(TRUE);
                    break;

                default:
                    
                    break;
            };
        }
    }

    return bindex;
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
//                  mdrv_mhl_CbusLinkCheck()
//  [Description]
//                  MHL Cbus handle send MSC message
//  [Arguments]:
//                  
//  [Return]:
//
//**************************************************************************
void mdrv_mhl_CbusLinkCheck(void)
{
    BYTE ucindex = GET_CBUS_LINK_CHECK() ^MHL_CBUS_LINK_MASK;
    
    if(ucindex > 0)
    {
        SET_CBUS_LINK_CHECK(GET_CBUS_LINK_CHECK() |mdrv_mhl_CbusGetLeastFlag(ucindex));
    }
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
BYTE mdrv_mhl_CbusTxHandler(void)
{
    BYTE ucindex = 0;
    BYTE res = FALSE;

    
#if DMHL_CTS
    // CTS 6.3.6.4 verify DUT receives result timeout => couldn's send any MSC packet within 2sec  
    // CTS 6.3.6.5 verify no next command until hold-off after Abort seen => couldn's send any MSC packet within 2sec
    if(GET_MHL_TIME_OUT_FLAG() &&(!GET_MHL_TIMEOUT_TIMER()))
    {
        MHL_DPUTSTR("**MHL TOUT");
        SET_MHL_CBUS_STATE(CBUS_STATE_IDLE);
        SET_MHL_QUEUE_STATE(gMHLInfo.MhlCbusInfo.curIdx, CBUS_REQ_IDLE);

        mdrv_mhl_SetControlTimer(0);
    }
    
#endif

    switch(GET_MHL_CBUS_STATE())
    {                 
        case CBUS_STATE_IDLE:
            ucindex = mdrv_mhl_CBusGetQueueIndex(gMHLInfo.MhlCbusInfo.curIdx);

            if((ucindex < MHL_CBUS_TOTAL_QUEUE_NUM) && !GET_MHL_DEBOUNCE_TIMER())
            {                
                if(mdrv_mhl_CBusWriteCmd(&(GET_MHL_CBUS_QUEUE(ucindex)))) 
                {
                    // Retry send packet twice
                    if(GET_MHL_QUEUE_STATE(ucindex) == CBUS_REQ_SENT)
                    {
                        SET_MHL_QUEUE_STATE(ucindex, CBUS_REQ_IDLE);
                        MHL_DPUTSTR("MHL Tx retry");
                    }
                    else if(GET_MHL_QUEUE_STATE(ucindex) > CBUS_REQ_SENT)
                    {
                        SET_MHL_QUEUE_STATE(ucindex, CBUS_REQ_SENT);
                    }

                    // Update current index to search index, but not Rx queue index
                    if(ucindex < MHL_CBUS_RX_QUEUE_INDEX)
                    {
                        gMHLInfo.MhlCbusInfo.curIdx = ucindex;
                    }
                    
                    SET_MHL_CBUS_STATE(CBUS_STATE_SENT);
                    mdrv_mhl_SetControlTimer(CBUS_COMMAND_TIMEOUT_SECS);
	#if 0
                    if( (gMHLInfo.MhlCbusInfo.request[idx].cmd == MSC_WRITE_STAT_OR_SET_INT) && 
                        (gMHLInfo.MhlCbusInfo.request[idx].msgData[0] == 0x20) && (gMHLInfo.MhlCbusInfo.request[idx].msgData[1] == BIT2) )
                    {
                        MHL_DPUTSTR("**Waiting to respond to reqWrt!!\n");
                        gMHLInfo.MhlCbusInfo.reqWrt = TRUE;
                    }
	#endif

                    res = TRUE;
                }
            }
            
            break;

        case CBUS_STATE_SENT:
        case CBUS_STATE_WAITING_ACK:
        case CBUS_STATE_RECEIVED: 
            break;
            
        case CBUS_STATE_ACK:
//MHL_DPUTSTR("TX ACK"); // 120717
            ucindex = mdrv_mhl_CBusGetQueueIndex(gMHLInfo.MhlCbusInfo.curIdx);
            
            // Update current index to search index, but not Rx queue index
            if(ucindex < MHL_CBUS_RX_QUEUE_INDEX)
            {
                gMHLInfo.MhlCbusInfo.curIdx = ucindex;
            }

            mdrv_mhl_CbusLinkCheck();
            
            SET_MHL_CBUS_STATE(CBUS_STATE_IDLE);
            //res = TRUE;
            
            break;
    
        default:
            SET_MHL_CBUS_STATE(CBUS_STATE_IDLE);
            break;
    }

    return res;
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

    if(mhal_mhl_CbusIsMscMsgReceived()) // received valid message 
    {
        if(GET_MHL_TIME_OUT_FLAG())
        {
            mdrv_mhl_SetControlTimer(0);    // timeout reset.
        }

        // Read received Cbus packet from register
        mhal_mhl_CbusIntCB(&(preq.rcState), &(preq.cmd), &(preq.msgData), &(preq.len), &bflag);

#if 1

#if DMHL_CTS
        // CTS 6.3.10.5
    #if 0
        if((preq.cmd != MSC_GET_MSC_ERRORCODE) && (preq.rcState == 0))
            g_u8ErrorCode = MSC_ERRORCODE_NO_ERROR;
    #endif
        
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
                mdrv_mhl_CbusReply(MSC_ABORT, FALSE, 0);
            }
        }
        
#endif
    
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
#if DMHL_CTS
            // CTS 6.3.6.3
            if((GET_MHL_CBUS_STATE() == CBUS_STATE_SENT) && ((preq.cmd == MSC_ACK) && bflag))
            {
                mdrv_mhl_CbusReply(MSC_ABORT, FALSE, 0);
                SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_PROTOCOL_ERROR);
            }
            
            // CTS 6.3.6.5
            if((GET_MHL_CBUS_STATE() == CBUS_STATE_SENT) && (preq.cmd == MSC_ABORT))
            {
                mdrv_mhl_SetControlTimer(CBUS_ABORT_PADING_SECS);
            }
            
            // CTS 6.3.6.4 & 6.3.6.5
            if((preq.rcState != CBUS_RECEIVE_TIMEOUT) && (preq.cmd != MSC_ABORT))
#endif
            {
                if(GET_MHL_QUEUE_STATE(MHL_CBUS_RX_QUEUE_INDEX) != CBUS_REQ_IDLE)
                {
                    SET_MHL_QUEUE_STATE(MHL_CBUS_RX_QUEUE_INDEX, CBUS_REQ_IDLE);
                }
                else
                {
                    SET_MHL_QUEUE_STATE(gMHLInfo.MhlCbusInfo.curIdx, CBUS_REQ_IDLE);
                }

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
#else
        qidx = gMHLInfo.MhlCbusInfo.curIdx;

        if((preq.cmd != MSC_ACK) && 
            (preq.cmd != MSC_NACK) &&
            (preq.cmd != MSC_ABORT))
        {
            if( (preq.cmd != MSC_GET_STATE) && (preq.cmd != MSC_GET_VENDOR_ID) && (preq.cmd != MSC_READ_DEVCAP) && (preq.cmd != MSC_GET_MSC_ERRORCODE)
    #if DMHL_RCP_RAP_HWACK
                && (preq.cmd != MSC_MSC_MSG)
    #endif
                )
            {
                mdrv_mhl_CbusReply(MSC_ACK, FALSE, 0);
            }
            if (preq.cmd == MSC_READ_DEVCAP)
            {
        #if !DMHL_DEVCAP_HWACK
                mdrv_mhl_CbusReply(MSC_ACK, TRUE, MhlDevCap_tbl[preq.msgData[0]]);
        #endif // #if !DMHL_DEVCAP_HWACK
            }
            else
            {
                memcpy(&(gMHLInfo.MhlCbusInfo.request[qidx]), &preq, sizeof(CbusReq_S));
            }   
            switch (gMHLInfo.MhlCbusInfo.MhlCbusState)
            {
                case CBUS_STATE_IDLE:
                case CBUS_STATE_SENT:
                    gMHLInfo.MhlCbusInfo.request[qidx].reqStatus = CBUS_REQ_RECEIVED;
                    gMHLInfo.MhlCbusInfo.MhlCbusState = CBUS_STATE_RECEIVED;
                    break;
                default:
                    gMHLInfo.MhlCbusInfo.MhlCbusState = CBUS_STATE_IDLE;
                    break;
            }
            //MHL_DPRINTF("**Cbus Rev = 0x%x", preq.cmd);
        }
     else
     {
#if DMHL_CTS
            // CTS 6.3.6.5
            if(preq.cmd == MSC_ABORT)
                gMHLInfo.MhlCbusInfo.abortTimer = CBUS_FW_HOLD_OFF_SECS * 1000 / DEM_POLLING_DELAY;  
            // CTS 6.3.6.4 & 6.3.6.5
         if((preq.rcState != 1) && (preq.cmd != MSC_ABORT))
#endif
         {
                gMHLInfo.MhlCbusInfo.request[qidx].reqStatus = CBUS_REQ_IDLE;
                gMHLInfo.MhlCbusInfo.MhlCbusState = CBUS_STATE_IDLE;
         }
            //MHL_DPRINTF("**Cbus Rev ACK= 0x%x", preq.cmd);
     }
#endif
    }   
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
    BYTE ucCount = 0;
    Bool bretAck = TRUE;

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
                    mdrv_mhl_SetControlTimer(CBUS_ABORT_PADING_SECS);
                    break;  
        
                case MSC_WRITE_STAT_OR_SET_INT:
#if !DMHL_WRITE_STATE_INT_HWACK
#if 0
                    switch(gMHLInfo.MhlCbusInfo.request[gMHLInfo.MhlCbusInfo.curIdx].msgData[0])
                    {
                        case 0x20: // INT: device register change
                            if(gMHLInfo.MhlCbusInfo.request[gMHLInfo.MhlCbusInfo.curIdx].msgData[1] & BIT0) // DCAP_CHG
                            {
                                //MHL_DPUTSTR("**INT: DCAP_CHG \n");
                            }
                            if(gMHLInfo.MhlCbusInfo.request[gMHLInfo.MhlCbusInfo.curIdx].msgData[1]  & BIT1) // DSCR_CHG
                            {
                                //MHL_DPUTSTR("**INT: DSCR_CHG \n");
                            }
                            if(gMHLInfo.MhlCbusInfo.request[gMHLInfo.MhlCbusInfo.curIdx].msgData[1]]  & BIT2) // REQ_WRT
                            {
                                MHL_DPUTSTR("**INT: REQ_WRT \n");
                                req.cmd = MSC_WRITE_STAT_OR_SET_INT;
                                req.msgData[0] = 0x20;
                                req.msgData[1] = 0x08; // GRT_WRT   
                                _mdrv_mhl_CbusSendMsg(&req);
                            }
                            if(gMHLInfo.MhlCbusInfo.request[gMHLInfo.MhlCbusInfo.curIdx].msgData[1]  & BIT3) // GRT_WRT
                            {
                                BYTE buf[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}; 
                                gMHLInfo.MhlCbusInfo.reqWrt = FALSE;
                                gMHLInfo.MhlCbusInfo.reqWrtTimer = 0;
                                mdrv_mhl_CbusWriteBurst(0x40, 16, &buf[0]);
                                MHL_DPUTSTR("**INT: GRT_WRT \n");
                            }
                            break;
                            
                        case 0x21: // INT: device state change
                            if(gMHLInfo.MhlCbusInfo.request[gMHLInfo.MhlCbusInfo.curIdx].msgData[1]  & BIT1) // EDID_CHG
                            {
                                //MHL_DPUTSTR("**INT: EDID_CHG \n");
                            }
                            
                        case 0x30: // Status: device status register
                            MHL_DPRINTF("**Status: Connected_Rdy: 0x%x \n", gMHLInfo.MhlCbusInfo.request[gMHLInfo.MhlCbusInfo.curIdx].msgData[1] );
                            break;  
        
                        case 0x31: // Status: active link mode
                            MHL_DPRINTF("**Status: Link_mode: 0x%x \n", gMHLInfo.MhlCbusInfo.request[gMHLInfo.MhlCbusInfo.curIdx].msgData[1] );
                            break;
                            
                        case 0x22:
                        case 0x23:
                        case 0x32:
                        case 0x33:
                            break;
                            
                        default:
                            bretAck = FALSE;
                            break;
                    }
                    //mdrv_mhl_CbusReply(bretAck ? MSC_ACK : MSC_ABORT, FALSE, 0);
#else
    #if DMHL_CTS
                    // CTS 6.3.11.8 && 6.3.11.11
                    if(gMHLInfo.MhlCbusInfo.bIsCmdinDataField)
                    {
                        SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_PROTOCOL_ERROR);
                        bretAck = FALSE;
                    }
    #endif
                
                    mdrv_mhl_CbusReply(bretAck ? MSC_ACK : MSC_ABORT, FALSE, 0);              
#endif
#endif // #if !DMHL_WRITE_STATE_INT_HWACK
                    break;
        
                case MSC_READ_DEVCAP:
                    // HW auto reply, REG_MHL_CBUS_23[10] = 1
#if !DMHL_DEVCAP_HWACK

    #if DMHL_CTS
                    // CTS 6.3.11.8
                    if(gMHLInfo.MhlCbusInfo.bIsCmdinDataField)
                    {
                        mdrv_mhl_CbusReply(MSC_ABORT, FALSE, 0);
                        SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_PROTOCOL_ERROR);
                    }
                    else
                    {
                        // CTS 6.3.11.9
                        if(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[0] <= 0x0F)
                        {
                            mdrv_mhl_CbusReply(MSC_ACK, TRUE, MhlDevCap_tbl[GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[0]]);
                            SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_NO_ERROR);
                        }
                        else
                        {
                            mdrv_mhl_CbusReply(MSC_ABORT, FALSE, 0);
                            SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_BAD_OFFSET);
                        }
                    }
   #else
                    mdrv_mhl_CbusReply(MSC_ACK, TRUE, MhlDevCap_tbl[GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[0]]);          
   #endif // #if DMHL_CTS
       
#endif
                    break;

                case MSC_GET_STATE:
                    mdrv_mhl_CbusReply(MSC_GET_STATE, TRUE, MhlDevCap_tbl[0]); // DevCap register 0x00
                    break;
                    
                case MSC_GET_VENDOR_ID:
                    mdrv_mhl_CbusReply(MSC_GET_VENDOR_ID, TRUE, DMHL_VENDOR_ID);
                    break;
        
                case MSC_MSC_MSG:
#if !DMHL_RCP_RAP_HWACK

    #if DMHL_CTS
                    // CTS 6.3.11.22 & 6.3.11.24
                    if(gMHLInfo.MhlCbusInfo.bIsCmdinDataField)
                    {
                        SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_PROTOCOL_ERROR);
                        bretAck = FALSE;
                    }
                    else
                    {
                        bretAck = mdrv_mhl_CBusProcessMSCSubCmd(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[0], GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[1]);
                    }
    #endif // #if DMHL_CTS
                
                    mdrv_mhl_CbusReply(bretAck ? MSC_ACK : MSC_ABORT, FALSE, 0);
#endif
                    break;
        
                case MSC_WRITE_BURST:
    #if DMHL_CTS
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
                    
    #endif // #if DMHL_CTS
                    mdrv_mhl_CbusReply(bretAck ? MSC_ACK : MSC_ABORT, FALSE, 0);
                    break;

                case MSC_GET_MSC_ERRORCODE:
                    mdrv_mhl_CbusReply(MSC_GET_MSC_ERRORCODE, TRUE, GET_MHL_CBUS_ERROE_CODE());
                    break;

                case MSC_GET_DDC_ERRORCODE:
                    mdrv_mhl_CbusReply(MSC_GET_DDC_ERRORCODE, TRUE, mhal_mhl_GetDDCErrorCode());
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
                    mdrv_mhl_CbusReply(MSC_ABORT, FALSE, 0);
                    SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_INVALID_OPCODE);
                    break;
                    
                default: 
                    // CTS 6.3.6.2 & 6.3.11.2
                    mdrv_mhl_CbusReply(MSC_ABORT, FALSE, 0);    
                    SET_MHL_CBUS_ERROE_CODE(MSC_ERRORCODE_INVALID_OPCODE);
                    break;
            }
        }
        else // received message is not valid
        {
            MHL_DPRINTF("**Unknow: 0x%x \n", GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).cmd);
            
            while(GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[ucCount] &BIT15)
            {
                MHL_DPRINTF("**Undata: 0x%x \n", GET_MHL_CBUS_QUEUE(MHL_CBUS_RX_QUEUE_INDEX).msgData[ucCount]);
                ucCount++;
            }
        }        
    }
}
#if 0
// config HPD in combo(MHL/HDMI) port A
void mdrv_mhl_SetHPD(Bool bflag)
{
    if(!GET_MHL_CABLE_PLUGGED())
    {
        mhal_mhl_SetHPD(bflag);
    }
}
#endif

#if 1//MS_PM
Bool mdrv_mhl_WakeupDetect(void)
{
    if(mdrv_mhl_CableDetect()) // MHL
    {
        //return ( (mhal_mhl_CbusStatus() & 0x03 == 0x03) ? TRUE : FALSE );
        if(mdrv_mhl_CbusStatus())
        {
#if DMHL_LG_PRADA_PATCH
            mhal_mhl_AdjustCommonModeResistor(FALSE);
#endif
            mhal_mhl_RxRtermControl(RX_MHL_RTERM);
            CLR_MHL_PORT_ON_FLAG();
                
            return TRUE;
        }
        else
            return FALSE;
    }
    else // HDMI
    {
        return ( mhal_mhl_GetTMDSClk() >= 0x30 ? TRUE : FALSE );
    }
}

#endif

// Check DE stable
Bool mdrv_mhl_CheckDEStable(void)
{
    return (mhal_mhl_CheckDEStable());
}

#endif //#if ENABLE_MHL

#endif // _MDRV_MHL_C_

