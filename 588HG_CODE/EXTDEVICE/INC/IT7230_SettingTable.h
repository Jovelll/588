// ****************************************************************************
// IT7230_SettingTable.h
// Create date  :   20120402
// Modifications:   20120402
// Version      :   IT7230_0006
// Definitions  : 
// Copyright (c) 2009 ITE Tech. Inc. All Rights Reserved.
// ****************************************************************************
#ifndef _IT7230_SettingTable_H
#define _IT7230_SettingTable_H

//Choice about the IT7230 PACKAGE
#define IT7230_AFN //40 PIN
//#define IT7230_BFN //24 PIN
//#define IT7230_EFN //16 PIN
//
//Change the I2C address of IT7230 (default value: 0x8C) if necessary.
#if defined(IT7230_AFN ) || defined(IT7230BFN) && !defined(IT7230_EFN)
    #define I2C_DEVICE_ADDR 0x8C //AFN, BFN package
#else
    #define I2C_DEVICE_ADDR 0x2E //EFN package
#endif

//INT define,dependent on Scaler 
//#define ENABLE_IT7230_INTPIN
#ifdef ENABLE_IT7230_INTPIN
sbit INTPin = P1^2;
#endif

/////////////////////////////////////////////////////////////////////////////////
//--------------------------Define Option function-----------------------------// 
/////////////////////////////////////////////////////////////////////////////////
//#define LED_FUNCTION // replace by TOUCH_KEY_CTRL_LED in DefaultComplierOption.h
//#define MOBILE_INTERFERENCE // remove to DefaultComplierOption.h
//#define ENABLE_LOW_CONTACT // remove to DefaultComplierOption.h


#endif