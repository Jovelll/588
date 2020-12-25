////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2008 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
#include "USBHostConfig_2.h"
#include <stdlib.h>
#ifndef ATV_SERISE_USE
#include <stdio.h>
#endif
#include <string.h>
#ifndef ATV_SERISE_USE
#include "datatype.h"
#else
#include "UsbHostDataDef.h"
#endif
#include "drvHostLib_2.h"
#include "drvHost200_2.H"
#include "drvScsi_2.h"
#include "drvMassStorage_2.h"
#include <R8051XC.h>
#ifdef ATV_SERISE_USE
#include "Board.h"
#include "common.h"
#include "uart.h"
#endif
#ifdef CERAMAL_SERISE_USE
#include "drvtimer.h"
#else
#include "msAPI_Timer.h"
#endif
#include "drvUsbMain_2.h"
#include "drvusb.h"

#define DRV_SCSI_DBG(x) // x;

static void vScsi_SendCmd_Done(struct scsi_cmnd *srb);
//struct us_data xdata *Mass_stor_us;
#define MAX_LUN_SUPPORT    5

struct us_data xdata Mass_stor_us1_port2;
struct LUN_Device xdata Mass_stor_device_1_port2; //support 4 LUN
struct LUN_Device xdata Mass_stor_device_2_port2;
struct LUN_Device xdata Mass_stor_device_3_port2;
struct LUN_Device xdata Mass_stor_device_4_port2;
struct LUN_Device xdata Mass_stor_device_5_port2;
extern 	UINT8 xdata *pUsbData_Port2;
Scsi_Cmnd  srb_data_port2;

UINT8 u8Drive_port2 = 0;
extern U8 gUsbStatus_Port2;
extern U8 gSpeed_Port2;

//BOOLEAN gDeviceFatalError=FALSE;
void Init_Mass_Storage_Structure_Port2()
{
  //Mass_stor_us=&Mass_stor_us1;
  memset(&Mass_stor_device_1_port2, 0, sizeof(Mass_stor_device_1_port2));
  Mass_stor_us1_port2.Mass_stor_device=&Mass_stor_device_1_port2;
  Mass_stor_us1_port2.max_lun=   Host20_MSCD_GetMaxLUN_Port2();//Only support 1 LUN
  //printf("Max Lun:%02bx\n",Mass_stor_us1.max_lun);
}
//***************************************************************************************
// Function Name: vSCSICmd_READ_10
// Description:
// Input:
// Output:
// Status:S
//***************************************************************************************
void vSCSICmd_READ_WRITE_10_Port2(BOOLEAN DoWrite,struct us_data *us, Scsi_Cmnd *srb, INT8U u8LunNum,
							  INT32U u32BlockAddr, INT16U u16BlockNum, U32 u8Buffer)
{


  /* set the command and the LUN */
  #if 0
  memset(srb->cmnd, 0, MAX_COMMAND_SIZE);
  srb->cmnd[0] = READ_10;

  srb->cmnd[2] = (INT8U)(u32BlockAddr >> 24);
  srb->cmnd[3] = (INT8U)(u32BlockAddr >> 16);
  srb->cmnd[4] = (INT8U)(u32BlockAddr >> 8);
  srb->cmnd[5] = (INT8U)(u32BlockAddr);

  srb->cmnd[7] = (INT8U)(u16BlockNum >> 8);
  srb->cmnd[8] = (INT8U)(u16BlockNum );
  #endif
  if (DoWrite)
    pOTGH_PT_BLK_Port2->sCBW.u8CB[0]=WRITE_10;
  else
    pOTGH_PT_BLK_Port2->sCBW.u8CB[0]=READ_10;
  pOTGH_PT_BLK_Port2->sCBW.u8CB[1]=0;

  pOTGH_PT_BLK_Port2->sCBW.u8CB[2]= (INT8U)(u32BlockAddr >> 24);
  pOTGH_PT_BLK_Port2->sCBW.u8CB[3]= (INT8U)(u32BlockAddr >> 16);
  pOTGH_PT_BLK_Port2->sCBW.u8CB[4]= (INT8U)(u32BlockAddr >> 8);
  pOTGH_PT_BLK_Port2->sCBW.u8CB[5]= (INT8U)(u32BlockAddr);

  pOTGH_PT_BLK_Port2->sCBW.u8CB[7] = (INT8U)(u16BlockNum >> 8);
  pOTGH_PT_BLK_Port2->sCBW.u8CB[8] = (INT8U)(u16BlockNum );


  /* FIXME: we must do the protocol translation here */
  /*
  if (us->subclass == US_SC_RBC || us->subclass == US_SC_SCSI)
    srb->cmd_len = 6;
  else
	srb->cmd_len = 12;
  */
  srb->cmd_len = 10;

  /* set the transfer direction */
  if (DoWrite)
  	srb->sc_data_direction = SCSI_DATA_WRITE;
  else
	srb->sc_data_direction = SCSI_DATA_READ;

  /* use the new buffer we have */
  srb->phy_buf_adr= u8Buffer;
  srb->buffer_type=BUFFER_PHY;

  /* set the buffer length for transfer */
  srb->request_bufflen = (us->Mass_stor_device[u8LunNum].u32BlockSize)*u16BlockNum;

  /* set up for no scatter-gather use */
  //srb->use_sg = 0;

  /* change the serial number -- toggle the high bit*/
  //srb->serial_number ^= 0x80000000;

  /* set Lun number*/
  srb->lun= u8LunNum;

  //srb->scsi_done = vScsi_SendCmd_Done;



}
#if 0
//***************************************************************************************
// Function Name:
// Description:
// Input:
// Output:
// Status:S
//***************************************************************************************
void vSCSICmd_WRITE_10(struct us_data *us, Scsi_Cmnd *srb,  INT8U u8LunNum,
							  INT32U u32BlockAddr, INT16U u16BlockNum, U32 u8Buffer)
{


  /* set the command and the LUN */
  #if 0
  memset(srb->cmnd, 0, MAX_COMMAND_SIZE);
  srb->cmnd[0] = WRITE_10;

  srb->cmnd[2] = (INT8U)(u32BlockAddr >> 24);
  srb->cmnd[3] = (INT8U)(u32BlockAddr >> 16);
  srb->cmnd[4] = (INT8U)(u32BlockAddr >> 8);
  srb->cmnd[5] = (INT8U)(u32BlockAddr);

  srb->cmnd[7] = (INT8U)(u16BlockNum >> 8);
  srb->cmnd[8] = (INT8U)(u16BlockNum );
  #endif
  pOTGH_PT_BLK_Port2->sCBW.u8CB[0]=WRITE_10;
  pOTGH_PT_BLK_Port2->sCBW.u8CB[1]=0;

  pOTGH_PT_BLK_Port2->sCBW.u8CB[2]= (INT8U)(u32BlockAddr >> 24);
  pOTGH_PT_BLK_Port2->sCBW.u8CB[3]= (INT8U)(u32BlockAddr >> 16);
  pOTGH_PT_BLK_Port2->sCBW.u8CB[4]= (INT8U)(u32BlockAddr >> 8);
  pOTGH_PT_BLK_Port2->sCBW.u8CB[5]= (INT8U)(u32BlockAddr);

  pOTGH_PT_BLK_Port2->sCBW.u8CB[7] = (INT8U)(u16BlockNum >> 8);
  pOTGH_PT_BLK_Port2->sCBW.u8CB[8] = (INT8U)(u16BlockNum );

  /* FIXME: we must do the protocol translation here */
  /*
  if (us->subclass == US_SC_RBC || us->subclass == US_SC_SCSI)
    srb->cmd_len = 6;
  else
	srb->cmd_len = 12;
  */
  srb->cmd_len = CB_LENGTH_WRITE_10;

  /* set the transfer direction */
  srb->sc_data_direction = SCSI_DATA_WRITE;

  /* use the new buffer we have */
  srb->phy_buf_adr=u8Buffer;
  srb->buffer_type=BUFFER_PHY;

  /* set the buffer length for transfer */
  srb->request_bufflen = us->Mass_stor_device[u8LunNum].u32BlockSize*u16BlockNum;

  /* set up for no scatter-gather use */
  //srb->use_sg = 0;

  /* change the serial number -- toggle the high bit*/
  //srb->serial_number ^= 0x80000000;

  /* set Lun number*/
  srb->lun= u8LunNum;

  //srb->scsi_done = vScsi_SendCmd_Done;


}
#endif
//***************************************************************************************
// Function Name:
// Description:
// Input:
// Output:
// Status:S
//***************************************************************************************
void vSCSICmd_INQUIRY_Port2( Scsi_Cmnd *srb, INT8U *u8Buffer)
{


  /* set the command and the LUN */
  //memset(srb->cmnd, 0, MAX_COMMAND_SIZE);
  //srb->cmnd[0] = INQUIRY;
  //srb->cmnd[4] = DATA_LENGTH_INQUIRY;
  memset(&pOTGH_PT_BLK_Port2->sCBW.u8CB[0], 0, 16);
  pOTGH_PT_BLK_Port2->sCBW.u8CB[0]= INQUIRY;
  pOTGH_PT_BLK_Port2->sCBW.u8CB[4]= DATA_LENGTH_INQUIRY;

  /* FIXME: we must do the protocol translation here */
  /*
  if (us->subclass == US_SC_RBC || us->subclass == US_SC_SCSI)
    srb->cmd_len = 6;
  else
	srb->cmd_len = 12;
  */
  srb->cmd_len = CB_LENGTH_INQUIRY;

  /* set the transfer direction */
  srb->sc_data_direction = SCSI_DATA_READ;

  /* use the new buffer we have */
  srb->request_buffer = u8Buffer;
  srb->buffer_type=BUFFER_LOGI;

  /* set the buffer length for transfer */
  srb->request_bufflen = DATA_LENGTH_INQUIRY;

  /* set up for no scatter-gather use */
  //srb->use_sg = 0;

  /* change the serial number -- toggle the high bit*/
  //srb->serial_number ^= 0x80000000;

  /* set Lun number*/
  //srb->lun= u8Drive;//0;

  //srb->scsi_done = vScsi_SendCmd_Done;


}
//***************************************************************************************
// Function Name:
// Description:
// Input:
// Output:
// Status:S
//***************************************************************************************
void vSCSICmd_READ_CAPACITY_Port2( Scsi_Cmnd *srb, INT8U *u8Buffer)
{

  /* set the command and the LUN */
  //memset(srb->cmnd, 0, MAX_COMMAND_SIZE);
  //srb->cmnd[0] = READ_CAPACITY;
  memset(&pOTGH_PT_BLK_Port2->sCBW.u8CB[0], 0, 16);
  pOTGH_PT_BLK_Port2->sCBW.u8CB[0]= READ_CAPACITY;

  /* FIXME: we must do the protocol translation here */
  /*
  if (us->subclass == US_SC_RBC || us->subclass == US_SC_SCSI)
    srb->cmd_len = 6;
  else
	srb->cmd_len = 12;
  */
  srb->cmd_len = CB_LENGTH_READ_CAPACITY;

  /* set the transfer direction */
  srb->sc_data_direction = SCSI_DATA_READ;

  /* use the new buffer we have */
  srb->request_buffer = u8Buffer;
  srb->buffer_type=BUFFER_LOGI;

  /* set the buffer length for transfer */
  srb->request_bufflen = DATA_LENGTH_READ_CAPACITY;

  /* set up for no scatter-gather use */
  //srb->use_sg = 0;

  /* change the serial number -- toggle the high bit*/
  //srb->serial_number ^= 0x80000000;

  /* set Lun number*/
  //srb->lun= u8Drive;

  //srb->scsi_done = vScsi_SendCmd_Done;


}

void vSCSICmd_READ_FORMAT_CAPACITY_Port2( Scsi_Cmnd *srb, INT8U *u8Buffer)
{

  /* set the command and the LUN */
  //memset(srb->cmnd, 0, MAX_COMMAND_SIZE);
  //srb->cmnd[0] = READ_CAPACITY;
  memset(&pOTGH_PT_BLK_Port2->sCBW.u8CB[0], 0, 16);
  pOTGH_PT_BLK_Port2->sCBW.u8CB[0]= READ_FORMAT_CAPACITY;
  pOTGH_PT_BLK_Port2->sCBW.u8CB[8]= 12; //Length

  /* FIXME: we must do the protocol translation here */
  /*
  if (us->subclass == US_SC_RBC || us->subclass == US_SC_SCSI)
    srb->cmd_len = 6;
  else
	srb->cmd_len = 12;
  */
  srb->cmd_len = CB_LENGTH_READ_CAPACITY;

  /* set the transfer direction */
  srb->sc_data_direction = SCSI_DATA_READ;

  /* use the new buffer we have */
  srb->request_buffer = u8Buffer;
  srb->buffer_type=BUFFER_LOGI;

  /* set the buffer length for transfer */
  srb->request_bufflen = 12;

  /* set up for no scatter-gather use */
  //srb->use_sg = 0;

  /* change the serial number -- toggle the high bit*/
  //srb->serial_number ^= 0x80000000;

  /* set Lun number*/
  //srb->lun= u8Drive;

  //srb->scsi_done = vScsi_SendCmd_Done;


}

//***************************************************************************************
// Function Name:vSCSICmd_MODE_SENSE_Port2
// Description:
// Input:
// Output:
// Status:
//***************************************************************************************
void vSCSICmd_MODE_SENSE_Port2( Scsi_Cmnd *srb,INT8U u8PageCode,INT8U *u8Buffer)
{

  /* set the command and the LUN */
  //memset(srb->cmnd, 0, MAX_COMMAND_SIZE);
  //srb->cmnd[0] = MODE_SENSE;
  //srb->cmnd[2] = u8PageCode;
  //srb->cmnd[4] = DATA_LENGTH_MODE_SENSE;
  memset(&pOTGH_PT_BLK_Port2->sCBW.u8CB[0], 0, 16);
  pOTGH_PT_BLK_Port2->sCBW.u8CB[0]= MODE_SENSE;
  pOTGH_PT_BLK_Port2->sCBW.u8CB[2]= u8PageCode;
  pOTGH_PT_BLK_Port2->sCBW.u8CB[4]= DATA_LENGTH_MODE_SENSE;

  /* FIXME: we must do the protocol translation here */
  /*
  if (us->subclass == US_SC_RBC || us->subclass == US_SC_SCSI)
    srb->cmd_len = 6;
  else
    srb->cmd_len = 12;
  */
  srb->cmd_len = CB_LENGTH_MODE_SENSE;

  /* set the transfer direction */
  srb->sc_data_direction = SCSI_DATA_READ;

  /* use the new buffer we have */
  srb->request_buffer = u8Buffer;
  srb->buffer_type=BUFFER_LOGI;
  /* set the buffer length for transfer */
  srb->request_bufflen = DATA_LENGTH_MODE_SENSE;

  /* set up for no scatter-gather use */
  //srb->use_sg = 0;

  /* change the serial number -- toggle the high bit*/
  //srb->serial_number ^= 0x80000000;

  /* set Lun number*/
  //srb->lun= u8Drive;

  //srb->scsi_done = vScsi_SendCmd_Done;


}
//***************************************************************************************
// Function Name:vSCSICmd_REQUEST_SENSE_Port2
// Description:
// Input:
// Output:
// Status:
//***************************************************************************************
void vSCSICmd_REQUEST_SENSE_Port2(  Scsi_Cmnd *srb, INT8U *u8Buffer)
{


  /* set the command and the LUN */
  //memset(srb->cmnd, 0, MAX_COMMAND_SIZE);
  //srb->cmnd[0] = REQUEST_SENSE;
  //srb->cmnd[4] = 18;
  memset(&pOTGH_PT_BLK_Port2->sCBW.u8CB[0], 0, 16);
  pOTGH_PT_BLK_Port2->sCBW.u8CB[0]= REQUEST_SENSE;
  pOTGH_PT_BLK_Port2->sCBW.u8CB[4]= 18;

  /* FIXME: we must do the protocol translation here */
  /*
  if (us->subclass == US_SC_RBC || us->subclass == US_SC_SCSI)
    srb->cmd_len = 6;
  else
	srb->cmd_len = 12;
  */
  srb->cmd_len = CB_LENGTH_REQUEST_SENSE;

  /* set the transfer direction */
  srb->sc_data_direction = SCSI_DATA_READ;

  /* use the new buffer we have */
  srb->request_buffer = u8Buffer;
  srb->buffer_type=BUFFER_LOGI;
  /* set the buffer length for transfer */
  srb->request_bufflen = 18;

  /* set up for no scatter-gather use */
  //srb->use_sg = 0;

  /* change the serial number -- toggle the high bit*/
  //srb->serial_number ^= 0x80000000;

  /* set Lun number*/
  //srb->lun= u8Drive;

  //srb->scsi_done = vScsi_SendCmd_Done;


}
#if 0
//***************************************************************************************
// Function Name:vSCSICmd_TEST_UNIT_READY
// Description:
// Input:
// Output:
// Status:S
//***************************************************************************************
void vSCSICmd_TEST_UNIT_READY( Scsi_Cmnd *srb)
{

  memset(&pOTGH_PT_BLK_Port2->sCBW.u8CB[0], 0, 16);
  pOTGH_PT_BLK_Port2->sCBW.u8CB[0]= TEST_UNIT_READY;

  srb->cmd_len = CB_LENGTH_TEST_UNIT_READY;


  /* set the transfer direction */
  srb->sc_data_direction = SCSI_DATA_READ;

  /* set the buffer length for transfer */
  srb->request_bufflen = 0;

}
#endif

void vSCSICmd_Prepare_Port2(Scsi_Cmnd *srb, U8 cmd)
{
  memset(&pOTGH_PT_BLK_Port2->sCBW.u8CB[0], 0, 16);
  pOTGH_PT_BLK_Port2->sCBW.u8CB[0]= cmd;

  srb->cmd_len = 6;
  /* set the transfer direction */
  srb->sc_data_direction = SCSI_DATA_READ;
  /* set the buffer length for transfer */
  srb->request_bufflen = 0;
}




BOOLEAN  vSCSI_EJECT_DEVICE_Port2(U8 lun)
{
  BOOLEAN result;

  Mass_stor_us1_port2.srb =&srb_data_port2;
  memset(Mass_stor_us1_port2.srb, 0, sizeof(struct scsi_cmnd ));

  vSCSICmd_Prepare_Port2(Mass_stor_us1_port2.srb,START_STOP);
  Mass_stor_us1_port2.srb->lun=lun;
  pOTGH_PT_BLK_Port2->sCBW.u8CB[4]= 2;                         //stop the device

  //result=Host20_MSCD_usb_stor_control_thread_Port2(&Mass_stor_us1);
  //if (result==FALSE) return result;
  //pOTGH_PT_BLK_Port2->sCBW.u8CB[0]= START_STOP;
  //pOTGH_PT_BLK_Port2->sCBW.u8CB[4]= 2;                         //stop the device
  //printf("lun:%02bx\n",lun);
  result=Host20_MSCD_usb_stor_control_thread_Port2(&Mass_stor_us1_port2);
  return result;
}





//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================
//***************************************************************************************
// Function Name:vSCSI_REQUEST_SENSE_Port2
// Description: Reserved.
//              If Device do not support the command-A
//              ,Host should issue command "vSCSI_REQUEST_SENSE" to ask "why not support".
// Input:
// Output:
// Status:P-OK
//***************************************************************************************
BOOLEAN vSCSI_REQUEST_SENSE_Port2(U8 lun)
{
  INT8U *u8RequestSenseData;
  //Scsi_Cmnd  srb_data;
  BOOLEAN result;

  u8RequestSenseData=pUsbData_Port2;
  Mass_stor_us1_port2.srb = &srb_data_port2;
  memset(Mass_stor_us1_port2.srb, 0, sizeof(struct scsi_cmnd ));

  // Build SCSI command.
  vSCSICmd_REQUEST_SENSE_Port2( Mass_stor_us1_port2.srb,u8RequestSenseData);

  Mass_stor_us1_port2.srb->lun=lun;

  //call mass storage function to send CBW
  //and get REQUEST_SENSE Data. Return CSW to check status.
  result=Host20_MSCD_usb_stor_control_thread_Port2(&Mass_stor_us1_port2);
  return result;



}

//***************************************************************************************
// Function Name:bSCSI_INQUIRY_Port2
// Description:To ask the device u8DeviceType/u8ProductID/u8ProductVer... information
// Input:
// Output:
// Status:P-OK
//***************************************************************************************
///////////////////////////////////////////////////////////////////////////////
//		bSCSI_INQUIRY_Port2()
//		Description:
//			1. scsi inquiry command.
//		input: none
//		output: TRUE or FALSE (BOOLEAN)
///////////////////////////////////////////////////////////////////////////////
//INT8U xdata u8InquiryData[DATA_LENGTH_INQUIRY] _at_ Host20_TEMP_DATA;
BOOLEAN bSCSI_INQUIRY_Port2(U8 lun)
{
  INT8U u8i, u8j;
  //Scsi_Cmnd  srb_data;
  U8 *u8InquiryData;
  BOOLEAN result;

  u8InquiryData=pUsbData_Port2;
  //printf("INQUIRY\n");
  for(u8j =0; u8j< ScsiCmd_Fail_Retry ;u8j++)
  {

    Mass_stor_us1_port2.srb =&srb_data_port2;
	memset(Mass_stor_us1_port2.srb, 0, sizeof(struct scsi_cmnd ));

	//Build SCSI command.
	vSCSICmd_INQUIRY_Port2( Mass_stor_us1_port2.srb, u8InquiryData);
    Mass_stor_us1_port2.srb->lun=lun;

	//call mass storage function to send CBW
	//and get INQUIRY Data. Return CSW to check status.
	result=Host20_MSCD_usb_stor_control_thread_Port2(&Mass_stor_us1_port2);
	if (result==FALSE) continue;

	if(Mass_stor_us1_port2.srb->result == SAM_STAT_GOOD)
	{
      //save all INQUIRY data
	  Mass_stor_us1_port2.device.u8DeviceType = (u8InquiryData[0] & 0x1F);
	  for(u8i =0; u8i <8; u8i++)
	    Mass_stor_us1_port2.Mass_stor_device[lun].u8VendorID[u8i] = u8InquiryData[u8i+8];
	  for(u8i =0; u8i <16; u8i++)
		Mass_stor_us1_port2.Mass_stor_device[lun].u8ProductID[u8i] = u8InquiryData[u8i+16];
	  for(u8i =0; u8i <4; u8i++)
	    Mass_stor_us1_port2.Mass_stor_device[lun].u8ProductVer[u8i] = u8InquiryData[u8i+32];

	  if(Mass_stor_us1_port2.device.u8DeviceType != TYPE_DISK)
	  {
	    DRV_SCSI_DBG(printf("Device type unsuport, it's not a scsi disk%s","\n"));

		return FALSE;
	  }
	  else
	  {

	    DRV_SCSI_DBG(SCSI_DbgPrint("SCSI INQUIRY : SCSI Device ID <%s>\n",&Mass_stor_us1_port2.Mass_stor_device[lun].u8VendorID));
		//SCSI_DbgPrint("SCSI INQUIRY : SCSI Product ID <%s>\n",&Mass_stor_us1.device.u8ProductID);
		//SCSI_DbgPrint("SCSI INQUIRY : SCSI Product ver <%s>\n",&Mass_stor_us1.device.u8ProductVer);
        return TRUE;
	  }


	}

  }
	return FALSE;
}
//***************************************************************************************
// Function Name:bSCSI_MODE_SENSE_Port2
// Description:To know the mode of device <1>.Write Protect => Y/N
// Input:
// Output:
// Status:P-OK
//***************************************************************************************
///////////////////////////////////////////////////////////////////////////////
//		bSCSI_MODE_SENSE()
//		Description:
//			1. scsi mode sense command.
//		input: none
//		output: TRUE or FALSE (BOOLEAN)
///////////////////////////////////////////////////////////////////////////////
BOOLEAN bSCSI_MODE_SENSE_Port2(U8 lun)
{
  INT8U  u8j;
  INT8U *u8ModeSenseData;
  BOOLEAN result;
  //Scsi_Cmnd  srb_data;

  u8ModeSenseData=pUsbData_Port2;

  Mass_stor_us1_port2.srb = &srb_data_port2;

  for(u8j =0; u8j< ScsiCmd_Fail_Retry ;u8j++)
  {
    memset(Mass_stor_us1_port2.srb, 0, sizeof(struct scsi_cmnd ));
    //Build SCSI command.
	vSCSICmd_MODE_SENSE_Port2( Mass_stor_us1_port2.srb,0x3F, u8ModeSenseData);
    Mass_stor_us1_port2.srb->lun=lun;

	//call mass storage function to send CBW
	//and get Mode Sense Data. Return CSW to check status.
	result=Host20_MSCD_usb_stor_control_thread_Port2(&Mass_stor_us1_port2);
	if (result==FALSE) return FALSE;

	if(Mass_stor_us1_port2.srb->result == SAM_STAT_GOOD)
	{
      //save all Mode Sense(page code=0x3F) data
	  //Now we only use u8ModeSenseData[2], this byte save device
	  //write protection information
	  Mass_stor_us1_port2.Mass_stor_device[lun].bWriteProtect = (u8ModeSenseData[2]==0x80)?
												TRUE:FALSE;


	  return TRUE;
	}
  }
  Mass_stor_us1_port2.Mass_stor_device[lun].bWriteProtect=FALSE;      //not support this command
  return FALSE;
}

//***************************************************************************************
// Function Name:bSCSI_READ_CAPACITY_Port2
// Description:To get the CAPACITY
// Input:
// Output:
// Status:S
//***************************************************************************************
///////////////////////////////////////////////////////////////////////////////
//		bSCSI_READ_CAPACITY_Port2()
//		Description:
//			1. scsi READ_CAPACITY command.
//		input: none
//		output: TRUE or FALSE (BOOLEAN)
///////////////////////////////////////////////////////////////////////////////
BOOLEAN bSCSI_READ_CAPACITY_Port2(U8 lun)
{
  INT8U u8j;
  INT8U *u8ReadCapacityData;
  BOOLEAN result;
  //Scsi_Cmnd  srb_data;

  u8ReadCapacityData=pUsbData_Port2;
  for(u8j =0; u8j< ScsiCmd_Fail_Retry ;u8j++)
  {
    Mass_stor_us1_port2.srb = &srb_data_port2;
	memset(Mass_stor_us1_port2.srb, 0, sizeof(struct scsi_cmnd ));

	//Build SCSI command.
	vSCSICmd_READ_CAPACITY_Port2(Mass_stor_us1_port2.srb, u8ReadCapacityData);
    Mass_stor_us1_port2.srb->lun=lun;

    //call mass storage function to send CBW
	//and get CAPACITY Data. Return CSW to check status.
	result=Host20_MSCD_usb_stor_control_thread_Port2(&Mass_stor_us1_port2);
	if (result==FALSE) return result;

	if(Mass_stor_us1_port2.srb->result == SAM_STAT_GOOD)
	{
	  //save all CAPACITY data
	  Mass_stor_us1_port2.Mass_stor_device[lun].u32BlockTotalNum = ((INT32U)u8ReadCapacityData[0] << 24) |
										   ((INT32U)u8ReadCapacityData[1] << 16) |
										   ((INT32U)u8ReadCapacityData[2] << 8) |
										   ((INT32U)u8ReadCapacityData[3] ) ;

	  Mass_stor_us1_port2.Mass_stor_device[lun].u32BlockSize = ((INT32U)u8ReadCapacityData[4] << 24) |
									   ((INT32U)u8ReadCapacityData[5] << 16) |
									   ((INT32U)u8ReadCapacityData[6] << 8) |
									   ((INT32U)u8ReadCapacityData[7] ) ;
     //if ( Mass_stor_us1.Mass_stor_device[lun].u32BlockSize > 1024)
     //{
     //  printf("Block size over 1024, is %X\n",Mass_stor_us1.Mass_stor_device[lun].u32BlockSize);
     //  while(1);
     //}

	 DRV_SCSI_DBG(SCSI_DbgPrint("SCSI CAPACITY : SCSI Device total block <0x%x%x>\n",
	                           (INT16U)(Mass_stor_us1_port2.Mass_stor_device[lun].u32BlockTotalNum >> 16),
					           (INT16U)Mass_stor_us1_port2.Mass_stor_device[lun].u32BlockTotalNum));
	 DRV_SCSI_DBG(SCSI_DbgPrint("SCSI CAPACITY : SCSI Product block size <0x%x bytes>\n",(INT16U)Mass_stor_us1_port2.Mass_stor_device[lun].u32BlockSize));

	 return TRUE;
   }
   else
   {
        vSCSI_REQUEST_SENSE_Port2(lun);
        return FALSE;
   }
 }
 return FALSE;

}

BOOLEAN bSCSI_READ_FORMAT_CAPACITY_Port2(U8 lun)
{
  INT8U u8j;
  INT8U *u8ReadCapacityData;
  BOOLEAN result;
  //Scsi_Cmnd  srb_data;

  u8ReadCapacityData=pUsbData_Port2;
  for(u8j =0; u8j< ScsiCmd_Fail_Retry ;u8j++)
  {
      Mass_stor_us1_port2.srb = &srb_data_port2;
	memset(Mass_stor_us1_port2.srb, 0, sizeof(struct scsi_cmnd ));

	//Build SCSI command.
	vSCSICmd_READ_FORMAT_CAPACITY_Port2(Mass_stor_us1_port2.srb, u8ReadCapacityData);
       Mass_stor_us1_port2.srb->lun=lun;

    //call mass storage function to send CBW
	//and get CAPACITY Data. Return CSW to check status.
	result=Host20_MSCD_usb_stor_control_thread_Port2(&Mass_stor_us1_port2);
	if (result==FALSE) return result;

	if(Mass_stor_us1_port2.srb->result == SAM_STAT_GOOD)
	{
	  //save all CAPACITY data
	  Mass_stor_us1_port2.Mass_stor_device[lun].u32BlockTotalNum = ((INT32U)u8ReadCapacityData[4] << 24) |
										   ((INT32U)u8ReadCapacityData[5] << 16) |
										   ((INT32U)u8ReadCapacityData[6] << 8) |
										   ((INT32U)u8ReadCapacityData[7] ) ;

	  Mass_stor_us1_port2.Mass_stor_device[lun].u32BlockSize = ((INT32U)u8ReadCapacityData[9] << 16) |
									   ((INT32U)u8ReadCapacityData[10] << 8) |
									   ((INT32U)u8ReadCapacityData[11] ) ;
     //if ( Mass_stor_us1.Mass_stor_device[lun].u32BlockSize > 1024)
     //{
     //  printf("Block size over 1024, is %X\n",Mass_stor_us1.Mass_stor_device[lun].u32BlockSize);
     //  while(1);
     //}

	 DRV_SCSI_DBG(SCSI_DbgPrint("SCSI CAPACITY : SCSI Device total block <0x%x%x>\n",
	                           (INT16U)(Mass_stor_us1_port2.Mass_stor_device[lun].u32BlockTotalNum >> 16),
					           (INT16U)Mass_stor_us1_port2.Mass_stor_device[lun].u32BlockTotalNum));
	 DRV_SCSI_DBG(SCSI_DbgPrint("SCSI CAPACITY : SCSI Product block size <0x%x bytes>\n",(INT16U)Mass_stor_us1_port2.Mass_stor_device[lun].u32BlockSize));

	 return TRUE;
   }
 }
 return FALSE;

}


//***************************************************************************************
// Function Name:bSCSI_TEST_UNIT_READY_Port2
// Description: To know that "device is ready ?"
// Input:
// Output:
// Status:
//***************************************************************************************
///////////////////////////////////////////////////////////////////////////////
//		bSCSI_TEST_UNIT_READY_Port2()
//		Description:
//			1. scsi TEST_UNIT_READY command.
//		input: none
//		output: TRUE or FALSE (BOOLEAN)
///////////////////////////////////////////////////////////////////////////////
extern void OTGH_PT_Bulk_Init_Port2(void);
extern BOOLEAN MDrv_UsbHost_Init_Port2(void);
extern UINT8 flib_Host20_Enumerate_Port2 (UINT8 bNormalEnumerate,UINT8 bAddress);
BOOLEAN bSCSI_TEST_UNIT_READY_Port2(U8 lun)
{
  INT8U u8j;
  BOOLEAN result;
  #ifdef ATV_SERISE_USE
  U32 i;
  #endif
  //Scsi_Cmnd  srb_data;
  //printf("TUR\n");

  for(u8j =0; u8j< TestUnitReady_Fail_Retry ;u8j++)
  {
    //printf("connect status:%02bx\n",mwHost20_PORTSC_ConnectStatus_Rd());
	if (mwHost20_PORTSC_ConnectStatus_Rd()==0)
      return FALSE;               //device is not connected

	Mass_stor_us1_port2.srb = &srb_data_port2;
	memset((U8 xdata *)Mass_stor_us1_port2.srb, 0, sizeof(struct scsi_cmnd ));

	// Build SCSI command.
	vSCSICmd_Prepare_Port2(Mass_stor_us1_port2.srb,TEST_UNIT_READY);
	//vSCSICmd_TEST_UNIT_READY(Mass_stor_us1.srb);
    Mass_stor_us1_port2.srb->lun=lun;
    //printf("Mass_stor_us1.srb:%02bx\n",Mass_stor_us1.srb->request_bufflen);
	//call mass storage function to send CBW
	//and get CAPACITY Data. Return CSW to check status.

	result=Host20_MSCD_usb_stor_control_thread_Port2(&Mass_stor_us1_port2);
    //printf("result:%02bx\n",result);
#if 1
        if (result == FALSE)
        {
            if (gUsbStatus_Port2== USB_TIMEOUT)
            {
                printf("resend TUR\n");
                if ( MDrv_UsbHost_Init_Port2() && (flib_Host20_Enumerate_Port2(1,3)!=0) )
                {
                    OTGH_PT_Bulk_Init_Port2();
                    continue;
                }
            }
            else
                return result;
        }
#else
        if (result==FALSE)
        {
            return result;
        }
#endif

	if(Mass_stor_us1_port2.srb->result == SAM_STAT_GOOD)
	{
	  Mass_stor_us1_port2.Mass_stor_device[lun].bDeviceReady = TRUE;
      //SCSI_DbgPrint("SCSI TEST UNIT READY : Succeed\n",u8Drive);

	  return TRUE;
	}
	else
	{
	  result=vSCSI_REQUEST_SENSE_Port2(lun);
      if ((pUsbData_Port2[12]==0x3a)&&(pUsbData_Port2[13]==0)) return FALSE;  //no media
	  if (result==FALSE) return result;
	}
    #ifndef ATV_SERISE_USE
	MDrv_Timer_Delayms(10);
	#else
	for(i=0;i<0x100;i++);
	#endif
  }
	//SCSI_DbgPrint("SCSI TEST UNIT READY : Failed%s","\n");
  return FALSE;
}
extern BOOLEAN  MDrv_Usb_Device_Enum_Port2(void);
#ifdef ATV_SERISE_USE
extern void EnableINT(void);
#endif

BOOLEAN bSCSI_TEST_UNIT_READY2_Port2(U8 lun)
{
  INT8U u8j;
  BOOLEAN result;
  U8  RetryCnt=0;

  #ifdef ATV_SERISE_USE
  U32 i;
  #endif
  //Scsi_Cmnd  srb_data;
  //printf("TUR\n");

  for(u8j =0; u8j< TestUnitReady_Fail_Retry ;u8j++)
  {
    //printf("connect status:%02bx\n",mwHost20_PORTSC_ConnectStatus_Rd());
	if (mwHost20_PORTSC_ConnectStatus_Rd()==0)
      return FALSE;               //device is not connected

	Mass_stor_us1_port2.srb = &srb_data_port2;
	memset((U8 xdata *)Mass_stor_us1_port2.srb, 0, sizeof(struct scsi_cmnd ));

	// Build SCSI command.
	vSCSICmd_Prepare_Port2(Mass_stor_us1_port2.srb,TEST_UNIT_READY);
	//vSCSICmd_TEST_UNIT_READY(Mass_stor_us1.srb);
    Mass_stor_us1_port2.srb->lun=lun;
    //printf("Mass_stor_us1.srb:%02bx\n",Mass_stor_us1.srb->request_bufflen);
	//call mass storage function to send CBW
	//and get CAPACITY Data. Return CSW to check status.

	result=Host20_MSCD_usb_stor_control_thread_Port2(&Mass_stor_us1_port2);
    //printf("result:%02bx\n",result);

    if (result==FALSE)
    {
      RetryCnt++;
      if (gUsbStatus_Port2==USB_TIMEOUT)
      {
        if (RetryCnt > 3) return FALSE;
        if ((RetryCnt==2)&&(mwOTG20_Control_HOST_SPD_TYP_Rd()==2) )           //make sure it is hi speed
        {
          DRV_SCSI_DBG( printf("Force FS\n"));
          UHC_XBYTE(0x40)|=0x80;//force enter FSmode
        }
        #ifndef ATV_SERISE_USE
        #ifndef CERAMAL_SERISE_USE
        msAPI_Timer_ResetWDT();
		#endif
        #else
          #ifdef Enable_Close_INT
          //XBYTE[0x2B00]&=0xFD;
          EnableINT();
          #endif
        #endif
        #ifdef Enable_Low_Temperature_Patch
	    if(gSpeed_Port2==0)
	    {
	      //printf("\r\n CDR toggle!!",0);
          XBYTE[gUTMI2_BASE+0x0a]^=0x10; //invert CDR_CLOCK
          XBYTE[gUTMI2_BASE+0x06]|=0x03; //reset UTMI
          XBYTE[gUTMI2_BASE+0x06]&=0xfc;
	    }
		#endif
       // MDrv_Usb_Device_Enum();
        if (MDrv_Usb_Device_Enum_Port2()==FALSE) return FALSE;

        continue;
      }
      else
        return FALSE;
    }

	if(Mass_stor_us1_port2.srb->result == SAM_STAT_GOOD)
	{
	  Mass_stor_us1_port2.Mass_stor_device[lun].bDeviceReady = TRUE;
      //SCSI_DbgPrint("SCSI TEST UNIT READY : Succeed\n",u8Drive);

	  return TRUE;
	}
	else
	{
	  result=vSCSI_REQUEST_SENSE_Port2(lun);
      if ((pUsbData_Port2[12]==0x3a)&&(pUsbData_Port2[13]==0)) return FALSE;  //no media
	  if (result==FALSE) return result;
	}
    #ifndef ATV_SERISE_USE
	MDrv_Timer_Delayms(10);
	#else
	for(i=0;i<0x100;i++);
	#endif
  }
	//SCSI_DbgPrint("SCSI TEST UNIT READY : Failed%s","\n");
  return FALSE;
}
//***************************************************************************************
// Function Name:bSCSI_Initial_Port2
// Description:
// Input:
// Output:
// Status:P-OK
//***************************************************************************************
U8 ValidLunNum_Port2;
BOOLEAN bSCSI_Initial_Port2(void)
{
  //INT8U i=0;
  //iCardSuddenlyRemove = FALSE;
  //iCardReplace = FALSE;
  BOOLEAN		bFoundValidDev = FALSE;
  struct LUN_Device* LunDevice = Mass_stor_us1_port2.Mass_stor_device;
  U8		LunIdx;

 ValidLunNum_Port2=0;
 for (	LunIdx=0; LunIdx<=Mass_stor_us1_port2.max_lun; LunIdx++)
  {
    LunDevice[LunIdx].bDeviceValid=FALSE;
	if (bSCSI_INQUIRY_Port2(LunIdx))
       {
            LunDevice[LunIdx].bDeviceValid=TRUE;
            ValidLunNum_Port2++;
       }
	//break;
  }
 DRV_SCSI_DBG(printf("ValidLunNum_Port2:%02bx\n",ValidLunNum_Port2));
    // For some usb harddrive without enough power, may meet following case.
    if (!ValidLunNum_Port2)
    {
        MDrv_SetUsbDeviceStatus_Port2(BAD_DEVICE);
        return bFoundValidDev;
    }

    // Sometime device is failed to initial at first time, but success at last.
    // It set the device status as BAD_DEVICE at first time when it failed.
    // We need to set correct value to device status when successed.
    if (gSpeed_Port2==2)
    {
        MDrv_SetUsbDeviceStatus_Port2(USB20_DEVICE);
    }
    else
    {
        MDrv_SetUsbDeviceStatus_Port2(USB11_DEVICE);
    }
  #ifdef ATV_SERISE_USE
    #ifdef Enable_Close_INT
  EA=0;
  #endif
  #endif
  for (	LunIdx=0; LunIdx<=Mass_stor_us1_port2.max_lun; LunIdx++)
  {
    LunDevice[LunIdx].bDeviceReady = FALSE;
	if (!bInit_USB_LUN_Port2(LunIdx))
	  continue;
	LunDevice[LunIdx].bDeviceReady = TRUE;
    bFoundValidDev = TRUE;

  }
  #ifdef ATV_SERISE_USE
    #ifdef Enable_Close_INT
  EA=1;
  #endif
  #endif
  return bFoundValidDev;

}
void GetValidLun_Port2()
{
  U8 LunIdx;
  struct LUN_Device* LunDevice = Mass_stor_us1_port2.Mass_stor_device;

  for (LunIdx=0; LunIdx<=Mass_stor_us1_port2.max_lun; LunIdx++)
  {
    if (LunDevice[LunIdx].bDeviceValid)
    {
      if(!bSCSI_TEST_UNIT_READY2_Port2(LunIdx))
        LunDevice[LunIdx].bDeviceReady=FALSE;
      else
        LunDevice[LunIdx].bDeviceReady = TRUE;
    }
    else
      LunDevice[LunIdx].bDeviceReady=FALSE;                   //not a valid device, so always not ready
  }
}
BOOLEAN bInit_USB_LUN_Port2(U8 Lun)
{
  if(!bSCSI_TEST_UNIT_READY_Port2(Lun))
  {
    DRV_SCSI_DBG(SCSI_DbgPrint("Scsi Device not ready (Lun=%d).\n",Lun));
	return FALSE;
  }
  //bSCSI_INQUIRY();
  if (!bSCSI_INQUIRY_Port2(Lun))
  {
    DRV_SCSI_DBG(SCSI_DbgPrint("Scsi inquiry failed (Lun=%d).\n",u8Drive_port2));
	return FALSE;
  }
  if(!bSCSI_READ_CAPACITY_Port2(Lun))
  {
    if (!bSCSI_READ_FORMAT_CAPACITY_Port2(Lun))
    {
        DRV_SCSI_DBG(SCSI_DbgPrint("Read CAPACITY failed.%s","\n"));
        return FALSE;
    }
  }
  bSCSI_MODE_SENSE_Port2(Lun);

  //if (!bSCSI_MODE_SENSE(Lun))
  //{
  //  SCSI_DbgPrint("Mode sense failed!\n");
  //  return FALSE;
  //}

  if(!bSCSI_TEST_UNIT_READY_Port2(Lun))
  {
    DRV_SCSI_DBG(SCSI_DbgPrint("Scsi Device not ready (Lun=%d).\n",u8Drive_port2));
	return FALSE;
  }
  return TRUE;

}


//***************************************************************************************
// Function Name:
// Description:
// Input:
// Output:
// Status:
//***************************************************************************************



BOOLEAN bSCSI_Read_Write10_Port2(BOOLEAN DoWrite, U8 lun,  INT32U u32BlockAddr, INT32U u32BlockNum,U32 u8Buffer)
{

  //Scsi_Cmnd  srb_data;
  BOOLEAN result;
  Mass_stor_us1_port2.srb = &srb_data_port2;
  //memset(Mass_stor_us1.srb, 0, sizeof(struct scsi_cmnd ));
  //printf("read sector:%lx, sz:%lx\n",u32BlockAddr,u32BlockNum);

  if (DoWrite)
  {
    if(Mass_stor_us1_port2.Mass_stor_device[lun].bWriteProtect)
    {
	  DRV_SCSI_DBG(printf("This device is write protect now\n"));
	  return FALSE;
	}
  }

  //Build SCSI command.
  vSCSICmd_READ_WRITE_10_Port2(DoWrite,&Mass_stor_us1_port2, Mass_stor_us1_port2.srb, lun,
								u32BlockAddr, (INT16U)u32BlockNum, u8Buffer);


  //call mass storage function to send scsi command
  result=Host20_MSCD_usb_stor_control_thread_Port2(&Mass_stor_us1_port2);
  if (result==FALSE) return result;

  if(Mass_stor_us1_port2.srb->result != SAM_STAT_GOOD)
  {
    DRV_SCSI_DBG(SCSI_DbgPrint("Scsi ReadWrite_10 command failed.%s","\n"));

	return FALSE;
  }


  //printf("#");
  return TRUE;



}

//***************************************************************************************
// Function Name:
// Description:
// Input:
// Output:
// Status:
//***************************************************************************************
#if 0

BOOLEAN bSCSI_Write_10(U8 lun, INT32U u32BlockAddr, INT32U u32BlockNum, U32 u8Buffer)
{
  INT32U u32DataLeft = (Mass_stor_us1.Mass_stor_device[lun].u32BlockSize )*u32BlockNum;
  INT32U u32TransBlockTmp, u32TransSizeTmp,u32BlockOfSet = 0, u32TransOfSet = 0;
  //Scsi_Cmnd  srb_data;
  BOOLEAN  result;

  // printf("write sector:%lx, sz:%lx\n",u32BlockAddr,u32BlockNum);
  if(!Mass_stor_us1.Mass_stor_device[lun].bWriteProtect)
  {
    Mass_stor_us1.srb =&srb_data;
	memset(Mass_stor_us1.srb, 0, sizeof(struct scsi_cmnd ));

	while(u32DataLeft > 0)
	{

      if(u32DataLeft > Scsi_Max_Transfer_Len)
	  {
	    u32TransBlockTmp = Scsi_Max_Transfer_Len / (Mass_stor_us1.Mass_stor_device[lun].u32BlockSize );
		u32TransSizeTmp = u32TransBlockTmp * (Mass_stor_us1.Mass_stor_device[lun].u32BlockSize );
		u32DataLeft -= u32TransSizeTmp;
	  }
	  else
	  {
	    u32TransBlockTmp = u32DataLeft/(Mass_stor_us1.Mass_stor_device[lun].u32BlockSize );
		u32TransSizeTmp = u32DataLeft;
		u32DataLeft = 0;
	  }


	  //Build SCSI command.
	  vSCSICmd_WRITE_10(&Mass_stor_us1, Mass_stor_us1.srb, lun,
								u32BlockAddr + u32BlockOfSet, (INT16U)u32TransBlockTmp,( u8Buffer + u32TransOfSet));

	  //call mass storage function to send scsi command
	  result=Host20_MSCD_usb_stor_control_thread(&Mass_stor_us1);
	  if (result==FALSE) return FALSE;

	  if(Mass_stor_us1.srb->result != SAM_STAT_GOOD)
	  {
	    DRV_SCSI_DBG(SCSI_DbgPrint("Scsi WRITE_10 command failed.%s","\n"));

		return FALSE;
	  }

	  u32BlockOfSet += u32TransBlockTmp;
	  u32TransOfSet += u32TransSizeTmp;

	}


	//printf("*");
	return TRUE;
  }
  else
  {
	 DRV_SCSI_DBG(printf("This device is write protect now\n"));
	 return FALSE;
  }
//
}
#endif

//***************************************************************************************
// Function Name:vScsi_SendCmd_Done
// Description:
// Input:
// Output:
// Status:
//***************************************************************************************
#if 0
static void vScsi_SendCmd_Done(struct scsi_cmnd *srb)
{

  //SCSI_DbgPrint("Send SCSI command (0x%x) Done (LUN=%d), result = 0x%x\n", srb->cmnd[0],u8Drive, srb->result);

}
#endif
#if 0
//U8 xdata abuf1[1024];
U8 xdata abuf[1024];


void TestUsbDisk_Port2()
{
  U16 i;
  U8 j;


  for (i=0 ; i < 0x400 ; i++)
  {
    abuf[i]=i & 0xff;
  }
  for (i=0 ; i < MAX_LUN_SUPPORT ; i++)
  {
    if (Mass_stor_us1.Mass_stor_device[i].bDeviceReady)
    {
      for (j=10 ; j < 150; j+=2)
      {
        printf("loop %02bx \n",j);

        if (!bSCSI_Write_10(i, j, 2,abuf))
        {
          printf("error write ..\n");
          break;

        }
        if (!bSCSI_READ_10(i, j, 2,abuf))
        {
          printf("error read ..\n");
          break;
        }
      }
      for (i=0 ; i < 0x400 ; i++)
      {
        if (abuf[i]!=(i&0xff))
          printf("data err\n");
      }
    }
  }

}

#endif




