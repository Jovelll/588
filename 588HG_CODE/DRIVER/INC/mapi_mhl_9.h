///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   Mapi_mhl.h
/// @author MStar Semiconductor Inc.
/// @brief  MHL driver Function
///////////////////////////////////////////////////////////////////////////////////////////////////

/*
    1. Check whether MHL cable is plugged or not in Rx side
    2. Disable R-term and output Vbus
    3. Check whether Cbus is connected(discovery pulse completed)
    4. Sink -> Source: "SET_HPD"
    5. Sink -> Source: Write_Stat - "DCAP_RDY" and Write_INT - "DCAP_CHG"
    6. Sink -> Source: Write_Stat - "PATH_EN"
*/


#ifndef _MAPI_MHL_H_
#define _MAPI_MHL_H_

//-------------------------------------------------------------------------------------------------
//  Function Prototype
//-------------------------------------------------------------------------------------------------
void mapi_mhl_init(void);
void mapi_mhl_CbusTimerInterrupt(void);
void mapi_mhl_PollingTimerhandler(void);
void mapi_mhl_IsrHandler(void);
void mapi_mhl_handler(void);
void mapi_mhl_SourceChange(void);
void mapi_mhl_PowerCtrl(BYTE ucState);
#if(MHL_CTS_SEND_MSC)
void mapi_mhl_CbusSendCommand(BYTE ucSelect);
#endif

#if MS_PM
Bool mapi_mhl_WakeupDetect(Bool bAutoSearch, BYTE *ucPort);
void mapi_mhl_ChargePortDetect(void);
#endif

#if(MHL_HW_RTERM_SUPPORT)
void mapi_mhl_RtermControlHWMode(Bool bFlag);
#endif
#if(MHL_RCP_FUNCTION_SUPPORT)
Bool mapi_mhl_SendRCPCommand(BYTE ucKeyCode);
#endif
#if(MHL_RAP_FUNCTION_SUPPORT)
Bool mapi_mhl_SendRAPCommand(BYTE ucKeyCode);
#endif
#if(MHL_UCP_FUNCTION_SUPPORT)
Bool mapi_mhl_SendUCPCommand(BYTE ucKeyCode);
#endif
#if(MHL_WRITE_BURST_SUPPORT)
Bool mapi_mhl_SendWriteBurst(BYTE ucLength, BYTE *ucData);
#endif
#if(MHL_HID_FUNCTION_SUPPORT)
Bool mapi_mhl_HIDSentWriteBurst(Bool bReset, Bool bReply, BYTE *ucLength, BYTE *ucData);
#endif

#endif //_MAPI_MHL_H_


