#pragma LARGE
#include "board.h"
#if ENABLE_USB_INPUT



///////////////////////////////////////////////////////////////////////////////
//
//  File name: drvUsbMain.C
//  Version: 0.1
//  Date: 2004/9/20
//  Company: Faraday Tech. Corp.
///////////////////////////////////////////////////////////////////////////////
#include "USBHostConfig_2.h"
//#include <stdlib.h>
//#include <stdarg.h>
#ifndef ATV_SERISE_USE
#include <stdio.h>
#endif
#include <string.h>
//#include "chipset.h"
//#include "flib.h"
//#include <MsCommon.h>
#include "drvUsbHostConfig.h"
#ifndef ATV_SERISE_USE
#include "datatype.h"
#else
#include "UsbHostDataDef.h"
#endif
#include "drvHostLib_2.h"
#include "drvHost200_2.h"
#include "drvUsbMain_2.h"
#ifndef ATV_SERISE_USE
#include "sysinfo.h"
#include "drvtimer.h"
#endif
#include "drvusb.h"
#ifdef Enable_Mass_Storage
#include "drvscsi_2.h"
#endif
//#include <R8051XC.h>
#ifdef ATV_SERISE_USE
#include "common.h"
//#include "uart.h"
//#include "command.h"
#endif
#ifdef CERAMAL_SERISE_USE
#include "drvtimer.h"
#include "Board.h"
#include "drvUTMI.h"
#else
//#include "msAPI_Timer.h"
#endif
#ifdef JUNIOR_SERIAL_USE
#include "ms_reg.h"
#endif
#ifdef OnePort_OTG_EHCI
#include "MsCompiler.h"
#include "msusb.h"
#include "msDRC.h"
#include "msconfig.h"
#include "mscpu.h"
#include "drvisr.h"
#endif

#include "drvUSBHost_PTP_2.h"


#define DrvUsbMain2_DEBUG    1
#if ENABLE_DEBUG && DrvUsbMain2_DEBUG
    #define DrvUsbMain2_printData(str, value)   printData(str, value)
    #define DrvUsbMain2_printMsg(str)           printMsg(str)
#else
    #define DrvUsbMain2_printData(str, value)
    #define DrvUsbMain2_printMsg(str)
#endif

#define DRV_USB_DBG(x)    x;
#define MINI_DEBUG(x)     x;

U8 gUsbStatus_Port2=USB_OK;
U8 gUsbTimeout_Port2=5;
#if 1
extern U8 gUsbDeviceState;
extern U8 gUsbDeviceState_Port2;
#else
U8 gUsbDeviceState_Port2=POWER_SAVING;
#ifdef USB_POWER_SAVING_MODE
extern U8 gUsbDeviceState;
#endif
#endif
U8  NowIsHubPort2;

extern U8 gSpeed_Port2;
#if 0
extern UINT8 xdata *pUsbData_Port2;
#else
extern  UINT8 xdata pUsbData_Port2[512];
extern U8 xdata PORT2_DEVICE[HOST20_INTERFACE_NUM_MAX];
#endif
static U32 original_xdwin1_Base_Port2=0xFFFFFFFF; //use 0xFFFFFFFF to represent void
static U8 xdwin1_lockCount_Port2=0;
#define STR_C51_VERSION(x) "C51:"##  #x

#define C51_VERSION(x) STR_C51_VERSION(x)

char* usb_host_p2_c51version = C51_VERSION(__C51__);

U8 code USB_VBuf_Port2[32] = {'M', 'S', 'V', 'C', '0', '0',             // 0, 0 fixed
                        'Z', '2',                                   // Library ID
#ifdef   DTV_STANDARD_LIB
                        'A', '0', '0', '1', '4', '0',               // build number
#endif
#ifdef ATV_SERISE_USE
                        'B', '0', '0', '1', '2', '3',               // build number
#endif
#ifdef CERAMAL_SERISE_USE
                        'C', '0', '0', '1', '2', '3',               // build number
#endif

                        '0', '0', '0', '0', '0', '0', '0', '0',     // change list 46058
                        'A', '4', 'A', '0', '0', '0', '0', '0', '0',// A4: Saturn, A:LG
                        'T'};

#ifdef OnePort_OTG_EHCI
U8 USBMode=0; //set default status=OTG device mode ,{default=0, otg=1, uhci=2}
extern U8 volatile usbUSBState;
#endif

//extern U8 RecoveryFlag_Port2;
//extern BOOLEAN gDeviceFatalError;

#ifdef OnePort_OTG_EHCI
extern void MDrv_OTG_Init(void);
extern U8 usbIsUSBConfiged(void);
enum
{
    USBHostMode_disconnect = 0,
    USBHostMode_connect,
    USBDeivceMode_disconnect,
    USBDeivceMode_connect
};
#endif

extern void drvUSBHost_TurnOffPowerDownMode(void);
extern void drvUSBHost_TurnOnPowerDownMode(void);
extern BOOLEAN drvUSBHost_isPowerSaveModeEnable();
//extern void drvUSBHost_UTMIInitial();
extern void drvUSBHost_UTMIInitial_Port2();
extern void UsbTurnOffPowerDownMode(void);
extern void UsbTurnOnPowerDownMode(void);
extern BOOLEAN drvUSBHost_isEnable();
extern BOOLEAN drvUSBHost_isPort2Enable();
#ifdef  Enable_PTP
extern U8 drvUSBHost_PTP_Init(U8 u8UsbPort);
#endif
extern U8 drvUSBHost_HID_Init(U8 u8UsbPort, U8 u8Interface);
#if 0
void UsbGetVerStringPort2(U8 *pVerString)
{

    memcpy(pVerString, &USB_VBuf_Port2[0], sizeof(USB_VBuf_Port2));

}
#endif
#if 0
void SetUsbTimeoutPort2(U8 x)
{
    gUsbTimeout_Port2=x;
}
#endif
#if 0
//============================================================================= ok
//      FUSBH200_PHY_Reset()
//      Description:
//      input: none
//      output: none
//=============================================================================
void FUSBH200_PHY_Reset(void)
{
  //Reset Phy
  //waiting for mdwOTGC_Control_B_SESS_END_Rd

  // kcj 2007-06-07 printf(">>> Waiting for B_SESS_END & Phy Reset...(under 5sec)\n");

  mwOTG20_Control_Phy_Reset_Set();
  MDrv_Timer_Delayms(1);
  mwOTG20_Control_Phy_Reset_Clr();
}
#endif


#ifdef USE_XDATA_ADDRESS_0XF000
void MDrv_USB_SetXdataWindow1_Port2(void)
{
  if(xdwin1_lockCount_Port2==0xFF)
  {
    DRV_USB_DBG( DrvUsbMain2_printMsg("error!! MAX allowed lock reached\n"));
  }

  if(xdwin1_lockCount_Port2==0)
  {
    #ifdef JUNIOR_SERIAL_USE
    U16 _win1Base = (XBYTE[REG_2BCD]<<8 | XBYTE[REG_2BCC]);
    original_xdwin1_Base_Port2=(U32)_win1Base;
    XBYTE[REG_2BCD] = (USB_BUFFER_START_ADR_4K_ALIGN_Var_Port2>>12)>>8;
    XBYTE[REG_2BCC] = (USB_BUFFER_START_ADR_4K_ALIGN_Var_Port2>>12)>>0;
    #else
    U16 _win1Base = (XBYTE[XDMIU_REG_BASE + 13]<<8 | XBYTE[XDMIU_REG_BASE + 12]);
    original_xdwin1_Base_Port2=(U32)_win1Base;
    XBYTE[XDMIU_REG_BASE + 13] = (USB_BUFFER_START_ADR_4K_ALIGN_Var_Port2>>12)>>8;
    XBYTE[XDMIU_REG_BASE + 12] = (USB_BUFFER_START_ADR_4K_ALIGN_Var_Port2>>12)>>0;
    #endif
  }
  xdwin1_lockCount_Port2++;
  //DRV_USB_DBG(printf("==== WIN1 LOCKED BY USB, COUNT=%bu\n",xdwin1_lockCount););
}
void MDrv_USB_ReleaseXdataWindow1_Port2(void)
{
  if(xdwin1_lockCount_Port2==0)
  {
    DRV_USB_DBG(DrvUsbMain2_printMsg("error!! no lock to release\n"));
  }

  if(xdwin1_lockCount_Port2==1)
  {
    #ifdef JUNIOR_SERIAL_USE
    XBYTE[REG_2BCD] = ((U16)original_xdwin1_Base_Port2)>>8;
    XBYTE[REG_2BCC] = ((U16)original_xdwin1_Base_Port2)>>0;
    #else
    XBYTE[XDMIU_REG_BASE + 13] = ((U16)original_xdwin1_Base_Port2)>>8;
    XBYTE[XDMIU_REG_BASE + 12] = ((U16)original_xdwin1_Base_Port2)>>0;
    #endif
    original_xdwin1_Base_Port2=0xFFFFFFFF;
  }
  xdwin1_lockCount_Port2--;
  //DRV_USB_DBG(printf("==== WIN1 UNLOCKED BY USB, COUNT=%bu\n",xdwin1_lockCount););

}
#endif
#ifdef Enable_Burning_Test
#ifndef CERAMAL_SERISE_USE
extern void msAPI_Timer_ResetWDT(void);
#endif
//U8 buf2[512];
void UsbTestPort2()
{
  U16 i,j;
  U32 idx=0,xxx;
  U8 VailLun,LunIndex;
  U8 xdata *buf2;

  DrvUsbMain2_printData("\r\n USB Port2 Burning Test\n",0);
  //printf("maxlun :%02bx lunbyte:%02bx\n",maxlun,lunbyte);
  //MDrv_UsbHost_Init();

  VailLun=MDrv_GET_MASS_VALID_LUN_PORT2();
  switch (VailLun)
  {
    case 0x01:
             LunIndex=0;
             break;
    case 0x02:
             LunIndex=1;
             break;
    case 0x04:
             LunIndex=2;
             break;
    case 0x08:
             LunIndex=3;
             break;
    case 0x10:
             LunIndex=4;
             break;
    case 0x20:
             LunIndex=5;
             break;
    case 0x40:
             LunIndex=6;
             break;
      case 0x80:
             LunIndex=7;
             break;
  }

  buf2=(U8 xdata*)(0xFE00);

  for (i=0 ; i < 0x200 ; i++)
  {
    buf2[i]= (i & 0xff);
  }

  //printf("\r\n addr=%x",(VirtoPhyAddr((U32)buf1)>>16));
  //printf(",%x",(VirtoPhyAddr((U32)buf1)));
  DrvUsbMain2_printData("gSpeed: %bd\n", gSpeed_Port2);
  while (1)
  {
    #ifndef CERAMAL_SERISE_USE
    msAPI_Timer_ResetWDT();
    #endif
    idx++;
    //if ((idx & 0x3f)==0)
    {
      DrvUsbMain2_printData("\r\n Loop:%x",(U16)(idx>>16));
      DrvUsbMain2_printData(",%x",(U16)(idx));
    }
    //MDrv_MIU_Copy
    xxx=idx % 10000;
    if (MDrv_UsbBlockWriteFromMIU_Port2(LunIndex,50+xxx,1,VirtoPhyAddr((U32)buf2))==FALSE)
    {
      DrvUsbMain2_printMsg("\r\n write failed\n",0);
      break;
    }
    for (j=0 ; j < 0x200 ; j++)
    {
      buf2[j] = 0;
    }
    if (MDrv_UsbBlockReadToMIU_Port2(LunIndex,50+xxx,1,VirtoPhyAddr((U32)buf2))==FALSE)
    {
      DrvUsbMain2_printData("\r\n read failed\n",0);
      break;
    }
    //MDrv_Timer_Delayms(10);
    for (j=0 ; j < 0x200 ; j++)
    {
      if (buf2[j]!= (j&0xff))
      {
        DrvUsbMain2_printData("\r\n data error\n",0);
        while(1);
      }
    }
    MDrv_Timer_Delayms(5);
  }

}
#endif
#if 0
U8 MDrv_UsbGetInterfaceClass(U8 i,U8 j)
{
  U8 tmp;
  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_SetXdataWindow1();
  #endif
  tmp=psAttachDevice_Port2->saCD[i].sInterface[j].bInterfaceClass;
  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_ReleaseXdataWindow1();
  #endif

  return tmp;
}
#endif

//====================================================================
// * Function Name: main
// * Description:
// * Input:
// * OutPut:
//====================================================================
BOOLEAN MDrv_UsbHost_Init_Port2(void)
{
  BOOLEAN result=FALSE;

  DRV_USB_DBG(DrvUsbMain2_printMsg("HOST200_main\n"));

  MINI_DEBUG(DrvUsbMain2_printMsg("uinit\n"));
  if (gUsbStatus_Port2==USB_EJECT)
  {
    //printf("2430:%02bx\n",XBYTE[0x2430]);
    if (UHC_XBYTE(0x30)&2)
    {
      gUsbStatus_Port2=USB_OK;
    }
    else
      return FALSE;         //eject state
  }

  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_SetXdataWindow1_Port2();
  #endif
#ifndef  Enable_FS_Only
//  XBYTE[gUTMI2_BASE+0x06]|=0x44;        //Force HS TX current enable and CDR stage select
  XBYTE[gUTMI2_BASE+0x06]|=0x40;        //Force HS TX current enable and CDR stage select
  XBYTE[gUTMI2_BASE+0x06]&=~0x20;              //clear bit 5

  XBYTE[gUTMI2_BASE+0x06]|=0x03;        //reset UTMI
  XBYTE[gUTMI2_BASE+0x06]&=0xfc;
  XBYTE[gUTMI2_BASE+0x29]=0x08;         //disable full speed retime
#ifdef  Enable_Issue_TestPacket
  XBYTE[gUTMI2_BASE+0x03]=0;         //for device disconnect status bit
#else
  XBYTE[gUTMI2_BASE+0x03]=0xa8;         //for device disconnect status bit
#endif

  //XBYTE[gUTMI2_BASE+0x07]|=0x02;      //
  #ifdef DTV_STANDARD_LIB
   if  (XBYTE[0x1ecc]==0x02)            //Neptune , after U06
   {
      if (XBYTE[0x1ecf]>= 0x06)              //U07
        {
            XBYTE[gUTMI2_BASE+0x07]|=0x02;
            XBYTE[gUTMI2_BASE+0x2c]=0xc1;
            XBYTE[gUTMI2_BASE+0x2d]=0x3b;//enable TX common mode,
            XBYTE[gUTMI2_BASE+0x2f]|=0x0e;            //preemsis
        }
        else          //before U06
        {
            XBYTE[gUTMI2_BASE+0x2c]|=0x01;
            XBYTE[gUTMI2_BASE+0x2d]=0x38;         //disable TX common mode

        }

    }
     else if ((XBYTE[0x1ecc]>=3)&&(XBYTE[0x1ecc]<5))                    //Eris: 3 ,Titania: 4, Pluto: 5
    {
            XBYTE[gUTMI2_BASE+0x2c]=0xc5;
            XBYTE[gUTMI2_BASE+0x2d]=0x3b;//enable TX common mode,
            XBYTE[gUTMI2_BASE+0x2f]|=0x0e;            //preemsis
    }
       else if (XBYTE[0x1ecc]==5)          // Pluto: 5
    {
        //    XBYTE[gUTMI2_BASE+0x2c]|=0xc1;
       //     XBYTE[gUTMI2_BASE+0x2d]|=0x3;//enable TX common mode,
       //     XBYTE[gUTMI2_BASE+0x2f]|=0x4a;            //preemsis
           if (XBYTE[0x1ecf]<= 0x03)
               XBYTE[gUTMI2_BASE+0x13]|=0x70;        //U04 and later don't set
            XBYTE[gUTMI_BASE+0]&=0xfe;       //clear port 0 IREF
     }

     else if (XBYTE[0x1ecc]==0x0b)          // Titania 2
    {
            XBYTE[gUTMI2_BASE+0x2c]|=0xc1;
            XBYTE[gUTMI2_BASE+0x2d]|=0x3;//enable TX common mode,
            XBYTE[gUTMI2_BASE+0x2f]|=0x4a;            //preemsis
     }
    else if (XBYTE[0x1ecc]==6)        //Triton=6
    {
            XBYTE[gUTMI2_BASE+0x2c]=0xc1;
            XBYTE[gUTMI2_BASE+0x2d]=0x3b;//enable TX common mode,
            XBYTE[gUTMI2_BASE+0x2f]|=0x0e;            //preemsis
    }

  else                                //before U06
  {
    XBYTE[gUTMI2_BASE+0x2c]|=0x01;
    #ifndef Process_018_USE
    XBYTE[gUTMI2_BASE+0x2d]=0x38;         //disable TX common mode
    #endif
  }
  #endif

  #ifdef CERAMAL_SERISE_USE
  if (CHIP_VERSION >= CHIP_VER_U03)
  {
    XBYTE[gUTMI2_BASE+0x2c]|=0xc1;     //Slew rate control,Common mode voltage,Pre-emphasis select
    XBYTE[gUTMI2_BASE+0x2d]|=0x07;     //HS RTerm current: 5/6,Enable common mode adjust,Enable common mode voltage,Pre-emphasis select
    XBYTE[gUTMI2_BASE+0x2f]|=0x0e;     //Enable pre-emphasis at data enable edge,Enable pre-emphasis at data transition edge,Enable pre-emphasis
  }
  #endif

  if (XBYTE[0x1ecc]==5)                   // If is Pluto
      XBYTE[gUTMI2_BASE+0x09]|=0x01;     //ISI improvement
  else
      XBYTE[gUTMI2_BASE+0x13]|=0x02;     //ISI improvement

  XBYTE[gUTMI2_BASE+0x09]|=0x60;//0x20;       //patch low tempture,FL meta issue and enable new FL RX engin


  XBYTE[gUTMI2_BASE+0x27]&=0xf3;
  XBYTE[gUTMI2_BASE+0x27]|=0x08;        //(1) Offset 27 (．h3AA7) bit <3:2> set 2・b10   // RX bias current => 60uA (default 40uA)

  //(2) Offset 2A (．h3AAA) bit <3:2> set 2・b11               // Squelch voltage => 100mV (default 150mV)
#ifdef DTV_STANDARD_LIB
  XBYTE[gUTMI2_BASE+0x2a]=0x07;
#endif

  XBYTE[gUTMI2_BASE+0x15]|=0x20; //HOST CHIRP Detect

  //(3) Offset 2D (．h3AAD) bit <5:3> set 3・b111           // HS_RTERM bias current 5/6
  //XBYTE[0x128a]=0x87;            //change UHC priority
  //XBYTE[0x128d]&=0x0f;                //clear bit 12~15
  //XBYTE[0x128e]|=0xf;            //upgrade UHC priority, set bit 0~3
  MDrv_Timer_Delayms(500);
#endif

  //MDrv_Sys_SetXdataWindow1Base(USB_BUFFER_START_ADR_4K_ALIGN>>12);        //switch window 1
  gUsbStatus_Port2=USB_OK;
 // gUsbTimeout=3;  //set at flib_Host20_Enumerate_Port2()
  //RecoveryFlag_Port2=0;             //initialize recovery flag
  gUsbDeviceState_Port2=USB11_DEVICE;           //1.1 at first

  //gDeviceFatalError=FALSE;
  if(flib_OTGH_Init_Port2(0))
    result =TRUE;

  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_ReleaseXdataWindow1_Port2();
  #endif

  return result;

}
#if 0
BOOLEAN MDrv_UsbHost_Init_Enum_Port2(void)
{
  BOOLEAN result;
  result=MDrv_Usb_Device_Enum_Port2();
  return result;
}
#endif
#ifdef Enable_Mass_Storage
U8 MDrv_UsbGetMaxLUNCount_Port2()
{
  if (gUsbDeviceState_Port2==BAD_DEVICE)
    return 0;

  if (mwHost20_PORTSC_ConnectStatus_Rd()==0)
    return 0;
  else return (Mass_stor_us1_port2.max_lun+1);
}
U8 MDrv_GET_MASS_MAX_LUN_PORT2()
{
  //printf("Get max lun func:%02bx\n",Mass_stor_us1.max_lun);
  if (mwHost20_PORTSC_ConnectStatus_Rd()==0)
    return 0;
  else return Mass_stor_us1_port2.max_lun;
}
#endif
extern void GetValidLun_Port2(void);

#if 0//def ATV_SERISE_USE
void DisableINT(void)
{
  XBYTE[0x2B03]|=0x20;
  XBYTE[0x2B18]|=0x80;
  XBYTE[0x2B19]|=0x04;
  XBYTE[0x2B1A]|=0x80;
}
void EnableINT(void)
{
  XBYTE[0x2B03]&=0xDF;
  XBYTE[0x2B18]&=0x7F;
  XBYTE[0x2B19]&=0xFB;
  XBYTE[0x2B1A]&=0x7F;
}
#endif
#ifdef Enable_Mass_Storage
extern U8 ValidLunNum_Port2;
U8 MDrv_GET_VALID_LUN_NUM_PORT2()
{
    return ValidLunNum_Port2;
}
U8 MDrv_GET_MASS_VALID_LUN_PORT2()
{
  U8 LunMatrix=0,i;
  struct LUN_Device* LunDevice;
  //printf("Get valid lun func\n");
  if (mwHost20_PORTSC_ConnectStatus_Rd()==0)
    return USB_NOT_RESPONSE;               //device is not connected

  if (gUsbDeviceState_Port2==BAD_DEVICE) return 0;

  if (gSpeed_Port2==1)    return 0;           //low speed device

  //if (gDeviceFatalError) return USB_NOT_RESPONSE;
  if (gUsbStatus_Port2==USB_EJECT) return USB_NOT_RESPONSE;

  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_SetXdataWindow1_Port2();        //switch window 1
  #endif

  LunDevice = Mass_stor_us1_port2.Mass_stor_device;

  #ifdef ATV_SERISE_USE
    #ifdef Enable_Close_INT
    //XBYTE[0x2B00]|=0x02;
    DisableINT();
    #endif
  #endif
  GetValidLun_Port2();
  #ifdef ATV_SERISE_USE
    #ifdef Enable_Close_INT
    //XBYTE[0x2B00]&=0xFD;
    EnableINT();
    #endif
  #endif

  if (gUsbStatus_Port2==USB_TIMEOUT)
  {
    #ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1_Port2();
    #endif
    return USB_NOT_RESPONSE;            //USB DEVICE not responding
  }

  for (i=0; i <= Mass_stor_us1_port2.max_lun ; i++)
  {
    LunMatrix=LunMatrix<<1 ;
    //printf("bDeviceReady:%02bx\n",LunDevice[Mass_stor_us1.max_lun-i].bDeviceReady);
    if (LunDevice[Mass_stor_us1_port2.max_lun-i].bDeviceReady == TRUE)
    {
      LunMatrix|=1;
    }
  }
  //printf("GetReadyLun:%02bx\n",LunMatrix);
  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_ReleaseXdataWindow1_Port2();
  #endif

  return LunMatrix;
}
#endif
U32 USB_BUFFER_START_ADR_4K_ALIGN_Var_Port2;
#ifndef USE_XDATA_ADDRESS_0XF000
#ifndef CERAMAL_SERISE_USE
extern DWORD jpCVBS5_Get_HK_XDATA_Addr(void);
#endif
#endif
void MDrv_USB_Init_Port2(DWORD USBAdr)
{
  //gProjectCode=ProjectCode;
  //gDeviceFatalError=FALSE;

  USB_BUFFER_START_ADR_4K_ALIGN_Var_Port2=USBAdr;

  //printf("\r\n USB_BUFFER_START_ADR_4K_ALIGN_Var=%x",(U16)(USB_BUFFER_START_ADR_4K_ALIGN_Var>>16));
  //printf(",%x",(U16)(USB_BUFFER_START_ADR_4K_ALIGN_Var));


  if (USB_BUFFER_START_ADR_4K_ALIGN_Var_Port2 % 4096 != 0)
  {
    DRV_USB_DBG( DrvUsbMain2_printMsg("Error USB Port2 Starting address is not 4K alignmented\n"));
  }

#if 1
  if (!drvUSBHost_isPowerSaveModeEnable())
  {
    #ifdef Enable_FS_Only
    //XBYTE[REG_1EDC] = 0x08;
    //XBYTE[REG_1ECE] = 0x40;
    //XBYTE[REG_1ED0] = 0x04;
    //while(1)
    //{
    //   if (XBYTE[REG_1EDC] & 0x80)
    //      break;
    //}

    XBYTE[gUTMI2_BASE+0x01] &= 0xEF;
    XBYTE[gUTMI2_BASE+0x09] |= 0x08;

    XBYTE[gUSBC2_BASE+0x03] = 0x00;
    XBYTE[gUSBC2_BASE+0x02] = 0x01;
    XBYTE[gUSBC2_BASE+0x00] = 0x28;
    #endif

    #ifdef Enable_Preamble
    XBYTE[gUSBC2_BASE+0x0F] |= 0x08;
    XBYTE[gUTMI2_BASE+0x3F] |= 0x80;
    #endif

    mbFUSBH200_VBUS_ON_Set();
    #ifndef Enable_FS_Only
    UHC_XBYTE(0x34)&=0xBF; //set suspend
    UHC_XBYTE(0x34)|=0x40; //clr suspend
    MDrv_Timer_Delayms(2);
    XBYTE[gUTMI2_BASE+0x00]|=0x01;      // override mode enable for power down control
    XBYTE[gUTMI2_BASE+0x01]|=0x40;  // enable IREF power down
    XBYTE[gUTMI2_BASE+0x01]|=0x02;   // enable PLL power down
    XBYTE[gUTMI2_BASE+0x01]&=0xFD;   // disable PLL power down
    #endif
    mbHost20_USBCMD_HCReset_Set();
  }

#else
  #ifndef USB_POWER_SAVING_MODE
  mbFUSBH200_VBUS_ON_Set();
  UHC_XBYTE(0x34)&=0xBF; //set suspend
  UHC_XBYTE(0x34)|=0x40; //clr suspend
  MDrv_Timer_Delayms(2);
  XBYTE[gUTMI_BASE+0x00]|=0x01;     // override mode enable for power down control
  XBYTE[gUTMI_BASE+0x01]|=0x40; // enable IREF power down
  XBYTE[gUTMI_BASE+0x01]|=0x02;   // enable PLL power down
  XBYTE[gUTMI_BASE+0x01]&=0xFD;   // disable PLL power down
  mbHost20_USBCMD_HCReset_Set();
  #endif
#endif

  #ifdef CERAMAL_SERISE_USE
    #if (CERAMAL_STEPPING>=2)
    XBYTE[gUTMI2_BASE+0x06]|=0x04; //HSTXIEN
    XBYTE[gUTMI2_BASE+0x00]=0x0;   //disable power down over write
    XBYTE[0x2601]|=0x20;            //HsEnable set of usb power register
    XBYTE[gUTMI2_BASE+0x01]=0x0;   //disable power down over write  high byte
    XBYTE[0x2500]|=0x08;            //disable init suspend state
    MDrv_Timer_Delayms(3);
    #endif
  XBYTE[0x2502]|=0x10; //id pull up
  XBYTE[gUTMI2_BASE+0x06]|=0x80; //dual otg mode
  #endif
}
#if 0
U8  MDrv_GetUsbDeviceStatusPort2()
{
  return gUsbDeviceState_Port2;
}

void MDrv_SetUsbDeviceStatus_Port2(U8 status)
{
    gUsbDeviceState_Port2 = status;
}

void  MDrv_ClearUsbDeviceStatusPort2()
{
  //if (gUsbDeviceState_Port2==BAD_DEVICE)
  //{
  //  printf("Clear bad device\n");
  //  gUsbDeviceState_Port2=USB11_DEVICE;
  //}
}
#endif
void ResetUsbHardwarePort2()
{
  XBYTE[gUTMI2_BASE+0x06]|=0x03;        //reset UTMI
  XBYTE[gUTMI2_BASE+0x06]&=0xfc;
  mbHost20_USBCMD_HCReset_Set();
  //MDrv_Timer_Delayms(200);
}
UINT32 UsbStartTimePort2=0;
UINT32 UsbPowerSavingTimerPort2=0;

#ifdef CERAMAL_SERISE_USE
void FUSBH200_Driver_VBUS(void)
{
  XBYTE[0x1E14]|=0x80; // GPIO AO enable, reg_drvbus_en=1, eable drive vbus pad
  mbFUSBH200_VBUS_ON_Set();
}
#endif

//#ifdef USB_POWER_SAVING_MODE
extern void UsbTurnOffPowerDownMode(void);
extern void UsbTurnOnPowerDownMode(void);
void UsbPort2UTMIInitial(void)
{
   #ifdef Enable_FS_Only
   XBYTE[gUTMI2_BASE+0x00] &= 0xEF;
   XBYTE[gUTMI2_BASE+0x09] |= 0x08;

   XBYTE[gUSBC2_BASE+0x03] = 0x00;
   XBYTE[gUSBC2_BASE+0x02] = 0x01;
   XBYTE[gUSBC2_BASE+0x00] = 0x28;
   #endif
   mbFUSBH200_VBUS_ON_Set();
   #ifndef Enable_FS_Only
   UHC_XBYTE(0x34)&=0xBF; //set suspend
   UHC_XBYTE(0x34)|=0x40; //clr suspend
   MDrv_Timer_Delayms(2);
   XBYTE[gUTMI2_BASE+0x00]|=0x01;       // override mode enable for power down control
   XBYTE[gUTMI2_BASE+0x01]|=0x40;   // enable IREF power down
   XBYTE[gUTMI2_BASE+0x01]|=0x02;   // enable PLL power down
   XBYTE[gUTMI2_BASE+0x01]&=0xFD;   // disable PLL power down
   XBYTE[gUTMI2_BASE+0x00]&=~0x01;      // override mode enable for power down control
   #endif
   mbHost20_USBCMD_HCReset_Set();
}
//#endif

BOOLEAN MDrv_UsbDeviceConnect_Port2(void)
{
  /*
  if (gDeviceFatalError)
  {
    gDeviceFatalError=FALSE;
    ResetUsbHardware();
    return FALSE;
  }*/
  /*
  if (XBYTE[0x0B04]&0x40)
  {
      XBYTE[0x3AC0] &= 0xFE;
  }
  */
#if 1
  if (drvUSBHost_isPowerSaveModeEnable())
  {
    //#ifdef USBHOST2PORT
    if (gUsbDeviceState==WAIT_INIT)
        return FALSE;
    //#endif
    if (gUsbDeviceState_Port2==WAIT_INIT)
    {
        if (MDrv_Timer_DiffTimeFromNow(UsbPowerSavingTimerPort2) < 300 )
            return FALSE;
    }
    if (gUsbDeviceState_Port2==POWER_SAVING)
    {
       if (XBYTE[gUSBC2_BASE+8]&0x40)
       {
           UsbTurnOffPowerDownMode();
           //UsbPort0UTMIInitial();
           drvUSBHost_UTMIInitial_Port2();
           //#ifdef USBHOST2PORT
           UsbPort2UTMIInitial();
           if (gUsbDeviceState==POWER_SAVING)
               gUsbDeviceState=NO_DEVICE;
           //#endif
           //UsbPowerSavingTimerPort2 = MDrv_Timer_GetTime0();
           gUsbDeviceState_Port2=WAIT_INIT;
           return FALSE;
           //MDrv_Timer_Delayms(300);
       }
       else
       {
           return FALSE;
       }
    }
  }
#else
  #ifdef USB_POWER_SAVING_MODE
  //#ifdef USBHOST2PORT
  if (gUsbDeviceState==WAIT_INIT)
      return FALSE;
  //#endif
  if (gUsbDeviceState_Port2==WAIT_INIT)
  {
      if (MDrv_Timer_DiffTimeFromNow(UsbPowerSavingTimerPort2) < 300 )
          return FALSE;
  }
  if (gUsbDeviceState_Port2==POWER_SAVING)
  {
     if (XBYTE[gUSBC2_BASE+8]&0x40)
     {
         UsbTurnOffPowerDownMode();
         //UsbPort0UTMIInitial();
         drvUSBHost_UTMIInitial();
         //#ifdef USBHOST2PORT
         UsbPort1UTMIInitial();
         if (gUsbDeviceState==POWER_SAVING)
             gUsbDeviceState=NO_DEVICE;
         //#endif
         UsbPowerSavingTimerPort2 = MDrv_Timer_GetTime0();
         gUsbDeviceState_Port2=WAIT_INIT;
         return FALSE;
         //MDrv_Timer_Delayms(300);
     }
     else
     {
         return FALSE;
     }
  }
  #endif
#endif
  if (gUsbStatus_Port2==USB_EJECT)
  {
    if (UHC_XBYTE(0x30)&2)
    {
      gUsbStatus_Port2=USB_OK;
    }
    else
      return FALSE;         //eject state
  }
  //FUSBH200_Driver_VBUS();             //make sure device is connected , then turn on VBUS

  #ifdef OnePort_OTG_EHCI
  if((XBYTE[gUTMI2_BASE+0x30]&0x80))   //CID==1 device
  {
     XBYTE[0x2440]=0x10;                //turn-off vbus control to prevent OVC and VBUSERR
     XBYTE[0x2502]=0x12;                // Enable OTG
     if(XBYTE[0x2508] & 0x01)           //VBUSVALID=1
     {
        if(USBMode!=1)
        {
           //switch to otg mode and call init otg function
           XBYTE[0x2440]=0x10; //turn-off vbus control to prevent OVC and VBUSERR
           XBYTE[0x2502]=0x12;
           //========================================================
           // Derek testing
           usbREG_WRITE8(REG_DEVCTL, usbREG_READ8(REG_DEVCTL) & (~M_DEVCTL_SESSION));// disable session
           usbREG_WRITE8(USB_REG_POWER,usbREG_READ8(USB_REG_POWER)& (~M_POWER_SOFTCONN));
           XBYTE[gUTMI2_BASE+0x00] &= ~(reg_dp_puen+reg_term_override);//disable term_over and dp_puen
           // Derek Added 2008/01/10, reset OTG IP
           XBYTE[0x2500] = 0x6;
           XBYTE[REG_UTMI_REG3]|=0x03;         //reset UTMI: prevent utmi error caused by switch
           XBYTE[0x2500] = 0x0;
           XBYTE[REG_UTMI_REG3]&=0xFC;

           if(XBYTE[0x2508] & 0x01)  // Derek debug
           {
               //add init otg here
               MDrv_OTG_Init();
               USBMode=1;
           }
           else
           {
               usbREG_WRITE8(REG_DEVCTL, usbREG_READ8(REG_DEVCTL) & (~M_DEVCTL_SESSION));// disable session
               //printf("Disable Session\r\n");
               usbUSBState = 0;
           }
        }
     }
     else
     {
        USBMode=0;
        // Derek added
        usbUSBState = 0;
        usbREG_WRITE8(REG_DEVCTL, usbREG_READ8(REG_DEVCTL) & (~M_DEVCTL_SESSION));// disable session
        //printf("Disable Session 2\r\n");
        XBYTE[gUTMI2_BASE+0x00] &= ~(reg_dp_puen+reg_term_override);
     }
     if(usbIsUSBConfiged())
        return USBDeivceMode_connect;
     else
        return USBDeivceMode_disconnect;
  }
  else                                  //CID==0 host
  {
     //printf("\r\n SwToUHC\n");
     if(USBMode!=2)
     {
        //disbale interrupt witch be active by OTG and switch to UHCI
        MDrv_Disable_IRQ(IRQ_INTERRUPT_OTG_INT); //close OTG interrupt
        XBYTE[0x2502]=0x11; //swicth to FARADAY UHC
        XBYTE[REG_UTMI_REG3] |= (reg_rx_swreset+reg_utmi_tx_swreset);         //reset UTMI: prevent utmi error caused by switch
        XBYTE[REG_UTMI_REG3] &= ~(reg_rx_swreset+reg_utmi_tx_swreset);
        FUSBH200_Driver_VBUS();  //make sure device is connected , then turn on VBUS
        MDrv_Timer_Delayms(100); //add delay time for UHC to complete device detection
     }
     USBMode=2;
  }
  #endif
  if (mwHost20_PORTSC_ConnectStatus_Rd())
  {
    if ( (gUsbDeviceState_Port2==USB11_DEVICE) ||(gUsbDeviceState_Port2==USB20_DEVICE) )
    {
        // If the device is connected and we get a connection change.
        // It means that the user change the device and we just missed.
        if (mwHost20_PORTSC_ConnectChange_Rd())
        {
            DrvUsbMain2_printMsg("dev changed, we missed\n");
            gSpeed_Port2=0xff;  //reset this value
            UHC_XBYTE(0x40)&=~0x80;//clear force enter FSmode
            gUsbDeviceState_Port2= NO_DEVICE;
            return FALSE;
        }
    }

   if (gUsbDeviceState_Port2==BAD_DEVICE) return FALSE;      //not repeating doing emunerate

//    if (gSpeed_Port2==0x01)
//        {
//           MINI_DEBUG(DrvUsbMain2_printMsg("ls-exit\n"));
//           return FALSE;
//        }
    if (gUsbDeviceState_Port2==NO_DEVICE)
        gUsbDeviceState_Port2=CONNECT_DEVICE;
    #ifdef OnePort_OTG_EHCI
    return USBHostMode_connect;
    #else
    return TRUE;
    #endif
  }
  else
  {
    gSpeed_Port2 = 0xFF;
    UHC_XBYTE(0x40)&=~0x80;//clear force enter FSmode
#if 1
    if (drvUSBHost_isPowerSaveModeEnable())
    {
        if ((gUsbDeviceState==NO_DEVICE)||(gUsbDeviceState==POWER_SAVING))
        {
            if (gUsbDeviceState_Port2!=POWER_SAVING)
                UsbTurnOnPowerDownMode();
            gUsbDeviceState_Port2=POWER_SAVING;
            if (drvUSBHost_isPort2Enable())
            {
                gUsbDeviceState_Port2=POWER_SAVING;
            }
        }
        else
            gUsbDeviceState_Port2=NO_DEVICE;
    }
    else
        gUsbDeviceState_Port2=NO_DEVICE;
#else
    #ifdef USB_POWER_SAVING_MODE
    if ((gUsbDeviceState==NO_DEVICE)||(gUsbDeviceState==POWER_SAVING))
    {
        if (gUsbDeviceState_Port2!=POWER_SAVING)
            UsbTurnOnPowerDownMode();
        gUsbDeviceState=POWER_SAVING;
        gUsbDeviceState_Port2=POWER_SAVING;
    }
    else
        gUsbDeviceState_Port2=NO_DEVICE;
    #else
    gUsbDeviceState_Port2=NO_DEVICE;
    #endif
#endif


    #ifndef ATV_SERISE_USE
    #ifdef CERAMAL_SERISE_USE
    if (MDrv_Timer_TimeDifference(MDrv_Timer_GetTime0(), UsbStartTimePort2) > 1000UL)
    #else
    if (MDrv_Timer_DiffTimeFromNow(UsbStartTimePort2) > 1000 )
    #endif
    {

      UsbStartTimePort2=MDrv_Timer_GetTime0();
      ResetUsbHardwarePort2();

    }
    #else
    UsbStartTimePort2++;
    if(UsbStartTimePort2>0x600)
    {
      UsbStartTimePort2=0;
      ResetUsbHardwarePort2();
    }
    #endif
    //MINI_DEBUG( printf("no_con \n"));
    #ifdef OnePort_OTG_EHCI
    return USBHostMode_disconnect;
    #else
    return FALSE;
    #endif
  }
}
#ifdef Enable_Mass_Storage
void MDrv_EjectUsbDevice_Port2(void)
{
  struct LUN_Device* LunDevice;
  U8 i;

  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_SetXdataWindow1_Port2();
  #endif

  //printf("eject\n");
  LunDevice = Mass_stor_us1_port2.Mass_stor_device;
  for (i=0; i <= Mass_stor_us1_port2.max_lun ; i++)
  {

    if (LunDevice[i].bDeviceReady == TRUE)
    {
      vSCSI_EJECT_DEVICE_Port2(i);
      gUsbStatus_Port2=USB_EJECT;
      mwHost20_PORTSC_ConnectChange_Set();        //clear port connect change bit
      //printf("ej_ok\n");
    }
  }
  flib_Host20_Close_Port2();

  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_ReleaseXdataWindow1_Port2();
  #endif

}
#endif
#if 0
void MDrv_UsbClose_Port2(void)
{
  flib_Host20_Close_Port2();
}
#endif
#ifdef Enable_Mass_Storage
BOOLEAN MDrv_UsbBlockReadToMIU_Port2(U8 lun,U32 u32BlockAddr, U32 u32BlockNum,U32 u32MIUAddr)
{
  BOOLEAN result;
  U8 retrycnt=0;
  struct LUN_Device* LunDevice = Mass_stor_us1_port2.Mass_stor_device;

  if (mwHost20_PORTSC_ConnectStatus_Rd()==0)
    return FALSE;               //device is not connected

  if (gUsbDeviceState_Port2==BAD_DEVICE) return FALSE;


  if (u32BlockAddr > Mass_stor_us1_port2.Mass_stor_device[lun].u32BlockTotalNum)
  {
    MINI_DEBUG(DrvUsbMain2_printData("USBRead address is over the range:%lx\n",u32BlockAddr));
    return FALSE;
  }
  //printf("usb read sector:%lx\n",u32BlockNum);

  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_SetXdataWindow1_Port2();       //switch window 1
  #endif
  #ifdef ATV_SERISE_USE
    #ifdef Enable_Close_INT
    XBYTE[0x2B00]|=0x02;
    DisableINT();
    #endif
  #endif
  //result= bSCSI_Read_Write10_Port2(FALSE,lun,  u32BlockAddr, u32BlockNum, u32MIUAddr);
  while (1)
  {
    retrycnt++;
    result= bSCSI_Read_Write10_Port2(FALSE,lun,  u32BlockAddr, u32BlockNum, u32MIUAddr);
    if (result==TRUE) break;
    if (result==FALSE)
    {
      if (retrycnt > 3)
      {
       if (gUsbStatus_Port2==USB_TIMEOUT)
             gUsbDeviceState_Port2=BAD_DEVICE;    //mark as bad device
#if 0  //Removed. It gets problem if remove card while play from USB card reader.
       else
             LunDevice[lun].bDeviceValid=FALSE;      //mark as bad lun
#endif

        break;          //return FALSE
      }
      MINI_DEBUG( DrvUsbMain2_printMsg("USBDisk Read failed\n"));
       if (gUsbStatus_Port2==USB_TIMEOUT)
       {
        if ((retrycnt==2)&&(mwOTG20_Control_HOST_SPD_TYP_Rd()==2) )              //make sure it is hi speed
        {
          MINI_DEBUG(DrvUsbMain2_printMsg("Force FS\n"));
          UHC_XBYTE(0x40)|=0x80;//force enter FSmode
        }
        #ifndef ATV_SERISE_USE
        #ifndef CERAMAL_SERISE_USE
        msAPI_Timer_ResetWDT();
        #endif
        #else
          #ifdef Enable_Close_INT
          XBYTE[0x2B00]&=0xFD;
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

       // MDrv_Usb_Device_Enum();             //reinit usb device
          if (MDrv_Usb_Device_Enum_Port2()==FALSE)
         {
              result=FALSE;
              break;
         }
      }
    }

  }

  #ifdef ATV_SERISE_USE
    #ifdef Enable_Close_INT
    XBYTE[0x2B00]&=0xFD;
    EnableINT();
    #endif
  #endif
  //MDrv_Sys_ReleaseXdataWindow1();
  if (result==FALSE)
  {
    DRV_USB_DBG(DrvUsbMain2_printMsg("USBDisk Read failed\n"));
    //printf("read failed\n");
  }

  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_ReleaseXdataWindow1_Port2();
  #endif
  return result;

}

BOOLEAN MDrv_UsbBlockWriteFromMIU_Port2(U8 lun, U32 u32BlockAddr, U32 u32BlockNum,U32 u32MIUAddr)
{
  BOOLEAN result;
  if (mwHost20_PORTSC_ConnectStatus_Rd()==0)
    return FALSE;               //device is not connected

   if (gUsbDeviceState_Port2==BAD_DEVICE) return FALSE;

  if (u32BlockAddr > Mass_stor_us1_port2.Mass_stor_device[lun].u32BlockTotalNum)
  {
    MINI_DEBUG(DrvUsbMain2_printData("USBWrite address is over the range:%lx\n",u32BlockAddr));
    return FALSE;
  }

  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_SetXdataWindow1_Port2();       //switch window 1
  #endif

  result= bSCSI_Read_Write10_Port2(TRUE,lun,  u32BlockAddr, u32BlockNum, u32MIUAddr);

  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_ReleaseXdataWindow1_Port2();
  #endif

  return result;

}
U32 MDrv_GetUsbBlockSize_Port2(U8 lun)
{
  return Mass_stor_us1_port2.Mass_stor_device[lun].u32BlockSize;
}

U32 MDrv_GetUsbBlockNum_Port2(U8 lun)
{
  return Mass_stor_us1_port2.Mass_stor_device[lun].u32BlockTotalNum;
}
#endif
#if 0
U8 MDrv_USB_GetXDataExtStackCountPort2()
{
  return xdwin1_lockCount_Port2;
}
//---if you don't know how to use it, don't use it
void EnterUXBModePort2()
{
  MINI_DEBUG( DrvUsbMain2_printMsg("Enter UXB mode\n"));
  UHC_XBYTE(0x40)|=0x80;//force enter FSmode
  MDrv_Usb_Device_Enum_Port2();
}
void LeaveUXBModePort2()
{
  MINI_DEBUG(DrvUsbMain2_printMsg("leave UXB mode \n"));
  UHC_XBYTE(0x40)&=~0x80;//leave  FSmode
  MDrv_Usb_Device_Enum_Port2();

}
#endif
BOOLEAN  MDrv_Usb_Device_Enum_Port2(void)
{
  BOOLEAN result=FALSE;
  U8  RetryCount=0,i=0,j=0, total_interface=0;

  U8 rootUSBDeviceInterfaceClass = USB_INTERFACE_CLASS_NONE;

  if (gUsbDeviceState_Port2==BAD_DEVICE) return FALSE;        //bad device , no more enumerate

  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_SetXdataWindow1_Port2();
  #endif
  NowIsHubPort2=0;

port2_redo_init:
  //USB ROOT
  RetryCount++;
  if (MDrv_UsbHost_Init_Port2()==FALSE) goto port2_fail_exit;
  if  (flib_Host20_Enumerate_Port2(1,4)==0)
  {
    if ((gUsbStatus_Port2==USB_TIMEOUT)||(gUsbStatus_Port2==USB_INIT_FAIL))
    {
      #ifdef DTV_STANDARD_LIB
      if ((RetryCount==2)&&(mwOTG20_Control_HOST_SPD_TYP_Rd()==2))
      {                                                               //make sure it is hi speed
        MINI_DEBUG(DrvUsbMain2_printMsg("Force FS\n"));
        UHC_XBYTE(0x40)|=0x80;//force enter FSmode
      }
      #endif

      if (RetryCount >= 3)
      {
        MINI_DEBUG(DrvUsbMain2_printMsg("bad device\n"));
        gUsbDeviceState_Port2=BAD_DEVICE;
        goto port2_fail_exit;
      }
      #ifndef ATV_SERISE_USE
      #ifndef CERAMAL_SERISE_USE
      msAPI_Timer_ResetWDT();
      #endif
      #endif
      goto port2_redo_init;
    }
    gUsbDeviceState_Port2=BAD_DEVICE;
    goto port2_fail_exit;
  }
  else
  {
#ifdef DTV_STANDARD_LIB
    XBYTE[gUTMI2_BASE+0x2a]=0x0;
#endif
    total_interface=psAttachDevice_Port2->saCD[0].bINTERFACE_NUMBER; 
for(j=0;j<total_interface;j++)
{
    rootUSBDeviceInterfaceClass= psAttachDevice_Port2->saCD[i].sInterface[j].bInterfaceClass;
    //MDrv_UsbGetInterfaceClass(0, 0);
    DRV_USB_DBG(DrvUsbMain2_printMsg("Check USD Device 4\n"););
#if 0  //We don't need to check here
    if(rootUSBDeviceInterfaceClass != 0x08 && rootUSBDeviceInterfaceClass != 0x09)
    {
      //printf("unsupport class\n",0);
      DRV_USB_DBG(printf("unsupport USB root class=%02bx\n", rootUSBDeviceInterfaceClass););
      result= FALSE;
      goto port2_fail_exit;
    }
#endif

    DRV_USB_DBG(DrvUsbMain2_printData("USB root class=%x\n",rootUSBDeviceInterfaceClass););
    if (rootUSBDeviceInterfaceClass==USB_INTERFACE_CLASS_HUB)//HUB
    {
      U8 PortNum,i,devaddr;

      devaddr=6;
      DRV_USB_DBG(DrvUsbMain2_printMsg("Hub class!\n"));

      PortNum=Usb_Hub_Port_Num_Port2();
      for (i=1; i <= PortNum ; i++)
      {
        psAttachDevice_Port2->bAdd=4;
        pHost20_qHD_List_Control1_Port2.bDeviceAddress=psAttachDevice_Port2->bAdd;
        if (USB_Hub_Handle_Port2(i)==FALSE)
        {
          result=FALSE;
          continue;
        }
        devaddr++;
        NowIsHubPort2=1;
        if (flib_Host20_Enumerate_Port2(1,devaddr)==0)
        {
          result=FALSE;
          continue;
        }
        psAttachDevice_Port2->bAdd=devaddr;
        #ifdef Enable_Mass_Storage
        result=MassStorage_Init_Port2();              //do mass storage class init
        #endif
        if (drvUSBHost_HID_Init(USB_PORT_2,j) == 0)
        {
            result = TRUE;
            DRV_USB_DBG(DrvUsbMain2_printMsg("HID initial ok\r\n"));
        }        
        if (result==TRUE) break;
      }
        if (result==FALSE)
        {
             MINI_DEBUG(DrvUsbMain2_printMsg("unsupport hub class device,->bad device\n"));
             gUsbDeviceState_Port2=BAD_DEVICE;
        }
    }
    #ifdef Enable_Mass_Storage
   else  if(rootUSBDeviceInterfaceClass==USB_INTERFACE_CLASS_MSD)//Mass storage class
    {
      result=MassStorage_Init_Port2();
      #ifdef Enable_Burning_Test
      UsbTestPort2();
      #endif

      #ifdef Enable_Low_Temperature_Patch
      if((gSpeed_Port2==0)&&(gUsbStatus_Port2==USB_TIMEOUT))
      {
        //printf("\r\n CDR toggle!!",0);
        XBYTE[gUTMI2_BASE+0x0a]^=0x10; //invert CDR_CLOCK
        XBYTE[gUTMI2_BASE+0x06]|=0x03; //reset UTMI
        XBYTE[gUTMI2_BASE+0x06]&=0xfc;
      }
      #endif

      if((gSpeed_Port2==2)&&(gUsbStatus_Port2==USB_TIMEOUT))
      {

        UHC_XBYTE(0x40)|=0x80; //force enter FS mode
        goto port2_redo_init;
      }

      if(result==FALSE)
      {
          if (RetryCount >= 3)
          {
             goto port2_fail_exit;
          }

          goto port2_redo_init;
      }
    }
    #endif
    #ifdef  Enable_PTP
   else  if (rootUSBDeviceInterfaceClass == USB_INTERFACE_CLASS_IMAGE)
    {
        if (drvUSBHost_PTP_Init(USB_PORT_2) == PTP_OK)
        {
            result = TRUE;
            DRV_USB_DBG(DrvUsbMain2_printMsg("PTP initial ok\r\n"));
        }
    }
    #endif
   else if (rootUSBDeviceInterfaceClass==3)//HID
    {
        if (drvUSBHost_HID_Init(USB_PORT_2,j) == 0)
        {
            result = TRUE;
            DRV_USB_DBG(DrvUsbMain2_printMsg("HID initial ok\r\n"));
        }
    }
   else
    {
       MINI_DEBUG(DrvUsbMain2_printData("unsupport %x class device->bad device\n",rootUSBDeviceInterfaceClass));
       gUsbDeviceState_Port2=BAD_DEVICE;           //not belong to any above
    }

    DRV_USB_DBG(DrvUsbMain2_printMsg("USB_CON\n"););
    //return result;
}
   for(i=0;i<total_interface;i++)
   {
        if(PORT2_DEVICE[i]==1 /*|| PORT_DEVICE[i]==2*/) //1=Keyboard
            flib_Host20_Interrupt_Init_Port2(4,i,0);
        else if(PORT2_DEVICE[i]==2) //2=mouse
            flib_Host20_Interrupt_Init_Port2(4,i,1); //2=mouse
   }

  }

port2_fail_exit:
  memset(pUsbData_Port2, 0, 8);
  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_ReleaseXdataWindow1_Port2();
  #endif

  if (mwHost20_PORTSC_ConnectChange_Rd())
    mwHost20_PORTSC_ConnectChange_Set();

  return result;
}
//////////////////////////////////////////////////////////////////////////////////////////
//
// USB Issue Test packet function !!
//
/////////////////////////////////////////////////////////////////////////////////////////

#ifdef Enable_Issue_TestPacket
//#if 1 // Always enable
void IssueTestPacket_Port2(U8 xdata *TestData)
{
  U32 DMAAddress,datreg32;

  DrvUsbMain2_printMsg("Test Packet on port1\n");
  #ifdef Issue_OUTTestPacket
  TestData[0]=0x55;
  TestData[1]=0x53;
  TestData[2]=0x42;
  TestData[3]=0x43;
  TestData[4]=0x88;
  TestData[5]=0x88;
  TestData[6]=0x89;
  TestData[7]=0xa0;
  TestData[8]=0x00;
  TestData[9]=0x02;
  TestData[10]=0x00;
  TestData[11]=0x00;
  TestData[12]=0x80;
  TestData[13]=0x00;
  TestData[14]=0x0a;
  TestData[15]=0x28;
  TestData[16]=0x00;
  TestData[17]=0x00;
  TestData[18]=0x00;
  TestData[19]=0x00;
  TestData[20]=0x84;
  TestData[21]=0x00;
  TestData[22]=0x00;
  TestData[23]=0x01;
  TestData[24]=0x00;
  TestData[25]=0x00;
  TestData[26]=0x00;
  TestData[27]=0x00;
  TestData[28]=0x00;
  TestData[29]=0x00;
  TestData[30]=0x00;
  #endif

  //printf("3AAC:%x\n",XBYTE[0x3AAC]);
  //printf("UTMI(0x2c):%x\n",XBYTE[gUTMI2_BASE+0x2C]);
//  #ifdef Issue_TestPacket
  #if 1 //Always enabled
  TestData[0]=0x0;
  TestData[1]=0x0;
  TestData[2]=0x0;
  TestData[3]=0x0;
  TestData[4]=0x0;
  TestData[5]=0x0;
  TestData[6]=0x0;
  TestData[7]=0x0;
  TestData[8]=0x0;
  TestData[9]=0xaa;
  TestData[10]=0xaa;
  TestData[11]=0xaa;
  TestData[12]=0xaa;
  TestData[13]=0xaa;
  TestData[14]=0xaa;
  TestData[15]=0xaa;
  TestData[16]=0xaa;
  TestData[17]=0xee;
  TestData[18]=0xee;
  TestData[19]=0xee;
  TestData[20]=0xee;
  TestData[21]=0xee;
  TestData[22]=0xee;
  TestData[23]=0xee;
  TestData[24]=0xee;
  TestData[25]=0xfe;
  TestData[26]=0xff;
  TestData[27]=0xff;
  TestData[28]=0xff;
  TestData[29]=0xff;
  TestData[30]=0xff;
  TestData[31]=0xff;
  TestData[32]=0xff;
  TestData[33]=0xff;
  TestData[34]=0xff;
  TestData[35]=0xff;
  TestData[36]=0xff;
  TestData[37]=0x7f;
  TestData[38]=0xbf;
  TestData[39]=0xdf;
  TestData[40]=0xef;
  TestData[41]=0xf7;
  TestData[42]=0xfb;
  TestData[43]=0xfd;
  TestData[44]=0xfc;
  TestData[45]=0x7e;
  TestData[46]=0xbf;
  TestData[47]=0xdf;
  TestData[48]=0xfb;
  TestData[49]=0xfd;
  TestData[50]=0xfb;
  TestData[51]=0xfd;
  TestData[52]=0x7e;
  #endif

  //printf("[9]=%2bx\n", XBYTE[((U16)TestData)+9]);

  //DbgPortEanble();

  UHC_XBYTE(0x50)|=0x14; //enable test packet and lookback

  XBYTE[gUTMI2_BASE+0x06]|=0x03; //TR/RX reset
  XBYTE[gUTMI2_BASE+0x06]&=0xFC;

  //while(1)
  {


     DMAAddress=VirtoPhyAddr((U16)TestData);

     //set DMA memory base address
     UHC_XBYTE(0x74)=(U8)DMAAddress;
     UHC_XBYTE(0x75)=(U8)(DMAAddress>>8);
     UHC_XBYTE(0x76)=(U8)(DMAAddress>>16);
     UHC_XBYTE(0x77)=(U8)(DMAAddress>>24);

    //printf("start check 2474=%2bx\n",XBYTE[0x2474]);
    //printf("start check 2475=%2bx\n",XBYTE[0x2475]);
    //printf("start check 2476=%2bx\n",XBYTE[0x2476]);
    //printf("start check 2477=%2bx\n",XBYTE[0x2477]);


    //set DMA data Length and type(memory to FIFO)
    //#ifdef Issue_TestPacket
    #if 1   // Always enabled
    datreg32 = 53;
    #else
    datreg32 = 31;
    #endif

    datreg32 = datreg32 << 8;
    datreg32 = datreg32 | 0x02;

    UHC_XBYTE(0x70)=(U8)datreg32;
    UHC_XBYTE(0x71)=(U8)(datreg32>>8);
    UHC_XBYTE(0x72)=(U8)(datreg32>>16);
    UHC_XBYTE(0x73)=(U8)(datreg32>>24);

    UHC_XBYTE(0x70)|=0x01;//DMA start

    //printf("start check 2470=%2bx\n",XBYTE[0x2470]);
    //printf("start check 2471=%2bx\n",XBYTE[0x2471]);
    //printf("start check 2472=%2bx\n",XBYTE[0x2472]);
    //printf("start check 2473=%2bx\n",XBYTE[0x2473]);

    //MDrv_Timer_Delayms(1000);

    //printf("start check 2444=%2bx\n",XBYTE[0x2444]);

    while(!(UHC_XBYTE(0x44) &0x08))
    {
      //printf("XBYTE[0x2444]=%2bx\n",XBYTE[0x2444]);
      //MDrv_Timer_Delayms(10);//delay
    }

    //printf("Dma success\n",0);

    MDrv_Timer_Delayms(10);
  }

}

void Port1_IssueTestPacket_Initial(void)
{
    XBYTE[gUSBC2_BASE]|=0x02; //Enable UHC_RST
    MDrv_Timer_Delayms(1);
    XBYTE[gUSBC2_BASE]&=0xFD;

    XBYTE[0x25C0]&=0xEF; //Enable current source in DVI
    XBYTE[0x3aac]|=0x04; //Select current switch from DVI

    //open port 1
   // #ifdef  USBHOST2PORT
    XBYTE[gUSBC2_BASE]|=0x08; //disable init suspend state
//    XBYTE[0x3ac0+0x00]=0x01;   // override mode enable for power down control
//    XBYTE[0x3ac0+0x01]|=0x02;   // enable PLL power down
//    XBYTE[0x3ac0+0x01]&=0xFD;   // disable PLL power down
//    XBYTE[0x3ac0+0x00]&=0xFE;
    XBYTE[gUTMI2_BASE]=0x0;   //disable power down over write
    XBYTE[gUTMI2_BASE+0x01]=0x0;   //disable power down over write high byte
    //XBYTE[0x3ac6]|=0x04; //bit2 HSTXIEN, bit7 otg dull role mode
    XBYTE[gUSBC2_BASE]|=0x20; //Enable UHC XIU
    MDrv_Timer_Delayms(3);
    XBYTE[gUSBC2_BASE+0x02]|=0x01;  //UHC select enable
    XBYTE[gUTMI2_BASE+0x06]|=0x40;        //Force HS TX current enable and CDR stage select
    XBYTE[gUTMI2_BASE+0x06]|=0x03;        //reset UTMI
    XBYTE[gUTMI2_BASE+0x06]&=0xfc;
    XBYTE[gUTMI2_BASE+0x29]=0x08;         //disable full speed retime
    XBYTE[gUTMI2_BASE+0x03]=0xa8;         //for device disconnect status bit
    //XBYTE[UTMIBaseAddr+0x07]|=0x02;      //
    if  (XBYTE[0x1ecc]==0x02)            //Neptune , after U06
    {
      if (XBYTE[0x1ecf]>= 0x06)              //U07
        {
            XBYTE[gUTMI2_BASE+0x07]|=0x02;
            XBYTE[gUTMI2_BASE+0x2c]=0xc1;
            XBYTE[gUTMI2_BASE+0x2d]=0x3b;//enable TX common mode,
            XBYTE[gUTMI2_BASE+0x2f]|=0x0e;            //preemsis
        }
    }
    else if ((XBYTE[0x1ecc]>=3)&&(XBYTE[0x1ecc]<5))                    //Eris: 3 ,Titania: 4, Pluto: 5
    {
            XBYTE[gUTMI2_BASE+0x2c]=0xc5;
            XBYTE[gUTMI2_BASE+0x2d]=0x3b;//enable TX common mode,
            XBYTE[gUTMI2_BASE+0x2f]|=0x0e;            //preemsis
    }
    else if (XBYTE[0x1ecc]==5)          // Pluto: 5
    {
            XBYTE[gUTMI2_BASE+0x2c]|=0xc1;
            XBYTE[gUTMI2_BASE+0x2d]|=0x3;//enable TX common mode,
            XBYTE[gUTMI2_BASE+0x2f]|=0x4a;            //preemsis
            XBYTE[gUTMI2_BASE+0x13]|=0x70;
            XBYTE[gUTMI_BASE+0]&=0xfe;       //clear port 0 IREF
     }
    else if (XBYTE[0x1ecc]==6)        //Triton=6
    {
            XBYTE[gUTMI2_BASE+0x2c]=0xc1;
            XBYTE[gUTMI2_BASE+0x2d]=0x3b;//enable TX common mode,
            XBYTE[gUTMI2_BASE+0x2f]|=0x0e;            //preemsis
    }
    else                                //before U06
    {
      XBYTE[gUTMI2_BASE+0x2c]|=0x01;
      #ifndef Process_018_USE
      XBYTE[gUTMI2_BASE+0x2d]=0x38;         //disable TX common mode
      #endif
    }

    #ifdef CERAMAL_SERISE_USE
    if (CHIP_VERSION >= CHIP_VER_U03)
    {
      XBYTE[gUTMI2_BASE+0x2c]|=0xc1;     //Slew rate control,Common mode voltage,Pre-emphasis select
      XBYTE[gUTMI2_BASE+0x2d]|=0x07;     //HS RTerm current: 5/6,Enable common mode adjust,Enable common mode voltage,Pre-emphasis select
      XBYTE[gUTMI2_BASE+0x2f]|=0x0e;     //Enable pre-emphasis at data enable edge,Enable pre-emphasis at data transition edge,Enable pre-emphasis
    }
    #endif

    if (XBYTE[0x1ecc]==5)                   // If is Pluto
        XBYTE[gUTMI2_BASE+0x09]|=0x02;     //ISI improvement
    else
        XBYTE[gUTMI2_BASE+0x13]|=0x02;     //ISI improvement

    XBYTE[gUTMI2_BASE+0x09]|=0x60;//0x20;       //patch low tempture,FL meta issue and enable new FL RX engin

    XBYTE[gUTMI2_BASE+0x27]&=0xf3;
    XBYTE[gUTMI2_BASE+0x27]|=0x08;      //(1) Offset 27 (．h3AA7) bit <3:2> set 2・b10   // RX bias current => 60uA (default 40uA)

    //(2) Offset 2A (．h3AAA) bit <3:2> set 2・b11               // Squelch voltage => 100mV (default 150mV)
    XBYTE[gUTMI2_BASE+0x2a]=0x07;

    XBYTE[gUTMI2_BASE+0x15]|=0x20; //HOST CHIRP Detect
    XBYTE[gUHC2_BASE+0x32]|=0x01; //Force issue test packet
    MDrv_Timer_Delayms(10);
    //#endif
}

void IssueTestJ_Port2(void)
{
    DrvUsbMain2_printMsg("TEST_J on Port1\n", 0);

    if (XBYTE[0x1ecc]==5)          // Pluto: 5
    {
        XBYTE[gUTMI2_BASE+0x2c]=0x04;
        XBYTE[gUTMI2_BASE+0x2d]=0x20;
        XBYTE[gUTMI2_BASE+0x2e]=0x00;
    }
    else // Titania, 4 is tested.
    {
        XBYTE[gUTMI2_BASE+0x2c]=0x04;
        XBYTE[gUTMI2_BASE+0x2d]=0x38;
        XBYTE[gUTMI2_BASE+0x2e]=0x00;
        XBYTE[gUTMI2_BASE+0x2f]=0x00;
    }

    XBYTE[gUSBC2_BASE]|=0x02; //Enable UHC_RST
    MDrv_Timer_Delayms(10);
    XBYTE[gUSBC2_BASE]&=0xFD;

    UHC_XBYTE(0x32)|=0x01;
    MDrv_Timer_Delayms(10);
    UHC_XBYTE(0x32)&=0xfe;

    MDrv_Timer_Delayms(10);
    UHC_XBYTE(0x50)|=0x01; //enable test J
}

void IssueTestK_Port2(void)
{
    DrvUsbMain2_printMsg("TEST_K on port1\n", 0);

    if (XBYTE[0x1ecc]==5)          // Pluto: 5
    {
        XBYTE[gUTMI2_BASE+0x2c]=0x04;
        XBYTE[gUTMI2_BASE+0x2d]=0x20;
        XBYTE[gUTMI2_BASE+0x2e]=0x00;
    }
    else // Titania, 4 is tested.
    {
        XBYTE[gUTMI2_BASE+0x2c]=0x04;
        XBYTE[gUTMI2_BASE+0x2d]=0x38;
        XBYTE[gUTMI2_BASE+0x2e]=0x00;
        XBYTE[gUTMI2_BASE+0x2f]=0x00;
    }

    XBYTE[gUSBC2_BASE]|=0x02; //Enable UHC_RST
    MDrv_Timer_Delayms(10);
    XBYTE[gUSBC2_BASE]&=0xFD;

    UHC_XBYTE(0x32)|=0x01;
    MDrv_Timer_Delayms(10);
    UHC_XBYTE(0x32)&=0xfe;

    MDrv_Timer_Delayms(10);
    UHC_XBYTE(0x50)|=0x02; //enable test K
}

void IssueSE0_Port2(void)
{
    DrvUsbMain2_printMsg("SE0 on port1\n", 0);
    XBYTE[gUTMI2_BASE+0x06]=0xA0;
    XBYTE[gUTMI2_BASE+0x07]=0x04;

    XBYTE[gUSBC2_BASE]|=0x02; //Enable UHC_RST
    MDrv_Timer_Delayms(10);
    XBYTE[gUSBC2_BASE]&=0xFD;

    UHC_XBYTE(0x32)|=0x01;
    MDrv_Timer_Delayms(10);
    UHC_XBYTE(0x32)&=0xfe;
}

void MDrv_UsbSendTestPacket_Port2(void)
{

#ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_SetXdataWindow1_Port2();
#endif

    Port1_IssueTestPacket_Initial();
    IssueTestPacket_Port2(pUsbData_Port2);

#ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1_Port2();
#endif
}

void MDrv_UsbSendSE0_Port2(void)
{

#ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_SetXdataWindow1_Port2();
#endif

    Port1_IssueTestPacket_Initial();
    IssueSE0_Port2();

#ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1_Port2();
#endif
}

void MDrv_UsbSendTestJ_Port2(void)
{

#ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_SetXdataWindow1_Port2();
#endif

    Port1_IssueTestPacket_Initial();
    IssueTestJ_Port2();

#ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1_Port2();
#endif
}

void MDrv_UsbSendTestK_Port2(void)
{

#ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_SetXdataWindow1_Port2();
#endif

    Port1_IssueTestPacket_Initial();
    IssueTestK_Port2();

#ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1_Port2();
#endif
}

#endif
#if 0
U8 MDrv_GetUsbDeviceType_Port2()
{
    U8      u8DevType;

    if (gUsbDeviceState_Port2==BAD_DEVICE)
        return USB_INTERFACE_CLASS_NONE;

#ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_SetXdataWindow1_Port2();
#endif

    u8DevType = psAttachDevice_Port2->saCD[0].sInterface[0].bInterfaceClass;

#ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1_Port2();
#endif

    return (u8DevType);
}

void MDrv_GetUsbString_Port2(U8 u8StrID, S8 *pStrBuf, U8 u8BufLen)
{
    U8 ii;

#ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_SetXdataWindow1_Port2();
#endif

    pStrBuf[0] = 0;

    if (u8StrID == USB_STR_VENDOR)
    {
        for (ii=0; ii<u8BufLen; ii++)
        {
            pStrBuf[ii] = psAttachDevice_Port2->bStringManufacture[ii];
            if (pStrBuf[ii] == 0)
                break;
        }
    }
    else if (u8StrID == USB_STR_PRODUCT)
    {
        for (ii=0; ii<u8BufLen; ii++)
        {
            pStrBuf[ii] = psAttachDevice_Port2->bStringProduct[ii];
            if (pStrBuf[ii] == 0)
                break;
        }
    }

#ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1_Port2();
#endif

}
#endif
#ifdef Enable_Mass_Storage
void MDrv_GetUsbStorString_Port2(U8 uLun, U8 u8StrID, S8 *pStrBuf, U8 u8BufLen)
{
    U8 ii;

    pStrBuf[0] = 0;

    if (u8StrID == USB_STR_VENDOR)
    {
        for (ii=0; ii<u8BufLen; ii++)
        {
            pStrBuf[ii] = Mass_stor_us1_port2.Mass_stor_device[uLun].u8VendorID[ii];
            if (ii >= 8)
                break;
        }
        pStrBuf[ii] = 0; //Null terminal
    }
    else if (u8StrID == USB_STR_PRODUCT)
    {
        for (ii=0; ii<u8BufLen; ii++)
        {
            pStrBuf[ii] = Mass_stor_us1_port2.Mass_stor_device[uLun].u8ProductID[ii];
            if (ii >= 16)
                break;
        }
        pStrBuf[ii] = 0; //Null terminal
    }

}
#endif
#else
BYTE code drvusbmain_2[] = {0};
void msusbmain_2Dummy(void)
{
    BYTE xdata x = drvusbmain_2[0];
}
#endif
