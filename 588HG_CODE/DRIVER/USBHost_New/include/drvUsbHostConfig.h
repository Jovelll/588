#define ATV_SERISE_USE
//#define NEPTUNE_SERIAL_USE
//#define CERAMAL_SERISE_USE
//#define PLUTO_SERIAL_USE
//#define TITANIA2_SERIAL_USE
#define JUNIOR_SERIAL_USE

#ifdef JUNIOR_SERIAL_USE
#define USBCBase         0x0D00
#define UHC_BASE         0x0E00
#define UTMIBaseAddr     0x0F00

//#define USBCBase         0x3600
//#define UHC_BASE         0x3700
//#define UTMIBaseAddr     0x3B00

#endif

#ifdef PLUTO_SERIAL_USE
#define UHC_BASE    0x2400
#define UHC2_BASE   0x0b00
#define UTMIBaseAddr     0x3a80
#define UTMIBaseAddr2   0x3ac0
#define USBCBase            0x700
#define USBCBase2           0xc00
#endif

#ifdef TITANIA2_SERIAL_USE
#define UHC_BASE    0x2400
#define UHC2_BASE   0x0d00
#define UTMIBaseAddr     0x3a80
#define UTMIBaseAddr2   0x3a00
#define USBCBase            0x700
#define USBCBase2           0x780
#endif

#ifdef TRITON_SERIAL_USE
#define UHC_BASE    0x2e00
#define UTMIBaseAddr     0x3100
#endif

#ifdef NEPTUNE_SERIAL_USE
#define UHC_BASE    0x2400
#define UTMIBaseAddr     0x3a80
#define USBCBase            0x700
#endif

#if 0
#ifdef  CERAMAL_SERISE_USE
#define UHC_BASE    0x2400
#define UTMIBaseAddr     0x1f00
#define OnePort_OTG_EHCI
#define Process_018_USE
#endif
#endif

#ifdef  ATV_SERISE_USE
//#define UHC_BASE    0x2400
//#define UTMIBaseAddr     0x3a80
//#define MDrv_Timer_Delayms(x) Delay1ms(x);
#endif

#define ENABLE_USB    1
#define ENABLE_USB_2  0
//#define USB_PTP_ENABLE            //if you want to support PTP device
//#define USB2_PTP_ENABLE

#define USB_HID_ENABLE            //if you want to support USB joystick


