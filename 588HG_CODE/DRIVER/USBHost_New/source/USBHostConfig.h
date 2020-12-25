#define ATV_SERISE_USE

#if 0
//#define ATV_SERISE_USE
//#define NEPTUNE_SERIAL_USE
//#define CERAMAL_SERISE_USE
//#define PLUTO_SERIAL_USE
#define TITANIA2_SERIAL_USE

#ifdef PLUTO_SERIAL_USE
#define USBHOST2PORT            //for PLUTO 2 port solution
#define USB_POWER_SAVING_MODE
#define UHC_BASE    0x2400
#define UHC2_BASE   0x0b00
#define UTMIBaseAddr     0x3a80
#define UTMIBaseAddr2   0x3ac0
#define USBCBase            0x700
#define USBCBase2           0xc00
#endif

#ifdef TITANIA2_SERIAL_USE
#define USBHOST2PORT            //for PLUTO 2 port solution
#define USB_POWER_SAVING_MODE
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
#endif

#ifdef  CERAMAL_SERISE_USE
#define UHC_BASE    0x2400
#define UTMIBaseAddr     0x1f00
#define OnePort_OTG_EHCI
#define Process_018_USE
#endif

#ifdef  ATV_SERISE_USE
#define UHC_BASE    0x2400
#define UTMIBaseAddr     0x3a80
#define MDrv_Timer_Delayms(x) Delay1ms(x);
#endif

#endif

//#define ENABLE_FORCE_FS
#define USE_XDATA_ADDRESS_0XF000
//#define DTV_STANDARD_LIB

#define Enable_Low_Temperature_Patch

