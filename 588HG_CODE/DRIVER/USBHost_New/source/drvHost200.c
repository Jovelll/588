#pragma LARGE
#include "board.h"
#if ENABLE_USB_INPUT



#include "UsbHostConfig.h"
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
#include "drvHostLib.h"
#include "drvHost200.H"
//#include "Host20_AP_HID.H"
//#include "Host20_AP_Camera.H"
 #include "drvUsbMain.h"
#include "drvscsi.h"

#ifdef ATV_SERISE_USE
//#include "uart.h"
#endif
#define DRV_HOST_DBG(x)  //x;

OTGH_PT_BLK_Struct  xdata    OTGH_PT_BLK _at_ OTGH_PT_BLK_DATA;
OTGH_PT_BLK_Struct  xdata    *sOTGH_PT_BLK;

void OTGH_PT_Bulk_Init(void);


extern void Init_Mass_Storage_Structure();
extern void TestUsbDisk();

#ifdef USE_XDATA_ADDRESS_0XF000
extern void MDrv_USB_SetXdataWindow1();
extern void MDrv_USB_ReleaseXdataWindow1();
#endif


//====================================================================
// * Function Name: OTGH_AP_Main
// * Description: Main menu of path test
// * Input: none
// * OutPut: none
//====================================================================
#ifdef Enable_Mass_Storage
BOOLEAN MassStorage_Init(void)
{

  BOOLEAN result;

  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_SetXdataWindow1();
  #endif

  if (sAttachDevice.saCD[0].sInterface[0].bInterfaceClass!=0x08)
  {
    DRV_HOST_DBG(printMsg("not mass storage class!\n"));
    #ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1();
    #endif
    return FALSE;
  }
  DRV_HOST_DBG(printMsg("Mass storage device..\n"));
  Init_Mass_Storage_Structure();

  OTGH_PT_Bulk_Init();

  result=bSCSI_Initial();
  if(!result)
  {
    MDrv_Timer_Delayms(500);  //081205 at SEC, disk needs more time for TUR OK
    result=bSCSI_Initial();
  }
  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_ReleaseXdataWindow1();
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
  if (sAttachDevice.saCD[0].sInterface[0].sED[0].bED_bmAttributes==OTGH_ED_BULK)
  { //<5.1>.stop Asynchronous Schedule
    flib_Host20_Asynchronous_Setting(HOST20_Disable);
    if (Host20_qHD_List_Bulk0.bMaxPacketSize1> 2)           //larger than 512
    {//Support only 2 QHD
      mwHost20_CurrentAsynchronousAddr_Set(Host20_STRUCTURE_qHD_BASE_ADDRESS);
    }
    else
    {
      //<5.2>.Hang the qHD for ED0~ED3
      SetPointer(&(Host20_qHD_List_Control1.bNextQHDPointer3),VirtoPhyAddr((UINT16)&Host20_qHD_List_Control0));

    }

    //<5.2>.Enable Asynchronous Schedule
    flib_Host20_Asynchronous_Setting(HOST20_Enable);
  }

}
#endif
extern void Dump_Data(UINT16 addr,UINT16 sz);
extern BOOLEAN FirstBulkOut,FirstBulkIn;

//====================================================================
// * Function Name: OTGH_PT_Bulk_Init
// * Description:
// * Input: none
// * OutPut: none
//====================================================================
#ifdef Enable_Mass_Storage
void OTGH_PT_Bulk_Init(void)
{

  UINT16 wMaxPacketSize;

  // UINT8 *wTemp;

  wMaxPacketSize=sAttachDevice.saCD[0].sInterface[0].sED[0].bED_wMaxPacketSizeHighByte;
  wMaxPacketSize=(wMaxPacketSize<<8)+(sAttachDevice.saCD[0].sInterface[0].sED[0].bED_wMaxPacketSizeLowByte);



  //<5>.Hook the qHD
  if (sAttachDevice.saCD[0].sInterface[0].sED[0].bED_bmAttributes==OTGH_ED_BULK)
  { //<5.1>.stop Asynchronous Schedule
    //flib_Host20_Asynchronous_Setting(HOST20_Disable);
    //mbHost20_USBCMD_AsynchronousEnable_Clr();

    mbHost20_USBCMD_AsynchronousEnable_Clr();
    while(mwHost20_USBSTS_AsynchronousStatus_Rd()>0);

    if (wMaxPacketSize>512)
    { //Support only 2 QHD        //why???
      //SetPointer(&(Host20_qHD_List_Bulk0.bNextQHDPointer3),VirtoPhyAddr((UINT16)&Host20_qHD_List_Bulk1));
      //SetPointer(&(Host20_qHD_List_Bulk1.bNextQHDPointer3),VirtoPhyAddr((UINT16)&Host20_qHD_List_Bulk0));


      //mwHost20_CurrentAsynchronousAddr_Set(VirtoPhyAddr((UINT16)&Host20_qHD_List_Bulk0));
      //printf("USB Err");
      //while (1){}

    }
    else{
      //<5.2>.Hook the qHD for ED0~ED3
      SetPointer(&(Host20_qHD_List_Control1.bNextQHDPointer3),VirtoPhyAddr((UINT16)&Host20_qHD_List_Bulk0));
      SetPointer(&(Host20_qHD_List_Bulk0.bNextQHDPointer3),VirtoPhyAddr((UINT16)&Host20_qHD_List_Bulk1));
      SetPointer(&(Host20_qHD_List_Bulk1.bNextQHDPointer3),VirtoPhyAddr((UINT16)&Host20_qHD_List_Control0));


    }


    //<5.2>.Enable Asynchronous Schedule
    //flib_Host20_Asynchronous_Setting(HOST20_Enable);

  }


  FirstBulkIn=TRUE;
  FirstBulkOut=TRUE;

  //Setting Max Packet Size and print message
  wMaxPacketSize=sAttachDevice.saCD[0].sInterface[0].sED[0].bED_wMaxPacketSizeHighByte;
  wMaxPacketSize=(wMaxPacketSize<<8)+(sAttachDevice.saCD[0].sInterface[0].sED[0].bED_wMaxPacketSizeLowByte);
  Host20_qHD_List_Bulk0.bMaxPacketSize0=wMaxPacketSize&0xff;
  Host20_qHD_List_Bulk0.bMaxPacketSize1=wMaxPacketSize>>8;
  Host20_qHD_List_Bulk1.bMaxPacketSize0=wMaxPacketSize&0xff; //Suppose In/Out have the same Max packet Size
  Host20_qHD_List_Bulk1.bMaxPacketSize1=wMaxPacketSize>>8; //Suppose In/Out have the same Max packet Size
  Host20_qHD_List_Bulk0.bDeviceAddress=sAttachDevice.bAdd;
  Host20_qHD_List_Bulk1.bDeviceAddress=sAttachDevice.bAdd;
  Host20_qHD_List_Bulk0.bEdNumber=sAttachDevice.saCD[0].sInterface[0].sED[0].bED_EndpointAddress & 0x0f; //set endpoint address
  Host20_qHD_List_Bulk1.bEdNumber=sAttachDevice.saCD[0].sInterface[0].sED[1].bED_EndpointAddress & 0x0f; //set endpoint address

  //printf("add:%02bx\n",psDevice_AP->bAdd);
  //printf("Endpoint 1 addr: %02bx\n", Host20_qHD_List_Bulk0.bEdNumber);
  //printf("Endpoint 2 addr: %02bx\n", Host20_qHD_List_Bulk1.bEdNumber);


  //printf(">>> Bulk In/Out Max Packet Size = %x\n",wMaxPacketSize);

  //For Bulk Structure
  //sOTGH_PT_BLK=( OTGH_PT_BLK_Struct *)flib_Host20_GetStructure(Host20_MEM_TYPE_4K_BUFFER);//2=>4kBuffer;
  //sOTGH_PT_BLK=(OTGH_PT_BLK_Struct xdata *)OTGH_PT_BLK_DATA;            //bulk QH data structure
  //printf("PT_BLK:%x\n",(U16)&OTGH_PT_BLK);
  //printf("OTGH_PT_BLK_DATA:%x\n",OTGH_PT_BLK_DATA);
  //OTGH_PT_BLK.wDataRange=2*1024*1; ////2k


  if (((sAttachDevice.saCD[0].sInterface[0].sED[0].bED_EndpointAddress)&BIT7)>0)
  { OTGH_PT_BLK.bInQHDArrayNum=0;//Array 0           //for in 0, for out,1
    OTGH_PT_BLK.bOutQHDArrayNum=1;//Array 0
  }
  else
  {
    OTGH_PT_BLK.bInQHDArrayNum=1;//Array 0            //for in 1 for out,0
    OTGH_PT_BLK.bOutQHDArrayNum=0;//Array 0
  }

  OTGH_PT_BLK.bStatus = HOST20_OK;
}
#endif
#endif
