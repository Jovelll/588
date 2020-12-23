#pragma LARGE
#include "board.h"
#if ENABLE_USB_INPUT


//Copyright (C) 2008 MStar  All Rights Reserved.

/*! \file  drvUsbHostLib.c
    \brief  Platform function for USB Host.


 \n\b History:
    \n\b Date: 2008/8/27
    \n\b Author: Colin Tsai
    \n\b Reason: Create
    \n
*/
//=============================================================================
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "datatype.h"
#include "UsbHostDataDef.h"
#include "debug.h"  // for PLATFORM define
//#include "sysinfo.h"

#include "drvUSB.h"
#include "drvUsbHostConfig.h"

//extern void MDrv_Timer_Delayms(U32);

#ifdef  Enable_PTP
extern U8 drvUSBHost_PTP_Initial(void);
extern U8 drvUSBHost_PTP_Initial_Port2(void);
#endif
#if defined(USB_HID_ENABLE)
extern U8 drvUSBHost_HID_Initial(U8 u8Interface);
#endif
#if  defined(USB2_HID_ENABLE)
extern U8 drvUSBHost_HID_Initial_Port2(U8 u8Interface);
#endif
/////////////////////////////////////////////////////////////////////////////
//  For USB Port 0
/////////////////////////////////////////////////////////////////////////////
extern void UsbPort0UTMIInitial(void);

    #ifdef UHC_BASE
    U16     gUHC_BASE = UHC_BASE;
    #else
    U16     gUHC_BASE = 0;
    #endif

    #ifdef UTMIBaseAddr
    U16     gUTMI_BASE = UTMIBaseAddr;
    #else
    U16     gUTMI_BASE = 0;
    #endif

    #ifdef USBCBase
    U16     gUSBC_BASE = USBCBase;
    #else
    U16     gUSBC_BASE = 0;
    #endif

void drvUSBHost_TurnOffPowerDownMode(void)
{
#if (ENABLE_USB) && defined(USB_POWER_SAVING_MODE)
    //Power On
    XBYTE[gUTMI_BASE+8]&=0x7F;    //PD_BG_CURRENT
    XBYTE[gUTMI_BASE+0x2c]|=0x04;    //Switch current from DVI

    //Port0
    XBYTE[gUTMI_BASE+1]=0x6f;
    XBYTE[gUTMI_BASE]=0xc4;
    XBYTE[gUTMI_BASE+0x11]=0x00;
    XBYTE[gUTMI_BASE+0x10]=0x00;

    //open port 0
    XBYTE[gUTMI_BASE+1]=0x00;
    XBYTE[gUTMI_BASE]=0x00;
    XBYTE[gUSBC_BASE]|=0x08; //disable init suspend state
    XBYTE[gUSBC_BASE]|=0x20; //Enable UHC XIU
    MDrv_Timer_Delayms(3);
    XBYTE[gUSBC_BASE+2]|=0x01;  //UHC select enable
    //end

    // Power On USB UHC0
    XBYTE[gUHC_BASE+0x34] |= 0x40;

    MDrv_Timer_Delayms(2);
    XBYTE[gUHC_BASE+0x40] &=~0x10;
#endif
}

void drvUSBHost_TurnOnPowerDownMode(void)
{
#if (ENABLE_USB) && defined(USB_POWER_SAVING_MODE)
    MDrv_Timer_Delayms(2);

   XBYTE[gUSBC_BASE]&=0xD0; //Disable Port0 UHC XIU
   XBYTE[gUTMI_BASE+8]|=0x80;    //PD_BG_CURRENT
   XBYTE[gUTMI_BASE+0x2c]&=0xFB;    //Switch current from REXT, not DVI

   //Port 0
   XBYTE[gUTMI_BASE+0x11]=0x88;     //bit<15>:HS_TX_OVERRIDE, bit<11>:FL_TX_OVERRIDE
   XBYTE[gUTMI_BASE+0x10]=0x00;
   XBYTE[gUTMI_BASE+3]=0x90;     //bit<15>:reg_hs_rterm_pdn, bit<7>:reg_bond_sel
   XBYTE[gUTMI_BASE+2]=0x80;
   XBYTE[gUTMI_BASE+1]=0xef;     //bit<15>:reg_pdn
   XBYTE[gUTMI_BASE]=0xc7;
#endif
}
#if 0
void drvUSBHost_UTMIInitial()
{
#if (ENABLE_USB)
    UsbPort0UTMIInitial();
#endif
}
#endif
/*
BOOLEAN drvUSBHost_isPort0Enable()
{
#if (ENABLE_USB)
    return TRUE;
#else
    return FALSE;
#endif
}
*/
/////////////////////////////////////////////////////////////////////////////
// For USB Port 2
/////////////////////////////////////////////////////////////////////////////
extern void UsbPort2UTMIInitial(void);

    #ifdef UHC2_BASE
    U16     gUHC2_BASE = UHC2_BASE;
    #else
    U16     gUHC2_BASE = 0;
    #endif

    #ifdef UTMIBaseAddr2
    U16     gUTMI2_BASE= UTMIBaseAddr2;
    #else
    U16     gUTMI2_BASE= 0;
    #endif

    #ifdef USBCBase2
    U16     gUSBC2_BASE = USBCBase2;
    #else
    U16     gUSBC2_BASE = 0;
    #endif

void drvUSBHost_TurnOffPowerDownMode_Port2(void)
{
#if (ENABLE_USB_2) && defined(USB_POWER_SAVING_MODE)
    //Port1
    XBYTE[gUTMI2_BASE+1]=0x6f;
    XBYTE[gUTMI2_BASE]=0xc4;
    XBYTE[gUTMI2_BASE+0x11]=0x00;
    XBYTE[gUTMI2_BASE+0x10]=0x00;
    //Power On End

    MDrv_Timer_Delayms(2);

    //open port 1
    XBYTE[gUTMI2_BASE]=0x00;   //disable power down over write
    XBYTE[gUTMI2_BASE+1]=0x00;   //disable power down over write high byte
    XBYTE[gUSBC2_BASE]|=0x08;  //disable init suspend state
    XBYTE[gUSBC2_BASE]|=0x20;  //Enable UHC XIU
    MDrv_Timer_Delayms(3);
    XBYTE[gUSBC2_BASE+2]|=0x01;  //UHC select enable
    MDrv_Timer_Delayms(3);
    //end

    // Power On USB UHC1
    XBYTE[gUHC2_BASE+0x34] |= 0x40;
    MDrv_Timer_Delayms(2);
    XBYTE[gUHC2_BASE+0x40] &=~0x10;
#endif
}

void drvUSBHost_TurnOnPowerDownMode_Port2(void)
{
#if (ENABLE_USB_2) && defined(USB_POWER_SAVING_MODE)

    MDrv_Timer_Delayms(2);

   XBYTE[gUSBC2_BASE]&=0xD0; //Disable Port1 UHC XIU

   //Port 1
   XBYTE[gUTMI2_BASE+0x11]=0x88;
   XBYTE[gUTMI2_BASE+0x10]=0x00;
   XBYTE[gUTMI2_BASE+3]=0x90;
   XBYTE[gUTMI2_BASE+2]=0x80;
   XBYTE[gUTMI2_BASE+1]=0xef;
   XBYTE[gUTMI2_BASE]=0xc7;
#endif
}

void drvUSBHost_UTMIInitial_Port2()
{
#if (ENABLE_USB_2)
    UsbPort2UTMIInitial();
#endif
}

BOOLEAN drvUSBHost_isPort2Enable()
{
#if (ENABLE_USB_2)
    return TRUE;
#else
    return FALSE;
#endif
}

/////////////////////////////////////////////////////////////////////////////
// For Both Port 0 and Port 2
/////////////////////////////////////////////////////////////////////////////
#ifdef USB_POWER_SAVING_MODE
U8 gUsbDeviceState=POWER_SAVING;
#else
U8 gUsbDeviceState=NO_DEVICE;
#endif

#ifdef USB_POWER_SAVING_MODE
U8 gUsbDeviceState_Port2 = POWER_SAVING;
#else
U8 gUsbDeviceState_Port2=NO_DEVICE;
#endif

void UsbTurnOffPowerDownMode(void)
{
    drvUSBHost_TurnOffPowerDownMode();
    drvUSBHost_TurnOffPowerDownMode_Port2();
}

void UsbTurnOnPowerDownMode(void)
{
    drvUSBHost_TurnOnPowerDownMode();
    drvUSBHost_TurnOnPowerDownMode_Port2();
}

BOOLEAN drvUSBHost_isPowerSaveModeEnable()
{
#ifdef USB_POWER_SAVING_MODE
    return TRUE;
#else
    return FALSE;
#endif
}

#ifdef  Enable_PTP
U8 drvUSBHost_PTP_Init(U8 u8UsbPort)
{
    if (u8UsbPort == USB_PORT_0)
    {
#if defined(ENABLE_PTP) && defined(USB_PTP_ENABLE)
        return drvUSBHost_PTP_Initial();
#endif
    }

    if (u8UsbPort == USB_PORT_2)
    {
#if defined(ENABLE_PTP) && defined(USB2_PTP_ENABLE)
        return drvUSBHost_PTP_Initial_Port2();
#endif
    }

    return 1;   //PTP_RESP_FAIL
}
#endif
extern U8 xdata PORT_DEVICE[HOST20_INTERFACE_NUM_MAX];
extern U8 xdata PORT2_DEVICE[HOST20_INTERFACE_NUM_MAX];
U8 drvUSBHost_HID_Init(U8 u8UsbPort, U8 u8Interface)
{
   if (u8UsbPort == USB_PORT_0)
    {
#if defined(USB_HID_ENABLE)
//if(PORT_DEVICE[u8Interface]==1 || PORT_DEVICE[u8Interface]==2)
        return drvUSBHost_HID_Initial(u8Interface);
#endif
    }

    if (u8UsbPort == USB_PORT_2)
    {
#if  defined(USB2_HID_ENABLE)
//if(PORT2_DEVICE[u8Interface]==1)
        return drvUSBHost_HID_Initial_Port2(u8Interface);
#endif
    }

    return 1;   //

}
#endif
