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
#define _BIT(a)         (BIT0 << a)
#define MASKBIT(a)      (_BIT((1?a) +1) -_BIT((0?a)))

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

typedef struct  
{
    BYTE lens;
    WORD databuf[DMHL_CBUS_TX_FIFO];
} mhalCbusFifo_S;

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
Bool mhal_mhl_init(BYTE *edid, BYTE *devcap);
Bool mhal_mhl_CableDetect(void);
void mhal_mhl_Cbus_SetPathEn(Bool bflag);
void mhal_mhl_SetTMDSTxOnOff(Bool bflag);
Bool mhal_mhl_CbusIsMscMsgReceived(void);
void mhal_mhl_CbusIntCB(BYTE *rcstate, BYTE *rccmd, BYTE *rcdata, BYTE *rclen, Bool *bIsCmdInData);
WORD mhal_mhl_CbusStatus(void);
void mhal_mhl_CDRModeMonitor(Bool bPathEnable);
Bool mhal_mhl_IsCbusBusy(void);
void mhal_mhl_MHLForceToStandby(void);
//void mhal_mhl_MHLForceToFloating(Bool flag);
void mhal_mhl_RxRtermControl(RXRtermControl_T rctrl);
#if DMHL_LG_PRADA_PATCH
void mhal_mhl_AdjustCommonModeResistor(Bool bflag);
#endif
void mhal_mhl_LoadHdmiBypassTbl(void);
void mhal_mhl_LoadMhl24bitsModeTbl(void);
//void mhal_mhl_LoadMhlPackedPixelModeTbl(void);
void mhal_mhl_PowerCtrl(BYTE pctrl);
void mhal_mhl_SetHPD(Bool bflag);
BYTE mhal_mhl_GetDDCErrorCode(void);
WORD mhal_mhl_GetTMDSClk(void);
Bool mhal_mhl_CheckDEStable(void);
#if ENABLE_DEBUG
Bool ElandWriteHalfWord (WORD ucaddr, BYTE ucdata);
BYTE ElandReadHalfWord(WORD ucaddr);
void MhlTestfunction(BYTE tb1, BYTE tb2, BYTE tb3, BYTE tb4);
void MhlTestfunction1(BYTE tb1, BYTE tb2, BYTE tb3, BYTE tb4);
#endif

#endif //_MHAL_MHL_H_
