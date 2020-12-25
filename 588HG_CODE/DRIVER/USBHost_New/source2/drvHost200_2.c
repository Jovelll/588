#pragma LARGE
#include "board.h"
#if ENABLE_USB_INPUT




#include "UsbHostConfig_2.h"
#include <stdlib.h>
//#include <stdarg.h>
#ifndef ATV_SERISE_USE
#include <stdio.h>
#endif
#include <string.h>

//#include "chipset.h"
//#include "flib.h"
#ifndef ATV_SERISE_USE
#include "datatype.h"
#else
#include "UsbHostDataDef.h"
#endif
#include "drvHostLib_2.h"
#include "drvHost200_2.H"
//#include "Host20_AP_HID.H"
//#include "Host20_AP_Camera.H"
 #include "drvUsbMain_2.h"
#include "drvscsi_2.h"

#ifdef ATV_SERISE_USE
//#include "uart.h"
#endif
#define DRV_HOST_DBG(x)  //x;


//OTGH_PT_BLK_Struct  xdata    OTGH_PT_BLK _at_ OTGH_PT_BLK_DATA;
OTGH_PT_BLK_Struct  xdata    *pOTGH_PT_BLK_Port2 = OTGH_PT_BLK_DATA;
OTGH_PT_BLK_Struct  xdata    *sOTGH_PT_BLK_PORT2;

void OTGH_PT_Bulk_Init_Port2(void);


extern void Init_Mass_Storage_Structure_Port2();
extern void TestUsbDisk_Port2();

#ifdef USE_XDATA_ADDRESS_0XF000
extern void MDrv_USB_SetXdataWindow1_Port2();
extern void MDrv_USB_ReleaseXdataWindow1_Port2();
#endif


//====================================================================
// * Function Name: OTGH_AP_Main
// * Description: Main menu of path test
// * Input: none
// * OutPut: none
//====================================================================
#ifdef Enable_Mass_Storage
BOOLEAN MassStorage_Init_Port2(void)
{

  BOOLEAN result;

  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_SetXdataWindow1_Port2();
  #endif

  if (psAttachDevice_Port2->saCD[0].sInterface[0].bInterfaceClass!=0x08)
  {
    DRV_HOST_DBG(printf("not mass storage class!\n"));
    #ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1_Port2();
    #endif
    return FALSE;
  }
  DRV_HOST_DBG(printf("Mass storage device..\n"));
  Init_Mass_Storage_Structure_Port2();

  OTGH_PT_Bulk_Init_Port2();

  result=bSCSI_Initial_Port2();
  if(!result)
  {
    MDrv_Timer_Delayms(500); //081205 at SEC, disk needs more time for TUR OK
    result=bSCSI_Initial_Port2();
  }
  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_ReleaseXdataWindow1_Port2();
  #endif

  return result;
}
#endif



#if 0
//====================================================================
// * Function Name: OTGH_PT_Bulk_Close
// * Description:
// * Input: none
// * OutPut: none
//====================================================================
void OTGH_PT_Bulk_Close(void)
{

  //<5>.Hang the qHD
  if (psAttachDevice_Port2->saCD[0].sInterface[0].sED[0].bED_bmAttributes==OTGH_ED_BULK)
  { //<5.1>.stop Asynchronous Schedule
    flib_Host20_Asynchronous_Setting(HOST20_Disable);
    if (pHost20_qHD_List_Bulk0_Port2->bMaxPacketSize1> 2)           //larger than 512
    {//Support only 2 QHD
      mwHost20_CurrentAsynchronousAddr_Set(Host20_STRUCTURE_qHD_BASE_ADDRESS);
    }
    else
    {
      //<5.2>.Hang the qHD for ED0~ED3
      SetPointer_Port2(&(pHost20_qHD_List_Control1_Port2->bNextQHDPointer3),VirtoPhyAddr((UINT16)pHost20_qHD_List_Control0_Port2));

    }

    //<5.2>.Enable Asynchronous Schedule
    flib_Host20_Asynchronous_Setting(HOST20_Enable);
  }

}
#endif
extern void Dump_Data_Port2(UINT16 addr,UINT16 sz);
extern BOOLEAN FirstBulkOutPort2,FirstBulkInPort2;

//====================================================================
// * Function Name: OTGH_PT_Bulk_Init_Port2
// * Description:
// * Input: none
// * OutPut: none
//====================================================================
#ifdef Enable_Mass_Storage
void OTGH_PT_Bulk_Init_Port2(void)
{

  UINT16 wMaxPacketSize;

  // UINT8 *wTemp;

  wMaxPacketSize=psAttachDevice_Port2->saCD[0].sInterface[0].sED[0].bED_wMaxPacketSizeHighByte;
  wMaxPacketSize=(wMaxPacketSize<<8)+(psAttachDevice_Port2->saCD[0].sInterface[0].sED[0].bED_wMaxPacketSizeLowByte);



  //<5>.Hook the qHD
  if (psAttachDevice_Port2->saCD[0].sInterface[0].sED[0].bED_bmAttributes==OTGH_ED_BULK)
  { //<5.1>.stop Asynchronous Schedule
    //flib_Host20_Asynchronous_Setting(HOST20_Disable);
    //mbHost20_USBCMD_AsynchronousEnable_Clr();

    mbHost20_USBCMD_AsynchronousEnable_Clr();
    while(mwHost20_USBSTS_AsynchronousStatus_Rd()>0);

    if (wMaxPacketSize>512)
    { //Support only 2 QHD        //why???
      //SetPointer_Port2(&(pHost20_qHD_List_Bulk0_Port2->bNextQHDPointer3),VirtoPhyAddr((UINT16)pHost20_qHD_List_Bulk1_Port2));
      //SetPointer(&(pHost20_qHD_List_Bulk1_Port2bNextQHDPointer3),VirtoPhyAddr((UINT16)pHost20_qHD_List_Bulk0_Port2));


      //mwHost20_CurrentAsynchronousAddr_Set(VirtoPhyAddr((UINT16)pHost20_qHD_List_Bulk0_Port2));
      //printf("USB Err");
      //while (1){}

    }
    else{
      //<5.2>.Hook the qHD for ED0~ED3
      SetPointer_Port2(&(pHost20_qHD_List_Control1_Port2->bNextQHDPointer3),VirtoPhyAddr((UINT16)pHost20_qHD_List_Bulk0_Port2));
      SetPointer_Port2(&(pHost20_qHD_List_Bulk0_Port2->bNextQHDPointer3),VirtoPhyAddr((UINT16)pHost20_qHD_List_Bulk1_Port2));
      SetPointer_Port2(&(pHost20_qHD_List_Bulk1_Port2->bNextQHDPointer3),VirtoPhyAddr((UINT16)pHost20_qHD_List_Control0_Port2));


    }


    //<5.2>.Enable Asynchronous Schedule
    //flib_Host20_Asynchronous_Setting(HOST20_Enable);

  }


  FirstBulkInPort2=TRUE;
  FirstBulkOutPort2=TRUE;

  //Setting Max Packet Size and print message
  wMaxPacketSize=psAttachDevice_Port2->saCD[0].sInterface[0].sED[0].bED_wMaxPacketSizeHighByte;
  wMaxPacketSize=(wMaxPacketSize<<8)+(psAttachDevice_Port2->saCD[0].sInterface[0].sED[0].bED_wMaxPacketSizeLowByte);
  pHost20_qHD_List_Bulk0_Port2->bMaxPacketSize0=wMaxPacketSize&0xff;
  pHost20_qHD_List_Bulk0_Port2->bMaxPacketSize1=wMaxPacketSize>>8;
  pHost20_qHD_List_Bulk1_Port2->bMaxPacketSize0=wMaxPacketSize&0xff; //Suppose In/Out have the same Max packet Size
  pHost20_qHD_List_Bulk1_Port2->bMaxPacketSize1=wMaxPacketSize>>8; //Suppose In/Out have the same Max packet Size
  pHost20_qHD_List_Bulk0_Port2->bDeviceAddress=psAttachDevice_Port2->bAdd;
  pHost20_qHD_List_Bulk1_Port2->bDeviceAddress=psAttachDevice_Port2->bAdd;
  pHost20_qHD_List_Bulk0_Port2->bEdNumber=psAttachDevice_Port2->saCD[0].sInterface[0].sED[0].bED_EndpointAddress & 0x0f; //set endpoint address
  pHost20_qHD_List_Bulk1_Port2->bEdNumber=psAttachDevice_Port2->saCD[0].sInterface[0].sED[1].bED_EndpointAddress & 0x0f; //set endpoint address

  //printf("add:%02bx\n",psDevice_AP->bAdd);
  //printf("Endpoint 1 addr: %02bx\n", pHost20_qHD_List_Bulk0_Port2->bEdNumber);
  //printf("Endpoint 2 addr: %02bx\n", pHost20_qHD_List_Bulk1_Port2->bEdNumber);


  //printf(">>> Bulk In/Out Max Packet Size = %x\n",wMaxPacketSize);

  //For Bulk Structure
  //sOTGH_PT_BLK_PORT2=( OTGH_PT_BLK_Struct *)flib_Host20_GetStructure_Port2(Host20_MEM_TYPE_4K_BUFFER);//2=>4kBuffer;
  //sOTGH_PT_BLK_PORT2=(OTGH_PT_BLK_Struct xdata *)OTGH_PT_BLK_DATA;            //bulk QH data structure
  //printf("PT_BLK:%x\n",(U16)pOTGH_PT_BLK_Port2);
  //printf("OTGH_PT_BLK_DATA:%x\n",OTGH_PT_BLK_DATA);
  //pOTGH_PT_BLK_Port2->wDataRange=2*1024*1; ////2k


  if (((psAttachDevice_Port2->saCD[0].sInterface[0].sED[0].bED_EndpointAddress)&BIT7)>0)
  { pOTGH_PT_BLK_Port2->bInQHDArrayNum=0;//Array 0           //for in 0, for out,1
    pOTGH_PT_BLK_Port2->bOutQHDArrayNum=1;//Array 0
  }
  else
  {
    pOTGH_PT_BLK_Port2->bInQHDArrayNum=1;//Array 0            //for in 1 for out,0
    pOTGH_PT_BLK_Port2->bOutQHDArrayNum=0;//Array 0
  }

  pOTGH_PT_BLK_Port2->bStatus = HOST20_OK;
}
#endif
#endif
