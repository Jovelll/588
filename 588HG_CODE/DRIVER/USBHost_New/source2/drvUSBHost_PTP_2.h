/*********************************************************************
*   HEADER NAME:
*       drvUSBHost_PTP_2.h
*
*   Written by Derek.Horng@MSTAR Inc.
*---------------------------------------------------------------------
*
* Initial revision.
*********************************************************************/
#ifndef DRVUSBHOST_PTP_2_H
#define DRVUSBHOST_PTP_2_H

#include "drvUsbPTPLib.h"

/*--------------------------------------------------------------------
                            GENERAL INCLUDES
--------------------------------------------------------------------*/


/*--------------------------------------------------------------------
                            LITERAL CONSTANTS
--------------------------------------------------------------------*/
// USB interface class
#define USB_CLASS_IMAGE_INTERFACE       0x06
// interface subclass
#define USB_STILL_IMAGE_CAPTURE_DEVICE  0x01
// interface protocol
#define USB_PIMA15740                 0x01

#define INVALID_HANDLE_NO       0xFFFFFFFF

/*--------------------------------------------------------------------
                                 TYPES
--------------------------------------------------------------------*/
//-------------------------------------------
// The stucture define var
// should be assigned by us
//------------------------------------------
typedef struct _ASSIGNVAR {

    U16     u16OpCode;
    U32     u32TransId;
    U32     u32Buf;
    U32     u32Param1;
    U32     u32Param2;
    U32     u32Param3;
} ASSIGNVAR, *PASSIGNVAR;

//-----------------------------------------------------
// The stucture define variable
// will dynamic change from the data fetting from  DSC
//-----------------------------------------------------
typedef struct _DYMNVAR {

//U8          u8FileName[8];
//U8          u8ExtName [4];
U16         u16FileName[13];
U16         u16FileType;

U32         u32DataLen;
U32         u32StorageId;
U32         u32FileLength;
U32         u32FileOffset;
U32         u32DramAddr;
U32         ImagePixWidth;
U32         ImagePixHeight;
//-------------------------------------------------------
// In current status we do not get every Handle from DSC
// We get the  HandlNum and First Handle ID the seq add
// the Handle ID ---> for saving memory
//-------------------------------------------------------
//U32         u32HandleNum;
U32         u32HandleId;
struct _DATETIME sDateTime;
U16         u16ThumbFormat;
U32         u32ThumbSize;
U32         u32ThumbPixWidth;
U32         u32ThumbPixHeight;
U32         u32ParentObject;
} DYMNVAR, *PDYMNVAR;



typedef struct PTP_CONTAINER{
    U32 u32ConLen;
    U16 u16ConType;
    U16 u16OpCode;
    U32 u32TransActionID;
    U32 u32Param1;
    U32 u32Param2;
    U32 u32Param3;
}OTGH_PTP_CONTAINER;


typedef struct _PTP_DRIVER {
    U32 u32CapacityH;
    U32 u32CapacityL;
    U32 u32FreeSpaceH;
    U32 u32FreeSpaceL;
    U32 u32FirstFileHandleNo;
    U32 u32TotalHandleNo;
    U32 u32FirstHandleNo;
    U32 u32CurHandleIdx;
    U32 u32ObjectHandleNumBuf;
}HOST_PTP_DRIVER;

/*--------------------------------------------------------------------
                            PROJECT INCLUDES
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                            MEMORY CONSTANTS
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                               VARIABLES
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                                 MACROS
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                               PROCEDURES
--------------------------------------------------------------------*/
U8 drvUSBHost_PTP_SendOpCode_Port2(ASSIGNVAR xdata *pAssignVar,DYMNVAR xdata *pDymnVar);
void drvUSBHost_PTP_BulkInData_Port2  (ASSIGNVAR xdata *pAssignVar,DYMNVAR xdata *pDymnVar);
void drvUSBHost_PTP_Read1stBulkIn_Port2(ASSIGNVAR xdata *pAssignVar,DYMNVAR xdata *pDymnVar);
void drvUSBHost_PTP_ReadRemBulkIn_Port2(ASSIGNVAR xdata *pAssignVar,DYMNVAR xdata *pDymnVar );
U8 drvUSBHost_PTP_BulkInResponse_Port2(void);
U8 drvUSBHost_PTP_Initial_Port2(void);
//DYMNVAR *drvUSBHost_PTP_GetCurObjectInfo(void);
DYMNVAR *drvUSBHost_PTP_GetObjectInfo_Port2(U32 u32HandleNo );
//DYMNVAR *drvUSBHost_PTP_GetFirstFileObjectInfo(void);
//DYMNVAR *drvUSBHost_PTP_GetNextFileObjectInfo(void);
//DYMNVAR *drvUSBHost_PTP_GetPrevFileObjectInfo(void);
//U32 drvUSBHost_PTP_GetPartialObject(U32 u32TotalSize,U32 u32HandleIdx,U32 u32FileOffset,U32 u32Length,U32 u32DRAMAddr);
void drvUSBHost_PTP_GetBulkIn_Port2(DYMNVAR xdata *pDymnVar);
//U32 drvUSBHost_PTP_GetFreeSpace(void);
U32 drvUSBHost_PTP_GetObjectHandleNo_Port2(U32 u32HandleIdx);
void drvUSBHost_PTP_SaveObjectHandleNo_Port2(U32 u32HandleIdx,U32 u32HandleNo);
U32 drvUSBHost_PTP_GetObjectHandles_Port2(U16 u16ObjFormat, U32 u32AssociObjHandle);
//U32 drvUSBHost_PTP_GetThumbnail(U32 u32TotalSize,U32 u32HandleIdx,U32 u32FileOffset,U32 u32Length,U32 u32DRAMAddr);
#endif  //DRVUSBHOST_PTP_2_H