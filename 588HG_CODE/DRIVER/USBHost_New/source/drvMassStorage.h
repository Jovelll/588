///////////////////////////////////////////////////////////////////////////////
//
//	File name: drvMassStorage.H
//	Version: 1.0
//	Date: 2003/7/13
//
//	Author:
//	Email:
//	Phone: (03) 578-7888
//	Company: Faraday Tech. Corp.
///////////////////////////////////////////////////////////////////////////////

#ifndef MassStorage_H

#define MassStorage_H



//=================== 2.Define Data Type  ================================================================
//========================================================================================================
#define HOST20_MSCD_CSW_Status_PASS        0x00
#define HOST20_MSCD_CSW_Status_FAIL        0x01
#define HOST20_MSCD_CSW_Status_PASE_ERROR  0x02
#define HOST20_MSCD_CSW_Status_STALL       0x03
#define HOST20_MSCD_CSW_Status_NOT_VALID   0x04
#define HOST20_MSCD_DEVICE_FATAL_ERROR	0x05
//=================== 3.Structure Definition =============================================================
//========================================================================================================

//=================== 4.Extern Function Definition =======================================================
//========================================================================================================
extern BOOLEAN Host20_MSCD_usb_stor_control_thread(struct us_data *psMassStorage);
extern UINT8 Host20_MSCD_GetMaxLUN(void);
//=================== 5.Extern Variable Definition =======================================================
//========================================================================================================






#endif

