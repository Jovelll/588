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
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define DMHL_ADOPTER_ID             0x02CB // Mstar: 715
#define DMHL_DEVICE_ID              0x0000 // Chip ID?
#define DMHL_VENDOR_ID              0x00 // ?

#define DMHL_CTS_SEND_MSC           0

//-------------------------------------------------------------------------------------------------
//  Interface Link
//-------------------------------------------------------------------------------------------------
#define MHL_SELECT_PORT0            Input_HDMI
#define MHL_SELECT_PORT1            Input_HDMI2
#define MHL_SELECT_PORT2            Input_HDMI3

#ifdef ENABLE_MHL_C1
#define MHL_SUPPORT_PORT0           ENABLE_MHL_C1
#else
#define MHL_SUPPORT_PORT0           0
#endif

#ifdef ENABLE_MHL_C2
#define MHL_SUPPORT_PORT1           ENABLE_MHL_C2
#else
#define MHL_SUPPORT_PORT1           0
#endif

#ifdef ENABLE_MHL_C3
#define MHL_SUPPORT_PORT2           ENABLE_MHL_C3
#else
#define MHL_SUPPORT_PORT2           0
#endif

#ifdef XDATA_MHL_CBUS_START
#define MHL_CBUS_SRAM_ADDRESS       XDATA_MHL_CBUS_START
#else
#define MHL_CBUS_SRAM_ADDRESS       0
#endif

#if(MHL_SUPPORT_PORT0)
#define MHL_CABLE_DETECT_PORT0      hwDVI0Cable_Pin
#define SET_VBUS_CHARGE_PORT0()     SET_MHL_PS_CTRL_CHARGE0()
#define CLR_VBUS_CHARGE_PORT0()     CLR_MHL_PS_CTRL_CHARGE0()
#endif

#if(MHL_SUPPORT_PORT1)
#define MHL_CABLE_DETECT_PORT1      hwDVI1Cable_Pin
#define SET_VBUS_CHARGE_PORT1()     SET_MHL_PS_CTRL_CHARGE1()
#define CLR_VBUS_CHARGE_PORT1()     CLR_MHL_PS_CTRL_CHARGE1()
#endif

#if(MHL_SUPPORT_PORT2)
#define MHL_CABLE_DETECT_PORT2      hwDVI2Cable_Pin
#define SET_VBUS_CHARGE_PORT2()     SET_MHL_PS_CTRL_CHARGE2()
#define CLR_VBUS_CHARGE_PORT2()     CLR_MHL_PS_CTRL_CHARGE2()
#endif

//-------------------------------------------------------------------------------------------------
//  Function Prototype
//-------------------------------------------------------------------------------------------------
void mapi_mhl_Initial(void);
void mapi_mhl_CbusTimerInterrupt(void);
void mapi_mhl_Handler(BYTE ucPort);
Bool mapi_mhl_CbusLinkCheck(void);
void mapi_mhl_PowerControl(BYTE ucState);
#if(DMHL_CTS_SEND_MSC)
void mapi_mhl_CbusSendCommand(BYTE ucSelect);
#endif

#ifdef _MS_PM_
Bool mapi_mhl_WakeupDetect(void);
#endif

#endif //_MAPI_MHL_H_


