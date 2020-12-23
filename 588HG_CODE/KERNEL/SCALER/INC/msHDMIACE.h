/******************************************************************************
 Copyright (c) 2004 MStar Semiconductor, Inc.
 All rights reserved.

 [Module Name]: MsHDCP.h
 [Date]:        28-Jan-2004
 [Comment]:
   HDCP header file.
 [Reversion History]:
*******************************************************************************/
#ifndef _HDMIACE_H_
#define _HDMIACE_H_

#ifdef _HDMIACE_C_
#define _HDMIACEDEC_
#else
#define _HDMIACEDEC_ extern
#endif

_HDMIACEDEC_ void InitHDMIACEVar( void );
_HDMIACEDEC_ void msAdjustMainWinRedColor( BYTE ucColor );
_HDMIACEDEC_ void msAdjustMainWinGreenColor( BYTE ucColor );
_HDMIACEDEC_ void msAdjustMainWinBlueColor( BYTE ucColor );
_HDMIACEDEC_ void msAdjustMainWinContrast( BYTE ucRColor, BYTE ucGColor, BYTE ucBColor );
#endif

