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
#include "drvHost200_2.h"
#include "drvScsi_2.h"
#include "drvMassStorage_2.h"
#ifdef ATV_SERISE_USE
#include "uart.h"
#else
#include "drvmiu.h"
#endif
#include "drvUsbMain_2.h"

#define DRV_MASS_STORAGE_DBG(x)  //x;
#define ErrorRetryCount1 3

UINT32 gwTagPort2=0x88888888;
extern U8 gSpeed_Port2;

extern void Dump_Data_Port2(UINT16 addr,UINT16 sz);
#define Enable_UHCI_Recovery
//U8 RecoveryFlag_Port2=0;


//===============================================================================================
//===============================================================================================
//===============================================================================================
//============================= Group-1: Mass Storage Class Implement ===========================
//===============================================================================================
//===============================================================================================
//===============================================================================================
//***************************************************************************************
// Function Name: Host20_MSCD_ClearFeature_Port2
// Description:
// Input:
// Output:
// Status:S
//***************************************************************************************
code  UINT8 HOST20_CLEAR_FEATURE_PORT2[8]     = {0x02,0x01,0x00,0x00,0x81,0x00,0x00,0x00}; //

UINT8 Host20_MSCD_ClearFeature_Port2(struct us_data *psMassStorage)
{
  UINT8 bTemp;
  UINT8 bCMD[8];


  memcpy(bCMD,HOST20_CLEAR_FEATURE_PORT2,8);
  //bCMD[4]=bEdNum;
  if (psMassStorage->srb->sc_data_direction==SCSI_DATA_WRITE)
    bCMD[4]=psAttachDevice_Port2->saCD[0].sInterface[0].sED[ pOTGH_PT_BLK_Port2->bOutQHDArrayNum].bED_EndpointAddress;
  else
    bCMD[4]=psAttachDevice_Port2->saCD[0].sInterface[0].sED[ pOTGH_PT_BLK_Port2->bInQHDArrayNum].bED_EndpointAddress;

  //if (pOTGH_PT_BLK_Port2->bInQHDArrayNum==0) bCMD[4]=0x81;
  //else bCMD[4]=0x82;
  //printf("clear stall:%02bx\n",bCMD[4]);

  if (flib_Host20_Issue_Control_Port2 (1,bCMD,0x08,&bTemp)>0)
  {

    //printf(">>> Host20_MSCD_ClearFeature_Port2 Fail ...\r\n");
    return (0);
  }
  else
  {
    //printf("Clear stall succeed\n");
    return (1);
  }
}

#if 1
code   UINT8 HOST20_MSCD_GET_MAX_LUN_PORT2[8]     = {0xA1,0xFE,0x00,0x00,0x00,0x00,0x01,0x00}; //Get MAX LUN

//***************************************************************************************
// Function Name: Host20_MSCD_GetMaxLUN_Port2
// Description:
// Input:
// Output:
// Status:S
//***************************************************************************************
UINT8 Host20_MSCD_GetMaxLUN_Port2(void)
{
  UINT8 bMaxLun;
  UINT8 bCMD[8];
  //UINT8 HOST20_MSCD_GET_MAX_LUN[8]     = {0xA1,0xFE,0x00,0x00,0x00,0x00,0x01,0x00}; //Get MAX LUN

  //Setup=0xA1,0xFE,0x00,0x00,0x00,0x00,0x01,0x00
  //In =1 bytes
  //Ack
  //printf("data buffer:%x  %lx\n",psAttachDevice_Port2->bDataBuffer,USB_BUFFER_START_ADR_4K_ALIGN);
  memcpy(bCMD,HOST20_MSCD_GET_MAX_LUN_PORT2,8);

  if (flib_Host20_Issue_Control_Port2 (1,bCMD,0x01,&bMaxLun)>0)
  {

    DRV_MASS_STORAGE_DBG(printf(">>>Get Max Lun Fail ...\n"));
    return (0);
  }
  else
  {
    //printf("Get Max Lun succeed\n");
    return (bMaxLun);

  }



}

#endif

extern void Dump_Data_Port2(UINT16 addr,UINT16 sz);

UINT32 SwapWord_Port2(UINT32 dat1)
{
  U8  t1,t2,t3,t4;
  U32 temp;

  t1=(U8)(dat1 >> 24) & 0xff;
  t2=(U8)(dat1 >> 16) & 0xff;
  t3=(U8)(dat1 >> 8) & 0xff;
  t4=(U8)(dat1) & 0xff;

  temp=( (U32) t4 <<24) | ((U32) t3 << 16) |  ((U32) t2 << 8) | t1  ;
  return temp;
}
//***************************************************************************************
// Function Name:Host20_MSCD_issue_CBW_Port2
// Description:
// Input:
// Output:
// Status:P-OK
//***************************************************************************************
UINT8 Host20_MSCD_issue_CBW_Port2(struct us_data *psMassStorage)
{
  //Host20_BufferPointerArray_Structure aTemp;
  U8 result;
  U32 buf;

  //   printf("issue CBW...\n");
  //<1>.Prepare the CBW data
  pOTGH_PT_BLK_Port2->sCBW.u32Signature = OTGH_PT_CBW_SIGNATE;
  pOTGH_PT_BLK_Port2->sCBW.u32DataTransferLength=SwapWord_Port2(psMassStorage->srb->request_bufflen);
  pOTGH_PT_BLK_Port2->sCBW.u32Tag=gwTagPort2;
  pOTGH_PT_BLK_Port2->sCBW.u8LUN=psMassStorage->srb->lun;

  switch (psMassStorage->srb->sc_data_direction)
  {
    case SCSI_DATA_WRITE:
         //printf("data wr\n");
         pOTGH_PT_BLK_Port2->sCBW.u8Flags=0x00;
         break;
    case SCSI_DATA_READ:
         //printf("data rd\n");

         pOTGH_PT_BLK_Port2->sCBW.u8Flags=0x80;  //john 0x01
         break;
    //case SCSI_DATA_UNKNOWN:
    //case SCSI_DATA_NONE:
    //DRV_MASS_STORAGE_DBG(printf(">>> Unknow 'psMassStorage->srb->sc_data_direction'... \n"));
    //while(1);
    //break;
  }

  pOTGH_PT_BLK_Port2->sCBW.u8LUN=psMassStorage->srb->lun;//Only support 1 LUN
  pOTGH_PT_BLK_Port2->sCBW.u8CBLength=psMassStorage->srb->cmd_len;//Command Length
  //memcpy((U8 xdata *)&(pOTGH_PT_BLK_Port2->sCBW.u8CB[0]),(U8 xdata *)&(psMassStorage->srb->cmnd[0]),16);

  //printf("signature:%lx\n",(pOTGH_PT_BLK_Port2->sCBW.u32Signature));
  //printf("scbw:%x\n",(U16)&(pOTGH_PT_BLK_Port2->sCBW.u32Signature));
  //Dump_Data((U16)&(pOTGH_PT_BLK_Port2->sCBW.u32Signature),31);
  //buf=(U8 xdata *)psAttachDevice_Port2->bDataBuffer;
  //memcpy(buf,(U8 xdata *)&(pOTGH_PT_BLK_Port2->sCBW.u32Signature),31);
  buf=VirtoPhyAddr((U16)&(pOTGH_PT_BLK_Port2->sCBW.u32Signature));
  result=flib_Host20_Issue_Bulk_Port2 (pOTGH_PT_BLK_Port2->bOutQHDArrayNum,31,buf,OTGH_Dir_Out);
  return(result);
}

//***************************************************************************************
// Function Name:Host20_MSCD_issue_CSW_Port2
// Description:
// Input:
// Output:
// Status:P-OK
//***************************************************************************************
UINT8 Host20_MSCD_issue_CSW_Port2()
{
  UINT32 wCBWTag,wCSWTag;
  //Host20_BufferPointerArray_Structure aTemp;
  U32 buf;
  U8 result,retry=0;

  //printf("Get CSW");
  // buf=(U8 xdata *)psAttachDevice_Port2->bDataBuffer;
  buf=VirtoPhyAddr(psAttachDevice_Port2->bDataBuffer);

  redo2:
       result=flib_Host20_Issue_Bulk_Port2 (pOTGH_PT_BLK_Port2->bInQHDArrayNum,13,buf,OTGH_Dir_IN);
       if (result==HOST20_TRANSACTION_ERROR)
       {
         retry++;
         if(mwHost20_PORTSC_ConnectStatus_Rd()==0 || (retry>ErrorRetryCount1))
           return HOST20_FATAL;

         #ifdef Enable_Low_Temperature_Patch
         if(retry<=ErrorRetryCount1)
         {
           if(gSpeed_Port2==0)
           {
             //printf("\r\n CSW toggle!!",0);
             XBYTE[gUTMI2_BASE+0x0a]^=0x10; //invert CDR_CLOCK
             XBYTE[gUTMI2_BASE+0x06]|=0x03;        //reset UTMI
             XBYTE[gUTMI2_BASE+0x06]&=0xfc;
           }
         }
         #endif
         //printf("csw trans err retry\n");
         goto redo2;
       }

   // printf("result:%02bx\n",result);
   if (result==HOST20_FATAL) return HOST20_MSCD_DEVICE_FATAL_ERROR;
   else if (result!=0)
   {  //john    if (bTemp==HOST20_FAIL) {
     return(HOST20_MSCD_CSW_Status_STALL);
   }
   memcpy((U8 xdata *)&(pOTGH_PT_BLK_Port2->sCSW.u32Signature),(U8 xdata *)psAttachDevice_Port2->bDataBuffer,13);

   //<2>.Check the data-Tag
   wCBWTag=(UINT32)(pOTGH_PT_BLK_Port2->sCBW.u32Tag);
   wCSWTag=(UINT32)(pOTGH_PT_BLK_Port2->sCSW.u32Tag);
   if (wCBWTag!=wCSWTag)
   {
     DRV_MASS_STORAGE_DBG(printf(">>> Error: Received CSW->Tag fail (Expected data:0x%x / Received Data:0x%x)\n",(UINT32)(wCBWTag),(UINT32)(wCSWTag)));
     return(HOST20_MSCD_CSW_Status_NOT_VALID);
   }

   //<3>.Check the data-Signature
   if ((pOTGH_PT_BLK_Port2->sCSW.u32Signature)!=OTGH_PT_CSW_SIGNATE)
   {
     DRV_MASS_STORAGE_DBG(printf(">>> Error: Received CSW->Signature fail (Expected data:%d / Received Data:%d)\n",OTGH_PT_CSW_SIGNATE,(UINT32)(sOTGH_PT_BLK_PORT2->sCSW.u32Tag)));
     return(HOST20_MSCD_CSW_Status_NOT_VALID);
   }

   //<4>.Checking Status OTGH_PT_CSW_PASS
   if ((pOTGH_PT_BLK_Port2->sCSW.u8Status)!=HOST20_MSCD_CSW_Status_PASS)
   {//printf(">>> Error: Received CSW->Status fail 'HOST20_MSCD_CSW_Status_FAIL'\n");
     return(pOTGH_PT_BLK_Port2->sCSW.u8Status);
   }
   return(HOST20_MSCD_CSW_Status_PASS);
}
extern U16 TotalBytes_Port2;

//***************************************************************************************
// Function Name: Host20_MSCD_issue_Data_Port2
// Description:
// Input:
// Output:
// Status:P-OK
//***************************************************************************************
UINT8 Host20_MSCD_issue_Data_Port2(struct us_data *psMassStorage)
{

  // UINT8 *pbDataPage[5];
  UINT32 wTotalLengthRemain,Offset;
  U32 buf;
  U16 TransferLengh;
  U8 result,retry;

  //printf("data phase\n");


  //<1>.To fill the data buffer

  wTotalLengthRemain=psMassStorage->srb->request_bufflen;


  if (psMassStorage->srb->buffer_type==BUFFER_LOGI)
               // memcpy(buf,psMassStorage->srb->request_buffer,psMassStorage->srb->request_bufflen);
        buf=VirtoPhyAddr((U16)(psMassStorage->srb->request_buffer));
  else
        buf=(psMassStorage->srb->phy_buf_adr);


  //<2>.Issue Transfer

    switch (psMassStorage->srb->sc_data_direction)
    {
      case SCSI_DATA_WRITE://Out
      Offset=0;
      while(wTotalLengthRemain>0)
      {
        if(wTotalLengthRemain > Scsi_Max_Transfer_Len)
        {
          TransferLengh = Scsi_Max_Transfer_Len;
        }
        else
        {
          TransferLengh=wTotalLengthRemain;
        }

        result=flib_Host20_Issue_Bulk_Port2 (pOTGH_PT_BLK_Port2->bOutQHDArrayNum,TransferLengh
                             ,buf+Offset,OTGH_Dir_Out);
        retry=0;
        while (result==HOST20_TRANSACTION_ERROR)
        {
          retry++;
          //RecoveryFlag_Port2|=0x01;
          if(pOTGH_PT_BLK_Port2->bOutQHDArrayNum)
            pHost20_qHD_List_Bulk1_Port2->bDataToggleControl=1;
          else
            pHost20_qHD_List_Bulk0_Port2->bDataToggleControl=1;

          DRV_MASS_STORAGE_DBG(printf("\r\n retry OUT\n",0));

          if(mwHost20_PORTSC_ConnectStatus_Rd()==0 || (retry>ErrorRetryCount1))
          {
            if(pOTGH_PT_BLK_Port2->bOutQHDArrayNum)
              pHost20_qHD_List_Bulk1_Port2->bDataToggleControl=0;
            else
              pHost20_qHD_List_Bulk0_Port2->bDataToggleControl=0;
            //RecoveryFlag_Port2&=0xFE;
            return HOST20_FATAL;
          }

          result=flib_Host20_Issue_Bulk_Port2 (pOTGH_PT_BLK_Port2->bOutQHDArrayNum, TotalBytes_Port2,
                     buf+Offset+(TransferLengh-TotalBytes_Port2),OTGH_Dir_Out);

          if(pOTGH_PT_BLK_Port2->bOutQHDArrayNum)
            pHost20_qHD_List_Bulk1_Port2->bDataToggleControl=0;
          else
            pHost20_qHD_List_Bulk0_Port2->bDataToggleControl=0;
          //RecoveryFlag_Port2&=0xFE;

        }
        Offset+=TransferLengh;
        wTotalLengthRemain-=TransferLengh;
      }
      return result;

      break;

      case SCSI_DATA_READ://In
      //<2>.Issue the Loop2-InData & Compare
      //    if ((psMassStorage->srb->phy_buf_adr & 0xfff)==0)
      //      buf=(UINT8*)(psMassStorage->srb->phy_buf_adr);  //use original buffer

      Offset=0;
      while(wTotalLengthRemain>0)
      {
        if(wTotalLengthRemain > Scsi_Max_Transfer_Len)
        {
          TransferLengh = Scsi_Max_Transfer_Len;
        }
        else
        {

          TransferLengh=wTotalLengthRemain;
        }

        result=flib_Host20_Issue_Bulk_Port2 (pOTGH_PT_BLK_Port2->bInQHDArrayNum, TransferLengh
                             ,buf+Offset,OTGH_Dir_IN);

        if (result==HOST20_TRANSACTION_ERROR)
        {
          retry=0;
          XREDO:
               retry++;
               //printf("data in transerror retry\n");
               if(mwHost20_PORTSC_ConnectStatus_Rd()==0 || (retry>ErrorRetryCount1))
                 return HOST20_FATAL;
               #ifdef Enable_Low_Temperature_Patch
               if(retry<=ErrorRetryCount1)
               {
                 if(gSpeed_Port2==0)
                 {
                   //printf("\r\n IN toggle!!",0);
                   XBYTE[gUTMI2_BASE+0x0a]^=0x10; //invert CDR_CLOCK
                   XBYTE[gUTMI2_BASE+0x06]|=0x03;        //reset UTMI
                   XBYTE[gUTMI2_BASE+0x06]&=0xfc;
                 }
               }
               #endif
               result=flib_Host20_Issue_Bulk_Port2 (pOTGH_PT_BLK_Port2->bInQHDArrayNum, TotalBytes_Port2,
                   buf+Offset+(TransferLengh-TotalBytes_Port2),OTGH_Dir_IN);
               if (result==HOST20_TRANSACTION_ERROR)
                 goto XREDO;
        }
        else if (result==HOST20_FATAL)   return result;               //timeout
        Offset+=TransferLengh;
        wTotalLengthRemain-=TransferLengh;
      }
      //for (i=0 ; i < 16 ; i++)
      //    printf("%02bx ", buf[i]);
      //printf("\n");
      //printf("\ndump data\n");
      //Dump_Data((U16)psMassStorage->srb->request_buffer,psMassStorage->srb->request_bufflen);
      //printf("\ndump buf\n");
      //Dump_Data((U16)buf,36);

      return result;

      break;



    }

    return(result);

}


//***************************************************************************************
// Function Name: Host20_MSCD_usb_stor_control_thread_Port2
// Description:
//             <1>.Issue CBW
//             <2>.Issue Data Stage
//             <3>.Issue CSW
//             <4>.Fill the result & return
// Input:
// Output:
// Status:S
//***************************************************************************************
BOOLEAN Host20_MSCD_usb_stor_control_thread_Port2(struct us_data *psMassStorage)
{
  UINT8 bValue,retry;
  UINT8 result;
  //<1>.CBW issue
  result=Host20_MSCD_issue_CBW_Port2(psMassStorage);
  if (result==HOST20_FATAL) return FALSE;
  else if (result==HOST20_TRANSACTION_ERROR)
  {
    DRV_MASS_STORAGE_DBG( printf("\r\n ??? 'Host20_MSCD_issue_CBW_Port2' Fail...(%bx)",pOTGH_PT_BLK_Port2->sCBW.u8Flags));
    #ifdef Enable_UHCI_Recovery
    DRV_MASS_STORAGE_DBG(printf("\r\n retry cbw",0));
    //RecoveryFlag_Port2|=0x01;
    if(pOTGH_PT_BLK_Port2->bOutQHDArrayNum)
      pHost20_qHD_List_Bulk1_Port2->bDataToggleControl=1;
    else
      pHost20_qHD_List_Bulk0_Port2->bDataToggleControl=1;
    retry=0;
    while(1)
    {
      retry++;

      if(mwHost20_PORTSC_ConnectStatus_Rd()==0 || (retry>ErrorRetryCount1))
      {
        if(pOTGH_PT_BLK_Port2->bOutQHDArrayNum)
          pHost20_qHD_List_Bulk1_Port2->bDataToggleControl=0;
        else
          pHost20_qHD_List_Bulk0_Port2->bDataToggleControl=0;
        //RecoveryFlag_Port2&=0xFE;
        return FALSE;
      }

      result=Host20_MSCD_issue_CBW_Port2(psMassStorage);

      if (result==HOST20_OK) break;
      else if (result==HOST20_FATAL) return FALSE;
    }
    if(pOTGH_PT_BLK_Port2->bOutQHDArrayNum)
      pHost20_qHD_List_Bulk1_Port2->bDataToggleControl=0;
    else
      pHost20_qHD_List_Bulk0_Port2->bDataToggleControl=0;
    //RecoveryFlag_Port2&=0xFE;
    #else
    return FALSE;
    #endif
  }

  //<2>.Data issue
  retry=0;
  if (psMassStorage->srb->request_bufflen>0)
  {
    bValue=Host20_MSCD_issue_Data_Port2(psMassStorage);
    if (bValue==HOST20_DEVICE_STALL)
      Host20_MSCD_ClearFeature_Port2(psMassStorage);
    else if (bValue==HOST20_FATAL)   return FALSE;

  }
  //<3>.CSW issue
  //printf("Get CSW");
  //printf("bULK 0:\n");
  // Dump_Data(pHost20_qHD_List_Bulk0_Port2,0x30);

  bValue=Host20_MSCD_issue_CSW_Port2();
  if (bValue==HOST20_MSCD_DEVICE_FATAL_ERROR) return FALSE;
  if(bValue>0)
  {
    //printf("??? 'Host20_MSCD_issue_CSW_Port2' Fail...\n");
    //For Stall=>try again
    psMassStorage->srb->result = SAM_STAT_CHECK_CONDITION;        //problem happened
    if (bValue==HOST20_MSCD_CSW_Status_STALL)
    {
      //printf(">>> Device Stall the Command, CSW issue again.\n");
      //<3.1>.Clear Feature
      Host20_MSCD_ClearFeature_Port2(psMassStorage);
      //<3.2>.Read CSW again
      bValue=Host20_MSCD_issue_CSW_Port2();
      if(bValue>0)
        psMassStorage->srb->result = SAM_STAT_CHECK_CONDITION;        //problem happened
      else
        psMassStorage->srb->result = SAM_STAT_GOOD;        //problem happened


    }


  }
  else
    psMassStorage->srb->result = SAM_STAT_GOOD;

  gwTagPort2++;

  return TRUE;
}

