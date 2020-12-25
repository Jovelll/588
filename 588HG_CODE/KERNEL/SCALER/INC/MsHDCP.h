/******************************************************************************
Copyright (c) 2004 MStar Semiconductor, Inc.
All rights reserved.
[Module Name]: MsHDCP.h
[Date]:        28-Jan-2004
[Comment]:
HDCP header file.
[Reversion History]:
 *******************************************************************************/
#ifndef _MSHDCP_H_
#define _MSHDCP_H_

#ifdef _MSHDCP_C_
#define _MSHDCPIDEC_
#else
#define _MSHDCPIDEC_ extern
#endif

#if ENABLE_HDCP
_MSHDCPIDEC_ void msInitHDCPKeySelectorVector( void );
_MSHDCPIDEC_ void msInitHDCPProductionKey( void );
_MSHDCPIDEC_ void msEnableHDCP( void );
_MSHDCPIDEC_ void msDisableHDCP( void );
_MSHDCPIDEC_ void msPullLowHPD1( void );
_MSHDCPIDEC_ void msPullHighHPD1( void );
_MSHDCPIDEC_ void msPullLowHPD2( void );
_MSHDCPIDEC_ void msPullHighHPD2( void );
_MSHDCPIDEC_ Bool IsSrcUseDualLinkDviPort(void);
_MSHDCPIDEC_ void msDVIDualLinkMode(Bool bEnable);
_MSHDCPIDEC_ Bool msGetDVIDualLinkStatus(void);

#endif
#endif
