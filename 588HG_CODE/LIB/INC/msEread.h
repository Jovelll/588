/******************************************************************************
 Copyright (c) 2012 MStar Semiconductor, Inc.
 All rights reserved.

 [Module Name]: msEread.h
 [Date]:        09-Nov-2012
 [Comment]:
   Color adjust header file.
 [Reversion History]:
*******************************************************************************/

#ifndef _MSEREAD_H_
#define _MSEREAD_H_


// Check any valid HDCP key in Efuse
Bool msEread_IsHDCPKeyInEfuse(void);

// Get HDCP key from Efuse
BYTE msEread_GetHDCPKeyFromEfuse(WORD wCount);

// Compare checksum is equal to Efuse or not
Bool msEread_CompareHDCPChecksumInEfuse(WORD u16Value);

// Get ADC bandgap trimming value from Efuse
BYTE msEread_GetADCBandgapTrimValFromEfuse(void);

// Get public data from Efuse
BYTE msEread_GetDataFromEfuse(WORD wCount);

// Set Combo initial value
void msEread_SetComboInitialValue(void);

// Set HDMI initial value
void msEread_SetHDMIInitialValue(void);

// Set MHL initial value
void msEread_SetMHLInitialValue(void);

// Get reserved value
BYTE msEread_GetReseredValue(void);

//#if defined(_EFUSE_TSUM9_)
void msEread_SelectEfuse(BYTE u8EfuseSel);	// for TSUM9 only
//#endif

#endif
