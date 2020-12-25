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
#include "USBHostConfig.h"
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
#include "drvHostLib.h"
#include "drvHost200.h"
#include "drvUsbMain.h"
#ifndef ATV_SERISE_USE
#include "sysinfo.h"
#include "drvtimer.h"
#endif
#include "drvusb.h"
#ifdef Enable_Mass_Storage
#include "drvscsi.h"
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

#include "drvUSBHost_PTP.h"


#define DrvUsbMain_DEBUG    1
#if ENABLE_DEBUG && DrvUsbMain_DEBUG
    #define DrvUsbMain_printData(str, value)   printData(str, value)
    #define DrvUsbMain_printMsg(str)           printMsg(str)
#else
    #define DrvUsbMain_printData(str, value)
    #define DrvUsbMain_printMsg(str)
#endif

#define DRV_USB_DBG(x)    x;
#define MINI_DEBUG(x)      x;

U8 gUsbStatus=USB_OK;
U8 gUsbTimeout=5;

#if 1
extern U8 gUsbDeviceState;
extern U8 gUsbDeviceState_Port2;
#else
#ifdef USB_POWER_SAVING_MODE
U8 gUsbDeviceState=POWER_SAVING;
#ifdef USBHOST2PORT
extern U8 gUsbDeviceState_Port2;
#endif
#else
U8 gUsbDeviceState=NO_DEVICE;
#endif
#endif
U8  NowIsHub;

extern  UINT8 xdata pUsbData[512];
extern U8 xdata PORT_DEVICE[HOST20_INTERFACE_NUM_MAX];
extern U8 gSpeed;
static U32 original_xdwin1_Base=0xFFFFFFFF; //use 0xFFFFFFFF to represent void
static U8 xdwin1_lockCount=0;
#define STR_C51_VERSION(x) "C51:"##  #x

#define C51_VERSION(x) STR_C51_VERSION(x)

char* usb_host_p1_c51version = C51_VERSION(__C51__);
#if 0
U8 code USB_VBuf[32] = {'M', 'S', 'V', 'C', '0', '0',               // 0, 0 fixed
                        'Z', '2',                                   // Library ID
#ifdef   DTV_STANDARD_LIB
                        'A', '0', '0', '1', '4', '0',               // build number
#endif
#ifdef ATV_SERISE_USE
                        'B', '0', '0', '1', '2', '4',               // build number
#endif
#ifdef CERAMAL_SERISE_USE
                        'C', '0', '0', '1', '2', '4',               // build number
#endif

                        '0', '0', '0', '0', '0', '0', '0', '0',     // change list 46058
                        'A', '4', 'A', '0', '0', '0', '0', '0', '0',// A4: Saturn, A:LG
                        'T'};
#endif
#ifdef OnePort_OTG_EHCI
U8 USBMode=0; //set default status=OTG device mode ,{default=0, otg=1, uhci=2}
extern U8 volatile usbUSBState;
#endif

U8 RecoveryFlag;
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
extern void drvUSBHost_UTMIInitial();
extern void drvUSBHost_UTMIInitial_Port2();
extern void UsbTurnOffPowerDownMode(void);
extern void UsbTurnOnPowerDownMode(void);
extern BOOLEAN drvUSBHost_isPort2Enable();
#ifdef  Enable_PTP
extern U8 drvUSBHost_PTP_Init(U8 u8UsbPort);
#endif
extern U8 drvUSBHost_HID_Init(U8 u8UsbPort, U8 u8Interface);

#if 0
void UsbGetVerString(U8 *pVerString)
{

    memcpy(pVerString, &USB_VBuf[0], sizeof(USB_VBuf));

}
#endif
#if 0

void SetUsbTimeout(U8 x)
{
  gUsbTimeout=x;
}

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
void MDrv_USB_SetXdataWindow1(void)
{
  if(xdwin1_lockCount==0xFF)
  {
    DRV_USB_DBG( DrvUsbMain_printMsg("error!! MAX allowed lock reached\n"));
  }

  if(xdwin1_lockCount==0)
  {
    #ifdef JUNIOR_SERIAL_USE
    U16 _win1Base = (XBYTE[REG_2BCD]<<8 | XBYTE[REG_2BCC]);
    original_xdwin1_Base=(U32)_win1Base;
    XBYTE[REG_2BCD] = (USB_BUFFER_START_ADR_4K_ALIGN_Var>>12)>>8;
    XBYTE[REG_2BCC] = (USB_BUFFER_START_ADR_4K_ALIGN_Var>>12)>>0;
    #else
    U16 _win1Base = (XBYTE[XDMIU_REG_BASE + 13]<<8 | XBYTE[XDMIU_REG_BASE + 12]);
    original_xdwin1_Base=(U32)_win1Base;
    XBYTE[XDMIU_REG_BASE + 13] = (USB_BUFFER_START_ADR_4K_ALIGN_Var>>12)>>8;
    XBYTE[XDMIU_REG_BASE + 12] = (USB_BUFFER_START_ADR_4K_ALIGN_Var>>12)>>0;
    #endif
  }
  xdwin1_lockCount++;
  //DRV_USB_DBG(printf("==== WIN1 LOCKED BY USB, COUNT=%bu\n",xdwin1_lockCount););
}
void MDrv_USB_ReleaseXdataWindow1(void)
{
  if(xdwin1_lockCount==0)
  {
    DRV_USB_DBG(DrvUsbMain_printMsg("error!! no lock to release\n"));
  }

  if(xdwin1_lockCount==1)
  {
    #ifdef JUNIOR_SERIAL_USE
    XBYTE[REG_2BCD] = ((U16)original_xdwin1_Base)>>8;
    XBYTE[REG_2BCC] = ((U16)original_xdwin1_Base)>>0;
    #else
    XBYTE[XDMIU_REG_BASE + 13] = ((U16)original_xdwin1_Base)>>8;
    XBYTE[XDMIU_REG_BASE + 12] = ((U16)original_xdwin1_Base)>>0;
    #endif
    original_xdwin1_Base=0xFFFFFFFF;
  }
  xdwin1_lockCount--;
  //DRV_USB_DBG(printf("==== WIN1 UNLOCKED BY USB, COUNT=%bu\n",xdwin1_lockCount););

}
#endif
#ifdef Enable_Burning_Test
#ifndef CERAMAL_SERISE_USE
extern void msAPI_Timer_ResetWDT(void);
#endif
//U8 buf1[512];
void UsbTest()
{
  U16 i,j;
  U32 idx=0,xxx;
  U8 VailLun,LunIndex;
  U8 xdata *buf1;

  DrvUsbMain_printMsg("\r\n USB Burning Test\n");
  //printf("maxlun :%02bx lunbyte:%02bx\n",maxlun,lunbyte);
  //MDrv_UsbHost_Init();

  VailLun=MDrv_GET_MASS_VALID_LUN();
  DrvUsbMain_printData("VailLun: %bd\n", VailLun);
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

  buf1=(U8 xdata*)(0xFE00);

  for (i=0 ; i < 0x200 ; i++)
  {
    buf1[i]= (i & 0xff);
  }

  DrvUsbMain_printData("buf1 real addr: 0x%lX\n", (VirtoPhyAddr((U32)buf1)));
  //printf("\r\n addr=%x",(VirtoPhyAddr((U32)buf1)>>16));
  //printf(",%x\n",(VirtoPhyAddr((U32)buf1)));
  DrvUsbMain_printData("gSpeed: %bd\n", gSpeed);

  while (1)
  {
    #ifndef CERAMAL_SERISE_USE
    msAPI_Timer_ResetWDT();
    #endif
    idx++;
    //if ((idx & 0x3f)==0)
    {
      DrvUsbMain_printData("\r\n Loop:%x",(U16)(idx>>16));
      DrvUsbMain_printData(",%x",(U16)(idx));
    }
    //MDrv_MIU_Copy
    xxx=idx % 10000;
    if (MDrv_UsbBlockWriteFromMIU(LunIndex,50+xxx,1,VirtoPhyAddr((U32)buf1))==FALSE)
    {
      DrvUsbMain_printMsg("\r\n write failed\n");
      break;
    }
    if (MDrv_UsbBlockReadToMIU(LunIndex,50+xxx,1,VirtoPhyAddr((U32)buf1))==FALSE)
    {
      DrvUsbMain_printMsg("\r\n read failed\n");
      break;
    }
    //MDrv_Timer_Delayms(10);
    for (j=0 ; j < 0x200 ; j++)
    {
      if (buf1[j]!= (j&0xff)) DrvUsbMain_printMsg("\r\n data error\n");
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
  tmp=sAttachDevice.saCD[i].sInterface[j].bInterfaceClass;
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
BOOLEAN MDrv_UsbHost_Init(void)
{
  BOOLEAN result=FALSE;

  DRV_USB_DBG(DrvUsbMain_printMsg("HOST200_main\n"));

  MINI_DEBUG(DrvUsbMain_printMsg("uinit\n"));
  if (gUsbStatus==USB_EJECT)
  {
    DrvUsbMain_printData("UHC 2430:%02bx\n",XBYTE[gUHC_BASE+0x30]);
    if (UHC_XBYTE(0x30)&2)
    {
      gUsbStatus=USB_OK;
    }
    else
      return FALSE;         //eject state
  }

  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_SetXdataWindow1();
  #endif

#ifndef  Enable_FS_Only

  //XBYTE[gUTMI_BASE+0x06]|=0x44;        //Force HS TX current enable and CDR stage select
  XBYTE[gUTMI_BASE+0x06]|=0x40;        //Force HS TX current enable and CDR stage select
  XBYTE[gUTMI_BASE+0x06]&=~0x20;              //clear bit 5

  XBYTE[gUTMI_BASE+0x06]|=0x03;        //reset UTMI
  XBYTE[gUTMI_BASE+0x06]&=0xfc;
  XBYTE[gUTMI_BASE+0x29]=0x08;         //disable full speed retime
#ifdef  Enable_Issue_TestPacket
  XBYTE[gUTMI_BASE+0x03]=0;         //for device disconnect status bit
#else
  XBYTE[gUTMI_BASE+0x03]=0xa8;         //for device disconnect status bit
#endif
  //XBYTE[gUTMI_BASE+0x07]|=0x02;      //
  #ifdef DTV_STANDARD_LIB
   if  (XBYTE[0x1ecc]==0x02)            //Neptune , after U06
   {
      if (XBYTE[0x1ecf]>= 0x06)              //U07
        {
            XBYTE[gUTMI_BASE+0x07]|=0x02;
            XBYTE[gUTMI_BASE+0x2c]=0xc1;
            XBYTE[gUTMI_BASE+0x2d]=0x3b;//enable TX common mode,
            XBYTE[gUTMI_BASE+0x2f]|=0x0e;            //preemsis
        }
      else          //before U06
        {
            XBYTE[gUTMI_BASE+0x2c]|=0x01;
             XBYTE[gUTMI_BASE+0x2d]=0x38;         //disable TX common mode

        }
    }
     else if ((XBYTE[0x1ecc]>=3)&&(XBYTE[0x1ecc]<5))                    //Eris: 3 ,Titania: 4,
    {
            XBYTE[gUTMI_BASE+0x2c]=0xc5;
            XBYTE[gUTMI_BASE+0x2d]=0x3b;//enable TX common mode,
            XBYTE[gUTMI_BASE+0x2f]|=0x0e;            //preemsis
    }
    else if (XBYTE[0x1ecc]==5)          // Pluto: 5
    {
       //     XBYTE[gUTMI_BASE+0x2c]|=0xc5;
          //  XBYTE[gUTMI_BASE+0x2d]|=0x3;//enable TX common mode,
          //  XBYTE[gUTMI_BASE+0x2f]|=0x4a;            //preemsis
           if (XBYTE[0x1ecf]<= 0x03)
               XBYTE[gUTMI_BASE+0x13]|=0x70;        //U04 and later don't set
     }
    else if (XBYTE[0x1ecc]==0x0b)          // Titania 2
    {
            XBYTE[gUTMI_BASE+0x2c]|=0xc1;
            XBYTE[gUTMI_BASE+0x2d]|=0x3;//enable TX common mode,
            XBYTE[gUTMI_BASE+0x2f]|=0x4a;            //preemsis
     }
    else if (XBYTE[0x1ecc]==6)        //Triton=6
    {
            XBYTE[gUTMI_BASE+0x2c]=0xc1;
            XBYTE[gUTMI_BASE+0x2d]=0x3b;//enable TX common mode,
            XBYTE[gUTMI_BASE+0x2f]|=0x0e;            //preemsis
    }

  else                                //before U06
  {
    XBYTE[gUTMI_BASE+0x2c]|=0x01;
    #ifndef Process_018_USE
    XBYTE[gUTMI_BASE+0x2d]=0x38;         //disable TX common mode
    #endif
  }
  #endif

  #ifdef CERAMAL_SERISE_USE
  if (CHIP_VERSION >= CHIP_VER_U03)
  {
    XBYTE[gUTMI_BASE+0x2c]|=0xc1;     //Slew rate control,Common mode voltage,Pre-emphasis select
    XBYTE[gUTMI_BASE+0x2d]|=0x07;     //HS RTerm current: 5/6,Enable common mode adjust,Enable common mode voltage,Pre-emphasis select
    XBYTE[gUTMI_BASE+0x2f]|=0x0e;     //Enable pre-emphasis at data enable edge,Enable pre-emphasis at data transition edge,Enable pre-emphasis
  }
  #endif

  if (XBYTE[0x1ecc]==5)                   // If is Pluto
      XBYTE[gUTMI_BASE+0x09]|=0x01;     //ISI improvement
  else
      XBYTE[gUTMI_BASE+0x13]|=0x02;     //ISI improvement

  XBYTE[gUTMI_BASE+0x09]|=0x60;//0x20;       //patch low tempture,FL meta issue and enable new FL RX engin

   if (mwOTG20_Control_HOST_SPD_TYP_Rd()==1)            //low speed,for HID
            XBYTE[gUTMI_BASE+0x09]&=~0x40;                      //old setting


  XBYTE[gUTMI_BASE+0x27]&=0xf3;
  XBYTE[gUTMI_BASE+0x27]|=0x08;     //(1) Offset 27 (．h3AA7) bit <3:2> set 2・b10   // RX bias current => 60uA (default 40uA)

  //(2) Offset 2A (．h3AAA) bit <3:2> set 2・b11               // Squelch voltage => 100mV (default 150mV)
#ifdef DTV_STANDARD_LIB
  XBYTE[gUTMI_BASE+0x2a]=0x07;
#endif

  XBYTE[gUTMI_BASE+0x15]|=0x20; //HOST CHIRP Detect

  //(3) Offset 2D (．h3AAD) bit <5:3> set 3・b111           // HS_RTERM bias current 5/6
  //XBYTE[0x128a]=0x87;            //change UHC priority
  //XBYTE[0x128d]&=0x0f;                //clear bit 12~15
  //XBYTE[0x128e]|=0xf;            //upgrade UHC priority, set bit 0~3
  MDrv_Timer_Delayms(500);

#endif

  //MDrv_Sys_SetXdataWindow1Base(USB_BUFFER_START_ADR_4K_ALIGN>>12);        //switch window 1
  gUsbStatus=USB_OK;
 // gUsbTimeout=3;  //set at flib_Host20_Enumerate()
  RecoveryFlag=0;             //initialize recovery flag
  gUsbDeviceState=USB11_DEVICE;           //1.1 at first

  //gDeviceFatalError=FALSE;
  if(flib_OTGH_Init(0))
    result =TRUE;

  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_ReleaseXdataWindow1();
  #endif

  return result;

}
#if 0
BOOLEAN MDrv_UsbHost_Init_Enum(void)
{
  BOOLEAN result;
  result=MDrv_Usb_Device_Enum();
  return result;
}
#endif
#ifdef Enable_Mass_Storage
U8 MDrv_UsbGetMaxLUNCount()
{
  if (gUsbDeviceState==BAD_DEVICE)
    return 0;

  if (mwHost20_PORTSC_ConnectStatus_Rd()==0)
    return 0;
  else return (Mass_stor_us1.max_lun+1);
}
U8 MDrv_GET_MASS_MAX_LUN()
{
  //printf("Get max lun func:%02bx\n",Mass_stor_us1.max_lun);
  if (mwHost20_PORTSC_ConnectStatus_Rd()==0)
    return 0;
  else return Mass_stor_us1.max_lun;
}
extern void GetValidLun(void);
#endif


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
extern U8 ValidLunNum;
U8 MDrv_GET_VALID_LUN_NUM()
{
    return ValidLunNum;
}
U8 MDrv_GET_MASS_VALID_LUN()
{
  U8 LunMatrix=0,i;
  struct LUN_Device* LunDevice;
  //printf("Get valid lun func\n");
  if (mwHost20_PORTSC_ConnectStatus_Rd()==0)
    return USB_NOT_RESPONSE;               //device is not connected

  if (gUsbDeviceState==BAD_DEVICE) return 0;

  if (gSpeed==1)    return 0;           //low speed device

  //if (gDeviceFatalError) return USB_NOT_RESPONSE;
  if (gUsbStatus==USB_EJECT) return USB_NOT_RESPONSE;

  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_SetXdataWindow1();        //switch window 1
  #endif

  LunDevice = Mass_stor_us1.Mass_stor_device;

  #ifdef ATV_SERISE_USE
    #ifdef Enable_Close_INT
    //XBYTE[0x2B00]|=0x02;
    DisableINT();
    #endif
  #endif
  GetValidLun();
  #ifdef ATV_SERISE_USE
    #ifdef Enable_Close_INT
    //XBYTE[0x2B00]&=0xFD;
    EnableINT();
    #endif
  #endif

  if (gUsbStatus==USB_TIMEOUT)
  {
    #ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1();
    #endif
    return USB_NOT_RESPONSE;            //USB DEVICE not responding
  }

  for (i=0; i <= Mass_stor_us1.max_lun ; i++)
  {
    LunMatrix=LunMatrix<<1 ;
    //printf("bDeviceReady:%02bx\n",LunDevice[Mass_stor_us1.max_lun-i].bDeviceReady);
    if (LunDevice[Mass_stor_us1.max_lun-i].bDeviceReady == TRUE)
    {
      LunMatrix|=1;
    }
  }
  //printf("GetReadyLun:%02bx\n",LunMatrix);
  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_ReleaseXdataWindow1();
  #endif

  return LunMatrix;
}
#endif

U32 USB_BUFFER_START_ADR_4K_ALIGN_Var;
#ifndef USE_XDATA_ADDRESS_0XF000
#ifndef CERAMAL_SERISE_USE
//extern DWORD jpCVBS5_Get_HK_XDATA_Addr(void);
#endif
#endif
void MDrv_USB_Init(DWORD USBAdr)
{
  //gProjectCode=ProjectCode;
  //gDeviceFatalError=FALSE;

  USB_BUFFER_START_ADR_4K_ALIGN_Var=USBAdr;

  //DrvUsbMain_printData("\r\n USB_BUFFER_START_ADR_4K_ALIGN_Var=%x",(U16)(USB_BUFFER_START_ADR_4K_ALIGN_Var>>16));
  //DrvUsbMain_printData(",%x",(U16)(USB_BUFFER_START_ADR_4K_ALIGN_Var));

  if (USB_BUFFER_START_ADR_4K_ALIGN_Var % 4096 != 0)
  {
    DRV_USB_DBG( DrvUsbMain_printMsg("Error USB Starting address is not 4K alignmented\n"));

  }

#if 1
  if (!drvUSBHost_isPowerSaveModeEnable())
  {
    #ifdef Enable_FS_Only
    XBYTE[REG_1EDC] = 0x08;
    XBYTE[REG_1ECE] = 0x40;
    XBYTE[REG_1ED0] = 0x04;
    while(1)
    {
       if (XBYTE[REG_1EDC] & 0x80)
          break;
    }

    XBYTE[gUTMI_BASE+0x01] &= 0xEF;
    XBYTE[gUTMI_BASE+0x09] |= 0x08;

    XBYTE[gUSBC_BASE+0x03] = 0x00;
    XBYTE[gUSBC_BASE+0x02] = 0x01;
    XBYTE[gUSBC_BASE+0x00] = 0x28;
    #endif

    #ifdef Enable_Preamble
    XBYTE[gUSBC_BASE+0x0F] |= 0x08;
    XBYTE[gUTMI_BASE+0x3F] |= 0x80;
    #endif

    mbFUSBH200_VBUS_ON_Set();
    #ifndef Enable_FS_Only
    UHC_XBYTE(0x34)&=0xBF; //set suspend
    UHC_XBYTE(0x34)|=0x40; //clr suspend
    MDrv_Timer_Delayms(2);
    XBYTE[gUTMI_BASE+0x00]|=0x01;       // override mode enable for power down control
    XBYTE[gUTMI_BASE+0x01]|=0x40;   // enable IREF power down
    XBYTE[gUTMI_BASE+0x01]|=0x02;   // enable PLL power down
    XBYTE[gUTMI_BASE+0x01]&=0xFD;   // disable PLL power down
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
    XBYTE[gUTMI_BASE+0x06]|=0x04; //HSTXIEN
    XBYTE[gUTMI_BASE+0x00]=0x0;   //disable power down over write
    XBYTE[0x2601]|=0x20;            //HsEnable set of usb power register
    XBYTE[gUTMI_BASE+0x01]=0x0;   //disable power down over write  high byte
    XBYTE[0x2500]|=0x08;            //disable init suspend state
    MDrv_Timer_Delayms(3);
    #endif
  XBYTE[0x2502]|=0x10; //id pull up
  XBYTE[gUTMI_BASE+0x06]|=0x80; //dual otg mode
  #endif
}

#if 0
U8  MDrv_GetUsbDeviceStatus()
{
  return gUsbDeviceState;
}

void MDrv_SetUsbDeviceStatus(U8 status)
{
  gUsbDeviceState = status;
}

void  MDrv_ClearUsbDeviceStatus()
{
  //if (gUsbDeviceState==BAD_DEVICE)
  //{
  //  printf("Clear bad device\n");
  //  gUsbDeviceState=USB11_DEVICE;
  //}
}
#endif
void ResetUsbHardware()
{
#if 0
  XBYTE[gUTMI_BASE+0x06]|=0x03;        //reset UTMI
  XBYTE[gUTMI_BASE+0x06]&=0xfc;
  mbHost20_USBCMD_HCReset_Set();
#else
    if ( (XBYTE[0x1ecc]==5) && (XBYTE[0x1ecf]<= 0x03) )
    {  // Pluto: 5 U02 and U03 has bugs, reset RX would make port2 stoped.
        XBYTE[gUTMI_BASE+0x06]|=0x02;        //reset UTMI
        mbHost20_USBCMD_HCReset_Set();
        MDrv_Timer_Delayms(2);
        XBYTE[gUTMI_BASE+0x06]&=0xfd;
    }
    else
    {
        XBYTE[gUTMI_BASE+0x06]|=0x03;        //reset UTMI
        mbHost20_USBCMD_HCReset_Set();
        MDrv_Timer_Delayms(2);
        XBYTE[gUTMI_BASE+0x06]&=0xfc;
    }
#endif
  //MDrv_Timer_Delayms(200);
}
UINT32 UsbStartTime=0;
UINT32 UsbPowerSavingTimer=0;

#ifdef CERAMAL_SERISE_USE
void FUSBH200_Driver_VBUS(void)
{
  XBYTE[0x1E14]|=0x80; // GPIO AO enable, reg_drvbus_en=1, eable drive vbus pad
  mbFUSBH200_VBUS_ON_Set();
}
#endif

#if 1
void UsbPort0UTMIInitial(void)
{
   #ifdef Enable_FS_Only
   XBYTE[gUTMI_BASE+0x00] &= 0xEF;
   XBYTE[gUTMI_BASE+0x09] |= 0x08;

   XBYTE[gUSBC_BASE+0x03] = 0x00;
   XBYTE[gUSBC_BASE+0x02] = 0x01;
   XBYTE[gUSBC_BASE+0x00] = 0x28;
   #endif
   mbFUSBH200_VBUS_ON_Set();
   #ifndef Enable_FS_Only
   UHC_XBYTE(0x34)&=0xBF; //set suspend
   UHC_XBYTE(0x34)|=0x40; //clr suspend
   MDrv_Timer_Delayms(2);
   XBYTE[gUTMI_BASE+0x00]|=0x01;        // override mode enable for power down control
   XBYTE[gUTMI_BASE+0x01]|=0x40;    // enable IREF power down
   XBYTE[gUTMI_BASE+0x01]|=0x02;   // enable PLL power down
   XBYTE[gUTMI_BASE+0x01]&=0xFD;   // disable PLL power down
   XBYTE[gUTMI_BASE+0x00]&=~0x01;       // override mode enable for power down control
   #endif
   mbHost20_USBCMD_HCReset_Set();
}
#else
#ifdef USB_POWER_SAVING_MODE
void UsbTurnOffPowerDownMode(void)
{
    //Power On
    XBYTE[gUTMI_BASE+8]&=0x7F;    //PD_BG_CURRENT
    XBYTE[gUTMI_BASE+0x2c]|=0x04;    //Switch current from DVI

    //Port0
    XBYTE[gUTMI_BASE+1]=0x6f;
    XBYTE[gUTMI_BASE]=0xc4;
    XBYTE[gUTMI_BASE+0x11]=0x00;
    XBYTE[gUTMI_BASE+0x10]=0x00;
    #ifdef USBHOST2PORT
    //Port1
    XBYTE[gUTMI2_BASE+1]=0x6f;
    XBYTE[gUTMI2_BASE]=0xc4;
    XBYTE[gUTMI2_BASE+0x11]=0x00;
    XBYTE[gUTMI2_BASE+0x10]=0x00;
    //Power On End
    #endif
    //open port 0
    XBYTE[gUTMI_BASE+1]=0x00;
    XBYTE[gUTMI_BASE]=0x00;
    XBYTE[gUSBC_BASE]|=0x08; //disable init suspend state
    XBYTE[gUSBC_BASE]|=0x20; //Enable UHC XIU
    MDrv_Timer_Delayms(3);
    XBYTE[gUSBC_BASE+2]|=0x01;  //UHC select enable
    //end

    // Power On USB UHC0
    XBYTE[gUHC_BASE+0x34] |= 0x40;

    MDrv_Timer_Delayms(2);
    XBYTE[gUHC_BASE+0x40] &=~0x10;

    #ifdef USBHOST2PORT
    //open port 1
    XBYTE[gUTMI2_BASE]=0x00;   //disable power down over write
    XBYTE[gUTMI2_BASE+1]=0x00;   //disable power down over write high byte
    XBYTE[gUSBC_BASE]|=0x08;  //disable init suspend state
    XBYTE[gUSBC_BASE]|=0x20;  //Enable UHC XIU
    MDrv_Timer_Delayms(3);
    XBYTE[gUSBC_BASE+2]|=0x01;  //UHC select enable
    //end

    // Power On USB UHC1
    XBYTE[UHC2_BASE+0x34] |= 0x40;
    MDrv_Timer_Delayms(2);
    XBYTE[UHC2_BASE+0x40] &=~0x10;
    #endif
}

void UsbTurnOnPowerDownMode(void)
{
    MDrv_Timer_Delayms(2);

   XBYTE[gUSBC_BASE]&=0xD0; //Disable Port0 UHC XIU
   #ifdef USBHOST2PORT
   XBYTE[gUSBC2_BASE]&=0xD0; //Disable Port1 UHC XIU
   #endif
   XBYTE[gUTMI_BASE+8]|=0x80;    //PD_BG_CURRENT
   XBYTE[gUTMI_BASE+0x2c]&=0xFB;    //Switch current from REXT, not DVI

   //Port 0
   XBYTE[gUTMI_BASE+0x11]=0x88;     //bit<15>:HS_TX_OVERRIDE, bit<11>:FL_TX_OVERRIDE
   XBYTE[gUTMI_BASE+0x10]=0x00;
   XBYTE[gUTMI_BASE+3]=0x90;     //bit<15>:reg_hs_rterm_pdn, bit<7>:reg_bond_sel
   XBYTE[gUTMI_BASE+2]=0x80;
   XBYTE[gUTMI_BASE+1]=0xef;     //bit<15>:reg_pdn
   XBYTE[gUTMI_BASE]=0xc7;
   #ifdef USBHOST2PORT
   //Port 1
   XBYTE[gUTMI2_BASE+0x11]=0x88;
   XBYTE[gUTMI2_BASE+0x10]=0x00;
   XBYTE[gUTMI2_BASE+3]=0x90;
   XBYTE[gUTMI2_BASE+2]=0x80;
   XBYTE[gUTMI2_BASE+1]=0xef;
   XBYTE[gUTMI2_BASE]=0xc7;
   #endif
}

void UsbPort0UTMIInitial(void)
{
   mbFUSBH200_VBUS_ON_Set();
   UHC_XBYTE(0x34)&=0xBF; //set suspend
   UHC_XBYTE(0x34)|=0x40; //clr suspend
   MDrv_Timer_Delayms(2);
   XBYTE[gUTMI_BASE+0x00]|=0x01;        // override mode enable for power down control
   XBYTE[gUTMI_BASE+0x01]|=0x40;    // enable IREF power down
   XBYTE[gUTMI_BASE+0x01]|=0x02;   // enable PLL power down
   XBYTE[gUTMI_BASE+0x01]&=0xFD;   // disable PLL power down
   XBYTE[gUTMI_BASE+0x00]&=~0x01;       // override mode enable for power down control

   mbHost20_USBCMD_HCReset_Set();
}
extern void UsbPort1UTMIInitial(void);
#endif
#endif
BOOLEAN MDrv_UsbDeviceConnect(void)
{
  /*
  if (gDeviceFatalError)
  {
    gDeviceFatalError=FALSE;
    ResetUsbHardware();
    return FALSE;
  }*/
  #if 1
  if (drvUSBHost_isPowerSaveModeEnable())
  {
    //#ifdef USBHOST2PORT
    if (gUsbDeviceState_Port2==WAIT_INIT)
        return FALSE;
    //#endif
    if (gUsbDeviceState==WAIT_INIT)
    {
        if (MDrv_Timer_DiffTimeFromNow(UsbPowerSavingTimer) < 300 )
            return FALSE;
    }
    if (gUsbDeviceState==POWER_SAVING)
    {
       if (XBYTE[gUSBC_BASE+8]&0x40)
       {
           UsbTurnOffPowerDownMode();
           UsbPort0UTMIInitial();
           //#ifdef USBHOST2PORT
           if (drvUSBHost_isPort2Enable())
          {
               //UsbPort1UTMIInitial();
               drvUSBHost_UTMIInitial_Port2();
               if (gUsbDeviceState_Port2==POWER_SAVING)
                   gUsbDeviceState_Port2=NO_DEVICE;
          }
           //#endif
           //UsbPowerSavingTimer = MDrv_Timer_GetTime0();
           gUsbDeviceState=WAIT_INIT;
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
  #ifdef USBHOST2PORT
  if (gUsbDeviceState_Port2==WAIT_INIT)
      return FALSE;
  #endif
  if (gUsbDeviceState==WAIT_INIT)
  {
      if (MDrv_Timer_DiffTimeFromNow(UsbPowerSavingTimer) < 300 )
          return FALSE;
  }
  if (gUsbDeviceState==POWER_SAVING)
  {
     if (XBYTE[gUSBC_BASE+8]&0x40)
     {
         UsbTurnOffPowerDownMode();
         UsbPort0UTMIInitial();
         #ifdef USBHOST2PORT
         UsbPort1UTMIInitial();
         if (gUsbDeviceState_Port2==POWER_SAVING)
             gUsbDeviceState_Port2=NO_DEVICE;
         #endif
         UsbPowerSavingTimer = MDrv_Timer_GetTime0();
         gUsbDeviceState=WAIT_INIT;
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
  if (gUsbStatus==USB_EJECT)
  {
    if (UHC_XBYTE(0x30)&2)
    {
      gUsbStatus=USB_OK;
    }
    else
      return FALSE;         //eject state
  }
  //FUSBH200_Driver_VBUS();             //make sure device is connected , then turn on VBUS

  #ifdef OnePort_OTG_EHCI
  if((XBYTE[gUTMI_BASE+0x30]&0x80))   //CID==1 device
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
           XBYTE[gUTMI_BASE+0x00] &= ~(reg_dp_puen+reg_term_override);//disable term_over and dp_puen
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
        XBYTE[gUTMI_BASE+0x00] &= ~(reg_dp_puen+reg_term_override);
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
    if ( (gUsbDeviceState==USB11_DEVICE) ||(gUsbDeviceState==USB20_DEVICE) )
    {
        // If the device is connected and we get a connection change.
        // It means that the user change the device and we just missed.
        if (mwHost20_PORTSC_ConnectChange_Rd())
        {
            DrvUsbMain_printMsg("dev changed, we missed\n");
            gSpeed=0xff;  //reset this value
            UHC_XBYTE(0x40)&=~0x80;//clear force enter FSmode
            gUsbDeviceState = NO_DEVICE;
            return FALSE;
        }
    }

     if (gUsbDeviceState==BAD_DEVICE) return FALSE;      //not repeating doing emunerate

    //if (gSpeed==0x01)
        //{
//           MINI_DEBUG(printf("ls-exit\n"));
    //       return FALSE;
        //}
    if (gUsbDeviceState==NO_DEVICE)
        gUsbDeviceState=CONNECT_DEVICE;
    #ifdef OnePort_OTG_EHCI
    return USBHostMode_connect;
    #else
    return TRUE;
    #endif
  }
  else
  {

    gSpeed=0xff;  //reset this value
    UHC_XBYTE(0x40)&=~0x80;//clear force enter FSmode
#if 1
    if (drvUSBHost_isPowerSaveModeEnable())
    {
        //#ifdef USBHOST2PORT
        if ((gUsbDeviceState_Port2==NO_DEVICE)||(gUsbDeviceState_Port2==POWER_SAVING))
        //#endif
        {
            if  (gUsbDeviceState!=POWER_SAVING)
                 UsbTurnOnPowerDownMode();
            gUsbDeviceState=POWER_SAVING;
            //#ifdef USBHOST2PORT
            if (drvUSBHost_isPort2Enable())
            {
                gUsbDeviceState_Port2=POWER_SAVING;
            }
            //#endif
        }
        //#ifdef USBHOST2PORT
        else
            gUsbDeviceState=NO_DEVICE;
        //#endif
    }
    else
        gUsbDeviceState=NO_DEVICE;
#else
    #ifdef USB_POWER_SAVING_MODE
    #ifdef USBHOST2PORT
    if ((gUsbDeviceState_Port2==NO_DEVICE)||(gUsbDeviceState_Port2==POWER_SAVING))
    #endif
    {
        if  (gUsbDeviceState!=POWER_SAVING)
             UsbTurnOnPowerDownMode();
        gUsbDeviceState=POWER_SAVING;
        #ifdef USBHOST2PORT
        gUsbDeviceState_Port2=POWER_SAVING;
        #endif
    }
    #ifdef USBHOST2PORT
    else
        gUsbDeviceState=NO_DEVICE;
    #endif
    #else
    gUsbDeviceState=NO_DEVICE;
    #endif
#endif

    #ifndef ATV_SERISE_USE
    #ifdef CERAMAL_SERISE_USE
    if (MDrv_Timer_TimeDifference(MDrv_Timer_GetTime0(), UsbStartTime) > 1000UL)
    #else
    if (MDrv_Timer_DiffTimeFromNow(UsbStartTime) > 1000 )
    #endif
    {

      UsbStartTime=MDrv_Timer_GetTime0();
      ResetUsbHardware();

    }
    #else
    UsbStartTime++;
    if(UsbStartTime>0x600)
    {
      UsbStartTime=0;
      ResetUsbHardware();
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
void MDrv_EjectUsbDevice(void)
{
  struct LUN_Device* LunDevice;
  U8 i;

  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_SetXdataWindow1();
  #endif

  //printf("eject\n");
  LunDevice = Mass_stor_us1.Mass_stor_device;
  for (i=0; i <= Mass_stor_us1.max_lun ; i++)
  {

    if (LunDevice[i].bDeviceReady == TRUE)
    {
      vSCSI_EJECT_DEVICE(i);
      gUsbStatus=USB_EJECT;
      mwHost20_PORTSC_ConnectChange_Set();        //clear port connect change bit
      //printf("ej_ok\n");
    }
  }
  flib_Host20_Close();

  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_ReleaseXdataWindow1();
  #endif

}
#endif
#if 0
void MDrv_UsbClose(void)
{
  flib_Host20_Close();
}
#endif
#ifdef Enable_Mass_Storage
BOOLEAN MDrv_UsbBlockReadToMIU(U8 lun,U32 u32BlockAddr, U32 u32BlockNum,U32 u32MIUAddr)
{
  BOOLEAN result;
  U8 retrycnt=0;
  struct LUN_Device* LunDevice = Mass_stor_us1.Mass_stor_device;

  if (mwHost20_PORTSC_ConnectStatus_Rd()==0)
    return FALSE;               //device is not connected

  if (gUsbDeviceState==BAD_DEVICE) return FALSE;


  if (u32BlockAddr > Mass_stor_us1.Mass_stor_device[lun].u32BlockTotalNum)
  {
    MINI_DEBUG(printf("USBRead address is over the range:%lx\n",u32BlockAddr));
    return FALSE;
  }
  //printf("usb read sector:%lx\n",u32BlockNum);

  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_SetXdataWindow1();       //switch window 1
  #endif
  #ifdef ATV_SERISE_USE
    #ifdef Enable_Close_INT
    XBYTE[0x2B00]|=0x02;
    DisableINT();
    #endif
  #endif
  //result= bSCSI_Read_Write10(FALSE,lun,  u32BlockAddr, u32BlockNum, u32MIUAddr);
  while (1)
  {
    retrycnt++;
    result= bSCSI_Read_Write10(FALSE,lun,  u32BlockAddr, u32BlockNum, u32MIUAddr);
    if (result==TRUE) break;
    if (result==FALSE)
    {
      if (retrycnt > 3)
      {
       if (gUsbStatus==USB_TIMEOUT)
             gUsbDeviceState=BAD_DEVICE;    //mark as bad device
#if 0  //Removed. It gets problem if remove card while play from USB card reader.
       else
             LunDevice[lun].bDeviceValid=FALSE;      //mark as bad lun
#endif
        break;          //return FALSE
      }
      MINI_DEBUG( printf("USBDisk Read failed\n"));
       if (gUsbStatus==USB_TIMEOUT)
       {
        if ((retrycnt==2)&&(mwOTG20_Control_HOST_SPD_TYP_Rd()==2) )              //make sure it is hi speed
        {
          MINI_DEBUG(printf("Force FS\n"));
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
        if(gSpeed==0)
        {
          //printf("\r\n CDR toggle!!",0);
          XBYTE[gUTMI_BASE+0x0a]^=0x10; //invert CDR_CLOCK
          XBYTE[gUTMI_BASE+0x06]|=0x03; //reset UTMI
          XBYTE[gUTMI_BASE+0x06]&=0xfc;
        }
        #endif

       // MDrv_Usb_Device_Enum();             //reinit usb device
          if (MDrv_Usb_Device_Enum()==FALSE)
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
    DRV_USB_DBG(DrvUsbMain_printMsg("USBDisk Read failed\n"));
    //printf("read failed\n");
  }

  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_ReleaseXdataWindow1();
  #endif
  return result;

}

BOOLEAN MDrv_UsbBlockWriteFromMIU(U8 lun, U32 u32BlockAddr, U32 u32BlockNum,U32 u32MIUAddr)
{
  BOOLEAN result;
  if (mwHost20_PORTSC_ConnectStatus_Rd()==0)
    return FALSE;               //device is not connected

   if (gUsbDeviceState==BAD_DEVICE) return FALSE;

  if (u32BlockAddr > Mass_stor_us1.Mass_stor_device[lun].u32BlockTotalNum)
  {
    MINI_DEBUG(DrvUsbMain_printData("USBWrite address is over the range:%lx\n",u32BlockAddr));
    return FALSE;
  }

  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_SetXdataWindow1();       //switch window 1
  #endif

  result= bSCSI_Read_Write10(TRUE,lun,  u32BlockAddr, u32BlockNum, u32MIUAddr);

  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_ReleaseXdataWindow1();
  #endif

  return result;

}
U32 MDrv_GetUsbBlockSize(U8 lun)
{
  return Mass_stor_us1.Mass_stor_device[lun].u32BlockSize;
}

U32 MDrv_GetUsbBlockNum(U8 lun)
{
  return Mass_stor_us1.Mass_stor_device[lun].u32BlockTotalNum;
}
#endif
#if 0
U8 MDrv_USB_GetXDataExtStackCount()
{
  return xdwin1_lockCount;
}
#endif
//---if you don't know how to use it, don't use it
#if 0
void EnterUXBMode()
{
  MINI_DEBUG( printf("Enter UXB mode\n"));
  UHC_XBYTE(0x40)|=0x80;//force enter FSmode
  MDrv_Usb_Device_Enum();
}
void LeaveUXBMode()
{
  MINI_DEBUG(printf("leave UXB mode \n"));
  UHC_XBYTE(0x40)&=~0x80;//leave  FSmode
  MDrv_Usb_Device_Enum();

}
#endif
BOOLEAN  MDrv_Usb_Device_Enum(void)
{
  BOOLEAN result=FALSE;
  U8  RetryCount=0,i=0,j=0, total_interface=0;

  U8 rootUSBDeviceInterfaceClass = USB_INTERFACE_CLASS_NONE;

  if (gUsbDeviceState==BAD_DEVICE) return FALSE;        //bad device , no more enumerate

  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_SetXdataWindow1();
  #endif
  NowIsHub=0;

redo_init:
  //USB ROOT
  RetryCount++;
  if (MDrv_UsbHost_Init()==FALSE) goto fail_exit;
  if  (flib_Host20_Enumerate(1,3)==0)
  {
    if ((gUsbStatus==USB_TIMEOUT)||(gUsbStatus==USB_INIT_FAIL))
    {
      #ifdef DTV_STANDARD_LIB
      if ((RetryCount==2)&&(mwOTG20_Control_HOST_SPD_TYP_Rd()==2))
      {                                                               //make sure it is hi speed
        MINI_DEBUG(DrvUsbMain_printMsg("Force FS\n"));
        UHC_XBYTE(0x40)|=0x80;//force enter FSmode
      }
      #endif
      if (RetryCount >= 3)
      {
        MINI_DEBUG(DrvUsbMain_printMsg("bad device\n"));
        gUsbDeviceState=BAD_DEVICE;
        goto fail_exit;
      }
      #ifndef ATV_SERISE_USE
      #ifndef CERAMAL_SERISE_USE
      msAPI_Timer_ResetWDT();
      #endif
      #endif
      goto redo_init;
    }
    gUsbDeviceState=BAD_DEVICE;
    goto fail_exit;
  }
  else
  {
#ifdef DTV_STANDARD_LIB
    XBYTE[gUTMI_BASE+0x2a]=0x0;
#endif
    total_interface=sAttachDevice.saCD[0].bINTERFACE_NUMBER; 
//for(i=0;i<sAttachDevice.sDD.bCONFIGURATION_NUMBER;i++)
{
for(j=0;j<total_interface;j++)
{
    rootUSBDeviceInterfaceClass= sAttachDevice.saCD[i].sInterface[j].bInterfaceClass;
    //MDrv_UsbGetInterfaceClass(0, 0);
    DRV_USB_DBG(DrvUsbMain_printMsg("Check USD Device 4\n"););
#if 0 // We don't need to check here
    if(rootUSBDeviceInterfaceClass != 0x08 && rootUSBDeviceInterfaceClass != 0x09)
    {
      //printf("unsupport class\n",0);
      DRV_USB_DBG(printf("unsupport USB root class=%02bx\n", rootUSBDeviceInterfaceClass););
      result= FALSE;
      goto fail_exit;
    }
#endif

    DRV_USB_DBG(DrvUsbMain_printData("USB root class=%x\n",rootUSBDeviceInterfaceClass););
    if (rootUSBDeviceInterfaceClass==USB_INTERFACE_CLASS_HUB)//HUB
    {
      U8 PortNum,i,devaddr;

      devaddr=6;
      DRV_USB_DBG(DrvUsbMain_printMsg("Hub class!\n"));

      PortNum=Usb_Hub_Port_Num();
      for (i=1; i <=PortNum ; i++)
      {
        sAttachDevice.bAdd=3;
        Host20_qHD_List_Control1.bDeviceAddress=sAttachDevice.bAdd;
        if (USB_Hub_Handle(i)==FALSE)
        {
          result=FALSE;
          continue;
        }
        devaddr++;
        NowIsHub=1;

        // Set Hub DS device to low speed
        //Host20_qHD_List_Control0.bEdSpeed=1;
        //Host20_qHD_List_Control1.bEdSpeed=1;

        if (flib_Host20_Enumerate(1,devaddr)==0)
        {
          result=FALSE;
          continue;
        }
        sAttachDevice.bAdd=devaddr;
        #ifdef Enable_Mass_Storage
        result=MassStorage_Init();              //do mass storage class init
        #endif
        if (drvUSBHost_HID_Init(USB_PORT_0,j) == 0)
        {
            result = TRUE;
            DRV_USB_DBG(DrvUsbMain_printMsg("HID initial ok\r\n"));
        }
        if (result==TRUE) break;
      }
       if (result==FALSE)
        {
             MINI_DEBUG(DrvUsbMain_printMsg("unsupport hub class device,->bad device\n"));
             gUsbDeviceState=BAD_DEVICE;
        }
    }

    #ifdef Enable_Mass_Storage
    else if(rootUSBDeviceInterfaceClass==USB_INTERFACE_CLASS_MSD)//Mass storage class
    {
      #ifdef Enable_Mass_Storage
      result=MassStorage_Init();
      #endif
      #ifdef Enable_Burning_Test
      UsbTest();
      #endif

      #ifdef Enable_Low_Temperature_Patch
      if((gSpeed==0)&&(gUsbStatus==USB_TIMEOUT))
      {
        //printf("\r\n CDR toggle!!",0);
        XBYTE[gUTMI_BASE+0x0a]^=0x10; //invert CDR_CLOCK
        XBYTE[gUTMI_BASE+0x06]|=0x03; //reset UTMI
        XBYTE[gUTMI_BASE+0x06]&=0xfc;
      }
      #endif

      if((gSpeed==2)&&(gUsbStatus==USB_TIMEOUT))
      {

        UHC_XBYTE(0x40)|=0x80; //force enter FS mode
        goto redo_init;
      }

      if(result==FALSE)
      {
          if (RetryCount >= 3)
          {
             goto fail_exit;
          }

          goto redo_init;
      }
    }

   #endif
   #ifdef  Enable_PTP
   else  if (rootUSBDeviceInterfaceClass == USB_INTERFACE_CLASS_IMAGE)
    {
        if (drvUSBHost_PTP_Init(USB_PORT_0) == PTP_OK)
        {
            result = TRUE;
            DRV_USB_DBG(DrvUsbMain_printMsg("PTP initial ok\r\n"));
        }
    }
   #endif
   else if (rootUSBDeviceInterfaceClass==3)//HID
    {
        if (drvUSBHost_HID_Init(USB_PORT_0,j) == 0)
        {
            result = TRUE;
            DRV_USB_DBG(DrvUsbMain_printData("HID initial_%x ok\r\n",j));
        }
    }
   else
    {
       MINI_DEBUG(DrvUsbMain_printData("unsupport %x class device->bad device\n",rootUSBDeviceInterfaceClass));
       gUsbDeviceState=BAD_DEVICE;           //not belong to any above
    }
    DRV_USB_DBG(DrvUsbMain_printMsg("USB_CON\n"););
    //return result;
  }
}

   for(i=0;i<total_interface;i++)
   {
        if(PORT_DEVICE[i]==1 /*|| PORT_DEVICE[i]==2*/) //1=Keyboard
            flib_Host20_Interrupt_Init(3,i,0);
        else if(PORT_DEVICE[i]==2) //2=mouse
            flib_Host20_Interrupt_Init(3,i,1); //2=mouse
   }
}
fail_exit:

  memset(pUsbData, 0, 8);
  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_ReleaseXdataWindow1();
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
void IssueTestPacket(U8 xdata *TestData)
{
  U32 DMAAddress,datreg32;

  DrvUsbMain_printMsg("Test Packet on port0\n");
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

  //printf("3AAC:%x\n",XBYTE[ 0x3AAC]);
  //printf("UTMI(0x2c):%x\n",XBYTE[gUTMI_BASE+0x2C]);
  //#ifdef Issue_TestPacket
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

  XBYTE[gUTMI_BASE+0x06]|=0x03; //TR/RX reset
  XBYTE[gUTMI_BASE+0x06]&=0xFC;

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

    // printf("Dma success\n",0);

    MDrv_Timer_Delayms(10);
    }

    }

void Port0_IssueTestPacket_Initial(void)
{
    XBYTE[gUSBC_BASE]|=0x02; //Enable UHC_RST
    MDrv_Timer_Delayms(1);
    XBYTE[gUSBC_BASE]&=0xFD;

    XBYTE[0x25C0]&=0xEF; //Enable current source in DVI
    XBYTE[0x3aac]|=0x04; //Select current switch from DVI

    //open port 0
    XBYTE[gUSBC_BASE]|=0x08; //disable init suspend state
//    XBYTE[UTMIBaseAddr+0x00]=0x01;   // override mode enable for power down control
//    XBYTE[UTMIBaseAddr+0x01]|=0x02;   // enable PLL power down
//    XBYTE[UTMIBaseAddr+0x01]&=0xFD;   // disable PLL power down
//    XBYTE[UTMIBaseAddr+0x00]&=0xFE;
    XBYTE[gUTMI_BASE]=0x0;   //disable power down over write
    XBYTE[gUTMI_BASE+0x01]=0x0;   //disable power down over write high byte
    //XBYTE[0x3a86]|=0x04; //HSTXIEN
    XBYTE[gUSBC_BASE]|=0x20; //Enable UHC XIU
    MDrv_Timer_Delayms(3);
    XBYTE[gUSBC_BASE+0x02]|=0x01;  //UHC select enable
    XBYTE[gUTMI_BASE+0x06]|=0x40;        //Force HS TX current enable and CDR stage select
    XBYTE[gUTMI_BASE+0x06]|=0x03;        //reset UTMI
    XBYTE[gUTMI_BASE+0x06]&=0xfc;
    XBYTE[gUTMI_BASE+0x29]=0x08;         //disable full speed retime
    XBYTE[gUTMI_BASE+0x03]=0xa8;         //for device disconnect status bit
    //XBYTE[UTMIBaseAddr+0x07]|=0x02;      //
    if  (XBYTE[0x1ecc]==0x02)            //Neptune , after U06
    {
      if (XBYTE[0x1ecf]>= 0x06)              //U07
        {
            XBYTE[gUTMI_BASE+0x07]|=0x02;
            XBYTE[gUTMI_BASE+0x2c]=0xc1;
            XBYTE[gUTMI_BASE+0x2d]=0x3b;//enable TX common mode,
            XBYTE[gUTMI_BASE+0x2f]|=0x0e;            //preemsis
        }
    }
    else if ((XBYTE[0x1ecc]>=3)&&(XBYTE[0x1ecc]<=5))                    //Eris: 3 ,Titania: 4, Pluto: 5
    {
            XBYTE[gUTMI_BASE+0x2c]=0xc5;
            XBYTE[gUTMI_BASE+0x2d]=0x3b;//enable TX common mode,
            XBYTE[gUTMI_BASE+0x2f]|=0x0e;            //preemsis
    }
    else if (XBYTE[0x1ecc]==6)        //Triton=6
    {
            XBYTE[gUTMI_BASE+0x2c]=0xc1;
            XBYTE[gUTMI_BASE+0x2d]=0x3b;//enable TX common mode,
            XBYTE[gUTMI_BASE+0x2f]|=0x0e;            //preemsis
    }
    else                                //before U06
    {
      XBYTE[gUTMI_BASE+0x2c]|=0x01;
      #ifndef Process_018_USE
      XBYTE[gUTMI_BASE+0x2d]=0x38;         //disable TX common mode
      #endif
    }

    #ifdef CERAMAL_SERISE_USE
    if (CHIP_VERSION >= CHIP_VER_U03)
    {
      XBYTE[gUTMI_BASE+0x2c]|=0xc1;     //Slew rate control,Common mode voltage,Pre-emphasis select
      XBYTE[gUTMI_BASE+0x2d]|=0x07;     //HS RTerm current: 5/6,Enable common mode adjust,Enable common mode voltage,Pre-emphasis select
      XBYTE[gUTMI_BASE+0x2f]|=0x0e;     //Enable pre-emphasis at data enable edge,Enable pre-emphasis at data transition edge,Enable pre-emphasis
    }
    #endif

    if (XBYTE[0x1ecc]==5)                   // If is Pluto
        XBYTE[gUTMI_BASE+0x09]|=0x01;     //ISI improvement
    else
        XBYTE[gUTMI_BASE+0x13]|=0x02;     //ISI improvement

    XBYTE[gUTMI_BASE+0x09]|=0x60;//0x20;       //patch low tempture,FL meta issue and enable new FL RX engin

    XBYTE[gUTMI_BASE+0x27]&=0xf3;
    XBYTE[gUTMI_BASE+0x27]|=0x08;       //(1) Offset 27 (．h3AA7) bit <3:2> set 2・b10   // RX bias current => 60uA (default 40uA)

    //(2) Offset 2A (．h3AAA) bit <3:2> set 2・b11               // Squelch voltage => 100mV (default 150mV)
    XBYTE[gUTMI_BASE+0x2a]=0x07;

    XBYTE[gUTMI_BASE+0x15]|=0x20; //HOST CHIRP Detect
    XBYTE[gUHC_BASE+0x32]|=0x01; //Force issue test packet
    MDrv_Timer_Delayms(10);
}

void IssueTestJ(void)
{
    DrvUsbMain_printMsg("TEST_J on Port0\n");

    if (XBYTE[0x1ecc]==5)          // Pluto: 5
    {
        XBYTE[gUTMI_BASE+0x2c]=0x04;
        XBYTE[gUTMI_BASE+0x2d]=0x20;
        XBYTE[gUTMI_BASE+0x2e]=0x00;
    }
    else // Titania, 4 is tested.
    {
        XBYTE[gUTMI_BASE+0x2c]=0x04;
        XBYTE[gUTMI_BASE+0x2d]=0x38;
        XBYTE[gUTMI_BASE+0x2e]=0x00;
        XBYTE[gUTMI_BASE+0x2f]=0x00;
    }

    XBYTE[gUSBC_BASE]|=0x02; //Enable UHC_RST
    MDrv_Timer_Delayms(10);
    XBYTE[gUSBC_BASE]&=0xFD;

    //UHCREG(0x32)|=0x01;
    UHC_XBYTE(0x32)|=0x01;
    MDrv_Timer_Delayms(10);
    //UHCREG(0x32)&=0xfe;
    UHC_XBYTE(0x32)&=0xfe;

    MDrv_Timer_Delayms(10);
    UHC_XBYTE(0x50)|=0x01; //enable test J
}

void IssueTestK(void)
{
    DrvUsbMain_printMsg("TEST_K on port0\n");

    if (XBYTE[0x1ecc]==5)          // Pluto: 5
    {
        XBYTE[gUTMI_BASE+0x2c]=0x04;
        XBYTE[gUTMI_BASE+0x2d]=0x20;
        XBYTE[gUTMI_BASE+0x2e]=0x00;
    }
    else // Titania, 4 is tested.
    {
        XBYTE[gUTMI_BASE+0x2c]=0x04;
        XBYTE[gUTMI_BASE+0x2d]=0x38;
        XBYTE[gUTMI_BASE+0x2e]=0x00;
        XBYTE[gUTMI_BASE+0x2f]=0x00;
    }

    XBYTE[gUSBC_BASE]|=0x02; //Enable UHC_RST
    MDrv_Timer_Delayms(10);
    XBYTE[gUSBC_BASE]&=0xFD;

    //UHCREG(0x32)|=0x01;
    UHC_XBYTE(0x32)|=0x01;
    MDrv_Timer_Delayms(10);
    //UHCREG(0x32)&=0xfe;
    UHC_XBYTE(0x32)&=0xfe;

    MDrv_Timer_Delayms(10);
    UHC_XBYTE(0x50)|=0x02; //enable test K
}

void IssueSE0(void)
{
    DrvUsbMain_printMsg("SE0 on port0\n");

    XBYTE[gUTMI_BASE+0x06]=0xA0;
    XBYTE[gUTMI_BASE+0x07]=0x04;

    MDrv_Timer_Delayms(10);
    XBYTE[gUSBC_BASE]|=0x02; //Enable UHC_RST
    MDrv_Timer_Delayms(10);
    XBYTE[gUSBC_BASE]&=0xFD;

    //UHCREG(0x32)|=0x01;
    UHC_XBYTE(0x32)|=0x01;
    MDrv_Timer_Delayms(10);
    //UHCREG(0x32)&=0xfe;
    UHC_XBYTE(0x32)&=0xfe;
}

//extern    UINT8 xdata pUsbData[256];
void MDrv_UsbSendTestPacket(void)
{

#ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_SetXdataWindow1();
#endif

    Port0_IssueTestPacket_Initial();
    IssueTestPacket(pUsbData);

#ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1();
#endif
}

void MDrv_UsbSendSE0(void)
{

#ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_SetXdataWindow1();
#endif

    Port0_IssueTestPacket_Initial();
    IssueSE0();

#ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1();
#endif
}

void MDrv_UsbSendTestJ(void)
{

#ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_SetXdataWindow1();
#endif

    Port0_IssueTestPacket_Initial();
    IssueTestJ();

#ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1();
#endif
}

void MDrv_UsbSendTestK(void)
{

#ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_SetXdataWindow1();
#endif

    Port0_IssueTestPacket_Initial();
    IssueTestK();

#ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1();
#endif
}


#endif

#if 0
U8 MDrv_GetUsbDeviceType()
{
    U8      u8DevType;

    if (gUsbDeviceState==BAD_DEVICE)
        return USB_INTERFACE_CLASS_NONE;

#ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_SetXdataWindow1();
#endif

    u8DevType = sAttachDevice.saCD[0].sInterface[0].bInterfaceClass;

#ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1();
#endif

    return (u8DevType);
}
#endif
#if 0
void MDrv_GetUsbString(U8 u8StrID, S8 *pStrBuf, U8 u8BufLen)
{
    U8 ii;

#ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_SetXdataWindow1();
#endif

    pStrBuf[0] = 0;

    if (u8StrID == USB_STR_VENDOR)
    {
        for (ii=0; ii<u8BufLen; ii++)
        {
            pStrBuf[ii] = sAttachDevice.bStringManufacture[ii];
            if (pStrBuf[ii] == 0)
                break;
        }
    }
    else if (u8StrID == USB_STR_PRODUCT)
    {
        for (ii=0; ii<u8BufLen; ii++)
        {
            pStrBuf[ii] = sAttachDevice.bStringProduct[ii];
            if (pStrBuf[ii] == 0)
                break;
        }
    }

#ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1();
#endif

}
#endif
#ifdef Enable_Mass_Storage
void MDrv_GetUsbStorString(U8 uLun, U8 u8StrID, S8 *pStrBuf, U8 u8BufLen)
{
    U8 ii;

    pStrBuf[0] = 0;

    if (u8StrID == USB_STR_VENDOR)
    {
        for (ii=0; ii<u8BufLen; ii++)
        {
            pStrBuf[ii] = Mass_stor_us1.Mass_stor_device[uLun].u8VendorID[ii];
            if (ii >= 8)
                break;
        }
        pStrBuf[ii] = 0; //Null terminal
    }
    else if (u8StrID == USB_STR_PRODUCT)
    {
        for (ii=0; ii<u8BufLen; ii++)
        {
            pStrBuf[ii] = Mass_stor_us1.Mass_stor_device[uLun].u8ProductID[ii];
            if (ii >= 16)
                break;
        }
        pStrBuf[ii] = 0; //Null terminal
    }

}
#endif
#else
BYTE code drvusbmain[] = {0};
void msusbmainDummy(void)
{
    BYTE xdata x = drvusbmain[0];
}
#endif
