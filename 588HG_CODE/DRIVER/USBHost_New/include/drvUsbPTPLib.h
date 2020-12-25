//Copyright (C) 2008 MStar  All Rights Reserved.

/*! \file  drvUsbPTPLib.h
    \brief  Library for PTP functions.


 \n\b History:
    \n\b Date: 2008/8/18
    \n\b Author: Colin Tsai
    \n\b Reason: Create
    \n
*/
//=============================================================================

#ifndef DRVUSBPTPLIB_H
#define DRVUSBPTPLIB_H

//#include "datatype.h"
#include "UsbHostDataDef.h"

//#include "..\source\drvUSBHost_PTP.h"

typedef enum PTP_OPERATION_CODE
{
    PTP_UnDefined = 0x1000,
    PTP_GetDeviceInfo,
    PTP_OpenSession,
    PTP_CloseSession,
    PTP_GetStorageIDs,
    PTP_GetStorageInfo,
    PTP_GetNumObjects,
    PTP_GetObjectHandles,
    PTP_GetObjectInfo,
    PTP_GetObject,
    PTP_GetThumb,
    PTP_DeleteObject,
    PTP_SendObjectInfo,
    PTP_SendObject,
    PTP_InitiateCapture,
    PTP_FormatStore,
    PTP_ResetDevice,
    PTP_SelfTest,
    PTP_SetObjectProtection,
    PTP_PowerDown,
    PTP_GetDevicePropDesc,
    PTP_GetDevicePropValue,
    PTP_SetDevicePropValue,
    PTP_ResetDevicePropValue,
    PTP_TerminateOpenCapture,
    PTP_MoveObject,
    PTP_CopyObject,
    PTP_GetPartialObject,
    PTP_InitiateOpenCapture
}ePTP_OPERATION_CODE;

enum
{
  UnKnownFile,
  JpegFile,
  OtherFile
};


enum
{
  PTP_OK=0,
  PTP_RESP_FAIL

};

typedef enum PTP_OBJECTFORMATCODE
{
    OBJ_UNDEFINED_NON_IMAGE = 0x3000,
    OBJ_ASSOCIATION,
    OBJ_SCRIPT,
    OBJ_EXECUTABLE,
    OBJ_TEXT,
    OBJ_HTML,
    OBJ_DPOF,
    OBJ_AIFF,
    OBJ_WAV,
    OBJ_MP3,
    OBJ_AVI,
    OBJ_MPEG,
    OBJ_ASF,
    OBJ_UNDIFINED_IMAGE = 0x3800,
    OBJ_JPEG,
    OBJ_TIFF_EP,
    OBJ_FLASHPIX,
    OBJ_BMP,
    OBJ_CIFF,
    OBJ_UNDIFFINED_RESERVED_0,
    OBJ_GIF,
    OBJ_JFIF,
    OBJ_PCD,
    OBJ_PICT,
    OBJ_PNG,
    OBJ_UNDIFFINED_RESERVED_1,
    OBJ_TIFF,
    OBJ_TIFF_IT,
    OBJ_JP2,
    OBJ_JPX

}ePTP_OBJECTFORMATCODE;

typedef struct _DATETIME {

    U8      bDateTime[5];   // 5+2+2+2 = 13 bytes
    U16     wTemp;
    //-------------------------------------------------------
    // bit 0- bit 4   Day   (1-31)
    // bit 5- bit 8   Month (1-12)
    // bit 9- bit f   Year    relative to 1980
    //-------------------------------------------------------
    U16     wDate;
    //-------------------------------------------------------
    // bit 0- bit 4 binary numbers of 2 secs increament 0-58
    // bit 5- bit a binary numbers of minutes 0-59
    // bit b- bit f binary numbers of hours   0-23
    //-------------------------------------------------------
    U16     wTime;

} DATETIME, *PDATETIME;

typedef struct _PTP_OBJECTINFO
{
    U16     ObjectFormat;
    U32     ObjectCompressedSize;
    U16     ThumbFormat;
    U32     ThumbCompressedSize;
    U32     ThumbPixWidth;
    U32     ThumbPixHeight;
    U32     ImagePixWidth;
    U32     ImagePixHeight;
    U32     ParentObject;
    U16     Filename[13];           // 8 + . + 3 + null
    struct _DATETIME sDateTime;
} PTP_OBJECTINFO, *PPTP_OBJECTINFO;

//Function_Name: drvUsbPTP_GetObjectHandles( )
/*!  \brief Gather all object handles according to parameters passed in.

This function will try to gather all object handles according to parameters passed in.
The handles are stored in memory of PTP library.

    \param u8UsbPort    The USB port number.

    \param u16ObjFormat     The object format code you want to get. All handles returned
                                        will belong to this format. The value 0 means "Dont care"

    \param u32AssociObjHandle   The associate object handle. All handles returned will
                                               be its children. The value 0 means "Don't case",
                                               and 0xFFFFFFFF means the "Root"

\return The number of object handles it got.
*/
U32 drvUsbPTP_GetObjectHandles(U8 u8UsbPort, U16 u16ObjFormat, U32 u32AssociObjHandle);

//Function_Name: drvUsbPTP_GetObjectHandleByIndex( )
/*!  \brief Get real object handle from PTP library memory.

This function will try to get the real object handle from PTP library memory. It should be
called after the function msAPI_PTP_GetObjectHandles.

    \param u8UsbPort    The USB port number.

    \param u32Index     The object index you want to get.

\return Object handle or 0xFFFFFFFF for invalid object handle.
*/
U32 drvUsbPTP_GetObjectHandleByIndex(U8 u8UsbPort, U32 u32Index);

//Function_Name: drvUsbPTP_GetObjectInfo( )
/*!  \brief Get PTP object information.

This function will try to get object information.

    \param u8UsbPort    The USB port number.
    \param u32ObjectHandle  The object handle you want to get.

\return Pointer for ObjectInfo.
*/
PTP_OBJECTINFO * drvUsbPTP_GetObjectInfo(U8 u8UsbPort, U32 u32ObjectHandle);

//Function_Name: drvUsbPTP_GetThumb( )
/*!  \brief Get the thumbnail of object.

This function will try to get the thumbnail of object.

    \param u8UsbPort    The USB port number.
    \param u32ObjectHandle  The object handle you want to get.
    \param u32Offset    The start offset you want to get.
    \param u32Length    The buffer size.
    \param u32BufAddr   The buffer address.

\return acutal thumb size it got.
*/
U32 drvUsbPTP_GetThumb(U8 u8UsbPort, U32 u32ObjectHandle, U32 u32Offset, U32 u32Length, U32 u32BufAddr);

//Function_Name: drvUsbPTP_GetObject( )
/*!  \brief Get the object.

This function will try to get content of object.

Please follow the offset sequence to get the object data. When the u32Offset is zero,
PTP USB driver will issue PTP command (GetObject) to the camera , and get the first bank of data.
For example, if the AP has a 16K buffer to get image data, the operation sequence is as follow.
1st : offset 0 , length 16K
2nd: offset 16K, length 16K
3rd: offset 32K, length 16K
..
If users want to stop or issue other commands before the end of the object data stream,
driver has to read all data at the camera side, it may take a period of time.

    \param u8UsbPort    The USB port number.
    \param u32ObjectHandle  The object handle you want to get.
    \param u32Offset    The start offset you want to get.
    \param u32Length    The size you want to get.
    \param u32BufAddr   Buffer address.

\return acutal object size it got.
*/
U32 drvUsbPTP_GetObject(U8 u8UsbPort, U32 u32ObjectHandle, U32 u32Offset, U32 u32Length, U32 u32BufAddr);


//Function_Name: drvUsbPTP_GetObjectEnd( )
/*!  \brief Terminate the operation of getting object.

This function will try to terminate the operation of getting object.

    \param u8UsbPort    The USB port number.
    \param u32ObjectHandle  The object handle you want to get.
    \param u32BufAddr   The buffer address. (buffer size must be 16KB at least)

\return none.
*/
void drvUsbPTP_GetObjectEnd(U8 u8UsbPort, U32 u32ObjectHandle, U32 u32BufAddr);

//Function_Name: drvUsbPTP_GetError( )
/*!  \brief Get the error code of PTP.

This function will try to get the error code of PTP library.

    \param u8UsbPort    The USB port number.

\return USB error code.
    #define HOST20_OK                              0x00
    #define HOST20_FAIL                            0x01
    #define HOST20_FATAL                      0x02
    #define HOST20_DEVICE_STALL             0x03
    #define HOST20_TRANSACTION_ERROR   0x04
*/
U8 drvUsbPTP_GetError(U8 u8UsbPort);

#endif // #ifndef DRVUSBPTPLIB_H

