///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   Mdrv_mhl.h
/// @author MStar Semiconductor Inc.
/// @brief  MHL driver Function
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MMHL_MHL_H_
#define _MMHL_MHL_H_

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define _BIT(a)                 (BIT0 << a)
#define MASKBIT(a)              (_BIT((1?a) +1) -_BIT((0?a)))

#define MHL_ADDERSS_8BIT        0
#define MHL_ADDERSS_16BIT       1

#define MHL_ADDRESS_TYPE        MHL_ADDERSS_8BIT
//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
enum MHL_SYSTEM_CONTROL_TYPE
{
    MHL_CABLE_PLUGGED = 0,
    MHL_CABLE_UNPLUGGED,
    MHL_CBUS_UNCONNECTION,
    MHL_CBUS_CONNECTION,
    MHL_CBUS_NORMAL_24BIT_MODE,
    MHL_CBUS_PACKET_PIXEL_MODE,
    MHL_CBUS_AUTO_EQ_SETTING,
    MHL_CBUS_WAKE_UP_DETECT,
    MHL_CBUS_PM_MODE,
    MHL_CBUS_TEST_RASING,
    MHL_CBUS_TEST_FALLING,
};

typedef enum
{
    RX_HDMI_RTERM = 0,    // HDMI mode
    RX_MHL_RTERM,    // MHL mode
    RX_RTERM_OFF,    // R-term off
} RXRtermControl_T;

//-------------------------------------------------------------------------------------------------
//  Function Prototype
//-------------------------------------------------------------------------------------------------
Bool mhal_mhl_CBusWrite(mhalCbusFifo_S *pdatabuf);
void mhal_mhl_CbusIsolate(Bool bFlag);
void mhal_mhl_Initial(BYTE *pEdid, BYTE *pDevcap);
void mhal_mhl_PortSwitchMux(BYTE ucPort);
Bool mhal_mhl_CableDetect(void);
void mhal_mhl_Cbus_SetPathEn(Bool bFlag);
void mhal_mhl_SetTMDSTxOnOff(Bool bFlag);
Bool mhal_mhl_CbusIsMscMsgReceived(void);
void mhal_mhl_CbusIntCB(BYTE *rcstate, BYTE *rccmd, BYTE *rcdata, BYTE *rclen, Bool *bIsCmdInData);
WORD mhal_mhl_CbusStatus(void);
void mhal_mhl_CDRModeMonitor(Bool bPathEnable);
void mhal_mhl_MHLForceToStandby(void);
void mhal_mhl_RxRtermControl(RXRtermControl_T rctrl);
void mhal_mhl_PowerControl(BYTE ucState);
void mhal_mhl_SetHPD(Bool bFlag);
BYTE mhal_mhl_GetDDCErrorCode(void);
Bool mhal_mhl_CheckDEStable(void);
void mhal_mhl_RxSRAMSwitch(void);
void mhal_mhl_DriverControl(BYTE ucState);

#endif //_MHAL_MHL_H_
