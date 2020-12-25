// ****************************************************************************
// IT7230_I2C.h
// Create date  :   20120104
// Modifications:   20120402
// Version      :   IT7230_0006
// Definitions  : The IT7230 I2C interface functions.
// Copyright (c) 2009 ITE Tech. Inc. All Rights Reserved.
// ****************************************************************************

#ifndef _IT7230_I2C_H
#define _IT7230_I2C_H

#include "IT7230_SettingTable.h"
//#define I2C_WR		0
//#define I2C_RD		1
//#define	ACK			0
//#define	NACK		1

//SCL & SDA define,dependent on Scaler  
//sbit P1_0 = P1^0;
//sbit P1_1 = P1^1;

//#define SCL             P1_0
//#define SDA             P1_1
//

//#define IIC_SCL_HIGH   (SCL=1)
//#define IIC_SDA_HIGH   (SDA=1)
//#define IIC_SCL_LOW    (SCL=0)
//#define IIC_SDA_LOW    (SDA=0)
//#define IIC_SDA_STATUS (SDA)



static BYTE xdata isInit=FALSE;

//void vDelayXms(unsigned short dly);

// Local function prototypes
//static void I2CInit(void);
//static void I2CWait(void);
//static void I2CStart(void);
//static void I2CStop(void);
//static BOOL I2CSentByte(BYTE byteData);
//static BYTE I2CReceiveByte(void);
//static void SendAcknowledge(BOOL ack);
//static BOOL I2CWriteData(const BYTE registerStartAddr, const WORD NumOfByteToWrite, PUCHAR DataBuffer);


BOOL IT7230_I2C_WriteWord(const BYTE registerStartAddr, const WORD WriteData);
BOOL IT7230_I2C_ReadWord(const BYTE registerStartAddr, WORD* recWord);
BYTE IT7230_Init();
void IT7230_I2C_Reset(void);
BOOL IT7230_I2C_Device_Addr_Check(void);



#endif //#ifndef _IT7230_I2C_H
