#pragma LARGE
#include "board.h"
#if ENABLE_USB_INPUT


#include "USBHostConfig.h"
#ifndef ATV_SERISE_USE
#include <stdio.h>
#endif
#include <string.h>
#ifndef ATV_SERISE_USE
#include "datatype.h"
#else
#include "UsbHostDataDef.h"
#endif
#include "drvHostLib.h"
#include "drvUsbMain.h"
#ifndef ATV_SERISE_USE
#include "sysinfo.h"
#include "drvtimer.h"
#endif
//#include "R8051XC.h"
#ifdef ATV_SERISE_USE
#include "Board.h"
#include "common.h"
//#include "uart.h"
#endif
#include "drvusb.h"

//#include "msAPI_Timer.h"

#define DrvHostLib_DEBUG    1
#if ENABLE_DEBUG&&DrvHostLib_DEBUG
    #define DrvHostLib_printData(str, value)   printData(str, value)
    #define DrvHostLib_printMsg(str)           printMsg(str)
#else
    #define DrvHostLib_printData(str, value)
    #define DrvHostLib_printMsg(str)
#endif

#define DEBUG_USB(msg)          msg
#define  MINI_DEBUG2(x)         x;

code UINT8 OTGH_GETDESCRIPTOR_DEVICE_8[]  = {0x80,0x06,0x00,0x01,0x00,0x00,0x08,0x00};
code UINT8 OTGH_GETDESCRIPTOR_DEVICE[]    = {0x80,0x06,0x00,0x01,0x00,0x00,0x12,0x00};
code UINT8 OTGH_GETDESCRIPTOR_CONFIG[]    = {0x80,0x06,0x00,0x02,0x00,0x00,0x08,0x00};

code UINT8 OTGH_SETADDRESS[]              = {0x00,0x05,03,0x00,0x00,0x00,0x00,0x00};

code UINT8 OTGH_SETCONFIGURATION[]        = {0x00,0x09,0x01,0x00,0x00,0x00,0x00,0x00};
code UINT8 OTGH_GETDESCRIPTOR_STR[]     = {0x80,0x06,0x00,0x03,0x00,0x00,0x02,0x00}; //Get String Descriptor
//code UINT8 OTGH_SET_FEATURE_OTG[]         = {0x00,0x03,0x03,0x00,0x00,0x00,0x00,0x00};
//code UINT8 OTGH_SETDESCRIPTOR_DEVICE[]    = {0x00,0x07,0x00,0x01,0x00,0x00,0x12,0x00}; //Set Device Descriptor
//code UINT8 OTGH_GETDESCRIPTOR_STR70[]     = {0x80,0x06,0x70,0x03,0x00,0x00,0x06,0x00}; //Get String80 Descriptor
//code UINT8 OTGH_SETDESCRIPTOR_STR70[]     = {0x00,0x07,0x70,0x03,0x00,0x00,0x06,0x00}; //Set String80 Descriptor
//code UINT8 OTGH_GETDESCRIPTOR_STR80[]     = {0x80,0x06,0x80,0x03,0x00,0x00,0x12,0x00}; //Get String80 Descriptor
//code UINT8 OTGH_SETDESCRIPTOR_STR80[]     = {0x00,0x07,0x80,0x03,0x00,0x00,0x12,0x00}; //Set String80 Descriptor
//code UINT8 waIntervalMap[11]={1,2,4,8,16,32,64,128,256,512,1024};

qHD_Structure xdata Host20_qHD_List_Control0  _at_ Host20_STRUCTURE_qHD_BASE_ADDRESS;
qHD_Structure xdata Host20_qHD_List_Control1  _at_ (Host20_STRUCTURE_qHD_BASE_ADDRESS+Host20_qHD_SIZE);
#ifdef Enable_Mass_Storage
qHD_Structure xdata Host20_qHD_List_Bulk0  _at_  (Host20_STRUCTURE_qHD_BASE_ADDRESS+2*Host20_qHD_SIZE);
qHD_Structure xdata Host20_qHD_List_Bulk1  _at_  (Host20_STRUCTURE_qHD_BASE_ADDRESS+3*Host20_qHD_SIZE);
#endif
UINT8    xdata        Host20_qTD_Manage[Host20_qTD_MAX];        //1=>Free 2=>used
Host20_Attach_Device_Structure  xdata sAttachDevice _at_ Host20_Attach_Device_DATA;
BOOLEAN FirstBulkOut,FirstBulkIn;
//Host20_Attach_Device_Structure  xdata *psDevice_AP;

UINT8 xdata pUsbData[512] _at_ Host20_TEMP_DATA;
UINT8 gSpeed;
U8 xdata PORT_DEVICE[HOST20_INTERFACE_NUM_MAX];
//volatile UINT32 gwLastqTDSendOK;
extern U8 gUsbTimeout;
extern U8 gUsbStatus;
U8 DataToggleBackup;
U8 DataToggleBackup_qh2;
extern U8 RecoveryFlag;
//extern BOOLEAN gDeviceFatalError;
extern U8  gUsbDeviceState;
extern U8  NowIsHub;
#ifdef CERAMAL_SERISE_USE
U8 u8varUHCI_FORCE_FS=0;
U8 u8varOTG_FORCE_FS=0;
#endif

#ifdef USE_XDATA_ADDRESS_0XF000
extern void MDrv_USB_SetXdataWindow1();
extern void MDrv_USB_ReleaseXdataWindow1();
#endif

//************************************************************************************************************
//************************************************************************************************************
//                          *** Group-1:Main Function ***
//*************************************************************************************************************
//************************************************************************************************************

//====================================================================
// * Function Name: flib_OTGH_Init
// * Description: Init the Host HW and prepare the ED/TD
//   <1>.Init All the Data Structure
//       <1.1>.Build control list
//       <1.2>.Build Bulk list
//       <1.3>.Build Interrupt list
//       <1.4>.Build ISO list (Reserved)
//   <2>.Reset the chip
//   <3>.Set HW register
//       <3.1>.Enable FIFO-Enable(0x100->Bit5) & FPGA-Mode-Half-Speed (0x100->Bit1)
//       <3.2>.Enable interrupts
//       <3.3>.Periodic List Base Address register
//       <3.4>.USBCMD (Interrupt/Threshod/Frame List/Run-Stop)
//
// * Input: wTimeWaitForConnect_ms:The time of waitting for connecting
// * OutPut: 0:Device do not connect
//           1:Host init ok
//           2:Bus Rest Fail
//====================================================================
#ifdef ATV_SERISE_USE
//extern U8 jpCVBS5_Get_Device_Define(void);
#endif

#define   wTimeWaitForConnect_ms 1500

extern DWORD xdata u32UsbCount;
UINT32 MDrv_Timer_DiffTimeFromNow(UINT32 old_time)
{
  return (u32UsbCount-old_time);
}

//UINT32 MDrv_Timer_GetTime0(void)
//{
//  return 0;
//}

UINT8 flib_OTGH_Init(UINT8 wForDevice_B)
{
  UINT8 wValue;
  UINT16 wTimer_ms;

#ifndef  Enable_FS_Only
  MDrv_Timer_Delayms(2);//2ms
  XBYTE[gUTMI_BASE+0x00]&=0xfe;
  MDrv_Timer_Delayms(2);//2ms
#endif

  //<1>.Waiting for the Device connect
  #if 1
  if (wForDevice_B==0) {
    mbHost20_USBCMD_HCReset_Set();            //host controller reset
    while(mbHost20_USBCMD_HCReset_Rd()>0);
  }
  #endif

  wValue=0;
  wTimer_ms=0;
  do {
       wValue=mwHost20_PORTSC_ConnectStatus_Rd();
       DrvHostLib_printData("wValue: %x\n",wValue);

       if (wValue==0) {
         MDrv_Timer_Delayms(1);//10, wait 1 ms
         wTimer_ms++;
       }
       //if (mwHost20_PORTSC_ConnectStatus_Rd()==0) return 0;
       if (wTimer_ms>wTimeWaitForConnect_ms)          // Case1:Waiting for 10 sec=10000
       { // Case2:Waiting for 100 ms =100
         DEBUG_USB(DrvHostLib_printMsg("??? Waiting for Peripheral Connecting Fail...\n"));
         return (0);
       }
  }while(wValue==0);



  //mwHost20_Misc_EOF1Time_Set(Host20_EOF1Time);
  DrvHostLib_printData("0x2434:%x\n",XBYTE[gUHC_BASE+0x34]);
  //<2>.Init All the Data Structure & write Base Address register
  flib_Host20_InitStructure();

  //Write Base Address to Register
  // Host20_STRUCTURE_BASE_ADDRESS

  mwHost20_CurrentAsynchronousAddr_Set(VirtoPhyAddr(Host20_STRUCTURE_qHD_BASE_ADDRESS));
  //mwHost20_CurrentAsynchronousAddr_Set(VirtoPhyAddr((U16)&Host20_qHD_List_Work));
   //DrvHostLib_printData("qhead:%x\n",VirtoPhyAddr(Host20_STRUCTURE_qHD_BASE_ADDRESS));

  //<4>.Enable interrupts
  mwHost20_USBINTR_Set( HOST20_USBINTR_SystemError |

                        HOST20_USBINTR_IntOnAsyncAdvance |
                        HOST20_USBINTR_USBError |
                        HOST20_USBINTR_CompletionOfTransaction);

  #ifdef ATV_SERISE_USE
  //if(jpCVBS5_Get_Device_Define())
  //      UHC_XBYTE(0x40)|=0x80;//force enter FSmode
  #endif

  #ifdef CERAMAL_SERISE_USE
  if(u8varUHCI_FORCE_FS)
        UHC_XBYTE(0x40)|=0x80;//force enter FSmode
  #endif

  if (wForDevice_B==0) {
    if (flib_Host20_PortBusReset()>0)
      return(2);
  }

  return (1);
}
void USB_Write_REG32(UINT8 addr,UINT32 val)
{
  mwHost20Port(addr)=(unsigned char)val & 0xff;
  mwHost20Port(addr+1)=(unsigned char)((val >>8));
  mwHost20Port(addr+2)=(unsigned char)((val>>16));
  mwHost20Port(addr+3)=(unsigned char)((val>>24));

}

//====================================================================
// * Function Name: flib_Host20_Close
// * Description:
//   <1>.Suspend Host
//   <2>.Disable the interrupt
//   <3>.Clear Interrupt Status
//   <4>.Issue HW Reset
//   <5>.Free the Allocated Memory
// * Input:
// * OutPut:
//====================================================================
UINT8 flib_Host20_Close()
{
#if 0 //We don't need to suspend the bus.
  UINT32 wTemp;

  if (mwHost20_USBINTR_Rd()>0) {
    //<1>.Suspend Host
    flib_Host20_Suspend();

    //<2>.Disable the interrupt
    mwHost20_USBINTR_Set(0);

    //<3>.Clear Interrupt Status
    wTemp=mwHost20_USBSTS_Rd();
    wTemp=wTemp&0x0000003F;
    mwHost20_USBSTS_Set(wTemp);
  }
#endif

  return (1);
}

//====================================================================
// * Function Name: flib_Host20_StopRun_Setting
// * Description:
// * Input:
// * OutPut:
//====================================================================
void flib_Host20_StopRun_Setting(UINT8 bOption)
{
  if (bOption==HOST20_Enable) {
    if (mbHost20_USBCMD_RunStop_Rd()>0)
      return;

    mbHost20_USBCMD_RunStop_Set();

    while(mbHost20_USBCMD_RunStop_Rd()==0);
  }
  else {
    if (mbHost20_USBCMD_RunStop_Rd()==0)
      return;

    mbHost20_USBCMD_RunStop_Clr();

    while(mbHost20_USBCMD_RunStop_Rd()>0);
 }

}
#if 0
//====================================================================
// * Function Name: flib_Host20_Asynchronous_Setting
// * Description:
// * Input:
// * OutPut:
//====================================================================
void flib_Host20_Asynchronous_Setting(UINT8 bOption)
{
  if (bOption==HOST20_Enable) {
    if (mwHost20_USBSTS_AsynchronousStatus_Rd()>0)
      return;

    mbHost20_USBCMD_AsynchronousEnable_Set();

    while(mwHost20_USBSTS_AsynchronousStatus_Rd()==0);
  }
  else{
    if (mwHost20_USBSTS_AsynchronousStatus_Rd()==0)
      return;

    mbHost20_USBCMD_AsynchronousEnable_Clr();

    while(mwHost20_USBSTS_AsynchronousStatus_Rd()>0);
  }

}
#endif
//void _noop_()
//{
   //  int i=0;
//}
//====================================================================
// * Function Name: flib_Host20_PortBusReset
// * Description:
//   <1>.Waiting for HCHalted=0
//   <2>.Write PortEnable=0(Reserved for Faraday-IP)
//   <3>.Write PortReset=0
//   <4>.Wait time
//   <5>.Write PortReset=0
//   <6>.Waiting for IRS->PortChangeDetect
// * Input:
// * OutPut:
//====================================================================
UINT8 flib_Host20_PortBusReset(void)
{
  UINT8 bExitLoop;
  //UINT8 ttt;
  UINT32 wTmp;
  //flib_Host20_ForceSpeed(0);

  //<1>.Disable RunStop
  // bResetDuringRun=0;

  //printf("disable run\n");
  //if (mbHost20_USBCMD_RunStop_Rd()>0)
  flib_Host20_StopRun_Setting(HOST20_Disable);
  //printf("0x10 ->%02bx \n",XBYTE[0x2410]);

  //<2>.Write PortReset=0
     if (XBYTE[0x1ecc]==5)          // Pluto: 5
    {
            XBYTE[gUTMI_BASE+0x2c]=4;
            XBYTE[gUTMI_BASE+0x2d]=0;//enable TX common mode,
            XBYTE[gUTMI_BASE+0x2f]=0;            //preemsis
            XBYTE[gUTMI_BASE+0x2a]=0xd0;            //preemsis
     }
  mwHost20_PORTSC_PortReset_Set();

  MDrv_Timer_Delayms(30);
  //flib_Debug_LED_Off_All();; //GPIO-High
  if (XBYTE[0x1ecc]==5)          // Pluto: 5
       XBYTE[gUTMI_BASE+0x2a]=0x0;            //preemsis
  MDrv_Timer_Delayms(70);

  //<3>.Wait time=>55ms
  //flib_Host20_TimerEnable(55);
 // MDrv_Timer_Delayms(100);
  //flib_Debug_LED_Off_All();; //GPIO-High

  mwHost20_PORTSC_PortReset_Clr();

  //printf("Host Speed:%02bx\n",mwOTG20_Control_HOST_SPD_TYP_Rd());

  //<5>.Waiting for IRS->PortChangeDetect
  //printf("wait reset\n");
  #if 1
  bExitLoop=0;
  wTmp=0;

  do {
       if (mwHost20_PORTSC_PortReset_Rd()==0)
       bExitLoop=1;
       //USBDELAY(100);
       //if (mwHost20_PORTSC_ConnectStatus_Rd()==0) return 1;
       wTmp++;
       if (wTmp>200) {
         DEBUG_USB(DrvHostLib_printMsg("??? Error waiting for Bus Reset Fail...==> Reset HW Control\n"));
         mbHost20_USBCMD_HCReset_Set();
         //while(mbHost20_USBCMD_HCReset_Rd()==1);
         return (1);
       }
  }
  while(bExitLoop==0);
  #endif
  //<6>.Enable RunStop Bit
  #if 0
  if (mwHost20_PORTSC_ForceSuspend_Rd())
  {
    printf("port suspend\n");
    mwHost20_PORTSC_ForceResume_Set();          //force resume
    USBDELAY(14);
    mwHost20_PORTSC_ForceResume_Clr();
  }
  #endif
  #ifndef Enable_FS_Only
  //mwHost20_PORTSC_ConnectChange_Set();  //clear connection change bit
   if (XBYTE[0x1ecc]==5)          // Pluto: 5
    {
            XBYTE[gUTMI_BASE+0x2c]|=0xc5;
            XBYTE[gUTMI_BASE+0x2d]|=0x3;//enable TX common mode,
            XBYTE[gUTMI_BASE+0x2f]|=0x4a;            //preemsis
        //    XBYTE[gUTMI_BASE+0x13]|=0x70;
     }
  #endif

  XBYTE[gUTMI_BASE+0x06]|=0x03;        //reset UTMI
  XBYTE[gUTMI_BASE+0x06]&=0xfc;
  flib_Host20_StopRun_Setting(HOST20_Enable);

  //flib_Debug_LED_On_All();   //GPIO-Low
  MDrv_Timer_Delayms(50);

  //<7>.Detect Speed
  gSpeed= mwOTG20_Control_HOST_SPD_TYP_Rd();
  //ttt=mwOTG20_Control_HOST_SPD_TYP_Rd();
  //printf("\r\n Host type:%x", sAttachDevice.bSpeed);
  //<8>.Delay 20 ms
  //flib_Host20_TimerEnable(5);//After Reset => Host must reserve 20 ms for Device init

  if (gSpeed==0)                 //full speed
  {

    XBYTE[gUTMI_BASE+0x09]&=0x7F;
    mwHost20_Misc_EOF1Time_Set(2);
    gUsbDeviceState=USB11_DEVICE;

  }
  else if (gSpeed==2)                //high speed
  {
    XBYTE[gUTMI_BASE+0x09]|=0x80;              //HS rx robust enable
    mwHost20_Misc_EOF1Time_Set(3);
    gUsbDeviceState=USB20_DEVICE;

  }

  flib_Host20_QHD_Control_Init();
  //printf("return 1");
//  if(gSpeed==0x01)
   // {
       //  MINI_DEBUG2(printf("low speed device\n"));
       //  return 0; //not support Low Speed device
   // }
  return (0);
}

//====================================================================
// * Function Name: flib_Host20_Suspend
// * Description:
//   <1>.Make sure PortEnable=1
//   <2>.Write PORTSC->Suspend=1
//   <3>.Waiting for the ISR->PORTSC->Suspend=1
// * Input:
// * OutPut: 0:OK
//           1:Fail
//====================================================================
#if 0
void flib_Host20_Suspend(void)
{
  #if 0
  if (mbHost20_USBCMD_RunStop_Rd()==0)
    return(1);

  //<1>.Make sure PortEnable=1
  if (mwHost20_PORTSC_EnableDisable_Rd()==0)
    return(1);
  #endif
  //<2>.Write PORTSC->Suspend=1
  flib_Host20_StopRun_Setting(HOST20_Disable);//For Faraday HW request

  //<3>.Write PORTSC->Suspend=1
  mwHost20_PORTSC_ForceSuspend_Set();

  //<4>.Waiting for the ISR->PORTSC->Suspend=1
  #if 0
  flib_Host20_TimerEnable_UnLock(1);//1sec
  bExitFlag=0;

  do {
       if (mwHost20_PORTSC_ForceSuspend_Rd()>0)
       bExitFlag=1;

       if (gwOTG_Timer_Counter>5) {
         bExitFlag=1;
         printf(">>> Fail => Time Out for Waiting ForceSuspend...\n");
       }
  }
  while(bExitFlag==0);
  #else
  while(mwHost20_PORTSC_ForceSuspend_Rd()==0);
  #endif

}

#endif


//====================================================================
// * Function Name: flib_Host20_Control_Command_Request
// * Description:
// * Input: none
// * OutPut: none
//====================================================================
#if 0
void flib_Host20_Control_Command_Request(Host20_Control_Command_Structure *pbCMD,UINT8 bmRequestType_Temp,UINT8 bRequest_Temp,UINT16 wValue_Temp,UINT16 wIndex_Temp,UINT16 wLength_Temp)
{
  UINT8 i;
  UINT8 *pbTemp;

  pbTemp=(UINT8*)pbCMD;
  for (i=0;i<8;i++)
    *pbTemp++=0x00;
  pbCMD->bmRequestType=bmRequestType_Temp;  //Byte-0:In/Standard/Device
  pbCMD->bRequest=bRequest_Temp;

  pbCMD->wValueLow=(UINT8)wValue_Temp;
  pbCMD->wValueHigh=(UINT8)(wValue_Temp>>8);

  pbCMD->wIndexLow=(UINT8)wIndex_Temp;
  pbCMD->wIndexHigh=(UINT8)(wIndex_Temp>>8);

  pbCMD->wLengthLow=(UINT8)wLength_Temp;
  pbCMD->wLengthHigh=(UINT8)(wLength_Temp>>8);
}
#endif
#if 0
void FillBufferArray(qTD_Structure xdata *spTempqTD, UINT16 bpDataPage)
{

  U32 temp;
  temp=VirtoPhyAddr(bpDataPage);
  spTempqTD->ArrayBufferPointer[0].Byte1=(UINT8)(temp>>8)&0xf0;
  spTempqTD->ArrayBufferPointer[0].Byte2=(UINT8)(temp>>16)&0xff;
  spTempqTD->ArrayBufferPointer[0].Byte3=(UINT8)(temp>>24)&0xff;

}
#endif
void FillBufferArray2(qTD_Structure xdata *spTempqTD, UINT32 bpDataPage,UINT16 size)

{

  U32 Addrtemp;
  S16 sizetemp;
  U8 index=0;

  Addrtemp=(bpDataPage);
  sizetemp=size;

  spTempqTD->ArrayBufferPointer[index].Byte0=(UINT8)(Addrtemp)&0xff;
  spTempqTD->ArrayBufferPointer[index].Byte1=(UINT8)(Addrtemp>>8)&0xff;
  spTempqTD->ArrayBufferPointer[index].Byte2=(UINT8)(Addrtemp>>16)&0xff;
  spTempqTD->ArrayBufferPointer[index].Byte3=(UINT8)(Addrtemp>>24)&0xff;

  sizetemp=sizetemp-(0x1000-(U16)(Addrtemp&0x00000fff));

  while(sizetemp>0)
  {
    index++;
    if(index > 4)
    {
      DEBUG_USB(DrvHostLib_printMsg("\r\n Over transfer buffer!!"));
      break;
    }

    Addrtemp=(Addrtemp & 0xfffff000)+0x1000;
    spTempqTD->ArrayBufferPointer[index].Byte0=0;
    spTempqTD->ArrayBufferPointer[index].Byte1=(UINT8)(Addrtemp>>8)&0xf0;
    spTempqTD->ArrayBufferPointer[index].Byte2=(UINT8)(Addrtemp>>16)&0xff;
    spTempqTD->ArrayBufferPointer[index].Byte3=(UINT8)(Addrtemp>>24)&0xff;

    sizetemp-=0x1000;
  }

}

void Dump_QTD(UINT16 addr);
void Dump_Data(UINT16 addr,UINT16 sz);
#if 0
void Dump_Data(UINT16 addr,UINT16 sz)
{
  UINT16 i;
  printf("addr:%x -> \r\n",addr);
  for (i=0; i < sz ; i++)
    printf("%02bx ",*(unsigned char volatile xdata *)(addr+i));
}
#endif
//====================================================================
 //For Control-Single qTD// * Function Name: flib_Host20_Issue_Control
// * Description:
//   <1>.Analysis the Controller Command => 3 type
//   <2>.Case-1:"Setup/In/Out' Format..."
//       (get status/get descriptor/get configuration/get interface)
//   <3>.Case-2:'Setup/In' Format...      => Faraday Driver will not need
//       (clear feature/set feature/set address/set Configuration/set interface  )
//   <4>.Case-3:'Setup/Out/In'
//       (set descriptor)
// * Input:
// * OutPut: 0: OK
//           X:>0 => Fail
//====================================================================
#define CTRL_SETUP_IN_OUT   0
#define CTRL_SETUP_IN       1
#define CTRL_SETUP_OUT_IN   2

UINT8 flib_Host20_Issue_Control (UINT8 bEdNum,UINT8* pbCmd,UINT16 hwDataSize,UINT8* pbData)
{
  qTD_Structure xdata *spTempqTD;
  UINT16       bpDataPage;
  UINT8        bReturnValue;
  UINT8        bSetupType;
  //UINT8 i;
  qHD_Structure xdata *qh_ptr;

  if (bEdNum==0)
    qh_ptr=&Host20_qHD_List_Control0;
  else
    qh_ptr=&Host20_qHD_List_Control1;

  //<0>.Allocate qTD & Data Buffer
  spTempqTD=(qTD_Structure xdata *) flib_Host20_GetStructure(Host20_MEM_TYPE_qTD);//0=>qTD
  //bpDataPage=sAttachDevice.bDataBuffer;           //2//2k buffer
  bpDataPage=sAttachDevice.bDataBuffer;
  //<2.1>.Setup packet
  //<A>.Fill qTD
  spTempqTD->bPID=HOST20_qTD_PID_SETUP;                   //Bit8~9
  //SetValue(spTempqTD+9,0,2,HOST20_qTD_PID_SETUP);
  spTempqTD->bTotalBytes0=8;           //Bit16~30
  spTempqTD->bTotalBytes1=0;

  spTempqTD->bDataToggle=0;            //Bit31
  //FillBufferArray(spTempqTD, bpDataPage);
  FillBufferArray2(spTempqTD, VirtoPhyAddr(bpDataPage),8);
  //DrvHostLib_printData("bpDataPage:%x\n",bpDataPage);
  memcpy((U8 xdata *)bpDataPage,pbCmd,8);

  #if 1
  //<B>.Send qTD
  //Dump_QTD(spTempqTD);

  //Dump_Data(0x2400,0x50);
  //bReturnValue=flib_Host20_Send_qTD(spTempqTD ,psHost20_qHD_List_Control[bEdNum],5);
  bReturnValue=flib_Host20_Send_qTD(spTempqTD ,qh_ptr,gUsbTimeout);

  if (bReturnValue>0)
    goto exit_issue_control;
  //<1>.Analysis the Controller Command
  switch (*(pbCmd+1)) { // by Standard Request codes
    // <2>.Case-1:"Setup/In/Out' Format..."
    case 10:
        if ((*pbCmd & REQTYPE_MASK) == TYPE_CLASS)
            bSetupType = CTRL_SETUP_IN;            //HID class, set idle
        else
            bSetupType = CTRL_SETUP_IN_OUT;        //Standard class, get interface
        break;
    case 0:        // get status
    case 6:        // get descriptor
    case 8:        // get configuration
    case 0xfe:     //get Max Lun
        bSetupType = CTRL_SETUP_IN_OUT;
        break;
    //<3>.Case-2:'Setup/In' Format...      => Faraday Driver will not need
    case 1:        // clear feature
    case 3:        // set feature
    case 5:        // set address
    case 9:        // set Configuration
    case 11:       // set interface
        bSetupType = CTRL_SETUP_IN;
        break;
    //<4>.Case-3:'Setup/Out/In'
    case 7:        // set descriptor
        bSetupType = CTRL_SETUP_OUT_IN;
        break;
    default:
        DrvHostLib_printMsg("Not supported request!!");
        break;
  }



  //<1>.Analysis the Controller Command
  //switch (*(pbCmd+1)) { // by Standard Request codes
  switch(bSetupType){
    // <2>.Case-1:"Setup/In/Out' Format..."
#if 0
             if ((*pbCmd & REQTYPE_MASK) == TYPE_CLASS) //HID, set idle
                goto setup_in;
#endif
#if 0
    case 0:        // get status
    case 6:        // get descriptor
    case 8:        // get configuration
    case 10:       // get interface
    case 0xfe:     //get Max Lun
#endif
    case CTRL_SETUP_IN_OUT:
             //<2.2>.In packet

             //<A>.Fill qTD
             spTempqTD=(qTD_Structure xdata *) flib_Host20_GetStructure(Host20_MEM_TYPE_qTD);//0=>qTD
             spTempqTD->bPID=HOST20_qTD_PID_IN;                   //Bit8~9
             spTempqTD->bTotalBytes0=(UINT8)hwDataSize;           //Bit16~30
             spTempqTD->bTotalBytes1=(UINT8)(hwDataSize>>8);           //Bit16~30

             spTempqTD->bDataToggle=1;            //Bit31
             //printf("ToByte0:%02bx, ToByte1:%02bx , Toggle:%02bx\n",spTempqTD->bTotalBytes0,spTempqTD->bTotalBytes1,spTempqTD->bDataToggle);

             //spTempqTD->ArrayBufferPointer_Word[0]=VirtoPhyAddr(bpDataPage);
             //FillBufferArray(spTempqTD, bpDataPage);
             FillBufferArray2(spTempqTD, VirtoPhyAddr(bpDataPage),hwDataSize);

             //Dump_QTD(spTempqTD);

             //<B>.Send qTD
             //bReturnValue=flib_Host20_Send_qTD(spTempqTD ,psHost20_qHD_List_Control[bEdNum],5);
             bReturnValue=flib_Host20_Send_qTD(spTempqTD ,qh_ptr,gUsbTimeout);
             //printf("\noutcome->\r\n");
             //for (i=0 ; i < 8 ; i++)
             //printf("%02bx ",*((unsigned char volatile xdata*)(bpDataPage+i)));
             //Dump_Data(bpDataPage,hwDataSize);
             if (bReturnValue>0)
               goto exit_issue_control;
             //printf("c4");


             //<C>.Waiting for result
             memcpy((U8 xdata *)pbData,(U8 xdata *)bpDataPage,hwDataSize);

             //Dump_Data((UINT16)pbData,hwDataSize);

             //<2.3>.Out packet

             //<A>.Fill qTD
             spTempqTD=(qTD_Structure xdata *) flib_Host20_GetStructure(Host20_MEM_TYPE_qTD);//0=>qTD
             spTempqTD->bPID=HOST20_qTD_PID_OUT;                   //Bit8~9
             spTempqTD->bTotalBytes0=0;           //Bit16~30
             spTempqTD->bTotalBytes1=0;           //Bit16~30

             spTempqTD->bDataToggle=1;            //Bit31

             //<B>.Send qTD
             //bReturnValue=flib_Host20_Send_qTD(spTempqTD ,psHost20_qHD_List_Control[bEdNum],5);
             bReturnValue=flib_Host20_Send_qTD(spTempqTD ,qh_ptr,gUsbTimeout);

             if (bReturnValue>0)
               goto exit_issue_control;

             break;

    //<3>.Case-2:'Setup/In' Format...      => Faraday Driver will not need
#if 0
    case 1:        // clear feature
    case 3:        // set feature
    case 5:        // set address
    case 9:        // set Configuration
    case 11:       // set interface
#endif
    case CTRL_SETUP_IN:

             //<3.2>.In packet

             //<A>.Fill qTD
             spTempqTD=(qTD_Structure xdata *) flib_Host20_GetStructure(Host20_MEM_TYPE_qTD);//0=>qTD
             spTempqTD->bPID=HOST20_qTD_PID_IN;                   //Bit8~9
             spTempqTD->bTotalBytes0=(UINT8)hwDataSize;           //Bit16~30
             spTempqTD->bTotalBytes1=(UINT8)(hwDataSize>>8);           //Bit16~30
             spTempqTD->bDataToggle=1;            //Bit31
             //spTempqTD->ArrayBufferPointer_Word[0]=VirtoPhyAddr(bpDataPage);
             //FillBufferArray(spTempqTD, bpDataPage);
             FillBufferArray2(spTempqTD, VirtoPhyAddr(bpDataPage),hwDataSize);
             //<B>.Send qTD
             //bReturnValue=flib_Host20_Send_qTD(spTempqTD ,psHost20_qHD_List_Control[bEdNum],5);
             bReturnValue=flib_Host20_Send_qTD(spTempqTD ,qh_ptr,gUsbTimeout);

             if (bReturnValue>0)
               goto exit_issue_control;
             //<C>.Copy Result
             //memcpy(pbData,(UINT8*)bpDataPage,hwDataSize);
             break;

    //<4>.Case-3:'Setup/Out/In'
#if 0
    case 7:        // set descriptor
#endif
    case CTRL_SETUP_OUT_IN:
             //<4.2>.Out packet
             //<A>.Fill qTD
             spTempqTD=(qTD_Structure xdata *) flib_Host20_GetStructure(Host20_MEM_TYPE_qTD);//0=>qTD
             spTempqTD->bPID=HOST20_qTD_PID_OUT;                   //Bit8~9
             spTempqTD->bTotalBytes0=(UINT8)hwDataSize;           //Bit16~30
             spTempqTD->bTotalBytes1=(UINT8)(hwDataSize>>8);           //Bit16~30
             spTempqTD->bDataToggle=1;            //Bit31
             //spTempqTD->ArrayBufferPointer_Word[0]=VirtoPhyAddr(bpDataPage);
             //FillBufferArray(spTempqTD, bpDataPage);
             FillBufferArray2(spTempqTD, VirtoPhyAddr(bpDataPage),hwDataSize);

             memcpy((UINT8*)bpDataPage,pbData,hwDataSize);

             //<B>.Send qTD
             //bReturnValue=flib_Host20_Send_qTD(spTempqTD ,psHost20_qHD_List_Control[bEdNum],5);
             bReturnValue=flib_Host20_Send_qTD(spTempqTD ,qh_ptr,gUsbTimeout);

             if (bReturnValue>0)
               goto exit_issue_control;


             //<4.3>.In packet
             //<A>.Fill qTD
             spTempqTD=(qTD_Structure xdata *) flib_Host20_GetStructure(Host20_MEM_TYPE_qTD);//0=>qTD
             spTempqTD->bPID=HOST20_qTD_PID_IN;                   //Bit8~9
             spTempqTD->bTotalBytes0=0;           //Bit16~30
             spTempqTD->bTotalBytes1=0;           //Bit16~30
             spTempqTD->bDataToggle=1;            //Bit31

             //<B>.Send qTD
             //bReturnValue=flib_Host20_Send_qTD(spTempqTD ,psHost20_qHD_List_Control[bEdNum],5);
             bReturnValue=flib_Host20_Send_qTD(spTempqTD ,qh_ptr,gUsbTimeout);

             if (bReturnValue>0)
               goto exit_issue_control;

             break;

    default:
             break;
  }
  #endif
  return (0);
exit_issue_control:

  return (bReturnValue);

}

#if 0
void Dump_QTD(UINT16 addr)
{
  UINT8 i;
  // kcj 2007-06-07 printf("QTD:%x -> \n",addr);
  // kcj 2007-06-07 for (i=0; i < 0x20 ; i++)
  // kcj 2007-06-07 printf("%02bx ",*(unsigned char volatile xdata *)(addr+i));


}
void Dump_Data(UINT16 addr,UINT16 sz)
{
  UINT16 i;
  // kcj 2007-06-07 printf("addr:%x -> \n",addr);
  // kcj 2007-06-07 for (i=0; i < sz ; i++)
  // kcj 2007-06-07 printf("%02bx ",*(unsigned char volatile xdata *)(addr+i));


}
#endif

//====================================================================
// * Function Name: flib_Host20_Issue_Bulk
// * Description: Input data must be 4K-Alignment
//               <1>.MaxSize=20 K
//               <2>.Support Only 1-TD
// * Input:
// * OutPut:
//====================================================================
#ifdef Enable_Mass_Storage
UINT8  flib_Host20_Issue_Bulk (UINT8 bArrayListNum,UINT16 hwSize,UINT32 pwBufferArray,UINT8 bDirection)
{
  qTD_Structure xdata *spTempqTD;
  UINT8 bTemp;
  qHD_Structure xdata *spTempqH;

  //<1>.Fill TD
  if (bArrayListNum==0)
    spTempqH=&Host20_qHD_List_Bulk0;
  else if (bArrayListNum==1)
    spTempqH=&Host20_qHD_List_Bulk1;

  spTempqTD =(qTD_Structure xdata *) flib_Host20_GetStructure(Host20_MEM_TYPE_qTD); //The qTD will be release in the function "Send"
  spTempqTD->bTotalBytes0=hwSize & 0xff;
  spTempqTD->bTotalBytes1=(UINT8)(hwSize >>8);

  FillBufferArray2(spTempqTD,pwBufferArray,hwSize);         //use global buffer , because it is 4k alignment


  //<2>.Analysis the Direction
  if (bDirection==OTGH_Dir_IN)
  {
    spTempqTD->bPID=HOST20_qTD_PID_IN;
    if (FirstBulkIn)
    {
      spTempqTD->bDataToggle=0;
      spTempqH->bDataToggleControl=1;
    }
  }
  else
  {
    spTempqTD->bPID=HOST20_qTD_PID_OUT;
    if (FirstBulkOut)
    {
      spTempqTD->bDataToggle=0;
      spTempqH->bDataToggleControl=1;

    }
  }


  //<3>.Send TD

  //if (bArrayListNum==0)
  bTemp=flib_Host20_Send_qTD(spTempqTD ,spTempqH,gUsbTimeout);
  //else if (bArrayListNum==1)
  //bTemp=flib_Host20_Send_qTD(spTempqTD ,&Host20_qHD_List_Bulk1,gUsbTimeout);
  //printf("Send Bulk:Critical error\n");

  if ((FirstBulkIn)&&(bDirection==OTGH_Dir_IN))
  {
    spTempqH->bDataToggleControl=0;
    FirstBulkIn=FALSE;
  }
  if ((FirstBulkOut)&&(bDirection==OTGH_Dir_Out))
  {
    spTempqH->bDataToggleControl=0;
    FirstBulkOut=FALSE;
  }

  return (bTemp);


}
#endif
//====================================================================
// * Function Name: flib_Host20_AnalysisConfigyration
// * Description:
// * Input:
//
// * OutPut:
//====================================================================
UINT8 flib_Host20_AnalysisConfigyration (UINT8  *pbData)
{
  UINT8 i,j,k;


  //<1>.Copy Configuration 1~2
  if (sAttachDevice.sDD.bCONFIGURATION_NUMBER>HOST20_CONFIGURATION_NUM_MAX)
  {
    // kcj 2007-06-07   printf("??? Analysis Configuration Fail(bCONFIGURATION_NUMBER>Max)...");
    //while(1);
  }
  else
    DrvHostLib_printData("bCONFIGURATION_NUMBER:%x\n",sAttachDevice.sDD.bCONFIGURATION_NUMBER);
  //for (i=0;i<sAttachDevice.sDD.bCONFIGURATION_NUMBER;i++)
  i=0;
  {

    memcpy((U8 xdata *)&(sAttachDevice.saCD[i]),pbData,HOST20_CONFIGURATION_LENGTH);
    pbData=pbData+HOST20_CONFIGURATION_LENGTH;

    //<2>.Copy Interface 1~5
    if (sAttachDevice.saCD[i].bINTERFACE_NUMBER>HOST20_INTERFACE_NUM_MAX)
    {
      // kcj 2007-06-07 printf("??? Analysis Interface Fail(bINTERFACE_NUMBER>Max)...");
      //while(1);
    }
    else
        DrvHostLib_printData("bINTERFACE_NUMBER:%x\n",sAttachDevice.saCD[i].bINTERFACE_NUMBER);
    for (j=0;j<sAttachDevice.saCD[i].bINTERFACE_NUMBER;j++)
    {
      memcpy((U8 xdata *)&(sAttachDevice.saCD[i].sInterface[j]),pbData,HOST20_INTERFACE_LENGTH);
      pbData=pbData+HOST20_INTERFACE_LENGTH;

      //<3>.Copy Class HID
      if (sAttachDevice.saCD[i].sInterface[j].bInterfaceClass ==3)
      {//Only support 1 class
        memcpy((U8 xdata *)&(sAttachDevice.saCD[i].sInterface[j].sClass[0]),pbData,*pbData);
        pbData=pbData+(*pbData);
      }

      //<4>.Copy Endpoint 1~5
      if (sAttachDevice.saCD[i].sInterface[j].bEP_NUMBER>HOST20_ENDPOINT_NUM_MAX)
      {
        // kcj 2007-06-07   printf("??? Analysis Endpoint Fail(bEP_NUMBER>Max)...");
        //while(1);
      }
      else
        DrvHostLib_printData("bEP_NUMBER:%x\n",sAttachDevice.saCD[i].sInterface[j].bEP_NUMBER);
      for (k=0;k<sAttachDevice.saCD[i].sInterface[j].bEP_NUMBER;k++)
      {
        memcpy((U8 xdata *)&(sAttachDevice.saCD[i].sInterface[j].sED[k]),pbData,HOST20_ENDPOINT_LENGTHX);
        pbData=pbData+HOST20_ENDPOINT_LENGTHX;
        DrvHostLib_printData("EP:%x\n",sAttachDevice.saCD[i].sInterface[j].sED[k].bED_EndpointAddress);
      }//Endpoint

      PORT_DEVICE[j]=sAttachDevice.saCD[0].sInterface[j].bInterfaceProtocol;
    }//Interface

  }//Configuration
#if 0
      DrvHostLib_printData("######bInterfaceProtocol:%x\n",sAttachDevice.saCD[i].sInterface[j].bInterfaceProtocol);
      //1=keyboard
      //2=mouse
#endif
  //<4>.Checkong the OTG Descriptor
  //if (*pbData==3)
  //{ if (*(pbData+1)==9)
  //  {
  //    sAttachDevice.sOTG.bED_OTG_Length=3;
  //    sAttachDevice.sOTG.bED_OTG_bDescriptorType=9;
  //    sAttachDevice.sOTG.bED_OTG_bAttributes=*(pbData+2);
  //  }
  //}

  return (1);

}

//====================================================================
// * Function Name: flib_Host20_FirstTransfer        ==>51ok
// * Description:
// * Input:
//
// * OutPut:
//====================================================================
UINT8 flib_Host20_FirstTransfer (void)
{

  UINT8  bLoop;
  UINT8 bCMD[8];
  bLoop=0;
  do{
      bLoop++;
      //<1>.GetDescriptor - Device (8 Bytes)
      //while (1)
      //{

      memcpy(bCMD,OTGH_GETDESCRIPTOR_DEVICE_8,8);

      if (flib_Host20_Issue_Control (0,bCMD,8,(UINT8*)&(sAttachDevice.sDD))==0)
      {

        return (0);//ok
      }
      //}
      //<2>.Suspend the Bus
      DrvHostLib_printMsg("first\n");
      if (gUsbStatus==USB_TIMEOUT) return 1;
      #if 1
      flib_Host20_Close();
      //
      XBYTE[gUTMI_BASE+0x06]|=0x03;        //reset UTMI
      XBYTE[gUTMI_BASE+0x06]&=0xfc;
      mbHost20_USBCMD_HCReset_Set();
      MDrv_Timer_Delayms(100);

      //<3>.Reset Port
      flib_Host20_PortBusReset();
      #endif
  }while(bLoop<6);
   gUsbStatus=USB_INIT_FAIL;

  return(1);

}
#if 1
code UINT8 SET_FEATURE_PORT_POWER[]        = {0x23,0x03,0x08,0x00,0x01,0x00,0x00,0x00};
code UINT8 CLEAR_FEATURE_C_PORT_CONNECTION[]={0x23,0x01,0x10,0,0x01,0,0,0};
code UINT8 SET_FEATURE_PORT_RESET[]        = {0x23,0x03,0x04,0x00,0x01,0x00,0x00,0x00};
code UINT8 CLEAR_FEATURE_C_PORT_RESET[]={0x23,0x01,0x14,0,0x01,0,0,0};
code UINT8 OTGH_GETDESCRIPTOR_HUBCLASS[]={0xA0,0x06,0x00,0x00,0x00,0x00,0x09,0x00};
code UINT8 GET_PORT_STATUS[]={0xA3,0x00,0x00,0x00,0x01,0x00,0x04,0x00};
U8 Usb_Hub_Port_Num()
{
  UINT8 bCMD[8];
  U8 PortNum,i;

  memcpy(bCMD,OTGH_GETDESCRIPTOR_HUBCLASS,8);

  if (flib_Host20_Issue_Control (1,bCMD,0x09,pUsbData)>0)
  {
    return FALSE;
  }
  PortNum=*(pUsbData+2);
  #if 1
  for(i=1;i<=PortNum;i++)
  {
    //set feature port power
    memcpy(bCMD,SET_FEATURE_PORT_POWER,8);
    bCMD[4]=i;
    if ( flib_Host20_Issue_Control (1,bCMD,0,NULL)>0)
    {
      DEBUG_USB(DrvHostLib_printMsg("set port power failed\n"));
      return PortNum;
    }
  }
  MDrv_Timer_Delayms(125);

  for(i=1;i<=PortNum;i++)
  {
    //clear feature c_port_connection
    memcpy(bCMD,CLEAR_FEATURE_C_PORT_CONNECTION,8);
    bCMD[4]=i;
    if (flib_Host20_Issue_Control (1,bCMD,0,NULL)>0)
    {
      DEBUG_USB(DrvHostLib_printMsg("clear feature failed\n"));
      return PortNum;
    }
  }
  #endif
  return PortNum;
}
BOOLEAN USB_Hub_Handle(U8 port)
{
  UINT8 bCMD[8];

  MDrv_Timer_Delayms(10);//10, wait 1 ms

  //get all prot status


  //get prot status
  memcpy(bCMD,GET_PORT_STATUS,8);
  bCMD[4]=port;
  if (flib_Host20_Issue_Control (1,bCMD,0x04,pUsbData)>0)
  {
    DEBUG_USB(DrvHostLib_printMsg("Get port status failed\n"));

    return FALSE;
  }

  DEBUG_USB(DrvHostLib_printData(" Port:%x Status=",port));
  DEBUG_USB(DrvHostLib_printData("%x",(*pUsbData)));
  if(!((*pUsbData)&0x01))
    return FALSE;
  //for (i=0 ; i < 3 ; i++)
  {
    DEBUG_USB(DrvHostLib_printData("Port %x have device.\n",port));
    //set feature port_reset
    memcpy(bCMD,SET_FEATURE_PORT_RESET,8);
    bCMD[4]=port;
    if (flib_Host20_Issue_Control (1,bCMD,0,NULL)>0)
      return FALSE;
    MDrv_Timer_Delayms(10);//10, wait 1 ms

    memcpy(bCMD,GET_PORT_STATUS,8);
    bCMD[4]=port;
    if (flib_Host20_Issue_Control (1,bCMD,0x04,pUsbData)>0)
    {
      DEBUG_USB(DrvHostLib_printMsg("Get port status failed\n"));

      return FALSE;
    }

    #if 1
    DEBUG_USB(DrvHostLib_printData(" Port:%x Status=",port));
    DEBUG_USB(DrvHostLib_printData("%x",(*pUsbData)));
    //clear feature c_port_reset
    memcpy(bCMD,CLEAR_FEATURE_C_PORT_RESET,8);
    bCMD[4]=port;
    if (flib_Host20_Issue_Control (1,bCMD,0,NULL)>0)
      return FALSE;

    memcpy(bCMD,GET_PORT_STATUS,8);
    bCMD[4]=port;
    if (flib_Host20_Issue_Control (1,bCMD,0x04,pUsbData)>0)
    {
      DEBUG_USB(DrvHostLib_printMsg("Get port status failed\n"));

      return FALSE;
    }
    #endif
    // flib_Host20_Interrupt_Init(1,1,1);
    //if (pUsbData[2]!=0)
    //{
    //  OTGH_PT_Bulk_Init_for_Int();
    //  result=flib_Host20_Issue_Bulk (0,1,pUsbData,OTGH_Dir_IN);
    //  // flib_Host20_Issue_Interrupt(1,HOST20_qTD_PID_IN);

    MDrv_Timer_Delayms(30);//10, wait 1 ms
  }
  return TRUE;
}
#endif

#ifdef Enable_Issue_TestPacket
extern void IssueTestPacket(U8 xdata *TestData);
#endif

//====================================================================
// * Function Name: flib_Host20_Enumerate
// * Description:
// * Input:
//
// * OutPut:
//====================================================================
UINT8 flib_Host20_Enumerate (UINT8 bNormalEnumerate,UINT8 bAddress)
{
  UINT8 bCMD[8];
  UINT16 wLength;
  UINT16 ii, jj;
  //UINT32 i;
  //UINT8 bFaradayAP[4]={0x10,0x23,0x78,0x56};
  //UINT8 pbData[128];     //yuwen ,move to USB global SDRAM
  //UINT32 wSizeTemp;
  UINT8 bReturn;

  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_SetXdataWindow1();
  #endif

  DrvHostLib_printMsg("\r\n Enumerate");
  bReturn=1;
  //sAttachDevice.bISOTransferEnable=0;
   gUsbTimeout=6;  //090304, for samsung SUM-BWB32

  //<1>.GetDescriptor - Device (8 Bytes)
  if (flib_Host20_FirstTransfer()>0)
  {
    #ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1();
    #endif
    return(0);//Fail
  }
   gUsbTimeout=11;           //extend usb timeout , for some really slow response HD

  //Set the ep0 max packet size

  Host20_qHD_List_Control0.bMaxPacketSize0=sAttachDevice.sDD.bEP0MAXPACKETSIZE;
  //psHost20_qHD_List_Control[0]->bMaxPacketSize1=sAttachDevice.sDD.bEP0MAXPACKETSIZE>>8;

  Host20_qHD_List_Control1.bMaxPacketSize0=sAttachDevice.sDD.bEP0MAXPACKETSIZE;
  //psHost20_qHD_List_Control[1]->bMaxPacketSize1=sAttachDevice.sDD.bEP0MAXPACKETSIZE>>8;

  //printf("max packet:%02bx\n",sAttachDevice.sDD.bEP0MAXPACKETSIZE);
  //printf("max packet:%02bx\n",Host20_qHD_List_Control0.bMaxPacketSize0);

  #if 1
  //<2>.bNormalEnumerate =>Issue the Bus Reset
  //Issue Bus Reset
  //flib_Host20_Suspend();//Bruce;;add;;06102005
  //flib_Host20_TimerEnable(55);//Bruce;;add;;06102005
  DrvHostLib_printMsg("bus reset again...\n");
  if(!NowIsHub)
  flib_Host20_PortBusReset();
  #endif
  //<2.2>.Get Descriptor again
  memcpy(bCMD,OTGH_GETDESCRIPTOR_DEVICE,8);

  if (flib_Host20_Issue_Control (0,bCMD,0x12,(UINT8*)&(sAttachDevice.sDD))>0)
  {
     gUsbStatus=USB_INIT_FAIL;

    #ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1();
    #endif
    return(0);//Fail
  }
  #ifdef Enable_Issue_TestPacket
  IssueTestPacket(pUsbData);
  #endif
  #ifdef Enable_SOF_Only
  DrvHostLib_printMsg("Repeat SOF...\n");
  while (mwHost20_PORTSC_ConnectStatus_Rd() != 0)
  {
    MDrv_Timer_Delayms(100);
  }
  #endif

  //if (bNormalEnumerate>0)
  //  if (memcmp(bFaradayAP,&(sAttachDevice.sDD.bVIDLowByte),4)>0)
  //  {//This is not the Faraday Test AP
  //    printf(">>>Error:This is not the Faraday Test AP...\n");
  //    flib_DumpDeviceDescriptor(&(sAttachDevice.sDD));
  //    bReturn=2;
  //  }

  //<3>.Set Address to i
  memcpy(bCMD,OTGH_SETADDRESS,8);
  sAttachDevice.bAdd=bAddress;
  Host20_qHD_List_Control1.bDeviceAddress=sAttachDevice.bAdd;
  bCMD[2]=sAttachDevice.bAdd;
  if (flib_Host20_Issue_Control(0,bCMD,0,OTGH_NULL)>0)
  {
     gUsbStatus=USB_INIT_FAIL;

    #ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1();
    #endif
    return(0);//Fail
  }
        MDrv_Timer_Delayms(100);            //leave some time for device to be ready

  //<4>.GetDescriptor - Configuration (8Bytes)
  memcpy(bCMD,OTGH_GETDESCRIPTOR_CONFIG,8);
  //pbData=USBMALLOC(1000 );
  //for (i=0;i<1000;i++)
  //  *(pbData+i)=0;
  if (flib_Host20_Issue_Control (1,bCMD,0x08,pUsbData)>0)
  {
     gUsbStatus=USB_INIT_FAIL;

    #ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1();
    #endif
    return(0);//Fail
  }

  wLength=*(pUsbData+2)+((*(pUsbData+3))<<8);
  if (wLength > 128)
    wLength=128;
  //printf("Warning too long descriptor\n");
  //<5>.GetDescriptor - Configuration (Y-Bytes = wLength)
  bCMD[6]=*(pUsbData+2);//Low Byte
  bCMD[7]=*(pUsbData+3) ;//High Byte
  if (flib_Host20_Issue_Control (1,bCMD,wLength,pUsbData)>0)
  {
    #ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1();
    #endif
    return(0);//Fail
  }


  //printf("pUsbData:%x\n",wLength);
  //Dump_Data((UINT16)pUsbData,wLength);
  flib_Host20_AnalysisConfigyration(pUsbData);

  //Give it a try

  //<6>.If OTG-Descriptor exist => then return to issue the HNP_Enable
  if (bNormalEnumerate==0)
  {//To check the OTG Descriptor
    if (sAttachDevice.sOTG.bED_OTG_bDescriptorType==9)
    {
      #ifdef USE_XDATA_ADDRESS_0XF000
      MDrv_USB_ReleaseXdataWindow1();
      #endif
      return(1);//Fail
    }
  }

  //<7>.Get String
  //sAttachDevice.bStringLanguage[0]=0;
  //sAttachDevice.bStringManufacture[0]=0;
  //sAttachDevice.bStringProduct[0]=0;
  //sAttachDevice.bStringSerialN[0]=0;

  sAttachDevice.bStringLanguage[0]=0;
  sAttachDevice.bStringManufacture[0]=0;
  sAttachDevice.bStringProduct[0]=0;

  if ((sAttachDevice.sDD.bManufacturer == 0) && (sAttachDevice.sDD.bProduct == 0))
    goto Get_Str_Done;

  memcpy(bCMD, OTGH_GETDESCRIPTOR_STR, 8);
  bCMD[6] = 4;
  if (flib_Host20_Issue_Control (1,bCMD,4, pUsbData)>0)
  {
    goto Get_Str_Done;
    #ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1();
    #endif
    return(0);//Fail
  }
  sAttachDevice.bStringLanguage[0] = pUsbData[2];
  sAttachDevice.bStringLanguage[1] = pUsbData[3];

  if (sAttachDevice.sDD.bManufacturer != 0)
  {
      memcpy(bCMD, OTGH_GETDESCRIPTOR_STR, 8);
      bCMD[2] = sAttachDevice.sDD.bManufacturer;
      bCMD[4] = sAttachDevice.bStringLanguage[0];
      bCMD[5] = sAttachDevice.bStringLanguage[1];
      bCMD[6] = 2;

      if (flib_Host20_Issue_Control (1,bCMD,2, pUsbData)>0)
      {
        #ifdef USE_XDATA_ADDRESS_0XF000
        MDrv_USB_ReleaseXdataWindow1();
        #endif
        return(0);//Fail
      }

      bCMD[6] = pUsbData[0]; //real string length
      if (flib_Host20_Issue_Control (1,bCMD,bCMD[6], pUsbData)>0)
      {
        #ifdef USE_XDATA_ADDRESS_0XF000
        MDrv_USB_ReleaseXdataWindow1();
        #endif
        return(0);//Fail
      }

      for (ii=2, jj=0; ii<pUsbData[0]; ii+=2, jj++)
            sAttachDevice.bStringManufacture[jj] = pUsbData[ii];

      sAttachDevice.bStringManufacture[jj] = 0;

#if 0
      DrvHostLib_printMsg("bStringManufacture: ");

      for (ii=0; ii<64; ii++)
      {
        DrvHostLib_printData("%x", sAttachDevice.bStringManufacture[ii]);
        if (sAttachDevice.bStringManufacture[ii] == 0)
            break;
      }

      DrvHostLib_printMsg("\n");
#endif
  }

  if (sAttachDevice.sDD.bProduct != 0)
  {
      memcpy(bCMD, OTGH_GETDESCRIPTOR_STR, 8);
      bCMD[2] = sAttachDevice.sDD.bProduct;
      bCMD[4] = sAttachDevice.bStringLanguage[0];
      bCMD[5] = sAttachDevice.bStringLanguage[1];
      bCMD[6] = 2;

      if (flib_Host20_Issue_Control (1,bCMD,2, pUsbData)>0)
      {
        #ifdef USE_XDATA_ADDRESS_0XF000
        MDrv_USB_ReleaseXdataWindow1();
        #endif
        return(0);//Fail
      }

      bCMD[6] = pUsbData[0]; //real string length
      if (flib_Host20_Issue_Control (1,bCMD,bCMD[6], pUsbData)>0)
      {
        #ifdef USE_XDATA_ADDRESS_0XF000
        MDrv_USB_ReleaseXdataWindow1();
        #endif
        return(0);//Fail
      }

      for (ii=2, jj=0; ii<pUsbData[0]; ii+=2, jj++)
            sAttachDevice.bStringProduct[jj] = pUsbData[ii];

      sAttachDevice.bStringProduct[jj] = 0;

#if 0
      DrvHostLib_printMsg("bStringProduct: ");

      for (ii=0; ii<pUsbData[0]-2; ii++)
      {
        DrvHostLib_printData("%x", sAttachDevice.bStringProduct[ii]);
        if (sAttachDevice.bStringProduct[ii] == 0)
            break;
      }

      DrvHostLib_printMsg("\n");
#endif
  }
  //#endif //Skip Get String

Get_Str_Done:
  //<8>.Set Configurarion to 0
  memcpy(bCMD,OTGH_SETCONFIGURATION,8);
  bCMD[2]=sAttachDevice.saCD[0].bConfigurationValue;
  if (flib_Host20_Issue_Control (1,bCMD,0x00,OTGH_NULL)>0)
  {
    #ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1();
    #endif
    return(0);//Fail
  }

  //<9>.Printf the Device-Descriptor/Configuration-Descriptor

  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_ReleaseXdataWindow1();
  #endif
  return (bReturn);
}






//************************************************************************************************************
//************************************************************************************************************
//                          *** Group-4:Structure Function ***
//*************************************************************************************************************
//************************************************************************************************************
//====================================================================
// * Function Name: flib_Host20_InitStructure
// * Description:
//              1.Init qHD for Control
//                qHD_C-->qHD_C-->qHD_C
//              2.Init qHD for Bulk
//                |-------------------------|
//                qHD_C-->qHD_C-->qHD_B-->qHD_B
//
//              3.Init qHD for Interrupt
//              4.Init iTD for ISO (Reserved for feature)
// * Input:Type =0 =>iTD
//              =1 =>qTD
//              =2
// * OutPut: 0:Fail
//           1:ok
//====================================================================
extern UINT8 gPort0_PeridoicRun;
extern UINT8 gPort0_PeridoicRun_qh2;
extern UINT8 xdata gState_Change[HOST20_INTERFACE_NUM_MAX];
extern U8 fram_init_done;
extern long logical_max,logical_min;
void flib_Host20_InitStructure(void)
{
  UINT16  i,j;
  //UINT8 *pData;

  //<1>.Clear memory
  //pData=(UINT8*)Host20_STRUCTURE_qHD_BASE_ADDRESS;
  gPort0_PeridoicRun=0; //reset value, if plug-out and then plug-in cable
  gPort0_PeridoicRun_qh2=0;
  fram_init_done=0;
  logical_max=0;
  logical_min=0; 
  for(j=0;j<HOST20_INTERFACE_NUM_MAX;j++)
  {
    gState_Change[j]=0;
    PORT_DEVICE[j]=0;
  }
  DrvHostLib_printData("Host20_STRUCTURE_qHD_BASE_ADDRESS %x\n", Host20_STRUCTURE_qHD_BASE_ADDRESS);
  for ( i=0 ; i < Host20_qHD_SIZE*Host20_qHD_MAX ; i++)
    XBYTE[Host20_STRUCTURE_qHD_BASE_ADDRESS+i]=0x00;

  DrvHostLib_printData("Host20_STRUCTURE_qTD_BASE_ADDRESS %x\n", Host20_STRUCTURE_qTD_BASE_ADDRESS);
  for ( i=0 ; i < Host20_qTD_SIZE*Host20_qTD_MAX ; i++)
    XBYTE[Host20_STRUCTURE_qTD_BASE_ADDRESS+i]=0x00;

  //<2>.For qTD & iTD & 4K-Buffer Manage init
  for (i=0;i<Host20_qTD_MAX;i++)
    Host20_qTD_Manage[i]=Host20_MEM_FREE;

  //sAttachDevice.bDataBuffer=flib_Host20_GetStructure(Host20_MEM_TYPE_4K_BUFFER);//For Control
  sAttachDevice.bDataBuffer=Host20_DATA_PAGE_BASE_ADDRESS;     //we only support one buffer,only 2K

  DrvHostLib_printData("Host20_TEMP_DATA:%x", Host20_TEMP_DATA);
  DrvHostLib_printData("Host20_Attach_Device_DATA:%x", Host20_Attach_Device_DATA);
  //XBYTE[sAttachDevice.bDataBuffer]=0x55;
  DrvHostLib_printData("bDataBuffer:%x\n",XBYTE[sAttachDevice.bDataBuffer]);
  //while (1){}


  #if 1
  //psHost20_qHD_List_Control[0]->bType=HOST20_HD_Type_QH;
  DrvHostLib_printData("List_control 0:%x\n",(UINT16) &Host20_qHD_List_Control0);
  flib_Host20_Allocate_QHD(&Host20_qHD_List_Control0,HOST20_HD_Type_QH,0,1,0,8);//Address=0,Head=1,EndPt=0,Size
  DrvHostLib_printData("List_control 1:%x\n",(UINT16) &Host20_qHD_List_Control1);
  flib_Host20_Allocate_QHD(&Host20_qHD_List_Control1,HOST20_HD_Type_QH,1,0,0,64);//Address=1,Head=0,EndPt=0,Size

  #endif

  #ifdef Enable_Mass_Storage
  flib_Host20_Allocate_QHD(&Host20_qHD_List_Bulk0,HOST20_HD_Type_QH,1,0,1,64);//Address=1,Head=0,EndPt=1,Size
  flib_Host20_Allocate_QHD(&Host20_qHD_List_Bulk1,HOST20_HD_Type_QH,1,0,2,64);//Address=1,Head=0,EndPt=2,Size
  #endif


  //<3.3>.Link the qHD (contol)
  #if 1
  SetPointer(&(Host20_qHD_List_Control0.bNextQHDPointer3),VirtoPhyAddr((UINT16)&Host20_qHD_List_Control1));
  SetPointer(&(Host20_qHD_List_Control1.bNextQHDPointer3),VirtoPhyAddr((UINT16)&Host20_qHD_List_Control0));
  //SetPointer(&(Host20_qHD_List_Work.bNextQHDPointer3),VirtoPhyAddr((UINT16)&Host20_qHD_List_Work));
  #endif

}

void SetPointer( UINT8 *ptr, UINT32 val)
{
  *(ptr-3)&=0x1f;
  *(ptr-3)|=(UINT8)val & 0xe0;
  *(ptr-2)=(UINT8)(val >> 8);
  *(ptr-1)=(UINT8)(val >> 16);
  *(ptr)=(UINT8)(val >> 24);

}
//====================================================================
// * Function Name: flib_Host20_GetStructure
// * Description:
//
// * Input:Type =0 =>qTD
//              =1 =>iTD
//              =2 =>4K Buffer
// * OutPut: 0:Fail
//           ~0:Addrress
//====================================================================
UINT16 flib_Host20_GetStructure(UINT8 Type)
{
  UINT32 i;
  UINT8 bFound;
  //UINT16 spTempqTD;
  xdata qTD_Structure   *spTempqTD;
  //iTD_Structure  *spTempiTD;
  //siTD_Structure  *spTempsiTD;
  bFound=0;

  //DrvHostLib_printMsg("flib_Host20_GetStructure:");
  switch(Type)
  {
    case Host20_MEM_TYPE_qTD:
         //For qTD

         for (i=0;i<Host20_qTD_MAX;i++)
           if (Host20_qTD_Manage[i]==Host20_MEM_FREE)
           {
               bFound=1;
               Host20_qTD_Manage[i]=Host20_MEM_USED;
               break;
           }


         if (bFound==1)
         {
           //printf("USB base:%lx  \n",USB_BUFFER_START_ADR);

           spTempqTD=(qTD_Structure xdata   *)(Host20_STRUCTURE_qTD_BASE_ADDRESS+i*Host20_qTD_SIZE);
           memset((unsigned char volatile xdata *)spTempqTD ,0, Host20_qTD_SIZE);
           //DrvHostLib_printData("Get QTD:%x",(UINT16)spTempqTD);
           //DrvHostLib_printData("%dth QTD\n", i);
           spTempqTD->bTerminate=1;         //Bit0
           spTempqTD->bStatus_Active=0;             //Bit7
           spTempqTD->bInterruptOnComplete=1;   //Bit15

           spTempqTD->bAlternateTerminate=1;
           spTempqTD->bErrorCounter=3;
           return ((UINT16)spTempqTD);
         }
         else
         {
           DEBUG_USB(DrvHostLib_printMsg("QTD underrun!\n"));
         }



         break;

    default:
         return 0;
         break;

  }

  //Not Found...
  //while(1)
  //{
  //  printf("i:%d",i);
  //}

  return (0);

}


//====================================================================
// * Function Name: flib_Host20_ReleaseStructure
// * Description:
//
// * Input:Type =0 =>qTD
//              =1 =>iTD
//              =2
// * OutPut: 0:Fail
//           ~0:Addrress
//====================================================================
void flib_Host20_ReleaseStructure(UINT8 Type,UINT16 pwAddress)
{
  //UINT8 i;
  UINT16 wReleaseNum;
  UINT8 xdata *pData;

  //printf("release QTD:%x\n",pwAddress);
  pData=(UINT8*)pwAddress;

  switch(Type)
  {
    case Host20_MEM_TYPE_qTD:

         if (pwAddress<Host20_STRUCTURE_qTD_BASE_ADDRESS)
         {
           // kcj 2007-06-07    printf("??? Memory release area fail...\n");
           //while(1);
         }

         if ((pwAddress-Host20_STRUCTURE_qTD_BASE_ADDRESS)==0)
           wReleaseNum=0;
         else
           wReleaseNum=(pwAddress-Host20_STRUCTURE_qTD_BASE_ADDRESS)/Host20_qTD_SIZE;

         if (wReleaseNum>=Host20_qTD_MAX)//ERROR FIX Prevent Tool 070522
         {
           // kcj 2007-06-07    printf("??? Memory release area fail...\n");
           //while(1);
         }

         //DrvHostLib_printData("Release QTD:%x", wReleaseNum);
         //DrvHostLib_printData("%dth QTD\n", wReleaseNum);
         Host20_qTD_Manage[wReleaseNum]=Host20_MEM_FREE;

         // Removed, for improve the performance,
         // because we will clear qtd structure when we get it, so don��t have to do it when release it.
         //for (i=0;i<Host20_qTD_SIZE ;i++) //qTD size=32 bytes
         //  *(pData+i)=0;


         break;

    default:

         // kcj 2007-06-07  printf("??? Memory release type fail...\n");
         //while(1);

         break;

  }


}
//====================================================================
// * Function Name: flib_Host20_QHD_Control_Init
// * Description:
//
// * Input:Type =0 =>qTD
//              =1 =>iTD
//              =2
// * OutPut: 0:Fail
//           ~0:Addrress
//====================================================================
void flib_Host20_QHD_Control_Init(void)
{


  //<1>.Init Control-0/1
  Host20_qHD_List_Control0.bEdSpeed=gSpeed;
  //printf("bEdSpeed:%bx\n",sAttachDevice.bSpeed);
  Host20_qHD_List_Control0.bInactiveOnNextTransaction=0;
  Host20_qHD_List_Control0.bDataToggleControl=1;

  Host20_qHD_List_Control1.bEdSpeed=gSpeed;
  Host20_qHD_List_Control1.bInactiveOnNextTransaction=0;
  Host20_qHD_List_Control1.bDataToggleControl=1;

#ifdef Enable_Mass_Storage
  //<2>.Init Bulk-0/1
  Host20_qHD_List_Bulk0.bEdSpeed=gSpeed;
  Host20_qHD_List_Bulk0.bInactiveOnNextTransaction=0;
  Host20_qHD_List_Bulk0.bDataToggleControl=0;

  Host20_qHD_List_Bulk1.bEdSpeed=gSpeed;
  Host20_qHD_List_Bulk1.bInactiveOnNextTransaction=0;
  Host20_qHD_List_Bulk1.bDataToggleControl=0;
#endif

  //printf("enable aynch \n");
  //<12>.Enable Asynchronous

  //mbHost20_USBCMD_AsynchronousEnable_Set();    //Temp;;Bruce
  //printf("0x10:%02bx",XBYTE[0x2410]);


}

//====================================================================
// * Function Name: flib_Host20_Allocate_QHD
// * Description:
//
// * Input:Type =0 =>qTD
//              =1 =>iTD
//              =2
// * OutPut: 0:Fail
//           ~0:Addrress
//====================================================================
void flib_Host20_Allocate_QHD(qHD_Structure  *psQHTemp,UINT8 bNextType,UINT8 bAddress,UINT8 bHead,UINT8 bEndPt, UINT32 wMaxPacketSize)
{
  qTD_Structure xdata *spTempqTD;
  UINT32 sp;

  //<1>.Set the QHead

  //<1.1>.qHD_1 Word
  // sp=;
  //printf("psQHTemp:%lx\n",(UINT32)psQHTemp);
  psQHTemp->bTerminate=0;             //Bit0
  psQHTemp->bType=bNextType;          //Bit2~1

  //<1.2>.qHD_2 Word
  psQHTemp->bDeviceAddress=bAddress;             //Bit0~6
  //?? psQHTemp->bInactiveOnNextTransaction:1; //Bit7           //??
  psQHTemp->bEdNumber=bEndPt;                  //Bit11~8
  //?? psQHTemp->bEdSpeed:2;                   //Bit13~12
  //??  psQHTemp->bDataToggleControl=1;         //Bit14:1 for control 0 for Bulk
  psQHTemp->bHeadOfReclamationListFlag=bHead; //Bit15
  psQHTemp->bMaxPacketSize0=wMaxPacketSize;            //Bit16~26
  psQHTemp->bMaxPacketSize1=wMaxPacketSize>>8;
  //psQHTemp->bControlEdFlag:1;             //Bit27
  //psQHTemp->bNakCounter=15;                //Bit28~31
  psQHTemp->bNakCounter=Host20_QHD_Nat_Counter;
  //<1.3>.qHD_3 Word
  //psQHTemp->bInterruptScheduleMask:8;     //Bit0~7
  //psQHTemp->bSplitTransactionMask:8;      //Bit8~15
  //psQHTemp->bHubAddr:7;                   //Bit16~22
  //psQHTemp->bPortNumber:7;                //Bit23~29
  //psQHTemp->bHighBandwidth:2;             //Bit30~31

  psQHTemp->bOverlay_NextTerminate=1;
  psQHTemp->bOverlay_AlternateNextTerminate=1;

  //<2>.allocate dumy qTD
  #if 1
  //<2.1>.Allocate qTD
  spTempqTD=(qTD_Structure xdata *) flib_Host20_GetStructure(0);//0=>qTD
  //DrvHostLib_printData("spTempqTD:%x\n",(UINT16)(spTempqTD));
  sp=VirtoPhyAddr((UINT16)spTempqTD);
  //<2.2>.Put qTD to QHD
  //DrvHostLib_printData("sp:%x\n",(UINT16)(sp>>16));
  //DrvHostLib_printData("%x\n",(UINT16)(sp));
  SetPointer(&(psQHTemp->bOverlay_NextqTD3),sp);
  #endif
  //<2.3>.Active the qTD
  psQHTemp->bOverlay_NextTerminate=0;

}

//====================================================================
// * Function Name: flib_Host20_CheckingForResult_QHD
// * Description:
// * Input:Type
// * OutPut:
//====================================================================
UINT8 flib_Host20_CheckingForResult_QHD(qHD_Structure *spTempqHD)
{
  UINT8 bQHStatus;
  U16 wIntStatus;

  wIntStatus=mwHost20_USBSTS_Rd();
  //printf("USB int:%x\n",wIntStatus);

  //<2>.Checking for the Error type interrupt => Halt the system
  if (wIntStatus&HOST20_USBINTR_SystemError)
  {
    DEBUG_USB(DrvHostLib_printMsg("???System Error... Halt the system...\n "));
    sAttachDevice.bSendStatusError=1;

  }
  if (wIntStatus&HOST20_USBINTR_USBError) {
    mwHost20_USBSTS_USBError_Set();
    sAttachDevice.bSendStatusError=1;
  }

  if (sAttachDevice.bSendStatusError==0)
    return HOST20_OK;

  //<1>.Analysis the qHD Status
  sAttachDevice.bSendStatusError=0;
  //printf("??? USB Error Interrupt Event...\n");
  bQHStatus=spTempqHD->bOverlay_Status;

  if (bQHStatus&HOST20_qTD_STATUS_Halted)
  {
    if (bQHStatus&HOST20_qTD_STATUS_TransactionError)
    {
      //printf("\r\n transaction error!!",0);
      gUsbStatus=USB_TRANS_ERROR;              //trans error
      return HOST20_TRANSACTION_ERROR;
    }

    //printf("??? qHD Status => Halted (<1>.Stall/<2>.Babble/<3>.Error Counter=0)...(Device Not Supported...)\n");
    spTempqHD->bOverlay_Status=0;      //clear halt status
    spTempqHD->bOverlay_TotalBytes0=0;
    spTempqHD->bOverlay_TotalBytes1=0;
    spTempqHD->bOverlay_Direction=0;
    spTempqHD->ArrayBufferPointer[0].Byte0=0;   //clear offset
    spTempqHD->ArrayBufferPointer[0].Byte1=0;   //clear offset
    if (bQHStatus==HOST20_qTD_STATUS_Halted)            //no other error status
    {
      //printf("STALL\n");
      return HOST20_DEVICE_STALL;
    }
  }
  if (bQHStatus&HOST20_qTD_STATUS_BufferError)
  {
    // kcj 2007-06-07   printf("??? qHD Status => HOST20_qTD_STATUS_BufferError...\n");
  }
  if (bQHStatus&HOST20_qTD_STATUS_Babble)
  {
    DEBUG_USB(DrvHostLib_printMsg("??? qHD Status => HOST20_qTD_STATUS_Babble...\n"));
    //sAttachDevice.bSendStatusError=0;         //don't fix babble error for Bert
    #ifdef Enable_Low_Temperature_Patch
    if(gSpeed==0)
    {
      //printf("\r\n CDR toggle!!",0);
      XBYTE[gUTMI_BASE+0x0a]^=0x10; //invert CDR_CLOCK
      XBYTE[gUTMI_BASE+0x06]|=0x03; //reset UTMI
      XBYTE[gUTMI_BASE+0x06]&=0xfc;
    }
    #endif
    spTempqHD->bOverlay_Status=0;
    return HOST20_OK;

  }
  if (bQHStatus&HOST20_qTD_STATUS_MissMicroFrame)
  {
    // kcj 2007-06-07   printf("??? qHD Status => HOST20_qTD_STATUS_MissMicroFrame...\n");
  }
  //<2>.Clear the status
  spTempqHD->bOverlay_Status=0;

  return HOST20_FAIL;


}
qTD_Structure  *GetPointer(UINT8*  ptr)
{
  UINT32 temp;
  //DrvHostLib_printMsg("GetPointer\n");
  temp=(*(ptr-3) & 0xe0)+(*(ptr-2)<<8) +(*(ptr-1)<<16) +(*(ptr)<<24);
  //DrvHostLib_printData("PhyAddr:0x%x\n", (UINT16)(temp>>16));
  //DrvHostLib_printData("%x\n", (UINT16)(temp));
  temp=PhytoVirAddr(temp);
  //DrvHostLib_printData("=>VirAddr:%x\n", (UINT16)(temp>>16));
  //DrvHostLib_printData("%x\n", (UINT16)(temp));
  return((qTD_Structure *)temp);

}

#if 1
U16  TotalBytes;

//====================================================================
// * Function Name: flib_Host20_Send_qTD
// * Description:
//   Case-1:1qTD
//   Case-2:2qTD
//   Case-3:3qTD above
// * Input:Type
// * OutPut: 0 => OK
//           1 => TimeOut
//====================================================================
UINT8 flib_Host20_Send_qTD(qTD_Structure  *spHeadqTD ,qHD_Structure  *spTempqHD,U16 wTimeOutSec)
{
  UINT8 bExitLoop,bReturnValue;
  qTD_Structure xdata *spNewDumyqTD;
  qTD_Structure xdata *spOldDumyqTD;
  qTD_Structure xdata *spReleaseqTD;
  qTD_Structure xdata *spReleaseqTDNext;
  qTD_Structure xdata *spLastqTD;
  //UINT32 wDummyTemp;
  //UINT32 xdata *pwData;
  #ifdef ATV_SERISE_USE
  UINT32 wTimes;
  UINT32 StartTime=0;
  #else
  UINT16 wTimes;
  UINT32 StartTime;
  UINT32 LastTime;
  UINT16 FrameIdx=0;

  #endif

  #ifdef ATV_SERISE_USE
  wTimeOutSec=wTimeOutSec;
  #endif

  //if (wTimeOutSec==0)
  //{
  //  DEBUG_USB(printf("error, timeout sec is zero\n"));
  //}
  spOldDumyqTD=GetPointer(&(spTempqHD->bOverlay_NextqTD3));

  //spTempqHD->bOverlay_Status|=HOST20_qTD_STATUS_Halted;
  while (mbHost20_USBCMD_AsynchronousEnable_Rd())
  {
    mbHost20_USBCMD_AsynchronousEnable_Clr();//pause asynchronous scheduler
  }

  //spHeadqTD->bTerminate=1;            //set to terminate
  memcpy(spOldDumyqTD,spHeadqTD,Host20_qTD_SIZE);
  //spOldDumyqTD->bStatus_Halted=1;
  if(RecoveryFlag&0x1)
    spOldDumyqTD->bDataToggle=DataToggleBackup;

  //spOldDumyqTD->bStatus_Active=0;

  //<2>.Prepare new dumy qTD
  spNewDumyqTD=spHeadqTD;
  //DrvHostLib_printData("spNewDumyqTD->0x%x",(U16)spNewDumyqTD);
  memset((unsigned char volatile xdata *)spNewDumyqTD ,0, Host20_qTD_SIZE);
  spNewDumyqTD->bTerminate=1;
  //spNewDumyqTD->bAlternateTerminate=1;
  //spNewDumyqTD->bStatus_Halted=1;
  //<3>.Find spLastqTD & link spLastqTD to NewDumyqTD & Set NewDumyqTD->T=1
  spLastqTD=spOldDumyqTD;
  //DrvHostLib_printData("spLastqTD->0x%x",(U16)spLastqTD);
  wTimes = 0;
  while(spLastqTD->bTerminate==0) {
    spLastqTD=GetPointer(&(spLastqTD->bNextQTDPointer3));
  };

  SetPointer(&(spLastqTD->bNextQTDPointer3),VirtoPhyAddr((UINT32)spNewDumyqTD));
  spLastqTD->bTerminate=0;

  //Link Alternate qTD pointer
  SetPointer(&(spLastqTD->bAlternateQTDPointer3),VirtoPhyAddr((UINT32)spNewDumyqTD));

  spLastqTD->bAlternateTerminate=0;

  //<4>.Set OldDumyqTD->Active=1
  //gwLastqTDSendOK=0;
  //sAttachDevice.psSendLastqTD=spLastqTD;
  sAttachDevice.bSendStatusError=0;

  //Dump_QTD(spOldDumyqTD);
  //Dump_QTD(spNewDumyqTD);

  while (mwHost20_USBSTS_Rd() & 0x3b)
  {
    mwHost20_USBSTS_Set(0x3b);      //clear interrupt, don't clear port change int
  }
  //bExitLoop=0;
  //spOldDumyqTD->bStatus_Halted=0;
  //spOldDumyqTD->bStatus_Halted=0;
  spOldDumyqTD->bStatus_Active=1;
  while (mbHost20_USBCMD_AsynchronousEnable_Rd()==0)
  {
    mbHost20_USBCMD_AsynchronousEnable_Set();//re start asynchronous scheduler
  }
  //spTempqHD->bOverlay_Status&=~HOST20_qTD_STATUS_Halted;

  //wait until asynchronous scheduler is idle

  //mbHost20_USBCMD_RunStop_Set();

  //spTempqHD->bOverlay_Status&=~0x40;          //clr HALT bit, start this queue head
  //XBYTE[0x2410]|=1;//start run
  //wDummyTemp=0;
  wTimes=0;
  #ifndef ATV_SERISE_USE
  StartTime=MDrv_Timer_GetTime0();
   LastTime=StartTime;
   FrameIdx=mwHost20_FrameIndex_Rd();
  msAPI_Timer_ResetWDT();
  #endif
  //<5>.Waiting for result
  //EAL=0;
  while (1)
  {
    #ifdef ATV_SERISE_USE
    wTimes++;
    #endif
    if ((spOldDumyqTD->bStatus_Active==0) &&
        ( mwHost20_USBSTS_Rd()& HOST20_USBINTR_CompletionOfTransaction))              //wait until Status_Active become 0
    {
      bReturnValue=HOST20_OK;

      break;
      //bExitLoop=1;
      //printf("bReturn:%02bx\n",    bReturnValue);
    }
    //if (bExitLoop) break;
    #if 1
    if (mwHost20_USBSTS_Rd()&(HOST20_USBINTR_SystemError+HOST20_USBINTR_USBError))
    {
      bReturnValue=HOST20_OK;
      //if (spOldDumyqTD->bStatus_Active==1)
      //{
      //  printf("something wrong..USBINTR:%02bx\n",mwHost20_USBSTS_Rd());
      //  printf("QH status:%02bx\n",spTempqHD->bOverlay_Status);
      //}
      break;            //USB interrupt happened
    }
    #endif
    #if 1
    //if (gwOTG_Timer_Counter>wTimeOutSec)
    //  wTimes++;
    //if (wTimes > wTimeOutSec*400)
    if (mwHost20_PORTSC_ConnectStatus_Rd()==0)
    {
        //return HOST20_FATAL;
        bReturnValue = HOST20_FATAL;
        break;
    }
    //if (mwHost20_PORTSC_ConnectChange_Rd())
    //{
    //  gDeviceFatalError=TRUE;
    //  mwHost20_PORTSC_ConnectChange_Set();
    //  return HOST20_FATAL;          //usb has been plug out and in
    //}
    #ifndef ATV_SERISE_USE
    #ifdef CERAMAL_SERISE_USE
    if (MDrv_Timer_TimeDifference(MDrv_Timer_GetTime0(),StartTime) > (U32)wTimeOutSec *1000)
    #else
      if (MDrv_Timer_DiffTimeFromNow(LastTime) >(U32)3)     //over 1 mini sec
        {

            if (mwHost20_FrameIndex_Rd()==FrameIdx)
            {
                MINI_DEBUG2(printf("1C stop,timeout !!\n"));
                gUsbStatus=USB_TIMEOUT;
                bReturnValue=HOST20_FATAL;  //ESD , USB hang,should be timeout
                break;
            }
            FrameIdx=mwHost20_FrameIndex_Rd();
            LastTime=MDrv_Timer_GetTime0();

        }
    if (MDrv_Timer_DiffTimeFromNow(StartTime) > (U32)wTimeOutSec *1000)
    #endif
    #else
    if(wTimes>0x80000)
    #endif
    {
      #if 0
      printf("\nQH1:\n");
      Dump_Data(0xf800,0x30);
      printf("\nQH2:\n");
      Dump_Data(0xf840,0x30);
      printf("\nQH3:\n");
      Dump_Data(0xf880,0x30);
      printf("\nQH4:\n");
      Dump_Data(0xf8c0,0x30);
      printf("\n");
      Dump_Data(0xfa80,0x20);
      printf("\n");
      Dump_Data(0xfb00,0x20);
      printf("\n");
      Dump_Data(0xfae0,0x20);
      printf("\n");
      Dump_Data(0xfaa0,0x20);
      printf("\n");
      Dump_Data(0xfac0,0x20);

      printf("\nQH->%x\n",(U16)spTempqHD);
      //Dump_Data((U16)spTempqHD,0x30);

      printf("\nqtd->\n");
      Dump_Data((U16)spOldDumyqTD,0x20);
      printf("\nEHCI REG:\n");
      Dump_Data(0x2400,0x50);
      #endif
      DrvHostLib_printData("\nQtd->%x\n",(U16)spOldDumyqTD);
      DrvHostLib_printData("spTempqHD->bPID:%d\n", spOldDumyqTD->bPID);
      bExitLoop=2;
      bReturnValue=HOST20_FATAL;
      DrvHostLib_printMsg("cc\n");
      gUsbStatus=USB_TIMEOUT;
      //gDeviceFatalError=TRUE;

      MINI_DEBUG2(DrvHostLib_printData("Time Out:%02bx\n",XBYTE[gUHC_BASE+30]));
      //while(1);
      break;
    }
    #endif
    //}
  }
  #ifndef ATV_SERISE_USE
  msAPI_Timer_ResetWDT();       //in case spend too much time at polling
  #endif
  while (mbHost20_USBCMD_AsynchronousEnable_Rd())
  {
    mbHost20_USBCMD_AsynchronousEnable_Clr();//pause asynchronous scheduler
  }
  //<6>.Checking the Result
  if (bReturnValue!=HOST20_FATAL)
    bReturnValue=flib_Host20_CheckingForResult_QHD(spTempqHD);
  if (bReturnValue==HOST20_TRANSACTION_ERROR)
  {
    DrvHostLib_printMsg("Transaction Error\n");

    spTempqHD->bOverlay_Status=0;      //clear halt status
    //SetPointer(&(spTempqHD->bOverlay_NextqTD3),0);
    //SetPointer(&(spTempqHD->bOverlay_AlternateqTD3),0);
    SetPointer(&(spTempqHD->bOverlay_CurqTD3),VirtoPhyAddr((UINT16)spNewDumyqTD));

    //printf("TotalBytes:%x\n",TotalBytes);

    //printf("spNewDumyqTD->%x\n",(U16)spNewDumyqTD);
    //printf("spOldDumyqTD->%x\n",(U16)spOldDumyqTD);
    //Dump_Data((U16)spOldDumyqTD,0x20);
    //printf("\nQH->%x\n",(U16)spTempqHD);
    //Dump_Data((U16)spTempqHD,0x20);

  }
  TotalBytes=  ((spOldDumyqTD->bTotalBytes1 )<<8)+(spOldDumyqTD->bTotalBytes0);

  //<5>.Release the all the qTD (Not include spNewDumyqTD)
  #if 1
  spReleaseqTD=spOldDumyqTD;
  do {
    //spReleaseqTDNext=((UINT32)(spReleaseqTD->bNextQTDPointerL))<<5 + ((UINT32)(spReleaseqTD->bNextQTDPointerH))<<16  ;
    spReleaseqTDNext=GetPointer(&(spReleaseqTD->bNextQTDPointer3));
    DataToggleBackup=spOldDumyqTD->bDataToggle;

    flib_Host20_ReleaseStructure(Host20_MEM_TYPE_qTD,(UINT16)spReleaseqTD);

    spReleaseqTD=spReleaseqTDNext;
  } while(((UINT32)spReleaseqTD)!=((UINT32)spNewDumyqTD));
  #endif

 return (bReturnValue);
}

//====================================================================
// * Function Name: flib_Host20_Send_qTD
// * Description:
//   Case-1:1qTD
//   Case-2:2qTD
//   Case-3:3qTD above
// * Input:Type
// * OutPut: 0 => OK
//           1 => TimeOut
//====================================================================
extern UINT8 gPort0_PeridoicRun;
extern UINT8 gPort0_PeridoicRun_qh2;
UINT8 flib_Host20_Send_qTD_Int_Pipe(qTD_Structure  *spHeadqTD ,qHD_Structure  *spTempqHD,U16 wTimeOutSec, U8 u8Interface)
{
  UINT8 /*bExitLoop,*/bReturnValue;
  qTD_Structure xdata *spNewDumyqTD;
  qTD_Structure xdata *spOldDumyqTD;
//  qTD_Structure xdata *spReleaseqTD;
//  qTD_Structure xdata *spReleaseqTDNext;
  qTD_Structure xdata *spLastqTD;
  #ifdef ATV_SERISE_USE
  UINT32 wTimes;
  UINT32 StartTime=0;
  #else
  UINT16 wTimes;
  UINT32 StartTime;
  UINT32 LastTime;
  UINT16 FrameIdx=0;

  #endif

  #ifdef ATV_SERISE_USE
  wTimeOutSec=wTimeOutSec;
  #endif

  spOldDumyqTD=GetPointer(&(spTempqHD->bOverlay_NextqTD3));

#if 0
  while (mbHost20_USBCMD_AsynchronousEnable_Rd())
  {
    mbHost20_USBCMD_AsynchronousEnable_Clr();//pause asynchronous scheduler
  }
#endif

  memcpy(spOldDumyqTD,spHeadqTD,Host20_qTD_SIZE);
  if(RecoveryFlag&0x1)
    spOldDumyqTD->bDataToggle=DataToggleBackup;


  //<2>.Prepare new dumy qTD
  spNewDumyqTD=spHeadqTD;
  //DrvHostLib_printData("spNewDumyqTD->0x%x",(U16)spNewDumyqTD);
  memset((unsigned char volatile xdata *)spNewDumyqTD ,0, Host20_qTD_SIZE);
  spNewDumyqTD->bTerminate=1;
  //spNewDumyqTD->bAlternateTerminate=1;
  //spNewDumyqTD->bStatus_Halted=1;
  //<3>.Find spLastqTD & link spLastqTD to NewDumyqTD & Set NewDumyqTD->T=1
  spLastqTD=spOldDumyqTD;
  //DrvHostLib_printData("spLastqTD->0x%x",(U16)spLastqTD);
  wTimes = 0;
  while(spLastqTD->bTerminate==0) {
    spLastqTD=GetPointer(&(spLastqTD->bNextQTDPointer3));
  };

  SetPointer(&(spLastqTD->bNextQTDPointer3),VirtoPhyAddr((UINT32)spNewDumyqTD));
  spLastqTD->bTerminate=0;

  //Link Alternate qTD pointer
  SetPointer(&(spLastqTD->bAlternateQTDPointer3),VirtoPhyAddr((UINT32)spNewDumyqTD));

  spLastqTD->bAlternateTerminate=0;

  //<4>.Set OldDumyqTD->Active=1
  //gwLastqTDSendOK=0;
  //sAttachDevice.psSendLastqTD=spLastqTD;
  sAttachDevice.bSendStatusError=0;

  //Dump_QTD(spOldDumyqTD);
  //Dump_QTD(spNewDumyqTD);

  while (mwHost20_USBSTS_Rd() & 0x3b)
  {
        mwHost20_USBSTS_Set(0x3b);      //clear interrupt, don't clear port change int
  }
  //bExitLoop=0;
  //spOldDumyqTD->bStatus_Halted=0;
if(PORT_DEVICE[u8Interface]==1)  
  gPort0_PeridoicRun=1;
else if(PORT_DEVICE[u8Interface]==2)
    gPort0_PeridoicRun_qh2=1;

  spOldDumyqTD->bStatus_Active=1;
#if 0
  while (mbHost20_USBCMD_AsynchronousEnable_Rd()==0)
  {
    mbHost20_USBCMD_AsynchronousEnable_Set();//re start asynchronous scheduler
  }
#endif

#if 0
  //spTempqHD->bOverlay_Status&=~HOST20_qTD_STATUS_Halted;

  //wait until asynchronous scheduler is idle

  //mbHost20_USBCMD_RunStop_Set();

  //spTempqHD->bOverlay_Status&=~0x40;          //clr HALT bit, start this queue head
  //XBYTE[0x2410]|=1;//start run
  //wDummyTemp=0;
  wTimes=0;
  #ifndef ATV_SERISE_USE
  StartTime=MDrv_Timer_GetTime0();
  LastTime=StartTime;
  FrameIdx=mwHost20_FrameIndex_Rd();
  msAPI_Timer_ResetWDT();
  #endif
  //<5>.Waiting for result
  //EAL=0;
  while (1)
  {
    #ifdef ATV_SERISE_USE
    wTimes++;
    #endif
    if ((spOldDumyqTD->bStatus_Active==0) &&
        ( mwHost20_USBSTS_Rd()& HOST20_USBINTR_CompletionOfTransaction))              //wait until Status_Active become 0
    {
      bReturnValue=HOST20_OK;

      break;
      //bExitLoop=1;
      //printf("bReturn:%02bx\n",    bReturnValue);
    }
    //if (bExitLoop) break;
    #if 1
    if (mwHost20_USBSTS_Rd()&(HOST20_USBINTR_SystemError+HOST20_USBINTR_USBError))
    {
      bReturnValue=HOST20_OK;
      //if (spOldDumyqTD->bStatus_Active==1)
      //{
      //  printf("something wrong..USBINTR:%02bx\n",mwHost20_USBSTS_Rd());
      //  printf("QH status:%02bx\n",spTempqHD->bOverlay_Status);
      //}
      break;            //USB interrupt happened
    }
    #endif
    #if 1
    //if (gwOTG_Timer_Counter>wTimeOutSec)
    //  wTimes++;
    //if (wTimes > wTimeOutSec*400)
    if (mwHost20_PORTSC_ConnectStatus_Rd()==0)
    {
        //return HOST20_FATAL;
        bReturnValue = HOST20_FATAL;
        break;
    }
    //if (mwHost20_PORTSC_ConnectChange_Rd())
    //{
    //  gDeviceFatalError=TRUE;
    //  mwHost20_PORTSC_ConnectChange_Set();
    //  return HOST20_FATAL;          //usb has been plug out and in
    //}
    #ifndef ATV_SERISE_USE
    #ifdef CERAMAL_SERISE_USE
    if (MDrv_Timer_TimeDifference(MDrv_Timer_GetTime0(),StartTime) > (U32)wTimeOutSec *1000)
    #else
      if (MDrv_Timer_DiffTimeFromNow(LastTime) >(U32)3)     //over 1 mini sec
        {

            if (mwHost20_FrameIndex_Rd()==FrameIdx)
            {
                MINI_DEBUG2(printf("1C stop,timeout !!\n"));
                gUsbStatus=USB_TIMEOUT;
                bReturnValue=HOST20_FATAL;  //ESD , USB hang,should be timeout
                break;
            }
            FrameIdx=mwHost20_FrameIndex_Rd();
            LastTime=MDrv_Timer_GetTime0();

        }
    if (MDrv_Timer_DiffTimeFromNow(StartTime) > (U32)wTimeOutSec *1000)
    #endif
    #else
    if(wTimes>/*0x80000*/0xF0000000)
    #endif
    {
      #if 0
      printf("\nQH1:\n");
      Dump_Data(0xf800,0x30);
      printf("\nQH2:\n");
      Dump_Data(0xf840,0x30);
      printf("\nQH3:\n");
      Dump_Data(0xf880,0x30);
      printf("\nQH4:\n");
      Dump_Data(0xf8c0,0x30);
      printf("\n");
      Dump_Data(0xfa80,0x20);
      printf("\n");
      Dump_Data(0xfb00,0x20);
      printf("\n");
      Dump_Data(0xfae0,0x20);
      printf("\n");
      Dump_Data(0xfaa0,0x20);
      printf("\n");
      Dump_Data(0xfac0,0x20);

      printf("\nQH->%x\n",(U16)spTempqHD);
      //Dump_Data((U16)spTempqHD,0x30);

      printf("\nqtd->\n");
      Dump_Data((U16)spOldDumyqTD,0x20);
      printf("\nEHCI REG:\n");
      Dump_Data(0x2400,0x50);
      #endif
      DrvHostLib_printData("\nQtd->%x\n",(U16)spOldDumyqTD);
      DrvHostLib_printData("spTempqHD->bPID:%d\n", spOldDumyqTD->bPID);
      bExitLoop=2;
      bReturnValue=HOST20_FATAL;
      gUsbStatus=USB_TIMEOUT;
      //gDeviceFatalError=TRUE;

      MINI_DEBUG2(DrvHostLib_printData("Time Out:%02bx\n",XBYTE[gUHC_BASE+30]));
      //while(1);
      break;
    }

    #endif
    //}
  }
#endif

#if 0
  while (mbHost20_USBCMD_AsynchronousEnable_Rd())
  {
    mbHost20_USBCMD_AsynchronousEnable_Clr();//pause asynchronous scheduler
  }
#endif

  //<6>.Checking the Result
#if 0
  if (bReturnValue!=HOST20_FATAL)
    bReturnValue=flib_Host20_CheckingForResult_QHD(spTempqHD);
  if (bReturnValue==HOST20_TRANSACTION_ERROR)
  {
    DrvHostLib_printMsg("Transaction Error\n");

    spTempqHD->bOverlay_Status=0;      //clear halt status
    //SetPointer(&(spTempqHD->bOverlay_NextqTD3),0);
    //SetPointer(&(spTempqHD->bOverlay_AlternateqTD3),0);
    SetPointer(&(spTempqHD->bOverlay_CurqTD3),VirtoPhyAddr((UINT16)spNewDumyqTD));

    //printf("TotalBytes:%x\n",TotalBytes);

    //printf("spNewDumyqTD->%x\n",(U16)spNewDumyqTD);
    //printf("spOldDumyqTD->%x\n",(U16)spOldDumyqTD);
    //Dump_Data((U16)spOldDumyqTD,0x20);
    //printf("\nQH->%x\n",(U16)spTempqHD);
    //Dump_Data((U16)spTempqHD,0x20);

  }
  TotalBytes=  ((spOldDumyqTD->bTotalBytes1 )<<8)+(spOldDumyqTD->bTotalBytes0);
#endif
  //<5>.Release the all the qTD (Not include spNewDumyqTD)
  #if 0
  spReleaseqTD=spOldDumyqTD;
  do {
    DrvHostLib_printData("spReleaseqTD:%x", (U16)spReleaseqTD);
    //spReleaseqTDNext=((UINT32)(spReleaseqTD->bNextQTDPointerL))<<5 + ((UINT32)(spReleaseqTD->bNextQTDPointerH))<<16  ;
    spReleaseqTDNext=GetPointer(&(spReleaseqTD->bNextQTDPointer3));
    DataToggleBackup=spOldDumyqTD->bDataToggle;

    flib_Host20_ReleaseStructure(Host20_MEM_TYPE_qTD,(UINT16)spReleaseqTD);

    spReleaseqTD=spReleaseqTDNext;
  } while(((UINT32)spReleaseqTD)!=((UINT32)spNewDumyqTD));
  #endif

 return (bReturnValue);
}

#endif
#else
BYTE code drvhostlib[] = {0};
void mshostlibDummy(void)
{
    BYTE xdata x = drvhostlib[0];
}
#endif
