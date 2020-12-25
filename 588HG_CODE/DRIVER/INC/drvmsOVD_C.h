#ifndef _MSOVD_7_H_
#define _MSOVD_7_H_

#ifdef _MSOVD_7_C_
  #define _MSOVDDEC_7_
#else
  #define _MSOVDDEC_7_ extern
#endif

//*************************************************************************************************
// Description: OD look-up table loader.
//
// ex: msLoadODT(tOverDrive)
//
// It provides two kinds of format (normal and compressed) for OD look-up table.
// The look-up table and OD lib should be located in the same bank!!


_MSOVDDEC_7_ void drvmsOverDriverInit(void);
_MSOVDDEC_7_ void drvmsLoadODTable( BYTE *pODTbl);
_MSOVDDEC_7_ void drvmsOverDriveOnOff(Bool bEnable);

#undef _MSOVDDEC_7_
#endif
