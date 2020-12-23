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
#define _BIT(a)                         (BIT0 << a)
#define MASKBIT(a)                      (_BIT((1?a) +1) -_BIT((0?a)))
#define MASKSET(a, b, c)                ((a &~c) | (b &c))
#define ABS_MINUS(a, b)                 ((a > b)? (a -b): (b -a))

#define MHL_PLL_DIV_OVERWRITE           1
#define MHL_FORCE_VCO_LOOP_DEV2         0

#define MHL_CDR_STABLE_THRESHOLD        100
#define MHL_CDR_STABLE_OFFSET           10

#define MHL_AUTO_EQ_TRIGGER_INTERVAL    1
#define MHL_AUTO_EQ_RESULT_INTERVAL     50

#define CBUS_SAR_VALTAGE_TARGET         20
#define CBUS_CABLE_DETECT_LEVEL         (CBUS_SAR_VALTAGE_TARGET *255 /33)

#define CBUS_CDR_CODE_CALCULATE(a)      (a *64 /14.318)

#define CBUS_CDR_CLOCK_24_TARGET        20
#define CBUS_CLOCK_24_DETECT_LEVEL      (CBUS_CDR_CODE_CALCULATE(CBUS_CDR_CLOCK_24_TARGET))

#define CBUS_CDR_CLOCK_PP_TARGET        10
#define CBUS_CLOCK_PP_DETECT_LEVEL      (CBUS_CDR_CODE_CALCULATE(CBUS_CDR_CLOCK_PP_TARGET))

#define CBUS_CDR_OFFSET_TARGET          5
#define CBUS_CLOCK_DETECT_OFFSET        (CBUS_CDR_CODE_CALCULATE(CBUS_CDR_OFFSET_TARGET) /10)

#define CBUS_CDR_CLOCK_CALCULATE(a)     (a *14.318 /64)

#define MHL_24BIT_VCO_LOOP_DIV2         48
#define MHL_24BIT_VCO_LOOP_DIV4         0

#define MHL_PPMODE_VCO_LOOP_DIV2        36

#define MHL_VCO_CODE_CALCULATE(a)       (a *64 /14.318)

#define MHL_CDR_PHASE_CODE              0x00F

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
enum MHL_CBUS_SELECT_TYPE
{
    MHL_CBUS_SELECT_PORT0 = 0,
    MHL_CBUS_SELECT_PORT1,
    MHL_CBUS_SELECT_PORT2,
    MHL_CBUS_SELECT_MASK,
    MHL_CBUS_SELECT_NONE = MHL_CBUS_SELECT_MASK,
    MHL_CBUS_SELECT_PSEUDO,
};

enum MHL_SYSTEM_CONTROL_TYPE
{
    MHL_CABLE_PLUGGED = 0,
    MHL_CABLE_UNPLUGGED,
    MHL_CBUS_UNCONNECTION,
    MHL_CBUS_CONNECTION,
    MHL_CBUS_NORMAL_24BIT_MODE,
    MHL_CBUS_PACKET_PIXEL_MODE,
    MHL_CBUS_WAKE_UP_DETECT,
    MHL_CBUS_PM_MODE,
    MHL_CBUS_SOURCE_CHANGE,
    MHL_CBUS_TEST_RASING,
    MHL_CBUS_TEST_FALLING,
};

typedef enum
{
    RX_HDMI_RTERM = 0,    // HDMI mode
    RX_MHL_RTERM,    // MHL mode
    RX_RTERM_OFF,    // R-term off
} RXRtermControl_T;

enum MHL_CABLE_DETECT_FLAG_TYPE
{
    MHL_CABLE_DETECT_ENABLE0_FLAG = BIT0,
    MHL_CABLE_DETECT_ENABLE1_FLAG = BIT1,
    MHL_CABLE_DETECT_ENABLE2_FLAG = BIT2,
};

#define GET_MHL_CABLE_DETECT_ENABLE0()  (Bool)((ucCableDetectEnable & MHL_CABLE_DETECT_ENABLE0_FLAG) ?TRUE :FALSE)
#define SET_MHL_CABLE_DETECT_ENABLE0()  (ucCableDetectEnable |= MHL_CABLE_DETECT_ENABLE0_FLAG)
#define CLR_MHL_CABLE_DETECT_ENABLE0()  (ucCableDetectEnable &= ~MHL_CABLE_DETECT_ENABLE0_FLAG)

#define GET_MHL_CABLE_DETECT_ENABLE1()  (Bool)((ucCableDetectEnable & MHL_CABLE_DETECT_ENABLE1_FLAG) ?TRUE :FALSE)
#define SET_MHL_CABLE_DETECT_ENABLE1()  (ucCableDetectEnable |= MHL_CABLE_DETECT_ENABLE1_FLAG)
#define CLR_MHL_CABLE_DETECT_ENABLE1()  (ucCableDetectEnable &= ~MHL_CABLE_DETECT_ENABLE1_FLAG)

#define GET_MHL_CABLE_DETECT_ENABLE2()  (Bool)((ucCableDetectEnable & MHL_CABLE_DETECT_ENABLE2_FLAG) ?TRUE :FALSE)
#define SET_MHL_CABLE_DETECT_ENABLE2()  (ucCableDetectEnable |= MHL_CABLE_DETECT_ENABLE2_FLAG)
#define CLR_MHL_CABLE_DETECT_ENABLE2()  (ucCableDetectEnable &= ~MHL_CABLE_DETECT_ENABLE2_FLAG)

//-------------------------------------------------------------------------------------------------
//  Function Prototype
//-------------------------------------------------------------------------------------------------
#if(MHL_CBUS_USE_INTERRUPT)
void mhal_mhl_IsrMaskControl(Bool bFlag);
void mhal_mhl_IsrRtermOff(void);
Bool mhal_mhl_IsrMscReceiveFlag(void);
Bool mhal_mhl_IsrStucktoLowFlag(void);
#endif
BYTE mhal_mhl_PortToCbusMapping(BYTE ucPort);
BYTE mhal_mhl_CbusToPortMapping(BYTE ucCbusSelect);
Bool mhal_mhl_CheckInputPort(BYTE ucPort);
void mhal_mhl_CbusFloating(Bool bFlag);
void mhal_mhl_Initial(BYTE *pEdid, BYTE *pDevcap, BYTE ucVenderID);
void mhal_mhl_CbusSwitchMux(BYTE ucPort);
Bool mhal_mhl_CableDetect(void);
Bool mhal_mhl_IsCbusBusy(void);
void mhal_mhl_RxRtermControl(RXRtermControl_T rctrl);
void mhal_mhl_RtermControlHWMode(Bool bFlag);
BYTE mhal_mhl_CbusStatus(void);
Bool mhal_mhl_CbusIsMscMsgReceived(void);
Bool mhal_mhl_CbusWakeupIntFlag(void);
Bool mhal_mhl_CBusWrite(mhalCbusFifo_S *pdatabuf);
void mhal_mhl_CbusForceToStandby(void);
void mhal_mhl_Cbus_SetPathEn(Bool bFlag);
Bool mhal_mhl_CbusIntCB(BYTE *rcstate, BYTE *rccmd, BYTE *rcdata, BYTE *rclen, Bool *bIsCmdInData);
void mhal_mhl_RxSRAMSwitch(void);
void mhal_mhl_DriverControl(BYTE ucState);
Bool mhal_mhl_CDRModeMonitor(Bool bPathEnable);
void mhal_mhl_PowerControl(BYTE ucState);
void mhal_mhl_SetHPD(Bool bFlag);
BYTE mhal_mhl_GetDDCErrorCode(void);

void mhal_mhl_CbusPathLinkProc(BYTE ucPort);
void mhal_mhl_CbusPathRemoveProc(BYTE ucPort);
void mhal_mhl_CbusConnectProc(BYTE ucPort);
void mhal_mhl_CbusStucktoLowProc(BYTE ucPort);
void mhal_mhl_ClockModeSwitchProc(BYTE ucPort, Bool bPPmode);
void mhal_mhl_PortSwitchProc(BYTE ucPort);
void mhal_mhl_CableDetectControl(Bool bEnable, BYTE ucPort);

#if MS_PM
Bool mhal_mhl_CableDetectPM(void);
#endif

#endif //_MHAL_MHL_H_

