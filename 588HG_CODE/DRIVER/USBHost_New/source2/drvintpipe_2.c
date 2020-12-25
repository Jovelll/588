#pragma LARGE
#include "board.h"
#if ENABLE_USB_INPUT




#include "USBHostConfig_2.h"
//#include "datatype.h"
#include "UsbHostDataDef.h"
#include "drvhost200_2.h"
#include "drvHostLib_2.h"
#include "drvUsbMain_2.h"
//#include "sysinfo.h"
//#include "drvtimer.h"
//#include "R8051XC.h"
#include "drvusb.h"

//#include "msAPI_Timer.h"
#include "common.h"


#define DrvintPipe2_DEBUG    1
#if ENABLE_DEBUG&&DrvintPipe2_DEBUG
    #define DrvintPipe2_printData(str, value)   printData(str, value)
    #define DrvintPipe2_printMsg(str)           printMsg(str)
#else
    #define DrvintPipe2_printData(str, value)
    #define DrvintPipe2_printMsg(str)
#endif

 Periodic_Frame_List_Structure   xdata Host20_FramList_Port2 _at_ FRAME_LIST_DATA;
 qHD_Structure xdata *pHost20_qHD_List_Intr_Port2;
 qHD_Structure xdata Host20_qHD_List_Intr_Port2  _at_  (Host20_STRUCTURE_qHD_BASE_ADDRESS+4*Host20_qHD_SIZE);

 qHD_Structure xdata *pHost20_qHD2_List_Intr_Port2;
 qHD_Structure xdata Host20_qHD2_List_Intr_Port2  _at_  (Host20_STRUCTURE_qHD_BASE_ADDRESS+5*Host20_qHD_SIZE);

U8 fram_init_done_Port2;
//extern  Host20_Attach_Device_Structure xdata psAttachDevice_Port2;
extern UINT8 gSpeed_Port2;

void SetFrameList_Port2(UINT8 i)
{
    Host20_FramList_Port2.sCell[i].bTerminal=0;
    Host20_FramList_Port2.sCell[i].bType=HOST20_HD_Type_QH;
    SetPointer_Port2(&(Host20_FramList_Port2.sCell[i].bLinkPointer3),VirtoPhyAddr((UINT16)pHost20_qHD_List_Intr_Port2));
}

void SetFrameList_qh2_Port2(UINT8 i)
{
    Host20_FramList_Port2.sCell[i].bTerminal=0;
    Host20_FramList_Port2.sCell[i].bType=HOST20_HD_Type_QH;
    SetPointer_Port2(&(Host20_FramList_Port2.sCell[i].bLinkPointer3),VirtoPhyAddr((UINT16)pHost20_qHD2_List_Intr_Port2));
}
//====================================================================
// * Function Name: flib_Host20_Interrupt_Init
// * Description:
//   //Reserve=> <1>.If Periodic Enable => Disable Periodic
//   <2>.Disable all the Frame List (Terminal=1)
//   <3>.Hang the Interrupt-qHD-1 to Frame List
//
// * Input: wInterval=1~16 => Full Speed => 1ms ~ 32 sec
//                            High Speed => 125us ~ 40.96 sec
// * OutPut:
//====================================================================
extern UINT8 frame_number;
void  flib_Host20_Interrupt_Init_Port2(UINT8 bAddr, U8 u8Interface, U8 QH_number)
{
 UINT32 i,j;
// UINT32 wSize;
 UINT8 xdata *pData;
 UINT16 wForceInterval=0;
 UINT8 EndPaddr,MaxPacketSize;
  //struct usb_interface_descriptor *ip;

     //  ip=&dev->config.if_desc[0];

 //<1>.Disable the Periodic
       flib_Host20_Periodic_Setting_Port2(HOST20_Disable);

if(fram_init_done_Port2==0)
{
       mbHost20_USBCMD_FrameListSize_Set(HOST20_USBCMD_FrameListSize_256);
  //<3>.Link qHD to the FameListCell by wInterval
  //    printf("clear frame list data \n");
       pData=(UINT8 *)&Host20_FramList_Port2;
       for (j=0;j<FRAME_LIST_SIZE;j++)
            *(pData+j)=0;
       // printf("OK\n");

        for (i=0;i< Host20_Preiodic_Frame_List_MAX ;i++)
            Host20_FramList_Port2.sCell[i].bTerminal=1;
       //Find the Interval-X
      //  Host20_FramList_Port2.sCell[0].bLinkPointer=((UINT32)(pHost20_qHD_List_Intr_Port2))>>5;

 //<4>.Set Periodic Base Address
       //  printf("FrameListPhy:%lx\n",VirtoPhyAddr((UINT16)&Host20_FramList_Port2));

     mwHost20_PeriodicBaseAddr_Set(VirtoPhyAddr((UINT16)&Host20_FramList_Port2));
     fram_init_done_Port2=1;
}

if(QH_number==0)
{
 //<2>.Init qHD for Interrupt(7~9) Scan the ED
       pHost20_qHD_List_Intr_Port2=&Host20_qHD_List_Intr_Port2;       //use bulk's QH, we do'nt have to support bulk at HID anyway
       pData=(UINT8*)pHost20_qHD_List_Intr_Port2;
       for (j=0 ; j< sizeof(qHD_Structure) ;j++)
            *(pData+j)=0;

      //  printf("battribute:%02bx\n", psAttachDevice_Port2.saCD[0].sInterface[0].sED[0].bED_bmAttributes);
       EndPaddr=  psAttachDevice_Port2->saCD[0].sInterface[u8Interface].sED[0].bED_EndpointAddress & 0x7f;
       DrvintPipe2_printData("interrupt addr:%x\n",EndPaddr);
       DrvintPipe2_printData("pHost20_qHD_List_Intr:%x", (UINT16) pHost20_qHD_List_Intr_Port2);
       MaxPacketSize=psAttachDevice_Port2->saCD[0].sInterface[u8Interface].sED[0].bED_wMaxPacketSizeLowByte;
       DrvintPipe2_printData("MaxPacketSize:%x\n",MaxPacketSize);
       if(MaxPacketSize<8)
            MaxPacketSize=8; 
       flib_Host20_Allocate_QHD_Port2(pHost20_qHD_List_Intr_Port2,HOST20_HD_Type_QH,bAddr,0,EndPaddr,MaxPacketSize);//Address=0,Head=1,EndPt=0,Size
       pHost20_qHD_List_Intr_Port2->bHighBandwidth=1;
       pHost20_qHD_List_Intr_Port2->bInterruptScheduleMask=1;
       pHost20_qHD_List_Intr_Port2->bEdSpeed= gSpeed_Port2;
       pHost20_qHD_List_Intr_Port2->bDataToggleControl=0;

       SetPointer_Port2(&(pHost20_qHD_List_Intr_Port2->bNextQHDPointer3),VirtoPhyAddr((UINT16)pHost20_qHD_List_Intr_Port2));

       //       pHost20_qHD_List_Intr_Port2->bNextQHDPointer=(((UINT32)pHost20_qHD_List_Intr_Port2)>>5);      //point to itself
       pHost20_qHD_List_Intr_Port2->bTerminate=1;          //terminated

       //    psHost20_FramList_Port2=&Host20_FramList_Port2;



        //SetFrameList_Port2(0);
        //SetFrameList_Port2(50);
        //SetFrameList_Port2(100);
        //SetFrameList_Port2(150);
        //SetFrameList_Port2(200);
        for(i=0 ; i<50 ; i++)
        {
            j=(frame_number*i)+(frame_number/2);
            if(j > 256)
                break;
            //DrvintPipe2_printData("frame_number_2:%d\n",j);
            SetFrameList_Port2(j);
        }         
}
else if(QH_number==1)
{
 //<2>.Init qHD for Interrupt(7~9) Scan the ED
       pHost20_qHD2_List_Intr_Port2=&Host20_qHD2_List_Intr_Port2;       //use bulk's QH, we do'nt have to support bulk at HID anyway
       pData=(UINT8*)pHost20_qHD2_List_Intr_Port2;
       for (j=0 ; j< sizeof(qHD_Structure) ;j++)
            *(pData+j)=0;

      //  printf("battribute:%02bx\n", psAttachDevice_Port2.saCD[0].sInterface[0].sED[0].bED_bmAttributes);
       EndPaddr=  psAttachDevice_Port2->saCD[0].sInterface[u8Interface].sED[0].bED_EndpointAddress & 0x7f;
       DrvintPipe2_printData("interrupt addr:%x\n",EndPaddr);
       DrvintPipe2_printData("pHost20_qHD_List_Intr:%x", (UINT16) pHost20_qHD2_List_Intr_Port2);
       MaxPacketSize=psAttachDevice_Port2->saCD[0].sInterface[u8Interface].sED[0].bED_wMaxPacketSizeLowByte;
       DrvintPipe2_printData("MaxPacketSize:%x\n",MaxPacketSize);
       if(MaxPacketSize<8)
            MaxPacketSize=8; 
       flib_Host20_Allocate_QHD_Port2(pHost20_qHD2_List_Intr_Port2,HOST20_HD_Type_QH,bAddr,0,EndPaddr,0x10);//Address=0,Head=1,EndPt=0,Size
       pHost20_qHD2_List_Intr_Port2->bHighBandwidth=1;
       pHost20_qHD2_List_Intr_Port2->bInterruptScheduleMask=1;
       pHost20_qHD2_List_Intr_Port2->bEdSpeed= gSpeed_Port2;
       pHost20_qHD2_List_Intr_Port2->bDataToggleControl=0;

       SetPointer_Port2(&(pHost20_qHD2_List_Intr_Port2->bNextQHDPointer3),VirtoPhyAddr((UINT16)pHost20_qHD2_List_Intr_Port2));

       //       pHost20_qHD_List_Intr_Port2->bNextQHDPointer=(((UINT32)pHost20_qHD_List_Intr_Port2)>>5);      //point to itself
       pHost20_qHD2_List_Intr_Port2->bTerminate=1;          //terminated

       //    psHost20_FramList_Port2=&Host20_FramList_Port2;



        //SetFrameList_qh2_Port2(10);
        //SetFrameList_qh2_Port2(60);
        //SetFrameList_qh2_Port2(110);
        //SetFrameList_qh2_Port2(160);
        //SetFrameList_qh2_Port2(210);
        for(i=0 ; i<50 ; i++)
        {
            j=(frame_number*i)+(frame_number/2);
            if(j > 256)
                break;
            //DrvintPipe2_printData("frame_number_2:%d\n",j);
            SetFrameList_qh2_Port2(j);
        }         
}
       // printf("FrameList:%x\n",FRAME_LIST_DATA);
       // printf("first QH:%lx\n",VirtoPhyAddr((UINT16)pHost20_qHD_List_Intr_Port2));

        //while (1){}
 #if 0
        x=0;
        while(waIntervalMap[x]<wForceInterval)
        {
          x++;
          if (x>10)
             {
                 printf("Interval Input Error...\n");
                 return;
             }
          };

        for (i=0;i<Host20_Preiodic_Frame_List_MAX;i=i+waIntervalMap[x])
             {
               Host20_FramList_Port2.sCell[i].bLinkPointer=((UINT32)(pHost20_qHD_List_Intr_Port2))>>5;
               Host20_FramList_Port2.sCell[i].bTerminal=0;
               Host20_FramList_Port2.sCell[i].bType=HOST20_HD_Type_QH;
             }
#endif

 //<5>.Enable the periodic
     flib_Host20_Periodic_Setting_Port2(HOST20_Enable);

}
//====================================================================
// * Function Name: flib_Host20_Periodic_Setting
// * Description:
// * Input:
// * OutPut:
//====================================================================
void flib_Host20_Periodic_Setting_Port2(UINT8 bOption)
{
  if (bOption==HOST20_Enable)
     {
     //<1>.If Already enable => return
           if (mwHost20_USBSTS_PeriodicStatus_Rd()>0)
               return ;

     //<2>.Disable Periodic
           mbHost20_USBCMD_PeriodicEnable_Set();

     //<3>.Polling Status
          // while(mwHost20_USBSTS_PeriodicStatus_Rd()==0);

     }else
     if (bOption==HOST20_Disable)
                {
                 //<1>.If Already Disable => return
                       if (mwHost20_USBSTS_PeriodicStatus_Rd()==0)
                           return ;

                 //<2>.Enable Periodic
                       mbHost20_USBCMD_PeriodicEnable_Clr();

                 //<3>.Polling Status
                 //      while(mwHost20_USBSTS_PeriodicStatus_Rd()>0);

                }
              //  else
               // {
                  //  printf("??? Input Error 'flib_Host20_Periodic_Setting'...");
               // }


}
#if 1
//====================================================================
// * Function Name: flib_Host20_Issue_Interrupt
// * Description:
// * Input:
// * OutPut:
//====================================================================
#if 0
void  flib_Host20_Issue_Interrupt(UINT16 buf,UINT16 hwSize)
{

    qTD_Structure xdata *spTempqTD;


    //<1>.Fill TD
        spTempqTD =flib_Host20_GetStructure(Host20_MEM_TYPE_qTD); //The qTD will be release in the function "Send"
        spTempqTD->bTotalBytes0=(UINT8)hwSize;               //Bit16~30
        spTempqTD->bTotalBytes1=(UINT8)(hwSize>>8);          //Bit16~30
        // printf("buf:%lx\n",VirtoPhyAddr(buf));
        FillBufferArray2(spTempqTD, VirtoPhyAddr(buf),8);


    //<2>.Analysis the Direction
        spTempqTD->bPID=HOST20_qTD_PID_IN;
    //<3>.Send TD
        flib_Host20_Send_qTD_Int_Pipe(spTempqTD ,pHost20_qHD_List_Intr_Port2,3);

}
#else
extern U8 DataToggleBackupPort2;
UINT8 gPort2_PeridoicRun = 0;
UINT8 gPort2_PeridoicRun_qh2 = 0;
static qTD_Structure xdata *spFirstqTD;
static qTD_Structure xdata *spFirstqTD2;
extern UINT8 xdata gState_Change_Port2[HOST20_INTERFACE_NUM_MAX];
extern void FillBufferArray2_Port2(qTD_Structure xdata *spTempqTD, UINT32 bpDataPage,UINT16 size);

void  flib_Host20_Issue_Interrupt_qh2_Port2(UINT16 buf,UINT16 hwSize, U8 u8Interface)
{
    qTD_Structure xdata *spTempqTD;
#if 1
    if (gPort2_PeridoicRun_qh2 == 1)
    {
        // If periodic still running, return, else release first qtd point
        if (spFirstqTD2->bStatus_Active==1)
        {
            gState_Change_Port2[u8Interface]= 0;
            return;
        }
        else
        {
          gState_Change_Port2[u8Interface]= 1;
          gPort2_PeridoicRun_qh2 = 0;
          //DrvintPipe2_printData("spFirstqTD:%x", (UINT16)spFirstqTD2);
          DataToggleBackupPort2=spFirstqTD2->bDataToggle;
          flib_Host20_ReleaseStructure_Port2(Host20_MEM_TYPE_qTD,(UINT16)spFirstqTD2);
        }
    }

    // Keep first qtd point
    spFirstqTD2=GetPointer_Port2(&(pHost20_qHD2_List_Intr_Port2->bOverlay_NextqTD3));
#endif

    //<1>.Fill TD
    spTempqTD =flib_Host20_GetStructure_Port2(Host20_MEM_TYPE_qTD); //The qTD will be release in the function "Send"
    spTempqTD->bTotalBytes0=(UINT8)hwSize;               //Bit16~30
    spTempqTD->bTotalBytes1=(UINT8)(hwSize>>8);          //Bit16~30
    // printf("buf:%lx\n",VirtoPhyAddr(buf));
    FillBufferArray2_Port2(spTempqTD, VirtoPhyAddr(buf),hwSize);

    //<2>.Analysis the Direction
    spTempqTD->bPID=HOST20_qTD_PID_IN;
    //<3>.Send TD
    flib_Host20_Send_qTD_Int_Pipe_Port2(spTempqTD ,pHost20_qHD2_List_Intr_Port2,3,u8Interface);
}

void  flib_Host20_Issue_Interrupt_Port2(UINT16 buf,UINT16 hwSize, U8 u8Interface)
{
    qTD_Structure xdata *spTempqTD;
#if 1
    if (gPort2_PeridoicRun == 1)
    {
        // If periodic still running, return, else release first qtd point
        if (spFirstqTD->bStatus_Active==1)
        {
            gState_Change_Port2[u8Interface]= 0;
            return;
        }
        else
        {
          gState_Change_Port2[u8Interface]= 1;
          gPort2_PeridoicRun = 0;
          //DrvintPipe2_printData("spFirstqTD:%x", (UINT16)spFirstqTD);
          DataToggleBackupPort2=spFirstqTD->bDataToggle;
          flib_Host20_ReleaseStructure_Port2(Host20_MEM_TYPE_qTD,(UINT16)spFirstqTD);
        }
    }

    // Keep first qtd point
    spFirstqTD=GetPointer_Port2(&(pHost20_qHD_List_Intr_Port2->bOverlay_NextqTD3));
#endif

    //<1>.Fill TD
    spTempqTD =flib_Host20_GetStructure_Port2(Host20_MEM_TYPE_qTD); //The qTD will be release in the function "Send"
    spTempqTD->bTotalBytes0=(UINT8)hwSize;               //Bit16~30
    spTempqTD->bTotalBytes1=(UINT8)(hwSize>>8);          //Bit16~30
    // printf("buf:%lx\n",VirtoPhyAddr(buf));
    FillBufferArray2_Port2(spTempqTD, VirtoPhyAddr(buf),hwSize);

    //<2>.Analysis the Direction
    spTempqTD->bPID=HOST20_qTD_PID_IN;
    //<3>.Send TD
    flib_Host20_Send_qTD_Int_Pipe_Port2(spTempqTD ,pHost20_qHD_List_Intr_Port2,3,u8Interface);
}

#endif
#endif

#else
#define DrvintPipe2_DEBUG  0
#endif //#if ENABLE_HDMI

#if (!(ENABLE_DEBUG &&DrvintPipe2_DEBUG)) || (!ENABLE_USB_INPUT)
BYTE code drvintpipe_2[1] = {0};
void msdrvintpipe_2Dummy(void)
{
    BYTE xdata i = drvintpipe_2[0];
}
#endif

