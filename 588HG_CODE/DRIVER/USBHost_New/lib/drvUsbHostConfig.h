
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

#define USBCBase2        0x3600
#define UHC2_BASE        0x3700
#define UTMIBaseAddr2    0x3B00
#endif
#define ENABLE_USB    1
#define ENABLE_USB_2  0
//#define USB_PTP_ENABLE            //if you want to support PTP device
//#define USB2_PTP_ENABLE
#define USB_HID_ENABLE            //if you want to support USB joystick
#define USB2_HID_ENABLE