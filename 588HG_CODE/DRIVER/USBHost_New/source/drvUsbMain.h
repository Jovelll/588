///////////////////////////////////////////////////////////////////////////////
//
//	File name: drvUsbMain.H
//	Version: 1.0
//	Date: 2004/12/08
//
//	Company: Faraday Tech. Corp.
//
//	Description:
///////////////////////////////////////////////////////////////////////////////
#ifndef Main_Host_Only__H
#define Main_Host_Only__H

//=================== 1.Condition Definition  ============================================================
//========================================================================================================

	#define mbFUSBH200_HALFSPEEDEnable_Set()		  (mwHost20Bit_Set(0x40,BIT2))
	#define mbFUSBH200_Interrupt_OutPut_High_Set()	  (mwHost20Bit_Set(0x40,BIT3))
       #define mbFUSBH200_Interrupt_OutPut_Low_Set()     (mwHost20Bit_Clr(0x40,BIT3))
	#define mbFUSBH200_VBUS_OFF_Set()		          (mwHost20Bit_Set(0x40,BIT4))
	#define mbFUSBH200_VBUS_ON_Set()   		          (mwHost20Bit_Clr(0x40,BIT4))
	#define mbFUSBH200_VBUS_VLD_Rd()   		          (mwHost20Port(0x40)&BIT8)


extern void FUSBH200_Driver_VBUS(void);
//extern void FUSBH200_Drop_VBUS(void);
extern U8 MDrv_UsbGetMaxLUNCount(void);
//#define Enable_Issue_TestPacket
//#define Issue_TestPacket
//#define Enable_Burning_Test
//#define Enable_SOF_Only
//#define Enable_Mass_Storage
//#define Enable_PTP
#define Enable_FS_Only
//#define Enable_Preamble

extern U16  gUHC_BASE;
extern U16  gUTMI_BASE;
extern U16  gUSBC_BASE;

#define  UHC_XBYTE(x)       (XBYTE[gUHC_BASE+x])
#define  UTMI_XBYTE(x)       (XBYTE[gUTMI_BASE+x])
#define  USBC_XBYTE(x)       (XBYTE[gUSBC_BASE+x])

#endif
