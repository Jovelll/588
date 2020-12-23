///////////////////////////////////////////////////////////////////////////////
//
//	File name: Host20_AP.H
//	Version: 1.0
//	Date: 2003/7/13
//
//	Author: Bruce
//	Email:
//	Phone: (03) 578-7888
//	Company: Faraday Tech. Corp.
//
//	Description:
///////////////////////////////////////////////////////////////////////////////
 #ifndef HOST200_AP__H

 #define HOST200_AP__H

 #define OTGH_PT_ADVANCE_TEST	                 0

//=================== 1.Condition Definition  ============================================================
//========================================================================================================
 #define OTGH_PT_DataStart	                     0x04000000
 #define OTGH_PT_AUTO_TEST


//=================== 2.Define Data Type  ================================================================
//========================================================================================================
 #define OTGH_PT_CBW_SIGNATE	                 0x55534243
 #define OTGH_PT_CSW_SIGNATE 	                 0x55534253
 #define OTGH_PT_CSW_PASS          			     0x00
 #define OTGH_Dir_IN 	                         0x01
 #define OTGH_Dir_Out 	                         0x00

//=================== 3.Structure Definition =============================================================
//========================================================================================================
 typedef struct
 {
 	UINT8  bDataCounter_IN;	         //0~255
 	UINT8  bDataCounter_OUT;	     //0~255
 	UINT32 wMaxSize;
    //UINT8 *pbDataPage[5];

 }OTGH_PT_CONTROL_Struct;


 typedef struct
 {
 	UINT8 bDataCounterIn;	         //0~255
 	UINT8 bDataCounterOut;	         //0~255
 	UINT8 bMaxSize;              //Assume 13
    UINT8 bINT_Dir;             //0:Init 1:IN 2:Out
 	UINT8 bInQHDArrayNum;
 	UINT8 bOutQHDArrayNum;
 }OTGH_PT_INT_Struct;

 #define OTGH_PT_ISO_DATABUFFER_NUM                600

 typedef struct
 {
 	UINT8       bDataMax;	      //252
 	UINT8       bDataCounter;     //Dynamic index
    UINT32      aDataBufferArray[OTGH_PT_ISO_DATABUFFER_NUM];//Max support 600*4K=2.4M

 }OTGH_PT_ISO_Struct;

 typedef struct CommandBlockWrapper
 {
 	UINT32 u32Signature;
 	UINT32 u32Tag;
 	UINT32 u32DataTransferLength;
 	UINT8 u8Flags;
 	UINT8 u8LUN;
 	UINT8 u8CBLength;
 	UINT8 u8CB[16];
 }OTGH_PT_CBW;

 typedef struct CommandStatusWrapper
 {
 	UINT32 u32Signature;
 	UINT32 u32Tag;
 	UINT32 u32DataResidue;
 	UINT8 u8Status;
 }OTGH_PT_CSW;

 typedef struct
 {
 	OTGH_PT_CBW sCBW;	                 //31 bytes
 	OTGH_PT_CSW sCSW;                    //13bytes
    //UINT32      wDataRange;
 	UINT8        bInQHDArrayNum;
 	UINT8        bOutQHDArrayNum;
    UINT8        bStatus;
 }OTGH_PT_BLK_Struct;

//=================== 4.Extern Function Definition =======================================================
//========================================================================================================
 extern void OTGH_AP_Main(void);
 extern void OTGH_PT_CV(void);
 extern void OTGH_PT_Bulk(void);
 extern void OTGH_PT_Interrupt_In_Out_RandomSize(void);
 extern void OTGH_PT_Interrupt_In_Out_FixSize(void);
 extern void OTGH_PT_GetDescriptor (UINT8 bCounter);

 extern void OTGH_PT_Bulk_SendCBW(UINT32 wLength, UINT16 hwStartAdd,  UINT32 wTagNum , UINT8 bDataDir);
 extern void OTGH_PT_Bulk_ReceiveCSW(void);
 extern void OTGH_PathTest_BULK_INT(void);
 extern void OTGH_PathTest_ISO_IN(void);
 extern void OTGH_PathTest_ISO_OUT(void);
 extern void OTGH_PathTest_Control(void);
 extern void OTGH_Suspend_Test(void);
 extern UINT8 OTGH_SetDescriptor_Test(void);
 extern void OTGH_PT_Control_Path(void);
 extern void OTGH_PT_Control_Command(void);
 extern void OTGH_PT_FrameNumber(void);
 extern void OTGH_PT_CV_Like(void);


extern void OTGC_A_Bus_Drop(void);
extern void OTGC_A_Bus_Drive(void);

extern void OTGH_Hub_Main(void);
extern void OTGH_AP_Faraday(void);
extern void OTGH_PT_Init(void);
extern void Host20_MSCD_MassStorage_Main(void);

extern void OTGH_PT_Bulk_Close(void);

extern void OTGH_AP_RemoteWakeUp_Test(void);
extern void OTGH_AP_Suspend_Resume_Test(void);
#ifdef Enable_Mass_Storage
extern BOOLEAN MassStorage_Init_Port2(void);
#endif
void OTGH_Error_Handle(void);

//=================== 5.Extern Variable Definition =======================================================
//========================================================================================================
 extern  OTGH_PT_BLK_Struct   xdata   *sOTGH_PT_BLK_PORT2;
 extern  OTGH_PT_BLK_Struct   xdata   *pOTGH_PT_BLK_Port2;

#endif //HOST200_AP__H


