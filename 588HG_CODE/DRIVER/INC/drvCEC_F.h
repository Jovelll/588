
//***********************************************************************************
// @file: drvCEC.h
// @author: MStar Semiconductor Inc.
// @brief: CEC driver layer interface
//***********************************************************************************

#ifndef _DRV_CEC_9_H_
#define _DRV_CEC_9_H_

#if ENABLE_CEC
#include "apiCEC.h"

#ifdef _DRV_CEC_9_
    #define _DRV_CEC_9_DEF_
#else
    #define _DRV_CEC_9_DEF_ extern
#endif


//***********************************************************************************
//  Macro and Define
//***********************************************************************************

#define CEC_MSGQ_DEPTH          4
#define CEC_FIFO_DEPTH          16

#define CEC_RETRY_CNT           1
#define CEC_FRAME_INTERVAL      7
#define CEC_BUS_FREE_TIME       5
#define CEC_RESEND_INTERVAL     3

//***********************************************************************************
//  Type and Structure
//***********************************************************************************

typedef struct _stDRV_CEC_RX_FIFO_INFO_
{
    BYTE bLength;
    BYTE bData[CEC_FIFO_DEPTH];
} stDRV_CEC_RX_FIFO_INFO;

typedef struct _stDRV_CEC_INFO_
{
    BOOL bInitDone;                                     //CEC init process finish flag
    BOOL bIsDevExisted[15];                             //CEC device existed flag
    
    BYTE bMSGQStartIdx;                                 //CEC message queue start index
    BYTE bMSGQEndIdx;                                   //CEC message queue end index
    BYTE bMSGCnt;                                       //CEC message count
    stDRV_CEC_RX_FIFO_INFO stRxBufList[CEC_MSGQ_DEPTH]; //CEC RX buffer
    BYTE bTxBuf[CEC_FIFO_DEPTH];                        //CEC TX buffer

    enMsCEC_DEVICE_TYPE enMyDevType;                    //CEC device type
    enMsCEC_DEV_LOGIC_ADDR enMyLogicAddr;               //CEC logic address
    BYTE bMyPhysicalAddr[2];                            //CEC physcial address
    enMsCEC_POWER_STATUS enMyPwrStatus;                 //CEC power status
    
    BYTE bActDevCECVersion;                             //CEC device version
    enMsCEC_DEVICE_TYPE enActDevType;                   //CEC device type
    enMsCEC_DEV_LOGIC_ADDR enActLogicAddr;              //CEC logic address
    BYTE bActPhysicalAddr[2];                           //CEC physcial address
    enMsCEC_POWER_STATUS enActPwrStatus;                //CEC power status
    
} stDRV_CEC_INFO;

//***********************************************************************************
//  Global Variables
//***********************************************************************************

_DRV_CEC_9_DEF_ stDRV_CEC_INFO xdata gStCECInfo;

//***********************************************************************************
//  Function Prototype
//***********************************************************************************
_DRV_CEC_9_DEF_ void drv_CEC_Init(void);
_DRV_CEC_9_DEF_ void drv_CEC_CheckExistedDevice(void);
_DRV_CEC_9_DEF_ void drv_CEC_CheckRxBuffer(void);
_DRV_CEC_9_DEF_ void drv_CEC_CheckRxBuffer(void);
_DRV_CEC_9_DEF_ enMsCEC_INT_EVENT drv_CEC_ProcessTX(enMsCEC_DEV_LOGIC_ADDR dst_addr, enMsCEC_OPCODE_LIST msg, BYTE* operand_ptr, BYTE len);
_DRV_CEC_9_DEF_ BOOL drv_CEC_CheckFrame(enMsCEC_MSGTRANS_TYPE MsgType, BYTE length );



#endif //#if ENABLE_CEC

#endif //#ifndef _DRV_CEC_H_


