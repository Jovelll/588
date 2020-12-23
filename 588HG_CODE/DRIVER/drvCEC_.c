
#include "board.h"

#if ENABLE_CEC

#define _DRV_CEC_

#include "types.h"
//#include "msreg.h"
#include "Ms_rwreg.h"
#include "debug.h"
#include "Misc.h"

#include "Global.h"
#include "drvCEC_.h"


#define ENABLE_DRV_CEC_DEBUG 0

#if (ENABLE_DRV_CEC_DEBUG == 1)
#define DRVCEC_PRINT_MSG(str)          printMsg(str)//putstr(str)
#define DRVCEC_PRINT_DATA(str, x)      printData(str, x)//printf(str, x)
#else
#define DRVCEC_PRINT_MSG(str)
#define DRVCEC_PRINT_DATA(str, x)
#endif

//**************************************************************************
//  [Function Name]:
//                  _drv_CEC_InitVariable()
//  [Description]:
//                  initialize CEC relative structure and variables
//  [Arguments]:
//                  none
//  [Return]:
//                  none
//
//**************************************************************************
void _drv_CEC_InitVariable(void)
{
    BYTE i, j;

    DRVCEC_PRINT_MSG("[CEC]func: _drv_CEC_InitVariable()");
    
    gStCECInfo.bInitDone = FALSE;

    for ( i=0; i < 15; i++ )
        gStCECInfo.bIsDevExisted[i] = FALSE;

    gStCECInfo.bMSGQStartIdx = 0;
    gStCECInfo.bMSGQEndIdx = 0;
    gStCECInfo.bMSGCnt = 0;

    for ( i=0; i < CEC_MSGQ_DEPTH; i++ )
    {
        gStCECInfo.stRxBufList[i].bLength = 0;
        for ( j=0; j < CEC_FIFO_DEPTH; j++ )
            gStCECInfo.stRxBufList[i].bData[j] = 0;
    }

    for ( i=0; i < CEC_FIFO_DEPTH; i++ )
        gStCECInfo.bTxBuf[i] = 0;

    gStCECInfo.enMyDevType = CEC_DEV_TV;
    gStCECInfo.enMyLogicAddr = CEC_LA_TV;
    gStCECInfo.bMyPhysicalAddr[0] = 0x10;
    gStCECInfo.bMyPhysicalAddr[1] = 0x00;
    gStCECInfo.enMyPwrStatus = CEC_PWRSTS_ON;

    gStCECInfo.bActDevCECVersion = CEC_VER_11;
    gStCECInfo.enActDevType = CEC_DEV_RESERVED;
    gStCECInfo.enActLogicAddr = CEC_LA_TV;
    gStCECInfo.bActPhysicalAddr[0] = 0x00;
    gStCECInfo.bActPhysicalAddr[1] = 0x00;
    gStCECInfo.enActPwrStatus = CEC_PWRSTS_STANDBY;

}

//**************************************************************************
//  [Function Name]:
//                  _drv_CEC_SendFrame()
//  [Description]:
//                  initialize CEC relative structure and variables
//  [Arguments]:
//                  [BYTE] header: header byte of frame to be sent;
//                  [enMsCEC_OPCODE_LIST] opCode: opcode of frame to be sent;
//                  [BYTE*] operand: operand of frame to be sent;
//                  [BYTE] length: total lenght of frame to be sent;
//  [Return]:
//                  [_enMsCEC_INT_EVENT_]
//
//**************************************************************************
enMsCEC_INT_EVENT _drv_CEC_SendFrame( BYTE header, enMsCEC_OPCODE_LIST opCode, BYTE* operand, BYTE length )
{
    BYTE waitCnt = 0;
    BYTE cnt = 0;
    enMsCEC_INT_EVENT res;

    DRVCEC_PRINT_MSG("[CEC]func: _drv_CEC_SendFrame()");
    DRVCEC_PRINT_DATA("header %x\r\n", header);
    DRVCEC_PRINT_DATA("opCode %d\r\n", opCode);
    DRVCEC_PRINT_DATA("length %d\r\n", length);

    //Clear TX interrupt status;
    msWriteByteMask(REG_0425, 0x0E, 0x0E);
    msWriteByteMask(REG_0425, 0x00, 0x0E);

    //Fill TX data
    msWriteByte(REG_0430, header);
    msWriteByte(REG_0431, opCode);

    if ( length > 0 )
    {
        BYTE i;

        for ( i=0; i < length; i++ )
            msWriteByte(REG_0432 + i, *(operand + i));
    }

    if ((opCode = CEC_OPCODE_POLLING_MESSAGE) && (length == 0))
    {
        waitCnt = 5;
        msWriteByteMask(REG_0400, length, 0x0F); //set TX length
    }
    else
    {
        waitCnt = 30;
        msWriteByteMask(REG_0400, (length+1), 0x0F);
    }

    //The total time,
    //(1). successful, 4.5 ms + 10 * 2.4 ms * N = 4.5 ms + 24 * N
    //              = 28.5 ms (1), or 52.5 ms (2), ....
    //(2). NAK,        (4.5 ms + 10 * 2.4 ms) * 1 + (4.5 ms + 10 * 2.4 ms +7.2 ms(3 bit time)) * retry (3)
    //              = 28.5 + 35.2 * 3 = 133.6 ms
    
    ForceDelay1ms(20);

    do
    {
        ForceDelay1ms(10);
        if (cnt++ >= waitCnt)
            break;
    } while ((msReadByte(REG_0423) & 0x0E) == 0);

    res = (enMsCEC_INT_EVENT)msReadByte(REG_0423);

    if (cnt >= waitCnt)
        res |= CEC_EVENT_SYSTEM_BUSY;

    //Clear TX interrupt status;
    msWriteByteMask(REG_0425, 0x0E, 0x0E);
    msWriteByteMask(REG_0425, 0x00, 0x0E);

    return res;
    
}

//**************************************************************************
//  [Function Name]:
//                  _drv_CEC_PingDevice()
//  [Description]:
//                  Get device logical address
//  [Arguments]:
//                  [enMsCEC_DEV_LOGIC_ADDR] addr: logical address of device
//  [Return]:
//                  [enMsCEC_INT_EVENT]: ping result of the logical address
//
//**************************************************************************
enMsCEC_INT_EVENT _drv_CEC_PingDevice(enMsCEC_DEV_LOGIC_ADDR addr)
{
    enMsCEC_INT_EVENT error_code = CEC_EVENT_FEATURE_ABORT;

    DRVCEC_PRINT_MSG("[CEC]func: _drv_CEC_PingDevice()");

    if( addr >= CEC_LA_MAX )
    {
        error_code = CEC_EVENT_FEATURE_ABORT;
        return error_code;
    }

    error_code = _drv_CEC_SendFrame((( addr << 4 ) & 0xF0 ) | ( addr & 0x0F ), CEC_OPCODE_POLLING_MESSAGE, &addr, 0 );

    if (error_code & CEC_EVENT_TX_SUCCESS)
        error_code = CEC_EVENT_TX_SUCCESS;
    
    else if (error_code & CEC_EVENT_RETRY_FAIL)
        error_code = CEC_EVENT_RETRY_FAIL;
    
    else if (error_code & CEC_EVENT_LOST_ARBITRATION)
        error_code = CEC_EVENT_LOST_ARBITRATION;
    
    else if (error_code & CEC_EVENT_SYSTEM_BUSY)
        error_code = CEC_EVENT_SYSTEM_BUSY;

    return error_code;
}


//**************************************************************************
//  [Function Name]:
//                  _drv_CEC_GetSelfAddress()
//  [Description]:
//                  Get device logical address
//  [Arguments]:
//                  none
//  [Return]:
//                  none
//
//**************************************************************************
void _drv_CEC_GetSelfAddress(void)
{
    DRVCEC_PRINT_MSG("[CEC]func: _drv_CEC_GetSelfAddress()");

    if ( _drv_CEC_PingDevice(CEC_LA_TV) == CEC_EVENT_RETRY_FAIL )
        gStCECInfo.enMyLogicAddr = CEC_LA_TV;
    
    else if ( _drv_CEC_PingDevice(CEC_LA_FREE_USE) == CEC_EVENT_RETRY_FAIL )
        gStCECInfo.enMyLogicAddr = CEC_LA_FREE_USE;
    
    else 
        gStCECInfo.enMyLogicAddr = CEC_LA_UNREGISTERED;

    msWriteByteMask(REG_0404, gStCECInfo.enMyLogicAddr << 4, 0xF0); //[7:4]: logical address

}

//**************************************************************************
//  [Function Name]:
//                  drv_CEC_Init()
//  [Description]:
//                  initialize CEC variables, register settings
//  [Arguments]:
//                  none
//  [Return]:
//                  none
//
//**************************************************************************
void drv_CEC_Init(void)
{
    WORD clkCnt = 0;

    DRVCEC_PRINT_MSG("[CEC]func: drv_CEC_Init\r");
    
    _drv_CEC_InitVariable();

    msWriteByteMask(REG_0401, (0x10 | CEC_RETRY_CNT), 0x1F); //[4:3]: 1.5x low bit period [2:0]=CEC_RETRY_CNT
    msWriteByte(REG_0403, ((CEC_BUS_FREE_TIME << 4) | CEC_RESEND_INTERVAL)); //[3:0]: cnt1, [7:4]: cnt2
    msWriteByte(REG_0404,  ((CEC_LA_TV << 4) | CEC_FRAME_INTERVAL)); //[3:0]: cnt3, [7:4]: logical address: TV

    //Number of counts to achieve 10us
    clkCnt = (CRYSTAL_CLOCK % 100000L)*0.00016 + 0.5;
    msWriteByte(REG_0405, CRYSTAL_CLOCK/100000L); // integer part
    msWriteByteMask(REG_0406, clkCnt, 0x0F); // CEC time unit by Xtal(fractional)

    //select sw CEC ontroller
    msWriteByteMask(REG_0428, 0x00, _BIT1); //[1]: 0: clock source from xtal
    
    //[1:0]: tx rising shift 0us, [3:2]: tx falling shift 0us, [4]: cancel current tx = false, [5]: CEC main clock no gating, 
    //[6]: rx test mode:normal, [7]: enable PCM CEC controller
    msWriteByte(REG_0402, 0x80);

    msWriteByteMask(REG_0407, 0x00, _BIT4); //[4]: select CEC controller, 0:software / 1: PM HW 

    //clear CEC interrupt status
    msWriteByteMask(REG_0425, 0x1F, 0x1F);
    msWriteByteMask(REG_0425, 0x00, 0x1F);

    //undercheck
    //enable CEC function
    msWriteByteMask(REG_0206, _BIT4, _BIT4); //[4]: 1: Enable GPIO26 as CEC function

    #if (ENABLE_CEC_INT == 1)
    //TBD
    #endif

    _drv_CEC_GetSelfAddress();

    gStCECInfo.bInitDone = TRUE;

    DRVCEC_PRINT_MSG("[CEC]func: drv_CEC_Init done\r");

}

//**************************************************************************
//  [Function Name]:
//                  drv_CEC_CheckExistedDevice()
//  [Description]:
//                  check current existed device
//  [Arguments]:
//                  none
//  [Return]:
//                  none
//
//**************************************************************************
void drv_CEC_CheckExistedDevice(void)
{

    BYTE i;
    enMsCEC_INT_EVENT ret;

    DRVCEC_PRINT_MSG("[CEC]func: drv_CEC_CheckExistedDevice()");

    for ( i = CEC_LA_TV; i < CEC_LA_MAX; i++ )
    {
        ret = _drv_CEC_SendFrame(((gStCECInfo.enMyLogicAddr << 4) & 0xF0)|( i & 0x0F), CEC_OPCODE_POLLING_MESSAGE, &i, 0);

        if ( ret & CEC_EVENT_TX_SUCCESS )
        {
            gStCECInfo.bIsDevExisted[i] = TRUE;
            gStCECInfo.enActLogicAddr = (enMsCEC_DEV_LOGIC_ADDR)i;
        }
        else
            gStCECInfo.bIsDevExisted[i] = FALSE;
    }

}

//**************************************************************************
//  [Function Name]:
//                  drv_CEC_CheckRxBuffer()
//  [Description]:
//                  check RX buffer
//  [Arguments]:
//                  none
//  [Return]:
//                  none
//
//**************************************************************************
void drv_CEC_CheckRxBuffer(void) //don't place print message in this function
{
    BYTE i;

    //DRVCEC_PRINT_MSG("[CEC]func: drv_CEC_CheckRxBuffer()");
    
    if ( msReadByte( REG_0423 ) & _BIT0 ) //[0]: receive a new message successfully
    {
        gStCECInfo.stRxBufList[gStCECInfo.bMSGQEndIdx].bLength = (msReadByte(REG_0408) & 0x1F) + 1; //Get received message length

        if (gStCECInfo.stRxBufList[gStCECInfo.bMSGQEndIdx].bLength > 1)
        {
            for ( i = 0; i < gStCECInfo.stRxBufList[gStCECInfo.bMSGQEndIdx].bLength; i++ )
                gStCECInfo.stRxBufList[gStCECInfo.bMSGQEndIdx].bData[i] = msReadByte(REG_0440 + i); //extract message data

            gStCECInfo.bMSGCnt++;
            gStCECInfo.bMSGQEndIdx = ((++gStCECInfo.bMSGQEndIdx) >= CEC_MSGQ_DEPTH ? 0 : gStCECInfo.bMSGQEndIdx);
        }

        //clear RX interrupt status
        msWriteByteMask(REG_0425, _BIT0, _BIT0);
        msWriteByteMask(REG_0425, 0, _BIT0);
    }
}

//**************************************************************************
//  [Function Name]:
//                  drv_CEC_ProcessTX()
//  [Description]:
//                  check RX buffer
//  [Arguments]:
//                  none
//  [Return]:
//                  none
//
//**************************************************************************
enMsCEC_INT_EVENT drv_CEC_ProcessTX(enMsCEC_DEV_LOGIC_ADDR dst_addr, enMsCEC_OPCODE_LIST msg, BYTE* operand_ptr, BYTE len)
{
    enMsCEC_INT_EVENT retVal = CEC_EVENT_FEATURE_ABORT;
    BYTE header;

    DRVCEC_PRINT_MSG("[CEC]func: drv_CEC_ProcessTX()");
    DRVCEC_PRINT_DATA("dst_addr = %d\r", dst_addr);
    DRVCEC_PRINT_DATA("opcode = %d\r", msg);    
    
    header = (( gStCECInfo.enMyLogicAddr << 4 ) & 0xF0) | (dst_addr & 0x0F);

    retVal = _drv_CEC_SendFrame(header, msg, operand_ptr, len);

    if (retVal & CEC_EVENT_TX_SUCCESS)
        retVal = CEC_EVENT_TX_SUCCESS;
    else if (retVal & CEC_EVENT_RETRY_FAIL)
        retVal = CEC_EVENT_RETRY_FAIL;
    else if (retVal & CEC_EVENT_LOST_ARBITRATION)
        retVal = CEC_EVENT_LOST_ARBITRATION;
    else if (retVal & CEC_EVENT_SYSTEM_BUSY)
        retVal = CEC_EVENT_SYSTEM_BUSY;

    if ((msg == CEC_OPCODE_UI_PRESS) && (retVal == CEC_EVENT_TX_SUCCESS))
    {
        retVal = _drv_CEC_SendFrame(header, CEC_OPCODE_UI_RELEASE, operand_ptr, 0);

        if (retVal & CEC_EVENT_TX_SUCCESS)
            retVal = CEC_EVENT_TX_SUCCESS;
        else if (retVal & CEC_EVENT_RETRY_FAIL)
            retVal = CEC_EVENT_RETRY_FAIL;
        else if (retVal & CEC_EVENT_LOST_ARBITRATION)
            retVal = CEC_EVENT_LOST_ARBITRATION;
        else if (retVal & CEC_EVENT_SYSTEM_BUSY)
            retVal = CEC_EVENT_SYSTEM_BUSY;
    }

    return retVal;

}

//**************************************************************************
//  [Function Name]:
//                  drv_CEC_CheckFrame()
//  [Description]:
//                  check if frame is valid or not
//  [Arguments]:
//                  [enMsCEC_MSGTRANS_TYPE] MsgType: CEC message types;
//                  [BYTE] length: CEC command length, including header and opcode;
//  [Return]:
//                  [BOOL]
//
//**************************************************************************
BOOL drv_CEC_CheckFrame(enMsCEC_MSGTRANS_TYPE MsgType, BYTE length)
{
    BOOL isMsgBoardcast, isLenLegal, retVal;

    DRVCEC_PRINT_MSG("[CEC]func: drv_CEC_CheckFrame()");

    isMsgBoardcast = ((gStCECInfo.stRxBufList[gStCECInfo.bMSGQStartIdx].bData[0] & 0x0F) == 0x0F);
    isLenLegal = ((gStCECInfo.stRxBufList[gStCECInfo.bMSGQStartIdx].bLength) >= length);

    if ((gStCECInfo.stRxBufList[gStCECInfo.bMSGQStartIdx].bData[0] & 0xF0) == 0xF0)
    {
        switch (gStCECInfo.stRxBufList[gStCECInfo.bMSGQStartIdx].bData[1]) //check opcode
        {
            case CEC_OPCODE_INACTIVE_SOURCE:
            case CEC_OPCODE_REQ_ACTIVE_SOURCE:
            case CEC_OPCODE_ROUTING_CHANGE:
            case CEC_OPCODE_ROUTING_INFO:
            case CEC_OPCODE_SET_STREAM_PATH:

            case CEC_OPCODE_STANDBY:

            //following command invoke broadcast response
            case CEC_OPCODE_GIVE_PHY_ADDR:
            case CEC_OPCODE_GET_MENU_LANGUAGE:
            case CEC_OPCODE_GIVE_VENDOR_ID:
            break;
            
            default:
                return (FALSE);
            break;
         }
    }

    switch (MsgType)
    {
        case CEC_MSGTRANS_BROADCAST:
            retVal = isMsgBoardcast & isLenLegal;
        break;
        case CEC_MSGTRANS_DIRECT:
            retVal = (! isMsgBoardcast) & isLenLegal;
        break;
        case CEC_MSGTRANS_BOTH:
            retVal = isLenLegal;
        break;
    }

    return retVal;

}

//**************************************************************************
//  [Function Name]:
//                  drv_CEC_ConfigWakeUp()
//  [Description]:
//                  check RX buffer
//  [Arguments]:
//                  none
//  [Return]:
//                  none
//
//**************************************************************************
void drv_CEC_ConfigWakeUp(void)
{
    WORD clkCnt;
    DRVCEC_PRINT_MSG("\r\n Here do the PM config cec wakeup \r\n");

    //XBYTE[0x250C] &= ~BIT0;
    //msWriteByte( REG_290C, msReadByte(REG_290C) &(~ BIT0));

    //(1) enable chiptop clk_mcu & clk_pram
    //XBYTE[0x0E00] = 0x03;
    //enable PM_Sleep's clk_mcu and _pram
    //??    MDrv_WriteByte(L_BK_PMSLP(0x00), 0x03);
    //XBYTE[0x0E01] = 0x0F;
    //0x0C; Ken 20080916 for calibration to 1Mz
    //??    MDrv_WriteByte(H_BK_PMSLP(0x00), 0x0F);

    //(2) HDMI CEC settings
    msWriteByteMask(REG_0401, (0x10 | CEC_RETRY_CNT), 0x1F); //[4:3]: 1.5x low bit period [2:0]=CEC_RETRY_CNT
    //[1:0]: tx rising shift 0us, [3:2]: tx falling shift 0us, [4]: cancel current tx = false, [5]: CEC main clock no gating, 
    //[6]: rx test mode:normal, [7]: enable PCM CEC controller
    msWriteByte(REG_0402, 0x80);
    msWriteByte(REG_0403, ((CEC_BUS_FREE_TIME << 4) | CEC_RESEND_INTERVAL)); //[3:0]: cnt1, [7:4]: cnt2
    msWriteByte(REG_0404,  ((CEC_LA_TV << 4) | CEC_FRAME_INTERVAL)); //[3:0]: cnt3, [7:4]: logical address: TV
    
    //Number of counts to achieve 10us
    clkCnt = (CRYSTAL_CLOCK % 100000l)*0.00016 + 0.5;
    msWriteByte(REG_0405, CRYSTAL_CLOCK/1000001); // integer part
    msWriteByteMask(REG_0406, clkCnt, 0x0F); // CEC time unit by Xtal(fractional)

    //(3) PM Sleep: wakeup enable sources
    //??    PM_REG_WRITE(L_BK_PMMCU(0x00),0x01); // reg_cec_enw

    //(4) PM CEC power down controller settings
    msWriteByteMask(REG_0426, 0x1F, 0x1F); // Mask CEC interrupt in standby mode

    msWriteByteMask(REG_0402, 0x00, _BIT7); // [7]: Disable CEC controller
    msWriteByteMask(REG_0402, _BIT7, _BIT7); // [7]: Enable CEC controller

    msWriteByteMask(REG_0407, 0x50, 0x70); //[6:5]: Action for non-supported command, ,[4]: 0: software CEC controller; 1: power down hardware CEC controller;

    //(5) PM CEC wakeup opcode settings
    // OPCODE0: 0x04(Image view on)
    // OPCODE1: 0x0D(Text view on)
    // OPCODE2: 0x44 0x40(Power)
    //          0x44 0x6D(Power ON Function)
    // OPCODE3: N/A
    // OPCODE4: 0x82(Active source) length = 2
    msWriteByte(REG_040E, 0x37); // Enable OP0~2 and OP4
    msWriteByte(REG_040F, 0x24); // Eanble OPCODE2's operand
    msWriteByte(REG_0410, 0x04); // OPCODE0: Image View On
    msWriteByte(REG_0411, 0x0D); // OPCODE1: Text View ON
    msWriteByte(REG_0412, 0x44); // OPCODE2: User control
    msWriteByte(REG_0414, 0x82); // OPCODE4: Active source
    msWriteByte(REG_0417, 0x40); // User control - Power
    msWriteByte(REG_0418, 0x6D); // User control - Power ON Function
    msWriteByte(REG_041B, 0x84); // [2:0]: CEC version 1.3a; [7:3]: OP4 is broadcast message

    //(6) Device(TV) Vendor ID for customer (Big Endian)
    // It depends end-customer's vendor ID
    //MS_DEBUG_MSG(printf("!!!!!!!!!!!!!!!!!!!Change this Vendor ID according to customer!!!!!!!!!!!!!!!!\n"));
    msWriteByte(REG_041E,0x00); // Device Vendor ID
    msWriteByte(REG_041F,0x00); // Device Vendor ID
    msWriteByte(REG_0420,0x00); // Device Vendor ID
    msWriteByte(REG_0421,0x01); // [2:0]: Feature abort reason - "Not in correct mode to respond"

    //(7) Device Physical address: default is 0x00 0x00 0x00
    msWriteByte(REG_041C, 0x00); // Physical address 0.0
    msWriteByte(REG_041D, 0x00); // Physical address 0.0
    msWriteByte(REG_0429, 0x00); // Device type: TV

    //(8) Clear CEC status
    msWriteByte(REG_0422, 0x7F); // Clear CEC wakeup status
    msWriteByte(REG_0425, 0x1F); // Clear RX/TX/RF/LA/NACK status status
    msWriteByte(REG_0425, 0x00);

}


#endif //#if ENABLE_CEC
