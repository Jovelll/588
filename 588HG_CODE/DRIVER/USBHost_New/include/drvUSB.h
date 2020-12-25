//#define     USB_POWER_SAVING_MODE
#ifdef ENABLE_DMP
#define     USBHOST2PORT                                //for 2 usb ports board, if you use only one port , please undefine it.
#endif
#include "drvhostlib.h"


extern BOOLEAN MDrv_UsbHost_Init(void);
extern BOOLEAN MDrv_UsbHost_Init_Enum(void);

#define     MDrv_UsbDiskRead_Port2    MDrv_UsbBlockReadToMIU_Port2
#define     MDrv_UsbDiskWrite_Port2   MDrv_UsbBlockWriteFromMIU_Port2
extern BOOLEAN MDrv_UsbBlockReadToMIU_Port2(U8 lun,U32 u32BlockAddr, U32 u32BlockNum,U32 u32Buffer);
extern BOOLEAN MDrv_UsbBlockWriteFromMIU_Port2(U8 lun,U32 u32BlockAddr, U32 u32BlockNum,U32 u32Buffer);
extern BOOLEAN MDrv_UsbDeviceConnect_Port2(void);
U8 MDrv_GET_MASS_MAX_LUN_PORT2(void);
U8 MDrv_GET_MASS_VALID_LUN_PORT2(void);
extern BOOLEAN  MDrv_Usb_Device_Enum_Port2(void);
extern void MDrv_UsbClose_Port2(void);
extern void MDrv_USB_Init_Port2(U32);
extern U8 MDrv_UsbGetMaxLUNCount_Port2(void);
extern void MDrv_EjectUsbDevice_Port2(void);
U32 MDrv_GetUsbBlockSize_Port2(U8 lun);
U32 MDrv_GetUsbBlockNum_Port2(U8 lun);
extern BOOLEAN MDrv_UsbHost_Init_Enum_Port2(void);
U8  MDrv_GetUsbDeviceStatusPort2(void);
void MDrv_SetUsbDeviceStatus_Port2(U8 status);
U8 MDrv_GetUsbDeviceType_Port2(void);
void MDrv_GetUsbString_Port2(U8 u8StrID, S8 *pStrBuf, U8 u8BufLen);
void MDrv_UsbSendTestPacket_Port2(void);
void MDrv_UsbSendSE0_Port2(void);
void MDrv_UsbSendTestJ_Port2(void);
void MDrv_UsbSendTestK_Port2(void);
void MDrv_GetUsbStorString_Port2(U8 uLun, U8 u8StrID, S8 *pStrBuf, U8 u8BufLen);
//U8*  MDrv_Get_HID_Report_Raw_Data(void);


#define     MDrv_UsbDiskRead    MDrv_UsbBlockReadToMIU
#define     MDrv_UsbDiskWrite   MDrv_UsbBlockWriteFromMIU
extern BOOLEAN MDrv_UsbBlockReadToMIU(U8 lun,U32 u32BlockAddr, U32 u32BlockNum,U32 u32Buffer);
extern BOOLEAN MDrv_UsbBlockWriteFromMIU(U8 lun,U32 u32BlockAddr, U32 u32BlockNum,U32 u32Buffer);
extern BOOLEAN MDrv_UsbDeviceConnect(void);
extern U8 MDrv_UsbGetInterfaceClass(U8 i,U8 j);
extern void MDrv_UsbClose(void);
extern void MDrv_USB_Init(DWORD USBAdr);
extern BOOLEAN  MDrv_Usb_Device_Enum(void);
extern U8 flib_Host20_Enumerate (U8,U8 );
extern void MDrv_EjectUsbDevice(void);
U32 MDrv_GetUsbBlockSize(U8 lun);
U32 MDrv_GetUsbBlockNum(U8 lun);

extern U8 MDrv_UsbGetMaxLUNCount(void);
U8 MDrv_GET_MASS_MAX_LUN(void);
U8 MDrv_GET_MASS_VALID_LUN(void);

U8  MDrv_GetUsbDeviceStatus(void);
void MDrv_SetUsbDeviceStatus(U8 status);
void  MDrv_ClearUsbDeviceStatus(void);
U8 MDrv_GetUsbDeviceType(void);
void MDrv_GetUsbString(U8 u8StrID, S8 *pStrBuf, U8 u8BufLen);
void MDrv_UsbSendTestPacket(void);
void MDrv_UsbSendSE0(void);
void MDrv_UsbSendTestJ(void);
void MDrv_UsbSendTestK(void);
//U16  MDrv_GET_JOYSTICK_STATUS(void);
void MDrv_GetUsbStorString(U8 uLun, U8 u8StrID, S8 *pStrBuf, U8 u8BufLen);

#define USB_NOT_RESPONSE		    0xff
#define USB_INTERFACE_CLASS_NONE 0
#define USB_INTERFACE_CLASS_MSD  0x08
#define USB_INTERFACE_CLASS_HUB  0x09
#define USB_INTERFACE_CLASS_IMAGE  0x06
#define USB_INTERFACE_CLASS_HID  0x03

// project code definition
#define     JUPITER                 0
#define     NEPTUNE                 1
#define     RAPHAEL                 2
#define     ERIS                    3

#define 	NO_DEVICE               0
#define     USB11_DEVICE            1
#define 	USB20_DEVICE            2
#define	    BAD_DEVICE              3
#define     CONNECT_DEVICE          4
#define     POWER_SAVING            5
#define     WAIT_INIT               6

#define USB_PORT_0          0
#define USB_PORT_2          1

#define USB_STR_VENDOR              1
#define USB_STR_PRODUCT            2

