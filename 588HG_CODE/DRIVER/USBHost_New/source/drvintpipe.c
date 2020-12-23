#pragma LARGE
#include "board.h"
#if ENABLE_USB_INPUT

//#include "msAPI_Timer.h"



#include "USBHostConfig.h"
//#include "datatype.h"
#include "UsbHostDataDef.h"
#include "drvhost200.h"
#include "drvHostLib.h"
#include "drvUsbMain.h"
//#include "sysinfo.h"
//#include "drvtimer.h"
//#include "R8051XC.h"
#include "drvusb.h"

#include "common.h"

#define DrvintPipe_DEBUG    1
#if ENABLE_DEBUG&&DrvintPipe_DEBUG
    #define DrvintPipe_printData(str, value)   printData(str, value)
    #define DrvintPipe_printMsg(str)           printMsg(str)
#else
    #define DrvintPipe_printData(str, value)
    #define DrvintPipe_printMsg(str)
#endif

Periodic_Frame_List_Structure   xdata Host20_FramList _at_ FRAME_LIST_DATA;
qHD_Structure xdata *pHost20_qHD_List_Intr;
qHD_Structure xdata Host20_qHD_List_Intr  _at_  (Host20_STRUCTURE_qHD_BASE_ADDRESS+4*Host20_qHD_SIZE);
qHD_Structure xdata *pHost20_qHD2_List_Intr;
qHD_Structure xdata Host20_qHD2_List_Intr  _at_  (Host20_STRUCTURE_qHD_BASE_ADDRESS+5*Host20_qHD_SIZE);

U8 fram_init_done;
extern  Host20_Attach_Device_Structure xdata sAttachDevice;
extern UINT8 gSpeed;
extern U8 xdata PORT_DEVICE[HOST20_INTERFACE_NUM_MAX];
UINT8 frame_number=20;//16;//32;//64;//128;//16;

void SetFrameList(UINT8 i)
{
    Host20_FramList.sCell[i].bTerminal=0;
    Host20_FramList.sCell[i].bType=HOST20_HD_Type_QH;
    SetPointer(&(Host20_FramList.sCell[i].bLinkPointer3),VirtoPhyAddr((UINT16)pHost20_qHD_List_Intr));
}

void SetFrameList_qh2(UINT8 i)
{
    Host20_FramList.sCell[i].bTerminal=0;
    Host20_FramList.sCell[i].bType=HOST20_HD_Type_QH;
    SetPointer(&(Host20_FramList.sCell[i].bLinkPointer3),VirtoPhyAddr((UINT16)pHost20_qHD2_List_Intr));
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
void  flib_Host20_Interrupt_Init(UINT8 bAddr, U8 u8Interface, U8 QH_number)
{
 UINT32 i,j;
// UINT32 wSize;
 UINT8 xdata *pData;
 UINT16 wForceInterval=0;
 UINT8 EndPaddr,MaxPacketSize;

 //<1>.Disable the Periodic
       flib_Host20_Periodic_Setting(HOST20_Disable);
if(fram_init_done==0)
{
       mbHost20_USBCMD_FrameListSize_Set(HOST20_USBCMD_FrameListSize_256); 
  //<3>.Link qHD to the FameListCell by wInterval
  //    printf("clear frame list data \n");
       pData=(UINT8 *)&Host20_FramList;
       for (j=0;j<FRAME_LIST_SIZE;j++)
            *(pData+j)=0;
       // printf("OK\n");

       for (i=0;i< Host20_Preiodic_Frame_List_MAX ;i++)
           Host20_FramList.sCell[i].bTerminal=1;
       //Find the Interval-X
      //  Host20_FramList.sCell[0].bLinkPointer=((UINT32)(pHost20_qHD_List_Intr))>>5;
 //<4>.Set Periodic Base Address
       //  printf("FrameListPhy:%lx\n",VirtoPhyAddr((UINT16)&Host20_FramList));

     mwHost20_PeriodicBaseAddr_Set(VirtoPhyAddr((UINT16)&Host20_FramList));
     fram_init_done=1;
}

if(QH_number==0)
{
//u8Interface=0;
 //<2>.Init qHD for Interrupt(7~9) Scan the ED
       pHost20_qHD_List_Intr=&Host20_qHD_List_Intr;       //use bulk's QH, we do'nt have to support bulk at HID anyway
       pData=(UINT8*)pHost20_qHD_List_Intr;
       for (j=0 ; j< sizeof(qHD_Structure) ;j++)
            *(pData+j)=0;

      //  printf("battribute:%02bx\n", sAttachDevice.saCD[0].sInterface[0].sED[0].bED_bmAttributes);
       EndPaddr=  sAttachDevice.saCD[0].sInterface[u8Interface].sED[0].bED_EndpointAddress & 0x7f;
       DrvintPipe_printData("interrupt addr_%x:",u8Interface);
       DrvintPipe_printData("%x\n",EndPaddr);
       DrvintPipe_printData("pHost20_qHD_List_Intr:%x", (UINT16) pHost20_qHD_List_Intr);
       MaxPacketSize=sAttachDevice.saCD[0].sInterface[u8Interface].sED[0].bED_wMaxPacketSizeLowByte;
       DrvintPipe_printData("MaxPacketSize:%x\n",MaxPacketSize);
       if(MaxPacketSize<8)
            MaxPacketSize=8; 
       flib_Host20_Allocate_QHD(pHost20_qHD_List_Intr,HOST20_HD_Type_QH,bAddr,0,EndPaddr,MaxPacketSize);//Address=0,Head=1,EndPt=0,Size
       pHost20_qHD_List_Intr->bHighBandwidth=1;
       pHost20_qHD_List_Intr->bInterruptScheduleMask=1;
       pHost20_qHD_List_Intr->bEdSpeed= gSpeed;
       pHost20_qHD_List_Intr->bDataToggleControl=0;

       SetPointer(&(pHost20_qHD_List_Intr->bNextQHDPointer3),VirtoPhyAddr((UINT16)pHost20_qHD_List_Intr));

       //       pHost20_qHD_List_Intr->bNextQHDPointer=(((UINT32)pHost20_qHD_List_Intr)>>5);      //point to itself
       pHost20_qHD_List_Intr->bTerminate=1;          //terminated

       //    psHost20_FramList=&Host20_FramList;

        //SetFrameList(0);
        //SetFrameList(50);
        //SetFrameList(100);
        //SetFrameList(150);
        //SetFrameList(200);
        for(i=0 ; i<50 ; i++)
        {
            j=frame_number*i;
            if(j > 256)
                break;
            //DrvintPipe_printData("frame_number:%d\n",j);
            SetFrameList(j);
        } 
}
#if 1
else if(QH_number==1)
{
//u8Interface=1;
 //<2>.Init qHD for Interrupt(7~9) Scan the ED
       pHost20_qHD2_List_Intr=&Host20_qHD2_List_Intr;       //use bulk's QH, we do'nt have to support bulk at HID anyway
       pData=(UINT8*)pHost20_qHD2_List_Intr;
       for (j=0 ; j< sizeof(qHD_Structure) ;j++)
            *(pData+j)=0;

      //  printf("battribute:%02bx\n", sAttachDevice.saCD[0].sInterface[0].sED[0].bED_bmAttributes);
       EndPaddr=  sAttachDevice.saCD[0].sInterface[u8Interface].sED[0].bED_EndpointAddress & 0x7f;
       DrvintPipe_printData("interrupt addr_%x:",u8Interface);
       DrvintPipe_printData("%x\n",EndPaddr);
       DrvintPipe_printData("pHost20_qHD_List_Intr:%x", (UINT16) pHost20_qHD2_List_Intr);
       MaxPacketSize=sAttachDevice.saCD[0].sInterface[u8Interface].sED[0].bED_wMaxPacketSizeLowByte;
       DrvintPipe_printData("MaxPacketSize:%x\n",MaxPacketSize);
       if(MaxPacketSize<8)
            MaxPacketSize=8; 
       flib_Host20_Allocate_QHD(pHost20_qHD2_List_Intr,HOST20_HD_Type_QH,bAddr,0,EndPaddr,0x10);//Address=0,Head=1,EndPt=0,Size
       pHost20_qHD2_List_Intr->bHighBandwidth=1;
       pHost20_qHD2_List_Intr->bInterruptScheduleMask=1;
       pHost20_qHD2_List_Intr->bEdSpeed= gSpeed;
       pHost20_qHD2_List_Intr->bDataToggleControl=0;

       SetPointer(&(pHost20_qHD2_List_Intr->bNextQHDPointer3),VirtoPhyAddr((UINT16)pHost20_qHD2_List_Intr));

       //       pHost20_qHD_List_Intr->bNextQHDPointer=(((UINT32)pHost20_qHD_List_Intr)>>5);      //point to itself
       pHost20_qHD2_List_Intr->bTerminate=1;          //terminated

       //    psHost20_FramList=&Host20_FramList;

        //SetFrameList_qh2(10);
        //SetFrameList_qh2(60);
        //SetFrameList_qh2(110);
        //SetFrameList_qh2(160);
        //SetFrameList_qh2(210);
        for(i=0 ; i<50 ; i++)
        {
            j=frame_number*i;
            if(j > 256)
                break;
            //DrvintPipe_printData("frame_number:%d\n",j);
            SetFrameList_qh2(j);
        }        
}
#endif
 //<5>.Enable the periodic
     flib_Host20_Periodic_Setting(HOST20_Enable);

}
//====================================================================
// * Function Name: flib_Host20_Periodic_Setting
// * Description:
// * Input:
// * OutPut:
//====================================================================
void flib_Host20_Periodic_Setting(UINT8 bOption)
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
        flib_Host20_Send_qTD_Int_Pipe(spTempqTD ,pHost20_qHD_List_Intr,3);

}
#else
extern U8 DataToggleBackup;
extern U8 DataToggleBackup_qh2;
UINT8 gPort0_PeridoicRun = 0;
UINT8 gPort0_PeridoicRun_qh2 = 0;
static qTD_Structure xdata *spFirstqTD;
static qTD_Structure xdata *spFirstqTD2;
extern UINT8 xdata gState_Change[HOST20_INTERFACE_NUM_MAX];

void  flib_Host20_Issue_Interrupt(UINT16 buf,UINT16 hwSize, U8 u8Interface)
{
    qTD_Structure xdata *spTempqTD;
#if 1
    if (gPort0_PeridoicRun == 1)
    {
        // If periodic still running, return, else release first qtd point
        if (spFirstqTD->bStatus_Active==1)
        {
            gState_Change[u8Interface] = 0;
            return;
        }
        else
        {
          gState_Change[u8Interface] = 1;
          gPort0_PeridoicRun = 0;
          //DrvintPipe_printData("spFirstqTD:%x", (UINT16)spFirstqTD);
          DataToggleBackup=spFirstqTD->bDataToggle;
          flib_Host20_ReleaseStructure(Host20_MEM_TYPE_qTD,(UINT16)spFirstqTD);
        }
    }
    // Keep first qtd point
    spFirstqTD=GetPointer(&(pHost20_qHD_List_Intr->bOverlay_NextqTD3));
    //DrvintPipe_printData("spFirstqTD:%x", (UINT16)spFirstqTD);
#endif

    //<1>.Fill TD
    spTempqTD =flib_Host20_GetStructure(Host20_MEM_TYPE_qTD); //The qTD will be release in the function "Send"
    spTempqTD->bTotalBytes0=(UINT8)hwSize;               //Bit16~30
    spTempqTD->bTotalBytes1=(UINT8)(hwSize>>8);          //Bit16~30
    FillBufferArray2(spTempqTD, VirtoPhyAddr(buf),hwSize);
    //<2>.Analysis the Direction
    spTempqTD->bPID=HOST20_qTD_PID_IN;
    //<3>.Send TD
    flib_Host20_Send_qTD_Int_Pipe(spTempqTD ,pHost20_qHD_List_Intr,3,u8Interface);   
}

void  flib_Host20_Issue_Interrupt_qh2(UINT16 buf,UINT16 hwSize, U8 u8Interface)
{
    qTD_Structure xdata *spTempqTD;
#if 1
    if (gPort0_PeridoicRun_qh2== 1)
    {
        // If periodic still running, return, else release first qtd point
        if (spFirstqTD2->bStatus_Active==1)
        {
            gState_Change[u8Interface] = 0;
            return;
        }
        else
        {
          gState_Change[u8Interface] = 1;
          gPort0_PeridoicRun_qh2= 0;
          //DrvintPipe_printData("spFirstqTD2:%x", (UINT16)spFirstqTD2);
          DataToggleBackup_qh2=spFirstqTD2->bDataToggle;
          flib_Host20_ReleaseStructure(Host20_MEM_TYPE_qTD,(UINT16)spFirstqTD2);
        }
    }
    // Keep first qtd point
    spFirstqTD2=GetPointer(&(pHost20_qHD2_List_Intr->bOverlay_NextqTD3));
    //DrvintPipe_printData("spFirstqTD2:%x", (UINT16)spFirstqTD2);
 
#endif
    //<1>.Fill TD
    spTempqTD =flib_Host20_GetStructure(Host20_MEM_TYPE_qTD); //The qTD will be release in the function "Send"
    spTempqTD->bTotalBytes0=(UINT8)hwSize;               //Bit16~30
    spTempqTD->bTotalBytes1=(UINT8)(hwSize>>8);          //Bit16~30
    FillBufferArray2(spTempqTD, VirtoPhyAddr(buf),hwSize);
    //<2>.Analysis the Direction
    spTempqTD->bPID=HOST20_qTD_PID_IN;
    //<3>.Send TD
    flib_Host20_Send_qTD_Int_Pipe(spTempqTD ,pHost20_qHD2_List_Intr,3,u8Interface);     
}    

#endif
#endif
#else
#define DrvintPipe_DEBUG  0
#endif //#if ENABLE_HDMI

#if (!(ENABLE_DEBUG &&DrvintPipe_DEBUG)) || (!ENABLE_USB_INPUT)
BYTE code DrvintpipeNullData[1] = {0};


void msdrvintpipeDummy(void)
{
    BYTE xdata i = DrvintpipeNullData[0];
}
#endif

