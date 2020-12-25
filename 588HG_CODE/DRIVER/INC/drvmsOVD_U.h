#ifndef _MSOVD_U_H_
#define _MSOVD_U_H_

#ifdef _MSOVD_U_C_
  #define _MSOVDDEC_U_
#else
  #define _MSOVDDEC_U_ extern
#endif

//*************************************************************************************************
//Description: OD look-up table loader.
//
//           ex: msLoadODT(tOverDrive)
//
//           It provides two kinds of format (normal and compressed) for OD look-up table.
//           The look-up table and OD lib should be located in the same bank!!

_MSOVDDEC_U_ void drvmsOverDriverInit(void);
_MSOVDDEC_U_ void drvmsLoadODTable( BYTE *pODTbl);
_MSOVDDEC_U_ void drvmsLoad_MultiODTable(BYTE index);
_MSOVDDEC_U_ void drvmsOverDriveOnOff(BOOL bEnable);
#undef _MSOVDDEC_U_
#endif
