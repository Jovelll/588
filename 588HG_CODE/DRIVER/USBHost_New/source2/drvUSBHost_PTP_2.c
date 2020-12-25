//*********************************************************************
//
//   MODULE NAME:
//       drvUSBHost_PTP_2.c
//
//   DESCRIPTION:
//      give a short description about this file
//
//  PUBLIC PROCEDURES:
//       Name                    Title
//       ----------------------- --------------------------------------
//       int xxx_proc           declare in its corresponding header file
//
//   LOCAL PROCEDURES:
//       Name                    Title
//       ----------------------- --------------------------------------
//       get_prnt_cnvs           the local procedure in the file
//
//  Written by Derek.Horng@MSTAR Inc.
//---------------------------------------------------------------------
//
//********************************************************************

//--------------------------------------------------------------------
//                             GENERAL INCLUDE
//--------------------------------------------------------------------
#include "USBHostConfig_2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"  // for PLATFORM define
#include "Board.h"
#include "datatype.h"
//#include "mcu.h"
//#include "UsbHostDataDef_2.h"
#include "drvUsbMain_2.h"
#include "drvHostlib_2.h"
#include "drvHost200_2.h"
#include "drvscsi_2.h"
#include "drvUSBHost_PTP_2.h"
#include "drvtimer.h"
#include "drvMIU.h"
//#include "msAPI_Fat.h"
#include "drvUSB.h"

//--------------------------------------------------------------------
//  LITERAL CONSTANTS
//--------------------------------------------------------------------
//static code const char drvUSBHost_PTP_dummy_constant = 0;
//--------------------------------------------------------------------
//  TYPES
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//  MACROS
//--------------------------------------------------------------------
#define DBG_PTP(x)  //(x)
#define PTP_GEN_CONTAIN_SIZE        12
#define ErrorRetryCount2                    3

//--------------------------------------------------------------------
//  VARIABLES
//--------------------------------------------------------------------
//#if defined(ENABLE_PTP_HOST)
ASSIGNVAR   xAssignVar_Port2;
DYMNVAR     xDymnVar_Port2;
HOST_PTP_DRIVER PTP_Driver_Port2;
//OTGH_PTP_CONTAINER xdata  PTP_Cmd _at_ HOST20_PTP_CONTAINER;
static U16 gPtpBak_xdwin1_Base_Port2=0xFFFF; //use 0xFFFFFFFF to represent void

//extern U8 RecoveryFlag_Port2;
extern U8 gSpeed_Port2;
extern PTP_OBJECTINFO     gPtpObjectInfo;

code UINT8 PTP_GET_DEVICE_STATUS_Port2[8] = {0xA1,0x67,0x00,0x00,0x00,0x00,0x04,0x00};
code UINT8 USB_GET_ENDPOINT_STATUS_Port2[8] = {0x82,0x00,0x00,0x00,0x00,0x00,0x02,0x00};
code UINT8 USB_CLEAR_ENDPOINT_FEATURE_Port2[8] = {0x02,0x01,0x00,0x00,0x00,0x00,0x00,0x00};

//--------------------------------------------------------------------
//  External Function
//--------------------------------------------------------------------

extern U16 CPU2Le16(U16 u16Data);
extern U32 CPU2Le32(U32 u32Data);

extern void OTGH_PT_Bulk_Init_Port2(void);

extern void MDrv_USB_SetXdataWindow1_Port2();
extern void MDrv_USB_ReleaseXdataWindow1_Port2();

extern U32 drvUSBHost_PTP_GetPtpObjHandlesBufAdr_Port2();

//--------------------------------------------------------------------
//  Local Function
//--------------------------------------------------------------------
void drvUSBHost_PTP_Error_Handle_Port2();

void drvUSBHost_PTP_SetObjHWindow1_Port2(U32 u32PhyAddr)
{
    gPtpBak_xdwin1_Base_Port2 = (XBYTE[XDMIU_REG_BASE + 13]<<8 | XBYTE[XDMIU_REG_BASE + 12]);

    XBYTE[XDMIU_REG_BASE + 13] = (u32PhyAddr>>12)>>8;
    XBYTE[XDMIU_REG_BASE + 12] = (u32PhyAddr>>12)>>0;
}

void drvUSBHost_PTP_FreeObjHWindow1_Port2(void)
{
    XBYTE[XDMIU_REG_BASE + 13] = gPtpBak_xdwin1_Base_Port2 >> 8;
    XBYTE[XDMIU_REG_BASE + 12] = gPtpBak_xdwin1_Base_Port2 >> 0;

    gPtpBak_xdwin1_Base_Port2=0xFFFF;
}


void drvUSBHost_PTP_Transaction_Port2(
    ASSIGNVAR xdata *pAssignVar,
    DYMNVAR xdata *pDymnVar,
    BOOLEAN bNeedResp)
{
    U8 val,retry,result;

    if(drvUSBHost_PTP_SendOpCode_Port2 ( pAssignVar, pDymnVar)!=HOST20_OK)
    {
        DBG_PTP(printf("Send OpCode fail\r\n"));
        //RecoveryFlag_Port2|=0x01;
        if(pOTGH_PT_BLK_Port2->bOutQHDArrayNum)
            pHost20_qHD_List_Bulk1_Port2->bDataToggleControl=1;
        else
            pHost20_qHD_List_Bulk0_Port2->bDataToggleControl=1;
        retry=0;
        while(1)/*While Looping_Free:<Exit counter added>*/
        {
            retry++;

            if(mwHost20_PORTSC_ConnectStatus_Rd()==0||(retry>ErrorRetryCount2))
            {
                if(pOTGH_PT_BLK_Port2->bOutQHDArrayNum)
                    pHost20_qHD_List_Bulk1_Port2->bDataToggleControl=0;
                else
                    pHost20_qHD_List_Bulk0_Port2->bDataToggleControl=0;
                //RecoveryFlag_Port2&=0xFE;
                return;
            }
            result=drvUSBHost_PTP_SendOpCode_Port2 ( pAssignVar, pDymnVar);

            if (result==HOST20_OK)
                break;
            else if (result==HOST20_FATAL)
                return ;
        }
        if(pOTGH_PT_BLK_Port2->bOutQHDArrayNum)
            pHost20_qHD_List_Bulk1_Port2->bDataToggleControl=0;
        else
            pHost20_qHD_List_Bulk0_Port2->bDataToggleControl=0;
        //RecoveryFlag_Port2&=0xFE;
    }
    MDrv_Timer_Delayms(1);  // temp patch for in nak timeout issue
    drvUSBHost_PTP_BulkInData_Port2 ( pAssignVar, pDymnVar);
    if (pOTGH_PT_BLK_Port2->bStatus != HOST20_OK)
        goto Func_Done;

    MDrv_Timer_Delayms(1);  // temp patch for in nak timeout issue

    if (bNeedResp)
    {
        val = drvUSBHost_PTP_BulkInResponse_Port2();

        if( val != PTP_OK )
        {
            DBG_PTP(printf("return fail val = 0x%bx",val));
        }
        MDrv_Timer_Delayms(1);
    }

Func_Done:
    MDrv_Timer_Delayms(1);  // temp patch for in nak timeout issue
}

U8 drvUSBHost_PTP_SendOpCode_Port2(ASSIGNVAR xdata *pAssignVar,DYMNVAR xdata *pDymnVar)
{
    //OTGH_PTP_CONTAINER xdata *pPTP_Buf = (OTGH_PTP_CONTAINER xdata*)(&(PTP_Cmd));
    OTGH_PTP_CONTAINER xdata *pPTP_Buf;
    U8  u8result;
    U32 u32BufAddr;

    pPTP_Buf = (OTGH_PTP_CONTAINER xdata *) OTGH_PT_BLK_DATA;
    DBG_PTP(printf("\r\nSendOpCode\r\n"));
    pPTP_Buf->u16ConType = CPU2Le16(0x01);
    pPTP_Buf->u16OpCode = CPU2Le16(pAssignVar->u16OpCode);
    pPTP_Buf->u32TransActionID = CPU2Le32(pAssignVar->u32TransId);

    switch(pAssignVar->u16OpCode)
    {
    case PTP_OpenSession:
        DBG_PTP(printf("PTP_OpenSession\r\n"));
        pPTP_Buf->u32ConLen = CPU2Le32(0x10);
        pPTP_Buf->u32Param1 = CPU2Le32(0x01);   // session ID
        break;

    case PTP_GetDeviceInfo:
    case PTP_GetStorageIDs:
        pPTP_Buf->u32ConLen = CPU2Le32(0x0C);
        break;

    case PTP_GetStorageInfo:
        pPTP_Buf->u32ConLen = CPU2Le32(0x10);
        pPTP_Buf->u32Param1 = CPU2Le32(pDymnVar->u32StorageId);
        break;

    case PTP_GetObjectHandles:
        pPTP_Buf->u32ConLen = CPU2Le32(0x18);
        pPTP_Buf->u32Param1 = CPU2Le32(pAssignVar->u32Param1);
        pPTP_Buf->u32Param2 = CPU2Le32(pAssignVar->u32Param2);
        pPTP_Buf->u32Param3 = CPU2Le32(pAssignVar->u32Param3);
        break;

    case PTP_GetObjectInfo:
    case PTP_GetObject:
    case PTP_GetThumb:
        pPTP_Buf->u32ConLen = CPU2Le32(0x10);
        pPTP_Buf->u32Param1 = CPU2Le32(pDymnVar->u32HandleId);
        break;
    case PTP_GetPartialObject:
        pPTP_Buf->u32ConLen = CPU2Le32(0x18);
        pPTP_Buf->u32Param1 = CPU2Le32(pDymnVar->u32HandleId);
        pPTP_Buf->u32Param2 = CPU2Le32(pDymnVar->u32FileOffset);
        pPTP_Buf->u32Param3 = CPU2Le32(pDymnVar->u32DataLen);
        break;
    case PTP_CloseSession:
        pPTP_Buf->u32ConLen = CPU2Le32(0x0C);
        break;
    default:
        break;
    }

    //u32BufAddr = VirtoPhyAddr(((U16)&(PTP_Cmd)));
    u32BufAddr = VirtoPhyAddr((U16) pPTP_Buf);
    u8result = flib_Host20_Issue_Bulk_Port2 (pOTGH_PT_BLK_Port2->bOutQHDArrayNum,CPU2Le32(pPTP_Buf->u32ConLen),u32BufAddr,OTGH_Dir_Out);
    DBG_PTP(printf("SendOpCode end,0x%bX\r\n",u8result));
    pOTGH_PT_BLK_Port2->bStatus = u8result;
    return(u8result);
} //PTP_SendOpCode

/*********************************************************************
*
*   PROCEDURE NAME:
*   drvUSBHost_PTP_ChkZeroPacket
*   DESCRIPTION:
*   Check if we need get one more zero packet. If the data length for transfer is integral multiple
*   of the MaxPacketSize of the bulk endpoint.
*
*********************************************************************/
void drvUSBHost_PTP_ChkZeroPacket_Port2(U32 u32DataLen)
{
    U32     u32BufAddr;
    U16     u16MaxPacketSize;

    u16MaxPacketSize=psAttachDevice_Port2->saCD[0].sInterface[0].sED[0].bED_wMaxPacketSizeHighByte;
    u16MaxPacketSize=(u16MaxPacketSize<<8)+(psAttachDevice_Port2->saCD[0].sInterface[0].sED[0].bED_wMaxPacketSizeLowByte);

    if ( (u32DataLen > 0) && (u32DataLen % u16MaxPacketSize == 0) )
    {
        printf("In zero packet\n");
        u32BufAddr = VirtoPhyAddr(psAttachDevice_Port2->bDataBuffer);
        pOTGH_PT_BLK_Port2->bStatus = flib_Host20_Issue_Bulk_Port2 (pOTGH_PT_BLK_Port2->bInQHDArrayNum,1,u32BufAddr,OTGH_Dir_IN);
    }
}

/*********************************************************************
*
*   PROCEDURE NAME:
*   drvUSBHost_PTP_BulkInData
*   DESCRIPTION:
*   Data Phase of PTP, Bulk
*
*   NOTES:
*   xbPtpOpCode use as the global input parameter
*
*********************************************************************/
void drvUSBHost_PTP_BulkInData_Port2  (ASSIGNVAR xdata *pAssignVar,DYMNVAR xdata *pDymnVar)
{
    U8 xdata *pBuf=(U8 xdata *)(psAttachDevice_Port2->bDataBuffer);

    // Dprint(("PTP_BulkInData\n"));
    //----------------------------
    // Initialize DATA transaction
    //----------------------------
    switch( pAssignVar->u16OpCode )
    {
    case PTP_OpenSession:
    case PTP_CloseSession:
        //----------------
        // No data Phase
        //----------------
        DBG_PTP(printf(("Open Close Session\n")));
        break;

    case PTP_GetObjectInfo:
    case PTP_GetObjectHandles:
    case PTP_GetDeviceInfo:
    case PTP_GetStorageIDs:
    case PTP_GetStorageInfo:
        drvUSBHost_PTP_Read1stBulkIn_Port2( pAssignVar, pDymnVar );
        if (pOTGH_PT_BLK_Port2->bStatus!=HOST20_OK)
            goto Func_Done;

        drvUSBHost_PTP_ReadRemBulkIn_Port2( pAssignVar, pDymnVar );
        if (pOTGH_PT_BLK_Port2->bStatus!=HOST20_OK)
            goto Func_Done;

        drvUSBHost_PTP_ChkZeroPacket_Port2(pDymnVar->u32DataLen);
        break;
    //case PTP_GetObject:
        //get 12 bytes header
       //  flib_Host20_Issue_Bulk(pOTGH_PT_BLK_Port2->bInQHDArrayNum, 512,pDymnVar->u32DramAddr,OTGH_Dir_IN);
        //get real data
       // drvUSBHost_PTP_GetBulkIn( pAssignVar, pDymnVar );
    //    break;
    case PTP_GetPartialObject:
    case PTP_GetObject:
    case PTP_GetThumb:
        drvUSBHost_PTP_GetBulkIn_Port2( pDymnVar );
        break;

    default:
        break;
    }

Func_Done:
    MDrv_Timer_Delayms(1);  // temp patch for in nak timeout issue
} // PTP_BulkInData

/*********************************************************************
*
*   PROCEDURE NAME:
*   drvUSBHost_PTP_Read1stBulkIn
*
*   DESCRIPTION:
*     Read the first Bulk data 64 bytes a time
*
*********************************************************************/
void drvUSBHost_PTP_Read1stBulkIn_Port2(ASSIGNVAR xdata *pAssignVar,DYMNVAR xdata *pDymnVar)
{
    //U8 u8result;
    U8 str_len;
    U8 str_idx;
    U16 u16MaxPacketSize;
    #if (PLATFORM==MCU8051)
    //U8 xdata *pBuf=0xF000;
    U8 xdata *pBuf=(U8 xdata *)(psAttachDevice_Port2->bDataBuffer);

    //U8 u8Win1StarAddr,u8Win1EndAddr;
    //U32 u32Win1PhyAdr;

    //MDrv_GetXdataWindow1Map(&u8Win1StarAddr, &u8Win1EndAddr, &u32Win1PhyAdr);
    //MDrv_MapXdataWindow1(60,64,(((BASEADR_SDRAM_USB_DEV_BUF)+0xFFF) & ~0xFFF));
    //MDrv_MapXdataWindow1(60,64,BASEADR_SDRAM_NAND_COPY_BUF);
    #else
    //U8 *pBuf=(((BASEADR_SDRAM_USB_DEV_BUF)+0xFFF) & ~0xFFF);
    #endif

    DBG_PTP(printf("drvUSBHost_PTP_Read1stBulkIn\r\n"));
    u16MaxPacketSize=psAttachDevice_Port2->saCD[0].sInterface[0].sED[0].bED_wMaxPacketSizeHighByte;
    u16MaxPacketSize=(u16MaxPacketSize<<8)+(psAttachDevice_Port2->saCD[0].sInterface[0].sED[0].bED_wMaxPacketSizeLowByte);

    pOTGH_PT_BLK_Port2->bStatus = flib_Host20_Issue_Bulk_Port2(pOTGH_PT_BLK_Port2->bInQHDArrayNum, u16MaxPacketSize,VirtoPhyAddr(psAttachDevice_Port2->bDataBuffer),OTGH_Dir_IN);
    if( pOTGH_PT_BLK_Port2->bStatus != HOST20_OK)
    {
        DBG_PTP(printf("R 1st u8result=0x%bX\r\n"));
        goto Func_Done;
    }
    //------------------------------
    // Get the Bulk In data length
    //------------------------------
    pDymnVar->u32DataLen = LE2ME32(*((U32*)(&pBuf[0])));

    DBG_PTP(printf("DataLen=0x%lX\r\n",pDymnVar->u32DataLen));
    //------------------------------------------------
    // Get the transaction I.D. to verify the result
    //------------------------------------------------


    //---------------------------------------------
    // Parse the useful data from different Opcode
    //---------------------------------------------
    switch ( pAssignVar->u16OpCode )
    {
    //--------------------------------------
    // Get the Effective Storage and Its ID
    //--------------------------------------
    case PTP_GetStorageIDs:
        pDymnVar->u32StorageId =   LE2ME32(*(U32*)(&pBuf[16]));
        break;

    case PTP_GetStorageInfo:
        //printf("GetStorageInfo\r\n");
        PTP_Driver_Port2.u32CapacityL = LE2ME32( *(U32*)(&pBuf[18]));
        PTP_Driver_Port2.u32CapacityH = LE2ME32( *(U32*)(&pBuf[22]));
        PTP_Driver_Port2.u32FreeSpaceL= LE2ME32( *(U32*)(&pBuf[26]));
        PTP_Driver_Port2.u32FreeSpaceH= LE2ME32( *(U32*)(&pBuf[30]));
        //printf("u32CapacityH:0x%lX\r\n",PTP_Driver_Port2.u32CapacityH);
        //printf("u32CapacityL:0x%lX\r\n",PTP_Driver_Port2.u32CapacityL);
        break;
    case PTP_GetObjectInfo:
        //-----------------------------------------------------------------
        // check the file type , only check the jpeg file in current status
        // Other type of file could be added in the future
        //-----------------------------------------------------------------
        pDymnVar->u16FileType = LE2ME16(*((U16*)(&pBuf[16])));
        pDymnVar->u32FileLength = LE2ME32( *((U32*)(&pBuf[20])));
        pDymnVar->u16ThumbFormat = LE2ME16(*((U16*)(&pBuf[24])));
        pDymnVar->u32ThumbSize = LE2ME32(*((U32*)(&pBuf[26])));
        pDymnVar->u32ThumbPixWidth= LE2ME32(*((U32*)(&pBuf[30])));
        pDymnVar->u32ThumbPixHeight= LE2ME32(*((U32*)(&pBuf[34])));
        pDymnVar->ImagePixWidth= LE2ME32(*((U32*)(&pBuf[38])));
        pDymnVar->ImagePixHeight= LE2ME32(*((U32*)(&pBuf[42])));
        pDymnVar->u32ParentObject = LE2ME32(*((U32*)(&pBuf[50])));

        if(gSpeed_Port2 == 0x02)    // HS
        {
            //memset( pDymnVar->u8FileName, 0x20, 8 );
            //memset( pDymnVar->u8ExtName,  0x20, 3 );
            pDymnVar->sDateTime.wDate = 0;
            pDymnVar->sDateTime.wTime = 0;
            //--------------------------
            // get the file name data
            //--------------------------
            str_len = pBuf[64];  // Filename string Length, include the NULL.
            if (str_len > 0)
                memcpy((U8*)pDymnVar->u16FileName, &pBuf[64+1], str_len * 2);

            //-------------------------------------------
            // get the date and time
            // and change the format to meet the FAT spec
            //-------------------------------------------
            str_idx = (str_len<<1)+1 ;    // Jump over the filename
            str_idx+=64;

            //------------------------------------------
            // get the established date: year month day
            //------------------------------------------
            // year - 1980
            pDymnVar->sDateTime.bDateTime[0]= pBuf[str_idx+1];
            pDymnVar->sDateTime.bDateTime[1]= pBuf[str_idx+3];
            pDymnVar->sDateTime.bDateTime[2]= pBuf[str_idx+5];
            pDymnVar->sDateTime.bDateTime[3]= pBuf[str_idx+7];
            pDymnVar->sDateTime.bDateTime[4]= 0;
            pDymnVar->sDateTime.wTemp = (U16) atoi(pDymnVar->sDateTime.bDateTime);
            pDymnVar->sDateTime.wTemp = pDymnVar->sDateTime.wTemp- 1980;
            pDymnVar->sDateTime.wDate = pDymnVar->sDateTime.wDate | (pDymnVar->sDateTime.wTemp << 9);
            // month
            pDymnVar->sDateTime.bDateTime[0]= pBuf[str_idx+9];
            pDymnVar->sDateTime.bDateTime[1]= pBuf[str_idx+11];
            pDymnVar->sDateTime.bDateTime[2]= 0;
            pDymnVar->sDateTime.wTemp = (U16) atoi(pDymnVar->sDateTime.bDateTime);
      //      printf("month:%02bx\n",pDymnVar->sDateTime.wTemp );
            pDymnVar->sDateTime.wDate = pDymnVar->sDateTime.wDate | (pDymnVar->sDateTime.wTemp <<5 );

            // date
            pDymnVar->sDateTime.bDateTime[0]= pBuf[str_idx+13];
            pDymnVar->sDateTime.bDateTime[1]= pBuf[str_idx+15];
            pDymnVar->sDateTime.bDateTime[2]= 0;

            pDymnVar->sDateTime.wTemp = (U16) atoi(pDymnVar->sDateTime.bDateTime);

            pDymnVar->sDateTime.wDate = pDymnVar->sDateTime.wDate | pDymnVar->sDateTime.wTemp;

            //----------------------------------------
            // High byte Low Byte change
            //----------------------------------------
         //   pDymnVar->sDateTime.wTemp = pDymnVar->sDateTime.wDate >> 8;
          //  pDymnVar->sDateTime.wDate = (pDymnVar->sDateTime.wDate<< 8) | pDymnVar->sDateTime.wTemp;
            //----------------------------------------
            // get the established time: hour min sec
            //----------------------------------------
            if( pBuf[str_idx+17] == 0x54 )  // 'T' ascii is 0x54
            {
                //------------------------------------
                // hour
                //------------------------------------
                pDymnVar->sDateTime.bDateTime[0]= pBuf[str_idx+19];
                pDymnVar->sDateTime.bDateTime[1]= pBuf[str_idx+21];
                pDymnVar->sDateTime.bDateTime[2]= 0;
                pDymnVar->sDateTime.wTemp = (U16) atoi(pDymnVar->sDateTime.bDateTime);
                pDymnVar->sDateTime.wTime = pDymnVar->sDateTime.wTime | ( pDymnVar->sDateTime.wTemp<<11 );
                //------------------------------------
                // min
                //------------------------------------
                pDymnVar->sDateTime.bDateTime[0]= pBuf[str_idx+23];
                pDymnVar->sDateTime.bDateTime[1]= pBuf[str_idx+25];
                pDymnVar->sDateTime.bDateTime[2]= 0;
                pDymnVar->sDateTime.wTemp = (U16) atoi(pDymnVar->sDateTime.bDateTime);
                pDymnVar->sDateTime.wTime = pDymnVar->sDateTime.wTime | ( pDymnVar->sDateTime.wTemp<<5 );
                //------------------------------------
                // sec
                //------------------------------------
                pDymnVar->sDateTime.bDateTime[0]= pBuf[str_idx+27];
                pDymnVar->sDateTime.bDateTime[1]= pBuf[str_idx+29];
                pDymnVar->sDateTime.bDateTime[2]= 0;
                pDymnVar->sDateTime.wTemp = (U16) atoi(pDymnVar->sDateTime.bDateTime);
                pDymnVar->sDateTime.wTime = pDymnVar->sDateTime.wTime | (pDymnVar->sDateTime.wTemp>>1);
                //----------------------------------------
                // High byte Low Byte chabge
                //----------------------------------------
               // pDymnVar->sDateTime.wTemp = pDymnVar->sDateTime.wTime >> 8;
               // pDymnVar->sDateTime.wTime = (pDymnVar->sDateTime.wTime<< 8) | pDymnVar->sDateTime.wTemp;
            }
        }
        break;

    //----------------------------------------------
    // Get the # of Handle and the start Handle ID
    //----------------------------------------------
    case PTP_GetObjectHandles:
        {
            U16 u16Idx;
            U32 u32Tmp;

            //pDymnVar->u32HandleNum  = 0;
            PTP_Driver_Port2.u32TotalHandleNo = ReadU32LE(&pBuf[12]);

            //PTP_Driver_Port2.u32CurHandleIdx = (PTP_Driver_Port2.u32FirstHandleNo);
            DBG_PTP(printf("Num=%lx ,Id=%lx \n",PTP_Driver_Port2.u32TotalHandleNo,PTP_Driver_Port2.u32FirstHandleNo));
            PTP_Driver_Port2.u32CurHandleIdx=0;
            for(u16Idx=16; u16Idx<u16MaxPacketSize; u16Idx+=4)
            {
                u32Tmp = ReadU32LE(&pBuf[u16Idx]);
                drvUSBHost_PTP_SaveObjectHandleNo_Port2(PTP_Driver_Port2.u32CurHandleIdx, u32Tmp);
                PTP_Driver_Port2.u32CurHandleIdx++;
            }
        break;
        }

    default:
        break;
    }

Func_Done:
    MDrv_Timer_Delayms(1);  // temp patch for in nak timeout issue
    #if (PLATFORM==MCU8051)
    //MDrv_MapXdataWindow1(u8Win1StarAddr, u8Win1EndAddr, u32Win1PhyAdr);
    #endif
} // PTP_Read1stBulkIn


/********************************************************************************
*
*   PROCEDURE NAME:
*   PTP_ReadRemBulkIn
*
*   DESCRIPTION:
*     read the remaining data if the data length is longer then MAX_BULK_IN_LEN
*
*********************************************************************************/
void drvUSBHost_PTP_ReadRemBulkIn_Port2(ASSIGNVAR xdata *pAssignVar,DYMNVAR xdata *pDymnVar )
{
    U8 loop_num;
    U8 str_len;
    U8 str_idx; //,u8Result;
    U16 u16MaxPacketSize;
    U32 u32RemLen;
    #if (PLATFORM==MCU8051)
    //U8 xdata *pBuf=0xF000;
    U8 xdata *pBuf=(U8 xdata *)(psAttachDevice_Port2->bDataBuffer);
    //U8 u8Win1StarAddr,u8Win1EndAddr;
    //U32 u32Win1PhyAdr;

    //MDrv_GetXdataWindow1Map(&u8Win1StarAddr, &u8Win1EndAddr, &u32Win1PhyAdr);
    //MDrv_MapXdataWindow1(60,64,(((BASEADR_SDRAM_USB_DEV_BUF)+0xFFF) & ~0xFFF));
    //MDrv_MapXdataWindow1(60,64,BASEADR_SDRAM_NAND_COPY_BUF);
    #else
    //U8 *pBuf=(((BASEADR_SDRAM_USB_DEV_BUF)+0xFFF) & ~0xFFF);
    #endif

    DBG_PTP(printf("GetRemBulkIn\r\n"));
    loop_num = 0;

    u16MaxPacketSize=psAttachDevice_Port2->saCD[0].sInterface[0].sED[0].bED_wMaxPacketSizeHighByte;
    u16MaxPacketSize=(u16MaxPacketSize<<8)+(psAttachDevice_Port2->saCD[0].sInterface[0].sED[0].bED_wMaxPacketSizeLowByte);

    if(pDymnVar->u32DataLen < u16MaxPacketSize)
    {
        DBG_PTP(printf("No Remain block\r\n"));
        goto Exit;
    }
    u32RemLen = pDymnVar->u32DataLen - u16MaxPacketSize;

    //-----------------------------
    // read the remaining raw data
    //-----------------------------

    //  while ( pDymnVar->u32DataLen >  MAX_BULK_IN_LEN )
    //----------------------------------------------------------------
    // Change this method to check if the u32DataLen > 64
    // for u32DataLen = 0x80 case the while loop will not be entered ??
    //----------------------------------------------------------------
    while ( u32RemLen )
    {
        DBG_PTP(printf("u32RemLen=0x%lX\r\n",u32RemLen));
        loop_num++;

        if ( u32RemLen < u16MaxPacketSize )
        {
            u16MaxPacketSize = u32RemLen;
        }

        pOTGH_PT_BLK_Port2->bStatus = flib_Host20_Issue_Bulk_Port2(pOTGH_PT_BLK_Port2->bInQHDArrayNum, u16MaxPacketSize,VirtoPhyAddr(psAttachDevice_Port2->bDataBuffer),OTGH_Dir_IN);
        if( pOTGH_PT_BLK_Port2->bStatus !=HOST20_OK)
        {
            //printf("R 2nd u8result=0x%bX\r\n");
            goto Exit;
        }

        if ( u32RemLen < u16MaxPacketSize )
        {
            u16MaxPacketSize = u32RemLen;
        }

        switch(pAssignVar->u16OpCode)
        {
        case PTP_GetObjectInfo:
            if( (gSpeed_Port2 == 0x00) && ( (loop_num ==1) ))
            {
                //memset( pDymnVar->u8FileName, 0, 8 );
                //memset( pDymnVar->u8ExtName,  0, 3 );
                pDymnVar->sDateTime.wDate = 0;
                pDymnVar->sDateTime.wTime = 0;
                //--------------------------
                // get the file name data
                //--------------------------
                str_len = pBuf[0];  // Filename string Length
                if ( str_len > 0 )
                    memcpy((U8*)pDymnVar->u16FileName, &pBuf[1], str_len * 2);

                //-------------------------------------------
                // get the date and time
                // and change the format to meet the FAT spec
                //-------------------------------------------
                str_idx = (str_len<<1)+1 ;
                str_len = pBuf[str_idx];  // time string length

                //------------------------------------------
                // get the established date: year month day
                //------------------------------------------
                // year - 1980
                pDymnVar->sDateTime.bDateTime[0]= pBuf[str_idx+1];
                pDymnVar->sDateTime.bDateTime[1]= pBuf[str_idx+3];
                pDymnVar->sDateTime.bDateTime[2]= pBuf[str_idx+5];
                pDymnVar->sDateTime.bDateTime[3]= pBuf[str_idx+7];
                pDymnVar->sDateTime.bDateTime[4]= 0;
                pDymnVar->sDateTime.wTemp = (WORD) atoi(pDymnVar->sDateTime.bDateTime);
                pDymnVar->sDateTime.wTemp = pDymnVar->sDateTime.wTemp- 1980;
                pDymnVar->sDateTime.wDate = pDymnVar->sDateTime.wDate | (pDymnVar->sDateTime.wTemp << 9);
                // month
                pDymnVar->sDateTime.bDateTime[0]= pBuf[str_idx+9];
                pDymnVar->sDateTime.bDateTime[1]= pBuf[str_idx+11];
                pDymnVar->sDateTime.bDateTime[2]= 0;
                pDymnVar->sDateTime.wTemp = (WORD) atoi(pDymnVar->sDateTime.bDateTime);
                pDymnVar->sDateTime.wDate = pDymnVar->sDateTime.wDate | (pDymnVar->sDateTime.wTemp <<5 );
                // date
                pDymnVar->sDateTime.bDateTime[0]= pBuf[str_idx+13];
                pDymnVar->sDateTime.bDateTime[1]= pBuf[str_idx+15];
                pDymnVar->sDateTime.bDateTime[2]= 0;
                pDymnVar->sDateTime.wTemp = (WORD) atoi(pDymnVar->sDateTime.bDateTime);
                pDymnVar->sDateTime.wDate = pDymnVar->sDateTime.wDate | pDymnVar->sDateTime.wTemp;
                //----------------------------------------
                // High byte Low Byte chabge
                //----------------------------------------
              //  pDymnVar->sDateTime.wTemp = pDymnVar->sDateTime.wDate >> 8;
              //  pDymnVar->sDateTime.wDate = (pDymnVar->sDateTime.wDate<< 8) | pDymnVar->sDateTime.wTemp;

                //----------------------------------------
                // get the established time: hour min sec
                //----------------------------------------
                if( pBuf[str_idx+17] == 0x54 )  // 'T' ascii is 0x54
                {
                    //------------------------------------
                    // hour
                    //------------------------------------
                    pDymnVar->sDateTime.bDateTime[0]= pBuf[str_idx+19];
                    pDymnVar->sDateTime.bDateTime[1]= pBuf[str_idx+21];
                    pDymnVar->sDateTime.bDateTime[2]= 0;
                    pDymnVar->sDateTime.wTemp = (WORD) atoi(pDymnVar->sDateTime.bDateTime);
                    pDymnVar->sDateTime.wTime = pDymnVar->sDateTime.wTime | ( pDymnVar->sDateTime.wTemp<<11 );
                    //------------------------------------
                    // min
                    //------------------------------------
                    pDymnVar->sDateTime.bDateTime[0]= pBuf[str_idx+23];
                    pDymnVar->sDateTime.bDateTime[1]= pBuf[str_idx+25];
                    pDymnVar->sDateTime.bDateTime[2]= 0;
                    pDymnVar->sDateTime.wTemp = (WORD) atoi(pDymnVar->sDateTime.bDateTime);
                    pDymnVar->sDateTime.wTime = pDymnVar->sDateTime.wTime | ( pDymnVar->sDateTime.wTemp<<5 );
                    //------------------------------------
                    // sec
                    //------------------------------------
                    pDymnVar->sDateTime.bDateTime[0]= pBuf[str_idx+27];
                    pDymnVar->sDateTime.bDateTime[1]= pBuf[str_idx+29];
                    pDymnVar->sDateTime.bDateTime[2]= 0;
                    pDymnVar->sDateTime.wTemp = (WORD) atoi(pDymnVar->sDateTime.bDateTime);
                    pDymnVar->sDateTime.wTime = pDymnVar->sDateTime.wTime | (pDymnVar->sDateTime.wTemp>>1);
                    //----------------------------------------
                    // High byte Low Byte chabge
                    //----------------------------------------
                 //   pDymnVar->sDateTime.wTemp = pDymnVar->sDateTime.wTime >> 8;
                  //  pDymnVar->sDateTime.wTime = (pDymnVar->sDateTime.wTime<< 8) | pDymnVar->sDateTime.wTemp;
                }
            }
            break;
        case PTP_GetObjectHandles:
            {
                U16 u16Idx;
                U32 u32Tmp;

                for(u16Idx=0; u16Idx<u16MaxPacketSize; u16Idx+=4)
                {
                    if(u16Idx>u32RemLen)
                        break;

                    u32Tmp = ReadU32LE(&pBuf[u16Idx]);

                    drvUSBHost_PTP_SaveObjectHandleNo_Port2(PTP_Driver_Port2.u32CurHandleIdx,u32Tmp);
                    PTP_Driver_Port2.u32CurHandleIdx++;
                }
            }
            break;
        }
        u32RemLen -= u16MaxPacketSize;
    }
Exit:
    DBG_PTP(printf("GetRemBulkIn End\r\n"));
    #if (PLATFORM==MCU8051)
    //MDrv_MapXdataWindow1(u8Win1StarAddr, u8Win1EndAddr, u32Win1PhyAdr);
    #endif
}

/*********************************************************************
*
*   PROCEDURE NAME:
*   drvUSBHost_PTP_Read1stBulkIn
*
*   DESCRIPTION:
*     Read the first Bulk data 64 bytes a time
*
*********************************************************************/
U32 gTotalBytes_Port2;
extern U16 TotalBytes_Port2;
void drvUSBHost_PTP_GetBulkIn_Port2(DYMNVAR xdata *pDymnVar)
{
    //U8 u8result;
    U16 TransferLengh;
    U32 wTotalLengthRemain,Offset;

    DBG_PTP(printf("drvUSBHost_PTP_GetBulkIn\r\n"));
    wTotalLengthRemain=pDymnVar->u32DataLen;
    Offset=0;
    gTotalBytes_Port2=0;
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

        pOTGH_PT_BLK_Port2->bStatus = flib_Host20_Issue_Bulk_Port2(pOTGH_PT_BLK_Port2->bInQHDArrayNum, TransferLengh,
                                                            pDymnVar->u32DramAddr+Offset,OTGH_Dir_IN);
        if( pOTGH_PT_BLK_Port2->bStatus!=HOST20_OK)
        {
            DBG_PTP(printf("GetBulkIn u8result=0x%bX\r\n"));
            goto Func_Done;
        }

        Offset+=TransferLengh-TotalBytes_Port2;       // TotalBytes_Port2 is remaining bytes return from flib_Host20_Issue_Bulk
        wTotalLengthRemain-=TransferLengh-TotalBytes_Port2;
        //   printf("oNE TIME TransferLengh:%x\n",TransferLengh-TotalBytes_Port2);
        gTotalBytes_Port2+=TransferLengh-TotalBytes_Port2;
        if (TotalBytes_Port2) break;          //short packet end
    }

Func_Done:
    MDrv_Timer_Delayms(1);  // temp patch for in nak timeout issue
}

/*********************************************************************
*
*   PROCEDURE NAME:
*   drvUSBHost_PTP_BulkInResponse
*
*   DESCRIPTION:
*   Get the Response 12 bytes data
*
*   NOTES:
*   We can check if the response is O.K. in every reponse
*********************************************************************/
U8 drvUSBHost_PTP_BulkInResponse_Port2(void)
{
    U8 rtn_val,u8result;
    U32 u32BufAddr;

    //DBG_PTP(printf("BulkInResponse\r\n"));
    rtn_val     = PTP_OK;
    u32BufAddr = VirtoPhyAddr(psAttachDevice_Port2->bDataBuffer);
   // printf("resbuf:%lx\n",u32BufAddr);
    u8result = flib_Host20_Issue_Bulk_Port2 (pOTGH_PT_BLK_Port2->bInQHDArrayNum,12,u32BufAddr,OTGH_Dir_IN);
    //DBG_PTP(printf("result=0x%bX\r\n",u8result));
    //------------------------------------------
    // Check the status here
    // 0c 00 00 00 - 03 00 01 20 - transactio Id
    //------------------------------------------
    /*
    DBG_PTP(printf("RSP:\r\n"));
    for (i=0; i<12; i++ )
    {
        DBG_PTP(printf("%bx, ",((U8 xdata*)psAttachDevice_Port2->bDataBuffer)[i]));
    }
    DBG_PTP(printf(("\r\n")));
    */
    if( (((U8 xdata*)psAttachDevice_Port2->bDataBuffer)[0] != 0x0c) || (((U8 xdata*)psAttachDevice_Port2->bDataBuffer)[6] != 0x01) || ( ((U8 xdata*)psAttachDevice_Port2->bDataBuffer)[7] != 0x20 ) )
    {
        rtn_val     = PTP_RESP_FAIL;
    }

//    if ( (u8result!=HOST20_OK) || (rtn_val == PTP_RESP_FAIL) )
//        drvUSBHost_PTP_Error_Handle();

    return rtn_val;


} // PTP_BulkInResponse


U8 drvUSBHost_PTP_Initial_Port2(void)
{
    DBG_PTP(printf("drvUSBHost_PTP_Initial\r\n"));
    OTGH_PT_Bulk_Init_Port2();

    memset( (void*)(&PTP_Driver_Port2), 0, sizeof(PTP_Driver_Port2) );

    PTP_Driver_Port2.u32ObjectHandleNumBuf = drvUSBHost_PTP_GetPtpObjHandlesBufAdr_Port2();
    DBG_PTP(printf("Handle Buf Addr:%lx\n", PTP_Driver_Port2.u32ObjectHandleNumBuf ));

    if(gSpeed_Port2 == 0x02)    // HS
    {
        DBG_PTP(printf("HS PTP\r\n"));
    }
    else
    {
        DBG_PTP(printf("FS PTP\r\n"));
    }

    xDymnVar_Port2.u16FileType  = 0;
    xDymnVar_Port2.u32DataLen   = 0;
    xDymnVar_Port2.u32StorageId = 0;
    //xDymnVar_Port2.u32HandleNum = 0;
    xDymnVar_Port2.u32HandleId  = 0;
    MDrv_Timer_Delayms(100);    // Delay for DSC ready
    xAssignVar_Port2.u16OpCode = PTP_OpenSession;
    xAssignVar_Port2.u32TransId = 0x00;
    drvUSBHost_PTP_Transaction_Port2(&xAssignVar_Port2, &xDymnVar_Port2, TRUE);

    xAssignVar_Port2.u16OpCode = PTP_GetDeviceInfo;
    xAssignVar_Port2.u32TransId++;
    drvUSBHost_PTP_Transaction_Port2(&xAssignVar_Port2, &xDymnVar_Port2, TRUE);

    xAssignVar_Port2.u16OpCode = PTP_GetStorageIDs;
    xAssignVar_Port2.u32TransId++;
    drvUSBHost_PTP_Transaction_Port2(&xAssignVar_Port2, &xDymnVar_Port2, TRUE);

    xAssignVar_Port2.u16OpCode = PTP_GetStorageInfo;
    xAssignVar_Port2.u32TransId++;
    drvUSBHost_PTP_Transaction_Port2(&xAssignVar_Port2, &xDymnVar_Port2, TRUE);

#if 0
    //-------------------------------------
    // Get Handle number and the start ID
    //-------------------------------------
    xAssignVar_Port2.u16OpCode = PTP_GetObjectHandles;
    xAssignVar_Port2.u32TransId++;
    xAssignVar_Port2.u32Param1 = 0xFFFFFFFF;
    xAssignVar_Port2.u32Param2 = 0;
    xAssignVar_Port2.u32Param3 = 0;
    drvUSBHost_PTP_Transaction (&xAssignVar_Port2, &xDymnVar_Port2);
    DBG_PTP(printf("\r\n\r\nHandleNo=0x%lX\r\n",PTP_Driver_Port2.u32TotalHandleNo));
    printf("\r\n\r\nHandleNo=0x%lX\r\n",PTP_Driver_Port2.u32TotalHandleNo);
#endif
#if 0
    {
        U32                     ObjCount, ii, ObjHandle, GetOffset, transLen, reallen;
        PTP_OBJECTINFO     *pObjInfo;
        U8                      jj;
        U32     pBuf4GetTest;
        U8 xdata *pTestBuf = (U8 xdata*)0xF000;


        ObjCount = drvUsbPTP_GetObjectHandles(USB_PORT_2, 0, 0);

        printf("ObjCount: %ld\n", ObjCount);
        for (ii=0; ii<ObjCount; ii++)
        {
            ObjHandle = drvUsbPTP_GetObjectHandleByIndex(USB_PORT_2, ii);
            pObjInfo = drvUsbPTP_GetObjectInfo(USB_PORT_2, ObjHandle);

            printf("ObjHandle: %lX\n", ObjHandle);
            printf("ObjectFormat: %X\n", pObjInfo->ObjectFormat);
            printf("ObjectCompressedSize: %ld\n", pObjInfo->ObjectCompressedSize);
            printf("ThumbFormat: %X\n", pObjInfo->ThumbFormat);
            printf("ThumbCompressedSize: %ld\n", pObjInfo->ThumbCompressedSize);
            printf("ThumbPixWidth: %ld\n", pObjInfo->ThumbPixWidth);
            printf("ThumbPixHeight: %ld\n", pObjInfo->ThumbPixHeight);
            printf("ImagePixWidth: %ld\n", pObjInfo->ImagePixWidth);
            printf("ImagePixHeight: %ld\n", pObjInfo->ImagePixHeight);
            printf("ParentObject: %ld\n", pObjInfo->ParentObject);
            printf("Filename: ");
            for (jj=0; pObjInfo->Filename[jj] != 0; jj++)
                printf("%c", (U8) (LE2ME16(pObjInfo->Filename[jj])));
            printf("\n\n");

            if (pObjInfo->ObjectFormat == OBJ_JPEG)
            {
                pBuf4GetTest = PTP_Driver_Port2.u32ObjectHandleNumBuf + 32768;

                GetOffset = 0;
                while (GetOffset < pObjInfo->ThumbCompressedSize)
                {
                    if ((pObjInfo->ThumbCompressedSize - GetOffset) > 16*1024)
                        transLen = 16 * 1024;
                    else
                        transLen = pObjInfo->ThumbCompressedSize - GetOffset;

                    printf("GetOffset: %ld, transLen: %ld\n", GetOffset, transLen);
                    reallen = drvUsbPTP_GetThumb(USB_PORT_2, ObjHandle, GetOffset, transLen, pBuf4GetTest);
                    printf("reallen: %ld\n", reallen);

                    GetOffset += transLen;
                }

                GetOffset = 0;
                while (GetOffset < pObjInfo->ObjectCompressedSize)
                {
                    if ((pObjInfo->ObjectCompressedSize - GetOffset) > 16*1024)
                        transLen = 16 * 1024;
                    else
                        transLen = pObjInfo->ObjectCompressedSize - GetOffset;

                    printf("GetOffset: %ld, transLen: %ld\n", GetOffset, transLen);
                    reallen = drvUsbPTP_GetObject(USB_PORT_2, ObjHandle, GetOffset, transLen, pBuf4GetTest);
                    printf("reallen: %ld\n", reallen);
#if 0
                    drvUSBHost_PTP_SetObjHWindow1(pBuf4GetTest);
                    for (jj=0; jj<0x1000; jj++)
                    {
                        if (jj % 16 == 0)
                            printf("\n %08lx:", jj);

                        printf(" %02bX", pTestBuf[jj]);
                    }
                    printf("\n");
                    drvUSBHost_PTP_FreeObjHWindow1();
#endif
                    GetOffset += transLen;
                }
            }
        }
    }
#endif

#if 0
    idx=0;
    idy=0;
     while(idx < (PTP_Driver_Port2.u32TotalHandleNo))
    {
        DBG_PTP(printf(" idx=0x%lX\r\n",idx));
        u32Tmp = drvUSBHost_PTP_GetObjectHandleNo(idx);
      //  printf("Handle is :%lx\n",u32Tmp);
        if(u32Tmp==INVALID_HANDLE_NO)
            return(NULL);
        pdymnvar = drvUSBHost_PTP_GetObjectInfo(u32Tmp);
        if(pdymnvar == NULL)
        {
            DBG_PTP(printf("GetFirstFileObjectInfo NULL\r\n"));
            return(NULL);
        }
        if(pdymnvar->u16FileType != OBJ_ASSOCIATION)
        {
           drvUSBHost_PTP_SaveObjectHandleNo(idy,u32Tmp);
           idy++;
        }
        else
            printf("find dir %lx\n",u32Tmp);

         idx++;

        }
     PTP_Driver_Port2.u32TotalHandleNo=idy;
     printf("after fitering size:%lx\n",idy);
 #endif
    #if 0
    for( i=0 ; i < xDymnVar_Port2.u32HandleNum  ; i++ )
    {
        DBG_PTP(printf("i=%lx, Id=%lx\n",i,xDymnVar_Port2.u32HandleId ));
        //--------------------------------
        // Get the File Info
        //--------------------------------
        xAssignVar_Port2.u16OpCode = PTP_GetObjectInfo;
        xAssignVar_Port2.u32TransId++;
        xDymnVar_Port2.u16FileType = UnKnownFile;
        drvUSBHost_PTP_Transaction (&xAssignVar_Port2, &xDymnVar_Port2);
        DBG_PTP(printf("FileNand:"));
        for(j=0;j<8;j++)
        {
            if(xDymnVar_Port2.u8FileName[j] == NULL)
                DBG_PTP(printf(" "));
            else
                DBG_PTP(printf("%C",xDymnVar_Port2.u8FileName[j]));
        }
        DBG_PTP(printf("."));
        for(j=0;j<3;j++)
        {
            if(xDymnVar_Port2.u8ExtName[j] == NULL)
                DBG_PTP(printf(" "));
            else
                DBG_PTP(printf("%C",xDymnVar_Port2.u8ExtName[j]));
        }
        DBG_PTP(printf("\r\n"));
        //--------------------------------
        // Check the file type
        //--------------------------------
        if( xDymnVar_Port2.u16FileType == JpegFile )
        {
            MDrv_GetXdataWindow1Map(&u8Win1StarAddr, &u8Win1EndAddr, &u32Win1PhyAdr);
            MDrv_MapXdataWindow1(60,64,(((BASEADR_SDRAM_USB_DEV_BUF)+0xFFF) & ~0xFFF)+PTP_Driver_Port2.u16ValidHandleNo*32);
            memset(pDummyDirEntry,0x00,0x20);
            PTP_Driver_Port2.u16ValidHandleNo++;
            memcpy(pDummyDirEntry->ps8DirEntryName,xDymnVar_Port2.u8FileName,8);
            memcpy(pDummyDirEntry->ps8DirEntryExtName,xDymnVar_Port2.u8ExtName,3);
            pDummyDirEntry->u32DirEntryFileLength = CPU2Le32(xDymnVar_Port2.u32FileLength);
            pDummyDirEntry->u8DirEntryAttrib = ATTRIB_ARCHIVE;
            pDummyDirEntry->u16DirEntryStartClusterHI = 0x0000;
            pDummyDirEntry->u16DirEntryStartClusterLO = 0x0000;
            pDummyDirEntry->u16DirEntryTime = xDateTime.wTime;
            pDummyDirEntry->u16DirEntryDate = xDateTime.wDate;

            for(j=0;j<0x20;j++)
            {
                DBG_PTP(printf("%2bX ",((U8 *)pDummyDirEntry)[j]));
            }
            pDummyDirEntry++;
            #if (PLATFORM==MCU8051)
            MDrv_MapXdataWindow1(u8Win1StarAddr, u8Win1EndAddr, u32Win1PhyAdr);
            #endif
        }
        //{
        //  xAssignVar_Port2.u16OpCode  = GetObject;
        //  xAssignVar_Port2.u32TransId++;
        //  PTP_Transaction (&xAssignVar_Port2, &xDymnVar_Port2 );
        //}
        xDymnVar_Port2.u32HandleId++;
    }
    #endif
    //xAssignVar_Port2.u16OpCode = PTP_CloseSession;
    //xAssignVar_Port2.u32TransId ++;
    //drvUSBHost_PTP_Transaction(&xAssignVar_Port2, &xDymnVar_Port2);
    return(PTP_OK);
}

#if 0
DYMNVAR *drvUSBHost_PTP_GetCurObjectInfo(void)
{
  U32 u32Tmp = drvUSBHost_PTP_GetObjectHandleNo(PTP_Driver_Port2.u32CurHandleIdx);
    DBG_PTP(printf("drvUSBHost_PTP_GetCurObjectInfo:%bX\r\n",PTP_Driver_Port2.u32CurHandleIdx));
    if(u32Tmp==INVALID_HANDLE_NO)
        return(NULL);
    return(drvUSBHost_PTP_GetObjectInfo(u32Tmp));
}
#endif

DYMNVAR *drvUSBHost_PTP_GetObjectInfo_Port2(U32 u32HandleNo )
{
    U8     u8Status;

    DBG_PTP(printf("drvUSBHost_PTP_GetObjectInfo\r\n"));
    //printf("u32HandleNo:%lx\n",u32HandleNo);
    //printf("u32FirstHandleNo:%lx\n",PTP_Driver_Port2.u32FirstHandleNo);

    xAssignVar_Port2.u16OpCode = PTP_GetObjectInfo;
    xAssignVar_Port2.u32TransId++;
    xDymnVar_Port2.u16FileType = 0;
    xDymnVar_Port2.u32HandleId = u32HandleNo;

    #ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_SetXdataWindow1_Port2();             //Set USB window 1
    #endif

    drvUSBHost_PTP_Transaction_Port2 (&xAssignVar_Port2, &xDymnVar_Port2, TRUE);
    u8Status = pOTGH_PT_BLK_Port2->bStatus;

    #ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1_Port2();     //Release USB window 1
    #endif

    if (u8Status != HOST20_OK)
        return NULL;
    else
        return(&xDymnVar_Port2);
}
extern U8 Hex2Ascii(U8 x);
#if 0
DYMNVAR *drvUSBHost_PTP_GetFirstFileObjectInfo(void)
{
    DYMNVAR *pdymnvar;
    U32 u32Tmp;
    DBG_PTP(printf("drvUSBHost_PTP_GetFirstFileObjectInfo\r\n"));
    PTP_Driver_Port2.u32CurHandleIdx = 0;
    DBG_PTP(printf("u32FirstHandleNo=0x%lX\r\n",PTP_Driver_Port2.u32FirstHandleNo));
    DBG_PTP(printf("u32TotalHandleNo=0x%lX\r\n",PTP_Driver_Port2.u32TotalHandleNo));

    while(PTP_Driver_Port2.u32CurHandleIdx < (PTP_Driver_Port2.u32TotalHandleNo-1))
    {
        DBG_PTP(printf("u32CurHandleIdx=0x%lX\r\n",PTP_Driver_Port2.u32CurHandleIdx));
        u32Tmp = drvUSBHost_PTP_GetObjectHandleNo(PTP_Driver_Port2.u32CurHandleIdx);
      //  printf("Handle is :%lx\n",u32Tmp);
        if(u32Tmp==INVALID_HANDLE_NO)
            return(NULL);
        pdymnvar = drvUSBHost_PTP_GetObjectInfo(u32Tmp);
        if(pdymnvar == NULL)
        {
            DBG_PTP(printf("GetFirstFileObjectInfo NULL\r\n"));
            return(NULL);
        }
        if(pdymnvar->u16FileType != OBJ_ASSOCIATION)
        {
            PTP_Driver_Port2.u32FirstFileHandleNo = drvUSBHost_PTP_GetObjectHandleNo(PTP_Driver_Port2.u32CurHandleIdx);
            DBG_PTP(printf("u32CurHandleIdx=0x%lX\r\n",PTP_Driver_Port2.u32CurHandleIdx));
    //        sprintf(pdymnvar->u8FileName,"PTP");
            u32Tmp = PTP_Driver_Port2.u32FirstFileHandleNo;
            //pdymnvar->u8FileName[7] = (u32Tmp%10) + '0';
          //  u32Tmp /= 10;
           // pdymnvar->u8FileName[6] = (u32Tmp%10) + '0';
            //u32Tmp /= 10;
           // pdymnvar->u8FileName[5] = (u32Tmp%10) + '0';
           // u32Tmp /= 10;
          //  pdymnvar->u8FileName[4] = (u32Tmp%10) + '0';
          //  u32Tmp /= 10;
          //  pdymnvar->u8FileName[3] = (u32Tmp%10) + '0';
                pdymnvar->u8FileName[7] = Hex2Ascii(u32Tmp&0xf);
                pdymnvar->u8FileName[6] = Hex2Ascii((u32Tmp&0xf0)>>4) ;
                pdymnvar->u8FileName[5] = Hex2Ascii((u32Tmp&0xf00)>>8) ;
                pdymnvar->u8FileName[4] = Hex2Ascii((u32Tmp&0xf000)>>12) ;
                pdymnvar->u8FileName[3] = Hex2Ascii((u32Tmp&0xf0000)>>16) ;
                pdymnvar->u8FileName[2] = Hex2Ascii((u32Tmp&0xf00000)>>20) ;
                pdymnvar->u8FileName[1] = Hex2Ascii((u32Tmp&0xf000000)>>24) ;
                pdymnvar->u8FileName[0] = Hex2Ascii((u32Tmp&0xf0000000)>>28) ;

            return(pdymnvar);
        }
        PTP_Driver_Port2.u32CurHandleIdx++;
    }
    return(NULL);
}
DYMNVAR *drvUSBHost_PTP_GetOffsetFileObjectInfo(U32 offset)
{
        DYMNVAR *pdymnvar;
        U32 u32Tmp;

        if ((PTP_Driver_Port2.u32CurHandleIdx+offset) > (PTP_Driver_Port2.u32TotalHandleNo-1)) return NULL;
        PTP_Driver_Port2.u32CurHandleIdx+=offset;
       // printf("curidx:%lx\n",PTP_Driver_Port2.u32CurHandleIdx);
        u32Tmp = drvUSBHost_PTP_GetObjectHandleNo(PTP_Driver_Port2.u32CurHandleIdx);
        if(u32Tmp==INVALID_HANDLE_NO)
            return(NULL);
        pdymnvar = drvUSBHost_PTP_GetObjectInfo(u32Tmp);
        if(pdymnvar == NULL)
        {
            return(NULL);
        }

        return(pdymnvar);


}
DYMNVAR *drvUSBHost_PTP_GetNextFileObjectInfo(void)
{
    DYMNVAR *pdymnvar;
    U32 u32Tmp;

    //printf("<<GetNxtF-OBJ\r\n");
    while(PTP_Driver_Port2.u32CurHandleIdx < (PTP_Driver_Port2.u32TotalHandleNo-1))
    {
        PTP_Driver_Port2.u32CurHandleIdx++;
        ///printf("G_N_O:%lX\r\n",PTP_Driver_Port2.u32CurHandleIdx);
        u32Tmp = drvUSBHost_PTP_GetObjectHandleNo(PTP_Driver_Port2.u32CurHandleIdx);
        if(u32Tmp==INVALID_HANDLE_NO)
            return(NULL);
        pdymnvar = drvUSBHost_PTP_GetObjectInfo(u32Tmp);
        if(pdymnvar == NULL)
        {
            return(NULL);
        }
        if(pdymnvar->u16FileType != OBJ_ASSOCIATION)
        {
           // sprintf(pdymnvar->u8FileName,"PTP");
           // u32Tmp = PTP_Driver_Port2.u32CurHandleIdx;
           // pdymnvar->u8FileName[7] = (u32Tmp%10) + '0';
           // u32Tmp /= 10;
            //pdymnvar->u8FileName[6] = (u32Tmp%10) + '0';
          //  u32Tmp /= 10;
           // pdymnvar->u8FileName[5] = (u32Tmp%10) + '0';
           // u32Tmp /= 10;
            //pdymnvar->u8FileName[4] = (u32Tmp%10) + '0';
            //u32Tmp /= 10;
            //pdymnvar->u8FileName[3] = (u32Tmp%10) + '0';
            return(pdymnvar);
        }
        DBG_PTP(printf("u32CurHandleIdx=0x%lX\r\n",PTP_Driver_Port2.u32CurHandleIdx));
    }
    return(NULL);
}

DYMNVAR *drvUSBHost_PTP_GetPrevFileObjectInfo(void)
{
    DYMNVAR *pdymnvar;
    U32 u32Tmp;

    printf("<<GetNxtF-OBJ\r\n");
    while(PTP_Driver_Port2.u32CurHandleIdx)
    {
        PTP_Driver_Port2.u32CurHandleIdx--;
        u32Tmp = drvUSBHost_PTP_GetObjectHandleNo(PTP_Driver_Port2.u32CurHandleIdx);
        if(u32Tmp==INVALID_HANDLE_NO)
    {
            return(NULL);
        }
        pdymnvar = drvUSBHost_PTP_GetObjectInfo(u32Tmp);
        if(pdymnvar == NULL)
        {
            return(NULL);
        }
        if(pdymnvar->u16FileType != OBJ_ASSOCIATION)
        {
            sprintf(pdymnvar->u8FileName,"PTP");
            u32Tmp = PTP_Driver_Port2.u32CurHandleIdx;
            pdymnvar->u8FileName[7] = (u32Tmp%10) + '0';
            u32Tmp /= 10;
            pdymnvar->u8FileName[6] = (u32Tmp%10) + '0';
            u32Tmp /= 10;
            pdymnvar->u8FileName[5] = (u32Tmp%10) + '0';
            u32Tmp /= 10;
            pdymnvar->u8FileName[4] = (u32Tmp%10) + '0';
            u32Tmp /= 10;
            pdymnvar->u8FileName[3] = (u32Tmp%10) + '0';
            return(pdymnvar);
        }
        DBG_PTP(printf("u32CurHandleIdx=0x%lX\r\n",PTP_Driver_Port2.u32CurHandleIdx));
    }
    return(NULL);
}
#endif

static U32  g32PTPFilePtr;
extern U32  gTotalBytes_Port2;
U8   StartGetObject_Port2=0;

U32 drvUSBHost_PTP_GetObjectData_Port2(
    U16 u16OpCode,
    U32 u32TotalSize,
    U32 u32HandleIdx,
    U32 u32FileOffset,
    U32 u32Length,
    U32 u32DRAMAddr)
{
    U8 val;
    U32 uBufOffset=0, uTransLen;
    U16 wMaxPacketSize;

    #ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_SetXdataWindow1_Port2();             //Set USB window 1
    #endif
    wMaxPacketSize=psAttachDevice_Port2->saCD[0].sInterface[0].sED[0].bED_wMaxPacketSizeHighByte;
    wMaxPacketSize=(wMaxPacketSize<<8)+(psAttachDevice_Port2->saCD[0].sInterface[0].sED[0].bED_wMaxPacketSizeLowByte);

    if (u32FileOffset==0)
    {
        StartGetObject_Port2=1;
        g32PTPFilePtr=0;

        if (u32Length > wMaxPacketSize)
            uTransLen = wMaxPacketSize;
        else
            uTransLen = u32Length;

        xAssignVar_Port2.u16OpCode = u16OpCode;
        xAssignVar_Port2.u32TransId++;
        xDymnVar_Port2.u32FileOffset = 0;
        xDymnVar_Port2.u32DataLen = uTransLen;
        xDymnVar_Port2.u32HandleId = u32HandleIdx;
        xDymnVar_Port2.u32DramAddr = VirtoPhyAddr(psAttachDevice_Port2->bDataBuffer);
        drvUSBHost_PTP_Transaction_Port2(&xAssignVar_Port2, &xDymnVar_Port2, FALSE);
        if (pOTGH_PT_BLK_Port2->bStatus!=HOST20_OK)
            goto Func_Done;

        MDrv_MIU_Copy(VirtoPhyAddr(psAttachDevice_Port2->bDataBuffer) + PTP_GEN_CONTAIN_SIZE,
                u32DRAMAddr, gTotalBytes_Port2 - PTP_GEN_CONTAIN_SIZE, MIU_SDRAM2SDRAM);

        g32PTPFilePtr += gTotalBytes_Port2 - PTP_GEN_CONTAIN_SIZE;
        //printf("1. g32PTPFilePtr: %ld\n", g32PTPFilePtr);

        if (g32PTPFilePtr >= u32TotalSize)
            goto Get_Done;

        if ( (u32Length - g32PTPFilePtr) > wMaxPacketSize )
        {
            xAssignVar_Port2.u16OpCode = u16OpCode;
            xAssignVar_Port2.u32TransId++;
            xDymnVar_Port2.u32DataLen = (u32Length - g32PTPFilePtr) & ~((U32)wMaxPacketSize-1);
            xDymnVar_Port2.u32HandleId = u32HandleIdx;
            xDymnVar_Port2.u32DramAddr = u32DRAMAddr + g32PTPFilePtr;
            drvUSBHost_PTP_BulkInData_Port2 ( &xAssignVar_Port2, &xDymnVar_Port2);
            if (pOTGH_PT_BLK_Port2->bStatus!=HOST20_OK)
                goto Func_Done;

            g32PTPFilePtr += gTotalBytes_Port2;
            //printf("2. g32PTPFilePtr: %ld\n", g32PTPFilePtr);
        }

        if ( (u32Length - g32PTPFilePtr) > 0 )
        {
            if ((u32TotalSize - g32PTPFilePtr) > wMaxPacketSize)
                uTransLen = wMaxPacketSize;
            else
                uTransLen = u32Length - g32PTPFilePtr;

            xAssignVar_Port2.u16OpCode = u16OpCode;
            xAssignVar_Port2.u32TransId++;
            xDymnVar_Port2.u32DataLen = uTransLen;
            xDymnVar_Port2.u32HandleId = u32HandleIdx;
            xDymnVar_Port2.u32DramAddr = VirtoPhyAddr(psAttachDevice_Port2->bDataBuffer);
            drvUSBHost_PTP_BulkInData_Port2 ( &xAssignVar_Port2, &xDymnVar_Port2);
            if (pOTGH_PT_BLK_Port2->bStatus!=HOST20_OK)
                goto Func_Done;

            if (uTransLen == (u32Length - g32PTPFilePtr))
            {
                MDrv_MIU_Copy(VirtoPhyAddr(psAttachDevice_Port2->bDataBuffer), u32DRAMAddr + g32PTPFilePtr,
                        uTransLen, MIU_SDRAM2SDRAM);
                g32PTPFilePtr += gTotalBytes_Port2;
            }
            else
            {
                MDrv_MIU_Copy(VirtoPhyAddr(psAttachDevice_Port2->bDataBuffer), u32DRAMAddr + g32PTPFilePtr,
                        PTP_GEN_CONTAIN_SIZE, MIU_SDRAM2SDRAM);
                g32PTPFilePtr += (gTotalBytes_Port2 > PTP_GEN_CONTAIN_SIZE) ? PTP_GEN_CONTAIN_SIZE : gTotalBytes_Port2;
            }

            //printf("3. g32PTPFilePtr: %ld\n", g32PTPFilePtr);
        }

        uBufOffset = g32PTPFilePtr;
    }
    else if (u32FileOffset == g32PTPFilePtr)
    {
        // First, copy data from last packet
        if (u32Length < (gTotalBytes_Port2 -PTP_GEN_CONTAIN_SIZE) )
            uTransLen = u32Length;
        else
            uTransLen = gTotalBytes_Port2 -PTP_GEN_CONTAIN_SIZE;

        MDrv_MIU_Copy(VirtoPhyAddr(psAttachDevice_Port2->bDataBuffer) + PTP_GEN_CONTAIN_SIZE, u32DRAMAddr,
                uTransLen, MIU_SDRAM2SDRAM);

        g32PTPFilePtr += uTransLen;
        uBufOffset += uTransLen;
        //printf("4. g32PTPFilePtr: %ld\n", g32PTPFilePtr);

        if ( ((u32Length - uBufOffset) > 0) && (g32PTPFilePtr < u32TotalSize) )
        {
            if ( (u32Length - uBufOffset) > wMaxPacketSize)
            {
                uTransLen = (u32Length - uBufOffset) & ~((U32)wMaxPacketSize-1);

                xAssignVar_Port2.u16OpCode = u16OpCode;
                xAssignVar_Port2.u32TransId++;
                xDymnVar_Port2.u32DataLen = uTransLen;
                xDymnVar_Port2.u32HandleId = u32HandleIdx;
                xDymnVar_Port2.u32DramAddr = u32DRAMAddr + uBufOffset;
                drvUSBHost_PTP_BulkInData_Port2 ( &xAssignVar_Port2, &xDymnVar_Port2);
                if (pOTGH_PT_BLK_Port2->bStatus!=HOST20_OK)
                    goto Func_Done;

                g32PTPFilePtr += gTotalBytes_Port2;
                uBufOffset += gTotalBytes_Port2;
                //printf("5. g32PTPFilePtr: %ld\n", g32PTPFilePtr);
            }

            if ( ((u32Length - uBufOffset) > 0) && (g32PTPFilePtr < u32TotalSize) )
            {
                if ( (u32Length - uBufOffset) == (u32TotalSize - g32PTPFilePtr) )
                    uTransLen = u32Length - uBufOffset;         // Last data
                else
                    uTransLen = wMaxPacketSize;

                xAssignVar_Port2.u16OpCode = u16OpCode;
                xAssignVar_Port2.u32TransId++;
                xDymnVar_Port2.u32DataLen = uTransLen;
                xDymnVar_Port2.u32HandleId = u32HandleIdx;
                xDymnVar_Port2.u32DramAddr = VirtoPhyAddr(psAttachDevice_Port2->bDataBuffer);
                drvUSBHost_PTP_BulkInData_Port2 ( &xAssignVar_Port2, &xDymnVar_Port2);
                if (pOTGH_PT_BLK_Port2->bStatus!=HOST20_OK)
                    goto Func_Done;

                if ( uTransLen == (u32Length - uBufOffset) )
                {
                    MDrv_MIU_Copy(VirtoPhyAddr(psAttachDevice_Port2->bDataBuffer), u32DRAMAddr + uBufOffset,
                            uTransLen, MIU_SDRAM2SDRAM);
                    g32PTPFilePtr += uTransLen;
                    uBufOffset += uTransLen;
                    //printf("6. g32PTPFilePtr: %ld\n", g32PTPFilePtr);
                }
                else
                {
                    MDrv_MIU_Copy(VirtoPhyAddr(psAttachDevice_Port2->bDataBuffer), u32DRAMAddr + uBufOffset,
                            PTP_GEN_CONTAIN_SIZE, MIU_SDRAM2SDRAM);
                    g32PTPFilePtr += PTP_GEN_CONTAIN_SIZE;
                    uBufOffset += PTP_GEN_CONTAIN_SIZE;
                    //printf("7. g32PTPFilePtr: %ld\n", g32PTPFilePtr);
                }
            }

        }
        //  printf("length:%lx  Trans:%lx\n",u32Length,gTotalBytes_Port2);
    }
    else
    {
        printf("not as we thought!!\n");
    }

Get_Done:
    //  xTotal=u32TotalSize;
    // printf("fileptr:%lx , totalsize:%lx \n",g32PTPFilePtr,u32TotalSize);
    //xxx=g32PTPFilePtr - u32TotalSize;
    //   if (xxx>= 0)
    // if ( (U32 volatile) g32PTPFilePtr >= (U32 volatile) u32TotalSize )     //strange, have to be volatile
    //printf("g32PTPFilePtr: %ld, u32TotalSize: %ld\n", g32PTPFilePtr, u32TotalSize);
    if ( g32PTPFilePtr >=  (u32TotalSize) )
    {
        StartGetObject_Port2=0;

        drvUSBHost_PTP_ChkZeroPacket_Port2((U32)g32PTPFilePtr+PTP_GEN_CONTAIN_SIZE);
        if (pOTGH_PT_BLK_Port2->bStatus != HOST20_OK)
            goto Func_Done;

        //printf("Get object response:%lx\n",u32HandleIdx);
        val = drvUSBHost_PTP_BulkInResponse_Port2();

        if( val != PTP_OK )
        {
            printf("return fail val = 0x%bx",val);
        }
    }

Func_Done:
    #ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1_Port2();     //Release USB window 1
    #endif

    return(uBufOffset);
}

#if 0
//U32 volatile xTotal;
U32 drvUSBHost_PTP_GetPartialObject(
    U32 u32TotalSize,
    U32 u32HandleIdx,
    U32 u32FileOffset,
    U32 u32Length,
    U32 u32DRAMAddr)
{
    U8 val;
    //S32 xxx;

    //printf("Trans offset:%lx , length:%lx\n",u32FileOffset,u32Length);

    #ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_SetXdataWindow1();             //Set USB window 1
    #endif

    if (u32FileOffset==0)
    {
        StartGetObject_Port2=1;
        g32PTPFilePtr=0;
        xAssignVar_Port2.u16OpCode = PTP_GetObject;
        xAssignVar_Port2.u32TransId++;
        xDymnVar_Port2.u32FileOffset = u32FileOffset;
        xDymnVar_Port2.u32DataLen = u32Length;
        xDymnVar_Port2.u32HandleId = u32HandleIdx;
        xDymnVar_Port2.u32DramAddr = u32DRAMAddr;

        drvUSBHost_PTP_Transaction2 (&xAssignVar_Port2, &xDymnVar_Port2);
        g32PTPFilePtr+=gTotalBytes_Port2;
        //printf(" Trans:%lx",gTotalBytes_Port2);
    }
    else if (u32FileOffset==g32PTPFilePtr)
    {
        // printf("u32FileOffset=OurPredict\n");
        xAssignVar_Port2.u16OpCode = PTP_GetObject;
        xAssignVar_Port2.u32TransId++;
        xDymnVar_Port2.u32DataLen = u32Length;
        xDymnVar_Port2.u32HandleId = u32HandleIdx;
        xDymnVar_Port2.u32DramAddr = u32DRAMAddr;
        drvUSBHost_PTP_BulkInData ( &xAssignVar_Port2, &xDymnVar_Port2);
        g32PTPFilePtr+=gTotalBytes_Port2;
        //  printf("length:%lx  Trans:%lx\n",u32Length,gTotalBytes_Port2);
    }
    else
    {
        printf("not as we thought!!\n");
    }

    //  xTotal=u32TotalSize;
    // printf("fileptr:%lx , totalsize:%lx \n",g32PTPFilePtr,u32TotalSize);
    //xxx=g32PTPFilePtr - u32TotalSize;
    //   if (xxx>= 0)
    // if ( (U32 volatile) g32PTPFilePtr >= (U32 volatile) u32TotalSize )     //strange, have to be volatile
    printf("g32PTPFilePtr: %ld, u32TotalSize: %ld\n", g32PTPFilePtr, u32TotalSize);
    if ( g32PTPFilePtr >=  (u32TotalSize) )
    {
        drvUSBHost_PTP_ChkZeroPacket((U32)g32PTPFilePtr);
        StartGetObject_Port2=0;
        printf("Get object response:%lx\n",u32HandleIdx);
        val = drvUSBHost_PTP_BulkInResponse();

        if( val != PTP_OK )
        {
            printf("return fail val = 0x%bx",val);
        }
    }

    #ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1();     //Release USB window 1
    #endif

    return(gTotalBytes_Port2);
}
#endif
#if 0
void GetResidualBulkIn(U32 u32Total)
{
    //U8 xdata *pBuf=(U8 xdata *)(psAttachDevice_Port2->bDataBuffer);
    U8 u8result;

   if (StartGetObject_Port2==0) return;

    while (1)
        {
            u8result = flib_Host20_Issue_Bulk(pOTGH_PT_BLK_Port2->.bInQHDArrayNum, 16*1024,VirtoPhyAddr(BASEADR_SDRAM_USB_DUMMY_BUF),OTGH_Dir_IN);

            g32PTPFilePtr+=0x4000-TotalBytes_Port2;
       //     printf("cleanup-> fileptr:%lx , totalsize:%lx \n",g32PTPFilePtr,u32Total);

            if (TotalBytes_Port2) break;      //break when short packet
            if (g32PTPFilePtr >= u32Total ) break;     //BEAK WHEN data is engough

        }
    flib_Host20_Issue_Bulk(pOTGH_PT_BLK_Port2->bInQHDArrayNum, 12,VirtoPhyAddr(psAttachDevice_Port2->bDataBuffer),OTGH_Dir_IN);
    StartGetObject_Port2=0;
}

U32 drvUSBHost_PTP_GetFreeSpace(void)
{
    U32 u32Tmp;
    u32Tmp = (PTP_Driver_Port2.u32FreeSpaceH<<23)+(PTP_Driver_Port2.u32FreeSpaceL/200);
    return(u32Tmp);
}
#endif

U32 drvUSBHost_PTP_GetObjectHandleNo_Port2(U32 u32HandleIdx)
{
    U32 u32HandleNo;

    #if (PLATFORM==MCU8051)
    U32 u32Win1Adr = PTP_Driver_Port2.u32ObjectHandleNumBuf;
    U32 xdata *pu32HandleBuf = (U32 xdata*)0xF000;
    #endif

    if(u32HandleIdx>=PTP_Driver_Port2.u32TotalHandleNo)
        return(INVALID_HANDLE_NO);

    while(u32HandleIdx>0x400)
    {
        u32Win1Adr += 0x1000;
        u32HandleIdx -= 0x400;
    }

    #if (PLATFORM==MCU8051)
    drvUSBHost_PTP_SetObjHWindow1_Port2(u32Win1Adr);
    #endif

    u32HandleNo = pu32HandleBuf[u32HandleIdx];

    #if (PLATFORM==MCU8051)
    drvUSBHost_PTP_FreeObjHWindow1_Port2();
    #endif

    return(u32HandleNo);
}

void drvUSBHost_PTP_SaveObjectHandleNo_Port2(U32 u32HandleIdx,U32 u32HandleNo)
{
    #if (PLATFORM==MCU8051)
    U32 u32Win1Adr = PTP_Driver_Port2.u32ObjectHandleNumBuf;
    U32 xdata *pu32HandleBuf = (U32 xdata*)0xF000;
    #endif

    while(u32HandleIdx>0x400)
    {
        u32Win1Adr += 0x1000;
        u32HandleIdx -= 0x400;      //an index put at 4 bytes slot
    }

    #if (PLATFORM==MCU8051)
    drvUSBHost_PTP_SetObjHWindow1_Port2(u32Win1Adr);
    #endif

    pu32HandleBuf[u32HandleIdx] = u32HandleNo;

    #if (PLATFORM==MCU8051)
    drvUSBHost_PTP_FreeObjHWindow1_Port2();
    #endif
}

U32 drvUSBHost_PTP_GetObjectHandles_Port2(U16 u16ObjFormat, U32 u32AssociObjHandle)
{
    DBG_PTP(printf("drvUSBHost_PTP_GetObjectHandles\r\n"));
    DBG_PTP(printf("u16ObjFormat: 0x%x\n",u16ObjFormat));
    DBG_PTP(printf("u32AssociObjHandle: 0x%lx\n",u32AssociObjHandle));

    xAssignVar_Port2.u16OpCode = PTP_GetObjectHandles;
    xAssignVar_Port2.u32TransId++;
    xAssignVar_Port2.u32Param1 = 0xFFFFFFFF;
    xAssignVar_Port2.u32Param2 = u16ObjFormat;
    xAssignVar_Port2.u32Param3 = u32AssociObjHandle;

    #ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_SetXdataWindow1_Port2();             //Set USB window 1
    #endif

    drvUSBHost_PTP_Transaction_Port2 (&xAssignVar_Port2, &xDymnVar_Port2, TRUE);

    #ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1_Port2();     //Release USB window 1
    #endif

    DBG_PTP(printf("Obecjt count returned: %ld\n", PTP_Driver_Port2.u32TotalHandleNo));
    return (PTP_Driver_Port2.u32TotalHandleNo);
}

BOOLEAN drvUSBHost_PTP_GetObjInfo_Port2(U32 u32ObjectHandle)
{
    U8              ii;
    DYMNVAR     *pDymnVar;

    pDymnVar = drvUSBHost_PTP_GetObjectInfo_Port2(u32ObjectHandle);

    if (pDymnVar == NULL)
        return FALSE;

    gPtpObjectInfo.ObjectFormat = pDymnVar->u16FileType;
    gPtpObjectInfo.ObjectCompressedSize = pDymnVar->u32FileLength;
    gPtpObjectInfo.ThumbFormat = pDymnVar->u16ThumbFormat;
    gPtpObjectInfo.ThumbCompressedSize = pDymnVar->u32ThumbSize;
    gPtpObjectInfo.ThumbPixWidth = pDymnVar->u32ThumbPixWidth;
    gPtpObjectInfo.ThumbPixHeight = pDymnVar->u32ThumbPixHeight;
    gPtpObjectInfo.ImagePixWidth = pDymnVar->ImagePixWidth;
    gPtpObjectInfo.ImagePixHeight = pDymnVar->ImagePixHeight;
    gPtpObjectInfo.ParentObject = pDymnVar->u32ParentObject;
    //memcpy(gPtpObjectInfo.Filename, pDymnVar->u16FileName, 26);
    for (ii=0; ii<13; ii++)
       gPtpObjectInfo.Filename[ii] =  LE2ME16(pDymnVar->u16FileName[ii]);

    memcpy((U8*)&gPtpObjectInfo.sDateTime,
        (U8*) &pDymnVar->sDateTime,
        sizeof(struct _DATETIME));

    return TRUE;
}

U32 drvUSBHost_PTP_GetCurGetObjOffset_Port2(void)
{
    return g32PTPFilePtr;
}

U8 drvUSBHost_PTP_GetError_Port2(void)
{
    U8 u8UsbStatus;

    #ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_SetXdataWindow1_Port2();             //Set USB window 1
    #endif

    u8UsbStatus = pOTGH_PT_BLK_Port2->bStatus;

    #ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1_Port2();     //Release USB window 1
    #endif

    return u8UsbStatus;
}

#if 0
void drvUSBHost_PTP_Error_Handle_Port2()
{
    U8      bCMD[8];
    U8      u8Buff[4];

    printf("drvUSBHost_PTP_Error_Handle\n");

    memcpy(bCMD, PTP_GET_DEVICE_STATUS_Port2,8);
    if (flib_Host20_Issue_Control (1, bCMD, 4,  u8Buff) == 0)
    {
        printf("GetDevStatus: %02bX %02bX %02bX %02bX\n",
            u8Buff[0], u8Buff[1], u8Buff[2], u8Buff[3]);
    }

    memcpy(bCMD, USB_GET_ENDPOINT_STATUS_Port2, 8);
    bCMD[4] = psAttachDevice_Port2->saCD[0].sInterface[0].sED[0].bED_EndpointAddress & 0x0f;
    if ( (flib_Host20_Issue_Control (1, bCMD, 2,  u8Buff) == 0) && (u8Buff[0] != 0) )
    {
        printf("Get Ept %bd Status: 0x%bX\n", bCMD[4], u8Buff[0]);
        memcpy(bCMD, USB_CLEAR_ENDPOINT_FEATURE_Port2, 8);
        bCMD[4] = psAttachDevice_Port2->saCD[0].sInterface[0].sED[0].bED_EndpointAddress & 0x0f;
        flib_Host20_Issue_Control (1, bCMD, 0,  NULL);
    }

    memcpy(bCMD, USB_GET_ENDPOINT_STATUS_Port2, 8);
    bCMD[4] = psAttachDevice_Port2->saCD[0].sInterface[0].sED[1].bED_EndpointAddress & 0x0f;
    if ( (flib_Host20_Issue_Control (1, bCMD, 2,  u8Buff) == 0) && (u8Buff[0] != 0) )
    {
        printf("Get Ept %bd Status: 0x%X\n", bCMD[4], u8Buff[0]);
        memcpy(bCMD, USB_CLEAR_ENDPOINT_FEATURE_Port2, 8);
        bCMD[4] = psAttachDevice_Port2->saCD[0].sInterface[0].sED[1].bED_EndpointAddress & 0x0f;
        flib_Host20_Issue_Control (1, bCMD, 0,  NULL);
    }

    memcpy(bCMD, PTP_GET_DEVICE_STATUS_Port2,8);
    if (flib_Host20_Issue_Control (1, bCMD, 4,  u8Buff) == 0)
    {
        printf("GetDevStatus: %02bX %02bX %02bX %02bX\n",
            u8Buff[0], u8Buff[1], u8Buff[2], u8Buff[3]);
    }
}
#endif
//#endif // (defined(ENABLE_PTP_HOST) && (ENABLE_PTP_HOST==1))

